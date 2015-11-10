#include <iostream>
#include "tree.h"

using namespace std;

int main()
{
    node* root = new node("root");
    node* a = new node("A");
    node* b = new node("B");
    root->addChild(a);
    //root->addChild(new node("B"));
    root->addChild(b);
    node* c = new node("C");
    a->addChild(c);


    return 0;
}
