#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "node.h"

// Class to compute an upper bound for the tree edit distance.
class upper_bound {
public:
  // Constructor(s)
	upper_bound ();

  // Destructor(s)
	virtual ~upper_bound ();

  // Computes a very simple upper bound by just adding the subtree sizes of two
  // tree rooted at two given root nodes.
	int compute_simple_upper_bound (node* t1, node* t2);
};

#endif // UPPER_BOUND_H
