#include <iostream>
#include "lbl_tree.h"
#include "node.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "ted_context.h"

using namespace std;

int main(int argc, char** argv) {
	//Create test-trees for simplyUpperBound
	lbl_tree* t1 = new lbl_tree(1, 5);
  node* middleChild = new node(3);
	
  t1->add_child(new node(1));
	middleChild->add_child(new node(2));
	t1->add_child(middleChild);
	t1->add_child(new node(4));

  lbl_tree* t2 = new lbl_tree(13, 5);
	node* leftChild = new node(4);
  
  leftChild->add_child(new node(1));
  leftChild->add_child(new node(2));
  leftChild->add_child(new node(3));
  t2->add_child(leftChild);

  upper_bound* ub = new upper_bound();
  int sup = ub->calculate_simply_upper_bound(t1, t2);
  std::cout << "UpperBound t1->t2: " << sup << std::endl;

  string_edit_distance* sed = new string_edit_distance(1, 1, 1);

  int sed_result;
  lbl_tree* string1 = new lbl_tree(3,13);
  node* char2 = new node(15);
  node* char3 = new node(14);
  char3->add_child(new node(4));
  char2->add_child(char3);
  string1->add_child(char2);

  lbl_tree* string2 = new lbl_tree(4, 13);
  node* char22 = new node(15);
  node* char33 = new node(15);
  char33->add_child(new node(14));
  char22->add_child(char33);
  string2->add_child(char22);

  sed_result = sed->calculate_distance(string1, string2, false); //should be 2
  std::cout << "String Edit Distance: " << sed_result << std::endl;

  ted_context* context = new ted_context();
  context->set_strategy(ted_context::ZS, 1, 1, 1); //Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  std::cout << "TED = " << context->execute(t1, t2, false);

	return 0;
}
