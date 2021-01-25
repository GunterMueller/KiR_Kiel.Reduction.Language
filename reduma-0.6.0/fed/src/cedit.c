/* der reduktions-editors          cedit.c  */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    29.08.88                     */

#define IS_EDITOR 1
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
#include "cback.h"
#include "cfiles.h"
#include "cparm.h"
#include "cedstate.h"
#include "cglobals.h"
#include "ccommand.h"

#ifdef X_UDI
#include "udi-EditDefines.h"
extern int INPUT_LINE_VISIBLE;
#include <signal.h>
#include <sys/ioctl.h>
#endif

#define ZIEMLICH_VIEL	100000000
#define NO_OPERATION	255

/* externe routinen       */
extern void proc_time();

#ifdef X_UDI
extern int SERVER_MODE;
#endif

extern int Counter,rca;
/* nicht in globals, da nur temporaer benutzt        */
char msg[80];
long int lastdate;	/* letztes aufrufdatum       */

/* ---------- error -------------------------------- */
void error(m)	/* fehler-abbruch              */
 char *m;
 {
  message(MROW,MCOL,m);
  getunget();
  (void)switchterm(LINEMODE);
  exit(1);
 }
/* ---------- usage -------------------------------- */
static void usage()
 {
  extern char initfile[];

#if SINTRAN3
  fputs("usage: nd reduma [options] {<files>}\n",stdout);
#else
  fputs("usage: reduma [options] {<files>}\n",stdout);
#endif
  fputs("options: -n: show news\n",stdout);
#if !LETREC
  fputs(" -f: Function-Version\n",stdout);
  fputs(" -a: AP-Version\n",stdout);
#endif
  fputs(" -c: compare two files after reducing the first (no editing !)\n",
        stdout);
  fputs(" -p: print all files in prettyprint format\n",stdout);
  fputs(" -t: reduce file and print execution time\n",stdout);
  fputs(" -o <outfile>: reduce file and write result to <outfile>\n",stdout);
  fprintf(stdout," -i <file>: use <file> for initialisation [default %s]\n",
          initfile);
 }
/* -------- init_back ------------------------------ */
/* initialisiere backup-stacks                       */
void init_back(n)
 int n;
 {
  if (n) {
    clearstack(SST);
    clearstack(S1ST);
    clearstack(S2ST);
    clearstack(S3ST);
    clearstack(S4ST);
    clearstack(S5ST);
  }
  push(SST,PLACEH | toedit6(start_eno));
  push(S1ST,PLACEH | toedit6(start_eno));
  push(S2ST,PLACEH | toedit6(start_eno));
  push(S3ST,PLACEH | toedit6(start_eno));
  push(S4ST,PLACEH | toedit6(start_eno));
  push(S5ST,PLACEH | toedit6(start_eno));
 }

/* --------- clearstacks -------------------------------------------- */
/* erzeuge die stacks                                                  */
static void clearstacks(all)
 int all;
 {

  clearstack(EST); clearstack(AST);
  clearstack(VST); clearstack(BST);
  clearstack(U1ST); clearstack(U2ST); clearstack(U3ST);
  clearstack(MST); clearstack(M1ST); clearstack(M2ST);
  clearstack(RST); clearstack(R1ST); clearstack(R2ST);
  clearstack(EADRST); clearstack(AADRST);
  clearstack(S1ST); clearstack(S2ST);clearstack(S3ST);
  clearstack(S4ST); clearstack(S5ST);
  if (all) clearstack(SST);
 }

/* --------- createstacks -------------------------------------------- */
/* erzeuge die stacks                                                  */
createstacks()
 {

 return(crstack(EST,STKPAGE) && crstack(AST,STKPAGE)  && crstack(VST,STKPAGE) &&
   crstack(BST,STKPAGE) && crstack(U1ST,STKPAGE) && crstack(U2ST,STKPAGE) && crstack(U3ST,STKPAGE) &&
   crstack(MST,STKPAGE) && crstack(M1ST,STKPAGE) && crstack(M2ST,STKPAGE) &&
   crstack(SST,STKPAGE) && crstack(S1ST,STKPAGE) && crstack(S2ST,STKPAGE) &&
   crstack(S3ST,STKPAGE) && crstack(S4ST,STKPAGE) && crstack(S5ST,STKPAGE) &&
   crstack(RST,STKPAGE) && crstack(R1ST,STKPAGE) && crstack(R2ST,STKPAGE) &&
   crstack(EADRST,STKPAGE) && crstack(AADRST,STKPAGE) );
 }
