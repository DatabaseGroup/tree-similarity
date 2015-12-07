#ifndef ZHANG_SHASHA_H
#define ZHANG_SHASHA_H

#include <vector>

#include "node.h"

namespace zs {

extern std::vector<node*> tr_post1;
extern std::vector<node*> tr_post2;
extern std::vector<std::vector<int> > td;
extern std::vector<std::vector<int> > fd;
extern std::vector<int> lm1;
extern std::vector<int> lm2;
extern std::vector<int> kr1;
extern std::vector<int> kr2;

void lmld (node* root, std::vector<int>& lm);
std::vector<int> kr (std::vector<int>& l, int leaf_count);

template<class _node = node, class _costs = costs<_node>>
void forest_dist(int i, int j, _costs c = _costs()) {
  int tempmin;
  int cost_rename;

  fd[lm1[i] - 1][lm2[j] - 1] = 0;

  for (int di = lm1[i]; di <= i; di++) {
    fd[di][lm2[j] - 1] = fd[di - 1][lm2[j] - 1] + c.del();
  }

  for (int dj = lm2[j]; dj<= j; dj++) {
    fd[lm1[i] - 1][dj] = fd[lm1[i] - 1][dj - 1] + c.ins();
  }

  for (int di = lm1[i]; di <= i; di++) {
    for (int dj = lm2[j]; dj <= j; dj++) {
      if (lm1[di] == lm1[i] && lm2[dj] == lm2[j]) {
        cost_rename =
          (tr_post1[di - 1]->get_label_id() == tr_post2[dj - 1]->get_label_id())
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

/**
 * Generic function to compute the distance between two trees under a specified
 * cost model. Each tree is represented by its respective root node. A root node
 * type is specified as first template parameter, the cost model type as second
 * template parameter.
 */
template<class _node = node, class _costs = costs<_node>>
int compute_zhang_shasha (_node* t1, _node* t2, _costs c = _costs()) {
  tr_post1 = t1->generate_postorder();
  tr_post2 = t2->generate_postorder();

  td.resize(tr_post1.size() + 1);
  for (unsigned int i = 0; i < tr_post1.size() + 1; ++i) {
    td[i].resize(tr_post2.size() + 1);
  }

  lm1.reserve(tr_post1.size() + 1);
  lm2.reserve(tr_post2.size() + 1);

  for (unsigned i = 0; i < tr_post1.size(); ++i) {
    lm1[i] = 0;
    lm2[i] = 0;
  }

  int max = ((tr_post1.size() < tr_post2.size()) ? tr_post1.size() + 1 :
    tr_post2.size() + 1);

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

  t1->make_leaves();
  t2->make_leaves();
  lmld(t1, lm1);
  lmld(t2, lm2);
  kr1 = kr(lm1, t1->leaves.size());
  kr2 = kr(lm2, t2->leaves.size());

  //compute the distance
  for (unsigned int x = 1; x < kr1.size(); x++) {
    for (unsigned int y = 1; y < kr2.size(); y++) {
      forest_dist(kr1[x], kr2[y], c);
    }
  }

  return td[tr_post1.size()][tr_post2.size()];
}

};

#endif // ZHANG_SHASHA_H
