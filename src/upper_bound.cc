#include "upper_bound.h"

upper_bound::upper_bound () { }

upper_bound::~upper_bound () { }

double upper_bound::compute_simple_upper_bound (node* t1, node* t2) {
	return (t1->get_subtree_size() + t2->get_subtree_size());
}
