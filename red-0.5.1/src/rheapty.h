/* $Log: rheapty.h,v $
 * Revision 1.2  1992/12/16  12:49:58  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

/*****************************************************************************/
/* rheapty.h                                                                 */
/*               In dieser Datei stehen die Definitionen der Heaptypen       */
/*               und Heapstrukturen, sowie die Zugriffsmakros des Heaps.     */
/* ------------------------------------------------------------------------- */
/*   Ein Deskriptor hat eine Groesse von 16 Byte :                           */
/*   =============================================                           */
/*     Deskriptormaske :                                                     */
/*     -----------------                                                     */
/*       1 Byte fuer die Typdefinition der Deskriptor - Klassen              */
/*              (typedef char DESC_CLASS;)                                   */
/*       1 Byte fuer die Typdefinition der Deskriptor - Typen                */
/*              (typedef char DESC_TYPE;)                                    */
/*       2 Byte fuer die Typdefinition des Deskriptor - Refcount             */
/*              (typedef unsigned short COUNT;)                              */
/*                                                                           */
/*     Spezifikation der unterschiedlichen DESCriptor_CLASS Definitionen :   */
/*     -------------------------------------------------------------------   */
/*       12 Byte fuer die Typdefinition der Strukturen der                   */
/*               unterschiedlichen DESCriptor CLASS Definitionen             */
/*                                                                           */
/*       Alle Strukturen wurden auf 12 Byte aufgefuellt.                     */
/*       Platz fuer eine Edit-Information wurde in Listen-,                  */
/*       Conditional- und Funktionsstrukturen integriert.                    */
/*                                                                           */
/*  MC-68000 :                                                               */
/*       char  8 Bit  |  short 16 Bit  |   int  32 Bit   |   double 64 Bit   */
/*                                                                           */
/*       PTR_HEAPELEM  ist als Typ von int definiert.                        */
/*                                                                           */
/*****************************************************************************/

#ifndef RHEAPTY_H
#define RHEAPTY_H

#if MEASURE
#define MEAS_CALL(x) measure(x),
#else
#define MEAS_CALL(x)
#endif  /* MEASURE */

typedef unsigned char DESC_CLASS;     /* enum in 1 byte    */

/* the following comments apply to GRAPH only (the reduma is not affected): */
/* Die letzten beiden bit von class geben die Deskriptorlaenge an. */
/* Wenn Bit 7,6 = 0,0  -> normale      Deskriptorlaenge,           */
/* Wenn Bit 7,6 = 0,1  -> doppelte     Deskriptorlaenge,           */
/* Wenn Bit 7,6 = 1,0  -> dreifache    Deskriptorlaenge,           */
/* Wenn Bit 7,6 = 1,1  -> undefinierte Deskriptorlaenge.           */
/* D.h., die Deskriptorlaenge berechnet sich folgendermassen :     */
/*     l = (R_DESC(*p),class)>>6) + 1                              */
/* Dies wird in der Funktion compheap benutzt !                    */

#define C_SCALAR          '\1'
#define C_DIGIT           '\2'
#define C_LIST            '\3'
#define C_MATRIX          '\4'
/* dg *//* neue klasse fuer die compilierte version */
#define C_FUNC            '\5'
#define C_EXPRESSION      '\6'
#define C_CONSTANT        '\7'  /* free variable desc*/
#define C_VECTOR          '\10'
#define C_TVECTOR         '\11'
/* Temporaer fuer Heap-swap-Routine */
#define C_SWAPHEAP        '\12'
#define C_MATCHING        '\13'
#define C_PATTERN         '\14'
/* dg 02.05.91 cons-listen */
#define C_CONS            '\15'

/* ach 04/03/93, used for messages */
#define C_SENT            '\52'

/* folgende 6 Classes sind GRAPH-Classes */
#define C_PROCESS         '\200'
#define C_STREAM          '\201'
#define C_GFIELD          '\102'
#define C_MERGE_FIELD     '\103'
#define C_SEMAPHORE       '\104'
#define C_MERGE_STREAM    '\105'
#define C_EDIT            '\206'

typedef unsigned char DESC_TYPE;      /* enum in 1 byte     */

/* Descriptor ist in der Freiliste    */
#define TY_FREE              '\0'
#define TY_UNDEF             '\1'
#define TY_INTEGER           '\2'
#define TY_REAL              '\3'
#define TY_BOOL              '\5'
#define TY_STRING            '\6'
#define TY_DIGIT             '\7'
#define TY_REC              '\11'
#define TY_SUB              '\12'
#define TY_COND             '\13'
#define TY_EXPR             '\15'
#define TY_VAR              '\16'
#define TY_NAME             '\17'
#define TY_MATRIX           '\21'
/* for matrix with more than two dimensions  */
#define TY_SWITCH           '\22'  /* BAR */
#define TY_MATCH            '\23'
#define TY_NOMAT            '\25'
#define TY_LREC             '\26'  /* letrec   *//* CS */
#define TY_LREC_IND         '\27'  /* letrec   *//* CS */
#define TY_LREC_ARGS        '\31'  /* letrec   *//* CS */
/* dg *//* neue typen fuer die compilierte version */
#define TY_COMB             '\32'
#define TY_CLOS             '\33'
#define TY_CONDI            '\34'
#define TY_CASE             '\35'
#define TY_WHEN             '\36'
#define TY_GUARD            '\37'
#define TY_BODY             '\40'
#define TY_PRIM             '\41'

/* uh */ /* Neue Typen fuer ZF-Expressions */
#define TY_ZF               '\42'
#define TY_ZFCODE           '\43' /* Code-Descriptor */

/* uh */ /* Neue Typen fuer Pattern Matching */
#define TY_LIST             '\44'
#define TY_AS               '\45'
#define TY_DOTDOTDOT        '\46'
#define TY_LASTDOTDOTDOT    '\47'
#define TY_DOTPLUS          '\50'
#define TY_LASTDOTPLUS      '\51'
#define TY_DOTSTAR          '\52'
#define TY_LASTDOTSTAR      '\53'

#define TY_SELECTION        '\54'
#define TY_CLAUSE           '\55'
/* dg 02.05.91 cons-listen */
#define TY_CONS             '\56'

typedef unsigned short COUNT;         /*  Refcount  2 byte */

/* working-definitions */
typedef int                 T_HEAPELEM ;
typedef int                 *PTR_HEAPELEM ;
typedef struct T_descriptor *T_PTD;
typedef int                 INSTR; /* dg */

