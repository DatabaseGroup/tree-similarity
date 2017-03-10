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
/// representing the label (_LabelData).
/// The type of the label is parameterized using _LabelData and can be of
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
/// \tparam _LabelData Satellite data associated with the node.
template <class _LabelData>
class Node {
public:
    Node(_LabelData* data = new _LabelData());
    
    ~Node();
    
    /// Gets and sets members.
    /// @{
    inline _LabelData* data() const;
    inline void data(_LabelData* data);
    inline std::vector<Node<_LabelData>*>& children() const;
    inline void children(std::vector<Node<_LabelData>*>& children);
    /// @}

    /// Retrieves current number of children.
    ///
    /// \return Current number of children (i.e., entries in children_).
    int get_children_number() const;

    /// Adds a child at last position.
    ///
    /// \param child Pointer to the node to be added.
    void add_child(Node<_LabelData>* child);

    /// Gets the position-th child of this node (i.e., the n-th child).
    ///
    /// \param position Number of the child.
    /// \return Pointer to the position-th child.
    Node<_LabelData>* get_child(int position) const;

    /// Gets size of subtree rooted at this node.
    ///
    /// \return Size of the subtree rooted at this node (incl. this node).
    int get_subtree_size() const;

    /// Operator overloadings.
    /// @{
    bool operator<(const Node<_LabelData>& other) const;
    bool operator==(const Node<_LabelData>& other) const;
    /// @}

private:
    /// Pointers to all children of this node.
    std::vector<Node<_LabelData>*> children_;

    /// Data representing the label of this node.
    _LabelData* data_;
};

}

#endif // TREE_SIMILARITY_NODE_NODE_H
