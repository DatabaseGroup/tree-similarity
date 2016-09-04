#ifndef STRING_NODE_DATA_H
#define STRING_NODE_DATA_H

#include <vector>
#include <string>

#include "node.h"

namespace nodes {

// Example of a class representing a custom node in a tree to show how to use
// the framework with custom nodes. In this example, a nodelabel is a vector of
// strings. Children are again stored in an array of pointers to custom nodes.
class StringNodeData {
private:
  // custom label
  std::string label_;

public:
  // Basic constructor
  StringNodeData(std::string label = "");

  std::string get_label() const;

  bool operator<(const StringNodeData& other) const;
  bool operator==(const StringNodeData& other) const;
};

StringNodeData::StringNodeData(std::string label) : label_(label) { }

std::string StringNodeData::get_label() const {
  return label_;
}

bool StringNodeData::operator<(const StringNodeData& other) const {
  return (label_.compare(other.get_label()) < 0); // TODO: modify to have distance comparison
}

bool StringNodeData::operator==(const StringNodeData& other) const {
  return (label_.compare(other.get_label()) == 0);
}

// Example of a struct representing custom cost functions to be used for the
// distance computation. This shows how to use the framework with custom cost
// functions. Also these custom costs need to be generic for different node
// classes.
template<class _NodeData = StringNodeData>
struct StringCosts : public Costs<_NodeData> {
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

template<class _NodeData>
int StringCosts<_NodeData>::ren(_NodeData* node_data1, _NodeData* node_data2) {
  if (node_data1->get_label() == node_data2->get_label()) {
    return 0;
  }

  return ((del(node_data1) + ins(node_data2)) / 2);
}

template<class _NodeData>
int StringCosts<_NodeData>::del(_NodeData* node_data) {
  return 1;
}

template<class _NodeData>
int StringCosts<_NodeData>::ins(_NodeData* node_data) {
  return 1;
}

template<class _NodeData>
int StringCosts<_NodeData>::ren() {
  return 1; 
}

template<class _NodeData>
int StringCosts<_NodeData>::del() {
  return 1;
}

template<class _NodeData>
int StringCosts<_NodeData>::ins() {
  return 1;
}

} // namespace nodes

#endif // STRING_NODE_DATA_H