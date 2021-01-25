
/*{{{  {  Revision History}*/
/* file preproc.c */
/* Revision History


13 April 1992 
   Beim Binden von Pattern-Variablen musste auch nargs erhoeht werden.

12 Maerz 1992
   Der Heapdump lieferte Warnungen ueber (Null) Zeiger, die nicht in den Heap
   zeigten. Grund, ein vergessener SUB-Descriptor in store, M-Linie, bei der 
   SUB-Konstruktor-Behandlung.

29 Oktober 1991
   Der Fehler lag in EAPRE beim Wegschreiben der freien Variablen. Sie
   wurden naemlich in falscher Reihenfolge in den Heap gelegt!

25 Oktober 1991
   Alte Korrekturen in Store (Oct 90) waren falsch, der Fehler liegt woanders.

15 Oktober 1991
   Fehler beim Schliessen behoben, nbound wurde nicht gesetzt.

04 Oktober 1991
   Fehler bei der Indexberechnung der Variablen in EAPRE behoben.

09 August 1990
   Lambda Abstraktionen innerhalb von List-Comprehensions werden nun korrekt geschlossen.

02 August 1990
   Freie Variablen in ZF-Expressions werden nur in dem naechst-umgebenden Lambda Term
   geschlossen. Ein ZF-Ausdruck darf also den M1 Stack, der die relativ freien Variablen
   aufsammelt, nicht antasten! Das hat er bis dato getan. Nun nicht mehr.

24 Juli 1990
   PX wird als globale Variable benutzt um in STORE einen Zeiger auf
   die umgebende Funktion zu halten. Dieser Zeiger darf nicht ueberschrieben
   werden, wie es in STORE bisher geschah. Einfuehren der neuen Variablen
   ZFPX, die in den ZF-Teilen benutzt wird.

16 Juli 1990
   NBOUND wurde nicht geaendert, wenn eine ZF-Variable gebunden wurde.
   Daher wurden dann falsche Nummernvariablen erzeugt.
   Jetzt wird beim Binden NBOUND erhoeht, beim Aufloesen der Bindung
   NBOUND vermindert.



*/
/*}}}  */

/*--------------------------------------------------------------------------- */
/*  preproc.c  - external:  preproc                                           */
/*--------------------------------------------------------------------------- */

/*{{{  {  #includes}*/
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
/*}}}  */

/*{{{  {  externals}*/
/* --- externe variablen --- */

extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */

/* --- spezielle externe routinen --- */

extern STACKELEM      st_name();             /* rstpre.c */
extern STACKELEM      st_var();              /* rstpre.c */
extern STACKELEM      st_dec();              /* rstpre.c */
extern STACKELEM      st_mvt();              /* rstpre.c */
extern STACKELEM      st_cond();             /* rstpre.c */
extern STACKELEM      st_list();             /* rstpre.c */
extern STACKELEM      st_expr();
extern void           ptrav_a_e();           /* rtrav.c  */
extern void           trav_a_e();            /* rtrav.c  */
extern void           freeheap();            /* rheap.c  */
extern T_DESCRIPTOR   *newdesc();            /* rheap.c  */
extern T_HEAPELEM     *newheap();            /* rheap.c  */
/*}}}  */

/*{{{  {  Hilfsfunktionen}*/
/* --- Hilfsfunktionen --- */

/*{{{  {  trav_e_m1}*/
/*---------------------------------------------------------------------------
 * trav_e_m1 --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *               Single Atoms besteht rekursiv von E nach M1.
 *               Strings, die noch ausgebreitet auf dem Stack stehen, werden
 *               zu einem Atom zusammengefasst.
 * globals :     S_e, S_m1;
 *---------------------------------------------------------------------------
 */
void trav_e_m1()
{
    register STACKELEM  x = POPSTACK(S_e);
    register int        i;

    if ((T_STR(x)) && !(T_STR_CHAR(x))) {
      /* digitstring oder letterstring, auslagern? */
      PUSHSTACK(S_e,x);                        /* wieder zurueck */
      PUSHSTACK(S_m1,st_name());               /* Namenspointer erzeugen */
      return;
    }

    if (T_PLUS(x) || T_MINUS(x) || T_EXP(x)) {
      /* digit-string auslagern */
      PUSHSTACK(S_e,x); /* Konstruktor wieder zurueck */
      PUSHSTACK(S_m1,st_dec());
      return;
    }
    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_e_m1();

    PUSHSTACK(S_m1,x);
}
/*}}}  */

/*{{{  {  trav_m1_e}*/
/*---------------------------------------------------------------------------
 * trav_m1_e --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *               Single Atoms besteht rekursiv von M1 nach E.
 * globals :     S_m1, S_e;
 *---------------------------------------------------------------------------
 */
void trav_m1_e()
{
    register STACKELEM  x = POPSTACK(S_m1);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_m1_e();
    PUSHSTACK(S_e,x);
}
/*}}}  */

/*}}}  */

/*{{{  {  Macros}*/
/* --- makros -- */

#define ST_COND(x)         x = st_cond()
#define ST_LIST(x)         if ((x = st_list()) == 0) \
                             post_mortem("preproc: heap overflow"); \
                           else
#define ST_EXPR(p)         PTRAV_A_HILF; mvheap(A_EXPR((*p),pte))
#define ST_FUNC(p)         PTRAV_A_HILF; mvheap(A_FUNC((*p),pte))

#define MKDESC(p,r,c,t) if ((p = newdesc()) != 0) {  \
                              DESC_MASK(p,r,c,t);  \
                            }  \
                            else post_mortem("preproc: heap overflow") 

     /* printf("Descriptor %0x erzeugt %s %d\n",(int)p,__FILE__,__LINE__); */ 

/* #define GET_HEAP(n,a)      if (newheap(n,a) == 0) */ 
                             /*  post_mortem("preproc: heap overflow"); */ 
                           /* else */
/*}}}  */

/*{{{  {  Preprocessor}*/
/* ------------------------------------------------------------------ */
/* preproc:                                                           */
/* es wird vorausgesetzt, dass folgende situation vorliegt:           */
/* - der ausdruck liegt auf stack e in preorder linearisierter form   */
/* konstanten und variablen werden als erstes ausgelagert (eac)       */
/* - pointer auf variablennamen sind eindeutig (st_name)              */
/* erstes traversieren des ausdrucks, wobei folgendes erledigt wird:  */
/* - anlegen von descriptoren fuer SUB- und LREC-konstrukte           */
/* - gebundene variablen werden durch nummernvariablen ersetzt        */
/* - relativ freie variablen werden durch nummernvariablen ersetzt    */
/*   und im zugehoerigen SUB-descriptor an die namensliste angehaengt */
/* verwendung der stacks:                                             */
/* - stack a zielstack                                                */
/* - stack m : kontrollstack (konstruktoren werden doppelt abgelegt)  */
/* - stack i : bindende variablen kommen auf i                        */
/* - stack m1: relativ freie variablen kommen auf m1                  */
/* ------------------------------------------------------------------ */

/*{{{  {  Dietmars dreckiger Trick}*/
/* dirty trick!!! */
#define preproc ea_create
/*}}}  */