/* Berechnung der heapsize fuer newheap */
#define HEAPSIZEOF(x) ((sizeof(x) + sizeof(T_HEAPELEM) - 1)/sizeof(T_HEAPELEM))

typedef struct T_hblock {
          struct T_hblock *next;  /* ptr to next block        */
          T_HEAPELEM      **ref;  /* blk is ref'ed by ...     */
        }                T_HBLOCK;

/************************************************************************/
/*                        A C H T U N G !                               */
/*  Die Strukturkomponente 'special' wird in einigen der folgenden      */
/*  Definitionen fuer zwei unterschiedliche Zwecke verwendet:           */
/*     1) Bei Benutzung in einem "direkten" Deskriptor:                 */
/*          als "edit"-Feld                                             */
/*     2) Bei Benutzung in einem "indirekten" Deskriptor                */
/*        (wird nur bei Listen verwendet):                              */
/*          als "offset"-Feld.                                          */
/*  Dadurch wird ein einheitlicher Zugriff auf dieses Feld durch die    */
/*  L-,R- und A-Macros gewaehrleistet.                                  */
/*                                                                      */
/************************************************************************/

#if  ODDSEX    
#define HEADER   \
    DESC_TYPE   type; \
    DESC_CLASS class; \
    COUNT  ref_count
#else    
#define HEADER   \
    COUNT ref_count;  \
    DESC_CLASS class; \
    DESC_TYPE type
#endif    

typedef struct {
 HEADER;
 int vali;
 double valr;
}  ST_SCALAR;

typedef struct {
    HEADER;
#if  ODDSEX
    COUNT      ndigit;
    COUNT        sign;
#else 
    COUNT        sign;
    COUNT      ndigit;
#endif
    int           Exp;  /* wegen include der Standardbibliothek <"math.h">  */
    PTR_HEAPELEM ptdv;  /* in rbibfunc.c  und L_ exp = R_ exp Markozuweisung*/
} ST_DIGIT;

typedef struct {            /* SJ-ID  Typdefinition fuer direkte   */
    HEADER;
#if  ODDSEX
    COUNT          dim;
    COUNT          special; /*        und indirekte Listenstruktur */
#else 
    COUNT          special; /*        und indirekte Listenstruktur */
    COUNT          dim;
#endif
    T_PTD          ptdd;
    PTR_HEAPELEM   ptdv;
} ST_LIST;

typedef struct {
    HEADER;
#if  ODDSEX
    COUNT       ncols;
    COUNT       nrows;
#else 
    COUNT       nrows;
    COUNT       ncols;
#endif
    T_PTD        ptdd;
    PTR_HEAPELEM ptdv;
} ST_MVT;              /* matrix / vector / transposed vector */

typedef struct {
    HEADER;
#if  ODDSEX
    char freespace[2];
    COUNT       special;       /* SJ-ID */
#else 
    COUNT       special;       /* SJ-ID */
    char freespace[2];
#endif
    PTR_HEAPELEM  ptte;
    PTR_HEAPELEM  ptee;
} ST_COND;

typedef struct {
    HEADER;
    char freespace[8];
    PTR_HEAPELEM  pte;
} ST_EXPR;

typedef struct {
    HEADER;
    char freespace[4];
    int        nlabar;
    T_PTD        ptnd;   /* Pointer auf Namensdescriptor */
} ST_VAR;

typedef struct {
    HEADER;
    char freespace[8];
    PTR_HEAPELEM  ptn;   /* Heapadresse des Namenstrings */
} ST_NAME;

typedef struct {
    HEADER;
#if  ODDSEX
    COUNT           nargs;
    COUNT         special;          /* SJ-ID */
#else 
    COUNT         special;          /* SJ-ID */
    COUNT           nargs;
#endif
    PTR_HEAPELEM namelist;
    PTR_HEAPELEM      pte;
} ST_FUNC;
#define ZF_START     1
#define ZF_FILTER    2
#define ZF_GENERATOR 3
typedef struct {
    HEADER;
#if  ODDSEX
    COUNT           nargs;
    COUNT         special;          /* SJ-ID */
#else 
    COUNT         special;          /* SJ-ID */
    COUNT           nargs;
#endif
    PTR_HEAPELEM namelist;
    PTR_HEAPELEM      pte;
} ST_ZF;

typedef struct {
    HEADER;
#if  ODDSEX
    COUNT           nargs;
    COUNT         zfbound;          /* SJ-ID */
#else 
    COUNT         zfbound;          /* SJ-ID */
    COUNT           nargs;
#endif
    T_PTD             ptd;          /* Zeiger in Graph (Descriptor) */
    PTR_HEAPELEM varnames;          /* Zeiger auf gebundene Variablen */
} ST_ZFCODE;

/************ Neue Patternmatching Descriptoren UH 4.4.91 ******************/ 

typedef struct {
    HEADER;
    char freespace[4];
    T_PTD             next;         /* Zeiger auf naechsten CLAUSE-Descriptor */
    PTR_HEAPELEM      sons;         /* Zeiger auf Pattern-Guard-Body-Triple   */
} ST_CLAUSE;

typedef struct {
    HEADER;
#if  ODDSEX
    COUNT         nclauses;         /* Anzahl der Klauseln (incl OTHERWISE, excl END) */  
    char freespace[2];
#else 
    char freespace[2];
    COUNT         nclauses;         /* Anzahl der Klauseln (incl OTHERWISE, excl END) */  
#endif
    T_PTD          clauses;         /* Zeiger auf ersten CLAUSE-Descriptor */
    T_PTD        otherwise;         /* Zeiger auf OTHERWISE Expr oder NIL */
} ST_SELECTION;

typedef struct {                    /*         0 */
    HEADER;                         /* +4      4 */
    int              following;     /* +4      8 */
    T_PTD            pattern;       /* +4     12 */
    T_PTD            variable;      /* +4     16 */
} ST_PATTERN;

typedef struct {                 /* BAR  Patternmatch */
    HEADER;
#if  ODDSEX
    COUNT          case_type;
    COUNT           special;
#else 
    COUNT           special;
    COUNT          case_type;
#endif
    int                nwhen;
    PTR_HEAPELEM       ptse;
} ST_SWITCH;

typedef struct {                /* BAR  Patternmatch */
    HEADER;
    PTR_HEAPELEM     code;
    PTR_HEAPELEM     guard;
    PTR_HEAPELEM     body;
} ST_MATCH;

