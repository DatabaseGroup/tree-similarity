#include <iostream>

#include "LblTree.h"
#include "tree.h"
#include "SpecializedTree.h"

using namespace std;

int main(int argc, char** argv) {
	LblTree* t = new LblTree(1);
	std::cout << t->getId() << std::endl;

	t = new SpecializedTree(42);
	std::cout << t->getId() << std::endl;

	Node* root = new Node(0);
  	Node* a = new Node(1);
  	Node* b = new Node(2);
  	root->add_child(a);
  	//root->add_child(new Node(2));
  	root->add_child(b);
  	Node* c = new Node(3);
 	a->add_child(c);

  	std::cout << "Kinder root:" << root->get_children_number() << std::endl;

	return 0;
}
