#ifndef TREE_EDIT_DISTANCE_H
#define TREE_EDIT_DISTANCE_H

#include "zhang_shasha.h"

class tree_edit_distance {
private:
  tree_edit_distance_strategy* teds;

public:
  tree_edit_distance (tree_edit_distance_strategy* teds);
  ~tree_edit_distance () { }

  int execute (lbl_tree* t1, lbl_tree* t2, bool debug);
  void set_strategy (tree_edit_distance_strategy* teds);
  tree_edit_distance_strategy* get_strategy ();
};

#endif // TREE_EDIT_DISTANCE_H
