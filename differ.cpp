#include <cassert>
#include "bin-tree.hpp"
#include "operators.hpp"

const double EPSILON = 0.00001;
const char* INPUT_FILE = "diff.txt";


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
public:

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

    CTE::ERR read_undertree (FILE* stream, Node_t *node)
    {   
        if ( fscanf (stream, " %lf ", &(node -> data.value) ) )
        {
            fscanf (stream, " %c ", &symb);
            if ( symb != ')' )
                return CTE::NOT_READ; 
            return CTE::OK;
        }

        fscanf (stream, " %c ", &symb);
        if ( symb == '(' )
        {
            make_left (node, {}); 
            if ( read_undertree (stream, node -> left) == CTE::NOT_READ )
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
             if ( read_undertree (stream, node -> right) == CTE::NOT_READ )
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
        if (! node -> data.op )
        {
            fprintf (stream, "%lg", node -> data.value );
            return;
        }
        bool low_priority = false;
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

    double calculate ( Node_t *node )
    {
        if ( ! node -> data.op )
            return node -> data.value;

        double a = calculate (node -> left);
        double b = calculate (node -> right);
        double result = 0;
        use_operator ( a, b, node -> data.op, result );
        return result;
    }

public:


    CTE::ERR read_tree (const char* input_file)
    {
        FILE* stream = fopen (input_file, "r");
        assert (stream);
        char begin = 0;

        fscanf (stream, " %c ", &begin);
        if ( begin != '(' )
            return CTE::NOT_READ;

        CTE::ERR res = read_undertree (stream, head);
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

    /*void write_tree (const char* output_file)
    {
        FILE* stream = fopen (output_file, "w");
        assert (stream);
        write_undertree (stream, head);
        fclose (stream);
    }*/

};



int main ()
{
    CalcTree differ;
    if ( differ.read_tree (INPUT_FILE) == CTE::OK )
        printf ("That's nice!\n");
    else
    {
        abort ();
    }
    
    differ.draw ((char*)"open");
    double result = differ.calculate (differ.head);
    printf ("%lg\n", result);
    differ.write_example (stdout);
    printf ("\n");
    return 0;
}
