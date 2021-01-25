/*                                     cgetstr.c        */
/* letzte aenderung:    22.11.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cestack.h"
#include "cback.h"
#include "ckwtb.h"

extern char *skipblanks();

/* ------------ getletstr ------------------------- */
/* scannen einer alphanumerischen zeichenkette      */
getletstr(eno,pts,firstmark)
 register int eno;
 register char *(*pts);
 STACKELEM firstmark;
 {
  STACKELEM callkon();
  register int c,ispredef,index,nokwrd,expected;

  *pts     = skipblanks(*pts);
  c        = *(*pts);
  nokwrd   = expected = TRUE;
  ispredef = isalpha(c) &&
             ((index = ispredfct(*pts,functions,nfunct)) != FAILURE);
  if ((isalpha(c) || c == EXTRENN) &&  (expected = isvar(etptb[eno].exptp))  &&
      (nokwrd = iskwrd(*pts,kwrds,nkwrds) == FAILURE)  &&
      !((IS_DECN(eno) || eno == n_var || eno == n_varlst
        ||  eno == n_pattern ||  eno == n_pattlst
      ) && ispredef)) {
    if (ispredef)
      push(BST,functions[index].predelem | toedit6(eno));
    else
      push(BST,LSTR0 | firstmark | tovalue(TOUPPER(c)) | toedit6(eno));
    c = *(++(*pts));
    while (isalnum(c)  ||  c == EXTRENN  ||
           c == GRAPH1 || c == GRAPH2 || c == GRAPH3 ||
           (eno == n_fcn  &&  !ispredef  &&  c == INT_FC)) {
      if (!ispredef)
        push(BST,LSTR1 | tovalue(c) | toedit6(eno));
      c = *(++(*pts));
    }
    return(c);
   }
  else {
    push(BST,PLACEH | toedit6(eno));
    if (!nokwrd)  return(fehl(FKEY,eno));   else
    if (ispredef) return(fehl(FPRED,eno));  else
    if (expected) return(fehl(FLET,eno));
    else          return(fehl(FLETN,eno));
  }
 }

/* ------------ getdigstr ------------------------- */
getdigstr(eno,pts,firstmark)
 register int eno;
 register char *(*pts);
 STACKELEM firstmark;
 {
  register int c;

  c = *(*pts);
  if (isdigit(c)) {
    push(BST,DSTR0 | firstmark | tovalue(c) | toedit6(eno));
    c = *(++(*pts));
    while (isdigit(c)) {
      push(BST,DSTR1 | tovalue(c) | toedit6(eno));
      c = *(++(*pts));
    }
    return(c);
   }
  else {
    return(fehl(FDIG,eno));
  }
 }

/* ------------ getdecimal ------------------------ */
getdecimal(eno,pts,firstmark)
 register int eno;
 register char *(*pts);
 STACKELEM firstmark;
 {
  register int c;

  c = *(*pts);
  if (isdigit(c)) {
    if ((c = getdigstr(eno,pts,firstmark)) == DECPOINT) {
      push(BST,DSTR1 | tovalue(c) | toedit6(eno));   /* nix firstmark */
      c = *(++(*pts));
      if (isdigit(c))
        c = getdigstr(eno,pts,DSTR1);
      else {
        pop(BST);
        return(fehl(FDIG,eno));
      }
    }
    return(c);
   }
  else {
    return(fehl(FDIG,eno));
  }
 }

/* ------------ getnumber ------------------------- */
getnumber(eno,pts)
 register int eno;
 register char *(*pts);
 {
  register int c;

  *pts = skipblanks(*pts);
  c = *(*pts);
  if (isnumbval(etptb[eno].exptp)) {
    if (isdigit(c))
      return(getdecimal(eno,pts,(STACKELEM)DSTR0));
    else {
      switch(c) {
        case NEGSIGN : push(BST,DSTR0 | tovalue(c) | toedit6(eno));
                       c = *(++(*pts));
                       if (isdigit(c))
                         return(getdecimal(eno,pts,DSTR1));
                       else {
                         return(fehl(FDIG,eno));
                       }
        case '\0'    : push(BST,PLACEH | toedit6(eno));
                       return(c);
        default      : push(BST,PLACEH | toedit6(eno));
                       return(fehl(FNUMB,eno));
      }
    }
   }
  else {
    push(BST,PLACEH | toedit6(eno));
    return(fehl(FNUMBN,eno));
  }
 }
