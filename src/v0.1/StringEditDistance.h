#ifndef StringEditDistance_H
#define StringEditDistance_H

#include <string>
#include <iostream>

class StringEditDistance {
	public:
		StringEditDistance(){ };
		virtual ~StringEditDistance() { };

		int calculateSED(std::string, std::string);
};

#endif // StringEditDistance_H
