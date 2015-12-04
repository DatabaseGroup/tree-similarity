#ifndef TED_H
#define TED_H

#include "node.h"

/**
 * Generic function to compute the distance between two trees under a specified
 * cost model. Each tree is represented by its respective root node. A root node
 * type is specified as first template parameter, the cost model type as second
 * template parameter.
 */
template<class _node = node, class _costs = costs<_node>>
int compute_distance(_node n1, _node n2, _costs c = _costs()) {
  return (c.ren(n1, n2) + c.del(n1) + c.ins(n2));
}

#endif // TED_H
