/**********************************************************************************
***********************************************************************************
**
**   File        : gen-parser.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 09.02.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for parsergeneration
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
#include "list-modu.h"
#include "err-modu.h"

#include "trans-tbl.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define N_RULE "X_"

#define LIST_TRAV  0
#define LIST_WRITE 1

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static void PrSpecList(SpecList *list);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static FILE *FH;                            /* Global file handle to write rules */
static int TravMODE=LIST_TRAV;                        /* Mode for list traversal */
static SyntaxRule *ASR=NULL;                        /* Actual active syntax rule */

static char DELIS_BUF[1024];                        /* Buffer for list delimiter */

/*********************************************************************************/
/********** GLOBAL VARIABLES IN MODULE *******************************************/
/*********************************************************************************/

SyntaxRuleList *SRules=NULL;                                /* List of all rules */
StringStructList *StringList=NULL;                /* List of string rule symbols */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCAL FUNCTIONS ***********************************/
/*********************************************************************************/

static SRuleList *MkSRuleList(SyntaxRule *rule)
{ /* Creates a list of syntax rules with one entry *******************************/
 SRuleList *result=NULL;
 DBUG_ENTER("MkSRuleList");

 result=(SRuleList *)malloc(sizeof(SRuleList));
 DBUG_ASSERT(result);
 result->srule=rule;                /* Stores pointer to contents of syntax rule */
 result->next=NULL;

 DBUG_RETURN(result);
} /*******************************************************************************/

static SyntaxRuleList *MkSyntaxRuleList(SyntaxRuleHeader *header)
{ /* Creates a new list with one header element **********************************/
 SyntaxRuleList *result=NULL;
 DBUG_ENTER("MkSyntaxRuleList");
 
 result=(SyntaxRuleList *)malloc(sizeof(SyntaxRuleList));
 DBUG_ASSERT(result);                             /* Test memory allocation done */
 result->rule=header;
 result->next=NULL;

 DBUG_RETURN(result);
} /*******************************************************************************/

static SpecList *MkSpecList(AltElem *elem)
{ /* Allocates one list entry ****************************************************/
 SpecList *result=NULL;
 DBUG_ENTER("MkSpecList");
 result=(SpecList *)malloc(sizeof(SpecList));
 DBUG_ASSERT(result);
 result->elem=elem;                                               /* store entry */
 result->next=NULL;                                               /* End of list */
 DBUG_RETURN(result);
} /*******************************************************************************/

static AltList *MkAltList(SpecList *slist)
{ /* Allocates one list entry ****************************************************/
 AltList *result=NULL;
 DBUG_ENTER("MkAltList");
 result=(AltList *)malloc(sizeof(AltList));
 DBUG_ASSERT(result);
 result->list=slist;                                              /* store entry */
 result->next=NULL;                                               /* End of list */
 DBUG_RETURN(result);
} /*******************************************************************************/

static void WritePrec(FILE *fh,TokenList *list,char *text)
{ /* Write a precedence rule *****************************************************/
 if (list) {
  fprintf(fh,"%s",text);
  while (list) {
   if (list->token) fprintf(fh,"%s ",list->token);
   else fprintf(fh,"\n%s",text);
   list=list->next;
  }
  fprintf(fh,"\n");
 }
} /*******************************************************************************/

