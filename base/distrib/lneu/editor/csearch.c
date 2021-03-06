/*                                            csearch.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:Do  11.12.86        20:50:23        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cscred.h"
#include "cestack.h"
#include "ckwtb.h"
#include "cparm.h"
#include "ccommand.h"

/* ------------- encsearch ------------------------------------------- */
/* suche in tabelle tb (kwtb) nach mit eno kompatiblen eintrag,        */
/* dessen stackel. mit m uebereinstimmt                                */
encsearch(eno,m,tb,l)
 int eno,l;
 register STACKELEM m;
 struct kwrd tb[];
 {
  register int n;
  EXTYP etype;

  assex(etype,etptb[eno].exptp);
  if (isconstr(m)) m &= CLASS_FIELD;
  else             m &= ~EDIT_FIELD;
  for (n = 0; n < l; n++)
    if (m == tb[n].elem)
      if (iskomp(tb[n].etp,etype))
        if (eno != n_fcn)
          return(n);
        else /* eno == n_fcn       */
          if (is_predef(kwtb[n].kwtbinfo))
            return(n);
  return(FAILURE);
 }

/* ------------- kwsearch -------------------------------------------- */
/* suche in tabelle m nach eintrag mit stackel. m                      */
kwsearch(m,tb,l)
 register STACKELEM m;
 struct kwrd tb[];
 register int l;
 {
  register int n;

  if (isconstr(m)) m &= CLASS_FIELD;
  else             m &= ~EDIT_FIELD;
  for (n = 0; n < l; n++)
    if (m == tb[n].elem)
      return(n);
  return(FAILURE);
 }
/* ------------- wrdsearch ------------------------------------------- */
/* suche in tabelle tb nach einen eintrag, dessen dsp-string mit *istr */
/* 'matcht' (matchwrds) und der erlaubt (allowed) ist sowie kompatibel */
/* mit eno ist.                                                        */
wrdsearch(tstr,istr,tb,l,eno)
 char *(*tstr),*(*istr);
 int eno,l;
 struct kwrd tb[];
 {
  register int n,i;
  char *s;
  EXTYP etype;

  assex(etype,etptb[eno].exptp);
  s = *istr;
  for (n = 0; n < l; n++) {
    if (allowed(tb[n].kwtbinfo)  &&  (i = matchwrds(tb[n].dsp,s)))
      if (iskomp(tb[n].etp,etype)) {
        *istr= s + i;
        *tstr = tb[n].dsp + i;
        return(n);
      }
  }
  return(FAILURE);
 }
/* ------------- iskwrd ---------------------------------------------- */
/* suche in tabelle table nach string s (fuer schluesselwoerter)       */
iskwrd(s,table,anz)
 char s[],*table[];
 int anz;
 {
  register int i,j,found,eq;

  found = FALSE;
  i     = 0;
  while ( i < anz  &&  !found) {
    j  = 0;
    eq = TRUE;
    while (s[j] && (isalnum(s[j]) ||  s[j] == EXTRENN) && eq) {
      if (eq = s[j] == table[i][j])
        j++;
    }
    if (eq)
      eq = table[i][j] == '\0';
    if (!(found = eq))
      i++;
  }
  return(found ? i : FAILURE);
 }
/* ------------- ispredfct ------------------------------------------- */
/* ist s die bezeichnung einer vordefinierte funktion ?                */
ispredfct(s,table,anz)
 char s[];
 predefined table[];
 int anz;
 {
  register int i,j,found,eq;
  register char *t;

  found = FALSE;
  i     = 0;
  while ( i < anz  &&  !found  &&  s[0] >= (table[i].predfct)[0]) {
    j  = 0;
    eq = TRUE;
    t  = table[i].predfct;
    while (s[j] && (isalnum(s[j]) ||  s[j] == EXTRENN) && eq) {
      if (eq = s[j] == t[j])
        j++;
    }
    if (eq)
      eq = t[j] == '\0';
    if (!(found = eq))
      i++;
  }
  return(found ? i : FAILURE);
 }
