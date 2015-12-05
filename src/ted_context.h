#ifndef TED_CONTEXT_H
#define TED_CONTEXT_H

#include "zhang_shasha.h"

class ted_context {
private:
  tree_edit_distance *strategy;

public:
  enum strategy_type {
  ZS
  };
  ted_context () { };
  ~ted_context () { };

  int execute(lbl_tree*, lbl_tree*, bool);
  void set_strategy(int, int, int, int);
  tree_edit_distance* get_strategy();
};

#endif // TED_CONTEXT_H
