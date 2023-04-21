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
#include "../node/node.h"
#include "../data_structures/matrix.h"
#include "ted_algorithm_touzet.h"

namespace ted {

/**
 * This is an implementation of the original Touzet's algorithm WITHOUT
 * depth-based pruning.
 *
 * This is an optimized version. It uses keyroot nodes (as in Zhang and Shasha)
 * not to recompute subtree-pair distances.
 *
 * The idea is to execute forest distance only once for all relevant
 * subtree pairs being on the left paths of the same root node pair.
 *
 * It loops over all keyroot node pairs and finds the top nodes on their
 * left paths. See source code for details.
 *
 * The difference to TouzetKRLoopTreeIndex is in using a set to collect
 * key root pairs for which `tree_dist` function is executed.
 *
 * NOTE: Only node::TreeIndexTouzetKRSet or its superset can be used with
 *       TouzetKRSetTreeIndex.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexTouzetKRSet>
class TouzetKRSetTreeIndex : public TEDAlgorithmTouzet<CostModel, TreeIndex> {

public:
  
  // Base class members made visible for this class.
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::td_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::fd_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::init_matrices;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::subproblem_counter_;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::e_budget;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::k_relevant;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::TEDAlgorithmTouzet;
  using TEDAlgorithmTouzet<CostModel, TreeIndex>::tree_dist;

  double ted_k(const TreeIndex& t1, const TreeIndex& t2, const int k);
};

// Implementation details.
#include "touzet_kr_set_tree_index_impl.h"

}
