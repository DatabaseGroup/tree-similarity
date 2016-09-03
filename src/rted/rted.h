#ifndef RTED_RTED_H
#define RTED_RTED_H

namespace rted {

template<class _NodeData = nodes::StringNode, class _Costs = nodes::Costs<_NodeData>>
double compute_rted(nodes::Node<_NodeData>* tree1, nodes::Node<_NodeData>* tree2,
  _Costs costs = _Costs())
{
  // IMPLEMENT RTED THERE
  return 0.0;
}

}

#endif // RTED_RTED_H