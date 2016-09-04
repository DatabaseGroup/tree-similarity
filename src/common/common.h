#ifndef COMMON_H
#define COMMON_H

#include <sstream>
#include <map>
#include <set>
#include <array>
#include <unordered_map>

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace common {

typedef std::unordered_map<int, std::string> IDLabelMap;
typedef std::map<int, nodes::StringNodeData*, std::greater<int>> IDMappedNode;

// Recursively traverses the subtree rooted at a given root.
//
// Params:  root            The root node of the subtree to be traversed in
//                          postorder
//          tr_post         A pointer to a vector which is filled with the
//                          traverse nodes in postorder
//
// Return:  None (the result is stored in tr_post, which is altered)
template<class _NodeData = nodes::StringNodeData>
void postorder(nodes::Node<_NodeData>* root,
  std::vector<nodes::Node<_NodeData>*>* tr_post)
{
  if (root) {
    // traverse children first
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        postorder(root->get_child(i), tr_post);
      }
    }

    tr_post->push_back(root);
  }
}

// Generates the postorder for a tree rooted at a given root node.
//
// Params:  root  The root node of the tree to be 'postorderified'
//
// Return:  A pointer to a vector of node pointers of the 'postorderified' tree
template<class _NodeData = nodes::StringNodeData>
std::vector<nodes::Node<_NodeData>*>* generate_postorder(
  nodes::Node<_NodeData>* root)
{

  // TODO: rename to create_postorder

  // Heap allocation
  std::vector<nodes::Node<_NodeData>*>* tree_postorder =
    new std::vector<nodes::Node<_NodeData>*>();

  // Recursively traverse tree in postorder
  postorder(root, tree_postorder);
  return tree_postorder;
}

/*
// Generate a simple tree recursively.
// Each path has length equal to depth.
// Each node has a random fanout between 1 and max_fanout.
// ISSUE: Current random seed causes all nodes in a tree to have the same fanout.
//
// Params:  root        A node to be used as root of the tree
//          depth       Desired depth of the tree
//          max_fanout  Maximum fanout
//
// Return:  None
void generate_full_tree(nodes::Node *root, int fixed_depth, int max_fanout) {
  // If we reached the maximum depth, terminate this branch.
  if (fixed_depth == 0) {
    return;
  }

  // Seed the random generator.
  // time(NULL) returns seconds, thus each sibling has the same fanout. This is
  // also true for each node in small trees which are generated in less than a second.
  std::srand(std::time(NULL));
  // Generate a random fanout between 1 and max_fanout.
  int random_fanout = 1 + (std::rand() % (max_fanout - 1 + 1));
  int random_label = 0;
  // Add as many children to root as random_fanout.
  for (int i = 0; i < random_fanout; ++i) {
    // Generate random label as a number from interval 0 to 100.
    random_label = std::rand() % 100;
    // Create new node.
    // We have to use 'new' here. Otherwise as soon as we get out of this
    // method's scope, the object gets deleted.
    nodes::Node* new_node = new nodes::Node(random_label); // modified by stefan
    // Add node as a child of root.
    root->add_child(new_node);
    // Recursively generate consecutive levels.
    generate_full_tree(new_node, fixed_depth-1, max_fanout);
  }
}

void copy_tree(nodes::Node* original, nodes::Node* copy) {
  for(int i = 0; i < original->get_children_number(); i++){
    nodes::Node* temp = new nodes::Node(original->get_child(i)->get_label_id());
    copy->add_child(temp);
    copy_tree(original->get_child(i), temp);
  }
}

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
//
// Params:  node  The root node of the labels to be printed
//
// Return:  None
void print_tree_labels(nodes::Node* node) {
  // Print the label of node.
  // Recursively print labels of all descendants of node.
  std::vector<nodes::Node*> children = node->get_children();
  for ( std::vector<nodes::Node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    print_tree_labels(*node_it);
  }
  std::cout << node->get_label_id() << std::endl;
}

// TODO replace hashtable with a custom node class that supp. strings as labels
// Creates a json string for the given node
// e.g.: the json string for the entire tree, recursively
//
// Params:  r1      the root of the (sub)tree
//          level   the int level for this level
//
// Return: a string in json format
template<class _node = nodes::Node>
std::string get_json_tree(_node* root, int level, IDLabelMap hashtable,
  int* map = nullptr, int tree = 0)
{
  std::stringstream str;
  if (root) {
    str << "{\"scope\":" << level;
    str << ",\"label\":\"" << hashtable[root->get_label_id()] << "\"";

    if (tree != 0) {
      str << ",\"tree\":" << tree << "";
    }

    if (map != nullptr) {
      str << ",\"mappedTo\":" << map[root->get_id()] << "";
    }
    str << ",\"children\":";

    if (root->get_children_number() > 0) {
      str << "[";
      for (int i = 0; i < root->get_children_number(); ++i) {
        str << get_json_tree(root->get_child(i), (level+1), hashtable, map, tree);
        if ((i + 1) < root->get_children_number()) {
          str << ",";
        }
      }
      str << "]";
    } else {
      str << " null ";
    }
    str << "}";
  }
  // is this ok? or should it be passed differently?
  return str.str();
}

template<class _node = nodes::Node>
std::string get_json_side_by_side(_node* tree1, _node* tree2, 
  IDLabelMap hashtable_id_to_label, int** edit_mapping_int_array = nullptr)
{
  std::string sbs = "[";
  sbs += common::get_json_tree(tree1, 0, hashtable_id_to_label,
    edit_mapping_int_array[0], 1
  );
  sbs += ",";
  sbs += common::get_json_tree(tree2, 0, hashtable_id_to_label,
    edit_mapping_int_array[1], 2
  );
  sbs += "]";

  return sbs;
}

// check if two trees are the same (speaking of labels & structure)
template<class _node = nodes::Node>
bool check_if_same_trees(_node* t1, _node* t2){
  bool result = false;

  if(t1->get_label_id() == t2->get_label_id()){
    if(t1->get_children_number() == t2->get_children_number()){
      for(int i = 0; i < t1->get_children_number(); i++){
        result = check_if_same_trees(t1->get_child(i), t2->get_child(i));
        if(!result) { return false; }
      }
      result = true;
    } else {
      std::cout << "diff: children at: " << t1->get_id() << std::endl;
    }
  } else {
    std::cout << "diff: label at: " << t1->get_id() << std::endl;
  }
  
  return result;
}



// prints the given tree indented using dots
template<class _node = nodes::Node>
void print_tree_indented(_node* n, int level, IDLabelMap hashtable_id_to_label){
  for(int i = 0; i < level; i++){
    std::cout << ".";
  }
  std::cout << hashtable_id_to_label[n->get_label_id()] << " (id: "
    << n->get_id() << ")" << std::endl;
  for(int i = 0; i < n->get_children_number(); i++){
    print_tree_indented(n->get_child(i), (level+1),hashtable_id_to_label);
  }
}
*/

} // namespace common

#endif // COMMON_H
