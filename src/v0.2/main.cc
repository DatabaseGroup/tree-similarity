#include <iostream>

#include "node.h"
#include "ted.h"

int main(int argc, char** argv) {
  node leaf_n1("Leaf node 1"), leaf_n2("Leaf node 2"), leaf_n3("Leaf node 3"),
    parent("Parent node", std::vector<node*>({ &leaf_n1, &leaf_n2, &leaf_n3 }));

  custom_node leaf_cn1({"One", "Two"}), leaf_cn2({"Three", "Four", "Five"}),
    leaf_cn3({"Six", "Seven", "Eight", "Nine"}),
    parent_c({"Ten"}, std::vector<custom_node*>({ &leaf_cn1, &leaf_cn2, &leaf_cn3 }));
  custom_costs<custom_node> cc;

  // standard nodes and standard cost model
  std::cout << "Distance (standard nodes, standard cost model): " << "\t"
    << compute_distance<node, costs<node>>(leaf_n1, leaf_n2) << std::endl;

  // custom nodes and custom cost model
  std::cout << "Distance (custom nodes, custom cost model): " << "\t\t"
    << compute_distance<custom_node, custom_costs<custom_node>>(leaf_cn1, leaf_cn2, cc)
    << std::endl;
  
  return 0;
}
