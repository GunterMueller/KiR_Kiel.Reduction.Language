/* $Log: rmstruct.small.c,v $
 * Revision 1.2  1992/12/16  12:50:20  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* dummy functions only!!! */

/*****************************************************************************/
/* rmstruct.c                                                                */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rlmvt.h"

int red_mcut(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_mcut");
  END_MODUL("red_mcut");
  return(0);
}   /**** end of function red_mcut ****/

int red_mselect(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_mselect");
  END_MODUL("red_mselect");
  return(0);
} /**** end of function red_mselect ****/

int red_mreplace(arg1,arg2,arg3,arg4)
STACKELEM arg1,arg2,arg3,arg4;
{
  START_MODUL("red_mreplace");
  END_MODUL("red_mreplace");
  return(0);
} /**** end of function red_mreplace ****/

int red_mre_c(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_mre_c");
  END_MODUL("red_mre_c");
  return(0);
} /**** end of function red_mre_c ****/

int red_mre_r(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_mre_r");
  END_MODUL("red_mre_r");
  return(0);
} /**** end of function red_mre_r ****/

int red_mrotate(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_mrotate");
  END_MODUL("red_mrotate");
  return(0);
} /**** end of function red_mrotate ****/

int red_munite(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_munite");
  END_MODUL("red_munite");
  return(0);
} /**** end of function red_munite ****/

int red_transform(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_transform");
  END_MODUL("red_transform");
  return(0);
} /**** end of function red_transform ****/

int red_transpose(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_transpose");
  END_MODUL("red_transpose");
  return(0);
}

int red_cut(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_cut");
  END_MODUL("red_cut");
  return(0);
} /**** end of function red_cut ****/

int red_select(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_select");
  END_MODUL("red_select");
  return(0);
} /**** end of function red_select ****/

int red_replace(arg1,arg2,arg3,arg4)
register STACKELEM arg1,arg2,arg3,arg4;
{
  START_MODUL("red_replace");
  END_MODUL("red_replace");
  return(0);
} /**** end of function red_replace ****/

int red_rotate(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_rotate");
  END_MODUL("red_rotate");
  return(0);
} /**** end of function red_rotate ****/

int red_unite(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  START_MODUL("red_unite");
  END_MODUL("red_unite");
  return(0);
} /**** end of function red_unite ****/

/*************************  end of file rmstruct.c  **************************/
