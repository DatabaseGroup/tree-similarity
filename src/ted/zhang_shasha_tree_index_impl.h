// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
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

/// Contains the implementation of the Zhang and Shasha algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double ZhangShashaTreeIndex<CostModel, TreeIndex>::ted(
    const TreeIndex& t1, const TreeIndex& t2) {
  
  using data_structures::Matrix;

  const int kT1Size = t1.tree_size_;
  const int kT2Size = t2.tree_size_;

  // NOTE: The default constructor of Matrix is called while constructing ZS-Algorithm.
  // NOTE: Shouldn't we implement Matrix::resize() instead of constructing matrix again?
  td_ = Matrix<double>(kT1Size+1, kT2Size+1);
  fd_ = Matrix<double>(kT1Size+1, kT2Size+1);

  // Reset subproblem counter.
  subproblem_counter_ = 0;

  // Nested loop over key-root node pairs.
  for (auto kr1 : t1.list_kr_) {
    for (auto kr2 : t2.list_kr_) {
      // '+1' due to previous ZhangShasha postorder starting with 1.
      forest_distance(t1, t2, kr1 + 1, kr2 + 1);
    }
  }

  return td_.at(kT1Size, kT2Size);
}

template <typename CostModel, typename TreeIndex>
void ZhangShashaTreeIndex<CostModel, TreeIndex>::forest_distance(
    const TreeIndex& t1,
    const TreeIndex& t2,
    int kr1,
    int kr2) {
  const int kKr1Lld = t1.postl_to_lld_[kr1 - 1]+1; // See declaration of t1_lld_.
  const int kKr2Lld = t2.postl_to_lld_[kr2 - 1]+1;
  const int kT1Empty = kKr1Lld - 1;
  const int kT2Empty = kKr2Lld - 1;
  // Distance between two empty forests.
  fd_.at(kT1Empty, kT2Empty) = 0.0;

  // Distances between a source forest and an empty forest.
  for (int i = kKr1Lld; i <= kr1; ++i) {
    fd_.at(i, kT2Empty) = fd_.at(i - 1, kT2Empty) + c_.del(t1.postl_to_label_id_[i - 1]);
    // For t1_node_[i - 1] see declaration of t1_node_.
  }

  // Distances between a destination forest and an empty forest.
  for (int j = kKr2Lld; j <= kr2; ++j) {
    fd_.at(kT1Empty, j) = fd_.at(kT1Empty, j - 1) + c_.ins(t2.postl_to_label_id_[j - 1]);
  }

  // Distances between non-empty forests.
  for (int i = kKr1Lld; i <= kr1; ++i) {
    for (int j = kKr2Lld; j <= kr2; ++j) {
      ++subproblem_counter_;
      // If we have two subtrees.
      if (t1.postl_to_lld_[i - 1]+1 == kKr1Lld && t2.postl_to_lld_[j - 1]+1 == kKr2Lld) {
        fd_.at(i, j) = std::min(
            {fd_.at(i - 1, j) + c_.del(t1.postl_to_label_id_[i - 1]), // Delete root node in source subtree.
             fd_.at(i, j - 1) + c_.ins(t2.postl_to_label_id_[j - 1]), // Insert root node in destination subtree.
             fd_.at(i - 1, j - 1) + c_.ren(t1.postl_to_label_id_[i - 1], t2.postl_to_label_id_[j - 1])}); // Rename the root nodes.
        td_.at(i, j) = fd_.at(i, j);
      } else { // We have two forests.
        fd_.at(i, j) = std::min(
            {fd_.at(i - 1, j) + c_.del(t1.postl_to_label_id_[i - 1]), // Delete rightmost root node in source subforest.
             fd_.at(i, j - 1) + c_.ins(t2.postl_to_label_id_[j - 1]), // Insert rightmost root node in destination subforest.
             fd_.at(t1.postl_to_lld_[i - 1]+1 - 1, t2.postl_to_lld_[j - 1]+1 - 1) + td_.at(i, j)}); // Delete the rightmost subtrees + keep the rightmost subtrees.
      }
      // std::cout << "--- fd[" << i << "][" << j << "] = " << fd_.at(i, j) << std::endl;
    }
  }
  // std::cout << "--- td[" << kr1 << "][" << kr2 << "] = " << td_.at(kr1, kr2) << std::endl;
}
