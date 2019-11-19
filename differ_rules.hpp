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
            case '-':
                new_node -> data.op = '-';
                new_node -> left  = differentiate ( node -> left  );
                new_node -> right = differentiate ( node -> right );
                return new_node;
            case '*':
                new_node -> data.op = '+';
                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> data.op = '*';
                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> data.op = '*';

                new_node -> left -> left = differentiate (node -> left);
                new_node -> left -> right = new CalcTree::Node_t ( *(node -> right) );

                new_node -> right -> right = differentiate (node -> right);
                new_node -> right -> left = new CalcTree::Node_t ( *(node -> left) );
                return new_node;
                break;
            case '/':
                new_node -> data.op = '/';
                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> data.op = '^';

                new_node -> right -> right = new CalcTree::Node_t;
                    new_node -> right -> right -> data.value = 2;
                new_node -> right -> left = new CalcTree::Node_t ( *(node -> right) );

                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> data.op = '-';
                
                new_node -> left -> right = new CalcTree::Node_t;
                    new_node -> left -> right -> data.op = '*';

                new_node -> left -> left = new CalcTree::Node_t;
                    new_node -> left -> left -> data.op = '*';

                new_node -> left -> right -> right = differentiate (node -> right);
                new_node -> left -> right -> left  = new CalcTree::Node_t ( *(node -> left) );

                new_node -> left -> left -> left  = differentiate (node -> left);
                new_node -> left -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                return new_node;
                break;
            default: delete new_node; abort (); break;
        }
    }


};

#endif