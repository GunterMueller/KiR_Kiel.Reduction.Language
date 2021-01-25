/* keyword-table u.a.                         ciotab.c  */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#define iskwrdtb 1        /* fuer kwtb.h, um external's zu verhindern */

#include "cswitches.h"
#include "cportab.h"
#include "cedit.h"
#include "cestack.h"
#include "cencod.h"
#include "cetb.h"
#include "ckwtb.h"

/**************************************************************************/
/*   Ein/Ausgabe-Tabelle                                                  */
/*   ~~~~~~~~~~~~~~~~~~~                                                  */
/*                                                                        */
/* Jede Zeile spezifiziert die interne und externe Syntax eines Ausdrucks */
/* und enthaelt vier Eintragungen:                                        */
/* 1. ein Wort das den Typ des entsprechenden Ausdrucks festlegt,         */
/* 2. eine Zeichenkette die die Syntax spezifiziert,                      */
/* 3. die Mindestanzahl der Zeilen verringert um eins und                 */
/* 4. die Mindestanzahl der Spalten verringert um eins, die noetig sind   */
/*    um den Ausdruck extern darzustellen.                                */
/*                                                                        */
/* zu 1. siehe cetb.h                                                     */
/* zu 2. durch \33 wird die Codierung eines Unterausdrucks eingeleitet    */
/*       die drei Byte lang ist. Das erste Byte enthaelt eine Ausdruck-   */
/*       nummer, das ist ein Offset in der Ausdrucktabelle (cetab.c),     */
/*       das zweite Byte enthaelt verschiedene Informationen fuer die     */
/*       Displayfunktionen (cdispl.c,  cfdispl.c ) und das dritte Byte    */
/*       gibt an wieviele Spalten fuer die Darstellung der folgenden      */
/*       Textzeichen bzw. Unterausdruecke reserviert werden muessen.      */
/*       Fuer Unterausdruecke wird eine, fuer Listen von Unterausdruecken */
/*       werden zwei Spalten reserviert.                                  */
/*                                                                        */
/**************************************************************************/

/* achtung: \5 wird fuer sonderzwecke eingesetzt, falls benoetigt:    */
/* cmatch.c aendern (fuer notwendiges teilwort)                       */

