// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik, and Nikolaus Augsten
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

/// \file node/node.h
///
/// \details
/// Contains two class declarations:
///  1. Node (represents a node in a tree)
///  2. Costs (a basic cost model)

#ifndef TREE_SIMILARITY_NODE_NODE_H
#define TREE_SIMILARITY_NODE_NODE_H

#include <vector>

namespace node {

/// \class Node
///
/// \details
/// Represents a node in a tree. Every node holds some satellite data
/// representing the label (_NodeData).
/// The type of the label is parameterized using _NodeData and can be of
/// arbitrary type as long as the requirements for computing the tree edit
/// distance are met. We refer to the Costs class (in this file) for further
/// information on the requirements.
/// The children of a node are stored in a dynamic array of Node pointers. This
/// class provides all necessary methods for the tree algorithms. All fields are
/// declared
/// 
/// \remark
/// Typically, the user of this framework does not need to modify this class by
/// any means. If one decides to modify this class it may actually break some
/// tree algorithms, so be careful.
///
/// \tparam _NodeData Satellite data associated with the node.
template <class _NodeData>
class Node {
public:
    Node(_NodeData* data = new _NodeData());
    Node(const Node<_NodeData>& other); // copy constr.
    ~Node();

    /// Gets and sets members.
    /// @{
    inline const _NodeData* data() const;
    inline void data(_NodeData* data);
    inline const std::vector<Node<_NodeData>*>& children() const;
    inline void children(std::vector<Node<_NodeData>*>& children);
    /// @}

    /// Retrieves current number of children.
    ///
    /// \return Current number of children (i.e., entries in children_).
    inline int get_children_number() const;

    /// Adds a child at last position.
    ///
    /// \param child Pointer to the node to be added.
    inline void add_child(Node<_NodeData>* child);

    /// Gets the position-th child of this node (i.e., the n-th child).
    ///
    /// \param position Number of the child.
    /// \return Pointer to the position-th child.
    inline const Node<_NodeData>* get_child(int position) const;

    /// Gets size of subtree rooted at this node.
    ///
    /// \return Size of the subtree rooted at this node (incl. this node).
    int get_subtree_size() const;

    /// Operator overloadings.
    /// @{
    inline const Node<_NodeData>& operator=(const Node<_NodeData>& other);
    inline bool operator<(const Node<_NodeData>& other) const;
    inline bool operator==(const Node<_NodeData>& other) const;
    /// @}

private:
    /// Pointers to all children of this node.
    std::vector<Node<_NodeData>*> children_;

    /// Data representing the label of this node.
    _NodeData* data_;

private:
    /// Computes the size of the subtree rooted at this node iteratively,
    /// resp. recursively.
    ///
    /// \return Size of the subtree rooted at this node (incl. this node).
    /// @{
    int get_subtree_size_iterative() const;
    int get_subtree_size_recursive() const;
    /// @}
};

/// \class Costs
///
/// \details
/// Represents the cost model to be used for the distance computation.
/// Costs are generic for different node classes. A cost model has to provide
/// three cost functions:
///  - ren(n1, n2); cost of updating the data (label) of n1 to the data (label)
///    of n2.
///  - del(n); cost of deleting node n.
///  - ins(n); cost of inserting node n.
/// All three cost functions must return an integer.
template <typename _NodeData>
struct Costs {
    /// Basic rename cost function (unit cost model).
    ///
    /// \param node1 The node to be renamed.
    /// \param node2  The node having the desired name.
    /// \return Integer cost of renaming node1 to node2.
    virtual inline int ren(const _NodeData* node1, const _NodeData* node2) = 0;

    /// Basic delete cost function.
    ///
    /// \param node The node to be deleted.
    /// \return Integer cost of deleting node.
    virtual inline int del(const _NodeData* node) = 0;

    /// Basic insert cost function.
    ///
    /// \param node The node to be inserted.
    /// \return Integer cost of inserting node.
    virtual inline int ins(const _NodeData* node) = 0;
};

}

#endif // TREE_SIMILARITY_NODE_NODE_H
