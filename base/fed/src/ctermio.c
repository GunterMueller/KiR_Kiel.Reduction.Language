/*                                                cterm.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* screen functions						*/

#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cswitches.h"
#include "ctermio.h"

static int ONLY_LICO=0;

#if BATCH
cursorxy(x,y) int x,y; {}
termcaps() {return(TRUE);}
getkey() {return('y');}
switchterm(mode) int mode; {return(TRUE);}
readkeys(fp) FILE *fp; {return(TRUE);}
void showkeys(fp) FILE *fp; {}
defkeys() {return(TRUE);}

#else	/* !BATCH	*/


#if UNIX

#ifdef TERMIOS
#include <termios.h>
#if __convex__
#include <sys/ioctl.h>        /* non-standard, for TIOCGWINSZ */
#endif
#else
#ifdef TERMIO
#include <termio.h>
#else
#include <sgtty.h>
#endif
#endif

#ifdef TERMIOS
struct termios _tty,
	       _old_tty;
#define	TTYGET(where)	tcgetattr(0,(where))
#define	TTYSET(where)	tcsetattr(0,TCSADRAIN,(where))
#else	/*TERMIOS*/
# ifdef TERMIO
struct termio _tty, 
              _old_tty;
#define	TTYGET(where)	ioctl(0,TCGETA,(where))
#define	TTYSET(where)	ioctl(0,TCSETAW,(where))
# else
struct sgttyb _tty,
	      _old_tty;
#define	TTYGET(where)	ioctl(0,TIOCGETP,(where))
#define	TTYSET(where)	ioctl(0,TIOCSETP,(where))
# endif	/*TERMIO*/
#endif	/*TERMIOS*/


#define KH "\016"	/* CTRL-N cursorhome	*/
#define IC "\005"	/* CTRL-E insert	*/
#define DC "\008"	/* CTRL-H delete	*/

#define COLS  80	/* default screen size	*/
#if SINTRAN3
#define LINES 25
#else
#define LINES 24
#endif

#undef DEBUG

#ifdef DEBUG
#define SHOWCAP(c) fprintf(stderr,"c ="); showstr(c)

showstr(s)
 char	*s;
 {
  char	*h;

  if (h = s) {
  while (*h) {
    fprintf(stderr,"%x ",(int)(*h));
    ++h;
  }
  }
  fprintf(stderr,"\n");
 }
#else
#define SHOWCAP(c) 
#endif	/* endif	*/

int  li = LINES
    ,co = COLS;

#if APOLLO
#ifdef DOMAIN10_3
#include "/sys/ins/base.ins.c"
#include "/sys/ins/pad.ins.c"
#include "/sys/ins/pgm.ins.c"
#include "/sys/ins/error.ins.c"
#else
#include <apollo/base.h>
#include <apollo/pad.h>
#include <apollo/pgm.h>
#include <apollo/error.h>
#endif

#define max_windows     10
#define font_size       NULLPTR    /* No need for font pathname */

/*********************************************************************/
/* check return code from standard system calls                      */
/*********************************************************************/
static void check_status(status)
status_$t status;
{   if(status.all != status_$ok)
        error_$print(status);
}                                              /* end check_status() */

/*********************************************************************/
/*  getsize returns the size of the current window in characters.    */
/*********************************************************************/
static void getsize(row,col)
long *row,*col;
{
pad_$window_list_t  window_info;
short               n_windows = 1;
short               font_width;
short               font_hight;
pad_$string_t       font_name;
short               font_len;
status_$t           status;

/* Set scale to 1,1 to get width and height in raster units. */
    pad_$set_scale ( stream_$stdout,
                     (short)1,
                     (short)1,
#ifdef DOMAIN10_3
                     status
#else
                     &status
#endif
                  );

    check_status(status);
/* Get window information about user's standard output stream. */
    pad_$inq_windows (stream_$stdout,/* Standard output (display)     */
                      window_info,   /* Current position of window    */
                      max_windows,   /* Maximum no. of windows desired*/
#ifdef DOMAIN10_3
                      n_windows,     /* Number of windows open to pad */
                      status
#else
                      &n_windows,    /* Number of windows open to pad */
                      &status
#endif
                      );       /* Completion status             */
    check_status(status);
    pad_$inq_font ( stream_$stdout,
#ifdef DOMAIN10_3
                    font_width,
                    font_hight,
#else
                    &font_width,
                    &font_hight,
#endif
                    font_name,
                    (short) 0,
#ifdef DOMAIN10_3
                    font_len,
                    status
#else
                    &font_len,
                    &status
#endif
                    );
    check_status(status);
    if (status.all !=  0) {
      *col = COLS;                                         /* default  */
      *row = LINES;                                        /* default  */
     }
    else {
      *col =  (long)(window_info[0].width/font_width);
      *row =  (long)(window_info[0].height/font_hight);
    }
}
#endif	/* APOLLO	*/


