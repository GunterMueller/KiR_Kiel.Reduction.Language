/**********************************************************************************
***********************************************************************************
**
**   File        : ccomp.y 
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 04.01.1993
**   Contents    : Syntax description of Scanner/Parsergenerator
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

%{

#include "dbug.h"
#include <string.h>

#include "gen-options.h"
#include "gen-scanner.h"
#include "gen-parser.h"
#include "trans-tbl.h"
#include "gen-ccode.h"
#include "err-modu.h"
#include "list-modu.h"

extern int yyLineNumber;         /* Declarations of line counter in file ccomp.l */

extern int VerbatimGet();                            /* Declared in file ccomp.l */
extern char *yytext;                                 /* Declared in file ccomp.l */

#define VBUF_LEN 	256
static char vbuf[VBUF_LEN];                                       /* Read buffer */
static char ERR_MSGBUF[VBUF_LEN];
static SYtype SY_type;             /* Actual keyword type while scanning symbols */
static int specifier_typ=0; /* Type of last used specifier */

%}

%union{ /* holds semantical values for terminal symbols and grammarrule values ***/
 char text[256];                       /* used for string constants and litarals */
 int number;                                       /* value for enumerable types */
 int ctype;                                          /* Storage for symbol types */
 char *extender;                            /* Holds pointer to keyword extender */
 TokenList        *TOKENLIST;
 SyntaxRuleList   *SYNTAXRULELIST;
 SyntaxRuleHeader *SYNTAXRULEHEADER;
 SRuleList        *SRULELIST;
 SyntaxRule       *SYNTAXRULE;
 SpecList         *SPECLIST;
 AltElem          *ALTELEM;
 AltList          *ALTLIST;
 FormatStr        *FORMATSTR;
 ListRule         *LISTRULE;
 StringStruct     *STRINGSTRUCT;
 StringStructList *STRINGSTRUCTLIST;
 CCSymbol         *CCSYMBOL;
 CCSymbolList     *CCSYMBOLLIST;
 CCLine           *CCLINE;
 CCLines          *CCLINES;
 CCode            *CCODE;
} /*******************************************************************************/

/**** Definition of tokens used in this grammar **********************************/
%token PROJECT SCANNER PARSER

%token KEYWORDS PRIMFUNCTIONS TYPES IDENTIFIERS COMMENTS DELIMETERS
%token SYNTAXRULES STRING
%token LEFT RIGHT NONASSOC

%token NAME ONLYTOKEN 
%token EXTENDER
%token NESTED NEWLINES TEST MAKEFILE
%token BUFFERSIZE EXPR_SY

%token YES NO

%token START_SY END_SY

%token BRA_SY KET_SY SBRA_SY SKET_SY RBRA_SY RKET_SY LBRA_SY LKET_SY
%token COMMA_SY EQUAL_SY COLON_SY ALT_SY SEMICOLON_SY DOLLAR_SY
%token TAB_SY NEWLINE_SY LEQUAL_SY REQUAL_SY
%token SETTAB_SY CLEARTAB_SY BIND_SY INDENT_SY FRM_BRA_SY FRM_KET_SY
%token FRM_ELEM FRM_DELI

%token BEGIN_C END_C BEGIN_F END_F

%token REGEXPR INT_TYPE FLOAT_TYPE IDENTIFIER_TYPE DOUBLE_TYPE LONG_TYPE CHAR_TYPE
%token GETCHAR 
/*********************************************************************************/

/* Definition of token which have a semantical values ****************************/
%token <text>    NAME_ID TEXT_ID DOLLAR_TOK REPEATNUMBER
%token <number>  NUMBER_TOK

/* Type of syntax rules **********************************************************/
%type <number>   optional_yn standart_type regexpr_type limits
%type <extender> ext_opt
%type <extender> specifier

%type <SYNTAXRULELIST>   syntax_terms
%type <SYNTAXRULEHEADER> syntax_rule
%type <SRULELIST>        syntax_rules
%type <SYNTAXRULE>       syntax_line
%type <CCODE>            c_code_part
%type <FORMATSTR>        format_part
%type <SPECLIST>         syntax_spec syntax_speclist
%type <ALTELEM>          syntax_altelem
%type <ALTLIST>          syntax_altlist
%type <TOKENLIST>        prec_list specifiers
%type <LISTRULE>         list_rule
%type <STRINGSTRUCT>     string_term
%type <STRINGSTRUCTLIST> string_terms
%type <CCSYMBOL>         c_parameter format_term
%type <CCSYMBOLLIST>     c_parameters c_func_call format_terms
%type <CCLINE>           c_line c_line_alt
%type <CCLINES>          c_code_terms

