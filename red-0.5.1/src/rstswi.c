/* $Log: rstswi.c,v $
 * Revision 1.2  1992/12/16  12:51:14  rs
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
/*-----------------------------------------------------------------------------
 * Externe Routinen und Variablen:
 *-----------------------------------------------------------------------------
 */
extern  PTR_DESCRIPTOR newdesc ();                      /* heap:c */
extern int  newheap ();                                 /* heap:c */
extern  void trav_a_hilf ();                            /* trav:c */
extern  STACKELEM pm_st_match ();                       /* pmst-match */
extern int  mvheap ();                                  /* mvheap:c */
extern int  vali ();                                    /* val:c */
extern double   valr ();                                /* val:c */
extern  STACKELEM st_dec ();                            /* st_dec:c */
extern  BOOLEAN _formated;                              /* reduct:c */

/* RS 30/10/92 */ 
extern void stack_error();                    /* rstack.c */
extern void trav();                           /* rtrav.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*----------------------------------------------------------------------*/
/* pm_st_switch kopiert switch-ausdruck vom stack S_hilf                */
/* mit mvheap in einen  Switchdeskriptor.                               */
/* Der Parameter n gibt die Zahl der Matchdeskriptoren an               */
/*----------------------------------------------------------------------*/
STACKELEM pm_st_switch (n)
int     n;
{
    register    PTR_DESCRIPTOR desc;
    register    PTR_DESCRIPTOR nomatchdesc;
 /* register    STACKELEM x;
    register    PTR_HEAPELEM p;
    register int    level;               RS 30/10/92 */ 
    int     i;
    PTR_DESCRIPTOR pf;   

    START_MODUL ("pm_st_switch");

    if ((desc = newdesc ()) == 0)
        post_mortem ("st-switch: Heap out of Space");

    DESC_MASK (desc, 1, C_EXPRESSION, TY_SWITCH);

    if (T_PM_END (POPSTACK (S_a)))/* es wurde kein otherwise Ausdruck */
    {                             /* angegeben                        */

       /*teilweise ausgefuellter Switch-Deskriptor auf den A-Stack    */
        PUSHSTACK (S_a, (STACKELEM) desc);

       /*Anlegen eines Nomatchdeskriptors*/
        if ((nomatchdesc = newdesc ()) == 0)/* Anlegen eines */
            post_mortem ("pm_st_switch: Heap out of Space");

        DESC_MASK (nomatchdesc, 1, C_EXPRESSION, TY_NOMAT);

      /* den Switch Deskriptor in den Nomatchdeskriptor retten*/
        L_NOMAT ((*nomatchdesc), ptsdes) = (T_PTD)POPSTACK (S_a);
        L_NOMAT ((*nomatchdesc), act_nomat) = 0;

#if DEBUG
        L_SWITCH ((*desc), ptse) = (PTR_HEAPELEM) 1;
#endif
      /*guard-body gleich 0 kodiert einen an dieser Stelle*/
      /*angelegten Nomatchdeskriptor                      */ 
	L_NOMAT ((*nomatchdesc), guard_body) = 0;

        PUSHSTACK (S_a, (STACKELEM) nomatchdesc);
    }
    else         /* es gibt otherwise Ausdruck   */
    {

         /* matchdescriptor fuer otherwise Ausdruck anlegen*/
         if ((pf = newdesc()) == NULL)           
           post_mortem("pmwhfunc: Heap out of space");

         DESC_MASK(pf,1,C_EXPRESSION,TY_MATCH);    

         /*Ausdruck auf den Hilfstack bringen*/
         trav(&S_a, &S_hilf);

         /*Ausdruck wegschreiben*/
         if (mvheap (A_MATCH((*pf), body)) == 0)
         {
            DEC_REFCNT(pf);
            END_MODUL("pm_st_match"); 
            return((STACKELEM) NULL);
         } 

         /*dummywert fuer den guard*/
         L_MATCH((*pf), guard)=  (PTR_HEAPELEM) 0;

         /* dummywert fuer den code*/
         L_MATCH((*pf), code) = (PTR_HEAPELEM) 0;      /* = weg RS 30/10/92 */ 

         /* Matchdeskriptor fuer den Otherwise Ausdruck*/
         /* auf den A-Stack legen                      */
         PUSHSTACK ( S_a, pf);
    } /*Ende else*/

    /* die Matchdeskriptoren vom a_stack auf den Hilfstack legen */
    for (i = 1; (i <= n); ++i) 
        PUSHSTACK (S_hilf, POPSTACK (S_a));

   /* die Matchdeskriptoren vom Hilfstack in den Switch Deskriptor  */
   /* schreiben                                                     */
    if (mvheap (A_SWITCH ((*desc), ptse)) == 0)
        post_mortem ("kein platz um den Switch auszulagern");

   /* Zahl der Matchdeskriptoren wegschreiben */
    L_SWITCH ((*desc), nwhen) = n;

    END_MODUL ("pm_st_switch");

   /* Funktion gibt einen ausgefuellten Switchdeskriptor als Ergebnis*/
   /* zurueck                                                        */
    return ((STACKELEM) desc);    
}                                                           
