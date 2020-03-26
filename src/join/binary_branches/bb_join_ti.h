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

/// \file join/binary_branches/bb_join.h
///
/// \details
/// Implements the BBJoin tree similarity join. First, a tree is converted into 
/// binary branches. Second, a candidate index is used to retrieve candidates by 
/// applying the binary branch lower bound by Yang et al. Next, the resulting 
/// tree pairs, called candidates, have to be verified. 

#pragma once

#include <vector>
#include <functional>
#include "join_result_element.h"
#include "node.h"
#include "greedy_ub.h"
#include "bb_candidate_index.h"
#include "bin_branch_histogram_converter.h"

namespace join {

template <typename Label, typename VerificationAlgorithm>
class BBJoinTI {
// Member functions.
public:
  /// Constructor.
  BBJoinTI();
  /// Given a collection of trees, the candidates are retrieved by an 
  /// efficient and effective candidate index. Next, all candidates are 
  /// verified with the label guided mapping upper bound. Last, the 
  /// remaining candidates are evaluated by Touzets algorithm.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param binary_branch_collection A vector containing the according 
  ///                                    binary branch histograms of the input 
  ///                                    trees in trees_collection.
  /// \param candidates A vector of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector with the join result.
  void execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, std::unordered_map<unsigned int, unsigned int>>>& histogram_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold);
  /// A given collection of trees is converted into a collection of binary branch historgrams.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \return A vector containing the according binary branch histograms of the input trees
  ///         in trees_collection.
  void convert_trees_to_histograms(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, std::unordered_map<unsigned int, unsigned int>>>& histogram_collection);
  /// Uses the BBJoin candidate index to retrieve candidates that are further 
  /// verified.
  ///
  /// \param binary_branch_collection A vector containing the according 
  ///                                    binary branch histograms of the input 
  ///                                    trees in trees_collection.
  /// \param candidates A vector of candidate tree pairs.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector containing pairs of trees ids (candidates) that are 
  ///         considered candidates.
  void retrieve_candidates(
      std::vector<std::pair<unsigned int, std::unordered_map<unsigned int, unsigned int>>>& histogram_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      const double distance_threshold);
  /// Uses the label guided mapping upper bound (lgm) to send candidates to 
  /// the result set without verification.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param candidates A vector of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  void verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result, 
    const double distance_threshold);
  /// Returns the number of precandidates.
  ///
  /// \return The number of precandidates.
  const unsigned long long int get_number_of_pre_candidates() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  const unsigned long long int get_subproblem_count() const;
  /// Returns the number of inverted list lookups.
  ///
  /// \return The number of inverted list lookups.
  const unsigned long long int get_number_of_il_lookups() const;
// Member variables.
private:
  /// Inverted list size.
  unsigned long long int il_size_;
  /// Number of precandidates.
  unsigned long long int pre_candidates_;
  /// Number of subproblrems encoutered in the verification step.
  unsigned long long int sum_subproblem_counter_;
  /// Number of precandidates.
  unsigned long long int il_lookups_;
};

// Implementation details.
#include "bb_join_ti_impl.h"

}
