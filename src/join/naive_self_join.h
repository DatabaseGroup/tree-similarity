// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file join/naive_self_join.h
///
/// \details
/// Implements a naive tree similarity join as a nested loop and executing
/// tree edit distance (Zhang and Shasha) for each pair of trees from a
/// collection.

#ifndef TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_H
#define TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_H

#include <vector>
#include "node.h"
#include "zhang_shasha.h"

namespace join {

/// This is an element of the join's result set.
struct JoinResultElement {
  /// Tree id of the left-hand tree in the result element.
  int tree_id_1;
  /// Tree id of the right-hand tree in the result element.
  int tree_id_2;
  /// Tree edit distance between tree_id_1 and tree_id_2.
  double ted_value;
  /// Constructor.
  JoinResultElement(int tree_id_1, int tree_id_2, double ted_value);
};

/// Represents a naive self tree similarity join.
template <typename Label, typename CostModel>
class NaiveSelfJoin {
// Member functions.
public:
  /// Constructor.
  NaiveSelfJoin();
  /// Executes the join algorithm.
  ///
  /// \param trees_collection A vector holding all trees from the relation to
  ///                         self-join.
  /// \param similarity_threshold The maximum number of edit operations that
  ///                             differs two trees in the join's result set.
  /// \return A vector with the join result.
  std::vector<join::JoinResultElement> execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      const double similarity_threshold) const;
// Member variables.
private:
  /// Cost model.
  ///
  /// NOTE: This join supports only unit cost model.
  const CostModel c_;
};

// Implementation details.
#include "naive_self_join_impl.h"

}

#endif // TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_H
