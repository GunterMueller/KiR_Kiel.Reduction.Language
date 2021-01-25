/*********************************************************************************
***********************************************************************************
**
**   File        : gen-ccode.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 17.03.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for transformationcode
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

#include "hash-fcts.h"

#include "dbug.h"
#include "gen-scanner.h"
#include "gen-parser.h"
#include "gen-ccode.h"

#include "marco-create.h"
#include "trans-tbl.h"

#include "list-modu.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*#define FREE(ptr) DBUG_PRINT("FREE",("ptr=%p",ptr));free(ptr)*/
#define FREE(ptr) free(ptr)

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static CCSymbol *CopyCCSymbol(CCSymbol *);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUF[1024];

/*********************************************************************************/
/********** GLOBAL VARIABLES IN MODULE *******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static CCSymbolList *MkCCSymbolList(CCSymbol *symbol)
{ /* Create one Symbollist entry *************************************************/
 CCSymbolList *result;
 DBUG_ENTER("MkCCSymbolList");
 result=(CCSymbolList *)malloc(sizeof(CCSymbolList));
 DBUG_ASSERT(result);
 result->symbol=symbol;                                   /* Store symbolpointer */
 result->next=NULL;                                               /* End of list */
 DBUG_RETURN(result);
} /*******************************************************************************/

static CCLines *MkCCLines(CCLine *line)
{ /* Create new lines list *******************************************************/
 CCLines *result;
 DBUG_ENTER("MkCCLines");
 result=(CCLines *)malloc(sizeof(CCLines));
 DBUG_ASSERT(result);
 result->line=line;
 result->next=NULL;
 DBUG_RETURN(result);
} /*******************************************************************************/

static AltElem *GetAltElem(SpecList *list,int nr)
{ /* Return pointer to the symbol number nr **************************************/
 while (list) {                                                 /* Traverse list */
  if (list->elem) if (list->elem->nr==nr) return (list->elem);
  list=list->next;
 }
 return (NULL);
} /*******************************************************************************/

static int PARA_LEVEL,PARA_NUMBER;

static int GetParaNumberList(CCSymbolList *list,int d_nr,int help)
{ /* Returns number of parameter, which include dollarvar d_nr *******************/
 while (list) {                                                 /* Traverse list */
  if (help>0) return (help);
  if (list->symbol) {                                       /* exists a symbol ? */
   switch (list->symbol->type) {
    case CCTYPE_DOLLARVAR : 
     if (PARA_LEVEL==0) PARA_NUMBER++;
     if (d_nr==atoi(list->symbol->U.name)) return (d_nr);
     break;
    case CCTYPE_FUNCCALL :
     PARA_LEVEL++;
     if (PARA_LEVEL==1) PARA_NUMBER++;
     help=GetParaNumberList(list->symbol->U.func,d_nr,help);
     PARA_LEVEL--;
     break;
    default : break;
   }
  }
  list=list->next;
 }
 return (help);
} /*******************************************************************************/

static int GetParaNumber(CCode *code,int d_nr)
{ /* Returns number of parameter, which include dollarvar d_nr *******************/
 CCLines *list=code->lines;
 CCLine *line;
 while (list) {                                                 /* Traverse list */
  line=list->line;
  if (line) {                                               /* Codeline exists ? */
   if ((line->name==NULL)&&(line->func)) {                     /* Functioncall ? */
    PARA_LEVEL=PARA_NUMBER=0;
    if (GetParaNumberList(line->func,d_nr,0)>0) return (PARA_NUMBER);
   }
  }
  list=list->next;
 }
 return (0);
} /*******************************************************************************/

static CCSymbol *GetSymbolNumberHlp(CCSymbolList *list,int d_nr)
{ /* Returns CCSymbol of parameter number d_nr ***********************************/
 while (list) {                                                 /* Traverse list */
  if (list->symbol) {                                       /* exists a symbol ? */
   PARA_NUMBER++;
   if (d_nr==PARA_NUMBER) return (list->symbol);
  }
  list=list->next;
 }
 return (NULL);
} /*******************************************************************************/

