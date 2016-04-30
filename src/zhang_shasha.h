#ifndef ZHANG_SHASHA_H
#define ZHANG_SHASHA_H

#include <vector>
#include <algorithm>
#include "node.h"

namespace zs {

std::vector<node*>* tr_post1; //postorder of tree 1
std::vector<node*>* tr_post2; //postorder of tree 2
std::vector<std::vector<double> > td; //stores the tree-edit-distances
std::vector<std::vector<double> > fd; //stores the forest-distances
std::vector<int> lm1; //stores the left-most-leaf-descendants of tree 1
std::vector<int> lm2; //stores the left-most-leaf-descendants of tree 2
std::vector<node*> leaves_t1; //stores the leaves of tree 2
std::vector<node*> leaves_t2; //stores the leaves of tree 2

//Computes the left most leaf descendant for all subtrees of the input tree
//
//Param: root of the tree, a vector which stores the left most leaf descendant for each subtree
void lmld (node* root, std::vector<int>& lm) {
  // call lmld recursively and compute the left-most-leaf descendants
  for (int i = 1; i <= root->get_children_number(); ++i) {
    lmld(root->get_children().at(i - 1), lm);
  }

  if (root->get_children_number() == 0) {
    // is leaf
    lm.at(root->get_id()) = root->get_id();
  } else {
    lm.at(root->get_id()) = lm.at(root->get_children().at(0)->get_id());
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

void set_leaves (node* root, int whichTree) {
  if (whichTree == 1) {
    if (root) {
      if (root->get_children_number() > 0) {
        for (int i = 0; i < root->get_children_number(); ++i) {
          set_leaves(root->get_child(i), 1);
        }
      } else {
        leaves_t1.push_back(root);
      }
    }
  } else {
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        set_leaves(root->get_child(i), 2);
      }
    } else {
      leaves_t2.push_back(root);
    }
  }
}

//Computes for each input-tree a vector consisting of all leaves of the specific tree.
//
//Parameters: left-hand tree, right-hand tree
void make_leaves (node* t1, node* t2) {
  set_leaves(t1, 1);
  set_leaves(t2, 2);
}

//computes the forest-distance of:
// T1[l(i)...di], where di is element of desc(T1[i]) and
// T2[l(j)...dj], where dj is element of desc(T2[j])
//
//Param: i, j defined as above and a optional cost-model
template<class _node = node, class _costs = costs<_node>>
void forest_dist(int i, int j, _costs c = _costs()) {
  int tempmin;
  int cost_rename;

  fd.at(lm1.at(i) - 1).at(lm2.at(j) - 1) = 0;

  for (int di = lm1.at(i); di <= i; ++di) {
    fd.at(di).at(lm2.at(j) - 1) = fd.at(di - 1).at(lm2.at(j) - 1) + c.del();
  }
 
  for (int dj = lm2.at(j); dj <= j; ++dj) {
    fd.at(lm1.at(i) - 1).at(dj) = fd.at(lm1.at(i) - 1).at(dj - 1) + c.ins();
  }

  for (int di = lm1.at(i); di <= i; ++di) {
    for (int dj = lm2.at(j); dj <= j; ++dj) {
      if (lm1.at(di) == lm1.at(i) && lm2.at(dj) == lm2.at(j)) {
        cost_rename = ((*tr_post1).at(di - 1)->get_label_id() == (*tr_post2).at(dj - 1)->get_label_id())
          ? 0 : c.ren();
        fd.at(di).at(dj) = std::min(
          std::min(fd.at(di - 1).at(dj) + c.del(), fd.at(di).at(dj - 1) + c.ins()),
          fd.at(di - 1).at(dj - 1) + cost_rename
        );

        td.at(di).at(dj) = fd.at(di).at(dj);
      } else {
        fd.at(di).at(dj) = std::min(
          std::min(fd.at(di - 1).at(dj) + c.del(), fd.at(di).at(dj - 1) + c.ins()),
          fd.at(lm1.at(di) - 1).at(lm2.at(dj) - 1) + td.at(di).at(dj)
        );
      }
    }
  }
}

template<class _node = node, class _costs = costs<_node>>
std::vector<std::array<int, 2> > compute_edit_mapping(node* r1, node* r2, _costs c = _costs()){

  tr_post1 = generate_postorder(r1);
  tr_post2 = generate_postorder(r2);

  td.resize(tr_post1->size() + 1);
  for (unsigned int i = 0; i < tr_post1->size() + 1; ++i) {
    td.at(i).resize(tr_post2->size() + 1);
  }

  lm1.resize(tr_post1->size() + 1);
  lm2.resize(tr_post2->size() + 1);

  for (unsigned i = 0; i < tr_post1->size(); ++i) {
    lm1.at(i) = 0;
    lm2.at(i) = 0;
  }

  int max = ((tr_post1->size() < tr_post2->size()) ? tr_post1->size() + 1 :
    tr_post2->size() + 1);

  fd.resize(max);
  for (int i = 0; i < max; ++i) {
    fd.at(i).resize(max);
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      td.at(i).at(j) = 0;
    }
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      fd.at(i).at(j) = 0;
    }
  }

  make_leaves(r1, r2);

  lmld(r1, lm1);
  lmld(r2, lm2);

  std::vector<int> kr1;
  std::vector<int> kr2;
  kr1 = kr(lm1, leaves_t1.size());
  kr2 = kr(lm2, leaves_t2.size());

  //compute the distance
  for (unsigned int x = 1; x < kr1.size(); ++x) {
    for (unsigned int y = 1; y < kr2.size(); ++y) {
      forest_dist(kr1.at(x), kr2.at(y), c);
    }
  }
  
  std::vector<std::array<int, 2> > tree_pairs;
  std::vector<std::array<int, 2> > edit_mapping;
  tree_pairs.push_back({ r1->get_subtree_size(), r2->get_subtree_size() });  
  std::array<int, 2> tree_pair;
  bool root_node_pair = true;

  while (!tree_pairs.empty()) {
    tree_pair = tree_pairs.back();
    tree_pairs.pop_back();

    int last_row = tree_pair.at(0);
    int last_col = tree_pair.at(1);

    if (!root_node_pair) {
      forest_dist(last_row, last_col, c);
    }
    root_node_pair = false;

    int first_row = lm1.at(last_row) - 1;
    int first_col = lm2.at(last_col) - 1;
    int row = last_row;
    int col = last_col;

    while ((row > first_row) || (col > first_col)) {
      int cost_delete = c.del();
      int cost_insert = c.ins();

      if ((row > first_row) && (fd.at(row - 1).at(col) + cost_delete == fd.at(row).at(col)))
      {
        edit_mapping.push_back({ row, 0 });
        --row;
      } else if ( (col > first_col)
                  && (fd.at(row).at(col - 1) + cost_insert == fd.at(row).at(col)))
      {
        edit_mapping.push_back({ 0, col });
        --col;
      } else {
        if (lm1.at(row) == lm1.at(last_row)
            && lm2.at(col) == lm2.at(last_col))
        {  
          edit_mapping.push_back({ row, col });
          --row;
          --col;
        } else {
          tree_pairs.push_back({ row, col });
          row = lm1.at(row) - 1;
          col = lm2.at(col) - 1;
        }
      }
    }
  }

  return edit_mapping;
}

