#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "lbl_tree.h"

class upper_bound {
public:
	upper_bound () { };
	virtual ~upper_bound () { };
	int calculate_simply_upper_bound (lbl_tree* t1, lbl_tree* t2);
};

#endif // UPPER_BOUND_H
