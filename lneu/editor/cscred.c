/* Bildschirm-Aufbau und -Variablen sowie Eingabebehandlung    cscred.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    26.10.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#define scred 1 /* fuer vereinbarungen in cscred.h */
#include "cscred.h"
#include "ctermio.h"

extern void unbuf(),buff();

#ifdef UnBenutzt
/* ------------ longtoasc ---------------------------------------- */
char *longtoasc(n)
long n;
{
 int neg,i = 20;
 static char s[20];

 if (neg = (n < 0L))
    n = -n;
 s[--i] = '\0';
 do {
    s[--i] = n % 10 + '0';
  } while (( n /= 10) > 0L);
 if (neg)
    s[--i] = '-';
 return(s + i);
}
#endif /* UnBenutzt	*/

/* fuer 'automatisches' loeschen des message-feldes vor ausgabe einer  */
/* neuen message (zumindest an der alten position)                     */
static int msg_displ,              /* message gezeigt ?                */
           l_m_row,l_m_col;        /* letzte position der meldung      */
#define MSG_TRUE  { msg_displ = TRUE; l_m_row = mrow; l_m_col = mcol; }
#define MSG_FALSE   msg_displ = FALSE
#define MSG_CLEAR(clear) {if(msg_displ && l_m_row==mrow && l_m_col==mcol) clear;}

/* ------------ initscreen --------------------------------------- */
/* Bildschirminitialisierung                                       */
initscreen()
 {
  int i;

  if (!batch) {
    CLEARSCREEN();
    fprintf(stdout,"Message :\n\r");
    fprintf(stdout,"Input   :\n\r");
    for ( i = 1; i <= co; i++) {
      if (i % 10) putc('-',stdout);
      else        putc('+',stdout);
    }
    putc('\r',stdout);
    putc('\n',stdout);
    srow = FAROW;
    scol = FACOL;
    MSG_FALSE;
  }
 }
/* ------------ putstr ------------------------------------------- */
/* ausgabe von s                                                   */
/* besser fputs(s,stdout) !                                        */
putstr(s)
char *s;
 {
  while (*s) {
     putc(*s++,stdout);
  }
}
/* ------------ printstr ----------------------------------------- */
/* stringausgabe, cntrl-zeichen als ^, dabei aendern von scol,srow */
printstr(s)
 char *s;
 {
  register char c;

  while ( (c = *s++) != '\0' ) {
    if (iscntrl(c)) { putc('^',stdout); }
               else { putc(c,stdout);   }
    scol++;
  }
  srow += scol / co;
  scol  = scol % co;
 }
/* ------------ message ------------------------------------------ */
/* ausgabe der meldung str an pos. mrow,mcol, danach rueckkehr zur */
/* gegenwaertigen cursorposition (srow,scol)                       */
message(mrow,mcol,str)
 int mrow,mcol;
 char *str;
 {
  register char c;

  if (!batch) {
    MSG_CLEAR(iclrmsg(mrow,mcol));
    CURSORXY(mrow,mcol);
  }
  while (*str)  { c = *str++; putc(c,stdout); }
  if (!batch) {
    CURSORXY(srow,scol);
    fflush(stdout);
    MSG_TRUE;
   }
  else if (c != '\n') putc('\n',stdout);
 }
/* ------------ mesg2 -------------------------------------------- */
/* wie oben, nur zwei strings                                      */
mesg2(mrow,mcol,str1,str2)
 int mrow,mcol;
 char *str1,*str2;
 {
  register char c;

  if (!batch) {
    MSG_CLEAR(iclrmsg(mrow,mcol));
    CURSORXY(mrow,mcol);
  }
  while (*str1)  { putc(*str1++,stdout); }
  while (*str2)  { c = *str2++; putc(c,stdout); }
  if (!batch) {
    CURSORXY(srow,scol);
    fflush(stdout);
    MSG_TRUE;
   }
  else if (c != '\n') putc('\n',stdout);
 }
/* ------------ iclrmsg ------------------------------------------ */
/* loeschen zeile mrow ab pos. mcol                                */
iclrmsg(mrow,mcol)
 int mrow,mcol;
 {
  if (!batch) {
    CURSORXY(mrow,mcol);
    cleartoeol();
  }
 }
/* ------------ clrmsg ------------------------------------------- */
/* loeschen zeile mrow ab pos. mcol                                */
clrmsg(mrow,mcol)
 int mrow,mcol;
 {
  if (!batch) {
    CURSORXY(mrow,mcol);
    cleartoeol();
    CURSORXY(srow,scol);
    fflush(stdout);
    MSG_FALSE;
  }
 }
