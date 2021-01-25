/* include-file fuer edit.c u.ae.              cedit.h		*/
/* letzte aenderung:    25.07.88		*/

#define TOUPPER(c) (islower(c) ? toupper(c) : (c))
#define TOLOWER(c) (isupper(c) ? tolower(c) : (c))

#if N_MAT
#if N_STR
#define VERSION "2.3"		/* Versions-nummer		*/
#define OLD_VERS "2.2"		/* alte Versions-nummer		*/
#define V_OLD_VERS "2.1"	/* sehr alte Versions-nummer		*/
#else
#define VERSION "2.2"		/* Versions-nummer		*/
#define OLD_VERS "2.1"		/* alte Versions-nummer		*/
#endif
#else
#define VERSION "2.1"		/* Versions-nummer		*/
#endif

#define isapvers  (edit_vers == AP_VERSION)
#define isfctvers (edit_vers == FCT_VERSION)
#define EDIT_VERS (isapvers ? "AP" : "Function")
#define EDIT_VERS_C (isapvers ? 'A' : 'F')

typedef long int EXTYP[2];		/* fuer expressiontyp		*/

#define FCOLS 130		/* spalten fuer prettyprint		*/
#define MINCOLS 20		/* mindest-spalten		*/

#define LENFN  64		/* laenge eines file-namens		*/
#define LENLIB 128		/* laenge einer library		*/
#define LENFIL 256		/* laenge eines (kompl.) file-namens		*/
#define LENEXT 6		/* laenge einer file-extension		*/

#define US_MARK  ((STACKELEM)(LSTR1 | tovalue('_')))
#define US_MARK0  ((STACKELEM)(LSTR0 | tovalue('_')))

/* Editor-Symbole		*/
#define FIL_DOLLAR '$'
#define FIL_RED '^'
#define FIL_PRE '!'		/* extern		*/
#define FILIPRE '@'		/* fuer reduma		*/
#define INT_FC  '&'		/* kennzeichen interner funktionen		*/
#define STR_VAR  '%'		/* kennzeichen von variablen in strings		*/
#define GRAPH1 '$'	/* speziell fuer streamvariablen in graph	*/
#define GRAPH2 '#'	/* speziell fuer streamvariablen in graph	*/
#define GRAPH3 '@'	/* speziell fuer streamvariablen in graph	*/
/* interne kennzeichnung von pfadnamen		*/
#define TRENN   '!'		/* trennzeichen in fnam		*/
#define EXTRENN '_'		/* externes trennzeichen		*/

#define MUST '\5'		/* in kwtb: naechstes zeichen muss in der		*/
		/* eingabe auftauchen		*/
#define PRED_KL '('		/* klammer fuer vordefinierte funktionen		*/
#define CALL_KL '['		/*      "        andere CALL's		*/

/* displ		*/
#define UNPR '\0'		/* nicht darstellbares zeichen (fuer AADR)		*/
#define UNPR_C '^'		/* nicht darstellbares zeichen (fuer putchar)		*/
#define UNKN_C '~'		/* unknown constructor		*/
#define UNKN_A '%'		/* unknown atom		*/
#define ABBRSMB '?'		/* abbreviationsymbol		*/
#define LABBRSMB '.'		/* long -    "		*/
#define DECPOINT '.'		/* decimalpoint		*/
#define NEGSIGN '~'		/* negative sign (unaer)*/
#define DECEXP 'e'		/* exponential		*/
#define STRBEGIN '\''		/* string-begin		*/
#define STREND '`'		/* string-end		*/
#define LSTBEGIN '<'
#define LSTEND   '>'
#define CRSEXPR '%'		/* cursor-expression		*/

#define ISTOP  (topof(AADRST)== STBOTTOM || class(topof(AADRST))== UNPR  && second(AADRST)== STBOTTOM)

#define DSMALL	0x01	/* small-modus	*/
#define DDECL	0x02	/* def abkuerzen	*/
#define DGOAL	0x04	/* bei abgekuerzten def goal anzeigen	*/
#define DFCT	0x08	/* bei abgekuerzten def fct-liste anzeigen	*/
#define DTDECL	0x10	/* def nur in defs abkuerzen	*/
#define DVERT	0x20	/* vertikal anzeigen		*/
#define DVERT1	0x40	/* vertikal anzeigen		*/
#define DVERT2	0x80	/* vertikal anzeigen		*/

