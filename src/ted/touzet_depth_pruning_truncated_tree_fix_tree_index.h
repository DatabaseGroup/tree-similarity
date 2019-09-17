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

#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <cmath>
#include "node.h"
#include "matrix.h"
#include "ted_algorithm_touzet.h"
#include "touzet_baseline_tree_index.h"

namespace ted {

/**
 * This is an implementation of the original Touzet's algorithm with
 * depth-based pruning.
 *
 * This version ensures that only the nodes
 * in trucated tree are traversed. We traverse them once to find them, and
 * then second time to actually compute the distances.
 *
 * NOTE: Only node::TreeIndexTouzetDepthPruningTruncatedTreeFix or its superset
 *       can be used with TouzetDepthPruningTruncatedTreeFixTreeIndex.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexTouzetDepthPruningTruncatedTreeFix>
class TouzetDepthPruningTruncatedTreeFixTreeIndex :
    public TouzetBaselineTreeIndex<CostModel, TreeIndex> {

public:

  // Base class members made visible for this class.
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::td_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::fd_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::e_budget;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::k_relevant;
  using TouzetBaselineTreeIndex<CostModel, TreeIndex>::TouzetBaselineTreeIndex;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::c_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::subproblem_counter_;

  // Depth pruning heuristic requires a different `tree_dist` function.
  // That from TEDAlgorithmTouzet is overriden here.
  // It additionally needed `k` value - added to arguments.
  double tree_dist(const TreeIndex& t1, const TreeIndex& t2, const int x,
      const int y, const int k, const int e);
  
  // `ted_k` funtion is inherited from TouzetBaselineTreeIndex.
  // double ted_k(const TreeIndex& t1, const TreeIndex& t2, const int k);
};

// Implementation details.
#include "touzet_depth_pruning_truncated_tree_fix_tree_index_impl.h"
}
