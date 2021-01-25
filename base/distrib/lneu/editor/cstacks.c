/*                                          cstacks.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:  31.05.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"
#include "ckwtb.h"
#include "ctred.h"

extern char msg[];

#define ASSERT(c)	if (!(c)) { error("c"); } else

/* ----------------- asgnst ------------------------------------------ */
/* Assignment von Stack-Strukturen.                                    */
asgnst(st1,st2)
register STACK *st1;
register STACK *st2;
{
 st1->stptr = st2->stptr;
 st1->cnt   = st2->cnt;
 st1->siz   = st2->siz;
 st1->pred  = st2->pred;
ASSERT((st1->cnt >= 0L) && (st1->cnt <= st1->siz));

}
/* --------------- crstack ---------------------------------------------------- */
/* erzeugt einen Stack mit einer Speicherseite der Laenge n.			*/
/* pred erhaelt den Wert 0 und stptr zeigt auf die				*/
/* neue Speicherseite, siz wird zu n-2 und cnt zu 0.				*/
crstack(st,n)
register STACK *st;
register unsigned n;
{
 char *calloc();
 register STACKELEM *p;

 if ((p = (STACKELEM *)calloc((unsigned)n,(unsigned)sizeof(STACKELEM)))
      != SNULL ) {
    st->cnt   = 0L;
    st->siz   = (long)(n - 2);
    st->pred  = SNULL;
    st->stptr = p;
    return TRUE;
  }
 else {
    message(MROW,MCOL,"Severe : No more core available ! (crstack)\n");
    st->cnt   = 0L;
    st->siz   = 0L;
    st->pred  = SNULL;
    st->stptr = SNULL;
    return FALSE;
  }
}
/* --------------- mkstack ---------------------------------------------- */
/*           Vergroessert einen Stack um eine Speicherseite der Laenge n. */
/*           pred erhaelt den Wert von stptr und stptr zeigt auf die      */
/*           neue Speicherseite, siz wird zu n-2 und cnt zu 0.            */
mkstack(st,n)
register STACK *st;
register unsigned n;
{
 char *calloc();
 register STACKELEM *p;

 if ((p = (STACKELEM *)calloc((unsigned)n,(unsigned)sizeof(STACKELEM)))
      != SNULL ) {
    st->cnt   = 0L;
    st->siz   = (long)(n - 2);
    st->pred  = st->stptr;
    st->stptr = p;
    return TRUE;
  }
 else {
    message(MROW,MCOL,"Severe : No more core available ! (mkstack)\n");
    st->cnt   = 0L;
    st->siz   = 0L;
    st->pred  = SNULL;
    st->stptr = SNULL;
    return FALSE;
  }
}
/* ------------ clearstack ----------------------------------------------- */
/* Loescht einen Stack und gibt bis auf die Erste alle Speicherseiten frei */
void clearstack(st)
 register STACK *st;
 {
  while ((st->pred) != SNULL) {
    st->stptr -= st->cnt;
    cfree((char *)(st->stptr));
    st->stptr = st->pred;
    st->pred  = (STACKELEM *)(*(st->stptr));
    st->cnt   = *(--st->stptr);
    st->siz   = st->cnt;
  }
  st->stptr -= st->cnt;
  st->cnt    = 0L;
 }
