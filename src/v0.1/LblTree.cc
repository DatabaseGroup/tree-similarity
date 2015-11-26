#include "LblTree.h"

int LblTree::nodeIDCounter = 0;

const int LblTree::getId() const {
	return id;
}

std::vector<Node*> LblTree::generatePostorder(){
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
}

void LblTree::setLeaves(LblTree* root, std::vector<Node*>& leaves){
    if(root){ //not null
        if(root->get_children_number() > 0){
            for(int i = 0; i < root->get_children_number(); i++){
                root->setLeaves((LblTree*)root->get_child(i), leaves);
            }
        }else{
            leaves.push_back(root);
        }
    }
}