static char	*ho;	/* cursor home		*/
       char	*nd;	/* cursor right (non desctructive space)	*/
       char	*le;	/* cursor left		*/
static char	*up;	/* cursor up		*/
static char	*down;	/* cursor down		*/

static char	*kh;	/* cursor home key	*/
static char	*kr;	/* cursor right key	*/
static char	*kl;	/* cursor left key	*/
static char	*ku;	/* cursor up key	*/
static char	*kd;	/* cursor down key	*/
static char	*ks;	/* cursor transmit on	*/
static char	*ke;	/* cursor transmit off	*/
static char	*ml;	/* memlock on		*/
static char	*mu;	/* memlock off		*/
       char	*cl;	/* clear screen		*/
       char	*ce;	/* clear to eoln	*/
       char	*cd;	/* clear to eos		*/
       char	*ic;	/* insert character	*/
       char	*dc;	/* delete character	*/
       char	*cm;	/* cursor motion	*/
static char	*so;	/* shift out (reverse)	*/
static char	*se;	/* shift end		*/
static char	*us;	/* underline start	*/
static char	*ue;	/* underline end	*/
static char	*k0;	/* function keys	*/
static char	*k1;	/* 	*/
static char	*k2;	/* 	*/
static char	*k3;	/* 	*/
static char	*k4;	/* 	*/
static char	*k5;	/* 	*/
static char	*k6;	/* 	*/
static char	*k7;	/* 	*/
static char	*k8;	/* 	*/
static char	*k9;	/* 	*/
static char	*l0;	/* function keys	*/
static char	*l1;	/* 	*/
static char	*l2;	/* 	*/
static char	*l3;	/* 	*/
static char	*l4;	/* 	*/
static char	*l5;	/* 	*/
static char	*ti;	/* terminal init	*/
static char	*te;	/* terminal end		*/


static int firstcall	= TRUE;
static int rawmode	= FALSE;

/* ------------------------------------	*/
int switchterm(toraw)
int toraw;
{
 int erg;

  erg = TRUE;
  if (firstcall) {
    if (TTYGET(&_old_tty) == FAILURE) {
      erg = FALSE;
    }
    firstcall = FALSE;
  }
  if (!toraw && rawmode) {
    if (TTYSET(&_old_tty) == FAILURE) {
      erg = FALSE;
    }
    rawmode = FALSE;
    if (te != NULLPTR) { tputs(te,1,putch); }
    if (ke != NULLPTR) { tputs(ke,1,putch); }
   } else
  if (toraw && ! rawmode) {
    if (TTYGET(&_tty) == FAILURE) {    /** again! **/
      erg = FALSE;
    }
#if !defined(TERMIO) && !defined(TERMIOS)
    _tty.sg_flags &= ~(ECHO);	/* echo off */
    _tty.sg_flags |= RAW;	/* raw on    */
#else
    _tty.c_lflag &= ~(ISIG | ICANON | ECHO);	/* noecho raw mode        */
    _tty.c_cc[VMIN] = '\01';	/* minimum # of chars to queue    */
    _tty.c_cc[VTIME] = '\0';	/* minimum time to wait for input */
#endif
    if (TTYSET(&_tty) == FAILURE) {
      erg = FALSE;
    }
    rawmode = TRUE;
    if (ti != NULLPTR) { tputs(ti,1,putch); }
    if (ks != NULLPTR) { tputs(ks,1,putch); }
  }
  return(erg);
}

/*
 * TERMCAP
 * =======
 */


