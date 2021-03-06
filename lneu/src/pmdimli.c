/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*----------------------------------------------------------------*/
/*  include files                                                 */
/*                                                                */
/*----------------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "pminstr.h"


/*----------------------------------------------------------------*/
/* Typdefinitionen Makros                                         */
/*----------------------------------------------------------------*/
#define DESC(x) (*((PTR_DESCRIPTOR)(x)))

extern BOOLEAN _formated;                 /* <r>, ldim, dim */
extern PTR_DESCRIPTOR conv_int_digit();     /* digit-v1:c */
extern PTR_DESCRIPTOR newdesc();            /* heap:c */

extern  StackDesc S_pm;                   /* Hilfsstack, auf den die
                                             Variableninst waehrend des
                                             Matching gepuscht werden */

/*-------------------------------------------------------------*/
/* pmdimli (li_desc) liefert als Resultat einen Descriptor , in*/
/* dem die Laenge des Argumentdeskriptors lidesc steht         */
/*------------------------------------------------------------*/

pmdimli(li_desc)
register STACKELEM li_desc;
{
  register PTR_DESCRIPTOR desc;
  START_MODUL("pmdimli");


    if (_formated)
    {
      NEWDESC(desc);
      DESC_MASK(desc,1,C_SCALAR,TY_INTEGER);
      L_SCALAR((*desc),vali) = R_LIST(DESC(li_desc),dim);
    }
    else
#if LARGE
      if ((desc = conv_int_digit(R_LIST(DESC(li_desc),dim))) == NULL) goto fail;
#else
      goto fail;
#endif 
    goto success;
fail:
  END_MODUL("pmdimli");
  return(0);

success:
  END_MODUL("pmdimli");
  PUSHSTACK(S_i, desc);/* Deskriptor auf i-stack laden */
  return(1);
}


