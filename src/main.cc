#include <iostream>

#include "node.h"
#include "ted.h"

int main(int argc, char** argv) {
  // generate 3 basic nodes
  node leaf_n1("Leaf node 1"), leaf_n2("Leaf node 2"), leaf_n3("Leaf node 3");
  // generate basic root node
  node parent("Parent node",
    std::vector<node*>({ &leaf_n1, &leaf_n2, &leaf_n3 })
  );

  // generate 3 custom nodes
  custom_node leaf_cn1({"One", "Two"}), leaf_cn2({"Three", "Four", "Five"}),
    leaf_cn3({"Six", "Seven", "Eight", "Nine"});
  // generate custom root node
  custom_node parent_c({"Ten"},
    std::vector<custom_node*>({ &leaf_cn1, &leaf_cn2, &leaf_cn3 })
  );
  // generate custom cost model
  custom_costs<custom_node> cc;

  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model): " << "\t"
    << compute_distance<node, costs<node>>(leaf_n1, leaf_n2) << std::endl;

  // compute distance using custom nodes and custom cost model
  std::cout << "Distance (custom tree, custom cost model): " << "\t" <<
    compute_distance<custom_node, custom_costs<custom_node>>(leaf_cn1,
      leaf_cn2, cc) << std::endl;
  
  return 0;
}
