/* $Log: pmdimli.c,v $
 * Revision 1.1  1994/11/14  10:02:17  mah
 * Initial revision
 * */

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
extern void DescDump();

extern void stack_error();
extern  StackDesc *ps_a;                   /* Hilfsstack, auf den die
                                              Variableninst waehrend des
                                              Matching gepuscht werden */
#define PUSH_PM(x) PUSHSTACK(*ps_a,x)

/*-------------------------------------------------------------*/
/* pmdimli (li_desc) liefert als Resultat einen Descriptor , in*/
/* dem die Laenge des Argumentdeskriptors lidesc steht         */
/*------------------------------------------------------------*/

int pmdimli(li_desc)
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
      if ((desc = conv_int_digit(R_LIST(DESC(li_desc),dim))) == NULL) goto fail;
    goto success;
fail:
  END_MODUL("pmdimli");
  return(0);

success:
  END_MODUL("pmdimli");
  PUSH_PM(desc);/* Deskriptor auf a-stack laden */
  return(1);
}


