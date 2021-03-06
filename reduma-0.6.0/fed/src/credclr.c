/*						credclr.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    25.07.88                                 */

#define istr     1      /* fuer definition div. daten   */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"
#include "cencod.h"
#include "cetb.h"
#include "ctred.h"
#include "cparm.h"
#include "ccommand.h"
#include "ckwtb.h"
#include "cfiles.h"

extern char initfile[];

#if SINTRAN3
#define SwitchTTY()       { if (!batch) SWITCHTTY();}
#define ResetTTY()        { if (!batch) RESETTTY(); }
#else
#define SwitchTTY()       { if (!batch) switchterm(RAWMODE); }
#define ResetTTY()        { if (!batch) switchterm(LINEMODE); }
#endif

#define N_R_E "No reducible Expression !"
#define E_T_B "Expression is too big for reduma (qstacksize) !"
#define N_M_C "Severe : No more core available ! (redcall) "
#define M_INIT "Initialising the Reduction Unit"
#define M_INIT_OK "Initialising the Reduction Unit: ok"
#define MSG_RED if (Parms.formated) sprintf(msg,"Reducing with rc = %d",Parms.redcnt);  \
                else sprintf(msg,"Reducing with parameters : rc = %d div_pr = %d mult_pr = %d\n" \
                      ,Parms.redcnt,Parms.precision,Parms.prec_mult)

#define I_READ	0
#define I_WRITE	1
#define I_FAIL	(-1)

extern char msg[],protfil[];
extern int Counter,rca,width,lastred;

extern int XUDI_up,XUDI_down;

#if UNIX
extern int readkeys();
extern void showkeys();
#endif

static char warn_msg[] = "Warning: %s (please press <CR>)";
char errfn[LENFIL];

#define NEU_INIT 1