/* Startsymbol for syntax description ********************************************/
%start start


%% /******************************************************************************/

start		: project_part scanner_part parser_part
		;

/*********************************************************************************/
/*** Vorspann/Globale Definition fuer Lex/Parser Generator        ****************/
/*********************************************************************************/

project_part	: PROJECT
		| PROJECT BRA_SY project_opts KET_SY
		| error { YYABORT; }
		;

project_opts 	: proj_opt_alt
		| project_opts COMMA_SY proj_opt_alt
		;

proj_opt_alt	: NAME EQUAL_SY NAME_ID 
                   {Project_OPT.name=(char*)strdup($3);}     /* Filenamen setzen */
                | MAKEFILE optional_yn                      /* Makefile erzeugen */
                   {Project_OPT.makefile=$2;}
                | TEST optional_yn {Project_OPT.test=$2}
		| error {
 sprintf(ERR_MSGBUF,"Unknown option <%s> in project definition !\n",yytext);
 err_Message(ERR_WARN,ERR_MSGBUF);
 yyclearin;
}
		;

/*********************************************************************************/
/*** Allgemeine Parseregeln                                       ****************/
/*********************************************************************************/

optional_yn	:               { $$=TRUE; }   /* Nichts bedeutet YES */
		| EQUAL_SY YES  { $$=TRUE; }   /* YES bedeutet 1 */
		| EQUAL_SY NO   { $$=FALSE; }  /* NO  bedeutet 0 */
		| EQUAL_SY error {
 sprintf(ERR_MSGBUF,"Unknow keyword <%s>. Only {YES,NO} are valid, keyword treated as NO !\n",yytext);
 err_Message(ERR_WARN,ERR_MSGBUF);
 $$=FALSE;
 yyclearin;
 yyerrok;
}
		;

ext_opt		: EXTENDER EQUAL_SY NAME_ID { $$=$3; }
		| EXTENDER error {
 err_Message(ERR_WARN,"No extender given. Default extender used !\n");
 yyclearin;
 $$=NULL;
}
		;

name_terms	: name_term
		| name_terms name_term
		;

name_term	:
 NAME_ID EQUAL_SY {
  if (VerbatimGet(vbuf,VBUF_LEN)>0) {
   AddSymbol(SY_type,$1,vbuf,BLANKS_NORMAL);
  } else {
   sprintf(ERR_MSGBUF,"Text for token %s missing. Token ignored.\n",$1);
   err_Message(ERR_WARN,ERR_MSGBUF);
  }
 }
 | NAME_ID LEQUAL_SY {
  if (VerbatimGet(vbuf,VBUF_LEN)>0) {
   AddSymbol(SY_type,$1,vbuf,BLANKS_LEADING);
  } else {
   sprintf(ERR_MSGBUF,"Text for token %s missing. Token ignored.\n",$1);
   err_Message(ERR_WARN,ERR_MSGBUF);
  }
 }
 | NAME_ID REQUAL_SY {
  if (VerbatimGet(vbuf,VBUF_LEN)>0) {
   AddSymbol(SY_type,$1,vbuf,BLANKS_FOLLOWING);
  } else {
   sprintf(ERR_MSGBUF,"Text for token %s missing. Token ignored.\n",$1);
   err_Message(ERR_WARN,ERR_MSGBUF);
  }
 }
 | NAME_ID { AddSymbol(SY_type,NULL,$1,BLANKS_NORMAL); }
; 

/*********************************************************************************/
/*** Optionen fuer Scannergenerator holen und Regeln lesen        ****************/
/*********************************************************************************/

scanner_part	: scanner_def scanner_terms {
 if (yychar!=PARSER) {
  Project_OPT.sctest=Project_OPT.test;
 } else {
  Project_OPT.sctest=0;
 }
 ScannerDescComplete();
}
		;

scanner_def	: SCANNER /* Defaultwerte nehmen */
		| SCANNER BRA_SY scanner_opts KET_SY /* Optionen holen */
		;

scanner_opts	: scan_opt_alt /* Optional Parameter testen */
		| scanner_opts COMMA_SY scan_opt_alt
		;

