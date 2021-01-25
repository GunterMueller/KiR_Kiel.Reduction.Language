/**********************************************************************************
***********************************************************************************
**
**   File        : gen-options.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 03.2.1993
**   Contents    : 
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _GENOPTIONS_HEADER_
#define _GENOPTIONS_HEADER_ 1

/*********************************************************************************/
/********** BENOETIGTE INCLUDES **************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

/*********************************************************************************/
/********** TYPENDEFINITIONEN ****************************************************/
/*********************************************************************************/

typedef char  BOOL;

typedef struct TokenListTag { /* List of tokensymbols ****************************/
 int type;                                         /* Tokentype for syntax rules */
 char *token;                                                       /* Tokentext */
 struct TokenListTag *next;                    /* pointer to the next list entry */
 } TokenList; /*******************************************************************/

typedef enum SYtypesTag {KW_NONE_SY=0,             /* Typen in der Symboltabelle */
                         KW_IDENTIFIERS_SY,
                         KW_ID_STD_SY,
                         KW_ID_REGEXPR_SY,
			 KW_NORMAL_SY=100,
                         KW_KEYWORDS_SY,
                         KW_PRIMFUNCTIONS_SY,
                         KW_DELIMETERS_SY,
                         KW_TYPES_SY
} SYtype; /***********************************************************************/

typedef enum CASEtypeTag {NO_CASE=0,  /* Gross/Klein-Schreibung in den Sektionen */
                          LOWER_CASE=1,                   /* Nur Kleinbuchstaben */
                          UPPER_CASE,                     /* Nur Grossbuchstaben */
                          Upper_CASE,       /* Erste Buchstabe gross, Rest klein */
                          IGNORE_CASE                       /* Schreibweise egal */
} CASEtype; /*********************************************************************/

typedef struct PrjOptionsTag { /* Optionen, Parameter fuer den Lang Generator ****/
 char *name;                                                      /* Projektname */
 char *filename;                                          /* Optionaler Filename */
 int  mode;                                                    /* Generatormodus */
 int  IdStart;               /* Beginn der Identifierregeln in der Symboltabelle */
 int  sc_buf_size;                                     /* Size of scanner buffer */
 BOOL bit8;                                         /* 8 Bitscanner (TRUE,FALSE) */
 BOOL test;                                           /* Generate testfuncktions */
 BOOL sctest;                                 /* Test-Main-Funktion fuer Scanner */
 BOOL prtest;                                  /* Test-Main-Funktion fuer Parser */
 BOOL makefile;                                             /* Makefile erzeugen */
} PrjOptionsStruct; /*************************************************************/

typedef struct ParserOptionsTag { /* Holds options for parsergeneration **********/
 int       token;                       /* All keywords in grammar are token IDs */
 char      *extender;                             /* Extender for parser-outfile */
 char      *expr;                                     /* Rulename for expression */
 TokenList *left_prec,
           *right_prec,
           *nonassoc_prec;
} ParserOptionsStruct; /**********************************************************/

typedef struct SYOptionsTag { /* Optionen fuer Keywoerter ************************/
 SYtype type;                                               /* Type der Optionen */
 char *extender;                                               /* Symbolextender */
 CASEtype ctype;                                                 /* Schreibweise */
} SYOptionsStruct; /**************************************************************/

typedef struct CommentTag { /* Daten fuer Kommentarregeln ************************/
 BOOL newlines;              /* TRUE = Kommentar kann ueber mehrere Zeilen gehen */
 BOOL nested;                    /* TRUE = Schachtelung von Kommentaren moeglich */
 BOOL gen_code;             /* TRUE = Generiere Funktion in C-Code Teil (intern) */
 char *start;                                          /* Kommentaranfangssymbol */
 char *end;                                              /* Kommentarendezeichen */
} CommentStruct; /****************************************************************/

/*********************************************************************************/
/********** PROGRAMM DEFINES *****************************************************/
/*********************************************************************************/

#define MAKEFILE_EXTENDER ".mk"
#define PARSER_EXTENDER   ".y"
#define SCANNER_EXTENDER  ".flex"
#define TOKEN_HEADER_EXT  ".tab.h"

#define TRUE  1
#define FALSE 0
#define UNDEF -1

/* Definitionen zur Unterscheidung von Standartidentifier ************************/
#define OFFSET_INT        100
#define OFFSET_LONG       200
#define OFFSET_FLOAT      300
#define OFFSET_DOUBLE     400
#define OFFSET_IDENTIFIER 500
#define OFFSET_CHAR       600

/*********************************************************************************/
/********** MODUL GLOBALE VARIABLEN **********************************************/
/*********************************************************************************/

extern SYOptionsStruct Scanner_OPT,
                       Keyword_OPT,
                       Primfunction_OPT,
                       Type_OPT,
                       Delimeter_OPT,
                       Identifier_OPT;

extern CommentStruct Comment_OPT;

extern ParserOptionsStruct Parser_OPT;
extern PrjOptionsStruct    Project_OPT;

/*********************************************************************************/
/********** PROTOTYPE FUER EXTERNE FUNKTIONEN ************************************/
/*********************************************************************************/

/* Replacement for the standard strdup function **/
extern char *my_strdup(char *name);

/* Initialisiert die Optionen fuer den Generator *********************************/
void InitGeneratorOptions(char *filename,int htablesize,int mode);

/* Gibt den Speicher fuer Optionen wieder frei ***********************************/
void ReleaseGeneratorOptions(void);

/* Gibt zu einem Symboltype die entsprechende Textdarstellung ********************/
char *SYtype2Char(SYtype type);

/* Setzt in der Optionenstruktur die Gross/Klein Erkennnung **********************/
void SetCaseOption(SYOptionsStruct *opt,CASEtype ctype);

/* Setzt in der Optionenstruktur den Symbolnamen Sufix ***************************/
void SetExtOption(SYOptionsStruct *opt,char *ext);

/* Setzt Optionen fuer die Kommentarerkennung ************************************/
void SetCommentOption(int token,char yn);

/* Setzt Symbole fuer die Kommentarerkennung *************************************/
void SetCommentSymbol(int token,char *symbol);

TokenList *AddTokenList(TokenList *list,int type,char *token);
void FreeTokenList(TokenList *list);
TokenList *ConcTokenList(TokenList *l1,TokenList *l2);

#endif
