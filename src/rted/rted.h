#ifndef RTED_RTED_H
#define RTED_RTED_H

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace rted {

template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
double compute_rted(nodes::Node<_NodeData>* tree1, nodes::Node<_NodeData>* tree2,
  _Costs costs = _Costs())
{
  // IMPLEMENT RTED THERE
  return 0.0;
}

// Returns the size of a given tree
//
// Params:  tree   The root node of the tree of type nodes::Node<nodes::StringNodeData>*
// Return:  An integer which is the size of the tree
// Throws:  A char const* with the error message
int get_tree_size(nodes::Node<nodes::StringNodeData>* tree) {
    if(!tree) {
      throw "tree is undefined";
    }

    if(tree->get_children_number() > 0) {
      int count = 1;
      for (int i = 0; i < tree->get_children_number(); ++i) {
        count += get_tree_size(tree->get_child(i));
      }
      return count;
    }
    return 1;
}

} // END NAMESPACE

#endif // RTED_RTED_H