/* ------------ killstack ----------------------------------------------- */
/* Loest einen Stack auf; gibt alle Speicherseiten frei                   */
void killstack(st)
STACK *st;
{

 clearstack(st);
 cfree((char *)(st->stptr));
 st->stptr = SNULL;
 st->siz   = 0L;
 st->cnt   = 0L;
 st->pred  = SNULL;
}
/* ------------ push ---------------------------------------------------- */
/*        Legt ein Stackelement auf einem Stack st ab und alloziert,      */
/*        falls noetig eine zusaetzliche Speicherseite.                   */
void push(st,m)
register STACK   *st;
register STACKELEM m;
{
 if (st->cnt >= st->siz) {
    *(st->stptr)++ = (STACKELEM)st->cnt;
    *(st->stptr) = (STACKELEM)st->pred;
    st->pred = st->stptr;
    if (!mkstack(st,STKPAGE)) {
       error("push: mkstck: FAILURE");
    }
 }
 (st->cnt)++;
 *(st->stptr)++ = m;
}
/* ------------ pushi --------------------------------------------------- */
/*        Legt ein Stackelement auf einem Stack st ab und alloziert,      */
/*        falls noetig eine zusaetzliche Speicherseite.                   */
void pushi(m,st)
register STACKELEM m;
register STACK   *st;
{
 if (st->cnt >= st->siz) {
    *(st->stptr)++ = (STACKELEM)st->cnt;
    *(st->stptr) = (STACKELEM)st->pred;
    st->pred = st->stptr;
    if (!mkstack(st,STKPAGE)) {
       error("pushi: mkstck: FAILURE");
    }
 }
 (st->cnt)++;
 *(st->stptr)++ = m;
}
/* ------------ pop ----------------------------------------------------- */
/*       Holt das oberste Element von einem Stack st und gibt gegebenen-  */
/*       falls eine freigewordene Speicherseite frei.                     */
STACKELEM pop(st)
register STACK *st;
{

 if (--(st->cnt) < 0L) {
    if (st->pred == SNULL) {
       st->cnt = 0L;
       return(STBOTTOM);
     }
    else {
       cfree((char *)(st->stptr));
       st->stptr = st->pred;
       st->pred = (STACKELEM *)*(st->stptr);
       st->siz = *(--(st->stptr));
       st->cnt = st->siz - 1L;
    }
 }
 return(*(--(st->stptr)));
}
/* ----------- ppop ----------------------------------------------------- */
/* entfernt das oberste Element von einem Stack st und gibt gegebenenfalls*/
/* eine freigewordene Speicherseite frei.                                 */
void ppop(st)
register STACK *st;
{

 if (--(st->cnt) < 0L) {
    if (st->pred == SNULL) {       /* stack ist leer !        */
       st->cnt = 0L;
       return;
     }
    else {
       cfree((char *)(st->stptr));
       st->stptr = st->pred;
       st->pred = (STACKELEM *)*(st->stptr);
       st->siz = *(--(st->stptr));
       st->cnt = st->siz - 1L;
    }
 }
 --(st->stptr);
}
/* ------------ isempty ------------------------------------------------- */
/* ist stack leer ?                                                       */
isempty(st)
register STACK *st;
{
 if (st->cnt > 0L)
    return(FALSE);
 else
    return (st->pred == SNULL) ;
}
/* ------------ topof --------------------------------------------------- */
/* Liefert das oberste Element eines Stacks ohne den Stack zu veraendern  */
STACKELEM topof(st)
register STACK *st;
{
 if (!((st->cnt >= 0L) && (st->cnt <= st->siz))) {
   sprintf(lbuf,"cnt %ld size %ld",st->cnt,st->siz);
   error(lbuf);
 }
 if (st->cnt > 0L)
    return(*((st->stptr) - 1));
 else {
    if (st->pred == SNULL)
       return(STBOTTOM);
    else
       return(*((st->pred) - 2L));
 }
}
/* ------------ second -------------------------------------------------- */
/*          Liefert das zweitoberste Element eines Stacks st falls        */
/*          existent sonst STBOTTOM.                                      */
STACKELEM second(st)
register STACK *st;
{
  register long int count;

 if ((count = st->cnt) > 1L)
    return(*(st->stptr - 2));
 else {
    if (st->pred == SNULL)
       return(STBOTTOM);
    else
       return(*(st->pred - (3 - count)));
 }
}
/* ------------ bottom -------------------------------------------------- */
/* bottom: liefert unterstes element eines stacks                         */
STACKELEM bottom(st)
 STACK *st;
 {
  register STACKELEM *s,*s1;
  register long count;

  s = st->pred;
  if (s == SNULL) {                    /* erste seite ?    */
    if (st->cnt == 0l)                 /* kein element ?   */
      return(STBOTTOM);
    else {                             /* unterstes element          */
      count = st->cnt;
      return((STACKELEM)(*(st->stptr  - count)));
    }
   }
  else {                               /* solange pred ex.           */
    while ((s1 = (STACKELEM *)(*s)) != SNULL) {
      s = s1;
    }
    count = (long)(*(--s));            /* achtung : --s !            */
    return((STACKELEM)(*(s  - count)));
  }
 }
