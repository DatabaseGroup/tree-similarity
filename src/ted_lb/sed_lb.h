// The MIT License (MIT)
// Copyright (c) 2018 Nikolaus Augsten, Mateusz Pawlik
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

/// \file ted_lb/sed_lb.h
///
/// \details
/// Implements string edit distanc with linear memory complexity.
/// NOTE: Unit Cost only.
/// TODO: Expand.

#ifndef TREE_SIMILARITY_TED_LB_SED_LB_H
#define TREE_SIMILARITY_TED_LB_SED_LB_H

#include <vector>
#include <cmath>
#include "node.h"
#include "label_dictionary.h"

namespace ted_lb {

template <typename Label, typename CostModel>
class StringEditDistanceLB {
// Member struct.
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  StringEditDistanceLB();
  /// TODO
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2,
      double similarity_threshold);
  /// TODO
  double sed_lb_ted(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// TODO
  void init(const node::Node<Label>& t1, const node::Node<Label>& t2);
// Member variables.
private:
  /// The size of the source tree.
  int t1_input_size_;
  /// The size of the destination tree.
  int t2_input_size_;
    
  /// Stores Label to int id translation of all labels in both input trees.
  label::LabelDictionary<Label> dict_;
  /// Stores the label id from dict_ for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_label_;
  /// See t1_label_.
  std::vector<int> t2_label_;
  /// Cost model.
  const CostModel c_;
// Member functions.
private:
  
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  /// TODO
  /// \param root The root node of the tree to index.
  void index_nodes(const node::Node<Label>& root, std::vector<int>& label);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  /// TODO
  /// \param root The root node of the tree to index.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<int>& label,
                             int& start_postorder);
};

// Implementation details.
#include "sed_lb_impl.h"

}

#endif // TREE_SIMILARITY_TED_LB_SED_LB_H
