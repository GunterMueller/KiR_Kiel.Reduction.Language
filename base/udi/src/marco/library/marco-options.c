/**********************************************************************************
***********************************************************************************
**
**   File        : marco-options.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 27.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for retransformation
**                 from Orel0 to source code and return
**                 It contains also all function to drive to transformtion
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

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "dbug.h"

#include "marco-options.h"
#include "marco-create.h"
#include "marco-misc.h"
#include "marco-print.h"
#include "marco-gencode.h"

#ifdef INTERNAL_UDI_DEBUG
#include "marco-prnode.h"
#endif

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** EXTERN FUNCTION AND DATA(VARIABLES) TYPES ****************************/
/*********************************************************************************/

/* Extern declaration from generated scanner lex.yy.c ****************************/
extern void yyrestart( FILE *input_file );
extern char *YY_IN_FILENAME;
extern FILE *YY_ERRFILE;
extern int  YY_NR_ERRORS;
extern int  LineNumber;
extern int  NR_of_EXPRs;

/* Extern declaration from generated parser (lang).tab.c *************************/
extern int yyparse();       /* Parse function from generated parser (yacc/bison) */
extern FILE *yyin;             /* File handle of input file for scanning/parsing */
#if YYDEBUG
extern int yydebug;            /* Variable to enable parser debuging facillities */
#endif

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int NUMBER_OF_STARTS=0;                             /* Number of restarts */

static int NOTREE=1;         /* True, if it is not needed to generate orel0 code */

#ifdef INTERNAL_UDI_DEBUG
static int PRtree=0; /* If defined, print internal node representation to stdout */
static int PRSource=0;             /* If defined, print inputfile as prettyprint */
#endif

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

MarcoOptions OPTIONS;                                        /* Compiler options */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void SetCompilerOptions2Default(void)
{ /* Sets all compiler options to their default values ***************************/
 DBUG_ENTER("SetCompilerOptions2Default");

 OPTIONS.time=0;                              /* Print time used after reduction */
 OPTIONS.fileformat=FF_SOURCE;                  /* Default : read hl source code */
 OPTIONS.reduce=0;                                     /* default : no reduction */
 OPTIONS.conv2old=1;                                    /* convert to old format */
 OPTIONS.output_fh=stdout;                          /* Default output-filehandle */
 OPTIONS.Red_Params=NULL;                         /* No reduma parameter defined */
 OPTIONS.ONWS_RedParms=1;                 /* module owns the red_parms structure */
 OPTIONS.stack=NULL;                                /* No programstack available */
 OPTIONS.stacksize=0;                                           /* Size of stack */
 OPTIONS.stackelems=0;                                          /* No elems used */
 OPTIONS.red_counter=-1;                                         /* No reduction */
 OPTIONS.result_stacksize=0;                              /* Actually not needed */
 OPTIONS.outputformat=OF_SRC;                                    /* Outputformat */
 OPTIONS.test=0;                                               /* No test needed */
 OPTIONS.outfilename=NULL;                                  /* No filename given */
 OPTIONS.filename=NULL;                                     /* No filename given */
 OPTIONS.MEMBLOCKS=DEF_NR_NODES;   /* Nr of memblocks to allocate at startuptime */
 OPTIONS.AllocStack=1;            /* allocate a new stack for compiling purposes */
 OPTIONS.PRtree=0;                               /* Do not print a internal node */
 OPTIONS.PRSource=0;                         /* Do not print a pretty print node */
 OPTIONS.errfilename=NULL;                                 /* Name of error file */
 OPTIONS.initfilename=NULL;                                  /* Name of initfile */
 OPTIONS.error_fh=stderr;                              /* Erroroutput filehandle */
 OPTIONS.onexpr=-1;                              /* Scan/parse full programmtext */

 OPTIONS.INIT_DONE=1;

 DBUG_VOID_RETURN;
} /*******************************************************************************/

