/* file-io                             cfiles.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    23.06.88                */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#ifndef SEEK_END
#include <unistd.h>
#endif
#if UNIX
#include <pwd.h>
#include <sys/errno.h>
#endif
#include <ctype.h>
#include "cedit.h"
#include "cencod.h"
#include "cetb.h"
#include "cscred.h"
#include "ctermio.h"
#include "cestack.h"
#define isfile 1
#include "cfiles.h"

/* modify AP to SYSAP if storing FN_RED ?	*/
#define SYSAP_HACK	0

#define FILE_EXPR	'e'
#define FILE_SUB	's'

#define PER_LINE 8        /* stackelemente pro zeile */

#define komp_vers(v) (strcmp(v,VERSION) == SUCCESS)

extern char *strcpy(),msg[];

char edfilbuf[LENFIL];

#ifdef X_UDI
extern int SERVER_MODE;
#endif

#if N_STR
static int str_vers;	/* neue string-version einlesen ?	*/
#endif

/* ------- istop ----------------------------------------------- */
int istop()
 {
  register int erg;

  erg = isempty(MST);
  if (!erg) {
    erg = (edit6(topof(EST)) == n_fcn) && (second(MST) == STBOTTOM) &&
          (arity(topof(MST)) == arity(topof(M1ST)))  &&
          isempty(AST);
  }
  return(erg);
 }
/* ------- lastpos ----------------------------------------------- */
/* ist zeichen c in str s ?                                        */
/* return: falls nein: FAILURE, sonst letzte position !            */
static lastpos(c,s)
 char c,*s;
 {
  register char *t;
  register int pos;

  t = s;
  pos = -1;
  while (*t) {
    if (c == *t++) pos = (int)(t-s)-1;
  }
  return(pos);
 }

#if UNIX
/* ------- tilde ----------------------------------------------- */
static int tilde(file, exp)
  char *file, *exp;
{
  *exp = '\0';
  if (file) {
    if (*file == '~') {
      char user[128];
      struct passwd *pw = NULL;
      int i = 0;
      
      user[0] = '\0';
      file++;
      while (*file != '/' && i < sizeof(user))
        user[i++] = *file++;
      user[i] = '\0';
      if (i == 0) {
        char *login = (char *) getlogin();
	
        if (login == NULL && (pw = getpwuid(getuid())) == NULL)
	  return (0);
	if (login != NULL)
	  strcpy(user, login);
      }
      if (pw == NULL && (pw = getpwnam(user)) == NULL)
        return (0);
      strcpy(exp, pw->pw_dir);
    }
    strcat(exp, file);
    return (1);
  }
  return (0);
}
#endif

/* ----- chkfilname ---------------------------------------------- */
/* teste dateinamen name und schreibe (ev. modifizierte Version)   */
/* nach editfil[fnr]                                               */
/* falls keine extension: editext[fnr] anhaengen                   */
/* falls kein bezug auf directory: editlib[fnr] davor              */
chkfilname(fname,fnr)
 char *fname;
 int fnr;
 {
  register int ext,dir,off;
  char name[LENFIL];

  if (fname != NULLPTR) {
    if (*fname == '\0') strcpy(name,editfil[fnr]);
    else strcpy(name,fname);

#if SINTRAN3
    dir = lastpos(DIRCHS,name);
    if (dir != FAILURE) off = dir;
    else                off = 0;
    ext = lastpos(EXTCHS,name+off);         /* hinter dir-pos. ?       */
    if (ext == FAILURE) ext = lastpos(EXTCHS,name+off);
#else   /* UNIX */
    dir = lastpos(DIRCH,name);
    if (dir != FAILURE) off = dir;
    else                off = 0;
    ext = lastpos(EXTCH,name+off);        /* hinter dir-pos. ?       */
#endif
    if (fnr == FN_PRE) {
      name[off] = TOUPPER(name[off]);
    }
    if (ext  == FAILURE) {
      if (dir == FAILURE) {        /* keine extension, keine directory */
        sprintf(editfil[fnr],"%s%s%s",editlib[fnr],name,editext[fnr]);
       }
      else {                       /* keine extension, aber directory  */
        sprintf(editfil[fnr],"%s%s",name,editext[fnr]);
      }
     }
    else {       /* extension existiert     */
      if (dir == FAILURE) {        /* extension, aber keine directory  */
        sprintf(editfil[fnr],"%s%s",editlib[fnr],name);
       }
      else {                       /* extension und directory          */
        sprintf(editfil[fnr],"%s",name);
      }
    }
#if UNIX
    if (tilde(editfil[fnr],name)) strcpy(editfil[fnr],name);
#endif
  }
 }
