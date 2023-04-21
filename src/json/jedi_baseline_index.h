// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter
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

/// \file json/jedi_baseline_index.h
///
/// \details
/// Contains the declaration of the JSON edit distance class. Based on a 
/// combination of Kaizhong Zhang. “Algorithms for the constrained editing 
/// distance between ordered labeled trees and related problems”. In: Pattern 
/// Recognition (1995) and Kaizhong Zhang. “A constrained edit distance between 
/// unordered labeled trees”. In: Algorithmica 15.3 (1996). Baseline algorithm 
/// to QuickJEDI.

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

/**
 * Implements the JSON Edit Distance (JEDI).
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexJSON>
class JEDIBaselineTreeIndex : public json::JEDIAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using json::JEDIAlgorithm<CostModel, TreeIndex>::JEDIAlgorithm;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::jedi;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::jedi_k;
  using json::JEDIAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements ted function from the JEDIAlgorithm<CostModel, TreeIndex> class.
  double jedi(const TreeIndex& t1, const TreeIndex& t2);

  /// Implements ted function from the JEDIAlgorithm<CostModel, TreeIndex> class 
  /// leveraging a given threshold.
  double jedi_k(const TreeIndex& t1, const TreeIndex& t2, const double threshold);

// Member variables.
private:
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> dt_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> df_;
  /// Matrix storing string edit distance costs.
  data_structures::Matrix<double> e_;

// Member functions.
private:
  /// Print a given cost matrix.
  ///
  /// \param cost_matrix Cost matrix.
  void print_matrix(std::vector<std::vector<double> >& cost_matrix);

  /// Given a cost matrix with zeros in n many lines/rows it finds the minimal 
  /// mapping among the elements in the matrix.
  ///
  /// \param cost_matrix Cost matrix.
  /// \param matrix_size Used size of the cost matrix for current iteration.
  /// \param r Cost matrix.
  /// \param rows Number of rows of the cost matrix.
  /// \param cols Number of cols of the cost matrix.
  /// \param assignment Mapping of a row to a column.
  /// \param assigned_cols Remember all columns that are already assigned.
  /// \return True in case there is a minimal mapping, Flase otherwise.
  bool find_mapping(std::vector<std::vector<double> >& cost_matrix, 
      unsigned long& matrix_size, double r, int rows, 
      int cols, std::unordered_map<double, double>& assignment, 
      std::vector<bool>& assigned_cols);

  /// Compute the minimum mapping for a given cost matrix.
  ///
  /// \param cost_matrix Cost matrix.
  /// \return Returns the cost of the minimal mapping.
  double hungarian_algorithm(
      std::vector<std::vector<double> > cost_matrix,
      unsigned long& matrix_size);


  /// For each row of the cost matrix, find the smallest element and subtract
  /// it from every element in its row.  When finished, Go to Step 2.
  double execute_hungarian(
      std::vector<std::vector<double> >& cost_matrix,
      unsigned long& matrix_size);

  /// For each row of the cost matrix, find the smallest element and subtract
  /// it from every element in its row.  When finished, Go to Step 2.
  void step_one(std::vector<std::vector<double> >& cost_matrix,
      unsigned long& matrix_size, int& step);

  /// Find a zero (Z) in the resulting matrix.  If there is no starred 
  /// zero in its row or column, star Z. Repeat for each element in the 
  /// matrix. Go to Step 3.
  void step_two(std::vector<std::vector<double> >& cost_matrix,
      std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, std::vector<double>& row_cover,
      std::vector<double>& col_cover, int& step);

  /// Cover each column containing a starred zero.  If K columns are covered, 
  /// the starred zeros describe a complete set of unique assignments.  In this 
  /// case, Go to DONE, otherwise, Go to Step 4.
  void step_three(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, std::vector<double>& col_cover, int& step);

  /// Methods to support step 4
  void find_a_zero(std::vector<std::vector<double> >& cost_matrix,
      unsigned long& matrix_size, 
      std::vector<double>& row_cover,
      std::vector<double>& col_cover,
      int& row, int& col);

  bool star_in_row(std::vector<std::vector<double> >& mask_matrix, 
      unsigned long& matrix_size, int row);

  void find_star_in_row(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, int row, int& col);

  /// Find a noncovered zero and prime it.  If there is no starred zero 
  /// in the row containing this primed zero, Go to Step 5.  Otherwise, 
  /// cover this row and uncover the column containing the starred zero. 
  /// Continue in this manner until there are no uncovered zeros left. 
  /// Save the smallest uncovered value and Go to Step 6.
  void step_four(std::vector<std::vector<double> >& cost_matrix,
    std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, 
      std::vector<double>& row_cover,
      std::vector<double>& col_cover,
      double& path_row_0, double& path_col_0, 
      int& step);

  /// Methods to support step 5.
  void find_star_in_col(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, int& row, int col);

  void find_prime_in_row(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, int row, int& col);

  void augment_path(std::vector<std::vector<double> >& mask_matrix,
      std::vector<std::vector<double> >& path);

  void clear_covers(unsigned long& matrix_size, std::vector<double>& row_cover,
      std::vector<double>& col_cover);

  void erase_primes(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size);

  /// Construct a series of alternating primed and starred zeros as follows.  
  /// Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote 
  /// the starred zero in the column of Z0 (if any). Let Z2 denote the primed 
  /// zero in the row of Z1 (there will always be one).  Continue until the  
  /// series terminates at a primed zero that has no starred zero in its column.  
  /// Unstar each starred zero of the series, star each primed zero of the ,
  /// series erase all primes and uncover every line in the matrix.  Return to 
  /// Step 3.
  void step_five(std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size,
      std::vector<double>& row_cover,
      std::vector<double>& col_cover, 
      double& path_row_0, double& path_col_0,
      int& step);

  //methods to support step 6
  void find_smallest(std::vector<std::vector<double> >& cost_matrix,
      unsigned long& matrix_size,
      std::vector<double>& row_cover,
      std::vector<double>& col_cover, double& min_val);

  /// Add the value found in Step 4 to every element of each covered row, and 
  /// subtract it from every element of each uncovered column.  Return to Step 4  
  /// without altering any stars, primes, or covered lines.
  void step_six(std::vector<std::vector<double> >& cost_matrix,
      unsigned long& matrix_size,
      std::vector<double>& row_cover,
      std::vector<double>& col_cover, int& step);

  /// Compute the cost of the calculated mapping.
  void step_seven(std::vector<std::vector<double> >& cost_matrix,
      std::vector<std::vector<double> >& mask_matrix,
      unsigned long& matrix_size, double& costs);

};

// Implementation details.
#include "jedi_baseline_index_impl.h"

}
