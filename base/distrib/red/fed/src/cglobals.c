/*				cglobals.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/*  Alle (?) globalen Daten hier vereinbaren	*/

#define isstack 1
#define EDITSTATE 1

#include "cswitches.h"
#include "cportab.h"
#include "cedit.h"
#include "cestack.h"
#include "cparm.h"
#include "cedstate.h"
#include "cglobals.h"

extern char *malloc(),*strcpy();

#ifdef X_UDI
int SERVER_MODE=0;
#endif

/* die Stacks	*/

STACK E,A,M,M1,M2,B,U,U2,U3,V,S0,S1,S2,S3,R,R1,R2,EADR,AADR;
STACK S4,S5;

char initfile[LENFIL]="red.init";

char editlib[FN_ANZ][LENLIB] = {
#ifdef RALF
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/",
  "/usera/ralf/reduma/edi/"
#else
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0"
#endif
 };
char editfil[FN_ANZ][LENFIL] = {
#if UNIX
#ifdef RALF
  "/usera/ralf/reduma/edi/default.ed",
  "/usera/ralf/reduma/edi/default.pp",
  "/usera/ralf/reduma/edi/default.doc",
  "/usera/ralf/reduma/edi/default.prt",
  "/usera/ralf/reduma/edi/default.asc",
  "/usera/ralf/reduma/edi/default.red",
  "/usera/ralf/reduma/edi/default.pre"
#else
  "default.ed",
  "default.pp",
  "default.doc",
  "default.prt",
  "default.asc",
  "default.red",
  "default.pre"
#endif	/* RALF	*/
#else
  "default:ed",
  "default:pp",
  "default:doc",
  "default:prt",
  "default:asc",
  "default:red",
  "default:pre"
#endif	/* UNIX	*/
 };
char editext[FN_ANZ][LENEXT] = {
#if UNIX
   ".ed",
   ".pp",
   ".doc",
   ".prt",
   ".asc",
   ".red",
   ".pre"
#else
   ":ed",
   ":pp",
   ":doc",
   ":prt",
   ":asc",
   ":red",
   ":pre"
#endif	/* UNIX	*/
 };

int Changes = TRUE,	/* Veraenderungen am Ausdruck (wg. reduma)	*/
    Counter,		/* zaehler fuer uebergabe	*/
    dmode   = -1,	/* display-mode (tiefe der darstellung)	*/
    small   = 0,	/* display-mode (art der listen-darstellung)	*/
    news    = 0,	/* news ausgeben	*/
    placeh,		/* gibt es placeholder ? (wg. displ)	*/
    interact_in_subexpr = 0,   /* Sollen Interaktionen in Unterausdruecken */
                               /* ausgefuehrt werden? */
    rc,rca,		/* reduction-counter	*/
    def_redcnt,		/* default-reduction-counter	*/
#ifdef TRACE
    trace,		/* ausgabe von traces bei uebersetzung	*/
    ftrace,		/*                        rueckuebersetzung	*/
#endif
    initialize     =  TRUE;	/* initialisieren der reduma	*/

STACKELEM *Bottom  = SNULL;	/* zeiger auf uebergabe-seite	*/

T_UEBERGABE Parms  =  {	/* parameter-struktur	*/
       128,		/* page size	*/
  RED_INIT,		/* redcmd	*/
    999999,		/* redcnt	*/
         5,		/* precision	*/
         5,		/* truncation	*/
       200,		/* heapdes	*/
    250000,		/* heapsize	*/
	/* neu 03.12.87	*/      /* COMBINATOR           */
     10000,		/* qstacksize (e,a,h)	*/
     10000,		/* mstacksize (m,m1)	*/
     10000,		/* istacksize (i)	*/
         0,		/* formated   (0/1)	*/
     10000,		/* base (dec. arithm.)	*/
         0,		/* beta_count (0/1)	*/
         0		/* errflag	*/
       ,0.0		/* zeiten	*/
       ,0.0
       ,0.0
       ,0.0
,"\0+------------------------------  errmes  ------------------------70---+\0"
       ,0			/* file_descriptor	*/
       ,-1			/* prec_mult	*/
	,0		/* debug-info, kodierung s.o.		*/
	,0		/* dg 22.05.92 Changes_default, pi_red_plus */
	,0		/* for future use	*/
    };

/* ------------- assgnparms ---------------------------------	*/
void assgnparms(p1,p2)
 PTR_UEBERGABE p1,p2;
 {
    p1->pg_size       =  p2->pg_size;
    p1->redcmd        =  p2->redcmd;
    p1->redcnt        =  p2->redcnt;
    p1->precision     =  p2->precision;
    p1->prec_mult     =  p2->prec_mult;
    p1->truncation    =  p2->truncation;
    p1->heapdes       =  p2->heapdes;
    p1->heapsize      =  p2->heapsize;
    p1->qstacksize    =  p2->qstacksize;
    p1->mstacksize    =  p2->mstacksize;
    p1->istacksize    =  p2->istacksize;
    p1->formated      =  p2->formated;
    p1->base          =  p2->base;
    p1->beta_count    =  p2->beta_count;
    p1->errflag       =  p2->errflag;
    strcpy(p1->errmes,p2->errmes);
    p1->debug_info    =  p2->debug_info;
    p1->add_par1      =  p2->add_par1;
    p1->add_par2      =  p2->add_par2;
 }
