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
Algorithm<Label, CostModel>::Algorithm() : c_() {}

template <typename Label, typename CostModel>
int Algorithm<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::vector<int>& size,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    int& start_postorder,
    int& start_preorder) {

  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;

  // Here, start_preorder holds this node's preorder id.

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  start_preorder++;

  // Recursions to childen nodes.
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it=std::end(node.get_children());
  while (children_start_it != children_end_it) {
    desc_sum += index_nodes_recursion(*children_start_it, size, nodes,
                                      start_postorder, start_preorder);
    // Continue to consecutive children.
    ++children_start_it;
  }

  // Here, start_postorder holds this node's postorder id.

  if (node.is_leaf()) {
    // Leaf has size 1.
    size.push_back(1);
  } else {
    // Inner node has size desc_sum + 1.
    size.push_back(desc_sum + 1);
  }

  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));

  // Increment start_postorder for the consecutive node in postorder to have the
  // correct id.
  start_postorder++;

  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::vector<int>& size,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes) {

  // Index nodes of a tree.

  // TODO: Modify this comment.
  // Orders start with '1'. If '0' is needed, either index_nodes_recursion must
  // be modified (placement of increments must change), or we start with '0'.
  // The order-variables are modified by index_nodes_recursion.
  int start_postorder = 0;
  // NOTE: Preorder is not used in Touzet. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 0;
  index_nodes_recursion(root, size, nodes, start_postorder, start_preorder);

  // Here, start_postorder and start_preorder store the size of tree minus 1.
}

template <typename Label, typename CostModel>
double Algorithm<Label, CostModel>::touzet_ted(const node::Node<Label>& t1,
                                               const node::Node<Label>& t2,
                                               const int& k) {
  using data_structures::Matrix;

  // std::cerr << "--- touzet_ted ---" << std::endl;

  // TODO: If the sizes of index vectors are not initialised, index_nodes
  //       should be used to get input tree sizes.
  const int kT1Size = t1.get_tree_size();
  const int kT2Size = t2.get_tree_size();

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs((kT1Size-1) - (kT2Size-1)) > k) {
    return std::numeric_limits<double>::infinity();
  }

  // NOTE: The default constructor of Matrix is called while constructing ZS-Algorithm.
  // NOTE: Shouldn't we implement Matrix::resize() instead of constructing matrix again?
  td_ = Matrix<double>(kT1Size, kT2Size);
  fd_ = Matrix<double>(kT1Size+1, kT2Size+1);

  // Cleanup node indexes for consecutive use of the algorithm.
  t1_size_.clear();
  t2_size_.clear();
  t1_node_.clear();
  t2_node_.clear();
  // NOTE: It may be better to allocate the vectors with tree sizes and fill
  //       them in. Currently the correct values are collected in postorder and
  //       pushed-back. That results in linear-number of push_back invocations.
  //       The efficiency of that approach is unsure.
  index_nodes(t1, t1_size_, t1_node_);
  index_nodes(t2, t2_size_, t2_node_);

  // Nested loop over all node pairs in k-strip : |x-y|<=k.
  // NOTE: This loop iterates over all node pairs from k-strip, and verifies
  //       their k-relevancy.
  for (int x = 0; x < kT1Size; ++x) {
    // Initialise the entire row to infinity - not necessarily needed.
    // TODO: Verify which td values are used in forest distance.
    for (int y = 0; y < kT2Size; ++y) {
      td_.at(x, y) = std::numeric_limits<double>::signaling_NaN();
    }
    for (int y = std::max(0, x - k); y <= std::min(x + k, kT2Size-1); ++y) {
      // TODO: Implement the if below.
      if (!k_relevant(x, y, k)) {
        // Overwrite NaN to infinity.
        td_.at(x, y) = std::numeric_limits<double>::infinity();
      } else {
        // compute td(x, y) with e errors
        int e_errors = e(x, y, k);
        // td_.at(x, y) = 1;
        // std::cerr << "td(" << x << "," << y << ") = " << std::endl;
        td_.at(x, y) = tree_dist(x, y, k, e_errors);
        // std::cerr << td_.read_at(x, y) << std::endl;
      }
    }
  }

  return td_.at(kT1Size-1, kT2Size-1);
};

