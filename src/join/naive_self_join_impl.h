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

/// \file join/naive_self_join_impl.h
///
/// \details
/// Contains the implementation of a naive tree similarity self join.

#ifndef TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_IMPL_H
#define TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_IMPL_H

template <typename Label, typename CostModel>
NaiveSelfJoin<Label, CostModel>::NaiveSelfJoin() : c_() {}

template <typename Label, typename CostModel>
std::vector<join::JoinResultElement> NaiveSelfJoin<Label, CostModel>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    const double similarity_threshold) const {

  std::vector<join::JoinResultElement> result_set;

  ted::ZhangShasha<Label, CostModel> ted_algorithm;

  int i = 0;
  int j = 0;
  for (auto it_i = trees_collection.begin(); it_i != trees_collection.end(); ++it_i) {
    ++i;
    j = i;
    // Start the inner loop with the tree just iafter it_i.
    for (auto it_j = it_i+1; it_j != trees_collection.end(); ++it_j) {
      ++j;
      double ted_value = ted_algorithm.zhang_shasha_ted(*it_i, *it_j);
      if (ted_value <= similarity_threshold) {
        result_set.emplace_back(i, j, ted_value);
      }
    }
  }

  return result_set;
}

join::JoinResultElement::JoinResultElement(int tree_id_1, int tree_id_2, double ted_value)
    : tree_id_1(tree_id_1), tree_id_2(tree_id_2), ted_value(ted_value) {};

#endif // TREE_SIMILARITY_JOIN_NAIVE_SELF_JOIN_IMPL_H