/* ------------ move ---------------------------------------------------- */
/*        Bewegt ein Stackelement von einem Stack st1 auf einen Stack st2 */
STACKELEM move(st1,st2)
STACK *st1,*st2;
{
 register STACKELEM m;

 if ((m = pop(st1)) != STBOTTOM)
    push(st2,m);
 return(m);
}
/* ------------ move2 --------------------------------------------------- */
/*         Bewegt ein Stackelement von einem Stack st1 auf zwei Stacks    */
/*         st2 und st3.                                                   */
STACKELEM move2(st1,st2,st3)
STACK *st1,*st2,*st3;
{
 register STACKELEM m;

 if ((m = pop(st1)) != STBOTTOM)
  { push(st2,m);
    push(st3,m);
  }
 return(m);
}
/* ------------- rtransp ---------------------------------------------- */
/* uebertrage stack e nach b im reduma-format (d.h QUOTE !)            */
rtransp(e,b)
 STACK *e,*b;
 {
  register STACKELEM el;
  register int w,n;

  if (topof(e) != STBOTTOM) {
    push(MST,ENDSYMB);
    do {
      w = YES;
      while(w) {
        if (isconstr(el = pop(e))) {
          push(MST,el-ARITY1);
          push(M1ST,el);
         }
        else { /* nicht konstr       */
          w = NO;
          if (el == AQUOTE) {
            n = 1;
            push(b,el);
            do {
              if ((el = pop(e)) == AQUOTE) n++;      else
              if (el == EQUOTE) n--;
              push(b,el);
            } while (n);
           }                 else
          if (el == EQUOTE) {
            n = 1;
            push(b,el);
            do {
              if ((el = pop(e)) == EQUOTE) n++;      else
              if (el == AQUOTE) n--;
              push(b,el);
            } while (n);
           }                 else
          if (single_atom(el) || marked0(el))
            push(b,el);
          else {
            push(b,mark0(el));
            while(marked1(el = pop(e)))
              push(b,el);
            push(b,mark1(el));
          }
        }
      }
      w = 0;
      while (w == 0) {
        el = pop(MST);
        if (el == ENDSYMB) {
          w = 2;
         }                                           else
        if (isconstr(el)) {
          if (arity(el) > 0) {
            push(MST,el-ARITY1);
            w = 1;
           }
          else move(M1ST,b);
        }                                            else
        {
          sprintf(msg,"Illegal element %08lx in transp",el);
          error(msg);
        }
      }
    }  while (w == 1);
  }
 }
#ifdef UnBenutzt
/* ------------- transp ---------------------------------------------- */
/* uebertrage stack e nach b, loesche edit-infos                       */
transp(e,b)
 STACK *e,*b;
 {
  register STACKELEM el;
  register int w;

  if (topof(e) != STBOTTOM) {
    push(MST,ENDSYMB);
    do {
      w = YES;
      while(w) {
        if (isconstr(el = noedit(pop(e)))) {
          push(MST,el-ARITY1);
          push(M1ST,el);
         }
        else { /* nicht konstr       */
          w = NO;
          if (single_atom(el) || marked0(el))
            push(b,el);
          else {
            push(b,mark0(el));
            while(marked1(el = noedit(pop(e))))
              push(b,el);
            push(b,mark1(el));
          }
        }
      }
      w = 0;
      while (w == 0) {
        el = pop(MST);
        if (el == ENDSYMB) {
          w = 2;
         }                                           else
        if (isconstr(el)) {
          if (arity(el) > 0) {
            push(MST,el-ARITY1);
            w = 1;
           }
          else move(M1ST,b);
        }                                            else
        {
          sprintf(msg,"Illegal element %08lx in transp",el);
          error(msg);
        }
      }
    }  while (w == 1);
  }
 }
