/* neue scrollfunktionen fuer den editor       newscr.c */
/* letzte aenderung:Mo  12.01.87        11:35:16        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include "cestack.h"
#include "cedit.h"
#include "cencod.h"

static int dsp;

/* ----------- srght ----------------------------------------------- */
/* ein teil-ausdruck nach rechts                                     */
static srght()
 {
  register STACKELEM m;
  register int ret;

  ret = 1;
  if (!dsp) {
    if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
      if (!isdispld(m)) dsp = TRUE;
      move(MST,AST);
      pop(M1ST);
      ret = 0;
     }                                             else
    if ((m = topof(EST)) != STBOTTOM) {
      decrarity(M1ST);
      if (isdispld(m))  move(EADRST,AADRST);
      else              dsp = TRUE;
      if (isabbrev(m))  dsp = TRUE;
      if (isconstr(m)) {
        move2(EST,MST,M1ST);
       }
      else {
        transport(EST,AST);
      }
     }
    else
      ret = FAILURE;
   }
  else {
    if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
      if (!isdispld(m)) dsp = TRUE;
      move(MST,AST);
      pop(M1ST);
      ret = 0;
     }                                             else
    if ((m = topof(EST)) != STBOTTOM) {
      decrarity(M1ST);
      if (isconstr(m)) {
        move2(EST,MST,M1ST);
       }
      else {
        transport(EST,AST);
      }
     }
    else
      ret = FAILURE;
  }
  return(ret);
 }

/* ----------- sright ---------------------------------------------- */
/* ein teil-ausdruck nach rechts                                     */
static sright()
 {
  register STACKELEM m;
  register int ret;

  ret = TRUE;
  if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
    if (!isdispld(m)) dsp = TRUE;
    move(MST,AST);
    pop(M1ST);
   }                                             else
  if ((m = topof(EST)) != STBOTTOM) {
    decrarity(M1ST);
    if (isdispld(m)) {
      move(EADRST,AADRST);
      if (isabbrev(m)) {
        transport(EST,AST);
       }
      else {
        if (isconstr(m)) {
          move2(EST,MST,M1ST);
         }
        else {
          transport(EST,AST);
        }
      }
     }
    else {
      transport(EST,AST);
    }
   }
  else
    ret = FALSE;
  return(ret);
 }
/* ----------- sleft ----------------------------------------------- */
/* ein teil-ausdruck nach links                                      */
static sleft()
 {
  register STACKELEM m;
  register int ret;

  ret = TRUE;
  if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == arity(topof(MST))) {
    if (!isdispld(m))  dsp = TRUE;
    else               move(AADRST,EADRST);
    move(MST,EST);
    pop(M1ST);
    incrarity(M1ST);
   }                                             else
  if ((m = topof(AST)) != STBOTTOM) {
    if (isdispld(m)) {
      if (isabbrev(m)) {
        incrarity(M1ST);
        transport(AST,EST);
        move(AADRST,EADRST);
       }
      else {
        if (isconstr(m)) {
          move(AST,MST);
          push(M1ST,novalue(m));
         }
        else {
          incrarity(M1ST);
          move(AADRST,EADRST);
          transport(AST,EST);
        }
      }
     }
    else {
      incrarity(M1ST);
      move(AADRST,EADRST);
      transport(AST,EST);
    }
   }
  else
    ret = FALSE;
  return(ret);
 }
/* ----------- scrollgoal ------------------------------------------ */
scrollgoal(disp)
 int disp;
 {
  register STACKELEM m;

  dsp = FALSE;
  if (notdisp(m = topof(M1ST))  &&  arity(m) == arity(topof(MST))) {
    if (isdispld(m))  move(AADRST,EADRST);
    move(MST,EST);
    pop(M1ST);
    incrarity(M1ST);
  }
  if (class(m = bottom(M1ST)) == AR0(DECL)) {
    if (arity(m)  == 0) {
      /* gehe nach links, bis goal erreicht      */
      while (second(MST) != STBOTTOM  ||  class(m = topof(M1ST)) != AR0(DECL)
             ||  arity(m) != 1) {
        if (!sleft()) error("sleft: fehler\n\r");
      }
      if (!isdispld(m)) dsp = TRUE;
     }
    else {
      /* gehe nach rechts, bis goal erreicht      */
      while (second(MST) != STBOTTOM  ||  class(m = topof(M1ST)) != AR0(DECL)
             ||  arity(m) != 1) {
        if (!sright()) error("sright: fehler\n\r");
      }
      if (!isdispld(m)) dsp = TRUE;
    }
   }                                    else
  if (class(topof(EST)) == AR0(DECL)) {
    /* gehe nach rechts, bis goal erreicht      */
    while (/* second(MST) != STBOTTOM  || */  class(m = topof(M1ST)) != AR0(DECL)
           ||  arity(m) != 1) {
      if (!sright()) error("sright: fehler\n\r");
    }
    if (!isdispld(m)) dsp = TRUE;
  }
  if (disp) {
    if (dsp  ||  !isdispld(topof(EST))) {
      fdisplay(TRUE);
     }
    else {
      while (notdisp((m = topof(EST)))  &&  !isabbrev(m)) {
        scrlright();
      }
      putcursor();
    }
   }
  else {
    while (notdisp((m = topof(EST)))  &&  !isabbrev(m)) {
      scrlright();
    }
    putcursor();
  }
 }
