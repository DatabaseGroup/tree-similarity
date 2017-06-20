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
Algorithm<Label, CostModel>::Algorithm(const node::Node<Label>& t1,
                                       const node::Node<Label>& t2,
                                       const CostModel& c) :
                                       t1_(t1), t2_(t2), c_(c) {
  // TODO: The default constructor of Matrix is called while constructing ZS-Algorithm.
  t1_size_ = t1.get_tree_size();
  t2_size_ = t2.get_tree_size();
  td_ = data_structures::Matrix<double>(t1_size_+1, t2_size_+1);
  fd_ = data_structures::Matrix<double>(t1_size_+1, t2_size_+1);
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node, std::vector<int>& lld, std::vector<int>& kr,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    int& start_postorder, int& start_preorder) {
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
void Algorithm<Label, CostModel>::index_nodes(const node::Node<Label>& root,
    std::vector<int>& lld, std::vector<int>& kr,
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
double Algorithm<Label, CostModel>::zhang_shasha_ted() {
  std::cout << "=== zhang_shasha_ted ===" << std::endl;

  // TODO: It is better to allocate the vector with tree sizes and fill them in.
  //       Currently lld-values are pushed-back. That results in linear-number
  //       of push_back invocations.
  //       Simmilar approach could be used for kr-values, but the current index
  //       has to be maintained outside recursion.
  index_nodes(t1_, t1_lld_, t1_kr_, t1_node_);
  index_nodes(t2_, t2_lld_, t2_kr_, t2_node_);

  // Nested loop over key-root node pairs.
  for (auto kr1 : t1_kr_) {
    for (auto kr2 : t2_kr_) {
      forest_distance(kr1, kr2);
    }
  }

  return td_.at(t1_size_, t2_size_);
}

template <typename Label, typename CostModel>
void Algorithm<Label, CostModel>::forest_distance(int kr1, int kr2) {
  int kr1_lld = t1_lld_[kr1];
  int kr2_lld = t2_lld_[kr2];
  // Distance between two empty forests.
  fd_.at(kr1_lld - 1, kr2_lld - 1) = 0.0;
  // Distances between a source forest and an empty forest.
  for (int i = kr1_lld; i <= kr1; ++i) {
    fd_.at(i, kr2_lld - 1) = fd_.at(i - 1, kr2_lld - 1) + c_.del(t1_node_[i]);
  }
  // Distances between a destination forest and an empty forest.

  std::cout << "--- td[" << kr1 << "][" << kr2 << "] = " << td_.at(kr1, kr2) << std::endl;
}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_IMPL_H
