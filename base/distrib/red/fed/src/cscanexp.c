/*                                      scanexp.c        */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    22.04.88                        */

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

#if B_DEBUG
#define outdeb(c) { char out = (c); if (out == '\33') fputs("\\33",stderr); \
                    else if (out == '\n') fputs("\\n",stderr);              \
                    else if (iscntrl(out)) fprintf(stderr,"%x ",out);       \
                    else fputc(out,stderr); }
#endif

/* ---------- scanexp ------------------------------------------------ */
/* scannt einen ausdruck gemaess kwtb[]                                */
/* use: getexp, getlst, scanexp, scanlst                               */
scanexp(tstr,pts,n,eno)
 char *tstr,*(*pts);
 int n,eno;
 {
  char *skipblanks();
  STACKELEM callkon();
  static STACKELEM m;
  static int c,skip;
  register int fail,first,is_str;

  c    = 0;
  fail = FALSE;
  m    = kwtb[n].elem;
  if (isconstr(m)) {
    push(MST,m | toedit6(eno));
    push(M1ST,m);
    first = TRUE;
    while (*tstr != '\0' && c >= 0) {
      is_str = isstr(etptb[eno].exptp) || (m == AR0(KQUOTE) && first);
#if B_DEBUG
      if (debug > 5) {
          fprintf(stderr,"cutword: pts %s isstr %d tstr ",*pts,is_str);
          for (skip = 0; *(tstr+skip) ; skip++ )
            if ((*(tstr+skip)) == '\33') {
              fprintf(stderr,"\\%o",*(tstr+skip));
              fprintf(stderr,"\\%o",*(tstr +  ++skip));
              fprintf(stderr,"\\%o",*(tstr +  ++skip));
              fprintf(stderr,"\\%d ",*(tstr +  ++skip));
             }   else
            outdeb(*(tstr+skip));
          waitkey();
      }
#endif
      skip = cutword(&tstr,pts,is_str);
      if (skip == FAILURE)      /* notwendiges teilword ueberlesen ?    */
         fail |= (noedit(topof(BST)) != PLACEH);
        /* neu: |= statt = ! [ganz alt: fail = TRUE]     */
#if B_DEBUG
      if (debug > 5) {
          fprintf(stderr,"scan: skip %d fail %d pts %s *tstr ",skip,fail,*pts);
          outdeb(*tstr);
          fputc(' ',stderr);
          waitkey();
      }
#endif
      if (*tstr == '\33') {
        tstr++;
        eno = *tstr++;
        c   = *tstr++;
	/* vorsicht: dirty trick wg. US_KON : keine predef-functions !	*/
        if (m == AR0(US_KON) && eno == n_fcn) eno = n_var;
        tstr++;
        if (contbit(c)) {
          if (listbit(c)) {
            c = scanlst(kwtb[n + 1].dsp,pts,n + 1,eno);
           }
          else {
            c = scanexp(kwtb[n + 1].dsp,pts,n + 1,eno);
          }
         }
        else {   /* nicht continued         */
          if (listbit(c)) {
            c = getlst(eno,pts,plbit(c) ? eno : 0);
           }
          else {
            c = getexp(eno,pts,0);
          }
        }
        incrarity(MST);
      }
      first = FALSE;
     } /* end while */
     move(MST,BST);
     pop(M1ST);
    }
   else { /* single_atom */
     push(BST,m | toedit6(eno));
   }
#if B_DEBUG
   if (debug > 2) {
     int i;
     if (fail) i = FAILURE;           else
     if (c < 0) i = c;
     else {
       char *t;
       if (!isstr(etptb[eno].exptp)) t = skipblanks(*pts);
       else t = *pts;
       i = (int)*t;
     }
     if (i > 0) fprintf(stderr,"scanerg: %c ",(char)i);
     else       fprintf(stderr,"scanerg: %d ",i);
   }
#endif
   if (fail) return(FAILURE);           else
   if (c < 0) return(c);
   else {
     if (!isstr(etptb[eno].exptp))
       *pts = skipblanks(*pts);
     return(*(*pts));
   }
 }
/* ---------- scanlst ------------------------------------------------ */
scanlst(tstr,pts,n,eno)
 char *tstr,*(*pts);
 int n,eno;
 {
  register int c;

  if (ismatrl(eno)) {
    if ((c = scanexp(tstr,pts,n,eno)) == ',') {
      incrarity(MST);
      ++(*pts);
      c = getmatrows(eno,pts);
    }
   }
  else {
    while ((c = scanexp(tstr,pts,n,eno)) == ',') {
      incrarity(MST);
      ++(*pts);
    }
  }
  return(c);
 }
/* end of               scanexp.c */
