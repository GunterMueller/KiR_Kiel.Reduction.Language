/* $Log: rncsupport.c,v $
 * Revision 1.15  1999/11/25 11:58:18  rs
 * more DEBUG output
 *
 * Revision 1.14  1998/09/30 08:30:41  rs
 * minor bugfixes in the speculative tickets version
 *
 * Revision 1.13  1998/09/24 13:12:10  rs
 * bugfixed version for SPEC_TICKETS + some more features
 *
 * Revision 1.12  1998/09/13 15:30:30  rs
 * first version supporting speculative evaluations with TICKETS
 *
 * Revision 1.11  1997/06/06 09:13:11  rs
 * some more (complete) modifications concerning speculative evaluation
 * (don't say you have missed my talk ? ;-)
 *
 * Revision 1.10  1997/02/17  13:18:12  rs
 * some additions supporting speculative evaluation
 *
 * Revision 1.9  1996/06/11  15:27:46  rs
 * some scheduling improvements
 *
 * Revision 1.8  1996/01/12  15:02:58  rs
 * set ADV_SCHED for the "advanced ticket scheduling", still under construction !
 *
 * Revision 1.7  1995/12/11  08:30:25  rs
 * tickets_file inserted...
 *
 * Revision 1.6  1995/10/20  13:22:31  rs
 * some ncube+pvm changes
 *
 * Revision 1.5  1995/10/17  15:33:43  rs
 * minor changes (pvm)
 *
 * Revision 1.4  1995/10/02  16:46:25  rs
 * some more pvm + measure modifications
 *
 * Revision 1.3  1995/08/10  14:27:32  rs
 * some more dbug output
 *
 * Revision 1.2  1995/07/12  15:22:30  rs
 * some interrupt handlers added
 *
 * Revision 1.1  1995/07/10  09:17:48  car
 * Initial revision
 *
 * Revision 1.7  1995/07/07  15:15:17  rs
 * additional pvm changes
 *
 * Revision 1.6  1995/06/30  12:32:57  rs
 * pvm changes (to be continued)
 *
 * */

/**************************************************************/
/*                                                            */
/* rncmain --- main program for the nCube                     */
/*						              */	
/* ?!?!?!						      */
/*                                                            */
/* ach 22/01/93                                               */
/*							      */
/* changed to rncsupport.c by car 1995/07/10                  */
/*                                                            */
/**************************************************************/

#include "rheapty.h"
#include "rncstack.h"
#include "rncsched.h"
#include "rstdinc.h"
#include "rextern.h"
#include "rncmessage.h"
#include <signal.h>
#include <stdlib.h>
#if (nCUBE || D_PVM_NCUBE)
#include <nself.h>
#include <sysn.h>
#include <ntime.h>
#endif
#if D_PVM
#include "pvm3.h"
#endif /* D_PVM */
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <setjmp.h>
#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#include "d_mess_merge.h"
#endif

/* external variables */
extern PROCESS_CONTEXT *curr_pid;
#if D_PVM
extern int pvmcoding;
extern int pvmspawn_ntask;
extern int pvm_tids[];
#endif /* D_PVM */
#if (ADV_SCHED || D_SPEC)
extern int sig_interrupt;
#endif /* ADV_SCHED */

/* global variables */
INSTR * code;
int proc_id;
int host;
int cube_dim;
int _prec_mult;
int _prec;
int _prec10;
BOOLEAN _formated;
BOOLEAN LazyLists;
BOOLEAN _digit_recycling;
BOOLEAN _beta_count_only;
BOOLEAN _trunc;
BOOLEAN _count_reductions;
int _maxnum;
int _base;
int hd;
int hs;
int ss;
int sig_msg_flag = 0;

#if (ADV_SCHED || D_SPEC)
int sig_suspend = 0;
#endif

#if ADV_SCHED
int tickets_cond = 0;
int have_received_results = 0;
int init_tickets_cond = 0;
int take_yellow=0;

