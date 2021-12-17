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
  long int ancestors;
  long int right_left;

  // Iterate through all sets in a given collection.
  for (unsigned int s = 0; s < sets_collection.size(); s++) {
    // Iterate through elements of a given set.
    for (unsigned int e = 0; e < sets_collection[s].second.size(); e++) {
      label_id = sets_collection[s].second[e].id;
      descendants = sets_collection[s].second[e].descendants;
      ancestors = sets_collection[s].second[e].ancestors;
      right_left = sets_collection[s].first - descendants - ancestors - 1;
      il_index_[label_id].element_list[descendants][ancestors][right_left].push_back(s);
    }
  }
}

void TwoStageInvertedList::lookup(long int& q_label_id,
    long int descendants, long int ancestors, int& q_tree_size, 
    std::unordered_set<long int>& candidates,
    const double distance_threshold) {
  // All nodes except the current node, its descendants, and ancestors.
  long int right_left = q_tree_size - descendants - ancestors - 1;
  // Range for descendants stage.
  long int start_desc_range = descendants - distance_threshold;
  if (start_desc_range < 0) start_desc_range = 0;
  // Range for ancestor stage.
  long int start_anc_range = 0;
  // Range for right-left stage.
  long int start_rl_range = 0;
  // Hold the adaptive thresholds for the different stages of the index.
  long int threshold_stage1;
  long int threshold_stage2;
  long int threshold_stage3;
  // Binary search to first relevant element (threshold range) in 
  // the descandants index layer.
  auto iter_desc = il_index_[q_label_id].element_list.lower_bound(start_desc_range);
  while (iter_desc != il_index_[q_label_id].element_list.end()) {
    // Incrementally improving the threshold.
    threshold_stage1 = distance_threshold - std::abs(descendants - iter_desc->first);
    // Descendants lower bound exceeds the threshold.
    if (threshold_stage1 < 0) {
      ++iter_desc;
      continue;
    }
    // Binary search to first relevant element (threshold range) in 
    // the ancestor index layer.
    start_anc_range = ancestors - threshold_stage1;
    if (start_anc_range < 0) start_anc_range = 0;
    auto iter_anc = iter_desc->second.lower_bound(start_anc_range);
    while (iter_anc != iter_desc->second.end()) {
      threshold_stage2 = threshold_stage1 - std::abs(ancestors - iter_anc->first);
      // Descendants and ancestor lower bound exceeds the threshold.
      if (threshold_stage2 < 0) {
        ++iter_anc;
        continue;
      }
      // Binary search to first relevant element (threshold range) in 
      // the right-left index layer.
      start_rl_range = right_left - threshold_stage2;
      if (start_rl_range < 0) start_rl_range = 0;
      auto iter_rl = iter_anc->second.lower_bound(start_rl_range);
      while (iter_rl != iter_anc->second.end()) {
        threshold_stage3 = threshold_stage2 - std::abs(right_left - iter_rl->first);
        // Descendants, ancestor, and right-left lower bound exceeds the threshold.
        if (threshold_stage3 < 0) {
          ++iter_rl;
          continue;
        }
        // Add all to candidates.
        for (auto& tree_id : iter_rl->second) {
          candidates.insert(tree_id);
        }
        ++iter_rl;
      }
      ++iter_anc;
    }
    ++iter_desc;
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
