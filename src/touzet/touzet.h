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
#include <limits>
#include <cstdlib>

namespace touzet {

template <typename Label, typename CostModel>
class Algorithm {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<int>& t1_size;
    const data_structures::Matrix<double>& td;
    const std::vector<int>& t1_depth;
    const std::vector<std::vector<int>>& t1_dil;
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
  /// \param d_pruning Enables depth-based pruning (default: false).
  /// \return Tree edit distance regarding k.
  double touzet_ted(const node::Node<Label>& t1, const node::Node<Label>& t2,
      const int& k, const bool d_pruning = false);
  /// Creates a TestItems object and returns it.
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
// Member variables.
private:
  /// Stores the subtree size for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_size_;
  /// Stores the subtree size for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_size_;
  /// Stores references to nodes of the source tree. Indexed in postorder ids
  /// starting with 0.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Smart pointers introduce ownership, but
  // this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores references to nodes of the destination tree. Indexed in postorder-1.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Stores the depth for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_depth_;
  /// Stores the depth for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_depth_;
  /// An inverted list that for each depth value in source tree, stores node ids
  /// of nodes with that depth.
  /// Indexed in depth value.
  std::vector<std::vector<int>> t1_dil_;
  /// An inverted list that for each depth value in source tree, stores node ids
  /// of nodes with that depth.
  /// Indexed in depth value.
  std::vector<std::vector<int>> t2_dil_;
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> td_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> fd_;
  /// Cost model.
  const CostModel c_;
// Member functions.
private:
  /// Calculates the tree edit distance between two subtrees with the remaining
  /// budget of errors, e. Uses dynamic programming, with previously computed
  /// results stored in td_. Itself it fills in fd_ matrix.
  ///
  /// NOTE: x, y are copied.
  ///
  /// \param d_pruning Enables depth-based pruning. Initialised in the called
  ///                   of touzet_ted.
  double tree_dist(const int& x, const int& y, const int& k, const int& e,
      const bool& d_pruning);
  /// Calculates e(x,y) - a budget of the remaining number of errors
  /// (deletions and insertions) that are left for the pair of subtrees
  /// (T1_x,T2_y) after computing the lower bound for the nodes around them.
  ///
  /// e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
  ///
  int e(const int& x, const int& y, const int& k) const;
  /// Verifies if subtrees T1_x and T2_y are k-relevant.
  ///
  /// T1_x and T2_y are k-relevant if
  /// |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
  /// x and y are increased by one due to node indexing starting with 0.
  ///
  /// \param x postorder id of a node in source tree T1.
  /// \param y postorder id of a node in destination tree T2.
  /// \param k maximum number of structural canges.
  /// \return True if subtrees T1_x and T2_y are k-relevant, and false otherwise.
  bool k_relevant(const int& x, const int& y, const int& k) const;
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param depth Vector with node depths.
  /// \param nodes Vector with references to nodes.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& size,
                   std::vector<int>& depth,
                   std::vector<std::vector<int>>& dil,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param depth Vector with node depths.
  /// \param nodes Vector with references to nodes.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  /// \param start_depth Stores the depth of a node during traversal.
  /// \return Number of nodes in the subtree rooted at the caller node.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<int>& size, std::vector<int>& depth,
                             std::vector<std::vector<int>>& dil,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             int& start_postorder, int& start_preorder,
                             int& start_depth);
};

// Implementation details.
#include "touzet_impl.h"

}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_H
