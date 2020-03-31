// The MIT License (MIT)
// Copyright (c) 2018, 2019 Mateusz Pawlik.
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

/// Contains the implementation of the LGM algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double LGMTreeIndex<CostModel, TreeIndex>::ted(const TreeIndex& t1,
    const TreeIndex& t2) {
  // Use a trivial TED upper bound for k.
  // NOTE: This makes the algorithm quadratic and inefficient, but it is
  //       correct.
  return ted_k(t1, t2, t1.tree_size_ + t2.tree_size_);
}

template <typename CostModel, typename TreeIndex>
double LGMTreeIndex<CostModel, TreeIndex>::ted_k(const TreeIndex& t1,
    const TreeIndex& t2, const int k) {
  // Initialize the internals.
  init(t2);

  // We don't compute the upper bound for node pairs with size lower bound
  // greater than threshold.
  if (std::abs(t1.tree_size_ - t2.tree_size_) > k) {
    return std::numeric_limits<double>::infinity();
  }
  return mapping_cost(t1, t2, lb_mapping_fill_gaps(t1, t2, k));
}

template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::init(const TreeIndex& t2) {
  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  // Copy the inverted list such that we can modify it: assign `-1` to node
  // ids that have been used.
  t2_label_il_ = std::unordered_map<int, std::vector<int>>(t2.inverted_list_label_id_to_postl_);

  // Clear and initialise the staring position to traverse the inverted lists.
  t2_label_il_start_pos_.clear();
  for (const auto& pair : t2_label_il_) {
    t2_label_il_start_pos_[pair.first] = 0;
  }
}

template <typename CostModel, typename TreeIndex>
double LGMTreeIndex<CostModel, TreeIndex>::mapping_cost(
    const TreeIndex& t1, const TreeIndex& t2,
    const std::vector<std::pair<int, int>>& mapping) const {
  double result = 0.0;
  for (const auto& m : mapping) {
    result += c_.ren(t1.postl_to_label_id_[m.first], t2.postl_to_label_id_[m.second]); // renames
  }
  result += t1.tree_size_ - mapping.size(); // deletions
  result += t2.tree_size_ - mapping.size(); // insertions
  return result;
}

template <typename CostModel, typename TreeIndex>
std::vector<std::pair<int, int>> LGMTreeIndex<CostModel, TreeIndex>::lb_mapping(
    const TreeIndex& t1, const TreeIndex& t2, const int k) {
  std::vector<std::pair<int, int>> mapping;
  int cand_id = 0; // Postorder id of a candidate node in T2 carrying a matching label.
  int end_pos = 0; // Maximum end position for reading postorder ids of nodes with specific label.
  int pos = 0; // Position iterator.
  int t1_label_id = 0;
  for (int i = 0; i < t1.tree_size_; ++i) { // Loop in postorder.
    t1_label_id = t1.postl_to_label_id_[i];
    std::vector<int>& candidate_ids = t2_label_il_[t1_label_id]; // Postorder ids of nodes in T2 carrying label t1_label_id.
    // Use 2k+1 window.
    pos = t2_label_il_start_pos_[t1_label_id]; // Start position for reading postorder ids of nodes with specific label.
    end_pos = std::min(pos + 2 * k, (int)candidate_ids.size()-1);
    while (pos <= end_pos) {
      ++subproblem_counter_;
      cand_id = candidate_ids[pos];
      if (cand_id - i > k) {
        // We're certainly outside the window from the right side.
        break;
      }
      if (cand_id == -1 || i - cand_id > k) {
        // The label has been mapped or we're outside the window from the left side.
        ++t2_label_il_start_pos_[t1_label_id]; // For the consecutive values of i we don't need to start before.
      } else if (k_relevant(t1, t2, i, cand_id, k)) {
        // The pair can be mapped.
        mapping.push_back({i, cand_id});
        candidate_ids[pos] = -1; // Mark label as mapped - to not use it again.
        break;
      }
      ++pos;
    }
  }
  mapping = to_ted_mapping(t1, t2, mapping);
  return mapping;
}

template <typename CostModel, typename TreeIndex>
std::vector<std::pair<int, int>> LGMTreeIndex<CostModel, TreeIndex>::lb_mapping_fill_gaps(
    const TreeIndex& t1, const TreeIndex& t2, const int k) {
  std::vector<std::pair<int, int>> mapping = lb_mapping(t1, t2, k);
  mapping = fill_gaps_in_mapping(t1, t2, mapping, k);
  return mapping;
}