void preproc()
{
/*{{{  {  Variablen}*/
  register int            i;
  register STACKELEM      x,y;
  PTR_DESCRIPTOR          desc,px,zfpx;
  PTR_DESCRIPTOR          pmpx;
  PTR_HEAPELEM            ph;
  int                     nbound = 0;
  int                     nargs = 0;
  int                     nfree = 0;
  int                     within_pattern=FALSE;
  register unsigned short arity;
/*}}}  */

  START_MODUL("preproc");

/*{{{  {  Interruptbehandlung}*/
  /* interruptbehandlung preprocessing */
  setjmp(_interruptbuf);
  if (_interrupt) {
    /* interrupt erfolgt */
    _interrupt = FALSE;
    post_mortem("preproc: interrupt received");
  }
/*}}}  */

/*{{{  {  EAC}*/
  /*------------------------------------------------------------------------- */
  /* eac      --  Lagert aus einem auf dem E-Stack liegenden ausdruck alle    */
  /*              konstanten aus, liefert ergebnisausdruck auf dem E-Stack ab.*/
  /* globals  --  S_e,S_a     <w>                                             */
  /*------------------------------------------------------------------------- */

  START_MODUL("eac");

  PUSHSTACK(S_m,DOLLAR);                 /* Ende-Symbol */

/*{{{  {  EAC E-Linie}*/
eac_e:

  x = READSTACK(S_e);

  /*{{{  {  % Schweinerei}*/
  /* In Stringpattern werden Variablennamen vom Editor
     mit einem fuehrenden % versehen.  Hier wird es entfernt.
     Es wird im Postprozessor wahrscheinlich wieder benoetigt. ACHTUNG
     Eventuell gibt es noch andere %, die nicht weggeworfen werden duerfen. */
  if (T_STR(x) && (VALUE(x)=='%'))  {
     POPSTACK(S_e);
     goto eac_e;
  }
  /*}}}  */

  /*{{{  {  NIL-Konstanten}*/
  
    if T_NILMAT(x) {                       /* leere Matrix ?                 */
      PPOPSTACK(S_e);
      INC_REFCNT(_nilmat);                 /* durch nilmat pointer ersetzen  */
      PUSHSTACK(S_a,(STACKELEM)_nilmat);
      goto eac_m;
    }
    if T_NILVECT(x) {                      /* leerer Vektor ?                */
      PPOPSTACK(S_e);                      /* durch nilvect pointer ersetzen */
      INC_REFCNT(_nilvect);
      PUSHSTACK(S_a,(STACKELEM)_nilvect);
      goto eac_m;
    }
    if T_NILTVECT(x) {                     /* leerer TVektor ?               */
      PPOPSTACK(S_e);                      /* durch niltvect pointer ersetzen*/
      INC_REFCNT(_niltvect);
      PUSHSTACK(S_a,(STACKELEM)_niltvect);
      goto eac_m;
    }
    if T_NIL(x) {                          /* leere Liste ?                  */
      PPOPSTACK(S_e);                      /* durch nil pointer ersetzen     */
      INC_REFCNT(_nil);
      PUSHSTACK(S_a,(STACKELEM)_nil);
      goto eac_m;
    }
    if T_NILSTRING(x) {                    /* leerer String ?                */
      PPOPSTACK(S_e);                      /* durch nilstring pointer ersetzen*/
      INC_REFCNT(_nilstring);
      PUSHSTACK(S_a,(STACKELEM)_nilstring);
      goto eac_m;
    }
  /*}}}  */

  /*{{{  {  Konstruktoren}*/
  if (T_CON(x)) {
    /*{{{  {  Vector/Matrix Konstruktor}*/
    if T_MATRIX(x) {
      PUSHSTACK(S_a,st_mvt(C_MATRIX));              /* Matrix  auslagern   */
      goto eac_m;
    }
    if T_VECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_VECTOR));              /* Vektor  auslagern   */
      goto eac_m;
    }
    if T_TVECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_TVECTOR));               /* TVektor auslagern   */
      goto eac_m;
    }
    /*}}}  */
  
    /*{{{  {  Zahl Konstruktor (+, -, exp)}*/
    if (T_PLUS(x) || T_MINUS(x) || T_EXP(x)) {
      /* digit-string auslagern */
      PUSHSTACK(S_a,st_dec());
      goto eac_m;
    }
    /*}}}  */
  
    /*{{{  {  ZF-Konstruktor  -> Umsortieren der Komponenten}*/
    if (T_ZF_UH(x)) {
      /* E: ZFn expr qual1 ... qualn-1 */
    
      /* Die Qualifier normal (auf der E-Linie) behandeln. */
      PUSHSTACK(S_m,POPSTACK(S_e));
      PUSHSTACK(S_m,DEC(x));
    
      /* expr aus dem Weg (sprich M1) schaffen -- wird spaeter behandelt. */
      trav_e_m1();
    
      goto eac_e;
    }
    /*}}}  */
  
    /*{{{  {  ZF-Generator}*/
    if (T_ZFIN_UH(x)) {
      /* E: ZFIN2 var expr */
      /* Variable aus dem Weg schaffen (M1), Ausdruck normal behandeln. */
      PUSHSTACK(S_m,POPSTACK(S_e));   /* ZFIN mit Orginalstelligkeit 2 auf M */
      PUSHSTACK(S_m,DEC(x));          /* ZFIN 1 bekommt auf der M-Linie sofort die Kontrolle */
      /* Wenn noch ein Character-String auf E liegt, wird er ausgelagert. */
      x = READSTACK(S_e);
      if ((T_STR(x)) && !(T_STR_CHAR(x))) {
        /* digitstring oder letterstring, auslagern? */
        PUSHSTACK(S_m1,st_name());               /* Namenspointer erzeugen */
      } else
        PUSHSTACK(S_m1,POPSTACK(S_e));
    
      goto eac_e;
    }
    /*}}}  */
  
    /*{{{  {  Kommentar Konstruktor}*/
    if (T_K_COMM (x)) {
      PPOPSTACK(S_e);
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m,x);
      if (T_STRING(READSTACK(S_e)) || T_NILSTRING(READSTACK(S_e))) {
        goto eac_e;
      }
      else {
        if ((x = st_expr()) != 0)
          PUSHSTACK(S_a,x);
        else
          post_mortem("eac: heap out of space");
        goto eac_m;
      }
    }
    /*}}}  */
  
    /*{{{  {  SCALAR Konstruktor}*/
    if (T_SCALAR(x)) {
      post_mortem("eac: INT and REAL no more allowed");
    }
    /*}}}  */
  
    /*{{{  {  sonstiger Konstruktor Stelligkeit > 0}*/
    if (ARITY(x) > 0) {
      /* sonstiger Konstruktor mit Stelligkeit > 0 */
      PPOPSTACK(S_e);
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m,x);
      goto eac_e;
    }
    /*}}}  */
  
    /*{{{  {  sonstiger Konstruktor Stelligkeit = 0}*/
    /* sonst. Konstruktor mit Stelligkeit 0 */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto eac_m;
    /*}}}  */
  
  } /* end T_CON */
  /*}}}  */

  /*{{{  {  Ziffern- oder Buchstabenstring}*/
  if ((T_STR(x)) && !(T_STR_CHAR(x))) {
    /* digitstring oder letterstring, auslagern? */
    PUSHSTACK(S_a,st_name());               /* Namenspointer erzeugen */
    goto eac_m;
  }
  /*}}}  */

  /*{{{  {  single Atoms}*/
  if (T_ATOM(x)) {
    /* sonstige atoms */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto eac_m;
  }
  /*}}}  */

  /*{{{  {  kein bekanntes Stackelement}*/
  /* sonst */
  post_mortem("eac: No match successful on eac_e");
  /*}}}  */
/*}}}  */

/*{{{  {  EAC M-Linie}*/
eac_m:

  x = READSTACK(S_m);
  arity = ARITY(x);

  /*{{{  {  Stelligkeit > 1}*/
    if (arity > 1) {
      /* dh die Arity des Konstruktors war >= 2 */
      WRITE_ARITY(S_m,arity-1);
      goto eac_e;
    }
  /*}}}  */
  /*{{{  {  Stelligkeit = 1}*/
  if (arity > 0) {
    /* DOLLAR hat arity = 0 */
    if (T_ZF_UH(x)) {
      /* Nun sind alle Qualifier behandelt. Es fehlt noch der Zielausdruck, der auf M1 liegt. */
      /* Traversiere ihn zurueck auf E. */
      trav_m1_e();
      WRITESTACK(S_m,DEC(x));  /* Nun steht ZF0 auf M */
      goto eac_e;
    }
    if (T_ZFIN_UH(x)) {
      /* Der Ausdruck ist nun behandelt, es fehlt noch die Variable, die auf M1 liegt. */
      /* Hole sie nach E */
      PUSHSTACK(S_e,POPSTACK(S_m1));
      WRITESTACK(S_m,DEC(x));  /* Nun steht ZFIN0 auf M */
      goto eac_e;
    }
    PPOPSTACK(S_m);
    x = POPSTACK(S_m);
    PUSHSTACK(S_a,x);
  
    /* if (T_STRING(x)) {
      ST_LIST(x);
      PUSHSTACK(S_a,x);
    }  */
    goto eac_m;
  }
  /*}}}  */
  /*{{{  {  Stelligkeit = 0}*/
    if (T_ZF_UH(x) || T_ZFIN_UH(x)) {
      /* Nun sind alle Argumente behandelt. */
      /* Konstruktor oben auf A. */
      PPOPSTACK(S_m);
      PUSHSTACK(S_a,POPSTACK(S_m));  /* Mit Orginalstelligkeit */
      goto eac_m;
    }
  /*}}}  */

  /* Ein DOLLAR. Dann ist eac zu beenden */
  PPOPSTACK(S_m);         /* DOLLAR */
  PTRAV_A_E;             /* ausdruck wieder zurueck */
  /* v-stack leerraeumen */
  for (i = SIZEOFSTACK(S_v) ; i > 0 ; i--)
    PPOPSTACK(S_v);
/*}}}  */

  END_MODUL("eac");
/*}}}  */

/*{{{  {  EAPRE}*/
  /* eapre:                                                             */
  /* erstes traversieren des ausdrucks, wobei folgendes erledigt wird:  */
  /* - anlegen von descriptoren fuer SUB- und LREC-konstrukte           */
  /* - gebundene variablen werden durch nummernvariablen ersetzt        */
  /* - relativ freie variablen werden durch nummernvariablen ersetzt    */
  /*   und im zugehoerigen SUB-descriptor an die namensliste angehaengt */
  /* ZF-Ausdruecke werden zerlegt und in eine Applikationsfolge         */
  /* umgewandelt. Fuer ZF-Generatoren werden ZF-Descriptoren,           */
  /* entsprechend SUB-Descriptoren angelegt, in denen die Bindungen     */
  /* der ZF-Variablen und der relativ freien Variablen vermerkt sind.   */
  /* Sie werden durch zweistellige ZF_IN Konstruktoren zusammengehalten.*/
  /* ZF-Filterm die vom Editor ohne Speziellen Konstruktor kommen,      */
  /* Werden mit einem ZF_INT Konstruktor versehen. In STORE bekommen    */
  /* sie einen ZF-Descriptor.                                           */

  START_MODUL("eapre");

  PUSHSTACK(S_m,KLAA);               /* bottomsymbol auf m */
  PUSHSTACK(S_m1,DOLLAR);            /* bottomsymbol auf m1 */
  PUSHSTACK(S_i,DOLLAR);             /* bottomsymbol auf i */

/*{{{  {  EAPRE E-Linie}*/
eapre_e:

  x = POPSTACK(S_e);

  /*{{{  {  $ innerhalb eines Patterns}*/
  if (within_pattern) {
    if (T_PM_DOLLAR(x)) {
       post_mortem("In dieser Version wird $ nicht unterstuetzt!");
    }
  }
  /*}}}  */

