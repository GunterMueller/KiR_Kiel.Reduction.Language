/* $Log: rr1pr.c,v $
 * Revision 1.3  1993/09/01 12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:40  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  rr1pr.c  - external:  r1pr  , rr1pr ;
 *     Aufgerufen durch:  subfunc,recfunc;
 *  rr1pr.c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */

/* last update  by     why

   copied       by     to

 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"


/* ach 29/10/92 */
extern void stack_error();            /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
extern void test_dec_refcnt();
#endif
/* end of ach */

/*----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *----------------------------------------------------------------------------
 */
extern void ptrav_a_e();           /* trav.c     */
extern T_HEAPELEM *newbuff();
extern void freeheap();

/*------ r1pr -----------------------------------------------------------------
 *  Transformiert alle Variablen hinter einem SUB in Nummernvariable
 *  Aufrufsituation: SUB v1..vn expr  auf S_e
 *  globals--  S_e, S_a, (nur intern: S_Hilf)
 *-----------------------------------------------------------------------------
 */
 void r1pr(name)
 STACKELEM * name;        /* zeiger auf die namensliste       */
 {
    register int i,j,nbound,m,n;
    int * anzahl;         /* zeiger auf ein feld, in dem die anzahl innerer  */
    STACKELEM x,y;        /* bindungen steht                                 */
    STACKELEM z;

    START_MODUL("r1pr");

    x = POPSTACK(S_e);                      /* SUB                     */
    nbound = ARITY(x) - 1;                  /* anzahl gebundener var.  */
    if ((anzahl = (int *)newbuff(nbound+1)) == (int *)0)
      post_mortem("r1pr: Heap out of space");
    for ( i = nbound ; i > 0 ; i-- ) {      /* formale parameter in    */
      name[i] = y = POPSTACK(S_e);          /* name eintragen und      */
      anzahl[i] = 0;                        /* innere bindungen init., */
      for ( j = nbound ; j > i ; j-- ) {    /* ev. die anzahl gleicher */
        if (name[j] == y) anzahl[j]++;      /* (innerer) bindungen     */
      }                                     /* erhoehen                */
    }   /* end for i      */

main_e:
    x = READSTACK(S_e);
    if (T_AP(x) || T_COND(x)) {    /* sind uninteressant               */
       PPOPSTACK(S_e);             /* nur aus effizienzgruenden hier   */
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }
    if (T_SUB(x) || T_REC(x)) {    /* bindekonstruktor ?               */
       PPOPSTACK(S_e);             /* loeschen                         */
       for (i = ARITY(x) ; i > 1 ; i--) {   /* anzahl innerer bindungen*/
         y = POPSTACK(S_e);                 /* in anzahl[.] nachhalten */
         for ( j = nbound ; j > 0 ; j-- ) { /* durchlaufe zu ersetzenden */
           if (name[j] == y) anzahl[j]++;   /* variablen und vergleiche  */
         }
         PUSHSTACK(S_a,y);         /* geb. variable nach a und         */
         PUSHSTACK(S_hilf,y);      /* hilf wg. herunterzaehlen         */
       }
       PUSHSTACK(S_m1,x);          /* bindekonstruktor nach m,m1       */
       PUSHSTACK(S_m,x);
       goto main_e;                /* und rumpf bearbeiten             */
    }
    if (T_LREC(x))
    {
       PPOPSTACK(S_e);                       /* LETREC(3) */
       z = POPSTACK(S_e);                    /* <(m) */
       for (i = ARITY(z); i > 0; i--) {
          y = POPSTACK(S_e);
          for ( j = nbound ; j > 0 ; j-- ) { /* durchlaufe zu ersetzenden */
             if (name[j] == y) anzahl[j]++;   /* variablen und vergleiche  */
          }
          PUSHSTACK(S_a,y);         /* geb. variable nach a und         */
          PUSHSTACK(S_hilf,y);      /* hilf wg. herunterzaehlen         */
       }
       PUSHSTACK(S_a,z);                        /* <(m) */
       PUSHSTACK(S_m,DEC(x));             /* LETREC(2) */
       PUSHSTACK(S_m1,SET_ARITY(x,ARITY(z)));   /* LETREC(m) wg. S_hilf */
       goto main_e;
    }
    /* (geschuetzte) Variable ?        */
    if ((T_PROTECT(x)) ||
        ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME)) ) {
       n = 0;                               /* zaehle protects         */
       while ( T_PROTECT((y = POPSTACK(S_e))) ) n++;
       i = nbound;        /* durchlaufe variablen-liste (aeussere zuerst)  */
       while (i > 0) {
         if (y == name[i]) {       /* sind die namen gleich ? */
           m = anzahl[i];
           if (m == n) {           /* richtiger bindungslevel ?        */
             PUSHSTACK(S_a,SET_ARITY(NUM,i-1));      /* ja: ersetzen   */
             DEC_REFCNT((PTR_DESCRIPTOR)y); /* ref.count erniedrigen   */
             goto main_m;
            }    else
           if (n > m) n--;         /* ein protect entfernen            */
         }
         i--;                      /* naechste variable                */
       }                           /* end while i > 0                  */
       PUSHSTACK(S_a,y);           /* alte variable zurueck            */
       for ( ; n > 0 ; n--)        /* und richtige anzahl von protects */
         PUSHSTACK(S_a,SET_ARITY(PROTECT,1));        /* davor          */
       goto main_m;
    }
    if (T_NUM(x)) {                /* nummernvariable ?                   */
       i = ARITY(POPSTACK(S_e));   /* nummer um nbound erhoehen (so viele */
       PUSHSTACK(S_a,SET_ARITY(NUM,i+nbound)); /* zusaetzliche bindungen) */
       goto main_m;
    }
    if (T_CON(x)) {                /* anderer konstruktor ?            */
       if (ARITY(x) > 0) {
          PPOPSTACK(S_e);                   /* nur traversieren        */
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
          goto main_e;
       }
       PPOPSTACK(S_e);             /* Konstruktor mit Stelligkeit 0    */
       PUSHSTACK(S_a,x);
       goto main_m;
    }   /* end if T_CON(x) */
    if T_ATOM(x) {                 /* sonstiges atom traversieren */
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
    }
    post_mortem("r1pr main_e: no match");
