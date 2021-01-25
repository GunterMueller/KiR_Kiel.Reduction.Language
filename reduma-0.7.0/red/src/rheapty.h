/* $Log: rheapty.h,v $
 * Revision 1.27  1996/03/29  14:35:23  rs
 * some work on the famous MAH-tilde-BUG
 *
 * Revision 1.26  1996/02/29  16:32:56  rs
 * one comment added ;-)
 *
 * Revision 1.25  1995/10/11  15:17:58  stt
 * In L_ATOMTYPE class check corrected.
 *
 * Revision 1.24  1995/10/11  13:56:49  stt
 * In R_ATOMTYPE unit member corrected.
 *
 * Revision 1.23  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.22  1995/05/22  09:23:25  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.21  1995/03/28  14:24:49  cr
 * redefined C_FRAME,C_INTER for use with dealloc_tab
 *
 * Revision 1.20  1995/03/23  13:23:12  cr
 * minor change in ST_SLOT corrected
 *
 * Revision 1.19  1995/03/22  18:55:41  cr
 * minor change in ST_SLOT
 *
 * Revision 1.18  1995/03/15  16:25:55  cr
 * minor corrections
 *
 * Revision 1.17  1995/03/15  15:58:24  cr
 * minor corrections
 *
 * Revision 1.16  1995/03/09  14:38:18  cr
 * Added descriptors for interactions, frames and slots
 *
 * Revision 1.15  1994/11/25  12:40:53  car
 * ST_COMB becomes first component of union
 *
 * Revision 1.14  1994/07/20  15:04:55  car
 * combtype changed
 *
 * Revision 1.13  1994/06/22  07:49:51  car
 * ST_COMPTYPE modified
 *
 * Revision 1.12  1994/06/22  07:14:13  car
 * *** empty log message ***
 *
 * Revision 1.11  1994/06/20  06:03:43  car
 * first extension for optimizer
 *
 * Revision 1.10  1994/05/04  07:01:59  ach
 * first beta release of distributed tilde-version
 *
 * Revision 1.9  1994/03/29  17:26:43  mah
 * NOMATBODY added
 *
 * Revision 1.8  1994/03/08  18:53:11  mah
 * tilde pattern match added
 *
 * Revision 1.7  1994/03/08  13:13:00  car
 * support remote descriptors
 *
 * Revision 1.6  1994/02/04  12:44:31  car
 * changes in codefile output
 *
 * Revision 1.5  1994/01/20  08:27:32  car
 * corrections for rextcode.c
 *
 * Revision 1.4  1994/01/18  08:39:55  mah
 * tilde version
 * .,
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
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

/* descriptor class for type-infer mechanism */
#define C_ATOMTYPE        '\16'
#define C_COMPTYPE        '\17'
#define C_LINKNODE        '\20'

/* ach 04/03/93, used for messages */
#define C_SENT            '\52'

/* folgende 6 Classes sind GRAPH-Classes */
#define C_PROCESS         '\100'
#define C_STREAM          '\101'
#define C_GFIELD          '\102'
#define C_MERGE_FIELD     '\103'
#define C_SEMAPHORE       '\104'
#define C_MERGE_STREAM    '\105'
#define C_EDIT            '\106'

/* cr 09/03/95, kir(ff), START */
#define C_FRAME           '\21'
#define C_INTER           '\22'
/* cr 09/03/95, kir(ff), END */

/* interaction descriptor class  stt */
#define C_INTACT          '\23'

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
/* mah 111093 */
#define TY_FNAME            '\20'
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

/* mah 281093 tilde-stack */
#if WITHTILDE
#define TY_SNSUB             '\57' 
/* mah 090394 nomat body closure type */
#define TY_NOMATBODY         '\61'
#endif /* WITHTILDE */

/* special list/matrix type for remote structures */
#define TY_REMOTE            '\60'

/* constructor types for type inference mechanism */
#define TY_VECT              '\70'
#define TY_TVECT             '\71'
#define TY_MAT               '\72'
#define TY_FUNC              '\73'
#define TY_PROD              '\74'
#define TY_SUM               '\75'

/* interaction type     stt */
#define TY_INTACT            '\76'

/* cr 09/03/95, kir(ff), START */
#define TY_FRAME             '\100'
#define TY_SLOT              '\101'
#define TY_GET               '\102'
#define TY_PUT               '\103'
#define TY_DONE              '\104'
/* cr 09/03/95, kir(ff), END */

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
    char freespace[8];
    PTR_HEAPELEM  ptn;   /* Heapadresse des Namenstrings */
} ST_FNAME;