/*{{{  {  Konstruktor}*/
if (T_CON(x)) {
  /* Konstruktor auf E */

/*{{{  {  SUB-Konstruktor}*/
if (T_SUB(x)) {
  /* SUB(n+1) v_1 .. v_n expr | stack e */
  PUSHSTACK(S_m,SET_ARITY(x,1));         /* SUB(1) auf m */
  PUSHSTACK(S_m1,SET_ARITY(SUB,nbound)); /* nbound auf m1 retten */
  nbound = nargs = ARITY(x) - 1;         /* nbound, nargs initialisieren */

  /* bindende variablen auf i */
  for (i = nbound; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_e));

  /* und der SUB-konstruktor drauf */
  PUSHSTACK(S_i,DEC(x));                         /* SUB(n-1) auf i */
  /* zaehler fuer relativ freie variablen auf m1 */
  PUSHSTACK(S_m1,SET_ARITY(SNAP,0));             /* SNAP(0)  auf m1 */
  goto eapre_e;
} /* end T_SUB(x) */
/*}}}  */

  /*{{{  {  LetRec-Konstruktor}*/
  if (T_LREC(x)) {
    /* LREC(3) <(n) fnam_1 .. fnam_n <(n) func_1 .. func_n expr | stack e */
    PTR_DESCRIPTOR plrec;
    PTR_DESCRIPTOR plri;
    int            nfuncs;
    PUSHSTACK(S_m,DEC(x));                         /* LREC(2) auf m */
    y = POPSTACK(S_e);                             /* LIST(n) von e */
    nfuncs = ARITY(y);                             /* anzahl funktionen */
    /* der LREC-descriptor wird hier schon angelegt und initialisiert */
    /* damit letrec-gebundene variablen sofort beim antreffen durch */
    /* LREC_IND pointer ersetzt werden koennen */
    MKDESC(plrec,1,C_EXPRESSION,TY_LREC);       /* initialisierung */
    L_LREC((*plrec),nfuncs) = nfuncs;              /* anzahl funktionen */
    /* heap fuer die funktionsdefinitionen anfordern */
    GET_HEAP(nfuncs,A_LREC((*plrec),ptdv));
    /* mit LREC_IND-descriptoren initialisieren */
    for (i = nfuncs ; --i >= 0 ; ) {
      MKDESC(plri,1,C_EXPRESSION,TY_LREC_IND);  /* initialisieren */
      L_LREC_IND((*plri),index) = i;               /* nr. der funktion */
      L_LREC_IND((*plri),ptf) = (PTR_DESCRIPTOR)0; /* noch keine fkt. */
      L_LREC_IND((*plri),ptdd) = plrec;            /* zeiger auf LREC-des. */
      INC_REFCNT(plrec);                           /* refcountbehandlung */
      L_LREC((*plrec),ptdv)[i] = (T_HEAPELEM)plri; /* eintragen */
    }
    /* heap fuer die namensliste anfordern */
    GET_HEAP(nfuncs+1,A_LREC((*plrec),namelist));
    ph = R_LREC((*plrec),namelist);
    *ph++ = nfuncs;                         /* anzahl namen */
    /* letrec-gebundene variablen auf i und in den heap */
    for (i = nfuncs ; i > 0 ; i--) {
      y = POPSTACK(S_e);
      PUSHSTACK(S_i,y);
      *ph++ = y;
    }
    PUSHSTACK(S_i,plrec);                   /* LREC-descriptor auf i */
    PUSHSTACK(S_i,SET_ARITY(x,nfuncs));     /* LREC(nfuncs) auf i */
    /* der listenkonstruktor, der die funktionsdefinitionen zusammenhaelt */
    /* wird durch PRELIST ersetzt, das erleichtert einige abfragen */
    y = SET_ARITY(PRELIST,ARITY(POPSTACK(S_e)));
    PUSHSTACK(S_m,y);
    PUSHSTACK(S_m,y);
    /* falls kein SUB-konstruktor auf e, einen draufpacken */
    if (!T_SUB(READSTACK(S_e)))
      PUSHSTACK(S_e,SET_ARITY(SUB,1));
    goto eapre_e;
  } /* end T_LREC(x) */
  /*}}}  */

  /*{{{  {  ZF-Konstruktor}*/
  if (T_ZF_UH(x)) {
    /* Wenn ZF_UH n-stellig ist, liegt nun auf Stack E :  QUALIFIER1 ... QUALIFIERn-1 EXPR */
    PUSHSTACK(S_m,SET_ARITY(x,1));
    PUSHSTACK(S_m,INC(x));
  
    /* Anlegen der Bindungen der ZF-Variablen (zunaechst leer) */
    /* PUSHSTACK(S_i,SET_ARITY(SUB,0));   -uh 09Aug90 */
    PUSHSTACK(S_i,SET_ARITY(ZF_UH,0));    /* uh 09Aug90 */
    /* PUSHSTACK(S_m1,SET_ARITY(SNAP,0)); kein Antasten der relativ freien Variablen.
       Sie werden hier in der naechst umfassenden Funktion geschlossen.  uh 02Aug90 */
  
    goto eapre_m;
  
  } /* end T_ZF_UH */
  /*}}}  */

  /*{{{  {  ZF-Generator-Konstruktor}*/
  if (T_ZFIN_UH(x)) {
    /* Wenn der ZFIN Konstruktor angetroffen wird, so steht auf E: EXPR VAR. */
    /* Die Variable tritt an dieser Stelle definierend auf, muss also wie bei einem */
    /* Lambda behandelt werden. */
  
    /* Wenn ZFIN richtig benutzt wurde, so steht auf M jetzt entweder ein ZF oder ein */
    /* weiterer ZFIN Konstruktor. Ist das nicht der Fall, so liegt ein Fehler vor. */
    /* falsche Benutzung von ZFIN */
  
    if (!T_ZFIN_UH(READSTACK(S_m)) && !T_ZF_UH(READSTACK(S_m))) {
       post_mortem("eapre: Ungueltige Benutzung des Konstruktors <- .");
    }
  
    /* Zunaechst einfach den Ausdruck traversieren. */
    PUSHSTACK(S_m,DEC(x));
  
    goto eapre_e;
  } /* end T_ZFIN_UH */
  /*}}}  */

  /*{{{  {  WHEN-Konstruktor}*/
  if (T_PM_WHEN(x)) {
    /* Eine Case-Klausel. Auf E stehen jetzt das noch zu bearbeitende Pattern, dann der
       Guard und schliesslich der Rumpfausdruck. Da durch das Pattern evtl. Variablen
       gebunden werden, ist es sinnvoll schon jetzt einen Descriptor fuer diese
       Klausel anzulegen, indem dann die Namen der Variablen aufgenommen werden koennen. */
    /* Wie bei List-Comprehensions werden die Bindungen auf dem I-Stack gemerkt. Geschlossen
       wird eine Klausel allerdings erst im naechstumfassenden Lambda. */
  
    PUSHSTACK(S_i,SET_ARITY(PM_WHEN,0));
    PUSHSTACK(S_m,x);
    PUSHSTACK(S_m,x);
  
    /* Als naechstes geraten wir in das Pattern. Dort sind alle Vorkommen von Variablen
       definierend. Also muessen wir uns das merken. */
  
    within_pattern = TRUE;
  
    goto eapre_e;
  } /* PM_WHEN */
  /*}}}  */

  /*{{{  {  sonstige Konstruktoren}*/
  /* alle anderen konstruktoren */
  PUSHSTACK(S_m,x);
  PUSHSTACK(S_m,x);
  goto eapre_e;
  /*}}}  */

} /* end T_CON(x) */
/*}}}  */

  /*{{{  {  Descriptor}*/
  if (T_POINTER(x)) {
    PTR_DESCRIPTOR plrec;
  
    /*{{{  {  Variable innerhalb eines Patterns: definierendes Vorkommen}*/
    if (within_pattern && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME) ) {
      /* Eine Variable wird innerhalb eines Patterns angetroffen. Dieses Vorkommen ist definierend
         Die Variable wird nun auf dem I Stack gebunden. */
      y = POPSTACK(S_i);
      PUSHSTACK(S_i,x);
      PUSHSTACK(S_i,INC(y));
      PUSHSTACK(S_a,x);
      nbound++; /* damit die Variable als "lambda" gebunden gefunden wird. */
      nargs++; /* uh 13.4.92 */

      goto eapre_m;
    }
    /* Variable ausserhalb eines Patterns wie gewohnt behandeln... */
    /*}}}  */
  
    /*{{{  {  Variable}*/
    if (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME) {
      int bindings = 0; /* anzahl der (lambda) bindungen */
      int lrec     = 0; /* gesetzt falls letrec-variablen */
      /* bindendes vorkommen der variablen suchen */
      /*{{{  Variable suchen und finden: freie und relativ freie Variablen*/
      for (i = 0 ; (y = MIDSTACK(S_i,i)) != DOLLAR ; i++) {
      /* printf("eapre_e: Variablen  bindings=%d,nbound=%d, nargs=%d\n",bindings,nbound,nargs); */
        /*{{{  Konstruktor auf Stack i?*/
        if (T_CON(y)) {
          /* konstruktor auf i */
          if (T_LREC(y)) {
            /* LREC(n) p_lrec fnam_n .. fnam_1 | stack i */
            /* es folgen letrec-gebundene variablen (funktionsnamen) */
            /* davor steht aber ein pointer auf den LREC-descriptor */
            /* LREC(n) skippen und position von plrec (>0) merken */
            lrec = ++i;
            /* LREC-descriptor holen, dieser bleibt eine weile gueltig */
            plrec = (PTR_DESCRIPTOR)MIDSTACK(S_i,i); /* LREC-descriptor */
          }
          else lrec = 0;                       /* keine letrec-variablen */
        }
        /*}}}  */
        else {
          /* pointer auf name-descriptor */
          if (x == y) {
            /* bindende variable gefunden */
            /*{{{  Protected?*/
            if (T_PROTECT(READSTACK(S_m))) {
              /* geschuetzte variable, PROTECT abbauen und weitersuchen */
              PPOPSTACK(S_m);           /* PROTECT(1) */
              PPOPSTACK(S_m);           /* PROTECT(1) */
              bindings += !lrec;        /* ++ falls y keine letrec-variable */
            }
            /*}}}  */
            else
            /*{{{  lrec gebunden?*/
            if (lrec) {
              /* letrec-gebundene variable (funktionsname) */
              int index = R_LREC((*plrec),nfuncs) + lrec - i;
              PTR_DESCRIPTOR plri;        /* pointer auf LREC_IND-descriptor */
              plri = (PTR_DESCRIPTOR)R_LREC((*plrec),ptdv)[index];
              INC_REFCNT(plri);                      /* variable wird */
              PUSHSTACK(S_a,plri);                   /* durch plri ersetzt */
              DEC_REFCNT((PTR_DESCRIPTOR)x);         /* variable freigeben */
              break;
            } /* end if (lrec) */
            /*}}}  */
            else
            /*{{{  lambda gebunden?*/
            if (bindings < nbound) {
              /* (lambda) gebundene variable */
              PUSHSTACK(S_a,SET_ARITY(NUM,bindings));  /* ersetzen */
              DEC_REFCNT((PTR_DESCRIPTOR)x);           /* variable freigeben */
              break;
            }
            /*}}}  */
            else {
              /*{{{  relativ freie Variable gefunden*/
              /* relativ freie variable */
              y = READSTACK(S_m1);        /* SNAP(n) */
              /* nachschauen ob auf m1 bereits eine Bindung erzeugt wurde */
              /* Dazu den Stack M1 durchsuchen. Dort liegen y Variablen, */
              /* Die unterste muss die niedrigste Nummernvariable bekommen. */
              for (i = ARITY(y) ; i > 0 ; i--) {
                if (x == MIDSTACK(S_m1,i)) {
                  /* bindung schon vorhanden */
                  /* PUSHSTACK(S_a,SET_VALUE(NUM,nbound+i-1));  falsch */
                  /* Hier lag der Hase begraben................................. */
              
                  PUSHSTACK(S_a,SET_VALUE(NUM,nbound+ARITY(y)-i)); /* richtig 04Okt 91 */
                  DEC_REFCNT((PTR_DESCRIPTOR)x);     /* variable freigeben */
                  break;
                }
              }
              if (i == 0) {
                /* keine bindung auf m1, schliessen */
                PUSHSTACK(S_a,SET_ARITY(NUM,nargs)); /* neue nummernvariable */
                nargs++;                            /* zusaetzliches argument */
                y = READSTACK(S_m1);                /* SNAP(n) */
                WRITESTACK(S_m1,x);                 /* variable */
                PUSHSTACK(S_m1,INC(y));             /* SNAP(n+1) */
              }
              break;
              /*}}}  */
            }
          }
          else {
            /* bindende variable noch nicht gefunden */
            bindings += !lrec;        /* ++ falls y keine letrec-variable */
          }
        } /* end else */
      } /* end for */
      /*}}}  */
      /*{{{  Variable vergeblich suchen: absolut frei*/
      if (T_DOLLAR(y)) {
        /* keine bindung gefunden also eine absolut freie variable */
        int nprotect = 0;
        /* protects abbauen und zaehlen */
        while (T_PROTECT(READSTACK(S_m))) {
          PPOPSTACK(S_m);                 /* PROTECT(1) */
          PPOPSTACK(S_m);                 /* PROTECT(1) */
          nprotect++;
        }
        /* VAR-descriptor anlegen */
        MKDESC(px,1,C_CONSTANT,TY_VAR);
        L_VAR((*px),nlabar) = nprotect;
        L_VAR((*px),ptnd) = (PTR_DESCRIPTOR)x;
        PUSHSTACK(S_a,px);
      /*}}}  */
      } /* end T_DOLLAR(y) */
    } /* end if */
    /*}}}  */
  
    /*{{{  {  sonstiger Descriptor}*/
    else {
      /* sonstiger pointer */
      PUSHSTACK(S_a,x);
    }
    goto eapre_m;
    /*}}}  */
  
  } /* end T_POINTER(x) */
  /*}}}  */

  /*{{{  {  sonstige Stackelemente}*/
  /* sonstige stackelemente auf e */
  PUSHSTACK(S_a,x);
  goto eapre_m;
  /*}}}  */

