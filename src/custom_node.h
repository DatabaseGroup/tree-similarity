#ifndef CUSTOM_NODE_H
#define CUSTOM_NODE_H

#include <vector>
#include <string>

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class CustomNode {
private:
  // custom label
  std::vector<std::string> label_;
  std::vector<CustomNode*> children_;

public:
  // Basic constructor
  CustomNode (std::vector<std::string> label,
    std::vector<CustomNode*> children)
    : label_(label), children_(children)
  { }

  // Getter label
  std::vector<std::string> get_label () const {
    return label_;
  }

  // Getter children
  std::vector<CustomNode*> get_children () const {
    return children_;
  }
};

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _node>
struct CustomCosts {
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
  int ren () { return 2; }
  int del () { return 2; }
  int ins () { return 2; }
};

#endif // CUSTOM_NODE_H