/* ------------ clrmsg2  ------------------------------------------- */
/* wie oben, nur danach positionierung des cursors auf row,col       */
clrmsg2(mrow,mcol,row,col)
 int mrow,mcol,row,col;
 {
  if (!batch) {
    CURSORXY(mrow,mcol);
    cleartoeol();
    CURSORXY(row,col);
    fflush(stdout);
    MSG_FALSE;
  }
 }
/* ------------ mesg --------------------------------------------- */
/* wie message, nur danach positionierung auf rrow,rcol            */
mesg(mrow,mcol,str,rrow,rcol)
 int mrow,mcol,rrow,rcol;
 char *str;
 {
  register char c;

  if (!batch) {
    MSG_CLEAR(iclrmsg(mrow,mcol));
    CURSORXY(mrow,mcol);
  }
  while (*str)  { c = *str++; putc(c,stdout); }
  if (!batch) {
    CURSORXY(rrow,rcol);
    fflush(stdout);
    MSG_TRUE;
   }
  else if (c != '\n') putc('\n',stdout);
 }
/* ------------ dspstr ------------------------------------------- */
/* wie message ohne nachfolgende cursorpositionierung              */
dspstr(mrow,mcol,str)
 int mrow,mcol;
 char *str;
 {
  register char c;

  if (!batch) {
    MSG_CLEAR(iclrmsg(mrow,mcol));
    CURSORXY(mrow,mcol);
  }
  while (*str)  { c = *str++; putc(c,stdout); }
  if (!batch) {
    fflush(stdout);
    MSG_TRUE;
   }
  else if (c != '\n') putc('\n',stdout);
 }
/* ------------ dspstr2 ------------------------------------------ */
/* entspr. dspstr mit zwei strings                                 */
dspstr2(mrow,mcol,str1,str2)
 int mrow,mcol;
 char *str1,*str2;
 {
  register char c;

  if (!batch) {
    MSG_CLEAR(iclrmsg(mrow,mcol));
    CURSORXY(mrow,mcol);
  }
  while (*str1)  { putc(*str1++,stdout);   }
  while (*str2)  { c = *str2++; putc(c,stdout);   }
  if (!batch) {
    fflush(stdout);
    MSG_TRUE;
   }
  else if (c != '\n') putc('\n',stdout);
 }

/* Tabulatorschritt */
#define TABSTEP 8
/* Spalten fuer Error-,Insert- und Positionmessage */
#define EMCOL          (co-34)
#define IMCOL          (co-19)
#define PMCOL          (co-12)

/* Vorsicht: die folgenden Makros beim Aufruf korrekt klammern ! */
#define show_line()    CURSORXY(row,col);                                  \
                       cleartoeol();                                       \
                       for (h=nshift;(h<maxpos)&&(h<=(lim+nshift));h++)    \
                           putchar(buf[h]);                                \
                       sprintf(str," %5d-%-5d",nshift+1,nshift+lim+1);       \
                       mesg(MROW,PMCOL,str,row,col+wpos)

#define shift_right()  nshift += shift; \
                       wpos   -= shift; \
                       show_line()

#define shift_left()   nshift -= shift; \
                       wpos   += shift; \
                       show_line()

#define D_ERROR(s)       mesg(MMROW,EMCOL,s,row,col+wpos); \
                       errflag = !errflag

