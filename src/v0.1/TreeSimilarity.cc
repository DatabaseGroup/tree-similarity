#include <iostream>
#include "LblTree.h"
#include "Node.h"
#include "SpecializedTree.h"
#include "UpperBound.h"
#include "StringEditDistance.h"
#include "TEDContext.h"

using namespace std;

int main(int argc, char** argv) {
	//LblTree* t = new LblTree(11);
	//t = new SpecializedTree(42);

	//Create test-trees for simplyUpperBound
	LblTree* t1 = new LblTree(1, 5);
	t1->add_child(new Node(1));
	Node* middleChild = new Node(3);
	middleChild->add_child(new Node(2));
	t1->add_child(middleChild);
	t1->add_child(new Node(4));

    LblTree* t2 = new LblTree(13, 5);
	Node* leftChild = new Node(4);
    leftChild->add_child(new Node(1));
  	leftChild->add_child(new Node(2));
  	leftChild->add_child(new Node(3));
  	t2->add_child(leftChild);

    UpperBound* ub = new UpperBound();
    int sup = ub->calculateSimplyUpperBound(t1,t2);
    std::cout << "UpperBound t1->t2: " << sup << std::endl;

    StringEditDistance* sed = new StringEditDistance();

    int sedResult;
    sedResult = sed->calculateSED("mond","moon"); //should be 2
    std::cout << "String Edit Distance: " << sedResult << std::endl;

    TEDContext* context = new TEDContext();
    context->setStrategy(TEDContext::ZS,1,1,1); //Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
    std::cout << "TED = " << context->execute(t1,t2,false);

	return 0;
}