/* ----- getbase ------------------------------------------------- */
/* teste dateinamen editfil[fnr] und schreibe (ev. modifizierte    */
/* Version) nach fname                                             */
/* falls extension oder bezug auf directory: entfernen             */
getbase(fname,fnr)
 char *fname;
 int fnr;
 {
  register int ext,dir,off;
  char *name,c;

  name = editfil[fnr];
#if SINTRAN3
  dir = lastpos(DIRCHS,name);
  if (dir != FAILURE) off = dir;
  else                off = 0;
  ext = lastpos(EXTCHS,name+off);         /* hinter dir-pos. ?       */
  if (ext == FAILURE) ext = lastpos(EXTCH,name+off);
#else   /* UNIX */
  dir = lastpos(DIRCH,name);
  if (dir != FAILURE) off = dir+1;
  else                off = 0;
  ext = lastpos(EXTCH,name+off);        /* hinter dir-pos. ?       */
#endif
  if (ext == FAILURE) ext = LENFIL;
  else                ext += off;
  name += off;
  do {
    c = *name++;
    *fname++ = c;
    off++;
  } while (off <= ext && c);
  if (c) *--fname = '\0';
 }

static chkint(st1,st2)
 STACK *st1,*st2;
{ return(TRUE); }         /* aendern */

static STACKELEM ftype;

static int index;

/* ----- trtofile ------------------------------------------------ */
/* transportiere ausdruck von stack auf file ohne zu transponieren */
static trtofile(st,fp)
 register STACK *st;
 register FILE  *fp;
 {
  static STACKELEM m;
  register int n;

  if ((m = topof(st)) != STBOTTOM) {
    if (isconstr(m)) {
      if (ftype) {
        m &= ~EDIT_FIELD;
        if (class(m) == class(AP)) m |= ftype;
      }
      fprintf(fp,"%08lx\n",m);
      popnf(st);
      if (ferror(fp)) return(FAILURE);
      for (n = (int)arity(m); n > 0; --n) {
         if (trtofile(st,fp) == FAILURE) return(FAILURE);
      }
     }
    else {              /* atom */
      if (m == AQUOTE) {
        n = 0;
        do {
          if((m = popnf(st)) == AQUOTE) ++n; else
          if (m == EQUOTE) --n;
          fprintf(fp,"%08lx\n",m);
          if (ferror(fp)) return(FAILURE);
        } while (n);
       } else
      if (single_atom(m) || marked0(m)) {
        fprintf(fp,"%08lx\n",popnf(st));
        if (ferror(fp)) return(FAILURE);
       }
      else {
       do {
         m = popnf(st);
         fprintf(fp,"%08lx\n",m);
         if (ferror(fp)) return(FAILURE);
       } while (marked1(m));
      }
     }
   }
   return(SUCCESS);
 }

