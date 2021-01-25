


/*----------------------------------------------------------------------------
 * storepro.c
 * MODULE FUER DIE PRE- UND PROCESSING PHASE
 * storepro:c - external: st_expr,st_list;
 * storepro:c - internal:  keine;
 *----------------------------------------------------------------------------
 */
/* last update  by     why
   31.05.88     sj     Zugriff auf das Edit-Feld erfolgt nicht
                       mehr ueber STORE_EDIT  Makro sondern mit L_LIST Makro
                       SJ-ID
   02.06.88     sj     Makro EXTR_EDIT  eingefuegt  (defined in rstelm:h)
                       SJ-ID

   copied       by     to
   08.06.88     sj     (struck)istpro.c
 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();   /* heap:c */
extern int newheap();              /* heap:c */
extern void trav_a_hilf();         /* trav:c */
extern int mvheap();               /* rhinout.c */
extern int _argsneeded;   /* state:c - Zahl der benoetigten Argumente */
extern int red_dummy();  
extern int freeheap();                 /* rheap.c */
extern void freedesc();                /* rheap.c */

#define TET  0                     /* was ist das ?  */

/*--------------------------------------------------------------------------*/
/* store_e -- legt fuer einen reduzierten Ausdurck auf dem A-Stack einen    */
/*            Descriptor an. Dieser wird als Stackelement zurueckgeben.     */
/*            Falls der Heap voll ist, wird eine 0 zurueckgeliefert.        */
/* globals -- S_a           <w>       Zu verarbeitender Ausdruck            */
/* calls   -- newdesc, newheap                                              */
/* called by -- nur in der Processingphase, u.a. durch                      */
/*              ear                                                         */
/*--------------------------------------------------------------------------*/

STACKELEM store_e()
{
  register PTR_DESCRIPTOR desc;
  register PTR_HEAPELEM p;
  register int size, * desc_entry;
  STACKELEM x;

  START_MODUL("store_e");
#if MEASURE
  measure(STORE_EXPR);
#endif

  if ((desc = newdesc()) == NULL) {
    END_MODUL("store_e");
    return((STACKELEM)NULL);
  }
  DESC_MASK(desc,1,C_EXPRESSION,TY_EXPR);
  size = ARITY(x = POPSTACK(S_a)) + 1;
  if (!newheap(1 + size,A_EXPR((*desc),pte))) {
    DEC_REFCNT(desc);
    PUSHSTACK(S_a,x);
    END_MODUL("store_e");
    return((STACKELEM)NULL);
  }
  RES_HEAP;
  p    = (PTR_HEAPELEM) R_EXPR((*desc),pte);
  *p++ = (T_HEAPELEM) (size + TET);         /* 0. Eintrag: Groesse     */
  *p   = (T_HEAPELEM) x;                    /* 1. Eintrag: Konstruktor */
  p   += --size;
  for ( ; size ; --size )
    *p-- = (T_HEAPELEM) POPSTACK(S_a);
  REL_HEAP;
  END_MODUL("store_e");
  return((STACKELEM)desc);
}

/*--------------------------------------------------------------------------*/
/* store_l -- erwartet eine Liste auf dem A-Stack. In dieser Liste duerfen  */
/*  sich nur atomare Ausdruecke befinden ! Falls der Heap voll sein sollte, */
/*  so liefert diese Routine eine Null zurueck.                             */
/* globals -- S_a    <w>                                                    */
/* calls   -- newdesc, newheap                                              */
/*--------------------------------------------------------------------------*/

