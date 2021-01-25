/* eingabe fuer edit                           cinput.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    10.06.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cestack.h"
#include "cscred.h"
#include "ctermio.h"
#define isback  1 /* fuer back.h  */
#include "cback.h"
#include "ckwtb.h"

#define EOPARSE '\077'	/* ende parse-string	*/
#define NONE (-2)         /* kein ergebnis (nicht FAILURE, FALSE, TRUE) */

#define iscall(e)         ((e) == n_call  ||  (e)  == n_uskon)
#define IS_VAR(x)         (is_letter(x)  &&  value(x) != STREND)

extern void cfree();
extern int Changes,placeh;
extern char *sct[],*sct0[];

int EXT_INPUT_MODE=0; /* UDI : signs if input fct is called by ext fct*/
static char INPUT_ERR_STR[256]; /* UDI : Contains error message while parsing */

/* ------------- ext_input ---------------------------------------- */
/* Frontend fuer die input parse routine, unterdrueckt Meldung      */
/* und verlaesst die Routine bei einem Fehler                       */
ext_input(buf,buflen,btr,stack)
 char *buf;
 int buflen,btr;
 STACK *stack;
 {
  int okay; /* Error */

   EXT_INPUT_MODE=1; /* Inputmodus Flagge auf Wahr setzen */

   clearstack(stack);

   okay=input(buf,buflen,btr);

   if (okay==FALSE) { /* Error in parsing string ? */
    setcursor();
    dspstr(MROW,MCOL,INPUT_ERR_STR); /* Disply error message */ 
    okay=0;
   } else { 
    duplicate(EST,stack); /* Copy stack to external stack */
    okay=1; /* Expression parsed an stored at stack */
    putexpr(SST); /* Insert old stack again */
   }

   EXT_INPUT_MODE=0; /* Normaleinstellung, kein externes Kommando */

  return (okay);
 }

