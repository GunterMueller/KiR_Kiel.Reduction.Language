/**********************************************************************************
***********************************************************************************
**
**   File        : marco-print.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 3.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for internal node printouts
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
#include <ctype.h>
#include <string.h>

#include "marco-print.h"

#include "dbug.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define MAX_INDENT 	16864
#define MAX_INDENT_ADD	4096

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define MY_FLUSH()

/*********************************************************************************/
/********** EXTERN FUNCTION PROTOTYPES *******************************************/
/*********************************************************************************/

extern void print_expr_3(NODE *node);
extern void print_LIST(NODE *node);
extern void print_NODE(NODE *node);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int  Lines;                      /* Number of lines written to outputfile */
static int  Indent;                                       /* Actual indent value */
static char *INDENT=NULL;                        /* Array to store indent values */
static int  *TABS=NULL;                   /* Array to store Set/Clear Tab values */
static int  TabPos;                                 /* Actual tabulator position */
static int  PR_INDENT;                                   /* Print indention sign */
static int  PR_BLANK;              /* Print or do not print a blank before token */
static FILE *FH=stdout;                             /* Default output filehandle */
static int  indent_size=0;                               /* Size of indent array */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void Init_PrintModul(FILE *FileHandle)
{ /* Set up modul for printouts **/
 DBUG_ENTER("Init_PrintModul");

 if (INDENT==NULL) {
  INDENT=(char *)malloc(MAX_INDENT);
  TABS=(int *)malloc(MAX_INDENT*sizeof(int));
  indent_size=MAX_INDENT;
 }

 memset(INDENT,' ',indent_size);
 INDENT[0]='\0';
 Indent=0;
 TabPos=0;
 Lines=0;
 PR_INDENT=1;
 TABS[0]=0;
 PR_BLANK=0;
 FH=FileHandle;
 
 DBUG_VOID_RETURN;
} /*********/

void print_Token(char *token,int len)
{ /* Print one token to stdout ***************************************************/
 DBUG_ENTER("print_Token");

 if (PR_INDENT) print_Indent();

 if (PR_BLANK) {
  len++;
  fprintf(FH," %s",token);MY_FLUSH();
 } else {
  fprintf(FH,"%s",token);MY_FLUSH();
  PR_BLANK=1;
 }


 INDENT[Indent]=' ';
 Indent+=len;
 INDENT[Indent]='\0';

 if (Indent>(indent_size-MAX_INDENT_ADD)) { /* Buffer to small ? */
  indent_size+=MAX_INDENT_ADD;
  INDENT=(char *)realloc(INDENT,indent_size);
  TABS=(int *)realloc(TABS,indent_size*sizeof(int));
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_Delimiter(char *delimiter,int len)
{ /* Print one delimiter to stdout ************************************************/
 DBUG_ENTER("print_Delimiter");

 if (PR_BLANK) {
  len++;
  fprintf(FH," %s",delimiter);MY_FLUSH();
 } else {
  fprintf(FH,"%s",delimiter);MY_FLUSH();
 }

 INDENT[Indent]=' ';
 Indent+=len;
 INDENT[Indent]='\0';
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_Tab(void)
{ /* Print one tabulator to file *************************************************/
 DBUG_ENTER("print_Tab");

 fprintf(FH,"**TAB**");

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_SetTab(void)
{ /* Set internal tabulator ******************************************************/
 DBUG_ENTER("print_SetTab");

 TabPos++;
 TABS[TabPos]=Indent+1;
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_ClearTab(void)
{ /* Clear internal tabulator ****************************************************/
 DBUG_ENTER("print_ClearTab");

 if (TabPos>0) {
  TabPos--;
  INDENT[Indent]=' ';
  Indent=TABS[TabPos];
  INDENT[Indent]='\0';
  PR_INDENT=1; /* Before printing next token, print indent */
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_Indent(void)
{ /* Print actual indention to stdout ********************************************/
 DBUG_ENTER("print_Indent");

 fprintf(FH,"%s",INDENT);MY_FLUSH();
 PR_INDENT=0;
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_CarriageReturn(void)
{ /* Print linefeed and/or carriage return to file *******************************/
 DBUG_ENTER("print_CarriageReturn");

 INDENT[Indent]=' ';
 Indent=TABS[TabPos];
 INDENT[Indent]='\0';
 fprintf(FH,"\n");MY_FLUSH();
 PR_INDENT=1; /* Before printing next token, print indent */
 PR_BLANK=0;
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_BindSy(void)
{ /* Set internal bindflag and print next output directly beyond last output *****/
 DBUG_ENTER("print_BindSy");

 PR_BLANK=0;
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void print_prot_ident(NODE *node)        
{ /* Generated format function ! */        
 DBUG_ENTER("print_prot_ident");        
       
 if (node) {        
  int i;
  for (i=0;i<node->len;i++) print_Delimiter("\\",1);
  print_Delimiter(node->U.str,strlen(node->U.str));
 }
       
 DBUG_VOID_RETURN;        
} /* End of generated format function prot_ident */        

void print_NODE3(NODE *node)
{ /* **/
 DBUG_ENTER("print_NODE3");
 
 if (node) {
  switch (NODE_TYPE(node->type)) {
   case NT_EXPR : print_expr_3(node);break;
   default : fprintf(FH,"** UNKNOWN NODE3 **");MY_FLUSH();
  }
 }
 
 DBUG_VOID_RETURN;
} /******/

void print_NORMAL(NODE *node)
{ /* **/
 DBUG_ENTER("print_NORMAL");

 if (node) { /* Test pointer ? */
  if ((node->type&NT_LIST)==NT_LIST) print_LIST(node);
  else if ((node->type&NT_NODE3)==NT_NODE3) print_NODE3(node);
  else {
   switch (NODE_TYPE(node->type)) { /* Switch over some different node types */
    case NT_CHAR : /* Print one character */
     INDENT[Indent]=' ';
     switch (node->U.character) {
      case '"' : case '#' :
       fprintf(FH,"%c%c",34,node->U.character);MY_FLUSH();
       Indent++;
       break;
      default :fputc(node->U.character,FH);MY_FLUSH();
     }
     Indent++;
     INDENT[Indent]='\0';
     break;
    case NT_EXPR :
     print_NODE(node->U.one);
     break;
    case NT_IDENTIFIER :
     print_Token(node->U.str,strlen(node->U.str));
     break;
    case NT_NUMBER :
     print_Token(node->U.str,strlen(node->U.str));
     break;
    case NT_PATTIDENTIFIER :
     print_Token(node->U.str,strlen(node->U.str));
     break;
    case NT_PROTIDENT :
     print_Token(node->U.str,strlen(node->U.str));
     break;
    case NT_STRINGIDENT :
     print_Token(node->U.str,strlen(node->U.str));
     break;
    default : fprintf(FH,"** UNKNOWN NORMAL NODE **");MY_FLUSH();
   }
  }
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/