/* ----- trfromfile ------------------------------------------ */
/* transponiert einen ausdruck von einer datei auf einen stack */
static trfromfile(fp,st)
 register FILE *fp;
 register STACK *st;
 {
  static STACKELEM m;
  register int n,c;

  if ((c = fscanf(fp,"%lx",&m)) == 1) {
    if (isconstr(m)) {
      push(MST,m);
      for (n = (int)arity(m); n > 0; --n)
        if ((c = trfromfile(fp,st)) != 1) {
          m = pop(MST);
          push(MST,(m & ~VALUE_FIELD) | toarity((arity(m) - n + 1)));
          break;
        }
      move(MST,st);
     }
    else {                      /* atom */
#if N_STR
#define MAXL 64
#define ISAQUOTE(m) (isletstr(m) && value(m) == STRBEGIN)
#define ISEQUOTE(m) (isletstr(m) && value(m) == STREND)
     int len[MAXL];

      if (!str_vers && (m == AQUOTE || ISAQUOTE(m))) {
        n = 1;
        len[n] = 0;
        do {
          if ((c = fscanf(fp,"%lx",&m)) != 1)
            break;
          if (m == AQUOTE || ISAQUOTE(m)) {
            ++len[n];
            ++n;
            len[n] = 0;
           }  else
          if (m == EQUOTE || ISEQUOTE(m)) {
            if (len[n] == 0) push(st,NILSTR);
            else push(st,KQUOTE | toarity(len[n]));
            --n;
           }
          else {
            push(st,mark0(m));
            ++len[n];
          }
        } while (n);
       } else
      if (!str_vers && (m == EQUOTE || ISEQUOTE(m))) {
        n = 1;
        do {
          if ((c = fscanf(fp,"%lx",&m)) != 1)
            break;
          if (m == EQUOTE || ISEQUOTE(m)) {
            ++len[n];
            ++n;
            len[n] = 0;
           }  else
          if (m == AQUOTE || ISAQUOTE(m)) {
            if (len[n] == 0) push(st,NILSTR);
            else push(st,KQUOTE | toarity(len[n]));
            --n;
           }
          else {
            push(st,mark0(m));
            ++len[n];
          }
        } while (n);
#else
      if (m == AQUOTE) {
        n = 1;
        push(st,m);
        do {
          if ((c = fscanf(fp,"%lx",&m)) != 1)
            break;
          push(st,m);
          if (m == AQUOTE) ++n; else
          if (m == EQUOTE) --n;
        } while (n);
       } else
      if (m == EQUOTE) {
        n = 1;
        push(st,m);
        do {
          if ((c = fscanf(fp,"%lx",&m)) != 1)
            break;
          push(st,m);
          if (m == EQUOTE) ++n; else
          if (m == AQUOTE) --n;
        } while (n);
#endif
       } else
      if (single_atom(m) || marked0(m)) {
#ifdef N_MAT
        if (is_func(m)) m = new_fct(m);
#endif
        push(st,m);
       }
      else {
        push(st,mark0(m));
        do {
          if ((c = fscanf(fp,"%lx",&m)) != 1)
            break;
          else {
            if (isatom(m) && multi_atom(m))
              push(st,mark1(m));
            else {
              c = 0;
              break;
            }
          }
        } while (marked1(m));
      }
    }
   }
  else push(st,PLACEH);
  return(c);
 }

/* ----- saveexpr --------------------------------------------- */
/* versuche, stack st auf datei name abzuspeichern              */
/* falls datei ex., vorher fragen                               */
saveexpr(st,name,n,fnr)
 STACK *st;
 char *name;
 int n,fnr;
 {
  FILE *fp,*fopen();
  register STACKELEM m;
  register int p,ret,i;
  int c;
  char fvers[5];
  STACK X;

  p = 0;
#if SYSAP_HACK
  ftype = (fnr == FN_RED) ? LOWBIT : 0l;
#endif
  if (name == NILCHAR) {
    ret = NO_NAME;
   }
  else {
    chkfilname(name,fnr);
    if (noedit(topof(st)) == NOPAR) ret = EXNPAR;
    else {
      asgnst(XST,st);
      if (fnr != FN_RED  &&  (m = topof(M1ST)) != STBOTTOM  &&  notdisp(m)  &&
          arity(m) == arity(topof(MST))) {
         move2(MST,M2ST,XST);    /* move ndisp auf X     */
         p = 1;
      }
      if (chkint(XST,M2ST)) {
        i = 'y';                            /* vorbesetzung fuer unten      */
#ifdef X_UDI
        if (NO_EXTERNAL_CMD)
#endif
        if ((fp = fopen(editfil[fnr],"r")) != NULLFILE) {  /* ex. datei ?  */
          if (!batch) {
            sprintf(msg,"Replace %s (y/n) ? ",editfil[fnr]);
            dspstr(MROW,MCOL,msg);
            do {
              i = getkey();
            } while ( i != 'y'  &&  i != 'n');
            c = 1;
            if (i == 'y' && !istop() && fseek(fp,-2L,SEEK_END) == SUCCESS && (c = fgetc(fp)) == FILE_EXPR) {
              sprintf(msg,"Replace expression with subexpression (y/n) ? ");
              dspstr(MROW,MCOL,msg);
              do {
                i = getkey();
              } while ( i != 'y'  &&  i != 'n');
            }
            clrmsg(MROW,MCOL);
          }
          fclose(fp);
        }
       if (i == 'y'  &&  (fp = fopen(editfil[fnr],"w")) != NULLFILE) {
         if (ferror(fp)) ret = FAILURE;
         else {
          ret = SUCCESS;
          i = (int)edit6(topof(XST));
          if (n == 1) {
            index = 0;
            if (trtofile(XST,fp) != SUCCESS) {
              ret = FAILURE;
             }
            else {
              if (fnr != FN_RED) {
                fprintf(fp,"%d\n",edit_vers);
                fprintf(fp,"%s\n",VERSION);
                fprintf(fp,"%c\n",istop() ? FILE_EXPR : FILE_SUB);
              }
              if (ferror(fp)) ret = FAILURE;
            }
           }                         else
          if (islist(etptb[i].exptp)) {
            ret = SUCCESS;
            index = 0;
            c = n;
            while (c > 0  &&  islist(etptb[i].exptp)  &&
                  (ret = trtofile(XST,fp)) == SUCCESS) {
              i = (int)edit6(topof(XST));
              --c;
            }
            if (ret == SUCCESS) {
              if (fnr != FN_RED) {
                fprintf(fp,"%d\n",edit_vers);
                fprintf(fp,"%s\n",VERSION);
                fprintf(fp,"%c\n",istop() ? FILE_EXPR : FILE_SUB);
              }
              if (!ferror(fp)) ret = c;     /* >= 0 !         */
              else             ret = FAILURE;
             }
            else ret = FAILURE;
           }
          else ret = NOLIST;
         }
         fclose(fp);
         if (ret == SUCCESS  &&  fnr != FN_RED) {
           i = n;
           if ((fp = fopen(editfil[fnr],"r")) != NULLFILE) {
#if N_STR
             str_vers = 1;
#else
             str_vers = 0;
#endif
             while (i > 0  &&  (trfromfile(fp,RST) == 1))     /* vorsicht: R */
               --i;
             if (i) ret = FAILURE;
             else {
               if (fscanf(fp,"%d",&c) != 1  || fscanf(fp,"%s",fvers) != 1  ||
                   c != edit_vers  ||  !komp_vers(fvers)) {
                  ret = FAILURE;
               }
             }
             fclose(fp);
             if (ret == FAILURE) unlink(editfil[FN_ED]);
            }
           else ret = FAILURE;
         }
        }
       else ret = FAILURE;
      }
     else ret = EXINT;
   }
  }
  while (p-- > 0)
    move(M2ST,MST);
  return ( ret ) ;
 }

/* ----- writefile -------------------------------------------- */
/* schreibe stack st in datei, frage nach namen usw.            */
writefile(st)
 STACK *st;
 {
  char *fn,*buf,*readword();
  extern char *skipword();
  int c,n;

  c = NOFN;
  n = 1;
  putrce();
  dspstr(MROW,MCOL,"Enter output-filename");
  if ((fn = readword(lbuf,LBUFLEN)) != NILCHAR) {
    if (fn == NULLPTR) {
      strcpy(lbuf,editfil[FN_ED]);
      c = getline(lbuf,LENFIL-1,0,strlen(lbuf),INROW,INCOL,LENFIL-1);
      fn = lbuf;
      clrmsg(INROW,INCOL);
      if (c != FAILURE && *lbuf  &&  scanword(&fn,msg,LENFIL) <= LENFIL) {
        fn = strcpy(editfil[FN_ED],msg);
        c = 1;
        if (*lbuf && sscanf(lbuf,"%d",&n) == 1) {
        }
       }
      else {
        dspstr(MROW,MCOL,"Wrong output-filename");
        c = NOFN;
      }
     }
    else {
      c = 1;
      buf = skipword(lbuf);
      if (*buf && sscanf(buf,"%d",&n) == 1) {
      }
    }
  }
  if (c != NOFN) {
    clrmsg(MROW,MCOL);
    dspstr(MROW,MCOL,"Please wait");
    c = saveexpr(st,fn,n,FN_ED);
    res_save(c,n,editfil[FN_ED]);
#ifdef X_UDI
    if (SERVER_MODE) SendFilename(editfil[FN_ED]);
#endif
  }
  setcursor();
  getunget();
  clrmsg(MROW,MCOL);
 }

#ifdef X_UDI
/* ----- my_writefile ----------------------------------------- */
/* schreibe stack st in datei                                   */
my_writefile(name)
 char *name;
 {
  int c;
  c = saveexpr(EST,name,1,FN_ED);
  setcursor();
}

my_readfile(name)
 char *name;
{
 int c;
 clearstack(U1ST); clearstack(U2ST);
 c = loadexpr(U1ST,name,1,FN_ED);
 res_read(c,1,name,U1ST,U2ST,FALSE);
 clearstack(U1ST);
 setcursor();
}

#endif

/* ------------ res_save -------------------------------------- */
/* ausgabe der meldungen fuer das resultat c von saveexpr(,,n)  */
res_save(c,n,fn)
 register int c,n;
 char *fn;
 {

  if (c >= 0) {
    n -= c;
    sprintf(msg,"Wrote %d %sexpression%c to %s",n,
      istop() ? "" : "sub",
      (n == 1) ? ' ' : 's',fn);
    dspstr(MROW,MCOL,msg);
   }                                                     else
  if (c == EXNPAR)
    dspstr(MROW,MCOL,"Can't write NOPAR");               else
  if (c == EXINT)
    dspstr(MROW,MCOL,"Can't write internal function");   else
  if (c == NO_NAME)
    dspstr(MROW,MCOL,"No filename received");            else
  if (c == NOLIST)
    dspstr(MROW,MCOL,"More than one expression only in lists");
  else
   dspstr2(MROW,MCOL,"Can't write file ",fn);
 }

/* ----- loadexpr ------------------------------------- */
/* lade stack st aus datei name mit n ausdruecken       */
/* (falls n > 1: Liste ?)                               */
loadexpr(st,name,n,fnr)
 STACK *st;
 char *name;
 int n,fnr;
 {
  FILE *fp,*fopen();
  int c;
#if N_STR
  int n_alt;
#endif
  char fvers[5];

  chkfilname(name,fnr);
  if ((fp = fopen(editfil[fnr],"r")) != NULLFILE) {
    c = 0;
#if N_STR
    n_alt = n;
    while ( n-- > 0  &&  (c = trfromfile(fp,st)) == 1)
      ;
    if (c != 1) {
      rewind(fp);
      n = n_alt;
      while ( n-- > 0  &&  (c = trfromfile(fp,st)) == 1)
        ;
    }
#else
    while ( n-- > 0  &&  (c = trfromfile(fp,st)) == 1)
      ;
#endif
    if (c == 1) {
      if (fnr == FN_RED) c = RDOK;
      else
#if LETREC
      if (fscanf(fp,"%d",&c) == 1) { /* editor-version (AP,Function)   */
        if (fscanf(fp,"%s",fvers) == 1) {         /* VERSIONS-Nr.   */
          if (komp_vers(fvers)) c = RDOK;
          else {
#if N_MAT
#if N_STR
              extern int newetb[];
              int ena,enn;

              ena = edit6(topof(st));
              if ((enn = newetb[ena]) != (-1)) {
                push(st,noedit(pop(st)) | toedit6(enn));
              }
#endif
              c = DIFFVERS;
#else
              c = OLDVERS;
#endif
          }
         }
        else c = OLDVERS;
       }
      else c = IOERR;
#else
      if (fscanf(fp,"%d",&c) == 1) { /* editor-version (AP,Function)   */
        if (c == edit_vers) {
          if (fscanf(fp,"%s",fvers) == 1) {         /* VERSIONS-Nr.   */
            if (komp_vers(fvers)) c = RDOK;
            else {
#if N_MAT
#if N_STR
              extern int newetb[];
              int ena,enn;

              ena = edit6(topof(st));
              if ((enn = newetb[ena]) != (-1)) {
                push(st,noedit(pop(st)) | toedit6(enn));
              }
#endif
              c = DIFFVERS;
#else
              c = OLDVERS;
#endif
            }
           }
          else c = OLDVERS;
         }
        else c = ILLVERS;
       }
      else c = IOERR;
#endif
     }
    else c = WRONG;
    fclose(fp);
   }
  else {
    c = FILEERR;
    push(st,PLACEH);
  }
  return(c);
 }

/* ----- readfile ------------------------------------- */
static int readfile(st,st2)
 STACK *st,*st2;
 {
  char *fn,*readword();
  int c = NOFN;    /* exit for filename (fn) == NULLPTR */

  putrce();
  dspstr(MROW,MCOL,"Enter input-filename");
  if ((fn = readword(lbuf,LBUFLEN)) != NILCHAR) {
    if (fn == NULLPTR) {
      strcpy(lbuf,editfil[FN_ED]);
      c = getline(lbuf,LENFIL-1,0,strlen(lbuf),INROW,INCOL,LENFIL-1);
      fn = lbuf;
      clrmsg(INROW,INCOL);
      if (c != FAILURE && *lbuf  &&  scanword(&fn,msg,LENFIL) <= LENFIL) {
        fn = strcpy(editfil[FN_ED],msg);
       }
      else {
        dspstr(MROW,MCOL,"Wrong input-filename");
        c = NOFN;
      }
     }
    else c = 1;
    if (c != NOFN) {
      clrmsg(MROW,MCOL);
      chkfilname(fn,FN_ED);
      sprintf(msg,"Reading \"%s\"",editfil[FN_ED]);
      dspstr(MROW,MCOL,msg);
      c = loadexpr(st,fn,1,FN_ED);
      if (res_read(c,1,editfil[FN_ED],st,st2,FALSE)) {
#ifdef X_UDI
      if (SERVER_MODE) SendFilename(editfil[FN_ED]);
#endif
      }
      if (c == ILLVERS) {
        if (swt_vers()) {
          message(MROW,MCOL,"Trying another version !");
          clearstack(EST);
          clearstack(st);
          push(EST,PLACEH | toedit6(start_eno));
          c = loadexpr(st,fn,1,FN_ED);
          res_read(c,1,editfil[FN_ED],st,st2,FALSE);
         }
        else {
          message(MROW,MCOL,"Illegal version, can't switch !");
          c = FAILURE;
        }
      }
    }
   }
  else dspstr(MROW,MCOL,"Wrong input-filename");
  setcursor();
  return(c);
 }

/* ----------- res_read ------------------------------------- */
/* ausgabe des resultates von loadexpr                        */
/* auf stack st1 einsortieren in E, stack st2 als hilfsstack  */
/* fn : filename                                              */
res_read(c,n,fn,st1,st2,out)
 int   c,n,out;
 char  *fn;
 STACK *st1,*st2;
 {
 int okay=0;

   switch(c) {
      case DIFFVERS: sprintf(msg,"Warning: Different versions !");
      case RDOK    : if (!insert(st1,st2,!out)) {
                       sprintf(msg,"Type conflict, cmd rejected !");
                      }
                     else {
                       if (c == RDOK) {
                         okay=1;
                         sprintf(msg,"%d expression%c read from %s",n,
                                     (n == 1) ? ' ' : 's',fn);
                       }
                     }
                     break;
      case FILEERR : sprintf(msg,"Can't open file %s",fn);           break;
      case WRONG   : sprintf(msg,"Wrong expression on %s",fn);       break;
      case ILLVERS : sprintf(msg,"Illegal editor-version on %s",fn); break;
      case OLDVERS : sprintf(msg,"Sorry, wrong editor-version on %s",fn);
                     break;
      case IOERR   : sprintf(msg,"Can't read editor-version on %s",fn);
                     break;
      default      : msg[0]= '\0'; break;
   }
   dspstr(MROW,MCOL,msg);
   return (okay);
 }

/* ----- infile ------------------------------------- */
infile()
 {
  clearstack(U1ST); clearstack(U2ST);
  (void)readfile(U1ST,U2ST);
  clearstack(U1ST);
  getunget();
  clrmsg(MROW,MCOL);
 }

/* ----- insert ------------------------------------------------------ */
/* fuege inhalt von stack st in E ein, h Hilfsstack                    */
/* out: soll dispfa erfolgen ?                                         */
insert(st,h,out)
 STACK *st,*h;
 int out;
 {
  register STACKELEM m;
  register int eno,ene,clm,ena,n,i;

  ene = ena = (int)edit6(topof(EST));
  if ((m = topof(MST)) != STBOTTOM  &&  notdisp(m)  &&
      arity(m) == arity(topof(M1ST)))
    ene = (int)edit6(m);       /* info des konstruktors        */
  eno = (int)edit6( m = topof(st));
  clm = class(m);
  if (eno  == n_fdec)
    eno = n_expr;             else
  if (eno == n_dcex)
    if (clm == AR0(DECL))
      eno = n_decl;
    else
      eno = n_expr;
  if (iskomp(etptb[ene].exptp,etptb[eno].eetp) &&
      (!ismatel(ene) || (matpath1() && arity(topof(MST)) < 2L)
                        || matchmat(U1ST,EST))              ||
      (ene == n_dcex && clm == AR0(FDEC) && class(topof(EST)) != AR0(DECL)) ) {
    if (ene != ena) {                 /* fnam oder fcn auf e ?        */
      move(MST,EST);          /* kompl. ausdruck (call oder fdec)     */
      pop(M1ST);              /* nach e bringen                       */
      incrarity(M1ST);        /* arity korrigieren                    */
    }
    /* ev. korrekturen wg. NDISP-Konstr. vornehmen    */
    if (clm == AR0(FDEC)) {
      clm = 0;                                 /* CE nicht retten      */
      if (class(topof(MST)) == AR0(DECL)) {       /* decl bereits auf m ? */
        incrarity(MST);                       /* stelligkeiten        */
        incrarity(M1ST);                      /*  erhoehen            */
       }
      else {                                  /* kein decl: einfuegen */
        n = 1;
        while (class(topof(st)) == AR0(FDEC)) {
          n++;
          transport(st,h);
        }
        for ( i = n-1; i > 0 ; i-- ) {
          transport(h,st);
        }
        transport(EST,st);  /* uebertrage rumpf uebergeordneter fkt */
        push(U1ST,AR0(DECL) | toedit6(n_decl) | toarity(n));
      }
    }
    if (clm) {
      clearstack(SST);
      transport(EST,SST);
    }
    transport(st,EST);
    push(EST,noedit(pop(EST)) | toedit6(ene));
    if (islist(etptb[ene].exptp))
      while(!isempty(st)) {
        transport(st,EST);
        incrarity(MST);
        incrarity(M1ST);
        push(EST,noedit(pop(EST)) | toedit6(ene));
      }
    if (out) {
      clrmsg(MROW,MCOL);
      dispfa();
    }
    clm = TRUE;
   }
  else clm = FALSE;
  return(clm);
 }

/* ------------- filelength -------------------*/
static filelength(fp)
FILE *fp;
{
 register int c;

 c = 0;
 while (getc(fp) != EOF)
   c++;
 rewind(fp);
 return(c);
}

/* --------- scanfile -------------------------- */
scanfile(fname)
char *fname;
{
  extern void cfree();
  FILE *fp,*fopen();
  char *buf,*calloc();
  int c,flen,i;

  i = 0;
  if ((fp = fopen(fname,"r")) != NULLFILE) {
    flen = filelength(fp);
    if ((buf = calloc((unsigned)(flen+2),(unsigned)sizeof(char))) != NULLPTR) {
      while (isspace(c = getc(fp)))                      /* skip blanks */
       ;
      do {
        if (c > ' ')
          buf[i++] = c;
        else {
          buf[i++] = ' ';
          while (isspace(c = getc(fp)) && c != ' ')
            ;
          if (c == EOF)
             break;
          else buf[i++] = c;
         }
      } while ((c = getc(fp)) != EOF);
      fclose(fp);
      buf[i] = ' ';
      unget((int)'\r');
      lpos = 0;
      lmxpos = i;
      (void)input(buf,flen,strcmp(fname + (strlen(fname)-strlen(editext[FN_ASC])),
                            editext[FN_ASC]));
      cfree(buf);
     }
    else {
      message(MROW,MCOL,"Can't get enough space for input buffer");
      fclose(fp);
    }
  }
 else mesg2(MROW,MCOL,"Can't open ",fname);

 getunget();
 clrmsg(MROW,MCOL);
}
/* end of      cfiles.c */