main_m:
    x = READSTACK(S_m);
    if (T_SUB(x) || T_REC(x)) {
      for (i = ARITY(x) ; i > 1 ; i-- ) {   /* innere bindungen anhand */
        y = POPSTACK(S_hilf);      /* der auf hilf gespeicherten var.  */
        for (j = nbound ; j > 0 ; j-- ) {
          if (name[j] == y) anzahl[j]--;    /* herunterzaehlen         */
        }
      }
      PPOPSTACK(S_m);
      PPOPSTACK(S_m1);
      PUSHSTACK(S_a,x);
      goto main_m;
    }
    if (T_LREC(x)) {
       if (ARITY(x) > 1) {
          WRITESTACK(S_m, DEC(x));
          goto main_e;
       }
       else {
          for (i = ARITY(POPSTACK(S_m1)) ; i > 0 ; i-- ) {  /* LREC(m) */
                                        /* innere bindungen anhand */
             y = POPSTACK(S_hilf);      /* der auf hilf gespeicherten var.  */
             for (j = nbound ; j > 0 ; j-- ) {
               if (name[j] == y) anzahl[j]--;    /* herunterzaehlen         */
             }
          }
          PPOPSTACK(S_m);
          PUSHSTACK(S_a,SET_ARITY(x,3));                 /* LETREC(3) */
          goto main_m;
       }
    }
    if (T_PRELIST(x)) {            /* r1pr ist zu beenden              */
       PTRAV_A_E;                  /* ausdruck zurueck                 */
       freeheap((PTR_HEAPELEM)anzahl);      /* speicher freigeben      */
       END_MODUL("r1pr");
       return;
    }
    if (T_CON(x)) {
      if (ARITY(x) > 1) {         /* das uebliche */
        WRITESTACK(S_m, DEC(x));
        goto main_e;
      }
      PPOPSTACK(S_m);              /* arity == 1     */
      PUSHSTACK(S_a, POPSTACK(S_m1));
      goto main_m;
    }
    post_mortem("r1pr main_m: no match");
 }