#endif /* UnBenutzt	*/
/* ------------- edtransp -------------------------------------------- */
/* uebertrage stack e nach b, ersetze display-info durch ed            */
edtransp(e,b,ed)
 STACK *e,*b;
 register int ed;
 {
  register STACKELEM el;
  register int w;

  if (topof(e) != STBOTTOM) {
    push(MST,ENDSYMB);
    ed = todispl(ed);
    w  = 1;
    /* beim ersten element nichts veraendern !       */
    if (isconstr( el = pop(e) )) {
      push(MST,el-ARITY1);
      push(M1ST,el);
     }
    else { /* nicht konstr       */
      w = 0;
      if (single_atom(el) || marked0(el))
        push(b,el);
      else {
        push(b,mark0(el));
        while(marked1(el = pop(e)))
          push(b,el);
        push(b,mark1(el));
      }
    }
    do {
      while(w) {
        if (isconstr( el = (nodispl(pop(e)) | ed) )) {
          push(MST,el-ARITY1);
          push(M1ST,el);
         }
        else { /* nicht konstr       */
          w = NO;
          if (single_atom(el) || marked0(el))
            push(b,el);
          else {
            push(b,mark0(el));
            while(marked1(el = nodispl(pop(e))))
              push(b,el | ed);
            push(b,mark1(el | ed));
          }
        }
      }
      while (w == 0) {
        el = pop(MST);
        if (el == ENDSYMB) {
          w = 2;
         }                                           else
        if (isconstr(el)) {
          if (arity(el) > 0) {
            push(MST,el-ARITY1);
            w = 1;
           }
          else move(M1ST,b);
        }                                            else
        {
          sprintf(msg,"Illegal element %08lx in transp",el);
          error(msg);
        }
      }
    }  while (w == 1);
  }
 }
/* ------------ transport ---------------------------------------------*/
/* Transportiert einen Ausdruck von e nach b     (rekursiv)            */
transport(e,b)
 STACK *e,*b;
{
  register STACKELEM el;
  register int n;

  if ((el = pop(e)) != STBOTTOM) {
    if (isconstr(el)) {
      push(MST,el);
      for ( n = arity(el) ; n > 0 ; n-- ) {
         transport(e,b);
      }
      move(MST,b);
    }
   else { /* nicht konstr       */
     if (single_atom(el) || marked0(el))
       push(b,el);
     else {
       push(b,mark0(el));
       while(marked1(el = pop(e)))
         push(b,el);
       push(b,mark1(el));
     }
   }
  }
 }
