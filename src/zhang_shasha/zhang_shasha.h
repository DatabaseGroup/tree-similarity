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
#include <memory>
#include "node.h"
#include "matrix.h"
#include <iostream>

namespace zhang_shasha {

template <typename Label, typename CostModel>
class Algorithm {
// Member functions.
public:
  // Constructor.
  Algorithm(const node::Node<Label>& t1, const node::Node<Label>& t2, const CostModel& c);
  /// Computes the tree edit distance between two trees.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param c Cost model with the costs of edit operations.
  /// \return Tree edit distance value.
  double zhang_shasha_ted();

  const std::vector<int>& get_t1_kr_() const;
// Member variables.
private:
  /// Size of the source tree.
  int t1_size_;
  /// Size of the destination tree.
  int t2_size_;
  /// Key-root nodes of the source tree.
  std::vector<int> t1_kr_;
  /// Key-root nodes of the destination tree.
  std::vector<int> t2_kr_;
  /// Stores the postorder of the leftmost leaf descendant for each node of the
  /// source tree.
  /// Indexed in postorder-1.
  std::vector<int> t1_lld_;
  /// Stores the postorder of the leftmost leaf descendant for each node of the
  /// destination tree.
  /// Indexed in postorder-1.
  std::vector<int> t2_lld_;
  /// Stores pointers to nodes of the source tree. Indexed in postorder-1.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Other pointers introduce ownership, but
  // this vector is only an observer from a logical pont of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores pointers to nodes of the destination tree. Indexed in postorder-1.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> td_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> fd_;
  /// Source tree.
  const node::Node<Label>& t1_;
  /// Destination tree.
  const node::Node<Label>& t2_;
  /// Cost model.
  const CostModel& c_;
// Member functions.
private:
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param lld Vector with postorder ids of the leftmost leaf descendants.
  /// \param kr Vector with postorder ids of the key-root nodes.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& lld,
                   std::vector<int>& kr,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param lld Vector with postorder ids of the leftmost leaf descendants.
  /// \param kr Vector with postorder ids of the key-root nodes.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  void index_nodes_recursion(const node::Node<Label>& root, std::vector<int>& lld,
                             std::vector<int>& kr,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             int& start_postorder, int& start_preorder);
  /// Calculate distances for subforests and stores distances for subtrees.
  ///
  /// \param td Matrix storing subtree distances.
  /// \param fd Matrix storing subforest distances - not to create it each time forest_distance is executed.
  /// \param kr1 Current key-root node in source tree.
  /// \param kr2 Current key-root node in destination tree.
  void forest_distance(int kr1, int kr2);
};

// Implementation details.
#include "zhang_shasha_impl.h"

}

#endif // TREE_SIMILARITY_ZHANG_SHASHA_ZHANG_SHASHA_H