int InitCompilerUnit(void)
{ /* *****************************************************************************/
 DBUG_ENTER("InitCompilerUnit");

 DBUG_PRINT("Test init",("OPTIONS.INIT_DONE=%d",OPTIONS.INIT_DONE));

 if (OPTIONS.INIT_DONE) {                               /* Structure init done ? */

  YY_NR_ERRORS=0;                                    /* Set error number to zero */
  LineNumber=1;                  /* Start line number (increased while scanning) */

  if (!ALLOCNODES(OPTIONS.MEMBLOCKS)) {                /* Node allocation done ? */
   fprintf(stderr,"Warning : Can't alloc enough memory. Using smaller buffers !\n");
  }

  if (OPTIONS.AllocStack) {                 /* Allocate space for programstack ? */
   OPTIONS.stack=(STACKELEM *)calloc(DEFAULT_STACK_SIZE,sizeof(STACKELEM));
   if (OPTIONS.stack==NULL) {
    fprintf(stderr,"Can't allocated compiler stack !\n");
    DBUG_RETURN(0);
   }
   OPTIONS.stacksize=DEFAULT_STACK_SIZE;              /* Number of free elements */
  }

  if (OPTIONS.outfilename) {                             /* Use file as output ? */
   if ((OPTIONS.output_fh=fopen(OPTIONS.outfilename,"w"))==NULL) {  /* File OK ? */
    fprintf(stderr,"Can't open file <%s> for output\n",OPTIONS.outfilename);
    DBUG_RETURN(0);
   }
  }

 if (OPTIONS.errfilename) {
  if ((OPTIONS.error_fh=fopen(OPTIONS.errfilename,"w"))==NULL) {    /* File OK ? */
   fprintf(stderr,"Can't open file <%s> for error output\n",OPTIONS.errfilename);
   DBUG_RETURN(0);
  }
 }
 
 if (OPTIONS.fileformat==FF_SOURCE) { /* Fileformat = high level source code ? */

  if (OPTIONS.onexpr!=-1) NR_of_EXPRs=OPTIONS.onexpr;

  if (OPTIONS.filename) {                              /* is a filename given ? */
   if (strcmp(OPTIONS.filename,"STDIN")==0) {
    yyin=stdin;
    YY_IN_FILENAME="STDIN";
   } else {
    if ((yyin=fopen(OPTIONS.filename,"r"))==NULL) {                /* File OK ? */
     fprintf(stderr,"Can't open file <%s>\n",OPTIONS.filename);
     DBUG_RETURN(0);
    }
    YY_IN_FILENAME=OPTIONS.filename;
   }
  } else {                                                  /* no filename give */
   yyin=stdin;                              /* Read input file from standard in */
   YY_IN_FILENAME="STDIN";
  }  
  if (NUMBER_OF_STARTS>0) yyrestart(yyin);                    /* Reinit scanner */
  NUMBER_OF_STARTS++;                       /* Increase number of parsing tries */
 } else NOTREE=0;                             /* No internal node list needed ! */
 
 } else {                                                  /* Structure not init */
  fprintf(stderr,"OPTIONS structure not inited !\n");
  DBUG_RETURN(0);
 }

 DBUG_RETURN(1);
} /*******************************************************************************/

void CleanUpCompilerUnit(int freeall)
{ /* Deallocates all memory and tidy up this module ******************************/
 DBUG_ENTER("CleanUpCompilerUnit");

 DBUG_PRINT("Free Compiler Unit",("freeall=%d",freeall));

 if (OPTIONS.ONWS_RedParms) {                                      /* Is owner ? */
  if (OPTIONS.Red_Params) {
   free(OPTIONS.Red_Params);                  /* Free reduma parameter structure */
   OPTIONS.Red_Params=NULL;
  }
 }

 if (OPTIONS.stack) {
  free(OPTIONS.stack);                                /* Free compiler red stack */
  OPTIONS.stack=NULL;
 }
 
 if (OPTIONS.filename) {
  free(OPTIONS.filename);                                       /* Free filename */
  OPTIONS.filename=NULL;
 }

 if (OPTIONS.initfilename) {
  free(OPTIONS.initfilename);                                   /* Free filename */
  OPTIONS.initfilename=NULL;
 }

 if (OPTIONS.outfilename) {
  if (YY_NR_ERRORS>0) {                                              /* Errors ? */
   remove(OPTIONS.outfilename);                             /* remove outputfile */
  }
  free(OPTIONS.outfilename);                                   /* Free filename */
  OPTIONS.outfilename=NULL;
 }

 if (OPTIONS.errfilename) {
  if (YY_NR_ERRORS==0) remove(OPTIONS.errfilename);         /* remove outputfile */
  free(OPTIONS.errfilename);                                   /* Free filename */
  OPTIONS.errfilename=NULL;
 }

 if (OPTIONS.output_fh!=stdout) {
  fclose(OPTIONS.output_fh);                                 /* Close outputfile */
 }

 FREENODES();                                  /* Release memory for node buffer */

 NOTREE=1;                                              /* Codegeneration needed */

 OPTIONS.onexpr=-1;                              /* Scan/parse full programmtext */

#ifdef INTERNAL_UDI_DEBUG
 PRtree=0;
 PRSource=0;
#endif

 DBUG_VOID_RETURN;
} /*******************************************************************************/

