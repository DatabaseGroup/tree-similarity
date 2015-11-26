#include "LblTree.h"
#include <iostream>

int LblTree::nodeIDCounter = 0;

const int LblTree::getId() const {
	return id;
}


std::vector<Node*> LblTree::generatePostorder(){
    //tr_post = new std::vector<Node*>();
    nodeIDCounter = 1;
    LblTree::postorder((Node*) this);
    return tr_post;
}


void LblTree::postorder(Node* root){
    if(root){ //not null
        if(root->get_children_number() > 0){
            for(int i = 0; i < root->get_children_number(); i++){
                LblTree::postorder(root->get_child(i));
            }
        }
        root->id = nodeIDCounter;
        nodeIDCounter++;
        tr_post.push_back(root);
    }
}
void LblTree::makeLeaves(){
    LblTree::setLeaves(this, leaves);
    std::cout << "Leaves capacity:" <<  leaves.capacity()<< std::endl;
    for(unsigned int i = 0; i < leaves.size(); i++){
        std::cout << leaves[i]->id << " ";
    }
    std::cout << std::endl;
}

void LblTree::setLeaves(LblTree* root, std::vector<Node*>& leaves){
    if(root){ //not null
        //std::cout << "leaves size = " << leaves.size() << std::endl;
        if(root->get_children_number() > 0){
            for(int i = 0; i < root->get_children_number(); i++){
             //   std::cout << "Child: " << root->get_child(i)->get_labelID() << std::endl;
                root->setLeaves((LblTree*)root->get_child(i), leaves);

            }
        }else{
           // std::cout << "Keine Kinder: Node = Wurzel = " << root->get_labelID() << std::endl;
            //std::cout << "hinzu: " << root->label_id << std::endl;
            leaves.push_back(root);
            //std::cout << "capacity: " << leaves.size() << std::endl;
           // std::cout << "Nach push_back" << std::endl;
        }
    }
}

