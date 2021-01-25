/* $Log: rncmain.c,v $
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

/* global variables */
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
{time_t now;
 struct tm *date;
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

 now = time(NULL);
 date = localtime(&now);

/* #if D_MESS
 sprintf(fname,"d:t:o,//d02/red/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
#endif */
#if (nCUBE && !D_PVM)
 sprintf(fname,"d:t:o,/tmp/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
/* fname[0]='\0'; */
#endif /* nCUBE */
#if D_PVM
 sprintf(fname,"d:t:o,/tmp/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,pvm_mytid());
#endif /* D_PVM */
 DBUG_PUSH(fname); /*  initialize Fred-Fish-DBUG in nCUBE */

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

