#include <iostream>
#include "ZhangShasha.h"


int ZhangShasha::calculateDistance(LblTree* t1, LblTree* t2, bool debug){
    zaehler = 0;
    tr_post1 = t1->generatePostorder();
    tr_post2 = t2->generatePostorder();

    std::cout << "tr_post1.size " << tr_post1.size() << std::endl;
    std::cout << "tr_post2.size " << tr_post2.size() << std::endl;

    td.resize(tr_post1.size()+1);
    for(unsigned int i = 0; i < tr_post1.size()+1; i++){
        td[i].resize(tr_post2.size()+1);
    }

    //check tr_post1 and tr_post2 BEGIN

    for(unsigned int i = 0; i < tr_post1.size(); i++){
        std::cout << tr_post1.at(i)->label_id << ", ";
    }
    std::cout << std::endl;

    for(unsigned int i = 0; i < tr_post2.size(); i++){
        std::cout << tr_post2.at(i)->label_id<< ", ";
    }
    std::cout << std::endl;

    //END


    lm1.reserve(tr_post1.size()+1);
    lm2.reserve(tr_post2.size()+1);

    for(unsigned i = 0; i < tr_post1.size(); i++){
        lm1[i] = 0;
        lm2[i] = 0;
    }

    int max = (tr_post1.size() < tr_post2.size()) ? tr_post1.size()+1 : tr_post2.size()+1;
    std::cout << "max: " << max << std::endl;
    //std::cout << "lm1: " << lm1.size() << std::endl;
    //std::cout << "lm2: " << lm2.size() << std::endl;

    fd.resize(max);
    for(int i = 0; i < max; i++){
        fd[i].resize(max);
    }

    std::cout << "nach resize 1" << std::endl;

    //init td and fd with 0
    for(int i = 0; i < max; i++){
        for(int j = 0; j < max; j++){
            td[i][j] = 0;
        }
    }
    std::cout << "nach resize 2" << std::endl;


    for(int i = 0; i < max; i++){
        for(int j = 0; j < max; j++){
            fd[i][j] = 0;
        }
    }
    std::cout << "vor makeLeaves" << std::endl;
    t1->makeLeaves();
    //std::cout << "makeLeaves(2)" << std::endl;
    t2->makeLeaves();
    //std::cout << "makeLeaves fertig " << std::endl;

    //std::cout << "Root t1: " << t1->id << std::endl;
    //std::cout << "lm-size: vor lmld" << lm1.capacity() << std::endl;



    //std::cout << "vor lmld()" << std::endl;
    lmld(t1, lm1);
    std::cout << "LMLD fertig 1" << std::endl;
    lmld(t2, lm2);

    std::cout << "LMLD fertig 1 + 2" << std::endl;


    //Ausgabe lm1 und lm2
    std::cout << "lm1 size: " << lm1.capacity() << std::endl;
    for(unsigned int i = 0; i < lm1.capacity(); i++){
        std::cout << lm1[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "lm2 size: " << lm2.capacity() << std::endl;
    for(unsigned int i = 0; i < lm2.capacity(); i++){
        std::cout << lm2[i] << " ";
    }
    std::cout << std::endl;

    kr1 = kr(lm1, t1->leaves.size());
    kr2 = kr(lm2, t2->leaves.size());

    std::cout << "Keyroot 1 size:" << kr1.size() << " ," << kr1.capacity() <<std::endl;
    for(unsigned int i = 0; i < kr1.size(); i++){
        std::cout << kr1[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Keyroot 2 size:" << kr2.size() << " ," << kr2.capacity() << std::endl;
    for(unsigned int i = 0; i < kr2.size(); i++){
        std::cout << kr2[i] << " ";
    }
    std::cout << std::endl;



    //compute the distance
    std::cout << "kr1.size " << kr1.size() << std::endl;
    std::cout << "kr2.size " << kr2.size() << std::endl;

    for(unsigned int x = 1; x < kr1.size(); x++){
        for(unsigned int y = 1; y < kr2.size(); y++){
            forestDist(kr1[x], kr2[y]);
        }
    }
    return td[tr_post1.size()][tr_post2.size()];
}

void ZhangShasha::lmld(LblTree* root, std::vector<int> &lm){
    LblTree* tempNode;
    std::cout << "root-id anfang: " << root->id << std::endl;
    std::cout << "root Children number: " << root->get_children_number() << std::endl;
    for(int i = 1; i <= root->get_children_number(); i++){
        tempNode = (LblTree*) root->get_children().at(i-1);
        lmld(tempNode, lm);
    }

    if(root->get_children_number() == 0){
        std::cout << "root-id: " << root->id << std::endl;
        std::cout << "lm size: " << lm.capacity() << std::endl;
        lm[root->id] = root->id;
    }else{
        LblTree* child1 = (LblTree*) root->get_children().at(0);
        lm[root->id] = lm[child1->id]; //TODO fange bei 0 an
    }
}

std::vector<int> ZhangShasha::kr(std::vector<int>& l, int leafCount){
    std::vector<int> kr(leafCount+1);
    //std::cout << "capa: " << l.capacity() << " size: " << l.size() << std::endl;
    std::vector<int> visit(l.capacity()); //hier capacity ok weil haendisch gesetzt


    //std::cout << "l.capacity(): " << l.capacity() << std::endl;
    //std::cout << "visit.capacity(): " << visit.capacity() << std::endl;

    for(unsigned int i = 0; i < visit.capacity(); i++){
        visit[i] = 0;
    }

    int k = leafCount;
    int i = l.capacity()-1;

    std::cout << "k: " << k << "i: " << i << std::endl;

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
                std::cout << "Speichere in td: " << td[di][dj] << std::endl;
            }else{
                tempmin = (fd[di-1][dj] + w_delete < fd[di][dj-1] + w_insert) ? fd[di-1][dj] + w_delete : fd[di][dj-1] + w_insert;
                fd[di][dj] = (tempmin < fd[lm1[di]-1][lm2[dj]-1] + td[di][dj]) ? tempmin : fd[lm1[di]-1][lm2[dj]-1] + td[di][dj];
            }
        }
    }


    //Debug
    /*
    zaehler ++;
    std::cout << "Call " << zaehler << std::endl;
    std::cout << "fd-array" << std::endl;
    for(unsigned int a = 0; a < fd.size(); a++){
        for(unsigned int b = 0; b < fd.size(); b++){
            std::cout << fd[a][b] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "td-array" << std::endl;
    for(unsigned int a = 0; a < td.size(); a++){
        for(unsigned int b = 0; b < td.size(); b++){
            std::cout << td[a][b] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    zaehler ++;
    */

}


