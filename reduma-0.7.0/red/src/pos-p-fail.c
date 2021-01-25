/* $Log: pos-p-fail.c,v $
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
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
extern int mvheap();               /* rhinout.c */
extern int _argsneeded;   /* state:c - Zahl der benoetigten Argumente */
extern int red_dummy();  
extern void DescDump();
extern void res_heap();
extern void test_inc_refcnt();
extern void rel_heap();

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* st_switch_clos          aufgerufen aus: nomat_post                     */
/*                                                                        */
/* Modul fuer die Postprocessing-Phase: Anlegen eines Closure-Deskriptors */
/* fuer einen case-Ausdruck, der in der Preprocessing-Phase mit einer     */
/* oder mehreren lambda-Abstraktionen zu einem mehrstelligen case-Aus-    */
/* druck zusammengefasst wurde. In die pta-Liste werden die Argumente     */
/* eingetragen, die in der Processing-Phase zu einer Liste zusammenge-    */
/* fasst wurden. Diese Liste steht in dem Nomatch-Deskriptor.             */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

STACKELEM    st_switch_clos( nomat_desc )
PTR_DESCRIPTOR nomat_desc;                         
{
  PTR_DESCRIPTOR desc, sw_desc, arg_desc; 
  STACKELEM                       *p_arg;        
  STACKELEM                            x;
  int                      j,i, anz_args;
                          

  START_MODUL("st_switch_clos");

  if ((desc = newdesc()) == NULL)
  {
    END_MODUL("st_clos");
    return((STACKELEM)NULL);
  }   

  DESC_MASK(desc,1,C_FUNC,TY_CLOS);

#if 0
  L_CLOS((*desc),ndargs) = 0;
#else
  L_CLOS((*desc),nargs) = 0; /* lred-closure-struktur */
#endif /* 0 */

  L_CLOS((*desc),nfv) = 0;

/* mah 280194 das hier muss erstes element von pta werden! */
/* es gibt keinen ptfd mehr                                */
#if 0
  L_CLOS((*desc),ptfd) = ( sw_desc = (PTR_DESCRIPTOR)R_NOMAT((*nomat_desc),ptsdes));
    /* der Switch-Deskriptor */
  INC_REFCNT(sw_desc);
#endif /* 0 */
  
  /* Argument fuer die case-Funktion wird aus dem Nomatch-Deskriptor entnommen */
                      
     arg_desc = (PTR_DESCRIPTOR) R_NOMAT((*nomat_desc),guard_body)[3] ;
       /* Argumentliste */

     anz_args =  R_LIST((*arg_desc),dim);
       /* Anzahl der Argumente */

  if (newheap(anz_args + 1,A_CLOS((*desc),pta)) == NULL)
  {
    END_MODUL("st_switch_clos");
    return((STACKELEM)NULL);
  }     

/*   RES_HEAP;*/
  p_arg = (STACKELEM *)R_CLOS((*desc),pta);

  /* insert case - descriptor     */
  /* descriptor in dummy mode :-) */
  /* only entry ever used is ptd  */

  {
    PTR_DESCRIPTOR ptd;

    MAKEDESC(ptd,1,C_FUNC,TY_CASE);

    RES_HEAP;

    L_CASE((*ptd),args)  = 0; 
    L_CASE((*ptd),nargs) = 0;
    L_CASE((*ptd),nargs) = 0;
    L_CASE((*ptd),ptd)   =( sw_desc = (PTR_DESCRIPTOR)R_NOMAT((*nomat_desc),ptsdes));
    L_CASE((*ptd),ptc)   = 0;

    p_arg[0] = (STACKELEM)ptd;
  }

  INC_REFCNT(sw_desc);

  j = 0;                         

  /* Eintragen der Argumente in die pta-Liste: */

#if 0
  for ( i = anz_args ; i > 0 ; i--  )
#else
  for ( i = 1 ; i <= anz_args ; i++ )
#endif
  {
    p_arg[i] = ( x = (STACKELEM) R_LIST((*arg_desc),ptdv)[j ] ) ;
    j++;
    if( T_POINTER(x) )
       INC_REFCNT((PTR_DESCRIPTOR)x);
  }

#if 0
  p_arg[0] = anz_args;
#else
  L_CLOS(*desc,args) = anz_args;
#endif /* 0 */
  L_CLOS(*desc,ftype) = TY_CASE;

  REL_HEAP;

  END_MODUL("st_switch_clos");
  return((STACKELEM)desc);

}




