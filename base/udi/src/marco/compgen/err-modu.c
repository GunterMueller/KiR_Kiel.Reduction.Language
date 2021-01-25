/**********************************************************************************
***********************************************************************************
**
**   File        : err-modu.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 11.02.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Functions for errorhandling
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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dbug.h"

#include "err-modu.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct ErrorOptionsTag { /* Option for error handling ********************/
 FILE *msg;                                        /* Outputchannel for messages */
 FILE *warn;                                       /* Outputchannel for warnings */
 FILE *fatal;                          /* Outputchannel for fatal error messages */
} ErrorOptions; /*****************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define ERR_MSG_TXT \
"Message from line %d : %s"
#define ERR_WARN_TXT \
"Warning at line %d : %s"
#define ERR_FATAL_TXT \
"Fatal error at line %d : %s"
#define ERR_SYNTAX_TXT \
"Syntax error at line %d : %s"

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static ErrorOptions ERR;                           /* Errorhandling informations */

/*********************************************************************************/
/********** GLOBAL VARIABLES IN MODULE *******************************************/
/*********************************************************************************/

extern int yyLineNumber;                 /* Definied in ccomp.l scanner function */
int NR_OF_WARNINGS=0;
int NR_OF_ERRORS=0;

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void InitErrorModul(FILE *msg,FILE *warn,FILE *fatal)
{ /* Initialize module for errorhandling with file streams ***********************/
 DBUG_ENTER("InitErrorModul");
  NR_OF_WARNINGS=0;
  NR_OF_ERRORS=0;
  ERR.msg=msg;                                              /* Setup filestreams */
  ERR.warn=warn;
  ERR.fatal=fatal;
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void err_Message(int err_level,char *txt)
{ /* Handles error messages and involes actions dependent on err_level ***********/
 FILE *FH=NULL;                                        /* Outstream for messages */
 char *frmt=NULL;                    /* Text- and formathandle for error message */
 DBUG_ENTER("err_Message");

 if (err_level&ERR_MSG) {
  NR_OF_ERRORS++;
  FH=ERR.msg;                                 /* Set file stream for error class */
  frmt=ERR_MSG_TXT;
 } else if (err_level&ERR_WARN) {
  FH=ERR.warn;
  if (err_level&ERR_SYNTAX) {
   NR_OF_ERRORS++;
   frmt=ERR_SYNTAX_TXT;
  } else {
   NR_OF_WARNINGS++;
   frmt=ERR_WARN_TXT;
  }
 } else if (err_level&ERR_FATAL) {
  NR_OF_ERRORS++;
  FH=ERR.fatal;
  frmt=ERR_FATAL_TXT;
 }
 if (FH==NULL) DBUG_VOID_RETURN;                     /* No output stream defined */

 fprintf(FH,frmt,yyLineNumber,txt);                             /* Printout text */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

int err_OK(void)
{ /* Checks if errors are occured ************************************************/
 return(NR_OF_WARNINGS+NR_OF_ERRORS==0);
} /*******************************************************************************/

void err_Exit(void)
{ /* Call at program exit ********************************************************/
 DBUG_ENTER("err_Exit");
 if (ERR.msg) if ((ERR.msg!=stdout)&&(ERR.msg!=stderr))       fclose(ERR.msg);
 if (ERR.warn) if ((ERR.warn!=stdout)&&(ERR.warn!=stderr))    fclose(ERR.warn);
 if (ERR.fatal) if ((ERR.fatal!=stdout)&&(ERR.fatal!=stderr)) fclose(ERR.fatal);
 DBUG_VOID_RETURN;
} /*******************************************************************************/
