/* screen display for editor               cdispl.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"
#include "cetb.h"
#include "ckwtb.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define FUENF 5		/* for ldispl	*/
#define VIER  4		/* for ldispl	*/
#define DREI  3		/* for ldispl	*/
#define ZWEI  2

extern int dmode,small;

static int in_decl;

void displ();

#define ASSERT(c)	if (!(c)) { error("c"); } else
#define CHKST(st)	ASSERT(((st)->cnt >= 0L) && ((st)->cnt <= (st)->siz))

/* ---------- labbrev ------------------------------------------------	*/
static labbrev(eno,n)				/* abbreviate lists	*/
 register int eno,n;
 {
  push(AADRST,tovalue(srow * co + scol) | toclass(LABBRSMB));
  fputc(LABBRSMB,stdout);
  scol++;
  push(M1ST,pop(M1ST) - toarity(n));		/* decrement arity by n	*/
  push(EST,noedit(pop(EST)) | DSPLD_FLAG | ABBREV_FLAG | toedit6(eno));
  edtransp(EST,AST,0);				/* expression to a		*/
  push(AST,noedit(pop(AST)) | DSPLD_FLAG | ABBREV_FLAG | toedit6(eno));
  for (n -= 2 ; n > 0 ; --n) {
    push(EST,pop(EST) & ~ (DSPLD_FLAG | ABBREV_FLAG));
    edtransp(EST,AST,0);
    push(AST,pop(AST) & ~ (DSPLD_FLAG | ABBREV_FLAG));
  }
  push(EST,noedit(pop(EST)) | DSPLD_FLAG | ABBREV_FLAG | toedit6(eno));
  edtransp(EST,AST,0);
  push(AST,noedit(pop(AST)) | DSPLD_FLAG | ABBREV_FLAG | toedit6(eno));
  fputc(LABBRSMB,stdout);
  push(AADRST,tovalue(srow * co + scol) | toclass(LABBRSMB));
  scol++;
 }
/* ---------- ldispl -------------------------------------------------	*/
/* display a list							*/
static ldispl(eno,ecol,maxcol,c,plist,mode)
 int eno,		/* edit-info		*/
     ecol,		/* start column		*/
     maxcol,		/* maximum column	*/
     c,			/* display mode		*/
     plist,		/* list of parameters ?	*/
     mode;		/* depth of display	*/
 {
  register int n;

  for (n = (int)arity(topof(M1ST)) - rembit(c) ; n > 0 ; --n) {
    if (n > 1)
      if (vertbit(c)) {		/* vertical ? (indepent of isconstr !)	*/
        if (freerows) {		/* is there a free row ?	*/
            --freerows;
            if ((class(topof(EST)) != AR0(FDEC)) 
                && (class(topof(EST)) != AR0(DECL_CR))) {
              displ(eno,scol,maxcol - ZWEI,0,mode);
              printstr(",");
              CURSORXY(++srow,scol = ecol);
             }
            else {
              displ(eno,scol,maxcol,0,mode);
              CURSORXY(++srow,scol = ecol);
            }
         }
        else {			/* not enough place	*/
          labbrev(eno,n);	/* abbreviate list	*/
          break;		/* stop loop		*/
        }
       }
      else	/* horizontal	*/
        if ( maxcol - scol >  FUENF-IS_SMALL(small)) {	/* enough place ?	*/
          displ(eno,scol,maxcol - FUENF + IS_SMALL(small),0,mode);
          if (!strbit(c)) if (IS_SMALL(small)) printstr(", ");
                          else       printstr(" , ");
         }
        else {		/* horizontal not enough place	*/
          if (freerows > 0  && (plist &&  ecol != scol ||
                                maxcol-ecol > FUENF-IS_SMALL(small))
              && (!IS_SMALL(small) || !strbit(c)) ) {
              CURSORXY(++srow,scol = ecol);
              --freerows;
              displ(eno,scol,maxcol - FUENF + IS_SMALL(small),0,mode);
              if (!strbit(c)) if (IS_SMALL(small)) printstr(", ");
                              else       printstr(" , ");
           }
          else {		/* not enough room	*/
            labbrev(eno,n);	/* abbreviate list	*/
            break;		/* stop loop		*/
          }
        }
    else {		/* n = 1	*/
      if (plist  &&  maxcol-scol < 2  &&  freerows  &&  ecol != scol
          && !strbit(c)) {
          CURSORXY(++srow,scol = ecol);
          --freerows;
          displ(eno,scol,maxcol,0,mode);
       }
      else displ(eno,scol,maxcol,0,mode);
    }
   }	/* end for	*/
 }
