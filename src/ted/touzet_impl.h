// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file touzet/touzet_impl.h
///
/// \details
/// Contains the implementation of the Touzet's algorithm.

#ifndef TREE_SIMILARITY_TOUZET_TOUZET_IMPL_H
#define TREE_SIMILARITY_TOUZET_TOUZET_IMPL_H

template <typename Label, typename CostModel>
Touzet<Label, CostModel>::Touzet() : c_() {}

template <typename Label, typename CostModel>
int Touzet<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::vector<int>& size,
    std::vector<int>& depth,
    std::vector<int>& kr,
    std::vector<int>& lch,
    std::vector<int>& parent,
    std::vector<int>& subtree_max_depth,
    std::vector<std::vector<int>>& dil,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    int& start_postorder,
    int& start_preorder,
    int start_depth,
    int& parent_max_depth) {

  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;

  // Here, start_preorder holds this node's preorder id.

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;

  // Add a vector for depth=start_depth in depth inverted list.
  // NOTE: dil.size() returns an unsigned int. If dil.size = 0, then
  //       substracting 1 causes incorrect validation of the condition.
  if (dil.size() < (start_depth + 1)) {
    dil.push_back(std::vector<int>());
  }

  // This node subtree's max depth.
  int this_subtree_max_depth = 0;

  // This node's left child.
  int left_child = -1; // '-1' if there are no children.
  
  // To store postorder ids of this node's children.
  std::vector<int> nodes_children;

  // Recursions to childen nodes.
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    desc_sum += index_nodes_recursion(*children_start_it, size, depth, kr, lch,
                                      parent, subtree_max_depth, dil,
                                      nodes, start_postorder, start_preorder,
                                      start_depth + 1, this_subtree_max_depth);
    if (children_start_it == node.get_children().begin()) {
      left_child = start_postorder-1;
    } else { // if (children_start_it != node.get_children().begin()) {
      // Add current child to kr.
      kr.push_back(start_postorder-1);
    }
    // Collect children ids.
    nodes_children.push_back(start_postorder-1);
    // Continue to consecutive children.
    ++children_start_it;
  }

  // Here, start_postorder holds this node's postorder id.

  // Left child.
  lch.push_back(left_child);
    
  // Parent - dummy element to grow the vector by this node.
  parent.push_back(-1);
  // Set this node's postorder value as parent for all its children.
  for (auto child_id : nodes_children) {
    parent.at(child_id) = start_postorder;
  }

  if (node.is_leaf()) {
    // Leaf has size 1.
    size.push_back(1);
    // If this node has no children, set the max depth to this node's depth.
    this_subtree_max_depth = start_depth;
  } else {
    // Inner node has size desc_sum+1.
    size.push_back(desc_sum + 1);
  }

  // Depth, subtree max depth, and depth inverted list.
  depth.push_back(start_depth);
  dil.at(start_depth).push_back(start_postorder);
  subtree_max_depth.push_back(this_subtree_max_depth);

  // Update parent subtree's max depth.
  parent_max_depth = std::max(parent_max_depth, this_subtree_max_depth);

  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));

  // Increment start_postorder for the consecutive node in postorder to have the
  // correct id.
  ++start_postorder;

  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
}

template <typename Label, typename CostModel>
void Touzet<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::vector<int>& size,
    std::vector<int>& depth,
    std::vector<int>& kr,
    std::vector<int>& lch,
    std::vector<int>& parent,
    std::vector<int>& subtree_max_depth,
    std::vector<std::vector<int>>& dil,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes) {

  // Index nodes of a tree.

  // Orders start with '0'.
  // The order-variables are modified by index_nodes_recursion.
  int start_postorder = 0;
  // NOTE: Preorder is not used in Touzet. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 0;
  // Maximum input tree depth - the first reference passed to recursion.
  int input_max_depth = 0;
  index_nodes_recursion(root, size, depth, kr, lch, parent, subtree_max_depth, dil,
      nodes, start_postorder, start_preorder, 0, input_max_depth);

  // Here, start_postorder and start_preorder store the size of tree minus 1.

  // Add root to kr - not added in the recursion.
  kr.push_back(start_postorder-1);
}

