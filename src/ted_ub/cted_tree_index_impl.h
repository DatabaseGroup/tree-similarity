// The MIT License (MIT)
// Copyright (c) 2019 Mateusz Pawlik.
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

/// Contains the implementation of the Constrained Tree Edit Distance algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double CTEDTreeIndex<CostModel, TreeIndex>::ted(const TreeIndex& t1, const TreeIndex& t2) {

  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  int t1_input_size = t1.tree_size_;
  int t2_input_size = t2.tree_size_;
  
  // Initialise distance matrices.
  
  // TODO: Verify if a move assignment operator is used here.
  dt_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  df_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  e_ = data_structures::Matrix<double>(t1_input_size+1, t2_input_size+1);
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());
  e_.fill_with(std::numeric_limits<double>::infinity());
  
  std::vector<double> dt2(t2_input_size + 1);
  std::vector<double> df2(t2_input_size + 1);
  
  double a = -1;
  double b = -1;
  double c = -1;
  
  dt_.at(0, 0) = 0;
  df_.at(0, 0) = 0;
  for (int i = 1; i <= t1_input_size; ++i) {
    df_.at(i, 0) = 0;
    for (unsigned int k = 1; k <= t1.postl_to_children_[i-1].size(); ++k) {
      df_.at(i, 0) += dt_.at(t1.postl_to_children_[i-1][k-1] + 1, 0);
    }
    dt_.at(i, 0) = df_.at(i, 0) + c_.del(t1.postl_to_label_id_[i - 1]);
  }
  for (int j = 1; j <= t2_input_size; ++j) {
    df_.at(0, j) = 0;
    for (unsigned int k = 1; k <= t2.postl_to_children_[j-1].size(); ++k) {
      df_.at(0, j) += dt_.at(0, t2.postl_to_children_[j-1][k-1] + 1);
    }
    dt_.at(0, j) = df_.at(0, j) + c_.ins(t2.postl_to_label_id_[j - 1]);
  }
  
  for (int i = 1; i <= t1_input_size; ++i) {
    std::fill(dt2.begin(), dt2.end(), std::numeric_limits<double>::infinity());
    std::fill(df2.begin(), df2.end(), std::numeric_limits<double>::infinity());
    
    for (int j = 1; j <= t2_input_size; ++j) {
      e_.at(0, 0) = 0;
      for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
        e_.at(s, 0) = e_.at(s-1, 0) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 0); // here we access d for subtree rooted in the s'th child of node i AND s starts with 1 but the postorder of the first child of node i is children1[i][s-1]
      }
      for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
        e_.at(0, t) = e_.at(0, t-1) + dt_.at(0, t2.postl_to_children_[j-1][t-1] + 1);
      }
      
      // TODO: but we already went over each pair of children
      for (unsigned int s = 1; s <= t1.postl_to_children_[i-1].size(); ++s) {
        for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
          ++subproblem_counter_;
          a = e_.at(s, t-1) + dt_.at(0, t2.postl_to_children_[j-1][t-1] + 1);
          b = e_.at(s-1, t) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, 0);
          c = e_.at(s-1, t-1) + dt_.at(t1.postl_to_children_[i-1][s-1] + 1, t2.postl_to_children_[j-1][t-1] + 1); // TODO: This is a problematic part to reduce the memory. This requires to store distance for each pair of children of i and j.
          e_.at(s, t) = a >= b ? b >= c ? c : b : a >= c ? c : a;
        }
      }
      
      a = df_.at(0, j) + df2[j];
      b = df_.at(i, 0) + df_.at(i, j);
      c = e_.at(t1.postl_to_children_[i-1].size(), t2.postl_to_children_[j-1].size());
      
      df_.at(i, j) = a >= b ? b >= c ? c : b : a >= c ? c : a;
      
      a = dt_.at(0, j) + dt2[j];
      b = dt_.at(i, 0) + dt_.at(i, j);
      c = df_.at(i, j) + c_.ren(t1.postl_to_label_id_[i - 1], t2.postl_to_label_id_[j - 1]);
      
      dt_.at(i, j) = a >= b ? b >= c ? c : b : a >= c ? c : a;
      
      if (t2.postl_to_parent_[j-1] > -1) {
        if (df_.at(i, j) - df_.at(0, j) < df2[t2.postl_to_parent_[j-1]+1]) {
          df2[t2.postl_to_parent_[j-1]+1] = df_.at(i, j) - df_.at(0, j);
        }
        if (dt_.at(i, j) - dt_.at(0, j) < dt2[t2.postl_to_parent_[j-1]+1]) {
          dt2[t2.postl_to_parent_[j-1]+1] = dt_.at(i, j) - dt_.at(0, j);
        }
      }
      if (t1.postl_to_parent_[i-1] > -1) {
        if (df_.at(i, j) - df_.at(i, 0) < df_.at(t1.postl_to_parent_[i-1]+1, j)) {
          df_.at(t1.postl_to_parent_[i-1]+1, j) = df_.at(i, j) - df_.at(i, 0);
        }
        if (dt_.at(i, j) - dt_.at(i, 0) < dt_.at(t1.postl_to_parent_[i-1]+1, j)) {
          dt_.at(t1.postl_to_parent_[i-1]+1, j) = dt_.at(i, j) - dt_.at(i, 0);
        }
      }
    }
  }
  
  return dt_.at(t1_input_size, t2_input_size);
}
