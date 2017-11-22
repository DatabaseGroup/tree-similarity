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

/// \file ted/apted.h
///
/// \details
/// Implements the state-of-the-art tree edit distance algorithm APTED+ by
/// Pawlik and Augsten [1,2,3,4].
///
/// [1] M.Pawlik and N.Augsten. RTED: A Robust Algorithm for the Tree Edit
///     Distance. PVLDB. 2011.
/// [2] M.Pawlik and N.Augsten. A Memory-Efficient Tree Edit Distance Algorithm.
///     DEXA. 2014.
/// [3] M. Pawlik and N. Augsten. Efficient Computation of the Tree Edit
///     Distance. ACM Transactions on Database Systems (TODS). 2015.
/// [4] M. Pawlik and N. Augsten. Tree edit distance: Robust and
///     memory-efficient. Information Systems. 2016.

#ifndef TREE_SIMILARITY_TED_APTED_H
#define TREE_SIMILARITY_TED_APTED_H

#include <vector>
#include <memory>
#include "node.h"
#include "matrix.h"
#include <iostream>
#include <limits>
#include <cstdlib>

namespace ted {

template <typename Label>
class NodeIndexer {
public:
  // Structure indices.
  std::vector<std::reference_wrapper<const node::Node<Label>>> preL_to_node;
  std::vector<int> preL_to_size;
  std::vector<int> preL_to_parent;
  std::vector<std::vector<int>> preL_to_children;
  std::vector<int> postL_to_lld;
  std::vector<int> postR_to_rld;
  std::vector<int> preL_to_ln;
  std::vector<int> preR_to_ln;
  std::vector<bool> preL_to_node_type_L;
  std::vector<bool> preL_to_node_type_R;
  // Traversal translation indices.
  std::vector<int> preL_to_preR;
  std::vector<int> preR_to_preL;
  std::vector<int> preL_to_postL;
  std::vector<int> postL_to_preL;
  std::vector<int> preL_to_postR;
  std::vector<int> postR_to_preL;
  // Cost indices.
  std::vector<int> preL_to_kr_sum;
  std::vector<int> preL_to_rev_kr_sum;
  std::vector<int> preL_to_desc_sum;
  std::vector<int> preL_to_sumDelCost;
  std::vector<int> preL_to_sumInsCost;
  // Structure single-value variables.
  int tree_size;
  int lshl;
  int rchl;
private:
  // Variables holding values modified at runtime while the algorithm executes.
  int current_node;
  // Variables used temporarily while indexing.
  int size_tmp;
  int desc_sizes_tmp;
  int kr_sizes_sum_tmp;
  int rev_kr_sizes_sum_tmp;
  int preorder_tmp;
public:
  NodeIndexer(const node::Node<Label>& t);
private:
  int index_nodes(const node::Node<Label>& node, int postorder);
  void post_traversal_indexing();
public:
  int preL_to_lld(int preL);
  int preL_to_rld(int preL);
  node::Node<Label>& postL_to_node(int postL);
  node::Node<Label>& postR_to_node(int postR);
  int get_size();
  bool is_leaf(int node);
  int get_current_node();
  void set_current_node(int preorder);
};

/// This class implements the APTED algorithm by Pawlik and Augsten.
template <typename Label, typename CostModel>
class APTED {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<int>& preL_to_size;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  APTED();
  /// Computes the tree edit distance between two trees.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \return Tree edit distance between t1 and t2.
  double apted_ted(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// Creates a TestItems object and returns it (by value).
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  const unsigned long long int get_subproblem_count() const;
// Member variables.
private:
  // /// Stores the subtree size for each node of the source tree.
  // /// Indexed in preorder ids starting with 0.
  // std::vector<int> t1_size_;
  // /// Stores the subtree size for each node of the destination tree.
  // /// Indexed in preorder ids starting with 0.
  // std::vector<int> t2_size_;
  // /// Stores references to nodes of the source tree. Indexed in preorder ids
  // /// starting with 0.
  // ///
  // /// NOTE: We use reference_wrapper for const references to nodes. For now, we
  // /// decided not to use raw pointers. Smart pointers introduce ownership, but
  // /// this vector is only an observer from a logical point of view.
  // std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  // /// Stores references to nodes of the destination tree. Indexed in postorder-1.
  // std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Matrix storing subtree distances.
  data_structures::Matrix<double> delta_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> s_;
  /// Matrix storing subforest distances.
  data_structures::Matrix<double> t_;
  /// Cost model.
  const CostModel c_;
  /// Subproblem counter - for experiments only. Counts the number of
  /// non-trivial values filled in fd_ matrix: subproblems where both forests
  /// are not empty (including storing first infinities outside e-strip), and
  /// last value computed in fd_.
  unsigned long long int subproblem_counter;
// Member functions.
private:
  // /// Indexes the nodes of an input tree. Wrapper for the recursive
  // /// index_nodes_recursion. This method fills in the passed vectors.
  // ///
  // /// Call this method to index nodes.
  // ///
  // /// \param root The root node of the tree to index.
  // /// \param size Vector with subtree sizes.
  // /// \param nodes Vector with references to nodes.
  // void index_nodes(const node::Node<Label>& root, std::vector<int>& size,
  //                  std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes);
  // /// Traverses an input tree rooted at root recursively and collects
  // /// information into index structures.
  // ///
  // /// \param root The root node of the tree to index.
  // /// \param size Vector with subtree sizes.
  // /// \param nodes Vector with references to nodes.
  // /// \param start_postorder Stores the postorder id of a node during traversal.
  // ///                        Modified by recursion.
  // /// \param start_preorder Stores the preorder id of a node during traversal.
  // ///                       Modified by recursion.
  // /// \return Number of nodes in the subtree rooted at the caller node.
  // int index_nodes_recursion(const node::Node<Label>& root,
  //                            std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
  //                            int& start_postorder, int& start_preorder);
};

// Implementation details.
#include "apted_impl.h"

}

#endif // TREE_SIMILARITY_TED_APTED_H
