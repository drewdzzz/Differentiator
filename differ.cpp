#include <cassert>
#include "bin-tree.hpp"
#include "operators.hpp"

const double EPSILON = 0.00001;
const char* INPUT_FILE = "diff.txt";

//Если op >= 128, то это оператор
/*struct informative_value
{
    double value;
    unsigned int op;
};*/

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
            fprintf (stream," \n Address: %p\n Left: %p \n Right: %p", node, node -> left, node -> right);
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
            fscanf (stream, " ");
            fscanf (stream, ")");
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
            node -> data.op = get_operator_number (symb);

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
            return CTE::NOT_READ;           
    }

    double calculate ( Node_t *node )
    {
        if ( ( node -> data.op ) < 256 )
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

        fscanf (stream, " %c ", &begin);
        if ( begin != ')' )
            return CTE::NOT_READ;

        fclose (stream);
        return res;
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
    differ.draw ("open");
    double result = differ.calculate (differ.head);
    printf ("%lf\n", result);
    return 0;
}
