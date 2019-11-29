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
        write_data (stream, node -> node_data);
        if (dump)
            fprintf (stream," \n Address: %p\n Left: %p \n Right: %p \n Father: %p\n code: %d\n type: %d",
                     node, node -> left, node -> right, node -> father, node -> node_data.data.code, node -> node_data.type );
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

    Node_t* Get_G (char *&input, char &error)
    {
        Node_t* node = Get_E (input); 
        if ( *input == 0 )
        {
            error = 0;
            return node;
        }
        else
        {
            free_tree (node);
            error = *input;
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

            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = *input;

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
        Node_t* node = Get_K (input);
        Node_t* new_node = nullptr;
        while ( *input == '*' || *input == '/')
        {
            new_node = new Node_t;

            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = *input;

            new_node -> left = node;
            node -> father = new_node;
            node = new_node;
            new_node = nullptr;

            input++;

            node -> right = Get_K (input);
            node -> right -> father = node;
        }
        return node;     
    }

    Node_t* Get_K (char *&input)
    {
        Node_t* node = Get_P (input);
        Node_t* new_node = nullptr;
        while ( *input == '^')
        {
            new_node = new Node_t;

            new_node -> node_data.type = OPERATOR;
            new_node -> node_data.data.code = *input;

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
        else if ( (*input >= '0' && *input <= '9') || (*input == '-') )
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

            new_node -> node_data.type = UN_FUNCTION;
            new_node -> node_data.data.code = get_un_function_code (letters);
            new_node -> right = expression;
            expression -> father = new_node;
        }
        else
        {
            new_node -> node_data.type = VARIABLE;
            new_node -> node_data.data.code = letters[0];
        }
        return new_node;    
    }

    Node_t* Get_N (char *&input)
    {
        double value = 0.0;
        int read_count = 0;
        if ( ! sscanf (input, "%lf%n", &value, &read_count) )
        {
            assert (false);
        }
        input += read_count;
        Node_t *new_node = new Node_t;

        new_node -> node_data.data.code = QUANTITY;
        new_node -> node_data.data.value = value;
        return new_node;
    }



public: 
    char read_tree ()
    {  

        char input [BUFSIZE] = {};
        char* pos = &input[0];
        char err_symb = 0;
        scanf (" %[^\n]", input);
        delete_spaces (input);
        free_tree (head);
        head = Get_G (pos, err_symb);
        if (err_symb != 0)
            return err_symb;
        return 0;

    }

    void part_insert_var (Node_t *node, const char &variable, const double &var_value, long &counter)
    {
        assert (node);
        if ( node -> node_data.data.code == variable && node -> node_data.type == VARIABLE ) 
        {
            node -> node_data.type = QUANTITY;
            node -> node_data.data.value = var_value;
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

        if (node -> node_data.type == VARIABLE)
        {    
            fprintf (stream, "%c", node -> node_data.data.code);
            return;
        }
        if (node -> node_data.type == QUANTITY)
        {
            fprintf (stream, "%lg", node -> node_data.data.value );
            return;
        }

        bool low_priority = false;


        if ( node -> node_data.type == OPERATOR )
        {
            assert (node -> right);
            assert (node -> left);

            if ( node -> node_data.data.code == '/')
            {
                fprintf (stream, "\\cfrac"
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

                    if ( get_op_priority (node -> node_data.data.code)  <  get_op_priority (node -> father  -> node_data.data.code) )
                    {
                        low_priority = true;
                        fprintf (stream, "\\left( ");
                    }
                }

                if ( node -> node_data.data.code == '^' && ( node -> left -> node_data.type == UN_FUNCTION || node -> left -> node_data.data.code == '^') ) 
                    fprintf (stream, "\\left( ");

                tex_undertree (stream, node -> left );

                if (node -> node_data.data.code == '^' && ( node -> left -> node_data.type == UN_FUNCTION || node -> left -> node_data.data.code == '^') ) 
                    fprintf (stream, " \\right)");

                if ( node -> node_data.data.code == '*')
                    fprintf (stream, " \\cdot ");
                else
                    fprintf (stream, " %c ", node -> node_data.data.code);
                if ( node -> node_data.data.code == '^') 
                    fprintf (stream, "{");
                tex_undertree (stream, node -> right);
                if ( node -> node_data.data.code == '^') 
                    fprintf (stream, "}");

                if (low_priority)
                    fprintf (stream, " \\right)");
            }
        }   
       
        if ( node -> node_data.type == UN_FUNCTION )
        {
            assert (node -> right);

            fprintf (stream, "%s \\left( ", get_un_func_by_code ( node -> node_data.data.code ) );
            tex_undertree (stream, node -> right);
            fprintf (stream, " \\right)");
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
        if ( node -> node_data.type == QUANTITY )
            result = node -> node_data.data.value;
        double a = 0;
        double b = 0;

        if (node -> left)
            calculate (node -> left, a);
        if (node -> right)
            calculate (node -> right, b);

        if (node -> node_data.type == OPERATOR)
            use_operator ( a, b, node -> node_data.data.code, result );

        if (node -> node_data.type == UN_FUNCTION)
        {
            result = use_un_func (node -> node_data.data.code, b);
        }
        return OPE::OK;
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
        return ( is_leaf (node -> right) &&  ( ( node -> right -> node_data.type == QUANTITY ) && equal (node -> right -> node_data.data.value, 0) ) );
    }

    static bool left_operand_is_zero (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( node -> left -> node_data.type == QUANTITY ) && equal (node -> left -> node_data.data.value, 0) ) );
    }

    static bool right_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> right) &&  ( ( node -> right -> node_data.type == QUANTITY ) && equal (node -> right -> node_data.data.value, 1) ) );
    }

    static bool left_operand_is_one (Node_t *node)
    {
        return ( is_leaf (node -> left) &&  ( ( node -> left -> node_data.type == QUANTITY ) && equal (node -> left -> node_data.data.value, 1) ) );
    }

    OPE::ERR simplify_tree ()
    {
        return simplify (head);
    }