/*}}}  */

/*{{{  {  EAPRE M-Linie}*/
eapre_m:

  x = READSTACK(S_m);

/*{{{  {  Konstruktor}*/
  if (T_CON(x)) {
    /* konstruktor auf m */

/*{{{  {  Prelist}*/
if (T_PRELIST(x)) {
  /* eine funktionsdefinition ist traversiert */
  PTR_DESCRIPTOR plrec;
  y = READSTACK(S_a);                   /* funktion untersuchen */
  if (T_SUB(y)) {
    y = MIDSTACK(S_a,1);                /* SUB-descriptor holen */
  }
  else {
    /* nullstellige funktion! darf nicht vorkommen! */
    post_mortem("eapre: nullary function");
  }
  /* den zugehoerigen LREC-descriptor von i holen */
  plrec = (PTR_DESCRIPTOR)MIDSTACK(S_i,1); /* LREC-descriptor */
  ph = R_LREC((*plrec),ptdv);              /* func list */
  i = R_LREC((*plrec),nfuncs) - ARITY(x);  /* index */
  /* funktion hinter LREC_IND-descriptor haengen */
  px = (PTR_DESCRIPTOR)ph[i];
  if (T_POINTER(y))                      /* dg 01.08.90 */
    INC_REFCNT((T_PTD)y);                /* dg 01.08.90 */
  L_LREC_IND((*px),ptf) = (PTR_DESCRIPTOR)y;
  if (ARITY(x) > 1) {
    /* falls kein SUB-konstruktor auf e, einen draufpacken */
    if (!T_SUB(READSTACK(S_e)))
      PUSHSTACK(S_e,SET_ARITY(SUB,1));
    WRITESTACK(S_m,DEC(x));
    goto eapre_e;
  }
  else {
    PPOPSTACK(S_m);
    PUSHSTACK(S_a,POPSTACK(S_m));
    PUSHSTACK(S_a,plrec);
    goto eapre_m;
  }
}
/*}}}  */

/*{{{  {  Stelligkeit > 1}*/
if (ARITY(x) > 1) {
  /* Konstruktor mit Stelligkeit groesser 1 */

  /*{{{  ZF-Konstruktor*/
  if (T_ZF_UH(x)) {
  
    if (ARITY(x)>2) {
      if (!T_ZFIN_UH(READSTACK(S_e))) {
        /* Der naechset Teilausdruck ist ein Filter -- ein einstelliges ZFINT zur Markierung erzeugen. */
  
        x=POPSTACK(S_m);  /* ZF merken */
  
         /* Jetzt muss der Applikator erzeugt werden) */
  
        PUSHSTACK(S_m,SET_ARITY(AP,2));
        PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,1));
  
        PUSHSTACK(S_m,DEC(x));   /* ZF-Konstruktor wieder oben drauf zum Zaehlen */
        goto eapre_e;
      }
      /* Generatoren werden selbst auf der M-Linie behandelt.  s.u. */
      WRITESTACK(S_m,DEC(x));
      goto eapre_e;
    }
  
    /* ARITY == 2  Nur noch Zielausdruck behandeln. */
    WRITESTACK(S_m,DEC(x));
    goto eapre_e;
  }
  /*}}}  */

  /*{{{  PM WHEN Konstruktor*/
  if (T_PM_WHEN(x)) {
    /* Nun ist ein Teil der Klausel abgearbeitet. Ganz bestimmt ist dann also das Pattern zuende. */
    within_pattern = FALSE;
    WRITESTACK(S_m,DEC(x));
    goto eapre_e;
  }
  /*}}}  */

  WRITESTACK(S_m,DEC(x));  /* jeder andere noch nicht abgelaufene Konstruktor */
  goto eapre_e;
}
/*}}}  */

/*{{{  {  Stelligkeit = 1}*/
    /* stelligkeit gleich 1 */
    x = POPSTACK(S_m);

/*{{{  {  String}*/
if (T_STRING(x)) {
  x=POPSTACK(S_m);
  PUSHSTACK(S_a,x);
  ST_LIST(x);
  PUSHSTACK(S_a,x);
  goto eapre_m;
}
/*}}}  */

/*{{{  {  SUB Konstruktor}*/
if (T_SUB(x)) {
  /* SUB-descriptor anlegen und initialisieren */
  MKDESC(px,1,C_EXPRESSION,TY_SUB);
  L_FUNC((*px),nargs) = nargs;
  L_FUNC((*px),special) = EXTR_EDIT(x);    /* editfeld */
  L_FUNC((*px),pte) = (PTR_HEAPELEM)0;
  /* heapplatz fuer die namensliste besorgen */
  /* inclusive platz fuer die relativ freien variablen */
  GET_HEAP(nargs+1,A_FUNC((*px),namelist));
  ph = R_FUNC((*px),namelist);
  *ph++ = nbound;   /* laenge der namensliste eintragen */
  PPOPSTACK(S_i);   /* SUB-konstruktor runter von i */

  /* die bindenden variablen von i in den heap */
  for (i = nbound ; i > 0 ; i--) {
    *ph++ = POPSTACK(S_i);
  }

  /* relativ freie variablen? */
  y = POPSTACK(S_m1);                        /* SNAP(n) */
  nfree = ARITY(y);
  if (nfree > 0) {
    /* funktion hat relativ freie variablen */
    /* relativ freie variablen in den heap */
    /* Aber bitte in richtiger Reihenfolge: */
    /* Die hoeheren Nummernvariablen stehen weiter oben im Stack */
    /* und muessen spaeter (auf hoeheren Adressen) im den Heap stehen. */
    /* ph zeigt jetzt auf den Anfang der freien Variablen innerhalb der */
    /* Variablenliste:  */
    /*                             ph o                               */
    /*                                |                               */
    /*                                V                               */
    /*  nbound | v0 v1 ... vnbound-1  vnbound ... vnbound+nfree-1 |   */
    /*                        INDEX:  0, 1, 2 ...        nfree-1      */

    /* printf("Da hat nfree den Wert %d\n",nfree); */
    for (i = nfree ; i > 0 ; i--) {
      /* *ph++ = POPSTACK(S_m1);  falsch */
      ph[i-1]=POPSTACK(S_m1);  /* richtig uh 29Oct91 */
    }
  }
  /* sonst (noch) keine relativ freie variablen */
  /* nbound restaurieren */
  nbound = ARITY(POPSTACK(S_m1));          /* PRELIST(n) | stack m1 */
  /* nargs restaurieren */
  nargs = nbound + ARITY(READSTACK(S_m1)); /* SNAP(n) .. | stack m1 */
  /* SUB-descriptor auf a */
  PUSHSTACK(S_a,px);
  PUSHSTACK(S_a,SET_ARITY(x,2));           /* SUB(2) */
  goto eapre_m;
} /* end T_SUB(x) */
/*}}}  */

