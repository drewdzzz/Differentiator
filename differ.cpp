#include "differ-tree.hpp"

int main ()
{
    CalcTree differ;
    if ( differ.read_tree (INPUT_FILE) == CTE::OK )
        printf ("That's nice!\n");
    else
    {
        abort ();
    }
    
    /*differ.draw ((char*)"open");
    double result = 0;
    differ.calculate (differ.head, result);
    printf ("result: %lf\n", result);

    differ.write_example (stdout);
    $p;
    differ.simplify (differ.head);
    differ.draw ((char*)"open");
    printf ("\n");
    differ.write_example (stdout);
    printf ("\n");*/

    CalcTree differed;
    delete differed.head;
    differed.head = diff_funcs::differentiate (differ.head);
    differ.draw ("open");
    $p;
    differed.draw("open");
    return 0;
}