#endif /* ADV_SCHED */

char post_mortem_msg[256];
char ticket_file[250];
jmp_buf post_mortem_buffer;

#if D_SPEC_TICKETS
char spec_ticket_file[250];
#endif

extern DStackDesc *ps_a;
extern DStackDesc D_S_A,D_S_H,D_S_I,D_S_R;
extern int _redcnt;

/**************************************************************/
/*                                                            */
/* nc_init:  initializes the nCube:                           */
/*           -initializes heap area                           */
/*           -initializes stack area                          */
/*                                                            */
/**************************************************************/

void nc_init(int heapsize,int stacksize)
{void handle_signal();
 void buffer_overflow();
 int source,type,flag;

 static struct sigaction act_rcv = {handle_signal,0,0},
                         act_oflw = {buffer_overflow,0,0};
#if D_PVM
 int _tmp;
 struct pvmtaskinfo *taskp;
 int ntask;
#endif /* D_PVM */

#if D_MESS
#ifdef M_OLD_MERGING
 /* allocating Message-Buffer for merging ! */
  if ((d_mess_buff = malloc(D_MESS_MAXBUF)) == NULL)     /* later use ???? */
    post_mortem ("nc_init: cannot allocate measure-merging-buffer !"); 
#endif
#endif

#if nCUBE
 /* nrange (FIRST_TYPE, LAST_TYPE); */

 /* install signal handler for incoming messages */

 n_repeat = 100;
 n_delay = 50;

 sigaction (SIGMSGS,&act_rcv,NULL);
#endif /* nCUBE */

#if (nCUBE || D_PVM_NCUBE)
 /* install signal handler for dropped messages */
 sigaction (SIGDROP,&act_oflw,NULL);

 /* set communication mode */
 naddcmode(COMM_MODE_SIGRETRY);
#endif /* nCUBE etc. */

#if nCUBE
  type=-1; source=-1;
  DBUG_PRINT("nc_init", ("Receiving ticket-file-name !"));
  nread(ticket_file, 250, &source, &type, &flag);
#if D_SPEC_TICKETS
  nread(spec_ticket_file, 250, &source, &type, &flag);
  DBUG_PRINT("nc_init", ("Receiving spec-ticket-file-name : %s", spec_ticket_file));
#endif
  
#endif

#if D_PVM
  /* initializing nodes and tids array */
  pvm_recv(-1,-1);
  pvm_upkint(&pvmcoding, 1, 1);
  pvm_upkint(&cube_dim, 1, 1);
  pvm_upkint(&pvmspawn_ntask, 1, 1);
  pvm_upkint(pvm_tids, pvmspawn_ntask, 1);
  pvm_upkstr(ticket_file);
  for (_tmp =0; _tmp<pvmspawn_ntask; _tmp++) /* identify the logical node number */
    if (proc_id==pvm_tids[_tmp]){proc_id=_tmp; break;}
  DBUG_PRINT ("PVM", ("altogether %d nodes, I'm node %d (pvm_id:t%x) !", pvmspawn_ntask, proc_id, pvm_mytid()));
#endif /* D_PVM */

 DBUG_PRINT("nc_init", ("filename for tickets-file: %s", ticket_file));

 init_scheduler(); 

#if NCDEBUG
  DBNCinit();
#endif

#if (D_SLAVE && D_MESS && D_MCOMMU)
 (*d_m_msg_send)(host,MT_NCUBE_READY);
#else
 msg_send(host,MT_NCUBE_READY);
#endif

#if (D_PVM && D_MESS && !D_PVM_NCUBE)
  /* not used for ncube pvm-version */
  DBUG_PRINT ("PVM", ("Slave-Id (again :-): %x", proc_id));
  pvm_dsec=0; /* initializing */
  pvm_dusec=0;
  gettimeofday (&pvm_time, NULL);
  DBUG_PRINT("PVM", ("pvm_time %ld %ld", pvm_time.tv_sec, pvm_time.tv_usec));
  /*if (proc_id != 0) {*/   /* syncing with node 0 */
    if (pvm_tasks( pvm_tids[0], &ntask, &taskp) != 0)
      post_mortem("pvm_tasks failure");
    if (pvm_hostsync(taskp[0].ti_host, &pvm_time_host, &pvm_time_delta)!=PvmOk) {
      DBUG_PRINT("PVM", ("Sync'en hat nicht geklappt... :-("));
      post_mortem("Couldn't synchronize with the host !"); }
    DBUG_PRINT("PVM", ("pvm_time_host %ld %ld", pvm_time_host.tv_sec, pvm_time_host.tv_usec));
    DBUG_PRINT("PVM", ("pvm_time_delta %ld %ld", pvm_time_delta.tv_sec, pvm_time_delta.tv_usec)); 
    pvm_dsec = -pvm_time_delta.tv_sec;
    pvm_dusec = (pvm_time_delta.tv_sec<0)?pvm_time_delta.tv_usec:-pvm_time_delta.tv_usec; /*}
  else {
    DBUG_PRINT("PVM", ("I'm node 0, no syncing !"));
    } */
  gettimeofday (&pvm_time, NULL);
  DBUG_PRINT("PVM", ("pvm_time %ld %ld", pvm_time.tv_sec, pvm_time.tv_usec));
  DBUG_PRINT("PVM", ("real pvm_time_delta %ld %ld", pvm_dsec, pvm_dusec));
#endif /* D_PVM */
}

