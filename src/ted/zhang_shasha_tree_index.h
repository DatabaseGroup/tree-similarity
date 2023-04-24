// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik
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
#include "../node/node.h"
#include "../data_structures/matrix.h"
#include <iostream>
#include "../node/tree_indexer.h"
#include "ted_algorithm.h"

namespace ted {

/**
 * Implements the tree edit distance algorithm by Zhang and Shasha. K.Zhang and
 * D.Shasha. Simple fast algorithms for the editing distance between trees and
 * related problems. SIAM J. Comput. 1989.
 *
 * NOTE: only node::TreeIndexZhangShasha can be used with ZhangShasha.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexZhangShasha>
class ZhangShashaTreeIndex : public TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using TEDAlgorithm<CostModel, TreeIndex>::c_;
  using TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using TEDAlgorithm<CostModel, TreeIndex>::ted;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);
  
private:
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> td_;
  
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> fd_;
  
  /// Calculates distances for subforests and stores distances for subtrees.
  /**
   * \param kr1 Current keyroot node in source tree.
   * \param kr2 Current keyroot node in destination tree.
   */
  void forest_distance(const TreeIndex& t1, const TreeIndex& t2,
      int kr1, int kr2);
};

// Implementation details.
#include "zhang_shasha_tree_index_impl.h"

}