static struct keyentry
 { char	*seq;
   int   cod;
 } keytab[] =
    {  NULLPTR ,UP
      ,NULLPTR ,DOWN
      ,NULLPTR ,RIGHT
      ,NULLPTR ,LEFT
      ,NULLPTR ,HOME
      ,NULLPTR ,F1
      ,NULLPTR ,F2
      ,NULLPTR ,F3
      ,NULLPTR ,F4
      ,NULLPTR ,F5
      ,NULLPTR ,F6
      ,NULLPTR ,F7
      ,NULLPTR ,F8
      ,NULLPTR ,SHIFT_F1
      ,NULLPTR ,SHIFT_F2
      ,NULLPTR ,SHIFT_F3
      ,NULLPTR ,SHIFT_F4
      ,NULLPTR ,SHIFT_F5
      ,NULLPTR ,SHIFT_F6
      ,NULLPTR ,SHIFT_F7
      ,NULLPTR ,SHIFT_F8
};
#if FULL
static char * keyname[] =
    {  "UP"
      ,"DOWN"
      ,"RIGHT"
      ,"LEFT"
      ,"HOME"
      ,"F1"
      ,"F2"
      ,"F3"
      ,"F4"
      ,"F5"
      ,"F6"
      ,"F7"
      ,"F8"
      ,"SHIFT_F1"
      ,"SHIFT_F2"
      ,"SHIFT_F3"
      ,"SHIFT_F4"
      ,"SHIFT_F5"
      ,"SHIFT_F6"
      ,"SHIFT_F7"
      ,"SHIFT_F8"
};
#endif

#define KEYTABLEN sizeof(keytab) / sizeof(struct keyentry)
#if FULL
int keytablen = KEYTABLEN;
#endif

#define MK 16
static char keys[KEYTABLEN][MK];

int putch(c) int c; { putchar(c); return(c); }

#if TRICK1_AP
/* dirty trick wg. problem mit apollo       */
/* curses statt termcap                     */
#define CHGE1(c) /* c = ((c) == 0x4f) ? 0x5b : (c)	*/
#else
#define CHGE1(c)
#endif

/* --------------------	*/
static void keydef(i,s)
 int i;
 char	*s;
 {
  register char	*k;

  keytab[i].seq = s;
  if (s != NULLPTR) {
    for (k = keys[i] ;	*k++ =  *s++ ; )
      ;
   }
  else {
    keys[i][0] = '\0';
  }
 }

#if FULL
/* ------------------------------------	*/
/* definition von tastatursequenzen fuer verschiedene tasten */
#define SHOW(c) fprintf(stdout,"0x%02x ",(int)(c) & 0x00ff)

#define KEYD "key definitions :\n"
readkeys(fp)
 FILE *fp;
 {
  register int i,j;
  int h,c;
  char seq[MK],keyn[16];
  char *k;

  k = KEYD;
  /* suche k von key..    */
  while ((c = getc(fp)) != EOF && (char)c != 'k')
    ;
  if (c == EOF) return(FALSE);
  /* heisst es key def....  (s.o.) ?        */
  while ((c = getc(fp)) != EOF && (char)c != '\n' && (char)c == *++k)
    ;
  if (c == EOF || (char)c != '\n') return(FALSE);
#ifdef UnBenutzt
  /* suche nach eintrag fuer dieses Terminal  */
  do {
    k = term;
    while ((c = getc(fp)) != EOF && (char)c != *k)
      ;
    if (c == EOF) return(FALSE);
    while ((c = getc(fp)) != EOF && (char)c != '\n' && (char)c == *++k)
      ;
    if (c == EOF || (char)c != '\n') return(FALSE);
  } while ((char)c != '\n'  || *k);
#endif
  for (i = 0; i < keytablen ; i++ ) {
    if ((j = fscanf(fp,"%*d : %s :",keyn)) != 1) {
#ifdef UnBenutzt
      printf("fehler bei %d mit %d\n",i,j); getchar();
#endif
      return(FALSE);
    }
    if (strcmp(keyn,keyname[i])) {
#ifdef UnBenutzt
      printf("fehler bei name %d \"%s\"\n",i,keyn); getchar();
#endif
      return(FALSE);
    }
    j = 0;
    while (fscanf(fp,"%x",&h) == 1  &&  j < MK) {
      seq[j] = (char)h;
      j++;
    }
    if (j >= MK) {
      return(FALSE);
    }
    while ((c = getc(fp)) != EOF && (char)c != '\n')
      if ((char)c != '-' && !isspace((char)c)) return(FALSE);
    if (c == EOF) return(FALSE);
    if (j) {
      seq[j] = '\0';
      do {
        keys[i][j] = seq[j];
        j--;
      } while (j >= 0);
      keytab[i].seq = keys[i];
    }
  }
  return(TRUE);
 }
