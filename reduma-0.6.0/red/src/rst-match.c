/* $Log: rst-match.c,v $
 * Revision 1.2  1992/12/16  12:51:00  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rinstr.h"

 /*---------------------------------------------------------------------
 * Externe Routinen und Variablen:
 *---------------------------------------------------------------------
 */

extern  PTR_DESCRIPTOR newdesc ();                      /* heap:c */
extern int  newheap ();                                 /* heap:c */
extern  void trav_a_hilf ();                            /* trav:c */
extern int  mvheap ();                                  /* mvheap:c */
extern int  vali ();                                    /* val:c */
extern double   valr ();                                /* val:c */
extern  STACKELEM st_dec ();                            /* st_dec:c */

extern  BOOLEAN _formated;                              /* reduct:c */

/* RS 30/10/92 */ 
extern void trav();                            /* rtrav.c */
extern void pm_comp();                         /* rcomp.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

 /*----------------------------------------------------------------------------*/
 /* pm_st_match lagert den erzeugten Code, den Rumpf und den Guard-Ausdruck in */
 /* einen Matchdeskriptor ein                                                  */
 /*----------------------------------------------------------------------------*/

STACKELEM pm_st_match ()
{
    PTR_DESCRIPTOR desc;
/*  register    STACKELEM x;
    register    PTR_HEAPELEM p;
    register int    level;         RS 30/10/92 */ 

    START_MODUL ("pm_st_match");

    if ((desc = newdesc ()) == 0)
        post_mortem ("pm_st-match: Heap out of Space");

    DESC_MASK (desc, 1, C_EXPRESSION, TY_MATCH);

    trav (&S_a, &S_hilf); /* behandlung des body         */


    if (mvheap (A_MATCH ((*desc), body)) == 0)
    {
        DEC_REFCNT (desc);
        END_MODUL ("pm_st_match");
        return ((STACKELEM) NULL);
    }

    /*Unterscheiden zwischen einem guard true und einem Guard*/
    /*der ausgerechnet werden muss                           */
     if (T_SA_TRUE(READSTACK(S_a)))
     {
       /*guard besteht nur aus einem TRUE */
       /*in den guard eine Null schreiben */
        L_MATCH((*desc), guard)=  (PTR_HEAPELEM) 0;
        
        /*das TRUE auf dem A-Stack beseitigen*/
        PPOPSTACK(S_a);
     }
     else
     {
        trav (&S_a, &S_hilf);      /* behandlung der guard-expr   */

        if (mvheap (A_MATCH ((*desc), guard)) == 0)
        {
            DEC_REFCNT (desc);
            END_MODUL ("pm_st_match");
            return ((STACKELEM) NULL);
        }
      } /*ende T_SA_TRUE(...  */

       /* Aufruf von pm_comp zum Uebersetzen des Pattern. */
       /* In den Matchdeskriptor desc wird der erzeugte   */
       /* Code abgelegt                                   */
        pm_comp (desc);

        END_MODUL ("pm_st_match");
        return ((STACKELEM) desc);
}
