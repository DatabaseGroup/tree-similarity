#ifndef RTED_RTED_H
#define RTED_RTED_H

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace rted {

template<class _NodeData = nodes::StringNodeData>
struct NodeInfo {
  _NodeData* nodeData;
  int subtree_size;
  int parent_id;
  nodes::Node<_NodeData>** children; //FIXME: when memory for the struct_info array gets allocated, it is not known how many children there are i.e it is not known, how much memory this field needs
};

template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
double compute_rted(nodes::Node<_NodeData>* tree1, nodes::Node<_NodeData>* tree2,
  _Costs costs = _Costs())
{
  // IMPLEMENT RTED THERE
  return 0.0;
}

// Returns the size of a given tree
//
// Params:  tree   The root node of the tree
// Return:  An integer which is the size of the tree
// Throws:  A char const* with the error message
template<class _NodeData = nodes::StringNodeData>
int get_tree_size(nodes::Node<_NodeData>* tree) {
  if(!tree) { // has to be checked, because otherwise there is a segfault below in the code
    throw "tree is undefined"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }

  if(tree->get_children_number() > 0) {
    int count = 1;
    for (auto node_it : tree->get_children()) {
      count += get_tree_size(node_it);
    }
    return count;
  }
  return 1;
}

// Returns the size of a given tree
//
// Params:  tree                      The root node of the tree of type nodes::Node<nodes::StringNodeData>*
//          nodes_array_preorder      The array in which the nodes should be stored in preorder (size must be at least size of tree!)
//          node_info_array_preorder  The array in which the node infos should be stored (size must be at least size of tree!)
//          total_tree_size           Internally needed parameter, do not overwrite!
//          preorder_id_parent        Internally needed parameter, do not overwrite!
// Return:  An integer which is the size of the tree
// Throws:  A char const* with the error message
template<class _NodeData = nodes::StringNodeData>
int gather_tree_info(nodes::Node<_NodeData>* tree, nodes::Node<_NodeData>** nodes_array_preorder,
    NodeInfo<_NodeData>** node_info_array_preorder, int total_tree_size = 0, int preorder_id_parent = -1){

  if(!tree || !nodes_array_preorder) { // has to be checked, because otherwise there is a segfault below in the code
    throw "passed undefined argument"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }

  int current_id = total_tree_size; // the id of tree in the current invocation is the total_tree_size at the moment of the invocation
  nodes_array_preorder[current_id] = tree; // set the array in place current_id to the pointer of the current node
  node_info_array_preorder[current_id]->parent_id = preorder_id_parent;

  if(tree->get_children_number() > 0) { // check if it has children
    int current_children_counter = 0;
    for (auto node_it : tree->get_children()) { // iterate over the children
      int tmp = gather_tree_info(node_it, nodes_array_preorder, node_info_array_preorder, ++total_tree_size, current_id);
      total_tree_size += tmp; //needs to be stored in tmp because total_tree_size is also used as param - would create warning [-Wunsequenced]
      current_children_counter++;
    }
    node_info_array_preorder[current_id]->subtree_size = total_tree_size - current_id;
    return 1;
  }

  node_info_array_preorder[current_id]->subtree_size = 0;
  return 0;
}

} // END NAMESPACE

#endif // RTED_RTED_H
