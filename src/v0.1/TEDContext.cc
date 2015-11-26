#include "TEDContext.h"


int TEDContext::execute(LblTree* t1, LblTree* t2, bool debug){
    return strategy->calculateDistance(t1,t2,debug);
}

void TEDContext::setStrategy(int type, int ins, int del, int ren){
    //delete strategy;
    if(type == ZS){
        strategy = new ZhangShasha(ins, del, ren);
    }
}

TreeEditDistance* TEDContext::getStrategy(){
    return strategy;
}

