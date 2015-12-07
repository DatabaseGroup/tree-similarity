#include "upper_bound.h"

int upper_bound::calculate_simply_upper_bound (node* t1, node* t2) {
	return (t1->get_subtree_size() + t2->get_subtree_size());
}
