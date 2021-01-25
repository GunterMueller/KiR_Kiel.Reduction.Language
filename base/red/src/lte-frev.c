/* $Log: lte-frev.c,v $
 * Revision 1.2  1992/12/16 12:49:13  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 *  lte-frev.c  - external:  lte_freev
 *         Aufgerufen durch:  ea_create
 *  lte-frev.c  - internal:  keine
 *-----------------------------------------------------------------------------
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
extern PTR_DESCRIPTOR newdesc();

/* ach 29/10/92 */
extern int newheap();              /* rheap.c */
extern void stack_error();         /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern void DescDump();
#endif
/*end of ach */

/*
*--------------------------------------------------------------------------
*    lte_freev(nbound)
*--------------------------------------------------------------------------
*/
void lte_freev(nbound)
int nbound;
{
/*  STACKELEM z;          ach 29/10/92 */
  PTR_DESCRIPTOR pf;
  extern int _bound;               /* state.c */

  START_MODUL("lte_freev");

                       /* Anlegen eines TY_SUB Deskriptors mit: */
  if ((pf = newdesc()) == NULL)
     post_mortem("lte_freev : Heap out of space");

  DESC_MASK(pf,1,C_EXPRESSION,TY_SUB);
  L_FUNC((*pf),nargs) = _bound;
  if ((newheap(1,(int *)A_FUNC((*pf),namelist))) == 0)
     post_mortem("lte_freev : Heap out of space");
  L_FUNC((*pf),namelist)[0] = 0;
#if DEBUG
  L_FUNC((*pf),pte) = (PTR_HEAPELEM)1;
#endif /* DEBUG */
  PUSHSTACK(S_a,(STACKELEM)pf);
  PUSHSTACK(S_m,SET_ARITY(PRELIST,nbound));        /* kopie von nbound */
  PUSHSTACK(S_m,SET_ARITY(PRELIST,0));

  END_MODUL("lte_freev");
  return;
}
/* end of lte-frev.c */
