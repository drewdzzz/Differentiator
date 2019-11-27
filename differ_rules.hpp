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

    static CalcTree::Node_t* differentiate (CalcTree::Node_t *node, const char &diff_var, DFE::ERR &err_code)
    {

        if (node -> node_data.type == VARIABLE)
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            if ( node -> node_data.data.code == diff_var )
            {
                new_node -> node_data.type = QUANTITY;
                new_node -> node_data.data.value = 1;
            }
            else
            {
                new_node -> node_data.type = QUANTITY;
                new_node -> node_data.data.value = 0;
            }
            
            return new_node;
        }
        else if ( node -> node_data.type == QUANTITY )
        {
            CalcTree::Node_t *new_node = new CalcTree::Node_t;
            new_node -> node_data.type = QUANTITY;
            new_node -> node_data.data.value = 0;
            return new_node;   
        }
        else if ( node -> node_data.type == OPERATOR )
        {
            return operator_diff (node, diff_var, err_code);
        }
        else if ( node -> node_data.type == UN_FUNCTION )
        {
            return unary_function_diff (node, diff_var, err_code);
        }
        else 
        {
            err_code = DFE::NO_DIFFER_RULE;
            assert (false);
            return nullptr;
        }


    }

    static CalcTree::Node_t* operator_diff (CalcTree::Node_t *node, const char &diff_var, DFE::ERR &err_code)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;

        switch (node -> node_data.data.code)
        {
            case '+':
                new_node -> node_data.type = OPERATOR;
                new_node -> node_data.data.code = '+';

                new_node -> left  = differentiate ( node -> left, diff_var, err_code  );
                    father_to_son ( new_node, new_node -> left );

                new_node -> right = differentiate ( node -> right, diff_var, err_code );
                    father_to_son ( new_node, new_node -> right );

                break;

            case '-':
                new_node -> node_data.type = OPERATOR;
                new_node -> node_data.data.code = '-';

                new_node -> left  = differentiate ( node -> left, diff_var, err_code  );
                    father_to_son ( new_node,   new_node -> left );

                new_node -> right = differentiate ( node -> right, diff_var, err_code );
                    father_to_son ( new_node,   new_node -> right );

                break;

            case '*':
                new_node -> node_data.type = OPERATOR;
                new_node -> node_data.data.code = '+';

                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> node_data.type = OPERATOR;
                    new_node -> left -> node_data.data.code = '*';
                    father_to_son ( new_node,   new_node -> left );

                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> node_data.type = OPERATOR;
                    new_node -> right -> node_data.data.code = '*';
                    father_to_son ( new_node,   new_node -> right );

                new_node -> left -> left = differentiate (node -> left, diff_var, err_code);
                    father_to_son ( new_node -> left,   new_node -> left -> left );

                new_node -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> left,   new_node -> left -> right );

                new_node -> right -> right = differentiate (node -> right, diff_var, err_code);
                    father_to_son ( new_node -> right,   new_node -> right -> right );

                new_node -> right -> left = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( new_node -> right,   new_node -> right -> left );

                break;

            case '/':
                new_node -> node_data.type = OPERATOR;
                new_node -> node_data.data.code = '/';

                new_node -> right = new CalcTree::Node_t;
                    new_node -> right -> node_data.type = OPERATOR;
                    new_node -> right -> node_data.data.code = '^';
                    father_to_son ( new_node,   new_node -> right );

                new_node -> right -> right = new CalcTree::Node_t;
                    new_node -> right -> right -> node_data.type = QUANTITY;
                    new_node -> right -> right -> node_data.data.value = 2.0;
                    father_to_son ( new_node -> right,   new_node -> right -> right );

                new_node -> right -> left = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> right,   new_node -> right -> left );

                new_node -> left = new CalcTree::Node_t;
                    new_node -> left -> node_data.type = OPERATOR;
                    new_node -> left -> node_data.data.code = '-';
                    father_to_son ( new_node,   new_node -> left );
                
                new_node -> left -> right = new CalcTree::Node_t;
                    new_node -> left -> right -> node_data.type = OPERATOR;
                    new_node -> left -> right -> node_data.data.code = '*';
                    father_to_son ( new_node -> left,   new_node -> left -> right );

                new_node -> left -> left = new CalcTree::Node_t;
                    new_node -> left -> left -> node_data.type = OPERATOR;
                    new_node -> left -> left -> node_data.data.code = '*';
                    father_to_son ( new_node -> left,   new_node -> left -> left );

                new_node -> left -> right -> right = differentiate (node -> right, diff_var, err_code);
                    father_to_son ( new_node -> left -> right,   new_node -> left -> right -> right );

                new_node -> left -> right -> left  = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( new_node -> left -> right,   new_node -> left -> right -> left );

                new_node -> left -> left -> left  = differentiate (node -> left, diff_var,  err_code);
                    father_to_son ( new_node -> left -> left,   new_node -> left -> left -> left );

                new_node -> left -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                    father_to_son ( new_node -> left -> left,   new_node -> left -> left -> right );

                break;

            case '^':
            {

                new_node -> node_data.type = OPERATOR;
                new_node -> node_data.data.code = '*';
                new_node -> right = new CalcTree::Node_t ( *node );
                    father_to_son ( new_node,   new_node -> right );

                CalcTree::Node_t *left_part = new CalcTree::Node_t; 
                assert (left_part); 

                left_part -> node_data.type = OPERATOR;
                left_part -> node_data.data.code = '*';
                
                left_part -> left = new CalcTree::Node_t ( *(node -> right) ); 
                    father_to_son ( left_part,   left_part -> left );

                left_part -> right = new CalcTree::Node_t;
                    left_part -> right -> node_data.type = UN_FUNCTION;
                    left_part -> right -> node_data.data.code = get_un_function_code ( "ln" );     
                    father_to_son ( left_part,   left_part -> right );

                left_part -> right -> right = new CalcTree::Node_t ( *(node -> left) );
                    father_to_son ( left_part -> right,   left_part -> right -> right );

                new_node -> left = differentiate (left_part, diff_var, err_code);
                    father_to_son ( new_node,   new_node -> left );

                CalcTree::free_tree (left_part);

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

    static CalcTree::Node_t* unary_function_diff (CalcTree::Node_t *node, const char &diff_var, DFE::ERR &err_code)
    {
        CalcTree::Node_t *new_node = new CalcTree::Node_t;

        if ( is_this_un_func (node -> node_data.data.code, "ln") )
        {
            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '/';

            new_node -> left  = differentiate (node -> right, diff_var, err_code);
                father_to_son (new_node,   new_node -> left);

            new_node -> right = new CalcTree::Node_t ( *(node -> right) ); 
                father_to_son (new_node,   new_node -> right);
        }
        else if ( is_this_un_func (node -> node_data.data.code, "sin") )
        {

            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = '*';

            new_node -> left = new CalcTree::Node_t;
                new_node -> left -> node_data.type = UN_FUNCTION;
                new_node -> left -> node_data.data.code = get_un_function_code ( "cos" );
                father_to_son ( new_node,   new_node -> left);

            new_node -> left -> right = new CalcTree::Node_t ( *(node -> right) );
                father_to_son ( new_node -> left,   new_node -> left -> right );

            new_node -> right = differentiate (node -> right, diff_var, err_code);
                father_to_son ( new_node,   new_node -> right );

        }
        else if ( is_this_un_func (node -> node_data.data.code, "cos") )
        {

            new_node -> node_data.data.code = '*';

            new_node -> left = new CalcTree::Node_t;
                new_node -> left -> node_data.type = OPERATOR;
                new_node -> left -> node_data.data.code = '*';
                father_to_son ( new_node,   new_node -> left );

            new_node -> left -> left = new CalcTree::Node_t;
                new_node -> left -> left -> node_data.type = QUANTITY;
                new_node -> left -> left -> node_data.data.value = -1;
                father_to_son ( new_node -> left,   new_node -> left -> left );

            new_node -> left -> right = new CalcTree::Node_t;
                new_node -> left -> right -> node_data.type = UN_FUNCTION;
                new_node -> left -> right -> node_data.data.code = get_un_function_code ( "sin" );
                father_to_son ( new_node -> left,   new_node -> left -> right );
            
            new_node -> left -> right -> right = new CalcTree::Node_t ( *(node -> right) );
                father_to_son ( new_node -> left -> right,   new_node -> left -> right -> right );

            new_node -> right = differentiate ( node -> right, diff_var, err_code );
                father_to_son ( new_node,   new_node -> right );

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