#include "StringEditDistance.h"

int StringEditDistance::calculateSED(std::string s1, std::string s2)
{
    int c, temp;

    int result[s1.length()][s2.length()];
    for(unsigned int i = 0; i < s1.length(); i++){
        result[i][0] = i;
    }

    for(unsigned int i = 0; i < s2.length(); i++){
        result[0][i] = i;
    }

    for(unsigned int j = 1; j < s2.length(); j++){
        for(unsigned int i = 1; i < s1.length(); i++){

            c = (s1.at(i) == s2.at(j)) ? 0 : 1;

            //calculate minimum
            temp = (result[i-1][j-1] + c < result[i-1][j] + 1) ? result[i-1][j-1] + c : result[i-1][j] + 1;
            result[i][j] = (temp < result[i][j-1] + 1) ? temp : result[i][j-1] + 1;
        }
    }

    return result[s1.length()-1][s2.length()-1];
}
