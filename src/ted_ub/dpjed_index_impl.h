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

/// \file ted_ub/dpjed_index_impl.h
///
/// \details
/// Contains the implementation of the document preservering JSON edit distance 
/// class.


#pragma once

template <typename CostModel, typename TreeIndex>
double DPJEDTreeIndex<CostModel, TreeIndex>::ted(const TreeIndex& t1, const TreeIndex& t2) {

  // std::cout << " ***** DECLARATION ***** " << std::endl;

  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  int t1_input_size = t1.tree_size_;
  int t2_input_size = t2.tree_size_;
  
  // Initialise distance matrices.
  
  // TODO: Verify if a move assignment operator is used here.
  dt_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  df_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  // e_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());
  // e_.fill_with(std::numeric_limits<double>::infinity());
  
  // std::vector<double> dt2(t2_input_size + 1);
  // std::vector<double> df2(t2_input_size + 1);
  
  // double a = -1;
  // double b = -1;
  // double c = -1;
  
  // std::cout << " ***** INIT ***** " << std::endl;
  
  dt_.at(0, 0) = 0;
  df_.at(0, 0) = 0;
  // Deletion initialization.
  for (int i = 1; i <= t1_input_size; ++i) {
    df_.at(i, 0) = 0;
    for (unsigned int k = 1; k <= t1.postl_to_children_[i-1].size(); ++k) {
      df_.at(i, 0) += dt_.at(t1.postl_to_children_[i-1][k-1], 0);
    }
    dt_.at(i, 0) = df_.at(i, 0) + c_.del(t1.postl_to_label_id_[i - 1]);
  }
  // Insertion initialization.
  for (int j = 1; j <= t2_input_size; ++j) {
    df_.at(0, j) = 0;
    for (unsigned int k = 1; k <= t2.postl_to_children_[j-1].size(); ++k) {
      df_.at(0, j) += dt_.at(0, t2.postl_to_children_[j-1][k-1]);
    }
    dt_.at(0, j) = df_.at(0, j) + c_.ins(t2.postl_to_label_id_[j - 1]);
  }

  double min_for_ins = std::numeric_limits<double>::infinity();
  double min_tree_ins = std::numeric_limits<double>::infinity();
  double min_for_del = std::numeric_limits<double>::infinity();
  double min_tree_del = std::numeric_limits<double>::infinity();
  double min_for_ren = std::numeric_limits<double>::infinity();
  double min_tree_ren = std::numeric_limits<double>::infinity();


  for (int i = 1; i <= t1_input_size; ++i) {
    for (int j = 1; j <= t2_input_size; ++j) {
      // Cost for deletion in forest.
      min_for_del = std::numeric_limits<double>::infinity();
      min_tree_del = std::numeric_limits<double>::infinity();
      for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
        min_for_del = std::min(min_for_del,
            (df_.at(i, t2.postl_to_children_[j-1][t-1]) - 
             df_.at(0, t2.postl_to_children_[j-1][t-1])));
        min_tree_del = std::min(min_tree_del,
            (dt_.at(i, t2.postl_to_children_[j-1][t-1]) - 
             dt_.at(0, t2.postl_to_children_[j-1][t-1])));
      }
      min_for_del += df_.at(0, j);
      min_tree_del += dt_.at(0, j);
      
      // std::cout << " ******** 1 " << t1.postl_to_children_[i-1].size() << std::endl;

      // Cost for insertion in forest.
      min_for_ins = std::numeric_limits<double>::infinity();
      min_tree_ins = std::numeric_limits<double>::infinity();
      for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
        min_for_ins = std::min(min_for_ins, 
            (df_.at(t1.postl_to_children_[i-1][s-1], j) - 
             df_.at(t1.postl_to_children_[i-1][s-1], 0)));
        min_tree_ins = std::min(min_tree_ins, 
            (dt_.at(t1.postl_to_children_[i-1][s-1], j) - 
             dt_.at(t1.postl_to_children_[i-1][s-1], 0)));
      }
      min_for_ins += df_.at(i, 0);
      min_tree_ins += dt_.at(i, 0);

      // std::cout << " ******** 2 " << std::endl;

      // Cost for minimal mapping between trees in forest.
      min_for_ren = 0;
      min_tree_ren = 0;

      if (t2.postl_to_children_[j-1].size() > 1 && t1.postl_to_children_[i-1].size() > 1) {
        // std::vector<long long int> cost_matrix_line (t2.postl_to_children_[j-1].size(), 1);
        // std::cout << "rows: " << t1.postl_to_children_[i-1].size() << ", cols: " << t2.postl_to_children_[j-1].size() << std::endl;

        long long int matrix_size = t1.postl_to_children_[i-1].size() + t2.postl_to_children_[j-1].size();
        std::vector<std::vector<long long int> > cost_matrix(matrix_size, std::vector<long long int> (matrix_size, 0));
        // std::cout << "rows: " << cost_matrix.size() << ", cols: " << cost_matrix[0].size() << std::endl;
        // TODO: but we already went over each pair of children
        for (unsigned int s = 1; s <= matrix_size; ++s) {
          for (unsigned int t = 1; t <= matrix_size; ++t) {
            if (s <= t1.postl_to_children_[i-1].size())
            {
              if (t <= t2.postl_to_children_[j-1].size())
              {
                cost_matrix[s-1][t-1] = df_.at(s, t);
              }
              else
              {
                cost_matrix[s-1][t-1] = t1.postl_to_size_[t-1];
              }
            } else if (t <= t2.postl_to_children_[j-1].size())
            {
              if (s <= t1.postl_to_children_[i-1].size())
              {
                cost_matrix[s-1][t-1] = df_.at(s, t);
              }
              else
              {
                cost_matrix[s-1][t-1] = t2.postl_to_size_[s-1];
              }
            }
          }
        }
        
        // std::cout << " ******** 3 " << std::endl;
        // print_matrix(cost_matrix);

        // Compute Hungarian Algorithm for minimal tree mapping.
        min_for_ren = hungarian_algorithm(cost_matrix);
      }

      // std::cout << " ******** 4 " << (min_for_del >= min_for_ins ? min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : min_for_del >= min_for_ren ? min_for_ren : min_for_del) << std::endl;
      // std::cout << " ******** 4 del " << (min_for_del) << std::endl;
      // std::cout << " ******** 4 ins " << (min_for_ins) << std::endl;
      // std::cout << " ******** 4 ren " << (min_for_ren) << std::endl;

      // Compute minimal forest mapping costs.
      df_.at(i, j) = min_for_del >= min_for_ins ? min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : min_for_del >= min_for_ren ? min_for_ren : min_for_del;
      // Compute rename costs for trees i and j.
      min_tree_ren = df_.at(i, j) + c_.ren(t1.postl_to_label_id_[i - 1], t2.postl_to_label_id_[j - 1]);
      // Compute minimal tree mapping costs.
      dt_.at(i, j) = min_tree_del >= min_tree_ins ? min_tree_ins >= min_tree_ren ? min_tree_ren : min_tree_ins : min_tree_del >= min_tree_ren ? min_tree_ren : min_tree_del;

      // if (t2.postl_to_parent_[j-1] > -1) {
      //   if (df_.at(i, j) - df_.at(0, j) < df2[t2.postl_to_parent_[j-1]+1]) {
      //     df2[t2.postl_to_parent_[j-1]+1] = df_.at(i, j) - df_.at(0, j);
      //   }
      //   if (dt_.at(i, j) - dt_.at(0, j) < dt2[t2.postl_to_parent_[j-1]+1]) {
      //     dt2[t2.postl_to_parent_[j-1]+1] = dt_.at(i, j) - dt_.at(0, j);
      //   }
      // }
      // if (t1.postl_to_parent_[i-1] > -1) {
      //   if (df_.at(i, j) - df_.at(i, 0) < df_.at(t1.postl_to_parent_[i-1]+1, j)) {
      //     df_.at(t1.postl_to_parent_[i-1]+1, j) = df_.at(i, j) - df_.at(i, 0);
      //   }
      //   if (dt_.at(i, j) - dt_.at(i, 0) < dt_.at(t1.postl_to_parent_[i-1]+1, j)) {
      //     dt_.at(t1.postl_to_parent_[i-1]+1, j) = dt_.at(i, j) - dt_.at(i, 0);
      //   }
      // }


      // for (long long int r = 0; r < t1_input_size+1; r++)
      // {
      //   for (long long int c = 0; c < t2_input_size+1; c++)
      //   {
      //     std::cout << " " << df_.at(r, c);
      //   }
      //   std::cout << std::endl;
      // }
      // std::cout << std::endl;

      // for (long long int r = 0; r < t1_input_size+1; r++)
      // {
      //   for (long long int c = 0; c < t2_input_size+1; c++)
      //   {
      //     std::cout << " " << dt_.at(r, c);
      //   }
      //   std::cout << std::endl;
      // }
      // std::cout << std::endl;
    }
  }
  
  return dt_.at(t1_input_size, t2_input_size);
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::print_matrix(
    std::vector<std::vector<long long int> >& cost_matrix)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
    {
      std::cout << " " << cost_matrix[r][c];
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

template <typename cost_matrixModel, typename TreeIndex>
bool DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_mapping(
    std::vector<std::vector<long long int> >& cost_matrix, 
    long long int r, long long int rows, long long int cols, 
    std::unordered_map<long long int, long long int>& assignment, 
    std::vector<bool>& assigned_cols)
{
  // All rows are assigned.
  if (assignment.size() == cost_matrix.size())
  {
    return true;
  }

  for (long long int c = 0; c < cost_matrix[0].size(); c++)
  {
    if (cost_matrix[r][c] == 0 && assigned_cols[c] == 0)
    {
      assignment[r] = c;
      assigned_cols[c] = 1;
      if (find_mapping(cost_matrix, r + 1, rows, cols, assignment, assigned_cols))
      {
        return true;
      }
      assigned_cols[c] = 0;
    }
  }
  return false;
}

template <typename cost_matrixModel, typename TreeIndex>
long long int DPJEDTreeIndex<cost_matrixModel, TreeIndex>::hungarian_algorithm(
    std::vector<std::vector<long long int> > cost_matrix)
{
  // Given cost_matrix matrix.
  // std::vector<std::vector<long long int> > cost_matrix {
  //     {108, 125, 150},
  //     {150, 135, 175},
  //     {122, 148, 250}
  // };
  // std::vector<std::vector<long long int> > cost_matrix {
  //     {82,83,69,92},
  //     {77,37,49,92},
  //     {11,69, 5,86},
  //     { 8, 9,98,23}
  // };

  // Number of rows and columns in the given cost_matrix matrix.
  long long int rows =  cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  // Step 0. Initial adjacency matrix.
  // print_matrix(cost_matrix);

  // Step 1. Subtract row minimum.
  for (long long int r = 0; r < rows; r++)
  {
    long long int min_elem = cost_matrix[r][0];
    for (long long int i = 1; i < cols; i++)
    {
      min_elem = std::min(min_elem, cost_matrix[r][i]);
    }
    for (long long int c = 0; c < cols; c++)
    {
      cost_matrix[r][c] -= min_elem;
    }
  }
  // print_matrix(cost_matrix);

  // Step 2. Subtract column minimum.
  // TODO: Optimize by only reducing columns without a zero.
  for (long long int c = 0; c < cols; c++)
  {
    long long int min_elem = cost_matrix[0][c];
    for (long long int i = 1; i < rows; i++)
    {
      min_elem = std::min(min_elem, cost_matrix[i][c]);
    }
    for (long long int r = 0; r < rows; r++)
    {
      cost_matrix[r][c] -= min_elem;
    }
  }
  // print_matrix(cost_matrix);

  while (1)
  {
    // Step 3. Draw lines to cover all zeros in the cost_matrix matrix.
    std::unordered_map<long long int, long long int> min_row_zeros;
    std::unordered_map<long long int, long long int> min_col_zeros;
    for (long long int d = 0; d < cols; d++) // Go through diagonally.
    {
      // Count number of zeros for a certain row.
      long long int zeros_in_row = 0;
      for (long long int c = 0; c < cols; c++) {
        if (cost_matrix[d][c] == 0) {
          zeros_in_row++;
        }
      }
      // Count number of zeros for a certain column.
      long long int zeros_in_column = 0;
      for (long long int r = 0; r < rows; r++) {
        if (cost_matrix[r][d] == 0) {
          zeros_in_column++;
        }
      }
      // Store for all elements in the diagonal of the cost_matrix matrix, whether the 
      // row or the column have more zeros. For the smaller one, only more than 
      // one 0 are of interest, since one 0 is covered by the other anyways.
      if (zeros_in_row > zeros_in_column) {
        min_row_zeros[d] = d;
        if (zeros_in_column > 1) {
          min_col_zeros[d] = d;
        }
      } else if (zeros_in_row < zeros_in_column) {
        min_col_zeros[d] = d;
        if (zeros_in_row > 1) {
          min_row_zeros[d] = d;
        }
      } else {
        if ((zeros_in_row + zeros_in_column) >= 2) {
          min_row_zeros[d] = d;
          min_col_zeros[d] = d;
        }
      }
    }
    // Remove columns that are covered by rows.
    long long int zero_count;
    for (auto it = min_col_zeros.begin(); it != min_col_zeros.end();)
    {
      zero_count = 0;
      for (long long int i = 0; i < rows; i++)
      {
        if ((cost_matrix[i][it->first] == 0) && 
            (min_row_zeros.find(i) == min_row_zeros.end()))
        {
          zero_count++;
        }
      }
      if(zero_count == 0)
      {
        it = min_col_zeros.erase(it);
      }
      else 
      {
        ++it;
      }
    }
    // Remove rows that are covered by columns.
    for (auto it = min_row_zeros.begin(); it != min_row_zeros.end();)
    {
      zero_count = 0;
      for (long long int i = 0; i < cols; i++) {
        if ((cost_matrix[it->first][i] == 0) && 
            (min_col_zeros.find(i) == min_col_zeros.end()))
        {
          zero_count++;
        }
      }
      if(zero_count == 0)
      {
        it = min_row_zeros.erase(it);
      }
      else 
      {
        ++it;
      }
    }
    if (min_row_zeros.size() + min_col_zeros.size() == cost_matrix.size())
    {
      // Step 5. Identify mapping out of zeros.
      // Holds 1 if column is assigned, 0 otherwise.
      std::unordered_map<long long int, long long int> assignment;
      std::vector<bool> assigned_cols (cols);

      print_matrix(cost_matrix);

      // Print results if an assignment was found.
      if (find_mapping(cost_matrix, 0, rows, cols, assignment, assigned_cols))
      {
        long long int result = 0;
        for (auto& it: assignment)
        {
          result += cost_matrix[it.first][it.second];
        }
        return result;
      }
      else
      {
        std::cout << "Could not find a optimal mapping." << std::endl;
      }
      break;
    }

    // Step 4. Add minimum uncovered value to covered values.
    long long int min_elem = std::numeric_limits<long long int>::infinity();
    for (long long int r = 0; r < rows; r++)
    {
      // Find uncovered element with minimum cost_matrix.
      for (long long int c = 0; c < cols; c++)
      {
        // Check whether position r, c is uncovered.
        if (min_row_zeros.find(r) == min_row_zeros.end() && 
            min_col_zeros.find(c) == min_col_zeros.end())
        {
          min_elem = std::min(min_elem, cost_matrix[r][c]);
        }
      }
    }
    for (long long int r = 0; r < rows; r++)
    {
      for (long long int c = 0; c < cols; c++)
      {
        // Add the minimum element to all row covered positions.
        if (min_row_zeros.find(r) != min_row_zeros.end())
        {
          cost_matrix[r][c] += min_elem;
        }
        // Add the minimum element to all column covered positions.
        if (min_col_zeros.find(c) != min_col_zeros.end())
        {
          cost_matrix[r][c] += min_elem;
        }
        // Subtract the minimum element from all cost_matrix matrix positions.
        cost_matrix[r][c] -= min_elem;
      }
    }
  }
}