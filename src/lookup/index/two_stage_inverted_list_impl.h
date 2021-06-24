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
  int set_size;
  long int label_id;
  long int subtree_size;

  // Iterate through all sets in a given collection.
  for (unsigned int s = 0; s < sets_collection.size(); s++) {
    set_size = sets_collection[s].first;
    // Iterate through elements of a given set.
    for (unsigned int e = 0; e < sets_collection[s].second.size(); e++) {
      label_id = sets_collection[s].second[e].id;
      subtree_size = sets_collection[s].second[e].descendants + 1;
      // Create a new entry for element subtree_size in case it is not yet in the map.
      if (il_index_[label_id].element_list.find(subtree_size) == 
          il_index_[label_id].element_list.end()) {
        il_index_[label_id].element_list.insert(
          std::pair<int, std::multimap<int, std::pair<int, int>>> 
          (subtree_size, std::multimap<int, std::pair<int, int>>()));
      }
      // Insert new index element.
      il_index_[label_id].element_list[subtree_size].insert(
          std::multimap<int, std::pair<int, int>>::value_type(
            set_size, std::make_pair(s, sets_collection[s].second[e].ancestors)));
    }
  }
}

void TwoStageInvertedList::lookup(long int& label_id,
    long int subtree_size, int& size, 
    std::unordered_set<long int>& candidates,
    const double distance_threshold) {
  long int start_subtree_range = subtree_size - distance_threshold;
  if (start_subtree_range < 0) start_subtree_range = 0;
  long int end_subtree_range = subtree_size + distance_threshold;
  long int start_size_range = size - distance_threshold;
  if (start_size_range < 0) start_size_range = 0;
  long int end_size_range = size + distance_threshold;
  // Iterate over the first distance_threshold + 1 positions.
  auto iter = il_index_[label_id].element_list.lower_bound(start_subtree_range);
  while (iter != il_index_[label_id].element_list.end()) {
    if (iter->first > end_subtree_range) {
      break;
    }
    // for (auto const& y : iter->second) {
    auto iter2 = iter->second.lower_bound(start_size_range);
    while (iter2 != iter->second.end()) {
      if (iter2->first > end_size_range) {
        break;
      }
      candidates.insert(iter2->second.first);
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