typedef struct {              /*BAR  Patternmatch*/
    HEADER;
    PTR_HEAPELEM  guard_body; /*act_nom steht auf 0, falls es sich*/
    int           act_nomat;  /*um otherwise Ausdruck handelt*/
    T_PTD     ptsdes;
} ST_NOMAT;

typedef struct {                                                       /* CS */
    HEADER;
    int            nfuncs;                                             /* CS */
    PTR_HEAPELEM namelist;                                             /* CS */
    PTR_HEAPELEM     ptdv;                                             /* CS */
} ST_LREC;                                                             /* CS */

typedef struct {                                                       /* CS */
    HEADER;
    int             index;                                             /* CS */
    T_PTD            ptdd;                                             /* CS */
    T_PTD            ptf ;                  /* Verweis auf func_def */ /* CS */
} ST_LREC_IND;                                                         /* CS */

typedef struct {                                                       /* CS */
    HEADER;
    int             nargs;                                             /* CS */
    T_PTD            ptdd;                                             /* CS */
    PTR_HEAPELEM     ptdv;                                             /* CS */
} ST_LREC_ARGS;                                            /* CS */

typedef struct {                                           /* AE */
    HEADER;
   T_PTD       rueck;                                      /* AE */
   char        freespace[4];                               /* AE */
   T_PTD       vor;                                        /* AE */
} ST_FREE;

typedef struct {
    HEADER;
    long         heaplen;
    PTR_HEAPELEM heapdef;
    char         freespace[4];
 } ST_SWAP_HEAP;
/* dg */ typedef struct {             /*    --- combinator ---    */
/* dg */     HEADER;
#if  ODDSEX
/* dg */     COUNT         nargs;     /* anzahl der parameter     */
/* dg */     COUNT         args;      /* analogie zu closures     */
#else 
/* dg */     COUNT         args;      /* analogie zu closures     */
/* dg */     COUNT         nargs;     /* anzahl der parameter     */
#endif
/* dg */     INSTR        *ptc;       /* zeiger auf codesequenz   */
/* dg */     T_PTD         ptd;       /* zeiger auf letrecdescr.  */
/* dg */ } ST_COMB;
/* dg */
/* dg */ typedef struct {             /*     --- closure ---      */
/* dg */     HEADER;
#if  ODDSEX
/* dg */     COUNT         nargs;     /* benoetigte argumente     */
/* dg */     COUNT         args;      /* vorhandene argumente     */
#else 
/* dg */     COUNT         args;      /* vorhandene argumente     */
/* dg */     COUNT         nargs;     /* benoetigte argumente     */
#endif
/* dg */     PTR_HEAPELEM  pta;       /* argumentvektor           */
/* dg */     int           ftype;     /* fkt.typ COMB,PRIM,COND   */
/* dg */ } ST_CLOS;
/* dg */
/* dg */ typedef struct {             /*    --- conditional ---   */
/* dg */     HEADER;
#if  ODDSEX
/* dg */     COUNT         nargs;     /* benoetigte argumente     */
/* dg */     COUNT         args;      /* flag ob arg freigeb od nicht */
#else 
/* dg */     COUNT         args;      /* flag ob arg freigeb od nicht */
/* dg */     COUNT         nargs;     /* benoetigte argumente     */
#endif
/* dg */     INSTR        *ptc;       /* zeiger auf code          */
/* dg */     T_PTD         ptd;       /* zeiger auf cond-descr    */
/* dg */ } ST_CONDI;
/* dg */
/* dg */ typedef struct {             /*    --- case       ---    */
/* dg */     HEADER;
#if  ODDSEX
/* dg */     COUNT         nargs;     /* anzahl der parameter     */
/* dg */     COUNT         args;      /* analogie zu closures     */
#else 
/* dg */     COUNT         args;      /* analogie zu closures     */
/* dg */     COUNT         nargs;     /* anzahl der parameter     */
#endif
/* dg */     INSTR        *ptc;       /* zeiger auf codesequenz   */
/* dg */     T_PTD         ptd;       /* zeiger auf casedescr.    */
/* dg */ } ST_CASE;
/* dg */
/* dg */ typedef struct {             /*     --- universal ---   */
/* dg */     HEADER;
#if  ODDSEX
/* dg */     COUNT         nargs;     /* 16 bit                  */
/* dg */     COUNT         args;      /* 16 bit                  */
#else 
/* dg */     COUNT         args;      /* 16 bit                  */
/* dg */     COUNT         nargs;     /* 16 bit                  */
#endif
/* dg */     INSTR        *ptc;       /* 32 bit                  */
/* dg */     T_PTD         ptd;       /* 32 bit                  */
/* dg */ } ST_UNIV;

/* dg */ /* 02.05.91 cons-listen */
/* dg */ typedef struct {
/* dg */     HEADER;
/* dg */     T_PTD              hd;          /* head */
/* dg */     T_PTD              tl;          /*  tail */
/* dg */     char     freespace[4];
/* dg */ } ST_CONS;



typedef struct T_descriptor {
/*  struct T_descriptor  *heap_chain;                                 */
/*  Der Heapchainverweis wird, falls der Descriptor frei ist, auf die */
/*  letzten vier Byte des Descriptors gelegt.   (siehe rheap.c)       */

    union { /* die moeglichen Deskriptoren: */
            ST_SCALAR     sc;
            ST_DIGIT      di;
            ST_LIST       li;
            ST_MVT        mvt; /* matrix / vector / transposed vector */
            ST_VAR        va;  /* die Expressiondescriptoren: */
            ST_COND       co;
            ST_EXPR       ex;
            ST_FUNC       fu;
            ST_NAME       na;
            ST_SWITCH     sw;  /* BAR  Patternmatch */
            ST_MATCH      ma;  /* BAR  Patternmatch */
            ST_NOMAT      no;  /* BAR  Patternmatch */
            ST_LREC       lr;                              /* CS */
            ST_LREC_IND   ri;                              /* CS */
            ST_LREC_ARGS  la;                              /* CS */
            ST_SWAP_HEAP  sh;
/* dg */             ST_COMB       cmb;     /* combinator, code  */
/* dg */             ST_CLOS       cls;     /* closure */
/* dg */             ST_CONDI      cnd;     /* conditional, code */
/* dg */             ST_CASE       cse;     /* case */
/* dg */             ST_UNIV       uni;     /* universal */
/* dg */             ST_CONS       cns;     /* universal */
/* uh */             ST_ZF         zf;      /* ZF-Expressions */
/* uh */             ST_ZFCODE     zfcode;  /* ZF-Codedescriptor */
/* uh */             ST_SELECTION  sel;     /* Pattern */
/* uh */             ST_CLAUSE     clause;  /* Pattern */
/* uh */             ST_PATTERN    pat;     /* Pattern */
            ST_FREE       fr; /* Freier Descriptor */
    }      u;
} *PTR_DESCRIPTOR, T_DESCRIPTOR;

