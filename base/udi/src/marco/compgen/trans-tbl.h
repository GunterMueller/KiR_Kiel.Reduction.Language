/**********************************************************************************
***********************************************************************************
**
**   File        : trans-tbl.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 7.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for transformation table handling
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _TRANS_TBL_HEADER_
#define _TRANS_TBL_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include "gen-parser.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct SimpleTransTag { /* Transformation function ***********************/
 char *name;                                             /* Simple function name */
 char *code;                                     /* Code for this trans-function */
 int  func_ID;                                            /* function identifier */
} SimpleTrans; /******************************************************************/

typedef struct CharValueTag { /* Mapping from character string to int value ******/
 char *string;                                                 /* Stringconstant */
 int value;                                                       /* Numbervalue */
} CharValue; /********************************************************************/

typedef struct ParameterTypesTag { /* Info about parameter types *****************/
 int type;                                              /* Actual parameter type */
 void *ptr;                                              /* Pointer to parameter */
} ParameterTypes; /***************************************************************/

typedef struct CodeTag { /* List of code lines ***********************************/
 int ltype;                            /* Type of this Code line (CT_STR,CT_INT) */
 union {
  char *line;                                                   /* One code line */
  int value;                                                    /* Codeline type */
 } U;
 struct CodeTag *next;                              /* Pointer to next code line */
} Code; /*************************************************************************/

typedef struct CodeInfoTag { /* Code informations ********************************/
 int nr_lines;                                           /* Number of code lines */
 Code *insertline;                               /* Line to add additional cases */
 Code *lines;                               /* Pointer to lines of function code */
 Code *last;                                                  /* Pointer to line */
} CodeInfo;  /********************************************************************/

typedef struct TransformTableTag { /* Infos for transformation functions *********/
 CodeInfo *code;                      /* List of code generated for one function */
 char *name;                                                     /* Functionname */
 int fct_id;                                              /* Function identifier */
 int nr_param;                                   /* Number of function parameter */
 int node_type;                                                  /* Type of node */
 ParameterTypes *param_types;                      /* Info about parameter types */
 int nr_formats;                       /* number of possible alternative formats */
 int if_mode;                                   /* Different output alternetives */
} TransformTable; /***************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define LT_STR           1
#define LT_INT           2

#define FP_PF_TBL        1
#define FP_SYM_TBL       2
#define FP_CHAR_TBL      3
#define FP_CREATEFCT     4
#define FP_CREATEFCTLIST 5
#define FP_CHAR_VAL_TBL  6

#define FCTID_FIRST_RULE 1
#define FCTID_NONE	 2

#define NODE_LEN	1
#define NODE_VALUE	2
#define NODE_NODE	4
#define NODE_NODE2	8
#define NODE_NODE3	16
#define NODE_STRING	32
#define NODE_LIST	64
#define NODE_CHAR	128

/*********************************************************************************/
/***************** MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Write transfromation function to a file named filename ************************/
extern void WriteFormatFunctions(char *filename);

/* Tests, if name is legal edit info value ***************************************/
extern int IsEditInfo(char *name);

/* Tests, if name is a legal create function *************************************/
extern TransformTable *IsCreateFunc(char *name);

extern void AddListCase(SyntaxRule *rule,char *listname,char *delimiter);

/* Make code info header *********************************************************/
extern CodeInfo *MkCodeInfo(void);

/* Add one code line to structure ************************************************/
extern void AddCodeLine(CodeInfo *info,char *line,int value);

/* Set insert pointer at actual last element *************************************/
extern void SetInsertCodeLine(CodeInfo *info);

/* Insert one code line to structure *********************************************/
extern void InsertCodeLine(CodeInfo *info,char *line,int value);

SimpleTrans *IsSimpleTransFunc(char *name);
extern int CheckFunctionCall(char *name,CCSymbolList *list,char *msg);

#endif
