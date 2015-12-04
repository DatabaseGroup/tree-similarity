#ifndef StringEditDistance_H
#define StringEditDistance_H

#include <string>
#include <iostream>
#include "TreeEditDistance.h"


class StringEditDistance : public TreeEditDistance{
    private:
    std::vector<int> s1;
    std::vector<int> s2;

	public:
		StringEditDistance(int ins, int del, int ren) :TreeEditDistance(ins,del,ren){}
		virtual ~StringEditDistance() { };

		int calculateDistance(LblTree*, LblTree*, bool);
};

#endif // StringEditDistance_H
