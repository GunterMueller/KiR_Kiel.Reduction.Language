/**********************************************************************************
***********************************************************************************
**
**   File        : gen-options.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 11.1.1993
**   Contents    : Rountionen zur Verwaltung von Optionen fuer
**                 Scanner/Parser - Generierung
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** BENOETIGTE INCLUDES **************************************************/
/*********************************************************************************/

#include <stdio.h>
#ifdef MEMCHECK
#include "malloc.h"
#else
#include <stdlib.h>
#endif
#include <ctype.h>
#include <string.h>

#include "gen-options.h"
#include "gen-scanner.h"
#include "gen-parser.h"
#include "dbug.h"
#include "ccomp.tab.h"

/*********************************************************************************/
/********** TYPENDEFINITIONEN ****************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAMM DEFINES *****************************************************/
/*********************************************************************************/

#define DEF_BUFFER_SIZE 16834

#define ERR_SC_COM_NEWLINES_ADEF \
"Rule for newlines in Comments already definied !\n"
#define ERR_SC_COM_NESTED_ADEF \
"Rule for nested Comments already definied !\n"
#define ERR_SC_COMSTART_ADEF \
"Commentstartsymbol already definied with : %s\n"
#define ERR_SC_COMEND_ADEF \
"Commentendsymbol already definied with : %s\n"
#define ERR_SC_CASESEN_ADEF \
"Casesensitivity for %s already definied !\n"
#define ERR_SC_SYMBOL_EXTENDER_ADEF \
"Nameextender for %s already definied !\n"

/*********************************************************************************/
/********** LOKALE FUNKTIONS PROTOTYPES ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** MODUL LOKALE VARIABLEN ***********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** MODUL GLOBALE VARIABLEN **********************************************/
/*********************************************************************************/

SYOptionsStruct Scanner_OPT,                  /* Optionen fuer einzelne Segmente */
                Keyword_OPT,
                Primfunction_OPT,
                Type_OPT,
                Delimeter_OPT,
                Identifier_OPT;

CommentStruct   Comment_OPT;           /* Optionen fuer Kommenterkennungsscanner */

ParserOptionsStruct Parser_OPT;
PrjOptionsStruct    Project_OPT;

/*********************************************************************************/
/********** LOKALE FUNKTIONEN ****************************************************/
/*********************************************************************************/

static void InitSYOptionsStruct(SYOptionsStruct *ptr,SYtype type)
{ /* Initialisiert Struktur auf UNDEFINIED ***************************************/
 ptr->type=type;                                            /* Optiontype setzen */
 ptr->extender=NULL;                                  /* kein Extender definiert */
 ptr->ctype=NO_CASE;                          /* kein Casesensitivwert definiert */
} /*******************************************************************************/