template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::update_desc_when_not_mapped(
    const int node, std::vector<int>& count_mapped_desc,
    const std::vector<int>& parent, const int input_size) const {
  if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                               // the nodes of dummy mapping in fill_gaps_in_mapping
                               // do not exist.
    count_mapped_desc[parent[node]] += count_mapped_desc[node];
  }
}

template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::update_desc_when_mapped(
    const int node, std::vector<int>& count_mapped_desc,
    const std::vector<int>& parent, const int input_size) const {
  if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                               // the nodes of dummy mapping in fill_gaps_in_mapping
                               // do not exist.
    count_mapped_desc[parent[node]] += count_mapped_desc[node] + 1;
  }
}

template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::update_prop_desc_when_not_mapped(
    const int node, std::vector<int>& count_mapped_desc,
    std::vector<int>& propagate_mapped_desc_count,
    const std::vector<int>& parent, const int input_size) const {
  int prop = propagate_mapped_desc_count[node];
  if (prop > 0) {
    if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                                 // the nodes of dummy mapping in fill_gaps_in_mapping
                                 // do not exist.
      count_mapped_desc[parent[node]] += prop;
      propagate_mapped_desc_count[parent[node]] += prop;
    }
    propagate_mapped_desc_count[node] = 0; // This is actually not needed for
                                           // t1 because the node ids are ordered.
  }
}

template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::update_prop_desc_when_mapped(
    const int node, std::vector<int>& count_mapped_desc,
    std::vector<int>& propagate_mapped_desc_count,
    const std::vector<int>& parent, const int input_size) const {
  if (node < input_size - 1) { // Root has no parent nor the right leaf, and
                               // the nodes of dummy mapping in fill_gaps_in_mapping
                               // do not exist.
    count_mapped_desc[parent[node]] += 1;
    propagate_mapped_desc_count[parent[node]] += 1;
  }
  update_prop_desc_when_not_mapped(node, count_mapped_desc, propagate_mapped_desc_count, parent, input_size);
}

template <typename CostModel, typename TreeIndex>
void LGMTreeIndex<CostModel, TreeIndex>::get_mapped_ancestors_counts(
    const TreeIndex& t1, const TreeIndex& t2,
    std::vector<std::pair<int, int>>& mapping,
    std::vector<int>& t1_count_mapped_anc,
    std::vector<int>& t2_count_mapped_anc) const {
  std::vector<int> t1_mapped_nodes(t1.tree_size_); // indexed in postorder
  std::vector<int> t2_mapped_nodes(t2.tree_size_); // indexed in postorder
  for (const auto& m : mapping) {
    t1_mapped_nodes[m.first] = 1;
    t2_mapped_nodes[m.second] = 1;
  }
  int i_in_post = 0;
  // TODO: Verify if t1_mapped_nodes and t2_mapped_nodes are necessary.
  // T1
  for (int i = 1; i < t1.tree_size_; ++i) { // loop in preorder; root has no parent and 0 mapped ancestors
    i_in_post = t1.prel_to_postl_[i];
    t1_count_mapped_anc[i_in_post] = t1_mapped_nodes[t1.postl_to_parent_[i_in_post]];
    t1_mapped_nodes[i_in_post] += t1_mapped_nodes[t1.postl_to_parent_[i_in_post]];
  }
  // T2
  for (int i = 1; i < t2.tree_size_; ++i) { // loop in preorder; root has no parent and 0 mapped ancestors
    i_in_post = t2.prel_to_postl_[i];
    t2_count_mapped_anc[i_in_post] = t2_mapped_nodes[t2.postl_to_parent_[i_in_post]];
    t2_mapped_nodes[i_in_post] += t2_mapped_nodes[t2.postl_to_parent_[i_in_post]];
  }
}

