#ifndef CALC_TREE_OPERATORS_INCLUDED
#define CALC_TREE_OPERATORS_INCLUDED

#include <math.h>

namespace OPE //Operator errors
{
    enum ERR
    {
        OK,
        NOT_EXIST
    } LAST_ERR;
};

static char operators[] = {'-', 1, '+', 1,'*', 2, '/', 3, '^', 4};

bool is_operator (char symb)
{
    for (int i = 0; i < sizeof (operators); i+= 2)
        if (symb == operators[i])
            return true;
        
    return false; 
}

char get_op_priority (char symb)
{
    for (int i = 0; i < sizeof (operators); i+= 2)
        if (symb == operators[i]) 
            return operators [i+1];
    
    return 0;
}

OPE::ERR use_operator (double a, double b, unsigned int op, double &res)
{
   switch (op)
   {
       case '+': res = a + b; return OPE::OK;
       case '-': res = a - b; return OPE::OK;
       case '/': res = a / b; return OPE::OK;
       case '*': res = a * b; return OPE::OK;
       case '^': res = pow (a, b); return OPE::OK;
       default : return OPE::NOT_EXIST;
   } 

}

#endif