template <typename Label, typename CostModel>
void Touzet<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2, const int k) {
  using data_structures::BandMatrix;

  // Cleanup node indices for consecutive use of the algorithm.
  t1_size_.clear();
  t2_size_.clear();
  t1_node_.clear();
  t2_node_.clear();
  t1_depth_.clear();
  t2_depth_.clear();
  t1_kr_.clear();
  t2_kr_.clear();
  t1_lch_.clear();
  t2_lch_.clear();
  t1_parent_.clear();
  t2_parent_.clear();
  t1_subtree_max_depth_.clear();
  t2_subtree_max_depth_.clear();
  t1_dil_.clear();
  t2_dil_.clear();
  // NOTE: It may be better to allocate the vectors with tree sizes and fill
  //       them in. Currently the correct values are collected in postorder and
  //       pushed-back. That results in linear-number of push_back invocations.
  //       The efficiency of that approach is not evaluated.
  //       However, to get the tree size, one tree traversal is required.
  index_nodes(t1, t1_size_, t1_depth_, t1_kr_, t1_lch_, t1_parent_,
      t1_subtree_max_depth_, t1_dil_, t1_node_);
  index_nodes(t2, t2_size_, t2_depth_, t2_kr_, t2_lch_, t2_parent_,
      t2_subtree_max_depth_, t2_dil_, t2_node_);

  // NOTE: Retrive the input tree sizes. Do not call get_tree_size() that causes
  //       an additional tree traversal.
  t1_input_size_ = t1_size_.back();
  t2_input_size_ = t2_size_.back();

  // Reset subproblem counter.
  subproblem_counter = 0;

  // NOTE: The default constructor of Matrix is called while constructing
  //       the algorithm object.
  // QUESTION: Shouldn't we implement Matrix::resize() instead of constructing
  //           matrix again?
  // NOTE: The k may be larger than |T2| that uses more memory than needed.
  td_ = BandMatrix<double>(t1_input_size_, k);
  // Fill in for initially marking all subtree pairs as not eligable, and for debugging.
  td_.fill_with(std::numeric_limits<double>::signaling_NaN());
  // NOTE: The band_width=e for Touzet's fd_ matrix varies. It is however
  //       smaller or equal to the initialised band_width=k+1. As long as we
  //       read and write using the original band_width, addresses are not
  //       messed up. We only have to ensure that we do not iterate over too
  //       many elements using k instead of e.
  fd_ = BandMatrix<double>(t1_input_size_ + 1, k + 1);
  fd_.Matrix::fill_with(std::numeric_limits<double>::infinity());
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::touzet_ted(const node::Node<Label>& t1,
                                            const node::Node<Label>& t2,
                                            const int k) {
  // Reset internal data structures and index nodes.
  init(t1, t2, k);

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_input_size_ - t2_input_size_) > k) {
    return std::numeric_limits<double>::infinity();
  }

  // Loop over keyroot node pairs. If the pair is not k-relevant, try their
  // leftmost children. This has to return all top-pairs that are k-relevant.
  // That means, for the pair (k1,k2), if (k1,k2) is not relevant, but
  // (k1,leftchild(k2)) and (leftchild(k1),k2) are, then those two must be
  // returned.
  // BUG: This does too much. In the case above, it should return the pair
  //      (k1,k2), but maybe not compute until the end.
  // IDEA: Maybe Nikolaus's idea to start computing from the leaf and stop
  //       ass soon as we can is better than finding the nodes on the path.

  for (auto x : t1_kr_) {
    for (auto y : t2_kr_) {
      int x_i = x;
      int top_y = -1;
      while (x_i > -1) {
        int y_i = y;
        while (y_i > top_y) {
          // Check band condition.
          if (!(y_i < std::max(0, x_i - k) || y_i > std::min(x_i + k, t2_input_size_-1))) { // TODO: Iterate only over y_i in the band.
            // Check relevancy.
            if (!k_relevant(x_i, y_i, k)) {
              td_.at(x_i, y_i) = std::numeric_limits<double>::infinity();
              // continue;
            } else {
              // if (std::isnan(td_.read_at(x_i, y_i))) {
                // std::cout << "(" + std::to_string(x_i) + "," + std::to_string(y_i) + ")" << std::endl;
                td_.at(x_i, y_i) = tree_dist(x_i, y_i, k, e(x_i, y_i, k));
              // }
              top_y = y_i;
              break;
            }
          }
          // Increment y_i to the left child of y_i.
          y_i = t2_lch_[y_i];
        }
        // NOTE: Break the outher loop if top_y = y;
        if (top_y == y) {
          break;
        }
        // Increment x_i to the left child of x_i.
        x_i = t1_lch_[x_i];
      }
    }
  }
  return td_.at(t1_input_size_-1, t2_input_size_-1);
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::tree_dist(const int x, const int y,
                                              const int k, const int e) {
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];

  // Calculates offsets that let us translate i and j to correct postorder ids.
  int x_off = x - x_size;
  int y_off = y - y_size;

  // Initial cases.
  fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
  for (int j = 1; j <= std::min(y_size, e); ++j) { // i = 0; only j that are within e-strip.
    fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2_node_[j + y_off]);
  }
  if (e + 1 <= y_size) { // the first j that is outside e-strip
    fd_.at(0, e + 1) = std::numeric_limits<double>::infinity();
  }

  for (int i = 1; i <= std::min(x_size, e); ++i) { // j = 0; only i that are within e-strip.
    fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1_node_[i + x_off]);
  }
  if (e + 1 <= x_size) { // the first i that is outside e-strip
    fd_.at(e + 1, 0) = std::numeric_limits<double>::infinity();
  }

  double candidate_result = std::numeric_limits<double>::infinity();

  // General cases.
  for (int i = 1; i <= x_size; ++i) {
    if (i - e - 1 >= 1) { // First j that is outside e-strip - can be read for the cell to the right.
      fd_.at(i, i - e - 1) = std::numeric_limits<double>::infinity();
      ++subproblem_counter;
    }
    for (int j = std::max(1, i - e); j <= std::min(i + e, y_size); ++j) { // only (i,j) that are in e-strip
      ++subproblem_counter;
      // NOTE: It's about existence of a path from (i,j) to (x-x_size,y-y_size).
      //       It exists only then, if it existed for the neighburing nodes
      //       adding the costs for coming to (i,j).
      //       We don't have to verify e-strip because that's ensured with the
      //       for loop and the first j and the first i outside e-strip.
      // QUESTION: What about k-strip?
      //           (i,j), in the input trees scope including offsets, must be
      //           in k-strip. Otherwise, there is certainly no edit path from
      //           (i,j) to (0,0). In other words, thre is no edit path from
      //           (i,j) to (0,0) with the given budget k.
      // QUESTION: What about k-relevancy?
      //           Adding this to the condition below makes ted tests fail.
      //           (a,b) is k-relevant if (T1_a,T2_b) can be mapped based on
      //           the size lower bound of the nodes around these subtrees.
      //           Thus, it doesn't fit here.
      if (std::abs((i + x_off) - (j + y_off)) > k) {
        fd_.at(i, j) = std::numeric_limits<double>::infinity();
        // QUESTION: Do we have to check if this can be a pair of subtrees?
        //           Maybe not because eventhough we will read NaN and conver to inf.
        // if (i - t1_size_[i + x_off] == 0 && j - t2_size_[j + y_off] == 0) { // Pair of two subtrees.
        //   td_.at(i + x_off, j + y_off) = std::numeric_limits<double>::infinity();
        // }
      } else {
        candidate_result = std::numeric_limits<double>::infinity();
        candidate_result = std::min(candidate_result, fd_.read_at(i - 1, j) + c_.del(t1_node_[i + x_off]));
        candidate_result = std::min(candidate_result, fd_.read_at(i, j - 1) + c_.ins(t2_node_[j + y_off]));

        double fd_read = 0.0;
        // If one of the forests is a tree, look up the vlaues in fd_.
        // Otherwise, both forests are trees and the fd-part is empty.
        if (i - t1_size_[i + x_off] != 0 || j - t2_size_[j + y_off] != 0) { // TODO: Swap if-else conditions or use '>0'.
          // TODO: We're reading here NaN values because the pair of rightmost
          //       subtrees may not be eligable.
          // QUESTION: Is it possible that we read from outside the band? - No exception thrown.
          double td_read = td_.read_at(i + x_off, j + y_off);
          if (std::isnan(td_read)) { // TODO: We can't map rightmost subtrees, thus we can't map left forests? - Correct results.
            td_read = std::numeric_limits<double>::infinity();
            candidate_result = std::numeric_limits<double>::infinity();
          } else {
            // If the values to read are outside of the band, they exceed
            // the threshold or are not present in the band-matrix.
            if (j - t2_size_[j + y_off] < std::max(0, i - t1_size_[i + x_off] - e - 1)) {
              fd_read = std::numeric_limits<double>::infinity();
            } else if (std::min(i - t1_size_[i + x_off] + e + 1, y_size) < j - t2_size_[j + y_off]) {
              fd_read = std::numeric_limits<double>::infinity();
            } else {
              fd_read = fd_.read_at(i - t1_size_[i + x_off], j - t2_size_[j + y_off]);
            }
            candidate_result = std::min(candidate_result, fd_read + td_read);
          }
        } else { // Pair of two subtrees.
          fd_read = fd_.read_at(i - 1, j - 1) + c_.ren(t1_node_[i + x_off], t2_node_[j + y_off]);
          candidate_result = std::min(candidate_result, fd_read);
          // Write to td_ only if there is no value.
          // if (std::isnan(td_.at(i + x_off, j + y_off))) {
            if (candidate_result > e ) {
              td_.at(i + x_off, j + y_off) = std::numeric_limits<double>::infinity();
            } else {
              td_.at(i + x_off, j + y_off) = candidate_result;
            }
          // }
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
    if (i + e + 1 <= y_size) { // Last j that is outside e-strip - can be read from the row below.
      fd_.at(i, i + e + 1) = std::numeric_limits<double>::infinity();
      ++subproblem_counter;
    }
  }

  ++subproblem_counter;
  // QUESTION: Is it possible that for some e-value an infinity should be
  //           returned, because the last subproblem is too far away?

  // The distance between two subtrees cannot be greater than e-value for these
  // subtrees.
  if (candidate_result > e) {
    return std::numeric_limits<double>::infinity();
  } else {
    return candidate_result;
  }
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::touzet_ted_orig_nored(const node::Node<Label>& t1,
                                               const node::Node<Label>& t2,
                                               const int k) {
  init(t1, t2, k);

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_input_size_ - t2_input_size_) > k) {
    return std::numeric_limits<double>::infinity();
  }

  // // Nested loop over all node pairs in k-strip : |x-y|<=k. This loop iterates
  // // over all node pairs from k-strip, and verifies their k-relevancy.
  // for (int x = 0; x < t1_input_size_; ++x) {
  //   for (int y = std::max(0, x - k); y <= std::min(x + k, t2_input_size_-1); ++y) {
  //     if (!k_relevant(x, y, k)) {
  //       // Overwrite NaN to infinity.
  //       td_.at(x, y) = std::numeric_limits<double>::infinity(); std::cout << "td_" << std::endl;
  //     } else {
  //       // Compute td(x, y) with e errors - the value of e(x, y, k).
  //       td_.at(x, y) = tree_dist_orig_nored(x, y, k, e(x, y, k)); std::cout << "td_" << std::endl;
  //       std::cout << "(" + std::to_string(x) + "," + std::to_string(y) + ") = " + std::to_string(td_.read_at(x, y)) << std::endl;
  //     }
  //   }
  // }
  
  for (auto x : t1_kr_) {
    int x_size = t1_size_[x];
    int x_off = x - x_size; // Offset set for a keyroot.
    
    for (auto y : t2_kr_) {
      std::cout << "KR(" + std::to_string(x) + "," + std::to_string(y) + ")" << std::endl;
      int y_size = t2_size_[y];
      int y_off = y - y_size; // Offset set for a keyroot.
      
      
      
      
      int e_kr = e(x, y, k); // BUG: The relevancy of (x,y) is not verified causing e_kr being negative.
      
      // Do initial 0-cases here - this allows not to deal with them while
      // walking the left paths.
      // --- BEGIN initial cases
      fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
      // BUG: The first i and j outside band should store infinities.
      for (int j = 1; j <= fd_.get_band_width(); ++j) { // i = 0; only j that are within e-strip.
        fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2_node_[j + y_off]);
      }
      for (int i = 1; i <= fd_.get_band_width(); ++i) { // j = 0; only i that are within e-strip.
        fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1_node_[i + x_off]);
      }
      // for (int j = 1; j <= std::min(y_size, e_kr); ++j) { // i = 0; only j that are within e-strip.
      //   fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2_node_[j + y_off]);
      // }
      // if (e_kr + 1 <= y_size) { // the first j that is outside e-strip
      //   fd_.at(0, e_kr + 1) = std::numeric_limits<double>::infinity();
      // }
      // for (int i = 1; i <= std::min(x_size, e_kr); ++i) { // j = 0; only i that are within e-strip.
      //   fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1_node_[i + x_off]);
      // }
      // if (e_kr + 1 <= x_size) { // the first i that is outside e-strip
      //   fd_.at(e_kr + 1, 0) = std::numeric_limits<double>::infinity();
      // }
      // --- END initial cases
      
      int x_l = x - x_size + 1; // leftmost leaf of x
      while (x_l >= 0 && x_l <= x) {
        int y_l = y - y_size + 1; // leftmost leaf of y
        while (y_l >= 0 && y_l <= y) { // TODO: Iterate only over y_i in the band.
          std::cout << "LL(" + std::to_string(x_l) + "," + std::to_string(y_l) + ")" << std::endl;
          // for (int y = std::max(0, x - k); y <= std::min(x + k, t2_input_size_-1); ++y) {
          if (y_l < std::max(0, x_l - k) || y_l > std::min(x_l + k, t2_input_size_-1)) {
            // y_l is outside the band
            
            // NOTE: Do not store things because (x_l,y_l) is outside the band.
            
            // td_.at(x_l, y_l) = std::numeric_limits<double>::infinity();
          // }
          // Check relevancy.
          // if (!k_relevant(x_l, y_l, k)) {
            // td_.at(x_l, y_l) = std::numeric_limits<double>::infinity();
          } else {  
            td_.at(x_l, y_l) = tree_dist_orig_nored(x_l, y_l, k, e_kr, x_off, y_off);
            std::cout << "(" + std::to_string(x_l) + "," + std::to_string(y_l) + ") = " + std::to_string(td_.read_at(x_l, y_l)) << std::endl;
          }
          
          // Increment y_l to the parent of y_l.
          y_l = t2_parent_[y_l];
        }
        // Increment x_l to the parent of x_l.
        x_l = t1_parent_[x_l];
      }
    }
  }
  return td_.read_at(t1_input_size_-1, t2_input_size_-1);
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::tree_dist_orig_nored(const int x_l,
      const int y_l, const int k, const int e, const int x_off, const int y_off) {
  
  int x_l_size = t1_size_[x_l];
  int y_l_size = t2_size_[y_l];
  
  int x_lch = t1_lch_[x_l];
  int y_lch = t2_lch_[y_l];

  int x_lch_size = 0;
  int y_lch_size = 0;
  if (x_lch > -1) x_lch_size = t1_size_[x_lch];
  if (y_lch > -1) y_lch_size = t2_size_[y_lch];

  double candidate_result = std::numeric_limits<double>::infinity();

  // General cases.
  for (int i = 1; i <= x_l_size - x_lch_size; ++i) {
    if (i - e - 1 >= 1) { // First j that is outside e-strip - can be read for the cell to the right.
      fd_.at(i, i - e - 1) = std::numeric_limits<double>::infinity(); //std::cout << std::to_string(fd_.read_at(i, i - e - 1)) << std::endl;
      ++subproblem_counter;
    }
    for (int j = std::max(1, i - e); j <= std::min(i + e, y_l_size - y_lch_size); ++j) { // only (i,j) that are in e-strip
      ++subproblem_counter;
      if (std::abs((i + x_off) - (j + y_off)) > k) {
        fd_.at(i, j) = std::numeric_limits<double>::infinity(); //std::cout << std::to_string(fd_.read_at(i, j)) << std::endl;
      } else {
        candidate_result = std::numeric_limits<double>::infinity();
        candidate_result = std::min(candidate_result, fd_.read_at(i - 1, j) + c_.del(t1_node_[i + x_off]));
        candidate_result = std::min(candidate_result, fd_.read_at(i, j - 1) + c_.ins(t2_node_[j + y_off]));

        double fd_read = 0.0;
        // If one of the forests is a tree, look up the vlaues in fd_.
        // Otherwise, both forests are trees and the fd-part is empty.
        if (i - t1_size_[i + x_off] != 0 || j - t2_size_[j + y_off] != 0) { // TODO: Swap if-else conditions or use '>0'.
          // TODO: We're reading here NaN values because the pair of rightmost
          //       subtrees may not be eligable.
          // QUESTION: Is it possible that we read from outside the band? - No exception thrown.
          double td_read = td_.read_at(i + x_off, j + y_off);
          if (std::isnan(td_read)) { // TODO: We can't map rightmost subtrees, thus we can't map left forests? - Correct results.
            td_read = std::numeric_limits<double>::infinity();
            candidate_result = std::numeric_limits<double>::infinity();
          } else {
            // If the values to read are outside of the band, they exceed
            // the threshold or are not present in the band-matrix.
            if (j - t2_size_[j + y_off] < 0 || i - t1_size_[i + x_off] < 0) {
              if (j - t2_size_[j + y_off] < 0 && i - t1_size_[i + x_off] < 0) {
                fd_read = fd_.read_at(0, 0);
              } else if (j - t2_size_[j + y_off] < 0) {
                fd_read = fd_.read_at(i - t1_size_[i + x_off], 0);
              } else if (i - t1_size_[i + x_off] < 0) {
                fd_read = fd_.read_at(0, j - t2_size_[j + y_off]);
              }
            } else {
              if (j - t2_size_[j + y_off] < std::max(0, i - t1_size_[i + x_off] - e - 1)) {
                fd_read = std::numeric_limits<double>::infinity();
              } else if (std::min(i - t1_size_[i + x_off] + e + 1, y_l_size - y_lch_size) < j - t2_size_[j + y_off]) {
                fd_read = std::numeric_limits<double>::infinity();
              } else {
                fd_read = fd_.read_at(i - t1_size_[i + x_off], j - t2_size_[j + y_off]);
              }
            }
            candidate_result = std::min(candidate_result, fd_read + td_read);
          }
        } else { // Pair of two subtrees.
          fd_read = fd_.read_at(i - 1, j - 1) + c_.ren(t1_node_[i + x_off], t2_node_[j + y_off]);
          candidate_result = std::min(candidate_result, fd_read);
          // Write to td_ only if there is no value.
          // if (std::isnan(td_.at(i + x_off, j + y_off))) {
            if (candidate_result > e ) {
              td_.at(i + x_off, j + y_off) = std::numeric_limits<double>::infinity(); //std::cout << "td_" << std::endl;
            } else {
              td_.at(i + x_off, j + y_off) = candidate_result; //std::cout << "td_" << std::endl;
            }
          // }
        }

        // None of the values in fd_ can be greater than e-value for this
        // subtree pair.
        if (candidate_result > e) {
          fd_.at(i, j) = std::numeric_limits<double>::infinity(); //std::cout << std::to_string(fd_.read_at(i, j)) << std::endl;
        } else {
          fd_.at(i, j) = candidate_result; //std::cout << std::to_string(fd_.read_at(i, j)) << std::endl;
        }
      }
    }
    if (i + e + 1 <= y_l_size) { // Last j that is outside e-strip - can be read from the row below.
      fd_.at(i, i + e + 1) = std::numeric_limits<double>::infinity();// std::cout << std::to_string(fd_.read_at(i, i + e + 1)) << std::endl;
      ++subproblem_counter;
    }
  }

  ++subproblem_counter;
  // QUESTION: Is it possible that for some e-value an infinity should be
  //           returned, because the last subproblem is too far away?

  // The distance between two subtrees cannot be greater than e-value for these
  // subtrees.
  if (candidate_result > e) {
    return std::numeric_limits<double>::infinity();
  } else {
    return candidate_result;
  }
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::touzet_ted_depth_pruning(const node::Node<Label>& t1,
                                               const node::Node<Label>& t2,
                                               const int k) {
  init(t1, t2, k);

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_input_size_ - t2_input_size_) > k) {
    return std::numeric_limits<double>::infinity();
  }

  // Nested loop over all node pairs in k-strip : |x-y|<=k. This loop iterates
  // over all node pairs from k-strip, and verifies their k-relevancy.
  for (int x = 0; x < t1_input_size_; ++x) {
    for (int y = std::max(0, x - k); y <= std::min(x + k, t2_input_size_-1); ++y) {
      if (!k_relevant(x, y, k)) {
        // Overwrite NaN to infinity.
        td_.at(x, y) = std::numeric_limits<double>::infinity();
      } else {
        // Compute td(x, y) with e errors - the value of e(x, y, k).
        td_.at(x, y) = tree_dist_depth_pruning(x, y, k, e(x, y, k));
      }
    }
  }
  return td_.at(t1_input_size_-1, t2_input_size_-1);
};