/*{{{  {  LREC Konstruktor}*/
        if (T_LREC(x)) {
          PTR_DESCRIPTOR plrec;
          /* LREC-konstruktor runter von i */
          PPOPSTACK(S_i);
          /* LREC-descriptor runter von i */
          plrec = (PTR_DESCRIPTOR)POPSTACK(S_i);
          ph = R_LREC((*plrec),ptdv);
          /* die bindenden variablen runter von i */
          for (i = R_LREC((*plrec),nfuncs) ; --i >= 0 ; ) {
            PPOPSTACK(S_i);
/* #ifdef NotUsed */
/*             px = (PTR_DESCRIPTOR)ph[i]; */
/*             ph[i] = (T_HEAPELEM)R_LREC_IND((*px),ptf); */
/*             if (T_POINTER(ph[i])) */
/*               INC_REFCNT((PTR_DESCRIPTOR)ph[i]); */
/*             DEC_REFCNT(px); */
/* #endif NotUsed */
          }
          PUSHSTACK(S_a,SET_ARITY(x,3));       /* LREC(3) */
          goto eapre_m;
        } /* end T_LREC(x) */
/*}}}  */

/*{{{  {  REC Konstruktor}*/
if (T_REC(x))
  post_mortem("eapre: REC not implemented");
/*}}}  */

/*{{{  {  ZF-Generator Konstruktor}*/
if (T_ZFIN_UH(x)) {
       /* ZF-Generator! */
       /* Ein ZFIN Konstruktor findet nun den auf A traversierten Ausdruck und auf E die Variable. */
       /* Sie soll nun gebunden werden. */
       /* Die Bindung wirkt auf die noch folgenden Qualifier und den Zielausdruck der ZF-Expression. */
       /* Die Parameter, mit denen jetzt geschlossen werden muss, liegen auf Stack i unter dem SUB Konstruktor */

       /* Descriptor fuer "MAP"-Lambda anlegen. */
       int zfbound=0;
       int subbound=0;
       int nfree= ARITY(READSTACK(S_m1));
       int j;

       i=0;
       while ( T_ZF_UH(MIDSTACK(S_i,i)) ) {
         zfbound += ARITY(MIDSTACK(S_i,i));
         i += ARITY(MIDSTACK(S_i,i))+1;
       };

       if (T_SUB(MIDSTACK(S_i,i))) subbound = ARITY(MIDSTACK(S_i,i));

       MKDESC(zfpx,1,C_EXPRESSION,TY_ZF);
       L_ZF((*zfpx),nargs) = 1+zfbound+subbound+nfree;
       L_ZF((*zfpx),special) = EXTR_EDIT(x);        /* editfeld */
       L_ZF((*zfpx),pte) = (PTR_HEAPELEM)0;         /* wird in STORE besetzt. */
       GET_HEAP(1+zfbound+subbound+nfree+1,A_ZF((*zfpx),namelist));  /* Speicher fuer Parameter holen */
       ph = R_ZF((*zfpx),namelist);
       *ph++ = 1;                                 /* Anzahl der tatsaechlichen Parameter */
       *ph++ = READSTACK(S_e);                    /* der tatsaechliche Parameter */

       /* zusaetzliche ZF-gebundenene Parameter kopieren */
       j=1; i=0;
       while (i<zfbound) {
         if (!T_CON(MIDSTACK(S_i,j))) {
            *ph++ = MIDSTACK(S_i,j); i++;
         };
         j++;
       };

       /* Lambda gebundene Variablen: */
       for (i = j+subbound; i>j; i--) {
           *ph++ = MIDSTACK(S_i,i);
       }


       /* Relativ freie Variablen */
       for (i = 1; i<=nfree; i++) {
            *ph++ = MIDSTACK(S_m1,i);
       }

       PUSHSTACK(S_a,zfpx);

       x=POPSTACK(S_m);
       PUSHSTACK(S_m,SET_ARITY(AP,2));
       PUSHSTACK(S_m,SET_ARITY(ZFIN_UH,2));  /* Mit Konstruktor zusammenhalten! */
       PUSHSTACK(S_m,x);

       /* Den tatsaechlichen Parameter binden. */
       x=POPSTACK(S_i);
       PUSHSTACK(S_i,POPSTACK(S_e));
       PUSHSTACK(S_i,INC(x));

       nbound++;  /* uh 16Jul90 */
       nargs++;

       goto eapre_m;
};
/*}}}  */

/*{{{  {  ZF Konstruktor}*/
if (T_ZF_UH(x)) {
    /* Alle Qualifier und der Rumpfausdruck sind nun bearbeitet worden. Die gemachten Bindungen muessen nun */
    /* wieder geloest werden, die erzeugten Applikationen und 'MAP'-Lambdas inclusive der Variablen
    /* werden vom   Stack M nach Stack A bewegt.    */
    /* dann kann der ZF-Konstruktor mit Stelligkeit 1 oben auf die bearbeitete List-Comprehension */

    nbound -= ARITY(READSTACK(S_i));   /* uh 16Jul90 */
    for (i=ARITY(POPSTACK(S_i)); i>0; i--)  POPSTACK(S_i);
/* for (i=ARITY(POPSTACK(S_m1)); i>0;i--)  POPSTACK(S_m1); kein Antasten des m1 Stacks.
   Relativ freie Variablen werden in der naechstumfassenden Funktion geschlossen. uh 02Aug90 */

    while (!T_ZF_UH(READSTACK(S_m))) PUSHSTACK(S_a,POPSTACK(S_m));

    PUSHSTACK(S_a,POPSTACK(S_m));

    nargs = nbound + ARITY(READSTACK(S_m1));

    /*********** Hier muss evtl fuer PROBLEM 6 nargs wieder richtig gesetzt werden ! *****/


    goto eapre_m;
};
/*}}}  */

/*{{{  {  WHEN Konstruktor}*/
if (T_PM_WHEN(x)) {
  T_PTD patpx;
  PTR_HEAPELEM patvar;
  int j;
  /* Pattern Match Klausel. Nun liegen auf A die Teile (Pattern, Guard, Rumpfausdruck) der Klausel.
     Es wird nun Zeit den Descriptor fuer die Klausel anzulegen. */

     MKDESC(pmpx,1,C_MATCHING,TY_CLAUSE);

     GET_HEAP(3,A_CLAUSE(*pmpx,sons));  /* Platz fuer das Pattern,Guard,Body Triple */

     L_CLAUSE(*pmpx,sons)[1] = NULL; /* guard wird in STORE besetzt */
     L_CLAUSE(*pmpx,sons)[2] = NULL; /* body wird in STORE besetzt */

     /* Der Patternzeiger Zeiger zeigt auf einen SUB-Descriptor, */
     /* der die Liste der vom pattern gebundenen Variablen und das */
     /* Pattern selbst zusammenhaelt. */
     MKDESC(patpx,1,C_EXPRESSION,TY_SUB);

     L_CLAUSE(*pmpx,sons)[0] = (int)patpx;
         /* Der Descriptor wird in STORE weiter besetzt. */

     PUSHSTACK(S_a,pmpx);
     PUSHSTACK(S_a,INC(POPSTACK(S_m)));

     /* Fuer das Schliessen duerfen die Variablen hier nicht
        wegfallen, da sie ja durch die Klausel gebunden werden. */

     i=ARITY(POPSTACK(S_i));
     GET_HEAP(nbound+1,A_FUNC(*patpx,namelist));

     L_FUNC(*patpx,nargs)=nbound;

     patvar=R_FUNC(*patpx,namelist);
     *patvar++=i;

     for (; i>0 ; i--) {
        *patvar++=POPSTACK(S_i);
        nbound--;
     }

     /* nun noch die lambda gebundenen Variablen eintragen. */
     /* der Match-Descriptor enthaelt dann alle Bindungen im */
     /* aktuellen Stackframe */

     j=0;
     for (i=nbound; i>0; i--) {
        while (T_CON(MIDSTACK(S_i,j))) j++;
        *patvar++= MIDSTACK(S_i,j);
     }

     nargs = nbound + ARITY(READSTACK(S_m1)); /* SNAP(n) .. | stack m1 */

     goto eapre_m;

} /* PM_WHEN */
/*}}}  */

/*{{{  {  sonstige Konstruktoren}*/
/* sonstige konstruktoren auf m */
PUSHSTACK(S_a,POPSTACK(S_m));
goto eapre_m;
/*}}}  */

/*}}}  */

  } /* end T_CON(x) */
/*}}}  */

/*{{{  {  Endesymbol KLAA}*/
if (T_KLAA(x)) {
  /* bottomsymbol erreicht */
  PPOPSTACK(S_m);               /* KLAA */
  PPOPSTACK(S_i);               /* DOLLAR */
  PPOPSTACK(S_m1);              /* DOLLAR */
}
/*}}}  */

/*{{{  {  sonstiges Stackelement}*/
else post_mortem("eapre: no match on stack m");
/*}}}  */
/*}}}  */

  END_MODUL("eapre");
/*}}}  */

/*{{{  {  AEPRE}*/
/* ------------------------------------------------------------------- */
/* aepre:                                                              */
/* der partiell preprocesste ausdruck wird von a nach e traversiert    */
/* und dabei wird auf i eine tabelle angelegt die fuer das schliessen  */
/* benoetigt wird. */
/* ------------------------------------------------------------------- */

  START_MODUL("aepre");
  PUSHSTACK(S_m,KLAA);

