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
#include <nself.h>
#include <stdio.h>
#include <sys/types.h>
#include <sysn.h>
#include <ntime.h>
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
char post_mortem_msg[256];
jmp_buf post_mortem_buffer;

extern DStackDesc *ps_a;
extern DStackDesc D_S_A,D_S_H,D_S_I,D_S_R;
extern int _redcnt;

void nc_init();

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
 int dummy;
 
 whoami(&proc_id,&dummy,&host,&cube_dim);

#if D_MESS
  if (nsync(79))
    post_mortem ("NSYNC failed !");
  amicclk();
#ifndef M_OLD_MERGING
  m_init_merge();
#endif
#endif

 now = time(NULL);
 date = localtime(&now);
#if D_MESS
 sprintf(fname,"d:t:o,//d02/red/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
#else
 sprintf(fname,"d:t:o,//d02/red/tracefiles/tf.%02d%02d%02d%02d%02d.%02d",date->tm_mday,date->tm_mon, date->tm_year,date->tm_hour,date->tm_min,proc_id);
 /* fname[0]='\0';*/
#endif
 DBUG_PUSH(fname); /*  initialize Fred-Fish-DBUG in nCUBE */

 nc_init();

 setjmp(post_mortem_buffer);

 for(;;)
   {schedule();
    switch(inter())
      {case IR_EXT:
       case IR_DIST_END:
#if (nCUBE && D_MESS && D_MSCHED)
         (*d_m_process_terminate)(curr_pid);
#else
	 process_terminate(curr_pid);
#endif
	 break;
       case IR_WAIT:
#if (nCUBE && D_MESS && D_MSCHED)
         (*d_m_process_sleep)(curr_pid);
#else
	 process_sleep(curr_pid);
#endif
	 break;}
  if (sig_msg_flag)
    msg_check();}
}

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

 static struct sigaction act_rcv = {handle_signal,0,0},
                         act_oflw = {buffer_overflow,0,0};

#if D_MESS
#ifdef M_OLD_MERGING
 /* allocating Message-Buffer for merging ! */
  if ((d_mess_buff = malloc(D_MESS_MAXBUF)) == NULL)     /* later use ???? */
    post_mortem ("nc_init: cannot allocate measure-merging-buffer !"); 
#endif
#endif

 /* install signal handler for incoming messages */
 sigaction (SIGMSGS,&act_rcv,NULL);

 /* install signal handler for dropped messages */
 sigaction (SIGDROP,&act_oflw,NULL);

 /* set communication mode */
 naddcmode(COMM_MODE_SIGRETRY);

 init_scheduler(); 

#if NCDEBUG
  DBNCinit();
#endif

#if (nCUBE && D_MESS && D_MCOMMU)
 (*d_m_msg_send)(host,MT_NCUBE_READY);
#else
 msg_send(host,MT_NCUBE_READY);
#endif
}

/**************************************************************/
/*                                                            */
/* handle_signal     handles the SIGMSGS signal               */
/*                                                            */
/**************************************************************/

void handle_signal()
{sig_msg_flag = 1;}

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

{sprintf(post_mortem_msg,"Node %d: %s",proc_id,error);
 msg_send(host,MT_POST_MORTEM);
 msg_send(0xffff,MT_ABORT);
 process_terminate_all();
 flush_queue();
 longjmp(post_mortem_buffer,1);}
