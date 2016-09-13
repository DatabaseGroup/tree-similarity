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
  nodes::Node<_NodeData>** children;
  int full_decomp_size; // The number of subforests in the full decomposition of subtree rooted at this node (Lemma 5.1 in TODS paper)
  int left_decomp_size; // The number of relevant subforests produced by a recursive path decomposition (Lemma 5.3 in TODS paper)
  int right_decomp_size;
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
//          preorder_id               Internally needed parameter, do not overwrite!
//          preorder_id_parent        Internally needed parameter, do not overwrite!
//          full_decomp               Internally needed parameter, do not overwrite!
// Return:  An integer which is the size of the subtree rooted at the given node
// Throws:  A char const* with the error message or an std::exception
template<class _NodeData = nodes::StringNodeData>
int gather_tree_info(nodes::Node<_NodeData>* tree, nodes::Node<_NodeData>** nodes_array_preorder,
    NodeInfo<_NodeData>** node_info_array_preorder, int preorder_id = 0, int preorder_id_parent = -1, int* full_decomp = new int()){ //TODO full_decomp should be deleted anywhere (is on the heap)

  if(!tree || !nodes_array_preorder || !node_info_array_preorder) { // has to be checked, because otherwise there is a segfault below in the code
    throw "passed undefined argument"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }
  int current_preorder_id = preorder_id; // the id of the node in the current invocation is the preorder_id at the moment of the invocation

  nodes_array_preorder[current_preorder_id] = new nodes::Node<_NodeData>; // allocate memory for Node (new returns pointer)
  nodes_array_preorder[current_preorder_id] = tree; // set the array in place current_preorder_id to the pointer of the current node

  node_info_array_preorder[current_preorder_id] = new NodeInfo<_NodeData>; //allocate memory for NodeInfo struct in node_info_array
  node_info_array_preorder[current_preorder_id]->nodeData = tree->get_data();
  node_info_array_preorder[current_preorder_id]->parent_id = preorder_id_parent; //parent id is always the id of the caller (caller passes its id as an argument)

  if(tree->get_children_number() > 0) { // check if it has children
    int full_decomp_children_sum = 0;
    int children_subtree_sizes = 0; // size of subtrees rooted in all children computed incrementally while traversing the children
    for (auto node_it : tree->get_children()) { // iterate over the children
      int child_subtree_size = gather_tree_info(node_it, nodes_array_preorder, node_info_array_preorder, preorder_id + 1, current_preorder_id, full_decomp);
      preorder_id += child_subtree_size;
      children_subtree_sizes += child_subtree_size;
      full_decomp_children_sum += (*full_decomp);
    }
    *full_decomp = full_decomp_children_sum + children_subtree_sizes + 1;
    node_info_array_preorder[current_preorder_id]->subtree_size = children_subtree_sizes + 1;
    node_info_array_preorder[current_preorder_id]->full_decomp_size = (((children_subtree_sizes + 1) * (children_subtree_sizes + 1 + 3)) / 2) - (*full_decomp); // Lemma 5.1 of TODS
    return children_subtree_sizes + 1; // + 1 for current subtree root node
  }

  node_info_array_preorder[current_preorder_id]->subtree_size = 1; // if the node has no children, the subtree size is 1
  node_info_array_preorder[current_preorder_id]->full_decomp_size = 1; // Lemma 5.1 equals one if subtree_size is 1 and (*full_decomp) is 1
  (*full_decomp) = 1;
  return 1;
}

} // END NAMESPACE

#endif // RTED_RTED_H
