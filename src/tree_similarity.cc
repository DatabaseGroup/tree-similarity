#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>

#include "node.h"
#include "string_node.h"
#include "common.h"
#include "parser.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

// TODO: tobi: put everything in a method e.g. called get_sbs_fs(node* t1, node* t2)
//
int main (int argc, char* argv[]) {
  if(argc != 3 && argc != 4 && argc != 6) {
    std::cout << "TODO print a help message" << std::endl;
    return 0;
  }
  std::cout << argv[1] << " " << argv[2] << std::endl;
  // TODO replace hashtable with a custom node class that sup. strings as labels
  parser::LabelIDMap hashtable_label_to_id;
  common::IDLabelMap hashtable_id_to_label;
  int node_id_counter = 1;
  Node* tree1 = parser::create_tree_from_string(argv[1], hashtable_label_to_id,
    node_id_counter
  );
  Node* tree2 = parser::create_tree_from_string(argv[2], hashtable_label_to_id,
    node_id_counter
  );

  for ( auto it = hashtable_label_to_id.begin();
        it != hashtable_label_to_id.end(); ++it )
  {
    hashtable_id_to_label.emplace(it->second, it->first);
  }

  std::vector<Node*>* tree1_postorder = common::generate_postorder(tree1);
  std::vector<Node*>* tree2_postorder = common::generate_postorder(tree2);

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  
  StringNode* a1 = new StringNode("a");
  StringNode* r1 = new StringNode("r");
  StringNode* d1 = new StringNode("d");
  StringNode* e1 = new StringNode("d");
  StringNode* i1 = new StringNode("i");
  StringNode* l1 = new StringNode("l");
  StringNode* t1 = new StringNode("t");
  StringNode* k1 = new StringNode("k");
  StringNode* g1 = new StringNode("g");
  StringNode* h1 = new StringNode("h");
  
  StringNode* stree1 = a1;
  stree1->add_child(r1);
  stree1->add_child(d1);
  stree1->add_child(e1);
  e1->add_child(i1);
  e1->add_child(l1);
  e1->add_child(t1);
  t1->add_child(k1);
  t1->add_child(g1);
  t1->add_child(h1);

  StringNode* x2 = new StringNode("x");
  StringNode* stree2 = x2;

  // distance between stree1 and stree2 should be 20 (using StringCosts)
  std::cout
    << "Distance (string-labeled tree, string-labeled cost model, Zhang Shasha):\t"
    << zs::compute_zhang_shasha<StringNode, StringCosts<StringNode>>(stree1, stree2)
    << std::endl;

  /*
  std::vector<std::array<Node*, 2> > edit_mapping =
    zs::compute_edit_mapping<Node, Costs<Node>>(tree1, tree2);

  if(argc > 3) {
    std::string output;
    //std::cout << "'" << argv[3] << "'" << std::endl;
    if(argv[3] == std::string("-hybrid")){
      std::vector<int> edm;
      Node* hybrid = common::create_hybrid_graph(tree1, tree2, edit_mapping, hashtable_id_to_label);
      output = common::get_json_hybrid_graph_tree(hybrid, hashtable_id_to_label);
      //delete hybrid;
    } else if(strcmp(argv[3],"-sbs_fs")==0){
      std::cout << "sbs" << std::endl;
      int *edit_mapping_int_array[2];
      int max_nodes = std::max(tree1->get_subtree_size(), tree2->get_subtree_size());
     
      edit_mapping_int_array[0] = new int[max_nodes + 1];
      edit_mapping_int_array[1] = new int[max_nodes + 1];
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j <= max_nodes; ++j) {
          edit_mapping_int_array[i][j] = 0;
        }
      }
      zs::get_edit_mapping_int_array<Node>(edit_mapping, edit_mapping_int_array);
      
      output = common::get_json_side_by_side(tree1, tree2, 
        hashtable_id_to_label, edit_mapping_int_array);
        for (int i = 0; i < 2; ++i){
          delete[] edit_mapping_int_array[i];
        }
    }

    std::cout << output << std::endl;
    //std::cout << "output check" << std::endl;
    if(argc>=6 && (strcmp(argv[4],"-f") == 0)){
      std::fstream out;
      std::string filename = argv[5];
      filename += ".tree";
      out.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
      if (out.is_open()){
        out << output << std::endl;
        std::cout << "writing to the file: " << filename << std::endl;
      }
      out.close();
      output.clear();
    }

  }

  delete tree1;
  delete tree2;
  delete tree1_postorder;
  delete tree2_postorder;
*/
	return 0;
}
