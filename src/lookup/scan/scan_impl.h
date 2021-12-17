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

/// Contains the implementation of the scan lookup algorithm.

#pragma once

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
VerificationUBkScan<Label, VerificationAlgorithm, 
    UpperBound>::VerificationUBkScan() {
  verfications_ = 0;}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
std::vector<lookup::LookupResultElement> 
    VerificationUBkScan<Label, VerificationAlgorithm, UpperBound>::execute_lookup(
    std::vector<node::Node<Label>>& trees_collection, 
    unsigned int query_tree_id, const double distance_threshold) {

  label::LabelDictionary<Label> ld;
  typename VerificationAlgorithm::AlgsCostModel cm(ld);

  VerificationAlgorithm verification_algorithm(cm);
  UpperBound upper_bound_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex tq;
  typename VerificationAlgorithm::AlgsTreeIndex tc;

  // Stores the result set.
  std::vector<lookup::LookupResultElement> result_set;
  // Used to store the computed distance value.
  double distance = std::numeric_limits<double>::infinity();
  double upper_bound = std::numeric_limits<double>::infinity();

  // Index query tree.
  node::index_tree(tq, trees_collection[query_tree_id], ld, cm);

  // Verify which candidate trees are part of the result set.
  for (unsigned int candidate_tree_id = 0; 
      candidate_tree_id < trees_collection.size(); candidate_tree_id++) {
    // Index candidate tree.
    node::index_tree(tc, trees_collection[candidate_tree_id], ld, cm);

    // Compute the upper bound between the query and candidate tree.
    upper_bound = upper_bound_algorithm.jedi_k(tq, tc, distance_threshold);
    if (upper_bound <= distance_threshold) {
      result_set.emplace_back(query_tree_id, candidate_tree_id, -1, upper_bound, -1);
    } else {
      verfications_++;
      // Compute the distance between the query and candidate tree.
      distance = verification_algorithm.jedi(tq, tc);
      if (distance <= distance_threshold) {
        result_set.emplace_back(query_tree_id, candidate_tree_id, -1, upper_bound, distance);
      }
    }
    // Sum up all number of subproblems
    sum_subproblem_counter_ += verification_algorithm.get_subproblem_count();
  }

  return result_set;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkScan<Label,
    VerificationAlgorithm, UpperBound>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkScan<Label,
    VerificationAlgorithm, UpperBound>::get_verification_count() const {
  return verfications_;
}

// ****** Without given upper bound.

template <typename Label, typename VerificationAlgorithm>
VerificationScan<Label, VerificationAlgorithm>::VerificationScan() {
  verfications_ = 0;
}

template <typename Label, typename VerificationAlgorithm>
std::vector<lookup::LookupResultElement> 
    VerificationScan<Label, VerificationAlgorithm>::execute_lookup(
    std::vector<node::Node<Label>>& trees_collection, 
    unsigned int query_tree_id, const double distance_threshold) {

  label::LabelDictionary<Label> ld;
  typename VerificationAlgorithm::AlgsCostModel cm(ld);

  VerificationAlgorithm verification_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex tq;
  typename VerificationAlgorithm::AlgsTreeIndex tc;

  // Stores the result set.
  std::vector<lookup::LookupResultElement> result_set;
  // Used to store the computed distance value.
  double distance = std::numeric_limits<double>::infinity();

  // Index query tree.
  node::index_tree(tq, trees_collection[query_tree_id], ld, cm);

  // Verify which candidate trees are part of the result set.
  for (unsigned int candidate_tree_id = 0; 
      candidate_tree_id < trees_collection.size(); candidate_tree_id++) {
    // Index candidate tree.
    node::index_tree(tc, trees_collection[candidate_tree_id], ld, cm);

    // Compute the distance between the query and candidate tree.
    verfications_++;
    distance = verification_algorithm.jedi(tq, tc);
    if (distance <= distance_threshold) {
      result_set.emplace_back(query_tree_id, candidate_tree_id, -1, -1, distance);
    }
    // Sum up all number of subproblems
    sum_subproblem_counter_ += verification_algorithm.get_subproblem_count();
  }

  return result_set;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationScan<Label,
    VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationScan<Label,
    VerificationAlgorithm>::get_verification_count() const {
  return verfications_;
}