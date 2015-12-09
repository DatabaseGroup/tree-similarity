#include "node.h"

#include <iostream>
#include <ctime>

int node::node_id_counter = 1;

void node::add_child (node* child) {
  children.push_back(child);
  ++children_number;
}

int node::get_subtree_size () {
  int descendants_sum = 1;
  // Sum up sizes of subtrees rooted at all children nodes (number of descendants).
  for ( std::vector<node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    descendants_sum = descendants_sum + (*node_it)->get_subtree_size();
  }
  // Add this node to subtree size.
  return descendants_sum;
}

node::~node () {
  // Delete all children nodes.
  for ( std::vector<node*>::iterator node_it = children.begin();
        node_it != children.end(); ++node_it) {
    delete *node_it;
  }
  
  // Clear vector to avoid dangling pointers
  children.clear();
}

std::vector<node*> node::generate_postorder () {
  node_id_counter = 1;
  postorder(this);
  return tr_post;
}

void node::postorder (node* root) {
  // TODO: REVISE
  // As far as I understand it, this approach alters the ids of the original nodes

  if (root) { //not null
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        postorder(root->get_child(i));
      }
    }

    root->set_id(node_id_counter);
    ++node_id_counter;
    tr_post.push_back(root);
  }
}

void node::make_leaves () {
  set_leaves(this, leaves);
}

void node::set_leaves (node* root, std::vector<node*>& leaves) {
  if (root) { //not null
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        root->set_leaves(root->get_child(i), leaves);
      }
    } else {
      leaves.push_back(root);
    }
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
    //Node *node = new Node(i, random_label);
    node* new_node = new node(random_label); // modified by stefan
    // Add node as a child of root.
    root->add_child(new_node);
    // Recursively generate consecutive levels.
    generate_full_tree(new_node, fixed_depth-1, max_fanout);
  }
}

void print_tree_labels (node* n) {
  // Print the label of node.
  //std::cout << node->get_id() << "\n";
  // Recursively print labels of all descendants of node.
  std::vector<node*> children = n->get_children();
  for ( std::vector<node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    print_tree_labels(*node_it);
  }
}