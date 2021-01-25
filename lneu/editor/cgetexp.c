/*                                      getexp.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    07.06.88                 */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "ctermio.h"
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cscred.h"
#include "cestack.h"
#include "ckwtb.h"
#include "cback.h"

#define FILSYMB '^'

/* ---------- getexp -------------------------------------------- */
getexp(eno,pts,plist)
 int eno,plist;
 char *(*pts);
 {
  STACKELEM m,callkon();
  char *t,*old,*skipblanks();
  register int n,c,ind;
  static int ene;

  
  if (!isstr(etptb[eno].exptp)) {
    *pts = old = skipblanks(*pts);
   }
  else {
    old = *pts;
  }
  c    = *(*pts);
  if (c != '\0') {              /* eingabe noch nicht zuende ?  */
    /* neu fuer backtracking: getexp gemaess parse[];   */
    if ( ++level > mlev) mlev = level;  /* gilt das nicht immer ? */
    if (level >= MDEPTH) error("too deep nested backtracking");
    if (parse[level].enop == -2) {
        /* neueintrag   */
        parse[level].enop = parse[level].enip = eno;
        /* count unveraendert -1        */
#if B_DEBUG
      if (debug > 5) {
        fprintf(stderr,"getexp: neueintrag eno %o",eno);
        waitkey();
      }
#endif
     }
    else {
      eno = parse[level].enop;
#if B_DEBUG
      if (debug > 5) {
        fprintf(stderr,"getexp: alteintrag eno %o",eno);
        waitkey();
      }
#endif
    }
/* 'backtracking' - teil zuende */
#if B_DEBUG
      if (debug) {
        fprintf(stderr,"getexp: level %d, eno %o, ein: %s",level,eno,*pts);
        waitkey();
      }
#endif
    /* leere parameterliste ? (vorsicht)        */
    if (plist == n_exprlst && c == C_KLZU)
      push(BST,NOPAR | toedit6(eno));                           else
    if (c == K_KLZU  &&  ( plist == n_exprlst  ||  plist == n_pattlst) )
      push(BST,NOPAR | toedit6(eno));                           else
    if (plist == n_varlst && (c == F_KLZU || c == F_TRENN))
      push(BST,NOPAR | toedit6(eno));                           else
    if (c == CRSEXPR) {                 /* cursor-expression ?  */
      if (noedit(topof(EST)) == NOPAR) {
         push(BST,PLACEH | toedit6(eno));
         c = -1;                        /* nopar nicht als ce ! */
       }
      else {
        if (class(topof(EST)) == AR0(DECL)) ene = n_decl;
        else ene = (int)edit6(topof(EST));
        c   = *(++(*pts));
#if B_DEBUG
        if (debug > 2) {
          fprintf(stderr,"topof(E) %08lx ",class(topof(EST)));
          fprintf(stderr,"CE: ene: %o, eno %o",ene,eno);
          waitkey();
        }
#endif
        /* spezial-fall ? (ndisp-konstruktoren !)               */
       if (!IS_DECN(ene)  &&  ene != n_fcn  &&
           iskomp(etptb[eno].exptp,etptb[ene].eetp)) {
          duplicate(EST,BST);
          push(BST,noedit(pop(BST)) | toedit6(eno));
         }                              else
       if (ene == n_fnam)               /* fktsname (fdec)      */
         if (eno == n_fdex || eno == n_fdeclst) {
           /* setze fdec ein    */
           push(EST,noedit(tm) | toedit6(eno));
           duplicate(EST,BST);
           pop(EST);
           push(BST,noedit(pop(BST)) | toedit6(eno));
          }
         else {
           push(BST,PLACEH | toedit6(eno));
         }                              else
       if (ene == n_lnam)               /* letname (ldec)      */
         if (eno == n_ldlst) {
           /* setze ldec ein    */
           push(EST,noedit(tm) | toedit6(eno));
           duplicate(EST,BST);
           pop(EST);
           push(BST,noedit(pop(BST)) | toedit6(eno));
          }
         else {
           push(BST,PLACEH | toedit6(eno));
           if (c == '\0')  c = -1;
         }                                              else
#if LET_P
       if (ene == n_pnam)               /* letname (ldec)      */
         if (eno == n_pdlst) {
           /* setze pdec ein    */
           push(EST,noedit(tm) | toedit6(eno));
           duplicate(EST,BST);
           pop(EST);
           push(BST,noedit(pop(BST)) | toedit6(eno));
          }
         else {
           push(BST,PLACEH | toedit6(eno));
           if (c == '\0')  c = -1;
         }                                              else
#endif
       if (ene == n_fcn) {              /* fktsaufruf ? */
         n = encsearch(eno,AR0(tm),kwtb,kwtblen);
/*       if (iskomp(etptb[eno].exptp,etptb[n_call].eetp)) {   alt      */
         if (n != FAILURE  && iskomp(etptb[eno].exptp,kwtb[n].etp)) {
           push(EST,noedit(tm) | toedit6(eno));
           duplicate(EST,BST);
           pop(EST);
           push(BST,noedit(pop(BST)) | toedit6(eno));
          }
         else {
           push(BST,PLACEH | toedit6(eno));
           if (c == '\0')  c = -1;
         }
        }
       else {
         push(BST,PLACEH | toedit6(eno));
         if (c == '\0')  c = -1;
       }
     }
    }
        /* vorsicht hier: bei kompl. #  */
    else if (eno==n_var || eno==n_varlst  || IS_DECN(eno))
           c = getletstr(eno,pts,LSTR0);
    else if (eno == n_fcn) {
#if B_DEBUG
        if (debug > 2) {
          fprintf(stderr," getexp:n_fcn **pts = %c ",**pts);
          waitkey();
        }
#endif
            if (**pts != LBARSYMB) c = getletstr(eno,pts,LSTR0);
            else {
               n = encsearch(eno,AR0(PROTECT),kwtb,kwtblen);
#if B_DEBUG
        if (debug > 2) {
          fprintf(stderr," call scan with n=%d elem %08lx ",n,kwtb[n].elem);
          waitkey();
        }
#endif
               c = scanexp(t = kwtb[n].dsp,pts,n,eno);
             }
     }                                                                    else
    if (eno == n_nval || eno ==n_nvallst) c = getreal(eno,pts); /* alt: number */ else
    if (eno == n_sval || eno ==n_svallst) c = getstr(eno,pts);            else
    if (eno == n_fdeclst) {
      if (**pts == FILSYMB) {
        n = encsearch(eno,AR0(FILFKT),kwtb,kwtblen);
        c = scanlst(t = kwtb[n].dsp,pts,n,eno);
       }
      else {
        n = encsearch(eno,AR0(FDEC),kwtb,kwtblen);
        c = scanlst(t = kwtb[n].dsp,pts,n,eno);
      }
     }                                                                    else
#if LET_P
    if (eno == n_pdlst) {
      n = encsearch(eno,AR0(PDEC),kwtb,kwtblen);
      c = scanlst(t = kwtb[n].dsp,pts,n,eno);
     }                                                                    else
#endif
    if (eno == n_ldlst) {
      n = encsearch(eno,AR0(LDEC),kwtb,kwtblen);
      c = scanlst(t = kwtb[n].dsp,pts,n,eno);
     }                                                                    else
    if (eno == n_call) {
      n = encsearch(eno,AR0(CALL),kwtb,kwtblen);
      c = scanexp(t = kwtb[n].dsp,pts,n,eno);
     }                                                                    else
    if (eno == n_uskon  ||  eno == n_uskonp) {
      n = encsearch(eno,AR0(US_KON),kwtb,kwtblen);
      c = scanexp(t = kwtb[n].dsp,pts,n,eno);
     }                                                                    else
    if (eno == n_decl) {
      n = encsearch(eno,AR0(DECL),kwtb,kwtblen);
      c = scanexp(t = kwtb[n].dsp,pts,n,eno);
     }                                                                    else
    if (c == NO_PARC) {    /* nopar kann man nicht eingeben        */
      push(BST,PLACEH | toedit6(eno));
      c = FAILURE;
     }                                                                    else
    if ((n = wrdsearch(&t,pts,kwtb,kwtblen,eno)) != FAILURE) {
     /* ab hier spezial wg. predefined functions       */
     m = kwtb[n].elem;
#if B_DEBUG
     if (debug > 2) {
       ind = ispredef(m,functions,nfunct);
       fprintf(stderr," scan: %08lx ispredef: %d '%c' ",m,ind,**pts);
       if (ind != FAILURE) {
          register int j;
          j = encsearch(n_call,callkon(functions[ind].pred_ar+1),kwtb,kwtblen);
          fprintf(stderr," n %d elem %08lx ",j,kwtb[j].elem);
       }
       waitkey();
     }
#endif
     if (is_predef(kwtb[n].kwtbinfo)  &&
         (ind = ispredef(m,functions,nfunct)) != FAILURE  &&
         **pts == PRED_KL  &&
         iskomp(etptb[eno].exptp,etptb[n_call].eetp)  &&
         (n = encsearch(n_call,callkon(functions[ind].pred_ar+1),kwtb,kwtblen))
           != FAILURE
        ) {
       *pts = old;                          /* pts auf anfang          */
/*     parse[--level].enop == -2;           /* ???   */
       c = scanexp(kwtb[n].dsp,pts,n,n_call); /* auf CALLi scannen */
      }
     else
       c = scanexp(t,pts,n,eno);
     }                                                  else
    if (c == STRBEGIN) c = getstr(eno,pts);                             else
#if N_STR
    if ((eno == n_str || eno == n_strpl) && isprint(c)) {
      push(BST,tovalue(c) | toedit6(eno) | LSTR0);
      c = *(++(*pts));
     }                                                                  else
#endif
    if (isalpha(c) || c == EXTRENN) c = getletstr(eno,pts,LSTR0);       else
    if (isdigit(c) || c == NEGSIGN) {
       switch(eno) {
          case n_nmatels : c = getreal(eno,pts);
                           break;
          default        : c = getreal(eno,pts);
       }
     }
    else {
      push(BST,PLACEH | toedit6(eno));
    }
   }
  else {         /* eingabe zuende ! */
    push(BST,PLACEH | toedit6(eno));
  }
  if (c < 0) return(c);
  else {
    if (!isstr(etptb[eno].exptp))
       *pts = skipblanks(*pts);
    return(*(*pts));
  }
 }
