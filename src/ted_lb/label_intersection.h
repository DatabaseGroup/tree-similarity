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

#pragma once

#include <vector>
#include <cmath>
#include "../node/node.h"
#include "../label/label_dictionary.h"
#include "../node/tree_indexer.h"
#include "../ted/ted_algorithm.h"

namespace ted_lb {

/**
 * Implements the label intersection lower bound for trees.
 */
template <typename CostModel, typename TreeIndex>
class LabelIntersection : public ted::TEDAlgorithm<CostModel, TreeIndex>{

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;

public:
  /// This function computes the label intersection.
  double ted(const TreeIndex& t1, const TreeIndex& t2);
};

// Implementation details.
#include "label_intersection_impl.h"

}
