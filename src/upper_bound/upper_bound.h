#ifndef UPPER_BOUND_H
#define UPPER_BOUND_H

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace upper_bound {

// Class to compute an simple upper bound for the tree edit distance.
class UpperBound {
public:
  // Constructor(s)
  UpperBound();

  // Destructor
  ~UpperBound();

  // Computes a very simple upper bound by just adding the cost of deleting all
  // nodes of the left-hand-tree and the cost of insertion all nodes of the
  // right-hand-tree.
  //
  // Param: Root-node of the left-hand-tree, root-node of the right-hand-tree and
  //        optional cost-model
  //
  // Return: The simple upper bound for the two input-trees
  template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
  double compute_simple_upper_bound(nodes::Node<_NodeData>* tree1,
    nodes::Node<_NodeData>* tree2,_Costs costs = _Costs());
};

UpperBound::UpperBound() { }

UpperBound::~UpperBound() { }

template<typename _NodeData, typename _Costs>
double UpperBound::compute_simple_upper_bound(nodes::Node<_NodeData>* tree1,
  nodes::Node<_NodeData>* tree2, _Costs costs)
{
  return (tree1->get_subtree_size() * costs.del() +
    tree2->get_subtree_size() * costs.ins()
  );
}

} // namespace upper_bound

#endif // UPPER_BOUND_H
