#include "differ-tree.hpp"

const char*  INPUT_FILE = "diff.txt";

FILE* open_tex (const char* input_file)
{
    FILE* stream = fopen (input_file, "w");
    fprintf (stream, "\\documentclass[a4paper,12pt]{article}\n"
                     "\\begin{document}\n\n");
    return stream;
}
void close_tex (FILE* stream)
{
    fprintf (stream, "\n\n\\end{document}");
    fclose (stream);
}

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
    FILE *tex_stream = open_tex ("diff_latex.tex");
    if ( ! tex_stream  )
        exit (226);
    differ.tex_tree (tex_stream);
    $p;
    differ.draw ("open");
    /*
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
    differed.draw ("open");*/

    close_tex (tex_stream);

    return 0;
}