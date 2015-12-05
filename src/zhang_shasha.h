#ifndef ZHANG_SHASHA_H
#define ZHANG_SHASHA_H

#include <vector>
#include "tree_edit_distance.h"

class zhang_shasha : public tree_edit_distance {
private:
  std::vector<node*> tr_post1;
  std::vector<node*> tr_post2;
  std::vector<std::vector<int> > td;
  std::vector<std::vector<int> > fd;
  std::vector<int> lm1;
  std::vector<int> lm2;
  std::vector<int> kr1;
  std::vector<int> kr2;

  int cost_rename;
  int tempmin;

  void forest_dist (int, int);
  void lmld (lbl_tree*, std::vector<int>&);
  std::vector<int> kr (std::vector<int>&, int);

public:
  int zaehler; //debug

  zhang_shasha (int ins, int del, int ren)
    : tree_edit_distance(ins, del, ren) { }
  ~zhang_shasha ();

  int calculate_distance (lbl_tree*, lbl_tree*, bool);
};

#endif // ZHANG_SHASHA_H
