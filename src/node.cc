#include "node.h"
#include <random>
#include <iostream>
#include <ctime>

//Different constructors
//constructor with id, label_id, and number of children
node::node (int id, int label_id, int children_number)
  : id(id), label_id(label_id), children_number(children_number) { }
//constructor with only the label_id
node::node (int label_id)
  : label_id(label_id) { }
//constructor with id and label_id
node::node (int id, int label_id)
  : id(id), label_id(label_id) { }



//Deconstructor
node::~node () {
  // delete all children nodes
  for ( std::vector<node*>::iterator node_it = children.begin();
      node_it != children.end(); ++node_it)
  {
    delete *node_it;
  }

  // clear vector to avoid dangling pointers
  children.clear();
}


void node::copy_node(node *n){
    node tmp = *this;
    *n = tmp;
}


//Returns the ID of a node
int node::get_id () const {
  return id;
}

//Returns the ID of the label which is necessary to find the matching data (dictionary)
int node::get_label_id () const {
  return label_id;
}

//Returns the children nodes
std::vector<node*> node::get_children () const {
  return children;
}

//Returns the number of children of a node
int node::get_children_number () const {
  //return children_number;
  return children.size();
}

//Adds a child to the node
void node::add_child (node* child) {
  children.push_back(child);
  //++children_number;
}

//Returns the size of the subtree rooted at this node
int node::get_subtree_size () const {
  int descendants_sum = 1;
  // Sum up sizes of subtrees rooted at child nodes (number of descendants)
  for ( std::vector<node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    descendants_sum = descendants_sum + (*node_it)->get_subtree_size();
  }
  // Add this node to subtree size.
  return descendants_sum;
}

//Sets the ID of a node
void node::set_id(int id) {
  this->id = id;
}

//Sets the label-ID of a node
void node::set_label_id(int id) {
  this->label_id = id;
}

//Returns the i-th child of a node
node* node::get_child(int i) const {
  return children[i];
}

//Generate the postorder of a tree and returns the nodes in postorder stored in a vector
std::vector<node*>* generate_postorder (node* root) {
  int node_id_counter = 1;
  // Heap allocation
  std::vector<node*>* tr_post = new std::vector<node*>();

  // Recursively traverse tree in postorder
  postorder(root, tr_post, &node_id_counter);
  return tr_post;
}

void postorder (node* root, std::vector<node*>* tr_post, int* node_id_counter)
{
  if (root) {
    // traverse children first
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        postorder(root->get_child(i), tr_post, node_id_counter);
      }
    }
    root->set_id(*node_id_counter);
    ++(*node_id_counter);
    tr_post->push_back(root);
  }
}

//This function generates a tree
//Paramerers:
// root of the tree, the depth of the tree and the maximum fanout (numer of nodes in one level)
void generate_full_tree (node *root, int fixed_depth, int max_fanout) {
  // If we reached the maximum depth, terminate this branch.
  if (fixed_depth == 0)
    return;
  // Seed the random generator.
  // time(NULL) returns seconds, thus each sibling has the same fanout. This is
  // also true for each node in small trees which are generated in less than a second.
  std::srand(std::time(NULL));
  // Generate a random fanout between 1 and max_fanout.
  int random_fanout = 1 + ( std::rand() % ( max_fanout - 1 + 1 ) );
  int random_label = 0;
  // Add as many children to root as random_fanout.
  for (int i = 0; i < random_fanout; ++i) {
    // Generate random label as a number from interval 0 to 100.
    random_label = std::rand() % 100;
    // Create new node.
    // We have to use 'new' here. Otherwise as soon as we get out of this
    // method's scope, the object gets deleted.
    node* new_node = new node(random_label); // modified by stefan
    // Add node as a child of root.
    root->add_child(new_node);
    // Recursively generate consecutive levels.
    generate_full_tree(new_node, fixed_depth-1, max_fanout);
  }
}

void print_tree_labels (node* n) {
  // Print the label of node.
  // Recursively print labels of all descendants of node.
  std::vector<node*> children = n->get_children();
  for ( std::vector<node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    print_tree_labels(*node_it);
  }
}

void copy_tree(node* t, node* copy_t){
    for(int i = 0; i < t->get_children_number(); i++){
        node* temp = new node(t->get_child(i)->get_label_id());
        copy_t->add_child(temp);
        copy_tree(t->get_child(i),temp);
    }
}