typedef struct {         /* Tilde Sub */ 
    HEADER;
#if  ODDSEX
    COUNT           nargs;
    COUNT         special;
#else 
    COUNT         special;
    COUNT           nargs;
#endif
    PTR_HEAPELEM namelist; /* dummy: immer ~0 ~1 ... */ 
    PTR_HEAPELEM      pte;
} ST_SNSUB;

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
                                    /* und den alten MATCH-Deskriptor wegen   */
                                    /* der zum "online"-Postprocessing ;-)    */
                                    /* noetigen Angaben wie Stelligkeiten etc.*/
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

#if WITHTILDE

typedef struct {                 /* BAR  Patternmatch */
  HEADER;
  int             casetype;
  COUNT              nwhen;
  COUNT           anz_args;     
  PTR_HEAPELEM        ptse;
}               ST_SWITCH;

#else

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

#endif /* WITHTILDE */

typedef struct {                /* BAR  Patternmatch */
    HEADER;
    PTR_HEAPELEM     code;
    PTR_HEAPELEM     guard;
    PTR_HEAPELEM     body;
} ST_MATCH;

#if WITHTILDE

typedef struct {              /*BAR  Patternmatch*/
    HEADER;
    PTR_HEAPELEM  guard_body; /* guard, body, and argument */
    COUNT         act_nomat;  /*act_nom steht auf 0, falls es sich*/
                              /*um otherwise Ausdruck handelt*/
    COUNT            reason;
    T_PTD     ptsdes;
}               ST_NOMAT;

#else

typedef struct {              /*BAR  Patternmatch*/
    HEADER;
    PTR_HEAPELEM  guard_body; /*act_nom steht auf 0, falls es sich*/
    int           act_nomat;  /*um otherwise Ausdruck handelt*/
    T_PTD     ptsdes;
} ST_NOMAT;

#endif /* WITHTILDE */

#if !WITHTILDE

typedef struct {                                                       /* CS */
    HEADER;
    int            nfuncs;                                             /* CS */
    PTR_HEAPELEM namelist;                                             /* CS */
    PTR_HEAPELEM     ptdv;                                             /* CS */
} ST_LREC;                                                             /* CS */

#else /* WITHTILDE */

typedef struct {                                                 
    HEADER;
#if  ODDSEX
    COUNT         nfuncs;    /* number of functions */
    COUNT         ntilde;   /* number of tilde arguments of this def block */
#else
    COUNT         ntilde;    
    COUNT         nfuncs;   
#endif
    PTR_HEAPELEM namelist;                                        
    PTR_HEAPELEM     ptdv;                                          
} ST_LREC;    

#endif /* WITHTILDE */

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

#if WITHTILDE
 typedef struct {             /*     --- closure ---      */
     HEADER;
#if  ODDSEX
     COUNT         nargs;     /* benoetigte argumente     */
     COUNT         args;      /* vorhandene argumente     */
#else 
     COUNT         args;      /* vorhandene argumente     */
     COUNT         nargs;     /* benoetigte argumente     */
#endif
     PTR_HEAPELEM  pta;       /* argumentvektor           */
#if  ODDSEX
     COUNT         nfv;       /* freie Variablen          */
     COUNT         ftype;     /* fkt.typ COMB,PRIM,COND   */
#else
     COUNT         ftype;     /* fkt.typ COMB,PRIM,COND   */
     COUNT         nfv;       /* freie Variablen          */
#endif /* ODDSEX */
 } ST_CLOS;
#else
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
#endif /* WITHTILDE */

/* mah 141093: lneu-closure-definitionen */
/*          bis jetzt noch nicht benutzt */
/* nicht vergessen HEADER noch einzufuegen */
#if 0
typedef struct {
    COUNT          ndargs; /* wie nargs */
    COUNT             nfv; /* # of free variables */
    PTR_HEAPELEM      pta; /* wie oben nur ohne Funktionspointer */
    T_PTD            ptfd; /* Funtionspointer */
}               ST_CLOS;

typedef struct {
    char     freespace[2];
    COUNT          ndargs;
    int          primfunc;
    PTR_HEAPELEM      pta;
}               ST_P_CLOS;
#endif
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

/* type inference system car 9.06.94 */

/* atomic type, type field of header contains the type */
/* possible types are TY_INTEGER/TY_REAL/TY_BOOL/TY_STRING/TY_DIGIT (car) */
typedef struct {
	HEADER;
        int             value;
	char		freespace[8];
	} ST_ATOMTYPE;