/*{{{  {  AEPRE A-Linie}*/
aepr_a:
  x = POPSTACK(S_a);

  /*{{{  {  Konstruktor}*/
  if (T_CON(x)) {
  
    /*{{{  {  SUB Konstruktor}*/
    if (T_SUB(x)) {
      /* eine funktion soll traversiert werden */
      PUSHSTACK(S_m,POPSTACK(S_a));  /* SUB-descriptor */
      PUSHSTACK(S_m,DEC(x));         /* SUB(1) */
      PUSHSTACK(S_i,DOLLAR);         /* endemarkierung auf v */
    }
    else
    /*}}}  */
  
    /*{{{  {  ZF Generator Konstruktor}*/
    if (T_ZFIN_UH(x)) {
      /* Zf-Generator */
      PUSHSTACK(S_m,DEC(x));
      PUSHSTACK(S_i,DOLLAR);
    }
    else
    /*}}}  */
  
    /*{{{  {  WHEN Konstruktor}*/
    if (T_PM_WHEN(x)) {
      PUSHSTACK(S_m,POPSTACK(S_a));
      PUSHSTACK(S_m,DEC(x));
      PUSHSTACK(S_i,DOLLAR);
    }
    /*}}}  */
  
    /*{{{  {  sonstiger Konstruktor}*/
    else {
      /* sonstiger konstruktor auf a, dieser wird zweifach auf m gepusht */
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m,x);
    }
    goto aepr_a;
    /*}}}  */
  }
  /*}}}  */

  /*{{{  {  Descriptor}*/
  if (T_POINTER(x)) {
    px = (PTR_DESCRIPTOR)x;
    if (R_DESC((*px),type) == TY_LREC_IND
    ||  R_DESC((*px),type) == TY_SUB) {
      PUSHSTACK(S_i,x);
    }
  }
  /*}}}  */

  /*{{{  {  sonstiges Stackelement}*/
  PUSHSTACK(S_e,x);
  goto aepr_m;
  /*}}}  */
/*}}}  */

/*{{{  {  AEPRE M-Linie}*/
aepr_m:
  x = READSTACK(S_m);

  /*{{{  {  Konstruktor}*/
  if (T_CON(x)) {
  
    /*{{{  {  Stelligkeit > 1}*/
    if (ARITY(x) > 1) {
      WRITESTACK(S_m,DEC(x));
      goto aepr_a;
    }
    /*}}}  */
  
    /*{{{  {  Stelligkeit = 1}*/
    /* sonst ARITY(x) == 1 */
    
    /*{{{  {  ZF-Generator Konstruktor}*/
    if (T_ZFIN_UH(x)) {
      PUSHSTACK(S_i,READSTACK(S_a));  /* ZF-Descriptor */
      PUSHSTACK(S_e,POPSTACK(S_a));   /* dito */
      PUSHSTACK(S_e,INC(x));          /* ZF_IN(2) */
      POPSTACK(S_m);
      goto aepr_m;
    }
    /*}}}  */
    
    /*{{{  {  WHEN Konstruktor}*/
    if (T_PM_WHEN(x)) {
      POPSTACK(S_m);                 /* WHEN-Constr */
      PUSHSTACK(S_i,READSTACK(S_m)); /* WHEN-Descr  */
      PUSHSTACK(S_e,POPSTACK(S_m));  /* WHEN-Descr  */
      PUSHSTACK(S_e,SET_ARITY(x,4));
      goto aepr_m;
    }
    /*}}}  */
    
    /*{{{  {  sonstiger Konstruktor}*/
    PPOPSTACK(S_m);                   /* konstruktor */
    PUSHSTACK(S_e,POPSTACK(S_m));     /* konstruktor oder pointer */
    if (T_SUB(x)) {
      PUSHSTACK(S_i,READSTACK(S_e));  /* SUB-descriptor */
      PUSHSTACK(S_e,INC(x));          /* SUB(2) */
    }
    goto aepr_m;
    /*}}}  */
    
    /*}}}  */
  
  } /* end T_CON(x) */
  /*}}}  */

  /*{{{  {  Endesymbol KLAA}*/
  /* jetzt muss ein KLAA gefunden werden */
  if (T_KLAA(x)) {
    PPOPSTACK(S_m);       /* KLAA */
  }
  /*}}}  */

  /*{{{  {  sonstiges Stackelement}*/
  else post_mortem("aepre: invalid symbol on stack m");
  /*}}}  */
/*}}}  */

  END_MODUL("aepre");
/*}}}  */

/*{{{  {  CLOSE}*/
/* ---------------------------------------------------------------- */
/* close:                                                           */
/* durchlaufen der tabelle auf stack i und funktionen abschliessen, */
/* solange bis keine freien variablen mehr gefunden werden          */
/* ---------------------------------------------------------------- */

  START_MODUL("close");

PUSHSTACK(S_a,DOLLAR);
  do {
    PTR_DESCRIPTOR psub;
    nfree = 0;
    for (i = 0 ; i < SIZEOFSTACK(S_i) ; i++) {
      x = MIDSTACK(S_i,i);
      psub = (PTR_DESCRIPTOR)x;
    WRITESTACK(S_a,x); /* printf("i=%d\n",i); */
      /*{{{  SUB oder ZF-Descriptor   ... continue*/
      if (T_POINTER(x) && ((R_DESC((*psub),type) == TY_SUB) ||
                           (R_DESC((*psub),type) == TY_ZF) )) {
        int j;
        nargs = R_FUNC((*psub),nargs);
        ph = R_FUNC((*psub),namelist);
        /* die bindenden variablen auf m1 */
        for (j = nargs ; j > 0 ; j--)
          PUSHSTACK(S_m1,*++ph);
        /* und ein SNAP drueber */
        PUSHSTACK(S_m1,SET_ARITY(SNAP,nargs));
        PUSHSTACK(S_m,psub);
        continue;
      }
      /*}}}  */
      /*{{{  CLAUSE-Descriptor        ... continue*/
      if (T_POINTER(x) && (R_DESC(*psub,type) == TY_CLAUSE)) {
         T_PTD ptsub=(T_PTD)(R_CLAUSE(*psub,sons)[0]);
         int j;
         nargs = R_FUNC((*ptsub),nargs);
         ph = R_FUNC((*ptsub),namelist);
         /* die in der Klausel gebundenen Variablen auf m1 */
         for (j = nargs; j > 0; j--)
            PUSHSTACK(S_m1,*++ph);
         /* und ein SNAP drueber */
         PUSHSTACK(S_m1,SET_ARITY(SNAP,nargs));
         PUSHSTACK(S_m,ptsub);
         continue;
      }
      /*}}}  */
      /*{{{  Dollar*/
      if (T_DOLLAR(x)) {
        /* endemarkierung erreicht */
        x = POPSTACK(S_m);
        psub = (PTR_DESCRIPTOR)x;
        nargs = ARITY(POPSTACK(S_m1));        /* SNAP(n) von m1 */
       /* printf("nargs=%d, func_nargs=%d",nargs, R_FUNC((*psub),nargs)); */
        if ((R_DESC(*psub,type) == TY_SUB) && (nargs > R_FUNC((*psub),nargs))) {
          nbound = R_FUNC((*psub),namelist)[0]; /* uh 15Oct91 */
          freeheap(R_FUNC((*psub),namelist));
          GET_HEAP(nargs+1,A_FUNC((*psub),namelist));
          L_FUNC((*psub),nargs) = nargs;
          ph = R_FUNC((*psub),namelist);
          ph[0] = nbound;
          for ( ; nargs > 0 ; nargs--)
            ph[nargs] = POPSTACK(S_m1);
        }
        else {
          /* bindende variablen von m1 entfernen */
          for (; nargs > 0 ; nargs--)
            PPOPSTACK(S_m1);
        }
      }
      /*}}}  */
      /*{{{  LREC-Ind Descriptor*/
      if (T_POINTER(x) && R_DESC((*psub),type) == TY_LREC_IND) {
        psub = R_LREC_IND((*psub),ptf);
        x = (STACKELEM)psub;
      }
      /*}}}  */
      /*{{{  SUB-Descriptor*/
      if (T_POINTER(x) && R_DESC((*psub),type) == TY_SUB) {
        int freev;
        /* printf("%d sub-Descriptor! \n",i); */
        nbound = R_FUNC((*psub),namelist)[0];
        freev = R_FUNC((*psub),nargs) - nbound;
        ph = R_FUNC((*psub),namelist) + 1 + nbound ;
        for (; freev > 0 ; freev--) {
          int k;
          x = *ph++;
          k = ARITY(READSTACK(S_m1));
          for (; k > 0 ; k--) {
            if (x == MIDSTACK(S_m1,k))
              break;
          } /* end for k */
          if (k == 0) {
            /* freie variable gefunden */
            y = READSTACK(S_m1);          /* SNAP(n) */
            WRITESTACK(S_m1,x);
            PUSHSTACK(S_m1,INC(y));        /* SNAP(n+1) */
            nfree++;
          }
        } /* end for freev */
      }
      /*}}}  */
      /*{{{  the rest*/
      else
      if (!(T_POINTER(x) && R_DESC((*psub),type) == TY_ZF)
                         && R_DESC((*psub),type) == TY_CLAUSE) {
        /* parameterlose funktion */
        post_mortem("close: nullary function");
      }
      /*}}}  */
    } /* end for i */
  } while (nfree);

POPSTACK(S_a);

  /* i-stack abraeumen */
  for ( i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    PPOPSTACK(S_i);
  }
  END_MODUL("close");
/*}}}  */

/*{{{  {  STORE}*/
  /* ------------------------------------------------------------ */
  /* store: auslagern                                             */
  /*                                                              */
  /*                                                              */
  /* ------------------------------------------------------------ */


  START_MODUL("store");

  PUSHSTACK(S_m,KLAA);
  px = (PTR_DESCRIPTOR)0;

