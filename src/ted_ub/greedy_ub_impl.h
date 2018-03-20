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
  // std::list<int> candidate_ids;
  
  for (int i = 0; i < t1_input_size_; ++i) { // Loop in postorder.
    std::list<int>& candidate_ids = t2_label_il_[t1_node_[i].get().label().to_string()];
    // Map node i to the a node in T2 that is first in the inverted list.
    // If there is anything to map.
    if (candidate_ids.size() > 0) {
      mapping.push_back({t1_post_to_pre_[i], t2_post_to_pre_[candidate_ids.front()]});
      std::cout << "M " << t1_post_to_pre_[i] << "," << t2_post_to_pre_[candidate_ids.front()] << std::endl;
      // TODO: Use the node that satisfied the threshold and then delete it.
      candidate_ids.pop_front();
    }
  }
  
  mapping = revise_greedy_mapping(mapping);
  
  return mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::revise_greedy_mapping(std::vector<std::pair<int, int>>& mapping) {
  std::vector<std::pair<int, int>> revised_mapping;
  
  // TODO: Reorder the mappings such that as many as possible can be used.
  //       Write rules for reordering. It seems that it will be mostly
  //       dropping one pair in favour of more pairs.
  
  // TODO: Remember that the optimal mapping can rename nodes. If we have a gap
  //       on both sides, maybe we can map some node pairs.
  
  std::vector<int> t2_mapped(t2_input_size_, -1);
  for (unsigned int e = 0; e < mapping.size(); ++e) {
    t2_mapped[mapping[e].second] = e;
    std::cout << "MID " << mapping[e].second << ":" << e << std::endl;
  }
  
  int mapping_index = -1;
  int j_in_pre = 0;
  std::pair<int, int> previous_pair = {-1,-1};
  bool is_previous_pair = false;
  for (int j = 0; j < t2_input_size_; ++j) { // Loop in postorder.
    j_in_pre = t2_post_to_pre_[j];
    auto j_mapped = t2_mapped[j_in_pre];
    if(j_mapped > mapping_index) { // NOTE: This order test is not enough.
                                   // TODO: What does this order test ensure?
      mapping_index = j_mapped;
      
      std::cout << "Mindex:" << mapping_index << std::endl;
      std::cout << "PP:" << previous_pair.first << "," << previous_pair.second << std::endl;
      std::cout << "CP:" << mapping[mapping_index].first << "," << mapping[mapping_index].second << std::endl;
      
      // NOTE: Test for the current pair be up or right from previous pair.
      bool are_both_up = false;
      bool are_both_right = false;
      if (is_previous_pair) {
        if (mapping[mapping_index].first < previous_pair.first && mapping[mapping_index].second < previous_pair.second) {
          are_both_up = true;
        } else if (mapping[mapping_index].first > previous_pair.first && mapping[mapping_index].second > previous_pair.second) {
          are_both_right = true;
        }
      }
      
      if (!is_previous_pair || are_both_up || are_both_right) {
        revised_mapping.push_back(mapping[mapping_index]);
        is_previous_pair = true;
        previous_pair = mapping[mapping_index];
        std::cout << "RM " << mapping[mapping_index].first << "," << mapping[mapping_index].second << std::endl;
      }
    }
  }
  
  return revised_mapping;
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    std::vector<int>& post_to_pre,
    int& start_postorder,
    int& start_preorder) {
  // Here, start_preorder holds this node's preorder id.
  int current_preorder = start_preorder;

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;
  
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    index_nodes_recursion(*children_start_it, label_il, nodes, post_to_pre,
                          start_postorder, start_preorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Continue to consecutive children.
    ++children_start_it;
  }
  
  // Here, start_postorder holds this node's postorder id here.

  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));
  
  // Add current node postorder id to label inverted list.
  label_il[node.label().to_string()].push_back(start_postorder);
  
  // Store postorder to preorder translation.
  post_to_pre.push_back(current_preorder);
  
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  ++start_postorder;
}

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    std::vector<int>& post_to_pre) {
  // Orders start with '0'.
  int start_postorder = 0;
  // NOTE: Preorder is not used. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 0;
  index_nodes_recursion(root, label_il, nodes, post_to_pre, start_postorder, start_preorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.
}

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2) {
  t1_node_.clear();
  t2_node_.clear();
  t1_post_to_pre_.clear();
  t2_post_to_pre_.clear();
  t1_label_il_.clear();
  t2_label_il_.clear();
  
  // TODO: Do not call get_tree_size() that causes an additional tree traversal.
  //       Index subtree sizes instead - they'll be used anyways.
  t1_input_size_ = t1.get_tree_size();
  t2_input_size_ = t2.get_tree_size();
  
  index_nodes(t1, t1_label_il_, t1_node_, t1_post_to_pre_);
  index_nodes(t2, t2_label_il_, t2_node_, t2_post_to_pre_);
}

template <typename Label, typename CostModel>
const typename GreedyUB<Label, CostModel>::TestItems GreedyUB<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_label_il_,
    t1_post_to_pre_,
  };
  return test_items;
}

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H
