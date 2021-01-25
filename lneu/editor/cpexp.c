/* pick,put und delete                          pexp.c  */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:Do  08.01.87        20:08:45        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cscred.h"
#include "cestack.h"
#include "cfiles.h"

/* ------------- pickexpr -------------------------------- */
/* kopieren der CE auf stack st                            */
pickexpr(st)
 STACK *st;
 {
  register int res;
  
  res = pick(st);
  if (res == RDOK) {
    message(MROW,MCOL,"Picked up CE");
   }                                        else 
  if (res == EXNPAR) {
    message(MROW,MCOL,"Can't pick NOPAR");
   }                                        else 
   {
    message(MROW,MCOL,"pick: Unknown error");
   }    
  getunget();  
  clrmsg(MROW,MCOL);
 }
/* ------------- pick -------------------------------- */
/* kopieren der CE auf stack st                        */
pick(st)
 STACK *st;
 {
  register STACKELEM m;
  register int p,res;
  
  if (noedit(topof(EST)) != NOPAR) {
    clearstack(st);
    if ((m = topof(M1ST)) != STBOTTOM  &&  notdisp(m)  &&         
        arity(m) == arity(topof(MST))) {
      move(MST,EST);	/* move CALL oder FDEC auf E  */
      p = 1;
     }
    else	/* kein NDISP-Konstr.   */
      p = 0;
    duplicate(EST,st);	/* kopiere E nach st    */
    res = RDOK;
    while (p-- > 0)	/* ev. NDISP-Konstr. zurueck-'move'n    */
      move(EST,MST);
   }
  else {
    res = EXNPAR;
  }
  return(res);
 }
/* ------------- putexpr -------------------------------------- */
/* einsetzen der expression auf stacks st anstelle der CE       */
putexpr(st)
 register STACK *st;
 {
  register STACKELEM m;
  register int eno,ene,cm,ena;
  STACK X;

  if (noedit(topof(st)) != NOPAR) {
    X.stptr = SNULL;
    ene = ena = (int)edit6(topof(EST));
    if (IS_DECN(ena) ||  ena == n_fcn)	/* fnam oder fcn auf e ?        */
      ene = (int)edit6(topof(MST));	/* info des konstruktors nehmen */
    eno = (int)edit6( m = topof(st));
    cm  = class(m);	/* <> 0 !       */
    if (eno == n_fdex)
      eno = n_expr;               else
    if (eno == n_dcex)
      if (cm == AR0(DECL))
        eno = n_decl;
      else
        eno = n_expr;
    if (iskomp(etptb[ene].exptp,etptb[eno].eetp)  &&
       (!ismatel(eno) || (matpath1()  &&  arity(topof(&M)) < 2L) ||
        matchmat(st,EST) )                        ||              
       (ene == n_dcex && cm == AR0(FDEC)  && class(topof(EST)) != AR0(DECL))) {    
      if (ene != ena) {	/* fnam oder fcn auf e ?               */
        move(MST,EST);	/* kompl. ausdruck (call oder fdec)    */
        pop(M1ST);	/* nach e bringen                      */
        incrarity(M1ST);	/* arity korrigieren                   */
      }
  	/* ev. korrekturen wg. NDISP-Konstr. vornehmen      */
      if (cm == AR0(FDEC)) {	/* fdec einfuegen ?        */
        cm = 0;	/* CE nicht retten         */
        if (class(topof(MST)) == AR0(DECL)) {	/* ex. decl auf m ?        */
          incrarity(MST);
          incrarity(M1ST);
         }
        else {	/* decl einfuegen      */
          transport(EST,st);	/* uebertrage rumpf uebergeordneter fkt     */
          push(st,AR0(DECL) | toedit6(n_dcex) | toarity(2));
        }
      }	/* end if constr(m)    */
      if (cm) {	/* CE retten ?         */
        crstack(XST,STKPAGE);
        transport(EST,XST);
      }
      duplicate(st,EST);	/* st einsetzen        */
      push(EST,noedit(pop(EST)) | toedit(ene));
      if (cm) {
        killstack(SST);
        asgnst(SST,XST);	/* gerettete CE nach s     */
      }
      dispfa();
     }
    else {
      message(MROW,MCOL,"Type conflict, cmd ignored !");
      getunget();  
      clrmsg(MROW,MCOL);
    }
  }
 }