/*{{{  {  STORE E-Linie}*/
east_e:
  x = POPSTACK(S_e);

  /*{{{  {  Konstruktoren}*/
  if (T_CON(x)) {
    /*{{{  {  SUB Konstruktor}*/
    if (T_SUB(x)) {
      /* zeiger auf umgebende funktion initialisieren (px) */
      PUSHSTACK(S_m,px);                  /* px auf m! retten */
      px = (PTR_DESCRIPTOR)POPSTACK(S_e); /* SUB-descriptor */
      PUSHSTACK(S_m,DEC(x));           /* SUB(1) */
      goto east_e;
    }
    /*}}}  */
    /*{{{  {  ZF Konstruktor}*/
    if (T_ZFINT_UH(x)) {
      /* ZFINT gefunden => Descriptor fuer Filter anlegen. */
      MKDESC(zfpx,1,C_EXPRESSION,TY_ZF);
      L_ZF((*zfpx),nargs) = 1;
      L_ZF((*zfpx),special) = EXTR_EDIT(x);    /* editfeld */
      L_ZF((*zfpx),pte) = (PTR_HEAPELEM)0;
      L_ZF((*zfpx),namelist) = (PTR_HEAPELEM)0; 
      PUSHSTACK(S_m,zfpx);
      PUSHSTACK(S_m,x);
      goto east_e;
    }
    /*}}}  */
    /*{{{  {  ZF Generator Konstruktor}*/
    if (T_ZFIN_UH(x)) {
      /* ZF-Generator */
      /* Descriptor wurde schon in EAPRE angelegt - namensliste dort schon bearbeitet. */
      /* Dieser Descriptor liegt jetzt oben auf E */
      PUSHSTACK(S_m,px);                  /* px auf m! retten */
      px = (PTR_DESCRIPTOR)POPSTACK(S_e); /* ZF-descriptor */
      PUSHSTACK(S_m,DEC(x));
      goto east_e;
    }
    /*}}}  */
    /*{{{  {  WHEN Konstruktor}*/
    if (T_PM_WHEN(x)) {
      /* Eine Klausel */
      /* Auf E liegt nun der in EAPRE erzeugte WHEN-Descriptor, der spaeter noch Verweise
         auf das Pattern, den Guard und den Rumpfausdruck bekommen soll. */
      /* Er wird zur spaeteren Verwendung auf M gelegt. */
      T_PTD ptmatch=(T_PTD)POPSTACK(S_e);
    
      PUSHSTACK(S_m,px);
      PUSHSTACK(S_m,ptmatch);                   /* Match Descriptor */
      px = (T_PTD)(R_CLAUSE(*ptmatch,sons)[0]); /* Sub Descriptor  */
    
      PUSHSTACK(S_m,DEC(x));          /* Konstruktor oben drauf  */
      /* Auf E steht nun das Pattern, der Guard und dann der Rumpfausdruck */
      within_pattern=TRUE;
    
      goto east_e;
    }
    /*}}}  */
  
    /*{{{  {  sonstige Konstruktoren}*/
    /* Andere Konstruktoren */
    PUSHSTACK(S_m,x);
    PUSHSTACK(S_m,x);
    goto east_e;
    /*}}}  */
  }
  /*}}}  */

  /*{{{  {  PM END }*/
  if (T_PM_END(x)) {
     PUSHSTACK(S_a,(STACKELEM)NULL); /* Ende Markierung in der Klauselkette ist 0 */
     goto east_m;
  }
  /*}}}  */

  /*{{{  {  PM Skip}*/
  /*  if (T_PM_SKIP(x)) { /*
     /* PM skip */
     /* Auf E liegt der SKIP-Konstruktor. Er wird nun durch einen SKIP Descriptor
        ersetzt. */
      /* post_mortem("store: Skip Konstruktor!!!");       */
      /* goto east_m;*/
     /*} */
  /*}}}  */

  /*{{{  {  LREC IND Descriptor}*/
  if (T_POINTER(x) && R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_LREC_IND) {
    PTR_DESCRIPTOR psub = R_LREC_IND((*(PTR_DESCRIPTOR)x),ptf);
    if (T_POINTER((int)psub) && R_DESC((*psub),type) == TY_SUB) {
      nargs = R_FUNC((*psub),nargs);
      nbound = R_FUNC((*psub),namelist)[0];
      /* schliessen d.h. SNAP vor die funktion packen */
      if (nargs > nbound) {
        /* die funktion hat relativ freie variablen */
        register int j;
        nfree = nargs - nbound;
        /* for (i = nargs ; i > nbound ; i--) { /* dg 30.07.90 Korrektur war falsch */
        for (i = nbound+1 ; i <= nargs ; i++) { /* dg 25.10.91 */
          y = R_FUNC((*psub),namelist)[i];
          for (j = R_FUNC((*px),nargs) ; j > 0 ; j--) {
            if (y == R_FUNC((*px),namelist)[j]) {
              PUSHSTACK(S_a,SET_VALUE(NUM,j-1));
              break;
            }
          }
          /* sicherheithalber abfragen of das schliessen geklappt hat */
          if (j == 0)
            post_mortem("store_E: LREC_IND: free variable encountered");
        }
        PUSHSTACK(S_a,x);
        x = SET_ARITY(SNAP,nfree + 1);
      }
    }
  }
  /*}}}  */

  /*{{{  {  sonstiges Stackelement}*/
  /* sonst */
  PUSHSTACK(S_a,x);
  goto east_m;
  /*}}}  */

/*}}}  */

