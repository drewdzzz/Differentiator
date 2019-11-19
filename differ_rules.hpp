#ifndef INCLUDE_DIFFER_RULES
#define INCLUDE_DIFFER_RULES

#include "differ-tree.hpp"
#include <math.h>
#include "bin-tree.hpp"

namespace DFE
{
    enum ERR
    {
        OK,
        UNKNOWN_OP,
        UNKNOWN_UN_FUNC,
        NO_DIFFER_RULE
    };
};

inline void father_to_son (CalcTree::Node_t *father, CalcTree::Node_t *son)
{
    son -> father = father;
}

struct diff_funcs 
{
    static CalcTree::Node_t* differentiate (CalcTree::Node_t *node, DFE::ERR &err_code)
    {
        err_code = DFE::OK;

        if (node -> data.variable)
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> data.value = 1;
            return new_node;
        }
        else if ( CalcTree::is_leaf (node) && ! node -> data.variable )
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> data.value = 0;
            return new_node;   
        }
        else if ( node -> data.op )
        {
            return operator_diff (node, err_code);
        }
        else if ( node -> data.un_func )
        {
            return unary_function_diff (node, err_code);
        }
        else 
        {
            err_code = DFE::NO_DIFFER_RULE;
        }


    }

    static CalcTree::Node_t* operator_diff (CalcTree::Node_t *node, DFE::ERR &err_code)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;

        switch (node -> data.op)
        {
            case '+':
                new_node -> data.op = '+';

                new_node -> left  = differentiate ( node -> left, err_code  );
                    father_to_son ( new_node, new_node -> left );

                new_node -> right = differentiate ( node -> right, err_code );
                    father_to_son ( new_node, new_node -> right );

                break;

            case '-':
                new_node -> data.op = '-';

                new_node -> left  = differentiate ( node -> left, err_code  );
                    father_to_son ( new_node,   new_node -> left );

                new_node -> right = differentiate ( node -> right, err_code );
                    father_to_son ( new_node,   new_node -> right );

                break;

            case '*':
                new_node -> data.op = '+';

                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> data.op = '*';
                    father_to_son ( new_node,   new_node -> left );

                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> data.op = '*';
                    father_to_son ( new_node,   new_node -> right );

                new_node -> left -> left = differentiate (node -> left, err_code);
                    father_to_son ( new_node -> left,   new_node -> left -> left );

                new_node -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> left,   new_node -> left -> right );

                new_node -> right -> right = differentiate (node -> right, err_code);
                    father_to_son ( new_node -> right,   new_node -> right -> right );

                new_node -> right -> left = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( new_node -> right,   new_node -> right -> left );

                break;

            case '/':
                new_node -> data.op = '/';

                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> data.op = '^';
                    father_to_son ( new_node,   new_node -> right );

                new_node -> right -> right = new CalcTree::Node_t;
                    new_node -> right -> right -> data.value = 2;
                    father_to_son ( new_node -> right,   new_node -> right -> right );

                new_node -> right -> left = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> right,   new_node -> right -> left );

                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> data.op = '-';
                    father_to_son ( new_node,   new_node -> left );
                
                new_node -> left -> right = new CalcTree::Node_t;
                    new_node -> left -> right -> data.op = '*';
                    father_to_son ( new_node -> left,   new_node -> left -> right );

                new_node -> left -> left = new CalcTree::Node_t;
                    new_node -> left -> left -> data.op = '*';
                    father_to_son ( new_node -> left,   new_node -> left -> left );

                new_node -> left -> right -> right = differentiate (node -> right, err_code);
                    father_to_son ( new_node -> left -> right,   new_node -> left -> right -> right );

                new_node -> left -> right -> left  = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( new_node -> left -> right,   new_node -> left -> right -> left );

                new_node -> left -> left -> left  = differentiate (node -> left,   err_code);
                    father_to_son ( new_node -> left -> left,   new_node -> left -> left -> left );

                new_node -> left -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> left -> left,   new_node -> left -> left -> right );

                break;

            case '^':
            {
                char* ln = strdup ("ln");
                assert (ln);

                new_node -> data.op = '*';
                new_node -> right = new CalcTree::Node_t ( *node );
                    father_to_son ( new_node,   new_node -> right );

                CalcTree::Node_t *left_part = new CalcTree::Node_t; 
                assert (left_part); 

                left_part -> data.op = '*';
                
                left_part -> left = new CalcTree::Node_t ( *(node -> right) ); 
                    father_to_son ( left_part,   left_part -> left );

                left_part -> right = new CalcTree::Node_t;
                    left_part -> right -> data.un_func = get_un_function_code (ln);     
                    father_to_son ( left_part,   left_part -> right );

                left_part -> right -> right = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( left_part -> right,   left_part -> right -> right );

                new_node -> left = differentiate (left_part, err_code);
                    father_to_son ( new_node,   new_node -> left );

                CalcTree::free_tree (left_part);

                free (ln);
                break;
            }
            default: 
                delete new_node; 
                new_node = nullptr; 
                err_code = DFE::UNKNOWN_OP; 
                break;
        }
        return new_node;
    }

    static CalcTree::Node_t* unary_function_diff (CalcTree::Node_t *node, DFE::ERR &err_code)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;

        if ( is_this_un_func (node -> data.un_func, "ln") )
        {
            new_node -> data.op = '/';

            new_node -> left  = differentiate (node -> right, err_code);
                father_to_son (new_node,   new_node -> left);

            new_node -> right = new CalcTree::Node_t ( *(node -> right) ); 
                father_to_son (new_node,   new_node -> right);
        }
        else if ( is_this_un_func (node -> data.un_func, "sin") )
        {
            char* cos = strdup ("cos");

            new_node -> data.op = '*';

            new_node -> left = new CalcTree::Node_t;
                new_node -> left -> data.un_func = get_un_function_code (cos);
                father_to_son ( new_node,   new_node -> left);

            new_node -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                father_to_son ( new_node -> left,   new_node -> left -> right );

            new_node -> right = differentiate (node -> right, err_code);
                father_to_son ( new_node,   new_node -> right );

            free (cos);
        }
        else if ( is_this_un_func (node -> data.un_func, "cos") )
        {
            char* sin = strdup ("sin");

            new_node -> data.op = '*';

            new_node -> left = new CalcTree::Node_t;
                new_node -> left -> data.op = '*';
                father_to_son ( new_node,   new_node -> left );

            new_node -> left -> left = new CalcTree::Node_t;
                new_node -> left -> left -> data.value = -1;
                father_to_son ( new_node -> left,   new_node -> left -> left );

            new_node -> left -> right = new CalcTree::Node_t;
                new_node -> left -> right -> data.un_func = get_un_function_code (sin);
                father_to_son ( new_node -> left,   new_node -> left -> right );
            
            new_node -> left -> right -> right = new CalcTree::Node_t ( *(node -> right) );
                father_to_son ( new_node -> left -> right,   new_node -> left -> right -> right );

            new_node -> right = differentiate ( node -> right,   err_code );
                father_to_son ( new_node,   new_node -> right );

            free (sin);
        }
        else
        {
            err_code = DFE::UNKNOWN_UN_FUNC;
            delete new_node;
            new_node = nullptr;
        }       

        return new_node;
    }

};

#endif