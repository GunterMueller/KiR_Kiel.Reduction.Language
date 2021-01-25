/* $Log: lr-stor.c,v $
 * Revision 1.2  1992/12/16  12:49:12  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*----------------------------------------------------------------------------
 * MODUL FUER DIE PREROCESSING PHASE
 * lst_lrec:c - external: lst_lrec;
 * lst_lrec:c - internal:  keine;
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
extern int newheap();              /* heap:c */
extern void trav_a_e();

/* RS 30/10/92 */
extern void trav_e_a();           /* rtrav.c */
extern STACKELEM st_expr();       /* rstpro.c */
extern void freeheap();           /* rheap.c */
/* END of RS 30/10/92 */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
extern void DescDump();
extern void test_inc_refcnt();
extern void test_dec_refcnt();
#endif
/* end of ach */
  
/*-----------------------------------------------------------------------------
 * lst_lrec --
 * globals -- S_e,S_a    <wr>
 * calls   -- newheap, st_expr;
 *-----------------------------------------------------------------------------
 */

void lst_lrec()
{
  register PTR_DESCRIPTOR plr;
  PTR_DESCRIPTOR pfi;
  register int funcs;
  int i;
  STACKELEM x,z;
  T_HEAPELEM * tab;

  START_MODUL("lst_lrec");

  TRAV_A_E;               /* liste der func_def's    */
  PPOPSTACK(S_e);         /* listenkonstruktor       */
  tab = (T_HEAPELEM *)POPSTACK(S_a);
                          /* tabelle mit func_names und plrec_ind      */
  z = READSTACK(S_a);     /* plrec oder SNAP         */
  if (T_POINTER(z))
     plr = (PTR_DESCRIPTOR) z;     /* plrec */
  else {
     PPOPSTACK(S_a);                        /* SNAP  */
     plr = (PTR_DESCRIPTOR) READSTACK(S_a); /* plrec */
     PPUSHSTACK(S_a,z);                     /* SNAP  */
  }
  funcs = R_LREC((*plr),nfuncs);
  if (newheap(funcs,A_LREC((*plr),ptdv)) == 0) {
                          /* hole platz fuer verweise auf func_def's   */
    post_mortem("lst_lrec: Heap out of space");
    return;
  }
  for (i = 0; i < funcs; i++) {    /* func_def's     */
    x = READSTACK(S_e);
    if (T_CON(x)) {
       TRAV_E_A;
       if ((x = st_expr()) == NULL)
          post_mortem("lst_lrec: Heap out of space");
    }
    else
       PPOPSTACK(S_e);
    L_LREC((*plr),ptdv)[i] = x;    /* func_def fuer plrec@.ptdv        */
    z = tab[(2*i)+1];
    pfi = (PTR_DESCRIPTOR)z;
    L_LREC_IND((*pfi),ptf) = (T_PTD)x;    /* func_def fuer plrec_ind */
    if (T_POINTER(x))
       INC_REFCNT((PTR_DESCRIPTOR)x);
  }
  for (i = 0; i < funcs; i++) {
     z = tab[(2*i)+1];
     DEC_REFCNT((PTR_DESCRIPTOR)z);
  }
  freeheap(tab);

  END_MODUL("lst_lrec");
  return;
}       /* end of lst_lrec         */
/* end of file   */