scan_opt_alt	: ext_opt          { SetExtOption(&Scanner_OPT,$1); }
		| BUFFERSIZE EQUAL_SY NUMBER_TOK {Project_OPT.sc_buf_size=$3;}
		;

scanner_terms	: scanner_term /* Unterkomponenten des Scanners testen */
		| scanner_terms scanner_term
		;

scanner_term	: keywords_part /* Opt. Schluesselwoerter */
		| types_part /* Basistypen Erkennung */
		| primfuncs_part /* Vordefinierte Primitive Funktionen */
		| identifier_part /* Bezeichner Regeln */
		| delimeters_part /* Regeln fuer Eingabe Trennsymbole */
		| comments_part             /* Definition der Kommentarerkennung */
		;

/*********************************************************************************/
/*** Regeln zur Erkennung von Keywoertern                         ****************/
/*********************************************************************************/

keywords_part	: KEYWORDS keywords_def {SY_type=KW_KEYWORDS_SY;} name_terms
		;

keywords_def	: /* Keine Optionen angegeben */
		| BRA_SY keywords_opts KET_SY
		;

keywords_opts	: key_opt_alt
		| keywords_opts COMMA_SY key_opt_alt
		;

key_opt_alt	: ext_opt     { SetExtOption(&Keyword_OPT,$1); }
		;

/*********************************************************************************/
/*** Regeln zur Erkennung von Typenkennwoertern                   ****************/
/*********************************************************************************/

types_part	: TYPES types_def {SY_type=KW_TYPES_SY;} name_terms
		;

types_def	: /* Keine Optionen angegeben */
		| BRA_SY types_opts KET_SY
		;

types_opts	: typ_opt_alt
		| types_opts COMMA_SY typ_opt_alt
		;

typ_opt_alt	: ext_opt     { SetExtOption(&Type_OPT,$1); }
		;

/*********************************************************************************/
/*** Regeln zur Darstellung von Kommentarzeichen                  ****************/
/*********************************************************************************/

comments_part	: COMMENTS comments_def comments_terms
		;

comments_def	: /* Keine Optionen angegeben */
		| BRA_SY comments_opts KET_SY
		; 

comments_opts	: com_opt_alt
		| comments_opts COMMA_SY com_opt_alt
		;

com_opt_alt	: NESTED optional_yn     { SetCommentOption(NESTED,$2); }
		| NEWLINES optional_yn   { SetCommentOption(NEWLINES,$2); }
		;

comments_terms	: comments_term
		| comments_terms comments_term
		;

comments_term	: START_SY EQUAL_SY
                  { if (VerbatimGet(vbuf,VBUF_LEN)>0)
                     SetCommentSymbol(START_SY,vbuf);
		    else yyerror("Symbol for commentstart expected !");
                  }
 		| END_SY EQUAL_SY
                  { if (VerbatimGet(vbuf,VBUF_LEN)>0)
                     SetCommentSymbol(END_SY,vbuf);
		    else yyerror("Symbol for commentend expected !");
                  }
		;

/*********************************************************************************/
/*** Regeln zur Erkennung von Schluesselwoerter bei Primfuncs     ****************/
/*********************************************************************************/

primfuncs_part	: PRIMFUNCTIONS primfuncs_def {SY_type=KW_PRIMFUNCTIONS_SY;} name_terms
		;

primfuncs_def	: /* Keine Optionen angegeben */
		| BRA_SY primfuncs_opts KET_SY
		; 

primfuncs_opts	: prim_opt_alt
		| primfuncs_opts COMMA_SY prim_opt_alt
		;

prim_opt_alt	: ext_opt     {SetExtOption(&Primfunction_OPT,$1);}
		;

/*********************************************************************************/
/*** Regeln zur Erkennung von Identifikatoren                     ****************/
/*********************************************************************************/

identifier_part	: IDENTIFIERS identifier_def identifier_terms
		;

identifier_def	: /* Keine Optionen angegeben */
		| BRA_SY identifier_opts KET_SY
		; 

identifier_opts	: ide_opt_alt
		| identifier_opts COMMA_SY ide_opt_alt
		;

ide_opt_alt	: ext_opt     {SetExtOption(&Identifier_OPT,$1);}
		;

identifier_terms: identifier_term
		| identifier_terms identifier_term
		;

