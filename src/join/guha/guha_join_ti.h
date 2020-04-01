// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Thomas Huetter
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

/// \file join/guha/guha.h
///
/// \details
/// Implements partially the join algorithm by Guha [1,2].
/// The R-tree part from [2] is not included.
///
/// [1] Guha et al. Approximate XML joins. ACM SIGMOD. 2002.
/// [2] Guha et al. Integrating XML data sources using approximate joins. 
///     ACM TODS. 2006.

#pragma once

#include <vector>
#include <random>
#include "join_result_element.h"
#include "node.h"
#include "apted_tree_index.h"
#include "sed_tree_index.h"
#include "cted_tree_index.h"
#include <iostream>

namespace join {

template <typename Label, typename VerificationAlgorithm>
class GuhaJoinTI {
// Member functions.
public:
  /// Constructor.
  GuhaJoinTI();
  /// Implements the original RSB algorithm.
  ///
  /// TODO: Guha uses APTED, CTED, SED, and Touzet for verification. Parse
  ///       trees once and use a super TreeIndex.
  ///
  /// 1. Computes reference set.
  /// 2. Computes the exact TED from each tree to the reference set.
  /// 3. Applies metric lower and upper bounds to get candidates.
  /// 4. Refines further the candidates with SED and CTED.
  /// 5. Verifies the candidates with APTED.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param candidates Output vector to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  void execute_rsb_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold
  );
  /// Implements the original RSC algorithm.
  /// 1. Computes reference set.
  /// 2. Computes SED and CTED from each tree to the reference set.
  /// 3. Applies metric lower and upper bounds to get candidates.
  /// 4. Refines further the candidates with SED and CTED.
  /// 5. Verifies the candidates with APTED.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param candidates Output vector to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  void execute_rsc_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold
  );
  /// Computes TED from each tree in trees_collection to all in reference_set,
  /// and stores the values in ted_vectors.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param reference_set Reference set with tree IDs.
  /// \param ted_vectors Output vector.
  void compute_vectors(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<int>& reference_set,
    std::vector<std::vector<double>>& ted_vectors
  );
  /// Computes SED and CTED from each tree in trees_collection to all in
  /// reference_set, and stores the values in lb_vectors and ub_vectors,
  /// respectively.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param reference_set Reference set with tree IDs.
  /// \param lb_vectors Output vector for SED values.
  /// \param ub_vectors Output vector for CTED values.
  void compute_vectors(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<int>& reference_set,
    std::vector<std::vector<double>>& lb_vectors,
    std::vector<std::vector<double>>& ub_vectors
  );
  /// Retrieves the candidates based on metric lower and upper bounds using
  /// vectors with TED values.
  ///
  /// \param candidates Output vecot to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  /// \param ted_vectors Vectors with TED values.
  void retrieve_metric_candidates(
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold,
      std::vector<std::vector<double>>& ted_vectors
  );
  /// Retrieves the candidates based on metric lower and upper bounds using
  /// vectors with SED and CTED values.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param candidates Output vecot to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  /// \param lb_vectors Vectors with SED values.
  /// \param ub_vectors Vectors with CTED values.
  void retrieve_metric_candidates(
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold,
      std::vector<std::vector<double>>& lb_vectors,
      std::vector<std::vector<double>>& ub_vectors
  );
  /// Refines the join candidates by computing SED and CTED.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param candidates Output vecot to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  void retrieve_sc_candidates(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold
  );
  /// Verifies the jon candidates by computing APTED.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param candidates Output vecot to store candidates (pairs of tree IDs).
  /// \param join_result Output vector to stor partial join results (pairs of
  ///        tree IDs that passed an upper bound filter).
  /// \param distance_threshold Join similarity threshold.
  void verify_candidates(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result, 
      const double distance_threshold
  );
  /// Computes the reference set if the number of elements is not known.
  /// Implements the original method by Guha.
  ///
  /// NOTE: If too few clusters greater than 1 are found (the input may be too
  ///       small), `get_random_reference_set(trees_collection, 2)` is returned.
  ///
  /// NOTE: For details see comments in the implementation source code.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param distance_threshold Join similarity threshold.
  /// \return Reference set as a vector of tree IDs.
  std::vector<int> get_reference_set(
    std::vector<node::Node<Label>>& trees_collection,
    const double distance_threshold
  );
  /// Chooses k trees from trees_collection at random.
  /// 
  /// NOTE: It was the first and easiest way to get a reference set.
  ///
  /// \param trees_collection Input dataset to a join.
  /// \param k Size of reference set.
  /// \return Reference set as a vector of tree IDs.
  std::vector<int> get_random_reference_set(
    std::vector<node::Node<Label>>& trees_collection,
    int k
  );
  /// Computes the metric upper bound using either TED or CTED values.
  double u_t(std::vector<double>& v_1, std::vector<double>& v_2);
  /// Computes the metric upper bound using TED values.
  double l_t(std::vector<double>& v_1, std::vector<double>& v_2);
  /// Computes the metric upper bound using SED and CTED values.
  double l_t(std::vector<double>& lb_v_i, std::vector<double>& ub_v_i,
      std::vector<double>& lb_v_j, std::vector<double>& ub_v_j);
  /// Returns the number of candidates resulting from metric lower bound.
  long long int get_l_t_candidates() const;
  /// Returns the number of candidates resulting from SED lower bound.
  long long int get_sed_candidates() const;
  /// Returns the number of result set pairs comming from metric upper bound.
  long long int get_u_t_result_pairs() const;
  /// Returns the number of result set pairs comming from CTED upper bound.
  long long int get_cted_result_pairs() const;
private:
  /// LabelDictionary for using with LGMTreeIndex and VerificationAlgorithm.
  label::LabelDictionary<Label> ld_;
  /// Stores the number of candidates resulting from metric lower bound.
  long long int l_t_candidates_;
  /// Stores the number of candidates resulting from SED lower bound.
  long long int sed_candidates_;
  /// Stores the number of result set pairs comming from metric upper bound.
  long long int u_t_result_pairs_;
  /// Stores the number of result set pairs comming from CTED upper bound.
  long long int cted_result_pairs_;
};

// Implementation details.
#include "guha_join_ti_impl.h"

}
