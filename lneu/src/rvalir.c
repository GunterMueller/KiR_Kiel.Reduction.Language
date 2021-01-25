/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */




/*-----------------------------------------------------------------------------
 * Module der PREPROCESSING PHASE
 * val:c - external: vali, valr;
 *  Aufgerufen von : st_scal, st_vec;
 * val:c - internal: keine
 *-----------------------------------------------------------------------------
 */

/* last update  by     why

   copied       by     to

 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"


/*-----------------------------------------------------------------------------
 * vali -- konvertiert den auf dem E-Stack liegenden Digitstring in Binaerdar-
 *         stellung und gibt das Ergebnis, einen Integer, zurueck.
 *
 * Verwendete Globale Variablen:
 *         S_e  <w>
 * Verwendete externe Funktionen:
 *         keine
 *-----------------------------------------------------------------------------
 */
int vali()
{
  register int result = 0;
           int sign;
  register STACKELEM    x;

  START_MODUL("vali");

  /* zunaechst wird das Vorzeichen eingelesen: */
  if (T_PLUS(POPSTACK(S_e))) sign = 1;
  else                       sign = -1;

  while (T_DIG1((x = POPSTACK(S_e))))
    result = 10 * result + ((x & F_VALUE) >> O_VALUE) - '0';

  /* x muss nun ein DECPT sein */

  END_MODUL("vali");

  return(result * sign);
}

/*-----------------------------------------------------------------------------
 * valr -- konvertiert den auf dem E-Stack liegenden Digitstring in Binaerdar-
 *         stellung und gibt das Ergebnis, einen Double, zurueck.
 *         Der Ausgabeparameter ist INTEGER falls der Digitstring mit einem
 *         Dezimalpunkt aufhoert und in das Integer Format konvertierbar ist.
 *
 * Verwendete Globale Variablen:
 *         S_e  <w>
 * Verwendete externe Funktionen:
 *         keine
 *-----------------------------------------------------------------------------
 */
double valr(type)
int * type;
{
  register double result = 0.0;
  register double exp  = 1.0;
  register STACKELEM      x;
  int      sign ;

  START_MODUL("valr");

  if (T_EXP( (x = POPSTACK(S_e)) ))
  {
    register int i = vali();
    if (i < 0)
      for ( ; i < 0; i++ )  exp /= 10.0;
    else
      for ( ; i > 0; i--)   exp *= 10.0;
    x = POPSTACK(S_e);
  }

  /* nun das Vorzeichen */
  sign = (T_MINUS(x) ? -1 : 1);

  /* nun die Zeichen bis zum Dezimalpunkt: */
  while (!T_DECPT((x = POPSTACK(S_e))))
    result = 10.0 * result + ((x & F_VALUE) >> O_VALUE) - '0';

  *type = (T_DIG0(x) ? INTEGER : REAL);

  if (T_DIG1(x))  /* nun die Zeichen nach dem Dezimalpunkt: */
  {
    register double position = 1.0;
    do
    {
      x = POPSTACK(S_e);
      position /= 10.0;
      result += (((x & F_VALUE) >> O_VALUE) - '0') * position;
    }
    while (T_DIG1(x));
  }

  result *= exp;

  /* Typ Uberpruefung: */
  if (*type == INTEGER)
    if ((result >= (double) 0x7fffffff) ||    /* zu gross ?            */
        ( result != (double)((int)result)))  /* keine ganz Zahl ?     */
      *type = REAL;

  END_MODUL("valr");
  return(result * sign);
}