static CCSymbol *GetSymbolNumber(CCode *code,int d_nr)
{ /* Returns CCSymbol of parameter number d_nr ***********************************/
 CCLines *list=code->lines;
 CCLine *line;
 while (list) {                                                 /* Traverse list */
  line=list->line;
  if (line) {                                               /* Codeline exists ? */
   if ((line->name==NULL)&&(line->func)) {                     /* Functioncall ? */
    PARA_NUMBER=-2;                           /* offset for function paramenters */
    return (GetSymbolNumberHlp(line->func,d_nr));
   }
  }
  list=list->next;
 }
 return (NULL);
} /*******************************************************************************/

static void MkFormatFunction(char *name,SyntaxRule *rule)
{ /* Generate format function structure ******************************************/
 TransformTable *TT;
 CCSymbolList *flist;
 CCSymbol *sy;
 CodeInfo *info;
 int WriteCloseBraket=0;
 int HLP[4]={0,0,0,0};
 DBUG_ENTER("MkFormatFunction");

 name=strchr(name,'_');if (name) name++;                    /* Get fct name only */

 if (name==NULL) DBUG_VOID_RETURN;             /* No a vail create function name */
 if ((TT=IsCreateFunc(name))==NULL) DBUG_VOID_RETURN;      /* No create function */
 if (rule->format==NULL) DBUG_VOID_RETURN;                      /* No formatspec */

 if (TT->nr_formats==0) {     /* No more format alternatives for this function ? */
  ActiveRuleInfo(BUF);                              /* Copy rule infos to buffer */
  fprintf(stderr,"- Error %s :\n",BUF);
  fprintf(stderr,"  No more format alternatives allowed for <create_%s>\n",name);
  DBUG_VOID_RETURN;
 }
 
 TT->nr_formats--;                                 /* Decrease number of formats */

 if (TT->code==NULL) TT->code=MkCodeInfo();                   /* new info header */
 info=TT->code;                                    /* Code information structure */

 flist=rule->format->format;                            /* Pointer to formatlist */

 if (info->insertline==NULL) {                    /* No insert code generation ? */
  sy=GetSymbolNumber(rule->ccode,0);                       /* Get Editinfo value */
  if (sy) {                                              /* Exists this symbol ? */
   if (TT->if_mode) {
    if (strcmp(sy->U.name,"EDIT_DUMMY")!=0)
     sprintf(BUF,"if (node->info==%s) {",sy->U.name);
    else 
     sprintf(BUF,"if ((node->info==EDIT_DUMMY) || (node->info==%d)) {",TT->fct_id);
    TT->if_mode=0;
   } else sprintf(BUF,"else if (node->info==%s) {",sy->U.name);
   AddCodeLine(info,BUF,0);
   WriteCloseBraket=1;
  }
 }

 while (flist) {                                          /* traverse formatlist */
  sy=flist->symbol;
  if (sy!=NULL) {                                           /* Exists a symbol ? */
   switch (sy->type) {
    case CCTYPE_TABSY :
    case CCTYPE_NEWLINESY :
    case CCTYPE_BINDSY :
    case CCTYPE_SETTABSY :
    case CCTYPE_CLEARTABSY :
    case CCTYPE_INDENTSY :
     AddCodeLine(info,NULL,sy->type);
     break;
    case CCTYPE_DOLLARVAR : {
     int nr=atoi(sy->U.name);                         /* Value of dollarvariable */
     AltElem *act_sy=GetAltElem(rule->speclist,nr);
     char *text;
     static char TMP[256];
     if (act_sy!=NULL) {                                      /* Element found ? */
      text=GetSymbolText(act_sy->ELEM.symbol); 
      if (text) {                                             /* Is it a token ? */
       int i=0,len=0;
       while (*text) {                         /* Search for backslash sequences */
        if (*text=='\\') {
         text++;
         sprintf(&TMP[i],"\\%o",*text);            /* Replace it by octal values */
         text++;
         i+=3;
        } else {
         TMP[i]=*text;
         text++; 
         i++;
        }
        len++;
       }
       TMP[i]='\0';                                     /* Text for token in TMP */

       switch (TT->fct_id) {            /* Make code dependent on function class */
        case NT_TERMSY :
         AddCodeLine(info,"print_Token(node->U.str,strlen(node->U.str));",0);
         break; 
        case NT_CONST : case NT_PRIMFUNC : case NT_CLASS :
        case NT_TYPE : case NT_SYMBOL : {
         sy=GetSymbolNumber(rule->ccode,1);
         if (TT->nr_formats==-2) {                       /* First primfunction ? */
          AddCodeLine(info," switch (node->U.value) {",0);
          SetInsertCodeLine(info);
          sprintf(BUF,"  default : print_Token(%cUNKNOWN-SYMBOL%c,16);",34,34);
          AddCodeLine(info,BUF,0);
          AddCodeLine(info," }",0);
         }
         if (sy) {                                      /* Parameter available ? */
          sprintf(BUF,"  case %s : print_Token(%c%s%c,%d);break;",
           sy->U.name,34,TMP,34,len);
          InsertCodeLine(info,BUF,0);
         }
         break;
        }
        default :
         sprintf(BUF," print_Token(%c%s%c,%d);",34,TMP,34,len);
         AddCodeLine(info,BUF,0);
         break;
       } 

      } else {                                /* No token => its a syntax rule ! */
       int hlp=GetParaNumber(rule->ccode,nr);
       int type=TT->node_type;
       if ((hlp<4)&&(HLP[hlp]==0)) {
        HLP[hlp]=hlp;
        if ((type&NODE_NODE)==NODE_NODE) {
         AddCodeLine(info," print_NODE(node->U.one);",0);
        } else if ((type&NODE_NODE2)==NODE_NODE2) {
         sprintf(BUF," print_NODE(node->U.tuple->n%d);",hlp);
         AddCodeLine(info,BUF,0);
        } else if ((type&NODE_NODE3)==NODE_NODE3) {
         sprintf(BUF," print_NODE(node->U.triple->n%d);",hlp);
         AddCodeLine(info,BUF,0);
        } else {
         sprintf(BUF," print_NORMAL(node); /* AltElement = %s */",act_sy->ELEM.symbol);
         AddCodeLine(info,BUF,0);
        }
       }
      }
     }
     break;
    }
    default : break;
   }
  }
  flist=flist->next;                                              /* Next symbol */
 }
 
 if (WriteCloseBraket) {
  /*
  ** sprintf(BUF,"} else printf(%c** UNKNOWN NODEINFO IN <%s> **%c);",34,name,34);
  */
  AddCodeLine(info,"}",0);                                  /* Add one end block */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

CCSymbol *MkCCSymbol(int type,void *ptr)
{ /* Create one typed ccode symbol ***********************************************/
 CCSymbol *result=NULL;
 DBUG_ENTER("MkCCSymbol");
 result=(CCSymbol *)malloc(sizeof(CCSymbol));
 DBUG_ASSERT(result);
 result->type=type;                                           /* Set symbol type */
 switch (type) {                             /* Select action for specified type */
  case CCTYPE_REPEATLIST :
   result->U.number=atoi((char *)ptr);
   break;
  case CCTYPE_DOLLARSY :                                  /* NOT YET IMPLEMENTED */
   result->U.name=NULL;
   break;
  case CCTYPE_DOLLARVAR :
   result->U.name=(char *)my_strdup(ptr);
   break;
  case CCTYPE_NUMBER :
   result->U.number=*((int *)ptr);
   break;
  case CCTYPE_STRING :
   result->U.name=(char *)my_strdup(ptr);
   break;
  case CCTYPE_IDENTIFIER :
   result->U.name=(char *)my_strdup(ptr);
   break;
  case CCTYPE_FUNCCALL :
   result->U.func=ptr;
   break;
  case CCTYPE_CCODE :
   result->U.name=(char *)my_strdup(ptr);
   break;
  default : break;
 }
 DBUG_RETURN(result);
} /*******************************************************************************/

CCLine *MkCCLine(CCSymbol *symbol,CCSymbolList *list)
{ /* Create one CCode transformation rule entry **********************************/
 CCLine *result=NULL;
 DBUG_ENTER("MkCCLine");
 result=(CCLine *)malloc(sizeof(CCLine));
 DBUG_ASSERT(result);
 result->name=symbol;
 result->func=list;
 DBUG_RETURN(result);
} /*******************************************************************************/

CCode *MkCCode(CCLines *clines)
{ /* Transformation functions for one syntax rule ********************************/
 CCode *result=NULL;
 DBUG_ENTER("MkCCSymbol");
 result=(CCode *)malloc(sizeof(CCode));
 DBUG_ASSERT(result);
 result->lines=clines;
 DBUG_RETURN(result);
} /*******************************************************************************/

static CCSymbolList *CopyCCSymbolList(CCSymbolList *list)
{ /* Copy one CCSymbolList *******************************************************/
 CCSymbolList *result,*tmp;
 if (list==NULL) result=tmp=NULL;
 else result=tmp=(CCSymbolList *)malloc(sizeof(CCSymbolList));
 while (list) {
  tmp->symbol=CopyCCSymbol(list->symbol);
  if (list->next!=NULL) {
   tmp->next=(CCSymbolList *)malloc(sizeof(CCSymbolList));
   tmp=tmp->next;
  } else tmp->next=NULL;
  list=list->next;
 }
 return(result);
} /*******************************************************************************/

static CCSymbol *CopyCCSymbol(CCSymbol *symbol)
{ /* Copy one CCSymbol ***********************************************************/
 CCSymbol *result=NULL;
 if (symbol) {
  result=(CCSymbol *)malloc(sizeof(CCSymbol));
  result->type=symbol->type;
  switch (symbol->type) {
   case CCTYPE_NUMBER : break;
   case CCTYPE_FUNCCALL :
    result->U.func=CopyCCSymbolList(symbol->U.func);
    break;
   default :
    result->U.name=(char *)my_strdup(symbol->U.name);
    break;
  }
 }
 return (result);
} /*******************************************************************************/

static CCLines *CopyCCLines(CCLines *lines)
{ /* Copy CCLines structure ******************************************************/
 if (lines) {
  CCLines *result,*tmp;
  result=tmp=(CCLines *)malloc(sizeof(CCLines));
  while (lines) {
   if (lines->line) {
    tmp->line=(CCLine *)malloc(sizeof(CCLine));
    tmp->line->name=CopyCCSymbol(lines->line->name);
    tmp->line->func=CopyCCSymbolList(lines->line->func);
   } else tmp->line=NULL;
   if (lines->next==NULL) tmp->next=NULL;
   else {
    tmp->next=(CCLines *)malloc(sizeof(CCLines));
    tmp=tmp->next;
   }
   lines=lines->next;
   return(result);
  }
 }
 return (NULL);
} /*******************************************************************************/

CCode *CopyCCode(CCode *code)
{ /* Copy CCode structure ********************************************************/
 CCode *result;
 DBUG_ENTER("CopyCCode");
 if (code) {
  result=(CCode *)malloc(sizeof(CCode));
  result->lines=CopyCCLines(code->lines); 
 } else result=NULL;
 DBUG_RETURN(result);
} /*******************************************************************************/

CCSymbolList *JoinCCSymbolLists(CCSymbolList *list1,CCSymbolList *list2)
{ /* Join two lists **************************************************************/
 DBUG_ENTER("JoinCCSymbolList");

 if (list1==NULL) DBUG_RETURN(list2);
 if (list2==NULL) DBUG_RETURN(list1);
 list1->next=list2;

 DBUG_RETURN(list1);
} /*******************************************************************************/

CCSymbolList *AddCCSymbolList(CCSymbolList *list,CCSymbol *symbol)
{ /* Add one CCode element to a symbol list **************************************/
 CCSymbolList *result=NULL;
 DBUG_ENTER("AddCCSymbolList");
 if (list==NULL) {                                               /* Empty list ? */
  result=MkCCSymbolList(symbol);                     /* Create a new symbol list */
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((CCSymbolList *)GetEndOfList(list))->next=MkCCSymbolList(symbol);
  UpDateLER(list,result);                                  /* Update new listend */
  DBUG_RETURN(list);
 }
} /*******************************************************************************/

CCSymbolList *MkFuncCall(CCSymbol *symbol,CCSymbolList *list)
{ /* Create a function call structure ********************************************/
 CCSymbolList *result;
 DBUG_ENTER("MkFuncCall");
 result=MkCCSymbolList(symbol);                                /* Make list head */
 result->next=list;                                           /* Append old list */
 DBUG_RETURN(result);
} /*******************************************************************************/

CCLines *AddCCLines(CCLines *lines,CCLine *line)
{ /* Add one transformation rule to the list *************************************/
 CCLines *result=NULL;
 DBUG_ENTER("AddCCLines");
 if (lines==NULL) {                                              /* Empty list ? */
  result=MkCCLines(line);                              /* Create a new line list */
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((CCLines *)GetEndOfList(lines))->next=MkCCLines(line);
  UpDateLER(lines,result);                                 /* Update new listend */
  DBUG_RETURN(lines);
 }
} /*******************************************************************************/

FormatStr *MkFormatStr(CCSymbolList *format)
{ /* Create outputformat for one rule ********************************************/
 FormatStr *result=NULL;
 DBUG_ENTER("MkFormatStr");
 result=(FormatStr *)malloc(sizeof(FormatStr));
 DBUG_ASSERT(result);                             /* Test memory allocation done */
 result->format=format;
 DBUG_RETURN(result);
} /*******************************************************************************/

void FreeFormatStr(FormatStr *format)
{ /* Release allocated memory used by one format rule ****************************/
 DBUG_ENTER("FreeFormatStr");
 if (format) {
  DBUG_PRINT("FREE FORMATSTR !",("format=%p",format));
  FreeCCSymbolList(format->format);
  FREE(format);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeCCSymbol(CCSymbol *symbol)
{ /* Release symbol memory *******************************************************/
 DBUG_ENTER("FreeCCSymbol");
 if (symbol) {
  DBUG_PRINT("FREE SYMBOL !",("symbol=%p",symbol));
  switch (symbol->type) {                    /* Select action for specified type */
                            /*case CCTYPE_DOLLARVAR : FREE(symbol->U.name);break;*/
                             /*case CCTYPE_DOLLARSY : FREE(symbol->U.name);break;*/
   case CCTYPE_STRING : 
    FREE(symbol->U.name);
    break;
   case CCTYPE_IDENTIFIER :
    FREE(symbol->U.name);
    break;
   case CCTYPE_FUNCCALL :
    FreeCCSymbolList(symbol->U.func);
    break;
   default : break;                            /* Error case, actually no action */
  }
  FREE(symbol);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeCCSymbolList(CCSymbolList *list)
{ /* Release memory for complete symbollist **************************************/
 CCSymbolList *hlp;
 DBUG_ENTER("FreeCCSymbolList");
 while (list) {                                              /* List not empty ? */
  DBUG_PRINT("FREE CCSYMBOLLIST !",("list=%p",list));
  FreeCCSymbol(list->symbol);
  hlp=list;
  CancelLERList(list);
  list=list->next;
  FREE(hlp);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeCCLine(CCLine *line)
{ /* Release memory for one transformation rule **********************************/
 DBUG_ENTER("FreeCCLine");
 if (line) {
  FreeCCSymbol(line->name);
  FreeCCSymbolList(line->func);
  FREE(line);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeCCLines(CCLines *lines)
{ /* Release complete list of transformation rules *******************************/
 CCLines *hlp;
 DBUG_ENTER("FreeCCLines");
 DBUG_PRINT("FREE CCLINES !",("lines=%p",lines));
 while (lines) {                                             /* List not empty ? */
  FreeCCLine(lines->line);
  hlp=lines;
  lines=lines->next;
  CancelLERList(hlp);
  FREE(hlp);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeCCode(CCode *code)
{ /* Release all memory for ccode rules ******************************************/
 DBUG_ENTER("FreeCCode");
 if (code) {
  FreeCCLines(code->lines);
  FREE(code);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void PrFuncCallParas(FILE *FH,CCSymbolList *paras,int nr)
{ /* Writes code for a c functioncall ********************************************/
 while (paras) {                                    /* Parameterlist not empty ? */
  CCSymbol *symbol=paras->symbol;
  if (symbol) {
   switch (symbol->type) {
    case CCTYPE_FUNCCALL :
     fprintf(FH,"udi_%s(",symbol->U.func->symbol->U.name);
     PrFuncCallParas(FH,symbol->U.func->next,1);
     fprintf(FH,")");
     break;
    case CCTYPE_DOLLARVAR :
     fprintf(FH,"$%s",symbol->U.name);
     break;
    case CCTYPE_NUMBER :
     fprintf(FH,"%d",symbol->U.number);
     break;
    default :
     if (nr==1) {
      if (!IsEditInfo(symbol->U.name)) {              /* Unknow editinfo value ? */
       ActiveRuleInfo(BUF);                         /* Copy rule infos to buffer */
       fprintf(stderr,"- Error %s :\n",BUF);
       fprintf(stderr,"  Unknown editinformation value <%s>\n",symbol->U.name);
      }
     }
     fprintf(FH,"%s",symbol->U.name);
     break;
   }
   if (paras->next!=NULL) fprintf(FH,",");
  }
  nr++;
  paras=paras->next;
 }
} /*******************************************************************************/

void PrCCode(FILE *FH,CCode *code,char *rulename,void *rule)
{ /* Write CCode for transformation rules ****************************************/
 CCLine *line;

 if (code==NULL) {                                              /* CCode given ? */
  fprintf(FH," /* No C-Code */ ");
  return;
 }
 if (code->lines==NULL) {                                    /* Lines definied ? */
  fprintf(FH," /* No transformation lines */ ");
  return;
 }
 if (code->lines->line==NULL) {
  fprintf(FH," /* end of c-code */ ");
  return;
 }

 line=code->lines->line;

 if (line->name) {
  if (line->func) {                              /* Functioncall with variable ? */
  } else {                                         /* Simple transformation func */
   SpecList *tmp_sl;
   char buf[16];
   int nr;
   fprintf(FH,"{ ");
   switch (line->name->type) {
    case CCTYPE_IDENTIFIER : {
     SimpleTrans *hlp=IsSimpleTransFunc(line->name->U.name);
     char *scode=NULL;
     switch (hlp->func_ID) {
      case FCTID_NONE :
       sprintf(buf,"$$=NULL;");
       scode=buf;
       break;
      case FCTID_FIRST_RULE :
       tmp_sl=((SyntaxRule *)rule)->speclist;
       nr=0;
       while (tmp_sl) {
        nr++;
        if (tmp_sl->elem->type==RUL_TYP) {
         sprintf(buf,"$$=$%d; ",nr);
         scode=buf;
         break;
        }
        tmp_sl=tmp_sl->next;
       }
       break;
      default :
       scode=hlp->code;
       break;
     }
     if (hlp) fprintf(FH,"%s; ",scode);
     else fprintf(FH,"/* ERR: Illegal SimpleTransFct <%s> */ ",line->name->U.name);
     break;
    }
    case CCTYPE_CCODE :
     fprintf(FH,"%s",code->lines->line->name->U.name);
     break;
    default : break;
   }
   fprintf(FH,"}");
  }
 } else {                                                  /* line->name == NULL */
  if (line->func) {                                     /* normal functioncall ? */
   fprintf(FH,"{ $$=udi_");
   fprintf(FH,"%s(",line->func->symbol->U.name);
   PrFuncCallParas(FH,line->func->next,1);
   MkFormatFunction(line->func->symbol->U.name,(SyntaxRule *)rule); 
   fprintf(FH,"); ");
   fprintf(FH," }");
  }
 }

} /*******************************************************************************/