/*                                                                           */
/* getline : Zeileneditor                                                    */
/*                                                                           */
/* Externals :   getkey()                   Funktion                         */
/*               CURSORXY(row,col)          Makro                            */
/*               cleartoeol()               "                                */
/*               CURSOR_LEFT()              "                                */
/*               CURSOR_RIGHT()             "                                */
/*               insertchar()               "                                */
/*               deletechar()               "                                */
/*               beep()                     "                                */
/*               LEFT,RIGHT,UP,DOWN,HOME    Konstanten                       */
/*               DEL,_DELETE,CR,CNTC,EXIT   "                                */
/*               _DELEOL,_INSERT            "                                */
/*               _insert,_delete            "                                */
/*               MMROW,MMCOL,co,TABRIG,TABLEF                                */
/*               lpos,lmxpos                int                              */
/*               clrmsg2(mrow,mcol,row,col)                                  */
/*               mesg(mrow,mcol,mes,row,col)                                 */
/*                                                                           */
/* --------------- getline ------------------------------------------- */
/* einlesen einer zeile nach buf                                       */
getline(buf,buflen,pos,maxpos,row,col,lim)
char buf[];                                 /* Puffer                  */
int  buflen;                                /* Laenge des Puffers      */
int  pos;                                   /* Pufferzeiger            */
int  maxpos;                                /* max. erreichte Position */
int  row,col;                               /* Position der Eing.zeile */
int  lim;                                   /* Laenge der Eing.zeile-1 */
{
 int c,h,i;                                 /* Hilfsvariablen          */
 int wpos;                                  /* Pos. im Bildschirmfenster */
 int shift;                                 /* Verschiebungs Einheit   */
 int nshift;                                /* Gesamtverschiebung      */
 int insrt   = NO;                          /* Insertmode              */
 int errflag = NO;                          /*                         */
 char str[40];                              /*                         */

 shift  = (lim+1) / 2 ;                     /* Einheit                 */
 nshift = pos - (pos % shift);              /* Initiale Verschiebung   */
 wpos   = pos - nshift ;                    /* Pos. im Sichtfenster    */

 unbuf();
 for ( i = maxpos ; i < buflen ; i++ ) {
   buf[i] = ' ';
 }
 show_line();                               /* Zeile anzeigen          */

 do
  { c = getkey();                           /* read char               */
    if (errflag)
     {
       D_ERROR("               ");            /* clear errormessage      */
     }
    if (isprint(c))                         /* printable character?    */
     { if (wpos >= lim)                     /* line full? */
        {
          shift_right();
        }
       if (insrt)                                 /* insertmode ? */
        { if (maxpos < buflen)                    /* enough space? */
           { for (h = maxpos++ ; h > pos ; --h)   /* insert */
                 buf[h] = buf[h-1];               /* in buffer */
             insertchar();                        /* on screen */
           }
          else
           { beep();
             D_ERROR(" Out of Buffer!");           /* buffer full! */
             continue;
           }
        }
       if (pos < buflen)                          /* enough space? */
        {
          for ( ; maxpos <= pos ; )               /* set maxpos and */
              buf[maxpos++] = ' ';                /* clear buffer-area */
          buf[pos++] = c;                         /* new char */
          putchar(c);
          wpos++;                                 /* next position */
        }
       else
        { beep();
          D_ERROR(" Out of Buffer!");              /* buffer full */
          continue;
        }
     }
    else
     { switch(c)
        { case LEFT    : if (wpos == 0)
                          { if (nshift >= shift)
                             {
                               shift_left();
                             }
                            else break;
                          }
                         --pos ;
                         --wpos;
                         CURSOR_LEFT();
                         break;
          case RIGHT   : if (pos >= buflen)
                          { beep();
                            D_ERROR(" Out of Buffer!");
                            break;
                          }
                         if (wpos >= lim)
                          {
                            shift_right();
                          }
                         pos++ ;
                         wpos++;
                         CURSOR_RIGHT();
                         break;
          case UP      : if (nshift >= shift)
                          {
                            wpos -= shift;
                            pos  -= shift;
                            shift_left();
                          }
                         break;
          case DOWN    : wpos += shift;
                         pos  += shift;
                         shift_right();
                         break;
          case HOME    : pos = 0;
                         wpos = 0;
                         nshift = 0;
                         show_line();
                         break;
          case HT      : /* horizontal tab */
#ifdef TABRIG
          case TABRIG  :
#endif
                         h = TABSTEP - (pos % TABSTEP);
                         pos  += h;
                         wpos += h;
                         while (wpos > lim)
                          {
                            shift_right();
                          }
                         CURSORXY(row,col+wpos);
                         break;
          case 6       : /* backtab Notloesung */
#ifdef TABLEF
          case TABLEF  :
#endif
                         h = ((pos -1) % TABSTEP) + 1;
                         pos  -= h;
                         wpos -= h;
                         while (wpos < 0)
                          {
                            shift_left();
                          }
                         CURSORXY(row,col+wpos);
                         break;
          case _INSERT :
          case _insert : if (insrt)
                          { insrt = NO;
                            mesg(MMROW,IMCOL,"       ",row,col+wpos);
                          }
                         else
                          { insrt = YES;
                            mesg(MMROW,IMCOL," Insert",row,col+wpos);
                          }
                         break;
          case DEL     : if ((wpos == 0) && (nshift >= shift))
                          {
                            shift_left();
                          }
                         if (wpos > 0)
                          { for (h = --pos ; h < maxpos ; h++)
                                buf[h] = buf[h+1];
                            if (pos < maxpos)
                               --maxpos;
                            CURSOR_LEFT();
                            --wpos;
                            deletechar();
                            if (maxpos > (nshift+lim))
                             { CURSORXY(row,col+lim);
                               putchar(buf[nshift+lim]);
                               CURSORXY(row,col+wpos);
                             }
                            break;
                          }
          case _DELETE :
          case _delete : if (pos < maxpos)
                          { for (h = pos ; h < maxpos ; h++)
                                buf[h] = buf[h+1];
                            --maxpos;
                            deletechar();
                            if (maxpos > (nshift+lim))
                             { CURSORXY(row,col+lim);
                               putchar(buf[nshift+lim]);
                               CURSORXY(row,col+wpos);
                             }
                          }
                         break;
          case _DELEOL : cleartoeol();
                         pos = nshift + wpos;
                         if (maxpos > pos)
                            maxpos = pos;
                         break;
          case EXIT    :
          case CNTC    : lmxpos = lpos = maxpos = pos = 0;
                         *buf = '\0';
                         buff();
                         return(-1);
          case CR      : break;
          default      : beep();
        } /* end switch */
     } /* end else */
  } while (c != CR);
 if (insrt) clrmsg2(MMROW,MMCOL,row,col);
 lpos = pos;
 lmxpos = maxpos;
 buf[maxpos] = '\0';
 buff();
 return(maxpos);
}

