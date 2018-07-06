// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file ted/apted.h
///
/// \details
/// Implements the state-of-the-art tree edit distance algorithm APTED+ by
/// Pawlik and Augsten [1,2,3,4].
///
/// [1] M.Pawlik and N.Augsten. RTED: A Robust Algorithm for the Tree Edit
///     Distance. PVLDB. 2011.
/// [2] M.Pawlik and N.Augsten. A Memory-Efficient Tree Edit Distance Algorithm.
///     DEXA. 2014.
/// [3] M. Pawlik and N. Augsten. Efficient Computation of the Tree Edit
///     Distance. ACM Transactions on Database Systems (TODS). 2015.
/// [4] M. Pawlik and N. Augsten. Tree edit distance: Robust and
///     memory-efficient. Information Systems. 2016.

#ifndef TREE_SIMILARITY_TED_APTED_H
#define TREE_SIMILARITY_TED_APTED_H

#include <vector>
#include <stack>
#include <memory>
#include "node.h"
#include "matrix.h"
#include <iostream>
#include <limits>
#include <cstdlib>

namespace ted {

template <typename Label, typename CostModel>
class APTEDNodeIndexer {
public:
  // Structure indices.
  /// NOTE: We use reference_wrapper for const references to nodes. For now, we
  /// decided not to use raw pointers. Smart pointers introduce ownership, but
  /// this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> preL_to_node_;
  std::vector<int> preL_to_size_;
  std::vector<int> preL_to_parent_;
  std::vector<std::vector<int>> preL_to_children_;
  std::vector<int> postL_to_lld_;
  std::vector<int> postR_to_rld_;
  std::vector<int> preL_to_ln_;
  std::vector<int> preR_to_ln_;
  std::vector<bool> preL_to_node_type_L_;
  std::vector<bool> preL_to_node_type_R_;
  // Traversal translation indices.
  std::vector<int> preL_to_preR_;
  std::vector<int> preR_to_preL_;
  std::vector<int> preL_to_postL_;
  std::vector<int> postL_to_preL_;
  std::vector<int> preL_to_postR_;
  std::vector<int> postR_to_preL_;
  // Cost indices.
  std::vector<int> preL_to_kr_sum_;
  std::vector<int> preL_to_rev_kr_sum_;
  std::vector<int> preL_to_desc_sum_;
  std::vector<int> preL_to_sumDelCost_;
  std::vector<int> preL_to_sumInsCost_;
  // Structure single-value variables.
  int tree_size_;
  int lchl_;
  int rchl_;
private:
  // Variables holding values modified at runtime while the algorithm executes.
  int current_node_;
  // Variables used temporarily while indexing.
  int size_tmp_;
  int desc_sizes_tmp_;
  int kr_sizes_sum_tmp_;
  int rev_kr_sizes_sum_tmp_;
  int preorder_tmp_;
  /// Cost model.
  const CostModel c_;
public:
  APTEDNodeIndexer(const node::Node<Label>& t);
private:
  int index_nodes(const node::Node<Label>& node, int postorder);
  void post_traversal_indexing();
public:
  int preL_to_lld(int preL);
  int preL_to_rld(int preL);
  const node::Node<Label>& postL_to_node(int postL);
  const node::Node<Label>& postR_to_node(int postR);
  // int get_size();
  bool is_leaf(int node);
  int get_current_node();
  void set_current_node(int preorder);
};

/// This class implements the APTED algorithm by Pawlik and Augsten.
template <typename Label, typename CostModel>
class APTED {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<int>& preL_to_size;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  APTED();
  /// Computes the tree edit distance between two trees.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \return Tree edit distance between t1 and t2.
  double apted_ted(const node::Node<Label>& t1, const node::Node<Label>& t2);
  double apted_ted(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2);
  /// For common signature in experiments only. Calls apted_ted.
  double apted_ted_k(const node::Node<Label>& t1, const node::Node<Label>& t2, const int k = 0);
  /// Verifies if the tree edit distance between trees t1 and t1 is smaller or
  /// equal than the similarity_threshold value.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param similarity_threshold Maximum value of the tree edit distance.
  /// \return Tree edit distance value if it smaller or equal than the
  ///         similarity_threshold, and std::numeric_limits<double>::infinity()
  ///         otherwise.
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double similarity_threshold);
  /// Creates a TestItems object and returns it (by value).
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  const unsigned long long int get_subproblem_count() const;
private:
  data_structures::Matrix<double> compute_opt_strategy_postL(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2);
  data_structures::Matrix<double> compute_opt_strategy_postR(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2);
  void ted_init(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2);
  double gted(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2);
  double spf1(APTEDNodeIndexer<Label, CostModel>& ni_1, int subtreeRootNode1, APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode2);
  
  double spfA(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int pathID, int pathType, bool treesSwapped);
  
  double spfL(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, bool treesSwapped);
  int computeKeyRoots(APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode, int pathID, std::vector<int>& keyRoots, int index);
  void treeEditDist(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int it1subtree, int it2subtree, data_structures::Matrix<double>& forestdist, bool treesSwapped);
  
  double spfR(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, bool treesSwapped);
  int computeRevKeyRoots(APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode, int pathID, std::vector<int>& revKeyRoots, int index);
  void revTreeEditDist(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int it1subtree, int it2subtree, data_structures::Matrix<double>& forestdist, bool treesSwapped);
  
  int get_strategy_path_type(int pathIDWithPathIDOffset, int pathIDOffset, int currentRootNodePreL, int currentSubtreeSize);
  void updateFnArray(int lnForNode, int node, int currentSubtreePreL);
  void updateFtArray(int lnForNode, int node);
// Member variables.
private:
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> delta_;
  // /// Matrix storing subforest distances.
  // data_structures::Matrix<double> s_;
  // /// Matrix storing subforest distances.
  // data_structures::Matrix<double> t_;
  /// Cost model.
  const CostModel c_;
  
  std::vector<double> q_;
  std::vector<int> fn_;
  std::vector<int> ft_;
  
  int input_size_1_;
  int input_size_2_;
  
  /// Subproblem counter - for experiments only. Counts the number of
  /// non-trivial values filled in fd_ matrix: subproblems where both forests
  /// are not empty (including storing first infinities outside e-strip), and
  /// last value computed in fd_.
  unsigned long long int subproblem_counter_;
};

// Implementation details.
#include "apted_impl.h"

}

#endif // TREE_SIMILARITY_TED_APTED_H
