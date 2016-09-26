#ifndef RTED_RTED_H
#define RTED_RTED_H

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"
#include "../data_structures/array_2d.h"
#include "../data_structures/stack.h"

namespace rted {

template<class _NodeData = nodes::StringNodeData>
struct NodeInfo {
  _NodeData* nodeData;
  int subtree_size;
  int parent_id;
  int l_to_r;
  int r_to_l; // indexed in postorder!
  int number_of_children;
  int* children; // array of the preorder ids of the children
  //nodes::Node<_NodeData>* children; //Maybe change type later
  int full_decomp_size; // The number of subforests in the full decomposition of subtree rooted at this node (Lemma 5.1 in TODS paper)
  int left_decomp_size = 0; // The number of relevant subforests produced by a recursive path decomposition (Lemma 5.3 in TODS paper)
  int right_decomp_size = 0;
};

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

/*
 * Algorithm 3 in TODS paper
 */
template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
double spfG() {

}

/*
 * Algorithm 1 in TODS paper
 */
 template<class _NodeData = nodes::StringNodeData>
data_structures::Array2D<double>* gted(NodeInfo<_NodeData>* ia1, NodeInfo<_NodeData>* ia2, int root_node1, int root_node2, data_structures::Array2D<double>* str) {
  double pathId = (*str)[root_node1][root_node2]; // Line 1: Get the path id of the root-leave-path
  // std::cout << "gted was called for root_node1: " << root_node1 << " and root_node2: " << root_node2 << ";\tpathId: " << pathId << std::endl;
  int parentId = ia1[(int) pathId].parent_id; // Store the preorder id of the parent
  // Decomposition of the tree 1
  int path_node_id = pathId;
  if((int) pathId < ia1[0].subtree_size) { // Line 2: Check if the path is in the left hand tree
    // Line 3-4: Call the function recursively for all trees in the resulting subforest (the subforest that results from removing the root-leave-path)
    int i = pathId;
    while(i >= root_node1) { // Loop over all path nodes from pathId to root_node1 (walk up the path)
      for(int j = 0; j < ia1[i].number_of_children; ++j) { // At every parent node loop over all children and call for everyone that's not the node on the path the function
        if(ia1[i].children[j] != path_node_id) { // Check if the child is not the one on the path
          gted(ia1, ia2, ia1[i].children[j], root_node2, str);
        }
      }
      path_node_id = i;
      i = ia1[path_node_id].parent_id;
    }
  // Decomposition of tree 2
} else { // Line 6: Differs from the algorithm because we don't transpose the matrices. Instead we decompose the right tree aswell (like tree1)
    int i = pathId - ia1[0].subtree_size; // Substract the offset
    while(i >= root_node2) { // Loop over all path nodes from pathId to root_node2 (walk up the path)
      for(int j = 0; j < ia2[i].number_of_children; ++j) { // At every parent node loop over all children and call for everyone that's not the node on the path the function
        if(ia2[i].children[j] != path_node_id) { // Check if the child is not the one on the path
          gted(ia1, ia2, root_node1, ia2[i].children[j], str);
        }
      }
      path_node_id = i;
      i = ia2[path_node_id].parent_id;
    }
  }
  return str;
}

// Computes the Tree Edit Distance with the RTED Algorithm
//
// Params:  tree1     The first tree to be compared
//          tree2     The second tree to be compared
//          costs     The costs for the edit operations
// Return:  A double which is the tree edit distance of the two trees
// Throws:  A char const* with the error message or an std::exception
template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
double compute_rted(nodes::Node<_NodeData>* tree1, nodes::Node<_NodeData>* tree2/*,
  _Costs costs = _Costs()*/) //TODO add costs
{
  int tree1_size = get_tree_size(tree1);
  int tree2_size = get_tree_size(tree2);
  NodeInfo<_NodeData>* tree1_info_array = new NodeInfo<_NodeData>[tree1_size]; // on heap because it's too big for stack and size not known at compile time
  NodeInfo<_NodeData>* tree2_info_array = new NodeInfo<_NodeData>[tree2_size];

  gather_tree_info(tree1, tree1_info_array, tree1_size);
  gather_tree_info(tree2, tree2_info_array, tree2_size);

  data_structures::Array2D<double>* str = compute_strategy_right_to_left_preorder(tree1_info_array, tree2_info_array);

  gted(tree1_info_array, tree2_info_array, 0, 0, str);

  // IMPLEMENT RTED THERE
  return (double) (tree1_size + tree2_size);
}

// Returns the size of a given tree
//
// Params:  tree                        The root node of the tree of type nodes::Node<nodes::StringNodeData>*
//          nodes_array_preorder        The array in which the nodes should be stored in preorder (size must be at least size of tree!)
//          node_info_array_preorder    The array in which the node infos should be stored (size must be at least size of tree!) indexed in preorder
//          total_tree_size             The size of the tree. Must be passed by the caller - should be get from get_tree_size function
//          preorder_id                 Internally needed parameter, do not overwrite!
//          preorder_id_parent          Internally needed parameter, do not overwrite!
//          sum_of_subtree_sizes        Internally needed parameter, do not overwrite!
//          postorder_id                Internally needed parameter, do not overwrite!
//          has_left_or_right_sibling   Internally needed parameter, do not overwrite! -1=left, 0=inner, 1=right, -2=no siblings
// Return:  An integer which is the size of the subtree rooted at the given node
// Throws:  A char const* with the error message or an std::exception
template<class _NodeData = nodes::StringNodeData>
int gather_tree_info(nodes::Node<_NodeData>* tree, NodeInfo<_NodeData>* node_info_array_preorder,
    int total_tree_size, int preorder_id = 0, int preorder_id_parent = -1, int* sum_of_subtree_sizes = new int(),
    int* postorder_id = new int(0), int has_left_or_right_sibling = -2) { //TODO sum_of_subtree_sizes and postorder_id should be deleted anywhere (is on the heap)

  // Check if the arguments are valid
  if(!tree || !node_info_array_preorder || !total_tree_size) { // has to be checked, because otherwise there is a segfault below in the code
    throw "passed undefined argument"; // manually thrown exception because segfault would throw no exception (caller must catch this)
  }
  // variable needed in the algorithm
  int current_preorder_id = preorder_id; // the id of the node in the current invocation is the preorder_id at the moment of the invocation
  // set the data and the parent id of the NodeInfo struct of the current preorder id
  node_info_array_preorder[current_preorder_id].nodeData = tree->get_data();
  node_info_array_preorder[current_preorder_id].parent_id = preorder_id_parent; //parent id is always the id of the caller (caller passes its id as an argument)
  node_info_array_preorder[current_preorder_id].number_of_children = tree->get_children_number();
  node_info_array_preorder[current_preorder_id].children = new int[tree->get_children_number()];

  if(tree->get_children_number() > 0) {
    // code executed if the tree has children (is therefore not a leave)
    int children_subtree_sizes_sum = 0; // stores the sum of all sum_of_subtree_sizes values of the children
    int children_subtree_sizes = 0; // size of subtrees rooted in all children computed incrementally while traversing the children
    int children_counter = 0;
    // iterate over the children
    for (auto node_it : tree->get_children()) {
      // evaluating, whether the child called next has left or right siblings, to tell it the child via a parameter passed to the function
      int left_or_right_sibling = -2; // initially every child is assumed to have no siblings
      if(tree->get_children_number() > 2 && (node_it != tree->get_children().front() && node_it != tree->get_children().back())) {
        left_or_right_sibling = 0; // 0 stands for inner node (has both left and right siblings)
      } else if(tree->get_children_number() > 1 && node_it != tree->get_children().front()) { // check if the child for which the recursion is called next has left siblings
        left_or_right_sibling = -1; // -1 stands for has left siblings
      } else if(tree->get_children_number() > 1 && node_it != tree->get_children().back()) {
        left_or_right_sibling = 1; // 1 stands for has right siblings
      }

      // call the function recursively
      int child_subtree_size = gather_tree_info(node_it, node_info_array_preorder, total_tree_size, preorder_id + 1,
        current_preorder_id, sum_of_subtree_sizes, postorder_id, left_or_right_sibling);

      // after coming back from the recursion, the following code is executed:

      node_info_array_preorder[current_preorder_id].children[children_counter] = preorder_id + 1;
      preorder_id += child_subtree_size; // the preorder id must be increased by the number of nodes the last child contained, so when the next child is called, it is called with the right preorder id
      children_subtree_sizes += child_subtree_size; // the subtree size of the last child is summed up to the size of all children of the current node
      children_subtree_sizes_sum += (*sum_of_subtree_sizes); // the last subtree size sum is added to the sum of the last subtree size sum of the other child nodes
      ++children_counter;
    }

    // after iterating over all children, the following code is executed:

    // updating parents decomposition costs
    if(preorder_id_parent > -1) {
      if(has_left_or_right_sibling == 0) { // if I have both left and right siblings
        node_info_array_preorder[preorder_id_parent].left_decomp_size += node_info_array_preorder[current_preorder_id].left_decomp_size + children_subtree_sizes + 1;
        node_info_array_preorder[preorder_id_parent].right_decomp_size += node_info_array_preorder[current_preorder_id].right_decomp_size + children_subtree_sizes + 1;
      } else {
        if(has_left_or_right_sibling == -1) { // if I have left siblings - I update my parent with my current left decomposition value + my own subtree size
          node_info_array_preorder[preorder_id_parent].left_decomp_size += node_info_array_preorder[current_preorder_id].left_decomp_size + children_subtree_sizes + 1;
          node_info_array_preorder[preorder_id_parent].right_decomp_size += node_info_array_preorder[current_preorder_id].right_decomp_size;
        } else if(has_left_or_right_sibling == 1) { // if I have right siblings - I update my parent with my current right decomposition value + my own subtree size
          node_info_array_preorder[preorder_id_parent].right_decomp_size += node_info_array_preorder[current_preorder_id].right_decomp_size + children_subtree_sizes + 1;
          node_info_array_preorder[preorder_id_parent].left_decomp_size += node_info_array_preorder[current_preorder_id].left_decomp_size;
        } else { // if I have no siblings - I update my parent with my values only (left and right)
          node_info_array_preorder[preorder_id_parent].left_decomp_size += node_info_array_preorder[current_preorder_id].left_decomp_size;
          node_info_array_preorder[preorder_id_parent].right_decomp_size += node_info_array_preorder[current_preorder_id].right_decomp_size;
        }
      }
    }

    (*sum_of_subtree_sizes) = children_subtree_sizes_sum + children_subtree_sizes + 1; // setting the sum of the subtree sizes to the sum of the subtree sizes of all children plus my own subtree size (needed for full decomposition size)
    node_info_array_preorder[current_preorder_id].subtree_size = children_subtree_sizes + 1; // setting subtree size to the size of all subtrees plus the root node
    node_info_array_preorder[current_preorder_id].left_decomp_size += children_subtree_sizes + 1; // setting left decomposition cost to the left decomposition sum of the last child plus own subtree size
    node_info_array_preorder[current_preorder_id].right_decomp_size += children_subtree_sizes + 1; // updating right decomposition cost - own subtree size added
    node_info_array_preorder[current_preorder_id].full_decomp_size = (((children_subtree_sizes + 1) * (children_subtree_sizes + 1 + 3)) / 2) - (*sum_of_subtree_sizes); // setting full decomposition cost - Lemma 5.1 of TODS
    node_info_array_preorder[current_preorder_id].l_to_r = total_tree_size - 1 - (*postorder_id);
    node_info_array_preorder[total_tree_size - 1 - (*postorder_id)].r_to_l = current_preorder_id;


    ++(*postorder_id);
    return (children_subtree_sizes + 1); // returning the current subtree size which is the size of all subtrees plus the root node // + 1 for current subtree root node
  }

  // this code is executed if the current node is a leave

  // updating parents decomposition costs
  if(preorder_id_parent > -1) {
    if(has_left_or_right_sibling == -1) { // check if it has left children and if so, add the left decomposition size of 1 to the left decomposition size of the parent
      node_info_array_preorder[preorder_id_parent].left_decomp_size += 1; // 1 for subtree size
    } else if(has_left_or_right_sibling == 1) {
      node_info_array_preorder[preorder_id_parent].right_decomp_size += 1;
    } else if(has_left_or_right_sibling == 0) {
      node_info_array_preorder[preorder_id_parent].left_decomp_size += 1;
      node_info_array_preorder[preorder_id_parent].right_decomp_size += 1;
    }
  }

  (*sum_of_subtree_sizes) = 1; // the sum of subtree sizes is reset to 1 if it's a leave
  node_info_array_preorder[current_preorder_id].subtree_size = 1; // if the node has no children, the subtree size is 1
  node_info_array_preorder[current_preorder_id].left_decomp_size = 1;
  node_info_array_preorder[current_preorder_id].right_decomp_size = 1;
  node_info_array_preorder[current_preorder_id].full_decomp_size = 1; // Lemma 5.1 equals one if subtree_size is 1 and (*sum_of_subtree_sizes) is 1
  node_info_array_preorder[current_preorder_id].l_to_r = total_tree_size - 1 - (*postorder_id);
  node_info_array_preorder[total_tree_size - 1 - (*postorder_id)].r_to_l = current_preorder_id;

  ++(*postorder_id);
  return 1; // 1 is returned as the subtree size
}


/*
 * A path is identified by its leaf node's left-to-right-preorder id. To distinguish
 * between paths in the left hand and right hand input tree, we add an offset of
 * the size of the left hand tree to the ids of right hand tree paths.
 *
 *
 */
template<class _NodeData = nodes::StringNodeData>
data_structures::Array2D<double>* compute_strategy_right_to_left_preorder(
  NodeInfo<nodes::StringNodeData>* tree_info_array_1, NodeInfo<nodes::StringNodeData>* tree_info_array_2){

  // TODO precompute values for loops (especially inner one)

  int tree1_size = tree_info_array_1->subtree_size; // store tree sizes
  int tree2_size = tree_info_array_2->subtree_size;
  int lv, lw; // left-to-right-preorder of node v and w (we iterate over v and w in right-to-left-preorder)
  long min_cost, tmp_cost;
  long min_path = -1;
  data_structures::Stack<long*> reusable_rows_l; // calls default constructor even without parentheses
  data_structures::Stack<long*> reusable_rows_r;
  data_structures::Stack<long*> reusable_rows_i;

  // Algorithm 1 in APTED
  // Lines 1-4 Prepare arrays (initiate leafrow with 0s)
  data_structures::Array2D<double>* str = new data_structures::Array2D<double>(tree1_size, tree2_size); // allocate memory for the strategy matrix
  long** l1 = new long*[tree1_size]{NULL}; // allocate memory for the cost matrices
  long** r1 = new long*[tree1_size]{NULL}; // assign to NULL to be able to check if a row is already allocated
  long** i1 = new long*[tree1_size]{NULL};
  long* l2 = new long[tree2_size];
  long* r2 = new long[tree2_size];
  long* i2 = new long[tree2_size];
  long* leaf_row = new long[tree2_size]{};
  int* path2 = new int[tree2_size];

  // Line 5: outer loop, traverses over left hand tree in right to left preorder
  for(int v = tree1_size -1; v >= 0; --v) { // Line 5: outer loop
    lv = tree_info_array_1[v].r_to_l;
    // Line 6 and 7: dynamic (de)allocation
    if(tree_info_array_1[lv].subtree_size == 1) { // Line 6: if v is a leave, assign leave_row-pointer to cost-row-pointers
      //std::cout << "v[" << lv << "] is a leave. cost rows assigned to leaverow." << std::endl;
      l1[lv] = r1[lv] = i1[lv] = leaf_row; // no allocation needed, beacuse we can use the allocated space of leave_row - fewer space needed
      for(int i = 0; i < tree2_size; ++i) {
        (*str)[lv][i] = lv; // Path ids
      }
    }
    if(tree_info_array_1[lv].parent_id != -1 && l1[tree_info_array_1[lv].parent_id] == NULL) { // Line 7: if row for parent of v is not allocated, allocate cost rows for parent
      //std::cout << "parent row of v[" << lv << "] is not allocated. ";
      if(reusable_rows_l.isEmpty()) {
        //std::cout << "is being allocated now beacuse stack is empty." << std::endl;
        l1[tree_info_array_1[lv].parent_id] = new long[tree2_size]{0};
        r1[tree_info_array_1[lv].parent_id] = new long[tree2_size]{0};
        i1[tree_info_array_1[lv].parent_id] = new long[tree2_size]{0};
      } else {
        //std::cout << "uses row from stack." << std::endl;
        l1[tree_info_array_1[lv].parent_id] = reusable_rows_l.pop();
        r1[tree_info_array_1[lv].parent_id] = reusable_rows_r.pop();
        i1[tree_info_array_1[lv].parent_id] = reusable_rows_i.pop();
        for(int i = 0; i < tree2_size; ++i) {
          l1[tree_info_array_1[lv].parent_id][i] = 0;
          r1[tree_info_array_1[lv].parent_id][i] = 0;
          i1[tree_info_array_1[lv].parent_id][i] = 0;
        }
      }
    }
    for(int i = 0; i < tree2_size; ++i) {
      l2[i] = 0;
      r2[i] = 0;
      i2[i] = 0;
    }
    // Line 8: inner loop, traverses over right hand tree in right to left preorder
    for(int w = tree2_size -1; w >= 0; --w) { // Line 8: inner loop
      lw = tree_info_array_2[w].r_to_l;
      if(tree_info_array_2[lw].subtree_size == 1) { // Line 9: if w is leaf node then cost matrices rows of w equal 0
        l2[lw] = r2[lw] = i2[lw] = 0;
        path2[lw] = lw;
      }
      // Line 10-16: get cost-path pairs (6: left, inner, right for both F and G) and determine minimum cost and path
      std::string root_min_path = "l1";
      min_cost = (long) tree_info_array_1[lv].subtree_size * (long) tree_info_array_2[lw].left_decomp_size + l1[lv][lw];
      min_path = tree_info_array_1[v + tree_info_array_1[lv].subtree_size - 1].r_to_l; // left_path_tree1
      tmp_cost = (long) tree_info_array_1[lv].subtree_size * (long) tree_info_array_2[lw].right_decomp_size + r1[lv][lw];
      if(tmp_cost < min_cost) {
        root_min_path = "r1";
        min_cost = tmp_cost;
        min_path = lv + tree_info_array_1[lv].subtree_size - 1; // right_path_tree1
      }
      tmp_cost = (long) tree_info_array_1[lv].subtree_size * (long) tree_info_array_2[lw].full_decomp_size + i1[lv][lw];
      // if(lv == 284 && lw == 0) {
      //   std::cout << "subtree size: " << tree_info_array_1[lv].subtree_size << ", " << tree_info_array_2[lw].full_decomp_size << ", " << i1[lv][lw] << std::endl;
      // }
      if(tmp_cost < min_cost) {
        root_min_path = "i1";
        min_cost = tmp_cost;
        min_path = (*str)[lv][lw]; // inner_path_tree1
      }
      tmp_cost = (long) tree_info_array_2[lw].subtree_size * (long) tree_info_array_1[lv].left_decomp_size + l2[lw];
      if(tmp_cost < min_cost) {
        root_min_path = "l2";
        min_cost = tmp_cost;
        min_path = tree_info_array_2[w + tree_info_array_2[lw].subtree_size - 1].r_to_l + tree1_size; // left_path_tree2 + offset
      }
      tmp_cost = (long) tree_info_array_2[lw].subtree_size * (long) tree_info_array_1[lv].right_decomp_size + r2[lw];
      if(tmp_cost < min_cost) {
        root_min_path = "r2";
        min_cost = tmp_cost;
        min_path = lw + tree_info_array_2[lw].subtree_size - 1 + tree1_size; // right_path_tree2 + offest
      }
      tmp_cost = (long) tree_info_array_2[lw].subtree_size * (long) tree_info_array_1[lv].full_decomp_size + i2[lw];
      if(tmp_cost < min_cost) {
        root_min_path = "i2";
        min_cost = tmp_cost;
        min_path = path2[lw] + tree1_size; // inner_path_tree2 + offest
      }
      //if(min_cost < 0) {
      //  std::cout << "min_cost and min_path for node pair " << lv << "," << lw <<": " << (long) min_cost << ", " << min_path << std::endl;
      //}
      // Line 17-36: update parents
      if(tree_info_array_1[lv].parent_id != -1) { // Line 17: if v is not root then
        r1[tree_info_array_1[lv].parent_id][lw] += min_cost; // Line 18
        tmp_cost = -min_cost + i1[lv][lw]; // Line 19
        // std::cout << "tmp_cost after Line 19: " << tmp_cost << std::endl;
        // std::cout << "compared to the following value in Line 20: " << i1[tree_info_array_1[lv].parent_id][lw] << std::endl;
        if(tmp_cost < i1[tree_info_array_1[lv].parent_id][lw]) { // Line 20
          // std::cout << "i1[" << tree_info_array_1[lv].parent_id << "][" << lw << "] is being set to: " << tmp_cost << " by " << "[" << lv << "][" << lw << "] (Line 21)" << std::endl;
          i1[tree_info_array_1[lv].parent_id][lw] = tmp_cost; // Line 21
          (*str)[tree_info_array_1[lv].parent_id][lw] = (*str)[lv][lw]; // Line 22
        }
        if((v - tree_info_array_1[tree_info_array_1[lv].parent_id].l_to_r) == 1) { // Line 23
          // std::cout << "i1[" << tree_info_array_1[lv].parent_id << "][" << lw << "] is being increased by: " << r1[tree_info_array_1[lv].parent_id][lw] << std::flush;
          i1[tree_info_array_1[lv].parent_id][lw] += r1[tree_info_array_1[lv].parent_id][lw]; // Line 24
          // std::cout << " and is now: " << i1[tree_info_array_1[lv].parent_id][lw] << " (Line 24)" << std::endl << std::flush;
          r1[tree_info_array_1[lv].parent_id][lw] += (r1[lv][lw] - min_cost); // Line 25
          if(i1[tree_info_array_1[lv].parent_id][lw] < 0 || r1[tree_info_array_1[lv].parent_id][lw] < 0) {
            throw "r1 or i1 is negative!";
          }
        }
        l1[tree_info_array_1[lv].parent_id][lw] += (((lv - tree_info_array_1[lv].parent_id) == 1) ? l1[lv][lw] : min_cost); // Line 26
      }
      if(tree_info_array_2[lw].parent_id != -1) { // Line 27: if w is not root then
        r2[tree_info_array_2[lw].parent_id] += min_cost; // Line 28
        tmp_cost = -min_cost + i2[lw]; // Line 29
        if(tmp_cost < i2[tree_info_array_2[lw].parent_id]) { // Line 30
          i2[tree_info_array_2[lw].parent_id] = tmp_cost; // Line 31
          path2[tree_info_array_2[lw].parent_id] = path2[lw]; // Line 32
        }
        if((w - tree_info_array_2[tree_info_array_2[lw].parent_id].l_to_r) == 1) { // Line 33
          i2[tree_info_array_2[lw].parent_id] += r2[tree_info_array_2[lw].parent_id]; // Line 34
          r2[tree_info_array_2[lw].parent_id] += (r2[lw] - min_cost); // Line 35
          if(i2[tree_info_array_2[lw].parent_id] < 0 || i2[tree_info_array_2[lw].parent_id] < 0) {
            throw "r2 or i2 is negative!";
          }
        }
        l2[tree_info_array_2[lw].parent_id] += (((lw - tree_info_array_2[lw].parent_id) == 1) ? l2[lw] : min_cost); // Line 36
      }
      (*str)[lv][lw] = min_path; // Line 37: write path that maps to the minimun cost into the strategy matrix at [v,w]
      if(v == 0 && w == 0) {
        std::cout << "min_cost: " << min_cost << std::endl;
      }
    }
    // Line 38: dynamic deallocation
    if(tree_info_array_1[lv].subtree_size != 1) { // Line 38: if v is not a leaf node then deallocate the row of v in left, right and inner cost matrices
      //std::cout << "v[" << lv << "] is not a leave. its rows are being deallocated now." << std::endl;
      reusable_rows_l.push(l1[lv]);
      reusable_rows_r.push(r1[lv]);
      reusable_rows_i.push(i1[lv]);
    }
  }

  return str; // Line 39
}

} // END NAMESPACE

#endif // RTED_RTED_H
