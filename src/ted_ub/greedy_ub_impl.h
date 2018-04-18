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
  // return (t1_input_size_ + t2_input_size_ - 2 * mapping.size());
  
  double result = 0.0;
  for (auto m : mapping) {
    result += c_.ren(t1_node_[m.first], t2_node_[m.second]); // renames
  }
  result += t1_input_size_ - mapping.size(); // deletions
  result += t2_input_size_ - mapping.size(); // insertions
  
  return result;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::greedy_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2) {
  init(t1, t2);
  
  std::vector<std::pair<int, int>> mapping;
  
  for (int i = 0; i < t1_input_size_; ++i) { // Loop in postorder.
    // TODO: Ensure mapping conditions here and check the LB.
    // for (auto c : candidate_ids) {
    //   if (mapping to c makes entire mapping valid && LB <= k) {
    //     map to c;
    //     break;
    //   }
    std::list<int>& candidate_ids = t2_label_il_[t1_node_[i].get().label().to_string()];
    // Map node i to the a node in T2 that is first in the inverted list.
    // If there is anything to map.
    if (candidate_ids.size() > 0) {
      mapping.push_back({i, candidate_ids.front()}); // postorder
      // TODO: Use the node that satisfied the threshold and then delete it.
      candidate_ids.pop_front();
    }
  }
  mapping = to_ted_mapping(mapping);
  return mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::lb_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2, const int k) {
  init(t1, t2);
  std::vector<std::pair<int, int>> mapping;
  for (int i = 0; i < t1_input_size_; ++i) { // Loop in postorder.
    std::list<int>& candidate_ids = t2_label_il_[t1_node_[i].get().label().to_string()];
    
    // if (candidate_ids.size() > 0) {
    //   mapping.push_back({i, candidate_ids.front()}); // postorder
    //   // TODO: Use the node that satisfied the threshold and then delete it.
    //   candidate_ids.pop_front();
    // }
    
    std::list<int>::iterator cand_it = candidate_ids.begin();
    int cand_id = 0;
    for ( ; cand_it != candidate_ids.end(); ) {
      cand_id = *cand_it;
      if (k_relevant(i, cand_id, k)) {
        mapping.push_back({i, cand_id}); // postorder
        cand_it = candidate_ids.erase(cand_it); // Assignment not needed due to break.
        break;
      }
      ++cand_it;
    }
    
  }
  mapping = to_ted_mapping(mapping);
  mapping = fill_gaps_in_mapping(mapping, k);
  return mapping;
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::update_desc_when_not_mapped(const int node,
    std::vector<int>& count_mapped_desc, const std::vector<int>& parent,
    const int input_size) {
  if (node < input_size - 1) { // Root has no parent and the nodes of dummy
                               // mapping in fill_gaps_in_mapping do not exist.
    count_mapped_desc[parent[node]] += count_mapped_desc[node];
  }
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::update_desc_when_mapped(const int node,
    std::vector<int>& count_mapped_desc, const std::vector<int>& parent,
    const int input_size) {
  if (node < input_size - 1) { // Root has no parent and the nodes of dummy
                               // mapping in fill_gaps_in_mapping do not exist.
    count_mapped_desc[parent[node]] += count_mapped_desc[node] + 1;
  }
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::update_left_when_not_mapped(const int node,
    std::vector<int>& count_mapped_left, const std::vector<int>& parent,
    const std::vector<int>& rl, const std::vector<int>& size,
    const std::vector<int>& post_to_pre, const int input_size) {
  if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                               // the nodes of dummy mapping in fill_gaps_in_mapping
                               // do not exist.
    if (rl[node] > -1) { // if the node has a right leaf
      if (size[node] == 1) { // leaf node
        count_mapped_left[rl[node]] = count_mapped_left[node];
      }
    }
    if (post_to_pre[parent[node]] + 1 == post_to_pre[node]) { // leftmost child of its parent
                                                              // NOTE: Could be precomputed.
      count_mapped_left[parent[node]] = count_mapped_left[node];
    }
  }
};

template <typename Label, typename CostModel>
void GreedyUB<Label, CostModel>::update_left_when_mapped(const int node,
    std::vector<int>& count_mapped_left, const std::vector<int>& parent,
    const std::vector<int>& rl, const std::vector<int>& size,
    const std::vector<int>& post_to_pre, const int input_size) {
  if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                               // the nodes of dummy mapping in fill_gaps_in_mapping
                               // do not exist.
    if (rl[node] > -1) { // if the node has a right leaf
      if (size[node] == 1) { // leaf node
        count_mapped_left[rl[node]] = count_mapped_left[node] + 1;
      } else { // inner node
        count_mapped_left[rl[node]] += 1;
      }
    }
    if (post_to_pre[parent[node]] + 1 == post_to_pre[node]) { // leftmost child of its parent
                                                              // NOTE: Could be precomputed.
      count_mapped_left[parent[node]] = count_mapped_left[node];
    }
  }
};

template <typename Label, typename CostModel>
bool GreedyUB<Label, CostModel>::if_in_corresponding_regions(int t1_begin_gap,
    int i, int t1_end_gap, int t2_begin_gap, int j, int t2_end_gap) {
  
  // TODO: Begin or end may not exist.
  
  if (t1_begin_gap == -1 && t1_end_gap == t1_input_size_) { // also t2_begin_gap == -1 && t2_end_gap == t2_input_size_
    // Any mapping is fine - no defined regions.
    return true;
  }
  
  int i_pre = t1_post_to_pre_[i];
  int j_pre = t2_post_to_pre_[j];
  
  if (t1_begin_gap == -1) { // also t2_begin_gap == 1
    if (i_pre > t1_post_to_pre_[t1_end_gap] && j_pre > t2_post_to_pre_[t2_end_gap]) {
      return true;
    }
    if (i_pre < t1_post_to_pre_[t1_end_gap] && j_pre < t2_post_to_pre_[t2_end_gap]) {
      return true;
    }
  }
  
  if (t1_end_gap == t1_input_size_) { // also t2_end_gap == t2_input_size_
    if (i_pre > t1_post_to_pre_[t1_begin_gap] && j_pre > t2_post_to_pre_[t2_begin_gap]) {
      return true;
    }
    if (i_pre < t1_post_to_pre_[t1_begin_gap] && j_pre < t2_post_to_pre_[t2_begin_gap]) {
      return true;
    }
  }
  
  int t1_begin_gap_pre = t1_post_to_pre_[t1_begin_gap];
  int t2_begin_gap_pre = t2_post_to_pre_[t2_begin_gap];
  int t1_end_gap_pre = t1_post_to_pre_[t1_end_gap];
  int t2_end_gap_pre = t2_post_to_pre_[t2_end_gap];
  
  // Three different cases!
  if (t1_end_gap_pre < i_pre && i_pre < t1_begin_gap_pre) {
    if (t2_end_gap_pre < j_pre && j_pre < t2_begin_gap_pre) {
      return true;
    }
  }
  if (t1_end_gap_pre < i_pre && t1_begin_gap_pre < i_pre) {
    if (t2_end_gap_pre < j_pre && t2_begin_gap_pre < j_pre) {
      return true;
    }
  }
  if (t1_begin_gap_pre < i_pre && i_pre < t1_end_gap_pre) {
    if (t2_begin_gap_pre < j_pre && j_pre < t2_end_gap_pre) {
      return true;
    }
  }
  if (i_pre < t1_begin_gap_pre && i_pre < t1_end_gap_pre) {
    if (j_pre < t2_begin_gap_pre && j_pre < t2_end_gap_pre) {
      return true;
    }
  }
  
  return false;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::fill_gaps_in_mapping(std::vector<std::pair<int, int>>& mapping,
    const int k) {
  std::vector<std::pair<int, int>> result_mapping;
  
  // The counts for mapped descendants and nodes to the left must be maintained.
  std::vector<int> t1_count_mapped_desc(t1_input_size_);
  std::vector<int> t2_count_mapped_desc(t2_input_size_);
  std::vector<int> t1_count_mapped_left(t1_input_size_);
  std::vector<int> t2_count_mapped_left(t2_input_size_);
  
  std::pair<int, int> begin_gap = {-1, -1};
  
  // Add a dummy mapping pair to cover the gap from the last mapping until the
  // ends of the trees.
  mapping.push_back({t1_input_size_, t2_input_size_});
  
  for (auto end_gap : mapping) {
    // If there is no gap, continue with the next mapped pair.
    if (end_gap.first - begin_gap.first == 1 || end_gap.second - begin_gap.second == 1) {
      // TODO: Update the counts for nodes between begin_gap and end_gap,
      //       if any, as not-mapped nodes.
      if (end_gap.first - begin_gap.first > 1) { // There is a gap only between first elements.
        for (int not_mapped_first = begin_gap.first + 1; not_mapped_first < end_gap.first; ++not_mapped_first) {
          update_desc_when_not_mapped(not_mapped_first, t1_count_mapped_desc, t1_parent_,
              t1_input_size_);
          // std::cout << "update_desc_when_not_mapped T1 : " << not_mapped_first << std::endl;
          update_left_when_not_mapped(not_mapped_first, t1_count_mapped_left, t1_parent_,
              t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
        }
      }
      // TODO: Update the counts for nodes in end_gap as mapped nodes.
      update_desc_when_mapped(end_gap.first, t1_count_mapped_desc, t1_parent_,
          t1_input_size_);
      update_left_when_mapped(end_gap.first, t1_count_mapped_left, t1_parent_,
          t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
      
      // TODO: Update the counts for nodes between begin_gap and end_gap,
      //       if any, as not-mapped nodes.
      if (end_gap.second - begin_gap.second > 1) { // There is a gap only between second elements.
        for (int not_mapped_second = begin_gap.second + 1; not_mapped_second < end_gap.second; ++not_mapped_second) {
          update_desc_when_not_mapped(not_mapped_second, t2_count_mapped_desc, t2_parent_,
              t2_input_size_);
          update_left_when_not_mapped(not_mapped_second, t2_count_mapped_left, t2_parent_,
              t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
        }
      }
      // TODO: Update the counts for nodes in end_gap as mapped nodes.
      update_desc_when_mapped(end_gap.second, t2_count_mapped_desc, t2_parent_,
          t2_input_size_);
      update_left_when_mapped(end_gap.second, t2_count_mapped_left, t2_parent_,
          t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
      
      // Push the end of the current gap to the result.
      result_mapping.push_back(end_gap);    
      // Update the begin of the next gap.
      begin_gap = end_gap;
      continue;
    }
    
    
    
    // TODO: There is a gap --> try to map.
    
    // TODO: While filling the gaps the counts have to be updated.
    
    // // TODO: This is temporary counts update for non-mapped nodes in the gaps.
    // for (int i = begin_gap.first + 1; i < end_gap.first; ++i) {
    //   update_desc_when_not_mapped(i, t1_count_mapped_desc, t1_parent_,
    //       t1_input_size_);
    //   update_left_when_not_mapped(i, t1_count_mapped_left, t1_parent_,
    //       t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
    // }
    // for (int j = begin_gap.second + 1; j < end_gap.second; ++j) {
    //   update_desc_when_not_mapped(j, t2_count_mapped_desc, t2_parent_,
    //       t2_input_size_);
    //   update_left_when_not_mapped(j, t2_count_mapped_left, t2_parent_,
    //       t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
    // }
    
    // // Print the pairs form the gaps that are k_relevant and can be mapped,
    // // not breaking the ted conditions.
    // for (int i = begin_gap.first + 1; i < end_gap.first; ++i) {
    //   for (int j = begin_gap.second + 1; j < end_gap.second; ++j) {
    //     std::cout << "-- GC   : " << i << "," << j << std::endl;
    //     if (k_relevant(i, j, k)) {
    //       std::cout << "-- GC R : " << i << "," << j << std::endl;
    //       if (t1_count_mapped_desc[i] == t2_count_mapped_desc[j] &&
    //           t1_count_mapped_left[i] == t2_count_mapped_left[j]) {
    //         std::cout << "-- GC C : " << i << "," << j << std::endl;
    //       }
    //     }
    //   }
    // }
    
    int i = begin_gap.first + 1;
    int i_last = end_gap.first - 1;
    int j = begin_gap.second + 1;
    int j_last = end_gap.second - 1;
    std::pair<int, int> last_mapped = begin_gap;
    while (i <= i_last && j <= j_last) {
      // std::cout << "== GC   : " << i << "," << j << std::endl;
      if (k_relevant(i, j, k) &&
          (t1_count_mapped_desc[i] == t2_count_mapped_desc[j] &&
          t1_count_mapped_left[i] == t2_count_mapped_left[j]) &&
          if_in_corresponding_regions(last_mapped.first, i, end_gap.first, last_mapped.second, j, end_gap.second)) {
        // std::cout << "== GC C : " << i << "," << j << std::endl;
        // TODO: Map (i,j) + push to result mapping + update counts for both
        //       nodes incremented as mapped.
        update_desc_when_mapped(i, t1_count_mapped_desc, t1_parent_,
            t1_input_size_);
        update_left_when_mapped(i, t1_count_mapped_left, t1_parent_,
            t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
        
        // std::cout << "update mapped T2 : " << j << std::endl;
        update_desc_when_mapped(j, t2_count_mapped_desc, t2_parent_,
            t2_input_size_);
        update_left_when_mapped(j, t2_count_mapped_left, t2_parent_,
            t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
        result_mapping.push_back({i, j});
        last_mapped = {i, j};
        ++i;
        ++j;
      } else {
        // TODO: Update counts for the incremented node as non-mapped.
        if (i_last - i > j_last - j) {
          update_desc_when_not_mapped(i, t1_count_mapped_desc, t1_parent_,
              t1_input_size_);
          update_left_when_not_mapped(i, t1_count_mapped_left, t1_parent_,
              t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
          ++i;
        } else {
          // std::cout << "update not mapped T2 : " << j << std::endl;
          update_desc_when_not_mapped(j, t2_count_mapped_desc, t2_parent_,
              t2_input_size_);
          update_left_when_not_mapped(j, t2_count_mapped_left, t2_parent_,
              t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
          ++j;
        }
      }
    }
    // Check the case when only one of i or j incremented beyond the limit.
    // The gap in one of the trees has been used up.
    if (i_last - i < 0 && j_last - j >= 0) {
      for (int j_missing = j; j_missing <= j_last; ++j_missing) {
        // std::cout << "update not mapped T2 : " << j << std::endl;
        update_desc_when_not_mapped(j, t2_count_mapped_desc, t2_parent_,
            t2_input_size_);
        update_left_when_not_mapped(j, t2_count_mapped_left, t2_parent_,
            t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
      }
    }
    if (j_last - j < 0 && i_last - i >= 0) {
      for (int i_missing = i; i_missing <= i_last; ++i_missing) {
        update_desc_when_not_mapped(i, t1_count_mapped_desc, t1_parent_,
            t1_input_size_);
        update_left_when_not_mapped(i, t1_count_mapped_left, t1_parent_,
            t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
      }
    }
      
    // TODO: Update the counts for nodes in end_gap as mapped nodes.
    update_desc_when_mapped(end_gap.first, t1_count_mapped_desc, t1_parent_,
        t1_input_size_);
    update_left_when_mapped(end_gap.first, t1_count_mapped_left, t1_parent_,
        t1_rl_, t1_size_, t1_post_to_pre_, t1_input_size_);
    // std::cout << "update mapped T2 : " << end_gap.second << std::endl;
    update_desc_when_mapped(end_gap.second, t2_count_mapped_desc, t2_parent_,
        t2_input_size_);
    update_left_when_mapped(end_gap.second, t2_count_mapped_left, t2_parent_,
        t2_rl_, t2_size_, t2_post_to_pre_, t2_input_size_);
    // Push the end of the current gap to the result.
    // std::cout << "Push old pair : " << end_gap.first << "," << end_gap.second << std::endl;
    result_mapping.push_back(end_gap);    
    // Update the begin of the next gap.
    begin_gap = end_gap;
  }
  
  // std::cout << "t1_count_mapped_desc : ";
  // for (auto e : t1_count_mapped_desc) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t1_count_mapped_left : ";
  // for (auto e : t1_count_mapped_left) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t2_count_mapped_desc : ";
  // for (auto e : t2_count_mapped_desc) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t2_count_mapped_left : ";
  // for (auto e : t2_count_mapped_left) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  
  // Remove the last dummy mapping.
  result_mapping.pop_back();
  return result_mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::to_ted_mapping(const std::vector<std::pair<int, int>>& mapping) {
  std::vector<std::pair<int, int>> ted_mapping;
  
  std::vector<int> t1_count_mapped_desc(t1_input_size_);
  std::vector<int> t2_count_mapped_desc(t2_input_size_);
  std::vector<int> t1_count_mapped_left(t1_input_size_);
  std::vector<int> t2_count_mapped_left(t2_input_size_);
  
  int t1_i = 0;
  int t2_i = 0;
  int cur_t1 = 0;
  int cur_t2 = 0;
  // int prev_t1 = -1;
  int prev_t2 = -1;
  bool mapped_t1_node_processed  = false;
  bool mapped_t2_node_processed  = false;
  for (auto m : mapping) {
    cur_t1 = m.first;
    cur_t2 = m.second;
    // std::cout << "M(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
    
    // Increasing postorder id test.
    // The pairs in the input one-to-one mapping are sorted on the first element.
    // The second elements of two consecutive pairs must not have descreasing
    // values.
    if (cur_t2 < prev_t2) {
      // std::cout << "M DEL post(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
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
      // std::cout << "t1_i = " << t1_i << " processed before as non-mapped" << std::endl;
    }
    if (cur_t2 < t2_i) {
      t2_i = cur_t2;
      mapped_t2_node_processed = true;
      // std::cout << "t2_i = " << t2_i << " processed before as non-mapped" << std::endl;
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
      // std::cout << "processing t1_i = " << t1_i << std::endl;
      // descendants
      t1_count_mapped_desc[t1_parent_[t1_i]] += t1_count_mapped_desc[t1_i];
      // left
      if (t1_rl_[t1_i] > -1) {
        if (t1_size_[t1_i] == 1) { // leaf node
          t1_count_mapped_left[t1_rl_[t1_i]] = t1_count_mapped_left[t1_i];
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
      // std::cout << "processing t2_i = " << t2_i << std::endl;
      // descendants
      t2_count_mapped_desc[t2_parent_[t2_i]] += t2_count_mapped_desc[t2_i];
      // left
      if (t2_rl_[t2_i] > -1) {
        if (t2_size_[t2_i] == 1) { // leaf node
          t2_count_mapped_left[t2_rl_[t2_i]] = t2_count_mapped_left[t2_i];
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
      // std::cout << "M DEL desc(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
      continue;
    }
    // Mapped nodes to the left test.
    if (t1_count_mapped_left[cur_t1] != t2_count_mapped_left[cur_t2]) {
      // std::cout << "M DEL left(" << cur_t1 << "," << cur_t2 << ")" << std::endl;
      continue;
    }
    
    // Mapped node in T1.
    if (!mapped_t1_node_processed) {
      // std::cout << "processing t1_i = " << t1_i << std::endl;
      if (t1_parent_[t1_i] > -1) { // root has no parent
        // descendants
        t1_count_mapped_desc[t1_parent_[t1_i]] += t1_count_mapped_desc[t1_i] + 1;
        // left
        if (t1_rl_[t1_i] > -1) {
          if (t1_size_[t1_i] == 1) { // leaf node
            t1_count_mapped_left[t1_rl_[t1_i]] = t1_count_mapped_left[t1_i] + 1;
          } else { // inner node
            t1_count_mapped_left[t1_rl_[t1_i]] += 1;
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
      // std::cout << "processing t1_i = " << t1_i << std::endl;
      if (t1_parent_[t1_i] > -1) { // root has no parent
        // descendants
        t1_count_mapped_desc[t1_parent_[t1_i]] += 1; // THE ONLY DIFF TO ABOVE.
      }
      ++t1_i;
    }
    // Mapped node in T2.
    if (!mapped_t2_node_processed) {
      // mapped node
      // std::cout << "processing t2_i = " << t2_i << std::endl;
      if (t2_parent_[t2_i] > -1) { // root has no parent
        // descendants
        t2_count_mapped_desc[t2_parent_[t2_i]] += t2_count_mapped_desc[t2_i] + 1;
        // left
        if (t2_rl_[t2_i] > -1) {
          if (t2_size_[t2_i] == 1) { // leaf node
            t2_count_mapped_left[t2_rl_[t2_i]] = t2_count_mapped_left[t2_i] + 1;
          } else { // inner node
            t2_count_mapped_left[t2_rl_[t2_i]] += 1;
          }
        }
        if (t2_post_to_pre_[t2_parent_[t2_i]] + 1 == t2_post_to_pre_[t2_i]) { // leftmost child of its parent
          t2_count_mapped_left[t2_parent_[t2_i]] = t2_count_mapped_left[t2_i];
        }
      }
      ++t2_i;
    } else { // The current mapped node has been processed and it's enough to
             // fix the values it updated.
      // std::cout << "processing t2_i = " << t2_i << std::endl;
      if (t2_parent_[t2_i] > -1) { // root has no parent
        // descendants
        t2_count_mapped_desc[t2_parent_[t2_i]] += 1; // THE ONLY DIFF TO ABOVE.
      }
      ++t2_i;
    }
    
    ted_mapping.push_back({cur_t1, cur_t2});
    
    // prev_t1 = cur_t1;
    prev_t2 = cur_t2;
  }
  
  // TODO: Gap after the last mapped node until the last node in the tree. 
  
  // std::cout << "t1_count_mapped_desc : ";
  // for (auto e : t1_count_mapped_desc) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t1_count_mapped_left : ";
  // for (auto e : t1_count_mapped_left) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t2_count_mapped_desc : ";
  // for (auto e : t2_count_mapped_desc) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  // 
  // std::cout << "t2_count_mapped_left : ";
  // for (auto e : t2_count_mapped_left) {
  //   std::cout << e << " ";
  // } std::cout << std::endl;
  
  return ted_mapping;
};

template <typename Label, typename CostModel>
std::vector<std::pair<int, int>> GreedyUB<Label, CostModel>::revise_greedy_mapping(const std::vector<std::pair<int, int>>& mapping) {
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
    std::vector<int>& rl) {
  int current_leaf = -1;
  int post_id = -1;
  for(int i = 0; i < input_size; i++) { // loop in right-to-left preorder
    post_id = input_size - i - 1; // Translation from right-to-left preorder
                                  // to postorder.
    rl[post_id] = current_leaf;
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
  t1_rl_.clear();
  t2_rl_.clear();
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
  
  t1_rl_.resize(t1_input_size_);
  t2_rl_.resize(t2_input_size_);
  post_traversal_indexing(t1_input_size_, t1_node_, t1_rl_);
  post_traversal_indexing(t2_input_size_, t2_node_, t2_rl_);
};

template <typename Label, typename CostModel>
bool GreedyUB<Label, CostModel>::k_relevant(const int x, const int y, const int k) const {
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
const typename GreedyUB<Label, CostModel>::TestItems GreedyUB<Label, CostModel>::get_test_items() const {
  TestItems test_items = {
    t1_label_il_,
    t1_post_to_pre_,
    t1_parent_,
    t1_rl_,
    t1_depth_,
    t1_size_,
  };
  return test_items;
};

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_IMPL_H