struct kwrd kwtb[] = {
E_EXPR,       AR0(AP),         "ap \33\0\0\0\nto [\3 \33\1\6\2 ]\3",   1,8,1
,E_EXPR,       AR0(SUB),       "sub [\3 \33\3\5\2 ]\3\nin \33\0\2\0",  1,9,1
#if ClausR
,E_EXPR,       AR0(SUB_CR),    "[.\3 \33\3\5\2 ].\3 \33\0\2\0",  0,8,1
#endif /* ClausR	*/
#ifdef AltV
,E_EXPR,       AR0(E_LET),     "let \33\2\0\4 = \33\0\0\0\nin \33\0\2\0",  1,9,1
#else
,E_EXPR,	AR0(E_LET),	"let \33\63\105\0\nin \33\0\2\0",	1,6,1
,E_LDEC,	AR0(LDEC),	"\33\64\0\4 =\3 \33\0\2\0",		0,4,2

,E_EXPR,	AR0(LETPAR),	"letpar \33\63\105\0\nin \33\0\2\0",	1,9,1
/*
,E_LDEC,	AR0(LDEC),	"\33\64\0\4 =\3 \33\0\2\0",		0,4,2
*/

#if LET_P
,E_EXPR,	AR0(E_LETP),	"letp \33\66\105\0\nin \33\0\2\0",  1,6,1
,E_PDEC,	AR0(PDEC),	"\33\42\0\4 =\3 \33\0\2\0",		0,4,1
/* alt: ,E_PDEC,	AR0(PDEC),	"\33\67\0\4 =\3 \33\0\2\0",		0,4,1	*/
#endif

#endif

/* Besma	*/
,E_EXPR,	AR0(E_ALLOC),	"alloc \33\63\105\0\nin \33\0\2\0",	1,6,1
,E_LDEC,	AR0(LDEC),	"\33\64\0\4 =\3 \33\0\2\0",		0,4,2

,E_EXPR,	AR0(E_SEQ),	"seq \33\1\107\0\nin \33\0\2\0",	1,6,1
,E_EXPR,	AR0(E_ASSIGN),	"set \33\74\0\1 =\3 \33\0\2\0",		0,6,2
,E_VARSEL,	AR0(E_SEL),	"sel \33\64\0\1 \33\0\2\0",		0,4,2
/* Besma	*/


,E_EXPR	,AR0(ZF_UH)	,"{.\3 \33\0\0\6 | \33\1\6\2 }\3"	,0,10,0
,E_EXPR	,AR0(ZFIN_UH)	,"<-\3 \33\2\0\2 \33\0\0\0"		,0,9,0

#ifndef NOT_REC
,E_EXPR,       AR0(REC),        "rec \33\2\0\0\n:\3 \33\0\2\0",         1,4,1
#endif
,E_EXPR,       AR0(IF),         "if \33\0\0\0\n\33\4\12\0",             1,3,0
/* nichts hier zwischen, da cont-bit ! */
,E_COND,       AR0(THEN),       "then \33\0\0\0\nelse \33\0\2\0",       1,5,0
#ifdef UnBenutzt
,E_EXPR,       AR0(CASE),       "case \33\0\2\4 : \33\0\0\0\n\33\0\2\0",1,9,0
#endif
,E_EXPR,       AR0(E_COMM),     "/*\3 \33\14\0\3 */\3\n\33\0\2\0",	1,6,0
,E_EXPR,       AR0(APAL),       "(\3 \33\0\2\6 \33\0\2\4 \33\0\2\2 )\3",0,8,0
,E_ATOM,       NIL,             "<>\3",                                 0,1,0
,E_EXPR,       AR0(LIST),       "<\3 \33\1\6\2 >\3",                    0,5,0
,E_ATOM,       NILARRAY,        "[]\3",                                 0,1,0
,E_EXPR,       AR0(E_ARRAY),	"[\3 \33\1\6\2 ]\3",                    0,5,0
#if N_STR
,E_STRVAL,     NILSTR,          "'`\3",                                 0,1,0
,E_STRPATT,    NILSTR,          "'`\3",                                 0,1,0
,E_STRPATT,    AR0(KQUOTE),     "'\3\33\51\46\1`\3",                    0,3,0
,E_STRVAL,     AR0(KQUOTE),     "'\3\33\53\46\1`\3",                    0,3,0
 /*-dg 12.07.90 korrektur */
 /*-dg */ ,E_STRPATT,    AR0(EPM_SAS),    "\"as \33\50\2\3 \"\33\2\2\1\"\3",        0,7,0 /*dg*/
 /*-dg */ ,E_STRPATT,    AR0(EPM_IN),     "\"in \33\60\2\3 \33\14\2\1\"\3",       0,7,0 /*dg*/
,E_STRPATT,    DQUOTE,          "\"\"\3",                               0,1,0
,E_STRPATT,    DHASH,           "\"#\3",                                0,1,0
,E_STRPATT,    DNOPAR,		"\"@\3",                                0,1,0
,E_STRPATT,    DPERC,           "\"%\3",                                0,1,0
,E_STRPATT,	DSINGLE,	"\"'\3",                                0,1,0
,E_STRPATT,	DBACK,		"\"`\3",                                0,1,0
,E_CHAR,       DQUOTE,          "\"\"\3",                               0,1,0
,E_CHAR,       DHASH,           "\"#\3",                                0,1,0
,E_CHAR,	DNOPAR,		"\"@\3",                                0,1,0
,E_CHAR,       DPERC,           "\"%\3",                                0,1,0
,E_CHAR,       DSINGLE,		"\"'\3",                                0,1,0
,E_CHAR,       DBACK,		"\"`\3",                                0,1,0
,E_STRPATT,	PM_SSKSKIP       ,"\"...\3 "                           ,0,4,0
,E_STRPATT,	PM_SSKPLUS       ,"\".+\3 "                            ,0,3,0
,E_STRPATT,	PM_SSKSTAR       ,"\".*\3"				,0,2,0
,E_STRPATT,	PM_SSKIP         ,"\".\3 "                            ,0,2,0
,E_STRPATT,	PM_SDOLLAR       ,"\"$\3 "                            ,0,2,0
,E_SKIP,	PM_SSKSKIP       ,"\"...\3"	,0,3,0
,E_SKIP,	PM_SSKPLUS       ,"\".+\3"	,0,2,0
,E_SKIP,	PM_SSKSTAR       ,"\".*\3"	,0,2,0
,E_SKIP,	PM_SSKIP         ,"\".\3"	,0,1,0
,E_SKIP,	PM_SDOLLAR       ,"\"$\3"                                     ,0,1,0
,E_STRPATT,    AR0(EPM_VAR),    "\"\3\33\2\2\1\"\3",                    0,3,0

/*
,E_EXPR,	AR0(E_DISON)	,"dis$on\3[ \33\0\0\6, \33\0\0\4, \33\0\0\0 ]\3"  ,0,14,0
*/
,E_EXPR,	AR0(E_DISON)	,"dis$on\3[ \33\0\0\10, \33\0\0\6, \33\0\0\4, \33\0\0\0 ]\3"  ,0,16,0

,E_EXPR,	AR0(E_MAP_)	,"map$_\3[ \33\0\0\4, \33\0\0\0 ]\3"  ,0,10,0
,E_EXPR,	AR0(E_MAP)	,"map$\3[ \33\0\0\4, \33\0\0\0 ]\3"  ,0,9,0
,E_EXPR,	AR0(E_ONEOF)	,"one$of\3[ \33\0\0\0 ]\3"  ,0,7,0
,E_EXPR,	AR0(E_SUCCESS)	,"success$\3[ \33\0\0\0 ]\3"  ,0,9,0

,E_LIST,       NIL,             "<>\3",                                 0,1,0
,E_LIST,       AR0(LIST),       "<\3 \33\1\6\2 >\3",                    0,5,0
#endif

,E_EXPR,      AR0(EPM_EREC)    ,"caserec_e \33\2\0\0\n\33\40\105\0\n\33\41\22\0"
                                                                      ,2,11,0
,E_EXPR,      AR0(EPM_FREC)    ,"caserec_f \33\2\0\0\n\33\40\105\0\n\33\41\22\0"
                                                                      ,2,11,0
,E_EXPR,      AR0(EPM_REC)     ,"caserec \33\2\0\0\n\33\40\105\0\n\33\41\22\0"
                                                                      ,2,9,0
,E_EXPR,      AR0(EPM_FCASE)   ,"case_f \n\33\40\105\0\n\33\41\22\0"   ,2,7,0
,E_EXPR,      AR0(EPM_ECASE)   ,"case_e \n\33\40\105\0\n\33\41\22\0"   ,2,7,0
,E_EXPR,      AR0(EPM_SWITCH)  ,"case \n\33\40\105\0\n\33\41\22\0"     ,2,5,0
,E_EXPR,      AR0(EPM_SWITCH)  ,"switch \n\33\40\105\0\n\33\41\22\0"   ,2,7,0
,E_OTHER,     PM_END           ,"end"                                  ,0,2,0
,E_OTHER,     AR0(EPM_OTHERWISE),"otherwise \33\0\0\0\nend"            ,1,10,0
,E_WHEN,      AR0(EPM_WHEN)    ,"when \33\42\2\15 guard \33\0\2\5 do \33\0\2\0"
                                                                      ,0,18,0 | VERTIKAL
,E_WHEN,      AR0(EPM_WHEN)    ,"when \33\42\2\0\n guard \33\0\2\0\n do \33\0\2\0"
                                                                      ,2,8,0
,E_WHEN,      AR0(EPM_MATCH)   ,"match \33\42\2\6 \33\0\2\4 \33\0\2\2 \33\0\2\0"
                                                                      ,0,12,0
#if N_STR
,E_N_PATT,    AR0(NPATT)      ,"(\3 \33\43\6\2 )\3"                    ,0,5,0
,E_PATT1,     AR0(PM_VARC)    ,"^\3\33\2\2\0"                          ,0,2,0
 /*-dg 12.07.90 korrektur */
 /*-dg */ ,E_PATT1,     AR0(EPM_AS)     ,"as \33\57\2\2 \33\2\2\0"               ,0,5,0
 /*-dg */ ,E_PATT1,     AR0(EPM_LIST)   ,"<(\3\33\2\2\3)\3 \33\57\2\0"           ,0,5,0
 /*-dg */ ,E_PATT1,     AR0(EPM_LIST)   ,"list \33\2\2\2 \33\57\2\0"             ,0,7,0
,E_PATT1,     AR0(LIST)       ,"<\3 \33\43\6\2 >\3"                    ,0,5,0
,E_PATT1,     AR0(EPM_NOT)    ,"not \33\43\2\0"                        ,0,5,0
,E_WH_PA,     PM_FAIL         ,"fail"                                  ,0,3,0
,E_WH_PA,     PM_SUCC         ,"ok  "                                  ,0,3,0
,E_PATT1,     PM_SKSKIP       ,"...\3"                                 ,0,2,0
,E_PATT1,     PM_SKPLUS       ,".+\3"                                  ,0,1,0
,E_PATT1,     PM_SKSTAR       ,".*\3"                                  ,0,1,0
,E_PATT1,     PM_SKIP         ,".\3"                                   ,0,0,0
,E_PATT1,     PM_DOLLAR       ,"$\3"                                   ,0,0,0
#else
 /*-dg 12.07.90 korrektur */
 /*-dg */ ,E_PATTERN,   AR0(EPM_AS)     ,"as \33\57\2\2 \33\2\2\0"               ,0,5,0
 /*-dg */ ,E_PATTERN,   AR0(EPM_LIST)   ,"<(\3\33\2\2\3)\3 \33\57\2\0"           ,0,5,0
 /*-dg */ ,E_PATTERN,   AR0(EPM_LIST)   ,"list \33\2\2\2 \33\57\2\0"             ,0,7,0
,E_PATTERN,   AR0(LIST)       ,"<\3 \33\43\6\2 >\3"                    ,0,5,0
,E_PATTERN,   AR0(EPM_NOT)    ,"not \33\43\2\0"                        ,0,5,0
,E_WH_PA  ,   PM_FAIL         ,"fail"                                  ,0,3,0
,E_WH_PA  ,   PM_SUCC         ,"ok  "                                  ,0,3,0
,E_PATTERN,   PM_SKSKIP       ,"...\3"                                 ,0,2,0
,E_PATTERN,   PM_SKIP         ,".\3"                                   ,0,0,0
,E_PATTERN,   PM_DOLLAR       ,"$\3"                                   ,0,0,0
#endif
/*
,E_WHEN,      AR0(EPM_WHEN)    ,"when \33\57\2\2 \33\44\12\0"          ,0,7,0
,E_GUARD,     AR0(EPM_GUARD)   ,"guard \33\0\2\2 \33\45\12\0"          ,0,8,0
,E_DO,        AR0(EPM_DO)      ,"do \33\0\2\0"                         ,0,4,0
*/

#if ClausR
,E_EXPR,       AR0(N_CR),       "N. \33\0\0\0\n   \33\0\2\0",       1,6,0

,E_EXPR,      AR0(CASE_CR)  ,"case_c \n\33\71\105\0\n\33\41\22\0"     ,5,5,0
,E_WHEN_CR,   AR0(WHEN_CR)  ,"when \33\42\2\5 do \33\0\2\0"
                                                                      ,0,11,0 | VERTIKAL
,E_WHEN_CR,   AR0(WHEN_CR)  ,"when \33\42\2\0\n do \33\0\2\0"
                                                                      ,1,7,0
,E_DECL,      AR0(DECL_CR), "globals \n  \33\30\105\0\nin \33\0\2\0",   5,3,2
,E_EXPR,      AR0(STRUCT_CR)  ,"struct \n\33\73\105\0\n\33\41\22\0"     ,5,5,0
,E_WHENST_CR,   AR0(WHENST_CR)  ,"entry \33\14\2\5 : \33\0\2\0"
                                                                      ,0,11,0 | VERTIKAL
,E_WHENST_CR,   AR0(WHENST_CR)  ,"entry \33\14\2\0\n : \33\0\2\0"
                                                                      ,1,7,0
,E_EXPR,      AR0(PROT_CR),    "&\3\33\5\2\0",               0,1,0

,E_EXPR,      AR0(OBJ_CR)  ,"object \n\33\73\105\0\n\33\41\22\0"     ,5,6,0
,E_WHENST_CR,   AR0(WHENST_CR)  ,"entry \33\14\2\5 : \33\0\2\0"
                                                                      ,0,11,0 | VERTIKAL
,E_WHENST_CR,   AR0(WHENST_CR)  ,"entry \33\14\2\0\n : \33\0\2\0"
                                                                      ,1,7,0
#endif /* ClausR	*/

,E_EXPR,	AR0(E_ERROR)	,"error\3: \33\0\0\0"			,0,7,0
,E_EXPR,	AR0(E_STOP)	,"stop\3: \33\1\6\0"			,0,7,0
/* achtung hier: PREDEF gesetzt wg. encsearch ? */
,E_LBAR,       AR0(PROTECT),    "\\\3\33\5\2\0",               0,1,0 | PREDEF

,E_EXPR,       AR0(FILNAM),   "$\3\33\2\2\0",                         0,1,0
,E_EXPR,       AR0(FILRED),   "^\3\33\2\2\0",                         0,1,0
,E_EXPR,       AR0(FILPRE),   "!\3\33\2\2\0",                         0,1,0
#ifdef UnBenutzt
,E_SCAL,      AR0(DECSCAL),      "dec \33\10\2\0",                    0,4,0
,E_EXPR,      AR0(F),        "rat [\3 \33\10\2\6 | \33\10\2\2 ]\3",   0,12,0

,E_SET,       AR0(SET),          "set \n\33\16\12\0",                 1,3,0
,E_SETOBJ,    AR0(LIST),         "<\3 \33\7\6\2 >\3",                 0,5,0
#endif

#if N_MAT
,E_MAT,     AR0(FIELDFIELD),"mmat \n\33\26\116\1.\3"            ,1,6,0
,E_FMATRL,  AR0(LIST)     ,"<\3 \33\25\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(NUMBFIELD),"mat \n\33\17\116\0.\3"              ,1,4,0
,E_NMATRL,  AR0(LIST)     ,"<\3 \33\22\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(BOOLFIELD),"bmat \n\33\20\116\0.\3"             ,1,4,0
,E_BMATRL,  AR0(LIST)     ,"<\3 \33\23\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(STRFIELD) ,"smat \n\33\21\116\0.\3"             ,1,4,0
,E_SMATRL,  AR0(LIST)     ,"<\3 \33\24\6\2 >\3"                 ,0,5,0
,E_FMATEL,  NILFIELD      ,"nilmat"                             ,0,5,0

,E_NVECOBJ, AR0(NVECT)    ,"vect\3< \33\11\6\2 >\3"             ,0,9,0
,E_BVECOBJ, AR0(BVECT)    ,"bvect\3< \33\13\6\2 >\3"            ,0,10,0
,E_SVECOBJ, AR0(SVECT)    ,"svect\3< \33\15\6\2 >\3"            ,0,10,0
,E_ATOM,    NILVECT       ,"nilvect"                            ,0,6,0

,E_NVECOBJ, AR0(NTVECT)   ,"tvect\3< \33\11\106\2 >\3"          ,0,10,0
,E_BVECOBJ, AR0(BTVECT)   ,"btvect\3< \33\13\106\2 >\3"         ,0,11,0
,E_SVECOBJ, AR0(STVECT)   ,"stvect\3< \33\15\106\2 >\3"         ,0,11,0
,E_ATOM,    NILTVECT      ,"niltvect"                           ,0,7,0
#else
,E_MAT,     AR0(FIELDFIELD),"ffield \n\33\26\116\1.\3"          ,1,8,0
,E_FMATRL,  AR0(LIST)     ,"<\3 \33\25\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(NUMBFIELD),"field \n\33\17\116\0.\3"            ,1,6,0
,E_NMATRL,  AR0(LIST)     ,"<\3 \33\22\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(BOOLFIELD),"bfield \n\33\20\116\0.\3"           ,1,6,0
,E_BMATRL,  AR0(LIST)     ,"<\3 \33\23\6\2 >\3"                 ,0,5,0
,E_MAT,     AR0(STRFIELD) ,"sfield \n\33\21\116\0.\3"           ,1,6,0
,E_SMATRL,  AR0(LIST)     ,"<\3 \33\24\6\2 >\3"                 ,0,5,0
,E_FMATEL,  NILFIELD      ,"nilfield"                           ,0,7,0

#endif

,E_PLACEH,       PLACEH,         "#\3",                        0,0,0
,E_ATOM,         NIL,            "nil",                        0,2,0


,E_SETOBJ,       ESET,           "emptyset",                   0,4,0
/*
#if !STELEMDEF
,E_ATOM,         EXPRESSION,     "Expr",                       0,3,0
#endif
*/

,E_TYPE,         DECIMAL,        "number",                     0,5,0
,E_TYPE,         DECIMAL,        "Number",                     0,5,0
,E_TYPE,         DECIMAL,        "num",                        0,2,0
,E_TYPE,         DECIMAL,        "Num",                        0,2,0
,E_TYPE,         CHAR,           "string",                     0,5,0
,E_TYPE,         CHAR,           "String",                     0,5,0
,E_TYPE,         BOOL,           "bool",                       0,3,0
,E_TYPE,         BOOL,           "Bool",                       0,3,0
,E_BOOLVAL,      SA_TRUE,        "true",                       0,3,0
,E_BOOLVAL,      SA_FALSE,       "false",                      0,4,0

,E_ATOM,         SA_NULL,        "null",                       0,3,0
,E_ATOM,         SA_TERMINATE,   "terminate",                  0,8,0

,E_ATOM,   LAMBDA       ,"lambda"                            ,0,5,0
,E_ATOM,   LAMBDA       ,"Lambda"                            ,0,5,0
,E_ATOM,   CONDITIONAL  ,"cond"                              ,0,10,0
,E_ATOM,   CONDITIONAL  ,"Cond"                              ,0,10,0
,E_ATOM,   PRIMFUNC     ,"primfunc"                          ,0,7,0
,E_ATOM,   PRIMFUNC     ,"Primfunc"                          ,0,7,0
,E_ATOM,   USERFUNC     ,"userfunc"                          ,0,7,0
,E_ATOM,   USERFUNC     ,"Userfunc"                          ,0,7,0

,E_TYPE,   CL_SCALAR    ,"scalar"                              ,0,5,0
,E_TYPE,   CL_SCALAR    ,"Scalar"                              ,0,5,0
,E_TYPE,   CL_VECTOR    ,"vector"                              ,0,5,0
,E_TYPE,   CL_VECTOR    ,"Vector"                              ,0,5,0
,E_TYPE,   CL_TVECTOR   ,"tvector"                             ,0,6,0
,E_TYPE,   CL_TVECTOR   ,"Tvector"                             ,0,6,0
#if N_MAT
,E_TYPE,   CL_MATRIX    ,"matrix"                              ,0,5,0
,E_TYPE,   CL_MATRIX    ,"Matrix"                              ,0,5,0
,E_TYPE,   TFIELD       ,"mat"                                 ,0,2,0
,E_TYPE,   TFIELD       ,"Mat"                                 ,0,2,0
#else
,E_TYPE,   TFIELD       ,"field"                               ,0,4,0
,E_TYPE,   TFIELD       ,"Field"                               ,0,4,0
#endif
,E_ATOM,   CL_SET       ,"set"                                 ,0,2,0
,E_ATOM,   CL_SET       ,"Set"                                 ,0,2,0
,E_ATOM,   CL_TREE      ,"list"                                ,0,3,0
,E_ATOM,   CL_TREE      ,"List"                                ,0,3,0
,E_ATOM,   CL_TREE      ,"tree"                                ,0,3,0
,E_ATOM,   CL_TREE      ,"Tree"                                ,0,3,0
,E_ATOM,   CL_FUNCTION  ,"function"                            ,0,7,0
,E_ATOM,   CL_FUNCTION  ,"Function"                            ,0,7,0
/*
,E_ATOM,   CL_DVECTOR   ,"Digitstring"                         ,0,10,0
,E_ATOM,   CL_CVECTOR   ,"Charstring"                          ,0,9,0
*/

,E_FUNC,   M_NOT        ,"not"                   ,0,2,0 | PREDEF
,E_FUNC,   M_ABS        ,"abs"                   ,0,2,0 | PREDEF
,E_FUNC,   M_TRUNCATE   ,"trunc"                 ,0,4,0 | PREDEF
,E_FUNC,   M_FLOOR      ,"floor"                 ,0,4,0 | PREDEF
,E_FUNC,   M_CEIL       ,"ceil"                  ,0,3,0 | PREDEF
,E_FUNC,   M_FRAC       ,"frac"                  ,0,3,0 | PREDEF
/* struck        */
,E_FUNC,   M_SIN        ,"sin"                   ,0,2,0 | PREDEF
,E_FUNC,   M_COS        ,"cos"                   ,0,2,0 | PREDEF
,E_FUNC,   M_TAN        ,"tan"                   ,0,2,0 | PREDEF
/*
,E_FUNC,   M_ARCSIN     ,"arcsin"                ,0,5,0 | PREDEF
,E_FUNC,   M_ARCCOS     ,"arccos"                ,0,5,0 | PREDEF
,E_FUNC,   M_ARCTAN     ,"arctan"                ,0,5,0 | PREDEF
*/
,E_FUNC,   M_LN         ,"ln"                    ,0,2,0 | PREDEF
,E_FUNC,   M_EXP        ,"exp"                   ,0,3,0 | PREDEF
#ifndef MEINE_REDUMA
,E_FUNC,   M_EMPTY      ,"empty"                 ,0,4,0 | PREDEF
#endif
,E_FUNC     ,M_NEG           ,"neg"                   ,0 ,2,0 | PREDEF

,E_FUNC     ,LDIMENSION      ,"ldim"                  ,0 ,3,0 | PREDEF
,E_FUNC     ,LCUT            ,"lcut"                  ,0 ,3,0 | PREDEF
,E_FUNC     ,LSELECT         ,"|\3"                   ,0 ,1,0 | PRIM_FKT
,E_FUNC     ,LSELECT         ,"lselect"               ,0 ,6,0 | PREDEF
,E_FUNC     ,LROTATE         ,"lrotate"               ,0 ,6,0 | PREDEF
,E_FUNC     ,LUNI            ,"++\3"                  ,0 ,1,0 | PRIM_FKT
,E_FUNC     ,LUNI            ,"lunite"                ,0 ,5,0 | PREDEF
,E_FUNC     ,LREPLACE        ,"lreplace"              ,0 ,7,0 | PREDEF
,E_FUNC     ,REVERSE         ,"reverse"               ,0 ,7,0 | PREDEF
,E_FUNC     ,LTRANSFORM      ,"ltransform"            ,0 ,9,0 | PREDEF

,E_FUNC    ,DIMENSION       ,"dim"             ,0 ,2,0 | PREDEF
,E_FUNC    ,CUT             ,"cut"             ,0 ,2,0 | PREDEF
,E_FUNC    ,SELECT          ,"select"          ,0 ,5,0 | PREDEF
,E_FUNC    ,ROTATE          ,"rotate"          ,0 ,5,0 | PREDEF
,E_FUNC    ,UNI             ,"unite"           ,0 ,4,0 | PREDEF
#if N_MAT
,E_FUNC    ,MCUT            ,"mcut"            ,0 ,3,0 | PREDEF
,E_FUNC    ,MSELECT         ,"mselect"         ,0 ,6,0 | PREDEF
,E_FUNC    ,MROTATE         ,"mrotate"         ,0 ,6,0 | PREDEF
,E_FUNC    ,MUNI            ,"munite"          ,0 ,5,0 | PREDEF
,E_FUNC    ,MDIM            ,"mdim"            ,0 ,3,0 | PREDEF
,E_FUNC    ,REPLACE_C       ,"mreplace_c"      ,0 ,9,0 | PREDEF
,E_FUNC    ,REPLACE_R       ,"mreplace_r"      ,0 ,9,0 | PREDEF
,E_FUNC    ,MREPLACE        ,"mreplace"        ,0 ,7,0 | PREDEF
,E_FUNC    ,REPSTR          ,"repstr"          ,0 ,5,0 | PREDEF
,E_FUNC    ,SUBSTR          ,"substruct"       ,0 ,8,0 | PREDEF
,E_FUNC    ,SUBSTR          ,"substr"          ,0 ,5,0 | PREDEF
,E_FUNC    ,TOLIST          ,"to_list"         ,0 ,6,0 | PREDEF
,E_FUNC    ,FROMLIST        ,"to_field"        ,0 ,7,0 | PREDEF
,E_FUNC    ,MKSTRING        ,"mkstring"        ,0 ,7,0 | PREDEF
#endif
,E_FUNC    ,REPLACE         ,"replace"         ,0 ,6,0 | PREDEF
,E_FUNC    ,TRANSPOSE       ,"transpose"       ,0 ,8,0 | PREDEF
,E_FUNC    ,TRANSFORM       ,"transform"       ,0 ,8,0 | PREDEF
#if N_MAT
,E_FUNC    ,VDIM            ,"vdim"            ,0 ,3,0 | PREDEF
,E_FUNC    ,VCUT            ,"vcut"            ,0 ,3,0 | PREDEF
,E_FUNC    ,VSELECT         ,"vselect"         ,0 ,6,0 | PREDEF
,E_FUNC    ,VROTATE         ,"vrotate"         ,0 ,6,0 | PREDEF
,E_FUNC    ,VUNI            ,"vunite"          ,0 ,5,0 | PREDEF
,E_FUNC    ,VREPLACE        ,"vreplace"        ,0 ,7,0 | PREDEF

,E_FUNC    ,TO_SCAL         ,"to_scal"         ,0 ,6,0 | PREDEF
,E_FUNC    ,TO_VECT         ,"to_vect"         ,0 ,6,0 | PREDEF
,E_FUNC    ,TO_TVECT        ,"to_tvect"        ,0 ,7,0 | PREDEF
,E_FUNC    ,TO_MAT          ,"to_mat"          ,0 ,5,0 | PREDEF
#endif

,E_FUNC,   D_PLUS       ,"+\3"                  ,0,0,0 | PRIM_FKT
,E_FUNC,   D_MINUS      ,"-\3"                  ,0,0,0 | PRIM_FKT
,E_FUNC,   D_MULT       ,"*\3"                  ,0,0,0 | PRIM_FKT
,E_FUNC,   D_DIV        ,"/\3"                  ,0,0,0 | PRIM_FKT
,E_FUNC,   D_PLUS       ,"plus"                 ,0,3,0 | PREDEF
,E_FUNC,   D_MINUS      ,"minus"                ,0,4,0 | PREDEF
,E_FUNC,   D_MULT       ,"mult"                 ,0,3,0 | PREDEF
,E_FUNC,   D_DIV        ,"div"                  ,0,2,0 | PREDEF
,E_FUNC,   D_MOD        ,"mod"                  ,0,2,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_OR         ,"or"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_AND        ,"and"                  ,0,2,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_EXOR       ,"xor"                  ,0,2,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_EQ         ,"eq"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_NEQ        ,"ne"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   F_EQ         ,"f_eq"                 ,0,3,0 | PREDEF | PRIM_FKT
,E_FUNC,   F_NEQ        ,"f_ne"                 ,0,3,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_LESS       ,"lt"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_LEQ        ,"le"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_GREAT      ,"gt"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_GEQ        ,"ge"                   ,0,1,0 | PREDEF | PRIM_FKT
,E_FUNC,   D_MAX        ,"max"                  ,0,2,0 | PREDEF
,E_FUNC,   D_MIN        ,"min"                  ,0,2,0 | PREDEF

,E_FUNC,   IP_PLUS      ,"ip"                   ,0,1,0 | PREDEF

,E_FUNC,   CLASS        ,"class"                ,0,4,0 | PREDEF
,E_FUNC,   TYPE         ,"type"                 ,0,3,0 | PREDEF

,E_FUNC,   C_MAX        ,"c_max"                ,0,4,0 | PREDEF
,E_FUNC,   C_MIN        ,"c_min"                ,0,4,0 | PREDEF
,E_FUNC,   C_PLUS       ,"c_+"                  ,0,2,0
,E_FUNC,   C_MINUS      ,"c_-"                  ,0,2,0
,E_FUNC,   C_MULT       ,"c_*"                  ,0,2,0
,E_FUNC,   C_DIV        ,"c_/"                  ,0,2,0
,E_FUNC,   C_PLUS       ,"c_plus"               ,0,5,0 | PREDEF
,E_FUNC,   C_MINUS      ,"c_minus"              ,0,6,0 | PREDEF
,E_FUNC,   C_MULT       ,"c_mult"               ,0,5,0 | PREDEF
,E_FUNC,   C_DIV        ,"c_div"                ,0,4,0 | PREDEF
#if N_MAT
,E_FUNC,   VC_MAX        ,"vc_max"                ,0,5,0 | PREDEF
,E_FUNC,   VC_MIN        ,"vc_min"                ,0,6,0 | PREDEF
,E_FUNC,   VC_PLUS       ,"vc_+"                  ,0,3,0
,E_FUNC,   VC_MINUS      ,"vc_-"                  ,0,3,0
,E_FUNC,   VC_MULT       ,"vc_*"                  ,0,3,0
,E_FUNC,   VC_DIV        ,"vc_/"                  ,0,3,0
,E_FUNC,   VC_PLUS       ,"vc_plus"               ,0,6,0 | PREDEF
,E_FUNC,   VC_MINUS      ,"vc_minus"              ,0,7,0 | PREDEF
,E_FUNC,   VC_MULT       ,"vc_mult"               ,0,6,0 | PREDEF
,E_FUNC,   VC_DIV        ,"vc_div"                ,0,5,0 | PREDEF
#endif

#if ClausR
,E_FUNC,   AND_CR	,"AND"                ,0,2,0 | PREDEF
,E_FUNC,   OR_CR	,"OR"			,0,1,0 | PREDEF
,E_FUNC,   FILTER_CR	,"filter"		,0,5,0 | PREDEF
,E_FUNC,   SEL_CR        ,"sel"                ,0,2,0 | PREDEF
,E_FUNC,   UPDATE_CR     ,"update"             ,0,5,0 | PREDEF
,E_FUNC,   DELETE_CR     ,"delete"             ,0,5,0 | PREDEF
,E_FUNC,   UNIFY_CR      ,"unify"              ,0,4,0 | PREDEF
,E_FUNC	,ISN_CR		,"is_N"			,0,3,0 | PREDEF
,E_FUNC	,LEFT_CR	,"left"			,0,3,0 | PREDEF
,E_FUNC	,RIGHT_CR	,"right"		,0,4,0 | PREDEF
,E_FUNC	,MAP_CR		,"map"			,0,2,0 | PREDEF
#endif /* ClausR	*/

,E_ATOM,   SELF_S        ,"self$s"                ,0,5,0
,E_ATOM,   SELF_V        ,"self$v"                ,0,5,0
,E_ATOM,   DISI_S        ,"disi$s"                ,0,5,0
,E_ATOM,   DISI_V        ,"disi$v"                ,0,5,0
,E_ATOM,   DISE_S        ,"dise$s"                ,0,5,0
,E_ATOM,   DISE_V        ,"dise$v"                ,0,5,0
,E_ATOM,   SYS_S        ,"sys$s"                ,0,4,0
,E_ATOM,   SYS_V        ,"sys$v"                ,0,4,0

,E_EXPR,	AR0(CONS),	"o \33\0\0\2 \3\33\0\2\0",	0,4,0
,E_FUNC,	D_CONS,		":",			0 ,0,0 | PRIM_FKT
,E_FUNC,	D_CONS,		"cons",			0 ,0,0 | PREDEF
,E_FUNC,	M_FIRST,	"hd",			0 ,3,0 | PREDEF
,E_FUNC,	M_REST,		"tl",			0 ,3,0 | PREDEF
,E_EXPR,	AR0(E_DOT),	"dot \33\0\0\2.\3\33\0\2\0",	0,6,0
,E_FUNC,	M_HEAD,		"head",			0 ,4,0 | PREDEF
,E_FUNC,	M_TAIL,		"tail",			0 ,3,0 | PREDEF

/* MUCHA */
#ifdef MUCHA
,E_FUNC ,M_FIRST        ,"first"                       ,0 ,4,0 | PREDEF
,E_FUNC ,M_REST         ,"rest"                        ,0 ,3,0 | PREDEF
,E_FUNC ,M_SETEMPTY     ,"setempty"                    ,0 ,7,0 | PREDEF
,E_FUNC ,D_EQELEM       ,"eqelem"                      ,0 ,5,0 | PREDEF
,E_FUNC ,D_INSELEM      ,"inselem"                     ,0 ,6,0 | PREDEF
,E_FUNC ,D_MEMBER       ,"member"                      ,0 ,5,0 | PREDEF
,E_FUNC ,D_UNION        ,"union"                       ,0 ,4,0 | PREDEF
,E_FUNC ,D_INTERSEC     ,"intersec"                    ,0 ,7,0 | PREDEF
,E_FUNC ,D_DELELEM      ,"delelem"                     ,0 ,6,0 | PREDEF
#endif
/* MUCHA */

,E_FUNC	,LGEN_UH	,"lgen"				,0 ,3,0 | PREDEF

#ifdef MEINE_REDUMA
,E_FUNC,   APPEND       ,"append"                        ,0,5,0 | PREDEF
,E_FUNC,   FIRST        ,"first"                         ,0,4,0 | PREDEF
,E_FUNC,   SECOND       ,"second"                        ,0,5,0 | PREDEF
,E_FUNC,   REST         ,"rest"                          ,0,3,0 | PREDEF
,E_FUNC,   EMPTY        ,"empty"                         ,0,4,0 | PREDEF
#endif

,E_NOPAR,     NOPAR,         "@\3",                         0,0,2
,E_FDEC,      AR0(FILFKT),   "^\3\33\2\2\0",                0,1,2
,E_FDEC,      AR0(FDEC)
,"\33\32\2\13 \5[\3 \33\3\205\6 ]\3 \5=\3 \33\36\2\0",          0,12,2 | VERTIKAL
,E_FDEC,      AR0(FDEC)
,"\33\32\2\11 \5[\3 \33\3\205\4 ]\3 \5=\3\n \33\36\2\0",          1,10,2
/*
,E_FDEC,      AR0(FDEC)
"_func_ \33\32\2\13 \5[\3 \33\3\205\6 ]\3 \5=\3 \33\36\2\0",          0,19,2
*/
/*
,E_DECL,      AR0(DECL),      "def \n  \33\30\105\0\nin \33\33\2\0",   3,3,2
*/
,E_DECL,      AR0(DECL),      "def \n  \33\30\105\0\nin \33\0\2\0",   3,4,2

/* ___________________________________________________________________ */
/* Achtung: nichts zwischen diesen CALL's einfuegen, da mit offset in  */
/* dieser Tabelle gearbeitet wird (vgl. scanexp).                      */
/* Neu: CALL fname [ .. ]  : normal (d.h. bel. Stelligkeit)            */
/*      CALL pred ( . )    : vordefinierte Funktion (feste Stelligkeit)*/
/* Vgl. auch cedit.h PRED_KL, CALL_KL !                                */

,E_CALL,         AR0(CALL),   "\33\35\2\7 \5[\3 \33\1\206\2 ]\3", 0,7,0 | PRINT
,E_CALL,         AR0(CALL),   "_call_ \33\35\2\7 \5[\3 \33\1\206\2 ]\3", 0,14,0
,E_CALL,         AR0(CALL1),
  "\33\35\2\6\5(\3 \33\0\2\2 )\3",                              0,7,0
,E_CALL,         AR0(CALL2),
  "\33\35\2\12\5(\3 \33\0\2\6 , \33\0\2\2 )\3",                 0,11,0
,E_CALL,         AR0(CALL3),
  "\33\35\2\16\5(\3 \33\0\2\12 , \33\0\2\6 , \33\0\2\2 )\3",    0,15,0
,E_CALL,         AR0(CALL4),
  "\33\35\2\20\5(\3 \33\0\2\16 , \33\0\2\12 , \33\0\2\6 , \33\0\2\2 )\3",0,19,0
/* benutzer-definierte konstruktoren !      */
,E_USKON,  AR0(US_KON),   "\33\35\2\4\4{\3\33\1\206\1}\3",   0,5,0 | PRINT
,E_USKON,  AR0(US_KON),   "_kon_ \33\35\2\4\4{\3\33\1\206\1}\3",   0,11,0
,E_USKONP, AR0(US_KON),   "\33\35\2\4\4{\3\33\43\206\1}\3",  0,5,0 | PRINT
,E_USKONP, AR0(US_KON),   "_konp_ \33\35\2\4\4{\3\33\43\206\1}\3",  0,12,0

};

