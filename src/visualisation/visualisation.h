#ifndef VISUALISATION_H
#define VISUALISATION_H

#include <sstream>
#include <map>
#include <set>
#include <array>

#include "../nodes/hybrid_node.h"

namespace visualisation {


typedef std::unordered_map<int, std::string> IDLabelMap;
typedef std::map<int, nodes::HybridNode*, std::greater<int>> IDMappedNode;

template<class _node = nodes::HybridNode>
void copy_tree_with_colour(_node* original, _node* copy, char colour) {
  copy->set_colour(colour);
  for(int i = 0; i < original->get_children_number(); i++){
    _node* temp = new _node(original->get_child(i)->get_label_id());
    copy->add_child(temp);
    copy->add_edge(colour);
    copy_tree_with_colour(original->get_child(i), temp, colour);
  }
}

template<class _node = nodes::HybridNode>
std::string gather_links_nodes_hybrid_graph(_node* root, 
  std::set<_node*>& visited, int& id_counter, IDLabelMap ht) 
{
  std::stringstream str;
  if(visited.find(root) == visited.end()){
    visited.insert(root);
    root->set_id(id_counter);
    id_counter++;
    for(int i = 0; i < root->get_children_number(); i++){
      _node* tmp_c = root->get_child(i);
      str << gather_links_nodes_hybrid_graph(tmp_c, visited, id_counter, ht);
      str << "{\"source\": " << root->get_id() << ",\"target\": " << tmp_c->get_id()
      << ",\"colour\":\"" << root->get_edge_colour(i) << "\"}";
      if(root->get_id()!=1 || (i+1)!=root->get_children_number()) {
        str << ",";
      }
    }
  }
  return str.str();
}

template<class _node = nodes::HybridNode>
void increase_scope(_node* n) {
  for(int i = 0; i < n->get_children_number(); i++){
    if(n->get_child(i)->get_level() <= n->get_level()){
      n->get_child(i)->set_level(n->get_level()+1);
      increase_scope(n->get_child(i));
    }
  }
}

template<class _node = nodes::HybridNode>
std::string get_json_hybrid_graph(_node* root, IDLabelMap ht)
{
  std::stringstream str;

  int id_counter = 1;
  std::set<_node* > visited;
  root->set_level(0);
  increase_scope(root);
  char separator = ' ';
  str << "{\"links\": [" << gather_links_nodes_hybrid_graph(root, visited,
    id_counter, ht) << "]";
  str << ", \"nodes\": [";
  typename std::set<_node*>::iterator it;
  for(it = visited.begin(); it != visited.end(); ++it)
  {
    _node* n = *it;
    str << separator << "{\"name\": \"" << ht[n->get_label_id()] << 
      "\", \"id\": " << n->get_id() << ", \"colour\": \"" << n->get_colour() << "\",\"scope\":"<< n->get_level() <<", \"children\":";
    if(n->get_children_number() != 0){
      str << "\"";
      for(int i = 0; i < n->get_children_number(); i++){
        str << n->get_child(i)->get_id();
        if(i+1!=n->get_children_number()){
           str << ".";
        }
      }
      str << "\"";
    } else {
      str << "null";
    }
    str << "}";
    separator = ',';  
  }
  str << "]}";
  std::cout << "end" << std::endl;
  return str.str();
}

template<class _node = nodes::HybridNode>
std::string get_json_hybrid_tree(_node* root, IDLabelMap ht, std::set<_node*>& is_child, int pos, std::vector<std::string>& edges, char edgecolour = ' '){
  std::stringstream str;
  str << "{\"label\":\""<< ht[root->get_label_id()] << "\","
      << "\"id\":" << root->get_id() << ",\"pos\":" << pos << ",\"colour\":\"" << root->get_colour() << "\"";
  if(edgecolour!=' '){
    str << ",\"ec\":\""<< edgecolour << "\""; // ec = edge colour
  }
  if(root->get_children_number()>0){
    bool empty = true;
    std::stringstream str_tmp;
    for(int i = 0; i<root->get_children_number(); i++){
      _node* tmp_c = root->get_child(i);
      if(tmp_c->get_level() == (root->get_level()+1)){ // maybe consider an array here instead of a set
        if(is_child.find(tmp_c) == is_child.end()){
          is_child.insert(tmp_c);
          if((i != 0 && !empty) || (!empty && (i+1) != root->get_children_number())){
            str_tmp << ",";
          }
          str_tmp << get_json_hybrid_tree(tmp_c, ht, is_child, i, edges, root->get_edge_colour(i));
          empty = false;
        } else {
          std::stringstream k; k << "{\"sourceid\":" << root->get_id() << ", \"targetid\":" << tmp_c->get_id() << ",\"colour\":\""<< root->get_edge_colour(i)<<"\"}";
          std::cout << "{\"sourceid\":" << ht[root->get_label_id()] << ", \"targetid\":" << ht[tmp_c->get_label_id()] << ",\"colour\":\""<< root->get_edge_colour(i)<<"\"}";
          edges.push_back(k.str());
        }
      } else {
        std::stringstream k; k << "{\"sourceid\":" << root->get_id() << ", \"targetid\":" << tmp_c->get_id() << ",\"colour\":\""<< root->get_edge_colour(i)<<"\"}";
        std::cout << "{\"sourceid\":" << ht[root->get_label_id()] << ", \"targetid\":" << ht[tmp_c->get_label_id()] << ",\"colour\":\""<< root->get_edge_colour(i)<<"\"}";
        edges.push_back(k.str());
      }
      std::cout << ht[root->get_label_id()] << " : " << ht[root->get_child(i)->get_label_id()] << std::endl;
    }
    if(!empty) {
      str << ",\"children\": [" << str_tmp.str() << "]";
    } else {
      std::cout << "emtpy?!" << std::endl;
    }
  }
  str << "}";
  return str.str();
}

template<class _node = nodes::HybridNode>
std::string get_json_hybrid_graph_tree(_node* root, IDLabelMap ht)
{
  std::stringstream str;
  std::vector<std::string> edges;

  std::set<_node* > is_child;
  root->set_level(0);
  increase_scope(root);
  char separator = ' ';
  str << "{\"tree\": [";
  str << get_json_hybrid_tree(root, ht, is_child, 0,edges);
  str << "], \"addlinks\": [";
  typename std::vector<std::string>::iterator it;
  for(it = edges.begin(); it != edges.end(); ++it)
  {
    std::string e = *it;
    str << separator << e ;
    separator = ',';
  }
  str << "]}";
  
  return str.str();
}

// Fills the array with the ids of the parents,
// 
// Params:  root    the parent for its children
//          arr     the array which gets filled with (e.g.: postorder) ids (int)
template<class _node = nodes::HybridNode>
void get_parents(_node* root, int* array_to_fill, char colour = char(0)) {
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

template<class _node = nodes::HybridNode>
bool check_if_same_trees(_node* t1, _node* t2, char colour){
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
              result = check_if_same_trees<_node>(t1->get_child(i), t2->get_child(j), colour);
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
        std::cout << "diff: children at: " << t1->get_id() << ": " 
          << t1_edges << "(/" << t1->get_children_number() << ") <-> " 
          << t2_edges << std::endl;
        for(int i = 0; i < t1->get_children_number(); i++){
          if(t1->get_edge_colour(i) == colour || t1->get_edge_colour(i) == 'm'){
            std::cout << "src: " << t1->get_id() << "; target: "
              << t1->get_child(i)->get_id() << "; " <<
              t1->get_edge_colour(i) << std::endl;
          }
        }
        std::cout << "t1 / t2" << std::endl;
        for(int i = 0; i < t2->get_children_number(); i++){
          std::cout << "src: " << t2->get_id() << "; target: " <<
            t2->get_child(i)->get_id() << "; " << t2->get_edge_colour(i)
            << std::endl;
        }
      }
    } else {
      std::cout << "diff: colour at: " << t1->get_id() << std::endl;
      std::cout << " - t1: " << t1->get_colour() << "; t2: "
        << t2->get_colour() << std::endl;
    }
  } else {
    std::cout << "diff: label at: " << t1->get_id() << std::endl;
  }
  
  return result;
}


