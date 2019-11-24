#ifndef INCLUDE_DIFFER_TREE
#define INCLUDE_DIFFER_TREE

#include <cassert>
#include "bin-tree.hpp"
#include "operators.hpp"
#include "unary_functions.hpp"

static const double EPSILON    = 0.0001;
static const int    BUFSIZE    = 512;

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

void delete_spaces (char* input)
{
    while (*input)
    {
        if (*input == ' ')
        {
            int i = 1;
            for (i = 1; i < strlen (input) ; i++)
                input [i-1] = input [i];
            input [i-1] = 0;
        }
        else
        {
            input++;
        }
        
    }
}

class CalcTree: public Tree_t <informative_value>
{
    char symb = 0;
    bool has_variable = false;

    virtual void define_for_draw (FILE* stream, Node_t *node, bool dump)
    {
        fprintf (stream, "\"tree_node%p\" [label = \"", node);
        write_data (stream, node -> data);
        if (dump)
            fprintf (stream," \n Address: %p\n Left: %p \n Right: %p \n Father: %p \n Operator: %d \n UN_FUNC: %d \n priority: %d", node, node -> left, node -> right, node -> father, node -> data.op, node -> data.un_func, get_op_priority ( node -> data.op ) );
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

    Node_t* Get_G (char *&input)
    {
        Node_t* node = Get_E (input); 
        if ( *input == 0 )
        {
            return node;
        }
        else
        {
            printf ("%c", *input);
            free_tree (node);
            assert (false);
            return nullptr;
        }
        
    }

    Node_t* Get_E (char *&input)
    {
        Node_t* node = Get_T (input);
        Node_t* new_node = nullptr;
        while ( *input == '+' || *input == '-' )
        {
            new_node = new Node_t;

            new_node -> data.op = *input;
            new_node -> left = node;
            node -> father = new_node;
            node = new_node;
            new_node = nullptr;

            input++;

            node -> right = Get_T (input);
            node -> right -> father = node;
        }
        return node;
    }

    Node_t* Get_T (char *&input)
    {
        Node_t* node = Get_P (input);
        Node_t* new_node = nullptr;
        while ( *input == '*' || *input == '/' )
        {
            new_node = new Node_t;

            new_node -> data.op = *input;
            new_node -> left = node;
            node -> father = new_node;
            node = new_node;
            new_node = nullptr;

            input++;

            node -> right = Get_P (input);
            node -> right -> father = node;
        }
        return node;     
    }

    Node_t* Get_P (char *&input)
    {
        if ( *input == '(' )
        {
            input++;
            Node_t* node = Get_E (input);
            if ( *input == ')' )
            {
                input++;
            }
            else
            {
                assert (false);
                return nullptr;
            }
            return node;
        }
        else if ( *input >= '0' && *input <= '9' )
        {
            return Get_N (input);
        }
        else
        {
            return Get_F (input);
        }    
    }

    Node_t* Get_F (char *&input)
    {
        char letters[BUFSIZE] = {};
        int read_count = 0;
        Node_t *new_node = new Node_t;
        Node_t *expression = nullptr;

        if ( ! sscanf (input, "%[A-Za-z]%n", letters, &read_count) )
        {
            assert (false);                      
        }
        input += read_count;

        if ( is_un_function (letters) ) 
        {
            if ( *input == '(')
            {
                input++;
                expression = Get_E (input);
            }

            if ( *input == ')' )
            {
                input++;
            }
            else 
            {
                assert (false);
                return nullptr;
            }
            new_node -> data.un_func = get_un_function_code (letters);
            new_node -> right = expression;
            expression -> father = new_node;
        }
        else
        {
            new_node -> data.variable = letters[0];
        }
        return new_node;    
    }

    Node_t* Get_N (char *&input)
    {
        bool worked = false;
        double value = 0.0;
        int read_count = 0;
        if ( ! sscanf (input, "%lf%n", &value, &read_count) )
        {
            assert (false);
        }
        input += read_count;
        Node_t *new_node = new Node_t;
        new_node -> data.value = value;
        return new_node;
    }

public: 
    CTE::ERR read_tree (const char* input_file)
    {  

        char input [BUFSIZE] = {};
        char* pos = &input[0];
        scanf (" %[^\n]", input);
        delete_spaces (input);
        head = Get_G (pos);
        assert (head);
        return CTE::OK;

    }

    void write_ex_part (FILE* stream, Node_t *node)
    {        
        assert (node);
        assert (stream);

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
            assert (node -> right);
            assert (node -> left);

            if ( node != head )
            {
                assert ( node -> father );

                if ( get_op_priority (node -> data.op)  <  get_op_priority (node -> father -> data.op) )
                {
                    low_priority = true;
                    fprintf (stream, "( ");
                }
            }

            write_ex_part (stream, node -> left );
            fprintf (stream, " %c ", node -> data.op);
            write_ex_part (stream, node -> right);

            if (low_priority)
                fprintf (stream, " )");
        }

        if ( node -> data.un_func )
        {
            assert (node -> right);

            fprintf (stream, "%s ( ", get_un_func_by_code ( node -> data.un_func ) );
            write_ex_part (stream, node -> right);
            fprintf (stream, " )");
        }
    }

