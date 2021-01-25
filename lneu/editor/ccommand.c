/* commands					ccommand.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#define iscmd  1 /* for commands	*/

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"
#include "cedit.h"
#include "cencod.h"
#include "cparm.h"
#include "ccommand.h"
#include "ckwtb.h"
#include "cetb.h"
#include "cfiles.h"
#include "cedstate.h"
#include "cglobals.h"

#define CLSI() { CLEARSCREEN(); fflush(stdout); }

/* external functions	*/
extern int cat();
extern void help();
extern STACKELEM checkf();

extern char msg[],initfile[],* tormsg[];
extern int dmode,small,def_redcnt,doub_def;

char protfil[LENFN];
int width = FCOLS;
int lastred = L_UNDEFINED;

#ifdef UnBenutzt

/* -------- upper -----------	*/
void upper(s)
 char *s;
 {
  register char c;

  while (c = *s) {
    if (islower(c)) *s = toupper(c);
    ++s;
  }
 }
#endif /* UnBenutzt	*/

/* -------- refresh -----------	*/
void refresh()
 {
   CLSI();    /* new 17.12.87	*/
   initscreen();
   dispfa();
 }
#if X_WINDOWS

/* -------- resize -----------	*/
void resize(x,y)
 int x,y;
 {
  li = y;
  co = x;
  refresh();
 }
#endif

/* -------- getonoff ----------------------------------------	*/
static int getonoff(buf)
 char *buf;
 {
  register int c;
  int d;

  if (isdigit(*buf)) {
    if (sscanf(buf,"%d",&d) == 1) {
      c = d != FALSE;
     }
    else c = FAILURE;
   }
  else {
    if (*buf == 'o') c = *(buf+1) == 'n';   /* attention: no on/off !	*/
    else             c = FAILURE;
  }
  return(c);
 }

/* -------- chgdm -----------	*/
void chgdm(type,buf)
 int type;
 char *buf;
 {
  register int c;

  if (ISTOP) {
    if ((c = getonoff(buf)) != FAILURE) {
      if (c) small |= type;
      else   small &= ~type;
      dispfa();
    }
    switch(type) {
      case DDECL :
        sprintf(msg,"abbreviate defines is %s",IS_DDECL(small) ? "on" : "off");
        break;
      case DSMALL :
        sprintf(msg,"small %s",IS_SMALL(small) ? "on" : "off");
        break;
      case DGOAL :
        sprintf(msg,"show goal is %s",IS_DGOAL(small) ? "on" : "off");
        break;
      case DFCT :
        sprintf(msg,"show fct-list is %s",IS_DFCT(small) ? "on" : "off");
        break;
      case DTDECL :
        sprintf(msg,"abbreviate only inner defines is %s",IS_DTDECL(small) ? "on" : "off");
        break;
      case DVERT :
        sprintf(msg,"vertical display is %s",IS_DVERT(small) ? "on" : "off");
        break;
      case DVERT1 :
        sprintf(msg,"vertical display 1 is %s",IS_DVERT1(small) ? "on" : "off");
        break;
      case DVERT2 :
        sprintf(msg,"vertical display 2 is %s",IS_DVERT2(small) ? "on" : "off");
        break;
      default : msg[0] = '\0';
        break;
    }
    message(MROW,MCOL,msg);
   }
  else message(MROW,MCOL,"Can't switch at this position");
 }

/* -------- vers_msg ----------------------------------------	*/
vers_msg()
 {
  sprintf(msg,"RED %s %s- Version",VERSION,EDIT_VERS);
  message(MROW,MCOL,msg);
 }

/* -------- swt_vers ----------------------------------------	*/
swt_vers()
 {
  register int erg;

#if !LETREC
   if (isempty(AST)  &&  isempty(MST)  &&
       (isempty(EST)  ||  noedit(topof(EST)) == PLACEH)) {
     edit_vers = isapvers ? FCT_VERSION : AP_VERSION;
     if (!isempty(EST)) {
       push(EST,(pop(EST) & ~EDT6_FIELD) | toedit6(start_eno));
       init_back(TRUE);
     }
     erg = TRUE;
    }
   else erg = FALSE;
#else
   erg = FALSE;           /* if LETREC : no switch !	*/
#endif
   return(erg);
 }