/* ------------- ispredef -------------------------------------------- */
/* ist el die stackdar. einer vordefinierten funktion ?                */
ispredef(el,tb,anz)
 STACKELEM el;
 predefined tb[];
 int anz;
 {
  register int i,found;

  found = FALSE;
  i     = 0;
  while ( i < anz  &&  !found)
    if (!(found = tb[i].predelem == el))
      ++i;
  return(found ? i : FAILURE);
 }
/* ------------- cmdsearch ------------------------------------------- */
/* ist *cstr ein commando (bzw. das erste word von *cstr) ?            */
/* falls ja: *cstr nach dem commando plazieren                         */
cmdsearch(cstr,tb,len)
char *(*cstr);
struct cmnd tb[];
int len;
{
 char *s,*t,*skipblanks();
 int n,i;

 if ((s = skipblanks(*cstr)) != '\0') {
    for (n = 0 ; n < len ; n++) {
       t = tb[n].cmd;
       i = 0;
       while (TOLOWER(s[i]) == TOLOWER(t[i])) {
         if (iscntrl(s[i])  ||  s[i] == ' ') {
           *cstr = skipblanks(s + i);
           return(n);
          }
         else i++;
       }
       if ((s[i] == '\0'  ||  s[i] == ' ')  &&
           (t[i] == '\0'  ||  t[i] == ' '  ||  isupper(t[i])  ||
            i > 0  &&  isupper(t[i-1])))
         break;
    }
    if (n < len) {
      *cstr = skipblanks(s + i);
      return(n);
     }
    else return(FAILURE);
  }
 else
   return(FAILURE);
}
/* ----------- callkon -------------------------------------- */
/* liefert den CALL-Konstruktor fuer vordefinierte Funktionen */
/* mit der Stelligkeit i                                      */
STACKELEM callkon(i)
 int i;
 {
  register STACKELEM el1;

   switch(i) {
     case 2  : el1 = AR0(CALL1);        break;
     case 3  : el1 = AR0(CALL2);        break;
     case 4  : el1 = AR0(CALL3);        break;
     case 5  : el1 = AR0(CALL4);        break;
     default : el1 = AR0(CALL);         break;
   }
  return(el1);
 }
/* ----------- checkf --------------------------------------- */
/* checke, ob fct vordefinierte funktion ist                  */
/* wenn ja: ueberpruefe stelligkeit mit der von m und liefere */
/* bei uebereinstimmung den richtigen Konstruktor,            */
/* sonst: STBOTTOM                                            */
STACKELEM checkf(fct,m)
 STACKELEM fct,m;
 {
  register int i,ar,ok;

  ok = FALSE;
  if ((i = ispredef(noedit(fct),functions,nfunct)) != FAILURE) {
    ar = functions[i].pred_ar + 1;
    if (ok = (ar == arity(m))) {
      fct = callkon(ar);  /* den richtigen konstruktor */
    }
  }
  return(ok ? fct : STBOTTOM);
 }

/* ----------- checkia -------------------------------------- */
/* checke, ob fct vordefinierte interaktion ist               */
/* wenn ja: ueberpruefe stelligkeit mit der von m und liefere */
/* bei uebereinstimmung den richtigen Konstruktor,            */
/* sonst: STBOTTOM                                            */
STACKELEM checkia(fct,m)
 STACKELEM fct,m;
 {
  register int i,ar,ok;

  ok = FALSE;
  if ((i = ispredef(noedit(fct),interactions,nintact)) != FAILURE) {
    ar = interactions[i].pred_ar + 1;
    if (ok = (ar == arity(m))) {
      fct = callkon(ar);  /* den richtigen konstruktor */
    }
  }
  return(ok ? fct : STBOTTOM);
 }

/* ------------- isprim ----------------------------------------------- */
/* suche in tabelle tb (kwtb) nach stackelement m und test auf          */
/* primitive fkt                                                       */
isprim(m,tb,l)
 register STACKELEM m;
 struct kwrd tb[];
 register int l;
 {
  register int n;

  if (single_atom(m)) {
    m &= ~EDIT_FIELD;
    for (n = 0; n < l; n++)
      if (m == tb[n].elem)
        return(is_prim(tb[n].kwtbinfo));
  }
  return(FALSE);
 }
/* end of      csearch.c */
