#ifndef COMMON_H
#define COMMON_H

namespace common {

typedef std::unordered_map<int, std::string> IDLabelMap;

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
void postorder(Node* root, std::vector<Node*>* tr_post, int* node_id_counter) {
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
std::vector<Node*>* generate_postorder (Node* root) {
  int node_id_counter = 1;
  // Heap allocation
  std::vector<Node*>* tr_post = new std::vector<Node*>();

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
void generate_full_tree (Node *root, int fixed_depth, int max_fanout) {
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
    Node* new_node = new Node(random_label); // modified by stefan
    // Add node as a child of root.
    root->add_child(new_node);
    // Recursively generate consecutive levels.
    generate_full_tree(new_node, fixed_depth-1, max_fanout);
  }
}

void copy_tree(Node* t, Node* copy_t) {
  for(int i = 0; i < t->get_children_number(); i++){
    Node* temp = new Node(t->get_child(i)->get_label_id());
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
void print_tree_labels (Node* n) {
  // Print the label of node.
  // Recursively print labels of all descendants of node.
  std::vector<Node*> children = n->get_children();
  for ( std::vector<Node*>::const_iterator node_it = children.cbegin();
        node_it != children.cend(); ++node_it)
  {
    print_tree_labels(*node_it);
  }
  std::cout << n->get_label_id() << std::endl;
}

// TODO replace hashtable with a custom node class that supp. strings as labels
// Creates a json string for the given node
// e.g.: the json string for the entire tree, recursively
//
// Params:  r1      the root of the (sub)tree
//          level   the int level for this level
//
// Return: a string in json format
void get_json_tree (Node* root, int level, IDLabelMap hashtable,
  int* map = nullptr, int tree = 0)
{
  if (root) {
    std::cout << "{\"scope\":\"" << level << "\"";
    std::cout << ",\"label\":\"" << hashtable[root->get_label_id()] << "\"";
    
    if (tree != 0) {
      std::cout << ",\"tree\":" << tree << "";
    }

    if (map != nullptr) {
      std::cout << ",\"mappedTo\":" << map[root->get_id()] << "";
    }
    std::cout << ",\"children\":";

    if (root->get_children_number() > 0) {
      std::cout << "[";
      for (int i = 0; i < root->get_children_number(); ++i) {
        get_json_tree(root->get_child(i), (level + 1), hashtable, map, tree);
        if((i + 1) < root->get_children_number()){
          std::cout << ",";
        }
      }
      std::cout << "]";
    } else {
      std::cout << " null ";
    }
    std::cout << "}";
  }
}

// Fills the array with the ids of the parents,
// 
// Params:  root    the parent for its children
//          arr     the array which gets filled with (e.g.: postorder) ids (int)
void get_parents (Node* root, int arr[]) {
  for(int i = 0; i < root->get_children_number(); i++){
    arr[root->get_child(i)->get_id()] = root->get_id();
    get_parents(root->get_child(i), arr);
  }
}

// TODO update since edm has been updated
// Creates a hybrid tree based on the given edit mapping and the two trees
// (tree 2 will be taken and modified based on the edit mapping)
// (direction of the edit mapping is important)
//
// Params:  r1      the root of the first tree
//          r2      the root of the second tree
//          edm     the edit mapping array (a->b)
//
// ignore for now: Return: a string in json format (why json? ids would get mixed up)
Node* create_hybrid_tree (Node* r1, Node* r2,
  std::vector<std::array<Node*, 2> > edm)
{

  Node* hybrid = new Node(r2->get_id(), r2->get_label_id());
  copy_tree(r2, hybrid);
  std::vector<Node*>* post_hybrid = generate_postorder(hybrid);
  std::vector<Node*>* post_r1 = generate_postorder(r1);
  std::cout << "post_hybrid: " << post_hybrid->size() << std::endl;
  //set_tree_id(hybrid,2);

  int* parents_r1 = new int[r1->get_subtree_size()+1];
  get_parents(r1, parents_r1);
  for(int i = 1; i < r1->get_subtree_size()+1; i++){
    std::cout << "parent(" << i << "): " << parents_r1[i] << std::endl;
  }

  // TODO
  // figure out a way how to get the mapping to show in the json-string / tree
  // / node / whatever;

  // free everything not needed after the function call - ok
  delete post_hybrid;
  delete post_r1;
  delete parents_r1;
  return hybrid;
}

};

#endif // COMMON_H