/* -------- editparms -----------	*/
void editparms(fp)
 FILE *fp;
 {
  register int i;
  register char *newl;

  if (fp == stdout) {
    fputs("Editor Parameters:\r\n\n",fp);
    newl = "\r\n";
   }
  else newl = "\n";
  fprintf(fp,"RED %s %s- Version%s",VERSION,EDIT_VERS,newl);
  for (i = 0 ; i < FN_ANZ ; i++ ) {
    fprintf(fp,"%s-Lib is \"%s\"%s",editname[i],editlib[i],newl);
  }
  fputs(newl,fp);
  for (i = 0 ; i < FN_ANZ ; i++ ) {
    fprintf(fp,"%s-Extension is \"%s\"%s",editname[i],editext[i],newl);
  }
  fputs(newl,fp);
  if (fp == stdout) {
    fprintf(fp,"Small is %s    ",IS_SMALL(small) ? "on" : "off");
    fprintf(fp,"Dmode : %d     ",dmode);
    fprintf(fp,"Width : %d     ",width);
    fputs(newl,fp);
    fprintf(fp,"abbreviate def is %s   ",IS_DDECL(small) ? "on" : "off");
    fprintf(fp,"abbreviate only inner def is %s ",IS_DTDECL(small) ? "on" : "off");
    fputs(newl,fp);
    fprintf(fp,"show goal is %s   ",IS_DGOAL(small) ? "on" : "off");
    fprintf(fp,"show fct-list is %s ",IS_DFCT(small) ? "on" : "off");
    fputs(newl,fp);
    fprintf(fp,"vertical display is %s   ",IS_DVERT(small) ? "on" : "off");
    fputs(newl,fp);
    if (protfil[0]) {
       fprintf(fp,"protocoll-file is %s ",protfil);
       fprintf(fp,"type %d",PP_ASC(protocoll) ? FN_ASC : FN_PRT);
       if (PROT_PREV(protocoll) && PROT_AFTER(protocoll))
          fprintf(fp," before and after reduction");
       else
       if (PROT_PREV(protocoll))
          fprintf(fp," before reduction");
       else
       if (PROT_AFTER(protocoll))
          fprintf(fp," after reduction");
     }
    else fprintf(fp,"protocoll is off");
   }
  else {
    fprintf(fp,"Small : %d\n",IS_SMALL(small) ? 1 : 0);
    fprintf(fp,"Dmode : %d\n",dmode);
    fprintf(fp,"Width : %d\n",width);
    fprintf(fp,"abbreviate def : %d\n",IS_DDECL(small) ? 1 : 0);
    fprintf(fp,"abbreviate only inner def : %d\n",IS_DTDECL(small) ? 1 : 0);
    fprintf(fp,"show goal : %d\n",IS_DGOAL(small) ? 1 : 0);
    fprintf(fp,"show fct-list : %d\n",IS_DFCT(small) ? 1 : 0);
    fprintf(fp,"vertical display : %d\n",IS_DVERT(small) ? 1 : 0);
  }
  fputs(newl,fp);
  if (fp == stdout) fflush(stdout);
}

/* -------- proc_time ----------	*/
void proc_time()
{
  if (lastred == L_REDUCED) {
    fputs("Time used by last reduction:\r\n\n",stdout);
    fprintf(stdout,"pre-processing-time : %4.2f\r\n",Parms.preproc_time);
    fprintf(stdout,"processing-time     : %4.2f\r\n",Parms.process_time);
    fprintf(stdout,"post-processing-time: %4.2f\r\n",Parms.postproc_time);
    fprintf(stdout,"total-time          : %4.2f\r\n",Parms.total_time);
   }	else
  if (lastred == L_PRELOAD) {
    fputs("Time used by last preload:\r\n\n",stdout);
    fprintf(stdout,"pre-load-time : %4.2f\r\n",Parms.preproc_time);
    fprintf(stdout,"waiting-time  : %4.2f\r\n",Parms.process_time);
   }	else
  if (lastred == L_PREPROC) {
    fputs("Time used by last pre-processing:\r\n\n",stdout);
    fprintf(stdout,"pre-processing-time : %4.2f\r\n",Parms.preproc_time);
   }
  else {
    fputs("No reduction made !\r\n\n",stdout);
  }
  fflush(stdout);
}

/* -------- redparms -----------	*/
static void redparms()
{
  fputs("Reduction Machine Parameters:\r\n\n",stdout);
  fprintf(stdout,"pagesize           : %8d        ",Parms.pg_size);
  fprintf(stdout,"redcount           : %8d\r\n",Parms.redcnt);
  fprintf(stdout,"div_precision      : %8d        ",Parms.precision);
  fprintf(stdout,"mult_precision     : %8d\r\n",Parms.prec_mult);
  fprintf(stdout,"truncation         : %8d\r\n",Parms.truncation);
/*
  fprintf(stdout,"indentation        : %8d        ",Parms.indentation);
*/
  fprintf(stdout,"heapdes            : %8d        ",Parms.heapdes);
  fprintf(stdout,"heapsize           : %8d\r\n",Parms.heapsize);
/*
  fprintf(stdout,"stackdes           : %8d\r\n",Parms.stackdes);
  fprintf(stdout,"stackpage          : %8d        ",Parms.stackpage);
  fprintf(stdout,"stackcntpage       : %8d\r\n",Parms.stackcntpage);
*/
  /* COMBINATOR	*/
  fprintf(stdout,"qstacksize (e,a,h) : %8d        ",Parms.qstacksize);
  fprintf(stdout,"mstacksize (m,m1)  : %8d\r\n",Parms.mstacksize);
  fprintf(stdout,"istacksize (i)     : %8d        ",Parms.istacksize);
  fprintf(stdout,"base               : %8d\r\n",Parms.base);
  fprintf(stdout,"scalar format      : %s\r\n",Parms.formated ? "fixformat" : "varformat");
  fprintf(stdout,"beta_count_only    : %s\r\n",Parms.beta_count ? "on" : "off");
  fprintf(stdout,"errorflag          : %s\r\n",Parms.errflag ? "on" : "off");
  fprintf(stdout,"errormessage       : %s\r\n",Parms.errflag ? Parms.errmes : "none");
  if (IS_INFO(D_ERROR,Parms.debug_info))
     fprintf(stdout,"Stop at irreducible expressions\r\n");
  if (IS_INFO(D_STOP,Parms.debug_info))
     fprintf(stdout,"Stop at STOP-expression\r\n");
  if (IS_INFO(D_STOPPED,Parms.debug_info))
     fprintf(stdout,"STOP occurred\r\n");
  if (IS_INFO(D_ERRORED,Parms.debug_info))
     fprintf(stdout,"ERROR occurred\r\n");
  fputs("\r\n",stdout);
  proc_time();
}

