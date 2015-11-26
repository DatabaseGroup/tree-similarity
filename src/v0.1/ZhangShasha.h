#ifndef ZhangShasha_H
#define ZhangShasha_H

#include <vector>
#include "TreeEditDistance.h"


class ZhangShasha : public TreeEditDistance{
//class ZhangShasha{
    private:

    //Node* **tr_post1;
    //Node* **tr_post2;
    //tr_post1[i][j] = Node*
    //Allocate memory
    //p2DArray = new double*[HEIGHT];
    //for (int i = 0; i < HEIGHT; ++1)
      //  p2DArray[i] = new double[WIDTH];
      //delete/free mit Schleife nicht vergessen

    //int **td;
    //int ** fd;
    //int *lm1;
    //int *lm2;

    std::vector<Node*> tr_post1;
    std::vector<Node*> tr_post2;

    std::vector<std::vector<int> > td;
    std::vector<std::vector<int> > fd;
    std::vector<int> lm1;
    std::vector<int> lm2;
    std::vector<int> kr1;
    std::vector<int> kr2;

    //int w_delete;
    //int w_insert;
    //int w_rename;
    int cost_rename;

    int tempmin;

    void forest_dist(int, int);
    //void lmld(LblTree, std::vector<int>);
    void lmld(LblTree*, std::vector<int>&);
    //std::vector<int> kr(std::vector<int>, int);
    std::vector<int> kr(std::vector<int>&, int);

    void forestDist(int, int);

    public:
        ZhangShasha(int ins, int del, int ren) :TreeEditDistance(ins,del,ren){}
        ~ZhangShasha();

        int calculateDistance(LblTree*, LblTree*, bool);
        int zaehler; //debug
};

#endif // ZhangShasha_H
