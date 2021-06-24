// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter
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

/// \file lookup/index/two_stage_inv_list_impl.h

#pragma once

TwoStageInvertedList::TwoStageInvertedList(long int labels) {
  pre_candidates_ = 0;
  il_lookups_ = 0;
  nr_of_labels_ = labels;
  il_index_.resize(nr_of_labels_);
}

void TwoStageInvertedList::build(std::vector<std::pair<int, 
    std::vector<label_set_converter_index::LabelSetElement>>>& sets_collection) {
  long int label_id;
  long int descendants;

  // Iterate through all sets in a given collection.
  for (unsigned int s = 0; s < sets_collection.size(); s++) {
    // Iterate through elements of a given set.
    for (unsigned int e = 0; e < sets_collection[s].second.size(); e++) {
      label_id = sets_collection[s].second[e].id;
      descendants = sets_collection[s].second[e].descendants;
      // Create a new entry for element descendants in case it is not yet in the 
      // map.
      if (il_index_[label_id].element_list.find(descendants) == 
          il_index_[label_id].element_list.end()) {
        il_index_[label_id].element_list.insert(
          std::pair<int, std::multimap<int, std::pair<int, int>>> 
          (descendants, std::multimap<int, std::pair<int, int>>()));
      }
      // Insert new index element.
      il_index_[label_id].element_list[descendants].insert(
          std::multimap<int, std::pair<int, int>>::value_type(
            sets_collection[s].first - descendants, 
            std::make_pair(s, sets_collection[s].second[e].ancestors)));
    }
  }
}

void TwoStageInvertedList::lookup(long int& q_label_id,
    long int descendants, long int ancestors, int& q_tree_size, 
    std::unordered_set<long int>& candidates,
    const double distance_threshold) {
  // All nodes except for the current node and its descendants.
  long int rest_anc = q_tree_size - 1 - descendants;
  long int start_subtree_range = descendants - distance_threshold;
  if (start_subtree_range < 0) start_subtree_range = 0;
  // long int end_subtree_range = descendants + distance_threshold;
  long int start_size_range = 0;
  // long int end_size_range = rest_anc + distance_threshold;
  long int threshold_stage1;
  long int threshold_stage2;
  // Binary search to first relevant element (threshold range) in 
  // the subtree size index layer.
  auto iter = il_index_[q_label_id].element_list.lower_bound(start_subtree_range);
  while (iter != il_index_[q_label_id].element_list.end()) {
    // Incrementally improving the threshold.
    threshold_stage1 = distance_threshold - std::abs(descendants - iter->first);
    // Compare subtree size lower bound.
    if (threshold_stage1 < 0) {
      break;
    }
    // Binary search to first relevant element (threshold range) in 
    // the tree size index layer.
    start_size_range = rest_anc - threshold_stage1;
    if (start_size_range < 0) start_size_range = 0;
    auto iter2 = iter->second.lower_bound(start_size_range);
    while (iter2 != iter->second.end()) {
      threshold_stage2 = threshold_stage1 - std::abs(rest_anc - iter2->first);
      // Compare tree size lower bound with remaining threshold.
      if (threshold_stage2 < 0) {
        break;
      }
      // Compare ancestor lower bound with remaining threshold.
      if (threshold_stage2 >= std::abs(ancestors - iter2->second.second)) {
        candidates.insert(iter2->second.first);
      }
      ++iter2;
    }
    ++iter;
  }
}

long int TwoStageInvertedList::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

void TwoStageInvertedList::set_number_of_pre_candidates(const long int pc) {
  pre_candidates_ = pc;
}

long int TwoStageInvertedList::get_number_of_il_lookups() const {
  return il_lookups_;
}

void TwoStageInvertedList::set_number_of_il_lookups(const long int il) {
  il_lookups_ = il;
}