/**************************************************************************/

struct kwrd rkwtb[] = {
E_EXPR,  AR0(SYSAP)    ,"sysAP "                                ,0,7,0,
E_EXPR,  AR0(PLUS)     ,"plus "                                 ,0,7,0,
E_EXPR,  AR0(MINUS)    ,"minus "                                ,0,7,0,
E_EXPR,  AR0(SCALAR)   ,"scalar "                               ,0,7,0,
E_EXPR,  AR0(VECTOR)   ,"vector "                               ,0,7,0,
E_EXPR,  AR0(TVECTOR)  ,"tvector "                              ,0,7,0,
E_EXPR,  AR0(MATRIX)   ,"matrix "                               ,0,7,0,
E_EXPR,  AR0(EXP)      ,"exp "                                  ,0,7,0,
E_EXPR,  KLAA          ,"@"                                     ,0,0,0,
E_EXPR,  DOLLAR        ,"$"                                     ,0,0,0,
E_EXPR,  AQUOTE        ,"'"                                     ,0,0,0,
E_EXPR,  EQUOTE        ,"`"                                     ,0,0,0
,E_EXPR,  AR0(US_KON),   "us_kon "                              ,0,0,0
,E_EXPR,  AR0(CALL),     "call "                                ,0,0,0
,E_EXPR,  AR0(CALL1),    "call1 "                               ,0,0,0
,E_EXPR,  AR0(CALL2),    "call2 "                               ,0,0,0
,E_EXPR,  AR0(CALL3),    "call3 "                               ,0,0,0
,E_EXPR,  AR0(CALL4),    "call4 "                               ,0,0,0
,E_EXPR,  AR0(FDEC),     "fdec "                                ,0,0,0
,E_EXPR,  AR0(LDEC),     "ldec "                                ,0,0,0
,E_EXPR,  AR0(PM_OTHERWISE),     "PM_OTHERWISE "                                ,0,0,0
,E_EXPR,  AR0(PM_WHEN),     "PM_WHEN "                                ,0,0,0
,E_EXPR,  AR0(PM_MATCH),     "PM_MATCH "                                ,0,0,0
};

