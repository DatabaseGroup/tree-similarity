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

/// \file zhang_shasha/zhang_shasha.h
///
/// \details
/// Implements the tree edit distance algorithm by Zhang and Shasha. K.Zhang and
/// D.Shasha. Simple fast algorithms for the editing distance between trees and
/// related problems. SIAM J. Comput. 1989.

#ifndef TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_H
#define TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_H

#include "node.h"

#include <iostream>

namespace zhang_shasha {

template <typename Label, typename CostModel>
class Algorithm {
// Member functions.
public:
  // template <typename Label, typename CostModel>
  double zhang_shasha_ted(node::Node<Label>& t1, node::Node<Label>& t2, CostModel& c);
// Member variables.
// Member functions.
private:
  // template <typename Label>
  void traverse_subtree(node::Node<Label>& node);
  // template <typename Label>
  void get_key_roots(node::Node<Label>& root);
};

// Implementation details.
#include "zhang_shasha_impl.h"

}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_H
