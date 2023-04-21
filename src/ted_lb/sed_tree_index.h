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
#include <cmath>
#include "../node/node.h"
#include "../label/label_dictionary.h"
#include "../node/tree_indexer.h"
#include "../ted/ted_algorithm.h"

namespace ted_lb {

/**
 * Implements String Edit Distance using preorder and postorder traversals.
 *
 * This is a memory-efficient version using two vectors instead of a matrix.
 *
 * NOTE: This is unit-cost only implaementation. CostModel is not used.
 *
 * NOTE: only node::TreeIndexSED or its superset can be used with SEDTreeIndex.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexSED>
class SEDTreeIndex : public ted::TEDAlgorithm<CostModel, TreeIndex>{

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);

};

// Implementation details.
#include "sed_tree_index_impl.h"

}
