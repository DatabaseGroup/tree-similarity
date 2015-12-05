#include "upper_bound.h"

int upper_bound::calculate_simply_upper_bound (lbl_tree* t1, lbl_tree* t2) {
	return (t1->get_subtree_size() + t2->get_subtree_size());
}
