#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

/**
 * Represents a node in a tree. A node label is a string and the children
 * are stored in an array of pointers to nodes.
 */
class node {
protected:
  int id;
  int label_id; // TODO: change to string labels
  // A vector of pointers to child nodes
  std::vector<node*> children;
  int children_number;

public:
  // Basic constructor
  node () : id(-1), label_id(-1), children_number(0) { } 
  node (int label_id) : label_id(label_id) { }

  // Delete a node and all its descendants
  ~node ();
  
  // Getters
  int get_id () const { return id; }
  int get_label_id () const { return label_id; }
  std::vector<node*> get_children () const { return children; }
  int get_children_number () const { return children_number; }

  // Add a child at last position
  void add_child (node* child);

  // Return subtree size rooted at this node, including this node.
  int get_subtree_size ();

  void set_id (int id) { this->id = id; }
  node* get_child (int i) { return children[i]; }
};

// Generate a simple tree recursively.
// Each path has length equal to depth.
// Each node has a random fanout between 1 and max_fanout.
// ISSUE: Current random seed causes all nodes in a tree to have the same fanout.
void generate_full_tree (node *root, int depth, int max_fanout);

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
void print_tree_labels (node *node);

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
  int ren (_node n1, _node n2) { return 1; }

  // Basic delete cost function
  int del (_node n) { return 2; }

  // Basic insert cost function
  int ins (_node n) { return 3; }
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
  std::vector<custom_node*> children;

public:
  // Basic constructor
  custom_node (std::vector<std::string> label = std::vector<std::string>(),
    std::vector<custom_node*> children = std::vector<custom_node*>())
    : label(label), children(children)
  { }

  // Getters
  std::vector<std::string> get_label () const { return label; }
  std::vector<custom_node*> get_children () const { return children; }
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
  int ren (_node n1, _node n2) { return 11; }

  // custom delete cost function
  int del (_node n) { return 12; }
  
  // custom insert cost function
  int ins (_node n) { return 13; }
};

#endif // NODE_H

// TODO:
// NO - Should the fields of Node, especially children, be pointers?
// ?? - Should children be initialized only with the first child? Are they now?
// ?? - Fix the destructor which should delete all descendants.
// NO    - Should the nodes be wrapped with a Tree clas?
//       - How to recursively delete all nodes including the root node?
// NO       - Using 'delete this' in destructor is not entirely safe.
// ??       - I've found that one way of dealing with that is smart pointers like
//            'unique_ptr'.
// OK    - Is it enough to write a destructor that deletes all the descendants
//         without the root? Will the root gets deleted automatically when it goes
//         out of scope?
// OK       - Is what we have now enough?
// OK - Implement get_subtree_size() as member method.
//    - Implement get_postorder_traversal(Node *root) outside Node class.
