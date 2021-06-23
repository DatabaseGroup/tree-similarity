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
  long int postorder_id;

  // Iterate through all sets in a given collection.
  for (unsigned int s = 0; s < sets_collection.size(); s++) {
    set_size = sets_collection[s].first;
    // Iterate through elements of a given set.
    for (unsigned int e = 0; e < sets_collection[s].second.size(); e++) {
      label_id = sets_collection[s].second[e].id;
      postorder_id = sets_collection[s].second[e].postorder_id;
      // Create a new entry for element postorder_id in case it is not yet in the map.
      if (il_index_[label_id].element_list.find(postorder_id) == 
          il_index_[label_id].element_list.end()) {
        il_index_[label_id].element_list.insert(
          std::pair<int, std::vector<std::pair<int, int>>> 
          (postorder_id, std::vector<std::pair<int, int>>()));
      }
      // std::cout << label_id << " -> " << postorder_id << " -> (" << set_size << ", " << s << ")" << std::endl;
      il_index_[label_id].element_list[postorder_id].push_back(
        std::make_pair(set_size, s));
    }
  }
  // Sort all vectors by tree size for each label ...
  for (unsigned int l = 0; l < nr_of_labels_; l++) {
    // ... and position.
    for (auto& pos: il_index_[l].element_list) {
      // Using simple sort() function to sort
      sort(pos.second.begin(), pos.second.end());
    }
  }
}

void TwoStageInvertedList::lookup(long int& label_id,
    long int& postorder_id, int& size, 
    std::unordered_set<long int>& candidates,
    const double distance_threshold) {
  long int start_pid_range = postorder_id - distance_threshold;
  long int end_pid_range = postorder_id + distance_threshold;
  // long int start_size_range = size + distance_threshold;
  long int end_size_range = size + distance_threshold;
  // Iterate over the first distance_threshold + 1 positions.
  auto iter = il_index_[label_id].element_list.lower_bound(start_pid_range);
  while (iter != il_index_[label_id].element_list.end()) {
    // pos = postorder_id - distance_threshold; 
    //   pos <= postorder_id + distance_threshold
    if (iter->first > end_pid_range) {
      break;
    }
    // Go through sets in index until the size is too large.
    // TODO: Size filter from the beginning (binary search).
    for (auto const& y : iter->second) {
      if (y.first > end_size_range) {
        break;
      }
      candidates.insert(y.second);
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
