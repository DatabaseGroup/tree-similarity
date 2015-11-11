#ifndef UpperBound_H
#define UpperBound_H

#include "LblTree.h"

class UpperBound {
	protected:
		//int simplyUpperBound;

	public:
		UpperBound(){ };
		virtual ~UpperBound() { };

		int calculateSimplyUpperBound(LblTree* t1, LblTree* t2);
};


#endif // UpperBound_H
