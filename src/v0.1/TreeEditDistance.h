#ifndef TreeEditDistance_H
#define TreeEditDistance_H

#include "LblTree.h"

class TreeEditDistance{
    public:
        int w_insert, w_delete, w_rename;

        TreeEditDistance(int ins, int del, int ren){
            w_insert = ins;
            w_delete = del;
            w_rename = ren;
         }
        ~TreeEditDistance(){};
        virtual int calculateDistance(LblTree*, LblTree*, bool)=0; //=0 means that calculateDistnace is not implemented here
};

#endif // TreeEditDistance_H