/* ----------- scright -------------------------------------------- */
/* scrolle n-mal rechts                                             */
scright(n,disp)
 int n,disp;
 {
  register STACKELEM m;
  register int s;

  dsp = FALSE;
  if (notdisp(m = topof(M1ST))  &&  arity(m) == arity(topof(MST))) {
    if (isdispld(m))  move(AADRST,EADRST);
    move(MST,EST);
    pop(M1ST);
    incrarity(M1ST);
  }
  while (n > 0) {
    if ((s = srght()) == FAILURE) error("srght: fehler\n\r");
    n -= s;
  }
  while ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
    if (!isdispld(m)) dsp = TRUE;
    move(MST,AST);
    pop(M1ST);
  }
  if (disp) {
    if (dsp  ||  !isdispld(topof(EST)) ||  !isdispld(topof(MST))) {
      display(TRUE);      /* oder fdisplay ?         */
     }
    else {
      while (notdisp((m = topof(EST)))  &&  !isabbrev(m)) {
        scrlright();
      }
      putcursor();
    }
   }
  else {
    while (notdisp((m = topof(EST)))  &&  !isabbrev(m)) {
      scrlright();
    }
    putcursor();
  }
 }
#if !LETREC
/* ab hier fuer batch */

/* ----------- scrlrght -------------------------------------------- */
/* neu fuer batch                                                    */
static scrlrght()
 {
  register STACKELEM m;

  m = topof(EST);
  decrarity(M1ST);
  if (isatom(m)) {
    transport(EST,AST);
    while ((m = topof(M1ST)) != STBOTTOM && arity(m) == ARITY0) {
      move(MST,AST);
      pop(M1ST);
    }
   }
  else
    move2(EST,MST,M1ST);
 }

/* ----------- b_scrlgoal ------------------------------------------ */
b_scrlgoal()
 {
  register STACKELEM m;

  if (class(topof(EST)) == AR0(DECL)) {
    /* gehe nach rechts, bis goal erreicht      */
    while (second(MST) != STBOTTOM  ||  class(m = topof(M1ST)) != AR0(DECL)
           ||  arity(m) != 1) {
      if (!b_sright()) error("b_sright: fehler\n\r");
    }
  }
  while (notdisp(topof(EST))) {
    scrlrght();
  }
 }
/* ----------- b_sright -------------------------------------------- */
/* ein teil-ausdruck nach rechts                                     */
static b_sright()
 {
  register STACKELEM m;
  register int ret;

  ret = TRUE;
  if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
    move(MST,AST);
    pop(M1ST);
   }                                             else
  if ((m = topof(EST)) != STBOTTOM) {
    decrarity(M1ST);
    if (isconstr(m)) {
      move2(EST,MST,M1ST);
     }
    else {
      transport(EST,AST);
    }
   }
  else
    ret = FALSE;
  return(ret);
 }
/* ----------- b_sleft ----------------------------------------------- */
/* ein teil-ausdruck nach links                                      */
static b_sleft()
 {
  register STACKELEM m;
  register int n,ret;

  ret = TRUE;
  if ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == arity(topof(MST))) {
    move(MST,EST);
    pop(M1ST);
    incrarity(M1ST);
   }                                             else
  if ((m = topof(AST)) != STBOTTOM) {
    if (isconstr(m)) {
      move(AST,MST);
      push(M1ST,novalue(m));
     }
    else {
      incrarity(M1ST);
      transport(AST,EST);
    }
   }
  else
    ret = FALSE;
  return(ret);
 }
/* ------------- scrhome --------------------------------------------- */
/* scrolle alles auf stack E                                           */
scrhome()
 {
  register STACKELEM m;
  register int i;

  while ((m = pop(MST)) != STBOTTOM) {
    for ( i = arity(m) - arity(pop(M1ST)) ; i > 1 ; i-- )
      transport(AST,EST);
    push(EST,m);
  }
 }
#endif /* !LETREC	*/
/* end of       newscr.c */
