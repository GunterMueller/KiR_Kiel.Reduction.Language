/**********************************************************************************
***********************************************************************************
**
**   File        : marco-main.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 21.5.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Main programm for generated compiler. Drive transformation
**                 and parse the commandline for options.
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "dbug.h"
#include "marco-create.h"
#include "marco-options.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#ifdef INTERNAL_UDI_DEBUG
/* additional debug options for this tool ****************************************/
#define OPTION_STRING 	"i:e:O:f:F:o:M:#:s:hrtcC#:pTP"
extern print_NODE();
#else
#define OPTION_STRING 	"i:e:O:f:F:o:M:#:s:hrtcC#:"
#endif

/*********************************************************************************/
/********** EXTERN FUNCTION AND DATA(VARIABLES) TYPES ****************************/
/*********************************************************************************/

#ifndef HAS_GETOPT
extern int getopt(int,const char * const *,char *);
#endif

extern char *optarg;      /* Use in in unix function getop. Pointer to extra arg */
extern int optind;                            /*  Use in in unix function getop. */

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char *name="TEST";      /* Pointer to programmname, here defined as dummy */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void ShowUsage(int errors)
{ /* Show a simple information page **********************************************/
 DBUG_ENTER("ShowUsage");

 printf("Simple Usage (%s): \n",name);
 printf(" -h\t\t : Shows this helppage\n");
 printf(" -f filename\t : Source code file\n");
 printf(" -i initfile\t : pi-red init filename\n");
 printf(" -c\t\t : Convert stackelements to old format (default)\n");
 printf(" -C\t\t : Convert stackelements to new format\n");
 printf(" -F redfilename\t : Use a red file for reduction/transformation\n");
 printf(" -O outputformat : Outputformat (RED,OREL0,SRC,NONE)\n");
 printf(" -o outputfile\t : Destinationfile\n");
 printf(" -e errorfile\t : Outputfile for errors (default is stderr)\n");
 printf(" -r\t\t : Call reduma and reduce code (performs default number of steps)\n");
 printf(" -s number\t : Performs <number> reduction steps\n");
 printf(" -t \t\t : Print output processing time (reduce)\n");
#ifdef INTERNAL_UDI_DEBUG 
 printf(" -p\t\t : Print internal node representation before reduce\n");
 printf(" -T\t\t : Print internal node representation after reduce\n");
 printf(" -P\t\t : Print source code without reduction\n");
#endif
#ifdef DDBUG_OFF
 printf(" -# debugopts\t : Fred Fish debug package options\n");
#endif

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static int TestFilename(char *filename,int *errflag)
{ /* Tests filename is already defined and prints a error message ****************/
 if (filename!=NULL) {
  fprintf(stderr,"You already specified <%s> as filename !\n",filename);
  (*errflag)++;                                     /* increase number of errors */
  return (0);                                                         /* failure */
 }
 return (1);                                                             /* Okay */
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF PROGRAMM MAIN FUNCTION ****************************/
/*********************************************************************************/

int main(int argc,char *argv[])
{ /* Main program for compilers (Parse commandline and call reduce) **************/

 int c;                                    /* Result from argumentvector parsing */
 int errflag=0;      /* Indicates number of error during parsing the commandline */
 int showusage=0;               /* If defined, print usage information to stdout */

 DBUG_ENTER("MAIN PROGRAMM");

 SetCompilerOptions2Default();                  /* Setup compiler default values */
 
 /* Parse commandline ************************************************************/

 name=argv[0];                                               /* Set programmname */

 while ((c=getopt(argc,(const char * const *)argv,OPTION_STRING))!=-1) /**********/

  switch (c) { /* Test commandline switch ****************************************/

   case 'l' :
    break;

   case 'f' :                       /* Src filename given, compile this src file */
    if (TestFilename(OPTIONS.filename,&errflag)) {       /* Is a filename okay ? */
     OPTIONS.filename=(char *)my_strdup(optarg);               /* Store filename */
     OPTIONS.fileformat=FF_SOURCE;                       /* Fileformat is source */
    }
    break;

   case 'i' : /* Red init filename */
    if (TestFilename(OPTIONS.initfilename,&errflag)) {/* Is a filename okay ? */
     OPTIONS.initfilename=(char *)my_strdup(optarg);        /* Store filename */
    }
    break;

   case 'e' :                                                  /* Error filename */
    if (TestFilename(OPTIONS.errfilename,&errflag)) {/* Is a filename okay ? */
     OPTIONS.errfilename=(char *)my_strdup(optarg);            /* Store filename */
    }
    break;

   case 'F' :                                          /* Inputfile is redformat */
    if (TestFilename(OPTIONS.filename,&errflag)) {       /* Is a filename okay ? */
     OPTIONS.filename=(char *)my_strdup(optarg);               /* Store filename */
     OPTIONS.fileformat=FF_RED;                            /* Set fileformattype */
     OPTIONS.AllocStack=1;                                    /* Stack is needed */
     OPTIONS.conv2old=0;                                /* convert to new format */
    }
    break;

   case 'o' :                         /* out filename given, output to this file */
    if (TestFilename(OPTIONS.outfilename,&errflag)) {    /* Is a filename okay ? */
     OPTIONS.outfilename=(char *)my_strdup(optarg);            /* Store filename */
    }
    break;

   case 'O' :                                           /* Defined output format */
    OPTIONS.AllocStack=1;                                    /* Output is needed */
    if (strcmp(optarg,"OREL0")==0) OPTIONS.outputformat=OF_OREL0;
    else if (strcmp(optarg,"SRC")==0) OPTIONS.outputformat=OF_SRC;
    else if (strcmp(optarg,"RED")==0) OPTIONS.outputformat=OF_STACKELEMS;
    else if (strcmp(optarg,"NONE")==0) { 
     OPTIONS.outputformat=OF_NONE;
     OPTIONS.AllocStack=0;
    } else {
     fprintf(stderr,"Unknow output format <%s>\n",optarg);
     errflag++;                        /* increase number of commandline errors. */
     OPTIONS.AllocStack=0;                           /* Do not allocated a stack */
    }
    break;

   case 'M' :         /* Set amount of memory for internal nodes at startup time */
    OPTIONS.MEMBLOCKS=atoi(optarg);                 /* Set number of node blocks */
    if (OPTIONS.MEMBLOCKS<1) {
     fprintf(stderr,"Warning : Illegal memsize ! Using default value (%d).\n",
      DEF_NR_NODES
     );
     OPTIONS.MEMBLOCKS=DEF_NR_NODES;                        /* Set default value */
    }
    break;

   case 'h' :                                     /* Commandline option helppage */
    showusage=1;                    /* Show usage page at the end of cmd parsing */
    break;

   case 'r' :              /* After transformation (translating) reduce programm */
    OPTIONS.reduce=1;                                         /* Reduce programm */
    OPTIONS.AllocStack=1;                                        /* Stack needed */
    break;

   case 't' :                /* Print reduction time information after reduction */
    OPTIONS.reduce=1;                                         /* Reduce programm */
    OPTIONS.AllocStack=1;                                        /* Stack needed */
    OPTIONS.time=1;                                          /* Time info wanted */
    break;

   case 'c' :                    /* Convert stackelement to old format (default) */
    OPTIONS.conv2old=1;                                 /* convert to old format */
    break;

   case 'C' :                              /* Convert stackelement to new format */
    OPTIONS.conv2old=0;                                 /* convert to new format */
    break;

   case 's' :         /* Set number of reduction steps (overrides default value) */
    OPTIONS.red_counter=atoi(optarg);                     /* Number of red steps */
    break;

#ifdef INTERNAL_UDI_DEBUG
   case 'p' :                             /* Print internal nodes representation */
    OPTIONS.PRtree=1;                                                   /* Do it */
    break;
   case 'T' :
    OPTIONS.test=1;    /* Do additional test and print the testresults to stdout */
    break;
   case 'P' :
    OPTIONS.PRSource=1;          /* Print source code after first transformation */
    break;
#endif

   case '#' :                                  /* Fred fish debug paket features */
    DBUG_PUSH(optarg);                              /* Push options to debug fct */
    break;

   default : errflag++;                                           /* Parse error */

  } /* End of commandline switch statement ***************************************/

 /* Commandline parsing done ****************************************************/

 if (showusage || errflag) {                          /* Errors in commandline ? */
  if (OPTIONS.filename) free(OPTIONS.filename);       /* Free space for filename */
  if (OPTIONS.outfilename) free(OPTIONS.outfilename); /* Free space for filename */
  ShowUsage(errflag);                                   /* print a short helpage */
  exit(1);                                           /* Exit programm with error */
 }

/* Translation/ Transformation / Reduction/ Retranslation process ****************/
 
 if (InitCompilerUnit()) {                            /* Init translation unit ? */

  if (CompileLanguageFile()) {                         /* Loading/Parsing OKAY ? */

   ReduceProgramm();                     /* Start reduction and generate results */

  }                                             /* End of if CompileLanguageFile */

 }                                                 /* End of if InitCompilerUnit */

 CleanUpCompilerUnit(1); /* Free resources and tidy up modules *******************/

 DBUG_RETURN(0);                          /* Normal programm exit without errors */
} /*******************************************************************************/
