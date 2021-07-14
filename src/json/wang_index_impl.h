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

/// \file json/wang_index_impl.h
///
/// \details
/// Contains the implementation of the JEDI-Order class. Computes the JEDI-Order 
/// by using the algorithm by Lusheng Wang and Kaizhong Zhang. “Space efficient 
/// algorithms for ordered tree comparison”. In: Algorithmica 51.3 (2008). 
/// Complexity: O(|T1||T2|) time and O(log(|T1|)|T2|) memory.

#pragma once

template <typename CostModel, typename TreeIndex>
double WangTreeIndex<CostModel, TreeIndex>::jedi(const TreeIndex& t1, 
    const TreeIndex& t2) {

  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  int t1_input_size = t1.tree_size_;
  int t2_input_size = t2.tree_size_;
  // Stop if one of the trees is empty.
  if (t1_input_size < 1) {
    return t2_input_size;
  } else if (t2_input_size < 1) {
    return t1_input_size;
  }
  int t1_height = t1.postl_to_height_[t1_input_size-1];

  // Create cost matrices.
  del_t1_subtree_.resize(t1_input_size+1);
  del_f1_subtree_.resize(t1_input_size+1);
  ins_t2_subtree_.resize(t2_input_size+1);
  ins_f2_subtree_.resize(t2_input_size+1);
  dt_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  df_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  e0_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  e_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  del_forest_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  del_tree_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  fav_child_dt_ = data_structures::Matrix<double>(t1_height+1, t2_input_size+1);
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());
  e0_.fill_with(0);
  e_.fill_with(0);
  del_forest_.fill_with(std::numeric_limits<double>::infinity());
  del_tree_.fill_with(std::numeric_limits<double>::infinity());
  fav_child_dt_.fill_with(std::numeric_limits<double>::infinity());

  // Initialize cost matrices.
  del_t1_subtree_.at(0) = 0;
  del_f1_subtree_.at(0) = 0;
  for (int i = 1; i <= t1_input_size; ++i) {
    del_f1_subtree_.at(i) = 0;
    for (unsigned int k = 1; k <= t1.postl_to_children_[i-1].size(); ++k) {
      del_f1_subtree_.at(i) += del_t1_subtree_.at(t1.postl_to_children_[i-1][k-1] + 1);
    }
    del_t1_subtree_.at(i) = del_f1_subtree_.at(i) + c_.del(t1.postl_to_label_id_[i - 1]);
  }

  ins_t2_subtree_[0] = 0;
  ins_f2_subtree_[0] = 0;
  for (int j = 1; j <= t2_input_size; ++j) {
    ins_f2_subtree_.at(j) = 0;
    for (unsigned int k = 1; k <= t2.postl_to_children_[j-1].size(); ++k) {
      ins_f2_subtree_.at(j) += ins_t2_subtree_.at(t2.postl_to_children_[j-1][k-1] + 1);
    }
    ins_t2_subtree_.at(j) = ins_f2_subtree_.at(j) + c_.ins(t2.postl_to_label_id_[j - 1]);
  }
  
  double min_for_ins = std::numeric_limits<double>::infinity();
  double min_tree_ins = std::numeric_limits<double>::infinity();
  double min_for_del = std::numeric_limits<double>::infinity();
  double min_tree_del = std::numeric_limits<double>::infinity();
  double min_for_ren = std::numeric_limits<double>::infinity();
  double min_tree_ren = std::numeric_limits<double>::infinity();
  int i = 0;
  
  for (int x = 1; x <= t1_input_size; ++x) {
    // Get postorder number from favorable child order number.
    i = t1.postl_to_favorder_[x-1] + 1;
    // std::cout << "i: " << i << "\t";
    for (int j = 1; j <= t2_input_size; ++j) {
      // std::cout << "i: " << i << ", j: " << j << std::endl;
      // Cost for deletion.
      if (t1.postl_to_children_[i-1].size() == 0) {
        // t1[i] is a leaf node. Therefore, all nodes of F2 have to be inserted.
        // Must be set to infinity, since we allow infinity costs for different node types.
        min_for_del = std::numeric_limits<double>::infinity(); //ins_f2_subtree_.at(j);
        min_tree_del = std::numeric_limits<double>::infinity(); //ins_t2_subtree_.at(j);
      } else {
        // t1[i] is no leaf node. Therefore, read the previously computed value.
        min_for_del = del_forest_.at(t1.postl_to_height_[i-1], j-1);
        min_tree_del = del_tree_.at(t1.postl_to_height_[i-1], j-1);
      }

      // Cost for insertion.
      if (t2.postl_to_children_[j-1].size() == 0) {
        // t2[j] is a leaf node. Therefore, all nodes of F1 have to be inserted.
        // Must be set to infinity, since we allow infinity costs for different node types.
        min_for_ins = std::numeric_limits<double>::infinity(); //del_f1_subtree_.at(i);
        min_tree_ins = std::numeric_limits<double>::infinity(); //del_t1_subtree_.at(i);
      } else {
        // t2[j] is no leaf node. Therefore, computed the value.
        min_for_ins = std::numeric_limits<double>::infinity();
        min_tree_ins = std::numeric_limits<double>::infinity();
        for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
          min_for_ins = std::min(min_for_ins,
              (df_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t] + 1) - 
               ins_f2_subtree_.at(t2.postl_to_children_[j-1][t] + 1)));
          min_tree_ins = std::min(min_tree_ins,
              (dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t] + 1) - 
               ins_t2_subtree_.at(t2.postl_to_children_[j-1][t] + 1)));
        }
        min_for_ins += ins_f2_subtree_.at(j);
        min_tree_ins += ins_t2_subtree_.at(j);
      }

      // Cost for rename.
      min_for_ren = 0;
      if (t1.postl_to_children_[i-1].size() == 0) {
        // t1[i] is a leaf node. Therefore, all nodes of F2 have to be inserted.
        min_for_ren += ins_f2_subtree_.at(j);
      } else if (t2.postl_to_children_[j-1].size() == 0) {
        // t2[j] is a leaf node. Therefore, all nodes of F1 have to be deleted.
        min_for_ren += del_f1_subtree_.at(i);
      } else {
        min_for_ren = e_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t2.postl_to_children_[j-1].size()-1]+1);
      }
      // Fill forest distance matrix.
      df_.at(t1.postl_to_height_[i-1], j) = min_for_del >= min_for_ins ? min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : min_for_del >= min_for_ren ? min_for_ren : min_for_del;
      // Compute tree rename based on forest cost matrix.
      min_tree_ren = df_.at(t1.postl_to_height_[i-1], j) + c_.ren(t1.postl_to_label_id_[i-1], t2.postl_to_label_id_[j-1]);

      // Fill tree distance matrix.
      dt_.at(t1.postl_to_height_[i-1], j) = min_tree_del >= min_tree_ins ? min_tree_ins >= min_tree_ren ? min_tree_ren : min_tree_ins : min_tree_del >= min_tree_ren ? min_tree_ren : min_tree_del;

      // Do not compute for the parent of the root node in T1.
      if (i != t1_input_size)
      {
        int p_i = t1.postl_to_height_[t1.postl_to_parent_[i-1]];
        // Case 1: i is favorable child of parent.
        if (t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]] == i-1) {
          fav_child_dt_.at(p_i, j) = dt_.at(t1.postl_to_height_[i-1], j);
          del_forest_.at(p_i, j-1) = del_f1_subtree_.at(t1.postl_to_parent_[i-1]+1) + df_.at(t1.postl_to_height_[i-1], j) - del_f1_subtree_.at(i);
          del_tree_.at  (p_i, j-1) = del_t1_subtree_.at(t1.postl_to_parent_[i-1]+1) + dt_.at(t1.postl_to_height_[i-1], j) - del_t1_subtree_.at(i);
          e_.at(p_i, 0) = 0;
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            if (t == 0) {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = e_.at(p_i, 0) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1);
            } else {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = e_.at(p_i, t2.postl_to_children_[j-1][t-1]+1) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1);
            }
          }
        }
        // Case 2: i is either leftmost child (and favorable) OR not favorable child.
        if ((t1.postl_to_children_[t1.postl_to_parent_[i-1]].size() > 0 && 
            t1.postl_to_children_[t1.postl_to_parent_[i-1]][0] == i-1) ||
            t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]] != i-1) {
          del_forest_.at(p_i, j-1) = std::min(del_forest_.at(p_i, j-1), del_f1_subtree_.at(t1.postl_to_parent_[i-1]+1) + df_.at(t1.postl_to_height_[i-1], j) - del_f1_subtree_.at(i));
          del_tree_.at(p_i, j-1) = std::min(del_tree_.at(p_i, j-1), del_t1_subtree_.at(t1.postl_to_parent_[i-1]+1) + dt_.at(t1.postl_to_height_[i-1], j) - del_t1_subtree_.at(i));
          // Copy e into e0.
          e0_.at(p_i, 0) = e_.at(p_i, 0);
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            e0_.at(p_i, t2.postl_to_children_[j-1][t] + 1) = e_.at(p_i, t2.postl_to_children_[j-1][t] + 1);
          }
          // Fill next line.
          e_.at(p_i, 0) = e0_.at(p_i, 0) + del_t1_subtree_.at(i);
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            if (t == 0) {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = std::min(
                 e_.at(p_i, 0)                                 + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1), std::min(
                e0_.at(p_i, t2.postl_to_children_[j-1][t]  +1) + del_t1_subtree_.at(i),
                e0_.at(p_i, 0)                                 + dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t]+1))
              );
            } else {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = std::min(
                 e_.at(p_i, t2.postl_to_children_[j-1][t-1]+1) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1), std::min(
                e0_.at(p_i, t2.postl_to_children_[j-1][t]  +1) + del_t1_subtree_.at(i),
                e0_.at(p_i, t2.postl_to_children_[j-1][t-1]+1) + dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t]+1))
              );
            }
          }
          if (j != t2_input_size) {
            e_.at(p_i, 0) = e0_.at(p_i, 0);
          }
        }
        // Case 3: t[i] is the left sibling of the favorable child.
        if (t1.postl_to_left_fav_child_[t1.postl_to_parent_[i-1]] == i-1) {
          int fav_child_postid = t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]]+1;
          // Store initial value of first column in edit distance cost matrix.
          // It is needed for the next j. Therefore rewrite at the end.
          int old_val = e_.at(p_i, 0);
          // Copy e into e0.
          e0_.at(p_i, 0) = e_.at(p_i, 0);
          if (j != t2_input_size) {
            e0_.at(p_i, 0) += del_t1_subtree_.at(i);
          }
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            e0_.at(p_i, t2.postl_to_children_[j-1][t] + 1) = e_.at(p_i, t2.postl_to_children_[j-1][t] + 1);
          }
          // Fill next line.
          e_.at(p_i, 0) = e0_.at(p_i, 0) + del_t1_subtree_.at(fav_child_postid);
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            if (t == 0) {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = std::min(
                 e_.at(p_i, 0)                                 + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1), std::min(
                e0_.at(p_i, t2.postl_to_children_[j-1][t]  +1) + del_t1_subtree_.at(fav_child_postid),
                e0_.at(p_i, 0)                                 + fav_child_dt_.at(p_i, t2.postl_to_children_[j-1][t]+1))
              );
            } else {
              e_.at(p_i, t2.postl_to_children_[j-1][t]+1) = std::min(
                 e_.at(p_i, t2.postl_to_children_[j-1][t-1]+1) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t]+1), std::min(
                e0_.at(p_i, t2.postl_to_children_[j-1][t]  +1) + del_t1_subtree_.at(fav_child_postid),
                e0_.at(p_i, t2.postl_to_children_[j-1][t-1]+1) + fav_child_dt_.at(p_i, t2.postl_to_children_[j-1][t]+1))
              );
            }
          }
          if (j != t2_input_size) {
            e_.at(p_i, 0) = old_val;
          }
        }
      }
    }
  }

  return dt_.at(t1_height, t2_input_size);
}


template <typename CostModel, typename TreeIndex>
double WangTreeIndex<CostModel, TreeIndex>::jedi_k(const TreeIndex& t1, 
    const TreeIndex& t2, const double threshold) {
  // JEDI-Baseline cannot leverage a threshold, therefore discard.
  return jedi(t1, t2);
}