// The MIT License (MIT)
// Copyright (c) 2020 Thomas Huetter
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

/// \file ted_ub/dpjed_index.h
///
/// \details
/// Contains the declaration of the document preservering JSON edit distance 
/// class.

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

namespace dpjed_ub {

/**
 * Implements Document Preserving JSON Edit Distance (DPJED).
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexDPJED>
class DPJEDTreeIndex : public ted::TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements ted function from the DPJEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);

// Member variables.
private:
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> dt_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> df_;
  /// TODO
  data_structures::Matrix<double> e_;

// Member functions.
private:
  /// Print a given cost matrix.
  ///
  /// \param cost_matrix Cost matrix.
  void print_matrix(std::vector<std::vector<long long int> >& cost_matrix);

  /// Given a cost matrix with zeros in n many lines/rows it finds the minimal 
  /// mapping among the elements in the matrix.
  ///
  /// \param cost_matrix Cost matrix.
  /// \param r Cost matrix.
  /// \param rows Number of rows of the cost matrix.
  /// \param cols Number of cols of the cost matrix.
  /// \param assignment Mapping of a row to a column.
  /// \param assigned_cols Remember all columns that are already assigned.
  /// \return True in case there is a minimal mapping, Flase otherwise.
  bool find_mapping(std::vector<std::vector<long long int> >& cost_matrix, 
      long long int r, long long int rows, long long int cols, 
      std::unordered_map<long long int, long long int>& assignment, 
      std::vector<bool>& assigned_cols);

  /// Compute the minimum mapping for a given cost matrix.
  ///
  /// \param cost_matrix Cost matrix.
  /// \return Returns the cost of the minimal mapping.
  long long int hungarian_algorithm(
      std::vector<std::vector<long long int> > cost_matrix);
};

// Implementation details.
#include "dpjed_index_impl.h"

}
