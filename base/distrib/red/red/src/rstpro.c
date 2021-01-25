/* $Log: rstpro.c,v $
 * Revision 1.7  1995/05/22 11:28:32  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.6  1994/05/04  07:04:12  ach
 * first beta release of distributed tilde-version
 *
 * Revision 1.5  1994/01/04  14:15:33  mah
 * st_arglist added
 *
 * Revision 1.4  1993/11/23  15:46:52  car
 * possible refcount error
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:51:11  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



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
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

#include "dbug.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();   /* heap:c */
extern int newheap();              /* heap:c */
extern void trav_a_hilf();         /* trav:c */
extern int mvheap();               /* rhinout.c */
extern int freeheap();                 /* rheap.c */
extern void freedesc();                /* rheap.c */

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

#if D_MESS
#include "d_mess_io.h"
#endif

#if D_SLAVE
#define POP_A()       D_POPSTACK(D_S_A)
#define PUSH_A(arg)   D_PUSHSTACK(D_S_A,arg)
#define READ_A()      D_READSTACK(D_S_A)
#else
#define POP_A()       POPSTACK(S_a)
#define PUSH_A(arg)   PUSHSTACK(S_a,arg)
#define READ_A()      READSTACK(S_a)
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

/* RS 30/10/92 */
extern void stack_error();             /* rstack.c */
/* END of RS 30/10/92 */

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
  register int size/*, * desc_entry*/;  /* RS 30/10/92 */
  STACKELEM x;

  DBUG_ENTER ("store_e");

  START_MODUL("store_e");
#if MEASURE
  measure(STORE_EXPR);
