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

    CalcTree differed;
    delete differed.head;
    DFE::ERR ERR_CODE = DFE::OK;
    differed.head = diff_funcs::differentiate (differ.head, ERR_CODE);
    differ.dump ("open");
    $p;
    if (ERR_CODE != DFE::OK)
        printf ("NOT OK\n");
    //differed.simplify( differed.head );
    differed.write_example (stdout);
    printf ("\n");
    differed.dump ("open");

    return 0;
}
