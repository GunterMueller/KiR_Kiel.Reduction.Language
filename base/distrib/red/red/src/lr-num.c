/* $Log: lr-num.c,v $
 * Revision 1.2  1992/12/16 12:49:07  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 * test_num:c - external: test_num;
 * Aufgerufen durch:  lrecfunc;
 *-----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/* ach 29/10/92 */
extern void stack_error();          /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
#endif
/* end of ach */

/*-----------------------------------------------------------------------------
 * test_num --
 * globals -- S_e,S_a,S_m,S_m1   <wr>
 *-----------------------------------------------------------------------------
 */
extern void ptrav_a_e();

int test_num()
{
  STACKELEM x/*,y*/;                    /* ach 29/10/92 */

  START_MODUL("test_num");

  PUSHSTACK(S_m,KLAA);

main_ea:
  x = READSTACK(S_e);

  if (T_CON(x))
  {
     PUSHSTACK(S_m,x);
     PUSHSTACK(S_m1,POPSTACK(S_e));
     goto main_ea;
  }

  if (T_NUM(x))
  {
     goto main_aem;
  }

  PUSHSTACK(S_a,POPSTACK(S_e));
  goto main_eam;

main_eam:
  x = READSTACK(S_m);

  if (T_CON(x))
     if (ARITY(x) > 1)
     {
        WRITESTACK(S_m,DEC(x));
        goto main_ea;
     }
     else
     {
        PPOPSTACK(S_m);
        PUSHSTACK(S_a,POPSTACK(S_m1));
        goto main_eam;
     }
  else
  {
     PPOPSTACK(S_m);                        /* KLAA */
     PTRAV_A_E;
     END_MODUL("test_num");
     return(0);
  }

main_ae:
  x = READSTACK(S_a);

  if (T_CON(x))
  {
     PUSHSTACK(S_m,SET_ARITY(x,1));
     PUSHSTACK(S_m1,POPSTACK(S_a));
     goto main_ae;
  }

  PUSHSTACK(S_e,POPSTACK(S_a));
  goto main_aem;

main_aem:
  x = READSTACK(S_m);

  if (T_CON(x))
     if (ARITY(x) < ARITY(READSTACK(S_m1)))
     {
        WRITESTACK(S_m,INC(x));
        goto main_ae;
     }
     else
     {
        PPOPSTACK(S_m);
        PUSHSTACK(S_e,POPSTACK(S_m1));
        goto main_aem;
     }
  else
  {
     PPOPSTACK(S_m);                        /* KLAA */
     END_MODUL("test_num");
     return(1);
  }
}


