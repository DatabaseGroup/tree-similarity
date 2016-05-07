#ifndef ZHANG_SHASHA_H
#define ZHANG_SHASHA_H

#include <vector>
#include <algorithm>
#include "node.h"

namespace zs {

std::vector<Node*>* tree1_postorder; //postorder of tree 1
std::vector<Node*>* tree2_postorder; //postorder of tree 2
std::vector<std::vector<double> > td; //stores the tree-edit-distances
std::vector<std::vector<double> > fd; //stores the forest-distances
std::vector<int> l1; //stores the left-most-leaf-descendants of tree 1
std::vector<int> l2; //stores the left-most-leaf-descendants of tree 2
std::vector<Node*> leaves_tree1; //stores the leaves of tree 2
std::vector<Node*> leaves_tree2; //stores the leaves of tree 2

//Computes the left most leaf descendant for all subtrees of the input tree
//
//Param: root of the tree, a vector which stores the left most leaf descendant for each subtree
void lmld (Node* root, std::vector<int>& l) {
  // call lmld recursively and compute the left-most-leaf descendants
  for (int i = 1; i <= root->get_children_number(); ++i) {
    lmld(root->get_children().at(i - 1), l);
  }

  if (root->get_children_number() == 0) {
    // is leaf
    l.at(root->get_id()) = root->get_id();
  } else {
    l.at(root->get_id()) = l.at(root->get_children().at(0)->get_id());
  }
}

//Computes the key-roots of a specific tree. A key-root is either the root or a node with a left sibling.
//
//Param: the left-most leaf descendats for all subtrees of the input-tree, the number of leaves of the input-tree
//
//Return: A vector storing all key-roots of the input-tree
std::vector<int> kr (std::vector<int>& l, int leaf_count) {
  std::vector<int> kr(leaf_count + 1);
  std::vector<int> visit(l.size(), 0); // was node visited

  int k = kr.size() - 1;
  int i = l.size() - 1;
  while (k >= 1) {
    if (visit.at(l.at(i)) == 0) {
      kr.at(k--) = i;
      visit.at(l.at(i)) = 1;
    }
    i -= 1;
  }
  
  return kr;
}

void set_leaves (Node* root, int which_tree) {
  if (which_tree == 1) {
    if (root) {
      if (root->get_children_number() > 0) {
        for (int i = 0; i < root->get_children_number(); ++i) {
          set_leaves(root->get_child(i), 1);
        }
      } else {
        leaves_tree1.push_back(root);
      }
    }
  } else {
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        set_leaves(root->get_child(i), 2);
      }
    } else {
      leaves_tree2.push_back(root);
    }
  }
}

// Computes for each input-tree a vector consisting of all leaves of the specific
// tree.
//
// Parameters: left-hand tree, right-hand tree
void make_leaves (Node* tree1, Node* tree2) {
  set_leaves(tree1, 1);
  set_leaves(tree2, 2);
}

// Computes the forest-distance of:
// T1[l(i)...di], where di is element of desc(T1[i]) and
// T2[l(j)...dj], where dj is element of desc(T2[j])
//
// Param: i, j defined as above and a optional cost-model
template<class _node = Node, class _costs = Costs<_node>>
void forest_dist(int i, int j, _costs costs = _costs()) {
  int cost_rename;

  fd.at(l1.at(i) - 1).at(l2.at(j) - 1) = 0;

  int di = 0, dj = 0;
  for (di = l1.at(i); di <= i; ++di) {
    fd.at(di).at(l2.at(j) - 1) = fd.at(di - 1).at(l2.at(j) - 1) + costs.del();
  }
 
  for (dj = l2.at(j); dj <= j; ++dj) {
    fd.at(l1.at(i) - 1).at(dj) = fd.at(l1.at(i) - 1).at(dj - 1) + costs.ins();
  }

  for (di = l1.at(i); di <= i; ++di) {
    for (dj = l2.at(j); dj <= j; ++dj) {
      if (l1.at(di) == l1.at(i) && l2.at(dj) == l2.at(j)) {
        cost_rename = ((*tree1_postorder).at(di - 1)->get_label_id() == (*tree2_postorder).at(dj - 1)->get_label_id())
          ? 0 : costs.ren();
        fd.at(di).at(dj) = std::min(
          std::min(fd.at(di - 1).at(dj) + costs.del(), fd.at(di).at(dj - 1) + costs.ins()),
          fd.at(di - 1).at(dj - 1) + cost_rename
        );

        td.at(di).at(dj) = fd.at(di).at(dj);
      } else {
        fd.at(di).at(dj) = std::min(
          std::min(fd.at(di - 1).at(dj) + costs.del(), fd.at(di).at(dj - 1) + costs.ins()),
          fd.at(l1.at(di) - 1).at(l2.at(dj) - 1) + td.at(di).at(dj)
        );
      }
    }
  }
}