/* ------------ popnf --------------------------------------------------- */
/*         Holt das oberste Element von einem Stack st ohne jedoch ein    */
/*         gegebenenfalls freigewordenes Speichersgment freizugeben.      */
STACKELEM popnf(st)
register STACK *st;
{
 if (--(st->cnt) < 0L)
  { if (st->pred == SNULL)
     { st->cnt = 0L;
       return(STBOTTOM);
     }
    else
     { st->stptr = st->pred;
       st->pred = (STACKELEM *)(*(st->stptr));
       st->siz = *(--(st->stptr));
       st->cnt = st->siz - 1L;
     }
  }
 return(*(--(st->stptr)));
}
/* ------------ movenf -------------------------------------------------- */
/*          Bewegt ein Stackelement von einem Stack st1 auf einen Stack   */
/*          st2 , gibt leere Speichersegmente nicht frei.                 */
STACKELEM movenf(st1,st2)
STACK *st1,*st2;
{
 register STACKELEM m;

 if ((m = popnf(st1)) != STBOTTOM)
    push(st2,m);
 return(m);
}
/* ------------ transpnf ------------------------------------------------ */
/*            Transportiert einen Ausdruck von einem Stack st1 auf einen  */
/*            Stack st2,gibt leere Speicherseiten nicht frei.             */
transpnf(st1,st2)
STACK *st1,*st2;
{
 static STACKELEM m;
 register long n;

 if ((m = topof(st1)) != STBOTTOM)
  { if (isconstr(m))
     { movenf(st1,MST);
       for (n = (long)arity(m) ; n > 0L ; --n )
           transpnf(st1,st2);
       move(MST,st2);
     }
    else  /* atom */
     { if (single_atom(m) || marked0(m))
          movenf(st1,st2);
       else  /* string marked1 */
        { push(st2,(popnf(st1) & ~MARK_FLAG));
          while (marked1(movenf(st1,st2)));
          push(st2,(pop(st2) | MARK_FLAG));
        }
     }
  }
}
/* ------------ transp2 ------------------------------------------------- */
/*           Transportiert einen Ausdruck von einem Stack st1 auf zwei    */
/*           Stacks st2 und st3.                                          */
transp2(st1,st2,st3)
STACK *st1,*st2,*st3;
{
 register STACKELEM m;
 register long n;

 if ((m = topof(st1)) != STBOTTOM)
  { if (isconstr(m))
     { move(st1,MST);
       for (n = arity(m) ; n > 0L ; --n)
           transp2(st1,st2,st3);
       move2(MST,st2,st3);
     }
    else  /* atom */
     { if (single_atom(m) || marked0(m))
          move2(st1,st2,st3);
       else  /* string marked1 */
        { push(st1,pop(st1) & ~MARK_FLAG);
          move2(st1,st2,st3);
          while (marked1(move2(st1,st2,st3))) ;
          ;
          push(st2,pop(st2) | MARK_FLAG);
          push(st3,pop(st3) | MARK_FLAG);
        }
     }
  }
}
/* ------------ delete -------------------------------------------------- */
/*          Loescht den obersten Ausdruck eines Stacks st.                */
delete(st)
STACK *st;
{
 register STACKELEM m;
 register long n;

 if ((m = topof(st)) != STBOTTOM);
  { if (isconstr(m))
     { pop(st);
       for (n = (long)arity(m) ; n > 0L ; --n)
           delete(st);
     }
    else  /* atom */
     { if (single_atom(m))
          pop(st);
       else  /* string */
          while (marked1(pop(st)));
     }
  }
}
/* ------------ deletenf ------------------------------------------------ */
/*            Loescht einen Ausdruck; gibt keinen Speicher frei.          */
deletenf(st)
STACK *st;
{
 register STACKELEM m;
 register long n;

 if ((m = topof(st)) != STBOTTOM);
  { if (isconstr(m))
     { popnf(st);
       for (n = (long)arity(m) ; n > 0L ; --n)
           deletenf(st);
     }
    else  /* atom */
     { if (single_atom(m))
          popnf(st);
       else  /* string */
          while (marked1(popnf(st)));
     }
  }
}
/* ------------ duplicate ----------------------------------------------- */
/*             Dupliziert einen Ausdruck (transponiert) von st1 nach st2. */
duplicate(st1,st2)
STACK *st1,*st2;
{
 STACK X;

 asgnst(&X,st1);
 transpnf(&X,st2);
}
#ifdef UnBenutzt
/* ------------ copy ---------------------------------------------------- */
/*        Kopiert einen Ausdruck (nicht transponiert)                     */
copy(st1,st2)
STACK *st1,*st2;
{
 STACK X;

 X.stptr = SNULL;
 if (!mkstack(&X,STKPAGE)) {
    error("copy: mkstck: FAILURE");
 }
 transpnf(st1,&X);
 transport(&X,st2);
 killstack(&X);
}
#endif /* UnBenutzt	*/
/* ------------ stacksize ----------------------------------------------- */
/*             Liefert die Laenge eines Stacks.                           */
long stacksize(st)
STACK *st;
{
 register long n;
 register STACKELEM *pt;

 n = st->cnt;
 pt = st->pred;
 while (pt != SNULL)
  { n += (long)(*(pt - 1));
    pt = (STACKELEM *)(*(pt));
  }
 return(n);
}
/* ------------ copy1 --------------------------------------------------- */
static copy1(st1,st2)
 STACK *st1,*st2;
 {
  register STACKELEM m;

  while ((m = popnf(st1)) != STBOTTOM) {          /* stksize ?        */
    push(st2,m);
  }
 }
/* ------------ copyst -------------------------------------------------- */
/* kopiere kompletten stack st1 nach st2 ohne st1 zu zerstoeren      */
copyst(st1,st2)
 STACK *st1,*st2;
 {
  STACK X,Y;

  asgnst(&X,st1);
  Y.stptr = SNULL;
  if (!mkstack(&Y,STKPAGE)) error("copyst: mkstack: FAILURE");
  copy1(&X,&Y);
  clearstack(st2);
  copy1(&Y,st2);
  killstack(&Y);
 }
