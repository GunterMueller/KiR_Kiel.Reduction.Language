/* $Log: rconvir.c,v $
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

/*----------------------------------------------------------------------------
 * convr -- legt fuer den Realparameter den es erhaelt eine entsprechende
 *          Stackelementdarstellung auf den E-Stack.
 * called by -- ret_scal;
 * calls     -- keine;
 * globals   -- S_e;
 *              _trunc <r>;
 *----------------------------------------------------------------------------
 */
void convr(realval)
double realval;
{
  char hilf[20];
/*register int   index = 9;
  register int   exp  = 13;                      RS 30/10/92 */ 
  register int   i;
  BOOLEAN  positive = TRUE;
  double save_realval;
  extern int _trunc;

  START_MODUL("convr");

  if (realval < 0) {
    positive = FALSE;
    realval = -realval;
  }
  if (_trunc > 0) {
    save_realval = realval;
    for (i = 0; i < _trunc; i++)
      realval *= 10.0;
    if (realval > (double) ((int) 0x7fffffff)) {
      realval = save_realval;
      goto exp_darstellung;
    }
    else realval = (double) ((int)realval); /* truncation */
    for ( ; i > 0; i--)
      realval /= 10.0;
  }  /* Ende von _trunc > 0 */
  if (realval == 0.0) { /* sprintf funktioniert hier nicht */
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,'0'));
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,'.'));
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,'0'));
    END_MODUL("convr");
    return;
  }
exp_darstellung:
  if ((realval >= 1e8) || (realval <= 1e-5)) {
    /* Exponentialdarstellung */
    register int   index = 9;
    register int   exp  = 13;
    sprintf(hilf,"%.8e",realval);
    /* im Hilfstring hat man nun die Realzahl in folgendem Format:
     *                      1 1 1 1 1
     * |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4|
     * |z . z z z z z z z z e + z z  |
     * ===============================
     */
    while ((index > 2) && (hilf[index] == '0')) index--;
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,hilf[index--]));
    while (index >= 0)
      PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,hilf[index--]));
    /* nun das Vorzeichen der Mantisse */
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY((positive ? PLUS : MINUS),1));
    /* nun der Exponent : */
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,'.'));
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,hilf[exp--]));
    if (hilf[exp] != '0')
      PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,hilf[exp--]));
    else exp--;
    /* nun das Vorzeichen des Exponenten: */
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY((hilf[exp] == '+' ? PLUS : MINUS),1));
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(EXP,2));
    END_MODUL("convr");
  }
  else {
    /* Darstellung ohne Exponenten */
    register int index = 16;
    sprintf(hilf,"%17.8f",realval);
    /* im Hilfstring hat man nun die Realzahl in folgendem Format:
     * |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6|
     * |? ? ? ? ? ? ? z . z z z z z z z z|
     * ===============================
     */
    /* falls die Hostmaschine double-Zahlen auch nur mit 32 Bit darstellt,
       koennen nach der Truncateoperation durch Rechenungenauigkeiten
       Ziffern hinter dem Komma wieder ungleich Null werden!
       Diesem beugen wir dadurch vor:                                  */
    if ((_trunc < 8) && (_trunc > 0))
        index -= 8 - _trunc;
    /* hintere Nullen entfernen */
    while ((index > 9) && (hilf[index] == '0'))
      index--;
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG0,hilf[index--]));
    while (index > 6)
      PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,hilf[index--]));
    while ((index >= 0) && (hilf[index] >= '0') && (hilf[index] <= '9'))
      PUSHSTACK(S_e,(STACKELEM) SET_ARITY(DIG1,hilf[index--]));
    PUSHSTACK(S_e,(STACKELEM) SET_ARITY((positive ? PLUS : MINUS),1));
    END_MODUL("convr");
  }
}