int CompileLanguageFile(void)
{ /* *****************************************************************************/
 int State=1;                                                  /* Parsing result */

 DBUG_ENTER("CompileLanguageFile");

 if (OPTIONS.fileformat==FF_SOURCE) {                     /* Test outputformat ? */

#ifdef YYDEBUG
  yydebug=1;                                  /* Enable parser debugging feature */
#endif

  YY_ERRFILE=OPTIONS.error_fh;                     /* Set erroroutput filehandle */

  DBUG_PRINT("BEFORE YYPARSE",("State=%d",State));
  State=(yyparse()==0);                                            /* Parse file */
  DBUG_PRINT("AFTER YYPARSE",("State=%d",State));

  if (yyin!=stdin) fclose(yyin);                                   /* Close file */

  } else if (OPTIONS.fileformat==FF_RED) {                /* Input file is orel0 */

  if (OPTIONS.conv2old) OPTIONS.conv2old=2;              /* Conv to new elements */

  State=udi_ReadREDFile(OPTIONS.filename);                      /* Load RED file */

 } else State=0;                        /* No format defined (strange situation) */

 if (State) {                                                         /* State ? */
  if (NOTREE) {                                        /* Orel Code generation ? */
#ifdef INTERNAL_UDI_DEBUG
   if(PRtree) udi_PrTree(GOALEXPR);
   if (PRSource) {
    Init_PrintModul(OPTIONS.output_fh);
    print_NODE(GOALEXPR);printf("\n");
   }
#endif
   State=udi_GenerateOrel0();                             /* Codegeneration ok ? */
   REUSENODES();                                        /* Use node arrays again */
  }
 }

 DBUG_PRINT("Compile file done",("State=%d",State));

 if (OPTIONS.error_fh!=stderr) {
  fclose(OPTIONS.error_fh); /* Close error file */
 }

 DBUG_RETURN(State);                                                     /* Okay */
} /*******************************************************************************/

void ReduceProgramm(void)
{ /* Start reduction *************************************************************/
 DBUG_ENTER("ReduceProgramm");

 DBUG_PRINT("BEGIN",("OPTIONS.reduce=%d",OPTIONS.reduce));

 if (OPTIONS.ONWS_RedParms) {
  if (OPTIONS.Red_Params==NULL) {                    /* No parameter allocated ? */
   OPTIONS.Red_Params=(PTR_UEBERGABE)malloc(sizeof(*OPTIONS.Red_Params));
   if (OPTIONS.Red_Params==NULL) {
    fprintf(stderr,"Out of memory error. Can't allocate memory for redparms!\n");
    DBUG_VOID_RETURN;
   }
  }
  if (misc_InitRedParms(OPTIONS.initfilename,I_READ,OPTIONS.Red_Params)==0) {
   misc_SetDefRedParms(OPTIONS.Red_Params);                 /* set default values */
  }
 } else {
  if (OPTIONS.Red_Params==NULL) {
   fprintf(stderr,"Red parameter not initialised !\n");
   DBUG_VOID_RETURN;
  }
 }

 if (OPTIONS.reduce) {                                    /* Reduce expression ? */
  misc_CallReduma();                                        /* Reduce expression */
 } else {                                                      /* Only compile ! */
  OPTIONS.Red_Params->redcnt=0;                                  /* No reduction */
  OPTIONS.Red_Params->errflag=0;                                    /* No errors */
  OPTIONS.Red_Params->redcmd=RED_LRP;          /* Last and only page is on stack */
  OPTIONS.time=0;                                       /* Disable time facility */
 }
 misc_OutputResults();                                           /* Printresults */

 DBUG_PRINT("DONE",(""));

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void Set_RedParms(PTR_UEBERGABE Parms)
{ /* Sets reduma parameters. (Parms=NULL sets parameter to default (red.init)) **/
 if (Parms==NULL) { /* Reset parmameter to default values ************************/
  OPTIONS.Red_Params=NULL;
  OPTIONS.ONWS_RedParms=0;
 } else {
  if ((OPTIONS.ONWS_RedParms==0)&&(OPTIONS.Red_Params)) free(OPTIONS.Red_Params);
  OPTIONS.Red_Params=Parms;                                  /* Just set pointer */
  OPTIONS.ONWS_RedParms=1;
 }
} /*******************************************************************************/

char *my_strdup(char *name)
{ /* Replacement for the standard my_strdup function **/
 char *result=NULL;
 if (name) {
  int len=strlen(name)+1;
  result=(char *)malloc(len);
  memcpy(result,name,len);
 }
 return (result);
} /********/
