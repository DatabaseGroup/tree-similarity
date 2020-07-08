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

  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  int t1_input_size = t1.tree_size_;
  int t2_input_size = t2.tree_size_;
  
  // Initialise distance matrices.
  
  // TODO: Verify if a move assignment operator is used here.
  dt_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  df_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());

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
      min_for_ren = 0;
      min_tree_ren = 0;

      // TODO: FIX!!!!
      if (t2.postl_to_children_[j-1].size() > 0 || t1.postl_to_children_[i-1].size() > 0) {
        long long int matrix_size = t1.postl_to_children_[i-1].size() + t2.postl_to_children_[j-1].size();
        std::vector<std::vector<long long int> > cost_matrix(matrix_size, std::vector<long long int> (matrix_size, 0));

        // TODO: but we already went over each pair of children
        for (unsigned int s = 1; s <= matrix_size; ++s)
        {
          for (unsigned int t = 1; t <= matrix_size; ++t)
          {
            if (s <= t1.postl_to_children_[i-1].size())
            {
              if (t <= t2.postl_to_children_[j-1].size())
              {
                cost_matrix[s-1][t-1] = dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 
                    t2.postl_to_children_[j-1][t-1] + 1);
              }
              else
              {
                cost_matrix[s-1][t-1] = t1.postl_to_size_[t1.postl_to_children_[i-1][s-1]];
              }
            } else if (t <= t2.postl_to_children_[j-1].size())
            {
              cost_matrix[s-1][t-1] = t2.postl_to_size_[t2.postl_to_children_[j-1][t-1]];
            }
          }
        }

        // Compute Hungarian Algorithm for minimal tree mapping.
        min_for_ren = execute_hungarian(cost_matrix);
      }

      // Compute minimal forest mapping costs.
      df_.at(i, j) = min_for_del >= min_for_ins ? min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : min_for_del >= min_for_ren ? min_for_ren : min_for_del;
      // Compute rename costs for trees i and j.
      min_tree_ren = df_.at(i, j) + c_.ren(t1.postl_to_label_id_[i - 1], t2.postl_to_label_id_[j - 1]);
      // Compute minimal tree mapping costs.
      dt_.at(i, j) = min_tree_del >= min_tree_ins ? min_tree_ins >= min_tree_ren ? min_tree_ren : min_tree_ins : min_tree_del >= min_tree_ren ? min_tree_ren : min_tree_del;

      //// PRINT FOREST AND TREE MATRIX
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

  // for (long long int r = 0; r < t1_input_size+1; r++)
  // {
  //   for (long long int c = 0; c < t2_input_size+1; c++)
  //   {
  //     if (df_.at(r, c) <= 9) std::cout << " ";
  //     std::cout << " " << df_.at(r, c);
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;

  // for (long long int r = 0; r < t1_input_size+1; r++)
  // {
  //   for (long long int c = 0; c < t2_input_size+1; c++)
  //   {
  //     if (dt_.at(r, c) <= 9) std::cout << " ";
  //     std::cout << " " << dt_.at(r, c);
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;

  // std::cout << " " << dt_.at(t1_input_size, t2_input_size) << std::endl;
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