/* ------------- input -------------------------------------------- */
/* eingabe-behandlung                                               */
/* BST als Eingabestack, U1ST fuer Listen !                         */
/* use: getline, getexp, getlst                                     */
input(buf,buflen,btr)
 char *buf;
 int buflen,btr;
 {
  STACK	X;
  STACKELEM m,call,checkf(),checkia();
  char *t,
       *s,
       *skipblanks(),*calloc();
  int eno,
      ena,
      ene,
      c,
      alev,
      count0,
      again,
      e_list,
      l_c,
      inlist,
      newbuf,
      hpos,
      err,
      result,
      erg;                /* ergebnis von input               */

  newbuf = FALSE;
  erg    = NONE;
  putrce();
  ene = ena = (int)edit6(topof(EST));
  if (ena == n_fcn)
    ena  = edit6(topof(MST));   /* vorsicht     */
  e_list =  is_e_list(etptb[ena].exptp);
  tm     = topof(MST);
  sm     = second(MST);
  m      = topof(M1ST);
  if (!EXT_INPUT_MODE) {
   IF_NOEXTERNALCMD {
    clrmsg(MROW,MCOL);
    dspstr(MROW,MCOL,etptb[ena].tmsg);
   }
  }
  if (*buf == ' ') {
   if (!EXT_INPUT_MODE) {
    IF_NOEXTERNALCMD {
     getkey();
    }
   }
    if (isatom(topof(EST)) && multi_atom(topof(EST))     /* string          */
#if N_STR
       || isconstr(topof(EST)) && class(topof(EST)) == AR0(KQUOTE) &&
          (isstrval(etptb[ena].exptp) || isexpr(etptb[ena].exptp))
#endif
/*      && !(notdisp(m) &&  arity(tm) == arity(m))    oder was ?        */
/* soll heissen: keine variablen nach ndisp-konstruktoren              */
       ) {
       asgnst(XST,EST);
#if N_STR
       lmxpos = isconstr(topof(XST)) ? travstr(XST,U1ST,buf,buflen) :
                  travmastr(XST,U1ST,buf,buflen);          /* to input_buffer   */
#else
       lmxpos = travmastr(XST,U1ST,buf,buflen);          /* to input_buffer   */
#endif

       if (lmxpos >= buflen) {                          /* buffer too small  */
          if ((buf=calloc((unsigned)(lmxpos+100),(unsigned)sizeof(char)))
               != NULLPTR) {
             newbuf = TRUE;
             buflen = lmxpos + 99;
             asgnst(XST,EST);
             clearstack(U1ST);
#if N_STR
             lmxpos = isconstr(topof(XST)) ? travstr(XST,U1ST,buf,buflen) :
                  travmastr(XST,U1ST,buf,buflen);          /* to input_buffer   */
#else
             lmxpos = travmastr(XST,U1ST,buf,buflen);          /* to input_buffer   */
#endif

           }
          else {
             message(MROW,MCOL,"there's no more core for input buffer!");
             putce();
             CURSOR_LEFT();
             getunget();
             clrmsg(MROW,MCOL);
             return(FAILURE);
          }
        }
     }
    else {
       *buf = CRSEXPR;
       lmxpos = 1;
    }
  }

  /* loop 1     */
  do {
    /* eno = ena; unused laut optimizer              /* parse auf eno */
    if (!EXT_INPUT_MODE) {
     IF_NOEXTERNALCMD {
      getline(buf,buflen,lpos,lmxpos,INROW,INCOL,INROWLEN);
     }
    }
    hpos = -1;
    err  = 0;
    clrmsg(MROW,MCOL);
    clrmsg(INROW,INCOL);
    clearstack(BST);     /* fuer eingabe */
    clearstack(U1ST);    /* fuer listen-elemenete bei expr_list  */
    l_c = 0;             /* list_count   */
    t = s = skipblanks(buf);    /* merken von s in t    */
    if (*s != '\0') {           /* leere eingabe ? */
      do {                      /* loop fuer expr_list  */
#if B_DEBUG
        inlist = FALSE;        /* innerhalb expr_list ?*/
#endif
        for (c = 1; c < MDEPTH; ++c) {          /* init parse[]         */
           parse[c].enip = parse[c].count = -1;
           parse[c].enop = -2;
        }
        parse[0].enop = parse[0].enip = ena;    /* init parse[0]        */
        parse[0].count = count0 = -1;
        mlev = alev = 0;
        do {                                    /* backtracking-loop 2  */
          level = -1;
          s = t;                                /* s auf anfang setzen  */
          for (c = alev+1; c <= mlev; ++c) {
             parse[c].enip = parse[c].count = -1;
             parse[c].enop = -2;
          }
          eno = parse[0].enop;
#if B_DEBUG
          if (debug > 1) {
            for (c = 0; c <= alev; c++) {
              fprintf(stderr,"parse[%d].enip %o, ",c,parse[c].enip);
              fprintf(stderr,"count %d, eno %o",parse[c].count,parse[c].enop);
              fprintf(stderr," e_list %d, inlist %d",e_list,inlist);
              waitkey();
            }
          }
#endif
          mlev   = 0;     /* vorsicht: expr_list einzeln parsen   */
          if (islist(etptb[eno].exptp)  && !e_list)
             c = getlst(eno,&s,0);
          else
             c = getexp(eno,&s,0);
#if B_DEBUG
          if (debug) {
            fprintf(stderr,"ret c = %d, mlev : %d",c,mlev);
            fprintf(stderr," feno %o  fnr %d",feno(c),fehlnr(c));
            fprintf(stderr," islist %d",islist(etptb[eno].exptp));
            waitkey();
          }
#endif
          inlist = (e_list  &&  c == ',');
          if ( c != '\0'  &&  !inlist ) {
            result = (int) ( s - buf ) ;    /* vermutete fehlerposition      */
#if B_DEBUG
            if (debug) {
              fprintf(stderr," result %d hpos %d ",result,hpos);
              fprintf(stderr,"alte fnr %d ",fehlnr(err));
              waitkey();
            }
#endif
            if (result > hpos && (c != FAILURE || err == 0)) {
              hpos = result;                /* also neuer fehler             */
              err  = c;
#if B_DEBUG
              if (debug) { fprintf(stderr," neuer fehler "); }
#endif
            }
#if B_DEBUG
            else
              if (debug) { fprintf(stderr," alter fehler"); }
#endif
            /* delete input     */
            delete(BST);
            while(topof(BST) != STBOTTOM) {
              delete(BST);
              decrarity(MST);
            }
            /* versuche ev. auf etwas anderes zu parsen */
            /* gemaess sct[] oder sct0[]                */
            if ( c > 0 )        /* folgt noch ein zeichen ?     */
                alev = mlev;    /* ja: setze aenderungsebene um */
            again = btr;        /* backtracking erforderlich !  */
            if (!again) alev = -1; /* fuer nicht backtracking   */
            /* solange aenderungsebene >= 0  und  again         */
            while ( alev >= 0  && again ) {
              if ((eno=sct[parse[alev].enip][++(parse[alev].count)]) == EOPARSE) {
                --(parse[alev].count);  /* fuer ev. naechsten durchlauf */
                /* ex. eintrag in sct0[] (falls alev == 0) ?            */
                if  (alev == 0) /* z.Zt. nur noch ein Eintrag in sct0[] */
                  if ((eno=sct0[parse[0].enip][++count0]) == EOPARSE) {
                    --alev;     --count0;       /* aend.ebene ; vgl.oben*/
                   }
                  else { /* es existieren noch eintraege in sct0[]      */
                    parse[alev].enop = eno;
                    again = FALSE;      /* beenden der schleife */
                  }
                else
                  --alev;
               }
              else {            /* sct[] noch nicht zu ende     */
                if (alev > 0  &&  IS_DECL(eno))
                  --alev;       /* neu: nur bei alev==0 auf fdeclst     */
                else {
                  parse[alev].enop = eno;
                  again = FALSE;        /* beenden der schleife */
                }
              }
            }   /* end while (alev >= 0 && again)       */
#if B_DEBUG
            if (debug > 1) {
                fprintf(stderr,"alevel = %d",alev);
                waitkey();
            }
#endif
          }     /* end if c != 0  &&  !inlist           */
        } while ( alev >= 0  &&  c != 0   &&  !inlist );
        if ( inlist ) {
          t = s = skipblanks(++s);      /* t,s hinter das komma */
          ++l_c;
          transport(BST,AST);
          transport(AST,U1ST);          /* richtig herum !    */
        }
      } while (e_list  &&  c == ',');

        /* eingabe ok ? ( c == 0 ? )    */
        if (c != '\0') {        /* fehler       */
          if (err < FAILURE) {
            if (EXT_INPUT_MODE) { /* (UDI) External input fct call ? */
             erg=FALSE; /* Set error flag */
             strcpy(INPUT_ERR_STR,emsg[fehlnr(err)]); /* Copy error message */
             lpos=hpos;
            } else {
             dspstr(MROW,MCOL,emsg[fehlnr(err)]);
             lpos = hpos;
             IF_EXTERNALCMD(erg=FALSE);
            }
           }
          else {
            if (EXT_INPUT_MODE) { /* (UDI) External input fct call ? */
             erg=FALSE; /* Set error flag */
             strcpy(INPUT_ERR_STR,"Too many characters or missing '[', '{' or '=' ?");
             lpos = (int) (s - buf);
            } else {
             dspstr(MROW,MCOL,"Too many characters or missing '[', '{' or '=' ?");
             lpos = (int) (s - buf);
             IF_EXTERNALCMD(erg=FALSE);
            }
          }
         }
        else {  /* eingabe ok   */
        /* eventuell E korrigieren, wg NDISP-Konstr.    */
#if B_DEBUG
          if (debug > 2) {
            dsplstack(BST,"Eingabe",1,0);
            if (topof(U1ST) != STBOTTOM)
              dsplstack(U1ST,"Liste",1,0);
            fprintf(stderr,"eno %o ena %o ene %o",eno,ena,ene);
            waitkey();
          }
#endif
          if (e_list) { /* expr_list geparst ?  */
            if (l_c) {  /* mehr als ein element */
              transport(BST,EST);       /* letztes listenelement nach E */
              for ( c = l_c ; c > 0 ; c-- ) {
                transport(U1ST,EST);    /* alle anderen elemente nach E */
              }
              eno = edit6(topof(EST));  /* edit-info des eingelesenen   */
              for ( c = l_c ; c >= 0 ; c-- )
                transport(EST,BST);     /* alle elemente nach B         */
             }
            else { /* nur ein element   */
              eno = edit6(topof(BST));
            }
           }
          else {        /* keine expr_list      */
            for ( c = count0 = arity(topof(MST)) - arity(tm) ; c > 0 ; c-- )
               transport(BST,EST);
            eno = edit6(topof(BST));
            for ( c = count0 ; c > 0 ; c-- )
               transport(EST,BST);
          }      /* end if e_list */
          if (iscall(eno))                          /* call gelesen ?       */
            push(BST,noedit(pop(BST)) | toedit6(ena));
          if (class(topof(BST)) == AR0(DECL)) {     /* decl gelesen ?       */
            eno = n_decl;                           /* wichtig (s.u.) !     */
            push(BST,noedit(pop(BST)) | toedit6(n_dcex));
          }
          if (ena == n_fnam && eno == n_fdeclst) {
            /* CE:fnam, fdeclst gelesen                                     */
            if ((count0 = arity(pop(MST)) - arity(tm)) > 0 ) {
              /* mehr als ein fdec gelesen !                                */
              pop(M1ST);                                 /* fdec poppen     */
              for (  ; count0 > 0 ; --count0) {
                incrarity(MST);             /* arity von decl erhoehen      */
              }
              push(M1ST,m);                 /* fdec wieder zurueck          */
            }
            push(MST,tm);                   /*      -  "  -                 */
            delfdec(FALSE);                 /* loesche fdec auf E           */
           }                                            else
          if (ena == n_lnam && eno == n_ldlst) {
            if ((count0 = arity(pop(MST)) - arity(tm)) > 0 ) {
              /* mehr als ein ldec gelesen !                                */
              pop(M1ST);                                 /* ldec poppen     */
              for (  ; count0 > 0 ; --count0) {
                incrarity(MST);             /* arity von decl erhoehen      */
              }
              push(M1ST,m);                 /* ldec wieder zurueck          */
            }
            push(MST,tm);                   /*      -  "  -                 */
            delfdec(FALSE);                 /* loesche ldec auf E           */
           }                                            else
#if LET_P
          if (ena == n_pnam && eno == n_pdlst) {
            if ((count0 = arity(pop(MST)) - arity(tm)) > 0 ) {
              /* mehr als ein pdec gelesen !                                */
              pop(M1ST);                                 /* pdec poppen     */
              for (  ; count0 > 0 ; --count0) {
                incrarity(MST);             /* arity von decl erhoehen      */
              }
              push(M1ST,m);                 /* pdec wieder zurueck          */
            }
            push(MST,tm);                   /*      -  "  -                 */
            delfdec(FALSE);                 /* loesche pdec auf E           */
           }                                            else
#endif
          if (ene == n_fcn) {               /* CE: funct-call-name ?        */
            clearstack(SST); pick(SST);
            if (eno == n_call) {
              delfcn(FALSE);        /* gelesen: call, altes loeschen        */
             }                                  else
            if (eno == n_decl) {
              /* gelesen: decl, call uebernehmen oder loeschen             */
              if (noedit(second(BST)) == PLACEH) {   /* kein 'goal' ?      */
                move(MST,EST);                       /* call nach e        */
                pop(M1ST);
                move(BST,MST);                       /* decl nach m        */
                pop(BST);                            /* placeh loeschen    */
                transport(EST,BST);                 /* call nach b         */
                move(MST,BST);          /* decl zurueck nach b             */
                incrarity(M1ST);        /* arity von fdec erhoehen         */
               }
              else {
                delfcn(FALSE);          /* call loeschen        */
              }
             }                                  else
            if (eno == n_fdeclst) {
              /* gelesen: fdeclst       */
              pop(M1ST);        /* call von M1, M holen         */
              if ((count0 = arity(pop(MST)) - arity(tm)) > 0 ) {
                /* mehr als ein fdec gelesen !  */
                for (  ; count0 > 0 ; --count0) {
                  incrarity(MST);       /* arity von decl erhoehen      */
                }
              }
              push(EST,tm);     /* call auf E   */
              /* arity von DECL erhoehen        */
              incrarity(MST);
              incrarity(M1ST);  incrarity(M1ST);
             }                                  else
            /* test auf variable                       */
            if (class(tm) == AR0(US_KON)  &&  IS_VAR(topof(BST)) &&
                arity(topof(MST)) == arity(tm) && !(e_list && l_c)) {
                                                  /* keine liste !          */
              delete(EST);                        /* fcn loeschen           */
             }                                  else
            /* test auf fcn (variable oder neu: prot)                       */
            if ((IS_VAR(topof(BST)) || class(topof(BST)) == class(PROTECT))
                && arity(topof(MST)) == arity(tm) &&  class(tm) != US_KON
                && !(e_list && l_c)) {            /* keine liste !          */
              delete(EST);                        /* fcn loeschen           */
              if (class(tm) != AR0(CALL)) {       /* normales CALL ?        */
                push(MST,AR0(CALL) | (pop(MST) & ~CLASS_FIELD));
                push(M1ST,AR0(CALL) | (pop(M1ST) & ~CLASS_FIELD));
              }
             }                                  else
            if ((call = checkf(noedit(topof(BST)),topof(MST))) != STBOTTOM
                && arity(topof(MST)) == arity(tm)  && class(tm) != US_KON
                && !(e_list && l_c)          /* keine liste ! */
                ) {
              delete(EST);      /* fcn loeschen */
              push(MST,call | (pop(MST) & ~CLASS_FIELD));
              push(M1ST,call | (pop(M1ST) & ~CLASS_FIELD));
             }                                  else
            if ((call = checkia(noedit(topof(BST)),topof(MST))) != STBOTTOM
                && arity(topof(MST)) == arity(tm)  && class(tm) != US_KON
                && !(e_list && l_c)          /* keine liste ! */
                ) {
              delete(EST);      /* fcn loeschen */
              push(MST,call | (pop(MST) & ~CLASS_FIELD));
              push(M1ST,call | (pop(M1ST) & ~CLASS_FIELD));
             }
            else {
              count0 = arity(pop(MST)) - arity(tm);
              push(MST,tm);     /* altes topof M        */
              delfcn(FALSE);    /* loesche call         */
              /* liste mit mehr als einem element gelesen ?     */
              for (  ; count0 > 0 ; --count0)
                incrarity(MST); /* arity des listenkonstr. erhoehen     */
             }
           }                                                    else
          if (eno == n_decl) {
            if (noedit(second(BST)) == PLACEH) {   /* kein 'goal' ?     */
              if (class(topof(EST)) == AR0(DECL)) {   /* CE: decl ?   */
                for ( c = (int)arity(pop(EST)) ; c > 1 ; c-- )
                  delete(EST);                  /* alte fkten loeschen  */
              }                         /* nur goal bleibt ueber        */
              move(BST,MST);                            /* decl nach m  */
              pop(BST);                         /* placeh loeschen      */
              transport(EST,BST);               /* altes 'goal' nach b  */
              move(MST,BST);                    /* decl zurueck nach b  */
             }
            else {
              clearstack(SST);           /* alte CE retten       */
              transport(EST,SST);
            }
           }                                            else
          if (ena == n_fdex && eno == n_fdeclst) {
            if (class(topof(MST)) == AR0(DECL)) {
              incrarity(MST);
              incrarity(M1ST);
             }
            else error("Unknown situation in input");
           }                                            else
           {    /* alte CE auf S transportieren */
             clearstack(SST);
             transport(EST,SST);
           }
          Changes = TRUE;
          transport(BST,EST);                   /* eingabe uebertragen  */

          if ((EXT_INPUT_MODE)&&(topof(BST)!=STBOTTOM)) {
           strcpy(INPUT_ERR_STR,"Cursor at a list. Move cursor to another position, and try it again.");
           erg=FALSE;
          } else { 
           while(topof(BST) != STBOTTOM) {       /* liste uebertragen    */
             transport(BST,EST);
             incrarity(M1ST);
           }
           if (!EXT_INPUT_MODE) {
            for ( c = l_c ; c > 0 ; c-- )
               incrarity(MST);    /* bei expr_list: arity erhohen */
            dispfa();             /* setzt placeh, falls ex.      */
            if (placeh > 0)       /* scroll to PLACEH             */
             while(second(EADRST) != STBOTTOM && noedit(topof(EST)) != PLACEH)
                scrlright();
            putcursor();
           }
           erg = TRUE;
          }

        }
     }
    else {      /* leere eingabe, ignorieren    */
      setcursor();
      erg = FALSE;
    }
  } while (erg == NONE);  /* end loop 1                                */
  if (newbuf) cfree(buf); /* dann auch cfree nach unten ziehen !       */
  return(erg);
 }
/* end of       input.c */
