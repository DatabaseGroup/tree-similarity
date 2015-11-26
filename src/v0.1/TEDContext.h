#ifndef TEDContext_H
#define TEDContext_H

#include "ZhangShasha.h"


class TEDContext{

    private:
        TreeEditDistance *strategy;

    public:
        TEDContext(){};
        ~TEDContext(){};

        int execute(LblTree*, LblTree*, bool);
        void setStrategy(int, int, int, int);
        TreeEditDistance* getStrategy();

         enum StrategyType
        {
            ZS
        };
};

#endif // TEDContext_H
