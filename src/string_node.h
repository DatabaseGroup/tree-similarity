#ifndef STRING_NODE_H
#define STRING_NODE_H

#include <vector>
#include <string>

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class StringNode {
private:
  int id_; 
  // custom label
  std::string label_;
  std::vector<StringNode*> children_;

public:
  // Basic constructor
  StringNode (std::string label)
    : label_(label)
  { }

  // Getter label
  std::string get_label () const {
    return label_;
  }

  // Getter children
  std::vector<StringNode*> get_children () const {
    return children_;
  }

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child (StringNode* child) {
    children_.push_back(child);
    //++children_number;
  }

  // Get children_number
  int get_children_number () const {
    //return children_number;
    return children_.size();
  }

  // Get a pointer to a child at a given position.
  //
  // Params:  position Position of the child in the vector
  //
  // Return:  A pointer to the child at position i if it exists or nullptr
  //          otherwise
  StringNode* get_child (int position) const {
    return children_[position];
  }

  // Get id
  int get_id () const {
    return id_;
  }

  // Setter id
  void set_id (int id) {
    id_ = id;
  }
};

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _node>
struct StringCosts {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren (_node* node1, _node* node2) {
    if (node1->get_label() == node2->get_label()) {
      return 0;
    }

    return 2; 
  }

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
  int ren () { return 2; }
  int del () { return 2; }
  int ins () { return 2; }
};

#endif // STRING_NODE_H
