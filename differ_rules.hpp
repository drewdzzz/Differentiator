#ifndef INCLUDE_DIFFER_RULES
#define INCLUDE_DIFFER_RULES

#include "differ-tree.hpp"
#include <math.h>
#include "bin-tree.hpp"

struct diff_funcs 
{
    static CalcTree::Node_t* differentiate (CalcTree::Node_t *node)
    {
        if (node -> data.variable)
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> data.value = 1;
            return new_node;
        }
        if ( CalcTree::is_leaf (node) && ! node -> data.variable )
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> data.value = 0;
            return new_node;   
        }
        if ( node -> data.op )
        {
            return operator_diff (node);
        }
        if ( node -> data.un_func )
        {
            return unary_function_diff (node);
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
                break;

            case '-':
                new_node -> data.op = '-';
                new_node -> left  = differentiate ( node -> left  );
                new_node -> right = differentiate ( node -> right );
                break;

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
                break;

            case '^':
            {
                char* ln = strdup ("ln");
                assert (ln);

                new_node -> data.op = '*';
                new_node -> right = new CalcTree::Node_t ( *node );

                CalcTree::Node_t *left_part = new CalcTree::Node_t; 
                assert (left_part); 

                left_part -> data.op = '*';
                left_part -> left = new CalcTree::Node_t ( *(node -> right) ); 
                left_part -> right = new CalcTree::Node_t;
                    left_part -> right -> data.un_func = get_un_function_code (ln);     
                left_part -> right -> right = new CalcTree::Node_t ( *(node -> left) );

                new_node -> left = differentiate (left_part);

                CalcTree::free_tree (left_part);

                free (ln);
                break;
            }
            default: delete new_node; new_node = nullptr; exit(228); break;
        }
        return new_node;
    }

    static CalcTree::Node_t* unary_function_diff (CalcTree::Node_t *node)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;
        if ( is_this_un_func (node -> data.un_func, "ln") )
        {
            new_node -> data.op = '/';
            new_node -> left  = differentiate (node -> right);
            new_node -> right = new CalcTree::Node_t ( *(node -> right) ); 
        }
        else
        {    
            exit(1488);
        }
        return new_node;
    }

};

#endif