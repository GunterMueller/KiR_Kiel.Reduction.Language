/* $Log: rncmain.c,v $
 * Revision 1.30  1999/11/25 11:58:18  rs
 * more DEBUG output
 *
 * Revision 1.29  1998/09/30 08:30:41  rs
 * minor bugfixes in the speculative tickets version
 *
 * Revision 1.28  1998/09/24 13:12:10  rs
 * bugfixed version for SPEC_TICKETS + some more features
 *
 * Revision 1.27  1998/09/13 15:30:30  rs
 * first version supporting speculative evaluations with TICKETS
 *
 * Revision 1.26  1998/06/03 16:20:07  rs
 * speculative evaluations version 2 completed
 *
 * Revision 1.25  1998/03/31 09:54:42  rs
 * dbug-messages removed, ready for re-compilation
 *
 * Revision 1.24  1998/02/27 15:00:48  rs
 * further bugfixes on speculative evaluation (preliminary, beware of debug messages !)
 *
 * Revision 1.23  1998/02/18  13:34:36  rs
 * modifications for speculative evaluations regarding
 * meaning preserving transformations
 *
 * Revision 1.22  1997/11/24  08:19:48  rs
 * old version check-in
 *
 * Revision 1.21  1997/06/06  09:13:11  rs
 * some more (complete) modifications concerning speculative evaluation
 * (don't say you have missed my talk ? ;-)
 *
 * Revision 1.20  1997/04/14  09:52:06  rs
 * further work on speculative evaluation
 *
 * Revision 1.19  1997/02/17  13:18:12  rs
 * some additions supporting speculative evaluation
 *
 * Revision 1.18  1996/06/11  15:27:46  rs
 * some scheduling improvements
 *
 * Revision 1.17  1996/01/12  15:02:58  rs
 * set ADV_SCHED for the "advanced ticket scheduling", still under construction !
 *
 * Revision 1.16  1995/12/06  10:25:44  rs
 * some (final :-) changes for the nCUBE pvm version...
 *
 * Revision 1.15  1995/11/13  13:21:15  rs
 * some pvm + measure changes
 *
 * Revision 1.14  1995/10/20  13:22:31  rs
 * some ncube+pvm changes
 *
 * Revision 1.13  1995/10/02  16:46:25  rs
 * some more pvm + measure modifications
 *
 * Revision 1.12  1995/09/22  12:46:38  rs
 * additional pvm + measure changes
 *
 * Revision 1.11  1995/09/18  13:39:59  rs
 * pvm + measure changes
 *
 * Revision 1.10  1995/09/11  14:21:28  rs
 * some changes for the pvm measure version
 *
 * Revision 1.9  1995/07/12  15:23:16  rs
 * several pvm changes
 *
 * Revision 1.8  1995/07/10  09:15:38  car
 * main extracted to build nCUBE linkable modules
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
/*                                                            */
/* ach 22/01/93                                               */
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
#if nCUBE
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

#if D_SPEC
/* extern FILE *debugger;   */
extern int spec_sched_status;
#endif /* D_SPEC */

/* global variables */

void initialize_fred_fish();
int fred_fish_initialized=0;

extern INSTR * code;
extern int proc_id;
extern int host;
extern int cube_dim;
extern int _prec_mult;
extern int _prec;
extern int _prec10;
extern BOOLEAN _formated;
extern BOOLEAN LazyLists;
extern BOOLEAN _digit_recycling;
extern BOOLEAN _beta_count_only;
extern BOOLEAN _trunc;
extern BOOLEAN _count_reductions;
extern int _maxnum;
extern int _base;
extern int hd;
extern int hs;
extern int ss;
extern int sig_msg_flag;
extern char post_mortem_msg[256];
extern jmp_buf post_mortem_buffer;

extern DStackDesc *ps_a;
extern DStackDesc D_S_A,D_S_H,D_S_I,D_S_R;
extern int _redcnt;
extern int _interrupt;
extern _interruptbuf;

#if ADV_SCHED
extern int take_yellow;
#endif

void nc_init();
void restore();

/**************************************************************/
/*                                                            */
/* main:    the main program on the nCube:                    */
/*          -calls initialization                             */
/*          -calls the scheduler functions                    */
/*          -calls the interpreter                            */
/*                                                            */
/**************************************************************/