void showkeys(fp)
 FILE *fp;
 {
  register int i,j;
  char c;

  if (fp == stdout) {
    CLEARSCREEN();
    fprintf(fp," ---------- key definitions -------------\r\n");
    fprintf(fp,"please enter number of key to define :\r\n\n\n\n");
   }
  else {
    fprintf(fp,KEYD);
  }
  for (i = 0; i < keytablen ; i++ ) {
    fprintf(fp,"%2d : %8s : ",i,keyname[i]);
    for (j = 0; (c = keys[i][j]) ; j++ ) {
      if (fp == stdout) {
        SHOW(c);
       }
      else fprintf(fp,"%x ",0x00ff & (int)c);
    }
    if (fp == stdout) {
      if (i % 2) fprintf(fp,"\r\n");
      else fprintf(fp,"    ");
     }
    else {
      fprintf(fp,"--\n");
    }
  }
  if (fp == stdout) fflush(stdout);
 }
static int _getnumber(i)
 int *i;
 {
  register char c;
  register int j,ok;

  j = ok = 0;
  while ((c = getchar()) != '\n'  &&  isdigit(c)) {
    ok = 1;
    j = j*10 + (c - '0');
    putc(c,stdout);
  }
  *i = j;
  return(ok);
 }


#ifdef SETBUFFER
/* buffer fuer stdout	*/
#define MY_BUFSIZ	32768
static char STDBUF[MY_BUFSIZ];	/* BUFSIZ in stdio.h	*/

/* --------- unbuf ------------------------------------------	*/
/* leert buffer und setzt ungepufferte ausgabe	*/
void unbuf()
{
 fflush(stdout);
 setbuffer(stdout,(char *)0,0);
}

/* --------- buff -------------------------------------------	*/
/* setzt gepufferte ausgabe	*/
void buff()
{
 register int i;

 setbuffer(stdout,STDBUF,MY_BUFSIZ);
 for (i = MY_BUFSIZ-1 ; i ; i-- )
   STDBUF[i] = '\0';	/* init buffer for safety	*/
}
#endif

#define ENDE ' '
defkeys()
 {
  char seq[MK];
  char *s,*t,ende,c;
  int i,j;

  showkeys(stdout);
  unbuf();
  CURSORXY(1,40);
  while (_getnumber(&i)  &&  0 <= i && i < KEYTABLEN) {
    CURSORXY(2,0);
    fprintf(stdout,"please press key(s), terminated by two spaces  \n");
    CURSORXY(3,0);
    c = ende = '\0';
    j = 0;
    do {
      ende = c;
      c = getchar();
      seq[j++] = c;
      SHOW(c);
    } while ((ende != ENDE || c != ENDE) && j < MK);
    seq[j-2] = '\0';
    c = seq[0];
    if (isascii(c) && isprint(c) || j >= MK) {
      CURSORXY(2,0);
      if (j < MK)
        fprintf(stdout,"sequence must start with a nonprintable char !\n  ");
      else fprintf(stdout,"sequence is too long !                        \n  ");
      getchar();
     }
    else {
      t = seq;
      for (s = keys[i] ; *s++ =  *t++ ; )
        ;
      keytab[i].seq = keys[i];
    }
    showkeys(stdout);
    CURSORXY(1,40);
  }
  buff();
 }
#endif	/* FULL	*/

#define TGETSTR(name,cap,err,def)  if ((name = tgetstr(cap,&s)) == NULLPTR) { if (verbose) fprintf(stderr,"no %s capability (%d)\n",cap,++err); name = def; }

