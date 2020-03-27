// The MIT License (MIT)
// Copyright (c) 2018 Thomas Huetter
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

/// \file join/label_histogram/lh_candidate_index_impl.h
///
/// \details
/// Implements a candidate index that efficiently and effectively returns tree 
/// pairs that satisfy the label histogram lower bound. 

#pragma once

CandidateIndex::CandidateIndex() {
  pre_candidates_ = 0;
  il_lookups_ = 0;
}

void CandidateIndex::lookup(
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection,
    std::vector<std::pair<int, int>>& join_candidates,
    const int il_size,
    const double distance_threshold) {
  // inverted list index
  std::vector<std::vector<std::pair<int, int>>> il_index(il_size+1);
  // id of the tree that is currently processed
  int current_tree_id = 0;
  // overlap count for all trees
  std::vector<int> intersection_cnt(histogram_collection.size());
  // store ids of all tree with an overlap, called pre candidates

  // iterate through all histograms in the given collection
  for (auto& histogram: histogram_collection) {
    std::vector<int> pre_candidates;

    // add all small trees that does not have to share a common label in the prefix
    if(histogram.first <= distance_threshold) {
      for(int i = 0; i < current_tree_id; ++i) {
        pre_candidates.push_back(i);
        intersection_cnt[i] += 1;
      }
    }

    for (auto& element: histogram.second) {
      for (auto& il_entry: il_index[element.first]) {
        int intersection = std::min(element.second, il_entry.second);
        if(intersection_cnt[il_entry.first] == 0 && intersection != 0)
          pre_candidates.push_back(il_entry.first);
        intersection_cnt[il_entry.first] = std::min((intersection_cnt[il_entry.first] + intersection), histogram.first);
      }
      // add current element to the index
      il_index[element.first].emplace_back(current_tree_id, element.second);
    }

    // count the number of pre canidates
    pre_candidates_ += pre_candidates.size();

    // verify all pre candidates
    for(int pre_cand_id: pre_candidates) {
      if((histogram_collection[current_tree_id].first + histogram_collection[pre_cand_id].first - 
          (2 * intersection_cnt[pre_cand_id])) / 2 <= distance_threshold)
        join_candidates.emplace_back(current_tree_id, pre_cand_id);
      // reset intersection counter
      intersection_cnt[pre_cand_id] = 0;
    }
    current_tree_id++;
  }
}

long int CandidateIndex::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

void CandidateIndex::set_number_of_pre_candidates(
    const long int pc) {
  pre_candidates_ = pc;
}

long int CandidateIndex::get_number_of_il_lookups() const {
  return il_lookups_;
}
