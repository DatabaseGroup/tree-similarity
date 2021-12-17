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

/// \file json/jedi_baseline_index_impl.h
///
/// \details
/// Contains the implementation of the JSON edit distance class. Based on a 
/// combination of Kaizhong Zhang. “Algorithms for the constrained editing 
/// distance between ordered labeled trees and related problems”. In: Pattern 
/// Recognition (1995) and Kaizhong Zhang. “A constrained edit distance between 
/// unordered labeled trees”. In: Algorithmica 15.3 (1996). Baseline algorithm 
/// to QuickJEDI.


#pragma once

template <typename CostModel, typename TreeIndex>
double JEDIBaselineTreeIndex<CostModel, TreeIndex>::jedi(
    const TreeIndex& t1, const TreeIndex& t2) {

  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  int t1_input_size = t1.tree_size_;
  int t2_input_size = t2.tree_size_;
  int larger_tree_size = std::max(t1_input_size, t2_input_size);
  
  // Initialise distance matrices.
  dt_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  df_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  e_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  std::vector<std::vector<double> > hungarian_cm
      (2*larger_tree_size, std::vector<double> (2*larger_tree_size, 0));
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());
  e_.fill_with(std::numeric_limits<double>::infinity());

  dt_.at(0, 0) = 0;
  df_.at(0, 0) = 0;
  // Deletion initialization.
  for (int i = 1; i <= t1_input_size; ++i) {
    df_.at(i, 0) = 0;
    for (unsigned int k = 1; k <= t1.postl_to_children_[i-1].size(); ++k) {
      df_.at(i, 0) += dt_.at(t1.postl_to_children_[i-1][k-1] + 1, 0);
    }
    dt_.at(i, 0) = df_.at(i, 0) + c_.del(t1.postl_to_label_id_[i - 1]);
  }
  // Insertion initialization.
  for (int j = 1; j <= t2_input_size; ++j) {
    df_.at(0, j) = 0;
    for (unsigned int k = 1; k <= t2.postl_to_children_[j-1].size(); ++k) {
      df_.at(0, j) += dt_.at(0, t2.postl_to_children_[j-1][k-1] + 1);
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
            (df_.at(i, t2.postl_to_children_[j-1][t-1] + 1) - 
             df_.at(0, t2.postl_to_children_[j-1][t-1] + 1)));
        min_tree_del = std::min(min_tree_del,
            (dt_.at(i, t2.postl_to_children_[j-1][t-1] + 1) - 
             dt_.at(0, t2.postl_to_children_[j-1][t-1] + 1)));
      }
      min_for_del += df_.at(0, j);
      min_tree_del += dt_.at(0, j);

      // Cost for insertion in forest.
      min_for_ins = std::numeric_limits<double>::infinity();
      min_tree_ins = std::numeric_limits<double>::infinity();
      for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
        min_for_ins = std::min(min_for_ins, 
            (df_.at(t1.postl_to_children_[i-1][s-1] + 1, j) - 
             df_.at(t1.postl_to_children_[i-1][s-1] + 1, 0)));
        min_tree_ins = std::min(min_tree_ins, 
            (dt_.at(t1.postl_to_children_[i-1][s-1] + 1, j) - 
             dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 0)));
      }
      min_for_ins += df_.at(i, 0);
      min_tree_ins += dt_.at(i, 0);

      // Cost for minimal mapping between trees in forest.
      min_for_ren = std::numeric_limits<double>::infinity();
      min_tree_ren = std::numeric_limits<double>::infinity();
      // If we compare two array nodes, we need to consider the order amoung 
      // the children subtrees. Therefore, the string edit distance is used 
      // instead of the Hungarian Algorithm.
      if (t1.postl_to_type_[i - 1] == 1 && t2.postl_to_type_[j - 1] == 1)
      {
        // Compute string edit distance for array children.
        e_.at(0, 0) = 0;
        for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
          e_.at(s, 0) = e_.at(s-1, 0) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 0);
        }
        for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
          e_.at(0, t) = e_.at(0, t-1) + dt_.at(0, t2.postl_to_children_[j-1][t-1] + 1);
        }
        
        double a = -1;
        double b = -1;
        double c = -1;
        for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
          for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
            ++subproblem_counter_;
            a = e_.at(s, t-1) + dt_.at(0, t2.postl_to_children_[j-1][t-1] + 1);
            b = e_.at(s-1, t) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 0);
            c = e_.at(s-1, t-1) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, t2.postl_to_children_[j-1][t-1] + 1);
            e_.at(s, t) = a >= b ? b >= c ? c : b : a >= c ? c : a;
          }
        }
        // Assign string edit distance costs for subtree mapping cost.
        min_for_ren = e_.at(t1.postl_to_children_[i-1].size(), t2.postl_to_children_[j-1].size()); 
      }
      // In case of two keys, take the costs of mapping their child to one another.
      else if (t1.postl_to_type_[i - 1] == 2 && t2.postl_to_type_[j - 1] == 2 && 
            t1.postl_to_children_[i-1].size() > 0 && 
            t2.postl_to_children_[j-1].size() > 0)
      {
        // Keys have exactly one child, therefore, [0] always works.
        min_for_ren = dt_.at(t1.postl_to_children_[i-1][0] + 1, 
            t2.postl_to_children_[j-1][0] + 1);
      }
      // Values are leaves, mapping there subforests has cost 0.
      else if (t1.postl_to_type_[i - 1] == 3 && t2.postl_to_type_[j - 1] == 3)
      {
        min_for_ren = 0;
      }
      // If the nodes types are of type other than array, compute the 
      // Hungarian Algorithm.
      else
      {
        // Build a cost matrix such that each subtree can be mapped to another 
        // subtree or to an empty tree.
        unsigned long matrix_size = t1.postl_to_children_[i-1].size() + 
            t2.postl_to_children_[j-1].size();

        for (unsigned int s = 1; s <= matrix_size; ++s) {
          for (unsigned int t = 1; t <= matrix_size; ++t) {
            if (s <= t1.postl_to_children_[i-1].size()) {
              if (t <= t2.postl_to_children_[j-1].size()) {
                hungarian_cm[s-1][t-1] = dt_.at(
                    t1.postl_to_children_[i-1][s-1] + 1, 
                    t2.postl_to_children_[j-1][t-1] + 1);
              } else {
                hungarian_cm[s-1][t-1] = 
                    t1.postl_to_size_[t1.postl_to_children_[i-1][s-1]];
              }
            } else {
              if (t <= t2.postl_to_children_[j-1].size()) {
                hungarian_cm[s-1][t-1] = 
                    t2.postl_to_size_[t2.postl_to_children_[j-1][t-1]];
              } else {
                hungarian_cm[s-1][t-1] = 0;
              }
            }
          }
        }

        // Compute Hungarian Algorithm for minimal tree mapping.
        min_for_ren = execute_hungarian(hungarian_cm, matrix_size);
      }

      // Compute minimal forest mapping costs.
      df_.at(i, j) = min_for_del >= min_for_ins ?
          min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : 
          min_for_del >= min_for_ren ? min_for_ren : min_for_del;
      // Compute rename costs for trees i and j.
      // Consider the case that i is deleted and j is inserted.
      if (t1.postl_to_type_[i - 1] != t2.postl_to_type_[j - 1]) {
        min_tree_ren = df_.at(i, j) + c_.del(t1.postl_to_label_id_[i - 1]) + 
            c_.ins(t2.postl_to_label_id_[j - 1]);
      } else {
        min_tree_ren = df_.at(i, j) + c_.ren(t1.postl_to_label_id_[i - 1], 
            t2.postl_to_label_id_[j - 1]);
      }

      // Compute minimal tree mapping costs.
      dt_.at(i, j) = min_tree_del >= min_tree_ins ?
          min_tree_ins >= min_tree_ren ? min_tree_ren : min_tree_ins :
          min_tree_del >= min_tree_ren ? min_tree_ren : min_tree_del;
    }
  }

  return dt_.at(t1_input_size, t2_input_size);
}