private:


//Simplifying functions


    OPE::ERR simplify_unusuals (Node_t *node)
    {
        if ( (node -> node_data.type == OPERATOR)  &&  (node -> node_data.data.code == '/')  &&  (right_operand_is_zero) )
        {
           return OPE::DIVIDE_TO_ZERO;
        }
        if ( (node -> node_data.type == OPERATOR)  &&  (node -> node_data.data.code == '/')  &&  (left_operand_is_zero) )
        {
            kill_children (node);
            node -> node_data.data.value = 0.0;
            node -> node_data.type = QUANTITY;
            return OPE::OK;
        }
        if ( node -> node_data.type == OPERATOR && node -> node_data.data.code == '+')
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
        if ( node -> node_data.type == OPERATOR && node -> node_data.data.code == '-')
        {
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
        if ( node -> node_data.type == OPERATOR && node -> node_data.data.code == '*' )
        {
            if (right_operand_is_zero (node) || left_operand_is_zero (node) )
            {
                kill_children (node);
                node -> node_data.type = QUANTITY;
                node -> node_data.data.value = 0.0;
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
        if ( node -> node_data.type == QUANTITY || node -> node_data.type == VARIABLE )
            return OPE::OK;

        if ( node -> node_data.type == OPERATOR )
        {
            assert (node -> left);
            assert (node -> right);

            simplify (node -> left);
            simplify (node -> right);

            if ( node -> left -> node_data.type == QUANTITY  &&  node -> right -> node_data.type == QUANTITY )
            {
                use_operator ( node -> left -> node_data.data.value, node -> right -> node_data.data.value, node -> node_data.data.code, node -> node_data.data.value);
                node -> node_data.type = QUANTITY;
                kill_children (node);
                return OPE::OK;
            }
            else 
            {   
                if ( simplify_unusuals (node) == OPE::DIVIDE_TO_ZERO )
                    return OPE::DIVIDE_TO_ZERO;
                return OPE::OK;
            }
        }
        else if ( node -> node_data.type == UN_FUNCTION )
        {

            assert (node -> right);

            simplify (node -> right);

            if ( node -> right -> node_data.type == QUANTITY )
            {
                node -> node_data.type = QUANTITY;
                node -> node_data.data.value = use_un_func ( node -> node_data.data.code, node -> right -> node_data.data.value);
                kill_children (node);
            }
            return OPE::OK;
        }

    }


};

#include "differ_rules.hpp"

#endif