/* ------------ copy2 --------------------------------------------------- */
static copy2(st1,st2)
 STACK *st1,*st2;
 {
  register STACKELEM m;

  while ((m = noedit(popnf(st1))) != STBOTTOM) {          /* stksize ?        */
    push(st2,m);
  }
 }
/* ------------ copysted ------------------------------------------- */
/* kopiere kompletten stack st1 nach st2 ohne st1 zu zerstoeren      */
/* ohne editinformationen                                            */
copysted(st1,st2)
 STACK *st1,*st2;
 {
  STACK X,Y;

  asgnst(&X,st1);
  Y.stptr = SNULL;
  if (!mkstack(&Y,STKPAGE)) error("copyst: mkstack: FAILURE");
  copy1(&X,&Y);
  clearstack(st2);
  copy2(&Y,st2);
  killstack(&Y);
 }
/* ------------ expsiz -------------------------------------------------- */
static long expsiz(st)
STACK *st;
{
  static STACKELEM m;
  long i,n;

  if (isconstr(m = topof(st))) {
     popnf(st);
     n = 1L;
     for (i = (long)arity(m) ; i > 0L ; --i)
         n = n + expsiz(st);
   }
  else {
     if (single_atom(m)) {
        popnf(st);
        n = 1L;
      }
     else {
        n = 0L;
        do {
           m = popnf(st);
           n++;
        } while (marked1(m));
     }
  }
  return(n);
}
/* ------------ exprsize ------------------------------------------------ */
long exprsize(st)
STACK *st;
{
  static STACK X;

  asgnst(&X,st);
  return(expsiz(&X));
}
#ifdef UnBenutzt
/* ------------ nmoves -------------------------------------------------- */
long static nmoves(e)
 STACK *e;
 {
  register long moves,n;

  moves = 0l;
  n     = 1l;
  while ( n > 0l ) {
    --n;
    moves++;
    if (isconstr(topof(e))) {
      n += (long)arity(popnf(e));
      moves++;
     }
    else {
      while (marked1(popnf(e)))
      ;
    }
  }
  return(moves);
 }
/* ------------ exsize -------------------------------------------------- */
long exsize(st1)
 STACK *st1;
 {
  STACK X;

  asgnst(XST,st1);
  return(nmoves(XST));
 }
