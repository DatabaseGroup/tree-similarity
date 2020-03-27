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

/// \file join/leaf_dist_histogram/ldh_join_impl.h
///
/// \details
/// Implements the LDHJoin tree similarity join.

#pragma once

template <typename Label, typename VerificationAlgorithm>
LDHJoinTI<Label, VerificationAlgorithm>::LDHJoinTI() {
  pre_candidates_ = 0;
  sum_subproblem_counter_ = 0;
  il_lookups_ = 0;
}

template <typename Label, typename VerificationAlgorithm>
void LDHJoinTI<Label, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  // Convert trees to leaf distance histograms.
  convert_trees_to_histograms(trees_collection, histogram_collection);

  // Retrieves candidates from the candidate index.
  retrieve_candidates(histogram_collection, candidates, distance_threshold);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);
}


template <typename Label, typename VerificationAlgorithm>
void LDHJoinTI<Label, VerificationAlgorithm>::convert_trees_to_histograms(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection) {

  // Convert trees to leaf distance histograms.
  leaf_dist_histogram_converter::Converter<Label> ldhc;
  ldhc.create_histogram(trees_collection, histogram_collection);
  il_size_ = ldhc.get_maximum_leaf_dist();
}

template <typename Label, typename VerificationAlgorithm>
void LDHJoinTI<Label, VerificationAlgorithm>::retrieve_candidates(
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection,
    std::vector<std::pair<int, int>>& candidates,
    const double distance_threshold) {

  // Initialize candidate index.
  ldh_candidate_index::CandidateIndex c_index;

  // Retrieve candidates from the candidate index.
  c_index.lookup(histogram_collection, candidates, il_size_, distance_threshold);

  // Copy the number of pre-candidates.
  pre_candidates_ = c_index.get_number_of_pre_candidates();
  // Copy the number of inverted list lookups.
  il_lookups_ = c_index.get_number_of_il_lookups();
}

template <typename Label, typename VerificationAlgorithm>
void LDHJoinTI<Label, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  label::LabelDictionary<Label> ld;
  typename VerificationAlgorithm::AlgsCostModel cm(ld);
  VerificationAlgorithm ted_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex ti_1;
  typename VerificationAlgorithm::AlgsTreeIndex ti_2;

  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    node::index_tree(ti_1, trees_collection[pair.first], ld, cm);
    node::index_tree(ti_2, trees_collection[pair.second], ld, cm);
    double ted_value = ted_algorithm.ted_k(ti_1, ti_2, distance_threshold);
    if(ted_value <= distance_threshold)
      join_result.emplace_back(pair.first, pair.second, ted_value);
    // Sum up all number of subproblems
    sum_subproblem_counter_ += ted_algorithm.get_subproblem_count();
  }
}

template <typename Label, typename VerificationAlgorithm>
long long int
    LDHJoinTI<Label, VerificationAlgorithm>::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

template <typename Label, typename VerificationAlgorithm>
long long int LDHJoinTI<Label, VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm>
long long int
    LDHJoinTI<Label, VerificationAlgorithm>::get_number_of_il_lookups() const {
  return il_lookups_;
}
