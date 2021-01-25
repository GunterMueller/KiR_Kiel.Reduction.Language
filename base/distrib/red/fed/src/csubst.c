/* find, substitute                             subst.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:So  18.01.87        11:29:20        */

#include "cswitches.h"
#include "cportab.h"
#include "cestack.h"
#include "cedit.h"
#include "cscred.h"
#include "cencod.h"
#include "ctred.h"

extern char msg[];

static VOID ende(a,b)
 STACK *a,*b;
 {
  register STACKELEM el;

  while ( (el = pop(U1ST)) != ENDSYMB ) {
    push(a,el);
  }
  while ( (el = pop(U2ST)) != ENDSYMB ) {
    push(b,el);
  }
 }
static int end2(a,b,el1,el2)
 STACK *a,*b;
 STACKELEM el1,el2;
 {
   push(a,el1);         push(b,el2);
   while(pop(MST) != ENDSYMB)
   ;
   ende(a,b);
   return(NO);
 }

/* ========== findp =================================== */
/*  vergleicht die auf a und b liegenden ausdruecke     */
/* falls auf a PLACEH, so b egal                        */
int findp(a,b)
 STACK *a,*b;
 {
  register STACKELEM el,el2;
  register int w;

  push(U1ST,ENDSYMB);
  push(U2ST,ENDSYMB);
  push(MST,ENDSYMB);

  FOREVER {
    w = YES;
    while (w) {
     el = pop(a); el2 = pop(b);
     if (noedit(el) == PLACEH) {
       push(U1ST,el);
       push(b,el2);
       stransp(b,U2ST);
       w = NO;
      }                                                       else
     if (class(el) != class(el2) || value(el) != value(el2)) {
       return(end2(a,b,el,el2));
      }                                                         else
     if (isconstr(el)) {                /* => isconstr(el2) ! */
       if (arity(el) == arity(el2)) {
         push(U1ST,el);
         push(U2ST,el2);
         push(MST,el);
         w = NO;
        }
       else {
         return(end2(a,b,el,el2));
       }
      }                                                         else
     if (single_atom(el) || marked0(el)) {      /* vgl. oben    */
       push(U1ST,el);
       push(U2ST,el2);
       w = NO;
      }                                                         else
      {         /* multi_atom   */
       push(U1ST,el);
       push(U2ST,el2);
      }
    }
    w = YES;
    while (w) {
     el = pop(MST);
     if (el == ENDSYMB) {
        ende(a,b);
        return(YES);
      }              else
     if (isconstr(el)) {
       if (arity(el) > 0) {
         push(MST,el);
         decrarity(MST);
         w=NO;
       }
      }              else
      {
        sprintf(msg,"Illegal element %lx in findp",el);
        error(msg);
     }
    }
  }
 }

/* ======= find ================= */
int find(a,b,next)
 STACK *a,*b;
 register int next;
 {
  register int found,n;

/*
dsplstack(a,"a in find",1,1);
dsplstack(b,"b in find",1,1);
*/
  n     = 0;
  found = FALSE;
  if (next) {
    if (isempty(b)) return(FAILURE);
    else {
      if (notdisp(topof(b))) {
        pop(b);
        ++n;
      }
      ++n;
      if (isconstr(topof(b))) pop(b);
      else                    delete(b);
    }
  }
  while (!found  &&  !isempty(b)) {
    found = findp(a,b);
    if (!found) {
      if (isconstr(topof(b))) pop(b);
      else                    delete(b);
      ++n;
    }
  }

/*printf("found:%d n:%d\n",found,n);*/

  return(found ? n : FAILURE);
 }

/* ----------- copywhole ----------------- */
copywhole(st)
 STACK *st;
 {
  register STACKELEM m;
  register int p;

  if ((m = topof(MST)) != STBOTTOM  &&  notdisp(m)  &&
      arity(m) == arity(topof(M1ST))) {
     move(MST,EST);
     p = 1;
   }
  else
    p = 0;
  copyst(EST,st);
  while (p-- > 0)
    move(EST,MST);
 }
/* -------- stransp --------------------------- */
stransp(st1,st2)
 STACK *st1,*st2;
 {
  static STACKELEM m;
  register int n;

  if ((m = topof(st1)) != STBOTTOM)
    if (isconstr(m)) {
      move(st1,st2);
      for (n = arity(m) ; n > 0 ; n-- )
        stransp(st1,st2);
     }
    else {
      if (single_atom(m)  || marked0(m))
        move(st1,st2);
      else {
        move(st1,st2);
        while (marked1(move(st1,st2)))
        ;
      }
    }
 }
/* end of       subst.c */

