#include <iostream>

#include "node.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

int main () {
	// create test-trees for simplyUpperBound
	node* t1 = new node(1, 5);
  node* middle_child = new node(3);

  t1->add_child(new node(1));
	middle_child->add_child(new node(2));
	t1->add_child(middle_child);
	t1->add_child(new node(4));

  node* t2 = new node(13, 5);
	node* left_child = new node(4);

  left_child->add_child(new node(1));
  left_child->add_child(new node(2));
  left_child->add_child(new node(3));
  t2->add_child(left_child);

  upper_bound* ub = new upper_bound();
  double sup = ub->compute_simple_upper_bound(t1, t2);
  std::cout << "UpperBound t1->t2: " << sup << std::endl;

  int sed_result;
  node* string1 = new node(3, 13);
  node* char2 = new node(15);
  node* char3 = new node(14);
  char3->add_child(new node(4));
  char2->add_child(char3);
  string1->add_child(char2);

  node* string2 = new node(4, 13);
  node* char22 = new node(15);
  node* char33 = new node(15);
  char33->add_child(new node(14));
  char22->add_child(char33);
  string2->add_child(char22);

  //should be 2
  sed_result =
    sed::compute_string_edit_distance<node, costs<node>>(string1, string2);
  std::cout << "String Edit Distance: " << sed_result << std::endl;

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs<node>>(t1, t2)
    << std::endl;

	return 0;
}
