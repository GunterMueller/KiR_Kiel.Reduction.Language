/* $Log: lnr1pre.c,v $
 * Revision 1.2  1992/12/16  12:49:00  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 *  lr1pre:c  - external:  lr1eapre;
 *         Aufgerufen durch:  lrecfunc;
 *  lr1pre:c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"

/*----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *----------------------------------------------------------------------------
 */
extern void ptrav_a_e();           /* trav:c     */


/* ach 29/10/92 */
extern void stack_error();         /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
extern void test_inc_refcnt();
extern void test_dec_refcnt();
#endif
/* end of ach */

/*-----------------------------------------------------------------------------
 * lr1eapre   --  ersetzt eine LETREC-gebundene Variable FI durch
 *                - den Verweis P_FI auf einen Deskriptor vom Typ LREC_IND
 *                oder
 *                - snap(q+1) #0 ... #q-1 P_FI  falls nfree = q ne 0
 *-----------------------------------------------------------------------------
 */
 void lr1eapre(substitute,alt,speicher,fdef)
 STACKELEM substitute,alt;
 T_HEAPELEM * speicher;
 int fdef;
 {
    int n = 0;
    int b = 0;
    int bl = 0;
    int i;
    int j;
    int nfree = speicher[0];
    STACKELEM x,y;
    STACKELEM z;

    START_MODUL("lr1eapre");
    PUSHSTACK(S_m,KLAA);

main_e:

    x = READSTACK(S_e);

    if (T_AP(x) || T_COND(x))      /* sind uninteressant */
    {
       PPOPSTACK(S_e);
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }

    if (T_SUB(x) || T_REC(x))      /* jetzt muessen die definierenden Vorkommen */
    {                              /* gleichen Namens gezaehlt und registriert */
       PPOPSTACK(S_e);             /* werden */
       for (i = 1; i < ARITY(x); i++)
       {
          y = POPSTACK(S_e);
          if ( y  == alt) bl++;        /* ist y = dem zu ersetzenden Namen? */
          PUSHSTACK(S_a,y);
       }
       PUSHSTACK(S_m1,x);
       PUSHSTACK(S_m,SET_ARITY(x,bl));   /* alter Trick */
       b += bl;                          /* Anzahl gleicher Namen wird im SUB */
       bl = 0;                           /* bzw REC nachgehalten */
       goto main_e;
    }

    if (T_LREC(x))
    {
       PPOPSTACK(S_e);                       /* LETREC(3) */
       z = POPSTACK(S_e);                    /* <(m) */
       for (i = 1; i <= ARITY(z); i++)
       {
          y = POPSTACK(S_e);
          if ( y == alt) bl++;                  /* ist y = 'FI' */
          PUSHSTACK(S_a,y);
       }
       PUSHSTACK(S_a,z);                        /* <(m) */
       PUSHSTACK(S_m,SET_ARITY(x,ARITY(x)-1));  /* LETREC(2) */
       PUSHSTACK(S_m1,SET_ARITY(x,bl));         /* LETREC(bl) */
       b += bl;
       bl = 0;
       goto main_e;
    }

    if ((T_PROTECT(x)) ||
        ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME)) )
    {
       while ( (T_PROTECT((y = POPSTACK(S_e))))) n++;
       if ( y == alt)                       /* y = zu ersetzender Name ? */
       {
          if (n == b)                       /* hat y richtige Anzahl an PROTECTS? */
          {
             n = 0;                         /* PROTECTS wegwerfen */
             if ( (T_POINTER(substitute)) &&
                  (R_DESC((*(PTR_DESCRIPTOR)substitute),type)
                   == TY_LREC_IND) )
             {
                if (!fdef) {        /* func_def's */
                    for (j = 0; j < nfree; j++)
                        PUSHSTACK(S_a,SET_VALUE(NUM,j)); /* Nummernvar */
                    PUSHSTACK(S_a,substitute);           /* insert P_FI */
                    INC_REFCNT((PTR_DESCRIPTOR)substitute);
                    if (nfree)  PUSHSTACK(S_a,SET_ARITY(SNAP,nfree + 1));
                }
                else {             /* goal_expr */
                    for (j = 1; j <= nfree; j++)
                        PUSHSTACK(S_a,speicher[j]); /* Nummernvar */
                    PUSHSTACK(S_a,substitute);           /* insert P_FI */
                    INC_REFCNT((PTR_DESCRIPTOR)substitute);
                    if (nfree)  PUSHSTACK(S_a,SET_ARITY(SNAP,nfree + 1));
                }
                DEC_REFCNT((PTR_DESCRIPTOR)alt);
                goto main_m;
             }
             else
                post_mortem("lr1eapre: substitute not equal plrec_ind");
          }
          else
             if (n > b) n--;    /* gleicher Name, falsche Anzahl von PROTECTS */
       }                        /* also nun ein PROTECT weniger,falls diese  */
       /* else ;                   / Variable ausserhalb gebunden /   auskommentiert RS 18.3.1993 */
       PUSHSTACK(S_a,y);
       for ( ; n>0; n--) PUSHSTACK(S_a,SET_ARITY(PROTECT,1));
       goto main_m;                /* PROTECTS aufbauen */
    }

    if (T_NUM(x))
    {
       if ( (T_POINTER(substitute)) &&
          (R_DESC((*(PTR_DESCRIPTOR)substitute),type) == TY_LREC_IND) )
       {
          PUSHSTACK(S_a,POPSTACK(S_e));
          goto main_m;
       }
       else
                post_mortem("lr1eapre: substitute not equal plrec_ind");
    }

    if (T_CON(x))
    {
       if (ARITY(x) > 0) {
          PPOPSTACK(S_e);                   /* nur traversieren        */
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
          goto main_e;
       }
       PPOPSTACK(S_e);             /* Konstruktor mit Stelligkeit 0    */
       PUSHSTACK(S_a,x);
       goto main_m;
    }

    if T_ATOM(x)                              /* atom auf a-Stack  */
    {                                  /* sonstiges singleatom traversieren */
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
    }

    post_mortem("lr1eapre main_e: no match");

main_m:

    x = READSTACK(S_m);

    if (T_SUB(x) || T_REC(x))
    {
       b -= ARITY(x);              /* Anzahl der inneren Bindungen mit gleichem */
       PPOPSTACK(S_m);             /* Namen aendern, da innere Funktion fertig */
       PUSHSTACK(S_a,POPSTACK(S_m1));
       goto main_m;
    }

    if (T_LREC(x))
    {
       if (ARITY(x) > 1)
       {
          WRITESTACK(S_m, DEC(x));
          goto main_e;
       }
       z = POPSTACK(S_m1);
       b -= ARITY(z);  /* Anzahl der inneren Bindungen mit gleichem */
       PPOPSTACK(S_m); /* Namen aendern, da innere Funktion fertig */
       PUSHSTACK(S_a,SET_ARITY(z,3));                 /* LETREC(3) */
       goto main_m;
    }

    if (T_CON(x))
       if (ARITY(x) > 1)           /* das uebliche */
    {
       WRITESTACK(S_m, DEC(x));
       goto main_e;
    }
    else
    {
       PPOPSTACK(S_m);
       PUSHSTACK(S_a, POPSTACK(S_m1));
       goto main_m;
    }

    /* if (T_PRELIST(x)) ... */
    if (T_KLAA(x))                        /* lr1eapre ist zu beenden */
    {
       PPOPSTACK(S_m);

       END_MODUL("lr1eapre");
       return;
    }

    post_mortem("lr1eapre main_m: no match");
 }
