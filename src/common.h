#ifndef COMMON_H
#define COMMON_H
#include <sstream>
#include <map>
#include <set>
#include <array>

namespace common {

typedef std::unordered_map<int, std::string> IDLabelMap;
typedef std::map<int, Node*, std::greater<int>> IDMappedNode;

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
  std::vector<Node*>* tree_postorder = new std::vector<Node*>();

  // Recursively traverse tree in postorder
  postorder(root, tree_postorder, &node_id_counter);
  return tree_postorder;
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
    Node* new_node = new Node(random_label); // modified by stefan
    // Add node as a child of root.
    root->add_child(new_node);
    // Recursively generate consecutive levels.
    generate_full_tree(new_node, fixed_depth-1, max_fanout);
  }
}

void copy_tree(Node* original, Node* copy) {
  for(int i = 0; i < original->get_children_number(); i++){
    Node* temp = new Node(original->get_child(i)->get_label_id());
    copy->add_child(temp);
    copy_tree(original->get_child(i), temp);
  }
}

void copy_tree_with_colour(Node* original, Node* copy, char colour) {
  copy->set_colour(colour);
  for(int i = 0; i < original->get_children_number(); i++){
    Node* temp = new Node(original->get_child(i)->get_label_id());
    copy->add_child(temp);
    copy->add_edge(colour);
    copy_tree_with_colour(original->get_child(i), temp, colour);
  }
}