/* ---------- getlst -------------------------------------------- */
getlst(eno,pts,plist)
int eno,plist;
char *(*pts);
{
 char *skipblanks();
 int c;

 if (!isstr(etptb[eno].exptp))
    *pts = skipblanks(*pts);
 if (ismatel(eno))
    c = getmatels(eno,pts); else
 if (ismatrl(eno))
   c = getmatrows(eno,pts);
 else {
#if B_DEBUG
   if (debug > 5) {
     fprintf(stderr,"getlst: eno %o, ein: %s",eno,*pts);
     waitkey();
   }
#endif
#if N_STR
   if (eno == n_str  ||  eno == n_strpl) {
     while (isprint(c = getexp(eno,pts,plist)) && c != STREND) {
#if B_DEBUG
      if (debug > 5) {
        fprintf(stderr,"in while: eno %o, ein: %s",eno,*pts);
        waitkey();
      }
#endif
       incrarity(MST);
     }
/*     if (c == STREND) ++(*pts);	*/
   }  else
#endif
   while ((c = getexp(eno,pts,plist)) == ',') {
#if B_DEBUG
    if (debug > 5) {
      fprintf(stderr,"in while: eno %o, ein: %s",eno,*pts);
      waitkey();
    }
#endif
    incrarity(MST);
    ++(*pts);
   }
 }
 if (c <= 0)
    return(c);
 if (!isstr(etptb[eno].exptp))
    *pts = skipblanks(*pts);
 return(*(*pts));
}
/* end of       getexp.c */

