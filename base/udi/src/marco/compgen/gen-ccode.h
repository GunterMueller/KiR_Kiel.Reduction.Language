/*********************************************************************************
**********************************************************************************
**
**   File        : gen-ccode.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 17.03.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for transformation code generation
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _GENCCODE_HEADER_
#define _GENCCODE_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include "gen-options.h"

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define CCTYPE_DOLLARVAR  1
#define CCTYPE_NUMBER     2
#define CCTYPE_STRING     3
#define CCTYPE_IDENTIFIER 4
#define CCTYPE_DOLLARSY   5
#define CCTYPE_FUNCCALL   6
#define CCTYPE_TABSY      32
#define CCTYPE_NEWLINESY  33
#define CCTYPE_CURSORSY   34
#define CCTYPE_SETTABSY   35
#define CCTYPE_CLEARTABSY 36
#define CCTYPE_BINDSY     37
#define CCTYPE_INDENTSY   38
#define CCTYPE_LISTELEM   39
#define CCTYPE_LISTDELI   40
#define CCTYPE_REPEATLIST 41
#define CCTYPE_CCODE      64

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct CCSymbolListTag CCSymbolList;              /* Forward declaration */

typedef struct CCSymbolTag { /* CCode symbol *************************************/
 int type;                                                        /* Symbol type */
 union {
  char         *name;                                             /* Symbol name */
  int          number;                                          /* Integer value */
  CCSymbolList *func;                                  /* Functioncall structure */
 } U;
} CCSymbol; /*********************************************************************/

struct CCSymbolListTag { /* List of typed ccode symbols **************************/
 CCSymbol               *symbol;                           /* Actual list symbol */
 struct CCSymbolListTag *next;                   /* Pointer to next list element */
}; /******************************************************************************/

typedef struct CCLineTag { /* Data for one transformationrule ********************/
 CCSymbol   *name;                       /* Name of transformationsrule variable */
 CCSymbolList *func;                          /* Data of transformation function */
} CCLine; /***********************************************************************/

typedef struct CCLinesTag { /* List of transformation rule functions *************/
 CCLine *line;                                                   /* List element */
 struct CCLinesTag *next;                                   /* Next list element */
} CCLines; /**********************************************************************/

typedef struct CCodeTag { /* Holds Rules for transformation, etc *****************/
 CCLines *lines;                                 /* List of transformation rules */
} CCode; /************************************************************************/

typedef struct FormatStrTag { /* Holds information for external outputform *******/
 CCSymbolList *format;                   /* Format string, stored as linked list */
} FormatStr; /********************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Create outputformat for one rule **********************************************/
FormatStr *MkFormatStr(CCSymbolList *format);

/* Release allocated memory used by one format rule ******************************/
void FreeFormatStr(FormatStr *format);

/* Create one typed ccode symbol *************************************************/
CCSymbol *MkCCSymbol(int type,void *ptr);

/* Create one CCode transformation rule entry ************************************/
CCLine *MkCCLine(CCSymbol *symbol,CCSymbolList *list);

/* Transformation functions for one syntax rule **********************************/
CCode *MkCCode(CCLines *clines);

/* Copy CCode structure **********************************************************/
CCode *CopyCCode(CCode *code);

/* Create a function call structure **********************************************/
CCSymbolList *MkFuncCall(CCSymbol *symbol,CCSymbolList *list);

/* Add one CCode element to a symbol list ****************************************/
CCSymbolList *AddCCSymbolList(CCSymbolList *list,CCSymbol *symbol);

/* Join two lists ****************************************************************/
CCSymbolList *JoinCCSymbolLists(CCSymbolList *list1,CCSymbolList *list2);

/* Add one transformation rule to the list ***************************************/
CCLines *AddCCLines(CCLines *lines,CCLine *line);

/* Release memory for c code structure *******************************************/
void FreeCCode(CCode *code);

/* Release symbol memory *********************************************************/
void FreeCCSymbol(CCSymbol *symbol);

/* Release memory for complete symbollist ****************************************/
void FreeCCSymbolList(CCSymbolList *list);

/* Release memory for one transformation rule ************************************/
void FreeCCLine(CCLine *line);

/* Release complete list of transformation rules *********************************/
void FreeCCLines(CCLines *lines);

void PrCCode(FILE *FH,CCode *code,char *rulename,void *rule);

#endif