/*------ rr1pr ----------------------------------------------------------------
 *  Transformiert alle Variablen hinter einem REC f SUB v1..vn expr
 *  in Nummernvariable, dabei wird der rekursive Aufruf ev. durch die kuenst-
 *  lichen Argumente expandiert.
 *  Aufrufsituation : [v1 .. vn] expr       auf S_e
 *    nbound = ARITY(SUB) - 1  (0 falls kein SUB vorhanden)
 *    nfree von closefunc gesetzt bzw. 0 fuer anzahl freier variablen
 *    name[nbound+1] = f  (recname)
 *  globals--  S_e, S_a, (nur intern: S_Hilf)
 *-----------------------------------------------------------------------------
 */
 void rr1pr(name,nbound,nfree)
 STACKELEM * name;        /* zeiger auf die namensliste       */
 register int nbound,nfree;
 {
    register int i,j,m,n;
    int * anzahl;
    STACKELEM x,y;
    STACKELEM z;

    START_MODUL("rr1pr");
    /* situation:                                                      */
    /* auf E liegt [REC] f SUB(n+1) v1..vn expr                        */
    /* oder nur [REC] f expr                                           */
    /* dies erkennt man an nbound (>0 oder =0)                         */

    if ((anzahl = (int *)newbuff(nbound+2)) == (int *)0)
      post_mortem("rr1pr: Heap out of space");
    anzahl[nbound+1] = 0;                   /* fuer rek.variable       */
    for ( i = nbound ; i > 0 ; i-- ) {      /* formale parameter in    */
      name[i] = y = POPSTACK(S_e);          /* name eintragen und      */
      anzahl[i] = 0;
      for ( j = nbound+1 ; j > i ; j-- ) {  /* ev. die anzahl gleicher */
        if (name[j] == y) anzahl[j]++;      /* (innerer) bindungen     */
      }                                     /* erhoehen                */
    }   /* end for i      */

main_e:
    x = READSTACK(S_e);
    if (T_AP(x) || T_COND(x)) {    /* sind uninteressant               */
       PPOPSTACK(S_e);             /* aus effizienzgruenden hier       */
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }
    if (T_SUB(x) || T_REC(x)) {    /* bindekonstruktor (innerer) ?     */
       PPOPSTACK(S_e);
       for (i = ARITY(x) ; i > 1 ; i--) {   /* anzahl gleicher (innerer)    */
         y = POPSTACK(S_e);                 /* bindungen nachhalten         */
         for ( j = nbound+1 ; j > 0 ; j-- ) {
           if (name[j] == y) anzahl[j]++;
         }
         PUSHSTACK(S_a,y);                  /* variable nach a und          */
         PUSHSTACK(S_hilf,y);               /* hilf wg. herunterzaehlen !   */
       }
       PUSHSTACK(S_m1,x);
       PUSHSTACK(S_m,x);
       goto main_e;
    }
    if (T_LREC(x))
    {
       PPOPSTACK(S_e);                       /* LETREC(3) */
       z = POPSTACK(S_e);                    /* <(m) */
       for (i = ARITY(z); i > 0; i--) {
          y = POPSTACK(S_e);
          for ( j = nbound+1 ; j > 0 ; j-- ) { /* durchlaufe zu ersetzenden */
             if (name[j] == y) anzahl[j]++;   /* variablen und vergleiche  */
          }
          PUSHSTACK(S_a,y);         /* geb. variable nach a und         */
          PUSHSTACK(S_hilf,y);      /* hilf wg. herunterzaehlen         */
       }
       PUSHSTACK(S_a,z);                        /* <(m) */
       PUSHSTACK(S_m,DEC(x));             /* LETREC(2) */
       PUSHSTACK(S_m1,SET_ARITY(x,ARITY(z)));   /* LETREC(m) wg. S_hilf */
       goto main_e;
    }
    if ((T_PROTECT(x)) ||
        ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME)) ) {
       n = 0;
       while ( T_PROTECT((y = POPSTACK(S_e))) ) n++;
       i = nbound+1;
       while (i > 0) {
         if (y == name[i]) {
           m = anzahl[i];
           if (m == n) {
             if (i == nbound+1) {  /* rekursionsvariable      */
               for (j = 1 ; j <= nfree ; j++ )
                 PUSHSTACK(S_a,SET_VALUE(NUM,j+nbound));
               PUSHSTACK(S_a,SET_VALUE(NUM,nbound));
               if (nfree) PUSHSTACK(S_a,SET_ARITY(SNAP,nfree + 1));
               DEC_REFCNT((PTR_DESCRIPTOR)y);
               goto main_m;
              }
             else {       /* formaler parameter      */
               PUSHSTACK(S_a,SET_ARITY(NUM,i-1));
               DEC_REFCNT((PTR_DESCRIPTOR)y);
               goto main_m;
             }
            }    else
           if (m < n) n--;
         }
         i--;
       }
       PUSHSTACK(S_a,y);
       for ( ; n > 0 ; n--)
         PUSHSTACK(S_a,SET_ARITY(PROTECT,1));
       goto main_m;                /* PROTECTS aufbauen */
    }

    if (T_NUM(x)) {
       i = ARITY(POPSTACK(S_e));
       PUSHSTACK(S_a,SET_ARITY(NUM,i+nbound+1));
       goto main_m;
    }
    if (T_CON(x)) {
       if (ARITY(x) > 0) {
          PPOPSTACK(S_e);                   /* nur traversieren */
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
          goto main_e;
       }
       PPOPSTACK(S_e);                   /* Konstruktor mit Stelligkeit 0 */
       PUSHSTACK(S_a,x);
       goto main_m;
    }   /* end if T_CON(x) */
    if T_ATOM(x) {                   /* sonstiges atom traversieren */
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
    }
    post_mortem("rr1pr main_e: no match");

