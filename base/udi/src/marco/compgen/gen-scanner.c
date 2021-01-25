/**********************************************************************************
***********************************************************************************
**
**   File        : gen-scanner.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 11.1.1993
**   Contents    : Rountionen zur Verwaltung von Symboltabellen und
**                 Scannergenerierung
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** PROGRAMM INTERNE INFOS ***********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** BENOETIGTE INCLUDES **************************************************/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "hash-fcts.h"

#include "gen-scanner.h"
#include "gen-parser.h"
#include "dbug.h"
#include "ccomp.tab.h"

#include "scanner-txt.h"

/* Spezifikationparser ***********************************************************/
extern int yyparse();

/*********************************************************************************/
/********** TYPENDEFINITIONEN ****************************************************/
/*********************************************************************************/

typedef enum { /* Zustand des Scannergenerators **********************************/
 SC_EMPTY,                                       /* Anfangszustand, nichts getan */
 SC_OK,                               /* Erkennung/Generierung laueft fehlerfrei */
 SC_TABLEFULL   /* Symboltabelle ist voll, laueft aber weiter und zaehlt Symbole */
} ScannerState; /*****************************************************************/

typedef struct SYTag { /* Symbol Datenstruktur ***********************************/
 SYtype type;                                                /* Type des Symbols */
 int    idtype;                                           /* Type des Identifier */
 int    blanks;            /* Skip leading (-1) or following blanks (1) 0=normal */
 char   *name;                                           /* Name des Identifiers */
 char   *text;                                    /* Darstellung des Identifiers */
} SYStruct; /*********************************************************************/

typedef enum { /* Typen der Standartregeln in der Union Struktur *****************/
 TYP_TEXT,                /* Fuer kopieren des Token in die Textpufferkomponente */
 TYP_INT,                                /* Fuer kopieren in die Integervariable */
 TYP_LONG,                                      /* kopieren in die Long Variable */
 TYP_FLOAT,                                                /* kopieren als float */
 TYP_DOUBLE,                                        /* kopieren als double Value */
 TYP_CHAR                                                     /* Character value */
} StdUnionType; /*****************************************************************/

typedef struct StdRuleTag { /* Eintraege der Standart Identifierregeln ***********/
 char *text;         /* Textuelle Darstellung der Regeln als Regulaerer Ausdruck */
 StdUnionType type;                          /* Type der dazugehoerigen Variable */
} StdRuleStruct; /****************************************************************/

/*********************************************************************************/
/********** PROGRAMM DEFINES *****************************************************/
/*********************************************************************************/

#define MAXGETCHAR_SIZE        32

#define NR_SYMBOLS_ADD         10
#define TAB_SIZE               8

#define DEF_KEYWORD_CASE       IGNORE_CASE
#define DEF_KEYWORD_EXT        "_KWS"

#define SCANNER_EXTENDER       ".flex"

/* Definitionen zur Scannercodeerzeugung *****************************************/
#define SC_ID_PREFIX "id"

#define FLEX_REG_SPECIALS ".[]\\-^"

/*********************************************************************************/
/********** PROGRAMM MACROS ******************************************************/
/*********************************************************************************/

/* FrontEnd fuer die Conv2ID Funktion (kleiner Hack) *****************************/
#define SetUsedTypeFlag(type) (void)Conv2ID(type)

/* Test ob das angegebene Symbol kein Identifier ist *****************************/
#define NormalSymbol(ptr) (ptr->type>KW_NORMAL_SY)

/* Ueberprueft, ob das Symbol ein Identifikator ist ******************************/
#define IsIdentifierSymbol(ptr) ((ptr->type>KW_NONE_SY)&&(ptr->type<KW_NORMAL_SY))

/* Test ob 'suche' als Name in der Hashtabelle auftaucht *************************/
#define IsNameInHTable(suche) \
(BSEARCH.name=suche,(bsearch((char*)&BSEARCHHLP,(char *)Symbols,\
Nr_Symbols,sizeof(SYStruct*),SYStructSort)))

/*********************************************************************************/
/********** LOKALE FUNKTIONS PROTOTYPES ******************************************/
/*********************************************************************************/

static int SYStructSort();               /* Sortiert Symbol Eintraege nach Namen */

/*********************************************************************************/
/********** MODUL LOKALE VARIABLEN ***********************************************/
/*********************************************************************************/

static SYStruct **Symbols;                                      /* Symboltabelle */
static SYStruct BSEARCH;                       /* Hilfssymbol fuer binaere Suche */
static char *BSEARCHHLP=(char *)&BSEARCH;        /* Hack fuer den bsearch Aufruf */

static char delimeter_str[256];         /* Set of characters to speperatre token */
static int  delimeter_len=0;

static SYStruct *GetCharArray[MAXGETCHAR_SIZE];
static int GetCharSize=0;

static int Nr_Symbols;                      /* Anzahl der Symbole in der Tabelle */
static int SymbolTableSize;                         /* Groesse der Symboltabelle */
static DH_ENTRY item;/* Hilfselement zum Einfuegen von Elementen in die HTabelle */
static int HashTableSize;                             /* Groesse der Hashtabelle */
static ScannerState ScannerSTATE;                      /* Zustand des Generators */
static int intV,longV,floatV,doubleV,identifierV,charV;/* Flagge ob Type benutzt */

/* Anzahl der unterstuetzten Standart Indentifier ********************************/
#define NR_STD_INT     1
#define NR_STD_LONG    0
#define NR_STD_FLOAT   0
#define NR_STD_DOUBLE  0
#define NR_STD_ID      0
#define NR_STD_CHAR    0

static StdRuleStruct IntRuleList[]={ /* Liste der Regeln fuer Integer ************/
 {"(\x22+\x22|\x22-\x22)[0-9]+",TYP_INT},
 {"[0-9]+",TYP_INT}
}; /******************************************************************************/

static StdRuleStruct LongRuleList[]={ /* Liste der Regeln fuer Long **************/
 {"[0-9]+",TYP_LONG}
}; /******************************************************************************/

static StdRuleStruct FloatRuleList[]={ /* Liste der Regeln fuer Floats ***********/
 {"[0-9]+\x22.\x22[0-9]+",TYP_FLOAT}
}; /******************************************************************************/

static StdRuleStruct DoubleRuleList[]={ /* Liste der Regeln fuer Double **********/
 {"[0-9]+\x22.\x22[0-9]+",TYP_DOUBLE}
}; /******************************************************************************/

static StdRuleStruct IdRuleList[]={ /* Liste der Regeln fuer Identifier **********/
 {"[A-Z]+",TYP_TEXT}
}; /******************************************************************************/

static StdRuleStruct CharRuleList[]={    /* List of standart rules for charaters */
 {"[^ ^\\t^\\n]",TYP_CHAR}
}; /******************************************************************************/

static StdRuleStruct *StdRuleList[]={ /* Liste der definierten Standartregeln ****/
 IntRuleList,
 LongRuleList,
 FloatRuleList,
 DoubleRuleList,
 IdRuleList,
 CharRuleList
}; /******************************************************************************/

/*********************************************************************************/
/********** MODUL GLOBALE VARIABLEN **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOKALE FUNKTIONEN ****************************************************/
/*********************************************************************************/

static void FreeSYStruct(SYStruct *ptr)
{ /* Gibt Knoten samt Unterstruktur frei *****************************************/
 if (ptr) {
  if (ptr->name) free(ptr->name);
  if (ptr->text) free(ptr->text);
 }
} /*******************************************************************************/

