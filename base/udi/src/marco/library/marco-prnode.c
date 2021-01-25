/**********************************************************************************
***********************************************************************************
**
**   File        : marco-prnode.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 27.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for node print
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifdef INTERNAL_UDI_DEBUG

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

#include "marco-create.h"
#include "marco-prnode.h"
#include "marco-print.h"

#include "dbug.h"

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
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static void PrNODE2(FILE *fh,int type,NODE *node);
static void PrNODE3(FILE *fh,int type,NODE *node);
static void PrLIST(FILE *fh,int type,NODE *node);
static void PrNORMAL(FILE *fh,int type,NODE *node);
static void PrNODE(FILE *fh,NODE *node);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static int CalcNodeKind(int type)
{ /* Calculates kind of a node **/
 int kind;
 if ((type&NT_LIST)==NT_LIST) {
  kind=NT_LIST;
 } else if ((type&NT_ATOM)==NT_ATOM) {
  kind=NT_ATOM; 
 } else if ((type&NT_NODE2)==NT_NODE2) {
  kind=NT_NODE2;
 } else if ((type&NT_NODE3)==NT_NODE3) {
  kind=NT_NODE3;
 } else kind=0;
 return(kind);
} /*******/

static char *Nodetype2Str(int type)
{ /* Text for a typied node **/
 switch (type) {
  case NT_BINLISTCONCAT: return ("NT_BINLISTCONCAT");break;
  case NT_CLASS : return("NT_CLASS"); break;
  case NT_PRIMFUNC : return("NT_PRIMFUNC"); break;
  case NT_CONST : return("NT_CONST"); break;
  case NT_MATRIX : return("NT_MATRIX"); break;
  case NT_TVECTOR : return("NT_TVECTOR"); break;
  case NT_VECTOR : return("NT_VECTOR"); break;
  case NT_APPLICATION : return("NT_APPLICATION"); break;
  case NT_ASPATTERN : return("NT_ASPATTERN"); break;
  case NT_BINDING : return("NT_BINDING"); break;
  case NT_CASE : return("NT_CASE"); break;
  case NT_CHAR : return("NT_CHAR"); break;
  case NT_COND : return("NT_COND"); break;
  case NT_DEFINE : return("NT_DEFINE"); break;
  case NT_ERRORMESSAGE : return("NT_ERRORMESSAGE"); break;
  case NT_EXPR : return("NT_EXPR"); break;
  case NT_FUNCALL : return("NT_FUNCALL"); break;
  case NT_FUNDEF : return("NT_FUNDEF"); break;
  case NT_IDENTIFIER : return("NT_IDENTIFIER"); break;
  case NT_IFCOND : return("NT_IFCOND"); break;
  case NT_LET : return("NT_LET"); break;
  case NT_LETPAR : return("NT_LETPAR"); break;
  case NT_NUMBER : return("NT_NUMBER"); break;
  case NT_LETP : return("NT_LETP"); break;
  case NT_PATTERNS : return("NT_PATTERNS"); break;
  case NT_PATTERNSTRING : return("NT_PATTERNSTRING"); break;
  case NT_PATTERNSTRUCTURE : return("NT_PATTERNSTRUCTURE"); break;
  case NT_PROTIDENT : return("NT_PROTIDENT"); break;
  case NT_STRING : return("NT_STRING"); break;
  case NT_STRINGIDENT : return("NT_STRINGIDENT"); break;
  case NT_SUBSTITUTE : return("NT_SUBSTITUTE"); break;
  case NT_SYMBOL : return("NT_SYMBOL"); break;
  case NT_TERMSY : return("NT_TERMSY"); break;
  case NT_TYPE : return("NT_TYPE"); break;
  case NT_WHEN : return("NT_WHEN"); break;
  case NT_USERCON : return("NT_USERCON"); break;
  case NT_PATTIDENTIFIER : return("NT_PATTIDENTIFIER");break;
  case NT_DATA_LIST : return("NT_DATA_LIST");break;
  case NT_NUMBER_LIST : return("NT_NUMBER_LIST");break;
  case NT_BOOL_LIST : return("NT_BOOL_LIST");break;
  case NT_STRING_LIST : return("NT_STRING_LIST");
  default : return("*ERROR*");
 }
} /*****/

