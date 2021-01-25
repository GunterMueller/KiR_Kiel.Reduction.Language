#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "dbug.h"
#include "gen-options.h"
#include "gen-parser.h"
#include "trans-tbl.h"
#include "err-modu.h"

#define OPTION_STRING "f:S:#:"

#define DEFAULT_FILENAME	"empty"

#define DEFAULT_NR_OF_SYMBOLS 511

extern FILE *yyin;
extern int yyparse();
extern char *optarg;
extern int optind;

#ifndef HAS_GETOPT
extern int getopt(int,const char * const *,char *);
#endif

int main(int argc,char *argv[])
{ 
 char *filename=NULL;                            /* Name des Spezifikationsfiles */
 int  State=1;                            /* Programmstatus fuer Codegenerierung */
 int htablesize=DEFAULT_NR_OF_SYMBOLS;              /* Groesse der Symboltabelle */
 int c; /* Result from argumentvector parsing */
 int errflag=0;

 DBUG_ENTER("HAUPT PROGRAMM");

 /* Programmargumente parsen *****************************************************/

 while ((c=getopt(argc,(const char * const *)argv,OPTION_STRING))!=-1)
  switch (c) {
   case 'f' : filename=(char *)my_strdup(optarg);break; /* Get filename */
   case 'S' : htablesize=atoi(optarg);break; /* Size of symboltable */
   case '#' : DBUG_PUSH(optarg);break; /* Set debug values */
   default : errflag++;
  }

 /* Argumentzeile ausgewertet ****************************************************/

 if (errflag||(filename==NULL)) {
  printf("USAGE : %s [-S symboltablesize] -f filename\n",argv[0]);
  DBUG_RETURN(1);
 }

 InitErrorModul(stderr,stderr,stdout);

 if ((yyin=fopen(filename,"r"))==NULL) {                          /* File OK ? */
  fprintf(stderr,"Can't open file <%s>\n",filename);
  State=0;                                               /* Eingabe nicht ok ! */
 }

 if (State) {                           /* Codegenerierung starten, alles OK ? */
  InitGeneratorOptions(filename,htablesize,1); /* Initialisierung der Optionen */
  if ((yyparse()==0)&&(err_OK())) {
   WriteParserCode();
   WriteFormatFunctions(Project_OPT.name);
  } else {
   fprintf(stderr,"Compiler Generation FAILED !\n");
  }
  ReleaseGeneratorOptions();         /* Freigeben von Speicher in den Optionen */
 }

 if (filename!=NULL) free(filename);         /* Speicher fuer Filename freigeben */

 err_Exit(); /* End of program, call err Exit function to cleanup */

 DBUG_RETURN(0); 
}
