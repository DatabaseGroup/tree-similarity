#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

/**
 * Represents a node in a tree. A node label is a string and the children
 * are stored in an array of pointers to nodes.
 */
class node {
private:
  std::string label;
  std::vector<node>* children;

public:
  // Basic constructor
  node( std::string label = std::string(),
        std::vector<node>* children = new std::vector<node>())
    : label(label), children(children)
  {  }
  
  // Getters
  std::string get_label() const { return label; }
  std::vector<node>* get_children() const { return children; }
};

/**
 * Represents the cost functions to be used for the distance computation.
 * Costs are generic for different node classes. A cost model has to provide
 * three cost functions:
 *  - ren(n1, n2) for rename costs between two nodes n1 and n2
 *  - del(n) for deletion costs of a node n
 *  - ins(n) for insertion costs of a node n
 * All three cost functions must return an integer.
 */
template<class _node>
struct costs {
  // Basic rename cost function
  int ren(_node n1, _node n2) { return 1; }

  // Basic delete cost function
  int del(_node n) { return 2; }

  // Basic insert cost function
  int ins(_node n) { return 3; }
};

/**
 * Example of a class representing a custom node in a tree to show how to use
 * the framework with custom nodes. In this example, a nodelabel is a vector of
 * strings. Children are again stored in an array of pointers to custom nodes.
 */
class custom_node {
private:
  // custom label
  std::vector<std::string> label;
  std::vector<custom_node>* children;

public:
  // Basic constructor
  custom_node( std::vector<std::string> label = std::vector<std::string>(),
        std::vector<custom_node>* children = new std::vector<custom_node>())
    : label(label), children(children)
  { }

  // Getters
  std::vector<std::string> get_label() const { return label; }
  std::vector<custom_node>* get_children() const { return children; }
};

/**
 * Example of a struct representing custom cost functions to be used for the
 * distance computation. This shows how to use the framework with custom cost
 * functions. Also these custom costs need to be generic for different node
 * classes.
 */
template<class _node>
struct custom_costs {
  // custom rename cost function
  int ren(_node n1, _node n2) { return 11; }

  // custom delete cost function
  int del(_node n) { return 12; }
  
  // custom insert cost function
  int ins(_node n) { return 13; }
};

#endif // NODE_H
