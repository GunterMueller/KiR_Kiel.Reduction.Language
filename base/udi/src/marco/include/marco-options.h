/**********************************************************************************
***********************************************************************************
**
**   File        : marco-options.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 3.5.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for compiler main file 
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _UDIOPTIONS_HEADER_
#define _UDIOPTIONS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdio.h>
#include <sys/types.h> 
#include <fcntl.h>

#include "rstackty.h"
#include "rstelem.h"
#include "cparm.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct MarcoOptionsTag { /* Compiler options *****************************/
 int INIT_DONE;                        /* Signs if this structure is initialised */
 char *filename;                                            /* Name of inputfile */
 char *outfilename;                                        /* Name of outputfile */
 char *errfilename;                                         /* Name of errorfile */
 char *initfilename;                                     /* Name of initfilename */
 int MEMBLOCKS;                              /* Number blocks for internal nodes */
 int AllocStack;                                  /* Allocate a additional stack */
 int PRtree;                /* Signs, if internal nodes structure should printed */
 int PRSource;             /* Signs, if internal source structure should printed */
 int time;                                               /* Print reduction time */
 int outputformat;              /* Format of reduction or code generation output */
 int fileformat;                            /* Format of input file (RED,SOURCE) */
 int reduce;                                                   /* Reduce program */
 int conv2old;                                 /* Convert code to old stackelems */
 FILE *output_fh;                                            /* Code-output file */
 FILE *error_fh;                                             /* Errorfile handle */
 PTR_UEBERGABE Red_Params;                       /* Pointer to reduma parameters */
 int ONWS_RedParms;                   /* Module own the pointer to red structure */
 STACKELEM *stack;                                    /* Pointer to programstack */
 int stacksize;                                          /* Size of programstack */
 int stackelems;                                      /* Number of stackelements */
 int red_counter;                                    /* Number of reductionsteps */
 int result_stacksize;            /* Size of result stack after reduma reduction */
 int onexpr;                                     /* Scan/parse full programmtext */
 int test;
} MarcoOptions; /*****************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/* Input filename types **********************************************************/
#define FF_RED		1
#define FF_SOURCE	2

/* Output fileformat, format of results ******************************************/
#define OF_NONE		0
#define OF_OREL0	1
#define OF_STACKELEMS	2
#define OF_SRC		3

#define DEFAULT_STACK_SIZE	102400
#define DEFAULT_STACK_SIZE_ADD	10240

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern MarcoOptions OPTIONS;                            /* main compiler options */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Sets all compiler options to their default values *****************************/
extern void SetCompilerOptions2Default(void);

/* Init compiler unit for parsing etc ... **/
extern int InitCompilerUnit(void);

/* Deallocates all memory and tidy up this module ********************************/
extern void CleanUpCompilerUnit(int freeall);

/* Start compilation and transformation ******************************************/
extern int CompileLanguageFile(void);

/* Start reduction ***************************************************************/
extern void ReduceProgramm(void);

/* Sets reduma parameters. (Parms=NULL sets parameter to default (red.init)) **/
extern void Set_RedParms(PTR_UEBERGABE Parms);

/* Replacement for the standard strdup function **/
extern char *my_strdup(char *name);

#endif
