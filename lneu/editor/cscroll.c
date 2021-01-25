/* fuer editor                                  scroll.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    31.05.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"

/* ---------- setcursor ------------------------------------- */
setcursor()                   /* setze den cursor             */
 {
  CURSORXY(srow,scol);
  putceu();
  CURSOR_LEFT();
  fflush(stdout);
 }
/* ---------- putcursor ------------------------------------- */
putcursor()                   /* positioniere den cursor      */
 {
  register int n;

  n = (int)arity(topof(EADRST));        /* adresse              */
  CURSORXY(srow = n / co, scol = n % co);
  fflush(stdout);
 }
/* ---------- scrlup ---------------------------------------- */
/* scrolle um ein vaterausdruck nach links                    */
/* used by: fdisplay, scrollup                                */
scrlup()
 {
  static STACKELEM m;
  register int n,a;

  a = 0;
  if ((m = topof(M1ST)) != STBOTTOM) {  /* ex. vaterausdruck ?  */
    /* fuer alle teilausdruecke auf a   */
    for (n = (int)(arity(topof(MST)) - arity(m)) ; n > 0 ; --n) {
      a++;
      if (isdispld(m = topof(AST))) {   /* angezeigt ?  */
        if (isatom(m) || isabbrev(m)) { /* atom oder abgek. ?   */
          transport(AST,EST);           /* uebertrage ausdruck  */
          move(AADRST,EADRST);          /* und adr.             */
         }
        else {                          /* constr,displd,~abbrev */
          push(M1ST,novalue(move(AST,MST))); /* loesche arity fuer m1 */
          a += scrlup();                     /* rekursiver aufruf    */
        }
       }
      else                              /* ~displd      */
        transport(AST,EST);
    }                                   /* end for      */
    a++;
    move(MST,EST);                      /* konstr. von m nach e */
    pop(M1ST);                          /*  und von m1 loeschen */
    move(AADRST,EADRST);                /* adr. uebertragen     */
    incrarity(M1ST);            /* ein ausdruck mehr auf e      */
  }
  return(a);
 }
/* ---------- scrollup -------------------------------------- */
/* scrolle einen vaterausdruck nach links                     */
/* mit 'ueberscrollen' von ndisp und ev. neuanzeige (display) */
/* used by: main        , use: scrlup,display   */
scrollup(disp)
 int disp;
 {
  register int a;

  a = scrlup();                             /* einmal hoch  */
  if (isdispld(topof(EST))) {               /* angezeigt ?  */
    if (a == 1) {
      if (notdisp(topof(EST)) && !isabbrev(topof(EST))) {
        scrollup(disp);
        disp = FALSE;
      }
     }
    else {
      while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
        scrlright();               /* notdisp ueberscrollen        */
    }
    if (disp) putcursor();
   }
  else if (disp) display(TRUE);               /* nicht angezeigt      */
 }
/* ---------- scrldown -------------------------------------- */
/* used by: scrolldown  , use: scrlright                      */
static scrldown()
 {
  register STACKELEM m;

  scrlright();                          /* ein nach rechts      */
  if ((m = topof(MST)) != STBOTTOM) {   /* in unterausdruck ?   */
    if (isdispld(m)) {                  /* angezeigt ?          */
      while (isatom(m=topof(EST)) || isabbrev(m)) {     /* geaendert    */
          scrlright();  /* solange atom oder abgekuerzt nach rechts     */
      }
    }
  }
 }
/* ---------- scrolldown ------------------------------------ */
/* used by: main        , use: scrldown,scrlright             */
scrolldown(disp)
 int disp;
 {

  scrldown();                           /* ein nach 'unten'     */
  while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
    scrlright();                        /* neu fuer ndisp       */
  if (disp) putcursor();
 }
/* ---------- scrollhome ------------------------------------ */
/* used by: main        , use: display, scrolltop             */
scrollhome(disp)
 int disp;
 {
  register STACKELEM m;
  register int n,erg;

  /* ist links etwas angezeigt ?       */
  if ((m = topof(AADRST)) == STBOTTOM  ||
      class(m) == UNPR  &&  second(AADRST) == STBOTTOM) {
    erg = !isempty(MST);
    while ((m = topof(MST)) != STBOTTOM) {      /* bis nach oben        */
      for (n = (int)(arity(m) - arity(topof(M1ST))) ; n > 0 ; --n)
        transport(AST,EST);
      move(MST,EST);
      pop(M1ST);
      incrarity(M1ST);
    }
    if (disp  &&  erg) display(TRUE);             /* anzeigen des ganzen  */
   }
  else scrolltop(disp);
 }