// Prints the given edit mapping
//
// Params:  edm     the edit mapping
template<class _node = Node>
void print_pretty_edit_mapping (std::vector<std::array<Node*, 2>> edit_mapping)
{
  std::array<Node*, 2> em;
  for ( std::vector<std::array<Node*, 2> >::iterator it = --edit_mapping.end();
        it >= edit_mapping.begin(); --it)
  {
    em = *it;
    std::cout << "(";
    if (em[0] == nullptr) {
      std::cout << "0";
    } else {
      std::cout << em[0]->get_id();
    }
    std::cout << "->";
    if (em[1] == nullptr) {
      std::cout << "0";
    } else {
      std::cout << em[1]->get_id();
    }
    std::cout << ")" << std::endl;
  }
}

// "Returns"/Fills a two dimensional integer array for the given edit mapping
//                 array size: 2 x max(nodes_t1, nodes_t2)+1 or 2 x edm.size();
//
// Params:  edm     the edit mapping
//
// "Returns"/Fills: a two dimensional integer array, where arr[0][id] is the
//          mapping for a the node in the first tree (depends on the mapping !)
template<class _node = Node>
void get_edit_mapping_int_array (std::vector<std::array<Node*, 2> > edit_mapping,
  int** array_to_fill)
{
  std::array<Node*, 2> em;
  for ( std::vector<std::array<Node*, 2> >::iterator it = --edit_mapping.end();
        it >= edit_mapping.begin(); --it)
  {
    em = *it;
    if (em[0] == nullptr) {
      // insert
      array_to_fill[1][em[1]->get_id()] = 0;
    } else if (em[1] == nullptr) {
      // delete
      array_to_fill[0][em[0]->get_id()] = 0;
    } else {
      array_to_fill[0][em[0]->get_id()] = em[1]->get_id();
      array_to_fill[1][em[1]->get_id()] = em[0]->get_id();
    }
  }
}

// Computes the edit mapping for two trees (optional: custom costs)
//
// Params:  r1      the root of the first tree
//          r2      the root of the second tree
//
// Returns: a vector of node* arrays, which shows the mapping for each node,
//          e.g.: node_in_t1  -> node_in_t2   mapping or rename operation
//                nullptr     -> node_in_t2   insert operation
//                node_in_t1  -> nullptr      delete operation
template<class _node = Node, class _costs = Costs<_node>>
std::vector<std::array<Node*, 2> > compute_edit_mapping (Node* tree1,
  Node* tree2, _costs costs = _costs())
{
  tree1_postorder = common::generate_postorder(tree1);
  tree2_postorder = common::generate_postorder(tree2);

  td.resize(tree1_postorder->size() + 1);
  for (unsigned int i = 0; i < td.size(); ++i) {
    td.at(i).resize(tree2_postorder->size() + 1);
  }

  // is there any reason to use reserve instead? (reserve was used before)
  // however, reserve only increases the capacity of the vector, not the size
  // capacity = the space allocated for the vector (e.g. if push_back is used)
  // size = the number of actual elements stores in the vector
  l1.resize(tree1_postorder->size() + 1);
  l2.resize(tree2_postorder->size() + 1);
 
  // initialization (to zero)
  std::fill(l1.begin(), l1.end(), 0);
  std::fill(l2.begin(), l2.end(), 0);

  int max = std::max(tree1_postorder->size(), tree2_postorder->size()) + 1;

  fd.resize(max);
  for (int i = 0; i < max; ++i) {
    fd.at(i).resize(max);
  }

  // TODO: outsource this into a utils.h/common.h
  for ( std::vector<std::vector<double>>::iterator it = td.begin();
        it != td.end(); ++it)
  {
    std::fill(it->begin(), it->end(), 0);
  }

  for ( std::vector<std::vector<double>>::iterator it = fd.begin();
        it != fd.end(); ++it)
  {
    std::fill(it->begin(), it->end(), 0);
  }

  make_leaves(tree1, tree2);

  lmld(tree1, l1);
  lmld(tree2, l2);

  std::vector<int> kr1;
  std::vector<int> kr2;
  kr1 = kr(l1, leaves_tree1.size());
  kr2 = kr(l2, leaves_tree2.size());

  //compute the distance
  for ( std::vector<int>::iterator kr1_it = std::next(kr1.begin());
        kr1_it != kr1.end(); ++kr1_it)
  {
    for ( std::vector<int>::iterator kr2_it = std::next(kr2.begin());
          kr2_it != kr2.end(); ++kr2_it)
    {
      forest_dist(*kr1_it, *kr2_it, costs);
    }
  }
 
  std::vector<std::array<int, 2> > tree_pairs;
  std::vector<std::array<Node*, 2> > edit_mapping;
  tree_pairs.push_back({ tree1->get_subtree_size(), tree2->get_subtree_size() });
  std::array<int, 2> tree_pair;
  bool root_node_pair = true;

  while (!tree_pairs.empty()) {
    tree_pair = tree_pairs.back();
    tree_pairs.pop_back();

    int last_row = tree_pair.at(0);
    int last_col = tree_pair.at(1);

    if (!root_node_pair) {
      forest_dist(last_row, last_col, costs);
    }
    root_node_pair = false;

    int first_row = l1.at(last_row) - 1;
    int first_col = l2.at(last_col) - 1;
    int row = last_row;
    int col = last_col;

    while ((row > first_row) || (col > first_col)) {
      int cost_delete = costs.del();
      int cost_insert = costs.ins();

      if ((row > first_row) && (fd.at(row - 1).at(col) + cost_delete == fd.at(row).at(col)))
      {
        edit_mapping.push_back({ tree1_postorder->at(row-1), nullptr });
        --row;
      } else if ( (col > first_col)
                  && (fd.at(row).at(col - 1) + cost_insert == fd.at(row).at(col)))
      {
        edit_mapping.push_back({ nullptr, tree2_postorder->at(col - 1) });
        --col;
      } else {
        if (l1.at(row) == l1.at(last_row)
            && l2.at(col) == l2.at(last_col))
        {  
          edit_mapping.push_back(
            { tree1_postorder->at(row - 1), tree2_postorder->at(col - 1) }
          );
          --row;
          --col;
        } else {
          tree_pairs.push_back({ row, col });
          row = l1.at(row) - 1;
          col = l2.at(col) - 1;
        }
      }
    }
  }

  delete tree1_postorder;
  delete tree2_postorder;

  return edit_mapping;
}

