// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter.
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

/// \file json/jofilter_index.h
///
/// \details
/// Contains the declaration of the JEDI-Order class. Computes our JOFilter. 
/// Complexity: O(|T1|*tau) time and O(log(|T1|)*tau + |T2|) memory.

#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <cmath>
#include "../node/node.h"
#include "../data_structures/matrix.h"
#include "../label/label_dictionary.h"
#include "../node/tree_indexer.h"
#include "jedi_algorithm.h"

namespace json {

template <typename CostModel, typename TreeIndex = node::TreeIndexJSON>
class JOFilterTreeIndex : public json::JEDIAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using json::JEDIAlgorithm<CostModel, TreeIndex>::JEDIAlgorithm;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::jedi;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::jedi_k;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements jedi function from the JEDIAlgorithm<CostModel, TreeIndex> 
  /// class.
  double jedi(const TreeIndex& t1, const TreeIndex& t2);

  /// Implements ted function from the JEDIAlgorithm<CostModel, TreeIndex> class 
  /// leveraging a given threshold.
  double jedi_k(const TreeIndex& t1, const TreeIndex& t2, const double threshold);

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
#include "jofilter_index_impl.h"

}