// Generic function to compute the distance between two trees under a specified
// cost model. Each tree is represented by its respective root node. A root node
// type is specified as first template parameter, the cost model type as second
// template parameter.
template<class _node = node, class _costs = costs<_node>>
double compute_zhang_shasha (_node* t1, _node* t2, _costs c = _costs()) {
  tr_post1 = generate_postorder(t1);
  tr_post2 = generate_postorder(t2);

  td.resize(tr_post1->size() + 1);
  for (unsigned int i = 0; i < td.size(); ++i) {
    td.at(i).resize(tr_post2->size() + 1);
  }

  // is there any reason to use reserve instead? (reserve was used before)
  // however, reserve only increases the capacity of the vector, not the size
  // capacity = the space allocated for the vector (e.g. if push_back is used)
  // size = the number of actual elements stores in the vector
  lm1.resize(tr_post1->size() + 1);
  lm2.resize(tr_post2->size() + 1);

  // initialization (to zero)
  std::fill(lm1.begin(), lm1.end(), 0);
  std::fill(lm2.begin(), lm2.end(), 0);

  int max = std::max(tr_post1->size(), tr_post2->size()) + 1;

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

  make_leaves(t1, t2);

  lmld(t1, lm1);
  lmld(t2, lm2);

  std::vector<int> kr1;
  std::vector<int> kr2;
  kr1 = kr(lm1, leaves_t1.size());
  kr2 = kr(lm2, leaves_t2.size());

  //compute the distance
  for (unsigned int x = 1; x < kr1.size(); ++x) {
    for (unsigned int y = 1; y < kr2.size(); ++y) {
      forest_dist(kr1.at(x), kr2.at(y), c);
    }
  }

  double ted = td.at(tr_post1->size()).at(tr_post2->size());
  delete tr_post1;
  delete tr_post2;

  std::vector<node*>().swap(leaves_t1);
  std::vector<node*>().swap(leaves_t2);
  std::vector<int>().swap(lm1);
  std::vector<int>().swap(lm2);
  std::vector<int>().swap(kr1);
  std::vector<int>().swap(kr2);
  std::vector<std::vector<double> >().swap(td);
  std::vector<std::vector<double> >().swap(fd);

  return ted;
}

}

#endif // ZHANG_SHASHA_H