template <typename Label, typename CostModel>
double Touzet<Label, CostModel>::tree_dist_depth_pruning(const int x, const int y,
                                              const int k, const int e) {
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];

  // Calculates offsets that let us translate i and j to correct postorder ids.
  int x_off = x - x_size;
  int y_off = y - y_size;

  // Initial cases.
  fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
  for (int j = 1; j <= std::min(y_size, e); ++j) { // i = 0; only j that are within e-strip.
    fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2_node_[j + y_off]);
  }
  if (e + 1 <= y_size) { // the first j that is outside e-strip
    fd_.at(0, e + 1) = std::numeric_limits<double>::infinity();
  }
  // QUESTION: Is it necessary to verify depths here?
  //           It is not, because these values will never be used. We write
  //           them not to verify the condition.
  for (int i = 1; i <= std::min(x_size, e); ++i) { // j = 0; only i that are within e-strip.
    fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1_node_[i + x_off]);
  }
  if (e + 1 <= x_size) { // the first i that is outside e-strip
    fd_.at(e + 1, 0) = std::numeric_limits<double>::infinity();
  }

  double candidate_result = std::numeric_limits<double>::infinity();

  // General cases.

  // NOTE: max_depth has to be set to min(depth(x)+e+1, max depth of T1_x)
  //       because e+1 may exceed the maximum depth of T1_x.
  // TODO: Figure out why e+1.
  int max_depth = std::min(t1_depth_.at(x) + e + 1, t1_subtree_max_depth_.at(x));

  int i = 1;
  int max_depth_it = 0;
  // Move max_depth_it to point to the first node within this subtree (T1_x).
  // TODO: This requires a scan of one depth inversted list. Can we do better?
  while (t1_dil_.at(max_depth).at(max_depth_it) < i + x_off) {
    ++max_depth_it;
  }
  // Set the first i-value for the loop
  // TODO: Make this if more elegant.
  if (t1_depth_.at(i + x_off) > max_depth) {
    // x_off has to be substructed to get correct i.
    i = t1_dil_.at(max_depth).at(max_depth_it) - x_off;
    ++max_depth_it;
  } else if (t1_depth_.at(i + x_off) == max_depth) {
    ++max_depth_it;
  }
  // Traversing truncated tree to filter out i-values based on depth.
  while (i <= x_size) {
    // Old if statement for filtering i-values based on depth.
    // if (t1_depth_[i + x_off] - t1_depth_[x] > e + 1) { continue; }
    if (i - e - 1 >= 1) { // First j that is outside e-strip.
      fd_.at(i, i - e - 1) = std::numeric_limits<double>::infinity();
      ++subproblem_counter;
    }
    for (int j = std::max(1, i - e); j <= std::min(i + e, y_size); ++j) { // only (i,j) that are in e-strip
      // The td(x_size-1, y_size-1) is computed differently.
      // TODO: This condition is evaluated too often but passes only on the
      //       last i and j.
      if (i == x_size && j == std::min(i + e, y_size)) {
        break;
      }
      ++subproblem_counter;
      if (std::abs((i + x_off) - (j + y_off)) > k) {
        fd_.at(i, j) = std::numeric_limits<double>::infinity();
      } else {
        candidate_result = std::numeric_limits<double>::infinity();
        candidate_result = std::min(candidate_result, fd_.read_at(i, j - 1) + c_.ins(t2_node_[j + y_off]));
        double td_read = td_.read_at(i + x_off, j + y_off);
        double fd_read = 0.0;
        // If one of the forests is a tree, look up the vlaues in fd_.
        // Otherwise, both forests are trees and the fd-part is empty.
        if (i - t1_size_[i + x_off] != 0 || j - t2_size_[j + y_off] != 0) {
          // If the values to read are outside of the band, they exceed
          // the threshold or are not present in the band-matrix.
          if (j - t2_size_[j + y_off] < std::max(0, i - t1_size_[i + x_off] - e - 1)) {
            fd_read = std::numeric_limits<double>::infinity();
          } else if (std::min(i - t1_size_[i + x_off] + e + 1, y_size) < j - t2_size_[j + y_off]) {
            fd_read = std::numeric_limits<double>::infinity();
          } else {
            fd_read = fd_.read_at(i - t1_size_[i + x_off], j - t2_size_[j + y_off]);
          }
        }
        candidate_result = std::min(candidate_result, fd_read + td_read);

        // Value at (i-1,j) may not be calculated due to truncated tree,
        // thus it has to be verified separately.
        // NOTE: For i=1 there may be an out of bound exception for depth
        //       lookup.
        if (i == 1 || (i > 1 && t1_depth_.at(i - 1 + x_off) - t1_depth_[x] <= e + 1)) {
          candidate_result = std::min(
            candidate_result,
            fd_.read_at(i - 1, j) + c_.del(t1_node_[i + x_off])
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
      ++subproblem_counter;
    }
    // Set next i to iterate.
    ++i;
    // NOTE: i <= x_size verifies depth and dil[max_depth] bounds.
    // NOTE: For i=x_size (subtree root) the depth is always smaller than
    //       max_depth.
    // TODO: Make this if more elegant.
    if (i <= x_size && t1_depth_.at(i + x_off) > max_depth) {
      // If depth of i is greater than max_depth, there must be an ancestor
      // of i with depth equals max_depth, and max_depth_it points to it.
      // x_off has to be substructed to get correct i.
      i = t1_dil_.at(max_depth).at(max_depth_it) - x_off;
      ++max_depth_it;
    } else if (i <= x_size && t1_depth_.at(i + x_off) == max_depth) {
      ++max_depth_it;
    }
  }

  ++subproblem_counter;
  // QUESTION: Is it possible that for some e-value an infinity should be
  //           returned, because the last subproblem is too far away?
  candidate_result = std::min({
    fd_.read_at(x_size - 1, y_size) + c_.del(t1_node_[x]),                 // Delete root in source subtree.
    fd_.read_at(x_size, y_size - 1) + c_.ins(t2_node_[y]),                 // Insert root in destination subtree.
    fd_.read_at(x_size - 1, y_size - 1) + c_.ren(t1_node_[x], t2_node_[y]) // Rename root nodes of the subtrees.
  });
  // The distance between two subtrees cannot be greater than e-value for these
  // subtrees.
  if (candidate_result > e) {
    return std::numeric_limits<double>::infinity();
  } else {
    return candidate_result;
  }
};

template <typename Label, typename CostModel>
int Touzet<Label, CostModel>::e(const int x, const int y, const int k) const {
  // Lower bound formula (k - RA - L):
  // e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
  // New lower bound formula (k - R - A - L):
  // e(x,y) = k - |(|T1|-(x+1)-depth(x))-(|T2|-(y+1)-depth(y))| - |depth(x)-depth(y)| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];
  // int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
  //                   std::abs(((x+1) - x_size) - ((y+1) - y_size));
  int lower_bound = std::abs((t1_size_.back() - (x+1) - t1_depth_[x]) - (t2_size_.back() - (y+1) - t2_depth_[y])) +
                    std::abs(t1_depth_[x] - t2_depth_[y]) +
                    std::abs(((x+1) - x_size) - ((y+1) - y_size));
  return (k - lower_bound); // BUG: There is a bug here possibly returning a negative number.
};

template <typename Label, typename CostModel>
bool Touzet<Label, CostModel>::k_relevant(const int x, const int y, const int k) const {
  // The lower bound formula (RA + D + L):
  // |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  // New lower bound formula (R + A + D + L):
  // |(|T1|-(x+1)-depth(x))-(|T2|-(y+1)-depth(y))| + |depth(x)-depth(y)| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];
  // int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
  //                   std::abs(x_size - y_size) +
  //                   std::abs(((x+1) - x_size) - ((y+1) - y_size));

  int lower_bound = std::abs((t1_size_.back() - (x+1) - t1_depth_[x]) - (t2_size_.back() - (y+1) - t2_depth_[y])) +
                    std::abs(t1_depth_[x] - t2_depth_[y]) +
                    std::abs(x_size - y_size) +
                    std::abs(((x+1) - x_size) - ((y+1) - y_size));

  // NOTE: The pair (x,y) is k-relevant if lower_bound <= k.
  //       lower_bound < k is not correct because then (x,y) would be
  //       k-irrelevant for lower_bound = k. That would further mean that the
  //       subtrees T1_x and T2_y cannot be mapped with the given budget.
  if (lower_bound <= k) {
    return true;
  }
  return false;
};

template <typename Label, typename CostModel>
const typename Touzet<Label, CostModel>::TestItems Touzet<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_size_,
    td_,
    fd_,
    t1_depth_,
    t1_kr_,
    t1_lch_,
    t1_parent_,
    t1_dil_,
    t1_subtree_max_depth_,
  };
  return test_items;
}

template <typename Label, typename CostModel>
const unsigned long long int Touzet<Label, CostModel>::get_subproblem_count() const {
  return subproblem_counter;
}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_IMPL_H
