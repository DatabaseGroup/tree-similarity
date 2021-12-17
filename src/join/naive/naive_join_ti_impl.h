// The MIT License (MIT)
// Copyright (c) 2017-2020 Mateusz Pawlik.
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

/// Contains the implementation of the naive join algorithm.

#pragma once

template <typename Label, typename VerificationAlgorithm>
NaiveJoinTI<Label, VerificationAlgorithm>::NaiveJoinTI() {}

template <typename Label, typename VerificationAlgorithm>
std::vector<join::JoinResultElement> NaiveJoinTI<Label, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection, 
    const double distance_threshold) {

  label::LabelDictionary<Label> ld;
  typename VerificationAlgorithm::AlgsCostModel cm(ld);

  VerificationAlgorithm ted_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex ti_1;
  typename VerificationAlgorithm::AlgsTreeIndex ti_2;

  std::vector<join::JoinResultElement> result_set;

  double ted_value = std::numeric_limits<double>::infinity();
  int i = 0;
  int j = 0;
  for (auto it_i = trees_collection.begin(); it_i != trees_collection.end(); ++it_i) {
    ++i;
    // Not to do symmetric comparisons.
    j = i;
    node::index_tree(ti_1, *it_i, ld, cm);
    // Start the inner loop with the tree just iafter it_i.
    for (auto it_j = it_i+1; it_j != trees_collection.end(); ++it_j) {
      ++j;
      node::index_tree(ti_2, *it_j, ld, cm);
      ted_value = ted_algorithm.ted_k(ti_1, ti_2, distance_threshold);
      if (ted_value <= distance_threshold) {
        result_set.emplace_back(i, j, ted_value);
      }
      // Sum up all number of subproblems
      sum_subproblem_counter_ += ted_algorithm.get_subproblem_count();
    }
  }

  return result_set;
}

template <typename Label, typename VerificationAlgorithm>
long long int NaiveJoinTI<Label, VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}
