#ifndef LBLTREE_H
#define LBLTREE_H

#include "Node.h"

//TODO Brackets-Parsen

class LblTree : public Node{
	protected:
		int id;

	public:
		LblTree() : Node() {this->id = 0;}
		LblTree(int t_id) : Node() {this->id = t_id;}
		LblTree(int t_id, int label_id) : Node(label_id)
		{
            this->id = t_id;
        };
		virtual ~LblTree() { };
		const int getId() const;
};

#endif // LBLTREE_H
