/*						ctotx3.c	*/

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <fcntl.h>
#include "cedit.h"
#include "cestack.h"
#include "cparm.h"

static char cmsg[80];

#define APARMS	sizeof(T_UEBERGABE)

static int wrt_fd,rd_fd;
#define WRT	"write.red"
#define RD	"read.red"

#define open_tree(p,d,m)	if ((wrt_fd = open(WRT,O_SYNC|O_CREAT|O_RDWR,0777)) == FAILURE) \
	{sprintf(cmsg,"can't open \"%s\"\n",WRT);error(cmsg);}\
	if ((rd_fd = open(RD,O_SYNC|O_CREAT|O_RDWR,0777)) == FAILURE) \
	{sprintf(cmsg,"can't open \"%s\"\n",RD);error(cmsg);}
#define close_tree()	close(wrt_fd); close(rd_fd)

#define write_tree(b,n)	write(wrt_fd,b,n)
#define read_tree(b,n)	read(rd_fd,b,n)

#define debug(x)	printf(x)

static char cparms[APARMS];

startreduma()
{
 open_tree("reduma",1,1);
}
stopreduma()
{
 close_tree();
}

/* parms to tx3: just put the parms package in a char buffer	*/
void p2tx3(Parms)
PTR_UEBERGABE Parms;
 {
  int i;

  for (i = 0 ; i < APARMS ; i++ ) {
    cparms[i] = ((char *)Parms)[i];
  }
  if ((i = write_tree(cparms,APARMS)) != APARMS) {
    sprintf(cmsg,"parms:can't write %d bytes to reduma\n",APARMS - i);
    error(cmsg);
  }
 }

/* tx3 to parms : just get the parms package from a char buffer	*/
int tx32p(Parms)
PTR_UEBERGABE Parms;
 {
  int i;

  while ((i = read_tree(cparms,APARMS)) == 0)
    sleep(1);
  if (i != APARMS) {
    sprintf(cmsg,"missing %d bytes from reduma\n",APARMS - i);
    error(cmsg);
  }
  for (i = 0 ; i < APARMS ; i++ ) {
    ((char *)Parms)[i] = cparms[i];
  }
 }

/* ------ tx3tored ---------------- */
int tx3tored(stack,top,p_parms)
STACKELEM      *stack;
int            *top;
PTR_UEBERGABE  p_parms;
{
  char *b;
  int i,j;

 p2tx3(p_parms);	/* parameter-packet an reduma	*/
  switch (p_parms->redcmd) {
    case RED_INIT:  /* Initialisierung  */
    case RED_RS:
    {
      tx32p(p_parms);	/* synchronisation		*/
      return(TRUE);
    }
    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {

      b = (char *)stack;
      i = *top;
      i *= sizeof(STACKELEM)/sizeof(char);
      if ((j = write_tree(b,i)) != i) {	/* seite uebergeben	*/
        sprintf(cmsg,"page:can't write %d bytes to reduma\n",i-j);
        error(cmsg);
      }
      if (p_parms->redcmd == RED_NP) {
        tx32p(p_parms);			/* synchronisation	*/
        return; /* es fehlt noch etwas */
      }
	/* warten auf resultat ?	*/
#ifdef UnBenutzt
      do {
        i = get_treerequest();
      } while (i != T_WRITEREQUEST);
#endif
    }
    case RED_NRP:   /* Ausgabe der naechsten Seite des reduzierten Ausdrucks */
    case RED_LRP:
    {
      tx32p(p_parms);			/* synchronisation	*/
      i = p_parms->pg_size;
      if (i > *top) {
        free((char *)stack);
        if ((stack = (STACKELEM *)calloc(i,sizeof(STACKELEM))) == (STACKELEM *)0) {
          sprintf(cmsg,"can't get %d stackelems\n",i);
          error(cmsg);
        }
/*
        sprintf(cmsg,"page:new pagesize (%d) bigger than old one (%d)\n",i,*top);
        error(cmsg);
*/
      }
      i *= sizeof(STACKELEM)/sizeof(char);
      b = (char *)stack;
      while ((j = read_tree(b,i)) == 0)		/* seite abholen	*/
        sleep(1);
      if (j != i) {
        sprintf(cmsg,"page:missing %d bytes from reduma\n",i - j);
        error(cmsg);
      }
      return(0);
    }
    default:
      break;
  }
}
/* end of       ctotx3.c */
