#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "node.h"

// Class to compute an simple upper bound for the tree edit distance.
class UpperBound {
public:
  // Constructor(s)
    UpperBound () { }

  // Destructor(s)
    ~UpperBound () { }

  // Computes a very simple upper bound by just adding the cost of deleting all nodes of the left-hand-tree
  // and the cost of insertion all nodes of the right-hand-tree.
  //
  // Param: Root-node of the left-hand-tree, root-node of the right-hand-tree and optional cost-model
  //
  //Return: The simple upper bound for the two input-trees
  template<class _node = Node, class _costs = Costs<_node>>
  double compute_simple_upper_bound (_node* t1, _node* t2, _costs c = _costs())
  {
    return (t1->get_subtree_size() * c.del() + t2->get_subtree_size() * c.ins());
  }
};

#endif // UPPER_BOUND_H
