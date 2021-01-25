/*********************************************************************************
***********************************************************************************
**
**   File        : gen-parser.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 09.02.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for parser generation
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _GENPARSER_HEADER_
#define _GENPARSER_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include "gen-options.h"
#include "gen-ccode.h"

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define NON_TYP      0

#define SYM_TYP      16
#define TOK_TYP      (SYM_TYP|1)
#define RUL_TYP      (SYM_TYP|2)

#define SPC_TYP      32
#define REP_TYP      (SPC_TYP|1)
#define OPT_TYP      (SPC_TYP|2)
#define ALT_TYP      (SPC_TYP|3)

#define SHT_TYP      64
#define LST_TYP      (SHT_TYP|1)
#define CNT_TYP      (SHT_TYP|2)

/* Defines for syntax rule types *************************************************/
#define SRULETYPE_DYNAMIC    0
#define SRULETYPE_INT        1
#define SRULETYPE_CHARSTRING 2
#define SRULETYPE_LIST       3
#define SRULETYPE_STANDART   4

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct StringStructTag { /* Holds infos about string rules ***************/
 char *token;                                         /* Name of character token */
 char *start;                               /* Name of string token start symbol */
 char *end;                                   /* Name of string token end symbol */
} StringStruct; /*****************************************************************/

typedef struct StringStructListTag { /* List of string rule informations *********/
 StringStruct *elem;                                 /* String rule informations */
 struct StringStructListTag *next;                          /* Next list element */
} StringStructList; /*************************************************************/

typedef struct SpecListTag SpecList;                      /* Forward declaration */
typedef struct AltListTag AltList;                        /* Forward declaration */

typedef struct ListRuleTag { /* Structure for list generation ********************/
 int min;                                         /* Min number of list elements */
 int max;                                         /* Max number of list elements */
 TokenList *delis;                                    /* Pointer to list element */
 int type;                                               /* Type of rule element */
 char *elem;                                      /* Pointer to delimeter symbol */
} ListRule; /*********************************************************************/

typedef struct AltElemTag { /* Structure of syntax rules (repeat,optional,alt.) **/
 int type;                 /* Type of entry (SYMBOL,REPEAT,OPTIONAL,ALTERNATIVE) */
 int nr;                                                         /* Entry number */
 union {                                                /* Holds one alternative */
  ListRule *rlist;                            /* Pointer to an element rule list */
  char *symbol;                                         /* Pointer to symboltext */
  AltList *list;                                    /* Pointer to one typed list */
 } ELEM;                             /* Helpvariable to select actual union part */
} AltElem; /**********************************************************************/

struct AltListTag { /* List of Alternative Elements ******************************/
 SpecList *list;                                  /* Pointer to actual listentry */
 int nr;                                                      /* Number of entry */
 struct AltListTag *next;                          /* Pointer to next list entry */
}; /*******************************************************************************/

struct SpecListTag { /* List of alternative syntax rules *************************/
 AltElem *elem;                                                /* Actual element */
 struct SpecListTag *next;                          /* Pointer to next listentry */
}; /******************************************************************************/

typedef struct SyntaxRuleTag { /* Contents for one syntax alternative ************/
 SpecList *speclist;                              /* List rules and alternatives */
 CCode      *ccode;                        /* C style coded transformationsrules */
 FormatStr  *format;                              /* External formatstyle string */
 int list_len;                                             /* Length of SpecList */
 int *SpecTypes;                                      /* Dynamic Spec type array */
} SyntaxRule; /*******************************************************************/

typedef struct SRuleListTag { /* List of syntax rules ****************************/
 SyntaxRule          *srule;         /* Description for actual syntax expression */
 struct SRuleListTag *next;                            /* Next syntax rule entry */
} SRuleList; /********************************************************************/

