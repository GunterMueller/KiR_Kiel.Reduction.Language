#include <stdio.h>
#include "mytypes.h"

extern int yyparse(), georg(int), tis(), ris(), compile(), correct_t_frame();
void out_tree(PROGRAM *);

extern int verbose, lineno, optimize;
extern char * infile, * outfile;
extern FILE * yyin, * yyout;
extern PROGRAM * program_start;

extern char * program;
extern char * version, * vgeorg, * vtis, * vris, * vcompile;