#endif
/* ------------ incrarity ----------------------------------------------- */
/* Erhoeht die Stelligkeit des obersten Elements eines Stacks             */
void incrarity(st)
STACK *st;
{
 register STACKELEM m;

 if ((m = pop(st)) != STBOTTOM)
    push(st,m + ARITY1);
}
/* ------------ decrarity ----------------------------------------------- */
/* Erniedrigt die Stelligkeit des obersten Elements eines Stacks          */
void decrarity(st)
STACK *st;
{
 register STACKELEM m;

 if ((m = pop(st)) != STBOTTOM)
    push(st,m - ARITY1);
}
#ifdef UnBenutzt
/* ------------ dstrtostk ----------------------------------------------- */
/* transportieren strings auf einen Stack.                                */
void dstrtostk(s,st)
char *s;
STACK *st;
{
 push(st,DSTR0 | tovalue(*s++));
 while (*s != '\0')
   push(st,DSTR1 | tovalue(*s++));
}
#endif /* UnBenutzt	*/
#ifdef UnBenutzt
/* ------------ lstrtost ------------------------------------------------ */
lstrtost(s,st)
char *s;
STACK *st;
{
 push(st,LSTR0 | tovalue(*s++));
 while (*s != '\0')
   push(st,LSTR1 | tovalue(*s++));
}
#endif
/* ------------ travmastr ----------------------------------------------- */
/* Traversiert ein multi_atom von einem Stack st1 auf einen               */
/* Stack st2 und speichert bis zu maxlen value-Werte in dem               */
/* Zeichenstring str.Als Wert wird die Laenge des multi_atoms             */
/* geliefert.Es ist sicherzustellen dass auf st1 ein                      */
/* multi_atom steht und dass str mindestens maxlen+1 Zeichen fasst.       */
travmastr(st1,st2,str,maxlen)
STACK *st1,*st2;
register char *str;
register int maxlen;
{
 register STACKELEM m;
 register int len;
 register unsigned char c;

 m      = move(st1,st2);
 c = (unsigned char)value(m);
 if (c < 32 || c > 126)                                      /* stt  2.1.96 */
 {
   len = 4;
   if (len <= maxlen)
   {
     *str++ = '\\';
     *str++ = '0'+(c / 64);
     *str++ = '0'+((c % 64) / 8);
     *str++ = '0'+(c % 8);
   }
 }
 else
 {
   len = 1;
   *str++ = c;
 }
 if (marked1(m)) {
    push(st2,pop(st2) & ~MARK_FLAG);
    do {
      m = move(st1,st2);
      c = (unsigned char)value(m);
      if (c < 32 || c > 126)                                 /* stt  2.1.96 */
      {
        len += 4;
        if (len <= maxlen)
        {
          *str++ = '\\';
          *str++ = '0'+(c / 64);
          *str++ = '0'+((c % 64) / 8);
          *str++ = '0'+(c % 8);
        }
      }
      else
      {
        len++;
        if (len <= maxlen)
          *str++ = c;
      }
    } while (marked1(m));
    push(st2,pop(st2) | MARK_FLAG);
 }
 *str = '\0';
 return(len);
}
/* ------------ travstr ------------------------------------------------- */
/* Traversiert einen string   von einem Stack st1 auf einen               */
/* Stack st2 und speichert bis zu maxlen value-Werte in dem               */
/* Zeichenstring str.Als Wert wird die Laenge des multi_atoms             */
/* geliefert.Es ist sicherzustellen dass auf st1 ein                      */
/* string steht und dass str mindestens maxlen+1 Zeichen fasst.           */
travstr(st1,st2,str,maxlen)
STACK *st1,*st2;
register char *str;
register int maxlen;
{
 register STACKELEM m;
 register int i,ebene,len;
 register unsigned char c;

 i = arity(m = movenf(st1,MST));
 push(M1ST,m);
 ebene = len = 1;
 *str++ = (char)STRBEGIN;
 while (ebene > 0 && i > 0) {
   decrarity(M1ST);
   --i;
   if (isconstr(m = popnf(st1))) {
     i = arity(m);
     push(MST,m);
     push(M1ST,m);
     ++ebene;
     len++;
     if (len <= maxlen)
       *str++ = (char)STRBEGIN;
    }	else
   if (noedit(m) == NILSTR) {
     len++;
     if (len <= maxlen)
       *str++ = (char)STRBEGIN;
     len++;
     if (len <= maxlen)
       *str++ = (char)STREND;
    }
   else {
     push(st2,m);
     c = (unsigned char)value(m);
     if (c < 32 || c > 126)                               /* stt  2.1.96 */
     {
       len += 4;
       if (len <= maxlen)
       {
         *str++ = '\\';
         *str++ = '0'+(c / 64);
         *str++ = '0'+((c % 64) / 8);
         *str++ = '0'+(c % 8);
       }
     }
     else
     {
       len++;
       if (len <= maxlen)
         *str++ = c;
     }
   }
   while (i == 0 &&  ebene > 0) {
     --ebene;
     ppop(M1ST);
     move(MST,st2);
     i = arity(topof(M1ST));
     len++;
     if (len <= maxlen)
       *str++ = (char)STREND;
   }
 }
 *str = '\0';
 return(len);
}
/* ------------ dsplstack ----------------------------------- */
/* Zeigt den Inhalt eines Stacks auf dem Bildschirm an        */
dsplstack(st,name,exprst,disp)
STACK *st;
char *name;
int exprst,disp;
{
 STACKELEM m;
 long pos,stacksize();
 int n,c,ct;
 char *s;
 STACK X;
 extern char msg[];
 extern int li;

 asgnst(&X,st);
 pos = stacksize(&X);
 ct = 5;
 c = ' ';
 CLEARSCREEN();
 fprintf(stdout,"Stack %s\n\r",name);
 while (((m = popnf(&X)) != STBOTTOM) || (X.cnt != 0L)) {
  if (ct++ >= li) {
    ct = 5;
    fflush(stdout);
    if ((c = getkey()) != ' ')
      break;
  }
  sprintf(msg,"pos: %4d   %08lx  ",--pos,m);
  putstr(msg);
  m &= ~EDIT_FIELD;
  if (exprst) {
    if (isatom(m) && multi_atom(m)) {
      n = isprint((char)value(m)) ? (char)value(m) : ' ';
      sprintf(msg,"%c_%-1d",n,(int)marking(m));
      putstr(msg);
     }
    else {
      if ((n = kwsearch(m,kwtb,kwtblen)) != FAILURE &&
          kwtb[n].dsp[0] != '\33') {
         s = kwtb[n].dsp;
         while (isprint(*s))
           fputc(*s++,stdout);
         if (isconstr(m)) {
           sprintf(msg,"_%-d",arity(m));
           putstr(msg);
         }
        }                                            else
      if ((n = kwsearch(m,rkwtb,rkwtblen)) != FAILURE) {
         s = rkwtb[n].dsp;
         while (isprint(*s))
           fputc(*s++,stdout);
         if (isconstr(m)) {
           sprintf(msg,"_%-d",arity(m));
           putstr(msg);
         }
      }
     }
    }
   else {
     n = (int)arity(m);
     fprintf(stdout,"adr: ( %2d , %2d ) symb: %c",n/co,n%co,(char)class(m));
   }
   putstr("\n\r");
  }
  fflush(stdout);
  if (c == CNTC)  error("Abbruch in dsplst");
  if (c == ' ')   getkey();
  if (disp) {
    initscreen();
    dispfa();
  }
 }