/* -------- termcaps ----------------------------------------	*/
int termcaps(verbose)
 int verbose;
{
  static char	*term,*s,area[1024],caps[1024];
#if HP700 + HP92
  static char hu[6],hd[6],hr[6],hl[6];
#endif
  int erg,errcnt,i;

#if APOLLO
  getsize((long *)&li,(long *)&co);       /* hole window-groesse     */
#endif

  s = caps;
  i = erg = errcnt = 0;
  if ((term = getenv("TERM")) == NULLPTR) {
     fprintf(stderr,"Can't find TERM entry. Stop.\n");
     return(FALSE);
  }
  if ((erg = tgetent(area,term)) == FAILURE) {
     fprintf(stderr,"Can't find termcap file. Stop.\n");
     return(FALSE);
   }             else
  if (erg == 0) {
     fprintf(stderr,"Can't find termcap entry for \"%s\". Stop.\n",term);
     return(FALSE);
  }

/*
tabspacing	   = ((tabspacing=tgetnum("it"))==-1 ? 8 : tabspacing);
_automargin	   = tgetflag("am");
_eatnewlineglitch   = tgetflag("xn");
*/

#if !APOLLO
  if ((li = tgetnum("li")) == FAILURE) li = LINES;
  if ((co = tgetnum("co")) == FAILURE) co = COLS;
#endif /* APOLLO */

 if (ONLY_LICO==0) {

  TGETSTR(ku,"ku",errcnt,"") else { CHGE1(ku[1]); }
  TGETSTR(kd,"kd",errcnt,"") else { CHGE1(kd[1]); }
  TGETSTR(kr,"kr",errcnt,"") else { CHGE1(kr[1]); }
  TGETSTR(kl,"kl",errcnt,"") else { CHGE1(kl[1]); }
  TGETSTR(kh,"kh",i,KH);

  TGETSTR(up,"up",i,ku);
  TGETSTR(down,"do",i,kd);
  TGETSTR(nd,"nd",i,kr);
  TGETSTR(le,"le",i,kl);
  TGETSTR(ho,"ho",i,kh);

  TGETSTR(ti,"ti",i,"");
  TGETSTR(te,"te",i,"");
  TGETSTR(cl,"cl",errcnt,"");
  TGETSTR(ce,"ce",errcnt,"");
  TGETSTR(cd,"cd",errcnt,"");
  TGETSTR(ic,"ic",i,IC);
  TGETSTR(dc,"dc",i,DC);
  TGETSTR(cm,"cm",errcnt,"");
  TGETSTR(ks,"ks",i,"");
  TGETSTR(ke,"ke",i,"");
  TGETSTR(ml,"ml",i,"");
  TGETSTR(mu,"mu",i,"");
  if (errcnt) {
     fprintf(stderr,"Stop.\n");
     return(FALSE);
  }

SHOWCAP(ku);
SHOWCAP(kd);
SHOWCAP(kr);
SHOWCAP(kl);
SHOWCAP(kh);

  so = tgetstr("so",&s); se = tgetstr("se",&s);
  us = tgetstr("us",&s); ue = tgetstr("ue",&s);
  k0 = tgetstr("k0",&s); k1 = tgetstr("k1",&s);
  k2 = tgetstr("k2",&s); k3 = tgetstr("k3",&s);
  k4 = tgetstr("k4",&s); k5 = tgetstr("k5",&s);
  k6 = tgetstr("k6",&s); k7 = tgetstr("k7",&s);
  k8 = tgetstr("k8",&s); k9 = tgetstr("k9",&s);
  l0 = tgetstr("l0",&s); l1 = tgetstr("l1",&s);
  l2 = tgetstr("l2",&s); l3 = tgetstr("l3",&s);
  l4 = tgetstr("l4",&s); l5 = tgetstr("l5",&s);

#if HP700
  sprintf(hu,"%c%c%c\0",ESC,(char)0x5b,'A'); keydef( 0,hu);
  sprintf(hd,"%c%c%c\0",ESC,(char)0x5b,'B'); keydef( 1,hd);
  sprintf(hr,"%c%c%c\0",ESC,(char)0x5b,'C'); keydef( 2,hr);
  sprintf(hl,"%c%c%c\0",ESC,(char)0x5b,'D'); keydef( 3,hl);
#else
#if HP92
  sprintf(hu,"%c%c\0",ESC,(char)0x74); keydef( 0,hu);
  sprintf(hd,"%c%c\0",ESC,(char)0x75); keydef( 1,hd);
  sprintf(hl,"%c%c\0",ESC,(char)0x77); keydef( 2,hl);
  sprintf(hr,"%c%c\0",ESC,(char)0x76); keydef( 3,hr);
#else
  keydef( 0,ku); keydef( 1,kd); keydef( 2,kr); keydef( 3,kl);
SHOWCAP(keytab[0].seq);
#endif
#endif

  keydef( 4,kh); keydef( 5,k0);
  keydef( 6,k1); keydef( 7,k2); keydef( 8,k3);
  keydef( 9,k4); keydef(10,k5); keydef(11,k6);
  keydef(12,k7); keydef(13,k8); keydef(14,k9);
  keydef(15,l0); keydef(16,l1); keydef(17,l2);
  keydef(18,l3); keydef(19,l4); keydef(20,l5);

 } /* End if ONLY_LICO */

  return(TRUE);
}

