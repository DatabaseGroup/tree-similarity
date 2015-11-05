#include <iostream>

#include "LblTree.h"
#include "SpecializedTree.h"

int main(int argc, char** argv) {
	LblTree* t = new LblTree(1);
	std::cout << t->getId() << std::endl;

	t = new SpecializedTree(42);
	std::cout << t->getId() << std::endl;

	return 0;
}