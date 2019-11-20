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
    $p;
    DFE::ERR err_code;
    CalcTree differed = diff_funcs::differentiate_tree (differ, 'x', err_code );

    if (err_code != DFE::OK)
        printf ("SOMETHING IS NOT OK!!");
    $p;
    differ.draw ("open");
    $p;;
    differed.simplify( differed.head );
    differed.write_example (stdout);
    printf ("\n");
    differed.draw ("open");
    return 0;
}