/* hier alle keywords auffuehren (lexikalisch geordnet !)       */
/* aber nicht predefined functions                              */
char *kwrds[] = {
         "Bool"
        ,"Cond"
/*
        ,"Expr"
*/
        ,"Function"
        ,"Lambda","List"
        ,"Matrix" ,"Mat"
        ,"Number"
        ,"Primfunc"
        ,"Scalar","Set","String"
        ,"Tree","Tvector"
        ,"Vector"
        ,"ap"
        ,"as"
        ,"bmat" ,"bool", "btvect", "bvect"
        ,"case"
#if ClausR
        ,"case_c"
#endif /* ClausR	*/
	,"cond"
        ,"def" ,"do"
        ,"else","emptyset" ,"end", "error"
        ,"false"
#if !N_MAT
        ,"field"
#endif
        ,"function"
        ,"guard"
        ,"if" ,"in"
        ,"lambda","let"
	,"list"
        ,"mat","match","matrix","mmat"
        ,"nil","null","num","number"
        ,"otherwise"
        ,"primfunc"     
        ,"rec"
        ,"scalar", "set" ,"smat","stmat","stop","string","stvect","svect" ,"sub" ,"switch"
        ,"terminate","then" ,"to","tree","true", "tvect","tvector"  
        ,"userfunc"    
        ,"vect" ,"vector"
        ,"when"
      };

