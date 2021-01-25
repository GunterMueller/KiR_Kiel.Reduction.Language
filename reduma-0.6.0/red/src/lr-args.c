/* $Log: lr-args.c,v $
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:04  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*----------------------------------------------------------------------------
 * MODUL FUER DIE ROCESSING PHASE
 * lrec_args:c - external: lrec_args;
 *               aufgerufen von : ear;
 *                                ea-ret;
 *                                check_goal_expr;    * lr-goal.c *
 *                                che_ptdd;           * lrec-new.c *
 *                                che_plargs;         * lrec-new.c *
 *                                new_heapstruc;      * lrec-new.c *
 * lrec_args:c - internal:  keine;
 *----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();   /* heap:c */
extern int newheap();              /* heap:c */
extern void trav_a_e();

/* ach 30/10/92 */
extern void stack_error();
extern STACKELEM st_expr();
/* end of ach */

#if DEBUG
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void display_stack(); /* TB, 4.11.1992 */   /* rruntime.c */
#endif

/*-----------------------------------------------------------------------------
 * lrec_args --
 * globals -- S_e,S_a    <wr>
 * calls   -- newheap, newdesc, trav_a_e, st_expr;
 *
 *      legt eine heap-Struktur vom Typ LREC_ARGS an,
 *      in deren Datenvektor die (Verweise auf die) Argumente aus der
 *      SNAP-Applikation eingetragen werden
 *-----------------------------------------------------------------------------
 */

void lrec_args(estack,readi)
   int estack;            /* estack = 1 : die SNAP-Applikation liegt   */
                          /*              vollstaendig auf S_e;        */
                          /* sonst      : plrec auf S_e, SNAP auf S_m, */
                          /*              args auf S_a                 */
   int readi;             /* readi = 1  : fuehre Substitution aus      */
                          /* sonst      : traversiere                  */
{
  PTR_DESCRIPTOR pfa,plr;
  int i,args,sh;
  STACKELEM x;

  START_MODUL("lrec_args");

  if (estack)
     x = POPSTACK(S_e);                     /* SNAP(q+1) */
  else {
     x = POPSTACK(S_m1);                    /* SNAP(1)   */
     PPOPSTACK(S_m);                        /* SNAP(q+1) */
  }
  args = ARITY(x) - 1;
  if ((pfa = newdesc()) == NULL)
     post_mortem("lrec_args:Heap out of space");
                                            /* for further use: abfangen! */
                                            /* i.e., no heap structure    */
  DESC_MASK(pfa,1,C_EXPRESSION,TY_LREC_ARGS);
  L_LREC_ARGS((*pfa),nargs) = args;
  if (newheap(args,A_LREC_ARGS((*pfa),ptdv)) == 0)
     post_mortem("lrec_args: Heap out of space");
                                            /* for further use: abfangen! */
                                            /* i.e., no heap structure    */
  if (estack)    /* SNAP-args auf S_e*/
     for (i = (args-1); i >= 0; i--) {
        x = POPSTACK(S_e);
        if (T_CON(x)) {
           post_mortem("lr_args: non-atomic SNAP-arg");
        }
        else
        if (readi) {
           if (T_NUM(x)) {
              READ_I(VALUE(x),x);   /* enthaelt INC_REFCNT (siehe RSTACKTY:H) */
           }
         /*  else ; auskommentiert RS 14.6.1993 */
        }
        /* else ; auskommentiert RS 14.6.1993 */                    /* keine weitere Unterscheidung */
        L_LREC_ARGS((*pfa),ptdv)[i] = x;
      }
  else {          /* SNAP-args auf S_a */
     sh = SIZEOFSTACK(S_hilf);
     if (sh > 0) {        /* rette Elemente von S_hilf wg. MVHEAP in st_expr */
        for (i = 0; i < sh; i++)
           PUSHSTACK(S_v,POPSTACK(S_hilf));
     }
     /* else ; auskommentiert RS 14.6.1993 */
     for (i = 0; i < args; i++)
     {
        x = POPSTACK(S_a);
        if (T_CON(x)) {            /* existieren nach Ausfuehrung von */
                                   /* ret_list oder ret_expr          */
           PUSHSTACK(S_a,x);
           if ((x = st_expr()) == NULL) {
              post_mortem("lrec_args: Heap out of space");
                                            /* for further use: abfangen! */
                                            /* i.e., no heap structure    */
           }
        }
        /* else ; auskommentiert RS 14.6.1993 */                    /* keine weitere Unterscheidung */
        L_LREC_ARGS((*pfa),ptdv)[i] = x;
     }
     if (sh > 0) {        /* restauriere Elemente auf S_hilf */
        for (i = 0; i < sh; i++)
           PUSHSTACK(S_hilf,POPSTACK(S_v));
     }
     /* else ; auskommentiert RS 14.6.1993 */
  }     /* end else : (!estack) */
  plr = (PTR_DESCRIPTOR) POPSTACK(S_e);     /* p_lrec */
  L_LREC_ARGS((*pfa),ptdd) = (T_PTD) plr;
  PUSHSTACK(S_e,(STACKELEM)pfa);            /* plrec_args */

  END_MODUL("lrec_args");
  return;
}


