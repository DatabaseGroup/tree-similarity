#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "node.h"

// Class to compute an upper bound for the tree edit distance.
class upper_bound {
public:
  // Constructor(s)
    upper_bound () { }

  // Destructor(s)
    ~upper_bound () { }

  // Computes a very simple upper bound by just adding the subtree sizes of two
  // tree rooted at two given root nodes.
	template<class _node = node, class _costs = costs<_node>>
double compute_simple_upper_bound (_node* t1, _node* t2, _costs c = _costs()) {
    return (t1->get_subtree_size() * c.del() + t2->get_subtree_size() * c.ins());
}
};

#endif // UPPER_BOUND_H
