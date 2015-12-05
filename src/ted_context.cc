#include "ted_context.h"

int ted_context::execute(lbl_tree* t1, lbl_tree* t2, bool debug){
  return strategy->calculate_distance(t1, t2, debug);
}

void ted_context::set_strategy(int type, int ins, int del, int ren){
  //delete strategy;
  if(type == ZS){
    strategy = new zhang_shasha(ins, del, ren);
  }
}

tree_edit_distance* ted_context::get_strategy(){
  return strategy;
}
