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

  if(argc != 3) { return 0; }
  
  std::unordered_map<std::string, int> hashtable;
  int node_id_counter = 1;
  node* t1 = create_tree_from_string(argv[1], hashtable, node_id_counter);
  node* t2 = create_tree_from_string(argv[2], hashtable, node_id_counter);

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs<node>>(t1, t2)
    << std::endl;

  std::vector<std::array<int, 2> > edm = zs::compute_edit_mapping<node, costs<node>>(t1,t2);

  std::cout << "[beg] hybrid" << std::endl;
  create_hybrid_json_tree(t1, t2, edm);
  std::cout << "[end] hybrid" << std::endl;

  std::array<int, 2> em;
  std::cout << "[beg] edit mapping" << std::endl;
  while (!edm.empty())
  {
    em=edm.back();
    edm.pop_back();
    std::cout << "(" << em[0] << "->" << em[1] << ")" << std::endl;
  }
  std::cout << "[end] edit mapping" << std::endl;


  delete t1;
  delete t2;

  std::cout << "hashtable contains:";
  for ( auto it = hashtable.begin(); it != hashtable.end(); ++it )
    std::cout << " " << it->first << ":" << it->second;
  std::cout << std::endl;

	return 0;
}