regexpr_type    : DOUBLE_TYPE        {$$=DOUBLE_TYPE;}
		| INT_TYPE           {$$=INT_TYPE;}
		| LONG_TYPE          {$$=LONG_TYPE;}
		| FLOAT_TYPE         {$$=FLOAT_TYPE;}
		| IDENTIFIER_TYPE    {$$=IDENTIFIER_TYPE;}
		| CHAR_TYPE          {$$=CHAR_TYPE;}
		;

standart_type	: LONG_TYPE NUMBER_TOK        {$$=CheckStdType(LONG_TYPE,$2);}
		| INT_TYPE NUMBER_TOK         {$$=CheckStdType(INT_TYPE,$2);}
		| FLOAT_TYPE NUMBER_TOK       {$$=CheckStdType(FLOAT_TYPE,$2);}
		| DOUBLE_TYPE NUMBER_TOK      {$$=CheckStdType(DOUBLE_TYPE,$2);}
		| IDENTIFIER_TYPE NUMBER_TOK  {$$=CheckStdType(IDENTIFIER_TYPE,$2);}
		| CHAR_TYPE NUMBER_TOK        {$$=CheckStdType(CHAR_TYPE,$2);}
		;

identifier_term	: NAME_ID EQUAL_SY standart_type
                   {
                    sprintf(vbuf,"%d",$3);
                    AddSymbol(KW_ID_STD_SY,vbuf,$1,BLANKS_NORMAL);
                   }
 		| NAME_ID EQUAL_SY REGEXPR regexpr_type
                   { if (VerbatimGet(vbuf,VBUF_LEN)>0) {
                      AddRegExprSymbol($4,vbuf,$1);
		     } else {
		      yyerror("VALUE EXPECTED");
		     }
                   }
		| NAME_ID EQUAL_SY GETCHAR
                   { if (VerbatimGet(vbuf,VBUF_LEN)>0) {
                      AddGetCharSymbol($1,vbuf);
		     } else {
		      yyerror("VALUE EXPECTED");
		     }
                   }
		;

/*********************************************************************************/
/*** Regeln zur Behandlung von Trenners(Delimeter)                ****************/
/*********************************************************************************/

delimeters_part	: DELIMETERS delimeters_def {SY_type=KW_DELIMETERS_SY;} name_terms
		;

delimeters_def	:
		| BRA_SY delimeters_opts KET_SY
		; 

delimeters_opts	: del_opt_alt
		| delimeters_opts COMMA_SY del_opt_alt
		;

del_opt_alt	: ext_opt {SetExtOption(&Delimeter_OPT,$1);}
		;

/*********************************************************************************/
/*** Regeln zur optionalen Parsergenerierung                      ****************/
/*********************************************************************************/

parser_part	: /* Empty */
		| parser_def parser_terms
{ Project_OPT.prtest=Project_OPT.test;}
		;

parser_def	: PARSER                      /* Defaultwerte fuer Parser nehmen */
		| PARSER BRA_SY parser_opts KET_SY            /* Parser Optionen */
		;

parser_opts	: par_opt_alt          /* Optionale Parameter fuer Parser testen */
		| parser_opts COMMA_SY par_opt_alt
		;

par_opt_alt	: ext_opt          {Parser_OPT.extender=(char *)strdup($1);}
		;

parser_terms	: prec_part string_part syntax_part
		;

/*********************************************************************************/
/*** Rules for string definition                                  ****************/
/*********************************************************************************/

string_part : /* Empty rule */
 | string_terms                                             { StringList=$1; }
;

string_terms :
 string_term                                   { $$=AddStringStructList(NULL,$1); } 
 | string_terms string_term                      { $$=AddStringStructList($1,$2); }
;

string_term : 
 STRING BRA_SY 
  ONLYTOKEN EQUAL_SY NAME_ID COMMA_SY 
  START_SY EQUAL_SY NAME_ID COMMA_SY END_SY EQUAL_SY NAME_ID
 KET_SY {
  StringStruct *tmp=MkStringStruct($5,$9,$13,ERR_MSGBUF);
  if (tmp==NULL) {
   err_Message(ERR_WARN,ERR_MSGBUF);
  }
  $$=tmp;
 }
;

/*********************************************************************************/
/*** Rules for precedence definition                              ****************/
/*********************************************************************************/

prec_part : /* Empty rule */
  | prec_terms
;

prec_list :
  specifier                              {$$=AddTokenList(NULL,specifier_typ,$1);}
  | prec_list COMMA_SY specifier           {$$=AddTokenList($1,specifier_typ,$3);}