template <typename CostModel, typename TreeIndex>
double JEDIBaselineTreeIndex<CostModel, TreeIndex>::jedi_k(
    const TreeIndex& t1, const TreeIndex& t2, const double threshold) {
  // JEDI-Baseline cannot leverage a threshold, therefore discard.
  return jedi(t1, t2);
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::print_matrix(
    std::vector<std::vector<double> >& cost_matrix)
{
  double rows = cost_matrix.size();
  double cols = cost_matrix[0].size();

  for (unsigned long r = 0; r < rows; r++)
  {
    for (unsigned long c = 0; c < cols; c++)
    {
      std::cout << " " << cost_matrix[r][c];
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

template <typename cost_matrixModel, typename TreeIndex>
double JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::execute_hungarian(
    std::vector<std::vector<double> >& cost_matrix,
    unsigned long& matrix_size)
{
  std::vector<std::vector<double> > mask_matrix(
    matrix_size, std::vector<double> (matrix_size, 0));
  std::vector<double> row_cover (matrix_size, 0);
  std::vector<double> col_cover (matrix_size, 0);
  double path_row_0 = -1;
  double path_col_0 = -1;
  double costs = 0;

  // Copy the original cost matrix to look up the costs later.
  std::vector<std::vector<double> > orig_matrix(
    matrix_size, std::vector<double> (matrix_size));
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      orig_matrix[r][c] = cost_matrix[r][c];
    }
  }

  bool done = false;
  int step = 1;  
  while (!done)
  {
    switch (step)
    {
      case 1:
        step_one(cost_matrix, matrix_size, step);
        break;
      case 2:
        step_two(cost_matrix, mask_matrix, matrix_size, 
                  row_cover, col_cover, step);
        break;
      case 3:
        step_three(mask_matrix, matrix_size, col_cover, step);
        break;
      case 4:
        step_four(cost_matrix, mask_matrix, matrix_size, row_cover, col_cover,
                  path_row_0, path_col_0, step);
        break;
      case 5:
        step_five(mask_matrix, matrix_size, row_cover, col_cover, 
                  path_row_0, path_col_0, step);
        break;
      case 6:
        step_six(cost_matrix, matrix_size, row_cover, col_cover, step);
        break;
      case 7:
        step_seven(orig_matrix, mask_matrix, matrix_size, costs);
        done = true;
        break;
    }
  }

  return costs;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_one(
    std::vector<std::vector<double> >& cost_matrix,
    unsigned long& matrix_size,
    int& step)
{
  double min_in_row;
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    min_in_row = cost_matrix[r][0];
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (cost_matrix[r][c] < min_in_row)
      {
        min_in_row = cost_matrix[r][c];
      }
    }
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      cost_matrix[r][c] -= min_in_row;
    }
  }
  step = 2;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_two(
    std::vector<std::vector<double> >& cost_matrix,
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    int& step)
{
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (cost_matrix[r][c] == 0 && row_cover[r] == 0 && col_cover[c] == 0)
      {
        mask_matrix[r][c] = 1;
        row_cover[r] = 1;
        col_cover[c] = 1;
      }
    }
  }
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    row_cover[r] = 0;
  }
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    col_cover[c] = 0;
  }
  step = 3;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_three(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    std::vector<double>& col_cover,
    int& step)
{
  double col_count = 0;
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (mask_matrix[r][c] == 1)
      {
        col_cover[c] = 1;
      }
    }
  }

  col_count = 0;
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    if (col_cover[c] == 1)
    {
        col_count += 1;
    }
  }
  if (col_count >= matrix_size)
  {
    step = 7;
  }
  else
  {
    step = 4;
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::find_a_zero(
    std::vector<std::vector<double> >& cost_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    int& row, int& col)
{
  row = -1;
  col = -1;
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (cost_matrix[r][c] == 0 && row_cover[r] == 0 && col_cover[c] == 0)
      {
        row = r;
        col = c;
        return;
      }
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
bool JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::star_in_row(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size, int row)
{
  bool found_star = false;
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    if (mask_matrix[row][c] == 1)
    {
      found_star = true;
    }
  }

  return found_star;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::find_star_in_row(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size, int row, int& col)
{
  col = -1;
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    if (mask_matrix[row][c] == 1)
    {
      col = c;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_four(
    std::vector<std::vector<double> >& cost_matrix,
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    double& path_row_0, double& path_col_0,
    int& step)
{
  int row = -1;
  int col = -1;

  while (1)
  {
    find_a_zero(cost_matrix, matrix_size, row_cover, col_cover, row, col);
    if (row == -1)
    {
      step = 6;
      break;
    }
    else
    {
      mask_matrix[row][col] = 2;
      if (star_in_row(mask_matrix, matrix_size, row))
      {
        find_star_in_row(mask_matrix, matrix_size, row, col);
        row_cover[row] = 1;
        col_cover[col] = 0;
      }
      else
      {
        path_row_0 = row;
        path_col_0 = col;
        step = 5;
        break;
      }
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::find_star_in_col(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    int& row, int col)
{
  row = -1;
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    if (mask_matrix[r][col] == 1)
    {
      row = r;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::find_prime_in_row(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    int row, int& col)
{
  col = -1;
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    if (mask_matrix[row][c] == 2)
    {
      col = c;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::augment_path(
    std::vector<std::vector<double> >& mask_matrix, 
    std::vector<std::vector<double> >& path)
{
  for (unsigned long p = 0; p < path.size(); p++)
  {
    if (mask_matrix[path[p][0]][path[p][1]] == 1)
    {
      mask_matrix[path[p][0]][path[p][1]] = 0;
    }
    else
    {  
      mask_matrix[path[p][0]][path[p][1]] = 1;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::clear_covers(
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover)
{
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    row_cover[r] = 0;
  }
  for (unsigned long c = 0; c < matrix_size; c++)
  {
    col_cover[c] = 0;
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::erase_primes(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size)
{
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (mask_matrix[r][c] == 2)
      {
        mask_matrix[r][c] = 0;
      }
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_five(
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    double& path_row_0, double& path_col_0,
    int& step)
{
  int r = -1;
  int c = -1;

  std::vector<std::vector<double> > path;
  path.push_back(std::vector<double>());
  path[path.size() - 1].push_back(path_row_0);
  path[path.size() - 1].push_back(path_col_0);

  while (1)
  {
    find_star_in_col(mask_matrix, matrix_size, r, path[path.size() - 1][1]);
    if (r > -1)
    {
      path.push_back(std::vector<double>());
      path[path.size() - 1].push_back(r);
      path[path.size() - 1].push_back(path[path.size() - 2][1]);
    }
    else
    {
      break;
    }
    find_prime_in_row(mask_matrix, matrix_size, path[path.size() - 1][0], c);
    path.push_back(std::vector<double>());
    path[path.size() - 1].push_back(path[path.size() - 2][0]);
    path[path.size() - 1].push_back(c);
  }
  augment_path(mask_matrix, path);
  clear_covers(matrix_size, row_cover, col_cover);
  erase_primes(mask_matrix, matrix_size);
  step = 3;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::find_smallest(
    std::vector<std::vector<double> >& cost_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    double& min_val)
{
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (row_cover[r] == 0 && col_cover[c] == 0)
      {
        if (min_val > cost_matrix[r][c])
        {
          min_val = cost_matrix[r][c];
        }
      }
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_six(
    std::vector<std::vector<double> >& cost_matrix,
    unsigned long& matrix_size,
    std::vector<double>& row_cover,
    std::vector<double>& col_cover,
    int& step)
{
  double min_val = std::numeric_limits<double>::max();
  find_smallest(cost_matrix, matrix_size, row_cover, col_cover, min_val);

  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (row_cover[r] == 1)
      {
          cost_matrix[r][c] += min_val;
      }
      if (col_cover[c] == 0)
      {
          cost_matrix[r][c] -= min_val;
      }
    }
  }

  step = 4;
}

template <typename cost_matrixModel, typename TreeIndex>
void JEDIBaselineTreeIndex<cost_matrixModel, TreeIndex>::step_seven(
    std::vector<std::vector<double> >& cost_matrix,
    std::vector<std::vector<double> >& mask_matrix,
    unsigned long& matrix_size,
    double& costs)
{
  costs = 0;
  for (unsigned long r = 0; r < matrix_size; r++)
  {
    for (unsigned long c = 0; c < matrix_size; c++)
    {
      if (mask_matrix[r][c] == 1)
      {
        costs += cost_matrix[r][c];
      }
    }
  }
}