#define IS_SMALL(x)	((x) & DSMALL)
#define IS_DDECL(x)	((x) & DDECL)
#define IS_DGOAL(x)	((x) & DGOAL)
#define IS_DFCT(x)	((x) & DFCT)
#define IS_DTDECL(x)	((x) & DTDECL)
#define IS_DVERT(x)	((x) & DVERT)
#define IS_DVERT1(x)	((x) & DVERT1)
#define IS_DVERT2(x)	((x) & DVERT2)

/* editor-versionen		*/
#define ALL_VERSION 0
#define FCT_VERSION 1
#define AP_VERSION  2

/* aufruf der reduktion		*/
#define RED_T     0x01
#define RED_FROM  0x02		/* auch aufruf des editors	*/
#define RED_RED   0x04
#define RED_PP    0x08		/* commando preprocess		*/
#define RED_UNLD  0x400		/* commando unload		*/
#define RED_PLD   0x800		/* commando preload		*/
#define RED_RESET 0x40		/* reset bei uebergabe		*/
#define RED_TO   (RED_T | RED_RESET)
#define RED_PREP (RED_TO | RED_PP)		/* pre-process		*/
#define RED_ALL  (RED_TO | RED_FROM | RED_RED)
#define RED_PRELOAD (RED_TO | RED_FROM | RED_PLD)		/* pre-load		*/

#define red_to(c)	((c) & RED_T)
#define red_from(c)	((c) & RED_FROM)
#define red_red(c)	((c) & RED_RED)
#define red_pre(c)	((c) & RED_PP)
#define red_reset(c)	((c) & RED_RESET)
#define red_preld(c)	((c) & RED_PLD)
#define red_unld(c)	((c) & RED_UNLD)

#define L_UNDEFINED	0
#define L_REDUCED	1
#define L_PRELOAD	2
#define L_PREPROC	4

/* aufruf von prettyprint		*/
#define PPNORMAL  0
#define PPREPLACE 1
#define PPDISPLAY 2
#define PPLINE    4
#define PPASC     8
/* protocoll-arten	*/
#define PROTPREV	64	/* protokoll vor reduktion	*/
#define PROTAFTER	128	/* protokoll nach reduktion	*/

#define PP_REPLACE(c)     ((c) & PPREPLACE)
#define PP_DISPLAY(c)     ((c) & PPDISPLAY)
#define PP_LINE(c)        ((c) & PPLINE)
#define PP_ASC(c)         ((c) & PPASC)
#define PROT_NO(c)	((c) & PROTNO)
#define PROT_PREV(c)	((c) & PROTPREV)
#define PROT_AFTER(c)	((c) & PROTAFTER)

/* dateinamen-index		*/
#define FN_ED  0
#define FN_PP  1
#define FN_DOC 2
#define FN_PRT 3
#define FN_ASC 4
#define FN_RED 5
#define FN_PRE 6
#define FN_ANZ 7		/* anzahl dateinamen		*/

#ifdef IS_EDITOR
int edit_vers,		/* editor-version		*/
    compile_only,
    Changes_default = FALSE,
    batch;		/* batch-editor ? (oder so aehnlich)		*/
char *exednm;
int protocoll = PPLINE | PPNORMAL | PROTAFTER;		/* protocoll-type	*/
#else
extern int edit_vers,compile_only,Changes_default,batch;
extern char *exednm;
extern int protocoll;

extern void error();
#endif

#if SINTRAN3
#define HELPFILE "(inc)creduma:hlp"
#else
#define HELPFILE "../inc/creduma.hlp"
#endif

#if TX3
#define CALLREDUCE(s,t,p)	tx3tored(s,t,p)
#define STARTREDUMA()		startreduma()
#define STOPREDUMA()		stopreduma()
#else
#define CALLREDUCE(s,t,p)	reduce(s,t,p)
#define STARTREDUMA()
#define STOPREDUMA()
#endif

/* end of      cedit.h		*/
