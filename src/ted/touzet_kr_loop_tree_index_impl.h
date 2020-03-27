// The MIT License (MIT)
// Copyright (c) 2019 Mateusz Pawlik.
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

/// Contains the implementation of the Touzet KR Loop algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double TouzetKRLoopTreeIndex<CostModel, TreeIndex>::ted_k(const TreeIndex& t1,
    const TreeIndex& t2, const int k) {
  
  const int t1_size = t1.tree_size_;
  const int t2_size = t2.tree_size_;

  init_matrices(t1_size, k);

  // Reset subproblem counter.
  subproblem_counter_ = 0;

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_size - t2_size) > k) {
    return std::numeric_limits<double>::infinity();
  }
  
  // NOTE: There is an alternative approach to the kr-loop solution.
  //       Given kr pair, find top_x as in the current loop. Find top_y by
  //       swapping x and y parameters.
  //       The two solutions differ in the number of steps for the worst and
  //       best case.
  //       TODO: Verify empirically.
  
  // Iterate over all keyroot node pairs.
  for (auto x : t1.list_kr_) {
    for (auto y : t2.list_kr_) {
      int top_x = -1;
      int top_y = -1;
      // Search for top relevant pair.
      int x_l = x;
      while (x_l >= 0) { // We have to go through all x. Once we find a top_y,
                         // the consecutive x have to be checked against only
                         // y_l > top_y.
        int y_l = y;
        while (y_l > top_y) { // Verify only those nodes on the left
                              // path from y that are above the
                              // already found relevant node.
          if (k_relevant(t1, t2, x_l, y_l, k)) { // The pair has to be in the band
                                         // (std::abs(x_l - y_l) <= k satisfied
                                         // by the stronger k-relevancy)
                                         // and it has to be relevant.
            if (top_x == -1) top_x = x_l; // The first top_x found is maximal.
            top_y = y_l; // y_l always > top_y; std::max(top_y, y_l) not needed.
            break; // Don't continue down the path in the right-hand tree.
          }
          y_l = t2.postl_to_lch_[y_l];
        }
        x_l = t1.postl_to_lch_[x_l];
      }
      if (top_x > -1 && top_y > -1) {
        int e_max = e_budget(t1, t2, top_x, top_y, k);
        // Get max e over node pairs on left paths.
        // TODO: Whether to do it or not has to be evaluated. I don't remember
        //       but I think it is kind of an optimization if e_max can be
        //       smaller than k.
        // if (compute_e_max) {
          // e_max = 0;
          // int x_i = top_x;
          // while (x_i > -1) {
          //   int y_i = top_y;
          //   while (y_i > -1) {
          //     e_max = std::max(e_max, e_budget(t1, t2, x_i, y_i, k));
          //     y_i = t2.postl_to_lch_[y_i];
          //   }
          //   x_i = t1.postl_to_lch_[x_i];
          // }
        // }
        td_.at(top_x, top_y) = tree_dist(t1, t2, top_x, top_y, k, e_max);
      }
    }
  }
  return td_.read_at(t1_size-1, t2_size-1);
}