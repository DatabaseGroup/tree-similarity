#ifndef TreeEditDistance_H
#define TreeEditDistance_H

#include "LblTree.h"
#include <iostream>

class TreeEditDistance{
    public:
        int w_insert, w_delete, w_rename;

        TreeEditDistance(int ins, int del, int ren){
            std::cout << "Kosten: " << ins << " " << del << " " << ren << std::endl;
            w_insert = ins;
            w_delete = del;
            w_rename = ren;
            std::cout << "Kosten_W: " << w_insert << " " << w_delete << " " << w_rename << std::endl;
         }
        ~TreeEditDistance(){};
        virtual int calculateDistance(LblTree*, LblTree*, bool)=0; //=0 means that calculateDistnace is not implemented here
};

#endif // TreeEditDistance_H
