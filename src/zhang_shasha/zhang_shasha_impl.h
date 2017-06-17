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

/// \file zhang_shasha/zhang_shasha_impl.h
///
/// \details
/// Contains the implementation of the Zhang and Shasha algorithm.

#ifndef TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_IMPL_H
#define TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_IMPL_H

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes_recursion(const node::Node<Label>& node, int& start_postorder, int& start_preorder) {
  // TODO: kr and lld must be passed by reference.
  // TODO: The call node.label().label() looks little bit odd.
  std::cout << "-- node : " << node.label().label() << std::endl;
  // Here, start_preorder holds this node's preorder id here.
  std::cout << "-- preorder : " << start_preorder << std::endl;
  // Increment start_preorder for the consecutive node in preorder have the
  // correct id.
  start_preorder++;
  // Recursions to childen nodes.
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it=std::end(node.get_children());
  // Treat the first child separately (non-key-root, updates parent's lld).
  int first_child_postorder = -1;
  if (children_start_it != children_end_it) {
    index_nodes_recursion(*children_start_it, start_postorder, start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Set this node's lld to its first child's lld.
    first_child_postorder = start_postorder-1;
    // Continue to consecutive children.
    ++children_start_it;
  }
  while (children_start_it != children_end_it) {
    index_nodes_recursion(*children_start_it, start_postorder, start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Add current child to kr.
    kr1_.push_back(start_postorder-1);
    // Continue to consecutive children.
    ++children_start_it;
  }
  // Here, start_postorder holds this node's postorder id here.
  std::cout << "-- postorder : " << start_postorder << std::endl;
  std::cout << "-- leaf : " << node.is_leaf() << std::endl;
  if (node.is_leaf()) {
    // Set lld of this node to this node's postorer.
    lld1_.push_back(start_postorder);
  } else {
    std::cout << "-- first_child_postorder : " << first_child_postorder << std::endl;
    std::cout << "-- lld1_.size : " << lld1_.size() << std::endl;
    // This node's lld must be pushed after its childrens llds.
    // lld is indexed starting with 0, thus first_child_postorder-1.
    lld1_.push_back(lld1_.at(first_child_postorder-1));
  }
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  start_postorder++;
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes(const node::Node<Label>& root) {
  std::cout << "--- index_nodes ---" << std::endl;

  // Orders start with '1'. If '0' is needed, either index_nodes_recursion must
  // be modified (placement of increments must change), or we start with '-1'.
  // The order-variables are modified by index_nodes_recursion.
  int start_postorder = 1;
  // TODO: Preorder is not used in Zhang and Shasha. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 1;
  index_nodes_recursion(root, start_postorder, start_preorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.

  // Add root to kr - not added in the recursion.
  kr1_.push_back(start_postorder-1);

  // Print stuff.
  std::cout << "-- lld1_ : ";
  for (auto e : lld1_) std::cout << e << ' ';
  std::cout << std::endl;
  std::cout << "-- kr1_ : ";
  for (auto e : kr1_) std::cout << e << ' ';
  std::cout << std::endl;
}

template <typename Label, typename CostModel>
double Algorithm<Label, CostModel>::zhang_shasha_ted(const node::Node<Label>& t1, const node::Node<Label>& t2, const CostModel& c) {
  std::cout << "=== zhang_shasha_ted ===" << std::endl;
  index_nodes(t1);
  return 5.0;
}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_IMPL_H
