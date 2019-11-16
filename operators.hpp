#ifndef CALC_TREE_OPERATORS_INCLUDED
#define CALC_TREE_OPERATORS_INCLUDED

namespace OPE //Operator errors
{
    enum ERR
    {
        OK,
        NOT_EXIST
    } LAST_ERR;
};

static char operators[] = {'-', '+', '/', '*'};

bool is_operator (unsigned int symb)
{
    for (int i = 0; i < sizeof (operators); i++)
        if (symb == operators[i])
            return true;
        
    return false; 
}

unsigned int get_operator_number (unsigned int symb)
{
    for (int i = 0; i < sizeof (operators); i++)
        if (symb == operators[i])
            return i + 256;

}

unsigned int get_operator (unsigned int code)
{
    if (code < 256) return 0;
    return operators [code - 256];
}

OPE::ERR use_operator (double a, double b, unsigned int op, double &res)
{
   op = get_operator (op);
   switch (op)
   {
       case '+': res = a + b; return OPE::OK;
       case '-': res = a - b; return OPE::OK;
       case '/': res = a / b; return OPE::OK;
       case '*': res = a * b; return OPE::OK;
       default : return OPE::NOT_EXIST;
   } 

}

#endif