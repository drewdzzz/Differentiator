#include "differ-tree.hpp"

const char*  INPUT_FILE = "diff.txt";

FILE* open_tex (const char* input_file)
{
    FILE* stream = fopen (input_file, "w");
    fprintf (stream, "\\documentclass[a4paper, 12pt]{article}\n"
                     "\\usepackage[20pt]{extsizes}\n"
                     "\\author{Programm made by Saranchin A.N.}\n"
                     "\\title{Differentiator}\n"
                     "\\begin{document}\n"
                     "\\maketitle\n\n");
    return stream;
}
void close_and_make_tex (FILE* stream)
{
    fprintf (stream, "\n\n\\end{document}");
    fclose (stream);
    system ("pdflatex diff_latex.tex");
    system ("clear");
    system ("xdg-open diff_latex.pdf");
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

    FILE *tex_stream = open_tex ("diff_latex.tex");
    if ( ! tex_stream  )
        exit (226);

    differ.simplify_tree ();

    fprintf (tex_stream, "Your function: \\\\*");
    differ.tex_tree (tex_stream);
    fprintf (tex_stream, "\\\\ \\\\");

    int times_to_differ = 0;

    DFE::ERR err_code = DFE::OK;

    CalcTree differed;


    printf ("How many times do you want to differentiate?\n");
    if ( ! scanf ("%d", &times_to_differ) )
    {
        printf ("Can't understand you. I'll just show you your example\n");
        $p;
    }
    else
    {
        char variable = 'x';

        printf ("What variable do you want to differentiate?\n");
        fflush (stdin);
        if ( ! scanf (" %c ", &variable) )
        {
            printf ("Can't understand you. I will defferentiate to x");
            $p;
            $p;
        }

        CalcTree *differentiations = new CalcTree [times_to_differ];
        DFE::ERR err_code = DFE::OK;
        for (int i = 0; i < times_to_differ; i++)
        {
            delete differentiations[i].head;
            fprintf (tex_stream, "The %d differentiation: \\\\", i+1);
            if (i == 0)
            {
                differentiations[i].head = diff_funcs::differentiate (differ.head, variable, err_code);
                if (err_code != DFE::OK)
                    exit (123);
            }
            else
            {
                differentiations[i].head = diff_funcs::differentiate (differentiations[i-1].head, variable, err_code);
                if (err_code != DFE::OK)
                    exit (123);
            }
            differentiations[i].simplify_tree ();
            differentiations[i].tex_tree (tex_stream);
            fprintf (tex_stream,"\\\\ \\\\");
        }


    }
    
    close_and_make_tex (tex_stream);
    return 0;
}