/* $Log: rvalir.c,v $
 * Revision 1.2  1992/12/16  12:51:24  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */




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
#if nCUBE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

#if nCUBE
#define POP_H()    D_POPSTACK(D_S_H)
#else
#define POP_H()    POPSTACK(S_e)
#endif

#if D_MESS
#include "d_mess_io.h"
#endif

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
  if (T_PLUS(POP_H())) sign = 1;
  else                       sign = -1;

  while (T_DIG1((x = POP_H())))
    result = 10 * result + ((int)(x & F_VALUE) >>  O_VALUE) - '0';
                  /* int gecastet von RS 04/11/92 */ 

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

  if (T_EXP( (x = POP_H()) ))
  {
    register int i = vali();
    if (i < 0)
      for ( ; i < 0; i++ )  exp /= 10.0;
    else
      for ( ; i > 0; i--)   exp *= 10.0;
    x = POP_H();
  }

  /* nun das Vorzeichen */
  sign = (T_MINUS(x) ? -1 : 1);

  /* nun die Zeichen bis zum Dezimalpunkt: */
  while (!T_DECPT((x = POP_H())))
    result = 10.0 * result + ((int) (x & F_VALUE) >>  O_VALUE) - '0';
            /* int gecastet von RS 04/11/92 */ 

  *type = (T_DIG0(x) ? INTEGER : REAL);

  if (T_DIG1(x))  /* nun die Zeichen nach dem Dezimalpunkt: */
  {
    register double position = 1.0;
    do
    {
      x = POP_H();
      position /= 10.0;
      result += (((int) (x & F_VALUE) >>  O_VALUE) - '0') * position;
            /* int gecastet von RS 04/11/92 */ 
    }
    while (T_DIG1(x));
  }

  result *= exp;

  /* Typ Uberpruefung: */
  if (*type == INTEGER)
    if ((result >= (double) 0x3fffffff) ||    /* zu gross ?(incl. Tags)*//*BM*/
        ( result != (double)((int)result)))  /* keine ganze Zahl ?    */
      *type = REAL;

  END_MODUL("valr");
  return(result * sign);
}


