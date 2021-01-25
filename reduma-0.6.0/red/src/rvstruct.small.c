/* $Log: rvstruct.small.c,v $
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* dummy functions only!!! */

/*****************************************************************************/
/* rvstruct.c                                                                */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rlmvt.h"

int red_vcut(arg1,arg2)
register STACKELEM arg1,arg2;
{
  START_MODUL("red_vcut");
  END_MODUL("red_vcut");
  return(0);
} /**** end of function red_vcut ****/

int red_vselect(arg1,arg2)
register STACKELEM arg1,arg2;
{
  START_MODUL("red_vselect");
  END_MODUL("red_vselect");
  return(0);
} /**** end of function red_vselect ****/

int red_vreplace(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_vreplace");
  END_MODUL("red_vreplace");
  return(0);
} /**** end of function red_vreplace ****/

int red_vrotate(arg1,arg2)
STACKELEM arg1,arg2;
{
  START_MODUL("red_vrotate");
  END_MODUL("red_vrotate");
  return(0);
} /**** end of function red_vrotate ****/

int red_vunite(arg1,arg2)
register STACKELEM arg1,arg2;
{
  START_MODUL("red_vunite");
  END_MODUL("red_vunite");
  return(0);

} /**** end of function red_vunite ****/

/***************************** end of file rvstruct.c ************************/