// prints the given tree indented using dots and respects the given colour
template<class _node = nodes::HybridNode>
void print_tree_indented(_node* n, int level,
  IDLabelMap hashtable_id_to_label, char colour)
{
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
template<class _node = nodes::HybridNode>
_node* append_node_hybrid(_node* child, _node* parent, int pos, char colour) 
{
  // child->get_id()
  _node* node = new _node(0,child->get_label_id());
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
template<class _node = nodes::HybridNode>
void colour_hybrid_graph(_node*& hybrid, _node* t2, std::map<int, _node*, std::greater<int>>& ht_t2_to_h, 
  int* parents_h, int* parents_t2, std::vector<_node*>* postorder_h,
  std::vector<_node*>* postorder_t2)
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
        _node* par_h = ht_t2_to_h[parents_t2[t2->get_id()]];
        _node* par_t2 = postorder_t2->at(parents_t2[t2->get_id()]-1);
        int position = par_t2->get_child_position(t2);
        if(position != 0) {
          _node* left_sib = ht_t2_to_h[par_t2->get_child(position-1)->get_id()];
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
      _node* par_h = ht_t2_to_h[parents_t2[t2->get_id()]];
      _node* par_t2 = postorder_t2->at(parents_t2[t2->get_id()]-1);
      int position = par_t2->get_child_position(t2);
      if(position != 0) {
        _node* left_sib = ht_t2_to_h[par_t2->get_child(position-1)->get_id()];
        int position_l_sib = par_h->get_child_position(left_sib);
        if(position_l_sib != -1 && position_l_sib >= position){
          position = position_l_sib + 1;
        }
      }
      _node* n = append_node_hybrid(t2, par_h, position, 'b');
      n->set_id(hybrid->get_id() + t2->get_id());
      ht_t2_to_h[t2->get_id()] = n;

    } else {
      // root gets inserted
      _node* tmp = hybrid;
      hybrid = new _node(hybrid->get_id() + t2->get_id(), t2->get_label_id());
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

template<class _node = nodes::HybridNode>
// creates a hybrid graph, based on two trees and their given edit mapping
_node* create_hybrid_graph(_node* tree1, _node* tree2,
  std::vector<std::array<_node*, 2> > edit_mapping,
  IDLabelMap hashtable_id_to_label)
{
  std::map<int, _node*, std::greater<int>> ht_t2_to_h;
  std::vector<_node*>* postorder_t1 = common::generate_postorder<_node>(tree1);
  std::vector<_node*>* postorder_t2 = common::generate_postorder<_node>(tree2);

  _node* hybrid = new _node(tree1->get_id(), tree1->get_label_id());
  visualisation::copy_tree_with_colour<_node>(tree1, hybrid, 'r');
  std::vector<_node*>* postorder_hybrid = common::generate_postorder<_node>(hybrid);
  _node* hybrid_check_t1 = hybrid;

  typename std::vector<std::array<_node*,2> >::iterator it;
  typename std::array<_node*, 2> em;
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
  colour_hybrid_graph<_node>(hybrid, tree2, ht_t2_to_h, parents_h, parents_t2,
    postorder_hybrid, postorder_t2);

  _node* tree1_r = new _node(tree1->get_id(), tree1->get_label_id());
  copy_tree_with_colour(tree1, tree1_r, 'r');

  _node* tree2_b = new _node(tree2->get_id(), tree2->get_label_id());
  copy_tree_with_colour(tree2, tree2_b, 'b');

  // check with the mapped node --> maybe the root of T2 got inserted
  bool check_t1 = visualisation::check_if_same_trees<_node>(hybrid_check_t1, tree1_r, 'r');
  if(!check_t1){
    print_tree_indented<_node>(hybrid, 0, hashtable_id_to_label, 'r');
    std::cout << "hybrid end\n" << std::endl;
    print_tree_indented<_node>(tree1_r, 0, hashtable_id_to_label, 'r');
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
  bool check_t2 = visualisation::check_if_same_trees<_node>(ht_t2_to_h[tree2->get_id()], tree2_b,
    'b');
/*
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
  }*/

  delete postorder_hybrid;
  delete postorder_t1;
  delete postorder_t2;
  delete[] parents_h;
  delete[] parents_t2;

  return hybrid;
}
/**/

} //namespace

#endif //VISUALISATION_H


