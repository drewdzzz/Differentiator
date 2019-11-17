#ifndef CALC_TREE_UNARY_FUNCTIONS_INCLUDED
#define CALC_TREE_UNARY_FUNCTIONS_INCLUDED

#include <math.h>
#include <string.h>
#include <cassert>

static const char* un_functions[] = {"THAT'S AN EMPTY SPACE!!!!",
                                  "sin",
                                  "cos",
                                  "ln"};

bool is_this_un_func (char code, const char* func)
{   
    if ( ! strcmp ( un_functions [code], func ) )
        return true;
    else
        return false;  
}

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

char* get_un_func_by_code (char code)
{
    assert ( code < sizeof (un_functions) );
    char* res = strdup (un_functions[code]);
    return res;
}

double use_un_func (char func_code, double value)
{

    if (is_this_un_func (func_code, "sin") )
        return sin (value);
    if (is_this_un_func (func_code, "cos") )
        return cos (value);
    if (is_this_un_func (func_code, "ln") )
        return log (value);

    assert (false);
    return 0;
}

#endif