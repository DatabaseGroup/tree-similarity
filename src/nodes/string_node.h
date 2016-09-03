#ifndef STRING_NODE_H
#define STRING_NODE_H

#include <vector>
#include <string>

#include "node.h"

namespace nodes {

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class StringNode {
private:
  // custom label
  std::string label_;

public:
  // Basic constructor
  StringNode(std::string label = "");

  std::string get_label() const;

  bool operator<(const StringNode& other) const;
  bool operator==(const StringNode& other) const;
};

StringNode::StringNode(std::string label) : label_(label) { }

std::string StringNode::get_label() const {
  return label_;
}

bool StringNode::operator<(const StringNode& other) const {
  return (label_.compare(other.get_label()) < 0); // TODO: modify to have distance comparison
}

bool StringNode::operator==(const StringNode& other) const {
  return (label_.compare(other.get_label()) == 0);
}

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _Node = nodes::StringNode>
struct StringCosts {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren(_Node* node1, _Node* node2) {
    if (node1->get_label() == node2->get_label()) {
      return 0;
    }

    return ((del(node1) + ins(node2)) / 2);
  }

  // Basic delete cost function
  //
  // Params:  node The node to be deleted
  //
  // Return:  The cost of deleting node
  int del(_Node* node) {
    return 1;
  }

  // Basic insert cost function
  //
  // Params:  node The node to be inserted
  //
  // Return:  The cost of inserting node
  int ins(_Node* node) {
    return 1;
  }

  // Dummy methods
  int ren() { return 1; }
  int del() { return 1; }
  int ins() { return 1; }
};

} // namespace nodes

#endif // STRING_NODE_H