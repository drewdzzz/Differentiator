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

    differ.write_example (stdout);
    CalcTree differed;
    delete differed.head;
    DFE::ERR ERR_CODE = DFE::OK;
    differed.head = diff_funcs::differentiate (differ.head, 'x', ERR_CODE);
    differ.draw ("open");
    $p;
    if (ERR_CODE != DFE::OK)
        printf ("NOT OK\n");
    differed.simplify( differed.head );
    differed.write_example (stdout);
    printf ("\n");
    differed.draw ("open");



    return 0;
}