main_m:
    x = READSTACK(S_m);
    if (T_SUB(x) || T_REC(x)) {
      for (i = ARITY(x) ; i > 1 ; i-- ) {
        y = POPSTACK(S_hilf);
        for (j = nbound+1 ; j > 0 ; j-- ) {
          if (name[j] == y) anzahl[j]--;
        }
      }
      PPOPSTACK(S_m);
      PUSHSTACK(S_a,POPSTACK(S_m1));
      goto main_m;
    }
    if (T_LREC(x)) {
       if (ARITY(x) > 1) {
          WRITESTACK(S_m, DEC(x));
          goto main_e;
       }
       else {
          for (i = ARITY(POPSTACK(S_m1)) ; i > 0 ; i-- ) {  /* LREC(m) */
                                        /* innere bindungen anhand */
             y = POPSTACK(S_hilf);      /* der auf hilf gespeicherten var.  */
             for (j = nbound+1 ; j > 0 ; j-- ) {
               if (name[j] == y) anzahl[j]--;    /* herunterzaehlen         */
             }
          }
          PPOPSTACK(S_m);
          PUSHSTACK(S_a,SET_ARITY(x,3));                 /* LETREC(3) */
          goto main_m;
       }
    }
    if (T_PRELIST(x)) {            /* rr1pr ist zu beenden */
       PTRAV_A_E;
       freeheap((PTR_HEAPELEM)anzahl);
       END_MODUL("rr1pr");
       return;
    }
    if (T_CON(x)) {
       if (ARITY(x) > 1) {         /* das uebliche */
         WRITESTACK(S_m, DEC(x));
         goto main_e;
       }
       PPOPSTACK(S_m);
       PUSHSTACK(S_a, POPSTACK(S_m1));
       goto main_m;
    }
    post_mortem("rr1pr main_m: no match");
 }
/* end of r1pr.c */