/* ----------- readword ---------------------------------------------- */
char *readword(buf,buflen)
 char	*buf;
 int	buflen;
{
 char *skipblanks();
 register char *s;
 register int c;

 c = getline(buf,buflen,lpos = 0,lmxpos = 0,INROW,INCOL,buflen);
 clrmsg(INROW,INCOL);
 if (c < 0) return(NILCHAR);
 s = buf = skipblanks(buf);
 if (*s != '\0') {
   while (*s && !isspace(*s))
     s++;
   *s = '\0';
   return(buf);
  }
 else return(NULLPTR);
}

/* ----------- scanword ---------------------------------------------- */
scanword(buf,wbuf,maxlen)
 char *(*buf),*wbuf;
 int maxlen;
 {
  char c,*skipblanks();
  int n = 0;

  *buf = skipblanks(*buf);
  while ((c = *(*buf)++)) {
    if (!isspace(c) && (n < maxlen)) {
      *wbuf++ = c;
      n++;
     }
    else break;
  }
  *buf = skipblanks(*buf);
  *wbuf = '\0';
  return(n);
}

#if SINTRAN3
/****************************************************************************/
/* Externe Funktionen der Pascal Monitor-Bibliothek                         */
/****************************************************************************/

extern int  ANYKEY() ;

/****************************************************************************/
/* globale Variablen fuer readchar() und backch()                           */
/****************************************************************************/

static int char_in_buf = 0 ; static int bufchar = 'X' ;

/*****************************************************************************/
/* readchar() - backch()                                                     */
/* Routinen um getch() und ungetch() zu simulieren.                          */
/* backch() darf nicht mehrmals hintereinander aufgerufen werden, ohne       */
/* dass das zurueckgestellte Zeichen zuvor durch ein readch() abgeholt wird. */
/* scanf hat ebenfalls keinen Zugriff auf die mit backch() zurueckgestellten */
/* Zeichen.                                                                  */
/* Die Routinen arbeiten ueber die globalen Variablen char_in_buf und        */
/* bufchar.                                                                  */
/*                                                      JS  10.9.86          */
/*****************************************************************************/

int  readchar()
{
 int  c = ' ';

 if (char_in_buf) {
   char_in_buf = 0;
   return(bufchar);
  }
 else {
    c=( 0177 & ANYKEY());
    return(c);
  }
}

unget(c)
int  c;
{
 char_in_buf = 1;
 bufchar = c ;
}
getunget()
{
 int c;

 if (!batch) {
   c = readchar();
   unget(c);
 }
}

#else            /* nicht sintran3 */

int readchar()
{
 return(getchar());
}

getunget()
{
 register int c;

 if (!batch) {
   c = readchar();
   unget(c);
 }
}

#endif  /* SINTRAN3 / UNIX */
/* #ifdef Nix    */
void waitkey()
{
 register int c;

 if (!batch) {
   c = getkey();
   if (c == 3) error("abbruch in waitkey\n");
 }
}
/* #endif        */
/* end of      cscred.c */
