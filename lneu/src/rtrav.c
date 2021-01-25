/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
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
      if (ARITY(x) > 1)
      {
          WRITESTACK(S_m,DEC(x));
          goto trav_source;
      }
      else
          {
         POPSTACK(S_m);
         PUSHSTACK(S_sink,POPSTACK(S_m1));
         goto trav_m;
      }

    if (T_KLAA(x))
    {
        POPSTACK(S_m);
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
 * h_trav_a_hilf --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *                 Single Atoms besteht rekursiv von A nach Hilf.
 * globals -       S_a,S_hilf;
 *---------------------------------------------------------------------------
 */
void h_trav_a_hilf()
{

    PUSHSTACK(S_hilf,(STACKELEM)ENDE);

    trav_a_hilf();

}

/*---------------------------------------------------------------------------
 * ptrav_a_hilf --  traversiert einen Ausdruck der nur aus Konstruktoren und
 *                  Single Atoms besteht rekursiv von A nach Hilf.
 * globals -       S_a, S_hilf;
 *---------------------------------------------------------------------------
 */
void h_ptrav_a_hilf()
{

     PUSHSTACK(S_hilf,(STACKELEM)ENDE);
 
     ptrav_a_hilf();

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


