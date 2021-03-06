/*
 * $Log: rtrav.c,v $
 * Revision 1.3  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.2  1992/12/16 12:51:17  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */



/*-----------------------------------------------------------------------------
 * trav:c - external: trav;
 * Aufgerufen durch:  runtime;
 * trav:c - external: trav_a_e;
 * Aufgerufen durch : reduct;
 * trav:c - external: trav_e_a;
 * Aufgerufen durch : ear;
 * trav:c - external: trav_a_hilf;
 * Aufgerufen durch : store module;
 * trav:c - external: trav_hilf_a;
 * Aufgerufen durch : mvheap;
 * trav:c - external: ptrav_a_e;
 * Aufgerufen durch : reduct;
 * trav:c - external: ptrav_a_hilf;
 * Aufgerufen durch :  ?
 * trav:c - internal: keine;
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

/* RS 30/10/92 */
extern void stack_error();                 /* rstack.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */       /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 *  trav -- traversiert eine vollstaendigen Ausdruck von Parameterstack
 *          "source" nach "sink".
 *  globals -
 *         S_m,S_m1        fuer trav;
 *----------------------------------------------------------------------------
 */
void trav(source,sink)
StackDesc *source,*sink;
{
#define S_source *source
#define S_sink   *sink
/* die Stackroutinen (bzw Makros) erwarten immer einen StackDesc */

    register STACKELEM  x;


    START_MODUL("trav");

    PUSHSTACK(S_m,KLAA);

trav_source:

    x = POPSTACK(S_source);

    if (T_CON(x))
    {
       if (ARITY(x) > 0)
       {
         PUSHSTACK(S_m,x);
         PUSHSTACK(S_m1,x);
         goto trav_source;
       }
       else
       {
          PUSHSTACK(S_sink,x);
          goto trav_m;
       }
    }

    if (T_SA(x)) {
       PUSHSTACK(S_sink,x);
       goto trav_m;
    }

    if (T_STR0(x))
    {
        PUSHSTACK(S_sink,x);
        goto trav_m;
    }
    if (T_STR1(x))
    {
        PUSHSTACK(S_sink,TOGGLE(x));

        for( ; x = POPSTACK(S_source),T_STR1(x) ; )
        {
          PUSHSTACK(S_sink,x);
        }
        PUSHSTACK(S_sink,TOGGLE(x));
        goto trav_m;
    }
    post_mortem("trav: No match successfull on trav_source");

trav_m:

    x = READSTACK(S_m);

    if (T_CON(x))
    {
      if (ARITY(x) > 1)
      {
          WRITESTACK(S_m,DEC(x));
          goto trav_source;
      }
      else
      {
         PPOPSTACK(S_m); /* TB, 30.10.92 */
         PUSHSTACK(S_sink,POPSTACK(S_m1));
         goto trav_m;
      }
    }

    if (T_KLAA(x))
    {
        PPOPSTACK(S_m); /* TB, 30.10.92 */
        END_MODUL("trav");
        return;
    }

    post_mortem("trav: No match successfull on trav_m");
}

/*---------------------------------------------------------------------------
 * trav_a_e --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *              Single Atoms besteht rekursiv von A nach E.
 * globals -    S_a, S_e;
 *---------------------------------------------------------------------------
 */
void trav_a_e()
{
    register STACKELEM  x = POPSTACK(S_a);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_a_e();
    PUSHSTACK(S_e,x);
}

/*---------------------------------------------------------------------------
 * trav_e_a --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *              Single Atoms besteht rekursiv von E nach A.
 * globals :    S_e, S_a;
 *---------------------------------------------------------------------------
 */
void trav_e_a()
{
    register STACKELEM  x = POPSTACK(S_e);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_e_a();
    PUSHSTACK(S_a,x);
}

/*---------------------------------------------------------------------------
 * ptrav_a_e --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *               Single Atoms besteht rekursiv von A nach E.
 *               Genuegend Platz auf dem E-Stack sollte mittels IS_SPACE
 *               sichergestellt werden.
 * globals -     S_a, S_e;
 *---------------------------------------------------------------------------
 */
void ptrav_a_e()
{
    register STACKELEM  x = POPSTACK(S_a);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        ptrav_a_e();
    PPUSHSTACK(S_e,x);
}

/*---------------------------------------------------------------------------
 * trav_a_hilf --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *                 Single Atoms besteht rekursiv von A nach Hilf.
 * globals -       S_a,S_hilf;
 *---------------------------------------------------------------------------
 */
void trav_a_hilf()
{
    register STACKELEM  x = POPSTACK(S_a);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_a_hilf();
    PUSHSTACK(S_hilf,x);
}

/*---------------------------------------------------------------------------
 * ptrav_a_hilf --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *                  Single Atoms besteht rekursiv von A nach Hilf.
 * globals -       S_a, S_hilf;
 *---------------------------------------------------------------------------
 */
void ptrav_a_hilf()
{
    register STACKELEM  x = POPSTACK(S_a);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        ptrav_a_hilf();
    PPUSHSTACK(S_hilf,x);
}

/*---------------------------------------------------------------------------
 * trav_hilf_a -- traversiert einen Ausdruck der nur aus Konstruktoren und
 *                Single Atoms besteht rekursiv von hilf nach a.
 * globals -      S_hilf, S_a;
 *---------------------------------------------------------------------------
 */
void trav_hilf_a()
{
    register STACKELEM  x = POPSTACK(S_hilf);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        trav_hilf_a();
    PUSHSTACK(S_a,x);
}


