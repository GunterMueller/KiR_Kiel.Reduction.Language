/* $Log: rbibfunc.small.c,v $
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* dummys */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"
#include <string.h>                 /* to store length of string PI           */
#include <math.h>                   /* to eval. PIPREC and BIBPREC            */
                                    /* sin(x),cos(x),tan(x),log(x),exp(x)     */
#include <setjmp.h>

BOOLEAN inittrigdesc;                  /* flag for trigdesc()                 */
                                       /* (if called once then TRUE)          */
BOOLEAN initexpdesc;                   /* flag for expdesc()                  */
                                       /* (if called once then TRUE)          */

void inittrig()
{
  return;
}

void initexp()
{
  return;
}

int red_sin(arg)
register STACKELEM arg;
{
  return(0);
}

int red_cos(arg)
register STACKELEM arg;
{
  return(0);
}

int red_tan(arg)
register STACKELEM arg;
{
  return(0);
}

int red_ln(arg)
register STACKELEM arg;
{
  return(0);
}

int red_exp(arg)
register STACKELEM arg;
{
  return(0);
}

