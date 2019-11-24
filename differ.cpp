#include "differ-tree.hpp"

const char*  INPUT_FILE = "diff.txt";
const int BUFFERSIZE = 256;

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

void send_differentiation (char* mail)
{
    char command[BUFFERSIZE] = {};
    snprintf (command, sizeof (command), "echo 'Differentiation' | mail -s 'Differentiation' -A diff_latex.pdf %s", mail );
    system (command); 

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
    printf ("Enter your expression (I will differentiate only by x!!): \n");
    char last_symb = 0;
    last_symb = differ.read_tree ();
    if (last_symb)
    {
        printf ("SYNTAX ERROR near %c\n", last_symb);
        $p;
        $p;
        exit (13);
    }

    //differ.draw ("open");

    FILE *tex_stream = open_tex ("diff_latex.tex");
    if ( ! tex_stream  )
        exit (226);


    fprintf (tex_stream, "Your function: \n");
    differ.tex_tree (tex_stream);
    fprintf (tex_stream, "\\\\ \\\\");

    int times_to_differ = 0;
    double variable_value = 0.0;

    DFE::ERR err_code = DFE::OK;

    CalcTree differed;


    printf ("How many times do you want to differentiate?\n");
    scanf ("%d", &times_to_differ);
    printf ("What's the value of your variable?\n");
    scanf ("%lf", &variable_value);

    CalcTree *differentiations = new CalcTree [times_to_differ];

    for (int i = 0; i < times_to_differ; i++)
    {
        delete differentiations[i].head;
        if (i == 0)
        {
            differentiations[i].head = diff_funcs::differentiate (differ.head, 'x', err_code);
            if (err_code != DFE::OK)
            {
                printf ("I don't nave so much memory:(");
                $p;
                $p;
                exit(1);
            }
        }
        else
        {
            differentiations[i].head = diff_funcs::differentiate (differentiations[i-1].head, 'x', err_code);
            if (err_code != DFE::OK)
            {
                printf ("I don't nave so much memory:(");
                $p;
                $p;
                exit(1);
            }
        }
    }
    
    for (int i = 0; i < times_to_differ; i++)
    {
            fprintf (tex_stream, "\\par The %d differentiation: \\\\ ", i+1);
            differentiations[i].simplify_tree ();
            differentiations[i].tex_tree (tex_stream);
            fprintf (tex_stream,"\\\\ \n\n\\par Let x be %lg: \\\\ ", variable_value);
            differentiations[i].insert_variable ('x', variable_value);
            differentiations[i].tex_tree (tex_stream);
            fprintf (tex_stream,"\\\\ \n\n\\par Calculate: \\\\ ");
            differentiations[i].simplify_tree ();
            differentiations[i].tex_tree (tex_stream);
            fprintf (tex_stream,"\n \\\\ \\\\ \n");
    }

    char mail[BUFFERSIZE] = {};

    printf ("Enter your email\n");
    scanf ("%s", mail);

    close_and_make_tex (tex_stream);

    send_differentiation (mail);

    return 0;
    
}