;

prec_term :
  LEFT BRA_SY prec_list KET_SY
  {Parser_OPT.left_prec=ConcTokenList(Parser_OPT.left_prec,$3);}
  | RIGHT BRA_SY prec_list KET_SY
  {Parser_OPT.right_prec=ConcTokenList(Parser_OPT.right_prec,$3);}
  | NONASSOC BRA_SY prec_list KET_SY
  {Parser_OPT.nonassoc_prec=ConcTokenList(Parser_OPT.nonassoc_prec,$3);}
;

prec_terms :
  prec_term
  | prec_terms prec_term
;

/*********************************************************************************/
/*** Regeln zur Erfassung der Optionen zur Syntaxbeschreibung     ****************/
/*********************************************************************************/

syntax_part	: SYNTAXRULES syntax_def syntax_terms {SRules=$3;}
		;

syntax_def	: /* Keine Optionen angegeben, defaultwerte nehmen */
		| BRA_SY syntax_opts KET_SY
		;

syntax_opts	: syn_opt_alt
		| syntax_opts COMMA_SY syn_opt_alt
		;

syn_opt_alt	: EXPR_SY EQUAL_SY NAME_ID {Parser_OPT.expr=(char *)strdup($3);}
		;

/*********************************************************************************/
/*** Regeln zur Erfassung der Syntaxbeschreibung                  ****************/
/*********************************************************************************/

specifier :
  NAME_ID      { $$=(char *)strdup($1);specifier_typ=RUL_TYP; }
  | TEXT_ID
    {
      if (strcmp($1,"\x22\x22")==0) $$=NULL;
      else if (LookUpSymbol($1)==0) {
       sprintf(ERR_MSGBUF,"Symbol <%s> is not a terminal symbol !\n",$1);
       err_Message(ERR_WARN,ERR_MSGBUF);
       $$=(char *)strdup($1);
      } else {
       $$=(char *)strdup($1);
       specifier_typ=TOK_TYP;
      }
    }
;

specifiers :
  specifier                            { $$=AddTokenList(NULL,specifier_typ,$1); }
  | specifiers specifier                 { $$=AddTokenList($1,specifier_typ,$2); }
;

syntax_terms	: syntax_rule                      {$$=AddSyntaxRuleList(NULL,$1);}
		| syntax_terms syntax_rule           {$$=AddSyntaxRuleList($1,$2);}
;

syntax_rule	: NAME_ID COLON_SY syntax_rules SEMICOLON_SY
		  { $$=MkSyntaxRuleHeader($1,SRULETYPE_STANDART,$3,yyLineNumber,0); }
;

syntax_rules	: syntax_line                           {$$=AddSRuleList(NULL,$1);}
		| syntax_rules ALT_SY syntax_line         {$$=AddSRuleList($1,$3);}
;

syntax_line :
   syntax_spec c_code_part format_part { $$=MkSyntaxRule($1,$2,$3); }
;

syntax_spec	: /* Empty alternative (no list) */                    { $$=NULL; }
		| syntax_speclist                                        { $$=$1; }
;

syntax_speclist	: syntax_altelem                  { $$=AddSpecList(NULL,$1); }
		| syntax_speclist syntax_altelem    { $$=AddSpecList($1,$2); }
;

limits :
  NUMBER_TOK { $$=$1; }
  | NAME_ID  { $$=-1; }
;

list_rule :
  limits COMMA_SY limits COMMA_SY specifier COMMA_SY specifiers
  { $$=MkListRule($1,$3,$7,$5,specifier_typ); }
;

syntax_altelem	:
  BRA_SY  syntax_altlist KET_SY                       { $$=MkAltElem(OPT_TYP,$2); }
  | SBRA_SY syntax_altlist SKET_SY                    { $$=MkAltElem(REP_TYP,$2); }
  | RBRA_SY syntax_altlist RKET_SY                    { $$=MkAltElem(ALT_TYP,$2); }
  | LBRA_SY list_rule LKET_SY                         { $$=MkAltElem(LST_TYP,$2); }
  | LBRA_SY specifier LKET_SY                         { $$=MkAltElem(CNT_TYP,$2); }
  | specifier                                   { $$=MkAltElem(specifier_typ,$1); }
;