/* ------ keypressed ------------------------------------------	*/
/* ist eine taste gedrueckt ?					*/
int keypressed()
{
  return(0);
}

/* ------ unget ------------------------------------------	*/
VOID unget(c)
 int c;
 {
  ungetc(c,stdin);
 }

/* ------ substring ------------------------------------------	*/
/* is s1 substring of s2 ?					*/
/* 0: equal			*/
/* 1: s1 substring of s2	*/
/* 2: s1 is no substring of s2	*/
/* -1: failure			*/
static int substring(s1,s2)
char	*s1,*s2;
{
  if ((s1 != NULLPTR) && (s2 != NULLPTR)) {
     while (*s1 == *s2) {
        if (*s1 != '\0') { s1++;s2++; }
        else return(0);	/* 0 : equal	*/
     }
     if (*s1 == '\0') return(1);	/* 1 : substring	*/
     else return(2);	/* 2 : not substring	*/
   }
  else return(-1);	/* -1 : undefined	*/
}

/* ----------- getkey ---------------------------------------	*/
int getkey()
{
  static int n;
  static char sequ[32];
  char c;
  int i;

  *sequ = c = getchar();
#ifdef DEBUG
  fprintf(stderr,"got char %x\n",c);
#endif	/* endif	*/
  if (!isprint(c) || !isascii(c)) {
    i = 1;
    sequ[i] = '\0';
    for (n = 0 ; n < KEYTABLEN ; n++) {/* durchsuche tabelle	*/
       switch(substring(sequ,keytab[n].seq)) {
          case 0 : 
#ifdef DEBUG
fprintf(stderr,"got key %d\n",n);
#endif	/* endif	*/
            return((int)(keytab[n].cod));
          case 1 :
            sequ[i++] = getchar();
#ifdef DEBUG
fprintf(stderr,"got next char %x (entry %d)\n",sequ[i-1],n);
#endif	/* endif	*/
            sequ[i]   = '\0';
            --n;
            break;
          default:
#ifdef DEBUG
            SHOWCAP(sequ);
            SHOWCAP(keytab[n].seq);
            fprintf(stderr,"wrong substring\n");
#endif	/* endif	*/
            ;
       }
    }
  }
  if (*sequ == LF)	*sequ = CR;	/* map lf to cr   */
  return((*sequ != ESC) ? (0x00ff & (int)(*sequ)) : UNKNOWNKEY);
}


/* --------------------------------------------	*/

/* --------------------	*/
/* funktion fuer reduma	*/
void cursorxy(x,y)
 int x,y;
 {
   CURSORXY(x,y);
 }

void getnew_termsizes(x,y)
int *x,*y;
{ /* New function to resize window */
 ONLY_LICO=1;
 if (termcaps(0)) {
  *x=co;
  *y=li;
 }
 ONLY_LICO=0;
}

#ifdef FULL

/* putrev(): print a character revers */
VOID putrev(c)
int c;
{
 reverse();
 putchar(c);
 noreverse();
}
#endif

