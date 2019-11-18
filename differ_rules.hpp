#ifndef INCLUDE_DIFFER_RULES
#define INCLUDE_DIFFER_RULES

#include "differ-tree.hpp"
#include <math.h>
#include "bin-tree.hpp"

struct diff_funcs 
{
    static CalcTree::Node_t* differentiate (CalcTree::Node_t *node)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;
        if (node -> data.variable)
        {
            new_node -> data.value = 1;
            return new_node;
        }
        if ( CalcTree::is_leaf (node) && ! node -> data.variable )
        {
            new_node -> data.value = 0;
            return new_node;   
        }
        if ( node -> data.op )
        {
            return operator_diff (node);
        }

    }

    static CalcTree::Node_t* operator_diff (CalcTree::Node_t *node)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;
        switch (node -> data.op)
        {
            case '+':
                new_node -> data.op = '+';
                new_node -> left  = differentiate ( node -> left  );
                new_node -> right = differentiate ( node -> right );
                return new_node;
                break;
            default: abort (); break;
        }
    }
};

#endif