    void part_insert_var (Node_t *node, const char &variable, const double &var_value, long &counter)
    {
        assert (node);
        if ( node -> data.variable == variable) 
        {
            node -> data.variable = 0;
            node -> data.value = var_value;
        }

        if ( node -> left )
            part_insert_var (node -> left, variable, var_value, counter);

        if ( node -> right )
            part_insert_var (node -> right, variable, var_value, counter);
    }

    void tex_undertree (FILE* stream, Node_t *node)
    {
        assert (node);
        assert (stream);

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
            assert (node -> right);
            assert (node -> left);

            if ( node -> data.op == '/')
            {
                fprintf (stream, "\\frac"
                                 "{");
                tex_undertree (stream, node -> left);
                fprintf (stream, "}"
                                 "{");
                tex_undertree (stream, node -> right);
                fprintf (stream, "}");
            }
            else
            {            

                if ( node != head )
                {
                    assert ( node -> father );

                    if ( get_op_priority (node -> data.op)  <  get_op_priority (node -> father -> data.op) )
                    {
                        low_priority = true;
                        fprintf (stream, "( ");
                    }
                }

                if ( node -> data.op == '^' && node -> left -> data.un_func) 
                    fprintf (stream, "( ");

                tex_undertree (stream, node -> left );

                if ( node -> data.op == '^' && node -> left -> data.un_func) 
                    fprintf (stream, " )");

                fprintf (stream, " %c ", node -> data.op);
                if ( node -> data.op == '^') 
                    fprintf (stream, "{");
                tex_undertree (stream, node -> right);
                if ( node -> data.op == '^') 
                    fprintf (stream, "}");

                if (low_priority)
                    fprintf (stream, " )");
            }
        }   
       
        if ( node -> data.un_func )
        {
            assert (node -> right);

            fprintf (stream, "%s ( ", get_un_func_by_code ( node -> data.un_func ) );
            tex_undertree (stream, node -> right);
            fprintf (stream, " )");
        } 
    }

public:

    void tex_tree (FILE* stream)
    {
        assert (stream);
        assert (head);

        fprintf (stream, "$");
        tex_undertree (stream, head);
        fprintf (stream, "$");
    }

    long insert_variable (const char variable, const double var_value)
    {   
        long counter = 0;
        part_insert_var (head, variable, var_value, counter);
        return counter;
    }

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

    static bool is_leaf (Node_t *node)
    {
        return (! node -> left) && (! node -> right);
    }

    static bool right_operand_is_zero (Node_t *node)
    {
        return ( is_leaf (node -> right) &&  ( ( ! node -> right -> data.variable ) && equal (node -> right -> data.value, 0) ) );
    }

    static bool left_operand_is_zero (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( ! node -> left -> data.variable ) && equal (node -> left -> data.value, 0) ) );
    }

    static bool right_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> right) &&  ( ( ! node -> right -> data.variable ) && equal (node -> right -> data.value, 1) ) );
    }

    static bool left_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( ! node -> left -> data.variable ) && equal (node -> left -> data.value, 1) ) );
    }

    OPE::ERR simplify_tree ()
    {
        return simplify (head);
    }

private:
    OPE::ERR simplify_unusuals (Node_t *node)
    {
        if ( node -> data.op == '/' && right_operand_is_zero )
        {
           return OPE::DIVIDE_TO_ZERO;
        }
        if ( node -> data.op == '/' && left_operand_is_zero )
        {
            kill_children (node);
            node -> data.value = 0;
            node -> data.op = 0;
            return OPE::OK;
        }
        if ( node -> data.op == '+')
        { 
            if ( left_operand_is_zero (node) )
            {
                if (! node -> father)
                {
                    head = node -> right;
                }
                else if (node -> father -> left == node)
                {
                    node -> father -> left = node -> right; //Подвешиваем то, что справа, к левой ветке
                } 
                else
                {
                    node -> father -> right = node -> right; //Подвешиваем то, что справа, к правой ветке
                }
                delete node -> left;
                delete node; 
                return OPE::OK;      
            }
            
            if ( right_operand_is_zero (node) )
            {
                if (! node -> father)
                {
                    head = node -> left;
                }
                else if (node -> father -> left == node)
                {
                    node -> father -> left = node -> left; //Подвешиваем то, что слева, к левой ветке
                } 
                else
                {
                    node -> father -> right = node -> left; //Подвешиваем то, что слева, к правой ветке
                }
                delete node -> right;
                delete node; 
                return OPE::OK;      
            }
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
                if (! node -> father)
                {
                    head = node -> right;
                }
                else if (node -> father -> left == node)
                {
                    node -> father -> left = node -> right; //Подвешиваем то, что справа, к левой ветке
                    } 
                else
                {
                    node -> father -> right = node -> right; //Подвешиваем то, что справа, к правой ветке
                }
                delete node -> left;
                delete node; 
                return OPE::OK;      
            }
            
            if (right_operand_is_one (node) )
            {
                if (! node -> father)
                {
                    head = node -> left;
                }
                else if (node -> father -> left == node)
                {
                    node -> father -> left = node -> left; //Подвешиваем то, что слева, к левой ветке
                } 
                else
                {
                    node -> father -> right = node -> left; //Подвешиваем то, что слева, к правой ветке
                }
                delete node -> right;
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


};

#include "differ_rules.hpp"

#endif