/* ------------- rdfile ---------------------------------------------- */
rdfile(fn)
 char *fn;
 {
  register int c;

  c = loadexpr(U1ST,fn,1,FN_ED);
  res_read(c,1,fn,U1ST,U2ST,TRUE);
#if !LETREC
  if (c == ILLVERS) {
    if (swt_vers()) {
      sprintf(msg,"%s: Trying another version\n",exednm);
      message(MROW,MCOL,msg);
      clearstack(EST); clearstack(U1ST);
      push(EST,PLACEH | toedit6(start_eno));
      c = loadexpr(U1ST,fn,1,FN_ED);
      res_read(c,1,fn,U1ST,U2ST,TRUE);
     }
    else {
      sprintf(msg,"%s: Can't switch to another version\n",exednm);
      message(MROW,MCOL,msg);
      return(FAILURE);
    }
  }
#endif
  return(c);
 }
/* --------- editor -------------------------------------------------- */
/* der editor                                                          */
editor(state)
 P_EditState state;
 {
  extern void help();
  extern int Changes,news,def_redcnt;
  extern char initfile[],* strcpy();
  long int initedit(),initparms(),newdate;
  int compare,time,preld,v_opt,erg,print,out;
  register int c,ok;
  register char *s;
  int argc,lastc;
  char **argv,outfile[LENFN];


  erg = SUCCESS;
  if ( state == (P_EditState)0 ) {	/* kein zustand uebergeben ?        */
    state = createdst();	/* versuche neuen zustand zu erzeugen */
    if (state == (P_EditState)0) return(FAILURE);	/* fehler ?       */
    state->neu_  = ED_NEU;
    state->argc_ = 0;
  }
  if (state->neu_ == ED_INIT_TTY) {	/* ausdruck von reduma holen        */
    restoreedst(state);		/* hole zustand zurueck	*/
#if UNIX
    if (!termcaps(TRUE)) {
      fprintf(stderr,"%s: can't init terminal\nStop\n",exednm);
      exit(9);
    }
#endif
    SETSTATUS(state,ST_TERM);
#if LETREC
    v_opt     = TRUE;
#else
    v_opt     = FALSE;
#endif
    newdate   = initedit(TRUE);	/* setzt div. filenamen, newdate  */
    SETSTATUS(state,ST_EDIT);
    batch     = TRUE2;
    compile_only = FALSE;
    argc = state->argc_;
    argv = state->argv_;
    if (argc) exednm = *argv;
    else exednm = "reduma";
    while (--argc > 0  &&  (*++argv)[0] == '-') {
     for (s = argv[0]+1; *s != '\0'; s++) {
      switch (*s) {
#if !LETREC
        case 'a': edit_vers = AP_VERSION;    v_opt = TRUE;  break;
        case 'f': edit_vers = FCT_VERSION;   v_opt = TRUE;  break;
#endif
        case 'n': news      = TRUE;          break;
/* unused ??
        case 'c': compare   = batch = TRUE;  break;
        case 't': time      = batch = TRUE;  break;
        case 'p': print     = batch = TRUE;  break;
*/
        case 'i':
          s = *++argv;
          --argc;
          if (argc > 0  &&  *s) strcpy(initfile,s);
          else {
            fprintf(stdout,"%s: missing initfile\n",exednm);
            return(FAILURE);
          }
          *s-- = '\0';	/* damit innere schleife abbricht ! */
          break;
        default : fprintf(stdout,"%s: illegal option %c\n",exednm,*s);
                  return(FAILURE);
      }
     }	/* end for        */
     if (*(argv[0]+1) == '\0') {
       fprintf(stdout,"%s: missing option\n",exednm);
       return(FAILURE);
     }
    }


    if (batch == TRUE2) batch = FALSE;
    lastdate  = initparms(&Parms,TRUE,initfile,v_opt);	/* liest red.parameter, lastdate  */
    news = newdate > lastdate;
    SETSTATUS(state,ST_REDP);
    SETSTATUS(state,ST_EDP);
    rca = state->Parms_.redcnt;
    state->result_ = ED_EXIT;
    storeedst(FALSE,state);	/* zustand speichern	*/
    return(SUCCESS);
   }	else
  if (state->neu_ == ED_NEU) {
    restoreedst(state);		/* hole zustand zurueck	*/
    state->result_ = ED_EXIT;
    batch     = TRUE2;
    ok        = TRUE;
/*
#if UNIX
    if (!(EDSTATUS(state,ST_TERM))) {
      if (!termcaps(TRUE)) {
        fprintf(stderr,"%s: can't init terminal\nStop\n",exednm);
        exit(9);
      }
    }
#endif
*/
    fprintf(stdout,"RED Version %s\n",VERSION);
    edit_vers = FCT_VERSION;
    compare   = FALSE;	/* vergleich von zwei dateien       */
    time      = FALSE;	/* reduzieren einer datei mit time  */
    out       = FALSE;	/* reduzieren einer datei und ausgeben  */
    preld     = FALSE;	/* reduzieren einer datei mit preladen  */
    compile_only = FALSE;
#if LETREC
    v_opt     = TRUE;
#else
    v_opt     = FALSE;
#endif
    print     = FALSE;	/* ausgabe von dateien     */
    msg[0]    = '\0';
    newdate = initedit(!(EDSTATUS(state,ST_EDIT)));	/* setzt div. filenamen, newdate  */

    argc = state->argc_;
    argv = state->argv_;
    if (argc) exednm = *argv;
    else exednm = "reduma";
    while (--argc > 0  &&  (*++argv)[0] == '-') {
     for (s = argv[0]+1; *s != '\0'; s++) {
      switch (*s) {
#if !LETREC
        case 'a': edit_vers = AP_VERSION;    v_opt = TRUE;  break;
        case 'f': edit_vers = FCT_VERSION;   v_opt = TRUE;  break;
#endif
        case 'n': news      = TRUE;          break;
        case 'c': compare   = batch = TRUE;  break;
        case 't': time      = batch = TRUE;  break;
        case 'b': preld     = batch = TRUE;  break;
        case 'p': print     = batch = TRUE;  break;
        case 'C': compile_only = TRUE;  break;
#ifdef X_UDI
        case 'S': SERVER_MODE=TRUE;break; /* UDI : 2.7.93 */
#endif
        case 'o':
          s = *++argv;
          --argc;
          if (argc > 0  &&  *s) strcpy(outfile,s);
          else {
            fprintf(stdout,"%s: missing outfile\n",exednm);
            return(FAILURE);
          }
          *s-- = '\0';	/* damit innere schleife abbricht ! */
          out = batch = TRUE;
          break;
        case 'i':
          s = *++argv;
          --argc;
          if (argc > 0  &&  *s) strcpy(initfile,s);
          else {
            fprintf(stdout,"%s: missing initfile\n",exednm);
            return(FAILURE);
          }
          *s-- = '\0';	/* damit innere schleife abbricht ! */
          break;
        default : fprintf(stdout,"%s: illegal option %c\n",exednm,*s);
                  return(FAILURE);
      }
     }	/* end for        */
     if (*(argv[0]+1) == '\0') {
       fprintf(stdout,"%s: missing option\n",exednm);
       return(FAILURE);
     }
    }

#ifdef X_UDI
    if (SERVER_MODE) { /* Run programm in server mode ? */
     if (!(InstallServer())) {
      fprintf(stdout,"%s: can't install server for editor\n",exednm);
      return(FAILURE);
     }
    }
#endif

    if (batch == TRUE2) batch = FALSE;
#if UNIX
    if (!batch) {
      if (!(EDSTATUS(state,ST_TERM))) {
        if (!termcaps(TRUE)) {
          fprintf(stderr,"%s: can't init terminal\nStop\n",exednm);
          exit(9);
        }
        SETSTATUS(state,ST_TERM);
      }
    }
#endif
    init_back(FALSE);
    if (!(EDSTATUS(state,ST_REDP) || EDSTATUS(state,ST_EDP))) {
      lastdate  = initparms(&Parms,TRUE,initfile,v_opt);	/* liest red.parameter, lastdate  */
      rca = state->Parms_.redcnt;
      news = newdate > lastdate;
    }
    if (news) {
      lastdate = newdate;
      if (!saveparms(lastdate)) {
        fprintf(stdout,"%s: can't open %s for write !\n",exednm,initfile);
      }
    }
    if (compare || time || preld || out) {
      if (compare && argc == 2  ||  (time || preld || out)  && !compare) {
        char *fn1,*fn2;

        push(EST,PLACEH | toedit6(start_eno));
        fn1 = *argv;
        c = rdfile(fn1);
        if (!RD_OK(c)) return(FAILURE);
        if (b_initred() == FAILURE) return(FAILURE);
        Changes = YES;
#if !LETREC
        if (isfctvers) b_scrlgoal();
#endif
        if (preld) {
          if (redc(def_redcnt,0,0,isfctvers,0,RED_PRELOAD) != SUCCESS) {
            message(MROW,MCOL,msg[0] ? msg : "error in reduce");
            getunget();	/* warten auf tastendruck  */
            clrmsg(MROW,MCOL);	/* loeschen                */
          }
         }
        else {
          if (redc(def_redcnt,0,0,isfctvers,0,RED_ALL) != SUCCESS) {
            message(MROW,MCOL,msg[0] ? msg : "error in reduce");
            getunget();	/* warten auf tastendruck  */
            clrmsg(MROW,MCOL);	/* loeschen                */
          }
        }
        if (time || preld) proc_time();
        if (out) {
          c = saveexpr(EST,outfile,1,FN_ED);
          res_save(c,1,editfil[FN_ED]);
        }
        if (compare) {
	/* zurueck ? */
          transport(EST,U1ST);
          transport(U1ST,BST);
          ++argv;	/* unused:         --argc;	*/
          clearstack(EST); clearstack(U1ST); clearstack(U2ST);
          push(EST,PLACEH | toedit6(start_eno));
          fn2 = *argv;
          c = loadexpr(U1ST,fn2,1,FN_ED);
          res_read(c,1,fn2,U1ST,U2ST,TRUE);
          if (!RD_OK(c)) return(FAILURE);
          copywhole(VST);
          if (findp(BST,VST)) {
            fprintf(stdout,"Expressions in %s and %s are identical\n",fn1,fn2);
           }
          else {
            fprintf(stdout,"ERROR: Expressions in %s and %s are not identical\n"
                    ,fn1,fn2);
          }
          clearstack(BST);
          clearstack(VST);
        }
        return(OK_NOTC);
       }
      else {
        fprintf(stdout,"%s: Wrong number (%d) of arguments\n",exednm,argc);
        usage();
        return(FAILURE);
      }
     }         else
    if (print) {
      char *fn1;
      extern int width;

      while (argc > 0) {
        clearstack(EST);
        clearstack(U1ST);
        clearstack(U2ST);
        push(EST,PLACEH | toedit6(start_eno));
        fn1 = *argv;
        c = rdfile(fn1);
        if (!RD_OK(c)) return(FAILURE);
        if (prettyprint(fn1,"w",width,PPREPLACE,FN_PP) != SUCCESS) {
          fprintf(stdout,"error in prettyprint %s\n",fn1);
        }
        ++argv;         --argc;
      }
      return(OK_NOTC);
     }         else
    if (argc > 0) {
      if (argc == 1) {
        fprintf(stdout,"Please wait\n");
        push(EST,PLACEH | toedit6(start_eno));
        c = rdfile(*argv);
        if (!RD_OK(c)) return(FAILURE);
       }
      else {
        fprintf(stdout,"%s: Too many arguments\n",exednm);
        return(FAILURE);
      }
     }
    else {
      push(EST,PLACEH | toedit6(start_eno));
    }
    if (batch == TRUE2) batch = FALSE;
#if UNIX
    if (!batch) {
      if (!(EDSTATUS(state,ST_TERM))) {
        if (!termcaps(TRUE)) {
          fprintf(stderr,"%s: can't init terminal\nStop\n",exednm);
          exit(9);
        }
        SETSTATUS(state,ST_TERM);
      }
    }
#endif
    init_back(FALSE);
    if (!switchterm(RAWMODE) && isatty(0)) {
      fprintf(stderr,"%s: can't switch terminal\nStop\n",exednm);
      exit(8);
    }

    if (news) help("news",FALSE);
   }	else
  if (state->neu_ == ED_RED_FROM) {	/* ausdruck von reduma holen        */
    restoreedst(state);	/* editor-zustand laden             */
    if (!switchterm(RAWMODE)) {
      fprintf(stderr,"%s: can't switch terminal\nStop\n",exednm);
      exit(8);
    }
	/* was ist mit fehlern beim aufruf der reduma ?                    */
    ok = redc(rca,0,0,isfctvers,0,RED_FROM) == SUCCESS;	/* ausdr. holen*/
   }	else
  if (state->neu_ == ED_OLD) {	/* ausdruck von backupstack holen        */
    restoreedst(state);	/* editor-zustand laden             */
    if (!switchterm(RAWMODE)) {
      fprintf(stderr,"%s: can't switch terminal\nStop\n",exednm);
      exit(8);
    }
    if (ok = !isempty(SST)) {
      Changes = YES;
      transport(SST,EST);
      initscreen();	/* bildschirm init.        */
      display(TRUE);	/* ausdruck anzeigen       */
      argc = state->argc_;
      argv = state->argv_;
      s = argv[argc];
      message(MROW,MCOL,s);
      getunget();	/* warten auf tastendruck  */
      clrmsg(MROW,MCOL);	/* loeschen                */
    }
   }
  else {
    restoreedst(state);	/* editor-zustand laden             */
    if (!switchterm(RAWMODE)) {
      fprintf(stderr,"%s: can't switch terminal\nStop\n",exednm);
      exit(8);
    }
    ok = TRUE;	/* nicht neu      */
  }
  if (ok) {
    state->result_ = ED_EXIT;
    if (state->neu_ == ED_NEU) {
/*    init_back(FALSE);	/* backup-stacks init.     */
/*    switchterm(RAWMODE);	/* terminal einstellen     */
      initscreen();	/* bildschirm init.        */
      display(TRUE);	/* ausdruck anzeigen       */
      vers_msg();	/* versions-meldung        */
      getunget();	/* warten auf tastendruck  */
      clrmsg(MROW,MCOL);	/* loeschen                */
    }
    do {	/* haupt-schleife          */
#ifdef X_UDI
      INPUT_LINE_VISIBLE=0;
      lastc=c;
#endif
      c = getkey();
#ifdef X_UDI
      INPUT_LINE_VISIBLE=1;
#endif
      if (isascii(c)  &&  isprint(c) && (c != ':')) {
#ifdef X_UDI
        if (UDI_CHECK_MODE(S_TTY_READONLY)) {
         int bytes=0;
         if ((ioctl(0,FIONREAD,&bytes)!=-1)&&(bytes>0)) { /* Skip bytes */
          while (bytes>0) {getchar();bytes--;} /* empty stdin buffer */
         }
        } else {
#endif
         unget(c);	/* zeichen zurueckstellen  */
         *lbuf = c;	/* erstes zeichen eintragen*/
         lpos = lmxpos = 0;	/* init. fuer input        */
         (void)input(lbuf,LBUFLEN,TRUE);	/* eingabe eines ausdrucks */
        }
#ifdef X_UDI
       }
#endif
      else {
        if (c == T_FKT) {	/* fktstasten-simulation   */
          int fkey ;
          clrmsg(MMROW,MMCOL);
          message(MMROW,MMCOL,"- Function Key -") ;	/* oder mesg(,,,?,?) ? */
          clrmsg(INROW,INCOL);
          *lbuf = 'F';
          getline(lbuf,LBUFLEN,1,1,INROW,INCOL,INROWLEN );
          sscanf(lbuf,"F%d",&fkey) ;
          switch(fkey) {
             case 1  : c = F1;  break;
             case 2  : c = F2;  break;
             case 3  : c = F3;  break;
             case 4  : c = F4;  break;
             case 5  : c = F5;  break;
             case 6  : c = F6;  break;
             case 7  : c = F7;  break;
             case 8  : c = F8;  break;
             case 9  : c = SHIFT_F1;  break;
             case 10 : c = SHIFT_F2; break;
             case 11 : c = SHIFT_F3; break;
             case 12 : c = SHIFT_F4; break;
             case 13 : c = SHIFT_F5; break;
             case 14 : c = SHIFT_F6; break;
             case 15 : c = SHIFT_F7; break;
             case 16 : c = SHIFT_F8; break;
             default : c = ':' ;	/*   Fehlerbehandlung s. u. */
           }
          clrmsg(INROW,INCOL);
          clrmsg(MMROW,MMCOL);
#ifdef X_UDI
          if ((UDI_CHECK_MODE(S_TTY_COMMANDLINE))&&(c==F8)) goto UDILABEL;
          if ((c==F1)||(c==SHIFT_F1)) {
           if (!UDI_CHECK_MODE(S_TTY_EXECUTE)) c=NO_OPERATION;
           goto UDILABEL;
          }
          if (!UDI_CHECK_MODE(S_TTY_FUNCTIONKEYS)) c=NO_OPERATION;
          UDILABEL : ;
#endif
         }
         switch(c) {
            case NO_OPERATION : break;
#if X_WINDOWS
            case MouseLeft : {
              int x,y;

              Mouse(&x,&y);
              position(x,y,TRUE);
              break;
            }
#endif
#ifdef TABLEF
            case TABLEF :
#endif

            case UP      : /* Modified cursor movement by UDI 12.8.93 */
             switch (XUDI_up) {
              case 2     : /* One line down and to actual expr */
               if (srow>3) position(scol-1,srow-1,TRUE);
               else scrollup(TRUE);
               Changes=YES;
               break;
              case 1     : /* One line down and to first expr */
               if (srow>3) position(0,srow-1,TRUE);
               else scrollup(TRUE);
               Changes=YES;
               break;
              default    :
               scrollup(TRUE); Changes = YES; break;
             }
             break;

            case DOWN    :
             switch (XUDI_down) {
              case 2     : /* One line down and to actual expr */
               if (srow<li) position(scol,srow+1,TRUE);
               else rbscroll(TRUE);
               Changes=YES;
               break;
              case 1     : /* One line down and to first expr */
               if (srow<li) position(0,srow+1,TRUE);
               else rbscroll(TRUE);
               Changes=YES;
               break;
              default    :
               rbscroll(TRUE); Changes = YES; break;
             }
             break;

#ifdef TABRIG
            case TABRIG  :
#endif
            case _DOWN   : scrolldown(TRUE);
                           Changes = YES;
                           break;

            case CR      :
#ifdef X_UDI
             switch (XUDI_return) {
              case 1  : /* Expand actual expression to display */
               fdisplay(TRUE);
               Changes=YES;
               break;
              case 2  : /* Edit expression with teobject */
               if (lastc!=CR) CallTextEditObj();
               break;
              case 3  : /* Carriage return */
               position(0,srow,TRUE);
               Changes=YES;
               break;
              case 4  : /* Line feed */
               if (srow<li) position(scol-1,srow+1,TRUE);
               else rbscroll(TRUE);
               Changes=YES;
               break;
              case 5  : /* Carriage return + line feed */
               if (srow<li) position(0,srow+1,TRUE);
               else rbscroll(TRUE);
               Changes=YES;
               break;
              default : /* Curor right : default action */
               scrollright(TRUE);
               Changes = YES;
               break;
             }
             break;
#else
#endif
            case RIGHT   :
#ifdef X_UDI
             switch (XUDI_right) {
              case 1     : /* */
              default    :
               scrollright(TRUE); Changes = YES; break;
             }
             break;
#else
             scrollright(TRUE); Changes = YES; break;
#endif

            case LEFT    :
#ifdef X_UDI
             switch (XUDI_left) {
              case 1     : /* */
              default    :
               scrolleft(TRUE); Changes = YES; break;
             }
             break;
#else
             scrolleft(TRUE); Changes = YES; break;
#endif

            case HOME    : 
#ifdef X_UDI
             switch (XUDI_home) {
              case 1     : /* Programm home */
               scrollhome(TRUE);scrollhome(TRUE);Changes=YES;break;
              default    :
               scrollhome(TRUE); Changes = YES; break;
             }
             break;
#else
             scrollhome(TRUE); Changes = YES; break;
#endif

#ifdef PGDOW
            case PGDOW   :
#endif
            case _GOAL   : scrollgoal(TRUE);
                           Changes = YES;
                           break;
            case DEL     :
#ifdef X_UDI
                           if (!UDI_CHECK_MODE(S_TTY_READONLY))
#endif
                           delexp();
                           Changes = YES;
                           break;
#ifdef DISPL
            case DISPL   :
#endif
            case _displ  : display(TRUE);
                           break;
#ifdef FDISPL
            case FDISPL  :
#endif
            case _displf : fdisplay(TRUE);
                           break;
            case _pick   : pickexpr(SST);
                           break;
            case _pick1  : pickexpr(S1ST);
                           break;
            case _pick2  : pickexpr(S2ST);
                           break;
            case _put    : putexpr(SST);
                           Changes = YES;
                           break;
            case _put1   : putexpr(S1ST);
                           Changes = YES;
                           break;
            case _put2   : putexpr(S2ST);
                           Changes = YES;
                           break;
            case _wrfile : writefile(EST);
                           break;
            case _rdfile : infile();
                           Changes = YES;
                           break;
#if B_DEBUG
            case TRACEN  : debug = getkey()-'0';  break;
#endif
#ifdef _RED1
            case _RED1:
#endif
            case _red1   :
             if (redc(def_redcnt,0,0,isfctvers,0,RED_ALL) != SUCCESS) {
               message(MROW,MCOL,msg[0] ? msg : "error in reduce");
               getunget();	/* warten auf tastendruck  */
               clrmsg(MROW,MCOL);	/* loeschen                */
             }
                           break;
#ifdef _RED1000
            case _RED1000:
#endif
            case _red1000:
             if (redc(ZIEMLICH_VIEL,0,0,isfctvers,0,RED_ALL) != SUCCESS) {
               message(MROW,MCOL,msg[0] ? msg : "error in reduce");
               getunget();	/* warten auf tastendruck  */
               clrmsg(MROW,MCOL);	/* loeschen                */
             }
                           break;
#ifdef _COMMAND
            case _COMMAND:
#endif
            case _command:
              if ((c = getcmd(lbuf,LBUFLEN,lastdate)) == CALL_RED
                  || c == SAVE_GRAPH || c == EXIT_GRAPH) {
                 if (c == SAVE_GRAPH) {
                   message(MROW,MCOL,"Saving expression !");
                   clearstacks(FALSE);
                  }	else
                 if (c == EXIT_GRAPH) {
                   message(MROW,MCOL,"Exit from editor !");
                 }
                 storeedst(FALSE,state);	/* zustand speichern	*/
                 state->result_ = c;
                 c = CNTZ;
                 (void)switchterm(LINEMODE);
               }
               break;
            case EXIT  :
            case CNTC  :
               CLEARSCREEN();
            case CNTZ  :
               c = CNTZ;
               if (stopred() != SUCCESS) {
                 message(MROW,MCOL,"Error stopping reduma, halting anyway !");
               }
               (void)switchterm(LINEMODE);
               break;
            case HELP  :
               help("help",TRUE);break;
            case ':'   :
            default    :
               beep();
               message(MROW,MCOL,"No Function for that Key !");
               getunget();
               clrmsg(MROW,MCOL);
               break ;
          }
       }
    } while (c != CNTZ);
   }
  else {
    fprintf(stdout,"%s: Not enough space for stacks\n",exednm);
    erg = FAILURE;
  }
  return(erg);
 }
/* end of        cedit.c  */