static char *Kind2Str(int kind)
{ /* Text for node kind **/
 switch (kind) {
  case NT_ATOM : return("NT_ATOM");break;
  case NT_LIST : return("NT_LIST");break;
  case NT_NODE2 : return("NT_NODE2");break;
  case NT_NODE3 : return("NT_NODE3");break;
  default : return("NT_NORMAL");break;
 }
} /*****/

static void PrSubNODE(FILE *fh,int kind,int type,NODE *node)
{ /* Prints subnode **/
 if (node) {
  fprintf(fh,"(");
  switch (kind) {
   case NT_NODE2 : 
    PrNODE2(fh,type,node);
    break;
   case NT_NODE3 :
    PrNODE3(fh,type,node);
    break;
   case NT_LIST :
    PrLIST(fh,type,node);
    break;
   default :
    PrNORMAL(fh,type,node);
    break;
  }
  fprintf(fh,")\n");
 } else fprintf(fh,"< ***** EMPTY-NODE ***** >");
} /******/

static void PrNODE2(FILE *fh,int type,NODE *node)
{
 DBUG_ENTER("PrNODE2");
 fprintf(fh,"NODE2<%d,%s,INF=%d>,",type,Nodetype2Str(type),node->info);
 PrNODE(fh,node->U.tuple->n1);
 PrNODE(fh,node->U.tuple->n2);
 DBUG_VOID_RETURN;
}

static void PrNODE3(FILE *fh,int type,NODE *node)
{
 DBUG_ENTER("PrNODE3");
 fprintf(fh,"NODE3<%d,%s,INF=%d>,",type,Nodetype2Str(type),node->info);
 DBUG_PRINT("TEST",("NODE3<%d,%s>,",type,Nodetype2Str(type)));
 PrNODE(fh,node->U.triple->n1);
 PrNODE(fh,node->U.triple->n2);
 PrNODE(fh,node->U.triple->n3);
 DBUG_VOID_RETURN;
}

static void PrLIST(FILE *fh,int type,NODE *node)
{
 int ltype,lkind;
 NODEList *list;
 DBUG_ENTER("PrLIST");

 lkind=CalcNodeKind(type);
 ltype=CalcNodeType(type,lkind);
 fprintf(fh,"LIST<%d x %s of %s : ",node->len,Kind2Str(lkind),Nodetype2Str(ltype));
 list=node->U.list->first;
 while (list) {
  lkind=CalcNodeKind(list->node->type);
  ltype=CalcNodeType(list->node->type,lkind);
  PrSubNODE(fh,lkind,ltype,list->node);
  list=list->next;
 }
 fprintf(fh,">");
 
 DBUG_VOID_RETURN;
}

