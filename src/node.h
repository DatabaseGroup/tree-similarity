#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

// Represents a node in a tree. A node label is a string and the children
// are stored in an array of pointers to nodes.
class node {
private:
  int id;
  int label_id;
  // A vector of pointers to child nodes
  std::vector<node*> children;
  int children_number;

public:

  // Constructors
  node (int id, int label_id, int children_number);
  node (int label_id);
  node (int id, int label_id);
  //node(node&);

  // Destructor
  ~node ();

  void copy_node(node *n);

  // Get id
  int get_id () const;

  // Get label_id
  int get_label_id () const;

  // Get children
  std::vector<node*> get_children () const;

  // Get children_number
  int get_children_number () const;

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child (node* child);

  // Get subtree size rooted at this node.
  //
  // Return:  The size of the subtree rooted at this node (including this node)
  int get_subtree_size () const;

  // Setter id
  void set_id (int id);

  // Setter label_id
  void set_label_id(int id);

  // Get a pointer to a child at a given position.
  //
  // Params:  position Position of the child in the vector
  //
  // Return:  A pointer to the child at position i if it exists or nullptr
  //          otherwise
  node* get_child (int position) const;


};

// Generates the postorder for a tree rooted at a given root node.
//
// Params:  root  The root node of the tree to be 'postorderified'
//
// Return:  A pointer to a vector of node pointers of the 'postorderified' tree
std::vector<node*>* generate_postorder (node* root);

// Recursively traverses the subtree rooted at a given root.
//
// Params:  root            The root node of the subtree to be traversed in
//                          postorder
//          tr_post         A pointer to a vector which is filled with the
//                          traverse nodes in postorder
//          node_id_counter A pointer to the counter which is incremented each
//                          time a node is added to the resulting vector
//
// Return:  None (the result is stored in tr_post, which is altered)
void postorder(node* root, std::vector<node*>* tr_post, int* node_id_counter);

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
void generate_full_tree (node *root, int depth, int max_fanout);

void copy_tree(node* t, node* copy_t);

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
//
// Params:  node  The root node of the labels to be printed
//
// Return:  None
void print_tree_labels (node *node);

// Represents the cost functions to be used for the distance computation.
// Costs are generic for different node classes. A cost model has to provide
// three cost functions:
//  - ren(n1, n2) for rename costs between two nodes n1 and n2
//  - del(n) for deletion costs of a node n
//  - ins(n) for insertion costs of a node n
// All three cost functions must return an integer.
template<class _node>
struct costs {
  // Basic rename cost function
  //
  // Params:  n1  The node to be renamed
  //          n2  The node having the desired name
  //
  // Return:  The cost of renaming n1 to n2
  int ren (_node n1, _node n2) { return 1; }

  // Basic delete cost function
  //
  // Params:  n The node to be deleted
  //
  // Return:  The cost of deleting n
  int del (_node n) { return 1; }

  // Basic insert cost function
  //
  // Params:  n The node to be inserted
  //
  // Return:  The cost of inserting n
  int ins (_node n) { return 1; }

  // Dummy methods
  int ren() { return 1; }
  int del() { return 1; }
  int ins() { return 1; }
};

template<class _node>
struct costs2 {
  // Basic rename cost function
  //
  // Params:  n1  The node to be renamed
  //          n2  The node having the desired name
  //
  // Return:  The cost of renaming n1 to n2
  int ren (_node n1, _node n2) { return 2; }

  // Basic delete cost function
  //
  // Params:  n The node to be deleted
  //
  // Return:  The cost of deleting n
  int del (_node n) { return 2; }

  // Basic insert cost function
  //
  // Params:  n The node to be inserted
  //
  // Return:  The cost of inserting n
  int ins (_node n) { return 2; }

  // Dummy methods
  int ren() { return 2; }
  int del() { return 2; }
  int ins() { return 2; }
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