// Generic function to compute the distance between two trees under a specified
// cost model. Each tree is represented by its respective root node. A root node
// type is specified as first template parameter, the cost model type as second
// template parameter.
template<class _node = Node, class _costs = Costs<_node>>
double compute_zhang_shasha (_node* tree1, _node* tree2, _costs costs = _costs()) {
  tree1_postorder = common::generate_postorder(tree1);
  tree2_postorder = common::generate_postorder(tree2);

  td.resize(tree1_postorder->size() + 1);
  for (unsigned int i = 0; i < td.size(); ++i) {
    td.at(i).resize(tree2_postorder->size() + 1);
  }

  // is there any reason to use reserve instead? (reserve was used before)
  // however, reserve only increases the capacity of the vector, not the size
  // capacity = the space allocated for the vector (e.g. if push_back is used)
  // size = the number of actual elements stores in the vector
  l1.resize(tree1_postorder->size() + 1);
  l2.resize(tree2_postorder->size() + 1);

  // initialization (to zero)
  std::fill(l1.begin(), l1.end(), 0);
  std::fill(l2.begin(), l2.end(), 0);

  int max = std::max(tree1_postorder->size(), tree2_postorder->size()) + 1;

  fd.resize(max);
  for (int i = 0; i < max; ++i) {
    fd.at(i).resize(max);
  }

  // TODO: outsource this into a utils.h/common.h
  for ( std::vector<std::vector<double>>::iterator it = td.begin();
        it != td.end(); ++it)
  {
    std::fill(it->begin(), it->end(), 0);
  }

  for ( std::vector<std::vector<double>>::iterator it = fd.begin();
        it != fd.end(); ++it)
  {
    std::fill(it->begin(), it->end(), 0);
  }

  make_leaves(tree1, tree2);

  lmld(tree1, l1);
  lmld(tree2, l2);

  std::vector<int> kr1;
  std::vector<int> kr2;
  kr1 = kr(l1, leaves_tree1.size());
  kr2 = kr(l2, leaves_tree2.size());

  //compute the distance
  for ( std::vector<int>::iterator kr1_it = std::next(kr1.begin());
        kr1_it != kr1.end(); ++kr1_it)
  {
    for ( std::vector<int>::iterator kr2_it = std::next(kr2.begin());
          kr2_it != kr2.end(); ++kr2_it)
    {
      forest_dist(*kr1_it, *kr2_it, costs);
    }
  }

  double ted = td.at(tree1_postorder->size()).at(tree2_postorder->size());
  delete tree1_postorder;
  delete tree2_postorder;

  std::vector<Node*>().swap(leaves_tree1);
  std::vector<Node*>().swap(leaves_tree2);
  std::vector<int>().swap(l1);
  std::vector<int>().swap(l2);
  std::vector<int>().swap(kr1);
  std::vector<int>().swap(kr2);
  std::vector<std::vector<double> >().swap(td);
  std::vector<std::vector<double> >().swap(fd);

  return ted;
}

}

#endif // ZHANG_SHASHA_H
