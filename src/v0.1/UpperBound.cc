#include "UpperBound.h"

int UpperBound::calculateSimplyUpperBound(LblTree* t1, LblTree* t2)
{
	return t1->get_subtree_size() + t2->get_subtree_size();

}


