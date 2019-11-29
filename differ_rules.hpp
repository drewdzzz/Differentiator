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

class diff_funcs 
{
public:
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

private:

    static CalcTree::Node_t* operator_diff (CalcTree::Node_t *node, const char &diff_var, DFE::ERR &err_code)
    {
        switch (node -> node_data.data.code)
        {
            case '+':
                return ( * differentiate ( node -> left, diff_var, err_code  ) ) + ( * differentiate ( node -> right, diff_var, err_code ) );

            case '-':
                return ( * differentiate ( node -> left, diff_var, err_code  ) ) - ( * differentiate ( node -> right, diff_var, err_code ) );

            case '*':
            {
                CalcTree::Node_t * left_node  = ( * differentiate (node -> left, diff_var, err_code) ) * ( * new CalcTree::Node_t ( * (node -> right) ) );
                CalcTree::Node_t * right_node = ( * differentiate (node -> right, diff_var, err_code) ) * ( * new CalcTree::Node_t ( * (node -> left) ) );

                return ( * left_node ) + ( * right_node );
            }

            case '/':
            {
                CalcTree::Node_t *left_left  = ( * new CalcTree::Node_t ( *(node -> right) ) ) *  ( * differentiate (node -> left, diff_var,  err_code) );
                CalcTree::Node_t *left_right = ( * new CalcTree::Node_t ( *(node -> left) ) )  *  ( * differentiate (node -> right, diff_var, err_code) );
                CalcTree::Node_t *left       = ( * left_left ) - ( * left_right );
                
                CalcTree::Node_t *right      = ( * new CalcTree::Node_t ( *(node -> right) ) ) ^ ( * new CalcTree::Node_t (2.0) );

                return ( * left ) / ( * right );
            }

            case '^':
            {
                if ( node -> right -> node_data.type == QUANTITY)
                {
                    CalcTree::Node_t *right_part = ( * new CalcTree::Node_t ( * node -> left ) ) ^ ( * new CalcTree::Node_t ( node -> right -> node_data.data.value - 1.0 ) );
                    CalcTree::Node_t *first_diff = ( * new CalcTree::Node_t ( * node -> right) ) * ( * right_part );
                    return ( * differentiate (node -> left, diff_var, err_code) ) * ( * first_diff );
                }
                else
                {
                    CalcTree::Node_t *log = make_un_function ("ln", node -> left);
                    CalcTree::Node_t *left_part = ( * new CalcTree::Node_t ( *(node -> right) ) ) * ( * log ); 
                    CalcTree::Node_t *new_node = ( * differentiate (left_part, diff_var, err_code) ) * ( * new CalcTree::Node_t ( *node ) );

                    CalcTree::free_tree (left_part);
                    return new_node;
                }
            }
            default: 
                err_code = DFE::UNKNOWN_OP; 
                return nullptr;
                break;
        }
    }

    static CalcTree::Node_t* unary_function_diff (CalcTree::Node_t *node, const char &diff_var, DFE::ERR &err_code)
    {

        if ( is_this_un_func (node -> node_data.data.code, "ln") )
        {
            return ( * differentiate (node -> right, diff_var, err_code) ) / ( * new CalcTree::Node_t ( *(node -> right) ) );
        }
        else if ( is_this_un_func (node -> node_data.data.code, "sin") )
        {
            CalcTree::Node_t *cos = make_un_function ("cos", node -> right);

            return ( * cos ) * ( * differentiate (node -> right, diff_var, err_code) );
        }
        else if ( is_this_un_func (node -> node_data.data.code, "cos") )
        {
            CalcTree::Node_t *left_part =  ( * new CalcTree::Node_t (-1.0) ) * ( * make_un_function ("sin", (node -> right) ) );

            return ( * left_part ) * ( * differentiate ( node -> right, diff_var, err_code ) );
        }
        else
        {
            err_code = DFE::UNKNOWN_UN_FUNC;
            return nullptr;
        }       

        assert (false);
    }

    static CalcTree::Node_t* make_un_function (const char* function, const CalcTree::Node_t* argument)
    {
        assert ( is_un_function (function) );
        CalcTree::Node_t *func = new CalcTree::Node_t;
        func -> node_data.type = UN_FUNCTION;
        func -> node_data.data.code = get_un_function_code ( function );

        func -> right = new CalcTree::Node_t ( *argument );
        father_to_son ( func,   func -> right );
        return func;
    }

};

#endif