/*---------------------------------------------------------------------------
 * allgemeine Makros
 *--------------------------------------------------------------------------
 */

#if    SCAVENGE

#define INC_REFCNT(p)          while(0)
#define DEC_REFCNT(p)          while(0)
#define DEC_REFCNT_SCALAR(p)   while(0)
#define T_INC_REFCNT(p)        while(0)
#define T_DEC_REFCNT(p)        while(0)

/* vorsichtshalber die heaproutinen umdefinieren */
#define newdesc                get_desc
#define newheap                get_heap2
#define newbuff                get_buff

#define GET_DESC(p)            p  = (T_PTD)get_desc()
#define GET_HEAP(n,p)          *((int *)(p)) = (int)get_heap(n)
#define NEWDESC(p)             GET_DESC(p)
#define MAKEDESC(p,ref,cl,ty)  (GET_DESC(p)), DESC_MASK(p,ref,cl,ty)
/* kruecke, um zuweisungen an integer-variablen zu erm"oglichen */ 
#define MAKE_DESC(p,ref,cl,ty) (p = (int)get_desc()), DESC_MASK(p,ref,cl,ty)

#else /* !SCAVENGE */

#if DEBUG
#define INC_REFCNT(id)         test_inc_refcnt(id,__FILE__,0)
#define DEC_REFCNT(id)         test_dec_refcnt(id,__FILE__,0)
#define DEC_REFCNT_SCALAR(id)  DEC_REFCNT(id)
#else
#define INC_REFCNT(id)         (++((id)->u.sc.ref_count))
/*********** HIER KLEINE AENDERUNG  **********/
#define DEC_REFCNT(id)         if (--((id)->u.sc.ref_count) != 0) ; else \
                                 (*dealloc_tab[(id)->u.sc.class])(id)
/* #define DEC_REFCNT(id)         if (--((id)->u.sc.ref_count) == 0) \
                                 (*dealloc_tab[(id)->u.sc.class])(id); else
    das war das alte RS 8.12.1992 */
#if (nCUBE && D_MESS && D_MHEAP)
#define DEC_REFCNT_SCALAR(id)  DEC_REFCNT(id)
/* old: do {if (--((id)->u.sc.ref_count) == 0) \
        (*d_m_freedesc)(id) ;} while(0)  used only in rvalfunc.c */
#else
#define DEC_REFCNT_SCALAR(id)  DEC_REFCNT(id)
/* old: do {if (--((id)->u.sc.ref_count) == 0) \
        freedesc(id) ;} while(0)  used only in rvalfunc.c */
#endif
#endif /* DEBUG    auskommentiert RS 30.11.1992 */

#define T_INC_REFCNT(p)        do {if (T_POINTER(p)) { INC_REFCNT(p); }} while(0)
#define T_DEC_REFCNT(p)        do {if (T_POINTER(p)) { DEC_REFCNT(p); }} while(0)
#if (nCUBE && D_MESS && D_MHEAP)
#define GET_DESC(p)            do {if ((p = (T_PTD)(*d_m_newdesc)()) == 0) \
                                 post_mortem("heap overflow (newdesc)");} while(0)
#else
#define GET_DESC(p)            do {if ((p = (T_PTD)newdesc()) == 0) \
                                 post_mortem("heap overflow (newdesc)");} while(0)
#endif
#if (nCUBE && D_MESS && D_MHEAP)
#define GET_HEAP(n,p)          do {if (((*d_m_newheap)(n,p)) == 0) \
                                 post_mortem("heap overflow (newheap)");} \
                               while(0)
#else
#define GET_HEAP(n,p)          do {if ((newheap(n,p)) == 0) \
                                 post_mortem("heap overflow (newheap)");} \
                               while(0)
#endif
#define NEWDESC(p)             GET_DESC(p)
#if (nCUBE && D_MESS && D_MHEAP)
#define MAKEDESC(p,ref,cl,ty)  if ((p = (T_PTD)(*d_m_newdesc)()) == 0) \
                                 post_mortem("heap overflow (newdesc)"); \
                               else DESC_MASK(p,ref,cl,ty)
#else
#define MAKEDESC(p,ref,cl,ty)  if ((p = (T_PTD)newdesc()) == 0) \
                                 post_mortem("heap overflow (newdesc)"); \
                               else DESC_MASK(p,ref,cl,ty)
#endif
/*uh 17.6.91 */
/* Der PCS C-Compiler mag keine Type Casts auf der linken Seite von Zuweisungen! */
/* Aus diesem Grund muss im Makro MAKEDESC auf der rechten Seite ein CAST */
/* angebracht werden. Das Makro MAKE_DESC erledigt dies. */

#if (nCUBE && D_MESS && D_MHEAP)
#define MAKE_DESC(p,ref,cl,ty) if ((p = (int)(*d_m_newdesc)()) == 0) \
                                 post_mortem("heap overflow (newdesc)"); \
                               else DESC_MASK(p,ref,cl,ty)
#else
#define MAKE_DESC(p,ref,cl,ty) if ((p = (int)newdesc()) == 0) \
                                 post_mortem("heap overflow (newdesc)"); \
                               else DESC_MASK(p,ref,cl,ty)
#endif

#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */

#if DEBUG
#define INC_IDCNT              inc_idcnt() /* increment indirect descriptor counter */
#define DEC_IDCNT              dec_idcnt() /* decrement indirect descriptor counter */
#else
#define INC_IDCNT              idcnt++;    /* increment indirect descriptor counter */
#define DEC_IDCNT              idcnt--;    /* decrement indirect descriptor counter */
#endif /* DEBUG   auskommentiert RS 30.11.1992 */


/*********************************************************************/
/*                                                                   */
/*                 P R A E F I X  -  M A K R O S                     */
/*                                                                   */
/*********************************************************************/
/*  SJ-ID                                                            */
/*         p = Pointer auf entsprechenden Deskriptor                 */
/*         x = gewuenschte Komponente                                */
/*         L = sollte auf der linken Seite einer Zuweisung stehen    */
/*             Zugriffe haben zuweisenden Charakter                  */
/*         R = sollte auf der rechten Seite einer Zuweisung stehen   */
/*             im wesentlichen lesende Zugriffe                      */
/*         A = Adresse des Deskriptors                               */
/*********************************************************************/

