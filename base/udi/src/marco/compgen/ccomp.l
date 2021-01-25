/**********************************************************************************
***********************************************************************************
**
**   File        : ccomp.l
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 04.01.1993
**   Contents    : Scanner description of Scanner/Parsergenerator
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

%{

#include "gen-parser.h"
#include "ccomp.tab.h"
#define Tst(arg)	((yytext[0]==arg)||(yytext[yyleng]==arg))
#define Return(a)	{return(a);}

int yyLineNumber=1;

static int TstComments(void);

%}

DOLLAR_ID "$"[0-9]+
NAME_ID [_a-zA-Z][a-zA-Z0-9_]*
TEXT_ID "\042"("\\\""|[^"^\n])*"\042"
DIGITS	[0-9]+
REPEATNUMBER	"\\"[0-9]+

%%
"%PROJECT"		Return(PROJECT)
"%SCANNER"		Return(SCANNER)
"%KEYWORDS"		Return(KEYWORDS)
"%PRIMFUNCTIONS" 	Return(PRIMFUNCTIONS)
"%TYPES"		Return(TYPES)
"%IDENTIFIERS"		Return(IDENTIFIERS)
"%COMMENTS"	  	Return(COMMENTS)
"%DELIMITERS"		Return(DELIMETERS)
"%PARSER"	 	Return(PARSER)
"%LEFT"			Return(LEFT)
"%RIGHT"		Return(RIGHT)
"%NONASSOC"		Return(NONASSOC)
"%SYNTAXRULES"		Return(SYNTAXRULES)
"%STRING"		Return(STRING)

"#REGEXPR"		Return(REGEXPR)
"#INT"			Return(INT_TYPE)
"#LONG"                 Return(LONG_TYPE)
"#FLOAT"		Return(FLOAT_TYPE)
"#DOUBLE"               Return(DOUBLE_TYPE)
"#IDENTIFIER"		Return(IDENTIFIER_TYPE)
"#CHAR"			Return(CHAR_TYPE)
"#BREAKCHARS"		Return(GETCHAR)

YES			Return(YES)
NO			Return(NO)

BUFFERSIZE		Return(BUFFERSIZE)
NAME			Return(NAME)
TEST                    Return(TEST)
MAKEFILE                Return(MAKEFILE)
NESTED			Return(NESTED)
NEWLINES		Return(NEWLINES)
EXTENDER		Return(EXTENDER)
TOKEN			Return(ONLYTOKEN)
EXPRESSIONSYMBOL	Return(EXPR_SY)
STARTSYMBOL		Return(START_SY)
ENDSYMBOL		Return(END_SY)

"$$"			Return(DOLLAR_SY)
"-{"			Return(BEGIN_C)
"}-"			Return(END_C)
"-["			Return(BEGIN_F)
"]-"			Return(END_F)

"<"			Return(LBRA_SY)
">"			Return(LKET_SY)
"["			Return(BRA_SY)
"]"			Return(KET_SY)
"{"			Return(SBRA_SY)
"}"			Return(SKET_SY)
"("			Return(RBRA_SY)
")"			Return(RKET_SY)
","			Return(COMMA_SY)
"_="			Return(LEQUAL_SY)
"="			Return(EQUAL_SY)
"=_"			Return(REQUAL_SY)
":"			Return(COLON_SY)
"|"			Return(ALT_SY)
";"			Return(SEMICOLON_SY)

"\\t"			Return(TAB_SY)
"\\n"			Return(NEWLINE_SY)
"\\<"			Return(SETTAB_SY)
"\\>"			Return(CLEARTAB_SY)
"\\_"			Return(BIND_SY)
"\\-"			Return(INDENT_SY)
"\\{"			Return(FRM_BRA_SY)
"\\}"			Return(FRM_KET_SY)

"$ELEM"			Return(FRM_ELEM)
"$DELI"			Return(FRM_DELI)

{NAME_ID}		{
			 memcpy(yylval.text,yytext,yyleng);
                         yylval.text[yyleng]='\0';
                         return(NAME_ID);
                        }
{TEXT_ID}		{
			 memcpy(yylval.text,yytext,yyleng);
                         yylval.text[yyleng]='\0';
                         return(TEXT_ID);
                        }
{REPEATNUMBER}		{strcpy(yylval.text,&yytext[1]);return(REPEATNUMBER);}
{DIGITS}		{yylval.number=atoi(yytext);return(NUMBER_TOK);}
{DOLLAR_ID}		{strcpy(yylval.text,&yytext[1]);return(DOLLAR_TOK);}


 /** Rule for comment recognation **/