/* ---------- scrltop --------------------------------------- */
/* used by: scrolltop,scrlright,topb                          */
static scrltop()
 {
  register STACKELEM m;
  register int n;

  /* solange angezeigter vaterausdruck existiert        */
  while ((m = topof(MST)) != STBOTTOM && isdispld(m)) { /* geaendert    */
      for (n = (int)(arity(m) - arity(topof(M1ST))) ; n > 0 ; --n)
        transport(AST,EST);
      move(MST,EST);
      pop(M1ST);
      incrarity(M1ST);
  }
  while (move(AADRST,EADRST) != STBOTTOM)       /* uebertrage adressen  */
  ;
 }
/* ---------- scrolltop ------------------------------------- */
/* used by: scrollhome,scrollback                             */
scrolltop(disp)
 int disp;
 {

  scrltop();
  while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
    scrlright();                                /* neu fuer ndisp       */
  if (disp) putcursor();
 }
/* ---------- scrlright ------------------------------------- */
/* used by: input,dispfa,scrolldown,scrollright,scrlleft      */
/* use: scrltop                                               */
scrlright()
 {
  register STACKELEM m;
  register int rechts;

  if (rechts = (second(EADRST) != STBOTTOM)) {
    move(EADRST,AADRST);
    m = topof(EST);
    do {
      decrarity(M1ST);
      if (!isdispld(m) || isatom(m) || isabbrev(m)) {
        transport(EST,AST);
        while ((m = topof(M1ST)) != STBOTTOM && isdispld(m)
                && arity(m) == ARITY0) {
          move(MST,AST);
          pop(M1ST);
        }
       }
      else /* constr,displd,~abbrev */
        move2(EST,MST,M1ST);
    } while ((m=topof(EST)) != STBOTTOM && !isdispld(m));
   }
  else scrltop();
  return(rechts);
 }
/* ---------- scrollright ----------------------------------- */
/* used by: scrollhome,scrolltop        , use: scrlright      */
scrollright(disp)
 int disp;
 {
  scrlright();
  while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
    scrlright();                        /* neu fuer ndisp       */
  if (disp) putcursor();
 }
/* ---------- scrlleft -------------------------------------- */
/* used by: scrolleft,scrollup                                */
static scrlleft()
 {
  register STACKELEM m;

  if (topof(AADRST) != STBOTTOM) {      /* ex. linker (angez.) ausdruck */
    move(AADRST,EADRST);                /* ja: adr. von a nach e        */
    if (arity(topof(MST)) == arity(topof(M1ST))) {      /* nur konstr.? */
      move(MST,EST);                    /* ja: konstr. nach e           */
      pop(M1ST);
      incrarity(M1ST);
     }
    else {      /* vorsicht bei 0-stell. konstr. (?)    */
      while ((m = topof(AST)) != STBOTTOM) {    /* auf a etwas ?        */
        if (isdispld(m)) {                      /* angezeigt ?  */
          if (isatom(m) || isabbrev(m)) {       /* atom oder abgekuerzt ? */
            transport(AST,EST);                 /* ja: a->e     */
            incrarity(M1ST);
            break;                              /* achtung !!!! */
           }
          else {                                /* displd,constr,~abbrev */
            move(AST,MST);
            push(M1ST,novalue(m));
          }
         }
        else {                                  /* ~displd */
          transport(AST,EST);
          incrarity(M1ST);
          /* neu 16.05.89	*/
          if (arity(topof(MST)) == arity(topof(M1ST))) {
            /* letzten unterausdruck nach E geschoben !	*/
            move(MST,EST);
            pop(M1ST);
            incrarity(M1ST);
            if (isdispld(topof(EST))) break;
          }
        }
      } /* end while */
    }
   }
  else {                 /* scroll to the last screen-expression */
    while (second(EADRST) != STBOTTOM)
      scrlright();
  }
 }
/* ---------- scrolleft ------------------------------------- */
/* used by: main,scrolldown     ,use: scrlleft                */
scrolleft(disp)
 int disp;
 {

  scrlleft();
  while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
    scrlleft();                         /* neu fuer ndisp       */
  if (disp) putcursor();
 }
/* ---------- scrlrb ---------------------------------------- */
static scrlrb()
 {
  static STACKELEM m;
  register int n;

  if ((m = topof(EST)) != STBOTTOM) {
    decrarity(M1ST);
    if (isdispld(m)) {
      move(EADRST,AADRST);
      if (isatom(m)  || isabbrev(m))
        transport(EST,AST);
      else {
        move2(EST,MST,M1ST);
        for ( n = arity(m) ; n > 0 ; --n )
          scrlrb();
        move(MST,AST);
        pop(M1ST);
      }
     }
    else
      transport(EST,AST);
  }
 }
