#ifndef ZHANG_SHASHA_H
#define ZHANG_SHASHA_H

#include <vector>
#include "node.h"

namespace zs {

extern std::vector<node*>* tr_post1; //postorder of tree 1
extern std::vector<node*>* tr_post2; //postorder of tree 2
extern std::vector<std::vector<double> > td; //stores the tree-edit-distances
extern std::vector<std::vector<double> > fd; //stores the forest-distances
extern std::vector<int> lm1; //stores the left-most-leaf-descendants of tree 1
extern std::vector<int> lm2; //stores the left-most-leaf-descendants of tree 2
extern std::vector<node*> leaves_t1; //stores the leaves of tree 2
extern std::vector<node*> leaves_t2; //stores the leaves of tree 2

void lmld (node* root, std::vector<int>& lm); //see .cc-file
std::vector<int> kr (std::vector<int>& l, int leaf_count);//stores the key-roots (see .cc-file)

void make_leaves (node* t1, node* t2); //see .cc-file
void set_leaves (node* root, int whichTree);

//computes the forest-distance of:
// T1[l(i)...di], where di is element of desc(T1[i]) and
// T2[l(j)...dj], where dj is element of desc(T2[j])
//
//Param: i, j defined as above and a optional cost-model
template<class _node = node, class _costs = costs<_node>>
void forest_dist(int i, int j, _costs c = _costs()) {
  int tempmin;
  int cost_rename;

  fd[lm1[i] - 1][lm2[j] - 1] = 0;

  for (int di = lm1[i]; di <= i; ++di) {
    fd[di][lm2[j] - 1] = fd[di - 1][lm2[j] - 1] + c.del();
  }

  for (int dj = lm2[j]; dj<= j; ++dj) {
    fd[lm1[i] - 1][dj] = fd[lm1[i] - 1][dj - 1] + c.ins();
  }

  for (int di = lm1[i]; di <= i; ++di) {
    for (int dj = lm2[j]; dj <= j; ++dj) {
      if (lm1[di] == lm1[i] && lm2[dj] == lm2[j]) {
        cost_rename =
          ((*tr_post1)[di - 1]->get_label_id() == (*tr_post2)[dj - 1]->get_label_id())
          ? 0 : c.ren();
        tempmin =
          (fd[di - 1][dj] + c.del() < fd[di][dj - 1] + c.ins()) ?
          fd[di - 1][dj] + c.del() : fd[di][dj - 1] + c.ins();
        fd[di][dj] = (tempmin < fd[di - 1][dj - 1] + cost_rename) ?
          tempmin : fd[di - 1][dj - 1] + cost_rename;

        //copy result to permanent array td
        td[di][dj] = fd[di][dj];
      } else {
        tempmin = (fd[di - 1][dj] + c.del() < fd[di][dj - 1] + c.ins()) ?
          fd[di - 1][dj] + c.del() : fd[di][dj - 1] + c.ins();
        fd[di][dj] = (tempmin < fd[lm1[di] - 1][lm2[dj] - 1] + td[di][dj]) ?
          tempmin : fd[lm1[di] - 1][lm2[dj] - 1] + td[di][dj];
      }
    }
  }
}

template<class _node = node, class _costs = costs<_node>>
std::vector<std::array<int, 2> > computeEditMapping(node* r1, node* r2, _costs c = _costs()){

  tr_post1 = generate_postorder(r1);
  tr_post2 = generate_postorder(r2);

  td.resize(tr_post1->size() + 1);
  for (unsigned int i = 0; i < tr_post1->size() + 1; ++i) {
    td[i].resize(tr_post2->size() + 1);
  }

  lm1.reserve(tr_post1->size() + 1);
  lm2.reserve(tr_post2->size() + 1);

  for (unsigned i = 0; i < tr_post1->size(); ++i) {
    lm1[i] = 0;
    lm2[i] = 0;
  }

  int max = ((tr_post1->size() < tr_post2->size()) ? tr_post1->size() + 1 :
    tr_post2->size() + 1);

  fd.resize(max);
  for (int i = 0; i < max; ++i) {
    fd[i].resize(max);
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      td[i][j] = 0;
    }
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      fd[i][j] = 0;
    }
  }

  make_leaves(r1, r2);

  lmld(r1, lm1);
  lmld(r2, lm2);

  // ?? what, lm1.size() --> 1?? wt // kk his postorder starts with 0?
  //std::cout << "lm1size: " << lm1.size() << std::endl;

  std::vector<int> kr1;
  std::vector<int> kr2;
  kr1 = kr(lm1, leaves_t1.size());
  kr2 = kr(lm2, leaves_t2.size());

  //compute the distance
  for (unsigned int x = 1; x < kr1.size(); ++x) {
    for (unsigned int y = 1; y < kr2.size(); ++y) {
      forest_dist(kr1[x], kr2[y],c);
    }
  }
  
  std::vector<std::array<int, 2> > treePairs;
  std::vector<std::array<int, 2> > editMapping;
  treePairs.push_back({r1->get_subtree_size(), r2->get_subtree_size()});  
  std::array<int, 2> treePair;
  bool rootNodePair = true;

  std::vector<node*>* post_r1 = generate_postorder(r1);
  std::vector<node*>* post_r2 = generate_postorder(r2);

  while(!treePairs.empty()){
    treePair = treePairs.back();
    treePairs.pop_back();

    int lastRow = treePair[0];
    int lastCol = treePair[1];

    if(!rootNodePair){
      forest_dist(lastRow, lastCol,c);
    }
    rootNodePair = false;

    int firstRow = post_r1->at(lastRow-1)->get_lml()->get_id() - 1;
    int firstCol = post_r2->at(lastCol-1)->get_lml()->get_id() - 1;
    int row = lastRow;
    int col = lastCol;

    while((row > firstRow) || (col > firstCol)){
      int costDelete = c.del();
      int costInsert = c.ins();

      if((row > firstRow) && (fd[row-1][col] + costDelete == fd[row][col])){
        editMapping.push_back({row,0});
        row--;
      } else if((col > firstCol) && (fd[row][col -1] + costInsert == fd[row][col])){
        editMapping.push_back({0,col});
        col--;
      } else {
        if((post_r1->at(row-1)->get_lml() == post_r1->at(lastRow-1)->get_lml()
        && post_r2->at(col-1)->get_lml() == post_r2->at(lastCol-1)->get_lml())){
          
          editMapping.push_back({row,col});
          row--;
          col--;
        } else {
          treePairs.push_back({row, col});
          row = post_r1->at(row-1)->get_lml()->get_id() -1;
          col = post_r2->at(col-1)->get_lml()->get_id() -1;
        }
      }
    }
  }

  return editMapping;
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
  for (unsigned int i = 0; i < tr_post1->size() + 1; ++i) {
    td[i].resize(tr_post2->size() + 1);
  }

  lm1.reserve(tr_post1->size() + 1);
  lm2.reserve(tr_post2->size() + 1);

  for (unsigned i = 0; i < tr_post1->size(); ++i) {
    lm1[i] = 0;
    lm2[i] = 0;
  }

  int max = ((tr_post1->size() < tr_post2->size()) ? tr_post1->size() + 1 :
    tr_post2->size() + 1);

  fd.resize(max);
  for (int i = 0; i < max; ++i) {
    fd[i].resize(max);
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      td[i][j] = 0;
    }
  }

  for (int i = 0; i < max; ++i) {
    for (int j = 0; j < max; ++j) {
      fd[i][j] = 0;
    }
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
      forest_dist(kr1[x], kr2[y], c);
    }
  }

  double ted = td[tr_post1->size()][tr_post2->size()];
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
