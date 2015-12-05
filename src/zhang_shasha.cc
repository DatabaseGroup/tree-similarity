#include <iostream>
#include "ZhangShasha.h"

int ZhangShasha::calculateDistance(LblTree* t1, LblTree* t2, bool debug){
    zaehler = 0;
    tr_post1 = t1->generatePostorder();
    tr_post2 = t2->generatePostorder();

    td.resize(tr_post1.size()+1);
    for(unsigned int i = 0; i < tr_post1.size()+1; i++){
        td[i].resize(tr_post2.size()+1);
    }

    lm1.reserve(tr_post1.size()+1);
    lm2.reserve(tr_post2.size()+1);

    for(unsigned i = 0; i < tr_post1.size(); i++){
        lm1[i] = 0;
        lm2[i] = 0;
    }

    int max = (tr_post1.size() < tr_post2.size()) ? tr_post1.size()+1 : tr_post2.size()+1;

    fd.resize(max);
    for(int i = 0; i < max; i++){
        fd[i].resize(max);
    }

    //init td and fd with 0
    for(int i = 0; i < max; i++){
        for(int j = 0; j < max; j++){
            td[i][j] = 0;
        }
    }

    for(int i = 0; i < max; i++){
        for(int j = 0; j < max; j++){
            fd[i][j] = 0;
        }
    }

    t1->makeLeaves();
    t2->makeLeaves();
    lmld(t1, lm1);
    lmld(t2, lm2);
    kr1 = kr(lm1, t1->leaves.size());
    kr2 = kr(lm2, t2->leaves.size());

    //compute the distance
    for(unsigned int x = 1; x < kr1.size(); x++){
        for(unsigned int y = 1; y < kr2.size(); y++){
            forestDist(kr1[x], kr2[y]);
        }
    }
    return td[tr_post1.size()][tr_post2.size()];
}

void ZhangShasha::lmld(LblTree* root, std::vector<int> &lm){
    LblTree* tempNode;
    for(int i = 1; i <= root->get_children_number(); i++){
        tempNode = (LblTree*) root->get_children().at(i-1);
        lmld(tempNode, lm);
    }

    if(root->get_children_number() == 0){
        lm[root->id] = root->id;
    }else{
        LblTree* child1 = (LblTree*) root->get_children().at(0);
        lm[root->id] = lm[child1->id];
    }
}

std::vector<int> ZhangShasha::kr(std::vector<int>& l, int leafCount){
    std::vector<int> kr(leafCount+1);
    std::vector<int> visit(l.capacity());

    for(unsigned int i = 0; i < visit.capacity(); i++){
        visit[i] = 0;
    }

    int k = leafCount;
    int i = l.capacity()-1;

    while(k >= 1){
        if(visit[l[i]] == 0){
            kr[k] = i;
            k--;
            visit[l[i]] = 1;
        }
        i -=1;
    }
    return kr;
}

void ZhangShasha::forestDist(int i, int j){
    fd[lm1[i]-1][lm2[j]-1] = 0;

    for(int di = lm1[i]; di <= i; di++){
        fd[di][lm2[j]-1] = fd[di-1][lm2[j]-1] + w_delete;
    }

    for(int dj = lm2[j]; dj<= j; dj++){
        fd[lm1[i]-1][dj] = fd[lm1[i]-1][dj-1] + w_insert;
    }

    for(int di = lm1[i]; di <= i; di++){
        for(int dj = lm2[j]; dj <= j; dj++){
            if(lm1[di] == lm1[i] && lm2[dj] == lm2[j]){
                cost_rename = (tr_post1[di-1]->get_labelID() == tr_post2[dj-1]->get_labelID()) ? 0 : w_rename;
                tempmin = (fd[di-1][dj] + w_delete < fd[di][dj-1] + w_insert) ? fd[di-1][dj] + w_delete : fd[di][dj-1] + w_insert;
                fd[di][dj] = (tempmin < fd[di-1][dj-1] + cost_rename) ? tempmin : fd[di-1][dj-1] + cost_rename;

                //copy result to permanent array td
                td[di][dj] = fd[di][dj];
            }else{
                tempmin = (fd[di-1][dj] + w_delete < fd[di][dj-1] + w_insert) ? fd[di-1][dj] + w_delete : fd[di][dj-1] + w_insert;
                fd[di][dj] = (tempmin < fd[lm1[di]-1][lm2[dj]-1] + td[di][dj]) ? tempmin : fd[lm1[di]-1][lm2[dj]-1] + td[di][dj];
            }
        }
    }
}