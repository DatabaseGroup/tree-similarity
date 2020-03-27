// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// Contains the implementation of the Touzet Baseline with depth based
/// pruning and truncated tree fix.

#pragma once

template <typename CostModel, typename TreeIndex>
double TouzetDepthPruningTruncatedTreeFixTreeIndex<CostModel, TreeIndex>::tree_dist(
    const TreeIndex& t1, const TreeIndex& t2, const int x,
    const int y, const int k, const int e) {
  
  int x_size = t1.postl_to_size_[x];
  int y_size = t2.postl_to_size_[y];

  // Calculates offsets that let us translate i and j to correct postorder ids.
  int x_off = x - x_size;
  int y_off = y - y_size;

  // Initial cases.
  fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
  for (int j = 1; j <= std::min(y_size, e); ++j) { // i = 0; only j that are within e-strip.
    fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2.postl_to_label_id_[j + y_off]);
  }
  if (e + 1 <= y_size) { // the first j that is outside e-strip
    fd_.at(0, e + 1) = std::numeric_limits<double>::infinity();
  }
  // QUESTION: Is it necessary to verify depths here?
  //           It is not, because these values will never be used. We write
  //           them not to verify the condition.
  for (int i = 1; i <= std::min(x_size, e); ++i) { // j = 0; only i that are within e-strip.
    fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1.postl_to_label_id_[i + x_off]);
  }
  if (e + 1 <= x_size) { // the first i that is outside e-strip
    fd_.at(e + 1, 0) = std::numeric_limits<double>::infinity();
  }

  double candidate_result = std::numeric_limits<double>::infinity();

  // General cases.

  // NOTE: max_depth has to be set to min(depth(x)+e+1, max depth of T1_x)
  //       because e+1 may exceed the maximum depth of T1_x.
  // TODO: Figure out why e+1.
  int max_depth = 0;

  // Collect all nodes in truncated tree to traverse.
  std::vector<int> nodes_to_traverse; // Normalized to current subtree: 1..|x|.
  // NOTE: If we can't truncate anything, all the nodes from the subtree have
  //       to be traversed.
  if (t1.postl_to_subtree_max_depth_.at(x) <= t1.postl_to_depth_.at(x) + e + 1) {
    max_depth = t1.postl_to_subtree_max_depth_.at(x);
    for (int n = x_size; n >= 1; --n) {
      nodes_to_traverse.push_back(n);
    }
  } else {
    max_depth = t1.postl_to_depth_.at(x) + e + 1;
    int n = 1;
    int post_n = 0;
    int depth_n = 0;
    while (n <= x_size) { // Loop in right-to-left-preorder
      post_n = x_size - n + 1 + x_off;
      depth_n = t1.postl_to_depth_.at(post_n);
      // if (depth_n <= max_depth) { // NOTE: depth_n < max_depth is always satisfied.
        nodes_to_traverse.push_back(x_size - n + 1);
        if (depth_n == max_depth) {// Jump to the next node to the left with the max_depth.
          n += t1.postl_to_size_.at(post_n);
        } else {
          ++n;
        }
      // }
    }
  }

  // Traversing truncated tree to filter out i-values based on depth.
  typename std::vector<int>::reverse_iterator rit = nodes_to_traverse.rbegin();
  int i;
  for (; rit != nodes_to_traverse.rend(); ++rit) {
    i = *rit;
    // Old if statement for filtering i-values based on depth.
    // if (t1_depth_[i + x_off] - t1_depth_[x] > e + 1) { continue; }
    if (i - e - 1 >= 1) { // First j that is outside e-strip.
      fd_.at(i, i - e - 1) = std::numeric_limits<double>::infinity();
    }
    for (int j = std::max(1, i - e); j <= std::min(i + e, y_size); ++j) { // only (i,j) that are in e-strip
      // The td(x_size-1, y_size-1) is computed differently.
      // TODO: This condition is evaluated too often but passes only on the
      //       last i and j.
      if (i == x_size && j == std::min(i + e, y_size)) {
        break;
      }
      ++subproblem_counter_;
      if (std::abs((i + x_off) - (j + y_off)) > k) {
        fd_.at(i, j) = std::numeric_limits<double>::infinity();
      } else {
        candidate_result = std::numeric_limits<double>::infinity();
        candidate_result = std::min(candidate_result, fd_.read_at(i, j - 1) + c_.ins(t2.postl_to_label_id_[j + y_off]));
        double td_read = td_.read_at(i + x_off, j + y_off);
        double fd_read = 0.0;
        // If one of the forests is a tree, look up the vlaues in fd_.
        // Otherwise, both forests are trees and the fd-part is empty.
        if (i - t1.postl_to_size_[i + x_off] != 0 || j - t2.postl_to_size_[j + y_off] != 0) {
          // If the values to read are outside of the band, they exceed
          // the threshold or are not present in the band-matrix.
          if (j - t2.postl_to_size_[j + y_off] < std::max(0, i - t1.postl_to_size_[i + x_off] - e - 1)) {
            fd_read = std::numeric_limits<double>::infinity();
          } else if (std::min(i - t1.postl_to_size_[i + x_off] + e + 1, y_size) < j - t2.postl_to_size_[j + y_off]) {
            fd_read = std::numeric_limits<double>::infinity();
          } else {
            fd_read = fd_.read_at(i - t1.postl_to_size_[i + x_off], j - t2.postl_to_size_[j + y_off]);
          }
        }
        candidate_result = std::min(candidate_result, fd_read + td_read);

        // Value at (i-1,j) may not be calculated due to truncated tree,
        // thus it has to be verified separately.
        // NOTE: For i=1 there may be an out of bound exception for depth
        //       lookup.
        if (i == 1 || (i > 1 && t1.postl_to_depth_.at(i - 1 + x_off) - t1.postl_to_depth_[x] <= e + 1)) {
          candidate_result = std::min(
            candidate_result,
            fd_.read_at(i - 1, j) + c_.del(t1.postl_to_label_id_[i + x_off])
          );
        }

        // None of the values in fd_ can be greater than e-value for this
        // subtree pair.
        if (candidate_result > e) {
          fd_.at(i, j) = std::numeric_limits<double>::infinity();
        } else {
          fd_.at(i, j) = candidate_result;
        }
      }
    }
    if (i + e + 1 <= y_size) { // Last j that is outside e-strip.
      fd_.at(i, i + e + 1) = std::numeric_limits<double>::infinity();
    }
  }

  ++subproblem_counter_;
  // QUESTION: Is it possible that for some e-value an infinity should be
  //           returned, because the last subproblem is too far away?
  candidate_result = std::min({
    fd_.read_at(x_size - 1, y_size) + c_.del(t1.postl_to_label_id_[x]),                 // Delete root in source subtree.
    fd_.read_at(x_size, y_size - 1) + c_.ins(t2.postl_to_label_id_[y]),                 // Insert root in destination subtree.
    fd_.read_at(x_size - 1, y_size - 1) + c_.ren(t1.postl_to_label_id_[x], t2.postl_to_label_id_[y]) // Rename root nodes of the subtrees.
  });
  // The distance between two subtrees cannot be greater than e-value for these
  // subtrees.
  if (candidate_result > e) {
    return std::numeric_limits<double>::infinity();
  } else {
    return candidate_result;
  }
}