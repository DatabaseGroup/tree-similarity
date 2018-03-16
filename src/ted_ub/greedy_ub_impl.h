// The MIT License (MIT)
// Copyright (c) 2018 Mateusz Pawlik.
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

/// \file ted_ub/greedy_ub_impl.h
///
/// \details
/// Contains the implementation of the greedy mapping algorithm.

#ifndef TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H
#define TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H

template <typename Label, typename CostModel>
GreedyUB<Label, CostModel>::GreedyUB() : c_() {}

template <typename Label, typename CostModel>
double GreedyUB<Label, CostModel>::greedy_mapping_cost(const node::Node<Label>& t1, const node::Node<Label>& t2) {
  std::vector<std::pair<int, int>> mapping = greedy_mapping(t1, t2);
  return (t1_input_size_ + t2_input_size_ - 2 * mapping.size());
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::greedy_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2) {
  init(t1, t2);
  
  std::vector<std::pair<int, int>> mapping;
  std::list<int> candidate_ids;
  
  for (int i = 0; i < t1_input_size_; ++i) {
    candidate_ids = t2_label_il_[t1_node_[i].get().label().to_string()];
    mapping.push_back({i, candidate_ids.front()});
  }
  
  mapping = revise_greedy_mapping(mapping);
  
  return mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::revise_greedy_mapping(std::vector<std::pair<int, int>>& mapping) {
  std::vector<std::pair<int, int>> revised_mapping;
  
  int max_id = -1; // To make '0' pass the first if in the loop.
  for (auto e : mapping) {
    if (e.second > max_id) {
      revised_mapping.push_back(e);
    }
    max_id = std::max(max_id, e.second);
  }
  
  return revised_mapping;
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    int& start_postorder,
    int& start_preorder) {
  // Here, start_preorder holds this node's preorder id.

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;
  
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    index_nodes_recursion(*children_start_it, label_il, nodes, start_postorder,
                          start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Continue to consecutive children.
    ++children_start_it;
  }
  
  // Here, start_postorder holds this node's postorder id here.

  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));
  
  // Add current node postorder id to label inverted list.
  label_il[node.label().to_string()].push_back(start_postorder);
  
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  ++start_postorder;
}

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes) {
  // Orders start with '0'.
  int start_postorder = 0;
  // NOTE: Preorder is not used. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 0;
  index_nodes_recursion(root, label_il, nodes, start_postorder, start_preorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.
}

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2) {
  t1_node_.clear();
  t2_node_.clear();
  t1_label_il_.clear();
  t2_label_il_.clear();
  
  // TODO: Do not call get_tree_size() that causes an additional tree traversal.
  //       Index subtree sizes instead - they'll be used anyways.
  t1_input_size_ = t1.get_tree_size();
  t2_input_size_ = t2.get_tree_size();
  
  index_nodes(t1, t1_label_il_, t1_node_);
  index_nodes(t2, t2_label_il_, t2_node_);
}

template <typename Label, typename CostModel>
const typename GreedyUB<Label, CostModel>::TestItems GreedyUB<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_label_il_,
  };
  return test_items;
}

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H
