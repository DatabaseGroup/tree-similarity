// The MIT License (MIT)
// Copyright (c) 2017 Thomas Huetter
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

/// \file join/tjoin/t_join.h
///
/// \details
/// Implements the TJoin tree similarity join. First, a tree is converted into a 
/// label set. Second, a candidate index is used to retrieve candidates by looking  
/// up a node label inverted list and applying the structural filter. Next, the 
/// resulting tree pairs, called candidates, have to be verified. Therefore, the 
/// greedy label guided mapping upper bound sends candidates to the result set 
/// without TED computation. Last, the remaining candidates are evaluated by 
/// Touzets algorithm.

#ifndef TREE_SIMILARITY_JOIN_TJOIN_T_JOIN_H
#define TREE_SIMILARITY_JOIN_TJOIN_T_JOIN_H

#include <vector>
#include <functional>
#include "join_result_element.h"
#include "node.h"
#include "candidate_index.h"
#include "label_set_converter.h"
#include "label_set_element.h"

namespace join {

template <typename Label, typename CostModel, typename VerificationAlgorithm>
class TJoin {
// Member functions.
public:
  /// Constructor.
  TJoin();
  /// Given a collection of trees, the candidates are retrieved by an 
  /// efficient and effective candidate index. Next, all candidates are 
  /// verified with the label guided mapping upper bound. Last, the 
  /// remaining candidates are evaluated by Touzets algorithm.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param sets_collection A vector holding the label sets of trees_collection.
  /// \param candidates A vector of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector with the join result.
  void execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold);
  /// A given collection of trees is converted into a collection of label sets.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \return A vector containing the according label sets of the input trees
  ///         in trees_collection.
  void convert_trees_to_sets(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection);
  /// Uses the TJoin candidate index to retrieve candidates that are further 
  /// verified.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param candidates A vector of candidate tree pairs.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector containing pairs of trees ids (candidates) that are 
  ///         considered candidates.
  void retrieve_candidates(
      std::vector<std::pair<unsigned int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      const double distance_threshold);
  /// Verify for each candidate pair in candidates if the tree edit
  /// distance is below a certain distance_threshold.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param candidates A vector of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector containing the join result.
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
  /// Number of different labels in the trees.
  unsigned long long int number_of_labels_;
  /// Number of precandidates.
  unsigned long long int pre_candidates_;
  /// Number of subproblrems encoutered in the verification step.
  unsigned long long int sum_subproblem_counter_;
  /// Number of precandidates.
  unsigned long long int il_lookups_;
};

// Implementation details.
#include "t_join_impl.h"

}

#endif // TREE_SIMILARITY_JOIN_TJOIN_T_JOIN_H
