// The MIT License (MIT)
// Copyright (c) 2018 Mateusz Pawlik
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

/// \file ted_ub/greedy_ub.h
///
/// \details
/// Implements greedy mapping of the matching labels with the assumption that
/// similar trees should match many labels and should have similar structure.
/// NOTE: Works with unit cost only.

#ifndef TREE_SIMILARITY_TED_UB_GREEDY_UB_H
#define TREE_SIMILARITY_TED_UB_GREEDY_UB_H

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include "node.h"
#include <iostream>

namespace ted_ub {

template <typename Label, typename CostModel>
class GreedyUB {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::unordered_map<std::string, std::list<int>>& t1_label_il;
    const std::vector<int>& t1_post_to_pre;
    const std::vector<int>& t1_parent;
    const std::vector<int>& t1_rch;
    const std::vector<int>& t1_depth;
    const std::vector<int>& t1_size;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  GreedyUB();
  /// Verifies if the greedy label upper bound between trees t1 and t1 is
  /// smaller or equal than the similarity_threshold value.
  ///
  /// NOTE: Here, only unit cost model is allowed. similarity_threshold is
  ///       converted to integer value using std::ceil and static cast.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param similarity_threshold Maximum value of the tree edit distance.
  /// \return Greedy label upper bound value if it smaller or equal than the
  ///         similarity_threshold, and std::numeric_limits<double>::infinity()
  ///         otherwise.
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double similarity_threshold);
  /// Calculates the unit cost of passed mapping.
  double mapping_cost(const std::vector<std::pair<int, int>>& mapping);
  /// Produces a one-to-one mapping greedily matching labels.
  ///
  /// NOTE: The result is not necessarily TED mapping. It is verified using
  ///       revise_greedy_mapping.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  std::vector<std::pair<int, int>> greedy_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2);
  
  std::vector<std::pair<int, int>> lb_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2, const int k);
  
  /// Creates a TestItems object and returns it (by value).
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
// Member variables.
// Member variables.
private:
  /// The size of the source tree.
  int t1_input_size_;
  /// The size of the destination tree.
  int t2_input_size_;
  /// Stores pointers to nodes of the source tree. Indexed in postorder.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Smart pointers introduce ownership, but
  // this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores pointers to nodes of the destination tree. Indexed in postorder.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Stores preorder id of each node in the source tree. Indexed in postorder.
  std::vector<int> t1_post_to_pre_;
  /// Stores preorder id of each node in the source tree. Indexed in postorder.
  std::vector<int> t2_post_to_pre_;
  /// Stores the postorder id of the parent for each node of the source
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_parent_;
  /// Stores the postorder id of the parent for each node of the destination
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_parent_;
  /// Stores for each node x of the source tree the postorder id of the
  /// first leaf node to the righ of x. '-1' represents no such node.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_rch_;
  /// Stores for each node x of the destination tree the postorder id of the
  /// first leaf node to the righ of x. '-1' represents no such node.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_rch_;
  /// Stores the depth for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_depth_;
  /// Stores the depth for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_depth_;
  /// Stores the subtree size for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_size_;
  /// Stores the subtree size for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_size_;
  /// For each laebl in the source tree, stores postorder ids of nodes that
  /// carry it (the list is sorted in postorder).
  /// NOTE: Key should be of type Label - requires modifying implementation of
  ///       Label. It has to be hashable.
  std::unordered_map<std::string, std::list<int>> t1_label_il_;
  /// For each laebl in the destination tree, stores postorder ids of nodes that
  /// carry it (the list is sorted in postorder).
  /// NOTE: Key should be of type Label - requires modifying implementation of
  ///       Label. It has to be hashable.
  std::unordered_map<std::string, std::list<int>> t2_label_il_;
  /// Cost model.
  const CostModel c_;
// Member functions.
private:
  /// Takes a greedy_mapping and revises it such that the output is a valid
  /// TED mapping.
  ///
  /// TODO: Deprecated because incorrect.
  ///
  /// \param mapping A one-to-one mapping.
  /// \return A revised mapping that is a valid TED mapping.
  std::vector<std::pair<int, int>> revise_greedy_mapping(const std::vector<std::pair<int, int>>& mapping);
  /// Takes a one-to-one mapping and revises it such that the output is a valid
  /// TED mapping.
  ///
  /// \param mapping A one-to-one mapping.
  /// \return A revised mapping that is a valid TED mapping.
  std::vector<std::pair<int, int>> to_ted_mapping(const std::vector<std::pair<int, int>>& mapping);
  /// Resets and initialises algorithm's internal data structures and constants.
  /// Has to be called before computing the distance.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  void init(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param label_il Inverted list of labels to node postorder ids.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param post_to_pre Translation vector from postorder to preorder id.
  void index_nodes(const node::Node<Label>& root,
                   std::unordered_map<std::string, std::list<int>>& label_il,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                   std::vector<int>& post_to_pre,
                   std::vector<int>& parent,
                   std::vector<int>& depth,
                   std::vector<int>& size);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param label_il Inverted list of labels to node postorder ids.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param post_to_pre Translation vector from postorder to preorder id.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::unordered_map<std::string, std::list<int>>& label_il,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             std::vector<int>& post_to_pre,
                             std::vector<int>& parent,
                             std::vector<int>& depth,
                             std::vector<int>& size,
                             int& start_postorder, int& start_preorder,
                             unsigned int start_depth);
  /// Collects the first leaf node to the right of every node of input tree.
  ///
  /// \param input_size Size of the input tree.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param rch Vector with first leaf nodes to the right.
  void post_traversal_indexing(const int input_size,
                               const std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                               std::vector<int>& rch);
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
};

// Implementation details.
#include "greedy_ub_impl.h"

}

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_H
