#include "Node.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

//Node::Node(int val) :label(val) {}

Node::Node(){this->label = -1;}
Node::Node(int val)
//Node::Node(const std::string& val)
{
    this->label = val;
}

int Node::get_label() {
  return label;
}

void Node::add_child(Node* child) {
  this->children.push_back(child);
}

std::vector<Node*> Node::get_children() {
  return children;
}

int Node::get_subtree_size() {
  int descendants_sum = 0;
  // Sum up sizes of subtrees rooted at all children nodes (number of descendants).
  for (std::vector<Node*>::iterator pNode = children.begin(); pNode != children.end(); ++pNode) {
    descendants_sum = descendants_sum + (*pNode)->get_subtree_size();
  }
  // Add this node to subtree size.
  return descendants_sum + 1;
}

Node::~Node() {
  // Delete all children nodes.
  for (std::vector<Node*>::iterator pNode = children.begin(); pNode != children.end(); ++pNode) {
    delete *pNode;
  }
}

//modiefied by Sibylle and Stefan
//
int Node::get_children_number()
{
    return this->children.size();
}

Node* Node::get_child(int i)
{
    return this->children.at(i);
}

void Node::set_label(int new_int)
{
    this->label = new_int;
}


void generate_full_tree(Node *root, int fixed_depth, int max_fanout) {
  // If we reached the maximum depth, terminate this branch.
  if (fixed_depth == 0) return;
  // Seed the random generator.
  // time(NULL) returns seconds, thus each sibling has the same fanout. This is
  // also true for each node in small trees which are generated in less than a second.
  std::srand(std::time(NULL));
  // Generate a random fanout between 1 and max_fanout.
  int random_fanout = 1 + ( std::rand() % ( max_fanout - 1 + 1 ) );
  std::cout << "rf: " << random_fanout << "\n";
  int random_label = 0;
  // Add as many children to root as random_fanout.
  for (int i = 0; i < random_fanout; ++i) {
    // Generate random label as a number from interval 0 to 100.
    random_label = std::rand() % 100;
    // Create new node.
    // We have to use 'new' here. Otherwise as soon as we get out of this
    // method's scope, the object gets deleted.
    Node *node = new Node(random_label);
    // Add node as a child of root.
    root->add_child(node);
    // Recursively generate consecutive levels.
    generate_full_tree(node, fixed_depth-1, max_fanout);
  }
}

void print_tree_labels(Node *node) {
  // Print the label of node.
  std::cout << node->get_label() << "\n";

  // Recursively print labels of all descendants of node.
  std::vector<Node*> ch = node->get_children();
  for (std::vector<Node*>::iterator pNode = ch.begin(); pNode != ch.end(); ++pNode) {
    print_tree_labels(*pNode);
  }
}


