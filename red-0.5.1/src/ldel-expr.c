/* $Log: ldel-expr.c,v $
 * Revision 1.2  1992/12/16  12:48:57  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 *  ldel-expr.c  - external: ldel_expr;
 *                 internal:
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

/* RS 30/10/92 */ 
extern void stack_error();         /* rheap.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */       /* rstack.c */
#endif

/*----------------------------------------------------*/
/*  ldel_expr() -- loescht einen Ausdruck auf source  */
/*----------------------------------------------------*/

void ldel_expr(source)
  StackDesc *source;
{
#define S_s *source

   STACKELEM x;

   START_MODUL("ldel_expr");
   PUSHSTACK(S_m,KLAA);

main_s:
  x = POPSTACK(S_s);

  if (T_PRELIST(x))
  {                       /* PRELIST hat zwei Unterausdruecke */
     ldel_expr(source);
     ldel_expr(source);
     goto main_m;
  }
  else
  if (T_CON(x))
  {
     PUSHSTACK(S_m,x);
     goto main_s;
  }
  else
     goto main_m;

main_m:
  x = READSTACK(S_m);

  if (T_CON(x))
  {
     if (ARITY(x) > 1)
     {
        WRITESTACK(S_m,DEC(x));
        goto main_s;
     }
     else
     {
        PPOPSTACK(S_m);
        goto main_m;
     }
  }
  else
  {
     PPOPSTACK(S_m);      /* KLAA */
     END_MODUL("ldel_expr");
     return;
  }
}       /* end of ldel_expr */

/* end of file ldel-expr.c */