/* ---------- delfdec ------------------------------------------------ */
/* delete function-declaration                                         */
delfdec(a)
 int a;
 {
  register int e;
  
  for (e = (int)arity(pop(MST)); e > 0 ; --e) {	/* loesche fdec */
     if (isdispld(topof(EST)))
       pop(EADRST);	/* loesche Adr. ??????  */
     delete(EST);	/* fnam, v_l, body      */
  }
  if (isdispld(pop(M1ST)))	/* loesche fdec von M1  */
     pop(AADRST);
  if (a) {
    decrarity(MST);	/* erniedrige stelligkeit von decl      */
    if (arity(topof(MST)) == 1) {	/* nur noch expr ?      */
      pop(M1ST);	/* decl loeschen        */
	/* decl_expr als edit info                      */
      push(EST,noedit(pop(EST)) | toedit6(n_dcex));
      if (isdispld(pop(MST)))
        pop(AADRST);
      incrarity(M1ST);	/* korrekt ?    */
    }
   }
  else {	/* not a        */
    incrarity(M1ST);
  }
 }
/* ---------- deldecl ------------------------------------------------ */
/* delete declaration                                                  */
deldecl()
 {
  register int e;
  
  for (e = (int)arity(pop(EST)); e > 1 ; --e) {	/* loesche fdec's       */
     if (isdispld(topof(EST)))
       pop(EADRST);	/* loesche Adr. ??????  */
     delete(EST);	/* fnam, v_l, body      */
  }
  push(EST,noedit(pop(EST)) | toedit(n_dcex));
 }
/* ---------- delfcn ------------------------------------------------- */
/* delete call                                                         */
/* a: soll stattdessen PLACEH auf E geschrieben werden ?               */
delfcn(a)
 int a;
 {
  register int e;
  STACKELEM el;
  for (e = (int)arity(pop(MST)); e>0; --e) {	/* loesche call */
     if (isdispld(topof(EST)))
       pop(EADRST);	/* loesche Adr. ??????  */
     delete(EST);	/* fcn, e_l             */
  }
  if (isdispld(pop(M1ST)))	/* loesche call von M1  */
     pop(AADRST);
  incrarity(M1ST);
  if (a) {
    e = (topof(MST)==STBOTTOM) ? n_dcex : 0;
    if (class(el = topof(MST)) == AR0(CALL)  && arity(el) == 2)
      el = NOPAR;	/* keine parameter      */
    else
      el = PLACEH;	/* platzhalter          */
    push(EST,el | toedit6(e));
  }
 }
/* ---------- delexp ------------------------------------------------- */
/* loeschen eines ausdrucks aus einer liste mit mindestens 2 elementen */
delexp()
 {
  STACKELEM m;
  int e;

  m = topof(EST);
  e = (int)edit6(m);
  if ((islist(etptb[e].exptp)) && ((long)arity(topof(MST)) >= 2L)  &&
      (!ismatel(e)  ||  matpath1()))             {
   if (arity(topof(MST)) > arity(topof(M1ST))) {
    e = (int)edit6(topof(AST));
    if (islist(etptb[e].exptp)) {
      clearstack(SST);
      transport(EST,SST);
      decrarity(MST);
      transport(AST,EST);
      pop(EADRST);	/* adresse auf bildschirm entfernen */
     }
    else {	/* auf A kein listenelement     */
      if ((long)arity(topof(M1ST)) >= 2L) {
        transport(EST,AST);
        e = (int)edit6(topof(EST));	/* naechstes element auf E      */
        transport(AST,EST);
        if (islist(etptb[e].exptp)) {
          clearstack(SST);
          transport(EST,SST);
          decrarity(MST);
          decrarity(M1ST);
         }
        else {
          if (class(topof(MST)) == AR0(FDEC)) {
            clearstack(SST);
            transport(EST,SST);
            push(EST,NOPAR | toedit6(n_varlst));
           }
          else return;	/* del impossible */
        }
       }
      else {
        if ((e = class(topof(MST))) == AR0(CALL) ||  e == AR0(US_KON)) {
          clearstack(SST);
          transport(EST,SST);
          push(EST,NOPAR | toedit6(n_exprlst));
         }
        else return;
      }
    }
   }
  else {	/* arity(MST) == arity(M1ST)    */
#if ClausR
    if (class(topof(MST)) == AR0(SUB_CR)) {
      clearstack(SST);
      transport(EST,SST);
      push(EST,NOPAR | toedit6(n_varlst));
     }
    else {
#endif
    transport(EST,AST);
    e = (int)edit6(topof(EST));
    transport(AST,EST);
    if (islist(etptb[e].exptp)) {
      clearstack(SST);
      transport(EST,SST);
      decrarity(MST);
      decrarity(M1ST);
     }
    else return;	/* del impossible */
#if ClausR
    }
#endif
   }
  }
 else {	/* keine liste  */
   if (IS_DECN(edit6(topof(EST)))) {	/* function-decl. ?       */
     delfdec(TRUE);
    }   else
   if (edit6(topof(EST)) == n_fcn) {	/* function-call ? */
     delfcn(TRUE);
    }   else
   if (class(topof(EST)) == AR0(DECL)) {	/* decl ?               */
     deldecl();
    }
   else return;
 }
 dispfa();
}
/* end of       pexp.c */

