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
double GreedyUB<Label, CostModel>::verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double similarity_threshold) {
  double cost = mapping_cost(greedy_mapping(t1, t2));
  if (cost <= static_cast <int> (std::ceil(similarity_threshold))) {
    return cost;
  }
  return std::numeric_limits<double>::infinity();
};

template <typename Label, typename CostModel>
double GreedyUB<Label, CostModel>::mapping_cost(const std::vector<std::pair<int, int>>& mapping) {
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
    
    // TODO: Ensure mapping conditions here and check the LB.
    // for (auto c : candidate_ids) {
    //   if (mapping to c makes entire mapping valid && LB <= k) {
    //     map to c;
    //     break;
    //   }
    
    if (candidate_ids.size() > 0) {
      // mapping.push_back({t1_post_to_pre_[i], t2_post_to_pre_[candidate_ids.front()]}); // preorder
      mapping.push_back({i, candidate_ids.front()}); // postorder
      // std::cout << "M " << t1_post_to_pre_[i] << "," << t2_post_to_pre_[candidate_ids.front()] << std::endl;
      // TODO: Use the node that satisfied the threshold and then delete it.
      candidate_ids.pop_front();
    }
  }
  
  // std::cout << "---------------------1-1 Mapping" << std::endl;
  // for (auto m : mapping) {
  //   std::cout << m.first << "," << m.second << std::endl;
  // }
  
  // mapping = revise_greedy_mapping(mapping);
  mapping = to_ted_mapping(mapping);
  
  // std::cout << "---------------------Revised Mapping" << std::endl;
  // for (auto m : mapping) {
  //   std::cout << m.first << "," << m.second << std::endl;
  // }
  
  return mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::to_ted_mapping(std::vector<std::pair<int, int>>& mapping) {
  std::vector<std::pair<int, int>> ted_mapping;
  
  std::vector<int> t1_count_mapped_desc(t1_input_size_);
  std::vector<int> t2_count_mapped_desc(t2_input_size_);
  std::vector<int> t1_count_mapped_left(t1_input_size_);
  std::vector<int> t2_count_mapped_left(t2_input_size_);
  
  int t1_i = 0;
  int t2_i = 0;
  int cur_t1 = 0;
  int cur_t2 = 0;
  int prev_t1 = -1;
  int prev_t2 = -1;
  bool mapped_t1_node_processed  = false;
  bool mapped_t2_node_processed  = false;
  for (auto m : mapping) {
    cur_t1 = m.first;
    cur_t2 = m.second;
    std::cout << "M(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
    
    // Increasing postorder id test.
    // The pirs in the input one-to-one mapping are sorted on the first element.
    // The second elements of two consecutive pairs must not have descreasing
    // values.
    if (cur_t2 < prev_t2) {
      std::cout << "M DEL post(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
      continue;
    }
    
    // Go back with t1_i and t2_i if the current node ids are smaller.
    // If cur_t1 < t1_i => cur_t1 has been processed as a non-mapped node and
    // it has to be processed as a mapped node, rolling back the changes made
    // to it
    // If cur_t1 == t1_i => cur_t1 has to be processed as a mapped node,
    // it has not been processed before.
    if (cur_t1 < t1_i) {
      t1_i = cur_t1;
      mapped_t1_node_processed = true;
      std::cout << "t1_i = " << t1_i << " processed before as non-mapped" << std::endl;
    }
    if (cur_t2 < t2_i) {
      t2_i = cur_t2;
      mapped_t2_node_processed = true;
      std::cout << "t2_i = " << t2_i << " processed before as non-mapped" << std::endl;
    }
    if (cur_t1 == t1_i) {
      mapped_t1_node_processed = false;
    }
    if (cur_t2 == t2_i) {
      mapped_t2_node_processed = false;
    }
    
    // TODO: Review the following if statements.
    
    // NOTE: is_leaf : t1_node_[i].get().is_leaf() [with t1_size_[i] == 1 may be faster]
    // NOTE: first_leaf_to_the_right : precompute
    // NOTE: is_leftmost_child_of_its_parent : precompute OR post_to_pre[parent[i]] + 1 == post_to_pre[i]
    
    // Nodes not in mapping in T1.
    while (t1_i < cur_t1) {
      std::cout << "processing t1_i = " << t1_i << std::endl;
      // descendants
      t1_count_mapped_desc[t1_parent_[t1_i]] += t1_count_mapped_desc[t1_i];
      // left
      if (t1_rch_[t1_i] > -1) {
        if (t1_node_[t1_i].get().is_leaf()) { // leaf node
          t1_count_mapped_left[t1_rch_[t1_i]] = t1_count_mapped_left[t1_i];
        } else { // inner node
        }
      }
      if (t1_post_to_pre_[t1_parent_[t1_i]] + 1 == t1_post_to_pre_[t1_i]) { // leftmost child of its parent
                                                                            // NOTE: Could be precomputed.
        t1_count_mapped_left[t1_parent_[t1_i]] = t1_count_mapped_left[t1_i];
      }
      ++t1_i;
    }
    // Nodes not in mapping in T2.
    while (t2_i < cur_t2) {
      std::cout << "processing t2_i = " << t2_i << std::endl;
      // descendants
      t2_count_mapped_desc[t2_parent_[t2_i]] += t2_count_mapped_desc[t2_i];
      // left
      if (t2_rch_[t2_i] > -1) {
        if (t2_node_[t2_i].get().is_leaf()) { // leaf node
          t2_count_mapped_left[t2_rch_[t2_i]] = t2_count_mapped_left[t2_i];
        } else { // inner node
        }
      }
      if (t2_post_to_pre_[t2_parent_[t2_i]] + 1 == t2_post_to_pre_[t2_i]) { // leftmost child of its parent
        t2_count_mapped_left[t2_parent_[t2_i]] = t2_count_mapped_left[t2_i];
      }
      ++t2_i;
    }
    
    // If the current node pair cannot be mapped, the nodes must not be
    // processed as mapped (the next two if statements).
    
    // Mapped descendants test.
    if (t1_count_mapped_desc[cur_t1] != t2_count_mapped_desc[cur_t2]) {
      std::cout << "M DEL desc(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
      continue;
    }
    // Mapped nodes to the left test.
    if (t1_count_mapped_left[cur_t1] != t2_count_mapped_left[cur_t2]) {
      std::cout << "M DEL left(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
      continue;
    }
    
    // Mapped node in T1.
    if (!mapped_t1_node_processed) {
      std::cout << "processing t1_i = " << t1_i << std::endl;
      if (t1_parent_[t1_i] > -1) { // root has no parent
        // descendants
        t1_count_mapped_desc[t1_parent_[t1_i]] += t1_count_mapped_desc[t1_i] + 1;
        // left
        if (t1_rch_[t1_i] > -1) {
          if (t1_node_[t1_i].get().is_leaf()) { // leaf node
            t1_count_mapped_left[t1_rch_[t1_i]] = t1_count_mapped_left[t1_i] + 1;
          } else { // inner node
            t1_count_mapped_left[t1_rch_[t1_i]] += 1;
          }
        }
        if (t1_post_to_pre_[t1_parent_[t1_i]] + 1 == t1_post_to_pre_[t1_i]) { // leftmost child of its parent
                                                                              // NOTE: Could be precomputed.
          t1_count_mapped_left[t1_parent_[t1_i]] = t1_count_mapped_left[t1_i];
        }
      }
      ++t1_i;
    } else { // The current mapped node has been processed and it's enough to
             // fix the values it updated.
      std::cout << "processing t1_i = " << t1_i << std::endl;
      if (t1_parent_[t1_i] > -1) { // root has no parent
        // descendants
        t1_count_mapped_desc[t1_parent_[t1_i]] += 1; // THE ONLY DIFF TO ABOVE.
      }
      ++t1_i;
    }
    // Mapped node in T2.
    if (!mapped_t2_node_processed) {
      // mapped node
      std::cout << "processing t2_i = " << t2_i << std::endl;
      if (t2_parent_[t2_i] > -1) { // root has no parent
        // descendants
        t2_count_mapped_desc[t2_parent_[t2_i]] += t2_count_mapped_desc[t2_i] + 1;
        // left
        if (t2_rch_[t2_i] > -1) {
          if (t2_node_[t2_i].get().is_leaf()) { // leaf node
            t2_count_mapped_left[t2_rch_[t2_i]] = t2_count_mapped_left[t2_i] + 1;
          } else { // inner node
            t2_count_mapped_left[t2_rch_[t2_i]] += 1;
          }
        }
        if (t2_post_to_pre_[t2_parent_[t2_i]] + 1 == t2_post_to_pre_[t2_i]) { // leftmost child of its parent
          t2_count_mapped_left[t2_parent_[t2_i]] = t2_count_mapped_left[t2_i];
        }
      }
      ++t2_i;
    } else { // The current mapped node has been processed and it's enough to
             // fix the values it updated.
      std::cout << "processing t2_i = " << t2_i << std::endl;
      if (t2_parent_[t2_i] > -1) { // root has no parent
        // descendants
        t2_count_mapped_desc[t2_parent_[t2_i]] += 1; // THE ONLY DIFF TO ABOVE.
      }
      ++t2_i;
    }
    
    ted_mapping.push_back({cur_t1, cur_t2});
    
    prev_t1 = cur_t1;
    prev_t2 = cur_t2;
  }
  
  // TODO: Gap after the last mapped node until the last node in the tree. 
  
  std::cout << "t1_count_mapped_desc : ";
  for (auto e : t1_count_mapped_desc) {
    std::cout << e << " ";
  } std::cout << std::endl;
  
  std::cout << "t1_count_mapped_left : ";
  for (auto e : t1_count_mapped_left) {
    std::cout << e << " ";
  } std::cout << std::endl;
  
  std::cout << "t2_count_mapped_desc : ";
  for (auto e : t2_count_mapped_desc) {
    std::cout << e << " ";
  } std::cout << std::endl;
  
  std::cout << "t2_count_mapped_left : ";
  for (auto e : t2_count_mapped_left) {
    std::cout << e << " ";
  } std::cout << std::endl;
  
  return ted_mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::revise_greedy_mapping(std::vector<std::pair<int, int>>& mapping) {
  std::vector<std::pair<int, int>> revised_mapping;
  
  // IDEA: Reorder the mappings such that as many as possible can be used.
  //       Write rules for reordering. It seems that it will be mostly
  //       dropping one pair in favour of more pairs.
  
  // IDEA: Remember that the optimal mapping can rename nodes. If we have a gap
  //       on both sides, maybe we can map some node pairs.
  
  // For each node in preorder of destination tree, store the position of its
  // pair in the mapping sequence.
  std::vector<int> t2_mapped(t2_input_size_, -1);
  for (unsigned int e = 0; e < mapping.size(); ++e) {
    t2_mapped[mapping[e].second] = e;
    // std::cout << "MID " << mapping[e].second << ":" << e << std::endl;
  }
  
  int mapping_index = -1;
  int j_in_pre = 0;
  std::pair<int, int> previous_pair = {-1,-1};
  std::pair<int, int> current_pair = {-1,-1};
  bool is_previous_pair = false;
  for (int j = 0; j < t2_input_size_; ++j) { // Loop in postorder.
    j_in_pre = t2_post_to_pre_[j];
    auto j_mapped = t2_mapped[j_in_pre];
    // std::cout << "j_in_pre : " << j_in_pre << std::endl;
    // std::cout << "j_mapped : " << j_mapped << std::endl;
    if(j_mapped > mapping_index) { // QUESTION: What is this for?
      // mapping_index = j_mapped;
      
      current_pair = {mapping[j_mapped].first, mapping[j_mapped].second};
      
      // std::cout << "Mindex:" << mapping_index << std::endl;
      // std::cout << "PP:" << previous_pair.first << "," << previous_pair.second << std::endl;
      // std::cout << "CP:" << current_pair.first << "," << current_pair.second << std::endl;
      
      // Test for the current pair be up or right from previous pair.
      bool are_both_up = false;
      bool are_both_right = false;
      if (is_previous_pair) {
        if (current_pair.first < previous_pair.first && current_pair.second < previous_pair.second) {
          are_both_up = true;
        } else if (current_pair.first > previous_pair.first && current_pair.second > previous_pair.second) {
          are_both_right = true;
        }
      }
      
      // std::cout << "   are_both_up : " << are_both_up << std::endl;
      // std::cout << "are_both_right : " << are_both_right << std::endl;
      
      // Keep the mapping only if eligable.
      if (!is_previous_pair || are_both_up || are_both_right) {
        revised_mapping.push_back(current_pair);
        is_previous_pair = true; // Set to mark that there is already a mapepd pair.
        previous_pair = current_pair; // Record the previous pair.
        // std::cout << "RM " << mapping[mapping_index].first << "," << mapping[mapping_index].second << std::endl;
        mapping_index = j_mapped; // Update the mapping_index.
      }
    }
  }
  
  return revised_mapping;
};

template <typename Label, typename CostModel>
int GreedyUB<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    std::vector<int>& post_to_pre,
    std::vector<int>& parent,
    std::vector<int>& depth,
    std::vector<int>& size,
    int& start_postorder,
    int& start_preorder,
    unsigned int start_depth) {
  // Here, start_preorder holds this node's preorder id.
  int current_preorder = start_preorder;
  
  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;
  
  // To store postorder ids of this node's children.
  std::vector<int> nodes_children;
  
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    desc_sum += index_nodes_recursion(*children_start_it, label_il, nodes, post_to_pre,
                          parent, depth, size, start_postorder, start_preorder,
                          start_depth + 1);
    // Here, start_postorder-1 is the postorder of the current child.
    // Collect children ids.
    nodes_children.push_back(start_postorder-1);
    // Continue to consecutive children.
    ++children_start_it;
  }
  
  // Here, start_postorder holds this node's postorder id here.

  // Parent - dummy element to grow the vector by this node.
  parent.push_back(-1);
  // Set this node's postorder value as parent for all its children.
  for (auto child_id : nodes_children) {
    parent.at(child_id) = start_postorder;
  }
  
  if (node.is_leaf()) {
    // Leaf has size 1.
    size.push_back(1);
  } else {
    // Inner node has size desc_sum+1.
    size.push_back(desc_sum + 1);
  }
  
  // Depth.
  depth.push_back(start_depth);

  // Add current node to the nodes vector.
  nodes.push_back(std::ref(node));
  
  // Add current node postorder id to label inverted list.
  label_il[node.label().to_string()].push_back(start_postorder);
  
  // Store postorder to preorder translation.
  post_to_pre.push_back(current_preorder);
  
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  ++start_postorder;
  
  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::unordered_map<std::string, std::list<int>>& label_il,
    std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    std::vector<int>& post_to_pre,
    std::vector<int>& parent,
    std::vector<int>& depth,
    std::vector<int>& size) {
  // Orders start with '0'.
  int start_postorder = 0;
  // NOTE: Preorder is not used. Remove start_preorder. Or
  //       move the template traversal with postorder and preorder to some notes
  //       of how to traverse trees.
  int start_preorder = 0;
  index_nodes_recursion(root, label_il, nodes, post_to_pre, parent, depth, size, start_postorder, start_preorder, 0);
  // Here, start_postorder and start_preorder store the size of tree minus 1.
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::post_traversal_indexing(const int input_size,
    const std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
    std::vector<int>& rch) {
  int current_leaf = -1;
  int post_id = -1;
  for(int i = 0; i < input_size; i++) { // loop in right-to-left preorder
    post_id = input_size - i - 1; // Translation from right-to-left preorder
                                  // to postorder.
    rch[post_id] = current_leaf;
    if(nodes[post_id].get().is_leaf()) { // NOTE: Could be precomputed.
      current_leaf = post_id;
    }
  }
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2) {
  t1_node_.clear();
  t2_node_.clear();
  t1_post_to_pre_.clear();
  t2_post_to_pre_.clear();
  t1_label_il_.clear();
  t2_label_il_.clear();
  t1_parent_.clear();
  t2_parent_.clear();
  t1_rch_.clear();
  t2_rch_.clear();
  t1_depth_.clear();
  t2_depth_.clear();
  t1_size_.clear();
  t2_size_.clear();
  
  // TODO: Do not call get_tree_size() that causes an additional tree traversal.
  //       Index subtree sizes instead - they'll be used anyways.
  t1_input_size_ = t1.get_tree_size();
  t2_input_size_ = t2.get_tree_size();
  
  index_nodes(t1, t1_label_il_, t1_node_, t1_post_to_pre_, t1_parent_, t1_depth_, t1_size_);
  index_nodes(t2, t2_label_il_, t2_node_, t2_post_to_pre_, t2_parent_, t2_depth_, t2_size_);
  
  t1_rch_.resize(t1_input_size_);
  t2_rch_.resize(t2_input_size_);
  post_traversal_indexing(t1_input_size_, t1_node_, t1_rch_);
  post_traversal_indexing(t2_input_size_, t2_node_, t2_rch_);
};

template <typename Label, typename CostModel>
const typename GreedyUB<Label, CostModel>::TestItems GreedyUB<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_label_il_,
    t1_post_to_pre_,
    t1_parent_,
    t1_rch_,
    t1_depth_,
    t1_size_,
  };
  return test_items;
};

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H