void main()
{
 char fname[30];
 int dummy,d2,d3,d4;

#if (TIME_SLICE || ADV_SCHED)
 PROCESS_CONTEXT *ts_pid;
#endif

#if nCUBE
 whoami(&proc_id,&dummy,&host,&cube_dim);
#endif /* nCUBE */

#if D_PVM
 host = pvm_parent();
 proc_id = pvm_mytid();
#endif /* D_PVM */

 initialize_fred_fish();  

#if D_MESS
#if nCUBE
  if (nsync(79))
    post_mortem ("NSYNC failed !"); 
  amicclk();
#endif
#ifndef M_OLD_MERGING
#if !D_PVM
  m_init_merge();
#endif
#endif
#endif

#if D_SPEC
/* sprintf(fname, "/tmp/spec_debugger_%d", proc_id);
 debugger = fopen(fname,"w");   */
 spec_sched_status = 0;
#endif /* D_SPEC */

#if D_PVM
 DBUG_PRINT ("PVM", ("Slave-Id: %x", proc_id));
#endif

#if D_PVM_NCUBE
    whoami(&d2,&dummy,&d3,&d4);
    DBUG_PRINT("PVM", ("physical node-id: %d", d2));
#endif

 nc_init();

#if !DEBUG
  signal( SIGSEGV , restore);       /* segmentation violation */
  signal( SIGBUS , restore);       /* bus error */
#endif /* DEBUG */

 setjmp(post_mortem_buffer);

 for(;;)
   {schedule();
    switch(inter())
      {case IR_EXT:
       case IR_DIST_END:
#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_terminate)(curr_pid);
#else
	 process_terminate(curr_pid);
#endif
	 break;
       case IR_WAIT:
#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_sleep)(curr_pid);
#else
	 process_sleep(curr_pid);
#endif
	 break;
#if TIME_SLICE
       case IR_TIME_OUT:
         ts_pid = curr_pid;
#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_sleep)(curr_pid);
#else
         process_sleep(curr_pid);
#endif
#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_wakeup)(ts_pid);
#else
         process_wakeup(ts_pid);
#endif 
         break;
#endif /* TIME_SLICE */

       case IR_SCHED:  /* just schedule another process */
         break;

#if ADV_SCHED
       case IR_SUSPEND:
         ts_pid = curr_pid;

         /* put the running process to sleep errrr... to bed ;-) */

         take_yellow = 1; /* flag which is used to force the next (old) yellow */
                          /* process to be selected after the distributing */

#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_sleep)(curr_pid);
#else
         process_sleep(curr_pid);
#endif

         /* ok, now enqueue the former running process in the yellow-ready-queue */

#if (D_SLAVE && D_MESS && D_MSCHED)
         (*d_m_process_wakeup)(ts_pid);
#else
         process_wakeup(ts_pid);
#endif

         break; /* now the next white process will be activated by schedule */

#endif /* ADV_SCHED */

     default: post_mortem("unknown inter-return code"); break;

}
  if (sig_msg_flag)
    msg_check();}
}

/**************************************************************************
 *  R E S T O R E
 *
 *  Behandlung von Interrupts durch longjump zur Sprungadresse in
 *  interruptbuf.
 *
 *  Parameter: die Nummer des Signals
 ***************************************************************************
 */
void /*int*/ restore(sig)     /* RS 04/11/92 */
int sig;
{
/*  if (sig == SIGINT) {
    post_mortem("reduction process interrupted");
    } */

  if (sig == SIGSEGV) {
    post_mortem("segmentation fault: please email the example program to base@informatik.uni-kiel.d400.de");
    }

  if (sig == SIGBUS) {
    post_mortem("bus error: please email the example program to base@informatik.uni-kiel.d400.de");
    }

  post_mortem ("Interrupt occured...(sorry, no more information available)");

  _interrupt = TRUE;
  longjmp(_interruptbuf,0);

  return/*(0)*/; /* TB & RS, 28.10.92 */    /* RS 04/11/92 */
}

void initialize_fred_fish()
{
time_t now;
struct tm *date;
char fname[80];

/* fprintf(debugger, "Versuche, FRED FISH zu starten...\n"); fflush(debugger); */

if (!fred_fish_initialized) {

/* fprintf(debugger, "Starte FRED FISH !\n"); fflush(debugger); */
 
 fred_fish_initialized = 1;

 now = time(NULL);
 date = localtime(&now);

/* #if D_MESS
 sprintf(fname,"d:t:o,//d02/red/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
#endif */
/* "d:t:o,/tmp/tf.%02d%02d%02d%02d */
#if (nCUBE && !D_PVM)
 sprintf(fname,"d:t:o,//d02/red/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
/* fname[0]='\0'; */
#endif /* nCUBE */
#if D_PVM
 sprintf(fname,"d:t:o,/tmp/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,pvm_mytid());
#endif /* D_PVM */

/* fprintf(debugger, "FRED FISH-String ist %s !\n", fname); fflush(debugger); */

 DBUG_PUSH(fname); /*  initialize Fred-Fish-DBUG in nCUBE */
 }
}
