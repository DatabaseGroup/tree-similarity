#include <iostream>
#include <random>
#include <unordered_map>

#include "node.h"
#include "common.h"
#include "parser.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

int main (int argc, char* argv[]) {
  if(argc != 3) {
    return 0;
  }

  // TODO replace hashtable with a custom node class that sup. strings as labels
  std::unordered_map<std::string, int> hashtable_label_to_id;
  std::unordered_map<int, std::string> hashtable_id_to_label;
  int node_id_counter = 1;
  node* t1 = create_tree_from_string(argv[1], hashtable_label_to_id, node_id_counter);
  node* t2 = create_tree_from_string(argv[2], hashtable_label_to_id, node_id_counter);

  for ( auto it = hashtable_label_to_id.begin(); it != hashtable_label_to_id.end(); ++it ){
    hashtable_id_to_label.emplace(it->second, it->first);
  }

  std::vector<node*>* post1 = generate_postorder(t1);
  std::vector<node*>* post2 = generate_postorder(t2);

/*
  get_json_tree(t1, 0, hashtable_id_to_label);
  std::cout << "\n-" << std::endl;
  get_json_tree(t2, 0, hashtable_id_to_label);
  std::cout << std::endl;
*/
  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << std::endl;
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs<node>>(t1, t2)
    << std::endl;

  std::vector<std::array<node*, 2> > edm = zs::compute_edit_mapping<node, costs<node>>(t1,t2);
/*
  std::cout << "[beg] hybrid" << std::endl;
  node* hybrid = create_hybrid_tree(t1, t2, edm);
  get_json_tree(hybrid,0,hashtable_id_to_label);
  std::cout << "[end] hybrid" << std::endl;
*/

  std::cout << "[beg] edit mapping pretty (int)" << std::endl;
  zs::print_pretty_edit_mapping<node>(edm);
  std::cout << "[end] edit mapping pretty (int)" << std::endl;  

  std::array<node*, 2> em;
  std::cout << "[beg] edit mapping" << std::endl;
  while (!edm.empty())
  {
    em=edm.back();
    edm.pop_back();
    std::cout << "(" << em[0] << "->" << em[1] << ")" << std::endl;
  }
  std::cout << "[end] edit mapping" << std::endl;

  std::cout << "hashtable_label_to_id contains:";
  for ( auto it = hashtable_label_to_id.begin(); it != hashtable_label_to_id.end(); ++it ) {
    std::cout << " " << it->first << ":" << it->second;
  }
  std::cout << std::endl;

  delete t1;
  delete t2;
  delete post1;
  delete post2;
  //delete hybrid;

	return 0;
}
