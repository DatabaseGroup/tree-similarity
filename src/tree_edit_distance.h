#ifndef TREE_EDIT_DISTANCE_H
#define TREE_EDIT_DISTANCE_H

#include "lbl_tree.h"

class tree_edit_distance {
public:
  int w_insert, w_delete, w_rename;

  tree_edit_distance (int ins, int del, int ren)
    : w_insert(ins), w_delete(del), w_rename(ren) { }
  
  ~tree_edit_distance () { };
  virtual int calculate_distance (lbl_tree*, lbl_tree*, bool) = 0;
};

#endif // TREE_EDIT_DISTANCE_H