/* ------------- createdst ----------------------------------	*/
P_EditState createdst()
 {
  extern char *malloc();
  register P_EditState state;
  register P_FedState fstate;
  P_EditState storeedst();

  state = (P_EditState)malloc((unsigned)sizeof(struct Editstate));
  if (state != (P_EditState)0) {
    if (createstacks()) {
      Bottom = SNULL;	/* init. Bottom	*/
      Counter = 0;
      fstate = (P_FedState)malloc((unsigned)sizeof(struct Fedstate));
      if (fstate != (P_FedState)0) {
        state->edit_ = (long *)fstate;
        state = storeedst(FALSE,state);
        state->argc_  = 0;
        fstate->status_  = 0;
       }
      else return((P_EditState)0);	/* war nix	*/
     }
    else return((P_EditState)0);	/* war nix	*/
  }
  return(state);
 }
/* ------------- storeedst ----------------------------------	*/
P_EditState storeedst(neu,oldstate)
 int neu;
 P_EditState oldstate;
 {
  register P_EditState state;
  register P_FedState fstate;
  register int i;

  if (neu) {
    state = (P_EditState)malloc((unsigned)sizeof(struct Editstate));
    fstate = (P_FedState)malloc((unsigned)sizeof(struct Fedstate));
    if (fstate != (P_FedState)0) {
      state->edit_ = (long *)fstate;
     }
    else return((P_EditState)0);	/* war nix	*/
   }
  else     state = oldstate;
  if (state != (P_EditState)0) {
    fstate = (P_FedState)state->edit_;
    asgnst(&(fstate->E_),EST);
    asgnst(&(fstate->A_),AST);
    asgnst(&(fstate->B_),BST);
    asgnst(&(fstate->M_),MST);
    asgnst(&(fstate->M1_),M1ST);
    asgnst(&(fstate->M2_),M2ST);
    asgnst(&(fstate->S_),SST);
    asgnst(&(fstate->S1_),S1ST);
    asgnst(&(fstate->S2_),S2ST);
    asgnst(&(fstate->S3_),S3ST);
    asgnst(&(fstate->S3_),S3ST);
    asgnst(&(fstate->V_),VST);
    asgnst(&(fstate->U_),U1ST);
    asgnst(&(fstate->U2_),U2ST);
    asgnst(&(fstate->U3_),U3ST);
    asgnst(&(fstate->R_),RST);
    asgnst(&(fstate->R1_),R1ST);
    asgnst(&(fstate->R2_),R2ST);
    asgnst(&(fstate->EADR_),EADRST);
    asgnst(&(fstate->AADR_),AADRST);
    state->Changes_  = Changes;
    state->Counter_  = Counter;
    fstate->dmode_   = dmode;
    fstate->news_    = news;
    fstate->rc_      = rc;
    fstate->rca_     = rca;
    state->init_     = initialize;
    state->Bottom_   = Bottom;
    for (i = FN_ANZ-1 ; i >= 0 ; i-- ) {
      strcpy(fstate->editfil_[i],editfil[i]);
      strcpy(fstate->editlib_[i],editlib[i]);
      strcpy(fstate->editext_[i],editext[i]);
    }
    assgnparms(&(state->Parms_),&Parms);
  }
  return(state);
 }
/* ------------- restoreedst --------------------------------	*/
restoreedst(state)
 P_EditState state;
 {
  register int i;
  register P_FedState fstate;

  fstate = (P_FedState)state->edit_;
  asgnst(EST,&(fstate->E_));
  asgnst(AST,&(fstate->A_));
  asgnst(BST,&(fstate->B_));
  asgnst(MST,&(fstate->M_));
  asgnst(M1ST,&(fstate->M1_));
  asgnst(M2ST,&(fstate->M2_));
  asgnst(SST,&(fstate->S_));
  asgnst(S1ST,&(fstate->S1_));
  asgnst(S2ST,&(fstate->S2_));
  asgnst(S3ST,&(fstate->S3_));
  asgnst(VST,&(fstate->V_));
  asgnst(U1ST,&(fstate->U_));
  asgnst(U2ST,&(fstate->U2_));
  asgnst(U3ST,&(fstate->U3_));
  asgnst(RST,&(fstate->R_));
  asgnst(R1ST,&(fstate->R1_));
  asgnst(R2ST,&(fstate->R2_));
  asgnst(EADRST,&(fstate->EADR_));
  asgnst(AADRST,&(fstate->AADR_));
  Changes    =  state->Changes_;
  Counter    =  state->Counter_;
  dmode      =  fstate->dmode_;
  news       =  fstate->news_;
  rc         =  fstate->rc_;
  rca        =  fstate->rca_;
  initialize =  state->init_;
  Bottom     =  state->Bottom_;
  for (i = FN_ANZ-1 ; i >= 0 ; i-- ) {
    strcpy(editfil[i],fstate->editfil_[i]);
    strcpy(editlib[i],fstate->editlib_[i]);
    strcpy(editext[i],fstate->editext_[i]);
  }
  assgnparms(&Parms,&(state->Parms_));
 }

/* end of	cglobals.c	*/

