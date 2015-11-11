#include <vector>

// Node represents a single node in a tree.
class Node {
  // Node's label.
  int label;
  // Node's children - a vector of pointers to children.
  std::vector<Node*> children;

  // Public methods.
  public:
    // Constructor - label as input.
    Node(int val);
    //Node(const std::string& val);
    // Destructor which should delete a node and all its descendants.
    ~Node();
    // Add a child as the last child.
    void add_child(Node* child);
    // Return node's children.
    std::vector<Node*> get_children();
    // Return node's label.
    int get_label();
    // Return subtree size rooted at this node, including this node.
    int get_subtree_size();

    //modified by Sibylle and Stefan
    //set the ID of a node
    void set_label(int new_int);
    //returns the i-th child of a node
    Node* get_child(int i);
    //returns the number of children 
    int get_children_number();
};

// Generate a simple tree recursively.
// Each path has length equal to depth.
// Each node has a random fanout between 1 and max_fanout.
// ISSUE: Current random seed causes all nodes in a tree to have the same fanout.
void generate_full_tree(Node *root, int depth, int max_fanout);

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
void print_tree_labels(Node *node);

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
