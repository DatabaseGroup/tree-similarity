#ifndef CUSTOM_NODE_DATA_H
#define CUSTOM_NODE_DATA_H

#include <vector>
#include <string>

#include "node.h"

namespace nodes {

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class CustomNodeData {
private:
  // custom label
  std::vector<std::string> label_;

public:
  // Basic constructor
  CustomNodeData(std::vector<std::string> label);

  // Getter label
  std::vector<std::string> get_label() const;
};

CustomNodeData::CustomNodeData(std::vector<std::string> label) : label_(label) { }

std::vector<std::string> CustomNodeData::get_label() const {
  return label_;
}

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _NodeData = CustomNodeData>
struct CustomCosts : public Costs<_NodeData> {
  // Basic rename cost function
  //
  // Params:  node_data1  Data of the node to be renamed
  //          node_data2  Data of the node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren(_NodeData* node_data1, _NodeData* node_data2);

  // Basic delete cost function
  //
  // Params:  node_data Data of the node to be deleted
  //
  // Return:  The cost of deleting node
  int del(_NodeData* node_data);

  // Basic insert cost function
  //
  // Params:  node_data Data of the node to be inserted
  //
  // Return:  The cost of inserting node
  int ins(_NodeData* node_data);

  // Dummy methods
  int ren();
  int del();
  int ins();
};

template<typename _NodeData>
int CustomCosts<_NodeData>::ren(_NodeData* node_data1, _NodeData* node_data2) {
  return 2;
}

template<typename _NodeData>
int CustomCosts<_NodeData>::del(_NodeData* node_data) {
  return 2;
}

template<typename _NodeData>
int CustomCosts<_NodeData>::ins(_NodeData* node_data) {
  return 2;
}

template<typename _NodeData>
int CustomCosts<_NodeData>::ren() {
  return 2;
}

template<typename _NodeData>
int CustomCosts<_NodeData>::del() {
  return 2;
}

template<typename _NodeData>
int CustomCosts<_NodeData>::ins() {
  return 2;
}

} // namespace nodes

#endif // CUSTOM_NODE_DATA_H
