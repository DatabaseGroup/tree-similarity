#ifndef TED_H
#define TED_H

#include "node.h"

template<class _node = node, class _costs = costs<_node>>
int compute_distance(_node n1, _node n2, _costs c = _costs()) {
  return (c.ren(n1, n2) + c.del(n1) + c.ins(n2));
}

#endif // TED_H