/* ---------- abbrev -------------------------------------------------	*/
static abbrev()		/* abbreviate expression by abbrsymb		*/
 {
  push(EST,pop(EST) | DSPLD_FLAG | ABBREV_FLAG);	/* set dspld, abbrev	*/
  push(AADRST,pop(AADRST) | toclass(ABBRSMB));		/* abbrsymb for disp	*/
  fputc(ABBRSMB,stdout);				/* output abbrsymb	*/
  scol++;						/* increment column	*/
  edtransp(EST,AST,0);					/* transport expression	*/
  push(AST,pop(AST) | DSPLD_FLAG | ABBREV_FLAG);	/* set dspld, abbrev	*/
 }

/* ---------- displ --------------------------------------------------	*/
void displ(eno,ecol,maxcol,nabbr,mode)
 int eno,ecol,maxcol,nabbr,mode;
 {
  register char *s;
  register STACKELEM m;
  int i,e,c,n,plist,old_decl,ab;

/*
fprintf(stderr,"displ: ecol %d maxcol %d nabbr %d mode %x\n",ecol,maxcol,nabbr,mode);
fprintf(stderr," : srow %d scol %d freerows %d \n",srow,scol,freerows);
*/

  push(AADRST,tovalue((srow * co) + scol));			/* address	*/
  push(EST,noedit(pop(EST)) | DSPLD_FLAG | toedit6(eno));
  decrarity(M1ST);					/* decrement arity	*/
  if (isconstr(m = topof(EST))) {			/* constr. ?		*/
    plist = class(m) == AR0(CALL);			/* parameter list ?	*/
    old_decl = in_decl;
    in_decl |= class(m) == AR0(DECL);
    if ((n = encsearch(eno,m,kwtb,kwtblen)) != FAILURE) {	/* exists ?	*/
      if (IS_DVERT(small)  &&  is_vert(kwtb[n].kwtbinfo)) n++;
      if (IS_DVERT1(small)  &&  is_vert1(kwtb[n].kwtbinfo)) n++;
      if (IS_DVERT2(small)  &&  is_vert2(kwtb[n].kwtbinfo)) n++;
      if (mode) {
        if (class(m) == AR0(DECL) && mode != dmode) {
          if (old_decl && IS_DTDECL(small)) {
            e = !((freerows >= kwtb[n].rows || !IS_DGOAL(small) && !IS_DFCT(small))
                  && maxcol - scol >= kwtb[n].cols);
           }	else
          if (IS_DDECL(small)) {
            e = !((freerows >= kwtb[n].rows || !IS_DGOAL(small) && !IS_DFCT(small))
                  && maxcol - scol >= kwtb[n].cols);
           }
          else e = TRUE;
          if (!e) {
            freerows -= kwtb[n].rows;		/* enough room: decrement rows	*/
/*
fprintf(stderr,"-> freerows %d\n",freerows);
*/
            move2(EST,MST,M1ST);			/* constr. to M,M1	*/
            s = IS_DFCT(small) ? "def " : "def*";	/* display string	*/
            c = isprint(*s) ? *s : UNPR;		/* new			*/
            push(AADRST,pop(AADRST) | toclass(c));	/* cursor toggle	*/
            while ((c = *s++) != '\0') {		/* output s		*/
              fputc(c,stdout);
              scol++;
            }
            /* c = TRUE; ????	*/
            s = kwtb[n].dsp;			/* display string	*/
            while (*s++ != ESC)
              ;
            e = *s++;		/* eno of subexpression	*/
            c = *s++;		/* type of expression			*/
            n = ((limbit(c) ? maxcol : co-ZWEI) - (int)*s++);
            ab = TRUE;
            for (i = arity(m) - 1 ; i > 0 ; --i) {
              if (IS_DFCT(small)) {
                if (nabbr > 0) {			/* # of abbrev.	*/
                  if (nabbr > 1) labbrev(e,nabbr);	/* abbrev to ..	*/
                  else displ(e,scol,scol,0,1);	/* or abbr to ?	*/
                  nabbr = 0;			/* set # abbrev to 0	*/
                  if (arity(topof(M1ST)) > (int)rembit(c)) {
                    if (IS_SMALL(small)) printstr(", ");
                    else       printstr(" , ");
                    ldispl(e,scol,n,c,plist,1);
                  }
                 }		/* nabbr = 0	*/
                else ldispl(e,scol,n,c & ~VERTBIT,1,(mode < 0) ? 1 : MIN(1,mode));
                ab = FALSE;
               }
              else {
                push(EST,pop(EST) & ~(DSPLD_FLAG | ABBREV_FLAG));
                edtransp(EST,AST,0);
                push(AST,pop(AST) & ~(DSPLD_FLAG | ABBREV_FLAG));
                decrarity(M1ST);
              }
            }
            if (!IS_DGOAL(small)) {
              push(EST,pop(EST) & ~ (DSPLD_FLAG | ABBREV_FLAG));
              edtransp(EST,AST,0);
              push(AST,pop(AST) & ~ (DSPLD_FLAG | ABBREV_FLAG));
             }
            else {
              ab = FALSE;
              while (*s++ != '\n')
                ;
              CURSORXY(++srow,scol=ecol);	/* new row	*/
              /* --freerows;	*/
              while (*s++ != ESC)
                ;
              e = *s++;		/* eno of subexpression	*/
              c = *s++;		/* type of expression			*/
              n = ((limbit(c) ? maxcol : co-ZWEI) - (int)*s++);
              if (nabbr > 0) {
                displ(e,scol,scol,0,mode-1);
                nabbr--;		/* decrement nabbr	*/
               }
              else {
                displ(e,scol,n,0,mode-1);
              }
            }
            if (ab) push(AST,pop(MST) | ABBREV_FLAG | DSPLD_FLAG);
            else   push(AST,pop(MST) | DSPLD_FLAG);
            pop(M1ST);		/*  remove from m1		*/
            e = FALSE;
          }
         }
        else e = TRUE;
        if (e) {
          if (freerows >= kwtb[n].rows && maxcol - scol >= kwtb[n].cols) {
            freerows -= kwtb[n].rows;		/* enough room: decrement rows	*/
            move2(EST,MST,M1ST);			/* constr. to M,M1	*/
            s = kwtb[n].dsp;			/* display string	*/
            c = isprint(*s) ? *s : UNPR;		/* new			*/
            push(AADRST,pop(AADRST) | toclass(c));	/* cursor toggle	*/
            while ((c = *s++) != '\0') {		/* output s	*/
              if (isprint(c)) {
                fputc(c,stdout);
                scol++;
               }
              else
                if (c == '\n') CURSORXY(++srow,scol=ecol);	/* new row	*/
                else
                  if (c == ESC) {			/* subexpression	*/
                    e = *s++;		/* eno of subexpression	*/
                    c = *s++;		/* type of expression			*/
                    n = ((limbit(c) ? maxcol : co-ZWEI) - (int)*s++);
                    if (listbit(c)) {			/* list ?	*/
                      if (nabbr > 0) {			/* # abbrev.	*/
                        if (nabbr > 1) labbrev(e,nabbr);	/* abbrev to ..	*/
                        else displ(e,scol,scol,0,mode-1);	/* or abbr to ?	*/
                        nabbr = 0;			/* set # abbrev to 0	*/
                        if (arity(topof(M1ST)) > (int)rembit(c)) {
    			/* rembit: 0 or 1 for lists	*/
                          if (IS_SMALL(small)) printstr(", ");
                          else       printstr(" , ");
                          ldispl(e,scol,n,c,plist,mode-1);
                        }
                       }		/* nabbr = 0	*/
                      else ldispl(e,scol,n,c,plist,mode-1);
                     }
                    else {		/* no list	*/
                      if (nabbr > 0) {
                        displ(e,scol,scol,0,mode-1);
                        nabbr--;
                       }
                      else {
                        if (e == n_fcn  ||  IS_DECN(e))
                          displ(e,scol,n,0,mode);
                        else
                          displ(e,scol,n,0,mode-1);
                      }
                    }
              }
            }
            move(MST,AST);		/* construktor from m to a	*/
            pop(M1ST);			/*  remove from m1		*/
           }
          else abbrev();		/* not enough room	*/
         }
       }
      else abbrev();		/* not enough room	*/
     }
    else {		/* encsearch = failure : unknown constr	*/
      fputc(UNKN_C,stdout);
      scol++;
      edtransp(EST,AST,0);		/* tranport expression	*/
    }
    in_decl = old_decl;
   }
  else		/* atom	*/
    if (single_atom(m)) {
      if ((n = encsearch(eno,m,kwtb,kwtblen)) != FAILURE) {
        if (IS_DVERT(small)  &&  is_vert(kwtb[n].kwtbinfo)) n++;
        if (IS_DVERT1(small)  &&  is_vert1(kwtb[n].kwtbinfo)) n++;
        if (IS_DVERT2(small)  &&  is_vert2(kwtb[n].kwtbinfo)) n++;
        if (maxcol - scol >= kwtb[n].cols	/* enough room ?	*/
            &&  (mode  ||   kwtb[n].cols == 0) ) {
          s = kwtb[n].dsp;			/* displ.string		*/
          if (*s == NO_PARC) {
            push(AADRST,pop(AADRST) | toclass(' '));	/* cursor toggle	*/
            fputc(' ',stdout);
            scol++;
           }
          else {
            push(AADRST,pop(AADRST) | toclass(*s));	/* cursor toggle	*/
            while ((c = *s++) != '\0')
              if (isprint(c)) {
                fputc(c,stdout);
                scol++;
              }
          }
          move(EST,AST);	/* single_atom -> a	*/
         }
        else abbrev();	/* not enough room	*/
       }
      else {	/* unknown atom	*/
        fputc(UNKN_A,stdout);
        scol++;
        move(EST,AST);
      }
     }
    else {		/* multi_atom	*/
      n = travmastr(EST,AST,lbuf,co-1); 
      if (n <= maxcol-scol+1	/* 26.02.88	*/
          &&  (mode  ||  n <= 1) ) {
        push(AST,noedit(pop(AST)) | DSPLD_FLAG | toedit6(eno));
        printstr(lbuf);					/* from travmastr	*/
        push(AADRST,pop(AADRST) | toclass(*lbuf));	/* cursor toggle	*/
       }
      else {		/* not enough room	*/
        push(AST,noedit(pop(AST))|DSPLD_FLAG|ABBREV_FLAG|toedit6(eno));
        edtransp(AST,EST,0);			/* transport expression, set flags	*/
        push(EST,noedit(pop(EST))|DSPLD_FLAG|ABBREV_FLAG|toedit6(eno));
        transport(EST,AST);			/* and again to a	*/
        fputc(ABBRSMB,stdout);			/* abbreviate		*/
        scol++;
        push(AADRST,pop(AADRST) | toclass(ABBRSMB));	/* cursor toggle	*/
      }
    }
 }	/* end of displ	*/

