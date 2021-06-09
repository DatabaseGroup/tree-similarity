// The MIT License (MIT)
// Copyright (c) 2020 Thomas Huetter.
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
#include <iostream>
#include <memory>
#include <cmath>
#include "node.h"
#include "matrix.h"
#include "label_dictionary.h"
#include "tree_indexer.h"
#include "ted_algorithm.h"

namespace ted_ub {

/**
 * Implements Constrained Tree Edit Distance based on Wang 2008.
 * Runtime: O(|T1||T2|), Memory: O(log(|T1|)|T2|).
 *
 * NOTE: only node::TreeIndexCTED or its superset can be used with CTEDTreeIndex.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexCTED>
class MODPJEDIndex : public ted::TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);

  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted_k(const TreeIndex& t1, const TreeIndex& t2, const double threshold);

private:
  // Costs to delete T1[i].
  std::vector<double> del_t1_subtree_;
  // Costs to delete F1[i].
  std::vector<double> del_f1_subtree_;
  // Costs to insert T2[j].
  std::vector<double> ins_t2_subtree_;
  // Costs to insert F2[j].
  std::vector<double> ins_f2_subtree_;
  // Holds deletion line for the edit distance computation.
  std::vector<double> e_init_;
  // Tree distance matrix, initialized to infinity.
  data_structures::Matrix<double> dt_;
  // Forest distance matrix, initialized to infinity.
  data_structures::Matrix<double> df_;
  // Holds line c(s-1)(t) for the edit distance computation.
  data_structures::Matrix<double> e0_;
  // Holds line c(s)(t) for the edit distance computation.
  data_structures::Matrix<double> e_;
  // Iteratively compute forest deletion.
  data_structures::Matrix<double> del_forest_;
  // Holds line c(s)(t) for the edit distance computation.
  data_structures::Matrix<double> del_tree_;
  // Holds the distance matrix line of the favourable child.
  data_structures::Matrix<double> fav_child_dt_;
};

// Implementation details.
#include "modpjed_index_impl.h"

}