/**************************************************************/
/*                                                            */
/* handle_signal     handles the SIGMSGS signal               */
/*                                                            */
/**************************************************************/

void handle_signal()
{
/* 
 int source=-1;
 int type=-1,tmp;

 sig_msg_flag = 1; 
 tmp = ntest(&source,&type);

 if (((type>0) && ((type<FIRST_TYPE)||(type>LAST_TYPE))) || (tmp<0)) {
   printf("Moin ! %d meldet, was falsch ! %d,%x,%x \n", proc_id, tmp,source,type);
   }

 DBUG_PRINT("handle_signal", ("ntest result: %d, src %x, type %x", tmp, source,type)); 
*/

 sig_msg_flag = 1;

/* printf("Moin ! %d meldet, ich bin in handle_signal !\n", proc_id); */

#if (ADV_SCHED || D_SPEC)
 sig_interrupt = 1;
#endif /* ADV_SCHED */
}

/**************************************************************/
/*                                                            */
/* buffer_overflow   handles the SIGDROP signal               */
/*                                                            */
/**************************************************************/

void buffer_overflow()
{post_mortem("Message buffer overflow");}

/**************************************************************/
/*                                                            */
/* post_mortem       handles error_messages                   */
/*                                                            */
/**************************************************************/

post_mortem(error)
char *error; 

{
 DBUG_ENTER ("post_mortem");

 DBUG_PRINT ("RNCSUPPORT", ("post mortem: %s", error));
 sprintf(post_mortem_msg,"Node %d: %s",proc_id,error);

/* fprintf(debugger, "0x%x: POST_MORTEM (message: \"%s\") !!!\n", curr_pid, error); fflush(debugger);    */

 msg_send(host,MT_POST_MORTEM);
#if !D_PVM
 msg_send(0xffff,MT_ABORT);
#endif /* D_PVM */
 process_terminate_all();
/* exit(0); */
 flush_queue();
 longjmp(post_mortem_buffer,1);

 DBUG_VOID_RETURN;
}

/**************************************************************/
/*                                                            */
/* abort_init       aborts initialization of nCube            */
/*                                                            */
/**************************************************************/

abort_init(error)
char *error;

{sprintf(post_mortem_msg,"nCube init failed: %s",error);
 msg_send(host,MT_NCUBE_FAILED);
 exit(0);}
