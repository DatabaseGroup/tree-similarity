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
  std::cout << n->get_label_id() << std::endl;
}

// TODO extern file
// Creates a json string for the given node
// e.g.: the json string for the entire tree, recursively
//
// Params:  r1      the root of the (sub)tree
//          level   the int level for this level
//
// Return: a string in json format
int get_json_tree(node* root, int level) {
  if (root) {
    // traverse children first
    std::cout << "{\"scope\":\"" << level << "\",\"label\":\""
              << root->get_label_id() << "\",\"children\":";

    if (root->get_children_number() > 0) {
      std::cout << "[";
      for (int i = 0; i < root->get_children_number(); ++i) {
        get_json_tree(root->get_child(i), (level + 1));
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
return 0;
}

// TODO extern file
// Creates a hybrid tree based on the given edit mapping and the two trees
// (tree 2 will be taken and modified based on the edit mapping)
// (direction of the edit mapping is important)
//
// Params:  r1      the root of the first tree
//          r2      the root of the second tree
//          edm     the edit mapping array (a->b)
//
// Return: a string in json format (why json? ids would get mixed up)
int create_hybrid_json_tree (node* r1, node* r2, std::vector<std::array<int, 2> > edm) {

  node* h1 = new node(r2->get_id(), r2->get_label_id());
  copy_tree(r2, h1);
  std::vector<node*>* post_h1 = generate_postorder(h1);
  std::cout << post_h1->size() << std::endl;
  
  // TODO 
  // figure out a way how to get the mapping to show in the json-string / tree
  // / node / whatever;
  
  std::array<int, 2> em;
  while (!edm.empty()) {
    em=edm.back();
    edm.pop_back();
    std::cout << "(" << em[0] << "->" << em[1] << ")" << std::endl;
    if(em[0]==0){
      // so i dont get unused var
      std::cout << "ins to: " << r2->get_id() << std::endl;
    } else if(em[1]==0){
      // so i dont get unused var
      std::cout << "del from: " << r1->get_id() << std::endl;
    } else {
        
    }
  }

  get_json_tree(h1,0);
  std::cout << std::endl;

  return 0;
}