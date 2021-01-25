/*                                       cfdispl.c   */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:  03.06.88                       */

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
#include "cfiles.h"

#define FUENF 5  /* laenge eines abgekuerzten ausdrucks       */

static int fcol,abbrno,fcols,print;
extern int small;

void fdispl();

/* ----------- flabbrev ---------------------------------------------- */
/* Kuerzt eine Liste von Ausdruecken ab.                               */
static void flabbrev(n,fp)
register long n;                                        /* n >= 2 !!      */
FILE *fp;
{

 putc(LABBRSMB,fp);
 fcol++;
 pushi(DSTR0 | tovalue(abbrno),BST);
 pushi(AR0(LIST) | toarity(n + 1L),MST);
 for ( ; n > 0L ; --n) {
    transp2(EST,AST,BST);
 }
 move(MST,BST);
 putc(LABBRSMB,fp);
 fcol++;
 fprintf(fp,"_%03d",abbrno++);
 fcol += 4;
}

/* ----------- fldispl ----------------------------------------------- */
/* Schreibt eine Liste von Ausdruecken auf einen File.                 */
static fldispl(eno,ecol,maxcol,c,fp)
int eno,ecol,maxcol,c;
FILE *fp;
{
 register long n;

 for (n = (long)arity(topof(M1ST)) - (long)rembit(c) ; n > 0L ; --n) {
    if (n > 1L) {
/*
       if (vertbit(c)  && isconstr(topof(EST))) {
*/
	/* auf wunsch von harald, 06.05.91	*/
       if (IS_DVERT(small) || vertbit(c)) {
          fdispl(eno,fcol,maxcol,fp);
          if (!strbit(c)) {
            if (IS_SMALL(small)) fputs(",\n",fp);
            else fputs(" ,\n",fp);
            for (fcol = 0 ; fcol < ecol ; fcol++) {
              putc(' ',fp);
            }
          }
        }
       else {
          if ((maxcol - fcol) >= FUENF  ||  print) {
             fdispl(eno,fcol,maxcol - FUENF,fp);
             if (!strbit(c)) {
               if (IS_SMALL(small)) fputs(", ",fp);
               else fputs(" , ",fp);
               fcol += 3;
             }
           }
          else {
             flabbrev(n,fp);
             break;
          }
       }
     }
    else fdispl(eno,fcol,maxcol,fp);
  }
}

/* ----------- fabbrev ----------------------------------------------- */
/*  Kuerzt einen Ausdruck ab.                                          */
static void fabbrev(fp)
FILE *fp;
{
 putc(ABBRSMB,fp);                                      /* abbreviate     */
 fcol++;
 pushi(DSTR0 | tovalue(abbrno),BST);
 transp2(EST,AST,BST);
 pushi(AR0(LIST) | toarity(2),BST);
 fprintf(fp,"_%03d",abbrno++);
 fcol += 4;
}
/* ----------- fdispl ------------------------------------------------ */
/* schreibt den obersten Ausdruck von Stack E auf file fp, dabei wird  */
/* der Ausdruck nach Stack A traversiert.                              */
void fdispl(eno,ecol,maxcol,fp)
int eno,ecol,maxcol;
FILE *fp;
{
 long exprsize();
 register char *s;
 register STACKELEM m;
 int e,c,n;

 decrarity(M1ST);
 if (isconstr((m = topof(EST)))) {
    if ((n = encsearch(eno,m,kwtb,kwtblen)) != FAILURE) {
       if (print  &&  is_print(kwtb[n].kwtbinfo)) n++;
       if (IS_DVERT(small)  &&  is_vert(kwtb[n].kwtbinfo)) n++;
       if (IS_DVERT1(small)  &&  is_vert1(kwtb[n].kwtbinfo)) n++;
       if (IS_DVERT2(small)  &&  is_vert2(kwtb[n].kwtbinfo)) n++;
       if ((maxcol - fcol - FUENF) >= kwtb[n].cols  ||  print) {
          move2(EST,MST,M1ST);
          s = kwtb[n].dsp;                              /* display string */
          while ((c = *s++) != '\0') {                  /* displaying     */
             if (isprint(c)) {
                putc(c,fp);
                fcol++;
              }
             else {
                if (c == '\n') {                        /* next line      */
                   putc(c,fp);
                   for (fcol = 0 ; fcol < ecol ; fcol++)
                       putc(' ',fp);
                 }
                else {
                   if (c == ESC) {                      /* subexpression  */
                      e = *s++;
                      c = *s++;
                      n = ((limbit(c) ? maxcol : fcols - 2) - (int)*s++);
                      if (listbit(c)) {
                         fldispl(e,fcol,n,c,fp);
                       }
                      else {                             /* no list */
                         fdispl(e,fcol,n,fp);
                      }
                   }
                }
             }
          }                                      /* end while            */
          move(MST,AST);
          pop(M1ST);
        }                                         /* end constuctor       */
       else fabbrev(fp);
     }
    else {
       putc(UNKN_C,fp);                           /* unknown constr       */
       fcol++;
       transport(EST,AST);
     }
  }
 else {                                          /* atom */
    if (single_atom(m)) {
       if ((n = encsearch(eno,m,kwtb,kwtblen)) != FAILURE) {
          if (print  &&  is_print(kwtb[n].kwtbinfo)) n++;
          if (IS_DVERT(small)  &&  is_vert(kwtb[n].kwtbinfo)) n++;
          if (IS_DVERT1(small)  &&  is_vert1(kwtb[n].kwtbinfo)) n++;
          if (IS_DVERT2(small)  &&  is_vert2(kwtb[n].kwtbinfo)) n++;
          if ((maxcol - fcol - FUENF) >= kwtb[n].cols  ||  print ||
              kwtb[n].cols <= FUENF) {
             s = kwtb[n].dsp;
             if (*s == NO_PARC) {
               putc(' ',fp);
               fcol++;
              }
             else {
               while ((c = *s++) != '\0') {
                if (isprint(c)) {
                   putc(c,fp);
                   fcol++;
                }
              }                                         /* end while      */
             }
             move(EST,AST);
           }
          else fabbrev(fp);
        }
       else {
          putc(UNKN_A,fp);                              /* unknown atom   */
          fcol++;
          move(EST,AST);
       }
     }                                                  /* end singleatom */
    else {                                              /* multi_atom     */
       if ((n = exprsize(EST)) <= (long)(maxcol - fcol + 1 - FUENF)
           || n <= FUENF || fcol == 0 || print) {
          move(EST,AST);
          putc((char)value(m),fp);
          fcol++;
          if (marked1(m)) {
             pushi(pop(AST) & ~MARK_FLAG,AST);
             do {
                m = move(EST,AST);
                putc((char)value(m),fp);
                if (fcol++ >= fcols  &&  !print) {
                   putc('\n',fp);
                   fcol = 0;
                }
             } while (marked1(m));
             pushi(pop(AST) | MARK_FLAG,AST);
          }
        }
       else {
          fabbrev(fp);
       }
     }  /* end multi_atom */
  }  /* end atom */
}