"/*"		 	(void)TstComments();	/* Eat up comments */
"//"[^\n]*		/* Skip until end of line */

[ \t]+ 			/* Eatup whitespaces */
"\n"			yyLineNumber++; /* Counts number of lines */

[^ ^0-9^"^(^)^<^>^\t^\[^\]^\{^\}^,^=^|^;^:^\n^/]* {
	fprintf(stderr,"Illegal keyword <%s> in line %d\n",yytext,yyLineNumber);
                        }

%%

int VerbatimGet(char *buf,int len)
{ /* Liest beliebige Zeichen in buf ein. */
 register int c;
 register int pos=0;
 len--; /* Ende des Eingabepuffer korregieren */
 while ((c=input())&&(c!=EOF)) { /* Whitespaces ueberlesen */
  if ((c!=' ')&&(c!='\t')) {
   unput(c);
   break;
  }
 }
 while ((c=input()) && (c!=EOF)) { /* Zeichenkette lesen */
  if ((c=='\n')||(pos==len)) { /* Bis zum Zeilenende oder Pufferende lesen */
   unput(c);
   buf[pos]=0;
   break;
  } else buf[pos++]=c;
 }
 return (pos); /* Anzahl der gelesen Zeichen zurueckgeben */
}

int VerbatimGetLine(char *buf)
{ /* Liest beliebige Zeichen in buf ein. */
 register int c;
 register int pos=0;
 while ((c=input()) && (c!=EOF)) { /* Zeichenkette lesen */
  if (c=='\n') { 
   yyLineNumber++;
   buf[pos++]=c;
   break;
  } else if (c=='}') {
   c=input();
   if (c=='-') {
    buf[pos]=0;
    return(-pos);
   } else if (c==EOF) {
    unput('}');
    break;
   } else {
    buf[pos++]='}';
    unput(c);
   }
  } else buf[pos++]=c;
 }
 buf[pos]=0;
 return (pos); /* Anzahl der gelesen Zeichen zurueckgeben */
}

static int TstComments(void)
{ /* Tests nested comments in input file *****************************************/
 int c;                                                      /* actual character */
 static int ComLevel=0;                              /* Level of nested comments */
 static int ComStart=0;                               /* Store commentstart line */
 if (ComStart==0) ComStart=yyLineNumber;
 for (;;) {                                                 /* Scan input forver */
  while (((c=input())!=EOF)) {                                       /* Get text */
   if (c=='\x2a') {
    while ((c=input())=='\x2a');                                   /* Skip chars */
    if (c=='\x2f') {
     if (ComLevel>0) ComLevel--;
     else {
      ComStart=0;                                     /* Reset ComStart remember.*/
      return (1);
     }                                     /* Okay, comments match ! */
    }
   } else if (c=='\x2f') {                                   /* Nested comments */
   if ((c=input())=='\x2a') {
    ComLevel++;
    return (TstComments());                                  /* Recursive Call */
   }
  }
  if (c=='\n') {yyLineNumber++;continue;}
 }                                                        /* End of while loop */
 if (c==EOF) {                                                /* End of file ? */
  break;                                             /* Comments not matched ! */
 }
}                                                       /* End of forever loop */
if (c==EOF) {                                                 /* End of file ? */
 fprintf(stderr,"End of file in comment ! ");
}
fprintf(stderr,"Possible line of comment start : %d\n",ComStart);
ComStart=0;                                          /* Reset ComStart remember.*/
ComLevel=0;
return (0);                                      /* Matching comments not found */
} /*******************************************************************************/
