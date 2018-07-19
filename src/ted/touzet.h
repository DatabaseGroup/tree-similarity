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
/// Implements the tree edit distance algorithm by Helene Touzet [1]. This
/// algorithm takes an additional parameter k - the maximum number of structural
/// modifications (deletions and insertions). This results in O(nk^3) time and
/// O(nk^2) complexitites.
///
/// [1] H. Touzet. Comparing similar ordered trees in linear-time. Journal of
///     Discrete Algorithms. 2007.

#ifndef TREE_SIMILARITY_TOUZET_TOUZET_H
#define TREE_SIMILARITY_TOUZET_TOUZET_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <cmath>
#include "node.h"
#include "matrix.h"

namespace ted {

/// This class implements the tree edit distance algorithm by Helene Touzet.
/// The method to execute the algorithm is touzet_ted. It takes as input two
/// trees and a parameter k, that is the upper bound for the number of allowed
/// structural modifications (deletions and insertions). If the real TED value
/// is below k, the algorithm return the real TED value.
///
/// This is the memory-improved version. The base algorithm stores the
/// intermediate results values around diagonals of matrices td_ and fd_. These
/// diagonals are shifted left to use a reduced number of columns. The resulting
/// memory complexity is O(nk^3) instead of O(n^2).
///
/// The algorithm is implemented as it operates on rectangular matrices, as in
/// the base version. The translation of indices is done in BandMatrix,
/// transparently to the algorithm's implementation.
///
/// [1] H. Touzet. Comparing similar ordered trees in linear-time. Journal of
///     Discrete Algorithms. 2007.
template <typename Label, typename CostModel>
class Touzet {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<int>& t1_size;
    const data_structures::Matrix<double>& td;
    const data_structures::Matrix<double>& fd;
    const std::vector<int>& t1_depth;
    const std::vector<int>& t1_kr;
    const std::vector<int>& t1_lch;
    const std::vector<int>& t1_parent;
    const std::vector<int>& t1_nodes_kr;
    const std::vector<std::vector<int>>& t1_dil;
    const std::vector<int>& t1_max_subtree_depth;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  Touzet();
  /// Computes the tree edit distance between two trees assuming a maximum
  /// number of allowed structural modifications (deletions, insertions).
  ///
  /// This is an implementation of the original Touzet's algorithm WITHOUT
  /// depth-based pruning.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural modifications (deletions,
  ///          insertions).
  /// \return Tree edit distance regarding k.
  double touzet_ted(const node::Node<Label>& t1, const node::Node<Label>& t2,
      const int k);
  /// Computes the tree edit distance between two trees assuming a maximum
  /// number of allowed structural modifications (deletions, insertions).
  ///
  /// This is an implementation of the original Touzet's algorithm WITH
  /// depth-based pruning.
  ///
  /// NOTE: It involves a theoreticaly linear step to find the first node in
  ///       the truncated tree (Def. 12 in [1]). See the comments in the code.
  ///       We do it differently than in [1]. 
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural modifications (deletions,
  ///          insertions).
  /// \return Tree edit distance regarding k.
  double touzet_ted_depth_pruning(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Fixes the problem from NOTE in touzet_ted_depth_pruning. Nodes in
  /// truncated subtree are traversed twice: once to collect them, second time
  /// to compute the ted values.
  double touzet_ted_depth_pruning_truncated_tree_fix(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Computes the tree edit distance between two trees assuming a maximum
  /// number of allowed structural modifications (deletions, insertions).
  ///
  /// This is an optimized version compared to Touzet. It uses keyroot nodes
  /// (as in Zhang and Shasha) not to recompute subtree-pair distances.
  ///
  /// The idea is to execute forest distance only once for all relevant
  /// subtree pairs being on the left paths of the same root node pair.
  ///
  /// It loops over all keyroot node pairs and finds the top nodes on their
  /// left paths. See source code for details.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural modifications (deletions,
  ///          insertions).
  /// \return Tree edit distance regarding k.
  double touzet_ted_kr_loop(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Inteface function to execute touzet_ted_kr_loop with computing the
  /// e_max value. Currently for experiments only.
  double touzet_ted_kr_loop_e_max(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Inteface function to execute touzet_ted_kr_loop without computing the
  /// e_max value. Currently for experiments only.
  double touzet_ted_kr_loop_no_e_max(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Computes the tree edit distance between two trees assuming a maximum
  /// number of allowed structural modifications (deletions, insertions).
  ///
  /// This is an optimized version compared to Touzet. It uses keyroot nodes
  /// (as in Zhang and Shasha) not to recompute subtree-pair distances.
  ///
  /// The idea is to execute forest distance only once for all relevant
  /// subtree pairs being on the left paths of the same root node pair.
  ///
  /// It loops over all node pairs in the k-strip and identifies the top
  /// nodes on the left paths. See source code for details.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural modifications (deletions,
  ///          insertions).
  /// \return Tree edit distance regarding k.
  double touzet_ted_kr_set(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Inteface function to execute touzet_ted_kr_set with computing the
  /// e_max value. Currently for experiments only.
  double touzet_ted_kr_set_e_max(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Inteface function to execute touzet_ted_kr_set without computing the
  /// e_max value. Currently for experiments only.
  double touzet_ted_kr_set_no_e_max(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Verifies if the tree edit distance between trees t1 and t1 is smaller or
  /// equal than the similarity_threshold value.
  ///
  /// NOTE: Here, only unit cost model is allowed. similarity_threshold is
  ///       converted to integer value using std::ceil and static cast.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param similarity_threshold Maximum value of the tree edit distance.
  /// \return Tree edit distance value if it smaller or equal than the
  ///         similarity_threshold, and std::numeric_limits<double>::infinity()
  ///         otherwise.
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double similarity_threshold);
  /// Creates a TestItems object and returns it (by value).
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  const unsigned long long int get_subproblem_count() const;
  /// For experiments only.
  const unsigned long long int get_top_y_update_count() const;
// Member variables.
private:
  /// The size of the source tree.
  int t1_input_size_;
  /// The size of the destination tree.
  int t2_input_size_;
  /// Stores the subtree size for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_size_;
  /// Stores the subtree size for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_size_;
  /// Stores references to nodes of the source tree. Indexed in postorder ids
  /// starting with 0.
  ///
  /// NOTE: We use reference_wrapper for const references to nodes. For now, we
  /// decided not to use raw pointers. Smart pointers introduce ownership, but
  /// this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores references to nodes of the destination tree. Indexed in postorder-1.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Stores the depth for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_depth_;
  /// Stores the depth for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_depth_;
  /// Key-root nodes of the source tree.
  std::vector<int> t1_kr_;
  /// Key-root nodes of the destination tree.
  std::vector<int> t2_kr_;
  /// Stores the postorder id of the left child for each node of the source
  /// tree. '-1' represents no child.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_lch_;
  /// Stores the postorder id of the left child for each node of the destination
  /// tree. '-1' represents no child.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_lch_;
  /// Stores the postorder id of the parent for each node of the source
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_parent_;
  /// Stores the postorder id of the parent for each node of the destination
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_parent_;
  /// Stores the postorder id of the nearest keyroot-ancestor for each node of
  /// the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_nodes_kr_;
  /// Stores the postorder id of the nearest keyroot-ancestor for each node of
  /// the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_nodes_kr_;
  /// An inverted list that for each depth value in source tree, stores node ids
  /// of nodes with that depth.
  /// Indexed in depth value.
  std::vector<std::vector<int>> t1_dil_;
  /// An inverted list that for each depth value in destination tree, stores
  /// node ids of nodes with that depth.
  /// Indexed in depth value.
  std::vector<std::vector<int>> t2_dil_;
  /// Stores the maximum depth for each subtree of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_subtree_max_depth_;
  /// Stores the maximum depth for each subtree of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_subtree_max_depth_;
  /// Matrix storing subtree distances.
  data_structures::BandMatrix<double> td_;
  /// Matrix storing subforest distances.
  data_structures::BandMatrix<double> fd_;
  /// Cost model.
  const CostModel c_;
  /// Constant used in touzet_ted_kr_set for shifting bits in packing two
  /// values in a single long long (we need 32 bits for each value to pack in
  /// order to accomodate input tree sizes beyond 60K nodes).
  const short kBitsToShift = 32;
  /// Subproblem counter - for experiments only. Counts the number of
  /// non-trivial values filled in fd_ matrix: subproblems where both forests
  /// are not empty (including storing first infinities outside e-strip), and
  /// last value computed in fd_.
  unsigned long long int subproblem_counter;
  /// Counts the number of times top_y value is updated in touzet_ted_kr_set
  /// or touzet_ted_kr_loop functions.
  unsigned long long int top_y_update_counter;
  /// Specifies if e_max value should be computed in touzet_ted_kr_set or
  /// touzet_ted_kr_loop functions (default: true).
  bool compute_e_max;
// Member functions.
private:
  /// Resets and initialises algorithm's internal data structures and constants.
  /// Has to be called before computing the distance.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum number of allowed structural modifications (deletions,
  ///          insertions).
  void init(const node::Node<Label>& t1, const node::Node<Label>& t2, const int k);
  /// Calculates the tree edit distance between two subtrees with the remaining
  /// budget of errors, e. Uses dynamic programming, with previously computed
  /// results stored in td_. Itself it fills in fd_ matrix.
  ///
  /// This is implementation of the original Touzet's algorithm with
  /// a modification to store intermediate subtree pair distances. Used for
  /// the algorithm versions that remove redundancy by useng keyroot nodes.
  ///
  /// \param x Postorder ID of a subtree in the source tree.
  /// \param y Postorder ID of a subtree in the destination tree.
  /// \param k Original threshold for the number of structural modifications.
  /// \param e The remaining budget of structural modifications for (x,y).
  double tree_dist(const int x, const int y, const int k, const int e);
  /// Calculates the tree edit distance between two subtrees with the remaining
  /// budget of errors, e. Uses dynamic programming, with previously computed
  /// results stored in td_. Itself it fills in fd_ matrix.
  ///
  /// This is implementation of the original Touzet's algorithm, including
  /// depth-based pruning.
  ///
  /// \param x Postorder ID of a subtree in the source tree.
  /// \param y Postorder ID of a subtree in the destination tree.
  /// \param k Original threshold for the number of structural modifications.
  /// \param e The remaining budget of structural modifications for (x,y).
  double tree_dist_depth_pruning(const int x, const int y, const int k, const int e);
  /// This version of touzet_ted_depth_pruning ensures that only the nodes
  /// in trucated tree are traversed. We ttraverse them once to find them, and
  /// then second time to actually compute the distances.
  double tree_dist_depth_pruning_truncated_tree_fix(const int x, const int y, const int k, const int e);
  /// Calculates e(x,y) - a budget of the remaining number of errors
  /// (deletions and insertions) that are left for the pair of subtrees
  /// (T1_x,T2_y) after computing the lower bound for the nodes around them.
  ///
  /// \param x Postorder ID of a subtree in the source tree.
  /// \param y Postorder ID of a subtree in the destination tree.
  /// \param k Original threshold for the number of structural modifications.
  /// \return e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
  int e_budget(const int x, const int y, const int k) const;
  /// Verifies if subtrees T1_x and T2_y are k-relevant.
  ///
  /// T1_x and T2_y are k-relevant if
  /// |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k.
  ///
  /// NOTE: x and y are increased by one due to node indexing starting with 0.
  ///
  /// \param x postorder id of a node in source tree T1.
  /// \param y postorder id of a node in destination tree T2.
  /// \param k maximum number of structural canges.
  /// \return True if subtrees T1_x and T2_y are k-relevant, and false otherwise.
  bool k_relevant(const int x, const int y, const int k) const;
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion. This method fills in the passed vectors.
  ///
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param depth Vector with node depths.
  /// \param subtree_max_depth Vector with subtree maximum depths.
  /// \param dil Inverted list of node depths.
  /// \param nodes Vector with references to nodes.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& size,
                   std::vector<int>& depth,
                   std::vector<int>& kr,
                   std::vector<int>& lch,
                   std::vector<int>& parent,
                   std::vector<int>& subtree_max_depth,
                   std::vector<std::vector<int>>& dil,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param size Vector with subtree sizes.
  /// \param depth Vector with node depths.
  /// \param subtree_max_depth Vector with subtree maximum depths.
  /// \param dil Inverted list of node depths.
  /// \param nodes Vector with references to nodes.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  ///                        Modified by recursion.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  ///                       Modified by recursion.
  /// \param start_depth Stores the depth of a node, incremented during traversal.
  /// \param parent_max_depth Used to update parent subtree's max depth.
  /// \return Number of nodes in the subtree rooted at the caller node.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<int>& size, std::vector<int>& depth,
                             std::vector<int>& kr,
                             std::vector<int>& lch,
                             std::vector<int>& parent,
                             std::vector<int>& subtree_max_depth,
                             std::vector<std::vector<int>>& dil,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             int& start_postorder, int& start_preorder,
                             unsigned int start_depth, int& parent_max_depth);
  /// For each node, stores the postorder id of its nearest keyroot-ancestor.
  ///
  /// NOTE: Complexity is linear in the tree size.
  ///
  /// TODO: Try to do this while indexing the nodes.
  ///
  /// \param nodes_kr Vector with nodes keyroots.
  /// \param lch Vector with leftmost children.
  /// \param kr Vector with keyroot nodes.
  void fill_nodes_kr(std::vector<int>& nodes_kr, std::vector<int>& lch,
      std::vector<int>& kr);
};

// Implementation details.
#include "touzet_impl.h"

}

#endif // TREE_SIMILARITY_TOUZET_TOUZET_H
