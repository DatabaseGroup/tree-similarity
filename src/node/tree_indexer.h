// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
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

/// \file node/tree_indexer.h
///
/// \details
/// Contains the tools to index nodes of a tree into vectors.

#ifndef TREE_SIMILARITY_NODE_TREE_INDEXER_H
#define TREE_SIMILARITY_NODE_TREE_INDEXER_H

#include <vector>
#include <string>
#include <type_traits>
#include <label_dictionary.h>

namespace node {

/// Stores label id of each node in a tree.
/// Indexed in left-to-right postorder.
class PostLToLabelId {
  public: std::vector<unsigned int> postl_to_label_id_;
};

/// Stores subtree size of each node in a tree.
/// Indexed in left-to-right postorder.
class PostLToSize {
  public: std::vector<unsigned int> postl_to_size_;
};

/// Contains all tree indexes.
/// Used for correctness tests and prototyping.
class TreeIndexAll :
    public PostLToLabelId,
    public PostLToSize
{};

/// Main method to index a tree. It executes further recursive tree traversal.
/// It also resets the indexes and sets their length.
///
/// \param ti Tree index.
/// \param n Root node of the input tree.
/// \param ld LabelDictionary to collect node labels and assign their ids.
template <typename TreeIndex, typename Label>
void index_tree(TreeIndex& ti, const node::Node<Label>& n, label::LabelDictionary<Label>& ld);

/// Tree traversal method that indexes a tree.
/// Based on TreeIndex class, only index members of TreeIndex are filled in.
///
/// \param ti Tree index.
/// \param n Root node of the input tree.
/// \param ld LabelDictionary to collect node labels and assign their ids.
/// \param start_preorder Variable to hold current preorder id; modified during the recursion.
/// \param start_postorder Variable to hold current postorder id; modified during the recursion.
/// \return Subtree size rooted at node n.
template <typename TreeIndex, typename Label>
unsigned int index_tree_recursion(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, unsigned int& start_preorder,
    unsigned int& start_postorder);

// Implementation details
#include "tree_indexer_impl.h"

}

#endif // TREE_SIMILARITY_NODE_TREE_INDEXER_H
