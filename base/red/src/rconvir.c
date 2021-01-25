/* $Log: rconvir.c,v $
 * Revision 1.4  1996/02/27 13:08:21  car
 * last (?) rconvr bug removed
 *
 * Revision 1.3  1996/02/27  12:24:14  car
 * real output format fixed for asm output and editor
 *
 * Revision 1.2  1992/12/16  12:49:30  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */





/****************************************************************************/
/*                     MODUL FOR POSTPROCESSING PHASE                       */
/* ------------------------------------------------------------------------ */
/* rconv.c   - external  : convi, convr                                     */
/*           - internal  : none                                             */
/*           - called by : ret_scal, ret_mvt                                */
/****************************************************************************/

#include <math.h>
#include <strings.h>
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/* RS 30/10/92 */ 
extern void stack_error();                      /* rstack.c */
/* END of RS 30/10/92 */ 

/*----------------------------------------------------------------------------
 * convi -- legt fuer den Integerparameter den es erhaelt eine entsprechende
 *          Stackelementdarstellung auf den E-Stack.
 * called by -- ret_scal;
 * calls     -- keine;
 * globals   -- S_e;
 *----------------------------------------------------------------------------
 */
void convi(intval)
register int intval;
{
  BOOLEAN  positive = TRUE;

  START_MODUL("convi");

  if (intval < 0)
  {
    if (intval == MININT)				/* BM */
      post_mortem("convi :  integer overflow");
    positive = FALSE;
    intval = -intval;
  }

  PUSHSTACK(S_e,(STACKELEM) DECPT); /* auch Integer haben am Ende einen '.' */

  do
  {
    PUSHSTACK(S_e,(STACKELEM) SET_VALUE(DIG1,((intval % 10) + '0')));
    intval /= 10;
  }
  while (intval > 0);

  PUSHSTACK(S_e,(STACKELEM) (positive ? SET_ARITY(PLUS,1)
                                      : SET_ARITY(MINUS,1)));
  END_MODUL("convi");
}

/*
 * convrstr -- converts real to string
 * called by -- printelements, convr
 */
char *convrstr(val)
double val;
{
  static char tmp_string[32];

  sprintf(tmp_string, "%+.15g", val);
  if ((index(tmp_string, '.') == NULL) && (index(tmp_string, 'e') == NULL))
    strcat(tmp_string, ".0");
  return(tmp_string);
}

/*----------------------------------------------------------------------------
 * convr -- legt fuer den Realparameter den es erhaelt eine entsprechende
 *          Stackelementdarstellung auf den E-Stack.
 * called by -- ret_scal;
 * calls     -- convrstr;
 * globals   -- S_e;
 *----------------------------------------------------------------------------
 */
static void pushdigit(c)
char c;
{
  switch(c) {
    case '.': PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,'.')); 
      break;
    case '+': PUSHSTACK(S_e,(STACKELEM) SET_ARITY(PLUS,1)); 
      break;
    case '-': PUSHSTACK(S_e,(STACKELEM) SET_ARITY(MINUS,1)); 
      break;
    case 'e': PUSHSTACK(S_e,(STACKELEM) SET_ARITY(EXP,2));
      break; 
    default: PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,c));
    }
}
    
void convr(realval)
double realval;
{
  char * str, * p;

  START_MODUL("convr");

  str = convrstr(realval);
  if ((p = index(str, 'e')) == NULL)
    p = &str[strlen(str)];
  PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,*--p));
  do {
    pushdigit(*--p);
  } while(p > str);
  if (index(str, 'e')) {
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,'.'));
    p = &str[strlen(str)];
    do {
      pushdigit(*--p);
    } while(*p != 'e');
    }
  END_MODUL("convr");
}
