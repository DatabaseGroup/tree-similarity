// The MIT License (MIT)
// Copyright (c) 2017-2020 Mateusz Pawlik.
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
#include "node.h"
#include "tree_indexer.h"
#include "join_result_element.h"
#include <iostream>

namespace join {

/**
 * Implements a naive tree similarity join as a nested loop and executing
 * tree edit distance (Zhang and Shasha) for each pair of trees from a
 * collection.
 */
template <typename Label, typename VerificationAlgorithm>
class NaiveJoinTI {

public:
  NaiveJoinTI();
  
  /// Executes the join algorithm.
  /**
   * \param trees_collection A vector holding an input collection of trees.
   * \param distance_threshold The maximum number of edit operations that
   *                           differs two trees in the join's result set.
   * \return A vector with the join result.
   */
  std::vector<join::JoinResultElement> execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      const double distance_threshold);
  
  /// Returns the summed subproblem count of TED algorithm executions.
  /**
   * \return sum_subproblem_counter_
   */
  long long int get_subproblem_count() const;

private:
  /// Number of subproblrems encoutered in the verification step.
  long long int sum_subproblem_counter_;
};

// Implementation details.
#include "naive_join_ti_impl.h"

}
