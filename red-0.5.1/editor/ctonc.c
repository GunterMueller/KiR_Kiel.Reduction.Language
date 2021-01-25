/*						ctonc.c	*/

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncube/nhost.h>
#include "cedit.h"
#include "cestack.h"
#include "cparm.h"

static char cmsg[80];

#define APARMS	sizeof(T_UEBERGABE)

static int wrt_fd,rd_fd;
static int mynull = 0;
#define WRT	"write.red"
#define RD	"read.red"

#define open_tree(p,d,m)	if ((wrt_fd = nopen(0)) < 0) \
	{sprintf(cmsg,"can't open \"%s\"\n",WRT);error(cmsg);}
#define close_tree()	if ((rd_fd = nclose(wrt_fd)) < 0) \
	{sprintf(cmsg,"can't close ncube %d\n",rd_fd);error(cmsg);}

write_tree(b,n)
 char *b;
 int n;
{
/* #ifndef sgi	*/
 i4n((int *)b,n/sizeof(int));
/* #endif	*/
 return(nwrite(wrt_fd,b,n,0,0));
}
  
read_tree(b,n)
 char *b;
 int n;
{
 int i;

 i = nread(wrt_fd,b,n,&mynull,&mynull);
/* #ifndef sgi	*/
 i4n((int *)b,n/sizeof(int));
/* #endif	*/
 return(i);
}

#define debug(x)	printf(x)

static char cparms[APARMS];
static npid_t npid;

#define NPROG "reduma"

static char *argv[] = {
 NPROG,
 (char *)0
};

startreduma()
{
 open_tree("reduma",1,1);
 npid = rexecv(nodeset(wrt_fd,"all"),NPROG,argv);
debug("ed: reduma gestartet");
}

/* parms to nc: just put the parms package in a char buffer	*/
void p2nc(Parms)
PTR_UEBERGABE Parms;
 {
  int i;

  for (i = 0 ; i < APARMS ; i++ ) {
    cparms[i] = ((char *)Parms)[i];
  }
  if ((i = write_tree(cparms,APARMS)) != SUCCESS) {
    sprintf(cmsg,"parms:can't write reduma (%d)\n",i);
    error(cmsg);
  }
 }

/* nc to parms : just get the parms package from a char buffer	*/
int nc2p(Parms)
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

stopreduma()
{
 extern T_UEBERGABE Parms;

 Parms.redcmd = RED_STOP;
debug("stop reduma\n");
 p2nc(&Parms);	/* synchronisation		*/
 close_tree();
 return(TRUE);
}

/* ------ nctored ---------------- */
int nctored(stack,top,p_parms)
STACKELEM      *stack;
int            *top;
PTR_UEBERGABE  p_parms;
{
  char *b;
  int i,j;

debug("ed: parms to reduma\n");
 p2nc(p_parms);	/* parameter-packet an reduma	*/
  switch (p_parms->redcmd) {
    case RED_INIT:  /* Initialisierung  */
    case RED_RS:
    {
debug("ed: init reduma\n");
      nc2p(p_parms);	/* synchronisation		*/
      return(TRUE);
    }
    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {
debug("ed: write to reduma\n");

      b = (char *)stack;
      i = *top;
      i *= sizeof(STACKELEM)/sizeof(char);
      if ((j = write_tree(b,i)) != SUCCESS) {	/* seite uebergeben	*/
        sprintf(cmsg,"page:can't write to reduma %d\n",j);
        error(cmsg);
      }
      if (p_parms->redcmd == RED_NP) {
        nc2p(p_parms);			/* synchronisation	*/
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
debug("ed: read from reduma\n");
      nc2p(p_parms);			/* synchronisation	*/
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
/* end of       ctonc.c */
