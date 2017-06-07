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

/// \file node/node_impl.h
///
/// \details
/// Contains the implementation of the Node class.

#ifndef TREE_SIMILARITY_NODE_NODE_IMPL_H
#define TREE_SIMILARITY_NODE_NODE_IMPL_H

template<class _Label>
Node<_Label>::Node(_Label label) : label_(std::move(label)) {}

template<class _Label>
int Node<_Label>::children_number() const {
  return children_.size();
}

template<class _Label>
const _Label& Node<_Label>::label() const {
  return label_;
}

template<class _Label>
void Node<_Label>::add_child(std::unique_ptr<Node<_Label>> child) {
  children_.push_back(child);
}

#endif // TREE_SIMILARITY_NODE_NODE_IMPL_H
