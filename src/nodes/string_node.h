#ifndef STRING_NODE_H
#define STRING_NODE_H

#include <vector>
#include <string>

namespace nodes {

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
  StringNode();
  StringNode(std::string label);

  // Getter label
  std::string get_label() const;

  // Getter children
  std::vector<StringNode*> get_children() const;

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child(StringNode* child);

  // Get children_number
  int get_children_number() const;

  // Get a pointer to a child at a given position.
  //
  // Params:  position Position of the child in the vector
  //
  // Return:  A pointer to the child at position i if it exists or nullptr
  //          otherwise
  StringNode* get_child(int position) const;

  // Get id
  int get_id() const;

  // Setter id
  void set_id(int id);

  // Get subtree size rooted at this node.
  //
  // Return:  The size of the subtree rooted at this node (including this node)
  int get_subtree_size() const;

  bool operator<(const StringNode& other) const;
  bool operator==(const StringNode& other) const;
};

StringNode::StringNode() { }

StringNode::StringNode(std::string label) : label_(label) {}

std::string StringNode::get_label() const {
  return label_;
}

std::vector<StringNode*> StringNode::get_children() const {
  return children_;
}

void StringNode::add_child(StringNode* child) {
  children_.push_back(child);
  //++children_number;
}

int StringNode::get_children_number() const {
  //return children_number;
  return children_.size();
}

StringNode* StringNode::get_child(int position) const {
  return children_[position];
}

int StringNode::get_id() const {
  return id_;
}

void StringNode::set_id(int id) {
  id_ = id;
}

int StringNode::get_subtree_size() const {
  int descendants_sum = 1;
  // Sum up sizes of subtrees rooted at child nodes (number of descendants)
  for ( std::vector<StringNode*>::const_iterator node_it = children_.cbegin();
        node_it != children_.cend(); ++node_it)
  {
    descendants_sum = descendants_sum + (*node_it)->get_subtree_size();
  }

  // Add this node to subtree size.
  return descendants_sum;
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
template<class _node>
struct StringCosts {
  // Basic rename cost function
  //
  // Params:  node1  The node to be renamed
  //          node2  The node having the desired name
  //
  // Return:  The cost of renaming node1 to node2
  int ren(_node* node1, _node* node2) {
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
  int del(_node* node) {
    return 2;
  }

  // Basic insert cost function
  //
  // Params:  node The node to be inserted
  //
  // Return:  The cost of inserting node
  int ins(_node* node) {
    return 2;
  }

  // Dummy methods
  int ren() { return 2; }
  int del() { return 2; }
  int ins() { return 2; }
};

StringNode empty_string_node("");

} // namespace nodes

#endif // STRING_NODE_H