syntax_altlist	: syntax_speclist                       { $$=AddAltList(NULL,$1); }
		| syntax_altlist ALT_SY syntax_speclist   { $$=AddAltList($1,$3); }
		;

/*********************************************************************************/
/*** Rules for transformation/wellformchecks and typeinferenz     ****************/
/*********************************************************************************/

c_code_part : /* No CCode */                                           { $$=NULL; }
  | BEGIN_C c_code_terms END_C                                   { $$=MkCCode($2);}
;

c_code_terms : 
  c_line                                                { $$=AddCCLines(NULL,$1); }
  | c_code_terms c_line                                   { $$=AddCCLines($1,$2); }
;

c_line :
  c_line_alt SEMICOLON_SY { $$=$1; }
;

c_line_alt :
  NAME_ID  { 
   if (IsSimpleTransFunc($1)) {
    $$=MkCCLine(MkCCSymbol(CCTYPE_IDENTIFIER,$1),NULL);
   } else {
    sprintf(ERR_MSGBUF,"<%s> is not a simple transformation function !\n",$1);
    err_Message(ERR_SYNTAX,ERR_MSGBUF);
    $$=NULL;
   }
  }
  | c_func_call                                           { $$=MkCCLine(NULL,$1); }
;
 
c_func_call :
  NAME_ID RBRA_SY c_parameters RKET_SY {
   if (CheckFunctionCall($1,$3,ERR_MSGBUF)) {
    $$=MkFuncCall(MkCCSymbol(CCTYPE_IDENTIFIER,$1),$3);
   } else {
    err_Message(ERR_SYNTAX,ERR_MSGBUF);
    FreeCCSymbolList($3); 
    $$=NULL;
   }
  }
;

c_parameter :
  DOLLAR_TOK                                { $$=MkCCSymbol(CCTYPE_DOLLARVAR,$1); }
  | c_func_call                              { $$=MkCCSymbol(CCTYPE_FUNCCALL,$1); }
  | NAME_ID                                { $$=MkCCSymbol(CCTYPE_IDENTIFIER,$1); }
  | NUMBER_TOK                                { $$=MkCCSymbol(CCTYPE_NUMBER,&$1); }
  | TEXT_ID                                    { $$=MkCCSymbol(CCTYPE_STRING,$1); }
;

c_parameters :
  c_parameter                                      { $$=AddCCSymbolList(NULL,$1); }
  | c_parameters COMMA_SY c_parameter                { $$=AddCCSymbolList($1,$3); }
;

/*********************************************************************************/
/*** Rules for output formatdescription                           ****************/
/*********************************************************************************/

format_part : /* No format specification */                            { $$=NULL; }
  | BEGIN_F format_terms END_F                              { $$=MkFormatStr($2); }
;

format_terms :
  format_term                                      { $$=AddCCSymbolList(NULL,$1); }
  | format_terms format_term                         { $$=AddCCSymbolList($1,$2); }
  | REPEATNUMBER FRM_BRA_SY format_terms FRM_KET_SY {
     $$=JoinCCSymbolLists(
         AddCCSymbolList(NULL,MkCCSymbol(CCTYPE_REPEATLIST,$1)),
         $3
     );
    }
;

format_term :
  DOLLAR_TOK	                            { $$=MkCCSymbol(CCTYPE_DOLLARVAR,$1); }
  | TAB_SY                                    { $$=MkCCSymbol(CCTYPE_TABSY,NULL); }
  | NEWLINE_SY                            { $$=MkCCSymbol(CCTYPE_NEWLINESY,NULL); }
  | TEXT_ID                                    { $$=MkCCSymbol(CCTYPE_STRING,$1); }
  | SETTAB_SY                              { $$=MkCCSymbol(CCTYPE_SETTABSY,NULL); }
  | CLEARTAB_SY                          { $$=MkCCSymbol(CCTYPE_CLEARTABSY,NULL); }
  | BIND_SY                                  { $$=MkCCSymbol(CCTYPE_BINDSY,NULL); }
  | INDENT_SY                              { $$=MkCCSymbol(CCTYPE_INDENTSY,NULL); }
  | FRM_ELEM                               { $$=MkCCSymbol(CCTYPE_LISTELEM,NULL); }
  | FRM_DELI                               { $$=MkCCSymbol(CCTYPE_LISTDELI,NULL); }
;

%%

int yyerror(char *s)
{
 fprintf(stderr,"%s in line %d\n",s,yyLineNumber);
 return 1;
}
