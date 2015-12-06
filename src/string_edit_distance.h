#ifndef STRING_EDIT_DISTANCE_H
#define STRING_EDIT_DISTANCE_H

#include <string>
#include <iostream>
#include "tree_edit_distance_strategy.h"

class string_edit_distance : public tree_edit_distance_strategy {
private:
  std::vector<int> s1;
  std::vector<int> s2;

public:
  string_edit_distance (int ins, int del, int ren)
    : tree_edit_distance_strategy(ins, del, ren) { }
  virtual ~string_edit_distance () { };

  int calculate_distance(lbl_tree*, lbl_tree*, bool);
};

#endif // STRING_EDIT_DISTANCE_H