/* #if DEBUG && !SCAVENGE */
#if DEBUG

/*********************************************************************/
/* Descriptor Level                                                  */
/*********************************************************************/

/*  Die Or- und Shift-operationen aller DESC_MASK Makros werden zur    */
/*  Compilezeit durchgefuehrt, da die Argumente des Makros fast immer  */
/*  Konstanten sein werden.                                            */

#define LI_DESC_MASK(p,ref,cl,ty) \
  MEAS_CALL(MS_DESCREF)          \
  (*(int*) (p)) = (((ref) <<16) | ((int)(cl) << 8) | (int)(ty)),   \
  (*(((int*)(p))+1) = 0),         \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von Listen-Deskriptoren      */
/* Das Makro initialisiert                    -------------------      */
/*              das erste Wort  (refcount / class / type)              */
/*              das zweite Wort (special / dim) und                    */
/*              das dritte Wort (ptdd) des Listen-Deskriptors.         */

#define MVT_DESC_MASK(p,ref,cl,ty) \
  MEAS_CALL(MS_DESCREF)           \
  (*(int*) (p)) = (((ref) <<16) | ((int)(cl) << 8) | (int)(ty)),   \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von                          */
/*             Matrizen-, Vektor- und TVektor-Deskriptoren             */
/*             --------   ------      --------------------             */
/* Intialisierung des ersten und dritten Wortes der Deskriptoren       */

#define DESC_MASK(p,ref,cl,ty) \
  MEAS_CALL(MS_DESCREF)       \
  (*(int*) (p)) = (((ref) <<16) | ((int)(cl) << 8) | (int)(ty)),   \
  (*(((int*)(p))+1) = 0)
/* Benutzung : Initialisierungen aller sonstigen Deskriptoren          */
/* Das Makro DESC_MASK initialisiert                                   */
/*           das erste Wort (refcount / class / type)  und             */
/*           das zweite Worte des Deskriptors.                         */

/* DIRTY TRICK use first word of scalar desc .... */
#define L_DESC(p,x) MEAS_CALL(MS_DESCREF)(p).u.sc.x
#define R_DESC(p,x) (MEAS_CALL(MS_DESCREF)(p).u.sc.x)

/*********************************************************************/
/* Scalar Descriptor  - SCALAR                                       */
/*********************************************************************/

#define L_SCALAR(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != C_SCALAR    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_SCALAR"), 0)\
   : (MEAS_CALL(MS_DESCREF) 0)), (p).u.sc.x