/* ----------- getcmd ---------------------------------------	*/
getcmd(buf,buflen,lastdate)
 char *buf;
 int	buflen;
 long lastdate;
 {
  extern char *strcpy(),*skipblanks(),*skipword();
  extern long int initparms();
  extern char initfile[];
  static char fbuf[LENFIL],name[LENFIL];
  int n,c,d,dspl,rca,tr,ftr,back,erg;
  char *cmd,*ptr,docfil[LENFN];
  STACKELEM m,callkon();

  erg = SUCCESS;
  putrev((char)class(topof(EADRST)));
  dspstr(MROW,MCOL,"Please enter command :");
  getline(buf,buflen-1,lpos = 0,lmxpos = 0,INROW,INCOL,buflen-1);
  clrmsg(MROW,MCOL);
  clrmsg(INROW,INCOL);
  setcursor();
  cmd = buf = skipblanks(buf);
  if (*buf) {
   if ((n = cmdsearch(&buf,cmdtb,CMDTBLEN)) != FAILURE) {
     switch(c = cmdtb[n].cnmb) {
      case c_special:
        dspl  = FALSE;               /* display stack after reduction	*/
        rca   = 1;                   /* red.-counter	*/
        ftr   = tr    = FALSE;       /* trace (to/from-red)	*/
        back  = isfctvers;           /* retranslation	*/
        if (*buf  && sscanf(buf,"%d",&d) == 1) {
          rca = d;                    /* 1. parameter : red.-counter	*/
          buf = skipword(buf);
          if (*buf && sscanf(buf,"%d",&d) == 1) {
            back  = d;                /* 2. par.: retranslation	*/
            buf = skipword(buf);
            if (*buf && sscanf(buf,"%d",&d) == 1) {
              tr   = d;               /* 3. par.: trace tored	*/
              buf = skipword(buf);
              if (*buf && sscanf(buf,"%d",&d) == 1) {
                dspl = d;             /* 4. par.: display stacks	*/
                buf = skipword(buf);
                if (*buf && sscanf(buf,"%d",&d) == 1) {
                  ftr    = d;         /* 5. par.: trace fromred	*/
                  buf = skipword(buf);
                  /* rest ignored	*/
                }
              }
            }
          }
        }
        if (*buf) {
          sprintf(msg,"Illegal parameter %c",*buf);
          message(MROW,MCOL,msg);
          erg = FAILURE;
         }
        else
          erg = redc(rca,tr,dspl,back,ftr,RED_ALL);
        break;
      case c_transl :
        if (*buf) {
          sprintf(msg,"Illegal parameter %c",*buf);
          message(MROW,MCOL,msg);
          erg = FAILURE;
         }
        else {
          erg = redc(-1,FALSE,FALSE,!isfctvers,FALSE,RED_TO | RED_FROM);
          vers_msg();
        }
        break;
      case c_preload :
      case c_save_graph :
      case c_call_red   :
      case c_reduce     :
             if ((*buf != '\0') && (sscanf(buf," %d ",&d) == 1)) {
                rca = d;
                buf = skipword(buf);
                if ((*buf != '\0') && (sscanf(buf," %d ",&d) == 1)) {
                   Parms.precision = d;
                   buf = skipword(buf);
                   if ((*buf != '\0') && (sscanf(buf," %d ",&d) == 1)) {
                      Parms.truncation = d;
                      buf = skipword(buf);
/*
                      if ((*buf != '\0') && (sscanf(buf," %d ",&d) == 1)) {
                         Parms.indentation = d;
                         buf = skipword(buf);
                      }
*/
                   }
                }
              }
             else rca = def_redcnt;

             if (*buf == '\0') {
               switch(c) {
                 case c_preload : erg = redc(rca,0,0,isfctvers,0,RED_PRELOAD);
                   break;
                 case c_reduce : erg = redc(rca,0,0,isfctvers,0,RED_ALL);
                   break;
                 case c_call_red:
                   if ((erg = redc(rca,0,0,isfctvers,0,RED_TO)) == SUCCESS)
                     erg = CALL_RED;
                   break;
                 case c_save_graph :
                   if (!(n = istop())) {
                     dspstr(MROW,MCOL,"You selected a subexpression ! Are you sure ?");
                     do {
                       n = getkey();
                     } while ( n != 'y'  &&  n != 'n');
                     clrmsg(MROW,MCOL);
                     n = n == 'y';
                   }
                   if (n) {
                     Changes = YES;	/* force re-compilation	*/
                     if ((erg = redc(rca,0,0,isfctvers,0,RED_T)) == SUCCESS)
                       erg = SAVE_GRAPH;
                     else erg = FAILURE;	/* ??	*/
                    }
                   else erg = FAILURE;	/* ??	*/
                   break;
               }
              }
             else {
               message(MROW,MCOL,"Invalid Parameters");
               erg = FAILURE;
             }
             break;
      case c_prepro  :
        name[0] = '\0';
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          getbase(name,FN_PRE);
          strcpy(buf,name);
          message(MROW,MCOL,"Please enter Preproc-filename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          chkfilname(name,FN_PRE);	/* always creat ?	*/
          /* upper(editfil[FN_PRE]);	don't convert name to upper	*/
          if ((c = creat(editfil[FN_PRE],0777)) != FAILURE) {
            Parms.file_desc = c;
            d = edit6(topof(EST));
            erg = redc(rca,0,0,isfctvers,0,RED_PREP);
            (void)close(c);     /* close file	*/
            if (erg == SUCCESS) {
          /*  ptr = editfil[FN_PRE];    use complete filename or	*/
              getbase(name,FN_PRE);
              ptr = name;            /* only partial name ?	*/
              c = *ptr++;
              push(AST,tovalue(TOUPPER(c)) | LSTR0);
              while (c = (int)(*ptr++)) {
                push(AST,tovalue(TOUPPER(c)) | LSTR1);
              }
              transport(AST,EST);
              push(EST,FILPRE | toedit6(d));
              Changes = TRUE;
             }
            else {
              transport(SST,EST);    /* old expression ?	*/
            }
            if (!batch) dispfa();
           }
          else {
            sprintf(msg,"Can't open %s !",editfil[FN_PRE]);
            message(MROW,MCOL,msg);
            erg = FAILURE;
          }
         }
        else {
          message(MROW,MCOL,"Invalid Parameters");
          erg = FAILURE;
        }
        break;
      case c_arity  :
             if (isconstr(topof(EST))) {
                sprintf(msg,"arity = %d",(int)arity(topof(EST)));
              }                              else
             if (notdisp(topof(MST)) &&
                 arity(topof(MST)) == arity(topof(M1ST))) {
                sprintf(msg,"arity = %d",(int)arity(topof(MST)));
                message(MROW,MCOL,msg);
              }
             else {
                sprintf(msg,"CE is an atom");
              }
             message(MROW,MCOL,msg);
             break;
      case c_help   : help(buf,TRUE);                break;
      case c_stack  : switch (*buf++) {
                        case 'e' : switch(*buf++) {
                                      case 'a' : dsplstack(EADRST,"EADR:",0,1);
                                                 break;
                                      default  : dsplstack(EST,"E:",1,1);
                                    }
                                   break;
                        case 'a' : switch(*buf++) {
                                      case 'a' : dsplstack(AADRST,"AADR:",0,1);
                                                 break;
                                      default  : dsplstack(AST,"A:",1,1);
                                    }
                                   break;
                        case 'm' : switch(*buf++) {
                                      case '1' : dsplstack(M1ST,"M1:",1,1);
                                                 break;
                                      case '2' : dsplstack(M2ST,"M2:",1,1);
                                                 break;
                                      default  : dsplstack(MST,"M:",1,1);
                                    }
                                   break;
                        case 'b' : dsplstack(BST,"B:",1,1);
                                   break;
                        case 'u' : dsplstack(U1ST,"U:",1,1);
                                   break;
                        case 'r' : switch(*buf++) {
                                      case '1' : dsplstack(R1ST,"R1:",1,1);
                                                 break;
                                      case '2' : dsplstack(R2ST,"R2:",1,1);
                                                 break;
                                      default  : dsplstack(RST,"R:",1,1);
                                    }
                                   break;
                        case 's' : switch(*buf++) {
                                      case '1' : dsplstack(S1ST,"S1:",1,1);
                                                 break;
                                      case '2' : dsplstack(S2ST,"S2:",1,1);
                                                 break;
                                      default  : dsplstack(SST,"S:",1,1);
                                    }
                                   break;
                        default  : ;
                      }
                     break;
      case c_refresh:
        refresh();    /* new 17.12.87	*/
        break;
      case c_size  :
        sprintf(msg,"Window size is %d x %d",li,co);
        message(MROW,MCOL,msg);
        break;
      case c_shell  : CLSI();
        (void)switchterm(LINEMODE);
        system("sh");
        (void)switchterm(RAWMODE);
        initscreen();
        display(TRUE);
        break;
      case c_cat : CLSI();
        (void)switchterm(LINEMODE);
        if (!cat(buf)) {
          fprintf(stdout,"\r\nError occurred");
        }
        fprintf(stdout,"\r\nHit <CR> to continue ");
        fflush(stdout);
        getkey();
        (void)switchterm(RAWMODE);
        initscreen();
        dispfa();
        break;
      case c_cmd :
        if (*buf != '\0') {
          CLSI();
          (void)switchterm(LINEMODE);
          system(buf);
          fprintf(stdout,"\r\nHit <CR> to continue\n\r");
/*        fflush(stdout);	*/
          getkey();
          (void)switchterm(RAWMODE);
          initscreen();
          dispfa();
         }
        else {
          message(MROW,MCOL,"No system-command received");
        }
        break;
      case c_initparms:
#if LETREC
        initparms(&Parms,TRUE,initfile,TRUE);
#else
        initparms(&Parms,TRUE,initfile,FALSE);
#endif
        break;
      case c_saveparms:
        if (saveparms(lastdate)) {
          sprintf(msg,"Parameters written to %s\n",initfile);
         }
        else {
          sprintf(msg,"Can't write parameters to %s\n",initfile);
        }
        message(MROW,MCOL,msg);
        break;
      case c_redparms : CLSI();
        redparms();
        getkey();
        initscreen();
        dispfa();
        break;
      case c_proc_time: CLSI();
        proc_time();
        getkey();
        initscreen();
        dispfa();
        break;
      case c_redcnt :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
          Parms.redcnt = def_redcnt = c;
        }
        else beep();
        sprintf(msg,"redcnt = %d",Parms.redcnt);
        message(MROW,MCOL,msg);
        break;
      case c_precis :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
          Parms.precision = c;
        }
        else beep();
        sprintf(msg,"precision = %d",Parms.precision);
        message(MROW,MCOL,msg);
        break;
      case c_prec_mult :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
          Parms.prec_mult = c;
        }
        else beep();
        sprintf(msg,"mult_precision = %d",Parms.prec_mult);
        message(MROW,MCOL,msg);
        break;
      case c_trunc  :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
          Parms.truncation = c;
          Changes = YES;
        }
        else beep();
        sprintf(msg,"truncation = %d",Parms.truncation);
        message(MROW,MCOL,msg);
        break;
