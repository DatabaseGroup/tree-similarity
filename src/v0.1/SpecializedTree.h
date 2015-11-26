#ifndef SPECIALIZEDTREE_H
#define SPECIALIZEDTREE_H

#include "LblTree.h"

class SpecializedTree : public LblTree {
	private:
		int dummy;

	public:
		SpecializedTree() : LblTree() { }
		SpecializedTree(int t_id) : LblTree(t_id) {}
		SpecializedTree(int t_id, int label_id) : LblTree(t_id, label_id) { }
		~SpecializedTree() { }
		const int getDummy() const;
};

#endif // SPECIALIZEDTREE_H