// Print recursively the labels of all descendants of node, including node.
// Each label is printed in a separate line.
//
// Params:  node  The root node of the labels to be printed
//
// Return:  None
void print_tree_labels (Node* node) {
  // Print the label of node.
  // Recursively print labels of all descendants of node.
  std::vector<Node*> children = node->get_children();
  for ( std::vector<Node*>::const_iterator node_it = children.cbegin();
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
std::string get_json_tree (Node* root, int level, IDLabelMap hashtable,
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
        str << get_json_tree(root->get_child(i), (level + 1), hashtable, map, tree);
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

std::string get_json_side_by_side(Node* tree1, Node* tree2, 
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

std::string gather_links_nodes_hybrid_graph(Node* root, std::set<Node*>& visited, int& id_counter, IDLabelMap ht) {
  std::stringstream str;
  if(visited.find(root) == visited.end()){
    visited.insert(root);
    root->set_id(id_counter);
    id_counter++;
    for(int i = 0; i < root->get_children_number(); i++){
      Node* tmp_c = root->get_child(i);
      if(tmp_c->get_level() < (root->get_level()+1)){
        tmp_c->set_level(root->get_level()+1);
      }
      str << gather_links_nodes_hybrid_graph(tmp_c, visited, id_counter, ht);
      str << "{\"source\": " << root->get_id() << ", \"target\": " << tmp_c->get_id() << ", \"colour\": \"" << root->get_edge_colour(i) << "\"}";
      if(root->get_level()!=0 || i+1!=root->get_children_number()){ str << ","; }
    }
  }

  return str.str();
}

std::string get_json_hybrid_graph (Node* root, IDLabelMap ht)
{
  std::stringstream str;

  int id_counter = 1;
  std::set<Node* > visited;
  root->set_level(0);
  char separator = ' ';
  str << "{\"links\": [" << gather_links_nodes_hybrid_graph(root, visited, id_counter, ht) << "]";
  str << ", \"nodes\": [";
  for (std::set<Node*>::iterator it = visited.begin(); it != visited.end(); ++it)
  {
    Node* n = *it;
    str << separator << "{\"name\": \"" << ht[n->get_label_id()] << "\", \"id\":" << n->get_id() << ", \"scope\":\"" << n->get_level() << "\", \"colour\":\"" << n->get_colour() << "\"}";
    separator = ',';  
  }
  str << "]}";
  return str.str();
}



// Fills the array with the ids of the parents,
// 
// Params:  root    the parent for its children
//          arr     the array which gets filled with (e.g.: postorder) ids (int)
void get_parents (Node* root, int* array_to_fill, char colour = char(0)) {
  for(int i = 0; i < root->get_children_number(); i++){
    if(colour == char(0)){
      array_to_fill[root->get_child(i)->get_id()] = root->get_id();
      get_parents(root->get_child(i), array_to_fill, colour);
    } else if(root->get_edge_colour(i) == colour || root->get_edge_colour(i) == 'm'){
      array_to_fill[root->get_child(i)->get_id()] = root->get_id();
      get_parents(root->get_child(i), array_to_fill, colour);
    }
  }
}

// check if two trees are the same (speaking of labels & structure)
bool check_if_same_trees(Node* t1, Node* t2){
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

bool check_if_same_trees(Node* t1, Node* t2, char colour){
  bool result = false;

  if(t1->get_label_id() == t2->get_label_id()){
    if(t1->get_colour() == 'm' || t1->get_colour() == t2->get_colour()){
      int t1_edges = t1->get_edge_colours_count(colour, 'm');
      int t2_edges = t2->get_edge_colours_count(colour, 'm');
      if(t1_edges == t2_edges){
        int j = 0;
        for(int i = 0; i < t1->get_children_number(); i++){
          if(t1->get_edge_colour(i) == colour || t1->get_edge_colour(i) == 'm'){
            if(t1->get_edge_colour(i) == 'm' || t1->get_edge_colour(i) == t2->get_edge_colour(j)){
              result = check_if_same_trees(t1->get_child(i), t2->get_child(j), colour);
            } else {
              std::cout << "diff: edge colour at: " << t1->get_id() << "; edge nr.: " << i << std::endl;
              return false;
            }
            j++;          
            if(!result) { return false; }
          }
        }
        result = true;
      } else {
        std::cout << "diff: children at: " << t1->get_id() << ": " << t1_edges << "(/" << t1->get_children_number() << ") <-> " << t2_edges << std::endl;
        for(int i = 0; i < t1->get_children_number(); i++){
          if(t1->get_edge_colour(i) == colour || t1->get_edge_colour(i) == 'm'){
            std::cout << "src: " << t1->get_id() << "; target: " << t1->get_child(i)->get_id() << "; " << t1->get_edge_colour(i) << std::endl;
          }
        }
        std::cout << "t1 / t2" << std::endl;
        for(int i = 0; i < t2->get_children_number(); i++){
          std::cout << "src: " << t2->get_id() << "; target: " << t2->get_child(i)->get_id() << "; " << t2->get_edge_colour(i) << std::endl;
        }
      }
    } else {
      std::cout << "diff: colour at: " << t1->get_id() << std::endl;
      std::cout << " - t1: " << t1->get_colour() << "; t2: " << t2->get_colour() << std::endl;
    }
  } else {
    std::cout << "diff: label at: " << t1->get_id() << std::endl;
  }
  
  return result;
}

// prints the given tree indented using dots
void print_tree_indented(Node* n, int level, IDLabelMap hashtable_id_to_label){
  for(int i = 0; i < level; i++){
    std::cout << ".";
  }
  std::cout << hashtable_id_to_label[n->get_label_id()] << " (id: "
    << n->get_id() << ")" << std::endl;
  for(int i = 0; i < n->get_children_number(); i++){
    print_tree_indented(n->get_child(i), (level+1),hashtable_id_to_label);
  }
}

// prints the given tree indented using dots and respects the given colour
void print_tree_indented(Node* n, int level, IDLabelMap hashtable_id_to_label, char colour){
  if(n->get_colour() == 'm' || n->get_colour() == colour){
    for(int i = 0; i < level; i++){
      std::cout << ".";
    }
    std::cout << hashtable_id_to_label[n->get_label_id()] << " (id: "
      << n->get_id() << ") (c: " << n->get_colour() << ")" << std::endl;
    for(int i = 0; i < n->get_children_number(); i++){
      if(n->get_edge_colour(i) == 'm' || n->get_edge_colour(i) == colour){
        print_tree_indented(n->get_child(i), (level+1),hashtable_id_to_label, colour);
      }
    }
  }
}

// append a node to the hybrid graph at a given pos and a colour
Node* append_node_hybrid (Node* child, Node* parent, int pos, char colour) 
{
  // child->get_id()
  Node* node = new Node(0,child->get_label_id());
  node->set_colour(colour);
  if(pos!=-1 && pos <= parent->get_children_number()){
    parent->add_child_at(node, pos);
    parent->add_edge_at(colour, pos);
  } else {
    parent->add_child(node);
    parent->add_edge(colour);
  }  
  return node;
}

// creates a hybrid graph, based on two trees and their given edit mapping
// colors each node and edge according to the operation:
// 'm' = mapped (=black)
// 'r' = red (tree1)
// 'b' = blue (tree2)
void colour_hybrid_graph(Node*& hybrid, Node* t2, IDMappedNode& ht_t2_to_h, 
  int* parents_h, int* parents_t2, std::vector<Node*>* postorder_h,
  std::vector<Node*>* postorder_t2)
{

  if(ht_t2_to_h[t2->get_id()]!=nullptr){
    // Node is mapped, therefore mark it as black / mapped / 'm'

    ht_t2_to_h[t2->get_id()]->set_colour('m');
    if(parents_t2[t2->get_id()]!=0 && 
      parents_h[ht_t2_to_h[t2->get_id()]->get_id()] != 0)
    {
      if(ht_t2_to_h[parents_t2[t2->get_id()]]->get_id() == 
        postorder_h->at(parents_h[ht_t2_to_h[t2->get_id()]->get_id()]-1)->get_id())
      {
        // Node is mapped to the same parent --> colour the edge black (='m')

        int position = ht_t2_to_h[parents_t2[t2->get_id()]]
          ->get_child_position(ht_t2_to_h[t2->get_id()]);
        ht_t2_to_h[parents_t2[t2->get_id()]]->set_edge_colour(position, 'm');

      } else {
        // Node is mapped to a different parent --> add blue edge to b parent
        // the edge to the red parent stays red

        // get the position of its (if any) left sibling,
        // so we get the positioning right
        Node* par_h = ht_t2_to_h[parents_t2[t2->get_id()]];
        Node* par_t2 = postorder_t2->at(parents_t2[t2->get_id()]-1);
        int position = par_t2->get_child_position(t2);
        if(position != 0) {
          Node* left_sib = ht_t2_to_h[par_t2->get_child(position-1)->get_id()];
          int position_l_sib = par_h->get_child_position(left_sib);
          if(position_l_sib != -1 && position_l_sib >= position){
            position = position_l_sib + 1;
          }
        }
        ht_t2_to_h[parents_t2[t2->get_id()]]
          ->add_child_at(ht_t2_to_h[t2->get_id()], position);
        ht_t2_to_h[parents_t2[t2->get_id()]]->add_edge_at('b', position);
      
      }

    }

  } else {
    // Node is not mapped
    
    if(parents_t2[t2->get_id()] != 0){
      // insert the node --> blue; edge to parent: blue

      // get the position of its (if any) left sibling,
      // so we get the positioning right
      Node* par_h = ht_t2_to_h[parents_t2[t2->get_id()]];
      Node* par_t2 = postorder_t2->at(parents_t2[t2->get_id()]-1);
      int position = par_t2->get_child_position(t2);
      if(position != 0) {
        Node* left_sib = ht_t2_to_h[par_t2->get_child(position-1)->get_id()];
        int position_l_sib = par_h->get_child_position(left_sib);
        if(position_l_sib != -1 && position_l_sib >= position){
          position = position_l_sib + 1;
        }
      }
      Node* n = append_node_hybrid(t2, par_h, position, 'b');
      ht_t2_to_h[t2->get_id()] = n;

    } else {
      // root gets inserted
      Node* tmp = hybrid;
      hybrid = new Node(0, t2->get_label_id());
      hybrid->add_child(tmp);
      hybrid->add_edge('b');
      hybrid->set_colour('b');
      ht_t2_to_h[t2->get_id()] = hybrid;

    }

  }

  for(int i = 0; i < t2->get_children_number(); i++){
    colour_hybrid_graph(hybrid, t2->get_child(i), ht_t2_to_h, parents_h, 
      parents_t2, postorder_h, postorder_t2);
  }
}


// creates a hybrid graph, based on two trees and their given edit mapping
Node* create_hybrid_graph (Node* tree1, Node* tree2,
  std::vector<std::array<Node*, 2> > edit_mapping,
  IDLabelMap hashtable_id_to_label)
{
  IDMappedNode ht_t2_to_h;
  std::vector<Node*>* postorder_t1 = generate_postorder(tree1);
  std::vector<Node*>* postorder_t2 = generate_postorder(tree2);

  Node* hybrid = new Node(tree1->get_id(), tree1->get_label_id());
  copy_tree_with_colour(tree1, hybrid, 'r');
  std::vector<Node*>* postorder_hybrid = generate_postorder(hybrid);
  Node* hybrid_check_t1 = hybrid;

  std::vector<std::array<Node*,2> >::iterator it;
  std::array<Node*, 2> em;
  int inserted = 0;
  for(it=edit_mapping.begin() ; it < edit_mapping.end(); it++) {
    em = *it;
    if(em[1] != nullptr){
      
      if(em[0] == nullptr){
        inserted++;
        ht_t2_to_h.emplace(em[1]->get_id(), nullptr);
      } else {
        ht_t2_to_h.emplace(em[1]->get_id(), 
          postorder_hybrid->at(em[0]->get_id()-1));
      }
    }
  }

  int* parents_h = new int[hybrid->get_subtree_size() + 1];
  parents_h[0] = 0;
  get_parents(hybrid, parents_h, 'r');
  parents_h[hybrid->get_id()] = 0;

  int* parents_t2 = new int[tree2->get_subtree_size() + 1];
  parents_t2[0] = 0;
  get_parents(tree2, parents_t2);
  parents_t2[tree2->get_id()] = 0;

  // create the hybrid graph and colour the nodes and edges
  hybrid->set_id(tree1->get_subtree_size() + inserted);
  colour_hybrid_graph(hybrid, tree2, ht_t2_to_h, parents_h, parents_t2,
    postorder_hybrid, postorder_t2);

  Node* tree1_r = new Node(tree1->get_id(), tree1->get_label_id());
  copy_tree_with_colour(tree1, tree1_r, 'r');

  Node* tree2_b = new Node(tree2->get_id(), tree2->get_label_id());
  copy_tree_with_colour(tree2, tree2_b, 'b');

  // check with the mapped node --> maybe the root of T2 got inserted
  bool check_t1 = check_if_same_trees(hybrid_check_t1, tree1_r, 'r');
  if(!check_t1){
    print_tree_indented(hybrid, 0, hashtable_id_to_label, 'r');
    std::cout << "hybrid end\n" << std::endl;
    print_tree_indented(tree1_r, 0, hashtable_id_to_label, 'r');
    std::cout << "tree1_r end\n" << std::endl;
  }

  // rename here, or else we'd have to re-rename (=undo before t1 check)
  // and rename again for final hybrid graph and t2 check
  for(it=edit_mapping.begin() ; it < edit_mapping.end(); it++) {
    em = *it;
    if(em[0] != nullptr && em[1] != nullptr){
      postorder_hybrid->at(em[0]->get_id()-1)
        ->set_label_id(em[1]->get_label_id());
    }
  }
  
  // check with the mapped node --> maybe the root of T1 got deleted
  bool check_t2 = check_if_same_trees(ht_t2_to_h[tree2->get_id()], tree2_b,
    'b');

  if(!check_t2){
    print_tree_indented(ht_t2_to_h[tree2->get_id()], 0, hashtable_id_to_label,
      'b');
    std::cout << "hybrid end\n" << std::endl;
    print_tree_indented(tree2_b, 0, hashtable_id_to_label, 'b');
    std::cout << "tree2_b end\n" << std::endl;
  }

  if(check_t1 && check_t2){
    std::cout << "ok!" << std::endl;
  } else {
    std::cout << "NOT ok !!" << std::endl;
  }

  delete postorder_hybrid;
  delete postorder_t1;
  delete postorder_t2;
  delete[] parents_h;
  delete[] parents_t2;

  return hybrid;
}

};

#endif // COMMON_H