/*
      case c_indent :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
          Parms.indentation = c;
          Changes = YES;
        }
        else beep();
        sprintf(msg,"indentation = %d",Parms.indentation);
        message(MROW,MCOL,msg);
        break;
*/
      case c_pgsize :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1))
          Parms.pg_size = c;
        else beep();
        sprintf(msg,"pagesize = %d",Parms.pg_size);
        message(MROW,MCOL,msg);
        break;
      case c_fixform: Parms.formated = 1;   /* formatted scalars	*/
        Changes = YES;
        message(MROW,MCOL,"scalar format = fixformat");
        break;
      case c_varform: Parms.formated = 0;   /* unformatted scalars	*/
        Changes = YES;
        message(MROW,MCOL,"scalar format = varformat");
        break;
      case c_parms  :
      case c_hpdes  :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
           Parms.heapdes = c;
           buf = skipword(buf);
           initialize = YES;
           Changes = YES;
        }
        sprintf(msg,"heapdes = %d",Parms.heapdes);
        message(MROW,MCOL,msg);
        break;
      case c_hpsiz  :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
           Parms.heapsize = c;
           buf = skipword(buf);
           initialize = YES;
           Changes = YES;
        }
        sprintf(msg,"heapsize = %d",Parms.heapsize);
        message(MROW,MCOL,msg);
        break;
      case c_qstsiz :
       if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
           Parms.qstacksize = c;
           buf = skipword(buf);
           initialize = YES;
           Changes = YES;
        }
        sprintf(msg,"qstacksize = %d",Parms.qstacksize);
        message(MROW,MCOL,msg);
        break;
      case c_mstsiz :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
           Parms.mstacksize = c;
           buf = skipword(buf);
           initialize = YES;
           Changes = YES;
        }
        sprintf(msg,"mstacksize = %d",Parms.mstacksize);
        message(MROW,MCOL,msg);
        break;
      case c_istsiz :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1)) {
           Parms.istacksize = c;
           buf = skipword(buf);
           initialize = YES;
           Changes = YES;
        }
        sprintf(msg,"istacksize = %d",Parms.istacksize);
        message(MROW,MCOL,msg);
        break;
      case c_base   :
        if ((*buf != '\0') && (sscanf(buf," %d ",&c) == 1))
          if ((10 <= c) && (c <= 65535)) {
            Parms.base = c;
            buf = skipword(buf);
            Changes = YES;
           }
          else beep();
        sprintf(msg,"base = %d",Parms.base);
        message(MROW,MCOL,msg);
        break;
      case c_betacnt:
        if ((c = getonoff(buf)) != FAILURE) {
           Parms.beta_count = c;
           buf = skipword(buf);
           Changes = YES;
        }
        sprintf(msg,"beta_count = %s",Parms.beta_count ? "on" : "off");
        message(MROW,MCOL,msg);
        break;
      case c_small:
        chgdm(DSMALL,buf);
        break;
      case c_ddecl:
        chgdm(DDECL,buf);
        break;
      case c_dgoal:
        chgdm(DGOAL,buf);
        break;
      case c_dfct:
        chgdm(DFCT,buf);
        break;
      case c_dtdecl:
        chgdm(DTDECL,buf);
        break;
      case c_dvert:
        chgdm(DVERT,buf);
        break;
      case c_dvert1:
        chgdm(DVERT1,buf);
        break;
      case c_dvert2:
        chgdm(DVERT2,buf);
        break;
      case c_exit   : unget(CNTC);
                     break;
      case c_ex_graph :
        erg = EXIT_GRAPH;
        break;
      case c_prot   :
        if (*buf) {
          if (scanword(&buf,protfil,LENFN) <= LENFN) {
            sprintf(msg,"Protocoll on");
            while (*buf) {
              switch(*buf) {
                case 'a' : protocoll &= ~PROTPREV;
                  protocoll |= PROTAFTER;
                  break;
                case 'p' : protocoll |= PROTPREV;
                  protocoll &= ~PROTAFTER;
                  break;
                case 'b' : protocoll |= PROTPREV | PROTAFTER;
                  break;
                case 'P' : protocoll &= ~PPASC;
                  break;
                case 'A' : protocoll |= PPASC;
                  break;
                case 'L' : protocoll |= PPLINE;
                  break;
                case 'l' : protocoll &= ~PPLINE;
                  break;
                default : sprintf(msg,"Unknown option %c",*buf);
                  break;
              }
              ++buf;
            }
            message(MROW,MCOL,msg);
           }
          else {
            protfil[0] = '\0';
            message(MROW,MCOL,"Invalid Parameter");
          }
         }
        else {
          protfil[0] = '\0';
          message(MROW,MCOL,"Protocoll off");
        }
        break;
      case c_editlib :
        if (*buf  && sscanf(buf,"%d",&d) == 1  &&  0 <= d  &&  d < FN_ANZ) {
          buf = skipword(buf);
          if (*buf) {
            if (scanword(&buf,fbuf,LENLIB) <= LENLIB) {
              strcpy(editlib[d],fbuf);
             }
            else {         /* scanword faile	*/
            }
           }
          else {  /* buf empty	*/
            /* edit name with getline ?	*/
            strcpy(buf,editlib[d]);
            sprintf(msg,"Please enter %s-Lib",editname[d]);
            message(MROW,MCOL,msg);
            c = getline(buf,LENLIB-1,0,strlen(buf),INROW,INCOL,LENLIB-1);
            if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENLIB) <= LENLIB) {
              strcpy(editlib[d],fbuf);
             }
            else {         /* scanword failed	*/
            }
          }
         }
        else {    /* no parameter	*/
          d = FN_ED;
        }
        sprintf(msg,"%s-Lib is \"%s\"",editname[d],editlib[d]);
        clrmsg(MROW,MCOL);
        message(MROW,MCOL,msg);
        clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;
      case c_editext :
        if (*buf  && sscanf(buf,"%d",&d) == 1  &&  0 <= d  &&  d < FN_ANZ) {
          buf = skipword(buf);
          if (*buf) {
            if (scanword(&buf,fbuf,LENEXT) <= LENEXT) {
              strcpy(editext[d],fbuf);
             }
            else {         /* scanword failed	*/
            }
           }
          else {  /* buf empty	*/
            /* ev. editieren der editext mit getline ?	*/
            strcpy(buf,editext[d]);
            sprintf(msg,"Please enter %s-Extension",editname[d]);
            message(MROW,MCOL,msg);
            c = getline(buf,LENEXT-1,0,strlen(buf),INROW,INCOL,LENEXT-1);
            if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENEXT) <= LENEXT) {
              strcpy(editext[d],fbuf);
             }
            else {         /* scanword failed	*/
            }
          }
         }
        else {    /* no parameter	*/
          d = FN_ED;
        }
        sprintf(msg,"%s-Extension is \"%s\"",editname[d],editext[d]);
        clrmsg(MROW,MCOL);
        message(MROW,MCOL,msg);
        clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;
      case c_initfile:
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(initfile,fbuf);
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,initfile);
          message(MROW,MCOL,"Please enter Initfilename");
          c = getline(buf,LENLIB-1,0,strlen(buf),INROW,INCOL,LENLIB-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENLIB) <= LENLIB) {
            strcpy(initfile,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        sprintf(msg,"Initfilename is \"%s\"",initfile);
        clrmsg(MROW,MCOL);
        message(MROW,MCOL,msg);
        clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;
      case c_load:
        name[0] = '\0';
        d       = 1;
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
            if (*buf && sscanf(buf,"%d",&d) == 1) {
            }
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,editfil[FN_RED]);
          message(MROW,MCOL,"Please enter Inputfilename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          c = loadexpr(U1ST,name,d,FN_RED);
          if (RD_OK(c)) {
            rtransp(U1ST,U2ST);
            fromred(U2ST,U1ST,FALSE);       /* back ?	*/
            transport(U1ST,U2ST);
            Changes = YES;
          }
          res_read(c,1,name,U2ST,U1ST,FALSE);
        }
/*      clrmsg(MROW,MCOL);
        message(MROW,MCOL,msg);
*/      clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;

      case c_store:
        name[0] = '\0';
        msg[0]  = '\0';
        d       = 1;
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
            if (*buf && sscanf(buf,"%d",&d) == 1) {
            }
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,editfil[FN_RED]);
          message(MROW,MCOL,"Please enter Outputfilename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          copysted(EST,RST);       /* attention: clear editinfo	*/
          clearstack(R1ST);
          if ((m = topof(MST)) != STBOTTOM  &&  notdisp(m)  &&
              arity(m) == arity(topof(M1ST))) {
            push(RST,noedit(m));
          }
          c = tored(RST,R1ST);
          if (c >= SUCCESS) {
            c = saveexpr(R1ST,name,d,FN_RED);
            res_save(c,d,editfil[FN_RED]);
           }
          else {
            strcpy(msg,tormsg[-c]);
            clrmsg(MROW,MCOL);
            message(MROW,MCOL,msg);
          }
        }
        clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;

      case c_save:
        name[0] = '\0';
        msg[0]  = '\0';
        d       = 1;
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
            if (*buf && sscanf(buf,"%d",&d) == 1) {
            }
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,editfil[FN_ED]);
          message(MROW,MCOL,"Please enter Outputfilename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          clrmsg(MROW,MCOL);
          dspstr(MROW,MCOL,"Please wait");
          c = saveexpr(EST,name,d,FN_ED);
          res_save(c,d,editfil[FN_ED]);
        }
        clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;

      case c_get:
        name[0] = '\0';
        d       = 1;
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
            if (*buf && sscanf(buf,"%d",&d) == 1) {
            }
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,editfil[FN_ED]);
          message(MROW,MCOL,"Please enter Inputfilename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          clearstack(U1ST); clearstack(U2ST);
          c = loadexpr(U1ST,name,d,FN_ED);
          res_read(c,d,name,U1ST,U2ST,FALSE);
          clearstack(U1ST);
        }
