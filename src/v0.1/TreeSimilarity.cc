#include <iostream>

#include "LblTree.h"
#include "Node.h"
#include "SpecializedTree.h"
#include "UpperBound.h"

using namespace std;

int main(int argc, char** argv) {
	LblTree* t = new LblTree(11);
	std::cout << t->getId() << std::endl;

	t = new SpecializedTree(42);
	std::cout << t->getId() << std::endl;

	//Create test-trees for simplyUpperBound
	LblTree* t1 = new LblTree(1, 0);
	t1->add_child(new Node(1));
	t1->add_child(new Node(2));

    LblTree* t2 = new LblTree(2, 0);
	Node* leftChild = new Node(11);
  	leftChild->add_child(new Node(22));
  	leftChild->add_child(new Node(34));
  	leftChild->add_child(new Node(21));
  	t2->add_child(leftChild);

    UpperBound* ub = new UpperBound();
    int sup = ub->calculateSimplyUpperBound(t1,t2);
    std::cout << "UpperBound t1->t2: " << sup << std::endl;

	return 0;
}
