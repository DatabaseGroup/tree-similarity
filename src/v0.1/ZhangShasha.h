#ifndef ZhangShasha_H
#define ZhangShasha_H

#include <vector>
#include "TreeEditDistance.h"

class ZhangShasha : public TreeEditDistance{
//class ZhangShasha{
    private:
    std::vector<Node*> tr_post1;
    std::vector<Node*> tr_post2;
    std::vector<std::vector<int> > td;
    std::vector<std::vector<int> > fd;
    std::vector<int> lm1;
    std::vector<int> lm2;
    std::vector<int> kr1;
    std::vector<int> kr2;

    int cost_rename;
    int tempmin;

    void forest_dist(int, int);
    void lmld(LblTree*, std::vector<int>&);
    std::vector<int> kr(std::vector<int>&, int);
    void forestDist(int, int);

    public:
        ZhangShasha(int ins, int del, int ren) :TreeEditDistance(ins,del,ren){}
        ~ZhangShasha();

        int calculateDistance(LblTree*, LblTree*, bool);
        int zaehler; //debug
};

#endif // ZhangShasha_H
