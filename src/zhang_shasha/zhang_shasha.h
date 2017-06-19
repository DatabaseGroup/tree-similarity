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

#include <vector>
#include "node.h"
#include "matrix.h"

#include <iostream>

namespace zhang_shasha {

template <typename Label, typename CostModel>
class Algorithm {
// Member functions.
public:
  // TODO: Do we need a Constructor?
  // Algorithm();

  /// Computes the tree edit distance between two trees.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param c Cost model with the costs of edit operations.
  /// \return Tree edit distance value.
  double zhang_shasha_ted(const node::Node<Label>& t1, const node::Node<Label>& t2, const CostModel& c);
// Member variables.
private:
  /// Key-root nodes of the source tree.
  std::vector<int> kr1_;
  /// Key-root nodes of the destination tree.
  std::vector<int> kr2_;
  /// Stores the postorder of the leftmost leaf descendant for each node of the
  /// source tree.
  /// Index in postorder.
  std::vector<int> lld1_;
  /// Stores the postorder of the leftmost leaf descendant for each node of the
  /// destination tree.
  /// Index in postorder.
  std::vector<int> lld2_;
  /// Matrix storing subtree distances.

  /// Matrix storing subforest distances.
// Member functions.
private:
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param lld Vector with postorder ids of the leftmost leaf descendants.
  /// \param kr Vector with postorder ids of the key-root nodes.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& lld, std::vector<int>& kr);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param lld Vector with postorder ids of the leftmost leaf descendants.
  /// \param kr Vector with postorder ids of the key-root nodes.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  void index_nodes_recursion(const node::Node<Label>& root, std::vector<int>& lld, std::vector<int>& kr, int& start_postorder, int& start_preorder);
  /// Calculate distances for subforests and store distances for subtrees.
  ///
  ///
  void forest_distance();
};

// Implementation details.
#include "zhang_shasha_impl.h"

}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_H