static void WriteTestMakefile(void)
{ /* Create testmakefile for scanner / parser ************************************/
 FILE *fh;
 char buffer[256];
 DBUG_ENTER("WriteTestMakefile");
 sprintf(buffer,"%s%s",Project_OPT.name,MAKEFILE_EXTENDER);
 if ((fh=fopen(buffer,"w"))==NULL) {
  fprintf(stderr,"Can't open file %s for test makefile output !\n",buffer);
 } else {                                            /* Create new test makefile */
  sprintf(buffer,"Generated makefile for %s",Project_OPT.name);
  fprintf(fh,"#\n# %s\n",buffer);
  fprintf(fh,"#\n# DO NOT TOUCH\n#\n\n");

  sprintf(buffer,"%s.tab.o lex.yy.o %s.format.o",Project_OPT.name,Project_OPT.name);
  fprintf(fh,"LIBOBJS\t=%s\n",buffer);
  sprintf(buffer,"lex.yy.c %s.tab.c %s.tab.h %s%s %s.format.h %s.format.c %s.y %s.flex",
   Project_OPT.name,Project_OPT.name,Project_OPT.name,MAKEFILE_EXTENDER,
   Project_OPT.name,Project_OPT.name,Project_OPT.name,Project_OPT.name);
  fprintf(fh,"TMPFILES=%s\n\n",buffer);

  fprintf(fh,"# Goals\n\n");
  fprintf(fh,"all : $(LANGLIBNAME)\n\n");
  fprintf(fh,"clean ::\n\t$(RM) $(RMOPTS) $(LIBOBJS) $(TMPFILES) $(LANGLIBNAME)\n");
  fprintf(fh,"\nclobber : clean\n\n");
  fprintf(fh,"install : all\n\n");
  fprintf(fh,"depend : all\n\n");

  fprintf(fh,"# Subgoals\n\n");

  fprintf(fh,"$(LANGLIBNAME) : $(LIBOBJS)\n");
  fprintf(fh,"\t$(RM) $(RMOPTS) $(LANGLIBNAME);\n");
  fprintf(fh,"\t$(LD) $(LDOPTS) $(LANGLIBNAME) $(LIBOBJS);\n\n");

  fprintf(fh,".c.a : $(LIBOBJS)\n");
  fprintf(fh,"\t$(CC) $(CFLAGS) -o $%% $<\n\n");

  fprintf(fh,"%s.tab.c : %s.y\n",Project_OPT.name,Project_OPT.name);
  fprintf(fh,"\t$(BISON) $(BISONOPTS) %s.y\n\n",Project_OPT.name);
   
  fprintf(fh,"lex.yy.c : %s.flex\n",Project_OPT.name);
  fprintf(fh,"\t$(FLEX) $(FLEXOPTS) %s.flex\n",Project_OPT.name);
  
  fclose(fh);
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** EXPORT FUNKTIONEN ****************************************************/
/*********************************************************************************/

void InitGeneratorOptions(char *filename,int htablesize,int mode)
{ /* Initialisiert Optionen fuer den Generator ***********************************/
 DBUG_ENTER("InitGeneratorOptions");
 InitSYOptionsStruct(&Keyword_OPT,KW_KEYWORDS_SY);            /* Optionen setzen */
 InitSYOptionsStruct(&Type_OPT,KW_TYPES_SY);
 InitSYOptionsStruct(&Delimeter_OPT,KW_DELIMETERS_SY);
 InitSYOptionsStruct(&Identifier_OPT,KW_IDENTIFIERS_SY);
 InitSYOptionsStruct(&Primfunction_OPT,KW_PRIMFUNCTIONS_SY);
 Comment_OPT.newlines=UNDEF;                    /* Kommentar Opts initialisieren */
 Comment_OPT.nested=UNDEF;
 Comment_OPT.start=NULL;
 Comment_OPT.end=NULL;
 /* Set options for scanner generator module *************************************/
 Scanner_OPT.extender=NULL;
 /* Set  options for parser generator module *************************************/
 InitParserGenerator();                                          /* Setup module */
 Parser_OPT.extender=NULL;                                        /* No extender */
 Parser_OPT.expr=NULL;                          /* Expressionsymbol not definied */
 Parser_OPT.token=1;                   /* Only real token IDs are allowed to use */
 Parser_OPT.left_prec=NULL;
 Parser_OPT.right_prec=NULL;
 Parser_OPT.nonassoc_prec=NULL;
 /* Optionen des gesamten Projektes setzen ***************************************/
 Project_OPT.filename=(char *)my_strdup(filename);
 Project_OPT.mode=mode;
 Project_OPT.name=NULL;                                  /* Keine Name definiert */
 Project_OPT.bit8=UNDEF;                                /* 8 Bit Code generieren */
 Project_OPT.test=FALSE;
 Project_OPT.sctest=UNDEF;                          /* Testprogramm fuer Scanner */
 Project_OPT.prtest=UNDEF;                           /* Testprogramm fuer Parser */
 Project_OPT.makefile=FALSE;                        /* Erzeugung eines Makefiles */
 Project_OPT.sc_buf_size=DEF_BUFFER_SIZE;        /* Size of scanner input buffer */
 InitScannerGenerator(htablesize);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void ReleaseGeneratorOptions(void)
{ /* Gibt dynamisch allozierten Speicher der Optionen frei ***********************/
 DBUG_ENTER("ReleaseGeneratorOptions");
  if (Project_OPT.makefile) WriteTestMakefile();
  CleanUpParserGenerator();                 /* Frees all memory in pargen module */
  CleanUpScannerGenerator();                /* Free all dynamic allocated memory */
  if (Project_OPT.name)          free(Project_OPT.name);
  if (Comment_OPT.start)         free(Comment_OPT.start);
  if (Comment_OPT.end)           free(Comment_OPT.end);
  if (Scanner_OPT.extender)      free(Scanner_OPT.extender);
  if (Parser_OPT.extender)       free(Parser_OPT.extender);
  if (Parser_OPT.expr)           free(Parser_OPT.expr);
  if (Parser_OPT.left_prec)      FreeTokenList(Parser_OPT.left_prec);
  if (Parser_OPT.right_prec)     FreeTokenList(Parser_OPT.right_prec);
  if (Parser_OPT.nonassoc_prec)  FreeTokenList(Parser_OPT.nonassoc_prec);
  if (Keyword_OPT.extender)      free(Keyword_OPT.extender);
  if (Primfunction_OPT.extender) free(Primfunction_OPT.extender);
  if (Type_OPT.extender)         free(Type_OPT.extender);
  if (Delimeter_OPT.extender)    free(Delimeter_OPT.extender);
  if (Identifier_OPT.extender)   free(Identifier_OPT.extender);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetCommentOption(int token,char yn)
{ /* Prueft und setzt Kommentaroptionen ******************************************/
 DBUG_ENTER("SetCommentOption");
 if (token==NEWLINES) {                             /* Art der Option NewLines ? */
  if (Comment_OPT.newlines==UNDEF)                     /* Noch nicht definiert ? */
   Comment_OPT.newlines=yn ? TRUE : FALSE;                      /* Option setzen */
  else {                                                    /* War schon gesetzt */
   fprintf(stderr,ERR_SC_COM_NEWLINES_ADEF);
  }
 } else if (token==NESTED) {                       /* Geschachtelte Kommentare ? */
  if (Comment_OPT.nested==UNDEF)                         /* Noch nicht gesetzt ? */
   Comment_OPT.nested=yn ? TRUE : FALSE;
  else {                                                  /* War schon definiert */
   fprintf(stderr,ERR_SC_COM_NESTED_ADEF);
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetCommentSymbol(int token,char *symbol)
{ /* Prueft und setzt Kommentarbegrenzer *****************************************/
 DBUG_ENTER("SetCommentSymbol");
 if (token==START_SY) {                         /* Symbol fuer Kommentarbeginn ? */
  if (Comment_OPT.start==NULL) {                           /* Kein Startsymbol ? */
   Comment_OPT.start=my_strdup(symbol);
  } else {                                                    /* Schon definiert */
   fprintf(stderr,ERR_SC_COMSTART_ADEF,Comment_OPT.start);
  }
 } else if (token==END_SY) {                      /* Symbol fuer Kommentarende ? */
  if (Comment_OPT.end==NULL) {                              /* Kein Endesymbol ? */
   Comment_OPT.end=my_strdup(symbol);
  } else {                                                    /* Schon definiert */
   fprintf(stderr,ERR_SC_COMEND_ADEF,Comment_OPT.end);
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetCaseOption(SYOptionsStruct *opt,CASEtype ctype)
{ /* Setzt in der Optionstruktur die Casekomponente ******************************/
 DBUG_ENTER("SetCaseOption");
 if (ctype>0) {                                           /* Darstellung gegeben */
  if (opt->ctype==NO_CASE)                                    /* Nicht definiert */
   opt->ctype=ctype;                                      /* Wert ueberschreiben */
  else {                                                 /* Wert schon definiert */
   fprintf(stderr,ERR_SC_CASESEN_ADEF,SYtype2Char(opt->type));
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetExtOption(SYOptionsStruct *opt,char *ext)
{ /* Setzt in der Optionstruktur die Extenderkomponente **************************/
 DBUG_ENTER("SetExtOption");
 if (ext) {                                             /* Darstellung gegeben ? */
  if (opt->extender==NULL)                    /* Extender noch nicht definiert ? */
   opt->extender=my_strdup(ext);                             /* Wert ueberschreiben */
  else {                                                 /* Wert schon definiert */
   fprintf(stderr,ERR_SC_SYMBOL_EXTENDER_ADEF,SYtype2Char(opt->type));
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

char *SYtype2Char(SYtype type)
{ /* Gibt zum Symboltypen die Textdarstellung zurueck ****************************/
 switch (type) {
   case KW_NONE_SY          : return("NONE");
   case KW_KEYWORDS_SY      : return("KEYWORD");
   case KW_PRIMFUNCTIONS_SY : return("PRIMFUNCTION");
   case KW_DELIMETERS_SY    : return("DELIMETER");
   case KW_IDENTIFIERS_SY   : return("IDENTIFIER");
   case KW_ID_STD_SY        : return("STANDART IDENTIFIER RULE");
   case KW_ID_REGEXPR_SY    : return("REGULAR EXPRESSION");
   case KW_TYPES_SY         : return("TYPE");
   default                  : return("UNKOWN");
 }
} /*******************************************************************************/

TokenList *AddTokenList(TokenList *list,int type,char *token)
{ /* Add one token top of the list ***********************************************/
 TokenList *result=NULL;
 DBUG_ENTER("AddTokenList");
 result=(TokenList*)malloc(sizeof(TokenList));
 DBUG_ASSERT(result);
 result->type=type;
 result->next=list;
 result->token=token;
 DBUG_RETURN(result);
} /*******************************************************************************/

void FreeTokenList(TokenList *list)
{ /* Frees complete tokenlist ****************************************************/
 TokenList *hlp;
 DBUG_ENTER("FreeTokenList");
 while (list) {                                         /* End of list reached ? */
  hlp=list;
  if (list->token) free(list->token);                       /* Free symbol token */
  list=list->next;
  free(hlp);                                                  /* Free list entry */
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

TokenList *ConcTokenList(TokenList *l1,TokenList *l2)
{ /* Concatenate two TokeLists ***************************************************/
 TokenList *new,*hlp=l1;
 DBUG_ENTER("ConcTokenList");
 if (l1==NULL) DBUG_RETURN(l2);                              /* List one empty ? */
 if (l2==NULL) DBUG_RETURN(l1);                              /* List two empty ? */
 while (l1->next) {                                              /* Last entry ? */
  l1=l1->next;
 }
 new=(TokenList*)malloc(sizeof(TokenList));
 DBUG_ASSERT(new);
 new->token=NULL;                                     /* Indicates a new sublist */
 new->next=l2;
 l1->next=new;
 DBUG_RETURN(hlp);
} /*******************************************************************************/

char *my_strdup(char *name)
{ /* Replacement for the standard strdup function **/
 char *result=NULL;
 if (name) {
  int len=strlen(name)+1;
  result=(char *)malloc(len);
  memcpy(result,name,len);
 }
 return (result);
} /********/
