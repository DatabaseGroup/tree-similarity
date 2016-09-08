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
template<class _NodeData = nodes::StringNodeData>
int get_tree_size(nodes::Node<_NodeData>* tree) {
  if(!tree) { // has to be checked, because otherwise there is a segfault below in the code
    throw "tree is undefined"; // manually thrown exception because segfault would throw no exception (caller must catch this)
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

template<class _NodeData = nodes::StringNodeData>
int tree_to_array_preorder(nodes::Node<_NodeData>* tree, nodes::Node<_NodeData>** nodes_array, int counter = 0) {
  if(!tree || !nodes_array) { // has to be checked, because otherwise there is a segfault below in the code
    throw "passed undefined argument"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }

  nodes_array[counter] = tree;
  if(tree->get_children_number() > 0) {
    for (int i = 0; i < tree->get_children_number(); ++i) {
      int tmp = tree_to_array_preorder(tree->get_child(i), nodes_array, ++counter);
      counter += tmp;
    }
    return 1;
  }
  return 0;
}

} // END NAMESPACE

#endif // RTED_RTED_H
