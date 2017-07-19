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

  // TODO: If the sizes of index vectors are not initialised, index_nodes
  //       should be used to get input tree sizes.
  const int kT1Size = t1.get_tree_size();
  const int kT2Size = t2.get_tree_size();

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
  // TODO: This loop should iterate over all necessary node pairs, and not
  //       verify the validity of each node pair.
  for (int x = 0; x < kT1Size; ++x) {
    // TODO: Verify the boundaries.
    // TODO: Implement outer loops correctness test. Input: T1,T2,k. Output: (x,y) pairs.
    // Initialise the entire row to infinity.
    for (int y = 0; y < kT2Size; ++y) {
      td_.at(x, y) = std::numeric_limits<double>::infinity();
    }
    for (int y = std::max(0, x - k); y <= std::min(x + k, kT2Size-1); ++y) {
      // TODO: Implement the if below.
      // if not k-relevant(x, y)
      //   td(x, y) <- infty
      // else
      //   compute td(x, y) with e errors
      td_.at(x, y) = 1;
    }
  }

  return td_.at(kT1Size-1, kT2Size-1);
}

template <typename Label, typename CostModel>
const typename Algorithm<Label, CostModel>::TestItems Algorithm<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_size_,
    td_,
  };
  return test_items;
}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_IMPL_H