/* ---------- dispma -------------------------------------------------	*/
/* display a multi-atom							*/
static dispma(eno)
 int eno;
 {
  register STACKELEM m;
  int c,cnt;

  m = topof(EST);
  if (isatom(m) && multi_atom(m)) {		/* multi_atom ?	*/
    push(AADRST,tovalue(srow * co + scol) | toclass(value(m)));
    push(AST,(pop(EST)& ~(EDIT_FIELD|MARK_FLAG))|DSPLD_FLAG|toedit6(eno));
    c = (int)value(m);
    fputc(isprint(c) ? c : UNPR_C,stdout);
    cnt = (li - FAROW) * co - 4;
    if (marked1(m)) {
       do {
          if (--cnt > 0) {
             m = move(EST,AST);
             c = (int)value(m);
             fputc(isprint(c) ? c : UNPR_C,stdout);
           }
          else {
             putstr(" ->");
             dspstr(MROW,MCOL,"Display next Page? (y/n) ");
             fflush(stdout);
             if (getkey() == 'y') {
                clrmsg(MROW,MCOL);
                cleartoeos();
                putstr("-> ");
                cnt = (li - FAROW) * co - 8;
              }
             else {
                clrmsg(MROW,MCOL);
                while (marked1(m))
                   m = move(EST,AST);
             }
           }
        } while (marked1(m));
       push(AST,(pop(AST)& ~EDIT_FIELD)|DSPLD_FLAG|MARK_FLAG|toedit6(eno));
     }
    decrarity(M1ST);
  }
}
/* status informations for display and fdisplay	*/
static int fdispld = NO;
/* ----- display -----------------------------------------------------	*/
/* top expression becomes focus of attention				*/
display(disp)
 int disp;
 {
  register STACKELEM m;
  register int eno;

  in_decl = FALSE;
  if (disp  &&  (m = topof(M1ST)) != STBOTTOM  &&
      notdisp(m)  &&  arity(m) == arity(topof(MST))) {
     scrlup();
  }
  CURSORXY(srow = FAROW,scol = FACOL);		/* set cursor	*/
  cleartoeos();					/* clear to end of screen	*/
  freerows = li - FAROW - 1;			/* # of free rows	*/
  fdispld  = NO;				/* not fdispld	*/
  clearstack(EADRST);				/* init address stacks	*/
  clearstack(AADRST);
  if ((m = pop(M1ST)) != STBOTTOM) {		/* subexpression ?	*/
    push(M1ST, m & ~(DSPLD_FLAG | ABBREV_FLAG));	/* yes: clear dspld,	*/
    push(MST,pop(MST) & ~(DSPLD_FLAG | ABBREV_FLAG));	/* abbrev-flag	*/
  }
  m   = topof(EST);
  eno = (int)edit6(m);
  if (isatom(m) && multi_atom(m))
    dispma(eno);				/* display multi atom	*/
  else
    displ(eno,scol,co-ZWEI,0,dmode);		/* no multi_atom	*/
  scrollback();					/* goto correct place	*/
  fflush(stdout);
 }