typedef struct SyntaxRuleHeaderTag { /* Header for one syntax/expr ***************/
 char *RuleName;                                      /* Name of the syntax-rule */
 int  RuleType;                                           /* C-Type of this rule */
 int  LineNumber;       /* Number of startline in sourcefile for rule definition */
 int  NrAlts;                     /* Number of syntax alternatives for this rule */
 int  RefLevel;                       /* Number of  rules derived from root rule */
 int  RefCount;                  /* Number of references to this rule definition */
 SRuleList *synlist;                /* List of syntax alternatives for this rule */
} SyntaxRuleHeader; /*************************************************************/

typedef struct SyntaxRuleListTag { /* Holds information about all s-rules ********/
 SyntaxRuleHeader         *active;  /* Pointer to the rule, which is in progress */
 SyntaxRuleHeader         *rule;                           /* Actual syntax rule */
 struct SyntaxRuleListTag *next;                        /* Pointer to next rule  */
} SyntaxRuleList; /***************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern SyntaxRuleList *SRules;                              /* List of all rules */
extern StringStructList *StringList;              /* List of string rule symbols */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Copy rule infos to string *****************************************************/
void ActiveRuleInfo(char *string);

/* Setup ParserGenerator, will be done from InitGeneratorOptions ! ***************/
void InitParserGenerator(void);

/* Generates code  for a parser **************************************************/
void WriteParserCode(void);

/* Writes type declarations for syntax rules *************************************/
void WriteParserTypes(FILE *fh);

/* Free all memory, will done be from ReleaseGeneratorOptions ! ******************/
void CleanUpParserGenerator(void);


/* Creates structure infos for one list rule *************************************/
ListRule *MkListRule(int min,int max,TokenList *delis,char *elem,int type);

/* Release allocate memory *******************************************************/
void FreeListRule(ListRule *rule);

/* Creates one entry for an alternative list with specified type *****************/
AltElem *MkAltElem(int type,void *elem);

/* Release memory of this structure **********************************************/
void FreeAltElem(AltElem *elem);

/* Adds one specificationlist to a list of alternatives **************************/
AltList *AddAltList(AltList *alist,SpecList *slist);

/* Release all memory of this list and all memory of subcommponents ! ************/
void FreeAltList(AltList *list);

/* Adds one element to specification list ****************************************/
SpecList *AddSpecList(SpecList *slist,AltElem *elem);

/* Release all memory of this list and all memory of subcommponents ! ************/
void FreeSpecList(SpecList *list);


/* Constructor for one syntax rule in this language ******************************/
SyntaxRule *MkSyntaxRule(SpecList *list,CCode *code,FormatStr *fstr);

/* Test semantical values in one syntax rule *************************************/
SyntaxRule *CheckSyntaxRule(SyntaxRule *rule);

/* Release contens of one syntax rule ********************************************/
void FreeSyntaxRule(SyntaxRule *rule);

/* Adds one syntax rules to an existing list of syntax rules *********************/
SRuleList *AddSRuleList(SRuleList *list,SyntaxRule *rule);

/* Release all memory of this list and all memory of subcommponents ! ************/
void FreeSRuleList(SRuleList *list);


/* Creates header for a list of grammatical different syntax alternatives ********/
SyntaxRuleHeader *MkSyntaxRuleHeader(char *name,int RuleType,
				     SRuleList *list,int line_nr,int L);

/* Release all memory of this list and all memory of subcommponents ! ************/
void FreeSyntaxRuleHeader(SyntaxRuleHeader *header);

/* Adds rule for one syntax expression to an existing list of rules **************/
SyntaxRuleList *AddSyntaxRuleList(SyntaxRuleList *list,SyntaxRuleHeader *header);

/* Release all memory of this list and all memory of subcommponents ! ************/
void FreeSyntaxRuleList(SyntaxRuleList *list);


/* Create one String structure ***************************************************/
StringStruct *MkStringStruct(char *token,char *start,char *end,char *MSGBUF);

/* Adds one Stringstruct to a list ***********************************************/
StringStructList *AddStringStructList(StringStructList *list,StringStruct *ss);

/* Release memory of a string structure ******************************************/
void FreeStringStruct(StringStruct *ss);

/* Release a complete list of string structures (calls FreeStringStruct) *********/
void FreeStringStructList(StringStructList *list);

#endif
