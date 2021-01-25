/*						rtx3.c	*/
/* schnittstelle auf tx3-seite				*/

#include <stdio.h>
#include <fcntl.h>
#include "cparm.h"

#define FAILURE	(-1)

typedef long STACKELEM;

#define APARMS	sizeof(T_UEBERGABE)

static int wrt_fd,rd_fd;
#define RD	"write.red"
#define WRT	"read.red"

#define open_edit(p,d,m)	if ((wrt_fd = open(WRT,O_SYNC|O_CREAT|O_RDWR)) == FAILURE) \
	{fprintf(stderr,"can't open \"%s\"\n",WRT);exit(9); } \
	if ((rd_fd = open(RD,O_SYNC|O_CREAT|O_RDWR)) == FAILURE) \
	{fprintf(stderr,"can't open \"%s\"\n",RD);exit(9); }
#define close_edit()	close(wrt_fd); close(rd_fd)

#define write_edit(b,n)	write(wrt_fd,b,n)
#define read_edit(b,n)	read(rd_fd,b,n)

#define debug(x)	printf(x)

static char cparms[APARMS];

/* parms to tx3: just put the parms package in a char buffer	*/
void p2tx3(Parms)
PTR_UEBERGABE Parms;
 {
  int i;

debug("parms to editor\n");
  for (i = 0 ; i < APARMS ; i++ ) {
    cparms[i] = ((char *)Parms)[i];
  }
  if ((i = write_edit(cparms,APARMS)) != APARMS) {
    fprintf(stderr,"parms:can't write %d bytes to editor\n",APARMS-i);
    exit(9);
  }
 }

/* tx3 to parms : just get the parms package from a char buffer	*/
int tx32p(Parms)
PTR_UEBERGABE Parms;
 {
  int i;

debug("parms from editor\n");
  while ((i = read_edit(cparms,APARMS)) == 0)
    sleep(1);
  if (i != APARMS) {
    fprintf(stderr,"parms:missing %d bytes from editor\n",APARMS - i);
    exit(9);
  }
  for (i = 0 ; i < APARMS ; i++ ) {
    ((char *)Parms)[i] = cparms[i];
  }
 }

/* ------ fromedit ---------------- */
void fromedit()
{
  STACKELEM      *stack;
  T_UEBERGABE    parms;
  char *b;
  int i,j,ps;

  /* hole parameter packet vom editor	*/
  tx32p(&parms);
  /* dient zur synchronisation		*/

  stack = (STACKELEM *)0;
  j = 1;
  switch (parms.redcmd) {
    case RED_INIT:  /* Initialisierung  */
    case RED_RS:
      i = 0;
debug("reduce start");
      reduce(stack,&i,&parms);	/* initialisiere reduma	*/
      p2tx3(&parms);		/* gebe parameter packet zurueck (synchronisation)	*/
      break;
    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {
      ps = i = parms.pg_size;
      /* alloziere platz fuer die seite	*/
      if ((stack = (STACKELEM *)calloc(i,sizeof(STACKELEM))) == (STACKELEM *)0) {
        fprintf(stderr,"can't get %d stackelems\n",i);
        exit(9);
      }
      i *= sizeof(STACKELEM)/sizeof(char);
      b = (char *)stack;
debug("page from editor\n");
      while ((j = read_edit(b,i)) == 0) sleep(1);
      if (j != i) {
        fprintf(stderr,"RD_NP:missing %d bytes from editor\n",i - j);
        exit(9);
      }
debug("reduce page\n");
      reduce(stack,&ps,&parms);	/* uebergebe die seite an reduma	*/
      if (parms.redcmd == RED_NP) {			/* folgt noch eine seite ?	*/
        free(b);			/* gebe die seite wieder frei	*/
        p2tx3(&parms);		/* synchronisation mit editor		*/
        return;
      }
      j = 0;
    }
    /* FALL THROUGH !	*/
    case RED_NRP:   /* Ausgabe der naechsten Seite des reduzierten Ausdrucks */
    {
      if (j) {
        ps = i = parms.pg_size;
        /* alloziere platz fuer die seite	*/
        if ((stack = (STACKELEM *)calloc(i,sizeof(STACKELEM))) == (STACKELEM *)0) {
          fprintf(stderr,"can't get %d stackelems\n",i);
          exit(9);
        }
debug("reduce get page\n");
        reduce(stack,&ps,&parms);	/* hole seite von reduma	*/
      }
      parms.pg_size = ps;
      p2tx3(&parms);		/* synchronisation			*/
      b = (char *)stack;
      i = ps * sizeof(STACKELEM)/sizeof(char);
debug("page to editor\n");
      if ((j = write_edit(b,i)) != i) {
        fprintf(stderr,"can't write %d bytes to editor\n",i - j);
        exit(9);
      }
      free(b);			/* gebe die seite wieder frei	*/
      break;
    }
    case RED_LRP:   /* Ausgabe der naechsten Seite des reduzierten Ausdrucks */
    {
      p2tx3(&parms);		/* synchronisation			*/
      b = (char *)stack;
      i *= sizeof(STACKELEM)/sizeof(char);
debug("last page to editor\n");
      if ((j = write_edit(b,i)) != i) {
        fprintf(stderr,"can't write %d bytes to editor\n",i - j);
        exit(9);
      }
      free(b);			/* gebe die seite wieder frei	*/
    }
    default:
      break;
  }
  return;
}
main()
{
  /* oeffne die files (nur fuer die simulation !)	*/
  open_edit("reduma",1,1);
  /* forever !	*/
  while (1) {
    fromedit();
  }
  close_edit();
}
/* end of       rtx3.c */
