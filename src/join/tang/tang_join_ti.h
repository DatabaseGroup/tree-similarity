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

/// \file join/tang/tang_join.h
///
/// \details
/// Implements the tree similarity join algorithm proposed by Tang et. al in 
/// "Scaling Similarity Joins over Tree-Structured Data".

#pragma once

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "../join_result_element.h"
#include "../../node/node.h"
#include "binary_tree_converter.h"



namespace join {

/// Hashes pairs of integers.
struct hashintegerpair {
public:
  size_t operator()(const std::pair<int, int>& a) const {
    return std::hash<int>()(a.first) ^ 
           std::hash<int>()(a.second);
  }
};

template <typename Label, typename VerificationAlgorithm>
class TangJoinTI {
// Member functions.
public:
  /// Constructor.
  TangJoinTI();
  /// Given a collection of trees, the join candidates are computed.
  /// First, trees are transformed into binary trees. Then, the binary trees
  /// are splitted into subgraphs. Next, candidates are retrieved from a 
  /// two-layer index based on the top twig labels of the subgraphs the 
  /// postorder IDs of the nodes in a tree. Second the candidate pairs are 
  /// verified using a verification algorithm. The join result are stored 
  /// in result_collection_.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param binary_trees_collection A vector holding the binary trees of 
  ///                                trees_collection.
  /// \param candidates An unordered set of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector with the join result.
  void execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<node::BinaryNode<Label>>& binary_trees_collection,
      std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold);
  /// A given collection of trees is converted into a collection of sets.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param binary_trees_collection A vector holding the binary trees of 
  ///                                trees_collection.
  /// \return A vector containing all binary representations of the input trees.
  void convert_trees_to_binary_trees(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<node::BinaryNode<Label>>& binary_trees_collection);
  /// A given collection of binary trees candidates are retrieved from the two 
  /// layer index introduced by Tang et al.
  ///
  /// \param binary_trees_collection A vector holding the binary trees of 
  ///                                trees_collection.
  /// \param candidates An unordered set of candidate tree pairs.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return An unordered map containing pairs of trees ids (candidates) 
  ///         that are considered candidates.
  void retrieve_candidates(
      std::vector<node::BinaryNode<Label>>& binary_trees_collection,
      std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
      const double distance_threshold);
  /// Verify for each candidate pair in candidates if the tree edit
  /// distance is below a certain distance_threshold.
  ///
  /// \param trees_collection A vector holding an input collection of trees.
  /// \param candidates An unordered set of candidate tree pairs.
  /// \param join_result A vector of result tree pairs and their TED value.
  /// \param distance_threshold The maximum number of edit operations that
  ///                           differs two trees in the join's result set.
  /// \return A vector containing the join result.
  void verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
    std::vector<join::JoinResultElement>& join_result, 
    const double distance_threshold);
  /// Returns the number of precandidates.
  ///
  /// \return The number of precandidates.
  long long int get_number_of_pre_candidates() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  long long int get_subproblem_count() const;
  /// Returns the number of inverted list lookups.
  ///
  /// \return The number of inverted list lookups.
  long long int get_number_of_il_lookups() const;
// Member variables.
private:
  /// String that is used as empty label. Therefore, no label should contain this string. 
  std::string empty_string_ = "!§$%&/()=?";
  /// Number of precandidates.
  long long int pre_candidates_;
  /// Number of subproblrems encoutered in the verification step.
  long long int sum_subproblem_counter_;
  /// Number of precandidates.
  long long int il_lookups_;
  /// Inverted list: tree size -> (postorderid -> (top-twig-label -> tree number)).
  std::unordered_map<int, std::unordered_map<int, 
      std::unordered_map<std::string, std::vector<std::pair<int, node::BinaryNode<Label>*>>>>> inverted_list_;
// Member functions.
private:
  /// Returns tree IDs of trees that share at least one common subgraph.
  ///
  /// \param curr_node Current node of the current tree.
  /// \param tree_size Tree size of the current tree.
  /// \param curr_node_postorder_id postorder ID of the current node.
  /// \param S Vector that holds the tree IDs of all trees that share a common subgraph.
  void get_subgraphs(node::BinaryNode<Label>* curr_node, int tree_size, 
    int curr_node_postorder_id, std::vector<int>& S);

  /// Returns true if the subgraphs starting at left_tree_node and right_tree_node
  /// are identical.
  ///
  /// \param left_tree_node Root of subgraph in the left hand tree.
  /// \param right_tree_node Root of subgraph in the righ hand tree.
  /// 
  /// \return True if subgraphs are identical.
  bool check_subgraphs(node::BinaryNode<Label>* left_tree_node, node::BinaryNode<Label>* right_tree_node);

  /// Binary search to find the maximum value of gamma, s.t. the tree is 
  /// (delta, gamma)-partitionable. Complexity is O(|T_B| * log(|T_B| / delta)).
  ///
  /// \param curr_node Current node of the current tree.
  /// \param tree_size Tree size of the current tree.
  /// \param delta Partitioning value, i.e. tree is partitioned in delta subtrees.
  ///
  /// \return The maximum value of gamma.
  int max_min_size(node::BinaryNode<Label>* curr_node, int tree_size, 
    int delta);

  /// Binary search to find the maximum value of gamma, s.t. the tree is 
  /// (delta, gamma)-partitionable. Complexity is O(|T_B| * log(|T_B| / delta)).
  ///
  /// \param curr_node Current node of the current tree.
  /// \param delta Partitioning value, i.e. tree is partitioned in delta subtrees.
  /// \param gamma Each of each subgraph is at least gamma.
  ///
  /// \return The maximum value of gamma.
  bool partitionable(node::BinaryNode<Label>* curr_node, int delta, 
    int gamma);

  /// Test whether the current tree is (delta, gamma)-partitionable.
  ///
  /// \param curr_node Current node of the current tree.
  /// \param delta Partitioning value, i.e. tree is partitioned in delta subtrees.
  /// \param gamma Each of each subgraph is at least gamma.
  /// \param nr_of_subgraphs_found Number of common subgraphs found so far.
  ///
  /// \return The maximum value of gamma.
  bool recursive_partitionable(node::BinaryNode<Label>* curr_node, int delta, 
    int gamma, int& nr_of_subgraphs_found);

  bool update_inverted_list(node::BinaryNode<Label>* curr_node, int delta, 
    int gamma, int tree_size, int curr_tree_id,
    int& postorder_id, int& subgraph_id, const double distance_threshold);
};

// Implementation details.
#include "tang_join_ti_impl.h"

}
