#ifndef COMMON_H
#define COMMON_H
#include <sstream>
#include <map>

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

// TODO replace hashtable with a custom node class that supp. strings as labels
// Creates a json string for the given node
// e.g.: the json string for the entire tree, recursively
//
// Params:  r1      the root of the (sub)tree
//          level   the int level for this level
//
// Return: a string in json format
std::string get_json_hybrid_tree (Node* root, int level, IDLabelMap hashtable,
  std::vector<int> edm)
{
  std::stringstream str;
  if (root) {
    str << "{\"scope\":" << level;
    str << ",\"label\":\"" << hashtable[root->get_label_id()] << "\"";

    if (edm[root->get_id()] == 0) {
      str << ",\"tree\":2";
    } else {
      str << ",\"tree\":1";
    }

    str << ",\"op\":";
    if(edm[root->get_id()] > 0){
      str << "\"r\"";
      str << ",\"oldlabel\":\"" << hashtable[edm[root->get_id()]] << "\"";
    } else {
      if(edm[root->get_id()] == 0) {
        str << "\"i\"";
      } else if(edm[root->get_id()] == -1) {
        str << "\"m\"";
      } else if(edm[root->get_id()] == -2) {
        str << "\"d\"";
      } else {
        str << "\"unknown\"";
      }
    }
    
    str << ",\"children\":";

    if (root->get_children_number() > 0) {
      str << "[";
      for (int i = 0; i < root->get_children_number(); ++i) {
        str << get_json_hybrid_tree(root->get_child(i), (level + 1), hashtable, edm);
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

// Fills the array with the ids of the parents,
// 
// Params:  root    the parent for its children
//          arr     the array which gets filled with (e.g.: postorder) ids (int)
void get_parents (Node* root, int* array_to_fill) {
  for(int i = 0; i < root->get_children_number(); i++){
    array_to_fill[root->get_child(i)->get_id()] = root->get_id();
    get_parents(root->get_child(i), array_to_fill);
  }
}

Node* append_node_hybrid (Node* child, Node* parent, int pos) 
{
  Node* node = new Node(child->get_id(),child->get_label_id());
  if(pos!=-1 && pos <= parent->get_children_number()){
    parent->add_child_at(node, pos);
  } else {
    parent->add_child(node);
  }  
  return node;
}

// worked with:
// ./tree_similarity "{a{b{ba{bb}}}{c}{d}}" "{a{b}{c{ca}}{d}}" -hybrid
// ./tree_similarity "{a{x{y{b}{c{z}}{d}}}}" "{a{b}{c}{d}}" -hybrid
// ./tree_similarity "{a{x{y{b}{c{z}}{d}}}}" "{a{b}{c}}" -hybrid
// ./tree_similarity "{a{b{c{d{e{f{g}}}}}}}" "{a}" -hybrid
//
// worked, also with the position !! (didnt work before)
// ./tree_similarity "{a{b{ba{bb}{bc}}}{c{ca{cb}}}{d{da{db}{dc}}}}" "{a{ba}{c{cb}}{da{dd}}}" -hybrid
// ./tree_similarity "{a{b{ba{baa}{bab}{bac}}{bb{bba}{bbb}{bbc}}{bc{bca}{bcb}{bcc}}}{c{ca}{cb{cba}{cbb}{cbc}}{cc}}{d{da}{db}{dc}}{e{ea}{eb}{ec}}}" "{a{b{ba{bab}{bac}}{bb{bba}{bbc}}{bc{bca}{bcb}}}{ca}{cb{cba}{cbc}}{cc}{d{da}{db}{dc}}{e{ea}{eb}{ec}}}" -hybrid
//
// root deleted + pos:
// ./tree_similarity "{x{a{b}{c}{d}}}" "{a{b}{c}{d}}" -hybrid
// ./tree_similarity "{x{a{a{b}{c}{d}}{b}{c}{asdf}{gggg}{d}{last}}" "{a{b}{c}{d}{d}}" -hybrid

// Mateusz's tree1: {13{4{1}{3{2}}}{10{5}{9{8{6}{7}}}}{12{11}}} (note: postorder_id as label)
// Mateusz's tree2: {13{1}{2}{8{7{3}{6{4}{5}}}}{12{10{9}}{11}}} (same)
// mapping should be: 1,1 2,2 5,3 6,4 7,5 8,6 10,8 11,9 12,12 13,13 (just mapping, no ins/dels)
// mapping is nearly correct, adjust: 10,7 to (10->8); 11,11 to (11->9);

// {13{4{1}{3{2}}}{same{5}{9{8{6}{7}}}}{12{same}}}
// {13{1}{2}{same{7{3}{6{4}{5}}}}{12{10{same}}{11}}}
// ./tree_similarity "{13{4{1}{3{2}}}{same{5}{9{8{6}{7}}}}{12{same}}}" "{13{1}{2}{same{7{3}{6{4}{5}}}}{12{10{same}}{11}}}" -hybrid
// output correct?:
// {"scope":0,"label":"13","children":[{"scope":1,"label":"4","children":[{"scope":2,"label":"1","children": null },{"scope":2,"label":"3","children":[{"scope":3,"label":"2","children": null }]}]},{"scope":1,"label":"same","children":[{"scope":2,"label":"7","children":[{"scope":3,"label":"3","children": null }]},{"scope":2,"label":"9","children":[{"scope":3,"label":"6","children":[{"scope":4,"label":"4","children": null },{"scope":4,"label":"5","children": null }]}]}]},{"scope":1,"label":"12","children":[{"scope":2,"label":"10","children":[{"scope":3,"label":"same","children": null }]},{"scope":2,"label":"11","children": null }]}]}

// TODO insert node in the right place / position
// Creates a hybrid tree based on the given edit mapping and the two trees
// (tree 2 will be taken and modified based on the edit mapping)
// (direction of the edit mapping is important)
//
// Params:  tree1      the root of the first tree
//          tree2      the root of the second tree
//          edit_mapping     the edit mapping array (a->b)
//
Node* create_hybrid_tree (Node* tree1, Node* tree2,
  std::vector<std::array<Node*, 2> > edit_mapping, std::vector<int>& operations)
{
  Node* hybrid_tree = new Node(tree2->get_id(), tree2->get_label_id());
  copy_tree(tree2, hybrid_tree);
  std::vector<Node*>* hybrid_tree_postorder = generate_postorder(hybrid_tree);
  std::vector<Node*>* tree1_postorder = generate_postorder(tree1);

  int* parents_tree1 = new int[tree1->get_subtree_size() + 1];
  parents_tree1[tree1->get_id()]=0;
  get_parents(tree1, parents_tree1);

  int* parents_hybrid = new int[hybrid_tree->get_subtree_size() + 1];
  get_parents(hybrid_tree, parents_hybrid);
  parents_hybrid[hybrid_tree->get_id()] = 0;

  IDMappedNode ht_id_to_node;
  IDMappedNode ht_id_to_node_old;
  std::array<Node*, 2> em;
  while (!edit_mapping.empty())
  {
    em = edit_mapping.back();
    edit_mapping.pop_back();
    if(em[0]!=nullptr){
      if(em[1]!=nullptr){
        ht_id_to_node.emplace(em[0]->get_id(), 
          hybrid_tree_postorder->at(em[1]->get_id()-1));
      } else {
        ht_id_to_node.emplace(em[0]->get_id(), em[1]);
      }
    }
  }
  ht_id_to_node_old = ht_id_to_node;
  // important: must be descending // IDMappedNode is descending
  for ( auto it = ht_id_to_node.begin();
        it != ht_id_to_node.end(); ++it )
  {
    if(it->first == 0) { break; }
    if(it->second==nullptr){
      Node* node_in_t1 = tree1_postorder->at(it->first-1);
      if(ht_id_to_node[parents_tree1[it->first]] != nullptr){
        int pos = -1;
        for(int i = 0; i<node_in_t1->get_children_number(); i++){
          Node* tmp_child = ht_id_to_node[node_in_t1->get_child(i)->get_id()];
          if(tmp_child!=nullptr){
            pos = ht_id_to_node[parents_tree1[it->first]]
              ->get_child_position(tmp_child);
            if(pos!=-1){
              break;
            }
          }
        }
        if(pos==-1){
          pos = tree1_postorder->at(parents_tree1[it->first]-1)
            ->get_child_position(node_in_t1);
        }
        Node* n = append_node_hybrid(node_in_t1, 
          ht_id_to_node[parents_tree1[it->first]], pos);
        ht_id_to_node[it->first] = n;
        if(node_in_t1->get_children_number() > 0){
          for(int i = 0; i<node_in_t1->get_children_number(); i++){
            Node* child_mapped = ht_id_to_node[node_in_t1->get_child(i)->get_id()];
            if(child_mapped != nullptr){
              Node* parent_child_mapped = hybrid_tree_postorder
                ->at(parents_hybrid[child_mapped->get_id()] - 1);
              parent_child_mapped->remove_child(child_mapped);
              n->add_child(child_mapped);
            }
          }
        }

      } else { // ht_id_to_node[parents_tree1[it->first]] != nullptr
        std::cout << "parent is nullptr --> must be the root" << std::endl;
        Node* tmp = hybrid_tree;
        hybrid_tree = new Node(tmp->get_id()+1, node_in_t1->get_label_id());
        hybrid_tree->add_child(tmp);
        ht_id_to_node[it->first] = hybrid_tree;
        hybrid_tree_postorder = generate_postorder(hybrid_tree);
        parents_hybrid = new int[hybrid_tree->get_subtree_size() + 1];
        get_parents(hybrid_tree, parents_hybrid);
      }
      
    }
  }

  hybrid_tree_postorder = generate_postorder(hybrid_tree);
  parents_hybrid = new int[hybrid_tree->get_subtree_size() + 1];
  get_parents(hybrid_tree, parents_hybrid);
  parents_hybrid[hybrid_tree->get_id()] = 0;

  std::cout << "preserve order" << std::endl;
  // preserve order
  for ( auto it = ht_id_to_node_old.begin();
      it != ht_id_to_node_old.end(); ++it )
  {
    if(it->first != 0 && it->second == nullptr && parents_tree1[it->first] != 0){
      Node* node_in_t1 = tree1_postorder->at(it->first-1);
      int pos = tree1_postorder->at(parents_tree1[it->first]-1)
        ->get_child_position(node_in_t1);
      int currpos = hybrid_tree_postorder->at(parents_hybrid[ht_id_to_node[it->first]
        ->get_id()]-1)->get_child_position(ht_id_to_node[it->first]);
      int childrennumberhybrid = hybrid_tree_postorder->at(parents_hybrid[ht_id_to_node[it->first]
        ->get_id()]-1)->get_children_number();
      if(pos!=currpos && pos!=-1 && currpos!=-1 && pos <= childrennumberhybrid &&
          currpos <= childrennumberhybrid) {
        std::cout << "child# " << childrennumberhybrid << " swap " << it->first << " new: " << ht_id_to_node[it->first]->get_id() << " from " << currpos << " to " << pos << std::endl;
        hybrid_tree_postorder->at(parents_hybrid[ht_id_to_node[it->first]->get_id()]-1)
          ->swap_children(pos, currpos);
          hybrid_tree_postorder = generate_postorder(hybrid_tree);
          parents_hybrid = new int[hybrid_tree->get_subtree_size() + 1];
          get_parents(hybrid_tree, parents_hybrid);
          parents_hybrid[hybrid_tree->get_id()] = 0;
      }
      
    }
  }

  operations.resize(hybrid_tree->get_subtree_size() + 1);
  for(int i = 0; i < hybrid_tree->get_subtree_size() + 1; i++){
    operations[i] = 0;
    operations[i] = 0;
  }

  for ( auto it = ht_id_to_node_old.begin();
      it != ht_id_to_node_old.end(); ++it )
  {
    if(it->second == nullptr){
      operations[ht_id_to_node[it->first]->get_id()] = -2; // del
    } else if(tree1_postorder->at(it->first - 1)->get_label_id() == it->second->get_label_id()){
      operations[ht_id_to_node[it->first]->get_id()] = -1; // map
    } else {
      operations[ht_id_to_node[it->first]->get_id()] = it->first; // ren
    }
    // 0 = ins --> standard
  }

  for(int i = 0; i < hybrid_tree->get_subtree_size() + 1; i++){
    std::cout << i << ": " << operations[i] << std::endl;
  }
  std::cout << std::endl;

  delete hybrid_tree_postorder;
  delete tree1_postorder;
  delete parents_tree1;
  delete parents_hybrid; 
  
  return hybrid_tree;
}

};

#endif // COMMON_H
