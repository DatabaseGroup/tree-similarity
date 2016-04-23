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
  node* lml;

public:
  // Constructors
  node (int id, int label_id, int children_number)
    : id(id), label_id(label_id), children_number(children_number)
  { }

  node (int label_id)
    : label_id(label_id)
  { }

  node (int id, int label_id)
    : id(id), label_id(label_id)
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

  void set_lml(node* n) {
    this->lml = n;
  }

  node* get_lml() {
    return this->lml;
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

typedef std::unordered_map<std::string, int> ht_ids;

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
void postorder(node* root, std::vector<node*>* tr_post, int* node_id_counter) {
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

// Generates the postorder for a tree rooted at a given root node.
//
// Params:  root  The root node of the tree to be 'postorderified'
//
// Return:  A pointer to a vector of node pointers of the 'postorderified' tree
std::vector<node*>* generate_postorder (node* root) {
  int node_id_counter = 1;
  // Heap allocation
  std::vector<node*>* tr_post = new std::vector<node*>();

  // Recursively traverse tree in postorder
  postorder(root, tr_post, &node_id_counter);
  return tr_post;
}

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

void copy_tree(node* t, node* copy_t) {
  for(int i = 0; i < t->get_children_number(); i++){
    node* temp = new node(t->get_child(i)->get_label_id());
    copy_t->add_child(temp);
    copy_tree(t->get_child(i), temp);
  }
}

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
//
// Params:  node  The root node of the labels to be printed
//
// Return:  None
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

node* generate_tree_from_string (char* str, ht_ids& hashtable, int& labelid) {
  //std::cout << "----\ngenerating tree" << std::endl;  
  int length = std::strlen(str);
  //std::cout << "length: " << length << " string: " << str << std::endl;
  int scope = -1;
   
  std::vector<node*> scopeParentList;
  std::vector<node*>::const_iterator it;

  std::string label = "";
    
  node* root = new node(1,1);
  for (int i = 0; i < length; i++) {
    if (str[i] == '{') {
      if (label.length() != 0) {
        //std::cout << label << ":" << scope << std::endl;
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }

        node* tmpnode = new node(hashtable[label]);
        scopeParentList.push_back(tmpnode);
        
        if (scope > 0) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << std::endl;
          tmp->add_child(tmpnode);
        } else {
          root = tmpnode;
        }
        label = "";
      }
      scope++;
    } else if (str[i] == '}') {
      if(label.length() != 0){
        //std::cout << label << ":" << scope << std::endl;
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }
        
        node* tmpnode = new node(hashtable[label]);
        std::vector<node*>::reverse_iterator tmp_it = scopeParentList.rbegin();
        for (; tmp_it != scopeParentList.rend(); ++tmp_it) {
          node* tmp_par = *tmp_it;
          if (tmp_par->get_lml()) {
            break;
          } else {
            tmp_par->set_lml(tmpnode);
          }
        }
        
        tmpnode->set_lml(tmpnode);
        scopeParentList.push_back(tmpnode);
        
        if (scope > 0 && scopeParentList.size() > 1) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << std::endl;
          tmp->add_child(tmpnode);
        }
        label = "";
      }
      
      scopeParentList.resize(scope);
      --scope;
    } else {
      label += str[i];
    }
  }
  
  //std::cout << "generating tree [done]\n----"<< std::endl;

  return root;
}

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