//For each row of the cost matrix, find the smallest element and subtract
//it from every element in its row.  When finished, Go to Step 2.
template <typename cost_matrixModel, typename TreeIndex>
long long int DPJEDTreeIndex<cost_matrixModel, TreeIndex>::execute_hungarian(
    std::vector<std::vector<long long int> >& cost_matrix)
{
  std::vector<std::vector<long long int> > mask_matrix(
    cost_matrix.size(), std::vector<long long int> (cost_matrix[0].size(), 0));
  std::vector<long long int> row_cover (cost_matrix.size(), 0);
  std::vector<long long int> col_cover (cost_matrix[0].size(), 0);
  long long int path_row_0 = -1;
  long long int path_col_0 = -1;
  long long int costs = 0;

  // Copy the original cost matrix to look up the costs later.
  std::vector<std::vector<long long int> > orig_matrix(
    cost_matrix.size(), std::vector<long long int> (cost_matrix[0].size(), 0));
  for (long long int r = 0; r < cost_matrix.size(); r++)
  {
    for (long long int c = 0; c < cost_matrix[0].size(); c++)
    {
      orig_matrix[r][c] = cost_matrix[r][c];
    }
  }

  bool done = false;
  long long int step = 1;  
  while (!done)
  {
    switch (step)
    {
      case 1:
        step_one(cost_matrix, step);
        break;
      case 2:
        step_two(cost_matrix, mask_matrix, row_cover, col_cover, step);
        break;
      case 3:
        step_three(cost_matrix, mask_matrix, col_cover, step);
        break;
      case 4:
        step_four(cost_matrix, mask_matrix, row_cover, col_cover,
                  path_row_0, path_col_0, step);
        break;
      case 5:
        step_five(cost_matrix, mask_matrix, row_cover, col_cover, 
                  path_row_0, path_col_0, step);
        break;
      case 6:
        step_six(cost_matrix, row_cover, col_cover, step);
        break;
      case 7:
        step_seven(orig_matrix, mask_matrix, costs);
        done = true;
        break;
    }
  }

  return costs * 1.0;
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_one(
    std::vector<std::vector<long long int> >& cost_matrix, 
    long long int& step)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();
  long long int min_in_row;
  for (long long int r = 0; r < rows; r++)
  {
    min_in_row = cost_matrix[r][0];
    for (long long int c = 0; c < cols; c++)
    {
      if (cost_matrix[r][c] < min_in_row)
      {
        min_in_row = cost_matrix[r][c];
      }
    }
    for (long long int c = 0; c < cols; c++)
    {
      cost_matrix[r][c] -= min_in_row;
    }
  }
  step = 2;
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_two(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& step)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();
  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
    {
      if (cost_matrix[r][c] == 0 && row_cover[r] == 0 && col_cover[c] == 0)
      {
        mask_matrix[r][c] = 1;
        row_cover[r] = 1;
        col_cover[c] = 1;
      }
    }
  }
  for (long long int r = 0; r < rows; r++)
  {
    row_cover[r] = 0;
  }
  for (long long int c = 0; c < cols; c++)
  {
    col_cover[c] = 0;
  }
  step = 3;
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_three(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    std::vector<long long int>& col_cover,
    long long int& step)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();
  long long int col_count = 0;
  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
    {
      if (mask_matrix[r][c] == 1)
      {
        col_cover[c] = 1;
      }
    }
  }

  col_count = 0;
  for (long long int c = 0; c < cols; c++)
  {
    if (col_cover[c] == 1)
    {
        col_count += 1;
    }
  }
  if (col_count >= cols || col_count >= rows)
  {
    step = 7;
  }
  else
  {
    step = 4;
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_a_zero(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& row, long long int& col)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  row = -1;
  col = -1;
  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
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
bool DPJEDTreeIndex<cost_matrixModel, TreeIndex>::star_in_row(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    long long int row)
{
  long long int cols = cost_matrix[0].size();
  bool found_star = false;
  for (long long int c = 0; c < cols; c++)
  {
    if (mask_matrix[row][c] == 1)
    {
      found_star = true;
    }
  }

  return found_star;
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_star_in_row(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    long long int row, long long int& col)
{
  long long int cols = cost_matrix[0].size();

  col = -1;
  for (long long int c = 0; c < cols; c++)
  {
    if (mask_matrix[row][c] == 1)
    {
      col = c;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_four(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& path_row_0, long long int& path_col_0,
    long long int& step)
{
  long long int row = -1;
  long long int col = -1;

  while (1)
  {
    find_a_zero(cost_matrix, row_cover, col_cover, row, col);
    if (row == -1)
    {
      step = 6;
      break;
    }
    else
    {
      mask_matrix[row][col] = 2;
      if (star_in_row(cost_matrix, mask_matrix, row))
      {
        find_star_in_row(cost_matrix, mask_matrix, row, col);
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
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_star_in_col(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    long long int& row, long long int col)
{
  long long int rows = cost_matrix.size();

  row = -1;
  for (long long int r = 0; r < rows; r++)
  {
    if (mask_matrix[r][col] == 1)
    {
      row = r;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_prime_in_row(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    long long int row, long long int& col)
{
  long long int cols = cost_matrix[0].size();

  col = -1;
  for (long long int c = 0; c < cols; c++)
  {
    if (mask_matrix[row][c] == 2)
    {
      col = c;
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::augment_path(
    std::vector<std::vector<long long int> >& mask_matrix, 
    std::vector<std::vector<long long int> >& path)
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
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::clear_covers(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  for (long long int r = 0; r < rows; r++)
  {
    row_cover[r] = 0;
  }
  for (long long int c = 0; c < cols; c++)
  {
    col_cover[c] = 0;
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::erase_primes(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
    {
      if (mask_matrix[r][c] == 2)
      {
        mask_matrix[r][c] = 0;
      }
    }
  }
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_five(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& path_row_0, long long int& path_col_0,
    long long int& step)
{
  long long int r = -1;
  long long int c = -1;

  std::vector<std::vector<long long int> > path;
  path.push_back(std::vector<long long int>());
  path[path.size() - 1].push_back(path_row_0);
  path[path.size() - 1].push_back(path_col_0);

  while (1)
  {
    find_star_in_col(cost_matrix, mask_matrix, r, path[path.size() - 1][1]);
    if (r > -1)
    {
      path.push_back(std::vector<long long int>());
      path[path.size() - 1].push_back(r);
      path[path.size() - 1].push_back(path[path.size() - 2][1]);
    }
    else
    {
      break;
    }
    find_prime_in_row(cost_matrix, mask_matrix, path[path.size() - 1][0], c);
    path.push_back(std::vector<long long int>());
    path[path.size() - 1].push_back(path[path.size() - 2][0]);
    path[path.size() - 1].push_back(c);
  }
  augment_path(mask_matrix, path);
  clear_covers(cost_matrix, row_cover, col_cover);
  erase_primes(cost_matrix, mask_matrix);
  step = 3;
}

template <typename cost_matrixModel, typename TreeIndex>
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::find_smallest(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& min_val)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
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
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_six(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<long long int>& row_cover,
    std::vector<long long int>& col_cover,
    long long int& step)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  long long int min_val = std::numeric_limits<long long int>::max();
  find_smallest(cost_matrix, row_cover, col_cover, min_val);
  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
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
void DPJEDTreeIndex<cost_matrixModel, TreeIndex>::step_seven(
    std::vector<std::vector<long long int> >& cost_matrix,
    std::vector<std::vector<long long int> >& mask_matrix,
    long long int& costs)
{
  long long int rows = cost_matrix.size();
  long long int cols = cost_matrix[0].size();

  costs = 0;
  for (long long int r = 0; r < rows; r++)
  {
    for (long long int c = 0; c < cols; c++)
    {
      if (mask_matrix[r][c] == 1)
      {
        costs += cost_matrix[r][c];
      }
    }
  }
}

