#ifndef CALC_TREE_UNARY_FUNCTIONS_INCLUDED
#define CALC_TREE_UNARY_FUNCTIONS_INCLUDED

#include <math.h>
#include <string.h>
#include <cassert>

static char* un_functions[] = {"THAT'S AN EMPTY SPACE!!!!",
                                  "sin",
                                  "cos",
                                  "ln"};

bool is_un_function (char* potential_func)
{
    for (char i = 0; i < sizeof (un_functions); i++)
        if ( ! strcmp (un_functions[i], potential_func) )
            return true;
    
    return false;
}

char get_un_function_code (char* func)
{
    for (char i = 0; i < sizeof (un_functions); i++)
    {
        if ( ! strcmp (un_functions[i], func) )
        {
            return i;
        }
    }

    assert (false/*USE bool is_function*/);
    return 0;
}

const char* get_un_func_by_code (char code)
{
    assert ( code < sizeof (un_functions) );
    return un_functions[code];
}

double use_un_function (char* func)
{}

#endif