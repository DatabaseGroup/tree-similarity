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

/// Contains the implementation of the index lookup algorithm.

#pragma once

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
VerificationUBkIndex<Label, VerificationAlgorithm, 
    UpperBound>::VerificationUBkIndex() {
  verfications_ = 0;
  candidates_ = 0;
  pre_candidates_ = 0;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
std::vector<lookup::LookupResultElement> 
    VerificationUBkIndex<Label, VerificationAlgorithm, UpperBound>::execute_lookup(
    std::vector<node::Node<Label>>& trees_collection, 
    std::vector<std::pair<int, std::vector<label_set_converter_index::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& size_setid_map, lookup::TwoStageInvertedList& index,
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
  double intersection = std::numeric_limits<double>::infinity();
  double lower_bound = std::numeric_limits<double>::infinity();
  double upper_bound = std::numeric_limits<double>::infinity();

  // Index query tree.
  node::index_tree(tq, trees_collection[query_tree_id], ld, cm);

  // Get candidates from index.
  std::unordered_set<long int> candidates;

  // If the query tree is less than the threshold, all trees from 
  // size 0 up to threshold + 1 have to be considered.
  if (tq.tree_size_ < distance_threshold + 1) {
    for (auto const& set : size_setid_map) {
      if (set.first > tq.tree_size_ + distance_threshold + 1) {
        break;
      }
      candidates.insert(set.second);
    }
  }

  // Lookup the index for each element in the prefix of the query tree.
  long int prefix = std::min((int) sets_collection[query_tree_id].second.size() - 1, (int) distance_threshold + 1);
  for (long int pos = 0; pos <= prefix; pos++) {
    index.lookup(sets_collection[query_tree_id].second[pos].id, 
        sets_collection[query_tree_id].second[pos].descendants, 
        sets_collection[query_tree_id].second[pos].ancestors, 
        sets_collection[query_tree_id].first,
        candidates, distance_threshold);
  }
  pre_candidates_ = candidates.size();
  candidates_ = pre_candidates_;

  // Verify which candidate trees are part of the result set.
  for (long int const& candidate_tree_id: candidates) {
    // Index candidate tree.
    node::index_tree(tc, trees_collection[candidate_tree_id], ld, cm);

    // Compute the lower bound between the query and candidate tree.
    // If the lower bound exceeds the threshold, put the pair immediately
    // to the result set without verification.
    intersection = node_lower_bound(sets_collection[query_tree_id].second, 
        sets_collection[candidate_tree_id].second, 0, 0, 0);
    // Node intersection LB: DPJED >= max(T1, T2) - T1 intersection T2.
    lower_bound = std::max(sets_collection[query_tree_id].first, 
        sets_collection[candidate_tree_id].first) - intersection;
    // Prune candidate if the lower bound exceeds the threshold.
    if (lower_bound > distance_threshold) {
      candidates_--;
    } else {
      // Compute the upper bound between the query and candidate tree.
      // If the upper bound is already less than the threshold, the pair is 
      // certainly in the result set.
      upper_bound = upper_bound_algorithm.jedi_k(tq, tc, distance_threshold);
      if (upper_bound <= distance_threshold) {
        result_set.emplace_back(query_tree_id, candidate_tree_id, lower_bound, upper_bound, -1);
      } else {
        verfications_++;
        // Verify the candidate pair in case that the bounds do not apply.
        distance = verification_algorithm.jedi(tq, tc);
        if (distance <= distance_threshold) {
          result_set.emplace_back(query_tree_id, candidate_tree_id, lower_bound, upper_bound, distance);
        }
      }
      // Sum up all number of subproblems
      sum_subproblem_counter_ += verification_algorithm.get_subproblem_count();
    }
  }

  return result_set;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
double VerificationUBkIndex<Label, VerificationAlgorithm, UpperBound>::node_lower_bound(
    std::vector<label_set_converter_index::LabelSetElement>& r, 
    std::vector<label_set_converter_index::LabelSetElement>& s, 
    double olap, int pr, int ps) {
  int size_r = r.size();
  int size_s = s.size();
  while (pr < size_r && ps < size_s) {
    if (r[pr].id == s[ps].id) {
      olap += 1;
      ++pr; ++ps;
    } else if (r[pr].id < s[ps].id) {
      ++pr;
    } else {
      ++ps;
    }
  }

  return olap;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkIndex<Label,
    VerificationAlgorithm, UpperBound>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkIndex<Label,
    VerificationAlgorithm, UpperBound>::get_verification_count() const {
  return verfications_;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkIndex<Label,
    VerificationAlgorithm, UpperBound>::get_candidates_count() const {
  return candidates_;
}

template <typename Label, typename VerificationAlgorithm, typename UpperBound>
long long int VerificationUBkIndex<Label,
    VerificationAlgorithm, UpperBound>::get_pre_candidates_count() const {
  return pre_candidates_;
}

// ****** Without given upper bound.

template <typename Label, typename VerificationAlgorithm>
VerificationIndex<Label, VerificationAlgorithm>::VerificationIndex() {
  verfications_ = 0;
  candidates_ = 0;
  pre_candidates_ = 0;
}

template <typename Label, typename VerificationAlgorithm>
std::vector<lookup::LookupResultElement> 
    VerificationIndex<Label, VerificationAlgorithm>::execute_lookup(
    std::vector<node::Node<Label>>& trees_collection, 
    std::vector<std::pair<int, std::vector<label_set_converter_index::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& size_setid_map, lookup::TwoStageInvertedList& index,
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
  double intersection = std::numeric_limits<double>::infinity();
  double lower_bound = std::numeric_limits<double>::infinity();

  // Index query tree.
  node::index_tree(tq, trees_collection[query_tree_id], ld, cm);

  // Get candidates from index.
  std::unordered_set<long int> candidates;

  // If the query tree is less than the threshold, all trees from 
  // size 0 up to threshold + 1 have to be considered.
  if (tq.tree_size_ < distance_threshold + 1) {
    for (auto const& set : size_setid_map) {
      if (set.first > tq.tree_size_ + distance_threshold + 1) {
        break;
      }
      candidates.insert(set.second);
    }
  }

  // Lookup the index for each element in the prefix of the query tree.
  long int prefix = std::min((int) sets_collection[query_tree_id].second.size() - 1, (int) distance_threshold + 1);
  for (long int pos = 0; pos <= prefix; pos++) {
    index.lookup(sets_collection[query_tree_id].second[pos].id, 
        sets_collection[query_tree_id].second[pos].descendants, 
        sets_collection[query_tree_id].second[pos].ancestors, 
        sets_collection[query_tree_id].first,
        candidates, distance_threshold);
  }
  pre_candidates_ = candidates.size();
  candidates_ = pre_candidates_;

  // Verify which candidate trees are part of the result set.
  for (long int const& candidate_tree_id: candidates) {
    // Index candidate tree.
    node::index_tree(tc, trees_collection[candidate_tree_id], ld, cm);

    // Compute the lower bound between the query and candidate tree.
    // If the lower bound exceeds the threshold, put the pair immediately
    // to the result set without verification.
    intersection = node_lower_bound(sets_collection[query_tree_id].second, 
        sets_collection[candidate_tree_id].second, 0, 0, 0);
    // Node intersection LB: DPJED >= max(T1, T2) - T1 intersection T2.
    lower_bound = std::max(sets_collection[query_tree_id].first, 
        sets_collection[candidate_tree_id].first) - intersection;
    // Prune candidate if the lower bound exceeds the threshold.
    if (lower_bound > distance_threshold) {
      candidates_--;
    } else {
      verfications_++;
      // Compute the distance between the query and candidate tree.
      distance = verification_algorithm.jedi(tq, tc);
      if (distance <= distance_threshold) {
        result_set.emplace_back(query_tree_id, candidate_tree_id, lower_bound, -1, distance);
      }
    }
    // Sum up all number of subproblems
    sum_subproblem_counter_ += verification_algorithm.get_subproblem_count();
  }

  return result_set;
}

template <typename Label, typename VerificationAlgorithm>
double VerificationIndex<Label, VerificationAlgorithm>::node_lower_bound(
    std::vector<label_set_converter_index::LabelSetElement>& r, 
    std::vector<label_set_converter_index::LabelSetElement>& s, 
    double olap, int pr, int ps) {
  int size_r = r.size();
  int size_s = s.size();
  while (pr < size_r && ps < size_s) {
    if (r[pr].id == s[ps].id) {
      olap += 1;
      ++pr; ++ps;
    } else if (r[pr].id < s[ps].id) {
      ++pr;
    } else {
      ++ps;
    }
  }

  return olap;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationIndex<Label,
    VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationIndex<Label,
    VerificationAlgorithm>::get_verification_count() const {
  return verfications_;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationIndex<Label,
    VerificationAlgorithm>::get_candidates_count() const {
  return candidates_;
}

template <typename Label, typename VerificationAlgorithm>
long long int VerificationIndex<Label,
    VerificationAlgorithm>::get_pre_candidates_count() const {
  return pre_candidates_;
}