STACKELEM store_l()
{
  register PTR_DESCRIPTOR desc;
  register PTR_HEAPELEM p;
  register int dim;
  STACKELEM x;

  START_MODUL("store_l");

  dim = ARITY((x = POPSTACK(S_a)));

  if ((desc = newdesc()) == NULL) {
    PUSHSTACK(S_a,x);
    END_MODUL("store_l");
    return((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  L_LIST((*desc),special) = EXTR_EDIT(x);
  L_LIST((*desc),dim) = dim;

  if (newheap(dim,A_LIST((*desc),ptdv)) == 0) {
    PUSHSTACK(S_a,x);
    DEC_REFCNT(desc);
    END_MODUL("store_l");
    return((STACKELEM)NULL);
  }
  RES_HEAP;
  p = R_LIST((*desc),ptdv) + dim;
  while (dim-- > 0)
   *--p = (T_HEAPELEM)POPSTACK(S_a);
  REL_HEAP;
  END_MODUL("store_l");
  return((STACKELEM)desc);
}
/*--------------------------------------------------------------------------*/
/* st_arglist -- lagert die Argumente fuer ein n-stelliges case als Liste   */
/*                aus.                                                      */  
/* globals -- S_a    <w>                                                    */
/* calls   -- newdesc, newheap                                              */
/*--------------------------------------------------------------------------*/

STACKELEM st_arglist()
{
  register PTR_DESCRIPTOR desc;
  register PTR_HEAPELEM p;
  register int dim;
  STACKELEM x;

  START_MODUL("st_arglist");

  dim = ARITY((x = POPSTACK(S_a)));

  if ((desc = newdesc()) == NULL) {
    PUSHSTACK(S_a,x);
    END_MODUL("st_arglist");
    return((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  L_LIST((*desc),special) = EXTR_EDIT(x);
  L_LIST((*desc),dim) = dim;

  if (newheap(dim,A_LIST((*desc),ptdv)) == 0) {
    PUSHSTACK(S_a,x);
    DEC_REFCNT(desc);
    END_MODUL("st_arglist");
    return((STACKELEM)NULL);
  }
  RES_HEAP;
  p = R_LIST((*desc),ptdv) ;
  while (dim-- > 0)
   *p++ = (T_HEAPELEM)POPSTACK(S_a);
  REL_HEAP;
  END_MODUL("st_arglist");
  return((STACKELEM)desc);
}

/*-----------------------------------------------------------------------------
 * st_expr -- legt fuer einen reduzierten Ausdurck auf dem A-Stack einen
 *            Descriptor an. Dieser wird als Stackelement zurueckgeben.
 *            Falls der Heap voll ist, wird eine 0 zurueckliefert.
 * globals -- S_a           <w>       Zu verarbeitender Expression
 *            S_hilf        <w>
 * calls   -- newdesc, newheap, trav_a_hilf, mvheap;
 * called by -- nur in der Processingphase, u.a. durch
 *              ear, st_list, red_lunite, red_lreplace;
 *-----------------------------------------------------------------------------
 */

STACKELEM st_expr()
{
  register PTR_DESCRIPTOR desc;

  START_MODUL("st_expr");

  if ((desc = newdesc()) == NULL)
  {
    END_MODUL("st_expr");
    return((STACKELEM)NULL);
  }
  DESC_MASK(desc,1,C_EXPRESSION,TY_EXPR);

#if HEAP
     H_TRAV_A_HILF;
#else
     TRAV_A_HILF;
#endif

  if (mvheap(A_EXPR((*desc),pte)) == 0)
  {
    DEC_REFCNT(desc);
    END_MODUL("st_expr");
    return((STACKELEM)NULL);
  }
  END_MODUL("st_expr");
  return((STACKELEM)desc);
}

/*--------------------------------------------------------------------------*/
/* st_list -- erwartet eine Liste (d.h. u.U. einen String) auf dem A-Stack. */
/*            Falls in dieser Liste Konstruktoren sich befinden,            */
/*            so muessen diese durch st-expr ebenso ausgelagert werden.     */
/*            Falls der Heap voll sein sollte, liefert diese Routine        */
/*            eine Null zurueck.                                            */
/* globals -- S_a    <w>                                                    */
/* calls   -- newdesc, newheap, st_expr;                                    */
/*--------------------------------------------------------------------------*/

STACKELEM st_list()
{
  register PTR_DESCRIPTOR desc;
  register int dim;
  STACKELEM x;

  START_MODUL("st_list");

  dim = ARITY((x = POPSTACK(S_a)));

  if ((desc = newdesc()) == NULL)
  {
    PUSHSTACK(S_a,x);
    END_MODUL("st_list");
    return((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,(T_STRING(x) ?
                                TY_STRING :
                                TY_UNDEF  ) );   /*      string or list ?    */
  L_LIST((*desc),special) = EXTR_EDIT(x);        /*      Edit-Feld  SJ-ID    */
  L_LIST((*desc),dim) = dim;

  if (newheap(dim,A_LIST((*desc),ptdv)) == 0)
  {
    PUSHSTACK(S_a,SET_ARITY(LIST,dim));
    DEC_REFCNT(desc);
    END_MODUL("st_list");
    return((STACKELEM)NULL);
  }

  while (dim > 0)
   if ((L_LIST((*desc),ptdv)[--dim] = (T_HEAPELEM) (T_CON(READSTACK(S_a)) ? st_expr() : POPSTACK(S_a))) == NULL)
    {
      while (++dim < R_LIST((*desc),dim))
      {
        if T_POINTER(x = ((STACKELEM *)R_LIST((*desc),ptdv))[dim]);
          INC_REFCNT((PTR_DESCRIPTOR)x);
        PUSHSTACK(S_a,x);
      }
      PUSHSTACK(S_a,SET_ARITY(LIST,dim));
      DEC_REFCNT(desc);
      END_MODUL("st_list");
      return((STACKELEM)NULL);
    }

  END_MODUL("st_list");
  return((STACKELEM)desc);
}