/* structured type, may be TY_VECT/TY_TVECT/TY_MAT/TY_FUNC/TY_PROD/TY_SUM (car) */
typedef struct {
	HEADER;
	COUNT		cnt;
	COUNT		nargs; /* args left */
        COUNT           fkt;
	char		freespace[2];
	PTR_HEAPELEM	ptypes;
	} ST_COMPTYPE;

/* atomic type, type field of header contains the type (car) */
typedef struct {
	HEADER;
	COUNT		refcnt;
	char		freespace[6];
	T_PTD		ptype;
	} ST_LINKNODE;

/* cr 09/03/95, kir(ff), START */
/* Interaction (C_INTER), may be TY_GET, TY_PUT, TY_DONE */
typedef struct {
    HEADER;
#if  ODDSEX
    COUNT          dim;
    char           freespace[6];
#else 
    char           freespace[6];
    COUNT          dim;
#endif
    PTR_HEAPELEM    args;
    } ST_INTER;

/* Slot (Name,Value) of Frame */
typedef struct {
    HEADER;
    char            freespace[4];
    T_PTD           name;
    T_HEAPELEM      value;
    } ST_SLOT;

/* Frame, with list of Slots */
typedef struct {
    HEADER;
#if  ODDSEX
    COUNT          dim;
    char           freespace[6];
#else 
    char           freespace[6];
    COUNT          dim;
#endif
    PTR_HEAPELEM    slots;
    } ST_FRAME;
/* cr 09/03/95, kir(ff), END */

/* descriptor for interactions     stt */
typedef struct {
    HEADER;
    char           freespace[6];
    COUNT          dim;
    PTR_HEAPELEM   args;
} ST_INTACT;         


typedef struct T_descriptor {
/*  struct T_descriptor  *heap_chain;                                 */
/*  Der Heapchainverweis wird, falls der Descriptor frei ist, auf die */
/*  letzten vier Byte des Descriptors gelegt.   (siehe rheap.c)       */

    union { /* die moeglichen Deskriptoren: */
/* dg */             ST_COMB       cmb;     /* combinator, code  */
            ST_SCALAR     sc;
            ST_DIGIT      di;
            ST_LIST       li;
            ST_MVT        mvt; /* matrix / vector / transposed vector */
            ST_VAR        va;  /* die Expressiondescriptoren: */
            ST_COND       co;
            ST_EXPR       ex;
            ST_FUNC       fu;
            ST_NAME       na;
	    ST_FNAME      fn;
	    ST_SNSUB      sn;  /* sub~ */
            ST_SWITCH     sw;  /* BAR  Patternmatch */
            ST_MATCH      ma;  /* BAR  Patternmatch */
            ST_NOMAT      no;  /* BAR  Patternmatch */
            ST_LREC       lr;                              /* CS */
            ST_LREC_IND   ri;                              /* CS */
            ST_LREC_ARGS  la;                              /* CS */
            ST_SWAP_HEAP  sh;
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
	    ST_ATOMTYPE   at;
	    ST_COMPTYPE   ct;
	    ST_LINKNODE   ln;
/* cr 09/03/95, kir(ff), START */
        ST_INTER      inter;   /* Interaction */
        ST_FRAME      frame;   /* Frame */
        ST_SLOT       slot;    /* Slot */
/* cr 09/03/95, kir(ff), END */
            ST_INTACT     ia; /* interaction */
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
#if (D_SLAVE && D_MESS && D_MHEAP)
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
#if (D_SLAVE && D_MESS && D_MHEAP)
#define GET_DESC(p)            do {if ((p = (T_PTD)(*d_m_newdesc)()) == 0) \
                                 post_mortem("heap overflow (newdesc)");} while(0)
#else
#define GET_DESC(p)            do {if ((p = (T_PTD)newdesc()) == 0) \
                                 post_mortem("heap overflow (newdesc)");} while(0)
#endif
#if (D_SLAVE && D_MESS && D_MHEAP)
#define GET_HEAP(n,p)          do {if (((*d_m_newheap)(n,p)) == 0) \
                                 post_mortem("heap overflow (newheap)");} \
                               while(0)
#else
#define GET_HEAP(n,p)          do {if ((newheap(n,p)) == 0) \
                                 post_mortem("heap overflow (newheap)");} \
                               while(0)
