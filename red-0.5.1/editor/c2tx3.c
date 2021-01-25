/*						ctotx3.c	*/

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cestack.h"
#include "cencod.h"
#include "cetb.h"
#include "ctred.h"
#include "cparm.h"
#include "ccommand.h"
#include "ckwtb.h"
#include "cfiles.h"

#define APARMS	348	/* groesse der ascii-darstellung der parms	*/

static int wrt_fd,rd_fd;
#define WRT	"write.red"
#define RD	"read.red"

#define open_tree(p,d,m)	if ((wrt_fd = open(WRT,O_CREAT|O_RDWR)) == FAILURE) \
	{fprintf(stderr,"can't open \"%s\"\n",WRT);exit(9); } \
	if ((rd_fd = open(RD,O_CREAT|O_RDWR)) == FAILURE) \
	{fprintf(stderr,"can't open \"%s\"\n",RD);exit(9); }
#define close_tree()	close(wrt_fd); close(rd_fd)

#define write_tree(b,n)	write(wrt_fd,b,n)
#define read_tree(b,n)	read(rd_fd,b,n)


static char cparms[APARMS];

void p2tx3(Parms)
PTR_UEBERGABE Parms;
 {
sprintf(cparms,
"%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\m%08lx\m%08lx\m%08lx\m%08lx\m%08lx\m%8.4f\n%8.4f\n%8.4f\n%8.4f\n%60s\n"
,Parms->redcnt
,Parms->precision
,Parms->truncation
,Parms->indentation
,Parms->heapdes
,Parms->heapsize
,Parms->stackdes
,Parms->stackpage
,Parms->stackcntpage
,Parms->qstacksize
,Parms->mstacksize
,Parms->istacksize
,Parms->formated
,Parms->base
,Parms->beta_count
,Parms->errflag
,Parms->file_desc
,Parms->prec_mult
,Parms->debug_info
,Parms->add_par1
,Parms->add_par2
,Parms->preproc_time
,Parms->process_time
,Parms->postproc_time
,Parms->total_time
,Parms->errmes);
}

int tx32p(Parms)
PTR_UEBERGABE Parms;
 {
#define ANZEL	25
  int res = TRUE;
  char *c,*e;
  int i;

if (sscanf(cparms,
"%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\n%08lx\m%08lx\m%08lx\m%08lx\m%08lx\m%08lx\m%8.4f\n%8.4f\n%8.4f\n%8.4f\n"
,&(Parms->redcnt)
,&(Parms->precision)
,&(Parms->truncation)
,&(Parms->indentation)
,&(Parms->heapdes)
,&(Parms->heapsize)
,&(Parms->stackdes)
,&(Parms->stackpage)
,&(Parms->stackcntpage)
,&(Parms->qstacksize)
,&(Parms->mstacksize)
,&(Parms->istacksize)
,&(Parms->formated)
,&(Parms->base)
,&(Parms.beta_count)
,&(Parms->errflag)
,&(Parms->file_desc)
,&(Parms->prec_mult)
,&(Parms->debug_info)
,&(Parms->add_par1)
,&(Parms->add_par2)
,&(Parms->preproc_time)
,&(Parms->process_time)
,&(Parms->postproc_time)
,&(Parms->total_time)) != ANZEL) {
  res = FALSE;
 }
else {
 c = cparms,
 for (i = 0; i < ANZEL ; i++ ) {
  while (*++cparms != '\n')
    ;
 }
 e = Parms->errmes;
 while ((*e++ = *++cparms) != '\n')
   ;
 *e = '\0';
}

}

/* ------ tx3tored ---------------- */
int tx3tored(stack,top,p_parms)
STACKELEM      *stack;
int            *top;
PTR_UEBERGABE  p_parms;
{
  char *b;
  int i;


  switch (p_parms->redcmd) {
    case RED_INIT:  /* Initialisierung  */
    case RED_RS:
    {
      p2tx3(p_parms);
      write_tree(cparms,APARMS);
      return(TRUE);
    }
    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {
      register int i;
      register STACKELEM  *lauf = stack;

      p2tx3(p_parms);
      write_tree(cparms,APARMS);

      IS_SPACE(S_a,(i = *top));
      for ( ; i > 0; i--)
      {
        PPUSHSTACK(S_a,(*lauf++)); /* Stackelemente auf den A-stack legen */
      }

      if (p_parms->redcmd == RED_NP)
      {
        restore_state(old_reduma_state);
        return; /* es fehlt noch etwas */
      }
      /* es soll also reduziert werden */

      /* so geht das, Mr. Hurck ! das folgende sollte nie der Fall sein */
      if (SIZEOFSTACK(S_a) == 0)
        post_mortem("reduce: length of expression is 0");

      /* Den Ausdruck richtig herum auf den E-Stack legen */
      for (i = SIZEOFSTACK(S_a); i > 0; i--)
      {
        PUSHSTACK(S_e,POPSTACK(S_a));
      }

      /* Reduktion kann beginnen */
      _redcnt = p_parms->redcnt;
      _beta_count_only = p_parms->beta_count;
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;
      /* externe Praezision in interne Praezision konvertieren */
      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);
      _trunc  = p_parms->truncation;

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;

      reduct();

      p_parms->redcnt = _redcnt;
      p_parms->redcmd = RED_NRP;
      p_parms->preproc_time = tpre;
      p_parms->process_time = tpro;
      p_parms->postproc_time = tpos;
      p_parms->total_time = tges;

      /* und gleich ausgeben */
    }
    case RED_NRP:   /* Ausgabe der naechsten Seite des reduzierten Ausdrucks */
    {
       register int           i  = 0;
       register int         size = SIZEOFSTACK(S_e);
       register STACKELEM  *lauf = stack;

       if (size == 0)   /* ein Fehlerfall der eigentlich nicht auftreten darf */
       {
         p_parms->redcmd = RED_LRP;
         *top = 0;
         restore_state(old_reduma_state);
         return;
       }
       while (i < p_parms->pg_size)
       {
         *lauf++ = POPSTACK(S_e);
         i++;
         if (i == size)   /* falls fertig */
         {
           p_parms->redcmd = RED_LRP;
           break;
         }
       }
       *top = i;
       restore_state(old_reduma_state);
       return;
    }
    default:
      post_mortem("Reduce: Unknown redcommand");
  }
}
/* end of       ctotx3.c */
