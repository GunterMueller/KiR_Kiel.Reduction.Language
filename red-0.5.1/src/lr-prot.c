/* $Log: lr-prot.c,v $
 * Revision 1.2  1992/12/16  12:49:08  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 *  lr-prot.c  -  external:  set_protect
 *           Aufgerufen durch:  ea_retrieve
 *-----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */

/* ach 30/10/92 */
extern void stack_error();
/* end of ach */

#if DEBUG
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */       /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 *  set_protect  --  fuegt PROTECTs vor LREC-Funktionsnamen ein
 *-----------------------------------------------------------------------------
 */
void set_protect(plrec,fname)
   PTR_DESCRIPTOR plrec,fname;
{
   PTR_DESCRIPTOR x,y;
   int funcs,i;

   START_MODUL("set_protect");

   PUSHSTACK(S_a,KLAA);

   if (plrec == NULL)
   {
      while (!(T_KLAA(READSTACK(S_hilf))))
      {
         x = (PTR_DESCRIPTOR) POPSTACK(S_hilf);
         if (R_DESC((*x),type) == TY_LREC)
         {
            funcs = R_LREC((*x),nfuncs);
            for (i=1; i <= funcs; i++)
            {
               if ((PTR_DESCRIPTOR) R_LREC((*x),namelist)[i] == fname)
                  PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
                          /* Vergleich mit den externen func_names */
            }
         }
         else /* type == TY_LREC_ARGS */
         {
            y = (PTR_DESCRIPTOR) R_LREC_ARGS((*x),ptdd);
            funcs = R_LREC((*y),nfuncs);
            for (i=1; i <= funcs; i++)
            {
               if ((PTR_DESCRIPTOR) R_LREC((*y),namelist)[i] == fname)
                  PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
                          /* Vergleich mit den externen func_names */
            }
         }
         PUSHSTACK(S_a,(STACKELEM)x);
      }
   }
   else          /* plrec != NULL */
   {
   while (!(T_KLAA(READSTACK(S_hilf))))
   {
      x = (PTR_DESCRIPTOR) POPSTACK(S_hilf);
      if (R_DESC((*x),type) == TY_LREC)
      {
         if (x != plrec)
         {
            funcs = R_LREC((*x),nfuncs);

            for (i=1; i <= funcs; i++)
            {
               if ((PTR_DESCRIPTOR) R_LREC((*x),namelist)[i] == fname)
                  PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
                          /* Vergleich mit den externen func_names */
            }
         }
         else /* x == plrec */
         {
            PUSHSTACK(S_a,(STACKELEM)x);
            goto rest;
         }
      }
      else /* type == TY_LREC_ARGS */
      {
         y = (PTR_DESCRIPTOR) R_LREC_ARGS((*x),ptdd);
         if (y != plrec)
         {
            funcs = R_LREC((*y),nfuncs);

            for (i=1; i <= funcs; i++)
            {
               if ((PTR_DESCRIPTOR) R_LREC((*y),namelist)[i] == fname)
                  PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
                          /* Vergleich mit den externen func_names */
            }
         }
         else /* y == plrec */
         {
            PUSHSTACK(S_a,(STACKELEM)x);
            goto rest;
         }
      }

      PUSHSTACK(S_a,(STACKELEM)x);
   }    /* end while */
   }    /* end else  */

rest:
   while (!(T_KLAA(READSTACK(S_a))))
   {
      PUSHSTACK(S_hilf,POPSTACK(S_a));
   }

   PPOPSTACK(S_a); /* TB, 30.10.92 */                 /* KLAA */
   END_MODUL("set_protect");
   return;
}
/* end of lr-prot.c */