/* ----- fdisplay -----------------------------------------------------	*/
/* the father expression of the actual cursor expression becomes	*/
/* focus of attention. cursor expression stays the same			*/
/* used by: main,dispfa	*/
fdisplay(disp)
 int disp;
 {
  register STACKELEM m;
  register int eno;

  in_decl = FALSE;
  if (disp  &&  (m = topof(M1ST)) != STBOTTOM  &&
      notdisp(m)  &&  arity(m) == arity(topof(MST))) {
     scrlup();
  }
  if ((m = topof(M1ST)) != STBOTTOM) {		/* in subexpression ?		*/
    CURSORXY(srow = FAROW, scol = FACOL);	/* set cursor			*/
    cleartoeos();				/* clear to end of screen	*/
    freerows = li - FAROW - 1;			/* # of free rows		*/
    fdispld  = YES;				/* fdispld			*/
    scrlup();					/* up one expression		*/
    if (topof(M2ST) == STBOTTOM)  push(M2ST,m);	/* save m ?			*/
    eno = (int)edit6(topof(EST));
    clearstack(EADRST);				/* init address stacks		*/
    clearstack(AADRST);
    if (topof(M1ST) != STBOTTOM) {
      push(M1ST,pop(M1ST) & ~(DSPLD_FLAG | ABBREV_FLAG));
      push(MST ,pop(MST ) & ~(DSPLD_FLAG | ABBREV_FLAG));
    }
    displ(eno,scol,co-ZWEI,(int)(arity(topof(EST))-arity(m)),dmode);
    scrollback();				/* back to correct place	*/
    fflush(stdout);
   }
  else display(disp);				/* not in subexpression	*/
 }
/* ------------- dispfa ----------------------------------------------	*/
/* display focus of attention						*/
/* used by: dsplstack,getcmd,input,delexp,putexpr,infile		*/
dispfa()
 {
  register STACKELEM m;
  register int rechts;

  if (fdispld &&  isdispld(topof(MST))) {
    if ((m = pop(EST)) != STBOTTOM)
      push(EST, m | DSPLD_FLAG);
    topb();
    do {
      rechts = scrlright();			/* scroll right	*/
      /* scrolled right or started again ?	*/
    } while ((!isdispld(m=topof(EST)) || isabbrev(m)) &&
             rechts);
    fdisplay(FALSE);
   }
  else {					/* not fdispld	*/
    topb();
    display(FALSE);
  }
 }
/* end of	cdispl.c	*/