static void PrNORMAL(FILE *fh,int type,NODE *node)
{
 DBUG_ENTER("PrNORMAL");
 fprintf(fh,"NORMAL<%d,INF=%d>:",type,node->info);
 switch (type) {
  case NT_CLASS :
   fprintf(fh,"NT_CLASS<%d>",node->U.value);
   break;
  case NT_PRIMFUNC :
   fprintf(fh,"NT_PRIMFUNC<%d>",node->U.value);
   break;
  case NT_CONST :
   fprintf(fh,"NT_CONST<%d>",node->U.value);
   break;
  case NT_MATRIX :
   fprintf(fh,"NT_MATRIX of %d <",node->len);
   PrNODE(fh,node->U.one);
   fprintf(fh,">");
   break;
  case NT_TVECTOR :
   fprintf(fh,"NT_TVECTOR of %d <",node->len);
   PrNODE(fh,node->U.one);
   fprintf(fh,">");
   break;
  case NT_VECTOR :
   fprintf(fh,"NT_VECTOR of %d <",node->len);
   PrNODE(fh,node->U.one);
   fprintf(fh,">");
   break;
  case NT_APPLICATION :
   fprintf(fh,"NT_APPLICATION");
   PrNODE2(fh,node->type,node);
   break;
  case NT_BINLISTCONCAT:
   fprintf(fh,"NT_BINLISTCONCAT");
   PrNODE2(fh,node->type,node);
   break;
  case NT_ASPATTERN :
   fprintf(fh,"NT_ASPATTERN");
   PrNODE2(fh,node->type,node);
   break;
  case NT_BINDING :
   fprintf(fh,"NT_BINDING");
   PrNODE2(fh,node->type,node);
   break;
  case NT_CASE :
   fprintf(fh,"NT_CASE");
   PrNODE2(fh,node->type,node);
   break;
  case NT_CHAR :
   fprintf(fh,"NT_CHAR{%c}",node->U.character);
   break;
  case NT_COND :
   fprintf(fh,"NT_COND");
   PrNODE2(fh,node->type,node);
   break;
  case NT_DEFINE :
   fprintf(fh,"NT_DEFINE");
   PrNODE2(fh,node->type,node);
   break;
  case NT_ERRORMESSAGE :
   fprintf(fh,"NT_ERRORMESSAGE");
   PrNODE(fh,node->U.one);
   break;
  case NT_EXPR :
   fprintf(fh,"NT_EXPR");
   PrNODE(fh,node->U.one);
   break;
  case NT_DATA_LIST :
   fprintf(fh,"NT_DATA_LIST");
   PrNODE(fh,node->U.one);
   break;
  case NT_FUNCALL :
   fprintf(fh,"NT_FUNCALL");
   PrNODE2(fh,node->type,node);
   break;
  case NT_FUNDEF :
   fprintf(fh,"NT_FUNDEF");
   PrNODE3(fh,node->type,node);
   break;
  case NT_IDENTIFIER :
   fprintf(fh,"NT_IDENTIFIER(%d,%s)",node->len,node->U.str);
   break;
  case NT_PATTIDENTIFIER :
   fprintf(fh,"NT_PATTIDENTIFIER(%d,%s)",node->len,node->U.str);
   break;
  case NT_IFCOND :
   fprintf(fh,"NT_IFCOND");
   PrNODE2(fh,node->type,node);
   break;
  case NT_LET :
   fprintf(fh,"NT_LET");
   PrNODE2(fh,node->type,node);
   break;
  case NT_LETPAR :
   fprintf(fh,"NT_LETPAR");
   PrNODE2(fh,node->type,node);
   break;
  case NT_NUMBER :
   fprintf(fh,"NT_NUMBER(%s)",node->U.str);
   break;
  case NT_LETP :
   fprintf(fh,"NT_LETP");
   PrNODE2(fh,node->type,node);
   break;
  case NT_PATTERNS :
   fprintf(fh,"NT_PATTERNS");
   PrNODE(fh,node->U.one);
   break;
  case NT_PATTERNSTRING :
   fprintf(fh,"NT_PATTERNSTRING");
   PrNODE(fh,node->U.one);
   break;
  case NT_PATTERNSTRUCTURE :
   fprintf(fh,"NT_PATTERNSTRUCTURE");
   PrNODE(fh,node->U.one);
   break;
  case NT_PROTIDENT :
   fprintf(fh,"NT_PROTIDENT(%d,%s)",node->len,node->U.str);
   break;
  case NT_STRING :
   fprintf(fh,"NT_STRING");
   PrNODE(fh,node->U.one);
   break;
  case NT_STRINGIDENT :
   fprintf(fh,"NT_STRINGIDENT(%s)",node->U.str);
   break;
  case NT_SUBSTITUTE :
   fprintf(fh,"NT_SUBSTITUTE");
   PrNODE2(fh,node->type,node);
   break;
  case NT_SYMBOL :
   fprintf(fh,"NT_SYMBOL(%d)",node->U.value);
   break;
  case NT_TERMSY :
   fprintf(fh,"NT_TERMSY(%s)",node->U.str);
   break;
  case NT_TYPE :
   fprintf(fh,"NT_TYPE(%d)",node->U.value);
   break;
  case NT_WHEN :
   fprintf(fh,"NT_WHEN");
   PrNODE3(fh,node->type,node);
   break;
  case NT_USERCON :
   fprintf(fh,"NT_USERCON");
   PrNODE2(fh,node->type,node);
   break;
  default :
   fprintf(fh,"*ERROR*");
 }

 DBUG_VOID_RETURN;
}

static void PrNODE(FILE *fh,NODE *node)
{ /* Prints node to file fh **/
 DBUG_ENTER("PrNODE");

 if (node) {
  int kind,type;
  kind=CalcNodeKind(node->type);
  type=CalcNodeType(node->type,kind);
  DBUG_PRINT("TYPES",(" node_type=%d , type=%d , kind=%d",node->type,type,kind));
  PrSubNODE(fh,kind,type,node);
 } else fprintf(fh,"< ***** EMPTY-NODE ***** >");
  
 DBUG_VOID_RETURN;
} /****/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void udi_PrTree(NODE *node)
{ /* **/
 DBUG_ENTER("udi_PrTree");
  PrNODE(stdout,node);
 DBUG_VOID_RETURN;
} /******/
#endif