/* ---------- rbscroll -------------------------------------- */
rbscroll(disp)
 int disp;
 {
  register STACKELEM m;
  register int ok;

  if ((m = topof(M1ST)) != STBOTTOM) {
    ok = FALSE;
    if (notdisp(m)  && arity(topof(MST)) == arity(m)) {
      if ((m = second(M1ST)) != STBOTTOM  && isdispld(m)) {
        move(AADRST,EADRST);
        move(MST,EST); pop(M1ST);
        incrarity(M1ST);
        m = topof(M1ST);
        ok = TRUE;
      }
     }
    else
      ok = isdispld(m);
    if (ok) {
      if (arity(m) == 1) {
        scrollup(FALSE);
        scrlright();
       }
      else {
        do {
          scrlrb();
          if (arity(topof(M1ST)) == 0) {
            scrlup();
            scrlright();
          }
        } while (!isdispld(topof(EST)));
      }
      while (notdisp(topof(EST))  &&  !isabbrev(topof(EST)))
        scrlright();
      if (disp) putcursor();
    }
  }
 }
/* ---------- savepath -------------------------------------- */
/* used by: topb                                              */
static savepath()
 {
  register STACKELEM m;
  STACK X;

  asgnst(XST,M1ST);             /* copy m1 nach x       */
  clearstack(M2ST);             /* init m2 fuer topb    */
  while ((m = popnf(XST)) != STBOTTOM && isdispld(m))
    push(M2ST,m);       /* solange angezeigt: uebertragen       */
 }
/* ---------- topb ------------------------------------------ */
/* used by: dispfa      ,use: savepath, scrltop               */
topb()
 {
  savepath();
  scrltop();
 }
/* used by: scrlback,scrollback */
static scrlback(arit)
 int arit;
 {
  static STACKELEM m;
  register int n;
  extern int placeh;

  if (arit < 0) n = (int)arity(topof(AST));
  else          n = arit;
  push(M1ST,novalue(move(AST,MST)));
  for ( ; n > 0 ; --n) {
    if (isdispld(m = topof(AST))) {
      if (isabbrev(m) || isatom(m)) {
        if (noedit(m) == PLACEH)
          placeh++;
        transport(AST,EST);
        move(AADRST,EADRST);
        incrarity(M1ST);
       }
      else                      /* constr, displd,~abbrev       */
        scrlback(-1);
     }
    else {                      /* ~displd      */
      transport(AST,EST);
      incrarity(M1ST);
    }
  } /* end for */
  if (arit < 0) {
    move(MST,EST);
    pop(M1ST);
    move(AADRST,EADRST);
    incrarity(M1ST);
  }
 }
/* -------- scrollback -------------------------------------------- */
/* used by: display,fdisplay    */
scrollback()
 {
  extern int placeh;

  placeh = 0;
  if (isatom(topof(AST))) {
    transport(AST,EST);
    move(AADRST,EADRST);
    incrarity(M1ST);
    clearstack(M2ST);
   }
  else {                         /* constr */
    if (topof(M2ST) != STBOTTOM) {
      do {
        if (!isdispld(topof(AST))) {
          push(M1ST,novalue(move(AST,MST)));
          clearstack(M2ST);
          scrolltop(TRUE);
          return;
        }
        scrlback((int)arity(pop(M2ST)));
      } while (topof(M2ST) != STBOTTOM);
      if (!isdispld(topof(EST))) {
        scrolltop(TRUE);
        return;
      }
     }
    else scrlback(-1);
  }
  while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
    scrlright();        /* neu  */
  putcursor();
 }
#if X_WINDOWS
/*
static int get_col(m)
 STACKELEM m;
 {
  return(arity(m) % co);
 }
static int get_row(m)
 STACKELEM m;
 {
  return(arity(m) / co);
 }
*/
/* versuche cursor auf position x,y zu stellen	*/
position(x,y,disp)
 int x,y,disp;
 {
  int pos;

  pos = y * co + x;
  if (pos >= (int)arity(topof(EADRST))) {
    while (second(EADRST) != STBOTTOM && pos > (int)arity(topof(EADRST)) ) {
      scrollright(FALSE);
    }
   }
  else {
    while (!isempty(AADRST) && pos < (int)arity(topof(AADRST))) {
      scrolleft(FALSE);
    }
  }
  if (disp) putcursor();
 }
#endif
/* end of       scroll.c */

