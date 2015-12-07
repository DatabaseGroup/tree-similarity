#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "node.h"

class upper_bound {
public:
	upper_bound () { };
	virtual ~upper_bound () { };
	int calculate_simply_upper_bound (node* t1, node* t2);
};

#endif // UPPER_BOUND_H
