/**********************************************************************************
***********************************************************************************
**
**   File        : scanner-txt.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 11.1.1993
**   Contents    : Programmausgaben und Fehlertexte bei der Scannergenerierung
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _SCANNER_TEXTE_
#define _SCANNER_TEXTE_

#define TEXT_SCANNERGENERATORVERS \
" Scannergenerator Version 0.0"

/*********************************************************************************/
/********** PROGRAMMFEHLERMELDUNGEN PRINTF-TEXTE *********************************/
/*********************************************************************************/

#define ERR_SC_DUPLICATE_SYMBOLS \
"Duplicate symbol token name !\n"
#define ERR_SC_UNKNOWN_SYMBOL \
"Unknow symboltype found in table. Generator error !\n"
#define ERR_SC_CANT_OPEN_OUTPUTFILE \
"Can't open file <%s> for writing scanner !\n"
#define ERR_SC_STD_POSVALUES \
"Only positiv values for standart identifier !\n"
#define ERR_SC_STD_VALUE_RANGE \
"%d is out of supplied range for %s identifier (0,%d) !\n"
#define ERR_SC_SYMBOLTABLE_FULL \
"Symboltable is full ! Tablesize is : %d\n"
#define ERR_SC_CASESEN_ADEF \
"Casesensitivity for %s already definied !\n"
#define ERR_SC_RULE_ADEF \
"Rule for >| %s |< already defined !\n"
#define ERR_SC_SYMBOLTABLE_2_SMALL \
"Symboltable was to small, sorry.\n"
#define ERR_SC_SYMBOLTABLE_SIZE \
"Actual size : %d Symbols used : %d\n"
#define ERR_SC_NO_COMSTARTSYMBOL \
"No comment start symbol definied, but a end symbol given !\n"
#define ERR_SC_COMSYMBOL_AU \
"Commentsymbol %s already used as lexical symbol !\n"
#define ERR_COMMENTENND_FOUND \
"Unsuspected commentend-symbol found in line : %d\\n"

/*********************************************************************************/
/********** AUSGABETEXTE FUER SCANNERGENERIERUNG *********************************/
/*********************************************************************************/

#define TEXT_ID_IDENTIFIER       "IDENTIFIER"
#define TEXT_ID_DOUBLE           "DOUBLE"
#define TEXT_ID_LONG             "LONG"
#define TEXT_ID_FLOAT            "FLOAT"
#define TEXT_ID_INT              "INT"
#define TEXT_ID_CHAR		 "CHAR"

#define TEXT_SC_RETURNFUNCTION_HEADER \
"static int Conv2%s(YYSTYPE *lvalp,int token)"
#define TEXT_SC_RETURNFUNCTION_FOOTER \
" return (token);\n}\n\n"
#define TEXT_SC_RETURN_INT \
"atoi(yytext)"
#define TEXT_SC_RETURN_LONG \
"atol(yytext)"
#define TEXT_SC_RETURN_FLOAT \
"(float)atof(yytext)"
#define TEXT_SC_RETURN_DOUBLE \
"atof(yytext)"

#define TEXT_SC_TEST_YYLVAL_UNION \
"\nunion {\t\t\t/* Takes values of identifier rules */\n\
 char   *%sval;\t/* Holds text identifier values */\n\
 float  %sval;\t\t/* Holds single precision float values */\n\
 double %sval;\t\t/* Holds double precision float values */\n\
 long   %sval;\t\t/* Holds Natural or/and integer values */\n\
 int    %sval;\t\t\t/* Holds Natural or/and integer values */\n\
 char   %sval;\t\t\t/* Holds Character value */\n\
} yylval;\t\t/* Global scanner variable for semantical values */\n\n"

#define TEXT_SC_BEGINCOMMENTS "Rule for comment recognation"
#define TEXT_SC_TSTCOMMENTSFCT_PROTO "static int TstComments(void)"
#define TEXT_SC_EOLN_INCOMMENT "End of line in comment !"
#define TEXT_SC_EOF_INCOMMENT "End of file in comment !"

/* Standartausgabe Texte im Scannercode ******************************************/
#define TEXT_SC_MAINPART \
"Start of lexical analysing rules"
#define TEXT_SC_ENDPART \
"End of scanner rules. Begin of C-helpfunctions"
#define TEXT_SC_CDECLPART \
"Normal C-code, used to include definitions"
#define TEXT_SC_INCLUDES \
"#include <stdio.h>\n#include <string.h>\n"
#define TEXT_SC_IDRULES \
"Rules for identifiers, terminalsymbols and abbreviations"
#define TEXT_SC_NOCOMMENTS \
"No comment symbols definied !"
#define TEXT_SC_TESTMAIN \
"Example testmain function for scanner"

#define TEXT_COMSTART \
"/*********************************************************************************"

#define TEXT_COMFILL \
"**********************************************************************************"

#define TEXT_COMPRE \
"***"

#define TEXT_COMEND \
"*********************************************************************************/"

#endif
