#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>

#include "node.h"
#include "common.h"
#include "parser.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"
// TODO: tobi: put everything in a method e.g. called get_sbs_fs(node* t1, node* t2)
//
int main (int argc, char* argv[]) {
  if(argc < 3) {
    return 0;
  }

  // TODO replace hashtable with a custom node class that sup. strings as labels
  //std::unordered_map<std::string, int> hashtable_label_to_id;
  parser::LabelIDMap hashtable_label_to_id;
  //std::unordered_map<int, std::string> hashtable_id_to_label;
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
  //std::cout << std::endl;
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<Node, Costs<Node>>(tree1, tree2)
    << std::endl;

  std::vector<std::array<Node*, 2> > edit_mapping =
    zs::compute_edit_mapping<Node, Costs<Node>>(tree1, tree2);

  std::cout << "[beg] hybrid" << std::endl;
  Node* hybrid = common::create_hybrid_tree(tree1, tree2, edit_mapping);
  std::string hybrid_json = common::get_json_tree(hybrid, 0, hashtable_id_to_label);
  std::cout << hybrid_json << std::endl;
  hybrid_json.clear();
  std::cout << "[end] hybrid" << std::endl;

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
  
  std::string sbs = common::get_json_side_by_side(tree1, tree2, 
    hashtable_id_to_label, edit_mapping_int_array);
  std::cout << "[beg] sbs_fs\n" << sbs << "\n[end] sbs_fs " << std::endl;

  delete tree1;
  delete tree2;
  delete tree1_postorder;
  delete tree2_postorder;
  
  for (int i = 0; i < 2; ++i){
    delete[] edit_mapping_int_array[i];
  }
  delete hybrid;

	return 0;
}
