#include "differ-tree.hpp"

const char*  INPUT_FILE = "diff.txt";

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
    $p;
    differed.simplify(differed.head);
    differed.draw("open");
    $p;
    /*CalcTree differed_twice;
    delete differed_twice.head;
    differed_twice.head = diff_funcs::differentiate (differed.head);
    differed_twice.draw("open");*/

    return 0;
}
