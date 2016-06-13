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

// Represents a node in a tree. A node label is a string and the children
// are stored in an array of pointers to nodes.
class Node {
private:
  int id_;
  int label_id_;
  int level = 0;
  // A vector of pointers to child nodes
  std::vector<Node*> children_;
  int children_number_;
  std::vector<char> edges_;
  char colour_;

public:
  // Constructors
  Node (int id, int label_id, int children_number)
    : id_(id), label_id_(label_id), children_number_(children_number)
  { }

  explicit Node (int label_id)
    : Node(0, label_id)
  { }

  Node (int id, int label_id)
    : Node(id, label_id, 0)
  { }

  //Node (Node&);

  // Destructor
  ~Node () {
    // delete all children nodes
    for ( std::vector<Node*>::iterator node_it = children_.begin();
          node_it != children_.end(); ++node_it)
    {
      delete *node_it;
    }

    // clear vector to avoid dangling pointers
    children_.clear();
  }

  void copy_node(Node *node) {
    *node = *this;
  }

  // Get id
  int get_id () const {
    return id_;
  }

  // Get label_id
  int get_label_id () const {
    return label_id_;
  }

  // Get children
  std::vector<Node*> get_children () const {
    return children_;
  }

  // Get children_number
  int get_children_number () const {
    //return children_number;
    return children_.size();
  }

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child (Node* child) {
    children_.push_back(child);
    //++children_number;
  }

  void add_child_at(Node* child, int position) {
    std::vector<Node*>::iterator it = children_.begin();
    children_.insert(it + position, child);
  }

  // Get subtree size rooted at this node.
  //
  // Return:  The size of the subtree rooted at this node (including this node)
  int get_subtree_size () const {
    int descendants_sum = 1;
    // Sum up sizes of subtrees rooted at child nodes (number of descendants)
    for ( std::vector<Node*>::const_iterator node_it = children_.cbegin();
          node_it != children_.cend(); ++node_it)
    {
      descendants_sum = descendants_sum + (*node_it)->get_subtree_size();
    }

    // Add this node to subtree size.
    return descendants_sum;
  }

  // Setter id
  void set_id (int id) {
    id_ = id;
  }

  // Setter label_id
  void set_label_id(int id) {
    label_id_ = id;
  }

  // Get a pointer to a child at a given position.
  //
  // Params:  position Position of the child in the vector
  //
  // Return:  A pointer to the child at position i if it exists or nullptr
  //          otherwise
  Node* get_child (int position) const {
    return children_[position];
  }

  void remove_child (int position) {
    children_.erase(children_.begin() + position);
  }

  void remove_child (Node* n) {
    std::vector<Node*>::iterator it = std::find(children_.begin(), children_.end(), n);
    if (it != children_.end()){
      children_.erase(it);
    }
  }

  void remove_all_children() {
    children_.clear();
  }

  int get_child_position (Node* n) {
    std::vector<Node*>::iterator it = std::find(children_.begin(), children_.end(), n);
    if(it!=children_.end()){
      return (it - children_.begin());
    } else {
      return -1;
    }
  }

  void swap_children (int pos, int currpos) {
    iter_swap(children_.begin() + pos, children_.begin() + currpos);
  }

  // deleting in terms of removing and this node (=parent) inherits its children
  void delete_child (int pos) {
    Node* n = get_child(pos);
    for(int i = 0; i < n->get_children_number(); i++){
      this->add_child_at(n->get_child(i), (pos + i));
    }
    // might this be useless, since the child is deleted anyway?
    n->remove_all_children();
    remove_child(n);
  }

  void delete_child (Node* n) {
    int pos = get_child_position(n);
    delete_child(pos);
  }

  void add_edge (char colour) {
    edges_.push_back(colour);
  }

  void add_edge_at(char colour, int position) {
    std::vector<char>::iterator it = edges_.begin();
    edges_.insert(it + position, colour);
  }

  void remove_edge (int position) {
    edges_.erase(edges_.begin() + position);
  }

  void set_edge_colour (int position, char colour) {
    edges_[position] = colour;
  }

  char get_edge_colour (int position) {
    return edges_[position];
  }

  int get_edge_colours_count (char first, char second) {
    int counter = 0;    
    for(std::vector<char>::iterator it = edges_.begin(); it != edges_.end(); ++it) {
      char tmp = *it;
      if(tmp==first || tmp==second) { counter++; }
    }
    return counter;
  }

  void set_colour (char colour) {
    colour_ = colour;
  }

  char get_colour () {
    return colour_;
  }

  void set_level (int l) {
    level = l;
  }

  int get_level () {
    return level;
  }
};

// Represents the cost functions to be used for the distance computation.
// Costs are generic for different node classes. A cost model has to provide
// three cost functions:
//  - ren(n1, n2) for rename costs between two nodes n1 and n2
//  - del(n) for deletion costs of a node n
//  - ins(n) for insertion costs of a node n
// All three cost functions must return an integer.
template<class _node>
struct Costs {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren (_node* node1, _node* node2) {
    if (node1->get_label_id() == node2->get_label_id()) {
      return 0;
    }

    return 1;
  }

  // Basic delete cost function
  //
  // Params:  node The node to be deleted
  //
  // Return:  The cost of deleting node
  int del (_node node) { return 1; }

  // Basic insert cost function
  //
  // Params:  node The node to be inserted
  //
  // Return:  The cost of inserting node
  int ins (_node node) { return 1; }

  // Dummy methods
  int ren() { return 1; }
  int del() { return 1; }
  int ins() { return 1; }
};

template<class _node>
struct Costs2 {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren (_node node1, _node node2) { return 2; }

  // Basic delete cost function
  //
  // Params:  node The node to be deleted
  //
  // Return:  The cost of deleting node
  int del (_node node) { return 2; }

  // Basic insert cost function
  //
  // Params:  node The node to be inserted
  //
  // Return:  The cost of inserting node
  int ins (_node node) { return 2; }

  // Dummy methods
  int ren() { return 2; }
  int del() { return 2; }
  int ins() { return 2; }
};


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
