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
    if (ERR_CODE != DFE::OK)
        printf ("OT OK\n");
    differ.draw ("open");
    $p;
    differed.draw("open");
    $p;
    differed.simplify(differed.head);
    differed.draw("open");
    $p;

    return 0;
}