static void MkCountRuleCode(char *rulename,AltElem *elem)
{ /* Create a syntaxrule to count symbols ****************************************/
 int elem_type;
 DBUG_ENTER("MkCountRule");

 elem_type=LookUpSymbol(elem->ELEM.symbol);
 if (elem_type==0) elem_type=RUL_TYP;
 else elem_type=TOK_TYP;

 SRules=AddSyntaxRuleList(
  SRules,
  MkSyntaxRuleHeader(
   rulename,
   SRULETYPE_INT,
   AddSRuleList(
    AddSRuleList(NULL,                                             /* List head */
     MkSyntaxRule(
      AddSpecList(NULL,MkAltElem(elem_type,my_strdup(elem->ELEM.symbol))),
      MkCCode(
       AddCCLines(NULL,MkCCLine(MkCCSymbol(CCTYPE_CCODE,my_strdup("$$=1;")),NULL))
      ),
      NULL                                              /* No format info needed */
     )
    ),
    MkSyntaxRule(
     AddSpecList(
      AddSpecList(NULL,MkAltElem(RUL_TYP,my_strdup(rulename))),
      MkAltElem(elem_type,my_strdup(elem->ELEM.symbol))
     ),
     MkCCode(
      AddCCLines(NULL,
       MkCCLine(
        MkCCSymbol(CCTYPE_CCODE,my_strdup("$$=$1+1;")),NULL))),
     NULL                                               /* No format info needed */
    )
   ),
   SRules->active->LineNumber,
   SRules->active->RefLevel+1
  )
 );

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static SpecList *ReversList(TokenList *list,SpecList *sl,int *counter)
{ /* Returns list in revers order ************************************************/
 if (list) {
  ReversList(list->next,sl,counter);
  strcat(DELIS_BUF,GetSymbolText(list->token));
  sl=AddSpecList(sl,MkAltElem(list->type,my_strdup(list->token)));
  (*counter)++;
 }
 return(sl);
} /*******************************************************************************/

static void MkListRuleCode(char *rulename,AltElem *elem)
{ /* Generate rule and code for lists ********************************************/
 ListRule *lr=elem->ELEM.rlist;                  /* Pointer to listrulestructure */
 SRuleList *srl;
 SpecList *sl;
 int counter;
 char buf[256],buf2[128];
 char *p1=NULL,*p2=NULL;
 int SetType=1;
 DBUG_ENTER("MkListRule");

 if (lr->min<1) {                                           /* Empty list code ? */
  srl=AddSRuleList(NULL,                                            /* List head */
   MkSyntaxRule(NULL,                                           /* No specifiers */
    MkCCode(
     AddCCLines(NULL,                                         /* First CCodeline */
      MkCCLine(
       MkCCSymbol(CCTYPE_CCODE,my_strdup("$$=udi_Mk_EmptyList();")),NULL))),
    NULL                                                /* No format info needed */
   )
  );
  sprintf(buf,"%s_H",rulename);
  srl=AddSRuleList(
   srl,                                                   /* Append to this list */
   MkSyntaxRule(
    AddSpecList(NULL,MkAltElem(RUL_TYP,my_strdup(buf))),
    MkCCode(
     AddCCLines(NULL,
      MkCCLine(
       MkCCSymbol(CCTYPE_CCODE,my_strdup("$$=$1;")),NULL))),
    NULL                                                /* No format info needed */
   )
  );
  SRules=AddSyntaxRuleList(SRules,MkSyntaxRuleHeader(rulename,SRULETYPE_LIST,srl,
   SRules->active->LineNumber,SRules->active->RefLevel+1));
  rulename=buf;
 }
 
 if (ASR->ccode && ASR->ccode->lines && ASR->ccode->lines->line) {
  CCSymbolList *tmp=ASR->ccode->lines->line->func;
  if (tmp) {
   p1=my_strdup(tmp->next->symbol->U.name);
   p2=my_strdup(tmp->next->next->symbol->U.name);
  }
 }


 if (p1&&p2&&strcmp(p2,"NULL")&&((SetType=strcmp(p2,"SET_TYPE"))!=0))
  sprintf(buf2,"$$=udi_Mk_ListHead(udi_%s(%s,$1));",p2,p1);
 else sprintf(buf2,"$$=udi_Mk_ListHead($1);");
 
 srl=AddSRuleList(NULL,                                            /* List head */
  MkSyntaxRule(
   AddSpecList(NULL,MkAltElem(lr->type,my_strdup(lr->elem))),
   MkCCode(
    AddCCLines(NULL,                                          /* First CCodeline */
     MkCCLine(
      MkCCSymbol(CCTYPE_CCODE,my_strdup(buf2)),NULL))),
   NULL                                                /* No format info needed */
  )
 );

 sl=AddSpecList(NULL,MkAltElem(RUL_TYP,my_strdup(rulename)));
 counter=2;
 strcpy(DELIS_BUF,"");
 sl=ReversList(lr->delis,sl,&counter); 
 sl=AddSpecList(sl,MkAltElem(lr->type,my_strdup(lr->elem)));

 if (p1) {
  AddListCase(ASR,p1,DELIS_BUF);
  if (p2 && strcmp(p2,"NULL")&&((SetType=strcmp(p2,"SET_TYPE"))!=0))
   sprintf(buf2,"$$=udi_Mk_ListTail($1,udi_%s(%s,$%d));",p2,p1,counter);
  else sprintf(buf2,"$$=udi_Mk_ListTail($1,$%d);",counter);
 }
 
 srl=AddSRuleList(
  srl,                                                   /* Append to this list */
  MkSyntaxRule(
   sl,
   MkCCode(
    AddCCLines(NULL,
     MkCCLine(
      MkCCSymbol(CCTYPE_CCODE,my_strdup(buf2)),NULL))),
   NULL                                                /* No format info needed */
  )
 );

 SRules=AddSyntaxRuleList(SRules,MkSyntaxRuleHeader(rulename,SRULETYPE_LIST,srl,
  SRules->active->LineNumber,SRules->active->RefLevel+1));

 FreeCCode(ASR->ccode);

 if (SetType==0) {
  sprintf(buf2,"$$=udi_Mk_SetListType($1,%s);",p1);
 } else strcpy(buf2,"$$=$1;");

 ASR->ccode=MkCCode(
  AddCCLines(NULL,
   MkCCLine(MkCCSymbol(CCTYPE_CCODE,my_strdup(buf2)),NULL)
  )
 );

 SRules->active->RuleType=SRULETYPE_LIST;                     /* Setup rule type */
 
 if (p1) free(p1);
 if (p2) free(p2);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

/** Print syntax structure  ******************************************************/

static SRuleList *BuildAltList(int type,SRuleList *slist,AltList *list)
{
 AltList *ptr;
 CCode *cc=NULL;
 int nr;
 char buf[16];
 SpecList *tmp_sl=list->list;
 DBUG_ENTER("BuildAltList");
 if (type==ALT_TYP) {
  cc=CopyCCode(ASR->ccode);
 } 
 while (list) {                                              /* List not empty ? */
  ptr=list;
  if (type==OPT_TYP) {
   cc=NULL;
   nr=0;
   tmp_sl=list->list;
   while (tmp_sl) {
    nr++;
    if (tmp_sl->elem->type==RUL_TYP) {
     sprintf(buf,"$$=$%d; ",nr);
     cc=MkCCode(
      AddCCLines(NULL,
      MkCCLine(MkCCSymbol(CCTYPE_CCODE,buf),NULL)
      )
     );
     break;
    }
    tmp_sl=tmp_sl->next;
   }
  }
  slist=AddSRuleList(slist,MkSyntaxRule(list->list,cc,NULL));
  list=list->next;
  free(ptr);                                                 /* Discard old list */
 }
 DBUG_RETURN(slist);
} /*******************************************************************************/

static void IncreaseDollarVar(CCode *code)
{ /* Increase value of dollarvariables *******************************************/
 if (code) {
  CCLines *lines=code->lines;
  while (lines) {
   if (lines->line) {
    if (lines->line->func) {
     CCSymbolList *list=lines->line->func;
     while (list) {
      if ((list->symbol)&&(list->symbol->type==CCTYPE_DOLLARVAR)) {
       int value=atoi(list->symbol->U.name);
       char buf[8];
       value++;
       free(list->symbol->U.name);
       sprintf(buf,"%d",value);
       list->symbol->U.name=(char *)my_strdup(buf); 
       break;
      }
      list=list->next;
     }
    }
   }
   lines=lines->next;
  }
 }
} /*******************************************************************************/

static void PrSymbolCode(char *symbol)
{ /* Writes code for symbol to file **********************************************/
 StringStructList *list=StringList;
 int r;
 if (TravMODE) fprintf(FH,"%s ",symbol);
 while (list) {
  if (strcmp(list->elem->start,symbol)==0) {
   r=IsStopCharToken(list->elem->token);
   if (r>=0) {
    if (TravMODE) {
     fprintf(FH,"{ _GCM_=%d;_GCI_[%d].level++; } ",r,r);
     IncreaseDollarVar(ASR->ccode);
    }
   }
  } else if (strcmp(list->elem->end,symbol)==0) {
   r=IsStopCharToken(list->elem->token);
   if (r>=0) {
    if (TravMODE) fprintf(FH,"{ _GCI_[%d].level--; } ",r);
   }
  }
  list=list->next;
 }
} /*******************************************************************************/

static void PrAltElem(AltElem *elem)
{
 DBUG_ENTER("PrAltElem");
 if (elem) {
  if (elem->type&SYM_TYP) {                         /* Is it a terminal symbol ? */
   PrSymbolCode(elem->ELEM.symbol);
   DBUG_VOID_RETURN;
  } else {
   char *buf=(char *)malloc(sizeof(N_RULE)+strlen(SRules->active->RuleName)+64);
   if (SRules->active->RefLevel==0) {
    sprintf(buf,"%s%s%d",SRules->active->RuleName,
	    N_RULE,SRules->active->RefCount++);
   } else {
    sprintf(buf,"%s%d",SRules->active->RuleName,SRules->active->RefCount++);
   }
   if (elem->type==CNT_TYP) {
    MkCountRuleCode(buf,elem);
   } else if (elem->type==REP_TYP) {
    SRuleList *hlp=AddSRuleList(NULL,MkSyntaxRule(NULL,NULL,NULL));
    SpecList *new=(SpecList *)malloc(sizeof(SpecList));
    new->elem=MkAltElem(RUL_TYP,buf);
    new->next=elem->ELEM.list->list;
    elem->ELEM.list->list=new;
    hlp=BuildAltList(RUL_TYP,hlp,elem->ELEM.list);
    SRules=AddSyntaxRuleList(SRules,MkSyntaxRuleHeader(buf,SRULETYPE_STANDART,hlp,
     SRules->active->LineNumber,SRules->active->RefLevel+1));
   } else if (elem->type==OPT_TYP) {         /* Create new rules for optionals ? */
    SRuleList *hlp=AddSRuleList(NULL,
     MkSyntaxRule(NULL,
      MkCCode(
       AddCCLines(NULL,
        MkCCLine(MkCCSymbol(CCTYPE_CCODE,"$$=NULL; "),NULL)
       )
      ),
      NULL
     )
    );
    hlp=BuildAltList(OPT_TYP,hlp,elem->ELEM.list);
    SRules=AddSyntaxRuleList(SRules,MkSyntaxRuleHeader(buf,SRULETYPE_STANDART,hlp,
     SRules->active->LineNumber,SRules->active->RefLevel+1));
   } else if (elem->type==ALT_TYP) {
    SRuleList *hlp=BuildAltList(ALT_TYP,NULL,elem->ELEM.list);
    SRules=AddSyntaxRuleList(SRules,MkSyntaxRuleHeader(buf,SRULETYPE_STANDART,hlp,
     SRules->active->LineNumber,SRules->active->RefLevel+1));
   } else if (elem->type==LST_TYP) {                     /* Generate list code ? */
    MkListRuleCode(buf,elem);                /* Generate rule and code for lists */
   } else {
    free(buf);
    DBUG_VOID_RETURN;
   }
   elem->type=RUL_TYP;                                          /* Call new rule */
   elem->ELEM.symbol=buf;                                     /* Store rule name */
   PrAltElem(elem);
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void PrSpecList(SpecList *list)
{
 DBUG_ENTER("PrSpecList");
 while (list!=NULL) {                                        /* List not empty ? */
  PrAltElem(list->elem);
  list=list->next;
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/
 
static void PrSyntaxRule(SyntaxRule *rule,char *rulename)
{
 DBUG_ENTER("PrSyntaxRule");
 ASR=rule;                             /* Set pointer to actual used syntax rule */
 if (rule) {
  if (rule->speclist==NULL) {
   if (TravMODE) fprintf(FH,"/* Empty rule */");
  } else {
   PrSpecList(rule->speclist);
  }
  if (TravMODE) PrCCode(FH,rule->ccode,rulename,rule);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void PrSRuleList(SRuleList *list,char *rulename)
{
 DBUG_ENTER("PrSRuleList");
 while (list!=NULL) {                                        /* List not empty ? */
  PrSyntaxRule(list->srule,rulename);
  list=list->next;                                                /* Next please */
  if (list!=NULL) if (TravMODE) fprintf(FH,"\n | ");
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void PrSyntaxRuleHeader(SyntaxRuleHeader *header)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 DBUG_ENTER("PrSyntaxRuleHeader");
 if (TravMODE) fprintf(FH,"%s : ",header->RuleName);
 DBUG_PRINT("PrSyntaxRuleHeader",("NAME = %s",header->RuleName));
 PrSRuleList(header->synlist,header->RuleName);
 if (TravMODE) fprintf(FH,"\n ;\n\n");
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void PrSyntaxRuleList(int mode,FILE *fh,SyntaxRuleList *list)
{ /* Writes complete syntax description ******************************************/
 SyntaxRuleList *ptr=list;                                             /* Backup */
 DBUG_ENTER("PrSyntaxRuleList");
 FH=fh;
 TravMODE=mode;                                              /* Set travers mode */
 while (list!=NULL) {                                        /* List not empty ? */
  ptr->active=list->rule;                          /* Set pointer to actual rule */
  PrSyntaxRuleHeader(list->rule);
  list=list->next;                                                /* Next please */
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void InitParserGenerator(void)
{ /* Setup ParserGenerator, will be done from InitGeneratorOptions ! *************/
 DBUG_ENTER("InitParserGenerator");
 InitLER();                              /* SetUp end of list remember structure */
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void WriteParserTypes(FILE *fh)
{ /* Writes type declarations for syntax rules ***********************************/
 fprintf(fh," int INTEGERtype; /* Used for countrules, etc. */\n");
 fprintf(fh," NODE *NODETYPE; /* Used for standart nodes */\n");
} /*******************************************************************************/

static void WriteParserRuleTypes(FILE *fh)
{                                     /* Write types definitions for syntaxrules */
 SyntaxRuleList *tmp=SRules;
 int last_type=-1;
 DBUG_ENTER("WriteParserRuleTypes");
 while (tmp) {
  if (tmp->rule) {
   if (tmp->rule->RuleType!=last_type) {
    switch (tmp->rule->RuleType) {
     case SRULETYPE_INT :
      fprintf(fh,"\n%%type <INTEGERtype>");
      break;
     default :
      fprintf(fh,"\n%%type <NODETYPE>");
      break;
    }
    last_type=tmp->rule->RuleType;                            /* Store rule type */
   }
   fprintf(fh," %s",tmp->rule->RuleName);
  }
  tmp=tmp->next;
 }
 fprintf(fh,"\n");
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void WriteParserCode(void)
{ /* Generates code  for a parser ************************************************/
 char buffer[256];
 FILE *output;
 DBUG_ENTER("WriteParserCode");
 sprintf(buffer,"%s%s",Project_OPT.name,PARSER_EXTENDER);

 if ((output=fopen(buffer,"w"))==NULL) {
  fprintf(stderr,"Can't open file %s for parser output !\n",buffer);
 } else {
  PrSyntaxRuleList(LIST_TRAV,output,SRules);                      /* Write rules */
  fprintf(output,"%%pure_parser\n\n");
  fprintf(output,"%%{\n");
  fprintf(output,"#include %cmarco-create.h%c\n",34,34);
  WriteScannerExportVars(output);
  fprintf(output,"%%}\n");
  WriteToken(output);                                /* Write list of used token */
  WriteScannerUnionMembers(output);                       /* Write union members */
  WriteParserRuleTypes(output);       /* Write types definitions for syntaxrules */
  WritePrec(output,Parser_OPT.left_prec,"%left ");     /* Write precedence token */
  WritePrec(output,Parser_OPT.right_prec,"%right ");
  WritePrec(output,Parser_OPT.nonassoc_prec,"%nonassoc ");
  fprintf(output,"\n%%start ProgramGoalExpr\n\n");
  fprintf(output,"%%%%\n\n");
  if (Parser_OPT.expr) {
   fprintf(output,"ProgramGoalExpr : %s { GOALEXPR=$1;",Parser_OPT.expr);
  } else { /* First expression is start expression */
   fprintf(output,"ProgramGoalExpr : %s { GOALEXPR=$1;",
    SRules->rule->RuleName);
  }
  fprintf(output,"NR_of_EXPRs--;if (NR_of_EXPRs==0) {YYACCEPT;}}\n\n");
  PrSyntaxRuleList(LIST_WRITE,output,SRules);                     /* Write rules */
  fprintf(output,"\n%%%%\n\n");                           /* End of syntax rules */
  if (Project_OPT.prtest) {                 /* Generate a parser test function ? */
   fprintf(output,"void main(void)\n{\n");
   fprintf(output,"#ifdef YYDEBUG\n yydebug=1;\n#endif\n");
   fprintf(output," yyparse();\n}\n");
  }
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void CleanUpParserGenerator(void)
{ /* Free all memory, will done be from ReleaseGeneratorOptions ! ****************/
 DBUG_ENTER("CleanUpParserGenerator");
 FreeSyntaxRuleList(SRules);                                /* Release all rules */
 DeleteLER();                       /* Delete memory allocated by LER structures */
 SRules=NULL;
 DBUG_VOID_RETURN;
} /*******************************************************************************/

SyntaxRule *MkSyntaxRule(SpecList *list,CCode *code,FormatStr *fstr)
{ /* Constructor for one syntax rule in this language ****************************/
 SyntaxRule *result=NULL;
 DBUG_ENTER("MkSyntaxRule");

 result=(SyntaxRule *)malloc(sizeof(SyntaxRule));
 DBUG_ASSERT(result);                             /* Test memory allocation done */

 DBUG_PRINT("MkSyntaxRule",(""));

 result->speclist=list;                              /* Store symbollist pointer */
 result->ccode=code;            /* Store pointer to C-Code information structure */
 result->format=fstr;          /* Store pointer to info about syntax-expr format */

 if (list!=NULL) {                                        /* Exists a SpecList ? */
  result->list_len=GetDimOfList(list);                        /* Speclist length */
  DBUG_PRINT("LIST != NULL",("Listlen=%d",result->list_len));
  if (result->list_len>0) {                                  /* Rule not empty ? */
   int i;
   result->SpecTypes=(int *)malloc(sizeof(int)*result->list_len);
   for (i=0;i<result->list_len;i++) {
     DBUG_PRINT("INSIDE FOR",("i=%d result->SpecTypes[i]=%d",i,list->elem->type));
     result->SpecTypes[i]=list->elem->type; 
     list=list->next;
   }
  }
 }

 DBUG_RETURN(result);
} /*******************************************************************************/

SRuleList *AddSRuleList(SRuleList *list,SyntaxRule *rule)
{ /* Adds one syntax rules to an existing list of syntax rules *******************/
 SRuleList *result=NULL;
 DBUG_ENTER("AddSRuleList");

 if (list==NULL) {                                               /* Empty list ? */
  result=MkSRuleList(rule);                                 /* Create a new list */
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((SRuleList *)GetEndOfList(list))->next=MkSRuleList(rule);
  UpDateLER(list,result);                                  /* Update new listend */
  DBUG_RETURN(list);
 }

} /*******************************************************************************/

SyntaxRuleHeader *MkSyntaxRuleHeader(char *name,int RuleType,
				     SRuleList *list,int line_nr,int L)
{ /* Creates header for a list of grammatical different syntax alternatives ******/
 SyntaxRuleHeader *result=NULL;
 DBUG_ENTER("MkSyntaxRuleHeader");

 result=(SyntaxRuleHeader *)malloc(sizeof(SyntaxRuleHeader));
 DBUG_ASSERT(result);                             /* Test memory allocation done */
 result->RuleName=(char *)my_strdup(name);                             /* Copy name */
 result->RuleType=RuleType;                         /* store C-Type of this rule */
 result->LineNumber=line_nr;            /* Remember original line number in file */
 result->NrAlts=GetDimOfList(list);                  /* No alternatives definied */
 result->synlist=list;                                     /* Store list pointer */
 result->RefCount=0;                          /* No references to this structure */
 result->RefLevel=L;                                   /* Level of derived rules */

 DBUG_RETURN(result);
} /*******************************************************************************/

SyntaxRuleList *AddSyntaxRuleList(SyntaxRuleList *list,SyntaxRuleHeader *header)
{ /* Adds rule for one syntax expression to an existing list of rules ************/
 SyntaxRuleList *result=NULL;
 DBUG_ENTER("AddSyntaxRuleList");

 if (list==NULL) {                                               /* Empty list ? */
  result=MkSyntaxRuleList(header);                          /* Create a new list */
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((SyntaxRuleList *)GetEndOfList(list))->next=MkSyntaxRuleList(header);
  UpDateLER(list,result);                                  /* Update new listend */
  DBUG_RETURN(list);
 }

 DBUG_RETURN(result);
} /*******************************************************************************/

ListRule *MkListRule(int min,int max,TokenList *delis,char *elem,int type)
{ /* Creates structure infos for one list rule ***********************************/
 ListRule *result;
 DBUG_ENTER("MkListRule");
 result=(ListRule*)malloc(sizeof(ListRule));
 DBUG_ASSERT(result);
 result->min=min;
 result->max=max;
 if (delis) {                                           /* Delimiter available ? */
  if (strcmp(delis->token,"NULL")==0) {          /* Special token (empty list) ? */
   FreeTokenList(delis);                                    /* Release tokenlist */
   delis=NULL;
  }
 }
 result->delis=delis;
 result->elem=elem;
 result->type=type;
 DBUG_RETURN(result);
} /*******************************************************************************/

AltElem *MkAltElem(int type,void *elem)
{ /* Creates one entry for an alternative list with specified type ****************/
 AltElem *result=NULL;
 DBUG_ENTER("MkAltElem");
 result=(AltElem *)malloc(sizeof(AltElem));                      /* Alloc memory */
 DBUG_ASSERT(result);                                         /* Mem available ? */
 result->type=type;
 if (type&SYM_TYP) {                              /* is elem a symbol (string) ? */
  result->ELEM.symbol=(char *)elem;                              /* store string */
 } if (type==LST_TYP) {                                        /* Limited list ? */
  result->ELEM.rlist=(ListRule *)elem;
 } else if (type==CNT_TYP) {                     /* Create rule to count symbols */
  result->ELEM.symbol=(char *)elem;                         /* store symbol name */
 } else {                                                 /* elem must be a list */
  result->ELEM.list=(AltList*)elem;                     /* remember list pointer */
 }
 DBUG_RETURN(result);
} /*******************************************************************************/

AltList *AddAltList(AltList *alist,SpecList *slist)
{ /* Adds one specificationlist to a list of alternatives ************************/
 AltList *result=NULL;
 DBUG_ENTER("AddAltList");
 if (alist==NULL) {                                              /* Empty list ? */
  result=MkAltList(slist);                                  /* Create a new list */
  result->nr=1;
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((AltList *)GetEndOfList(alist))->next=MkAltList(slist);
  result->nr++;
  UpDateLER(alist,result);                                  /* Update new listend */
  DBUG_RETURN(alist);
 }
} /*******************************************************************************/

SpecList *AddSpecList(SpecList *slist,AltElem *elem)
{ /* Adds one element to specification list **************************************/
 SpecList *result=NULL;
 DBUG_ENTER("AddSpecList");
 if (slist==NULL) {                                              /* Empty list ? */
  result=MkSpecList(elem);                                  /* Create a new list */
  elem->nr=1;
  UpDateLER(result,result);      /* Start of list, remember list and end of list */
  DBUG_RETURN(result);
 } else {
  result=((SpecList *)GetEndOfList(slist));                 /* Last list element */
  elem->nr=result->elem->nr+1;
  result->next=MkSpecList(elem);
  UpDateLER(slist,result->next);                           /* Update new listend */
  DBUG_RETURN(slist);
 }
} /*******************************************************************************/

StringStruct *MkStringStruct(char *token,char *start,char *end,char *MSGBUF)
{ /* Create one String structure *************************************************/
 StringStruct *result;
 DBUG_ENTER("MkStringStruct");
 result=(StringStruct *)malloc(sizeof(StringStruct));
 DBUG_ASSERT(result);
 strcpy(MSGBUF,"");
 if (IsStopCharToken(token)==-1) {
  sprintf(MSGBUF,"No breakchar token identifier named <%s> definied.\n",token);
 }
 if (LookUpSymbol(start)==0) {
  sprintf(MSGBUF,"%sNo string start symbol named <%s> definied.\n",MSGBUF,start);
 }
 if (LookUpSymbol(end)==0) {
  sprintf(MSGBUF,"%sNo string end symbolnamed <%s> definied.\n",MSGBUF,end);
 }
 if (strlen(MSGBUF)>0) {
  free(result);
  result=NULL;
 } else {
  result->token=(char *)my_strdup(token);
  result->start=(char *)my_strdup(start);
  result->end=(char *)my_strdup(end);
 }
 DBUG_RETURN(result);
} /*******************************************************************************/

StringStructList *AddStringStructList(StringStructList *list,StringStruct *elem)
{ /* Adds one Stringstruct to a list *********************************************/
 StringStructList *result;
 DBUG_ENTER("AddStringStructList");
 if (elem==NULL) DBUG_RETURN(list);
 result=(StringStructList *)malloc(sizeof(StringStructList));
 DBUG_ASSERT(result);
 result->elem=elem;
 result->next=list;
 DBUG_RETURN(result);
} /*******************************************************************************/

/*********************************************************************************/
/** List destructors *************************************************************/
/*********************************************************************************/

void FreeStringStruct(StringStruct *ss)
{ /* Release memory of a string structure ****************************************/
 DBUG_ENTER("FreeStringStruct");
 if (ss) {
  if (ss->token) free(ss->token);
  if (ss->start) free(ss->start);
  if (ss->end)   free(ss->end);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeStringStructList(StringStructList *list)
{ /* Release a complete list of string structures (calls FreeStringStruct) *******/
 StringStructList *hlp;
 DBUG_ENTER("FreeStringStructList");
 while (list) {
  FreeStringStruct(list->elem);
  hlp=list;
  list=list->next;
  free(hlp);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeAltList(AltList *list)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 void *ptr;
 DBUG_ENTER("FreeAltList");
 while (list) {                                              /* List not empty ? */
  ptr=list;
  FreeSpecList(list->list);
  list=list->next;
  free(ptr);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeListRule(ListRule *rule)
{ /* Release allocate memory *****************************************************/
 DBUG_ENTER("FreeListRule");
 if (rule) {
  if (rule->delis) FreeTokenList(rule->delis);
  if (rule->elem) free(rule->elem);
  free(rule);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeAltElem(AltElem *elem)
{ /* Release memory of this structure ********************************************/
 DBUG_ENTER("FreeAltElem");
 if (elem) {                                                   /* Pointer okay ? */
  if ((elem->type&SYM_TYP)||(elem->type==CNT_TYP)) free(elem->ELEM.symbol);
  else if (elem->type==LST_TYP) FreeListRule(elem->ELEM.rlist);
  else FreeAltList(elem->ELEM.list);
  free(elem);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeSpecList(SpecList *list)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 void *ptr;
 DBUG_ENTER("FreeSpecList");
 while (list!=NULL) {                                        /* List not empty ? */
  ptr=list;
  FreeAltElem(list->elem);
  list=list->next;
  free(ptr);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/
 
void FreeSyntaxRule(SyntaxRule *rule)
{ /* Release contens of one syntax rule ******************************************/
 DBUG_ENTER("FreeSyntaxRule");
 if (rule) {
  FreeSpecList(rule->speclist);
  FreeCCode(rule->ccode);
  FreeFormatStr(rule->format);
  if (rule->SpecTypes) free(rule->SpecTypes);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeSRuleList(SRuleList *list)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 void *ptr;
 DBUG_ENTER("FreeSRuleList");
 while (list!=NULL) {                                        /* List not empty ? */
  ptr=list;
  FreeSyntaxRule(list->srule);
  list=list->next;                                                /* Next please */
  free(ptr);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeSyntaxRuleHeader(SyntaxRuleHeader *header)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 DBUG_ENTER("FreeSyntaxRuleHeader");
 if (header->RuleName) free(header->RuleName);                   /* Name given ? */
 FreeSRuleList(header->synlist);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void FreeSyntaxRuleList(SyntaxRuleList *list)
{ /* Release all memory of this list and all memory of subcommponents ! **********/
 void *ptr;
 DBUG_ENTER("FreeSyntaxRuleList");
 while (list!=NULL) {                                        /* List not empty ? */
  ptr=list;
  FreeSyntaxRuleHeader(list->rule);
  list=list->next;                                                /* Next please */
  free(ptr);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void ActiveRuleInfo(char *string)
{ /* Copy rule infos to string ***************************************************/
 if (SRules) {                                              /* Rules available ? */
  sprintf(string,"rule [%s] defined in line %d",
   SRules->active->RuleName,
   SRules->active->LineNumber
  );
 }
} /*******************************************************************************/
