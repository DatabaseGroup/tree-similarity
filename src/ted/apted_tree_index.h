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

#pragma once

#include <vector>
#include <stack>
#include <memory>
#include "node.h"
#include "matrix.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include "tree_indexer.h"
#include "ted_algorithm.h"
#include <iostream>

namespace ted {

/**
 * Implements the state-of-the-art tree edit distance algorithm APTED+ by
 * Pawlik and Augsten [1,2,3,4].
 *
 * [1] M.Pawlik and N.Augsten. RTED: A Robust Algorithm for the Tree Edit
 *     Distance. PVLDB. 2011.
 * [2] M.Pawlik and N.Augsten. A Memory-Efficient Tree Edit Distance Algorithm.
 *     DEXA. 2014.
 * [3] M. Pawlik and N. Augsten. Efficient Computation of the Tree Edit
 *     Distance. ACM Transactions on Database Systems (TODS). 2015.
 * [4] M. Pawlik and N. Augsten. Tree edit distance: Robust and
 *     memory-efficient. Information Systems. 2016.
 *
 * NOTE: only node::TreeIndexAPTED can be used with APTED.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexAPTED>
class APTEDTreeIndex : public TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using TEDAlgorithm<CostModel, TreeIndex>::c_;
  using TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using TEDAlgorithm<CostModel, TreeIndex>::ted;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);


private:
  data_structures::Matrix<double> compute_opt_strategy_postL(const TreeIndex& t1, const TreeIndex& t2);
  data_structures::Matrix<double> compute_opt_strategy_postR(const TreeIndex& t1, const TreeIndex& t2);
  void ted_init(const TreeIndex& t1, const TreeIndex& t2);
  double gted(const TreeIndex& t1, int t1_current_subtree,
      const TreeIndex& t2, int t2_current_subtree);
  
  double spf1(const TreeIndex& t1, int subtreeRootNode1, const TreeIndex& t2,
      int subtreeRootNode2);
  
  double spfA(const TreeIndex& t1, int t1_current_subtree,
      const TreeIndex& t2, int t2_current_subtree, int pathID,
      int pathType, bool treesSwapped);
  
  double spfL(const TreeIndex& t1, int t1_current_subtree,
      const TreeIndex& t2, int t2_current_subtree, bool treesSwapped);
  int computeKeyRoots(const TreeIndex& t2, int subtreeRootNode, int pathID,
      std::vector<int>& keyRoots, int index);
  void treeEditDist(const TreeIndex& t1, const TreeIndex& t2, int it1subtree,
      int it2subtree, data_structures::Matrix<double>& forestdist,
      bool treesSwapped);
  
  double spfR(const TreeIndex& t1, int t1_current_subtree,
      const TreeIndex& t2, int t2_current_subtree, bool treesSwapped);
  int computeRevKeyRoots(const TreeIndex& t2, int subtreeRootNode, int pathID,
      std::vector<int>& revKeyRoots, int index);
  void revTreeEditDist(const TreeIndex& t1, const TreeIndex& t2, int it1subtree,
      int it2subtree, data_structures::Matrix<double>& forestdist,
      bool treesSwapped);
  
  int get_strategy_path_type(int pathIDWithPathIDOffset,
      int pathIDOffset, int currentRootNodePreL,
      int currentSubtreeSize);
  void updateFnArray(int lnForNode, int node,
      int currentSubtreePreL);
  void updateFtArray(int lnForNode, int node);

  /// Matrix storing subtree distances.
  data_structures::Matrix<double> delta_;
  // /// Matrix storing subforest distances.
  // data_structures::Matrix<double> s_;
  // /// Matrix storing subforest distances.
  // data_structures::Matrix<double> t_;
  
  std::vector<double> q_;

  // TODO: I don't really remember what are these arrays exactly for.
  //       Java documentation says `Do not use it [1, Section 8.4]`.
  std::vector<int> fn_;
  std::vector<int> ft_;

};

// Implementation details.
#include "apted_tree_index_impl.h"

}
