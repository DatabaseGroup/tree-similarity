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
Algorithm<Label, CostModel>::Algorithm() : c_() {}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::vector<int>& lld,
    std::vector<int>& kr,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    int& start_postorder,
    int& start_preorder) {
  std::cout << "-- node : " << node.label().to_string() << std::endl;
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
    index_nodes_recursion(*children_start_it, lld, kr, nodes, start_postorder,
                          start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Set this node's lld to its first child's lld.
    first_child_postorder = start_postorder-1;
    // Continue to consecutive children.
    ++children_start_it;
  }

  while (children_start_it != children_end_it) {
    index_nodes_recursion(*children_start_it, lld, kr, nodes, start_postorder,
                          start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Add current child to kr.
    kr.push_back(start_postorder-1);
    // Continue to consecutive children.
    ++children_start_it;
  }
  // Here, start_postorder holds this node's postorder id here.
  std::cout << "-- postorder : " << start_postorder << std::endl;
  std::cout << "-- leaf : " << node.is_leaf() << std::endl;

  if (node.is_leaf()) {
    // Set lld of this node to this node's postorer.
    lld.push_back(start_postorder);
  } else {
    std::cout << "-- first_child_postorder : " << first_child_postorder << std::endl;
    std::cout << "-- lld.size : " << lld.size() << std::endl;
    // This node's lld must be pushed after its childrens llds.
    // lld is indexed starting with 0, thus first_child_postorder-1.
    lld.push_back(lld.at(first_child_postorder-1));
  }
  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  start_postorder++;
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::vector<int>& lld,
    std::vector<int>& kr,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes) {

  // Index nodes of the source tree.
  std::cout << "--- index_nodes : source tree ---" << std::endl;

  // Orders start with '1'. If '0' is needed, either index_nodes_recursion must
  // be modified (placement of increments must change), or we start with '-1'.
  // The order-variables are modified by index_nodes_recursion.
  int start_postorder = 1;
  // NOTE: Preorder is not used in Zhang and Shasha. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 1;
  index_nodes_recursion(root, lld, kr, nodes, start_postorder, start_preorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.

  // Add root to kr - not added in the recursion.
  kr.push_back(start_postorder-1);

  // Print stuff.
  std::cout << "--- printing stuff ---" << std::endl;
  std::cout << "-- lld : ";
  for (auto e : lld) std::cout << e << ' ';
  std::cout << std::endl;
  std::cout << "-- kr : ";
  for (auto e : kr) std::cout << e << ' ';
  std::cout << std::endl;
}

template <typename Label, typename CostModel>
double Algorithm<Label, CostModel>::zhang_shasha_ted(const node::Node<Label>& t1,
                                                     const node::Node<Label>& t2) {
  std::cout << "=== zhang_shasha_ted ===" << std::endl;

  using data_structures::Matrix;

  const int kT1Size = t1.get_tree_size();
  const int kT2Size = t2.get_tree_size();

  // NOTE: The default constructor of Matrix is called while constructing ZS-Algorithm.
  // NOTE: Shouldn't we implement Matrix::resize() instead of constructing matrix again?
  td_ = Matrix<double>(kT1Size+1, kT2Size+1);
  fd_ = Matrix<double>(kT1Size+1, kT2Size+1);

  // Cleanup node indexes for consecutive use of the algorithm.
  t1_lld_.clear();
  t2_lld_.clear();
  t1_kr_.clear();
  t2_kr_.clear();
  t1_node_.clear();
  t2_node_.clear();
  // TODO: It is better to allocate the vector with tree sizes and fill them in.
  //       Currently lld-values are pushed-back. That results in linear-number
  //       of push_back invocations.
  //       Simmilar approach could be used for kr-values, but the current index,
  //       of a kr-value to set has, to be maintained outside recursion.
  index_nodes(t1, t1_lld_, t1_kr_, t1_node_);
  index_nodes(t2, t2_lld_, t2_kr_, t2_node_);

  // Nested loop over key-root node pairs.
  for (auto kr1 : t1_kr_) {
    for (auto kr2 : t2_kr_) {
      forest_distance(kr1, kr2);
    }
  }

  return td_.at(kT1Size, kT2Size);
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::forest_distance(
    int kr1,
    int kr2) {
  const int kKr1Lld = t1_lld_[kr1 - 1]; // See declaration of t1_lld_.
  const int kKr2Lld = t2_lld_[kr2 - 1];
  const int kT1Empty = kKr1Lld - 1;
  const int kT2Empty = kKr2Lld - 1;
  // Distance between two empty forests.
  fd_.at(kT1Empty, kT2Empty) = 0.0;

  // Distances between a source forest and an empty forest.
  for (int i = kKr1Lld; i <= kr1; ++i) {
    fd_.at(i, kT2Empty) = fd_.at(i - 1, kT2Empty) + c_.del(t1_node_[i - 1]);
    // For t1_node_[i - 1] see declaration of t1_node_.
  }

  // Distances between a destination forest and an empty forest.
  for (int j = kKr2Lld; j <= kr2; ++j) {
    fd_.at(kT1Empty, j) = fd_.at(kT1Empty, j - 1) + c_.ins(t2_node_[j - 1]);
  }

  // Distances between non-empty forests.
  for (int i = kKr1Lld; i <= kr1; ++i) {
    for (int j = kKr2Lld; j <= kr2; ++j) {
      // If we have two subtrees.
      if (t1_lld_[i - 1] == kKr1Lld && t2_lld_[j - 1] == kKr2Lld) {
        fd_.at(i, j) = std::min(
            {fd_.at(i - 1, j) + c_.del(t1_node_[i - 1]), // Delete root node in source subtree.
             fd_.at(i, j - 1) + c_.ins(t2_node_[j - 1]), // Insert root node in destination subtree.
             fd_.at(i - 1, j - 1) + c_.ren(t1_node_[i - 1], t2_node_[j - 1])}); // Rename the root nodes.
        td_.at(i, j) = fd_.at(i, j);
      } else { // We have two forests.
        fd_.at(i, j) = std::min(
            {fd_.at(i - 1, j) + c_.del(t1_node_[i - 1]), // Delete rightmost root node in source subforest.
             fd_.at(i, j - 1) + c_.ins(t2_node_[j - 1]), // Insert rightmost root node in destination subforest.
             fd_.at(t1_lld_[i - 1] - 1, t2_lld_[j - 1] - 1) + td_.at(i, j)}); // Delete the rightmost subtrees + keep the rightmost subtrees.
      }
    }
  }
  std::cout << "--- td[" << kr1 << "][" << kr2 << "] = " << td_.at(kr1, kr2) << std::endl;
}

template <typename Label, typename CostModel>
const typename Algorithm<Label, CostModel>::TestItems Algorithm<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_kr_,
    t1_lld_,
  };
  return test_items;
}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_IMPL_H
