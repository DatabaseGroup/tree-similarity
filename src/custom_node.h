#ifndef CUSTOM_NODE_H
#define CUSTOM_NODE_H

#include <vector>
#include <string>

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class custom_node {
private:
  // custom label
  std::vector<std::string> label;
  std::vector<custom_node*> children;

public:
  // Basic constructor
  custom_node (std::vector<std::string> label,
    std::vector<custom_node*> children);

  // Getters
  std::vector<std::string> get_label () const;
  std::vector<custom_node*> get_children () const;
};

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _node>
struct custom_costs {
  // custom rename cost function
  int ren (_node n1, _node n2) { return 2; }

  // custom delete cost function
  int del (_node n) { return 2; }
  
  // custom insert cost function
  int ins (_node n) { return 2; }

  // Dummy methods
  // TODO: discuss zhang_shasha how to use with arguments
  int ren () { return 2; }
  int del () { return 2; }
  int ins () { return 2; }
};

#endif // CUSTOM_NODE_H
