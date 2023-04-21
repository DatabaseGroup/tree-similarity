// The MIT License (MIT)
// Copyright (c) 2019 Mateusz Pawlik
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
#include <iostream>
#include <memory>
#include <cmath>
#include "../node/node.h"
#include "../data_structures/matrix.h"
#include "../label/label_dictionary.h"
#include "../node/tree_indexer.h"
#include "../ted/ted_algorithm.h"

namespace ted_ub {

/**
 * Implements Constrained Tree Edit Distance.
 *
 * NOTE: only node::TreeIndexCTED or its superset can be used with CTEDTreeIndex.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexCTED>
class CTEDTreeIndex : public ted::TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  /// Complexity: O(n^2) time and space.
  double ted(const TreeIndex& t1, const TreeIndex& t2);

  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  /// Complexity: O(n*tau) time and space.
  double ted_k(const TreeIndex& t1, const TreeIndex& t2, const double threshold);

private:
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> dt_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> df_;
  /// TODO
  data_structures::Matrix<double> e_;
};

// Implementation details.
#include "cted_tree_index_impl.h"

}