/* als funktion fuer reduma !	*/
VOID clearscreen()
 {
#if HP700
  int i,j;
#endif

  CLEARSCREEN();
#if HP700
  for (i = li ; i > 0 ; i-- ) {
    for (j = co ; j > 0 ; j-- ) {
      putchar(' ');
    }
  }
  CLEARSCREEN();
#endif
 }

#if SINTRAN3

/*****************************************************************************/
/* Terminalsteuerung:                                                        */
/* ~~~~~~~~~~~~~~~~~                                                         */
/* switchterm() : Schaltet das Terminal in den RAW-Modus (Zeichenweise       */
/*               Eingabe,Steuercodes die vom Terminal eingegeben werden,     */
/*               werden nicht automatisch ausgefuehrt)                       */
/*               bzw. in den nomalen LINE-Modus (Zeilenweise Eingabe)        */
/*****************************************************************************/

switchterm(mode)
int mode ;
{
  if (mode)                                          /* RAWMODE              */
   { fputs("\33Q",stdout);                           /* Extended Control on  */
     fputs("\33[62l",stdout);                        /* GRM auf ATTR         */
     fputs("\33[62;62h",stdout);                     /* GRM auf SGR          */
     fflush(stdout);
     SWITCHTTY();                                    /* --> "monitor:c"      */
   }
  else                                               /* LINEMODE             */
   { RESETTTY();                                     /* --> "monitor:c       */
     fputs("\33[62l",stdout);                        /* GRM auf ATTR         */
     fputs("\33[66l",stdout);                        /* Extended Control off */
     fflush(stdout);
   }
}

/*****************************************************************************/
/* CURSORXY():   (Tandberg-Terminal)                                         */
/*****************************************************************************/

CURSORXY(row,col)                      /* Positioniert den Cursor           */
int row,col;
{
 char *longtoasc();
 putchar(ESC);
 putchar('[');
 fputs(longtoasc((long)(row + 1)),stdout);
 putchar(';');
 fputs(longtoasc((long)(col + 1)),stdout);
 putchar('H');
}

/*****************************************************************************/
/* Tastaturabfrage:                                                          */
/* ~~~~~~~~~~~~~~~                                                           */
/* getkey() : liest Zeichen und Escapesequenzen von der Tastatur und         */
/*            liefert einen entsprechenden Integer-Wert.                     */
/*****************************************************************************/

