#ifndef INCLUDE_DIFFER_RULES
#define INCLUDE_DIFFER_RULES

#include "differ-tree.hpp"
#include <math.h>
#include "bin-tree.hpp"

namespace diff_funcs 
{
    CalcTree::Node_t* differentiate (CalcTree::Node_t *node)
    {
        if (node -> data.variable)
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> data.value = 1;
            return new_node;
        }
    }
};

#endif