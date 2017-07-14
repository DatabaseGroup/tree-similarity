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

/// \file touzet/touzet.h
///
/// \details
/// Implements the tree edit distance algorithm by Helene Touzet. H. Touzet.
/// Comparing similar ordered trees in linear-time. Journal of Discrete
/// Algorithms. 2007.

#ifndef TREE_SIMILARITY_TOUZET_TOUZET_H
#define TREE_SIMILARITY_TOUZET_TOUZET_H

#include <vector>
#include <memory>
#include "node.h"
#include "matrix.h"
#include <iostream>

namespace touzet {

template <typename Label, typename CostModel>
class Algorithm {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<int>& t1_size;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  Algorithm();
  /// Computes the tree edit distance between two trees assuming a Maximum
  /// number of allowed structural changes (deletions, insertions).
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural changes (deletions,
  ///          insertions).
  /// \return Tree edit distance regarding k.
  double touzet_ted(const node::Node<Label>& t1, const node::Node<Label>& t2,
      const int& k);
  /// Creates a TestItems object and returns it.
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
// Member variables.
private:
  /// Stores the subtree size for each node of the source tree.
  /// Indexed in postorder-1.
  std::vector<int> t1_size_;
  /// Stores the subtree size for each node of the destination tree.
  /// Indexed in postorder-1.
  std::vector<int> t2_size_;
  /// Stores references to nodes of the source tree. Indexed in postorder-1.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Smart pointers introduce ownership, but
  // this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores references to nodes of the destination tree. Indexed in postorder-1.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> td_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> fd_;
  /// Cost model.
  const CostModel c_;
// Member functions.
private:
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param nodes Vector with references to nodes.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& size,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param nodes Vector with references to nodes.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  void index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<int>& size,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             int& start_postorder, int& start_preorder);
};

// Implementation details.
#include "touzet_impl.h"

}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_H
