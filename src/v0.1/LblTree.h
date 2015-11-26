#ifndef LBLTREE_H
#define LBLTREE_H

#include "Node.h"

//TODO Brackets-Parsen

class LblTree : public Node{
	protected:
        static int nodeIDCounter;

	public:
        int t_id;
        std::vector<Node*> tr_post;
        std::vector<Node*> leaves;

		LblTree() : Node() {this->t_id = 0;}
		LblTree(int t_id) : Node() {this->t_id = t_id;}
		LblTree(int t_id, int label_id) : Node(label_id)
		{
            this->t_id = t_id;
        };
		virtual ~LblTree() { };
		const int getId() const;

		std::vector<Node*> generatePostorder();
        void postorder(Node*);

        void setLeaves(LblTree*, std::vector<Node*>&);
        void makeLeaves();
};

#endif // LBLTREE_H