template <typename CostModel, typename TreeIndex>
bool LGMTreeIndex<CostModel, TreeIndex>::if_in_corresponding_regions(
    const TreeIndex& t1, const TreeIndex& t2, int t1_begin_gap, int i,
    int t1_end_gap, int t2_begin_gap, int j, int t2_end_gap) const {
  
  // Begin or end pairs of a gap may not exist.
  
  if (t1_begin_gap == -1 && t1_end_gap == t1.tree_size_) { // also t2_begin_gap == -1 && t2_end_gap == t2_input_size_
    // Any mapping is fine - no defined regions.
    return true;
  }
  
  int i_pre = t1.postl_to_prel_[i];
  int j_pre = t2.postl_to_prel_[j];
  
  // The following two if statements take care of the cases, when a gap is
  // before the first or after the last node pair in mapping.

  // These four variables can be initialized only if the begin and end gaps
  // are true notde pairs.
  int t1_begin_gap_pre = -1;
  int t2_begin_gap_pre = -1;
  int t1_end_gap_pre = t1.tree_size_;
  int t2_end_gap_pre = t2.tree_size_;
  if (t1_begin_gap > -1) { // also t2_begin_gap > -1
    t1_begin_gap_pre = t1.postl_to_prel_[t1_begin_gap];
    t2_begin_gap_pre = t2.postl_to_prel_[t2_begin_gap];
  }
  if (t1_end_gap < t1.tree_size_) { // also t2_end_gap < t2.tree_size_
    t1_end_gap_pre = t1.postl_to_prel_[t1_end_gap];
    t2_end_gap_pre = t2.postl_to_prel_[t2_end_gap];
  }
  
  // Gap is before first node pair in mapping. (i,j) is a pair of nodes with
  // postorder IDs smaller than nodes in end_gap. Then, if preorder IDs are
  // greater then end_gap, (i,j) is in descendants of end_gap. If preorder IDs
  // are smaller, (i,j) is to the left.
  if (t1_begin_gap == -1) { // also t2_begin_gap == -1
    if (i_pre > t1_end_gap_pre && j_pre > t2_end_gap_pre) {
      return true;
    }
    if (i_pre < t1_end_gap_pre && j_pre < t2_end_gap_pre) {
      return true;
    }
  }
  
  // Gap is after the last node pair in mapping. (i,j) is a pair of nodes with
  // postorder IDs greater than nodes in begin_gap. Then, if preorder IDs are
  // greater then begin_gap, (i,j) is to the right of begin_gap. If preorder IDs
  // are smaller, (i,j) is in ancestors.
  if (t1_end_gap == t1.tree_size_) { // also t2_end_gap == t2.tree_size_
    if (i_pre > t1_begin_gap_pre && j_pre > t2_begin_gap_pre) {
      return true;
    }
    if (i_pre < t1_begin_gap_pre && j_pre < t2_begin_gap_pre) {
      return true;
    }
  }
  
  // Four different cases.
  
  if (t1_end_gap_pre < i_pre && t2_end_gap_pre < j_pre) {
    // t1_begin_gap is desc of i is desc of t1_end_gap
    if (i_pre < t1_begin_gap_pre && j_pre < t2_begin_gap_pre) {
      return true;
    }
    // t1_begin_gap is desc of t1_end_gap, i is to the right of t1_begin_gap
    // or
    // t1_begin_gap is to the left of t1_end_gap, i is desc of t1_end_gap
    if (t1_begin_gap_pre < i_pre && t2_begin_gap_pre < j_pre) {
      return true;
    }
  }
  
  if (i_pre < t1_end_gap_pre && j_pre < t2_end_gap_pre) {
    // t1_end_gap is to the right of t1_begin_gap, i is to the right of
    // t1_begin_gap and to the left of t1_end_gap
    if (t1_begin_gap_pre < i_pre && t2_begin_gap_pre < j_pre) {
      return true;
    }
    // i is anc of t1_begin_gap, t1_end_gap is to the right of t1_begin_gap
    if (i_pre < t1_begin_gap_pre && j_pre < t2_begin_gap_pre) {
      return true;
    }
  }
  
  return false;
}

