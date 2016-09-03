#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <unordered_map>

#include "string_node.h"

namespace nodes {

// Represents a node in a tree. A node label is a string and the children
// are stored in an array of pointers to nodes.
template<class SatelliteData = nodes::StringNode>
class Node {
public:
  // A vector of pointers to child nodes
  std::vector<Node<SatelliteData>*> children_;
  // Additional node data
  SatelliteData* data_;

public:
  // Constructors
  Node(SatelliteData* data = new SatelliteData());

  // Destructor
  ~Node();

  SatelliteData* get_data() const;

  // Get children
  std::vector<Node<SatelliteData>*> get_children() const;

  // Get children_number
  int get_children_number() const;

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child(Node* child);

  // TODO
  Node<SatelliteData>* get_child(int position) const;

  // Get subtree size rooted at this node.
  //
  // Return:  The size of the subtree rooted at this node (including this node)
  int get_subtree_size() const;

  bool operator<(const Node<SatelliteData>& other) const;
  bool operator==(const Node<SatelliteData>& other) const;
};

template<class SatelliteData>
Node<SatelliteData>::Node(SatelliteData* data) : data_(data) { }

template<class SatelliteData>
Node<SatelliteData>::~Node() {
  // TODO: why does this not work any more but throws a segfault? Ask Willi
  // delete all children nodes
  /*for ( typename std::vector<Node<SatelliteData>*>::const_iterator node_it = children_.begin();
        node_it != children_.end(); ++node_it)
  {
    delete *node_it;
  }*/

  // clear vector to avoid dangling pointers
  //children_.clear();
}

template<class SatelliteData>
SatelliteData* Node<SatelliteData>::get_data() const {
  return data_;
}

template<class SatelliteData>
std::vector<Node<SatelliteData>*> Node<SatelliteData>::get_children() const {
  return children_;
}

template<class SatelliteData>
int Node<SatelliteData>::get_children_number() const {
  return children_.size();
}

template<class SatelliteData>
void Node<SatelliteData>::add_child(Node<SatelliteData>* child) {
  children_.push_back(child);
}

template<class SatelliteData>
Node<SatelliteData>* Node<SatelliteData>::get_child(int position) const {
  return children_[position];
}

template<class SatelliteData>
int Node<SatelliteData>::get_subtree_size() const {
  int descendants_sum = 1;
  // Sum up sizes of subtrees rooted at child nodes (number of descendants)
  for ( auto node_it = children_.begin();
        node_it != children_.end(); ++node_it)
  {;
    descendants_sum = descendants_sum + (*node_it)->get_subtree_size();
  }

  // Add this node to subtree size.
  return descendants_sum;
}

template<class SatelliteData>
bool Node<SatelliteData>::operator<(const Node<SatelliteData>& other) const {
  return (*data_ < *other.get_data());
}

template<class SatelliteData>
bool Node<SatelliteData>::operator==(const Node<SatelliteData>& other) const {
  return (*data_ == *other.get_data());
}

// Represents the cost functions to be used for the distance computation.
// Costs are generic for different node classes. A cost model has to provide
// three cost functions:
//  - ren(n1, n2) for rename costs between two nodes n1 and n2
//  - del(n) for deletion costs of a node n
//  - ins(n) for insertion costs of a node n
// All three cost functions must return an integer.
template<typename _Node>
struct Costs {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren(_Node* node1, _Node* node2);

  // Basic delete cost function
  //
  // Params:  node The node to be deleted
  //
  // Return:  The cost of deleting node
  int del(_Node node);

  // Basic insert cost function
  //
  // Params:  node The node to be inserted
  //
  // Return:  The cost of inserting node
  int ins(_Node node);

  // Dummy methods
  int ren();
  int del();
  int ins();
};

Node<> empty_node;

} // namespace nodes

#endif // NODE_H

// TODO:
// NO - Should the fields of Node, especially children, be pointers?
// ?? - Should children be initialized only with the first child? Are they now?
// ?? - Fix the destructor which should delete all descendants.
// NO    - Should the nodes be wrapped with a Tree clas?
//       - How to recursively delete all nodes including the root node?
// NO       - Using 'delete this' in destructor is not entirely safe.
// ??       - I've found that one way of dealing with that is smart pointers like
//            'unique_ptr'.
// OK    - Is it enough to write a destructor that deletes all the descendants
//         without the root? Will the root gets deleted automatically when it goes
//         out of scope?
// OK       - Is what we have now enough?
// OK - Implement get_subtree_size() as member method.
//    - Implement get_postorder_traversal(Node *root) outside Node class.