/* ----------- prettyprint ------------------------------------------- */
prettyprint(fname,mode,width,art,type)
char *fname,*mode;                               /* mode: "w" or "a" */
int width,art,type;
{
 FILE *fp,*fopen();
 STACKELEM m;
 int n,eno,p,erg;
 extern char msg[];

 erg = SUCCESS;
 print = PP_ASC(art);
 if (*fname != '\0') {
   p = strlen(editext[FN_ED]);     /* vorsicht hier !         */
   n = strlen(fname)-p;            /* loesche ev. editext[FN_ED]       */
   if (strcmp(fname+n,editext[FN_ED]) == SUCCESS) {
     fname[n] = '\0';
   }
 }
 chkfilname(fname,type);
 fcols = width;
 if (*mode == 'w') {
   if ((fp = fopen(editfil[type],"r")) != NULLFILE) {
     fclose(fp);
     IF_NOEXTERNALCMD { if (!PP_REPLACE(art)) {
        sprintf(msg,"Replace %s ? (y/n) :",editfil[type]);
        dspstr(MROW,MCOL,msg);
        if (!batch) n = getkey();
        else        n = 0;
       }
      else n = 'y';
      if (n != 'y') {
        clrmsg(MROW,MCOL);
        return(FAILURE);
      }
     }
   }
 }
 if ((fp = fopen(editfil[type],mode)) != NULLFILE) {
    abbrno = 1;
    clrmsg(MROW,MCOL);
    IF_NOEXTERNALCMD {
     sprintf(msg,"%s %s",(*mode == 'w') ? "Writing" : "Appending to",
                editfil[type]);
     dspstr(MROW,MCOL,msg);
    }
    clearstack(BST);
    if (PP_LINE(art))
      fputs("---------------------------------------------------------\n\n",fp);
    if ((m = topof(M1ST)) != STBOTTOM  &&  notdisp(m)  &&
        arity(m) == arity(topof(MST))) {
      move(MST,EST);
      move(M1ST,M2ST);
      p = 1;
     }
    else
      p = 0;
    eno = (int)edit6(topof(EST));
    fdispl(eno,fcol=0,fcols - 2,fp);
    transport(AST,EST);
    incrarity(M1ST);

    while (topof(BST) != STBOTTOM) {
       n = 0;
       while (topof(BST) != STBOTTOM) {
          transport(BST,EST);
          n++;
       }
       for ( ; n > 0 ; --n) {
          if ((int)arity(move2(EST,MST,M1ST)) == 2) {
             fprintf(fp,"\n\n\n?_%03d:\n\n",(int)value(move(EST,AST)));
             decrarity(M1ST);
             fdispl(eno = (int)edit6(topof(EST)),fcol = 0,fcols - 2,fp);
           }
          else {
             fprintf(fp,"\n\n\n.._%03d:\n\n",(int)value(move(EST,AST)));
             decrarity(M1ST);
             fldispl(eno = (int)edit6(topof(EST)),fcol = 0,fcols - 2,0,fp);
          }
          move(MST,AST);
          pop(M1ST);
          delete(AST);
        }
     } /* end while */

    fputs("\n\n",fp);
    fclose(fp);

    while (p-- > 0) {
      move(EST,MST);
      move(M2ST,M1ST);
    }
    IF_NOEXTERNALCMD {
     sprintf(msg,"%s Prettyprint to %s",
      (*mode == 'w') ? "Wrote" : "Appended",editfil[type]);
     dspstr(MROW,MCOL,msg);
    }
  }
 else {
   dspstr2(MROW,MCOL,"Can't open ",editfil[type]);
   erg = FAILURE;
 }

 if (PP_DISPLAY(art) && !batch) {
   setcursor();
   IF_NOEXTERNALCMD {
    getunget();
    clrmsg(MROW,MCOL);
   }
 }
 return(erg);
}
/* end of  cfdispl.c  */


