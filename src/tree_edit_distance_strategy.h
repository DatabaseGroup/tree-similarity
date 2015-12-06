#ifndef TREE_EDIT_DISTANCE_STRATEGY_H
#define TREE_EDIT_DISTANCE_STRATEGY_H

#include "lbl_tree.h"

class tree_edit_distance_strategy {
protected:
  int w_insert, w_delete, w_rename;

  tree_edit_distance_strategy (int ins, int del, int ren)
    : w_insert(ins), w_delete(del), w_rename(ren) { }
  ~tree_edit_distance_strategy () { };

public:
  virtual int calculate_distance (lbl_tree*, lbl_tree*, bool) = 0;
};

#endif // TREE_EDIT_DISTANCE_STRATEGY_H
