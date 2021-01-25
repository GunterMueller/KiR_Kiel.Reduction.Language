/*
 * $Log: rpos.c,v $
 * Revision 1.3  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.2  1992/12/16 12:50:31  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */



/*-----------------------------------------------------------------------------
 *  pos_func:c  - external:  pos_func;
 *         Aufgerufen durch:  ea_retrieve;
 *  pos_func:c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
extern int ptrav_a_e();       /* trav:c    */
extern void r1eapos();    /* r1eapos:c */

/* ach 30/10/92 */
extern void stack_error();
/* end of ach */

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern int isdesc(); /* TB, 4.11.1992 */           /* rheap.c */
#endif

/*-----------------------------------------------------------------------------
 *  pos_func   --  Steuert die Umbenennung der gebundenen Nummernvariablen
 *                 einer Funktion in ihre externen Namen. Ferner werden
 *                 die Konstruktoren SUB bzw. REC eingefuehrt. Die
 *                 Umbenennung erfolgt von innen nach aussen.
 *  globals    --
 *-----------------------------------------------------------------------------
 */

 void pos_func(narg)
 int narg;                /* Anzahl der uebrig bleibenden Variablen  */
 {
    STACKELEM pfunc, y;
    PTR_HEAPELEM name;
    int i;

    START_MODUL("pos_func");
    PTRAV_A_E;            /* Rumpf auf e; Var auf a */
    pfunc = POPSTACK(S_a);         /* Funktionsdeskriptor */

#if DEBUG
    if (!(T_POINTER(pfunc) && isdesc((PTR_DESCRIPTOR)pfunc)) )
      post_mortem("pos_func: invalid expression! no FUNC-Pointer");
#endif /* DEBUG */
                    /* An dieser Stelle muss ein Funktionspointer */
                    /* liegen. Also                              */
                    /* eine sehr gute Stelle, um unvollstaendige */
                    /* Ausdruecke zu entdecken */


    name = (STACKELEM *) (R_FUNC((*(PTR_DESCRIPTOR)pfunc),namelist));
        /* die Liste mit den Variablennamen */
    for (i = 1; i <= narg; i++)         /* Umbenennung hinter dem zu-   */
    {                                   /* kuenftigen SUB               */
       y = (STACKELEM) name[i];         /* Variablennamen holen         */
       PUSHSTACK(S_a,y);                /* auf A fuer das SUB           */
       INC_REFCNT((PTR_DESCRIPTOR)y);

       if (i == 1)                          /* noch kein SUB da? */
         PUSHSTACK(S_m,SET_ARITY(SUB,1));   /* eins heraufpacken */
       else
         PUSHSTACK(S_m,POPSTACK(S_e));      /* sonst von E holen */

       {
         int hilf;                                    /* SJ-ID */
         hilf = (int) R_FUNC((*(PTR_DESCRIPTOR)pfunc),special);
         hilf = SUB | SET_EDIT(hilf);     /* SJ-ID */
         PUSHSTACK(S_m1,SET_ARITY(hilf,i+1));
       }

       r1eapos(NUM,y);                      /* ersetze NUM( 0) durch y */
       if (i < narg) PTRAV_A_E;             /* wenn weitere Variablen zu er- */
    }                                       /* setzen traversiere zurueck */

    if (R_DESC((*(PTR_DESCRIPTOR)pfunc),type) == TY_REC
        && (name[0] - 1) == narg)
    {                                    /* Umbenennen der Rekur- */
                                         /* sionsvariable         */
       if (narg)   /* nicht bei rekursiven Funktionen ohne Argumente */
         PTRAV_A_E;

       y = (STACKELEM)name[(int)name[0]];
       PUSHSTACK(S_a,y);                 /* der Name der rekursiven Funktion */
       INC_REFCNT((PTR_DESCRIPTOR)y);

       PUSHSTACK(S_m,SET_ARITY(REC,1));  /* REC Konstrukt erzeugen */

       {
         int hilf;                                   /* SJ-ID */
         hilf = (int) R_FUNC((*(PTR_DESCRIPTOR)pfunc),special);
         hilf = REC | SET_EDIT(hilf);         /* SJ-ID */
         PUSHSTACK(S_m1,SET_ARITY(hilf,2));
       }

       if (narg)   /* nicht bei rekursiven Funktionen ohne Argumente */
       {
         PUSHSTACK(S_m1,POPSTACK(S_e));  /* SUB auf die M-Stacks legen */
         PUSHSTACK(S_m,READSTACK(S_m1));
       }

       r1eapos(NUM,y);             /* ersetze NUM(0) durch y (der Varname) */
    }

    DEC_REFCNT((PTR_DESCRIPTOR)pfunc);     /* wiederverpointertes Aufsetzen angenommen */
    END_MODUL("pos_func");
 }