/* ------ initparms --------------- */
long int initparms(p_Parms,all,init_file,v_opt)
 PTR_UEBERGABE p_Parms;
 int all;		/* alle parameter ?				*/
 char *init_file;	/* name des initfiles				*/
 int v_opt;             /* ist version bereits gesetzt oder kann sie	*/
 {                      /* entsprechend des initfiles gesetzt werden ?	*/
  FILE *fp,*fopen();
  int e,i,j;
  long int lastdate;
  extern int def_redcnt,small,dmode;

#if NEU_INIT
  if (initredp(init_file,I_READ,p_Parms) != I_READ) {
    if (all) def_redcnt = p_Parms->redcnt;
    return(lastdate = 0L);
  }
  if ((fp = fopen(init_file,"r")) != NULLFILE) {
    if (all) {
      i = '\n';
      do {	/* suche 'last_'	*/
        while ((e = getc(fp)) != EOF  &&  (e != 'l' || i != '\n'))
          i = e;
        if (e != EOF
            && (e = getc(fp)) == 'a'
            && (e = getc(fp)) == 's'
            && (e = getc(fp)) == 't'
            && (e = getc(fp)) == '_') j = FALSE;
        else j = TRUE;
      } while (e != EOF  && j);
      if (e == EOF) {
       sprintf(msg,"Invalid Initfile \"%s\"\n",init_file);
       message(MROW,MCOL,msg);
       return(lastdate = 0l);
      }
    }
#else
  if ((fp = fopen(init_file,"r")) != NULLFILE) {
   if (fscanf(fp,"%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n\
                  %*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n%*s %d\n",
               &(p_Parms->redcnt),&(p_Parms->precision),&(p_Parms->truncation),
               &(p_Parms->indentation),&(p_Parms->heapdes),&(p_Parms->heapsize),
               &(p_Parms->stackdes),&(p_Parms->stackpage),&(p_Parms->stackcntpage),
               &(p_Parms->qstacksize),&(p_Parms->mstacksize),&(p_Parms->istacksize),
               &(p_Parms->formated),&(p_Parms->base),&(p_Parms->beta_count)) != 15) {
      if (all) {
       sprintf(msg,"Invalid Initfile \"%s\"\n",init_file);
       message(MROW,MCOL,msg);
      }
      lastdate = 0l;
     }
    else {
#endif
     if (all) {
      def_redcnt = p_Parms->redcnt;
      if (fscanf(fp,"%*s %ld\n",&lastdate) != 1) {
        lastdate = 0l;
        sprintf(msg,"Can't read lastdate on \"%s\"\n",init_file);
        message(MROW,MCOL,msg);
       }
      else {
        while ((e = getc(fp)) != EOF  &&  e != 'A' && e != 'F')
          ;
        if (e == EOF) {
          lastdate = 0l;             /* erzwingt neu-schreiben !         */
          sprintf(msg,"Can't read Version on \"%s\"\n",init_file);
          message(MROW,MCOL,msg);
         }
        else {     /* version gelesen         */
          if (!v_opt  &&  (char)e != EDIT_VERS_C)
            if (!swt_vers()) { /* oder message ?      */
              message(MROW,MCOL,"Can't switch editor-version\n");
            }
          /* lese libraries	*/
          for (i = 0 ; i < FN_ANZ &&  e != EOF; i++ ) {
            while ((e = getc(fp)) != EOF  &&  e != '"')
              ;
            if (e != EOF) {
              j = 0;
              while ((e = getc(fp)) != EOF  &&  e != '"')
                editlib[i][j++] = e;
              editlib[i][j] = '\0';
            }
          }      /* end for i      */
          /* lese extensions	*/
          for (i = 0 ; i < FN_ANZ &&  e != EOF; i++ ) {
            while ((e = getc(fp)) != EOF  &&  e != '"')
              ;
            if (e != EOF) {
              j = 0;
              while ((e = getc(fp)) != EOF  &&  e != '"')
                editext[i][j++] = e;
              editext[i][j] = '\0';
            }
          }      /* end for i      */
          if (e != EOF) {
            while ((e = getc(fp)) != EOF  &&  e != 'S')	/* suche Small	*/
              ;
            if (fscanf(fp,"mall : %d\n",&j) == 1) {
              if (j) small |= DSMALL;
              else   small &= ~DSMALL;
            if (fscanf(fp,"Dmode : %d\n",&j) == 1) { dmode = j;
            if (fscanf(fp,"Width : %d\n",&j) == 1) { width = j;
            if (fscanf(fp,"abbreviate def : %d\n",&j) == 1) { 
              if (j) small |= DDECL;
              else   small &= ~DDECL;
            if (fscanf(fp,"abbreviate only inner def : %d\n",&j) == 1) { 
              if (j) small |= DTDECL;
              else   small &= ~DTDECL;
            if (fscanf(fp,"show goal : %d\n",&j) == 1) { 
              if (j) small |= DGOAL;
              else   small &= ~DGOAL;
            if (fscanf(fp,"show fct-list : %d\n",&j) == 1) { 
              if (j) small |= DFCT;
              else   small &= ~DFCT;
            if (fscanf(fp,"vertical display : %d\n",&j) == 1) { 
              if (j) small |= DVERT;
              else   small &= ~DVERT;
            if (fscanf(fp,"lineoriented cursor mode : %d\n",&j) == 1) { 
             if (j) {XUDI_up=2;XUDI_down=2;}
             else {XUDI_up=0;XUDI_down=0;} 
            }
            }
            }
            }
            }
            }
            }
            }
            }
          }
#if UNIX
          /* lese key-definitionen */
          if (!readkeys(fp)) lastdate = 0L; /* nicht geklappt ?        */
#endif
          if (e == EOF) lastdate = 0L;
        }
      }
     }
     else lastdate = 1L;
#if !NEU_INIT
    }
#endif
    fclose(fp);
  }
 else {
  if (all) {
   sprintf(msg,"Can't read \"%s\"\n",init_file);
   message(MROW,MCOL,msg);
  }
  lastdate = 0l;
 }
 return(lastdate);
}

/* ------ saveparms --------------- */
saveparms(lastdate)
 long int lastdate;
 {
  extern void editparms();
  FILE *fp,*fopen();

#if NEU_INIT
  if (initredp(initfile,I_WRITE,&Parms) != I_WRITE) return(FALSE);
  if ((fp = fopen(initfile,"a")) != NULLFILE) {
#else
  if ((fp = fopen(initfile,"w")) != NULLFILE) {
    fprintf(fp,"redcount        %d\n",Parms.redcnt);
    fprintf(fp,"precision       %d\n",Parms.precision);
    fprintf(fp,"truncation      %d\n",Parms.truncation);
    fprintf(fp,"indentation     %d\n",Parms.indentation);
    fprintf(fp,"heapdes         %d\n",Parms.heapdes);
    fprintf(fp,"heapsize        %d\n",Parms.heapsize);
    fprintf(fp,"stackdes        %d\n",Parms.stackdes);
    fprintf(fp,"stackpage       %d\n",Parms.stackpage);
    fprintf(fp,"stackcntpage    %d\n",Parms.stackcntpage);
    fprintf(fp,"qstacksize      %d\n",Parms.qstacksize);  /* COMBINATOR */
    fprintf(fp,"mstacksize      %d\n",Parms.mstacksize);  /* COMBINATOR */
    fprintf(fp,"istacksize      %d\n",Parms.istacksize);  /* COMBINATOR */
    fprintf(fp,"formated        %d\n",Parms.formated);    /* COMBINATOR */
    fprintf(fp,"base            %d\n",Parms.base);        /* COMBINATOR */
    fprintf(fp,"beta_count      %d\n",Parms.beta_count);  /* COMBINATOR */
#endif
    fprintf(fp,"last_date       %ld\n",lastdate);
    editparms(fp);
#if UNIX
    showkeys(fp);
#endif
    fclose(fp);
    return(TRUE);
  }
 else {
   return(FALSE);
 }
}

/* ------ my_saveparms --------------- */
my_saveparms(lastdate,initfile)
 long int lastdate;
 char *initfile;
 {
  extern void editparms();
  FILE *fp,*fopen();

#if NEU_INIT
  if (initredp(initfile,I_WRITE,&Parms) != I_WRITE) return(FALSE);
  if ((fp = fopen(initfile,"a")) != NULLFILE) {
#else
  if ((fp = fopen(initfile,"w")) != NULLFILE) {
    fprintf(fp,"redcount        %d\n",Parms.redcnt);
    fprintf(fp,"precision       %d\n",Parms.precision);
    fprintf(fp,"truncation      %d\n",Parms.truncation);
    fprintf(fp,"indentation     %d\n",Parms.indentation);
    fprintf(fp,"heapdes         %d\n",Parms.heapdes);
    fprintf(fp,"heapsize        %d\n",Parms.heapsize);
    fprintf(fp,"stackdes        %d\n",Parms.stackdes);
    fprintf(fp,"stackpage       %d\n",Parms.stackpage);
    fprintf(fp,"stackcntpage    %d\n",Parms.stackcntpage);
    fprintf(fp,"qstacksize      %d\n",Parms.qstacksize);  /* COMBINATOR */
    fprintf(fp,"mstacksize      %d\n",Parms.mstacksize);  /* COMBINATOR */
    fprintf(fp,"istacksize      %d\n",Parms.istacksize);  /* COMBINATOR */
    fprintf(fp,"formated        %d\n",Parms.formated);    /* COMBINATOR */
    fprintf(fp,"base            %d\n",Parms.base);        /* COMBINATOR */
    fprintf(fp,"beta_count      %d\n",Parms.beta_count);  /* COMBINATOR */
#endif
    fprintf(fp,"last_date       %ld\n",lastdate);
    editparms(fp);
#if UNIX
    showkeys(fp);
#endif
    fclose(fp);
    return(TRUE);
  }
 else {
   return(FALSE);
 }
}

#ifdef UnBenutzt
/* ------ resetred ---------------- */
resetred()
{

 Counter = 0;
 Parms.redcmd = RED_RS;
 message(MROW,MCOL,"Resetting the reduction unit\n");
 CALLREDUCE(Bottom,&Counter,&Parms);
/*  reset = NO;           */
 clrmsg(MROW,MCOL);
}
#endif

/* ------ b_initred --------------- */
b_initred()
{

 Parms.redcmd = RED_INIT;
 Parms.errflag = Counter = 0;
/* initparms(FALSE);      */
 fputs(M_INIT,stdout);
 CALLREDUCE(Bottom,&Counter,&Parms);
 if (Parms.errflag) {
    fputs(Parms.errmes,stdout);
    return(FAILURE);
  }
 initialize = FALSE;
 fputs(": ok\n",stdout);
 return(SUCCESS);
}

/* ------ stopred --------------- */
stopred()
{

 Parms.redcmd = RED_STOP;
 Parms.errflag = Counter = 0;
/* initparms(FALSE);      */
/* fputs(M_INIT,stdout);	*/
 CALLREDUCE(Bottom,&Counter,&Parms);
 if (Parms.errflag) {
    fputs(Parms.errmes,stdout);
    return(FAILURE);
  }
 return(SUCCESS);
}

/* ------ initred --------------- */
initred()
{

 Parms.redcmd = RED_INIT;
 Parms.errflag = Counter = 0;
 putcursor();
 putrce();
 message(MROW,MCOL,M_INIT);
 CALLREDUCE(Bottom,&Counter,&Parms);
 if (Parms.errflag) {
/*
#if DEBUG
*/
    initscreen();
    dispfa();
/*
#endif
*/
    strcpy(msg,Parms.errmes);
    message(MROW,MCOL,msg);
    putce();
    CURSOR_LEFT();
    return(FAILURE);
  }
 initialize = FALSE;
 message(MROW,MCOL,M_INIT_OK);
 putce();
 CURSOR_LEFT();
 return(SUCCESS);
}
/* ------ init2 ----------------- */
init2()
{
 int erg = SUCCESS;

  Parms.errflag = 0;
  Parms.redcmd = RED_RS;
  CALLREDUCE(Bottom,&Counter,&Parms);
  if (Parms.errflag) {
    SwitchTTY();
    dspstr2(MROW,MCOL,Parms.errmes," [Hit <return>] ");
    waitkey();
    erg = FAILURE;
  }
  return(erg);
}

#ifdef TRACE
extern int trace,ftrace;
#endif
extern int rc;
static int pagesize,back,dspl;
long stacksize();
#if !LETREC
long scrto();
#endif /* !LETREC */
#define NOT_FROM (-2)

static STACKELEM editfield;
/* ----------- _atored ------------------------------------------------ */
/* aufruf der transformationsroutinen und der reduktionseinheit         */
static _atored(red)
 int red;
 {
  register int n,erg;

  erg = SUCCESS;
  n = (int)edit6(topof(EST));
  if (edit6(topof(EST)) == n_fcn) {   /* function-call ?      */
    move(MST,EST);                    /* call nach e          */
    n = (int)edit6(topof(EST));
    pop(M1ST);
    incrarity(M1ST);
  }                                /* end if function-call    */
  if (isexpr(etptb[n].exptp)  &&  noedit(topof(EST)) != NOPAR) {
    editfield = topof(EST) & EDIT_FIELD;
    clearstack(RST);
    clearstack(SST);                                   /* backup         */
    if (PROT_PREV(protocoll) && protfil[0]) {
      if (prettyprint(protfil,"a",width,protocoll,
            PP_ASC(protocoll) ? FN_ASC : FN_PRT) != SUCCESS) {
        dspstr(MROW,MCOL,"Can't print protocoll\n");
      }
    }
    duplicate(EST,SST);                                /* backup         */
    ResetTTY();                                        /* Line-Modus     */
    if (initialize) {
      if (initred() == FAILURE) {
        getunget();
        clrmsg(MROW,MCOL);
        return(FAILURE);
      }
    }
    if (!Changes) {
       pushi(KLAA,RST);
       delete(EST);
     }
    else {
      erg = tored(EST,RST);                   /* translate    */
      if (erg >= SUCCESS) {
        if (erg != SUCCESS) {
          if (erg != UND_FIL) {
            sprintf(msg,warn_msg,tormsg[erg]);
           }
          else {
            sprintf(lbuf,warn_msg,tormsg[erg]);
            sprintf(msg,lbuf,errfn);
          }
          dspstr(MROW,MCOL,msg);
          erg = SUCCESS;
          waitkey();
        }
        if (red_reset(red)) erg = init2();
      }
    }
    if (erg >= SUCCESS) {
      Parms.redcmd  = RED_NP;
      Parms.redcnt  = rca;
      Parms.errflag = 0;
      pagesize = stacksize(RST);
      if (pagesize >= Parms.qstacksize) {
        strcpy(msg,E_T_B);
        message(MROW,MCOL,msg);
        waitkey();
        rca = -1;
        erg = FAILURE;
       }
      else {
        if (pagesize > 1  ||  pagesize > Parms.pg_size) {
          Parms.pg_size = pagesize;
          if (Bottom != SNULL) {
            cfree((char *)Bottom);
            Bottom = SNULL;
          }
        }
        if (Bottom == SNULL)
          Bottom = (STACKELEM *)calloc((unsigned)Parms.pg_size,
                                       (unsigned)sizeof(STACKELEM));
        if (Bottom != SNULL) {
          /* zu reduzierender ausdruck ist auf R        */
          for (Counter = 0 ; Counter < pagesize ; Counter++) {
            *(Bottom + Counter) = pop(RST);
          }                                    /* end while        */
          SwitchTTY();
         }
        else {      /* kein platz fuer uebergabe-seite  */
          strcpy(msg,N_M_C);
          message(MROW,MCOL,msg);
          rca = -1;
          erg = FAILURE;
        }
      }
     }
    else {                /* to_red falsch  */
      rca = -1;  /* fuer wiederholen des originalausdrucks */
      SwitchTTY();
      if (erg != FAILURE) {  /* von init2 ?    */
        strcpy(msg,tormsg[-erg]);
        message(MROW,MCOL,msg);
        getunget();
        erg = FAILURE;
      }
    }            /* end if erg (=tored) >= 0      */
      Changes = GET_PAR1(&Parms,P_CHANGES_DEFAULT) ; /* dg 22.05.92 */
   }
  else {         /* kein reduzierbarer ausdruck      */
    strcpy(msg,N_R_E);
    message(MROW,MCOL,msg);
    erg = NOT_FROM;
  }     /* end if reduc. expr      */
  return(erg);
 }

/* ----------- _afromred ---------------------------------------------- */
/* rueckgabe von der reduktionseinheit, aufruf der ruecktransformation  */
static _afromred()
 {
  register int i,c,erg;

  SwitchTTY();
  erg = SUCCESS;
  if (rca >= 0) {
    clearstack(U1ST);
    pagesize = Parms.pg_size;
    /* if (Bottom == SNULL)                                              */
    /*   Bottom = (STACKELEM *)calloc(pagesize,sizeof(STACKELEM));       */
    if (Bottom != SNULL) {
      /* reduzierten ausdruck nach U1ST      */
      while (TRUE) {                      /* Rueckgabe         */
        for (i = 0 ; i < Counter ; i++)
           pushi(*(Bottom + i),U1ST);
        if (Parms.redcmd != RED_LRP) {
           Counter = 0;
           Parms.redcmd = RED_NRP;
           CALLREDUCE(Bottom,&Counter,&Parms);
         }
        else break;
      }                                  /* Rueckgabe beendet */
      cfree((char *)Bottom);
      Bottom = SNULL;
      clearstack(RST);
  if (dspl == 6) dsplstack(U1ST,"U1 von reduma ",1,0);
      while (move(U1ST,RST) != STBOTTOM)     /* uebertrage U1ST nach RST */
        ;
  if (dspl == 5) dsplstack(RST,"R von reduma (moved)",1,0);
     }
    else {
      strcpy(msg,N_M_C);
      message(MROW,MCOL,msg);
      /* was dann ?       */
      rca = -1;
      erg = FAILURE;
    }
  }
  initscreen();
  if (Parms.errflag) {
    dspstr2(MROW,MCOL,Parms.errmes," (Hit <return>) ");
    waitkey();
  }
  if (rca == -1  ||  topof(RST) == KLAA) {    /* alter ausdruck ? */
    Changes = TRUE;       /* ?     */
    clearstack(RST);
    duplicate(SST,EST);
   }
  else {         /* reduzierter ausdruck    */
    dspstr(MROW,MCOL,"Re-Translating\n");
    fromred(RST,EST,FALSE);                      /* R --> E */
    if (isempty(MST)) push(EST,noedit(pop(EST)) | toedit(start_eno));
    else push(EST,noedit(pop(EST)) | editfield);
  }
  /* reduzierter ausdruck in transponierter form auf R1       */
  SwitchTTY();
  if (dspl > 2) {
    dsplstack(EST,"reduziert",1,0);
  }
  if (dspl) initscreen();
  if (PROT_AFTER(protocoll) && protfil[0]) {
    if (prettyprint(protfil,"a",width,protocoll,
          PP_ASC(protocoll) ? FN_ASC : FN_PRT) != SUCCESS) {
        dspstr(MROW,MCOL,"Can't print protocoll\n");
    }
  }
  if (!batch) dispfa();
  if (rca >= 0  &&  erg == SUCCESS) {
    c = rca - Parms.redcnt;
    sprintf(lbuf,"Performed %d reduction step%c ",
            c,(c == 1) ? ' ' : 's');
    c = strlen(lbuf);
    if (IS_INFO(D_STOPPED,Parms.debug_info)) {
       sprintf(lbuf+c,"STOP occurred ");
       c = strlen(lbuf);
    }
    if (IS_INFO(D_ERRORED,Parms.debug_info)) {
       sprintf(lbuf+c,"ERROR occurred ");
    }
    strcpy(msg,lbuf);
    message(MROW,MCOL,msg);
  }
  if (!batch) {
    getunget();
    clrmsg(MROW,MCOL);
  }
  return(erg);
 }

/* ----------- redc --------------------------------------------------- */
/* aufruf der transformationsroutinen und der reduktionseinheit         */
redc(rc0,tr,dspl0,back0,ftr,red)
 int rc0,tr,dspl0,back0,ftr,red;
 {
  int erg
#if !LETREC
  ,apv
#endif
  ;

  rc     = rca = rc0;                 /* red.-counter         */
#ifdef TRACE
  trace  = tr;
  ftrace = ftr;
#endif
  dspl   = dspl0;
  back   = back0;
  lastred = L_UNDEFINED;

  Parms.errflag = FALSE;
  msg[0] = '\0';
/* erstmal nicht !
  Parms.errmes[0] = '\0';
*/
  Changes |= GET_PAR1(&Parms,P_CHANGES_DEFAULT) ; /* dg 22.05.92 */
#if LETREC
  if (red_to(red)) {
    if (!Changes && IS_INFO(D_STOPPED,Parms.debug_info) && IS_INFO(D_STOP,Parms.debug_info)) {
      sprintf(msg,"STOP occurred and no changes: -> No Reduction");
      message(MROW,MCOL,msg);
      getunget();
      erg = FAILURE;
     }	else
    if (!Changes && IS_INFO(D_ERRORED,Parms.debug_info) && IS_INFO(D_ERROR,Parms.debug_info)) {
      sprintf(msg,"ERROR occurred and no changes: -> No Reduction");
      message(MROW,MCOL,msg);
      getunget();
      erg = FAILURE;
     }
    else {
      CLR_INFO(D_ERRORED,Parms.debug_info);
      CLR_INFO(D_STOPPED,Parms.debug_info);
      erg = _atored(red);
    }
  }
  else erg = SUCCESS;
#else
  apv    = !back && isapvers;
  if (red_to(red)) {
    if (!Changes && IS_INFO(D_STOPPED,Parms.debug_info) && IS_INFO(D_STOP,Parms.debug_info)) {
      sprintf(msg,"STOP occurred and no changes: -> No Reduction");
      message(MROW,MCOL,msg);
      getunget();
      erg = FAILURE;
     }	else
    if (!Changes && IS_INFO(D_ERRORED,Parms.debug_info) && IS_INFO(D_ERROR,Parms.debug_info)) {
      sprintf(msg,"ERROR occurred and no changes: -> No Reduction");
      message(MROW,MCOL,msg);
      getunget();
      erg = FAILURE;
     }
    else {
      CLR_INFO(D_ERRORED,Parms.debug_info);
      CLR_INFO(D_STOPPED,Parms.debug_info);
      erg = apv ? _atored(red) : _tored(red);
    }
  }
  else erg = SUCCESS;
#endif
  if (red_red(red)  &&  erg == SUCCESS) {
    MSG_RED;
    dspstr(MROW,MCOL,msg);
    ResetTTY();
    Parms.redcmd = RED_RD;          /* CALLREDUCE !!!!!!  */
    getbase(Parms.errmes,FN_ED);
    Parms.errmes[strlen(Parms.errmes)+1] = (char)compile_only;
    CALLREDUCE(Bottom,&Counter,&Parms);
    lastred = L_REDUCED;
  }
#if KEDIT
  if (red_unld(red)  &&  erg == SUCCESS) {
    ResetTTY();
    Parms.redcmd = RED_UNLOAD;
    Parms.errflag = 0;
    CALLREDUCE(Bottom,&Counter,&Parms);
    lastred = L_UNDEFINED;
    SwitchTTY();
    initscreen();
    dispfa();
    if (Parms.errflag) {
      strcpy(msg,Parms.errmes);
      message(MROW,MCOL,msg);
      putce();
      CURSOR_LEFT();
      erg = FAILURE;
    }
  }
  if (red_preld(red)  &&  erg == SUCCESS) {
    ResetTTY();
    Parms.redcmd = RED_PRELD;
    Parms.errflag = 0;
    CALLREDUCE(Bottom,&Counter,&Parms);
    lastred = L_PRELOAD;
    SwitchTTY();
    initscreen();
    dispfa();
    if (Parms.errflag) {
      strcpy(msg,Parms.errmes);
      message(MROW,MCOL,msg);
      putce();
      CURSOR_LEFT();
      erg = FAILURE;
    }
  }
#endif /* KEDIT */
  if (red_pre(red)  &&  erg == SUCCESS) {
    ResetTTY();
    Parms.redcmd = RED_PRE;
    Parms.errflag = 0;
    CALLREDUCE(Bottom,&Counter,&Parms);
    lastred = L_PREPROC;
    SwitchTTY();
    initscreen();
    if (Parms.errflag) {
      dispfa();
      strcpy(msg,Parms.errmes);
      message(MROW,MCOL,msg);
      putce();
      CURSOR_LEFT();
      erg = FAILURE;
    }
  }
#if LETREC
  if (red_from(red)  &&  erg != NOT_FROM) erg = _afromred();
#else
  if (red_from(red)  &&  erg != NOT_FROM)
    erg = apv ? _afromred() : _fromred();
#endif
  return(erg);
 }
#if !LETREC
/* ----------- _tored ------------------------------------------------- */
/* aufruf der transformationsroutinen und der reduktionseinheit         */
static _tored(red)
 int red;
 {
  register int n,c,erg;

  n = (int)edit6(topof(EST));
  if (edit6(topof(EST)) == n_fcn) {   /* function-call ?      */
    move(MST,EST);                    /* call nach e          */
    n = (int)edit6(topof(EST));
    pop(M1ST);
    incrarity(M1ST);
  }                                /* end if function-call    */
  if (isexpr(etptb[n].exptp)  &&  noedit(topof(EST)) != NOPAR) {
    erg = SUCCESS;              /* kein Fehler                */
    /* fuer to_red       */
    copysted(EST,RST);
    copysted(AST,R2ST);
    clearstack(BST);   clearstack(VST);
    clearstack(U1ST);  clearstack(U2ST);
    clearstack(R1ST);
    clearstack(SST);   pick(SST);
    if (PROT_PREV(protocoll) && protfil[0]) {
      if (prettyprint(protfil,"a",width,protocoll,
             PP_ASC(protocoll) ? FN_ASC : FN_PRT) != SUCCESS) {
        dspstr(MROW,MCOL,"Can't print protocoll\n");
      }
    }
    iclrmsg(MROW,MCOL);
    dspstr(MROW,MCOL,"Transforming\n");
    /* R:ausdruck,R1:transf.,R2 == A, B: hilf(fkten)    */
    if ((c=to_red(RST,R1ST,R2ST,BST)) >= 0) {    /* FAILURE < 0 ! */
      if (edit_vers != (back ? FCT_VERSION : AP_VERSION))  /* achtung */
        Changes = TRUE;
      clearstack(BST);           /* vorsicht : Funktionen ! */
      clearstack(R2ST);          /* vorsicht ! */
      transport(R1ST,R2ST);      /* richtig auf R2       */
      push(MST,ENDSYMB);
      if (dspl > 3) dsplstack(R2ST,"r2 vor scrto",1,0);
      scrto(R2ST,R1ST);         /* an richtige Stelle   */
      if ((c = incompl(R2ST,RST,rca)) >= 0) {
        if (c != SUCCESS) {
          sprintf(msg,warn_msg,tormsg[c]);
          dspstr(MROW,MCOL,msg);
          waitkey();
        }
        transp(R2ST,R1ST);      /* loesche edit-info's  */
        transport(R1ST,R2ST);   /* zurueck              */
        dspstr(MROW,MCOL,"Translating\n");
        if (dspl > 2) dsplstack(R2ST,"reduzieren",1,0);
  /* Situation: zu reduzierender Ausdruck auf R2 inkl. Rest des Ausdrucks*/
  /* auf R1 linker Teil des Ausdrucks                                    */

        clearstack(VST);                               /* backup       */
        duplicate(R2ST,VST);             /* kopiere Ausdruck nach R2   */
        if (rca >= 0) {
          if (initialize) {
            if (initred() == FAILURE) {
              getunget();
              clrmsg(MROW,MCOL);
              return(FAILURE);
            }
          }
          clearstack(RST);             /* loesche R fuer Uebertragung    */
          ResetTTY();
          /* zu reduzierender ausdruck von R2 kommt nach R  */
          if (!Changes) {             /* falls keine Aenderungen:   */
            pushi(KLAA,RST);          /* KLAA nach R                */
            delete(R2ST);             /* loesche obersten ausdruck von R2 */
           }
          else {                       /* sonst: uebersetzung von R2 */
             /* nach R                     */
            erg = tored(R2ST,RST);                   /* translate    */
            if (erg >= SUCCESS && red_reset(red)) erg = init2();
          }
          if (erg >= SUCCESS) {
            if (erg != SUCCESS) {
              sprintf(msg,warn_msg,tormsg[erg]);
              dspstr(MROW,MCOL,msg);
              erg = SUCCESS;
              waitkey();
            }
            Parms.redcmd  = RED_NP;
            Parms.redcnt  = rca;
            Parms.errflag = 0;
            pagesize = stacksize(RST);
            if (pagesize >= Parms.qstacksize) {
              strcpy(msg,E_T_B);
              message(MROW,MCOL,msg);
              waitkey();
              rca = -1;
              erg = FAILURE;
             }
            else {
              if (pagesize > 1  ||  pagesize > Parms.pg_size) {
                Parms.pg_size = pagesize;
                if (Bottom != SNULL) {
                  cfree((char *)Bottom);
                  Bottom = SNULL;
                }
              }
              if (Bottom == SNULL)
                Bottom = (STACKELEM *)calloc((unsigned)Parms.pg_size,
                                             (unsigned)sizeof(STACKELEM));
              if (Bottom != SNULL) {
                /* zu reduzierender ausdruck ist auf R        */
                Counter = 0;
                for (Counter = 0 ; Counter < pagesize ; Counter++) {
                  *(Bottom + Counter) = pop(RST);
                }                                    /* end while        */
                SwitchTTY();
               }
              else {      /* kein platz fuer uebergabe-seite  */
                strcpy(msg,N_M_C);
                message(MROW,MCOL,msg);
                /* was dann ?       */
                rca = -1;
                erg = FAILURE;
              }
            }    /* end if pagesize > qstacksize */
           }
          else { /* fehler bei tored */
            rca = -1;  /* fuer wiederholen des originalausdrucks */
            SwitchTTY();
            if (erg != FAILURE) {  /* von init2 ?    */
              strcpy(msg,tormsg[-erg]);
              message(MROW,MCOL,msg);
              getunget();
              erg = FAILURE;
            }
          }               /* end if erg >= SUCCESS   */
            Changes = GET_PAR1(&Parms,P_CHANGES_DEFAULT) ; /* dg 22.05.92 */
/*       }       */
/*      else {   */    /* rca < 0       */
        }             /* end if rca >= 0 */
       }
      else {            /* unvollstaendig       */
        if (pop(MST) != ENDSYMB) error("redc:Fatal error (endsymb)");
        strcpy(msg,tormsg[-c]);
        message(MROW,MCOL,msg);
        if (!batch) dispfa();
        erg = NOT_FROM;
      }
     }
    else {                /* to_red falsch  */
      strcpy(msg,tormsg[-c]);
      message(MROW,MCOL,msg);
      if (!batch) dispfa();
      erg = NOT_FROM;
    }            /* end if to_red >= 0      */
   }
  else {         /* kein reduzierbarer ausdruck      */
    strcpy(msg,N_R_E);
    message(MROW,MCOL,msg);
    erg = NOT_FROM;
  }     /* end if reduc. expr      */
  return(erg);
 }
/* ----------- _fromred ----------------------------------------------- */
/* rueckgabe von der reduktionseinheit, aufruf der ruecktransformation  */
static _fromred()
 {
  register int i,c,erg;

  SwitchTTY();
  erg = SUCCESS;
  if (rca >= 0) {
    clearstack(U1ST);
    pagesize = Parms.pg_size;
    /* if (Bottom == SNULL)                                              */
    /*   Bottom = (STACKELEM *)calloc(pagesize,sizeof(STACKELEM));       */
    if (Bottom != SNULL) {
      /* reduzierten ausdruck nach U1ST      */
      while (TRUE) {                      /* Rueckgabe         */
        for (i = 0 ; i < Counter ; i++)
           pushi(*(Bottom + i),U1ST);
        if (Parms.redcmd != RED_LRP) {
           Counter = 0;
           Parms.redcmd = RED_NRP;
           CALLREDUCE(Bottom,&Counter,&Parms);
         }
        else break;
      }                                  /* Rueckgabe beendet */
      cfree((char *)Bottom);
      Bottom = SNULL;
      clearstack(U2ST);
      while (move(U1ST,U2ST) != STBOTTOM)     /* uebertrage U1ST nach U2ST */
        ;
     }
    else {
      strcpy(msg,N_M_C);
      message(MROW,MCOL,msg);
      /* was dann ?       */
      rca = -1;
      erg = FAILURE;
    }
  }
  initscreen();
  if (Parms.errflag) {
    dspstr2(MROW,MCOL,Parms.errmes," (Hit <return>) ");
    waitkey();
/*  rca = -1;             notwendig ?       */
    erg = FAILURE;
  }
  if (rca == -1  ||  topof(U2ST) == KLAA) {    /* alter ausdruck ? */
    clearstack(U2ST);
    copy(VST,R1ST);
   }
  else {         /* reduzierter ausdruck    */
    dspstr(MROW,MCOL,"Re-Translating\n");
    fromred(U2ST,R2ST,back);                      /* U2 --> R2 */
    transp(R2ST,R1ST);
  }
  /* reduzierter ausdruck in transponierter form auf R1       */
  SwitchTTY();
  if (dspl > 2) dsplstack(R1ST,"reduziert",1,0);
  dspstr(MROW,MCOL,"Re-Transforming\n");
  transp(R1ST,R2ST);              /* richtig nach R2      */
  push(R2ST,markce(pop(R2ST)));   /* markiere ce  */
  scrba(R1ST,R2ST);               /* rest von R1 nach R2  */
  transport(R2ST,R1ST);           /* kompl. nach R1 (tr.) */
  clearstack(RST); clearstack(EST);
  if (back) {
    from_red(R1ST,EST,RST,BST);    /* E: fkten + goal*/
    clearstack(MST);
    clearstack(M1ST);
    clearstack(AST);
    Changes = TRUE;               /* ist das gut ?      */
   }
  else {                           /* not back       */
    clearstack(MST);
    clearstack(AST);
    ct2m(R1ST,EST,TRUE);
  }
  c = edit_vers;
  edit_vers = back ? FCT_VERSION : AP_VERSION; /* achtung */
  if (edit_vers != c) {
    init_back(TRUE);
  }
  /* achtung wg. NDISP-Konstruktoren !    */
  c = ismce(topof(EST));
  if (topof(MST)==STBOTTOM)       /* richtige info einsetzen      */
     push(EST,noedit(pop(EST)) | toedit6(start_eno));
  if (dspl) dsplstack(EST,"E: ",1,0);
#ifdef TRACE
  if (trace || ftrace  || dspl) initscreen();
#else
  if (dspl) initscreen();
#endif
  if (PROT_AFTER(protocoll) && protfil[0]) {
    if (prettyprint(protfil,"a",width,protocoll,
          PP_ASC(protocoll) ? FN_ASC : FN_PRT) != SUCCESS) {
      dspstr(MROW,MCOL,"Can't print protocoll\n");
    }
  }
  if (!batch) {
    if (c)
      display(TRUE);
    else
      anzeigen();
  }
  clrmsg(MROW,MCOL);
  if (rca >= 0  &&  erg == SUCCESS) {
    c = rca - Parms.redcnt;
    sprintf(lbuf,"Performed %d reduction step%c ",
            c,(c == 1) ? ' ' : 's');
    c = strlen(lbuf);
    if (IS_INFO(D_STOPPED,Parms.debug_info)) {
       sprintf(lbuf+c,"STOP occurred ");
       c = strlen(lbuf);
    }
    if (IS_INFO(D_ERRORED,Parms.debug_info)) {
       sprintf(lbuf+c,"ERROR occurred ");
    }
    strcpy(msg,lbuf);
    message(MROW,MCOL,msg);
  }
  if (!batch) {
    getunget();
    clrmsg(MROW,MCOL);
  }
  return(erg);
 }
#endif /* LETREC */

#if LETREC
/* static int str_ind;    unused ? */
static char strstor[SLEN];     /* string-storage       */

/* ============== chkdopp =================================== */
/* Situation:                                                 */
/* e: Liste der Funktionsnamen (transponiert)                 */
/* b: Hilfsstack                                              */
chkdopp(e,b)
 STACK *e,*b;
 {
  register int anz,i,j,erg;
  int index;

  anz = arity(topof(e));           /* anzahl fkten (LIST)              */
  erg = TRUE;	                   /* vorlaeufiges ergebnis            */
  if (anz > 1) {          /* mehr als eine fkt fkten vorhanden ?       */
    initstr();                     /* init. strstor                    */
    index = 0;                     /* index in strstor                 */
#ifdef TRACE
    if (trace > 6) {
      dsplstack(e,"e vor chkdopp",1,0);
      dsplstack(b,"b vor chkdopp",1,0);
    }
#endif
    move(e,MST);                   /* LIST -> M                        */
    i = 0;
    while (i < anz  &&  erg) {   /* durchlaufe fkten        */
      fkt[i] = index;     /* setze zeiger auf fktsname in strstor      */
      if (!apt_str(e,b,strstor,&index,SLEN-1))
        error(tormsg[-OVERST]);
      for ( j = 0 ; j < i ; j++ ) {         /* vorherg. fkten vergl.   */
        if (strcmp(strstor+fkt[i],strstor+fkt[j]) == SUCCESS)
          erg = FALSE;                     /* identisch: fehler       */
      }
      ++i;
    }
    for ( ; i > 0 ; i-- ) {        /* fktsnamen zurueck                */
      transport(b,e);
    }
    move(MST,e);                   /* LIST -> e zurueck                */
#ifdef TRACE
    if (trace > 6) {
      dsplstack(e,"e nach chkdopp",1,0);
      dsplstack(b,"b nach chkdopp",1,0);
    }
#endif
  }
  return(erg);                     /* ergebnis abliefern               */
 }
/* ------- apt_str --------------------------------------------------- */
/* transportiere string von e nach a und speichere ihn transponiert    */
/* in str ab *ind                                                      */
apt_str(e,a,str,ind,max)
 STACK *e,*a;
 char *str;
 int *ind,max;
 {
  register STACKELEM m;
  register int ret,l;
  char string[FNLEN],*s;

  ret = TRUE;
  l   = 1;
  s   = string;
  if (marked0(m = pop(e))) {
    push(a,m);
    *(s++) = value(m);
   }
  else {
    push(a,mark0(m));
    *(s++) = value(m);
    do {
      m = pop(e);
      push(a,m);
      if (++l < FNLEN)
        *(s++) = value(m);
      else
        ret = FALSE;
    } while (marked1(m));
    push(a,mark1(pop(a)));
  }
  if (ret  &&  l + *ind < max) {
    do {
      str[(*ind)++]     = *(--s);
    } while (s != string);
    str[(*ind)++]     = '\0';
   }
  else
    ret = FALSE;
  return(ret);
 }
/* ------- initstr ------------ */
/* initialisiere stringstorage  */
initstr()
{
 register int i;

 for ( i = (int)SLEN - 1 ; i >= 0 ; i-- )
    strstor[i] = '\0';
}
#endif /* LETREC */

/* end of      credclr.c */
