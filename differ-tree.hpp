#ifndef INCLUDE_DIFFER_TREE
#define INCLUDE_DIFFER_TREE

#include <cassert>
#include "bin-tree.hpp"
#include "operators.hpp"
#include "unary_functions.hpp"

static const double EPSILON    = 0.0001;
static const char*  INPUT_FILE = "diff.txt";
static const int    BUFSIZE    = 10;

bool equal (double a, double b)
{
    return fabs (a - b) < EPSILON;
}

namespace CTE
{
    enum ERR
    {
        OK,
        NOT_READ,
    } LAST_ERROR;
};

class CalcTree: public Tree_t <informative_value>
{
    char symb = 0;
    bool has_variable = false;

    virtual void define_for_draw (FILE* stream, Node_t *node, bool dump)
    {
        fprintf (stream, "\"tree_node%p\" [label = \"", node);
        write_data (stream, node -> data);
        if (dump)
            fprintf (stream," \n Address: %p\n Left: %p \n Right: %p \n Father: %p", node, node -> left, node -> right, node -> father);
        fprintf (stream,"\"]\n");

        if (node -> left)
        {
            define_for_draw (stream, node -> left, dump);
        }
        if (node -> right)
        {
            define_for_draw (stream, node -> right, dump);
        }
    }

    /*void write_undertree (FILE* stream, Node_t *node)
    {
        assert (stream);
        assert (node);

        fprintf (stream, "{ '");
        write_data(stream, node -> data);
        fprintf (stream, "' ");
        if (node -> left)
            write_undertree (stream, node -> left);
        if (node -> right)
            write_undertree (stream, node -> right);
        fprintf (stream, "}");
    }*/

    CTE::ERR read_undertree (FILE* stream, Node_t *node, char* treeInput)
    {  
        if ( fscanf (stream, " %lf ", &(node -> data.value) ) )
        {
            fscanf (stream, " %c ", &symb);
            if ( symb != ')' )
                return CTE::NOT_READ; 
            return CTE::OK;
        } 

        if ( fscanf (stream, " %[A-Za-z] ", treeInput) )
            if ( strlen (treeInput) != 1 )
            {
                if ( is_un_function (treeInput) )
                {
                    node -> data.un_func = get_un_function_code (treeInput);
                    make_right (node, {});

                    fscanf (stream, " %c ", &symb);
                    if ( symb != '(' )
                        return CTE::NOT_READ;
                    if ( read_undertree (stream, node -> right, treeInput) == CTE::NOT_READ )
                        return CTE::NOT_READ;  
                    fscanf (stream, " %c ", &symb);
                    if ( symb != ')' )
                        return CTE::NOT_READ; 
                    return CTE::OK;
                } 
            }
            else
            {
                node -> data.variable = treeInput [0];
                has_variable = true;
                fscanf (stream, " %c ", &symb);
                if ( symb != ')' )
                    return CTE::NOT_READ; 
                return CTE::OK;
            }
            

        fscanf (stream, " %c ", &symb);
        if ( symb == '(' )
        {
            make_left (node, {}); 
            if ( read_undertree (stream, node -> left, treeInput) == CTE::NOT_READ )
                return CTE::NOT_READ;
        }
        
        fscanf (stream, " %c ", &symb);
        if ( is_operator (symb) )
        {
            node -> data.op = symb;
            node -> data.priority = get_op_priority (symb);
        }
        else
        {
            return CTE::NOT_READ;
        }
        
        fscanf (stream, " %c ", &symb);
        if ( symb == '(' )
        {
             make_right (node, {}); 
             if ( read_undertree (stream, node -> right, treeInput) == CTE::NOT_READ )
                return CTE::NOT_READ;
        }
        
        fscanf (stream, " %c ", &symb);
        if ( symb == ')' ) 
            return CTE::OK;
        else
        {
            return CTE::NOT_READ; 
        }  
    }

    void write_ex_part (FILE* stream, Node_t *node)
    {        
        assert (node);
        if (! node -> left && ! node -> right )
        {
            if (node -> data.variable)
            {    
                fprintf (stream, "%c", node -> data.variable);
                return;
            }
            fprintf (stream, "%lg", node -> data.value );
            return;
        }

        bool low_priority = false;

        if ( node -> data.op )
        {
            if ( node != head )
                if ( node -> data.priority  <  node -> father -> data.priority)
                {
                    low_priority = true;
                    fprintf (stream, "( ");
                }

            write_ex_part (stream, node -> left );
            fprintf (stream, " %c ", node -> data.op);
            write_ex_part (stream, node -> right);

            if (low_priority)
                fprintf (stream, " )");
        }

        if ( node -> data.un_func )
        {
            fprintf (stream, "%s ( ", get_un_func_by_code ( node -> data.un_func ) );
            write_ex_part (stream, node -> right);
            fprintf (stream, " )");
        }
    }

public:

    OPE::ERR calculate ( Node_t *node, double &result )
    {
        if ( has_variable )
            return OPE::HAS_VARIABLE;
        if ( ! node -> left && ! node -> right )
            result = node -> data.value;
        double a = 0;
        double b = 0;
        if (node -> left)
            calculate (node -> left, a);
        if (node -> right)
            calculate (node -> right, b);

        if (node -> data.op)
            use_operator ( a, b, node -> data.op, result );

        if (node -> data.un_func)
        {
            result = use_un_func (node -> data.un_func, b);
        }
        return OPE::OK;
    }


    CTE::ERR read_tree (const char* input_file)
    {
        FILE* stream = fopen (input_file, "r");
        assert (stream);
        char begin = 0;

        fscanf (stream, " %c ", &begin);
        if ( begin != '(' )
            return CTE::NOT_READ;

        char treeInput[BUFSIZE] = {};

        CTE::ERR res = read_undertree (stream, head, treeInput);
        if (res != CTE::OK)
        {
            free_tree (head);
        }
        fclose (stream);
        return res;
    }

    void write_example (FILE* stream) 
    {
        write_ex_part (stream, head);
    }

    void kill_children (Node_t *node)
    {
        free_tree (node -> left);
        free_tree (node -> right);
        node -> left = nullptr;
        node -> right = nullptr;
    }

    bool is_leaf (Node_t *node)
    {
        return (! node -> left) && (! node -> right);
    }

    bool right_operand_is_zero (Node_t *node)
    {
        return ( is_leaf (node -> right) &&  ( ( ! node -> right -> data.variable ) && equal (node -> right -> data.value, 0) ) );
    }

    bool left_operand_is_zero (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( ! node -> left -> data.variable ) && equal (node -> left -> data.value, 0) ) );
    }

    bool right_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> right) &&  ( ( ! node -> right -> data.variable ) && equal (node -> right -> data.value, 1) ) );
    }

    bool left_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( ! node -> left -> data.variable ) && equal (node -> left -> data.value, 1) ) );
    }


    OPE::ERR simplify_unusuals (Node_t *node)
    {
        if ( node -> data.op == '/' && is_leaf ( node -> right) && equal ( node -> right -> data.value, 0) )
        {
           return OPE::DIVIDE_TO_ZERO;
        }
        if ( node -> data.op == '/' && is_leaf ( node -> right) && equal ( node -> right -> data.value, 0) )
        {
            kill_children (node);
            node -> data.value = 0;
            node -> data.op = 0;
            return OPE::OK;
        }
        if ( node -> data.op == '*' )
        {
            if (right_operand_is_zero (node) || left_operand_is_zero (node) )
            {
                kill_children (node);
                node -> data.op    = 0;
                node -> data.value = 0;
                return OPE::OK;
            }

            if (left_operand_is_one (node) )
            {
                if (node -> father -> left == node)
                {
                    node -> father -> left = node -> right; //Подвешиваем то, что справа, к левой ветке
                } 
                else
                {
                    node -> father -> right = node -> right; //Подвешиваем то, что справа, к правой ветке
                }
                delete node; 
                return OPE::OK;      
            }
            
            if (right_operand_is_one (node) )
            {
                if (node -> father -> left == node)
                {
                    node -> father -> left = node -> left; //Подвешиваем то, что слева, к левой ветке
                } 
                else
                {
                    node -> father -> right = node -> left; //Подвешиваем то, что слева, к правой ветке
                }
                delete node; 
                return OPE::OK;      
            }
            
        }
    }

    OPE::ERR simplify (Node_t *node)
    {
        if (! node -> left && ! node -> right)
            return OPE::OK;

        if ( node -> data.op )
        {
            assert (node -> left);
            assert (node -> right);

            simplify (node -> left);
            simplify (node -> right);

            if ( is_leaf (node -> left) && is_leaf (node -> right) && ! node -> left -> data.variable && ! node -> right -> data.variable )
            {
                use_operator ( node -> left -> data.value, node -> right -> data.value, node -> data.op, node -> data.value);
                node -> data.op = 0;
                kill_children (node);
                return OPE::OK;
            }
            else 
            {   
                if ( simplify_unusuals (node) != OPE::OK )
                    return OPE::DIVIDE_TO_ZERO;
                return OPE::OK;
            }
        }
        else if ( node -> data.un_func )
        {
            assert (node -> right);

            simplify (node -> right);

            if ( is_leaf (node -> right) && ! node -> right -> data.variable )
            {
                node -> data.value = use_un_func ( node -> data.un_func, node -> right -> data.value);
                node -> data.un_func = 0;
                kill_children (node);
                return OPE::OK;
            }
            else
            {
                return OPE::OK;
            } 
        }

    }


    /*void write_tree (const char* output_file)
    {
        FILE* stream = fopen (output_file, "w");
        assert (stream);
        write_undertree (stream, head);
        fclose (stream);
    }*/

};

#include "differ_rules.hpp"

#endif