#endif
#define NEWDESC(p)             GET_DESC(p)
#if (D_SLAVE && D_MESS && D_MHEAP)
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

#if (D_SLAVE && D_MESS && D_MHEAP)
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

#if WITHTILDE
#define L_FUNC(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (((p).u.sc.type != TY_SUB) && \
    ((p).u.sc.type != TY_SNSUB) && \
    ((p).u.sc.type != TY_REC) && \
    ((p).u.sc.type != TY_ZF)) || \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_FUNC"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.fu.x

#define R_FUNC(p,x) \
 (( (((p).u.sc.type != TY_SUB) && \
     ((p).u.sc.type != TY_SNSUB) && \
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
#else /* !WITHTILDE */
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

#endif /* WITHTILDE */

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
/* Funktionname Descriptor FNAME                                     */
/*********************************************************************/


#define L_FNAME(p,x)  \
  ( ( (p).u.sc.ref_count > 65000   || \
    (p).u.sc.ref_count == 0      || \
    (p).u.sc.type    != TY_FNAME    ||  \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries L_FNAME"))       \
  : (MEAS_CALL(MS_DESCREF) 0)),  \
  (p).u.fn.x

#define R_FNAME(p,x)  \
 (( (p).u.sc.type    != TY_FNAME    ||  \
    (p).u.sc.class   != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0) , \
  post_mortem("inconsistent descriptor entries R_FNAME"), \
   (p).u.fn.x) \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.fn.x))

#define A_FNAME(p,x)       \
  ( MEAS_CALL(MS_DESCREF)  \
  &(p).u.fn.x)


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

/***********************************************************/
/* AtomType     Descriptor - ATOMTYPE                * car */
/***********************************************************/

#define L_ATOMTYPE(p,x) /* TB, 3.11.1992 */\
  ( ( ( (p).u.at.ref_count > 65000 || \
    (p).u.at.ref_count == 0 ) || \
    (p).u.at.class   != C_ATOMTYPE ) /* stt, 11.10.95 */ \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_ATOMTYPE"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.at.x

#define R_ATOMTYPE(p,x) \
  ( ( (p).u.at.class   != C_ATOMTYPE  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_ATOMTYPE"), \
     (p).u.at.x) /* TB, 3.11.1992 *//* stt, 11.10.95 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.at.x ))

#define A_ATOMTYPE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.at.x)

/***********************************************************/
/* CombType     Descriptor - COMPTYPE                * car */
/***********************************************************/

#define L_COMPTYPE(p,x) /* TB, 3.11.1992 */\
  ( ( ( (p).u.ct.ref_count > 65000 || \
    (p).u.ct.ref_count == 0 ) || \
    (p).u.ct.class   != C_COMPTYPE ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_COMPTYPE"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.ct.x

#define R_COMPTYPE(p,x) \
  ( ( (p).u.ct.class   != C_COMPTYPE  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_COMPTYPE"), \
     (p).u.ct.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.ct.x ))

#define A_COMPTYPE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.ct.x)

/***********************************************************/
/* LinkNode     Descriptor - LINKNODE                * car */
/***********************************************************/

#define L_LINKNODE(p,x) /* TB, 3.11.1992 */\
  ( ( ( (p).u.ln.ref_count > 65000 || \
    (p).u.ln.ref_count == 0 ) || \
    (p).u.ln.class   != C_LINKNODE ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries L_LINKNODE"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.ln.x

#define R_LINKNODE(p,x) \
  ( ( (p).u.ln.class   != C_LINKNODE  ) \
  ? ( DescDump(stdout,&(p),0), \
     post_mortem("inconsistent descriptor entries R_LINKNODE"), \
     (p).u.ln.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
    (p).u.ln.x ))

#define A_LINKNODE(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
    &(p).u.ln.x)

/* cr 09/03/95, kir(ff), START */
/*********************************************************************/
/* Interaction Descriptor    - INTER                                 */
/*********************************************************************/

#define L_INTER(p,x) \
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != C_INTER    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_INTER"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.inter.x

#define R_INTER(p,x) \
 (((((p).u.sc.type   != TY_GET) && \
   ((p).u.sc.type    != TY_PUT) && \
   ((p).u.sc.type    != TY_DONE))|| \
   ((p).u.sc.class   != C_INTER)   ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_INTER"), \
   (p).u.inter.x) \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.inter.x))

#define A_INTER(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.inter.x)

/*********************************************************************/
/* Slot Descriptor    - SLOT                                         */
/*********************************************************************/

#define L_SLOT(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.type   != TY_SLOT     || \
    (p).u.sc.class   != C_FRAME    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_SLOT"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.slot.x

#define R_SLOT(p,x) \
 (( ((p).u.sc.type    != TY_SLOT)|| \
   ((p).u.sc.class   != C_FRAME)   ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_SLOT"), \
   (p).u.slot.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.slot.x))

#define A_SLOT(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.slot.x)


/*********************************************************************/
/* Frame Descriptor    - SLOT                                        */
/*********************************************************************/

#define L_FRAME(p,x) /* TB, 3.11.1992 */\
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.type   != TY_FRAME     || \
    (p).u.sc.class   != C_FRAME    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_FRAME"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.frame.x

#define R_FRAME(p,x) \
 ((((p).u.sc.type   != TY_FRAME) || \
   ((p).u.sc.class   != C_FRAME)   ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_FRAME"), \
   (p).u.frame.x) /* TB, 3.11.1992 */ \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.frame.x))

#define A_FRAME(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.frame.x)

/* cr 09/03/95, kir(ff), END */


/*********************************************************************/
/* Interaction Descriptor    - INTACT                 stt  11.09.95  */
/*********************************************************************/

#define L_INTACT(p,x) \
  ( ( (p).u.sc.ref_count > 65000     || \
    (p).u.sc.ref_count ==  0       || \
    (p).u.sc.class   != C_INTACT    ) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries L_INTACT"), 0)\
  : (MEAS_CALL(MS_DESCREF) 0)), (p).u.ia.x

#define R_INTACT(p,x) \
 (((p).u.sc.class   != C_INTACT) ? \
  ( DescDump(stdout,&(p),0) , \
   post_mortem("inconsistent descriptor entries R_INTACT"), \
   (p).u.ia.x) \
  : (MEAS_CALL(MS_DESCREF) \
  (p).u.ia.x))

#define A_INTACT(p,x) \
  ( MEAS_CALL(MS_DESCREF) \
  &(p).u.ia.x)



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
/* Funktionname Descriptor FNAME                                     */
/*********************************************************************/

#define L_FNAME(p,x) (p).u.fn.x
#define R_FNAME(p,x) (p).u.fn.x
#define A_FNAME(p,x) &(p).u.fn.x

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

/***********************************************************/
/* AtomType     Descriptor - ATOMTYPE                * car */
/***********************************************************/

#define L_ATOMTYPE(p,x) (p).u.at.x
#define R_ATOMTYPE(p,x) (p).u.at.x
#define A_ATOMTYPE(p,x) &(p).u.at.x

/***********************************************************/
/* CombType     Descriptor - COMPTYPE                * car */
/***********************************************************/

#define L_COMPTYPE(p,x) (p).u.ct.x
#define R_COMPTYPE(p,x) (p).u.ct.x
#define A_COMPTYPE(p,x) &(p).u.ct.x

/***********************************************************/
/* LinkNode     Descriptor - LINKNODE                * car */
/***********************************************************/

#define L_LINKNODE(p,x) (p).u.ln.x
#define R_LINKNODE(p,x) (p).u.ln.x
#define A_LINKNODE(p,x) &(p).u.ln.x

/* cr 09/03/95, kir(ff), START */
/*********************************************************************/
/* Interaction Descriptor - INTER                                    */
/*********************************************************************/

#define L_INTER(p,x) (p).u.inter.x
#define R_INTER(p,x) (p).u.inter.x
#define A_INTER(p,x) &(p).u.inter.x

/*********************************************************************/
/* Slot Descriptor - SLOT                                           */
/*********************************************************************/

#define L_SLOT(p,x) (p).u.slot.x
#define R_SLOT(p,x) (p).u.slot.x
#define A_SLOT(p,x) &(p).u.slot.x


/*********************************************************************/
/* Frame Descriptor - FRAME                                          */
/*********************************************************************/

#define L_FRAME(p,x) (p).u.frame.x
#define R_FRAME(p,x) (p).u.frame.x
#define A_FRAME(p,x) &(p).u.frame.x

/* cr 09/03/95, kir(ff), END */

/*********************************************************************/
/* Interaction Descriptor - INTACT                   stt 11.09.95    */
/*********************************************************************/

#define L_INTACT(p,x) (p).u.ia.x
#define R_INTACT(p,x) (p).u.ia.x
#define A_INTACT(p,x) &(p).u.ia.x


#endif     /* DEBUG  , NODEBUG  */

#endif /* RHEAPTY_H */
