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

/// Contains the implementation of the Constrained Tree Edit Distance algorithm
/// with space complexity O(n log(n)).

#pragma once

template <typename CostModel, typename TreeIndex>
double MODPJEDIndex<CostModel, TreeIndex>::ted(const TreeIndex& t1, 
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
  std::cout << "Create cost matrices. Height=" << t1_height << std::endl;
  del_t1_subtree_.resize(t2_input_size+1);
  del_f1_subtree_.resize(t2_input_size+1);
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
  e0_.fill_with(std::numeric_limits<double>::infinity());
  e_.fill_with(std::numeric_limits<double>::infinity());
  del_forest_.fill_with(std::numeric_limits<double>::infinity());
  del_tree_.fill_with(std::numeric_limits<double>::infinity());
  fav_child_dt_.fill_with(std::numeric_limits<double>::infinity());
  
  // Initialize cost matrices.
  del_t1_subtree_[0] = 0;
  del_f1_subtree_[0] = 0;
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
    for (int j = 1; j <= t2_input_size; ++j) {
      i = t1.postl_to_favorder_[x-1] + 1;
      std::cout << " --- START i: " << i-1 << " -> height=" << t1.postl_to_height_[i-1] << ", j: " << j-1 << " -> height=" << t2.postl_to_height_[j-1] << std::endl;
      // Cost for deletion.
      if (t1.postl_to_children_[i-1].size() == 0) {
        // t1[i] is a leaf node. Therefore, all nodes of F2 have to be inserted.
        min_for_del = ins_f2_subtree_.at(j);
        min_tree_del = ins_t2_subtree_.at(j);
      } else {
        // t1[i] is no leaf node. Therefore, read the previously computed value.
        min_for_del = del_forest_.at(t1.postl_to_height_[i-1], j-1);
        min_tree_del = del_tree_.at(t1.postl_to_height_[i-1], j-1);
      }
      std::cout << " --- DELETION min_for_del: " << min_for_del << ", min_tree_del: " << min_tree_del << std::endl;

      // Cost for insertion.
      if (t2.postl_to_children_[j-1].size() == 0) {
        // t2[j] is a leaf node. Therefore, all nodes of F1 have to be inserted.
        min_for_ins = del_f1_subtree_.at(i);
        min_tree_ins = del_t1_subtree_.at(i);
      } else {
        // t2[j] is no leaf node. Therefore, computed the value.
        min_for_ins = std::numeric_limits<double>::infinity();
        min_tree_ins = std::numeric_limits<double>::infinity();
        for (unsigned int t = 1; t <= t2.postl_to_children_[j-1].size(); ++t) {
          min_for_ins = std::min(min_for_ins,
              (df_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t-1] + 1) - 
               ins_f2_subtree_.at(t2.postl_to_children_[j-1][t-1] + 1)));
          min_tree_ins = std::min(min_tree_ins,
              (dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t-1] + 1) - 
               ins_t2_subtree_.at(t2.postl_to_children_[j-1][t-1] + 1)));
        }
        min_for_ins += ins_f2_subtree_.at(j);
        min_tree_ins += ins_t2_subtree_.at(j);
      }
      std::cout << " --- INSERTION min_for_ins: " << min_for_ins << ", min_tree_ins: " << min_tree_ins << std::endl;

      // Cost for rename.
      min_for_ren = 0;
      if (t1.postl_to_children_[j-1].size() == 0) {
        // t1[i] is a leaf node. Therefore, all nodes of F2 have to be inserted.
        for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
          min_for_ren += ins_f2_subtree_.at(t2.postl_to_children_[j-1][t] + 1);
        }
      } else if (t2.postl_to_children_[j-1].size() == 0) {
        // t2[j] is a leaf node. Therefore, all nodes of F1 have to be deleted.
        for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
          min_for_ren += del_f1_subtree_.at(t1.postl_to_children_[i-1][t] + 1);
        }
      } else {
        // t1[i] is no leaf node. Therefore, read the previously computed value.
        // TODO: Does this work?
        min_for_ren = e_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t2.postl_to_children_[j-1].size()-1]+1);
      }
      min_tree_ren = min_for_ren + c_.ren(t1.postl_to_label_id_[i - 1], t2.postl_to_label_id_[j - 1]);
      std::cout << " --- RENAME min_for_ren: " << min_for_ren << ", min_tree_ren: " << min_tree_ren << std::endl;

      // Fill forest distance matrix.
      df_.at(t1.postl_to_height_[i-1], j) = min_for_del >= min_for_ins ? min_for_ins >= min_for_ren ? min_for_ren : min_for_ins : min_for_del >= min_for_ren ? min_for_ren : min_for_del;
      // Fill tree distance matrix.
      dt_.at(t1.postl_to_height_[i-1], j) = min_tree_del >= min_tree_ins ? min_tree_ins >= min_tree_ren ? min_tree_ren : min_tree_ins : min_tree_del >= min_tree_ren ? min_tree_ren : min_tree_del;

      // Do not compute for the parent of the root node in T1.
      if (i != t1_input_size)
      {
        // Case 1: i is favorable child of parent.
        if (t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]] == i-1) {
          // std::cout << "CASE 1" << std::endl;
          fav_child_dt_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1) = dt_.at(t1.postl_to_height_[i-1], j);
          del_forest_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1) = del_f1_subtree_.at(t1.postl_to_parent_[i-1]+1) + df_.at(t1.postl_to_height_[i-1], j) - del_f1_subtree_.at(i);
          del_tree_.at  (t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1) = del_t1_subtree_.at(t1.postl_to_parent_[i-1]+1) + dt_.at(t1.postl_to_height_[i-1], j) - del_t1_subtree_.at(i);
          e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], 0) = 0;
          for (unsigned int t = 1; t < t2.postl_to_children_[j-1].size(); ++t) {
            e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t] + 1) = e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t-1] + 1) + ins_f2_subtree_.at(t2.postl_to_children_[j-1][t-1] + 1);
          }
        }
        // Case 2: i is either leftmost child (and favorable) OR not favorable child.
        if ((t1.postl_to_children_[t1.postl_to_parent_[i-1]].size() > 0 && 
            t1.postl_to_children_[t1.postl_to_parent_[i-1]][0] == i-1) ||
            t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]] != i-1) {
          // std::cout << "CASE 2" << std::endl;
          del_forest_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1) = std::min(del_forest_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1), 
                                                                      del_f1_subtree_.at(t1.postl_to_parent_[i-1]+1) + df_.at(t1.postl_to_height_[i-1], j) - del_f1_subtree_.at(i));
          del_tree_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1) = std::min(del_tree_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], j-1), 
                                                                      del_t1_subtree_.at(t1.postl_to_parent_[i-1]+1) + dt_.at(t1.postl_to_height_[i-1], j) - del_t1_subtree_.at(i));
          // Copy e into e0.
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t] + 1) = e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t] + 1);
          }
          e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], 0) = e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], 0) + del_t1_subtree_.at(i);
          // Fill next line.
          for (unsigned int t = 1; t < t2.postl_to_children_[j-1].size(); ++t) {
            e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t]+1) = std::min(
              e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t-1]+1) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t-1] + 1),
              std::min(
                e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t]+1) + del_t1_subtree_.at(i),
                e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t-1]+1) + dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t-1] + 1)
              )
            );
          }
        }
        // Case 3: t[i] is the left sibling of the favorable child.
        if (t1.postl_to_left_child_[t1.postl_to_parent_[i-1]] == i-1) {
          // std::cout << "CASE 3" << std::endl;
          int fav_child_postid = t1.postl_to_fav_child_[t1.postl_to_parent_[i-1]];
          // Copy e into e0.
          for (unsigned int t = 0; t < t2.postl_to_children_[j-1].size(); ++t) {
            e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t] + 1) = e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t] + 1);
          }
          e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], 0) = e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], 0) + del_t1_subtree_.at(fav_child_postid);
          // Fill next line.
          for (unsigned int t = 1; t < t2.postl_to_children_[j-1].size(); ++t) {
            e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t]+1) = std::min(
              e_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t-1]+1) + ins_t2_subtree_.at(t2.postl_to_children_[j-1][t-1] + 1),
              std::min(
                e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t]+1) + del_t1_subtree_.at(fav_child_postid),
                e0_.at(t1.postl_to_height_[t1.postl_to_parent_[i-1]], t2.postl_to_children_[j-1][t-1]+1) + fav_child_dt_.at(t1.postl_to_height_[i-1], t2.postl_to_children_[j-1][t-1] + 1)
              )
            );
          }
        }
      }
    }
  }
  
  for (int i = 0; i <= t1_height; ++i) {
    for (int j = 0; j <= t2_input_size; ++j) {
      std::cout << dt_.at(i, j) << "\t";
    }
    std::cout << std::endl;
  }


  return dt_.at(t1_height, t2_input_size);
}