static char *Conv2ID(int type)
{ /* Gibt zu einem Typeoffset eines Identifiers die lexikalische Darstellung *****/
 if (type>0) {                                               /* Type angegeben ? */
  if (type/OFFSET_CHAR)            {charV=1;return(TEXT_ID_CHAR);}
  else if (type/OFFSET_IDENTIFIER) {identifierV=1;return (TEXT_ID_IDENTIFIER);}
  else if (type/OFFSET_DOUBLE)     {doubleV=1;return (TEXT_ID_DOUBLE);}
  else if (type/OFFSET_FLOAT)      {floatV=1;return (TEXT_ID_FLOAT);}
  else if (type/OFFSET_LONG)       {longV=1;return (TEXT_ID_LONG);}
  else if (type/OFFSET_INT)        {intV=1;return (TEXT_ID_INT);}
 }
 return ("");                    /* Leere Zeichenkette, bedeutet kein Identifier */
} /*******************************************************************************/

static StdRuleStruct *GetStdRulePtr(SYStruct *sy)
{ /* Holt aus der Liste der Standartregeln die entsprechende Struktur ************/
 int nr=0;                                           /* Nummer der Standartregel */
 int typ=0;                                            /* Type der Standartregel */
 int value=sy->idtype;                                         /* Wert berechnen */
 if (value>0) {                                          /* Ist Wert vorhanden ? */
  if (value/OFFSET_CHAR) {
   nr=value%OFFSET_CHAR;
   typ=value/OFFSET_INT;
  } else if (value/OFFSET_IDENTIFIER) {
   nr=value%OFFSET_IDENTIFIER;
   typ=value/OFFSET_INT;
  } else if (value/OFFSET_DOUBLE) {
   nr=value%OFFSET_DOUBLE;
   typ=value/OFFSET_INT;
  } else if (value/OFFSET_FLOAT) {
   nr=value%OFFSET_FLOAT;
   typ=value/OFFSET_INT;
  } else if (value/OFFSET_LONG) {
   nr=value%OFFSET_LONG;
   typ=value/OFFSET_INT;
  } else if (value/OFFSET_INT) {
   nr=value%OFFSET_INT;
   typ=value/OFFSET_INT;
  }
  if (typ>0)                                           /* Passende ID gefunden ? */
   return(&StdRuleList[typ-1][nr]);                      /* Pointer zurueckgeben */
 }
 return (NULL);                           /* Kein zugehoeriger Eintrag vorhanden */
} /*******************************************************************************/

static char *TextStdRule(SYStruct *sy)
{ /* Gibt den Text fuer die jeweilige Standartregel *****************************/
 StdRuleStruct *ptr=GetStdRulePtr(sy);                          /* Eintrag holen */
 if (ptr) return (ptr->text);                             /* Inhalt zurueckgeben */
 else return ("");                                        /* kein Text vorhanden */
} /*******************************************************************************/