#ifdef UnBenutzt
/* ------------ dsplst ----------------------------------- */
dsplst(st,name)
 STACK *st;
 char *name;
 {
 STACKELEM m;
 long pos,stacksize();
 int n,c,ct,fnr;
 char *s;
 STACK X;
 extern char msg[];

 asgnst(&X,st);
 pos = stacksize(&X);
 ct  = 0;
 c   = ' ';
 CLEARSCREEN();
 sprintf(msg,"Stack %s\n\r\n\r",name); putstr(msg);
 while ((m = popnf(&X)) != STBOTTOM  ||  X.cnt != 0L ) {
    if (ct++ >= 20) {
      ct = 0;
      fflush(stdout);
      if ((c=getkey()) != ' ') break;
    }
    sprintf(msg,"pos: %4d   %08lx  ",--pos,m);   putstr(msg);
    if (chained(m)  &&  pos >= 0) {
      fnr = getfnr(popnf(&X));
      --pos;
     }
    else
      fnr = 0;
    m &= ~EDIT_FIELD;
    if (isatom(m) && multi_atom(m)) {
      n = isprint((char)value(m)) ? (char)value(m) : ' ';
      sprintf(msg,"%c_%-1d",n,(int)marking(m)); putstr(msg);
    }
    else {
     if ((n = kwsearch(m,kwtb,kwtblen)) != FAILURE) {
        s = kwtb[n].dsp;
        while (isprint(*s))
          fputc(*s++,stdout);
        if (isconstr(m)) {
          sprintf(msg,"_%-d",arity(m));       putstr(msg);
        }
       }                                                else
      if ((n = kwsearch(m,rkwtb,rkwtblen)) != FAILURE) {
         s = rkwtb[n].dsp;
         while (isprint(*s))
           fputc(*s++,stdout);
         if (isconstr(m)) {
           sprintf(msg,"_%-d",arity(m));
           putstr(msg);
         }
      }
    }
    if (fnr) {
      sprintf(msg,"    %4d    %4d",noparam(fnr),isparam(fnr));
      putstr(msg);
    }
    fputc('\r',stdout);
    fputc('\n',stdout);
  }
  fflush(stdout);
  if (c == CNTC)  error("Abbruch in dsplst");
  if (c == ' ')   getkey();
 }
#endif /* UnBenutzt	*/
/* end of      cstacks.c */
