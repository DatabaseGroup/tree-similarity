#include "node.h"

#include <iostream>
#include <ctime>

node::node (int id, int label_id, int children_number)
  : id(id), label_id(label_id), children_number(children_number) { }
node::node (int label_id)
  : label_id(label_id) { }
node::node (int id, int label_id)
  : id(id), label_id(label_id) { }

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

int node::get_id () const {
  return id;
}

int node::get_label_id () const {
  return label_id;
}

std::vector<node*> node::get_children () const {
  return children;
}

int node::get_children_number () const {
  return children_number;
}

void node::add_child (node* child) {
  children.push_back(child);
  ++children_number;
}

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

void node::set_id(int id) {
  this->id = id;
}

node* node::get_child(int position) const {
  return children[position];
}



std::vector<node*>* generate_postorder (node* root) {
  int node_id_counter = 1;
  // Heap allocation
  std::vector<node*>* tr_post = new std::vector<node*>();

  // Recursively traverse tree in postorder
  postorder(root, tr_post, &node_id_counter);

  return tr_post;
}

void postorder (node* root, std::vector<node*>* tr_post,
  int* node_id_counter)
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
  std::cout << "rf: " << random_fanout << "\n";
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