static void ResizeSymbolTable(void)
{ /* Groesse der SymbolTabelle veraendern ****************************************/
 DBUG_ENTER("ResizeSymbolTable");
 SymbolTableSize+=NR_SYMBOLS_ADD;                  /* Symboltabelle vergroessern */
 Symbols=(SYStruct **)realloc(Symbols,SymbolTableSize*sizeof(SYStruct *)); 
 DBUG_ASSERT(Symbols);                         /* Speicheranforderung erfuellt ? */
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void ToUpperStr(char *str)
{ /* Konvertiert einen AscII String in Grossbuchstaben ***************************/
 while (*str) {
  *str=toupper(*str);                                                  /* c -> C */
  str++;
 }
} /*******************************************************************************/

static char *AppendExtender(char *name,char *extender)
{ /* Haengt an den Namen einen Suffix an *****************************************/
 if (extender) {                                           /* Extender gegeben ? */
  name=(char *)realloc(name,strlen(name)+strlen(extender)+2);
  sprintf(name,"%s_%s",name,extender); /* Suffix anhaengen */ 
 }
 return name;
} /*******************************************************************************/

static char *Conv2SyName(char *text,char *extender)
{ /* Erzeugt aus dem Namen den Symbolnamen ***************************************/
 char *temp=my_strdup(text);                                       /* Text kopieren */
 ToUpperStr(temp);                            /* In Grossbuchstaben konvertieren */
 return (AppendExtender(temp,extender));                         /* Zuruechgeben */
} /*******************************************************************************/

static int MkSymbolNames(void)
{ /* Symbole benennen ************************************************************/
 int i;
 int State=1;                             /* Zustand der Benennung/Ueberpruefung */
 char *text,*name;                                               /* Abkuerzungen */
 SYOptionsStruct *opts;                              /* aktuelle Optionsstruktur */
 int type;
 DBUG_ENTER("MkSymbolNames");
 for (i=0;i<Nr_Symbols;i++) {                             /* Alle Symbole testen */
  text=Symbols[i]->text;
  name=Symbols[i]->name;
  type=Symbols[i]->type;
  switch (type) {                            /* Richtige Optionsstruktur waehlen */
   case KW_KEYWORDS_SY :
    opts=&Keyword_OPT;
    break;
   case KW_PRIMFUNCTIONS_SY : 
    opts=&Primfunction_OPT;
    break;
   case KW_DELIMETERS_SY : 
    opts=&Delimeter_OPT;
    break;
   case KW_TYPES_SY : 
    opts=&Type_OPT;
    break;
   case KW_ID_STD_SY :
    Symbols[i]->text=AppendExtender(text,Identifier_OPT.extender);
    continue;
   case KW_ID_REGEXPR_SY :
    Symbols[i]->text=AppendExtender(text,Identifier_OPT.extender);
    continue;
   default :                                      /* Darf/Sollte nicht passieren */
    fprintf(stderr,ERR_SC_UNKNOWN_SYMBOL);
    return (0);
  } 
  if (name==NULL) {                              /* Symbolname nicht definiert ? */
   Symbols[i]->name=Conv2SyName(text,opts->extender);        /* Namen generieren */
  } else {                                                       /* Name gegeben */
   Symbols[i]->name=AppendExtender(name,opts->extender);    /* Extender anhaengen*/
  } 
 }
 DBUG_RETURN(State);
} /*******************************************************************************/

static void WriteCommentSection(FILE *fh,char *text)
{ /* Schreibt Kommentare zu den einzelnen Sektionen ******************************/
 fprintf(fh,"\n %s\n",TEXT_COMSTART);                      /* Kommentar einfuegen */
 fprintf(fh," %s %s\n",TEXT_COMPRE,text);               /* Variablenteil ausgeben */
 fprintf(fh," %s\n\n",TEXT_COMEND);
} /*******************************************************************************/

static void WriteCommentHeader(FILE *fh)
{ /* Schreibt Kommentare und benutzte Optionen in den Lex Kopf *******************/
 DBUG_ENTER("WriteCommentHeader");
 fprintf(fh,"%s\n",TEXT_COMSTART);
 fprintf(fh,"%s\n",TEXT_COMFILL);
 fprintf(fh,"%s\n",TEXT_COMPRE);
 fprintf(fh,"%s %s\n",TEXT_COMPRE,TEXT_SCANNERGENERATORVERS);
 fprintf(fh,"%s\n",TEXT_COMPRE);
 fprintf(fh,"%s\n",TEXT_COMFILL);
 fprintf(fh,"%s\n",TEXT_COMEND);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteCHeader(FILE *fh)
{ /* Schreibt den C Deklarationpart in den Lex Kopf ******************************/
 int i,j;
 DBUG_ENTER("WriteCHeader");

 WriteCommentSection(fh,TEXT_SC_CDECLPART);           /* Erlauerterung einfuegen */

 fprintf(fh,"%%{\n");                         /* Anfang des C-Teils des Scanners */
 fprintf(fh,"%s",TEXT_SC_INCLUDES);               /* Standart includes einfuegen */
 fprintf(fh,"#include %cmarco-create.h%c\n",34,34);

 if (Project_OPT.sctest!=TRUE) { /* No testprogram, insert parser include file */
  fprintf(fh,"\n#include %c%s%s%c\n",34,Project_OPT.name,TOKEN_HEADER_EXT,34);
 }

 fprintf(fh,"\n#undef YY_DECL\n");
 fprintf(fh,"#define YY_DECL int yylex(lvalp,llocp) YYSTYPE *lvalp;void *llocp;\n");
 fprintf(fh,"\n#undef YY_BUF_SIZE\n");
 fprintf(fh,"#define YY_BUF_SIZE %d\n",Project_OPT.sc_buf_size);
 fprintf(fh,"\n/** Function prototypes **/\n");
 fprintf(fh,"void yyerror2(char *error,char *var);\n");
 if (intV)
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_INT);fprintf(fh,";\n");}
 if (longV)
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_LONG);fprintf(fh,";\n");}
 if (floatV)
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_FLOAT);fprintf(fh,";\n");}
 if (doubleV)
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_DOUBLE);fprintf(fh,";\n");}
 if (identifierV)  
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_IDENTIFIER);fprintf(fh,";\n");}
 if (charV)  
  {fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_CHAR);fprintf(fh,";\n");}
 if ((Comment_OPT.start!=NULL) && (Comment_OPT.end!=NULL)) /* Prototype ComFct ? */
  fprintf(fh,"%s;\n",TEXT_SC_TSTCOMMENTSFCT_PROTO);

 fprintf(fh,"\nint LineNumber=1;\t/* Counts number of lines in inputfile */\n");
 fprintf(fh,"\nchar *YY_IN_FILENAME;\t/* Name of inputfile */\n");
 fprintf(fh,"\nFILE *YY_ERRFILE=stderr;\t/* Errorfilehandle */\n");
 fprintf(fh,"\nint YY_NR_ERRORS=0;\t/* Number of errors while parsing */\n");

 if (Project_OPT.sctest==TRUE) {            /* Soll Testscanner erzeugt werden ? */
  fprintf(fh,TEXT_SC_TEST_YYLVAL_UNION,               /* Wert des Tokens bei IDs */
   TEXT_ID_IDENTIFIER,TEXT_ID_FLOAT,TEXT_ID_DOUBLE,TEXT_ID_LONG,TEXT_ID_INT,
   TEXT_ID_CHAR);
  fprintf(fh,"char *TokenList[]={\t/* list of available token IDs */\n");
  fprintf(fh," %cEOF%c,",'"','"');               /* Eof File entspricht der Null */
  for (i=0;i<Nr_Symbols-1;i++) {         /* Arry mit TokenID Stringdarstellungen */
   if (NormalSymbol(Symbols[i]))                      /* Normaler Identifierer ? */
    fprintf(fh," %c%s%c,",'"',Symbols[i]->name,'"');
   else                                                            /* Identifier */
    fprintf(fh," %c%s,%c,",'"',Symbols[i]->text,'"');
   if ((i%3)==0) fprintf(fh,"\n");             /* Alle 3 Tokenstring ein NewLine */
  }
  if (NormalSymbol(Symbols[i]))                       /* Normaler Identifierer ? */
   fprintf(fh," %c%s%c\n};\n\n",'"',Symbols[i]->name,'"');
  else                                                             /* Identifier */
   fprintf(fh," %c%s,%c\n};\n\n",'"',SYtype2Char(Symbols[i]->type),'"');
  /* Definies fuer die benutzten Token erzeugen **********************************/
  for (i=0;i<Nr_Symbols;i++) {                      /* Symboltabelle durchlaufen */
   if (NormalSymbol(Symbols[i]))                             /* Standartsymbol ? */
    fprintf(fh,"#define %s %d\n",Symbols[i]->name,i+1);    /* Eindeutige Nummern */
   else                                                       /* Identifierregel */
    fprintf(fh,"#define %s %d\n",Symbols[i]->text,i+1);
  }
  i++;
  for (j=0;j<GetCharSize;j++) 
   fprintf(fh,"#define %s %d\n",GetCharArray[j]->name,i+j);
  fprintf(fh,"\n");
 }

 if (GetCharSize>0) {
  fprintf(fh,"\nstatic int _GC_; /* Input character in special scan mode */\n");
  fprintf(fh,"int _GCM_=0;\n\n");
  fprintf(fh,"struct _GCINFO_ {char *stopchars;int stopid;int level;};\n");
  fprintf(fh,"struct _GCINFO_ _GCI_[]={\n");
  for (i=0;i<GetCharSize-1;i++) {
   fprintf(fh," {%c%s%c,%s,0},\n",34,GetCharArray[i]->text,34
    ,GetCharArray[i]->name);
  }
  fprintf(fh," {%c%s%c,%s,0}\n",34,GetCharArray[GetCharSize-1]->text,34
    ,GetCharArray[GetCharSize-1]->name);
  fprintf(fh,"};\n\n");
 }

 fprintf(fh,"int NR_of_EXPRs=-1;\n");

 fprintf(fh,"%%}\n");
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteIDRules(FILE *fh)
{ /* Schreibt die Regeln fuer Identifier und Abkuerzungen in den Lex Kopf ********/
 SYStruct *sy;                     /* Hilfspointer zur Abkuerzenden Schreibweise */
 int i;
 DBUG_ENTER("WriteIDRules");
 WriteCommentSection(fh,TEXT_SC_IDRULES);              /* Erlaeuterung einfuegen */
 for (i=0;i<Nr_Symbols;i++) {                       /* Symboltabelle durchlaufen */
  sy=Symbols[i];                                                   /* Abkuerzung */
  if (IsIdentifierSymbol(sy)) {                    /* Ist es ein Identifikator ? */
   if (sy->type==KW_ID_REGEXPR_SY) {      /* Definition fuer Regulaeren Ausdruck */
    fprintf(fh,"%s%s\t%s\n",SC_ID_PREFIX,sy->text,sy->name);
   } else if (sy->type==KW_ID_STD_SY) {             /* Standartregel einfuegen ? */
    fprintf(fh,"%s%s\t%s\n",SC_ID_PREFIX,sy->text,TextStdRule(sy));
   }
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteScannerHeader(FILE *fh)
{ /* Erzeugt den Vorspann des Scanners *******************************************/
 DBUG_ENTER("WriteScannerHeader");
 WriteCommentHeader(fh);                      /* Einleitung, Kommentare, etc ... */
 WriteCHeader(fh);                             /* C Deklarationpart des Scanners */
 WriteIDRules(fh);                                          /* Identifier Regeln */
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteScannerBodyStd(FILE *fh)
{ /* Fuegt an Ende die Standart Regeln zum Scannen ein ***************************/
 int i;
 DBUG_ENTER("WriteScannerBodyStd");

 /* Zaehlen der Zeilennummern ****************************************************/
 fprintf(fh,"\n\\n\t\t LineNumber++;\t/* Increase line number */\n");

 /* Regel zum Ueberlesen von Fuellzeichen Tab und Space **************************/
 fprintf(fh,"[ \\t]+\t\t /* Eat up whitespaces */\n");

 /* Letzte Regel, Nicht zuordungbares Zeichen ************************************/
 fprintf(fh,"[^ ^\\n^\\t");
 i=0;
 while (delimeter_str[i]) {
  fprintf(fh,"^");
  if (strchr(FLEX_REG_SPECIALS,delimeter_str[i])) fprintf(fh,"\\");
  fprintf(fh,"%c",delimeter_str[i]);
  i++;
 }
 fprintf(fh,"]*\t");
 fprintf(fh, "{yyerror2(%cUnknown identifier <%%s>%c,yytext);}\n",'"','"');
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteScannerTstComFct(FILE *fh)
{ /* Erzeugt Code zur Erkennung von normal Kommentaren wie diesen ****************/
 int len=strlen(Comment_OPT.end)-1;                   /* Laenge des Ende Symbols */
 int i;                                                          /* Laufvariable */
 char *s=Comment_OPT.start;                   /* Abkuerzung fuer das Startsymbol */
 char *e=Comment_OPT.end;                      /* Abkuerzung fuer das Endesymbol */
 char spaces[]="                                 ";                 /* 32 Spaces */
 int indent=30;                                          /* Aktuelle Einrueckung */
 DBUG_ENTER("WriteScannerTstComFct");

 fprintf(fh,"%s\n{\n int c;\t    /* actual character */\n", /* Fktkopf schreiben */
  TEXT_SC_TSTCOMMENTSFCT_PROTO);

 if (Comment_OPT.nested==TRUE)                    /* Verschachtelte Kommentare ? */
  fprintf(fh," static int ComLevel=0;\t /* Level of nested comments */\n");
 fprintf(fh," static int ComStart=0;\t/* Store commentstart line */\n");
 fprintf(fh," if (ComStart==0) ComStart=LineNumber;\n");

 /* Hauptschleife zum Erkennen von Kommentaren ***********************************/
 fprintf(fh," for (;;) { \t/* Scan input forver */\n");

 fprintf(fh,"  while (((c=input())!=EOF)");
 if (Comment_OPT.newlines==FALSE)                 /* Nur einzeilige Kommentare ? */
  fprintf(fh," && (c!='\\n')");                            /* Bei EOLN abbrechen */
 fprintf(fh,") {\t/* Get text */\n");

 /* If Abfragen fuer Kommentarende ***********************************************/
 fprintf(fh,"%sif (c=='\\x%x') {\n",&spaces[indent],e[0]);
 indent--;                                           /* Einrueckung vergroessern */
 if (len>0) {                            /* Endesymbol laenger als ein Zeichen ? */
  fprintf(fh,"%swhile ((c=input())=='\\x%x');\t/* Skip chars */\n",
   &spaces[indent],e[0]);
  e++;                                           /* Ab dem zweiten Zeichen lesen */
  while (e[1]) {                                        /* Code fuer Ende Symbol */
   fprintf(fh,"%sif ((c=input())=='\\x%x') {\n",&spaces[indent],*e);
   indent--;                                              /* Einzug vergroessern */
   e++;                                        /* Naechstes Zeichen ueberpruefen */
  }
  fprintf(fh,"%sif (c=='\\x%x') {\n",&spaces[indent--],*e);
 }
 if (Comment_OPT.nested==TRUE) {                   /* Geschachtelte Kommentare ? */
  fprintf(fh,"%sif (ComLevel>0) ComLevel--;\n",&spaces[indent]);
  fprintf(fh,"%selse {\n",&spaces[indent--]);
  fprintf(fh,"%sComStart=0;\t/* Reset ComStart remember.*/\n%s"
   ,&spaces[indent],&spaces[indent]);
 } else fprintf(fh,"%s{",&spaces[indent]);
 fprintf(fh,"return (1);}\t/* Okay, comments match ! */\n");

 /* Schliessende Klammern fuer if then else Bloecke ******************************/
 for (i=0;i<len;i++) {                         /* Geoeffnete Klammern schliessen */
  indent++;                                                 /* Einzug verringern */
  fprintf(fh,"%s}\n",&spaces[indent]);          /* Schliessende Klammern fuer if */
 }

 fprintf(fh,"%s}",&spaces[++indent]);                       /* Letzte Klammer zu */

 /* Code fuer Erkennung von geschachtelten Kommentaren ***************************/
 if (Comment_OPT.nested==TRUE) {                   /* Geschachtelte Kommentare ? */
  fprintf(fh," else if (c=='\\x%x') { /* Nested comments */\n",*s);
  indent--;                                          /* Einrueckung vergroessern */
  s++;
  while (*s) {                                          /* Code fuer Ende Symbol */
   fprintf(fh,"%sif ((c=input())=='\\x%x') {\n",&spaces[indent],*s);
   indent--;                                              /* Einzug vergroessern */
   s++;                                        /* Naechstes Zeichen ueberpruefen */
  }
  if (Comment_OPT.nested==TRUE) fprintf(fh,"%sComLevel++;\n",&spaces[indent]);
  fprintf(fh,"%sreturn (TstComments());\t/* Recursive Call */\n",&spaces[indent]);
  len=strlen(Comment_OPT.start);                     /* Laenge des Start Symbols */
  for (i=0;i<len;i++) {                        /* Geoeffnete Klammern schliessen */
   indent++;                                                /* Einzug verringern */
   fprintf(fh,"%s}\n",&spaces[indent]);         /* Schliessende Klammern fuer if */
  }
 } else fprintf(fh,"\n");                           /* Nur Ende der Zeile setzen */

 /* Ende der Schleifen im generierten Code ***************************************/
 fprintf(fh,"%s}\t/* End of while loop */\n",&spaces[++indent]);
 /* Abruchbedingungen fuer Kommentare ********************************************/
 if (Comment_OPT.newlines==FALSE) {               /* Nur einzeilige Kommentare ? */
  fprintf(fh,"%sif (c=='\\n') {\t/* End of line ? */\n",&spaces[indent--]);
  fprintf(fh,"%sbreak;\t/* Comments not matched ! */\n",&spaces[indent++]);
  fprintf(fh,"%s}\n",&spaces[indent]);
 } else {                                       /* Anzahl der Zeilen hochzaehlen */
  fprintf(fh,"%sif (c=='\\n') LineNumber++;\n",&spaces[indent]);
 }
 fprintf(fh,"%sif (c==EOF) { /* End of file ? */\n",&spaces[indent--]);
 fprintf(fh,"%sbreak;\t/* Comments not matched ! */\n",&spaces[indent++]);
 fprintf(fh,"%s}\n",&spaces[indent]);

 fprintf(fh,"%s}\t/* End of forever loop */\n",&spaces[++indent]);

 /* Test des zuletzt gelesenen Zeichen der Schleifen *****************************/
 if (Comment_OPT.newlines==FALSE) {               /* Nur einzeilige Kommentare ? */
  fprintf(fh,"%sif (c=='\\n') { /* End of line ? */\n",&spaces[indent--]);
  fprintf(fh,"%sLineNumber++;\n",&spaces[indent]);
  fprintf(fh,"%sfprintf(YY_ERRFILE,%c%s %c);\n",
   &spaces[indent++],34,TEXT_SC_EOLN_INCOMMENT,34);
  fprintf(fh,"%s}\n",&spaces[indent]);
 }
 fprintf(fh,"%sif (c==EOF) { /* End of file ? */\n",&spaces[indent--]);
 fprintf(fh,"%sfprintf(YY_ERRFILE,%c%s %c);\n",
  &spaces[indent++],34,TEXT_SC_EOF_INCOMMENT,34);
 fprintf(fh,"%s}\n",&spaces[indent]);

 /* Kommentare sind fehlerhaft. Null als Fehlerkennung zurueck *******************/
 fprintf(fh," fprintf(YY_ERRFILE,%cPossible line of comment start : %%d\\n%c,ComStart);\n",34,34);
 fprintf(fh," ComStart=0;\t/* Reset ComStart remember.*/\n");
 if (Comment_OPT.nested==TRUE) fprintf(fh," ComLevel=0;\n");
 fprintf(fh," return (0);\t/* Matching comments not found */\n}\n");

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteScannerCommentRule(FILE *fh)
{ /* Generiert Code zur Erkennung von Kommentaren in der Sprache *****************/
 DBUG_ENTER("WriteScannerCommentRule");

 Comment_OPT.gen_code=FALSE;       /* Keine Programmcode fuer Kommantar-Funktion */

 if (Comment_OPT.start==NULL) {                   /* Kein Kommentarstartsymbol ? */
  if (Comment_OPT.end==NULL)                            /* Auch kein Endsymbol ? */
   WriteCommentSection(fh,TEXT_SC_NOCOMMENTS);               /* Keine Kommentare */
  else                                           /* Nur ein Endesymbol vorhanden */
   fprintf(stderr,ERR_SC_NO_COMSTARTSYMBOL);
  DBUG_VOID_RETURN;
 }

 if (IsNameInHTable(Comment_OPT.start)) {   /* Kommentarstart schonmal benutzt ? */
  fprintf(stderr,ERR_SC_COMSYMBOL_AU,Comment_OPT.start);
  DBUG_VOID_RETURN;
 }

 fprintf(fh,"\n /** %s **/\n",TEXT_SC_BEGINCOMMENTS);
 if (Comment_OPT.end==NULL) {                      /* Einzeilige Endkommentare ? */
  fprintf(fh,"%c%s%c[^\\n]*\t LineNumber++;\t/* Eat up one-line comments */\n",
   34,Comment_OPT.start,34);
 } else {
  char *tabs=(strlen(Comment_OPT.start)+3>=TAB_SIZE) ? "\t" : "\t\t";
  fprintf(fh,"%c%s%c%s (void)TstComments();\t/* Eat up comments */\n",
   34,Comment_OPT.start,34,tabs);
  fprintf(fh,"%c%s%c\t\t{fprintf(YY_ERRFILE,%c%s%c,LineNumber);}\n",34,
   Comment_OPT.end,34,34,ERR_COMMENTENND_FOUND,34);
  Comment_OPT.gen_code=TRUE;             /* Programmcode fuer Kommantar-Funktion */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static char *CheckText(char *text)
{ /* Check text for illegal characters *******************************************/
 char *buf=(char *)malloc(2*strlen(text)+1);
 char *i=text,*j=buf;
 for (;;) {
  if ((*i=='\\')||(*i=='"')) {
   *j='\\';
   j++;
  }
  *j=*i;
  j++;
  if (*i=='\0') break;
  i++;
 }
 return(buf);
} /*******************************************************************************/

static void WriteGetCharFct(FILE *fh)
{ /* Schreibt Routine zum einlesen einzelner Zeichen *****************************/
 fprintf(fh,"\n { /* Test scan-mode */\n");
 fprintf(fh,"  if (_GCI_[_GCM_].level) { /* Scan nested character strings ? */\n");
 fprintf(fh,"   _GC_=input(); /* Get one character */\n");
 fprintf(fh,"   if (strchr(_GCI_[_GCM_].stopchars,_GC_)==NULL) {\n");
 fprintf(fh,"    lvalp->%sval=_GC_; /* Set semantical value */\n",TEXT_ID_CHAR);
 fprintf(fh,"    return(_GCI_[_GCM_].stopid); /* Return token */\n");
 fprintf(fh,"   } else { /* Stop character found */\n");
 fprintf(fh,"    unput(_GC_); /* Put character back to input stream */");
 fprintf(fh,"   }\n");
 fprintf(fh,"  }\n");
 fprintf(fh," }\n");
} /*******************************************************************************/

static void WriteScannerBody(FILE *fh)
{ /* Erzeugt den Hauptteil des Scanners ******************************************/
 int i;                                       /* Laufvariable fuer Symboltabelle */
 int idStart=-1;   /* Beginn der Identifierregeln, bei flex an Ende der Regeln ! */
 SYStruct *sy;                                                    /* Hilfszeiger */
 SYtype type=KW_NONE_SY;                          /* Gerade aktueller Symboltype */
 char *tabs;                                     /* Zeiger auf Tabulatorenstring */
 DBUG_ENTER("WriteScannerBody");
 WriteCommentSection(fh,TEXT_SC_MAINPART);              /* Erlaeuterung zum Code */
 fprintf(fh,"%%%%\n");

 if (GetCharSize>0) WriteGetCharFct(fh);

 for (i=0;i<Nr_Symbols;i++) {                             /* Alle Symbole testen */
  sy=Symbols[i];                                             /* aktuelles Symbol */
  if (sy->type!=type) {                             /* Wechsel des Symboltypes ? */
   type=sy->type;                                 /* aktuellen Symboltype setzen */
   if (IsIdentifierSymbol(sy)) {              /* Ist das Symbol ein Identifier ? */
    if (idStart==-1) idStart=i;                   /* Anfang der Identifierregeln */
   } else                               /* Nein, Erlaeuterung zum Code einfuegen */
    fprintf(fh,"\n /** Scanning rules for %s Symbols ! **/\n",SYtype2Char(type));
  }
  if (NormalSymbol(sy)) {               /* Defaultregeln fuer Keywords einfuegen */
   tabs=(strlen(sy->text)+3>=TAB_SIZE) ? "\t" : "\t\t";
   if (sy->blanks==BLANKS_LEADING) fprintf(fh,"[ ]*");
   fprintf(fh,"%c%s%c",34,sy->text,34);
   if (sy->blanks==BLANKS_FOLLOWING) fprintf(fh,"[ ]*");
   fprintf(fh," %s return(%s);\n",tabs,sy->name);
  }
 }
 Project_OPT.IdStart=idStart;         /* Merken des Anfangs der Identifierregeln */
 /* Code fuer Identifier generieren **********************************************/
 if (idStart>=0) {                          /* Regeln fuer Identifier einbauen ? */
  fprintf(fh,"\n /** Scanning rules for %s Symbols ! **/\n",
   SYtype2Char(Symbols[idStart]->type));
  for (i=idStart;i<Nr_Symbols;i++) {                  /* Identifierregeln suchen */
   sy=Symbols[i];                                            /* Aktuelles Symbol */
   if (IsIdentifierSymbol(sy)) {                                  /* Ist es ID ? */
    tabs=(strlen(sy->text)+5>=TAB_SIZE) ? "\t" : "\t\t";
    fprintf(fh,"{%s%s}%s return(Conv2%s(lvalp,%s));\n",  /* Aufruf der ConvFct */
     SC_ID_PREFIX,sy->text,tabs,Conv2ID(sy->idtype),sy->text);
   } else break;                           /* Ende der Identifierreglen erreicht */
  }
 }
 /* Hier wird die Tabelle der lexikalischen Symbol sortiert um einen schnellen
 ** Zugriff auf die Elemente der Tabelle, sowohl ueber die text Komponente
 ** via HashTabelle und ueber die Namenskomponente via bsearch,
 ** zu haben. 
 */ qsort(Symbols,Nr_Symbols,sizeof(SYStruct *),SYStructSort);

 WriteScannerCommentRule(fh);            /* Regeln zur Erkennung von Kommentaren */

 WriteScannerBodyStd(fh);                    /* Default Regeln am Ende anhaengen */

 fprintf(fh,"\n%%%%\n");                      /* Ende des Hauptteils vom Scanner */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteConvFct(FILE *fh,int type)
{ /* Schreibt die Konvertierungsfunktion, bzw. Aufruf der Konvertierung **********/
 switch (type) {                       /* Komponente und Konvfunktion auswaehlen */
  case OFFSET_INT :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_INT);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=%s;\n",TEXT_ID_INT,TEXT_SC_RETURN_INT);
   break;
  case OFFSET_LONG :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_LONG);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=%s;\n",TEXT_ID_LONG,TEXT_SC_RETURN_LONG);
   break;
  case OFFSET_FLOAT :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_FLOAT);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=%s;\n",TEXT_ID_FLOAT,TEXT_SC_RETURN_FLOAT);
   break;
  case OFFSET_DOUBLE :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_DOUBLE);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=%s;\n",TEXT_ID_DOUBLE,TEXT_SC_RETURN_DOUBLE);
   break;
  case OFFSET_IDENTIFIER :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_IDENTIFIER);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=(char *)my_strdup(yytext);\n",TEXT_ID_IDENTIFIER);
   break;
  case OFFSET_CHAR :
   fprintf(fh,TEXT_SC_RETURNFUNCTION_HEADER,TEXT_ID_CHAR);
   fprintf(fh,"\n{\n");
   fprintf(fh," lvalp->%sval=yytext[0];\n",TEXT_ID_CHAR);
   break;
  default :
   break;
 }
 fprintf(fh,TEXT_SC_RETURNFUNCTION_FOOTER);
} /*******************************************************************************/

static void WriteReturnFunctions(FILE *fh)
{ /* Erzeugt Rueckgabefunktionen fuer Token nimmt semantischen Wert **************/
 DBUG_ENTER("WriteReturnFunctions");
 if (intV)          WriteConvFct(fh,OFFSET_INT);
 if (longV)         WriteConvFct(fh,OFFSET_LONG);
 if (floatV)        WriteConvFct(fh,OFFSET_FLOAT);
 if (doubleV)       WriteConvFct(fh,OFFSET_DOUBLE);
 if (identifierV)   WriteConvFct(fh,OFFSET_IDENTIFIER);
 if (charV)         WriteConvFct(fh,OFFSET_CHAR);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteTestScannerfct(FILE *fh)
{ /* Schreibt das Hauptprogramm fuer den Scanner *********************************/
 DBUG_ENTER("WriteTestScannerfct");
 fprintf(fh,"int main(int argc,char *argv[])\n");
 fprintf(fh,"{ /* Scannertest mainfunction */\n");
 fprintf(fh," int token;\t/* Result of scannerfunction */\n");
 fprintf(fh,"\n while ((token=yylex())!=0) { /* Scan input while eof */\n");
 fprintf(fh,"  printf(%cInput >|%%s|< recoginzed as token : %%s\\n%c,yytext,TokenList[token]);\n",'"','"');
 fprintf(fh," }\n");
 fprintf(fh,"\n return (0);\t /* End of testprogram */\n");
 fprintf(fh,"}\n");
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteyyerrorFunction(FILE *fh)
{ /* Write simple syntax error function ******************************************/
 DBUG_ENTER("WriteyyerrorFunction");
 fprintf(fh,"void yyerror(char *error)\n{\n");
 fprintf(fh," YY_NR_ERRORS++;\n");
 fprintf(fh," fprintf(YY_ERRFILE,%c%%s:%%d: %%s\\n%c,YY_IN_FILENAME,LineNumber,error);\n",34,34);
 fprintf(fh,"}\n\n");
 fprintf(fh,"void yyerror2(char *error,char *var)\n{\n");
 fprintf(fh," static char BUFFER[256];\n");
 fprintf(fh," sprintf(BUFFER,error,var);\n");
 fprintf(fh," yyerror(BUFFER);\n");
 fprintf(fh,"}\n\n"); 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteScannerEnd(FILE *fh)
{ /* Erzeugt das Ende des Scanners ***********************************************/
 DBUG_ENTER("WriteScannerEnd");

 WriteCommentSection(fh,TEXT_SC_ENDPART);        /* Kommentar zum Code einfuegen */

 WriteyyerrorFunction(fh);
 WriteReturnFunctions(fh);                           /* Konvertierungsfunktionen */

 if (Comment_OPT.gen_code==TRUE)        /* Funktion fuer Kommentare generieren ? */
  WriteScannerTstComFct(fh);                                  /* Code generieren */

 if (Project_OPT.sctest==TRUE) {              /* Testmainfunktion fuer Scanner ? */
  WriteCommentSection(fh,TEXT_SC_TESTMAIN);
  WriteTestScannerfct(fh);                            /* Hauptprogramm schreiben */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static int WriteScannerCode(void)
{ /* Erzeugen und schreiben des Scannercodes *************************************/
 FILE *outfile=NULL;                                 /* Ausgabefile des Scanners */
 char buffer[256];                                  /* Puffer fuer den Filenamen */
 char *filename=Project_OPT.filename;
 DBUG_ENTER("WriteScannerCode");

 MkSymbolNames();                                            /* Symbole benennen */

 if (Project_OPT.name) {                                       /* Name gegeben ? */
  strcpy(buffer,Project_OPT.name);                   /* Projectname ist Filename */
  strcat(buffer,SCANNER_EXTENDER);                           /* Suffix anhaengen */
 } else {                                      /* Spezifikationsfilenamen nehmen */
  int i=0;
  while (*filename&&(*filename!='.')) {                         /* Suffix suchen */
   buffer[i++]=*filename++;                                          /* Kopieren */
  }
  Project_OPT.name=my_strdup(buffer);                         /* Projektname setzen */
  strcpy(&buffer[i],SCANNER_EXTENDER);             /* Scanner - Suffix anhaengen */
 }

 if ((outfile=fopen(buffer,"w"))==NULL) {   /* Kann die Datei geoeffnet werden ? */
  fprintf(stderr,ERR_SC_CANT_OPEN_OUTPUTFILE,buffer);
  DBUG_RETURN(0);
 } 

 /* Scannercode generieren *******************************************************/
 WriteScannerHeader(outfile);
 WriteScannerBody(outfile);
 WriteScannerEnd(outfile);

 fclose(outfile);

 DBUG_RETURN(1);                                                 /* Alles okay ! */
} /*******************************************************************************/

/*********************************************************************************/
/********** EXPORT FUNKTIONEN ****************************************************/
/*********************************************************************************/

void InitScannerGenerator(int htablesize)
{ /* Initialisiert den Scannergenerator ******************************************/
 char *testptr;              /* Ueberpruefen, ob anlegen der Hashtabelle klappte */
 DBUG_ENTER("InitScannerGenerator");
 charV=0;intV=longV=floatV=doubleV=identifierV=0;    /* Kein Type bisher benutzt */
 Nr_Symbols=0;                          /* Anzahl der Symbole in der Hashtabelle */
 ScannerSTATE=SC_EMPTY;                                  /* Noch nichts passiert */
 HashTableSize=htablesize;
 SymbolTableSize=HashTableSize;                           /* Groesse der Tabelle */
 Symbols=(SYStruct **)malloc(SymbolTableSize*sizeof(SYStruct *));
 testptr=(char*)DH_hcreate(HashTableSize);             /* Symboltabelle erzeugen */
 strcpy(delimeter_str,"0123456789");
 delimeter_len=strlen(delimeter_str);
 DBUG_ASSERT(testptr);
 DBUG_VOID_RETURN;
} /*******************************************************************************/

int CheckStdType(int sy_type,int number)
{ /* Ueberprueft ob fuer die angegebene Zahl eine Standartregel existiert ********/
 DBUG_ENTER("CheckStdtype");
 if (number<0) {                                              /* Negative Zahl ? */
  fprintf(stderr,ERR_SC_STD_POSVALUES);
  number=-number;                                          /* Vorzeichen aendern */
 }
 switch (sy_type) {                                  /* Identifier unterscheiden */
  case INT_TYPE        :
   if (number>NR_STD_INT) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"INT",NR_STD_INT);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_INT;
   break;
  case LONG_TYPE        :
   if (number>NR_STD_LONG) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"LONG",NR_STD_LONG);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_INT;
   break;
  case FLOAT_TYPE      :
   if (number>NR_STD_FLOAT) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"FLOAT",NR_STD_FLOAT);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_FLOAT;
   break;
  case DOUBLE_TYPE      :
   if (number>NR_STD_DOUBLE) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"DOUBLE",NR_STD_DOUBLE);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_FLOAT;
   break;
  case CHAR_TYPE :
   if (number>NR_STD_CHAR) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"CHAR",NR_STD_CHAR);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_CHAR;
   break;
  case IDENTIFIER_TYPE :
   if (number>NR_STD_ID) {
    fprintf(stderr,ERR_SC_STD_VALUE_RANGE,number,"IDENTIFIER",NR_STD_ID);
    DBUG_RETURN(0);                                       /* Standartwert nehmen */
   }
   number+=OFFSET_IDENTIFIER;
   break;
  default : DBUG_RETURN (0);
 }
 DBUG_RETURN(number);                                         /* Number ist okay */
} /*******************************************************************************/

static void AddDelimeterChar(char *text)
{ /* Add beginning character to delimeter set ************************************/
 if (!isalnum(text[0])) {
  if (strchr(delimeter_str,text[0])==NULL) {
   delimeter_str[delimeter_len++]=text[0];
   delimeter_str[delimeter_len]='\0';
  }
 }
} /*******************************************************************************/

int AddSymbol(int type,char *name,char *text,int blank)
{ /* Fuegt einen Symbolknoten in die Tabelle ein *********************************/
 SYStruct *temp=(SYStruct *)malloc(sizeof(SYStruct));
 DBUG_ENTER("AddSymbol");
 temp->name=(name==NULL) ? NULL : my_strdup(name);                      /* kopieren */
 temp->text=(text==NULL) ? NULL : CheckText(text);                   /* kopieren */
 temp->type=type;                                            /* Knotentyp setzen */

 temp->blanks=blank;                                           /* set blank mode */

 if (type==KW_ID_STD_SY) {                /* Ist es ein Standart Identifikator ? */
  temp->idtype=atoi(name);                               /* Type des Identifiers */
  SetUsedTypeFlag(temp->idtype);
 } else temp->idtype=0;                                   /* Kein Std-Identifier */

 AddDelimeterChar(text);

 item.key=temp->text;                      /* Textdarstellung ist der Schluessel */
 item.data=(char *)temp;                     /* Zeiger auf Daten (Symbol) setzen */
 if (DH_hsearch(item,DH_FIND)!=NULL) {               /* Symbol schon definiert ? */
  fprintf(stderr,ERR_SC_RULE_ADEF,text);               /* Fehlermeldung ausgeben */
  FreeSYStruct(temp);                               /* Speicher wieder freigeben */
  DBUG_RETURN(0);
 } else {
  if (DH_hsearch(item,DH_ENTER)==NULL) {     /* Knoten einfuegen, Tabelle voll ? */
   if (ScannerSTATE!=SC_TABLEFULL) {                       /* Symboltable voll ? */
    fprintf(stderr,ERR_SC_SYMBOLTABLE_FULL,HashTableSize);
    ScannerSTATE=SC_TABLEFULL;                            /* Fehlerquelle merken */
   }
  }
  Symbols[Nr_Symbols]=temp;                                     /* Symbol merken */
  Nr_Symbols++;                                               /* Anzahl erhoehen */
  if (Nr_Symbols==SymbolTableSize) ResizeSymbolTable();  /* Tabelle vergroessern */
  DBUG_RETURN(1);
 }
} /*******************************************************************************/

int AddRegExprSymbol(int type,char *name,char *text)
{ /* Fuegt einen getypten regulaeren Ausdruck in die Tabelle ein *****************/
 SYStruct *temp=(SYStruct *)malloc(sizeof(SYStruct));
 DBUG_ENTER("AddRegExprSymbol");
 temp->name=(name==NULL) ? NULL : my_strdup(name);                      /* kopieren */
 temp->text=(text==NULL) ? NULL : my_strdup(text);                      /* kopieren */
 temp->type=KW_ID_REGEXPR_SY;                                /* Knotentyp setzen */

 temp->blanks=BLANKS_NORMAL;

 if (type==INT_TYPE) temp->idtype=OFFSET_INT;            /* Type des Identifiers */
 else if (type==LONG_TYPE) temp->idtype=OFFSET_LONG;
 else if (type==FLOAT_TYPE) temp->idtype=OFFSET_FLOAT;
 else if (type==DOUBLE_TYPE) temp->idtype=OFFSET_DOUBLE;
 else if (type==IDENTIFIER_TYPE) temp->idtype=OFFSET_IDENTIFIER;
 else temp->idtype=0;
 SetUsedTypeFlag(temp->idtype);   /* Setzt Opts zum Generieren von Fct des Types */
 item.key=temp->text;                      /* Textdarstellung ist der Schluessel */
 item.data=(char *)temp;                     /* Zeiger auf Daten (Symbol) setzen */
 if (DH_hsearch(item,DH_FIND)!=NULL) {               /* Symbol schon definiert ? */
  fprintf(stderr,ERR_SC_RULE_ADEF,text);               /* Fehlermeldung ausgeben */
  FreeSYStruct(temp);                               /* Speicher wieder freigeben */
  DBUG_RETURN(0);
 } else {
  if (DH_hsearch(item,DH_ENTER)==NULL) {     /* Knoten einfuegen, Tabelle voll ? */
   if (ScannerSTATE!=SC_TABLEFULL) {                       /* Symboltable voll ? */
    fprintf(stderr,ERR_SC_SYMBOLTABLE_FULL,HashTableSize);
    ScannerSTATE=SC_TABLEFULL;                            /* Fehlerquelle merken */
   }
  }
  Symbols[Nr_Symbols]=temp;                                     /* Symbol merken */
  Nr_Symbols++;                                               /* Anzahl erhoehen */
  if (Nr_Symbols==SymbolTableSize) ResizeSymbolTable();  /* Tabelle vergroessern */
  DBUG_RETURN(1);
 }
} /*******************************************************************************/

int ScannerDescComplete(void)
{ /* Scanner description complete (called by parser ccomp.y) *********************/
 int State=1;
 DBUG_ENTER("ScannerDescComplete");

 if (ScannerSTATE==SC_TABLEFULL) {
  fprintf(stderr,ERR_SC_SYMBOLTABLE_2_SMALL);
  fprintf(stderr,ERR_SC_SYMBOLTABLE_SIZE,HashTableSize,Nr_Symbols); 
  State=0;                                                         /* Fehlerfall */
 }
 
 if (State) {                                      /* Keine Fehler aufgetreten ? */
  State=WriteScannerCode();                             /* Scannercode schreiben */
 }

 DBUG_RETURN(State);
} /*******************************************************************************/

void CleanUpScannerGenerator(void)
{ /* Raeumt das Modul wieder auf und gibt dynamisch Speicher frei ****************/
 int i;
 DBUG_ENTER("CleanUpScannerGenerator");
 for (i=0;i<Nr_Symbols;i++)                       /* Alle Symboledaten freigeben */
  if (Symbols[i]) FreeSYStruct(Symbols[i]);             /* Symboldaten freigeben */
 free((void*)Symbols);                                /* Symboltabelle freigeben */
 Nr_Symbols=0;                                   /* Keine Symbole mehr vorhanden */
 Symbols=NULL;                                                   /* Tabelle leer */
 SymbolTableSize=0; 
 /*hdestroy();*/                                        /* Hashtabelle freigeben */
 DBUG_VOID_RETURN;
} /*******************************************************************************/

int LookUpSymbol(char *symbolname)
{ /* Test if symbol is in Table **************************************************/
 int len;
 if ((len=strlen(symbolname))>0) {                               /* Name given ? */
  if (symbolname[0]=='"') {                             /* is it a text symbol ? */
   DH_ENTRY *fitem;
   static char syBUF[256];                          /* Static buffer for symbols */
   memcpy(syBUF,&symbolname[1],len);             /* Copy name and remove " chars */
   syBUF[len-2]='\0';
   item.key=syBUF;                                       /* Search in Hash Table */
   if ((fitem=DH_hsearch(item,DH_FIND))==NULL) return 0;
   else {
    strcpy(symbolname,((SYStruct *)(fitem->data))->name);
    return 1;
   }
  } else {
   return (IsNameInHTable(symbolname) ? 1 : 0);     /* Search in Array (BSearch) */
  }
 }
 return (0); 
} /*******************************************************************************/

char *GetSymbolText(char *symbolname)
{
 int i;
 for (i=0;i<Nr_Symbols;i++) {
  if (strcmp(Symbols[i]->name,symbolname)==0) {
   return(Symbols[i]->text);
  }
 }
 return (NULL);
}

void WriteScannerExportVars(FILE *fh)
{ /* Write external used variables in scanner file to parser declaration *********/
 if (GetCharSize>0) {
  fprintf(fh,"extern int _GCM_; /* Definied in scanner file */\n");
  fprintf(fh,"extern struct _GCINFO_ {char *stopchars;int stopid;int level;} _GCI_[];\n");
 }
 fprintf(fh,"extern int NR_of_EXPRs; /* Number of expression to derive */\n");
} /*******************************************************************************/

void WriteToken(FILE *fh)
{ /* Writes a list of all lexical token ******************************************/
 int i,print=0,token=0;
 char *tmp;
 for (i=0;i<Nr_Symbols;i++) {         /* Array with TokenID Stringrepresentation */
  if (((i%4)==0) || print) {fprintf(fh,"\n%%token ");print=0;token=1;}
  if (NormalSymbol(Symbols[i])) {                       /* Normal Identifierer ? */
   fprintf(fh,"%s ",Symbols[i]->name);
   token=0;
  } else {                                                         /* Identifier */
   tmp=Conv2ID(Symbols[i]->idtype);
   if (token==0) fprintf(fh,"\n%%token ");
   fprintf(fh,"<%sval> %s",tmp,Symbols[i]->text);
   print=1;
  }
 }
 for (i=0;i<GetCharSize;i++) {
  if ((i%4)==0) fprintf(fh,"\n%%token <%sval> ",TEXT_ID_CHAR);
  fprintf(fh,"%s ",GetCharArray[i]->name);
 }
 fprintf(fh,"\n\n");
} /*******************************************************************************/

void WriteScannerUnionMembers(FILE *fh)
{ /* Writes names with semantical values to file fh ******************************/
 fprintf(fh,"%%union {\n");
 if (intV)                     fprintf(fh," int %sval;\n",TEXT_ID_INT);
 if (longV)                    fprintf(fh," long %sval;\n",TEXT_ID_LONG);
 if (floatV)                   fprintf(fh," float %sval;\n",TEXT_ID_FLOAT);
 if (doubleV)                  fprintf(fh," double %sval;\n",TEXT_ID_DOUBLE);
 if (identifierV)              fprintf(fh," char *%sval;\n",TEXT_ID_IDENTIFIER);
 if ((charV)||(GetCharSize>0)) fprintf(fh," char %sval;\n",TEXT_ID_CHAR);
 WriteParserTypes(fh);
 fprintf(fh,"}\n\n");
} /*******************************************************************************/

static char *ParseGetChars(char *chars)
{ /* Testet den Ausdruck auf Korrektheit *****************************************/
 char *s=chars;
 static char buffer[256];
 static char nr[6];
 buffer[0]=0;
 if (*s=='"') {
  s++;
  for (;;) {
   if (*s==0) break;
   if (*s=='"') return((char *)my_strdup(buffer));
   if (*s=='\\') {
    s++;
    if (*s==0) break;
   }
   sprintf(nr,"\\x%x",*s);
   strcat(buffer,nr);
   s++;
  }
 }
 return(NULL);
} /*******************************************************************************/

int AddGetCharSymbol(char *name,char *chars)
{ /* Erzeugt Code zum lesen einzelner Zeichen ************************************/
 SYStruct *temp=(SYStruct *)malloc(sizeof(SYStruct));
 DBUG_ENTER("AddGetCharSymbol");
 temp->name=(name==NULL) ? NULL : my_strdup(name);                   /* kopieren */
 temp->name=AppendExtender(temp->name,Identifier_OPT.extender);
 temp->text=(chars==NULL) ? NULL : ParseGetChars(chars);
 if (temp->text!=NULL) {
  temp->idtype=GetCharSize;
  item.key=name;
  item.data=(char *)temp;                    /* Zeiger auf Daten (Symbol) setzen */
  if (DH_hsearch(item,DH_FIND)!=NULL) {              /* Symbol schon definiert ? */
   fprintf(stderr,ERR_SC_RULE_ADEF,name);              /* Fehlermeldung ausgeben */
  } else {
   int i;
   for (i=0;i<GetCharSize;i++) {
    if (strcmp(name,GetCharArray[i]->name)==0) {
     fprintf(stderr,ERR_SC_RULE_ADEF,name);            /* Fehlermeldung ausgeben */
    }
   }
   GetCharArray[GetCharSize++]=temp;
   DBUG_RETURN(1);
  }
 }  
 FreeSYStruct(temp);                               /* Speicher wieder freigeben */
 DBUG_RETURN(0);
} /*******************************************************************************/

int IsStopCharToken(char *name)
{ /* Is the token name a stop character symbol ? *********************************/
 int i;
 for (i=0;i<GetCharSize;i++) {
  if (strcmp(name,GetCharArray[i]->name)==0) return (GetCharArray[i]->idtype);
 }
 return -1;
} /*******************************************************************************/

/*********************************************************************************/
/********** Hilfsfunktion zum Sortieren ******************************************/
/*********************************************************************************/

static int SYStructSort(SYStruct **p1,SYStruct **p2)
{ /* Sortierfunktion der Symboleintraege fuer QSort ******************************/
 int cmp=strcmp((*p1)->name,(*p2)->name);            /* Symbol Namen vergleichen */
 if (((*p1)->type>KW_NORMAL_SY)&&((*p2)->type>KW_NORMAL_SY)) {  /* Normales SY ? */
  if (cmp==0) {                                       /* Zwei gleiche Elemente ? */
   fprintf(stderr,ERR_SC_DUPLICATE_SYMBOLS);
   fprintf(stderr,"(%s -> %s) and (%s -> %s)\n",
    (*p1)->name,(*p1)->text,(*p2)->name,(*p2)->text);
  }
 }
 return (cmp);
} /*******************************************************************************/
