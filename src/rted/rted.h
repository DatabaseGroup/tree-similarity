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
  int right_decomp_size = 0;
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
//          sum_of_subtree_sizes      Internally needed parameter, do not overwrite!
//          left_decomp_sum           Internally needed parameter, do not overwrite!
//          is_rightmost_child        Internally needed parameter, do not overwrite!
// Return:  An integer which is the size of the subtree rooted at the given node
// Throws:  A char const* with the error message or an std::exception
template<class _NodeData = nodes::StringNodeData>
int gather_tree_info(nodes::Node<_NodeData>* tree, nodes::Node<_NodeData>** nodes_array_preorder,
    NodeInfo<_NodeData>** node_info_array_preorder, int preorder_id = 0, int preorder_id_parent = -1,
    int* sum_of_subtree_sizes = new int(), int* left_decomp_sum = new int(0), bool is_rightmost_child = true){ //TODO *_decomp should be deleted anywhere (is on the heap)

  // Check if the arguments are valid
  if(!tree || !nodes_array_preorder || !node_info_array_preorder) { // has to be checked, because otherwise there is a segfault below in the code
    throw "passed undefined argument"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }
  // variable needed in the algorithm
  int current_preorder_id = preorder_id; // the id of the node in the current invocation is the preorder_id at the moment of the invocation
  // allocate memory for the Node class of the currrent node and set it
  nodes_array_preorder[current_preorder_id] = new nodes::Node<_NodeData>; // allocate memory for Node (new returns pointer)
  nodes_array_preorder[current_preorder_id] = tree; // set the array in place current_preorder_id to the pointer of the current node
  // allocate memory for the NodeInfo struct of the current node and set the data and the parent id
  node_info_array_preorder[current_preorder_id] = new NodeInfo<_NodeData>; //allocate memory for NodeInfo struct in node_info_array
  node_info_array_preorder[current_preorder_id]->nodeData = tree->get_data();
  node_info_array_preorder[current_preorder_id]->parent_id = preorder_id_parent; //parent id is always the id of the caller (caller passes its id as an argument)

  if(tree->get_children_number() > 0) {
    // code executed if the tree has children (is therefore not a leave)
    int children_subtree_sizes_sum = 0; // stores the sum of all sum_of_subtree_sizes values of the children
    int children_subtree_sizes = 0; // size of subtrees rooted in all children computed incrementally while traversing the children

    // iterate over the children
    for (auto node_it : tree->get_children()) {
      // evaluating, whether the child called next is a rightmost child or not, to tell it the child via a parameter passed to the function
      bool is_rightmost_child = true; // initially every child is assumed to be a rightmost child
      if(tree->get_children_number() > 1 && node_it != tree->get_children().back()) { // check if the child for which the recursion is called next is not rightmost child
        is_rightmost_child = false;
      }

      // call the function recusrively
      int child_subtree_size = gather_tree_info(node_it, nodes_array_preorder, node_info_array_preorder,
        preorder_id + 1, current_preorder_id, sum_of_subtree_sizes, left_decomp_sum, is_rightmost_child);

      // after coming back from the recursion, the following code is executed
      // check if the child for which the recursion was called last is a leftmost child and if it is, increase the left decomposition sum by the subtree size of the called child
      if(tree->get_children_number() > 1 && node_it != tree->get_children().front()) {
        (*left_decomp_sum) += child_subtree_size;
      }
      preorder_id += child_subtree_size; // the preorder id must be increased by the number of nodes the last child contained, so when the next child is called, it is called with the right preorder id
      children_subtree_sizes += child_subtree_size; // the subtree size of the last child is summed up to the size of all children of the current node
      children_subtree_sizes_sum += (*sum_of_subtree_sizes); // the last subtree size sum is added to the sum of the last subtree size sum of the other child nodes
    }

    // after iterating over all children, the following code is executed:
    // setting the sum of the subtree sizes to the sum of the subtree sizes of all children plus my own subtree size (needed for full decomposition size)
    *sum_of_subtree_sizes = children_subtree_sizes_sum + children_subtree_sizes + 1;
    // setting subtree size to the size of all subtrees plus the root node
    node_info_array_preorder[current_preorder_id]->subtree_size = children_subtree_sizes + 1;

    // updating parents right decomposition cost
    // passed to parent differently to left decomosition cost due to preorder traversal of the tree
    if(is_rightmost_child == false) { // if I'm not a rightmost child - I update my parent with my current right decomposition value + my own subtree size
      node_info_array_preorder[preorder_id_parent]->right_decomp_size += node_info_array_preorder[current_preorder_id]->right_decomp_size + children_subtree_sizes + 1;
    } else if(preorder_id_parent > -1) { // even if I am a rightmost child, I update my parent with the other right decomposition sizes from my descendants but not with my own subtree size
      node_info_array_preorder[preorder_id_parent]->right_decomp_size += node_info_array_preorder[current_preorder_id]->right_decomp_size;
    }

    // setting left decomposition cost to the left decomposition sum of the last child plus own subtree size
    node_info_array_preorder[current_preorder_id]->left_decomp_size = (*left_decomp_sum) + children_subtree_sizes + 1;
    // updating right decomposition cost - own subtree size added
    node_info_array_preorder[current_preorder_id]->right_decomp_size += children_subtree_sizes + 1;
    // setting full decomposition cost
    node_info_array_preorder[current_preorder_id]->full_decomp_size = (((children_subtree_sizes + 1) * (children_subtree_sizes + 1 + 3)) / 2) - (*sum_of_subtree_sizes); // Lemma 5.1 of TODS

    // returning the current subtree size which is the size of all subtrees plus the root node
    return (children_subtree_sizes + 1); // + 1 for current subtree root node
  }

  // this code is executed if the current node is a leave
  // set the right decomposition cost to 1
  node_info_array_preorder[current_preorder_id]->right_decomp_size = 1;
  // check if it's not a rightmost child and if so, add the right decomposition size of 1 to the right decomposition size of the parent
  if(is_rightmost_child == false) {
    node_info_array_preorder[preorder_id_parent]->right_decomp_size += node_info_array_preorder[current_preorder_id]->right_decomp_size;
  }
  // set subtree size, full and left decomposition size and the sum of the subtree sizes to 1
  node_info_array_preorder[current_preorder_id]->subtree_size = 1; // if the node has no children, the subtree size is 1
  node_info_array_preorder[current_preorder_id]->full_decomp_size = 1; // Lemma 5.1 equals one if subtree_size is 1 and (*sum_of_subtree_sizes) is 1
  node_info_array_preorder[current_preorder_id]->left_decomp_size = 1;
  (*sum_of_subtree_sizes) = 1; // the sum of subtree sizes is reset to 1 if it's a leave
  return 1; // 1 is returned as the subtree size
}

} // END NAMESPACE

#endif // RTED_RTED_H
