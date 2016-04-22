#include <iostream>
#include "zhang_shasha.h"

namespace zs {

std::vector<node*>* tr_post1;
std::vector<node*>* tr_post2;
std::vector<std::vector<double> > td;
std::vector<std::vector<double> > fd;
std::vector<int> lm1;
std::vector<int> lm2;
std::vector<node*> leaves_t1;
std::vector<node*> leaves_t2;

//Computes the left most leaf descendant for all subtrees of the input tree
//
//Param: root of the tree, a vector which stores the left most leaf descendant for each subtree
void lmld (node* root, std::vector<int> &lm) {
  node* tempNode;
  // call lmld recursively and compute the left-most-leaf descendants
  for (int i = 1; i <= root->get_children_number(); ++i) {
    tempNode = root->get_children().at(i-1);
    lmld(tempNode, lm);
  }

  if (root->get_children_number() == 0) {
    lm[root->get_id()] = root->get_id();
  } else {
    node *child1 = root->get_children().at(0);
    lm[root->get_id()] = lm[child1->get_id()];
  }
}

//Computes the key-roots of a specific tree. A key-root is either the root or a node with a left sibling.
//
//Param: the left-most leaf descendats for all subtrees of the input-tree, the number of leaves of the input-tree
//
//Return: A vector storing all key-roots of the input-tree
std::vector<int> kr (std::vector<int>& l, int leaf_count) {
  std::vector<int> kr(leaf_count + 1);
  std::vector<int> visit(l.capacity()); //Was node visited

//set all nodes to unvisited
  for (unsigned int i = 0; i < visit.capacity(); ++i) {
    visit[i] = 0;
  }

  int k = leaf_count;
  int i = l.capacity() - 1;

  while (k >= 1) {

    if (visit[l[i]] == 0) {
      kr[k] = i;
      --k;
      visit[l[i]] = 1;
    }
    i -= 1;
  }
  
  return kr;
}

//Computes for each input-tree a vector consisting of all leaves of the specific tree.
//
//Parameters: left-hand tree, right-hand tree
void make_leaves (node* t1, node* t2) {
  set_leaves(t1,1);
  set_leaves(t2,2);
}

void set_leaves (node* root, int whichTree) {
  if(whichTree == 1){
  if (root) {
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        set_leaves(root->get_child(i), 1);
      }
    } else {
      leaves_t1.push_back(root);
    }
  }
  }else{
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); ++i) {
        set_leaves(root->get_child(i), 2);
      }
    } else {
      leaves_t2.push_back(root);
    }
  }
}

}