#endif

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((desc = (*d_m_newdesc)()) == NULL) {
#else
  if ((desc = newdesc()) == NULL) {
#endif
    END_MODUL("store_e");
    DBUG_RETURN((STACKELEM)NULL);
  }
  DESC_MASK(desc,1,C_EXPRESSION,TY_EXPR);
  size = ARITY(x = POP_A()) + 1;
#if (D_SLAVE && D_MESS && D_MHEAP)
  if (!(*d_m_newheap)(1 + size,A_EXPR((*desc),pte))) {
#else
  if (!newheap(1 + size,A_EXPR((*desc),pte))) {
#endif
    DEC_REFCNT(desc);
    PUSH_A(x);
    END_MODUL("store_e");
    DBUG_RETURN((STACKELEM)NULL);
  }
  RES_HEAP;
  p    = (PTR_HEAPELEM) R_EXPR((*desc),pte);
  *p++ = (T_HEAPELEM) (size + TET);         /* 0. Eintrag: Groesse     */
  *p   = (T_HEAPELEM) x;                    /* 1. Eintrag: Konstruktor */
  p   += --size;
  for ( ; size ; --size )
    *p-- = (T_HEAPELEM) POP_A();
  REL_HEAP;
  END_MODUL("store_e");
  DBUG_RETURN((STACKELEM)desc);
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

  DBUG_ENTER ("store_l");

  START_MODUL("store_l");

  dim = ARITY((x = POP_A()));

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((desc = (*d_m_newdesc)()) == NULL) {
#else 
  if ((desc = newdesc()) == NULL) {
#endif
    PUSH_A(x);
    END_MODUL("store_l");
    DBUG_RETURN((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  L_LIST((*desc),special) = EXTR_EDIT(x);
  L_LIST((*desc),dim) = dim;

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((*d_m_newheap)(dim,A_LIST((*desc),ptdv)) == 0) {
#else
  if (newheap(dim,A_LIST((*desc),ptdv)) == 0) {
#endif
    PUSH_A(x);
    DEC_REFCNT(desc);
    END_MODUL("store_l");
    DBUG_RETURN((STACKELEM)NULL);
  }
  RES_HEAP;
  p = R_LIST((*desc),ptdv) + dim;
  while (dim-- > 0)
   *--p = (T_HEAPELEM)POP_A();
  REL_HEAP;
  END_MODUL("store_l");
  DBUG_RETURN((STACKELEM)desc);
}

/* st_arglist aus lneu-src kopiert, mah 210993 */

#if WITHTILDE
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

#if D_SLAVE
  dim = ARITY((x = D_POPSTACK(D_S_A)));
#else
  dim = ARITY((x = POPSTACK(S_a)));
#endif

  if ((desc = newdesc()) == NULL) {
#if D_SLAVE
    D_PUSHSTACK(D_S_A,x);
#else
    PUSHSTACK(S_a,x);
#endif
    END_MODUL("st_arglist");
    return((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  L_LIST((*desc),special) = EXTR_EDIT(x);
  L_LIST((*desc),dim) = dim;

  if (newheap(dim,A_LIST((*desc),ptdv)) == 0) {
#if D_SLAVE
    D_PUSHSTACK(D_S_A,x);
#else
    PUSHSTACK(S_a,x);
#endif
    DEC_REFCNT(desc);
    END_MODUL("st_arglist");
    return((STACKELEM)NULL);
  }
  RES_HEAP;
  p = R_LIST((*desc),ptdv) ;
  while (dim-- > 0)
#if D_SLAVE
   *p++ = (T_HEAPELEM)D_POPSTACK(D_S_A);
#else
   *p++ = (T_HEAPELEM)POPSTACK(S_a);
#endif
  REL_HEAP;
  END_MODUL("st_arglist");
  return((STACKELEM)desc);
}

#endif /* WITHTILDE */

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

  DBUG_ENTER ("st_expr");

  START_MODUL("st_expr");

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((desc = (*d_m_newdesc)()) == NULL)
#else
  if ((desc = newdesc()) == NULL)
#endif
  {
    END_MODUL("st_expr");
    DBUG_RETURN((STACKELEM)NULL);
  }
  DESC_MASK(desc,1,C_EXPRESSION,TY_EXPR);

  TRAV_A_HILF;
  if (mvheap(A_EXPR((*desc),pte)) == 0)
  {
    DEC_REFCNT(desc);
    END_MODUL("st_expr");
    DBUG_RETURN((STACKELEM)NULL);
  }
  END_MODUL("st_expr");
  DBUG_RETURN((STACKELEM)desc);
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

  DBUG_ENTER ("st_list");

  START_MODUL("st_list");

  dim = ARITY((x = POP_A()));

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((desc = (*d_m_newdesc)()) == NULL)
#else
  if ((desc = newdesc()) == NULL)
#endif
  {
    PUSH_A(x);
    END_MODUL("st_list");
    DBUG_RETURN((STACKELEM)NULL);
  }
  LI_DESC_MASK(desc,1,C_LIST,(T_STRING(x) ?
                                TY_STRING :
                                TY_UNDEF  ) );   /*      string or list ?    */
  L_LIST((*desc),special) = EXTR_EDIT(x);        /*      Edit-Feld  SJ-ID    */
  L_LIST((*desc),dim) = dim;

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((*d_m_newheap)(dim,A_LIST((*desc),ptdv)) == 0)
#else
  if (newheap(dim,A_LIST((*desc),ptdv)) == 0)
#endif
  {
    PUSH_A(SET_ARITY(LIST,dim));
    DEC_REFCNT(desc);
    END_MODUL("st_list");
    DBUG_RETURN((STACKELEM)NULL);
  }

  while (dim > 0)
   if ((L_LIST((*desc),ptdv)[--dim] = (T_HEAPELEM) (T_CON(READ_A()) ? st_expr() : POP_A())) == NULL)
    {
      while (++dim < (int) R_LIST((*desc),dim))
                 /* int gecastet von RS 04/11/92 */ 
      {
	/* changed by car 23.11.93 */
        /* if T_POINTER(x = ((STACKELEM *)R_LIST((*desc),ptdv))[dim]); */
        if T_POINTER(x = ((STACKELEM *)R_LIST((*desc),ptdv))[dim])
          INC_REFCNT((PTR_DESCRIPTOR)x);
        PUSH_A(x);
      }
      PUSH_A(SET_ARITY(LIST,dim));
      DEC_REFCNT(desc);
      END_MODUL("st_list");
      DBUG_RETURN((STACKELEM)NULL);
    }

  END_MODUL("st_list");
  DBUG_RETURN((STACKELEM)desc);
}



