// The MIT License (MIT)
// Copyright (c) 2018 Mateusz Pawlik
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

/// \file ted_ub/cted_ub.h
///
/// \details
/// Implements the constrained tree edit distance algorithm by ...
/// NOTE Unit cost only.
/// TODO

#ifndef TREE_SIMILARITY_TED_UB_CTED_UB_H
#define TREE_SIMILARITY_TED_UB_CTED_UB_H

#include <vector>
#include <iostream>
#include <memory>
#include <cmath>
#include "node.h"
#include "matrix.h"
#include "label_dictionary.h"

namespace ted_ub {

template <typename Label, typename CostModel>
class ConstrainedUB {
// Member struct.
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  ConstrainedUB();
  
  /// Computes the tree edit distance between two trees.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \return Tree edit distance value.
  double cted_ub_ted(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// For common signature in experiments only. Calls zhang_shasha_ted.
  double cted_ub_ted_k(const node::Node<Label>& t1, const node::Node<Label>& t2, const int k = 0);
  /// Verifies if the tree edit distance between trees t1 and t1 is smaller or
  /// equal than the distance_threshold value.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param distance_threshold Maximum value of the tree edit distance.
  /// \return Tree edit distance value if it smaller or equal than the
  ///         distance_threshold, and std::numeric_limits<double>::infinity()
  ///         otherwise.
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double distance_threshold);
  /// Resets and initialises algorithm's internal data structures and constants.
  /// Has to be called before computing the distance.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  void init(const node::Node<Label>& t1, const node::Node<Label>& t2);
// Member variables.
// Member variables.
private:
  /// The size of the source tree.
  int t1_input_size_;
  /// The size of the destination tree.
  int t2_input_size_;
  /// Stores the postorder id of the parent for each node of the source
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_parent_;
  /// Stores the postorder id of the parent for each node of the destination
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_parent_;
  /// Stores Label to int id translation of all labels in both input trees.
  label::LabelDictionary<Label> dict_;
  /// Stores the label id from dict_ for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_label_;
  /// See t1_label_.
  std::vector<int> t2_label_;
  /// TODO
  std::vector<std::vector<int>> t1_children_;
  /// TODO
  std::vector<std::vector<int>> t2_children_;
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> dt_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> df_;
  /// TODO
  data_structures::Matrix<double> e_;
  /// Cost model.
  const CostModel c_;
  /// Subproblem counter - for experiments only. Counts the number of
  /// non-trivial values filled in fd_ matrix: subproblems where both forests
  /// are not empty.
  unsigned long long int subproblem_counter;
// Member functions.
private:
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// TODO
  void index_nodes(const node::Node<Label>& root,
                   std::vector<int>& parent,
                   std::vector<std::vector<int>>& children,
                   std::vector<int>& label);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// TODO
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<int>& parent,
                             std::vector<std::vector<int>>& children,
                             std::vector<int>& label,
                             int& start_postorder);
};

// Implementation details.
#include "cted_ub_impl.h"

}

#endif // TREE_SIMILARITY_TED_UB_CTED_UB_H
