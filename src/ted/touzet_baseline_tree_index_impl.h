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

/// Contains the implementation of the Touzet Baseline algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double TouzetBaselineTreeIndex<CostModel, TreeIndex>::ted_k(const TreeIndex& t1,
    const TreeIndex& t2, const int k) {
  
  const int t1_size = t1.tree_size_;
  const int t2_size = t2.tree_size_;

  init_matrices(t1_size, k);
  
  // Reset subproblem counter.
  subproblem_counter_ = 0;

  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_size - t2_size) > k) {
    return std::numeric_limits<double>::infinity();
  }
  
  // Nested loop over all node pairs in k-strip : |x-y|<=k. This loop iterates
  // over all node pairs from k-strip, and verifies their k-relevancy.
  for (int x = 0; x < t1_size; ++x) {
    for (int y = std::max(0, x - k); y <= std::min(x + k, t2_size-1); ++y) {
      if (k_relevant(t1, t2, x, y, k)) {
        // Compute td(x, y) with e errors - the value of e(x, y, k).
        td_.at(x, y) = tree_dist(t1, t2, x, y, k, e_budget(t1, t2, x, y, k));
      }
    }
  }
  
  return td_.at(t1.tree_size_-1, t2.tree_size_-1);
}