template <typename CostModel, typename TreeIndex>
std::vector<std::pair<int, int>> LGMTreeIndex<CostModel, TreeIndex>::fill_gaps_in_mapping(
    const TreeIndex& t1, const TreeIndex& t2,
    std::vector<std::pair<int, int>>& mapping, const int k) {
  
  // In result_mapping we store the output of this function.
  std::vector<std::pair<int, int>> result_mapping;
  
  // The counts for mapped descendants and nodes to the left must be maintained.
  std::vector<int> t1_count_mapped_desc(t1.tree_size_);
  std::vector<int> t2_count_mapped_desc(t2.tree_size_);
    
  // The counts for mapped ancestors.
  std::vector<int> t1_count_mapped_anc(t1.tree_size_);
  std::vector<int> t2_count_mapped_anc(t2.tree_size_);
  get_mapped_ancestors_counts(t1, t2, mapping, t1_count_mapped_anc, t2_count_mapped_anc);
  
  // Add a dummy mapping pair to cover the gap from the last mapping until the
  // ends of the trees.
  mapping.push_back({t1.tree_size_, t2.tree_size_});
  
  std::pair<int, int> begin_gap = {-1, -1};
  std::pair<int, int> end_gap;
  std::vector<std::pair<int, int>>::iterator end_gap_it = mapping.begin();
  
  while (end_gap_it != mapping.end()) {
    end_gap = *end_gap_it;
    // If there is no gap, continue with the next mapped pair.
    if (end_gap.first - begin_gap.first == 1 || end_gap.second - begin_gap.second == 1) {
      // Update the counts for nodes between begin_gap and end_gap, if any,
      // as not-mapped nodes.
      for (int not_mapped_first = begin_gap.first + 1; not_mapped_first < end_gap.first; ++not_mapped_first) {
        update_desc_when_not_mapped(not_mapped_first, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
      }
      // Update the counts for nodes in end_gap as mapped nodes.
      update_desc_when_mapped(end_gap.first, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
      // Update the counts for nodes between begin_gap and end_gap, if any,
      // as not-mapped nodes.
      for (int not_mapped_second = begin_gap.second + 1; not_mapped_second < end_gap.second; ++not_mapped_second) {
        update_desc_when_not_mapped(not_mapped_second, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
      }
      // Update the counts for nodes in end_gap as mapped nodes.
      update_desc_when_mapped(end_gap.second, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
      // Push the end of the current gap to the result.
      result_mapping.push_back(end_gap);
      // Update the begin of the next gap.
      begin_gap = end_gap;
      ++end_gap_it;
      continue;
    }
    // There is a gap --> try to map nodes --> at the first node pair mapped,
    // change the gap and continue the loop.
    int i = begin_gap.first + 1;
    int i_last = end_gap.first - 1;
    int j = begin_gap.second + 1;
    int j_last = end_gap.second - 1;
    bool mapped_in_gap = false;
    while (i <= i_last && j <= j_last) {
      ++subproblem_counter_;
      if (t1_count_mapped_desc[i] == t2_count_mapped_desc[j] &&
          t1_count_mapped_anc[i] == t2_count_mapped_anc[j] &&
          k_relevant(t1, t2, i, j, k) &&
          if_in_corresponding_regions(t1, t2, begin_gap.first, i, end_gap.first, begin_gap.second, j, end_gap.second)) {
        // Map (i,j) + push to result mapping + update counts for both nodes
        // incremented as mapped.
        update_desc_when_mapped(i, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
        update_desc_when_mapped(j, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
        result_mapping.push_back({i, j});
        begin_gap = {i, j};
        mapped_in_gap = true;
        break;
      } else {
        // Update counts for the incremented node as non-mapped.
        if (i_last - i > j_last - j) {
          update_desc_when_not_mapped(i, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
          ++i;
        } else {
          update_desc_when_not_mapped(j, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
          ++j;
        }
      }
    }
    if (mapped_in_gap) {
      continue;
    } else {
      // Check the case when only one of i or j incremented beyond the limit.
      // The gap in one of the trees has been used up.
      // Nothing mapped in the gap.
      for (int j_missing = j; j_missing <= j_last; ++j_missing) {
        update_desc_when_not_mapped(j, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
      }
      for (int i_missing = i; i_missing <= i_last; ++i_missing) {
        update_desc_when_not_mapped(i, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
      }
      // Update the counts for nodes in end_gap as mapped nodes.
      update_desc_when_mapped(end_gap.first, t1_count_mapped_desc, t1.postl_to_parent_, t1.tree_size_);
      update_desc_when_mapped(end_gap.second, t2_count_mapped_desc, t2.postl_to_parent_, t2.tree_size_);
      // Push the end of the current gap to the result.
      result_mapping.push_back(end_gap);
      // Update the begin of the next gap.
      begin_gap = end_gap;
      ++end_gap_it;
      continue;
    }
  }
  // Remove the last dummy mapping.
  result_mapping.pop_back();
  return result_mapping;
}

template <typename CostModel, typename TreeIndex>
std::vector<std::pair<int, int>> LGMTreeIndex<CostModel, TreeIndex>::to_ted_mapping(
    const TreeIndex& t1, const TreeIndex& t2,
    const std::vector<std::pair<int, int>>& mapping) const {
  std::vector<std::pair<int, int>> ted_mapping;
  
  // Vectors storing the number of descendants mapped for every node.
  std::vector<int> t1_count_mapped_desc(t1.tree_size_);
  std::vector<int> t2_count_mapped_desc(t2.tree_size_);
  // Vector to store the probagation values of the mapped descendants.
  // This is a fix compared to the previous version.
  std::vector<int> t1_propagate_mapped_desc_count(t1.tree_size_);
  std::vector<int> t2_propagate_mapped_desc_count(t2.tree_size_);
  
  int t1_i = 0; // Iterator over nodes in T1 used to update descendants.
  int t2_i = 0; // Iterator over nodes in T2 used to update descendants.
  int cur_t1 = 0; // Node of T1 in the current mapped pair considered.
  int cur_t2 = 0; // Node of T2 in the current mapped pair considered.
  int prev_t2 = -1; // Previously mapped node in T2 used for increasing
                    // postorder test.
  
  // Loop over all pairs in a one-to-one mapping.
  for (const auto& m : mapping) {
    cur_t1 = m.first;
    cur_t2 = m.second;
    // Increasing postorder id test.
    // The pairs in the input one-to-one mapping are sorted on the first
    // element. The second elements of two consecutive pairs must not have
    // decreasing postorder ids.
    // NOTE: Non-decreasing ids of second elements could be ensured in lb_mapping.
    //       But, this will reduce the number of initially mapped node which may not be
    //       desired. For example, if given M=<(a,b),(a+1,b+2),(a+2,b+1)>, (a+1,b+2) is
    //       removed due to violating TED conditions, (a+2,b+1) would may still be valid, and
    //       have a lower cost than (a+1,b+1) possible from filling gaps.
    if (cur_t2 < prev_t2) {
      continue;
    }
    
    // NOTE: Go back with t2_i if the current node ids are smaller.
    //       If cur_t2 < t2_i => cur_t2 has been processed as a non-mapped node and
    //       it has to be processed as a mapped node, rolling back the changes made
    //       to it.
    //       If cur_t2 == t2_i => cur_t2 has to be processed as a mapped node,
    //       it has not been processed before.
    
    // NOTE: We need the descendants up to date for a node v, although we may drop the pair with that
    //       node. Then, the next node we process, v', may have the postorder smaller than v. This
    //       causes a reupdate of the descendants for nodes v' until v.
    
    // Nodes not in mapping in T1 from last mapped to one before cur_t1.
    while (t1_i < cur_t1) {
      update_prop_desc_when_not_mapped(t1_i, t1_count_mapped_desc, t1_propagate_mapped_desc_count, t1.postl_to_parent_, t1.tree_size_);
      ++t1_i;
    }
    // Nodes not in mapping in T2 from last mapped to one before cur_t2.
    while (t2_i < cur_t2) {
      update_prop_desc_when_not_mapped(t2_i, t2_count_mapped_desc, t2_propagate_mapped_desc_count, t2.postl_to_parent_, t2.tree_size_);
      ++t2_i;
    }
    // Mapped descendants test.
    if (t1_count_mapped_desc[cur_t1] == t2_count_mapped_desc[cur_t2]) {
      ted_mapping.push_back({cur_t1, cur_t2});
      prev_t2 = cur_t2;
      update_prop_desc_when_mapped(cur_t1, t1_count_mapped_desc, t1_propagate_mapped_desc_count, t1.postl_to_parent_, t1.tree_size_);
      t1_i = cur_t1 + 1;
      update_prop_desc_when_mapped(cur_t2, t2_count_mapped_desc, t2_propagate_mapped_desc_count, t2.postl_to_parent_, t2.tree_size_);
      t2_i = cur_t2 + 1;
    }// else {cur_t1, cur_t2} cannot be mapped.
  }
  
  // NOTE: The gap after the last mapped node until the last node in the tree
  //       does not matter. Here, we only revise the given mapping. If there
  //       are no pairs to revise left, we can stop. 
    
  return ted_mapping;
}

template <typename CostModel, typename TreeIndex>
bool LGMTreeIndex<CostModel, TreeIndex>::k_relevant(const TreeIndex& t1,
    const TreeIndex& t2,const int x, const int y, const int k) const {
  // The lower bound formula (RA + D + L):
  // |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  // New lower bound formula (R + A + D + L):
  // |(|T1|-(x+1)-depth(x))-(|T2|-(y+1)-depth(y))| + |depth(x)-depth(y)| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  int x_size = t1.postl_to_size_[x];
  int y_size = t2.postl_to_size_[y];
  // int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
  //                   std::abs(x_size - y_size) +
  //                   std::abs(((x+1) - x_size) - ((y+1) - y_size));

  int lower_bound = std::abs((t1.tree_size_ - (x+1) - t1.postl_to_depth_[x]) - (t2.tree_size_ - (y+1) - t2.postl_to_depth_[y])) +
                    std::abs(t1.postl_to_depth_[x] - t2.postl_to_depth_[y]) +
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
}