/*      clrmsg(MROW,MCOL);
        message(MROW,MCOL,msg);
*/      clrmsg(INROW,INCOL);
        erg = SUCCESS;
        break;

#if !BATCH
      case c_doc :
        if (*buf) {
          if (scanword(&buf,docfil,LENFN) <= LENFN) {
            chkfilname(docfil,FN_DOC);
           }
          else {
          }
         }
        else {    /* no parameter	*/
        }
#if UNIX
        if ((ptr = getenv("EDITOR")) != NULLPTR) {
           sprintf(buf,"%s %s\0",ptr,editfil[FN_DOC]);
           CLSI();
           (void)switchterm(LINEMODE);
           system(buf);
           (void)switchterm(RAWMODE);
           initscreen();
           dispfa();
         }
        else {
          message(MROW,MCOL,"can't get EDITOR from environment");
          erg = FAILURE;
        }
#endif
#if SINTRAN3
        sprintf(buf,"ped %s\0",editfil[FN_DOC]);
        CLSI();
        (void)switchterm(LINEMODE);
        system(buf);
        (void)switchterm(RAWMODE);
        initscreen();
        dispfa();
#endif
        break;
#endif
      case c_print  :
        if (scanword(&buf,fbuf,LENFN) <= LENFN)
           erg = prettyprint(fbuf,"w",width,PPDISPLAY | PPASC,FN_ASC);
        else
           message(MROW,MCOL,"Invalid Parameter");
        break;
      case c_pretty :
        if (scanword(&buf,fbuf,LENFN) <= LENFN)
           erg = prettyprint(fbuf,"w",width,PPDISPLAY,FN_PP);
        else
           message(MROW,MCOL,"Invalid Parameter");
        break;
      case c_append :
        if (scanword(&buf,fbuf,LENFN) <= LENFN)
           erg = prettyprint(fbuf,"a",width,PPDISPLAY,FN_PP);
        else {
           message(MROW,MCOL,"Invalid Parameter");
           erg = FAILURE;
        }
        break;
      case c_width  :
        if (*buf  &&  sscanf(buf,"%d",&c) ==1) {
          if (c > MINCOLS) width = c;
        }
        sprintf(msg,"Width = %d",width);
        message(MROW,MCOL,msg);
        break;
      case c_dmode  :
        if (*buf  &&  sscanf(buf,"%d",&c) == 1  && dmode != c) {
          dmode = c;
          dispfa();
        }
        sprintf(msg,"dmode = %d",dmode);
        message(MROW,MCOL,msg);
        break;
      case c_read   :
        name[0] = '\0';
        if (*buf) {
          if (scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
         }
        else {  /* buf empty	*/
          /* edit name with getline ?	*/
          strcpy(buf,editfil[FN_RED]);
          message(MROW,MCOL,"Please enter Inputfilename");
          c = getline(buf,LENFIL-1,0,strlen(buf),INROW,INCOL,LENFIL-1);
          if (c != FAILURE && *buf  &&  scanword(&buf,fbuf,LENFIL) <= LENFIL) {
            strcpy(name,fbuf);
           }
          else {         /* scanword failed	*/
          }
        }
        if (name[0]) {
          chkfilname(name,FN_ASC);
          scanfile(editfil[FN_ASC]);
        }
        break;
      case c_switch : (void)swt_vers(); /* FALLTHROUGH !	*/
      case c_version: vers_msg();            break;
      case c_comp   :
        copywhole(VST);
        clearstack(BST);
        duplicate(S1ST,BST);
        if (findp(BST,VST)) {
          message(MROW,MCOL,"Expressions are identical");
         }
        else {
          message(MROW,MCOL,"Expressions are not identical");
        }
        clearstack(BST);
        clearstack(VST);
        break;
      case c_next  :
      case c_find  :
        copywhole(BST);
        clearstack(VST);
        duplicate(S1ST,VST);
        if (( n = find(VST,BST,c == c_next)) == FAILURE) {
          message(MROW,MCOL,"Expression does not exist");
         }
        else {
          message(MROW,MCOL,"Expression exists");
          scright(n,TRUE);
        }
        clearstack(BST);
        clearstack(VST);
        break;
      case c_edp : CLSI();
        editparms(stdout);
        getkey();
        initscreen();
        dispfa();
        break;
#if UNIX
      case c_keys: CLSI();
        defkeys();
        initscreen();
        dispfa();
        break;
#endif
      case c_mkap:
        m = topof(MST);
        n = class(m);
        d = arity(m);
        if (edit6(topof(EST)) == n_fcn &&
            (n == AR0(CALL) ||  n == callkon(d))
            && d == arity(topof(M1ST))) {
          ppop(MST);
          ppop(M1ST);
          push(MST,AR0(AP) | (m & ~CLASS_FIELD));
          push(M1ST,AR0(AP) | (m & ~CLASS_FIELD));
          dispfa();
         }
        else {
          message(MROW,MCOL,"Wrong position ?");
          erg = FAILURE;
        }
        break;
      case c_mkfct:
       {
        STACKELEM te,m1;

        te = noedit(topof(EST));
        d = ispredef(te,functions,nfunct) != FAILURE;
        if ((is_letter(topof(EST)) || d) &&
            class(m = topof(MST)) == AR0(AP) && arity(m) == arity(topof(M1ST))) {
          ppop(MST);
          ppop(M1ST);
          if ((m1 = checkf(te,m)) != STBOTTOM) m1 = AR0(m1) | (m & ~CLASS_FIELD);
          else m1 = AR0(CALL) | (m & ~CLASS_FIELD);
          push(MST,m1);
          push(M1ST,m1);
          dispfa();
         }
        else {
          message(MROW,MCOL,"Wrong position or no variable ?");
          erg = FAILURE;
        }
        break;
       }
      case c_c_stop:
        CLR_INFO(D_STOP,Parms.debug_info);
        break;
      case c_c_error:
        CLR_INFO(D_ERROR,Parms.debug_info);
        break;
      case c_s_stop:
        SET_INFO(D_STOP,Parms.debug_info);
        break;
      case c_s_error:
        SET_INFO(D_ERROR,Parms.debug_info);
        break;
      case c_double:
        if ((c = getonoff(buf)) != FAILURE) {
           doub_def = c;
           buf = skipword(buf);
           Changes = YES;
        }
        sprintf(msg,"double definition allowed = %s",doub_def ? "on" : "off");
        message(MROW,MCOL,msg);
        break;
#if KEDIT
      case c_os_unload :
        erg = redc(rca,0,0,isfctvers,0,RED_UNLD);
        break;
#endif /* KEDIT	*/
#if X_WINDOWS
      case c_showkeys :
        showkeys(stdout);
        refresh();
        break;
      case c_user :
        user();
        break;
      case c_position : {
        int x,y;

        Mouse(&x,&y);
        position(x,y,TRUE);
        break;
      }
      case c_font  :
        if (scanword(&buf,fbuf,LENFN) <= LENFN) {
           if (fontload(fbuf)) {
             message(MROW,MCOL,"Ok");
             refresh();
            }
           else {
             message(MROW,MCOL,"Error: can't load font ?");
           }
         }
        else
           message(MROW,MCOL,"Invalid Parameter");
        break;
#endif
      default    : sprintf(msg,"Command \"%s\" not implemented yet",cmd);
        message(MROW,MCOL,msg);
        erg = FAILURE;
        break;
     }
    }
    else {
      sprintf(msg,"Command \"%s\" not found",cmd);
      message(MROW,MCOL,msg);
      erg = FAILURE;
    }
   }
  else {
    message(MROW,MCOL,"No command received");
    erg = FAILURE;
  }
  if (erg <= SUCCESS) getunget();
  clrmsg(MROW,MCOL);
  return(erg);
 }
/* end of      ccommand.c	*/
