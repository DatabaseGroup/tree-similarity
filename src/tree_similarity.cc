#include <iostream>
#include "node.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

int main(int argc, char** argv) {
	// Create test-trees for simplyUpperBound
	node* t1 = new node(1, 5);
  node* middleChild = new node(3);
	
  t1->add_child(new node(1));
	middleChild->add_child(new node(2));
	t1->add_child(middleChild);
	t1->add_child(new node(4));

  node* t2 = new node(13, 5);
	node* leftChild = new node(4);
  
  leftChild->add_child(new node(1));
  leftChild->add_child(new node(2));
  leftChild->add_child(new node(3));
  t2->add_child(leftChild);

  upper_bound* ub = new upper_bound();
  int sup = ub->calculate_simply_upper_bound(t1, t2);
  std::cout << "UpperBound t1->t2: " << sup << std::endl;

  int sed_result;
  node* string1 = new node(3,13);
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

  //custom_node* t3 = new custom_node(

  //should be 2
  sed_result = sed::compute_string_edit_distance<node, costs<node>>(string1, string2);
  std::cout << "String Edit Distance: " << sed_result << std::endl;

  //tree_edit_distance* context = new zhang_shasha(1, 1, 1);
  //Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  //std::cout << "TED = " << context->execute(t1, t2);i

  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs<node>>(t1, t2)
    << std::endl;

	return 0;
}