#define R_SCALAR(p,x) \
 (( (p).u.sc.class   != C_SCALAR    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_SCALAR"), \
   (p).u.sc.x) /* TB, 3.11.1992 */ \
   : (MEAS_CALL(MS_DESCREF) \
  (p).u.sc.x))

#define A_SCALAR(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.sc.x)

/*********************************************************************/
/* Digit Descriptor   - DIGIT                                        */
/*********************************************************************/

#define L_DIGIT(p,x)  /* TB, 3.11.1992 */\
  (( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != C_DIGIT     ) ? \
  ( DescDump(stdout,&(p),0), \
   post_mortem("inconsistent descriptor entries L_DIGIT"), 0) \
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.di.x

#define R_DIGIT(p,x) \
 (( (p).u.sc.type    != TY_DIGIT   || \
    (p).u.sc.class   != C_DIGIT     ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_DIGIT"), \
   (p).u.di.x)  /* TB, 3.11.1992 */\
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.di.x))

#define A_DIGIT(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.di.x)

/*********************************************************************/
/* List Descriptor    - LIST                                         */
/*********************************************************************/

#define L_LIST(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != C_LIST    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_LIST"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.li.x

#define R_LIST(p,x) \
 (((((p).u.sc.type   != TY_UNDEF) && \
   ((p).u.sc.type    != TY_STRING))|| \
   ((p).u.sc.class   != C_LIST)   ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_LIST"), \
   (p).u.li.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.li.x))

#define A_LIST(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.li.x)

/*********************************************************************/
/* Matrix / Vektor / TVector  Descriptor  ----- MVT                  */
/*********************************************************************/

#define L_MVT(p,x,class1) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != (DESC_CLASS)class1      ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_MVT"), 0)\
   : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.mvt.x

#define R_MVT(p,x,class1) \
 (( (p).u.sc.class   != (DESC_CLASS)class1      ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_MVT"), \
   (p).u.mvt.x) /* TB, 3.11.1992 */ \
   : (MEAS_CALL(MS_DESCREF) \
  (p).u.mvt.x))

#define A_MVT(p,x,class1) \
 (( (p).u.sc.class   != (DESC_CLASS)class1      ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries A_MVT"), \
   &(p).u.mvt.x) /* TB, 3.11.1992 */ \
   : (MEAS_CALL(MS_DESCREF) \
      &(p).u.mvt.x)), &(p).u.mvt.x

/*********************************************************************/
/* Expression Descriptor - EXPR                                      */
/*********************************************************************/

#define L_EXPR(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_EXPR    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_EXPR"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.ex.x

#define R_EXPR(p,x) \
 (( (p).u.sc.type    != TY_EXPR  || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_EXPR"), \
   (p).u.ex.x) /* TB, 3.11. 1992 */\
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.ex.x))

#define A_EXPR(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.ex.x)

/*********************************************************************/
/* Function Descriptor   - FUNC                                      */
/*********************************************************************/

#define L_FUNC(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (((p).u.sc.type != TY_SUB) && \
    ((p).u.sc.type != TY_REC) && \
    ((p).u.sc.type != TY_ZF)) || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_FUNC"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.fu.x

#define R_FUNC(p,x) \
 (( (((p).u.sc.type != TY_SUB) && \
     ((p).u.sc.type != TY_REC) && \
     ((p).u.sc.type != TY_ZF)) || \
      (p).u.sc.class   != C_EXPRESSION ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_FUNC"), \
   (p).u.fu.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.fu.x))

#define A_FUNC(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.fu.x)

#define L_ZF L_FUNC
#define R_ZF R_FUNC
#define A_ZF A_FUNC

/***************************************************************************/
/*                 List Comprehension Code-Descriptoren                    */
/***************************************************************************/

#define L_ZFCODE(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type != TY_ZFCODE   || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_FUNC"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.zfcode.x

#define R_ZFCODE(p,x) \
 (( (p).u.sc.type != TY_ZFCODE  || \
    (p).u.sc.class   != C_EXPRESSION ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_FUNC"), \
   (p).u.zfcode.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.zfcode.x))

#define A_ZFCODE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.zfcode.x)

/*********************************************************************/
/* PATTERN MATCHING DESCRIPTORN                                   UH */
/*********************************************************************/

#define L_SELECTION(p,x)/* TB, 3.11.1992 */ \
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_MATCHING || \
    (p).u.sc.type   != TY_SELECTION ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_SELECTION"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.sel.x

#define R_SELECTION(p,x) \
  ( ( (p).u.sc.class   != C_MATCHING || \
      (p).u.sc.type   != TY_SELECTION ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_SELECTION"), \
     (p).u.sel.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.sel.x ))

#define A_SELECTION(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.sel.x )

#define L_CLAUSE(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_MATCHING || \
    (p).u.sc.type    != TY_CLAUSE ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_CLAUSE"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.clause.x

#define R_CLAUSE(p,x) \
  ( ( (p).u.sc.class   != C_MATCHING || \
      (p).u.sc.type    != TY_CLAUSE ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_CLAUSE"), \
     (p).u.clause.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.clause.x ))

#define A_CLAUSE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.clause.x )

#define L_PATTERN(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_PATTERN ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_PATTERN"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.pat.x

#define R_PATTERN(p,x) \
  ( ( (p).u.sc.class   != C_PATTERN ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_PATTERN"), \
     (p).u.pat.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.pat.x ))

#define A_PATTERN(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.pat.x )

/*********************************************************************/
/* Conditional Descriptor - COND                                     */
/*********************************************************************/

#define L_COND(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_COND || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_COND"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.co.x

#define R_COND(p,x) \
 (( (p).u.sc.type    != TY_COND || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_COND"), \
   (p).u.co.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.co.x))

#define A_COND(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.co.x)

/*********************************************************************/
/* Variable Descriptor - VAR                                         */
/*********************************************************************/

#define L_VAR(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_VAR     || \
    (p).u.sc.class   != C_CONSTANT   )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_VAR"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.va.x

#define R_VAR(p,x) \
  (( (p).u.sc.type    != TY_VAR     || \
    (p).u.sc.class   != C_CONSTANT   )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_VAR"), \
   (p).u.va.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.va.x))

#define A_VAR(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.va.x)

/*********************************************************************/
/* Name Descriptor - NAME                                            */
/*********************************************************************/

#define L_NAME(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_NAME    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_NAME"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.na.x

#define R_NAME(p,x) \
 (( (p).u.sc.type    != TY_NAME    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_NAME"), \
   (p).u.na.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.na.x))

#define A_NAME(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.na.x)

/*********************************************************************/
/* Switch Descriptor - SWITCH                             BAR        */
/*********************************************************************/

#define L_SWITCH(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_SWITCH    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_SWITCH"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.sw.x

#define R_SWITCH(p,x) \
 (( (p).u.sc.type    != TY_SWITCH    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_SWITCH"), \
   (p).u.sw.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.sw.x))

#define A_SWITCH(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.sw.x)

/*********************************************************************/
/* Match Descriptor - MATCH                               BAR        */
/*********************************************************************/

#define L_MATCH(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_MATCH    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_MATCH"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.ma.x

#define R_MATCH(p,x) \
 (( (p).u.sc.type    != TY_MATCH    || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_MATCH"), \
   (p).u.ma.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.ma.x))

#define A_MATCH(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.ma.x)

/*********************************************************************/
/* Nomat(ch) Descriptor - NOMAT                           BAR        */
/*********************************************************************/

#define L_NOMAT(p,x)/* TB, 3.11.1992 */ \
  (( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_NOMAT     || \
    (p).u.sc.class   != C_EXPRESSION   )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_NOMAT"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),   (p).u.no.x

#define R_NOMAT(p,x) \
 (( (p).u.sc.type    != TY_NOMAT     || \
    (p).u.sc.class   != C_EXPRESSION   )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_NOMAT"), \
   (p).u.no.x) /* TB, 4.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.no.x))

#define A_NOMAT(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.no.x)

/*********************************************************************/
/* Expression Descriptor - LETREC                            */ /* CS */
/*********************************************************************/

#define L_LREC(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    ((p).u.sc.type != TY_LREC)   || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries _LREC"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.lr.x

#define R_LREC(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  (p).u.lr.x)

#define A_LREC(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.lr.x)

/*********************************************************************/
/* Expression Descriptor - LETREC_IND                         *//* CS */
/*********************************************************************/

#define L_LREC_IND(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    ((p).u.sc.type != TY_LREC_IND)   || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries _LREC_IND"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.ri.x

#define R_LREC_IND(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  (p).u.ri.x)

#define A_LREC_IND(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.ri.x)

/*********************************************************************/
/* Expression Descriptor - LETREC_ARGS                        *//* CS */
/*********************************************************************/

#define L_LREC_ARGS(p,x)/* TB, 3.11.1992 */ \
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    ((p).u.sc.type != TY_LREC_ARGS)   || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries _LREC_ARGS"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.la.x

#define R_LREC_ARGS(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  (p).u.la.x)

#define A_LREC_ARGS(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.la.x)

/********************************************************************/
/* Freizellen Descriptor FREE                               *//* AE */
/********************************************************************/

#define L_FREE(p,x) /* TB, 3.11.1992 */\
  ((!(isdesc(&(p))))  ? \
  ( DescDump (stdout,(PTR_DESCRIPTOR) &(p),0) , \
  post_mortem("not a pointer to descriptor L_FREE") ,0) \
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.fr.x

#define R_FREE(p,x) \
  (MEAS_CALL(MS_DESCREF) \
  (p).u.fr.x)

#define A_FREE(p,x) \
  &(p).u.fr.x

/********************************************************************/
/* swap descriptor                                          *//* AE */
/********************************************************************/

#define L_SWAPHEAP(p,x) /* TB, 3.11.1992 */\
  (((p).u.sc.ref_count >65000 || \
   (p).u.sc.ref_count == 0   || \
   ((p).u.sc.class != C_SWAPHEAP )) ? \
   (DescDump (stdout,&(p),0) , \
    post_mortem("inconsistent descriptor entries L_SWAPHEAP"), 0)\
   :(MEAS_CALL(MS_DESCREF) 0)), (p).u.sh.x

#define R_SWAPHEAP(p,x) \
  (MEAS_CALL(MS_DESCREF) \
   (p).u.sh.x)

#define A_SWAPHEAP(p,x) \
  (MEAS_CALL(MS_DESCREF) \
  &(p).u.sh.x)

/*********************************************************************/
/* Combinator   Descriptor - COMB                               * dg */
/*********************************************************************/

#define L_COMB(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_FUNC   || \
    (p).u.sc.type    != TY_COMB   ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_COMB"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.cmb.x

#define R_COMB(p,x) \
  ( ( (p).u.sc.class   != C_FUNC  || \
      (p).u.sc.type    != TY_COMB  ) \
  ? ( DescDump(stdout,&(p),0), \
    post_mortem("inconsistent descriptor entries R_COMB"), \
    (p).u.cmb.x) /* TB, 3.11.1992 */ \
  : ( MEAS_CALL(MS_DESCREF) \
    (p).u.cmb.x ))

#define A_COMB(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.cmb.x )

/*********************************************************************/
/* Closure      Descriptor - CLOS   (dg)                       * dg */
/*********************************************************************/

#define L_CLOS(p,x)/* TB, 3.11.1992 */ \
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_FUNC   || \
    (p).u.sc.type    != TY_CLOS   ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_CLOS"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.cls.x

#define R_CLOS(p,x) \
  ( ( (p).u.sc.class   != C_FUNC  || \
      (p).u.sc.type    != TY_CLOS  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_CLOS"), \
     (p).u.cls.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.cls.x ) )

#define A_CLOS(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.cls.x )

/*********************************************************************/
/* Conditional  Descriptor - CONDI   (dg)                       * dg */
/*********************************************************************/

#define L_CONDI(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_FUNC   || \
    (p).u.sc.type    != TY_CONDI  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_CONDI"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.cnd.x

#define R_CONDI(p,x) \
  ( ( (p).u.sc.class   != C_FUNC  || \
      (p).u.sc.type    != TY_CONDI ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_CONDI"), \
     (p).u.cnd.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.cnd.x ))

#define A_CONDI(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.cnd.x )

/*********************************************************************/
/* Case  Descriptor - CASE    (dg)                              * dg */
/*********************************************************************/

#define L_CASE(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.class   != C_FUNC   )  \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_CASE"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.cse.x

#define R_CASE(p,x) \
  ( ( (p).u.sc.class   != C_FUNC  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_CASE"), \
     (p).u.cse.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.cse.x ))

#define A_CASE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.cse.x )

/*********************************************************************/
/* Universeller Descriptor - UNIV    (dg)                       * dg */
/*********************************************************************/

#define L_UNIV(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000 || \
    (p).u.sc.ref_count == 0 ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_UNIV"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)),  (p).u.uni.x

#define R_UNIV(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    (p).u.uni.x )

#define A_UNIV(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.uni.x)

/*********************************************************************/
/* Cons         Descriptor - CONS    (dg)                       * dg */
/*********************************************************************/

#define L_CONS(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000 || \
    (p).u.sc.ref_count == 0 ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_CONS"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.cns.x

#define R_CONS(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    (p).u.cns.x )

#define A_CONS(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.cns.x)


      /* end DEBUG      */
#else
      /* start NO-DEBUG */

/*********************************************************************/
/* Descriptor Level                                                  */
/*********************************************************************/

/*  Die Or- und Shift-operationen aller DESC_MASK Makros werden zur        */
/*  Compilezeit durchgefuehrt, da die Argumente des Makros fast immer      */
/*  Konstanten sein werden.                                                */

#define LI_DESC_MASK(p,ref,cl,ty)             \
  (*(int*) (p)) = ((ref) <<16) | ((int)(cl) << 8) | (int)(ty) , \
  (*(((int*)(p))+1) = 0),                       \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von Listen-Deskriptoren          */
/* Das Makro initialisiert                    -------------------          */
/*              das erste Wort  (refcount / class / type)                  */
/*              das zweite Wort (special / dim) und                        */
/*              das dritte Wort (ptdd) des Listen-Deskriptors.             */

#define MVT_DESC_MASK(p,ref,cl,ty)            \
  (*(int*) (p)) = ((ref) <<16) | ((int)(cl) << 8) | (int)(ty) , \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von                              */
/*             Matrizen-, Vektor- und TVektor-Deskriptoren                 */
/*             --------   ------      --------------------                 */
/* Intialisierung des ersten und dritten Wortes der Deskriptoren           */

#define DESC_MASK(p,ref,cl,ty)                \
  (*(int*) (p)) = ((ref) <<16) | ((int)(cl) << 8) | (int)(ty) , \
  (*(((int*)(p))+1) = 0)
/* Benutzung : Initialisierungen aller sonstigen Deskriptoren              */
/* Das Makro DESC_MASK initialisiert                                       */
/*           das erste Wort (refcount / class / type)  und                 */
/*           das zweite Worte des Deskriptors.                             */

/* DIRTY TRICK use first word of scalar desc .... */
#define L_DESC(p,x) (p).u.sc.x
#define R_DESC(p,x) (p).u.sc.x

/*********************************************************************/
/* Scalar Descriptor  - SCALAR                                       */
/*********************************************************************/

#define L_SCALAR(p,x) (p).u.sc.x
#define R_SCALAR(p,x) (p).u.sc.x
#define A_SCALAR(p,x) &(p).u.sc.x

/*********************************************************************/
/* Digit Descriptor  - DIGIT                                        */
/*********************************************************************/

#define L_DIGIT(p,x) (p).u.di.x
#define R_DIGIT(p,x) (p).u.di.x
#define A_DIGIT(p,x) &(p).u.di.x

/*********************************************************************/
/* List Descriptor                                                   */
/*********************************************************************/

#define L_LIST(p,x) (p).u.li.x
#define R_LIST(p,x) (p).u.li.x
#define A_LIST(p,x) &(p).u.li.x

/*********************************************************************/
/* Matrix / Vektor / TVector  Descriptor  ----- MVT                  */
/*********************************************************************/

#define L_MVT(p,x,class1) (p).u.mvt.x
#define R_MVT(p,x,class1) (p).u.mvt.x
#define A_MVT(p,x,class1) &(p).u.mvt.x

/*********************************************************************/
/* Expression Descriptor - EXPR                                      */
/*********************************************************************/

#define L_EXPR(p,x) (p).u.ex.x
#define R_EXPR(p,x) (p).u.ex.x
#define A_EXPR(p,x) &(p).u.ex.x

/*********************************************************************/
/* Function Descriptor - FUNC                                        */
/*********************************************************************/

#define L_FUNC(p,x) (p).u.fu.x
#define R_FUNC(p,x) (p).u.fu.x
#define A_FUNC(p,x) &(p).u.fu.x

/*********************************************************************/
/* ZF-Descriptor - ZF                                                */
/*********************************************************************/

#define L_ZF(p,x) (p).u.fu.x
#define R_ZF(p,x) (p).u.fu.x
#define A_ZF(p,x) &(p).u.fu.x
#define L_ZFCODE(p,x) (p).u.zfcode.x
#define R_ZFCODE(p,x) (p).u.zfcode.x
#define A_ZFCODE(p,x) &(p).u.zfcode.x

/*********************************************************************/
/* PATTERN MATCHING DESCRIPTORN                                   UH */
/*********************************************************************/

#define L_SELECTION(p,x) (p).u.sel.x
#define R_SELECTION(p,x) (p).u.sel.x
#define A_SELECTION(p,x) &(p).u.sel.x

#define L_CLAUSE(p,x) (p).u.clause.x
#define R_CLAUSE(p,x) (p).u.clause.x
#define A_CLAUSE(p,x) &(p).u.clause.x

#define L_PATTERN(p,x) (p).u.pat.x
#define R_PATTERN(p,x) (p).u.pat.x
#define A_PATTERN(p,x) &(p).u.pat.x



/*********************************************************************/
/* Conditional Descriptor - COND                                     */
/*********************************************************************/

#define L_COND(p,x) (p).u.co.x
#define R_COND(p,x) (p).u.co.x
#define A_COND(p,x) &(p).u.co.x

/*********************************************************************/
/* Variable Descriptor - VAR                                         */
/*********************************************************************/

#define L_VAR(p,x) (p).u.va.x
#define R_VAR(p,x) (p).u.va.x
#define A_VAR(p,x) &(p).u.va.x

/*********************************************************************/
/* Name Descriptor - NAME                                            */
/*********************************************************************/

#define L_NAME(p,x) (p).u.na.x
#define R_NAME(p,x) (p).u.na.x
#define A_NAME(p,x) &(p).u.na.x

/*********************************************************************/
/* Switch Descriptor - SWITCH                             BAR        */
/*********************************************************************/

#define L_SWITCH(p,x) (p).u.sw.x
#define R_SWITCH(p,x) (p).u.sw.x
#define A_SWITCH(p,x) &(p).u.sw.x

/*********************************************************************/
/* Match Descriptor - MATCH                               BAR        */
/*********************************************************************/

#define L_MATCH(p,x) (p).u.ma.x
#define R_MATCH(p,x) (p).u.ma.x
#define A_MATCH(p,x) &(p).u.ma.x

/*********************************************************************/
/* Nomat(ch) Descriptor - NOMAT                           BAR        */
/*********************************************************************/

#define L_NOMAT(p,x) (p).u.no.x
#define R_NOMAT(p,x) (p).u.no.x
#define A_NOMAT(p,x) &(p).u.no.x

/*********************************************************************/
/* Expression Descriptor - LETREC                             *//* CS */
/*********************************************************************/

#define L_LREC(p,x) (p).u.lr.x
#define R_LREC(p,x) (p).u.lr.x
#define A_LREC(p,x) &(p).u.lr.x

/*********************************************************************/
/* Expression Descriptor - LETREC_IND                         *//* CS */
/*********************************************************************/

#define L_LREC_IND(p,x) (p).u.ri.x
#define R_LREC_IND(p,x) (p).u.ri.x
#define A_LREC_IND(p,x) &(p).u.ri.x

/*********************************************************************/
/* Expression Descriptor - LETREC_ARGS                        *//* CS */
/*********************************************************************/

#define L_LREC_ARGS(p,x) (p).u.la.x
#define R_LREC_ARGS(p,x) (p).u.la.x
#define A_LREC_ARGS(p,x) &(p).u.la.x

/********************************************************************/
/* Freizellen Descriptor FREE                               *//* AE */
/********************************************************************/

#define L_FREE(p,x) (p).u.fr.x
#define R_FREE(p,x) (p).u.fr.x
#define A_FREE(p,x) &(p).u.fr.x

/********************************************************************/
/* swap descriptor                                          *//* AE */
/********************************************************************/

#define L_SWAPHEAP(p,x) (p).u.sh.x
#define R_SWAPHEAP(p,x) (p).u.sh.x
#define A_SWAPHEAP(p,x) &(p).u.sh.x

/***********************************************************/
/* Combinator   Descriptor - COMB   (dg)              * dg */
/***********************************************************/

#define L_COMB(p,x) (p).u.cmb.x
#define R_COMB(p,x) (p).u.cmb.x
#define A_COMB(p,x) &(p).u.cmb.x

/***********************************************************/
/* Closure      Descriptor - CLOS   (dg)              * dg */
/***********************************************************/

#define L_CLOS(p,x) (p).u.cls.x
#define R_CLOS(p,x) (p).u.cls.x
#define A_CLOS(p,x) &(p).u.cls.x

/***********************************************************/
/* Conditional  Descriptor - CONDI   (dg)             * dg */
/***********************************************************/

#define L_CONDI(p,x) (p).u.cnd.x
#define R_CONDI(p,x) (p).u.cnd.x
#define A_CONDI(p,x) &(p).u.cnd.x

/***********************************************************/
/* Case  Descriptor - CASE   (dg)                     * dg */
/***********************************************************/

#define L_CASE(p,x) (p).u.cse.x
#define R_CASE(p,x) (p).u.cse.x
#define A_CASE(p,x) &(p).u.cse.x

/***********************************************************/
/* Universeller Descriptor - UNIV    (dg)             * dg */
/***********************************************************/

#define L_UNIV(p,x) (p).u.uni.x
#define R_UNIV(p,x) (p).u.uni.x
#define A_UNIV(p,x) &(p).u.uni.x

/***********************************************************/
/* Cons         Descriptor - CONS    (dg)             * dg */
/***********************************************************/

#define L_CONS(p,x) (p).u.cns.x
#define R_CONS(p,x) (p).u.cns.x
#define A_CONS(p,x) &(p).u.cns.x


#endif     /* DEBUG  , NODEBUG  */

#endif /* RHEAPTY_H */