getkey()
{
 int c;

 c = readchar();
 if (c != ESC)
    return(c);
 else
  { switch(c = readchar())
    { case '[' : switch(c = readchar())                   /* CSI-Sequenzen  */
                 { case '0' :                            /* Tandberg       */
                              switch(c = readchar())
                              { case '0' : c = MARK;        break; /* \33[00_ */
                                case '1' : c = SHIFT_MARK;  break; /* \33[01_ */
                                case '2' : c = FIELDK;       break; /* \33[02_ */
                                case '3' : c = SHIFT_FIELDK; break; /* \33[03_ */
                                case '4' : c = PARA;        break; /* \33[04_ */
                                case '5' : c = SHIFT_PARA;  break; /* \33[05_ */
                                case '6' : c = SENT;        break; /* \33[06_ */
                                case '7' : c = SHIFT_SENT;  break; /* \33[07_ */
                                case '8' : c = WORDK;       break; /* \33[08_ */
                                case '9' : c = SHIFT_WORD;  break; /* \33[09_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '1' :                            /* Tandberg       */
                              switch(c = readchar())
                              { case '0' : c = DELETE;      break; /* \33[10_ */
                                case '1' : c = SHIFT_DELETE;break; /* \33[11_ */
                                case '2' : c = COPY;        break; /* \33[12_ */
                                case '3' : c = SHIFT_COPY;  break; /* \33[13_ */
                                case '4' : c = MOVE;        break; /* \33[14_ */
                                case '5' : c = SHIFT_MOVE;  break; /* \33[15_ */
                                case '6' : c = TAB;         break; /* \33[16_ */
                                case '7' : c = SHIFT_TAB;   break; /* \33[17_ */
                                case '8' : c = POINTS;      break; /* \33[18_ */
                                case '9' : c = SHIFT_POINTS;break; /* \33[19_ */
                                default  : c = UNKNOWNKEY;
                               }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '2' :                            /* Tandberg       */
                              switch(c = readchar())
                              { case '0' : c = AAA;         break; /* \33[20_ */
                                case '1' : c = SHIFT_AAA;   break; /* \33[21_ */
                                case '2' : c = LELE;        break; /* \33[22_ */
                                case '3' : c = SHIFT_LELE;  break; /* \33[23_ */
                                case '4' : c = JUST;        break; /* \33[24_ */
                                case '5' : c = SHIFT_JUST;  break; /* \33[25_ */
                                case '6' : c = LERI;        break; /* \33[26_ */
                                case '7' : c = SHIFT_LERI;  break; /* \33[27_ */
                                case '8' : c = PGUP;        break; /* \33[28_ */
                                case '9' : c = SHIFT_PGUP;  break; /* \33[29_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '3' :                            /* Tandberg       */
                              switch(c = readchar())
                              { case '0' : c = CANCEL;      break; /* \33[30_ */
                                case '1' : c = SHIFT_CANCEL;break; /* \33[31_ */
                                case '2' : c = PGDOW;       break; /* \33[32_ */
                                case '3' : c = SHIFT_PGDOW; break; /* \33[33_ */
                                case '4' : c = PGLE;        break; /* \33[34_ */
                                case '5' : c = SHIFT_PGLE;  break; /* \33[35_ */
                                case '6' : c = PGRI;        break; /* \33[36_ */
                                case '7' : c = SHIFT_PGRI;  break; /* \33[37_ */
                                case '8' : c = TABLEF;      break; /* \33[38_ */
                                case '9' : c = SHIFT_TABLEF;break; /* \33[39_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '4' :                            /* Tandberg       */
                              switch(c = readchar())
                              { case '0' : c = TABRIG;      break; /* \33[40_ */
                                case '1' : c = SHIFT_TABRIG;break; /* \33[41_ */
                                case '2' : c = FUNC;        break; /* \33[42_ */
                                case '3' : c = SHIFT_FUNC;  break; /* \33[43_ */
                                case '4' : c = T_PRINT;     break; /* \33[44_ */
                                case '5' : c = SHIFT_PRINT; break; /* \33[45_ */
                                case '6' : c = HELP;        break; /* \33[46_ */
                                case '7' : c = SHIFT_HELP;  break; /* \33[47_ */
                                case '8' : c = EXIT;        break; /* \33[48_ */
                                case '9' : c = SHIFT_EXIT;  break; /* \33[49_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '5' :                            /* Tandberg       */
                              switch (c=readchar())
                              { case '0' : c = F1;          break; /* \33[50_ */
                                case '1' : c = SHIFT_F1;    break; /* \33[51_ */
                                case '2' : c = F2;          break; /* \33[52_ */
                                case '3' : c = SHIFT_F2;    break; /* \33[53_ */
                                case '4' : c = CTRL_F2;     break; /* \33[54_ */
                                case '5' : c = F3;          break; /* \33[55_ */
                                case '6' : c = SHIFT_F3;    break; /* \33[56_ */
                                case '7' : c = CTRL_F3;     break; /* \33[57_ */
                                case '8' : c = F4;          break; /* \33[58_ */
                                case '9' : c = SHIFT_F4;    break; /* \33[59_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   case '6' :
                              switch (c=readchar())
                              { case '0' : c = F5;          break; /* \33[60_ */
                                case '1' : c = SHIFT_F5;    break; /* \33[61_ */
                                case '2' : c = F6;          break; /* \33[62_ */
                                case '3' : c = SHIFT_F6;    break; /* \33[63_ */
                                case '4' : c = F7;          break; /* \33[64_ */
                                case '5' : c = SHIFT_F7;    break; /* \33[65_ */
                                case '6' : c = F8;          break; /* \33[66_ */
                                case '7' : c = SHIFT_F8;    break; /* \33[67_ */
                                default  : c = UNKNOWNKEY;
                              }
                              if (readchar() == '_')
                                 return(c);
                              else return(UNKNOWNKEY);
                   default  : readchar();
                              readchar();
                              return(UNKNOWNKEY);
                 }
      default  : return(UNKNOWNKEY);
    }
  }
}

#endif /* SINTRAN3        */

#endif /* UNIX	*/
#endif /* BATCH	*/

/* end of term.c	*/
