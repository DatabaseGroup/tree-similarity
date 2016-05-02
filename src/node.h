#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <unordered_map>

// Represents a node in a tree. A node label is a string and the children
// are stored in an array of pointers to nodes.
class node {
private:
  int id;
  int label_id;
  // A vector of pointers to child nodes
  std::vector<node*> children;
  int children_number;
  int id_t1;
  int id_t2;

public:
  // Constructors
  node (int id, int label_id, int children_number)
    : id(id), label_id(label_id), children_number(children_number)
  {
    // make sure to initalize ALL fields
    id_t1 = 0;
    id_t2 = 0;
  }

  explicit node (int label_id)
    : node(0, label_id)
  { }

  node (int id, int label_id)
    : node(id, label_id, 0)
  { }

  //node(node&);

  // Destructor
  ~node () {
    // delete all children nodes
    for ( std::vector<node*>::iterator node_it = children.begin();
          node_it != children.end(); ++node_it)
    {
      delete *node_it;
    }

    // clear vector to avoid dangling pointers
    children.clear();
  }

  void copy_node(node *n) {
    node tmp = *this;
    *n = tmp;
  }

  // Get id
  int get_id () const {
    return id;
  }

  // Get label_id
  int get_label_id () const {
    return label_id;
  }

  // Get children
  std::vector<node*> get_children () const {
    return children;
  }

  // Get children_number
  int get_children_number () const {
    //return children_number;
    return children.size();
  }

  void set_id_t1 (int t) {
    id_t1 = t;
  }

  void set_id_t2 (int t) {
    id_t2 = t;
  }

  int get_id_t1 () {
    return id_t1;
  }

  int get_id_t2 () {
    return id_t2;
  }

  // Add a child at last position.
  //
  // Params:  child A pointer to the node to be added
  //
  // Return:  None
  void add_child (node* child) {
    children.push_back(child);
    //++children_number;
  }

  // Get subtree size rooted at this node.
  //
  // Return:  The size of the subtree rooted at this node (including this node)
  int get_subtree_size () const {
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

  // Setter id
  void set_id (int id) {
    this->id = id;
  }

  // Setter label_id
  void set_label_id(int id) {
    this->label_id = id;
  }

  // Get a pointer to a child at a given position.
  //
  // Params:  position Position of the child in the vector
  //
  // Return:  A pointer to the child at position i if it exists or nullptr
  //          otherwise
  node* get_child (int position) const {
    return children[position];
  }
};

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