template <typename Label, typename CostModel>
double Algorithm<Label, CostModel>::tree_dist(const int& x, const int& y,
                                              const int& k, const int& e) {
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];

  // std::cerr << "IN tree_dist(" << x << "," << y << "," << k << "," << e << std::endl;
  // std::cerr << "subtree sizes: " << x_size << "," << y_size << std::endl;

  // Calculates offsets that let us translate i and j to correct postorder ids.
  int x_off = x - x_size;
  int y_off = y - y_size;

  // Initial cases.
  fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
  for (int j = 1; j <= std::min(y_size, e); ++j) { // i = 0; only j that are within e-strip.
    fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2_node_[j + y_off]);
  }
  if (e + 1 <= y_size) {
    fd_.at(0, e + 1) = std::numeric_limits<double>::infinity(); // the first j that is outside e-strip
  }
  for (int i = 1; i <= std::min(x_size, e); ++i) { // j = 0; only i that are within e-strip.
    fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1_node_[i + x_off]);
  }
  if (e + 1 <= x_size) {
    fd_.at(e + 1, 0) = std::numeric_limits<double>::infinity(); // the first i that is outside e-strip
  }

  // General cases.
  double candidate_result = 0.0;
  for (int i = 1; i <= x_size; ++i) {
    if (i - e - 1 >= 1) { // First j that is outside e-strip.
      fd_.at(i, i - e - 1) = std::numeric_limits<double>::infinity();
    }
    for (int j = std::max(1, i - e); j <= std::min(i + e, y_size); ++j) { // only (i,j) that are in e-strip
      // If (i+x_off,j+y_off) are not in k-strip or are not k-relevant, assign infinity to them.
      if (std::abs((i + x_off) - (j + y_off)) > k || !k_relevant(i + x_off, j + y_off, k)) {
        fd_.at(i, j) = std::numeric_limits<double>::infinity();
      }
      // The td(x_size-1, y_size-1) is computed differently.
      if (i == x_size && j == std::min(i + e, y_size)) {
        break;
      }
      fd_.at(i, j) = std::min({
        fd_.read_at(i - 1, j) + c_.del(t1_node_[i + x_off]),
        fd_.read_at(i, j - 1) + c_.ins(t2_node_[j + y_off]),
        fd_.read_at(i - t1_size_[i + x_off], j - t2_size_[j + y_off]) + td_.read_at(i + x_off, j + y_off)
      });
      // std::cerr << "fd(" << i << "," << j << ") = " << fd_.read_at(i, j) << std::endl;
    }
    if (i + e + 1 <= y_size) { // Last j that is outside e-strip.
      fd_.at(i, i + e + 1) = std::numeric_limits<double>::infinity();
    }
  }
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
int Algorithm<Label, CostModel>::e(const int& x, const int& y, const int& k) const {
  // e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];
  int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
                    std::abs(((x+1) - x_size) - ((y+1) - y_size));
  return (k - lower_bound);
};

template <typename Label, typename CostModel>
bool Algorithm<Label, CostModel>::k_relevant(const int& x, const int& y, const int& k) const {
  // lower bound formula:
  // |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  int x_size = t1_size_[x];
  int y_size = t2_size_[y];
  int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
                    std::abs(x_size - y_size) +
                    std::abs(((x+1) - x_size) - ((y+1) - y_size));
  // std::cerr << x << "," << y << "," << x_size << "," << y_size << ":" << lower_bound << std::endl;
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
const typename Algorithm<Label, CostModel>::TestItems Algorithm<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_size_,
    td_,
  };
  return test_items;
}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_IMPL_H