/* hier alle predefined functions auffuehren (s.o.)     */
predefined functions[] = {
#if	ClausR
	"AND"		,AND_CR		,func_arity(AND_CR),
	"OR"		,OR_CR		,func_arity(OR_CR),
#endif	/*	ClausR	*/
	"abs"		,M_ABS		,func_arity(M_ABS)
	,"and"		,D_AND		,func_arity(D_AND)
#ifdef	MEINE_REDUMA
	,"append"	,APPEND		,func_arity(APPEND)
#endif
/*
	,"arccos"	,M_ARCCOS	,func_arity(M_ARCCOS)
	,"arcsin"	,M_ARCCOS	,func_arity(M_ARCSIN)
	,"arctan"	,M_ARCTAN	,func_arity(M_ARCTAN)
*/
	,"c_div"	,C_DIV		,func_arity(C_DIV)
	,"c_max"	,C_MAX		,func_arity(C_MAX)
	,"c_min"	,C_MIN		,func_arity(C_MIN)
	,"c_minus"	,C_MINUS	,func_arity(C_MINUS)
	,"c_mult"	,C_MULT		,func_arity(C_MULT)
	,"c_plus"	,C_PLUS		,func_arity(C_PLUS)
	,"ceil"		,M_CEIL		,func_arity(M_CEIL)
	,"class"	,CLASS		,func_arity(CLASS)
        ,"cons"                   ,D_CONS           ,func_arity(D_CONS)
	,"cos"		,M_COS		,func_arity(M_COS)
	,"cut"		,CUT		,func_arity(CUT)
/*	,"cutfirst"	,CUTFIRST	,func_arity(CUTFIRST)	*/
/*	,"cutlast"	,CUTLAST	,func_arity(CUTLAST)	*/
#if	ClausR
	,"delete"	,DELETE_CR	,func_arity(DELETE_CR)
#endif	/*	ClausR	*/
	,"dim"		,DIMENSION	,func_arity(DIMENSION)
	,"div"		,D_DIV		,func_arity(D_DIV)
/*	,"drop"		,DROP		,func_arity(DROP)	*/
#ifdef	MEINE_REDUMA
	,"empty"	,EMPTY		,func_arity(EMPTY)
#else
	,"empty"	,M_EMPTY	,func_arity(M_EMPTY)
#endif
	,"eq"		,D_EQ		,func_arity(D_EQ)
	,"exp"		,M_EXP		,func_arity(M_EXP)
	,"f_eq"		,F_EQ		,func_arity(F_EQ)
	,"f_ne"		,F_NEQ		,func_arity(F_NEQ)
#if	ClausR
	,"filter"	,FILTER_CR	,func_arity(FILTER_CR)
#endif	/*	ClausR	*/
#ifdef	MEINE_REDUMA
	,"first"	,FIRST		,func_arity(FIRST)
#endif
	,"floor"	,M_FLOOR	,func_arity(M_FLOOR)
	,"frac"		,M_FRAC		,func_arity(M_FRAC)
	,"ge"		,D_GEQ		,func_arity(D_GEQ)
	,"gt"		,D_GREAT	,func_arity(D_GREAT)
	,"head"		,M_HEAD		,func_arity(M_HEAD)
        ,"hd"                     ,M_FIRST          ,func_arity(M_FIRST)
/*	,"invert"	,INVERT		,func_arity(INVERT)	*/
	,"ip"		,IP_PLUS	,func_arity(IP_PLUS)
#if	ClausR
	,"is_N"		,ISN_CR		,func_arity(ISN_CR)
#endif	/*	ClausR	*/
	,"lcut"		,LCUT		,func_arity(LCUT)
	,"ldim"		,LDIMENSION	,func_arity(LDIMENSION)
	,"le"		,D_LEQ		,func_arity(D_LEQ)
#if	ClausR
	,"left"		,LEFT_CR	,func_arity(LEFT_CR)
#endif	/*	ClausR	*/
	,"lgen"		,LGEN_UH	,func_arity(LGEN_UH)
	,"ln"		,M_LN		,func_arity(M_LN)
	,"lreplace"	,LREPLACE	,func_arity(LREPLACE)
	,"lrotate"	,LROTATE	,func_arity(LROTATE)
	,"lselect"	,LSELECT	,func_arity(LSELECT)
	,"lt"		,D_LESS		,func_arity(D_LESS)
	,"ltransform"	,LTRANSFORM	,func_arity(LTRANSFORM)
/*	,"lubnorm"	,LUB_NORM	,func_arity(LUB_NORM)	*/
	,"lunite"	,LUNI		,func_arity(LUNI)
#if	ClausR
	,"map"		,MAP_CR		,func_arity(MAP_CR)
#endif	/*	ClausR	*/
	,"max"		,D_MAX		,func_arity(D_MAX)
#if	N_MAT
	,"mcut"		,MCUT		,func_arity(MCUT)
	,"mdim"		,MDIM		,func_arity(MDIM)
#endif
	,"min"		,D_MIN		,func_arity(D_MIN)
	,"minus"	,D_MINUS	,func_arity(D_MINUS)
#if	N_MAT
	,"mkstring"	,MKSTRING	,func_arity(MKSTRING)
#endif
	,"mod"		,D_MOD		,func_arity(D_MOD)
#if	N_MAT
	,"mreplace"	,MREPLACE	,func_arity(MREPLACE)
	,"mreplace_c"	,REPLACE_C	,func_arity(REPLACE_C)
	,"mreplace_r"	,REPLACE_R	,func_arity(REPLACE_R)
	,"mrotate"	,MROTATE	,func_arity(MROTATE)
	,"mselect"	,MSELECT	,func_arity(MSELECT)
#endif
	,"mult"		,D_MULT		,func_arity(D_MULT)
#if	N_MAT
	,"munite"	,MUNI		,func_arity(MUNI)
#endif
	,"ne"		,D_NEQ		,func_arity(D_NEQ)
	,"neg"		,M_NEG		,func_arity(M_NEG)
	,"not"		,M_NOT		,func_arity(M_NOT)
	,"or"		,D_OR		,func_arity(D_OR)
	,"plus"		,D_PLUS		,func_arity(D_PLUS)
	,"rank"		,RANK		,func_arity(RANK)
	,"replace"	,REPLACE	,func_arity(REPLACE)
	,"repstr"	,REPSTR		,func_arity(REPSTR)
#ifdef	MEINE_REDUMA
	,"rest"		,REST		,func_arity(REST)
#endif
	,"reverse"	,REVERSE	,func_arity(REVERSE)
#if	ClausR
	,"right"	,RIGHT_CR	,func_arity(RIGHT_CR)
#endif	/*	ClausR	*/
	,"rotate"	,ROTATE		,func_arity(ROTATE)
#ifdef	MEINE_REDUMA
	,"second"	,SECOND		,func_arity(SECOND)
#endif
#if	ClausR
	,"sel"		,SEL_CR		,func_arity(SEL_CR)
#endif	/*	ClausR	*/
	,"select"	,SELECT		,func_arity(SELECT)
	,"sin"		,M_SIN		,func_arity(M_SIN)
#if	N_MAT
	,"substr"	,SUBSTR		,func_arity(SUBSTR)
	,"substruct"	,SUBSTR		,func_arity(SUBSTR)
#endif
/*	,"solve"	,SOLVE		,func_arity(SOLVE)	*/
	,"tail"		,M_TAIL		,func_arity(M_TAIL)
        ,"tl"                     ,M_REST           ,func_arity(M_REST) 
/*	,"take"		,TAKE		,func_arity(TAKE)	*/
	,"tan"		,M_TAN		,func_arity(M_TAN)
#if	N_MAT
	,"to_field"	,FROMLIST	,func_arity(FROMLIST)
	,"to_list"	,TOLIST		,func_arity(TOLIST)
	,"to_mat"	,TO_MAT		,func_arity(TO_MAT)
	,"to_scal"	,TO_SCAL	,func_arity(TO_SCAL)
	,"to_tvect"	,TO_TVECT	,func_arity(TO_TVECT)
	,"to_vect"	,TO_VECT	,func_arity(TO_VECT)
#endif
	,"transform"	,TRANSFORM	,func_arity(TRANSFORM)
	,"transpose"	,TRANSPOSE	,func_arity(TRANSPOSE)
	,"trunc"	,M_TRUNCATE	,func_arity(M_TRUNCATE)
	,"type"		,TYPE		,func_arity(TYPE)
/*	,"ulist"	,UNI_TREE	,func_arity(UNI_TREE)	*/
/*	,"umatrix"	,UNI_MATRIX	,func_arity(UNI_MATRIX)	*/
#if	ClausR
	,"unify"	,UNIFY_CR	,func_arity(UNIFY_CR)
#endif	/*	ClausR	*/
	,"unite"	,UNI		,func_arity(UNI)
#if	ClausR
	,"update"	,UPDATE_CR	,func_arity(UPDATE_CR)
#endif	/*	ClausR	*/
/*	,"utree"	,UNI_TREE	,func_arity(UNI_TREE)	*/
/*	,"uvector"	,UNI_VECTOR	,func_arity(UNI_VECTOR)	*/
#if	N_MAT
	,"vc_div"	,VC_DIV		,func_arity(VC_DIV)
	,"vc_max"	,VC_MAX		,func_arity(VC_MAX)
	,"vc_min"	,VC_MIN		,func_arity(VC_MIN)
	,"vc_minus"	,VC_MINUS	,func_arity(VC_MINUS)
	,"vc_mult"	,VC_MULT	,func_arity(VC_MULT)
	,"vc_plus"	,VC_PLUS	,func_arity(VC_PLUS)
	,"vcut"		,VCUT		,func_arity(VCUT)
	,"vdim"		,VDIM		,func_arity(VDIM)
	,"vreplace"	,VREPLACE	,func_arity(VREPLACE)
	,"vrotate"	,VROTATE	,func_arity(VROTATE)
	,"vselect"	,VSELECT	,func_arity(VSELECT)
	,"vunite"	,VUNI		,func_arity(VUNI)
#endif
	,"xor"		,D_EXOR		,func_arity(D_EXOR)
/*	,"zsnorm"	,ZS_NORM	,func_arity(ZS_NORM)	*/
	};

#define KWTBLEN (sizeof(kwtb)/sizeof(struct kwrd))
#define RKWTBLEN (sizeof(rkwtb)/sizeof(struct kwrd))
#define NKWRDS (sizeof(kwrds)/sizeof(kwrds[0]))
#define NFUNCT (sizeof(functions)/sizeof(functions[0]))

int kwtblen  = KWTBLEN,
    rkwtblen = RKWTBLEN,
    nkwrds   = NKWRDS,
    nfunct   = NFUNCT;

/* end of    ciotab.c */