/*{{{  {  STORE M-Linie}*/
east_m:

  x = READSTACK(S_m);

  /*{{{  {  Konstruktoren}*/
    if (T_CON(x)) {
      /* konstruktor auf m */
      /*{{{  {  Stelligkeit > 1}*/
      if (ARITY(x) > 1) {
        if (T_PM_WHEN(x)) within_pattern=FALSE;
        WRITESTACK(S_m,DEC(x));
        goto east_e;
      }
      /*}}}  */
      /*{{{  {  Stelligkeit = 1}*/
      /* ARITY(x) == 1 */
      PPOPSTACK(S_m);                          /* konstrutor(1) */
      
      /*{{{  {  SUB Konstruktor}*/
      if (T_SUB(x)) {
        PTR_DESCRIPTOR psub = px;              /* SUB-descriptor */
        px   = (PTR_DESCRIPTOR)POPSTACK(S_m);  /* umgebende funktion */
        /*{{{  {  Funktion ist nullstellig}*/
        if (R_FUNC((*psub),nargs) == 0) {
        /* !!! */
          /* nullstellige funktion, das SUB-konstrukt muss wieder entfernt werden */
          DEC_REFCNT(psub);   /* uh 12 Mar 92 */
          /* post_mortem("Hier wird der Sub-Descriptor nicht freigegeben!"); */
          if (T_CON(READSTACK(S_a))) {
            MKDESC(psub,1,C_EXPRESSION,TY_EXPR); /* neuer descriptor */
            ST_EXPR(psub);                          /* ausdruck auslagern */
            PUSHSTACK(S_a,psub);
          }  
        /* !!! */
        }
        else {
        /*}}}  */
        /*{{{  {  Funktion ist mehrstellig}*/
          ST_FUNC(psub);                         /* funktion auslagern */
          /* falls funktionsdefinition eines letrecs, nicht schliessen! */
          if (T_PRELIST(READSTACK(S_m))) {
            PUSHSTACK(S_a,psub);
          }
          else {
            /* evtl. schliessen d.h. SNAP vor die funktion packen */
            nargs = R_FUNC((*psub),nargs);
            nbound = R_FUNC((*psub),namelist)[0];
            if (nargs > nbound) {
              /* die funktion hat relativ freie variablen */
              register int j;
              nfree = nargs - nbound;
        
              /* Struktur der Namensliste:
              /*               <--- gebundene --><-------- freie ----------->
              /* Index  0        1       nbound   nbound+1     nbound+nfree
              /*      nbound  | v0 ... vnbound-1 vnbound ... vnbound+nfree-1 |*/
              /*               <------------ nargs Variablen ---------------> */
              /*                #0 #1 ...        #nbound     #nbound+nfree-1  */
        
              /* freie Variablen in der umgebenden Funktion suchen, um */
              /* passenden Index fuer SNAP zu finden.                  */
        
              /* for (i = nargs ; i > nbound ; i--) { /* dg 30.07.90 Korrektur war falsch */
              for (i = nbound+1 ; i <= nargs ; i++) { /* dg 25.10.91 */
                /* fuer jede freie Variable y Bindungsindex ermitteln: */
                y = R_FUNC((*psub),namelist)[i];
        
                for (j = R_FUNC((*px),nargs) ; j > 0 ; j--) {
                  if (y == R_FUNC((*px),namelist)[j]) {
                    PUSHSTACK(S_a,SET_VALUE(NUM,j-1));
                    break;
                  }
                }
                /* sicherheithalber abfragen ob das schliessen geklappt hat */
                if (j == 0) {
                  /* d.h. gesamte Variablenliste der umgebenden Funktion */
                  /* erfolglos durchsucht. */
                  printf("umfassende Funktion=%x\n",px);
                  printf("aktuelle Funktion  =%x\n",psub);
                  printf("freie Variable     =%x\n",y);
                  post_mortem("store_M,SUB: free variable encountered");
                }
              }
              PUSHSTACK(S_a,psub);
              PUSHSTACK(S_a,SET_ARITY(SNAP,nfree + 1));
            }
            else PUSHSTACK(S_a,psub);
          }
        /*}}}  */
        }
        goto east_m;
      } /* end T_SUB(x) */
      /*}}}  */
      
      /*{{{  {  interner ZF Konstruktor}*/
      if (T_ZFINT_UH(x)) {
        /* ZF Filter  -- jetzt steht auf M der schon fertige Descriptor. */
        /* Auf A ist der vorbereitete Ausdruck zum Auslagern. */
        /* Diesen Ausdruck auslagern und in das PTE-Feld des Descriptors eintragen. */
        PTR_DESCRIPTOR pzf=(PTR_DESCRIPTOR)POPSTACK(S_m);
        ST_EXPR(pzf);
        PUSHSTACK(S_a,pzf);
        goto east_m;
      }
      /*}}}  */
      
      /*{{{  {  ZF-Generator Konstruktor}*/
      if (T_ZFIN_UH(x)) {
       /* Generator -- Ausdruck auslagern keine SNAPS davorsetzten */
        PTR_DESCRIPTOR pzf=px;
        px = (PTR_DESCRIPTOR)POPSTACK(S_m);
        ST_EXPR(pzf);
        PUSHSTACK(S_a,pzf);
        goto east_m;
      }
      /*}}}  */
      
      /*{{{  {  ZF Konstruktor}*/
      if (T_ZF_UH(x)) {
       /* Gesamter ZF-Lonstruktor -- List Comprehension hinter Descriptor verstecken! */
       PTR_DESCRIPTOR pzf;
       /* Der Konstruktor in Orginalstelligkeit liegt noch auf M */
       POPSTACK(S_m);
       MKDESC(pzf,1,C_EXPRESSION,TY_ZF);
       L_ZF(*pzf,special)=ZF_START;  /* Markiere als ZF-Start-Descriptor */
       L_ZF(*pzf,nargs)= 0;
       /* L_ZF(*pzf,namelist)= px ? R_FUNC(*px,namelist) : 0; */
       if (px) { /* uh 12.3.92 */
          L_ZF(*pzf,namelist)= R_FUNC(*px,namelist);
       } else {
          GET_HEAP(1,A_ZF(*pzf,namelist));
          R_ZF(*pzf,namelist)[0]=0;
       }

       ST_EXPR(pzf);
       PUSHSTACK(S_a,pzf);
       goto east_m;
      }
      /*}}}  */
      
      /*{{{  {  WHEN Konstruktor}*/
      if (T_PM_WHEN(x)) {
        /* Klausel: Auf A stehen jetzt die Teile der Klausel, die jetzt in den
           Heap ausgelagert werden und ihre Zeiger in den WHEN-Descriptor
           eingetragen werden sollen, der oben auf M steht
           Dann soll die Adresse des Descriptors fuer die
           Klausel auf A gelegt werden. */
      
        PTR_DESCRIPTOR pmpx=(PTR_DESCRIPTOR)POPSTACK(S_m); /* Match desc */
        PTR_DESCRIPTOR patpx=(PTR_DESCRIPTOR)(R_CLAUSE(*pmpx,sons)[0]); /* SUB-Desc */
        PTR_DESCRIPTOR guardpx;
        PTR_DESCRIPTOR bodypx;
      
        MKDESC(guardpx,1,C_EXPRESSION,TY_EXPR);
        MKDESC(bodypx,1,C_EXPRESSION,TY_EXPR);
      
        PTRAV_A_HILF; mvheap(A_EXPR(*bodypx,pte));    /* Rumpfausdruck */
        PTRAV_A_HILF; mvheap(A_EXPR(*guardpx,pte));   /* Guard */
        PTRAV_A_HILF; mvheap(A_FUNC(*patpx,pte));     /* Pattern */
      
        L_CLAUSE(*pmpx,sons)[1] = (int)guardpx;
        L_CLAUSE(*pmpx,sons)[2] = (int)bodypx;
      
        px = (PTR_DESCRIPTOR) POPSTACK(S_m);
      
        PUSHSTACK(S_a,pmpx);
        goto east_m;
      }
      /*}}}  */
      
      /*{{{  {  CASE Konstruktor}*/
      if (T_PM_CASE(x)) {
         /* CASE: Auf A stehen jetzt die einzelnen Klauseln des CASE-
            Ausdrucks. Sie muessen jetzt hinter einem Case-Descriptor
            versteckt werden. */
         PTR_DESCRIPTOR pmpx;
         PTR_DESCRIPTOR clausepx;
         PTR_DESCRIPTOR otherwisepx;
      
         x=POPSTACK(S_m);  /* CASE Konstruktor in Orginalstelligkeit */
      
         /* CASE Descriptor erzeugen. */
         MKDESC(pmpx,1,C_MATCHING,TY_SELECTION);
         L_SELECTION(*pmpx,otherwise) = NULL;  /* uh 2.4.92 */
 
         if (ARITY(x)) {     /* Wenn Klauseln da */
           i = ARITY(x)-1;
      
           /* Otherwise da? */
           if (T_PM_OTHERWISE(READSTACK(S_a))) {
             POPSTACK(S_a);
             MKDESC(otherwisepx,1,C_EXPRESSION,TY_EXPR);
             PTRAV_A_HILF; mvheap(A_EXPR(*otherwisepx,pte)); /* otherwise ausdruck */
             L_SELECTION(*pmpx,otherwise)=otherwisepx;
             i--;                                   /* eine Klausel weniger */
           }
      
           for (; i>0; i--) {
             clausepx=(T_PTD)POPSTACK(S_a);          /* Klauseln verketten */
             L_CLAUSE(*(T_PTD)READSTACK(S_a),next)=clausepx;
           }
           L_SELECTION(*pmpx,clauses) = (T_PTD)POPSTACK(S_a); /* erste Klausel */
         }
      
         /* Anzahl der Klauseln  (incl. OTHERWISE, excl. END) eintragen */
         L_SELECTION(*pmpx,nclauses) = (L_SELECTION(*pmpx,otherwise)==NULL) ? ARITY(x)-1 : ARITY(x);
      
         PUSHSTACK(S_a,pmpx);
         goto east_m;
      }
      /*}}}  */
      
      x = POPSTACK(S_m);          /* Konstruktor */
      
      /*{{{  {  LREC Konstruktor}*/
      if (T_LREC(x)) {
        PUSHSTACK(S_a,DEC(x));     /* LREC(2) auf a */
        goto east_m;
      } /* end T_LREC(x) */
      /*}}}  */
      
      /*{{{  {  CONS Konstruktor}*/
      if (T_COND(x)) {
        PUSHSTACK(S_a,x);
        ST_COND(y);
        PUSHSTACK(S_a,y);
        goto east_m;
      } /* end T_COND(x) */
      /*}}}  */
      
      /*{{{  {  PRELIST Konstruktor}*/
      if (T_PRELIST(x)) {
        /* definitionen in den LREC-descriptor und in */
        /* die LREC_IND-descriptoren einbauen */
        register PTR_DESCRIPTOR plrec,psub;
        i = ARITY(x);                           /* anzahl der funktionen */
        plrec = (PTR_DESCRIPTOR)READSTACK(S_e); /* LREC-descriptor */
        ph = R_LREC((*plrec),ptdv);             /* liste der definitionen */
        /* die LREC_IND-descriptoren wieder rausnehmen */
        for (; --i >= 0 ; ) {
          psub = (PTR_DESCRIPTOR)POPSTACK(S_a);   /* funktionsdefinition */
          plrec = (PTR_DESCRIPTOR)ph[i];          /* LREC_IND-descriptor */
          if (psub != R_LREC_IND((*plrec),ptf)) {
            DEC_REFCNT(R_LREC_IND((*plrec),ptf)); /* funktion frei */
            if (T_POINTER((int)psub))
              INC_REFCNT(psub);
            L_LREC_IND((*plrec),ptf) = psub;      /* neue funktion eintragen */
          }
          ph[i] = (T_HEAPELEM)psub;               /* in definitionsliste */
          DEC_REFCNT(plrec);                      /* LREC_IND freigeben */
        }
        PUSHSTACK(S_a,POPSTACK(S_e));             /* LREC-descriptor */
        WRITESTACK(S_m,SET_ARITY(READSTACK(S_m),1));   /* LREC(1) */
        goto east_e;
      /* #ifdef NotUsed */
        /* plrec und LREC-konstruktor wegwerfen */
      /*   PPOPSTACK(S_e); */
      /*   PPOPSTACK(S_m); */
      /*   PPOPSTACK(S_m); */
      /*   goto east_e; */
      /* #endif NotUsed */
      }
      /*}}}  */
      
      /*{{{  {  LIST Konstruktor}*/
      if (T_LIST(x)) {
        /* gewoehnliche liste */
        PUSHSTACK(S_a,x);                    /* LIST(n) */
        /* auslagern moeglich? (vgl. check_list_for_store) */
        for (i = 0 , y = ARITY(x) ; ++i <= y ; ) {
          x = MIDSTACK(S_a,i);
          /* gruende um nicht auszulagern */
          if (T_CON(x)) break;
          if (T_NUM(x)) break;
          if (T_POINTER(x)
           && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_SUB
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_LREC_IND
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_COND
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_SWITCH
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_REC
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NOMAT
            || R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_ZF))
            break;
        }
        if ((i > y)|| within_pattern) {
          /* es darf ausgelagert werden */
          ST_LIST(x);
          PUSHSTACK(S_a,x);
        }
        goto east_m;
      } /* end T_LIST(x) */
      /*}}}  */
      
      /*{{{  {  sonstige Konstruktoren}*/
      /* sonstige konstruktoren auf m */
      PUSHSTACK(S_a,x);
      goto east_m;
      /*}}}  */
      
      /*}}}  */
    } /* end T_CON(x) */
  /*}}}  */

  /*{{{  {  Endesymbol KLAA}*/
  if (T_KLAA(x)) {
    /* ausdruck vollstaendig auslagern */
    if ((x = st_expr()) == 0)
      post_mortem("store: heap overflow (st_expr)");
    PUSHSTACK(S_e,x);
    PPOPSTACK(S_m);               /* KLAA */
  }
  /*}}}  */

  /*{{{  {  sonstiges Stacklement}*/
  else post_mortem("store: invalid symbol on stack m");
  /*}}}  */
/*}}}  */

  END_MODUL("store");
/*}}}  */

  END_MODUL("preproc");

} /* end preproc */
/*}}}  */

/* end of preproc.c */

