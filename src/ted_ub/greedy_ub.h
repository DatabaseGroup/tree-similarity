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
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  GreedyUB();
  /// Calculates the cost of greedy mapping, if it is ted mapping.
  double greedy_mapping_cost(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// Produces a one-to-one mapping greedily matching labels.
  /// Not necessarily TED mapping.
  std::vector<std::pair<int, int>> greedy_mapping(const node::Node<Label>& t1, const node::Node<Label>& t2);
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
  /// Stores pointers to nodes of the source tree. Indexed in postorder-1.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Smart pointers introduce ownership, but
  // this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores pointers to nodes of the destination tree. Indexed in postorder-1.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  
  std::vector<int> t1_post_to_pre_;
  std::vector<int> t2_post_to_pre_;
  
  // TODO: Key should be of type Label - requires modifying implementation of
  //       Label. It has to be hashable.
  std::unordered_map<std::string, std::list<int>> t1_label_il_;
  std::unordered_map<std::string, std::list<int>> t2_label_il_;
  
  /// Cost model.
  const CostModel c_;
// Member functions.
private:
  /// Takes a greedy_mapping and revises it such that the output is a TED mapping.
  std::vector<std::pair<int, int>> revise_greedy_mapping(std::vector<std::pair<int, int>>& mapping);
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
  void index_nodes(const node::Node<Label>& root,
                   std::unordered_map<std::string, std::list<int>>& label_il,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                   std::vector<int>& post_to_pre);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  void index_nodes_recursion(const node::Node<Label>& root,
                             std::unordered_map<std::string, std::list<int>>& label_il,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             std::vector<int>& post_to_pre,
                             int& start_postorder, int& start_preorder);
};

// Implementation details.
#include "greedy_ub_impl.h"

}

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_H
