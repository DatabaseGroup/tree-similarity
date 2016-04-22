#include <iostream>
#include <random>
#include <unordered_map>

#include "node.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

int main (int argc, char* argv[]) {

  if(argc != 3) { return 0; }

  std::unordered_map<std::string, int> hashtable;
  int node_id_counter = 1;

  std::cout << "tree1: " << argv[1] << std::endl;
  std::cout << "tree2: " << argv[2] << std::endl;

  node* t1 = generate_tree_from_string(argv[1], hashtable, node_id_counter);
	std::cout << "size tree 1: " << t1->get_subtree_size() << std::endl;

  node* t2 = generate_tree_from_string(argv[2], hashtable, node_id_counter);
	std::cout << "size tree 2: " << t2->get_subtree_size() << std::endl;

  std::vector<node*>* post_t1 = generate_postorder(t1);
  std::vector<node*>* post_t2 = generate_postorder(t2);

  std::cout << "post_t1" <<  std::endl;
  for(unsigned int i = 0; i < post_t1->size(); i++){
      std::cout << post_t1->at(i)->get_id() << "(lml: " << post_t1->at(i)->get_lml()->get_id() << ")" << ", ";
  }
  std::cout << std::endl;

  std::cout << "post_t2" <<  std::endl;
  for(unsigned int i = 0; i < post_t2->size(); i++){
      std::cout << post_t2->at(i)->get_id() << "(lml: " << post_t2->at(i)->get_lml()->get_id() << ")" << ", ";
  }
  std::cout << std::endl;

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs<node>>(t1, t2)
    << std::endl;

  std::vector<std::array<int, 2> > edm = zs::computeEditMapping<node, costs<node>>(t1,t2);
  std::array<int, 2> em;
  std::cout << "edit mapping:" << std::endl;
  while (!edm.empty())
  {
    em=edm.back();
    edm.pop_back();
    std::cout << "(" << em[0] << "->" << em[1] << ")" << std::endl;
  }

  delete t1;
  delete t2;

  std::cout << "hashtable contains:";
  for ( auto it = hashtable.begin(); it != hashtable.end(); ++it )
    std::cout << " " << it->first << ":" << it->second;
  std::cout << std::endl;

	return 0;
}