#ifdef UnBenutzt
/* ------------ getint ---------------------------- */
getint(eno,pts)
 register int eno;
 register char *(*pts);
 {
  register int c;

  *pts = skipblanks(*pts);
  c = *(*pts);
  if (isintval(etptb[eno].exptp)) {
    if (isdigit(c))
      return(getdigstr(eno,pts,(STACKELEM)DSTR0));
    else {
      switch(c) {
        case NEGSIGN : push(BST,DSTR0 | tovalue(c) | toedit6(eno));
                       c = *(++(*pts));
                       if (isdigit(c))
                         return(getdigstr(eno,pts,DSTR1));
                       else {
                         delete(BST);
                         push(BST,PLACEH | toedit6(eno));
                         return(fehl(FDIG,eno));
                       }
        case '\0'    : push(BST,PLACEH | toedit6(eno));
                       return(c);
        default      : push(BST,PLACEH | toedit6(eno));
                       return(fehl(FINT,eno));
      }
    }
   }
  else {
    push(BST,PLACEH | toedit6(eno));
    return(fehl(FINTN,eno));
  }
 }
#endif /* UnBenutzt	*/
#if N_STR
#define MAXL 64
/* ------------ getstr ---------------------------- */
getstr(eno,pts)
 register int eno;
 register char *(*pts);
 {
  register int c,slevel;
  int len[MAXL];

  slevel = 0;
  *pts  = skipblanks(*pts);
  c     = *(*pts);
  if (isstrval(etptb[eno].exptp) || isstr(etptb[eno].exptp)) {
    if (c == STRBEGIN) {
      slevel++;
      len[slevel] = 0;
      ++(*pts);
      /* eno = n_str;        /* ????? */
      do {
        c = *(*pts);
        if (isprint(c)) {
          if (c == STRBEGIN) {
            len[slevel]++;
            slevel++;
            len[slevel] = 0;
           } else
          if (c == STREND) {
            if (len[slevel] == 0) push(BST,NILSTR | toedit6(eno));
            else push(BST,KQUOTE | toarity(len[slevel]));
            slevel--;
           }
          else {
            push(BST,LSTR0 | tovalue(c) | toedit6(eno));
            len[slevel]++;
          }
          ++(*pts);
         }
        else break;
      } while(slevel > 0);
      for ( ; slevel > 0 ; --slevel) {
        if (len[slevel] == 0) push(BST,NILSTR | toedit6(eno));
        else push(BST,KQUOTE | toarity(len[slevel]));
      }
      return(*(*pts));
     }
    else {
      push(BST,PLACEH | toedit6(eno));
      return(fehl(FSTR,eno));
    }
   }
  else {
    push(BST,PLACEH | toedit6(eno));
    return(fehl(FSTRN,eno));
  }
 }
#else /* !N_STR  */
/* ------------ getstr ---------------------------- */
getstr(eno,pts)
 register int eno;
 register char *(*pts);
 {
  register int c,slevel;

  slevel = 0;
  *pts  = skipblanks(*pts);
  c     = *(*pts);
  if (isstrval(etptb[eno].exptp)) {
    if (c == STRBEGIN) {
      push(BST,LSTR0 | tovalue(c) | toedit6(eno));
      slevel++;
      ++(*pts);
      do {
        c = *(*pts);
        if (isprint(c)) {
          if (c == STRBEGIN) slevel++;            else
          if (c == STREND)   slevel--;
          push(BST,LSTR1 | tovalue(c) | toedit6(eno));
          ++(*pts);
         }
        else break;
      } while(slevel > 0);
      for ( ; slevel > 0 ; --slevel)
        push(BST,LSTR1 | tovalue(STREND) | toedit6(eno));
      return(*(*pts));
     }
    else {
      push(BST,PLACEH | toedit6(eno));
      return(fehl(FSTR,eno));
    }
   }
  else {
    push(BST,PLACEH | toedit6(eno));
    return(fehl(FSTRN,eno));
  }
 }
#endif
/* ------------ getreal --------------------------- */
getreal(eno,pts)
 register int eno;
 register char *(*pts);
{
 register int c;

 *pts = skipblanks(*pts);
 /* unused laut optimizer:	c = *(*pts);	*/
#ifdef OldVersion
 if (isrealval(etptb[eno].exptp)) {
#else
 if (isnumbval(etptb[eno].exptp)) {
#endif
    if ((c = getnumber(eno,pts)) == DECEXP) {
       push(BST,DSTR1 | tovalue(c) | toedit6(eno));
       c = *(++(*pts));
       if (c == NEGSIGN) {
          push(BST,DSTR1 | tovalue(c) | toedit6(eno));
          c = *(++(*pts));
       }
       if (isdigit(c))
          return(getdigstr(eno,pts,DSTR1));
       else {
          delete(BST);
          push(BST,PLACEH | toedit6(eno));
          return(fehl(FREAL,eno));
       }
     }
    else return(c);
  }
 else {
    push(BST,PLACEH | toedit6(eno));
    return(fehl(FREALN,eno));
  }
}
/* end of              cgetstr.c */
