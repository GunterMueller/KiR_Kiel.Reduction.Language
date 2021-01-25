/* $Log: rncmessage.c,v $
 * Revision 1.62  1998/09/30 08:30:41  rs
 * minor bugfixes in the speculative tickets version
 *
 * Revision 1.61  1998/09/24 13:12:10  rs
 * bugfixed version for SPEC_TICKETS + some more features
 *
 * Revision 1.60  1998/09/13 15:30:30  rs
 * first version supporting speculative evaluations with TICKETS
 *
 * Revision 1.59  1998/06/03 16:20:07  rs
 * speculative evaluations version 2 completed
 *
 * Revision 1.58  1998/03/31 09:54:42  rs
 * dbug-messages removed, ready for re-compilation
 *
 * Revision 1.57  1998/02/27 15:00:48  rs
 * further bugfixes on speculative evaluation (preliminary, beware of debug messages !)
 *
 * Revision 1.56  1998/02/18  13:34:36  rs
 * modifications for speculative evaluations regarding
 * meaning preserving transformations
 *
 * Revision 1.55  1997/11/24  08:19:48  rs
 * old version check-in
 *
 * Revision 1.54  1997/06/06  09:13:11  rs
 * some more (complete) modifications concerning speculative evaluation
 * (don't say you have missed my talk ? ;-)
 *
 * Revision 1.53  1997/04/14  09:52:06  rs
 * further work on speculative evaluation
 *
 * Revision 1.52  1997/02/17  13:18:12  rs
 * some additions supporting speculative evaluation
 *
 * Revision 1.51  1996/06/11  15:27:46  rs
 * some scheduling improvements
 *
 * Revision 1.50  1996/03/22  10:33:20  rs
 * some problems concerning PM-instructions fixed
 *
 * Revision 1.49  1996/03/19  10:57:38  rs
 * support for C_PATTERN descriptors added
 *
 * Revision 1.48  1996/03/18  16:58:03  cr
 * support for C_INTACT
 *
 * Revision 1.47  1996/03/07  18:16:17  rs
 * C_FUNC TY_CASE ptc's might be used to indicate the number of the when-clause
 * (bugfix for the distributed version)
 *
 * Revision 1.46  1996/03/07  15:38:22  rs
 * another bugfix concerning the SUB-pattern-descriptor...
 *
 * Revision 1.45  1996/03/06  16:45:16  rs
 * sending/receiving CLAUSE and SELECTION descriptors
 * seems to work now
 *
 * Revision 1.44  1996/03/01  16:21:47  rs
 * still some work left ;-)
 *
 * Revision 1.43  1996/02/29  16:56:01  rs
 * still some work on sending MATCHING-descriptors left
 *
 * Revision 1.42  1996/02/28  15:49:44  rs
 * problems with TGUARD fixed...
 *
 * Revision 1.41  1996/02/28  15:17:02  rs
 * fixed label problems of the pattern matching instructions
 *
 * Revision 1.40  1996/02/28  13:49:25  cr
 * added support for C_FRAME (TY_FRAME/TY_SLOT)
 *
 * Revision 1.39  1996/02/12  13:04:08  rs
 * more DBUG output
 *
 * Revision 1.38  1996/01/25  16:18:22  rs
 * some changes for ADV_SCHED (-uling)
 *
 * Revision 1.37  1996/01/12  15:02:58  rs
 * set ADV_SCHED for the "advanced ticket scheduling", still under construction !
 *
 * Revision 1.36  1995/12/07  16:48:50  rs
 * some pvm + ncube + measurement changes
 *
 * Revision 1.35  1995/12/06  10:26:17  rs
 * some (final :-) changes for the nCUBE pvm version...
 *
 * Revision 1.34  1995/10/20  13:22:31  rs
 * some ncube+pvm changes
 *
 * Revision 1.33  1995/10/17  15:33:43  rs
 * minor changes (pvm)
 *
 * Revision 1.32  1995/09/22  12:46:47  rs
 * additional pvm + measure changes
 *
 * Revision 1.31  1995/09/19  14:43:30  rs
 * changes for the pvm + measuer version
 *
 * Revision 1.30  1995/09/18  13:39:40  rs
 * more DBUG output
 *
 * Revision 1.29  1995/09/12  12:26:16  rs
 * pvm + measure fixes
 *
 * Revision 1.28  1995/09/11  14:21:22  rs
 * some changes for the pvm measure version
 *
 * Revision 1.27  1995/09/01  14:31:33  rs
 * bugfix for the tilde-versions: send args+nfv in closures...
 *
 * Revision 1.26  1995/08/08  14:37:47  rs
 * bug fix in sending doubles
 * (thanks to mr. held there was no doku on this...)
 *
 * Revision 1.25  1995/07/13  13:13:55  rs
 * pvm bug fixes
 *
 * Revision 1.24  1995/07/12  15:22:57  rs
 * pvm changes
 *
 * Revision 1.23  1995/07/10  14:10:11  rs
 * some minor pvm changes...
 *
 * Revision 1.22  1995/07/07  15:15:25  rs
 * additional pvm changes
 *
 * Revision 1.21  1995/07/05  14:21:09  rs
 * additional pvm changes
 *
 * Revision 1.20  1995/06/30  12:33:16  rs
 * pvm changes (to be continued)
 *
 * Revision 1.19  1995/05/22  09:55:52  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.18  1995/05/17  12:58:10  rs
 * another test check-in (IGNORE)
 *
 * */

/***************************************************************/
/*                                                             */
/* rncmessage.c --- message routines for the nCube             */
/*                                                             */
/* ach 26/01/93                                                */
/*                                                             */
/***************************************************************/

#ifndef D_MESS_ONLY  
#define RNCMESSAGE Message-System!
#endif
#ifdef D_NN_SCHED
extern void nn_msg_send();
extern void nn_msg_receive();
extern int proc_org;


#include "rheapty.h"


#endif

#include "rheapty.h"
#include "rncheap.h"
#include "rncstack.h"
#include "rstdinc.h"
#include "rncmessage.h"
#include "rncsched.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinter.h"
#include <stdio.h>
#if nCUBE
#include <nself.h>
#include <n_errno.h>
#endif /* nCUBE */
#include "dbug.h"
#include <malloc.h>
#include <string.h>
#include <setjmp.h>

#if D_MESS
#include <sys/stat.h>
#include <sys/signal.h>
#include "d_mess_io.h"
extern PROCESS_CONTEXT* curr_pid;
extern int newheap();
extern void freeheap();
extern T_DESCRIPTOR *newdesc();
extern void freedesc();
extern int compheap();
extern void allocate_d_stack_segment();
extern void free_d_stack_segment();
extern INSTR *distribute();
extern PROCESS_CONTEXT *process_create();
extern void process_terminate();
extern void process_sleep();
extern void process_wakeup();
extern void process_run();

extern int proc_tab_size;
#endif

#if D_PVM
#include "pvm3.h"
extern int pvmspawn_flag;
extern char *pvmspawn_where;
int pvmspawn_ntask;
int pvm_tids[128];
int pvmcoding; /* should be initialized in first communication... */
#endif /* D_PVM */

extern int d_sort_in;

/* extern void initialize_fred_fish();*/

#if D_SPEC
/* int m_curr_pid = 0; */  /******* DELETE ! for debugging purposes only *********/
extern int real_start_redcount;
extern int spec_father_slot;
#include "spec_sched.h"
extern int spec_sched_status;
extern FILE *debugger;
extern void abdul();
extern int rout_specmap2[];
#ifndef D_MESS_ONLY
int spec_message_size = MESSAGE_SIZE_OLD;
#endif
#endif

#if D_SPEC
#define MESSAGE_SIZE spec_message_size
#else  
#define MESSAGE_SIZE 1024
#endif 

#if D_MESS
#define D_MESS_DISABLED(text) { sprintf(m_super_name, "sorry, %s measurements are disabled", text); \
                              post_mortem(m_super_name); }
#endif

#if nCUBE
#ifndef D_MESS_ONLY      /* CHANGES IN DO_SEND, PLEASE ALSO IN D_MESS_ONLY CASE !!!!!! */

#if MSG_CHKSUM
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
			 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
			 buffer_end=buffer + MESSAGE_SIZE):0, \
			 _tmp = (int)(data), \
			 chksum += _tmp, \
			 *buffer_p++ = _tmp) 
#else
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
			 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
			 buffer_end=buffer + MESSAGE_SIZE):0, \
			 *buffer_p++ = (int)(data)) 
#endif
#else    /* DON'T FORGET CHANGES HERE !!!! */

#define DO_SEND(data)  ((buffer_p == buffer_end)? \
                        (result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
                         m_size += MESSAGE_SIZE*sizeof(int), \
                         buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
                         buffer_end=buffer + MESSAGE_SIZE):0, \
			*buffer_p++ = (int)(data))

#endif /* D_MESS_ONLY */
#endif /* nCUBE */

#if D_PVM
#define DO_SEND(data)  (_tmp=data, (pvm_counter > MESSAGE_SIZE)? \
                        (pvm_send(pvm_dest, type), \
                         pvm_counter = 0, \
                         pvm_initsend(pvmcoding)):0,\
                         pvm_pkint(&_tmp,1,1),\
                         pvm_counter++)
#endif /* D_PVM */

#if nCUBE
#ifndef D_MESS_ONLY  /* CHANGES IN DO_RECEIVE, PLEASE ALSO IN D_MESS_ONLY CASE !!!!!! */

#if MSG_CHKSUM
#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nrelp((char *)buffer), \
			   result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags), \
			   ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
			   buffer_end = buffer_p + result/sizeof(int)):0, \
			   chksum += *buffer_p, \
			  *buffer_p++) 
#else
#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nrelp((char *)buffer), \
			   result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags), \
			   ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
			   buffer_end = buffer_p + result/sizeof(int)):0, \
			  *buffer_p++) 
#endif
#else     /* DON'T FORGET CHANGES HERE !!!! */

#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
                          (nrelp((char *)buffer), \
                           m_size += (result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags)), \
                           ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
                           buffer_end = buffer_p + result/sizeof(int)):0, *buffer_p++)

#endif /* D_MESS_ONLY */
#endif /* nCUBE */

#if D_PVM
#define DO_RECEIVE()     ((pvm_counter > MESSAGE_SIZE)? \
                           (pvm_recv(pvm_src,type), \
                           pvm_counter = 0):0, \
                           pvm_upkint (&_tmp, 1, 1), \
                           pvm_counter++,_tmp)
#endif /* D_PVM */

#define SEND_2_SHORTS(data1,data2)  DO_SEND(((int)(data1) << 16) | (int)(data2))

#define RECEIVE_2_SHORTS(data1,data2) do {int tmp_data; \
					  tmp_data = DO_RECEIVE(); \
					  data1 = tmp_data >> 16; \
					  data2 = tmp_data & 0xffff;} while(0)

typedef struct sent_list_node {PTR_DESCRIPTOR ptr;
			       int original_entry;
			       DESC_CLASS original_class;
			       struct sent_list_node * next;} SENT_LIST_NODE;

extern unsigned int dist_stack_elems;        /* rncsched.c */
extern int dist_stack_free;          /* rncsched.c */
#if WITHTILDE
extern unsigned int dist_tilde_elems;        /* rncsched.c */
extern int dist_tilde_free;          /* rncsched.c */
#endif /* WITHTILDE */
extern DIST_TYPE dist_type;          /* rncsched.c */
extern INSTR * dist_instr;           /* rncsched.c */
extern DStackDesc D_S_H;             /* rncstack.c */
extern DStackDesc D_S_A;             /* rncstack.c */
extern DStackDesc D_S_R;             /* rncstack.c */
extern DStackDesc D_S_I;             /* rncstack.c */
extern DStackDesc D_S_hilf;          /* rncstack.c */
extern DStackDesc *ps_a;             /* rncinter.c */
extern DStackDesc *ps_w;             /* rncinter.c */
#if WITHTILDE
extern DStackDesc *ps_t;
extern DStackDesc *ps_r;
#endif /* WITHTILDE */
extern int proc_id;                  /* rncmain.c */
extern PROCESS_CONTEXT * curr_pid;   /* rncsched.c */
extern int ticket_pool[];
extern INSTR_DESCR instr_tab[];      /* rncinter.h */
extern int _redcnt;
extern INSTR* heap_code_start;       /* rncheap.c */
extern int _prec_mult;
extern int _prec;
extern int _prec10;
extern BOOLEAN _formated;
extern BOOLEAN LazyLists;
extern BOOLEAN _digit_recycling;
extern BOOLEAN _beta_count_only;
extern BOOLEAN _trunc;
extern BOOLEAN _count_reductions;
extern BOOLEAN _heap_reserved;
extern int _maxnum;
extern int _base;
extern int hd;
extern int hs;
extern int ss;
extern int stack_seg_size;
extern int sig_msg_flag;
extern int static_heap_upper_border;
extern int static_heap_lower_border;
extern int highdesc;
extern int stdesc_border;
extern int lookup_next;
extern int ticket_pool_counter;
extern int cube_dim;
extern char post_mortem_msg[256];
extern jmp_buf post_mortem_buffer;
extern int first_lookup;
#if ADV_SCHED
extern int tickets_cond, have_received_results, init_tickets_cond;
extern PROCESS_CONTEXT * ready_queue_first, *ready_queue_last;
#if D_MESS
extern int no_suspended;
#endif /* D_MESS */
#endif

extern PROCESS_CONTEXT *process_create();
extern void process_wakeup();

void flush_queue();

#if (ADV_SCHED || D_SPEC)
extern int sig_suspend;
#if D_MESS_ONLY
extern int sig_interrupt;
#else
int sig_interrupt=0;        /* checks whether the interpreter must be interrupted */
#endif /* D_MESS_ONLY */
#endif /* ADV_SCHED */

#if D_MESS
extern int host;
extern void handle_signal();
void msg_send();
void msg_receive();
#endif

#ifndef D_MESS_ONLY

int _tmp;

#else

extern int _tmp;

#endif

/***************************************************************/
/*                                                             */
/* msg_send()     sends a message                              */
/*                                                             */
/* arguments: dest   the number of the destination processor   */
/*                                                             */
/*            type   the type of the message                   */
/*                                                             */
/***************************************************************/

#ifdef D_MESS_ONLY

void m_msg_send(int dest, MESSAGE_TYPE type)

#else

void msg_send(int dest, MESSAGE_TYPE type)

#endif

{int stack_index=0;               /* number of sent stackelements  */
 int stack_elems;                 /* number of elements to be sent */
 int flags;                       /* for nwritep                   */
 int sent_index=0;                /* counter for sent descriptors  */
 int heap_elems,heap_counter;
 int code_sent=0;
 int param_counter;
 int result;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 INSTR_DESCR send_instr;
 DESC_CLASS d_class;              /* class of descriptor           */
 DESC_TYPE d_type;                /* type of descriptor            */
 STACKELEM send_data,result_stelem;
 PTR_HEAPELEM heap_ptr,code_address;
 int *buffer,*buffer_p,*buffer_end;
#if D_PVM
 int pvm_counter=0;
 int pvm_dest = dest;
#endif /* D_PVM */

#if D_SPEC
 int send_spec;
#endif

 SENT_LIST_NODE *sent_list = NULL,*new_node;

 int dummy,*dummy2,*dummy3; /* Debugging */

#ifdef D_MESS_ONLY
 int m_backup_pid;
 int m_size = 0;
#endif
 int m_pid = -1;
 char *m_type;
/* #endif*/

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_msg_send");
#else
 DBUG_ENTER("msg_send");
#endif

/* fprintf(debugger, "* ** entering msg_send, curr_pid: %x, sending type %d to node %d !\n", curr_pid, type, dest); fflush(debugger); */

#ifndef DBUG_OFF
  if (proc_id == dest)
    post_mortem("sending message to the same proc, shitty !");
#endif /* DBUG_OFF */

/*   if ((proc_id != 0) && (dest == host)) {
    post_mortem("Some node tried to send a message to the host...");
    } */
  

#if D_PVM
 if (pvm_dest != host) {
   DBUG_PRINT ("PVM", ("Sending to Node %d...", dest));
   pvm_dest = pvm_tids[dest];
   DBUG_PRINT ("PVM", ("this means tid t%x", pvm_dest));
   }
 else
   DBUG_PRINT ("PVM", ("Sending to the Host t%x", pvm_dest));
#endif /* D_PVM */

#ifdef D_MESS_ONLY
  m_backup_pid = m_curr_pid;
#endif

 if ((type == MT_NCUBE_READY) || (type == MT_ABORT))
   {

/*   fprintf(debugger, "0x%x: sending MT_NCUBE_READY, MT_ABORT to 0x%x\n", curr_pid, dest); fflush(debugger);     */

DBUG_PRINT ("PVM", ("MT_NCUBE_READY || MT_ABORT processing..."));

#ifdef D_MESS_ONLY

#if (D_SLAVE && D_MESS && D_MCOMMU)
    if (m_msg_send_begin)
      if (type == MT_NCUBE_READY)
        MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_NCUBE_READY", dest, -1);
      else
        MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_ABORT", dest, -1);
#endif
#endif /* D_MESS_ONLY */

#if D_PVM
    pvm_initsend(pvmcoding);
    pvm_send(pvm_dest,type);
#endif /* D_PVM */

#if nCUBE
    nwrite(NULL,0,dest,type,&flags);
#endif /* nCUBE */

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
    if (m_msg_send_end)
      if (type == MT_NCUBE_READY)
        MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, 0);
      else
        MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, 0);
#endif
#endif

    DBUG_VOID_RETURN;}

 if (type == MT_POST_MORTEM)
   {

/* fprintf(debugger, "0x%x: sending MT_POST_MORTEM to 0x%x\n", curr_pid, dest); fflush(debugger);     */

DBUG_PRINT ("PVM", ("MT_POST_MORTEM processing..."));

#if nCUBE
    nwrite(post_mortem_msg,strlen(post_mortem_msg)+1,dest,type,&flags);
#endif /* NCUBE */
#if D_PVM
    pvm_initsend(pvmcoding);
    pvm_pkstr(post_mortem_msg);
    pvm_send(dest,type);
#endif /* D_PVM */
    DBUG_VOID_RETURN;}

 if (type == MT_NCUBE_FAILED)
   {

/*  fprintf(debugger, "0x%x: sending MT_NCUBE_FAILED to 0x%x\n", curr_pid, dest); fflush(debugger);      */

DBUG_PRINT ("PVM", ("MT_NCUBE_FAILED processing..."));

#if nCUBE
     nwrite(post_mortem_msg,strlen(post_mortem_msg)+1,dest,type,&flags);
#endif /* nCUBE */
#if D_PVM
    pvm_initsend(pvmcoding);
    pvm_pkstr(post_mortem_msg);
    pvm_send(dest,type);
#endif /* D_PVM */
    DBUG_VOID_RETURN;}


 /* prepare the message buffer */
#if nCUBE
 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int));

/* fprintf(debugger, "********** buffer is now: %x, message size is %d\n", buffer, MESSAGE_SIZE); fflush(debugger);  */

#endif /* nCUBE */

#if D_PVM
   pvm_counter = 0;
   pvm_initsend(pvmcoding);
#endif /* D_PVM */

 buffer_end=buffer + MESSAGE_SIZE;
 switch (type)
   {
     case MT_CONTROL:

/*  fprintf(debugger, "sending MT_CONTROL\n"); fflush(debugger);   */

#if D_SPEC
       /* send CONTROL package */

       DBUG_PRINT ("MSG", ("send CONTROL message"));

#ifdef D_MESS_ONLY    /* *************************** correct later on !*/
      m_type = "MT_CONTROL"; /* this has to be interpreted, sorry */
      if (T_SPECMSG_GOON)
        m_type = "MT_CONTROL_GOON";
      else if (T_SPECMSG_BARR)
        m_type = "MT_CONTROL_BARR";
      else if (T_SPECMSG_FAIL)
        m_type = "MT_CONTROL_FAIL"; 
      else if (T_SPECMSG_HEREAMI)
        m_type = "MT_CONTROL_HERE";
      else if (T_SPECMSG_ACK)
        m_type = "MT_CONTROL_ACK";
      else if (T_SPECMSG_START)
        m_type = "MT_CONTROL_START";
      else if (T_SPECMSG_TERM)
        m_type = "MT_CONTROL_TERM";
      else if (T_SPECMSG_WIN)
        m_type = "MT_CONTROL_WIN";
      else if (T_SPECMSG_VITAL)
        m_type = "MT_CONTROL_VITAL";
      else post_mortem ("unknown CONTROL message type...");

      m_pid = spec_control[1];
      if (T_SPECMSG_START)
        m_curr_pid = spec_control[4];
      else
        m_curr_pid = spec_control[2];

/* fprintf(debugger, "0x%x: sending %s message to (0x%x,0x%x) from 0x%x\n",curr_pid, m_type, spec_control[1], dest, m_curr_pid); fflush(debugger);    */

/* #ifdef D_MESS_ONLY */  /*************** DELETE */

#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin)
        MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif

/* fprintf(debugger, "sending %s to %d,%x\n", m_type, dest, m_pid); fflush(debugger);    */

#endif

       DO_SEND(spec_control[0]);
       DO_SEND(spec_control[1]);
       DO_SEND(spec_control[2]);
       DO_SEND(spec_control[3]);
       DO_SEND(spec_control[4]);
       DO_SEND(spec_control[5]);
       DO_SEND(spec_control[6]);
       DO_SEND(spec_control[7]);
       DO_SEND(spec_control[8]);
       DO_SEND(spec_control[9]);

       if (T_SPECMSG_ACK) {
       /* send the data about the speculative sons to the control process */
       DBUG_PRINT("MSG", ("SPECMSG_ACK processing"));

#if D_SPEC_TICKETS
       if (spec_control[6] == 1) {
         DBUG_PRINT("MSG", ("post_proc processing..."));
         DO_SEND(SPEC_SF_NUMB_T(*(PTR_DESCRIPTOR)spec_control[7]));
         DO_SEND(SPEC_SF_NUMB_A(*(PTR_DESCRIPTOR)spec_control[7]));
         } else
#endif
       for (spec_control[4]=0; spec_control[4]<spec_control[3]; spec_control[4]++) {
         /* first the PID */
         DO_SEND(((PROCESS_CONTEXT *)spec_control[2])->processes[(spec_control[4]*2)]);
         DO_SEND(((PROCESS_CONTEXT *)spec_control[2])->processes[(spec_control[4]*2)+1]);
         DBUG_PRINT("MSG", ("it's pid %x on node %d !", ((PROCESS_CONTEXT *)spec_control[2])->processes[(spec_control[4]*2)], ((PROCESS_CONTEXT *)spec_control[2])->processes[(spec_control[4]*2)+1]));
         }
     }

#else
       post_mortem ("no CONTROL messages in non SPEC version!!!");
#endif /* D_SPEC */

       break;

     case MT_NEW_PROCESS:

/*   fprintf(debugger, "0x%x: sending MT_NEW_PROCESS to 0x%x\n", curr_pid, dest); fflush(debugger);       */

DBUG_PRINT ("PVM", ("MT_NEW_PROCESS processing..."));
      
#ifdef D_MESS_ONLY

/*      fprintf(debugger, "m_curr_pid: %x, curr_pid: %x\n", m_curr_pid, curr_pid); fflush(debugger); */

      m_type = "MT_NEW_PROCESS";

#if D_SPEC_TICKETS
#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin && (spec_post_proc != 0))
        PROCESS_PID = SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_frame);
#endif
#endif

#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin)
	MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif

      /* send the header */
#if D_SPEC_TICKETS
      if (spec_post_proc != 0) {
        DO_SEND(SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_frame)); /* sending pid of the father */
        } else
#endif
      DO_SEND((int)curr_pid);                                 /* the pid of the sending process */

#if (!D_SPEC)
      DO_SEND((int)(dist_instr-GET_CODE_START()));            /* the pc of the subprocess */
#else
      if (dist_instr == 0) {
        DBUG_PRINT("MSG", ("dist_instr was 0, process spawned by specmap !"));

/* fprintf(debugger, "%x: sending CODE %x\n", curr_pid, -1); fflush(debugger); */

        DO_SEND((int)(-1));
        } else {
        DBUG_PRINT("MSG", ("dist_instr was != 0, okay !"));
        DBUG_PRINT("MSG", ("dist_instr: %x, GET_CODE_START: %x", dist_instr, GET_CODE_START()));
        DO_SEND((int)(dist_instr-GET_CODE_START())); 

/* fprintf(debugger, "%x: sending CODE %x\n", curr_pid, dist_instr-GET_CODE_START()); fflush(debugger); */

        }
#endif
#if D_SPEC
#if D_SPEC_TICKETS
      if ((spec_post_proc != 0) || ((int)curr_pid->specmask1 > 0))  {
#else
      if ((int)curr_pid->specmask1 > 0) {
#endif

#if D_SPEC_TICKETS
        DO_SEND(spec_frame);
        DO_SEND(spec_post_proc);
        DBUG_PRINT("MSG", ("sending spec_frame %x, and a flag as a_stack_addr %d", spec_frame, spec_post_proc));
#else
        DO_SEND(spec_father_slot);
        DO_SEND((int)&(curr_pid->son_results[spec_father_slot]));
        DBUG_PRINT("MSG", ("store result in slot %d (%x)", spec_father_slot, &(curr_pid->son_results[spec_father_slot])));
#endif /* D_SPEC_TICKETS */
        }
      else {
      DO_SEND(0);
#endif /* D_SPEC */
      DO_SEND((int)D_TOP_ADDRESS(*ps_w));                     /* where to put the result */
#if D_SPEC
      }
#endif

#if WITHTILDE
      DBUG_PRINT("MSG", ("h/r-stack address %x", ((int)D_TOP_ADDRESS(*ps_r))));

#if D_SPEC_TICKETS
      if (spec_post_proc != 0) {
        DBUG_PRINT("MSG", ("post_proc, send real counter addr"));
        DO_SEND(SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_frame));
        } else
#endif
      DO_SEND((int)D_TOP_ADDRESS(*ps_r));                     /* where to find the counter */
#else

#if D_SPEC_TICKETS
      post_mortem("unsupported");
#endif

      DO_SEND((int)D_TOP_ADDRESS(D_S_H));                     /* where to find the counter */
#endif
#if D_SPEC_TICKETS
      if (spec_post_proc != 0) {
        DO_SEND(SPEC_SF_REDSTEPS(*(PTR_DESCRIPTOR)spec_frame));
        DBUG_PRINT("MSG", ("sending post_proc redsteps: %d", SPEC_SF_REDSTEPS(*(PTR_DESCRIPTOR)spec_frame)));
        } else
#endif
      DO_SEND((_redcnt)?_redcnt-1:0);                         /* how many reductions the process may use */

#if D_SPEC /* sende Maske fuer spekulativen Prozess und andere data... */
#if D_SPEC_TICKETS
      if (spec_post_proc != 0) {
      DBUG_PRINT("MSG", ("sending 1 as flag !"));
      DO_SEND(1); /* sending 1 as a flag for a speculative process */
      } else {
#endif
      DO_SEND((int)curr_pid->specmask1);
      DBUG_PRINT("MSG", ("sending specmask (%x)", curr_pid->specmask1));
#if D_SPEC_TICKETS
      }
#endif

#if D_SPEC_TICKETS
      if (spec_post_proc != 0) {
        DO_SEND((int)((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_frame))->control_pid); 
        DO_SEND((int)((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_frame))->control_node);
        DO_SEND((int)SPEC_SF_NESTING(*(PTR_DESCRIPTOR)spec_frame));
        } else
#endif
      if ((int)curr_pid->specmask1 > 0) {
        DO_SEND((int)curr_pid->control_pid);
        DO_SEND((int)curr_pid->control_node); 
#if D_SPEC_TICKETS
        DO_SEND((int)curr_pid->nesting);
#endif
        }
#endif /* D_SPEC */

#if ADV_SCHED
      DO_SEND(curr_pid->letpar_level); /* sending letpar_level */
      DBUG_PRINT ("ADV_SCHED", ("sending letpar_level: %i", curr_pid->letpar_level));
#endif /* ADV_SCHED */

      stack_elems = dist_stack_elems;
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);     /* push marker on Hilfstack */     
      if (dist_type == DIST_N)              /* send full stack frame? */
#if (D_SPEC && D_SPEC_TICKETS)
        if ((spec_post_proc != 0) || ((int)curr_pid->specmask1 > 0)) { /* it's a speculative process creation */
          stack_index=0;
          DBUG_PRINT("MSG", ("special frame communication for speculative processes (A-stack)"));

          for (;stack_index<stack_elems;stack_index++)
            D_PUSHSTACK(D_S_hilf,(*spec_frame).u.li.ptdv[SPEC_SF_DATA+stack_index+SPEC_SF_NUMB_T(*spec_frame)]);
          } else
        for (;stack_index<stack_elems;stack_index++)                      /* COPIED from down below ;-) */
          D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_a,stack_index));
#else
	for (;stack_index<stack_elems;stack_index++)
	  D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_a,stack_index));
#endif
      else                                  /* send only needed args */
	{while (stack_elems>0)
	   {if (stack_elems & 1)
	      D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_a,stack_index));
	   else
	     D_PUSHSTACK(D_S_hilf,ST_DUMMY);
	    stack_elems >>= 1;
	    stack_index++;}
	 for (;stack_index<dist_stack_free;stack_index++)
	   D_PUSHSTACK(D_S_hilf,ST_DUMMY);}
      
      DO_SEND(stack_index);

      DBUG_PRINT("MSG", ("%d A-stack elements sent...", stack_index));

#if WITHTILDE
      stack_index = 0;
      stack_elems = dist_tilde_elems;
#if (D_SPEC && D_SPEC_TICKETS)
      if ((spec_post_proc != 0) || ((int)curr_pid->specmask1 > 0)) { /* it's a speculative process creation */
        DBUG_PRINT("MSG", ("special frame communication for speculative processes (T-stack)"));

        for (;stack_index<stack_elems;stack_index++)
          D_PUSHSTACK(D_S_hilf,(*spec_frame).u.li.ptdv[SPEC_SF_DATA+stack_index]);
        } else
      for (;stack_index<stack_elems;stack_index++)               /* just COPIED from down below ! */
        D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_t,stack_index));
#else
      for (;stack_index<stack_elems;stack_index++)
	D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_t,stack_index));
#endif
      
      DO_SEND(stack_index);

      DBUG_PRINT("MSG", ("%d T-stack elements sent...", stack_index));

#endif /* WITHTILDE */
      
send_loop:

      while((send_data = D_POPSTACK(D_S_hilf)) != ST_END_MARKER)
	{if ((!T_POINTER(send_data) || !isdesc(send_data)) && send_data) {
           DBUG_PRINT ("MSG", ("no pointer, VAL_INT= %d, value=%x", VAL_INT(send_data), send_data));

/* fprintf(debugger, "sending NO pointer...\n"); fflush(debugger); */

	   DO_SEND(send_data); }
	else
	  {if (!send_data)                                  /* NULL pointer? */
	     {DBUG_PRINT ("MSG", ("null pointer"));
              DO_SEND(0);
	      DO_SEND(1);}
	  else
	    {if (IS_STATIC(send_data))                       /* pointer into static heap? */
	       {DBUG_PRINT("STATHEAP",("sending %08x as static heap",send_data));
		DO_SEND(0);
		DO_SEND(((int)STATIC_OFFSET(send_data) << 4) | 0x0a);}     /* ...1010 as tag for offset in static heap */
	    else
	      {DBUG_PRINT("DYNHEAP",("sending %08x (refcount %d) as dynamic heap",send_data, ((T_PTD)send_data)->u.sc.ref_count));
               if (R_DESC(*(T_PTD)send_data,class) == C_SENT)   /* descriptor already sent? */
		 {DBUG_PRINT("DYNHEAP",("already sent..."));
                  DO_SEND(0);
		  DO_SEND(((*((int *)send_data + 1)) << 4) | 0x0e);}
	      else
		{DO_SEND(0);
		 DO_SEND(0);                            /* tag for 'pointer to following descriptor' */

                 DBUG_PRINT ("MSG", ("send pointer to following descriptor"));
		 
		 /* send class and type of descriptor packed into one int */
		 
		 SEND_2_SHORTS((short)(d_class = R_DESC(*(T_PTD)send_data,class)),(short)(d_type = R_DESC(*(T_PTD)send_data,type)));
		 
		 /* refcount doesn`t need to be sent, will be set to 1 on dest processor */
		 /* and now: the main part of sending: the descriptor bodies */
		 switch(d_class)
		   {case C_SCALAR:
                      DBUG_PRINT("MSG", ("C_SCALAR"));
		      DO_SEND(R_SCALAR(*(T_PTD)send_data,vali));
#if (nCUBE || D_PVM_NCUBE)
		      if (type == MT_END_RESULT)
			{DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));
			 DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));}
		      else
#endif /* nCUBE */
			{DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));
			 DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));}
		      break;
		    case C_DIGIT:
                      DBUG_PRINT("MSG", ("C_DIGIT"));
		      SEND_2_SHORTS(R_DIGIT(*(T_PTD)send_data,sign),R_DIGIT(*(T_PTD)send_data,ndigit));
		      heap_elems = (int)R_DIGIT(*(T_PTD)send_data,ndigit);
		      DO_SEND(R_DIGIT(*(T_PTD)send_data,Exp));
		      heap_ptr = R_DIGIT(*(T_PTD)send_data,ptdv);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			DO_SEND(*heap_ptr);
		      break;
		    case C_LIST:
                       DBUG_PRINT("MSG", ("C_LIST, dim is %d", R_LIST(*(T_PTD)send_data,dim)));

/* fprintf(debugger, "sending C_LIST..., dim is %d\n", R_LIST(*(T_PTD)send_data,dim)); fflush(debugger);*/

		       SEND_2_SHORTS(R_LIST(*(T_PTD)send_data,special),R_LIST(*(T_PTD)send_data,dim));
		       heap_elems = (int)R_LIST(*(T_PTD)send_data,dim);
		       heap_ptr = R_LIST(*(T_PTD)send_data,ptdv);
		       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			 D_PUSHSTACK(D_S_hilf,*heap_ptr);
		       break;
                     case C_INTACT:
                       DBUG_PRINT("MSG", ("C_INTACT"));
                       heap_elems = (int)R_INTACT(*(T_PTD)send_data,dim);
                       DO_SEND(heap_elems);
                       heap_ptr = R_INTACT(*(T_PTD)send_data,args);
                       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
                         D_PUSHSTACK(D_S_hilf,*heap_ptr);
                       break;
                     case C_FRAME:
                        switch(d_type)
                        { case TY_FRAME:
                             DBUG_PRINT("MSG", ("C_FRAME/TY_FRAME"));
                             DO_SEND(R_FRAME(*(T_PTD)send_data,dim));
                             heap_elems = (int)R_FRAME(*(T_PTD)send_data,dim);
                             heap_ptr = R_FRAME(*(T_PTD)send_data,slots);
                             for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
                                 D_PUSHSTACK(D_S_hilf,*heap_ptr);
                             break;
                          case TY_SLOT:
                             DBUG_PRINT("MSG", ("C_FRAME/TY_SLOT"));
                             D_PUSHSTACK(D_S_hilf,R_SLOT(*(T_PTD)send_data,name));
                             D_PUSHSTACK(D_S_hilf,R_SLOT(*(T_PTD)send_data,value));
                             break;
                        }
                        break;
		     case C_MATRIX:
                       DBUG_PRINT("MSG", ("C_MATRIX"));
		     case C_VECTOR:
                       DBUG_PRINT("MSG", ("C_VECTOR"));
		     case C_TVECTOR:
                       DBUG_PRINT("MSG", ("C_TVECTOR ***"));
		       SEND_2_SHORTS(R_MVT(*(T_PTD)send_data,nrows,class),R_MVT(*(T_PTD)send_data,ncols,class));
		       heap_elems = (int)R_MVT(*(T_PTD)send_data,nrows,class) * (int)R_MVT(*(T_PTD)send_data,ncols,class);
		       if ((_formated == 1) && (d_type == TY_REAL))
			 heap_elems *= 2;
		       heap_ptr = R_MVT(*(T_PTD)send_data,ptdv,class);
		       if ((_formated == 1) && (d_type != TY_STRING))
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   DO_SEND(*heap_ptr);
		       else
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   D_PUSHSTACK(D_S_hilf,*heap_ptr);
		       break;
		     case C_EXPRESSION:
                       DBUG_PRINT("MSG", ("C_EXPRESSION"));
		     case C_CONSTANT:
                       DBUG_PRINT("MSG", ("C_CONSTANT ***"));
		       switch(d_type)
			 {case TY_REC:
                            DBUG_PRINT ("MSG", ("TY_REC"));
			  case TY_ZF:
                            DBUG_PRINT ("MSG", ("TY_ZF"));
			  case TY_SUB:
                            DBUG_PRINT ("MSG", ("TY_SUB ***"));
			    SEND_2_SHORTS(R_FUNC(*(T_PTD)send_data,special),R_FUNC(*(T_PTD)send_data,nargs));
			    if (R_FUNC(*(T_PTD)send_data,namelist))
			      {heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,namelist);
			       heap_ptr = R_FUNC(*(T_PTD)send_data,namelist) + 1;
			       DO_SEND(heap_elems);
#if WITHTILDE
			       param_counter = *(heap_ptr++);
			       DO_SEND(param_counter);
			       heap_elems--;
#endif
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
#if WITHTILDE
				 if ((param_counter > 0) && (heap_counter >= param_counter-2))
				   DO_SEND(*heap_ptr);
				 else
#endif
				   D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
#if WITHTILDE
			    if (R_FUNC(*(T_PTD)send_data,pte))
			      {
#endif
				heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
				heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
				DO_SEND(heap_elems);
				for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				  D_PUSHSTACK(D_S_hilf,*heap_ptr);
#if WITHTILDE
			      }
			    else
			      DO_SEND(-1);
#endif
			    break;
#if WITHTILDE
			  case TY_SNSUB:
                            DBUG_PRINT("MSG", ("TY_SNSUB"));
			    SEND_2_SHORTS(R_FUNC(*(T_PTD)send_data,special),R_FUNC(*(T_PTD)send_data,nargs));
			    heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
			    heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
#endif
			  case TY_ZFCODE:
                            DBUG_PRINT("MSG", ("TY_ZFCODE"));
			    SEND_2_SHORTS(R_ZFCODE(*(T_PTD)send_data,zfbound),R_ZFCODE(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_ZFCODE(*(T_PTD)send_data,ptd));
			    heap_elems = *(int *)R_ZFCODE(*(T_PTD)send_data,varnames);
			    heap_ptr = R_ZFCODE(*(T_PTD)send_data,varnames) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_COND:
                            DBUG_PRINT("MSG", ("TY_COND"));
			    DO_SEND((int)R_COND(*(T_PTD)send_data,special));
			    heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptte);
			    heap_ptr = R_COND(*(T_PTD)send_data,ptte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptee);
			    heap_ptr = R_COND(*(T_PTD)send_data,ptee) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_VAR:
                            DBUG_PRINT("MSG", ("TY_VAR"));
			    DO_SEND(R_VAR(*(T_PTD)send_data,nlabar));
			    D_PUSHSTACK(D_S_hilf,R_VAR(*(T_PTD)send_data,ptnd));
			    break;
			  case TY_EXPR:
                            DBUG_PRINT("MSG", ("TY_EXPR"));
			    heap_elems = *(int *)R_EXPR(*(T_PTD)send_data,pte);
			    heap_ptr = R_EXPR(*(T_PTD)send_data,pte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_NAME:
                            DBUG_PRINT("MSG", ("TY_NAME"));
                            if (T_INT((int)R_NAME(*(T_PTD)send_data,ptn))) {
                              /* sorry ;-), but this is the special case for NAME- */
                              /* descriptors in a (UH-) pattern graph, because */
                              /* the (real) name is irrelevant and so the index */
                              /* is stored in the ptn field */
                              DBUG_PRINT("MSG", ("It's an index: %d", VAL_INT((int)R_NAME(*(T_PTD)send_data,ptn))));
                              DO_SEND(R_NAME(*(T_PTD)send_data,ptn));
                            } else {
                            DO_SEND(0); /* T_INT will fail on that (hopefully) */
			    heap_elems = (*(int *)R_NAME(*(T_PTD)send_data,ptn));
                            DBUG_PRINT("MSG", ("heap_elems: %d", heap_elems));
			    heap_ptr = R_NAME(*(T_PTD)send_data,ptn) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				D_PUSHSTACK(D_S_hilf,*heap_ptr);
                            }
			    break;
			  case TY_SWITCH:
                            DBUG_PRINT("MSG", ("TY_SWITCH"));
#if WITHTILDE
			    SEND_2_SHORTS(R_SWITCH(*(T_PTD)send_data,nwhen),R_SWITCH(*(T_PTD)send_data,anz_args));
			    DO_SEND(R_SWITCH(*(T_PTD)send_data,casetype));
#else
			    SEND_2_SHORTS(R_SWITCH(*(T_PTD)send_data,special),R_SWITCH(*(T_PTD)send_data,case_type));
			    DO_SEND(R_SWITCH(*(T_PTD)send_data,nwhen));
#endif
			    heap_elems = *(int *)R_SWITCH(*(T_PTD)send_data,ptse);
			    heap_ptr = R_SWITCH(*(T_PTD)send_data,ptse) + 1;
			    DO_SEND(heap_elems);
#if WITHTILDE
			    DO_SEND(*(heap_ptr+heap_elems-1));
			    heap_elems--;
#endif
			    for (heap_counter = 0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_NOMAT:
                            DBUG_PRINT("MSG", ("TY_NOMAT"));
#if WITHTILDE
			    SEND_2_SHORTS(R_NOMAT(*(T_PTD)send_data,act_nomat),R_NOMAT(*(T_PTD)send_data,reason));
#else
			    DO_SEND(R_NOMAT(*(T_PTD)send_data,act_nomat));
#endif
			    if (R_NOMAT(*(T_PTD)send_data,guard_body))
			      {DBUG_PRINT("MSG", ("R_NOMAT(*(T_PTD)send_data,guard_body: %d", R_NOMAT(*(T_PTD)send_data,guard_body)));
                               heap_elems = *(int *)R_NOMAT(*(T_PTD)send_data,guard_body);
			       heap_ptr = R_NOMAT(*(T_PTD)send_data,guard_body) + 1;
			       DO_SEND(heap_elems);
                               DBUG_PRINT("MSG", ("heap_elems: %d", heap_elems));
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    D_PUSHSTACK(D_S_hilf,R_NOMAT(*(T_PTD)send_data,ptsdes));
			    break;
			  case TY_MATCH:
                            DBUG_PRINT("MSG", ("TY_MATCH"));
			    if (R_MATCH(*(T_PTD)send_data,guard))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,guard);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,guard) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    if (R_MATCH(*(T_PTD)send_data,body))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,body);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,body) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    if (R_MATCH(*(T_PTD)send_data,code))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,code);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,code) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 {if (T_POINTER(*heap_ptr) && isdesc(*heap_ptr))
				    {DO_SEND(PM_DESC_MARKER);
				     D_PUSHSTACK(D_S_hilf,*heap_ptr);}
				  else
				    DO_SEND(*heap_ptr);}}
			    else
			      DO_SEND(-1);
			    break;
			  case TY_LREC:
                            DBUG_PRINT("MSG", ("TY_LREC"));
			    DO_SEND(R_LREC(*(T_PTD)send_data,nfuncs));
			    heap_elems = *(int *)R_LREC(*(T_PTD)send_data,namelist);
			    heap_ptr = R_LREC(*(T_PTD)send_data,namelist) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    heap_elems = R_LREC(*(T_PTD)send_data,nfuncs);
#if WITHTILDE
			    heap_ptr = R_LREC(*(T_PTD)send_data,ptdv)+1;
#else
			    heap_ptr = R_LREC(*(T_PTD)send_data,ptdv);
#endif
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_LREC_IND:
                            DBUG_PRINT("MSG", ("TY_LREC_IND"));
			    DO_SEND(R_LREC_IND(*(T_PTD)send_data,index));
			    D_PUSHSTACK(D_S_hilf,R_LREC_IND(*(T_PTD)send_data,ptdd));
			    D_PUSHSTACK(D_S_hilf,R_LREC_IND(*(T_PTD)send_data,ptf));
			    break;
			  case TY_LREC_ARGS:
                            DBUG_PRINT("MSG", ("TY_LREC_ARGS"));
			    DO_SEND(heap_elems = R_LREC_ARGS(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_LREC_ARGS(*(T_PTD)send_data,ptdd));
			    heap_ptr = R_LREC_ARGS(*(T_PTD)send_data,ptdv);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
                          default:
                            DBUG_PRINT("MSG", ("Unknown C_EXPRESSION C_CONSTANT desc !"));
                            post_mortem ("Unknown C_EXPRESSION C_CONSTANT desc !");
                            break;
                            }
		       break;
		     case C_FUNC:
                       DBUG_PRINT("MSG", ("C_FUNC"));
		     case C_CONS:
                       DBUG_PRINT("MSG", ("C_CONS ***"));
		       switch(d_type)
			 {case TY_CLOS:
                            DBUG_PRINT("MSG", ("TY_CLOS"));
			    SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,args),R_CLOS(*(T_PTD)send_data,nargs));
#if WITHTILDE
			    SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,ftype),R_CLOS(*(T_PTD)send_data,nfv));
#else
			    DO_SEND(R_CLOS(*(T_PTD)send_data,ftype));
#endif
#if WITHTILDE
                            heap_elems = R_CLOS(*(T_PTD)send_data,args)+R_CLOS(*(T_PTD)send_data,nfv)+1;
#else
			    heap_elems = R_CLOS(*(T_PTD)send_data,args)+1;
#endif
			    heap_ptr = R_CLOS(*(T_PTD)send_data,pta);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_COMB:
                            DBUG_PRINT("MSG", ("TY_COMB"));
			    SEND_2_SHORTS(R_COMB(*(T_PTD)send_data,args),R_COMB(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_COMB(*(T_PTD)send_data,ptd));
			    heap_ptr = R_COMB(*(T_PTD)send_data,ptc);
			    goto send_code;
			  case TY_CONDI:
                            DBUG_PRINT("MSG", ("TY_CONDI"));
			    SEND_2_SHORTS(R_CONDI(*(T_PTD)send_data,args),R_CONDI(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_CONDI(*(T_PTD)send_data,ptd));
			    heap_ptr = R_CONDI(*(T_PTD)send_data,ptc);
			    goto send_code;
			  case TY_CONS:
                            DBUG_PRINT("MSG", ("TY_CONS"));
			    D_PUSHSTACK(D_S_hilf,R_CONS(*(T_PTD)send_data,hd));
			    D_PUSHSTACK(D_S_hilf,R_CONS(*(T_PTD)send_data,tl));
			    break;
			  case TY_CASE:
                            DBUG_PRINT("MSG", ("TY_CASE"));
			  case TY_WHEN:
                            DBUG_PRINT("MSG", ("TY_WHEN"));
			  case TY_GUARD:
                            DBUG_PRINT("MSG", ("TY_GUARD"));
			  case TY_BODY:
                            DBUG_PRINT("MSG", ("TY_BODY ***"));
			    SEND_2_SHORTS(R_CASE(*(T_PTD)send_data,args),R_CASE(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_CASE(*(T_PTD)send_data,ptd));
			    heap_ptr = R_CASE(*(T_PTD)send_data,ptc);
                            DO_SEND(heap_ptr);
                            if (T_INT((int)heap_ptr)) { /* it's NO code-Pointer ! */
                              DBUG_PRINT("MSG", ("no code pointer"));
                              break;
                              }
send_code:
                            DBUG_PRINT("MSG", ("code index..."));
			    DO_SEND(heap_ptr - GET_CODE_START());
			    break;
                          default:
                            DBUG_PRINT("MSG", ("unknown C_FUNC C_CONS desc !"));
                            post_mortem("unknown C_FUNC C_CONS desc !");
                            break;
			  }                 /* switch(d_type) */
		       break;
                     case C_MATCHING:
                       DBUG_PRINT("MSG", ("C_MATCHING"));
                       switch(d_type) {
                         case TY_SELECTION:
                           DBUG_PRINT("MSG", ("TY_SELECTION"));
                           DO_SEND((int)R_SELECTION(*(T_PTD)send_data, nclauses));
                           D_PUSHSTACK(D_S_hilf,R_SELECTION(*(T_PTD)send_data, clauses));
                           D_PUSHSTACK(D_S_hilf,R_SELECTION(*(T_PTD)send_data, otherwise));
                           break;
                         case TY_CLAUSE:
                           DBUG_PRINT("MSG", ("TY_CLAUSE"));
                           D_PUSHSTACK(D_S_hilf,(int)R_CLAUSE(*(T_PTD)send_data, next));
                           heap_ptr = R_CLAUSE(*(T_PTD)send_data, sons);
                           for (heap_counter=0;heap_counter < 4;heap_counter++,heap_ptr++) {
                             D_PUSHSTACK(D_S_hilf,*heap_ptr);
                             DBUG_PRINT("MSG", ("*heap_ptr = %x", *heap_ptr)); }
                           break;
                        default:
                          DBUG_PRINT("MSG", ("unknown d_type")); break;
                         }
                       break;
                     case C_PATTERN:
                       DBUG_PRINT("MSG", ("C_PATTERN"));
                       DO_SEND((int)R_PATTERN(*(T_PTD)send_data, following));
                       D_PUSHSTACK(D_S_hilf,R_PATTERN(*(T_PTD)send_data, pattern));
                       D_PUSHSTACK(D_S_hilf,R_PATTERN(*(T_PTD)send_data, variable)); 
                       break;
		     default:
                       DBUG_PRINT("MSG", ("unknown d_class desc !"));
                       post_mortem("unknown d_class desc !");
                       break;
                    }                      /* switch(d_class) */
		  new_node = (SENT_LIST_NODE *)malloc(sizeof(SENT_LIST_NODE));
		  if (!new_node)
		    post_mortem("insufficient memory for sent_list");
		  new_node->ptr = (T_PTD)send_data;
		  new_node->original_entry = *((int *)send_data + 1);
		  new_node->original_class = d_class;
		  new_node->next = sent_list;
		  sent_list = new_node;
		  L_DESC(*(T_PTD)send_data,class) = C_SENT;
		  *((int *)send_data + 1) = sent_index++;
	       }                           /* else {descriptor not sent yet}*/
	     }                                 /* else {not in static heap} */
	   }                                  /* else {null pointer}        */
	 }                                       /* else {pointer on stack} */
       }                                                           /* while */
#if MSG_CHKSUM
      DO_SEND(chksum);
#endif
      DO_SEND(END_MARKER);
      while (sent_list)
	{SENT_LIST_NODE *tmp;
	 
	 L_DESC(*(sent_list->ptr),class) = sent_list->original_class;
	 *((int *)sent_list->ptr + 1) = sent_list->original_entry;
	 tmp = sent_list;
	 sent_list = sent_list->next;
	 free(tmp);}

/*      if (type == MT_END_RESULT)
	post_mortem("Das war's!"); */
	
      break;
    case MT_RESULT:

  /* initialize_fred_fish();  */

/*  fprintf(debugger, "0x%x: sending MT_RESULT to pid 0x%x on node %d\n", curr_pid, (int) PC_GET_PPID(curr_pid), dest); fflush(debugger);      */

    DBUG_PRINT ("PVM", ("MT_RESULT processing..."));
      
#if D_SPEC
      if (PC_GET_PPID(curr_pid) == 0) {
        DBUG_PRINT("MSG", ("reinstall father_pid !"));
        post_mortem("PPID was 0 !");
       curr_pid->ppid = curr_pid->father_pid;
      }
#endif /* D_SPEC */

#ifdef D_MESS_ONLY
      m_type = "MT_RESULT";
      m_pid = (int) PC_GET_PPID(curr_pid);
#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin)
	MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif
      
     DBUG_PRINT("MSG",("Result -> Node %d: class %d,type %d, desc %x",dest,
			R_DESC(*((T_PTD)*(ps_w->TopofStack)),class),
			R_DESC(*((T_PTD)*(ps_w->TopofStack)),type),*(ps_w->TopofStack)));

      /* header */
      DO_SEND(PC_GET_PPID(curr_pid));

      DBUG_PRINT("MSG", ("sending to pid %x, curr_pid is %x", PC_GET_PPID(curr_pid), curr_pid));

#if WITHTILDE
      DO_SEND(D_POPSTACK(D_S_R));
      DO_SEND(D_POPSTACK(D_S_R));
#else
      DO_SEND(D_POPSTACK(D_S_H));
      DO_SEND(D_POPSTACK(D_S_H));
#endif

/* red counter handling */

      DO_SEND(PC_GET_SREDCNT(curr_pid)-_redcnt);

      DBUG_PRINT("MSG", ("the process used up %d steps...", PC_GET_SREDCNT(curr_pid)-_redcnt));

#if (D_SPEC && D_SLAVE)
      DBUG_PRINT("MSG", ("specmask1 is: %x", curr_pid->specmask1));
      DO_SEND(curr_pid->specmask1);
      DO_SEND(curr_pid->father_slot);
      DO_SEND(curr_pid);

      if (T_SPEC_WINNER(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_WINNER"));
        } 
      if (T_SPEC_NO_STEPS(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_NO_STEPS"));
        } 
      if (T_SPEC_FAIL(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_FAIL"));
        } 
      if (T_SPEC_NO_BSTEPS(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_NO_BSTEPS"));
        } 
      if (T_SPEC_GUARD(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_GUARD"));
        }
      if (T_SPEC_GUARD_SON(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_GUARD_SON"));
        }
      if (T_SPEC_UNRED_GUARD(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_UNRED_GUARD"));
        } 
      if (T_SPEC_INSYNC(curr_pid)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_INSYNC"));
        }
#endif /* D_SPEC */

      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      result_stelem = D_POPSTACK(*ps_w);
      D_PUSHSTACK(D_S_hilf,result_stelem);
      goto send_loop;
    case MT_END_RESULT:


/*   initialize_fred_fish(); */

/*    fprintf(debugger, "0x%x: sending MT_END_RESULT to 0x%x\n", curr_pid, dest); fflush(debugger);    */

    DBUG_PRINT ("PVM", ("MT_END_RESULT processing..."));

#ifdef D_MESS_ONLY
  m_type = "MT_END_RESULT";
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_send_begin)
    MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif

/*      DBUG_PRINT("MSG",("End result: class %d,type %d",
			     R_DESC(*((T_PTD)*(ps_w->TopofStack)),class),
			     R_DESC(*((T_PTD)*(ps_w->TopofStack)),type))); */

#if D_SPEC
      /* there was a problem returning all steps back to the host */

      if (real_start_redcount != curr_pid->start_red_count) {
        DBUG_PRINT("MSG", ("start_red_count was modified, recalculate !"));
        DBUG_PRINT("MSG", ("real_start_redcount: %d, start_red_count: %d, _redcnt: %d",  
                    real_start_redcount, curr_pid->start_red_count, _redcnt));
        _redcnt = real_start_redcount - curr_pid->start_red_count;
        } 
#endif

      DO_SEND(_redcnt);
#if (ADV_SCHED && D_MESS)
      DO_SEND(no_suspended); /* sending the no. of suspended processes */
#endif /* ADV_SCHED && D_MESS */
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      result_stelem = D_POPSTACK(*ps_w);
      D_PUSHSTACK(D_S_hilf,result_stelem);
      SET_STATIC_BORDER(stdesc_border);
      DBUG_PRINT("STATHEAP",("new end of static heap: %08x",stdesc_border));
      goto send_loop;
    }                                       /* switch(type) */

#if nCUBE
 result = nwritep((char *)buffer,(buffer_p - buffer) * sizeof(int),dest,type,&flags);
 if (result < 0)
   post_mortem("error on nwrite");
#endif /* nCUBE */

#if D_PVM
 pvm_send(pvm_dest, type);
#endif /* D_PVM */

#ifdef D_MESS_ONLY
 m_size += (buffer_p - buffer) * sizeof(int);
#endif

 if ((type == MT_RESULT) || (type == MT_END_RESULT))
    T_DEC_REFCNT((T_PTD)result_stelem);

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
 if (m_msg_send_end)
   MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_size);
#endif
 m_curr_pid = m_backup_pid;
#endif

/*  fprintf(debugger, "* ** leaving msg_send\n"); fflush(debugger); */

 DBUG_VOID_RETURN;
}


/***************************************************************/
/*                                                             */
/* msg_receive()  receives a message and calls msg_handle      */
/*                                                             */
/***************************************************************/

#ifdef D_MESS_ONLY

void m_msg_receive()

#else

void msg_receive()

#endif

{int *buffer,*buffer_p,*buffer_end;
 int src=-1,type = -1,flags;
 STACKELEM *h_stack_addr, *a_stack_addr;
 int new_redcnt, new_letpar_level;
 int stelem_counter,stelem_number,heap_elems,heap_counter,param_counter;
 int received_index= 0;
 int *rec_addr, rec_data;
 int counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 INSTR *new_pc = 0;
 INSTR_DESCR rec_instr;
 PROCESS_CONTEXT *new_pid,*ppid,*rec_pid;
 DESC_CLASS new_class;
 DESC_TYPE new_type;
 PTR_HEAPELEM new_heap,code_address;
 T_PTD *received_list;
 T_PTD new_desc;
 int rec_list_size = 256;
 int code_received = 0;
 int result;
#if D_SPEC
 PROCESS_CONTEXT *spec_tmp1;
 int spec_tmp2, spec_tmp3, spec_tmp4;
#if D_SPEC_TICKETS
 int spec_tmp6, spec_tmp7;
#endif
 int spec_flag; /* message for a speculative process */
#if D_SPEC_TICKETS
 int spec_help;
#endif
#endif
#if D_PVM
 int pvm_counter=0;
 int pvm_bufid;
 int pvm_length;
 int pvm_src;
 char m_remote_host[80];
 char m_remote_dir[80];
 char m_line[120];
#endif /* D_PVM */

#if ADV_SCHED
 int ic,jc;
#endif /* ADV_SCHED */
#if ADV_SCHED2
 PROCESS_CONTEXT *tmp1,*tmp2,*tmp3;
 int dummy;
#endif /* ADV_SCHED2 */
 
#if D_MESS
static struct sigaction act = {handle_signal,0,0};
#ifdef M_OLD_MERGING
 char mess_line[120];
#endif
 static char m_super_buffer[4096];
 int m_super_counter;
 struct stat stbuf;
 char name_line[120];
 int i,k;
 FILE *m_super_file1;
 FILE *m_super_file2;
 char m_super_name[120];
 int m_super_nr;
 int m_super_c;
#endif

#ifdef D_MESS_ONLY 
 int m_size = 0;
 char *m_type;
 int m_pid = -1;
 int f_pid = -1;
#endif 

#ifdef D_MESS_ONLY
 DBUG_ENTER ("m_msg_receive");
#else
 DBUG_ENTER("msg_receive");
#endif

/*   fprintf(debugger, "**** entering msg_receive\n"); fflush(debugger); */

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_receive_begin)
    MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);
#endif
#endif

 /* prepare the received list */
 received_list = (T_PTD *)malloc(sizeof(T_PTD)*rec_list_size);
 if (!received_list)
   post_mortem("insufficient memory for received_list (initializing)");

 /* receive first block off message */
#ifdef D_MESS_ONLY
#if nCUBE
 m_size += ( result = nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags) );
 if (result < 0)
   post_mortem("error on nread");
#endif
#if D_PVM
 pvm_bufid = pvm_recv(-1,-1);
 pvm_bufinfo (pvm_bufid, &pvm_length, &type, &pvm_src);
 m_size += pvm_length;
#endif /* D_PVM */
#else
#if nCUBE
 result = nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags);
 if (result < 0)
   post_mortem("error on nread");
#endif /* nCUBE */
#if D_PVM
 pvm_bufid = pvm_recv(-1,-1);
 pvm_bufinfo (pvm_bufid, &pvm_length, &type, &pvm_src); 
#endif /* D_PVM */
#endif
#if D_PVM
 pvm_counter = 0;
/* converting src back to logical node nr */
 if (pvm_src != host) {
   for (_tmp =0; _tmp<pvmspawn_ntask; _tmp++) /* identify the logical node number */
     if (pvm_src==pvm_tids[_tmp]){src=_tmp; break;}
   DBUG_PRINT("PVM", ("Receiving message from node %d, tid t%x", src, pvm_src)); }
 else
   DBUG_PRINT("PVM", ("Receiving message from Host tid t%x", pvm_src));
#endif /* D_PVM */

 buffer_p = buffer;
 buffer_end = buffer + result/sizeof(int);

/* if (type != MT_PARAM)
   hpchain2(); */

 res_heap();
 _heap_reserved = TRUE;

 DBUG_PRINT ("PVM", ("Before Switch..."));

 switch(type)
   {case MT_ABORT:

/*     fprintf(debugger, "receiving MT_ABORT\n"); fflush(debugger);   */

    DBUG_PRINT ("PVM", ("MT_ABORT processing..."));

#ifdef D_MESS_ONLY 
      m_type = "MT_ABORT";
#endif  /* D_MESS_ONLY */

      process_terminate_all();

      flush_queue();
      longjmp(post_mortem_buffer,1);
      break;

    case MT_CONTROL:

/*   fprintf(debugger, "receiving MT_CONTROL\n"); fflush(debugger);     */

#if D_SPEC
      DBUG_PRINT ("MSG", ("receiving MT_CONTROL message from node %d", src));

      /* receive the fixed package */
      spec_control[0] = DO_RECEIVE();
      spec_control[1] = DO_RECEIVE();
      spec_control[2] = DO_RECEIVE();
      spec_control[3] = DO_RECEIVE();
      spec_control[4] = DO_RECEIVE(); 
      spec_control[5] = DO_RECEIVE();
      spec_control[6] = DO_RECEIVE();
      spec_control[7] = DO_RECEIVE();
      spec_control[8] = DO_RECEIVE();
      spec_control[9] = DO_RECEIVE();

      /* now this stuff has to be interpreted */

      /* ok ok, I'll reformat the stuff vor 88 rows or so... ;-) */

      if (T_SPECMSG_HEREAMI) {   /********** T_SPECMSG_HEREAMI **********/

        DBUG_PRINT("MSG", ("HEREAMI-message received, processing"));
        DBUG_PRINT("MSG", ("the message is for pid : %x", spec_control[1]));

/*   fprintf(debugger, "0x%x: receiving HEREAMI from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);    */

#ifdef D_MESS_ONLY 
      m_type = "MT_CONTROL_HERE";
      m_pid = spec_control[2]; /* from which pid ?! */
      f_pid = spec_control[1]; /* for which pid ?! */
#endif

        spec_sched_status = 1;
        } else

      if (T_SPECMSG_ACK) {   /********** T_SPECMSG_ACK **********/
        DBUG_PRINT("MSG", ("ACK-message received, processing"));
        DBUG_PRINT("MSG", ("the message is for pid : %x", spec_control[1]));

#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_ACK";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

/*   fprintf(debugger, "0x%x: receiving SPECMSG_ACK from (0x%x, 0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);   */

        if (!T_SPEC_GRANDDAD((PROCESS_CONTEXT *)spec_control[1])) {
          DBUG_PRINT("MSG", ("target proc is no GRANDDAD any more"));
          post_mortem("SPEC_ACK message received, but it wasn't a granddad process...");
          } else {

          DBUG_PRINT("MSG", ("Okey, let's register the processes"));

          /* remove the father process */
  
         spec_tmp1 = spec_control[1];
         spec_tmp2 = spec_control[3];
         spec_tmp4 = spec_control[2];
#if D_SPEC_TICKETS
         spec_tmp6 = spec_control[6];
         spec_tmp7 = spec_control[7];
         if (spec_tmp6 == 1) {
           DBUG_PRINT("MSG", ("post_proc, don't remove the father !"));
           spec_control[4]=1;
           spec_tmp2 = 1;
           }
#endif

/*   fprintf(debugger, "** it's this ACK !\n"); fflush(debugger);  */

          if (spec_control[4]==1) { /* it was a granddad speculative process */
            DBUG_PRINT("MSG", ("DON'T remove the father's PID from the PIPE..."));
            } else {
            DBUG_PRINT("MSG", ("remove the father's PID from the PIPE..."));
            spec_remove_control_pid(spec_control[1], spec_control[2], src, 0);

 /* dump_processes(spec_control[1]);  */
            }

          /* oh, this is some kind of loop, so we can't use the globals any more... */

          if ((spec_tmp1->control->leaves+spec_tmp2)>max_spec_procs)
             post_mortem("Too many speculative processes in syncronisation !");
 
          DBUG_PRINT("MSG", ("already stored: %d", spec_tmp1->control->leaves));

/*fprintf(debugger, "0x%x: %d are stored, %d have acknowledged... control 0x%x\n", curr_pid, spec_tmp1->control->leaves, spec_tmp1->control->have_ack, spec_tmp1); fflush(debugger);*/

          for (spec_tmp3=0; spec_tmp3<spec_tmp2; spec_tmp3++) {
            spec_tmp1->control->data[(spec_tmp1->control->leaves*2)+2*spec_tmp3] =
                  heap_counter=DO_RECEIVE();
            spec_tmp1->control->data[(spec_tmp1->control->leaves*2)+2*spec_tmp3+1] =
                  param_counter=DO_RECEIVE();
            DBUG_PRINT("MSG", ("registering pid %x of node %d as number %d", 
                       heap_counter, param_counter, spec_tmp1->control->leaves+spec_tmp3));

/* fprintf(debugger, "0x%x: nl ACK; registering pid 0x%x of node %d as number %d\n", curr_pid, heap_counter, param_counter, spec_tmp1->control->leaves+spec_tmp3); fflush(debugger);*/

            }  
          spec_control[1] = src;
          spec_control[2] = spec_tmp4;
          spec_control[4] = spec_tmp1;
#if D_SPEC_TICKETS
          spec_control[8] = spec_tmp6;
          spec_control[9] = spec_tmp7;
#endif

/*fprintf(debugger, "0x%x: ACK processed, %d leaves registered, %d have acknowledged\n", curr_pid, spec_tmp1->control->leaves, spec_tmp1->control->have_ack); fflush(debugger);*/

          spec_sched_status = 2;
          spec_tmp1->control->leaves += spec_tmp2;

/* fprintf(debugger, "A %d %d\n", spec_tmp1->control->leaves, spec_tmp1->control->have_ack); fflush(debugger); */

/* fprintf(debugger, "* TOTAL %d PROCESSES !\n", spec_tmp1->control->leaves); fflush(debugger); */

          DBUG_PRINT("MSG", ("total processes: %d", spec_tmp1->control->leaves));

          /* dump_processes(spec_tmp1);  */

          }  
        } else

      if (T_SPECMSG_BARR) {    /********** T_SPECMSG_BARR **********/
        /* this has to be ignored when a result has been found already */
        DBUG_PRINT("MSG", ("BARR-message received, processing"));
        DBUG_PRINT("MSG", ("the message is for pid : %x", spec_control[1]));

/*  fprintf(debugger, "0x%x: receiving BARR from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */


#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_BARR";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

        spec_sched_status = 3;
        } else

      if (T_SPECMSG_GOON) {    /********** T_SPECMSG_GOON **********/
        DBUG_PRINT("MSG", ("GOON-message received, processing"));
        DBUG_PRINT("MSG", ("the message is for pid : %x, specmask %x", 
                  spec_control[1], ((PROCESS_CONTEXT *)spec_control[1])->specmask1));
        /* give the process more redsteps, err, more or less */
        /* first check whether it's not a waiting father or not */

#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_GOON";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

/* fprintf(debugger, "0x%x: receiving GOON from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */

        spec_sched_status = 4;
        } else

      if (T_SPECMSG_WIN) {    /********** T_SPECMSG_WIN **********/
        /* there's a result somewhere !!! */
        /* now someone has to mask that there's already a result */
        DBUG_PRINT("MSG", ("WIN-message received, processing !"));

#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_WIN";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

/* fprintf(debugger, "0x%x: receiving WIN from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */

        spec_sched_status = 5;

        } else 

      if (T_SPECMSG_START) {   /********** T_SPECMSG_START **********/
        /* finally, a speculative process may start working */
        DBUG_PRINT("MSG", ("SPECMSG_START received..."));
        DBUG_PRINT("MSG", ("initialize with %d red steps", spec_control[2]));

#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_START";
        m_pid = spec_control[4]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

/* fprintf(debugger, "0x%x: receiving START from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[4], src, spec_control[1]); fflush(debugger); */

        spec_sched_status = 6;

        } else 

      if (T_SPECMSG_TERM) {    /********** T_SPECMSG_TERM **********/
        /* oh, terminate a process, how comes ? ;) */
        DBUG_PRINT("MSG", ("SPECMSG_TERM received..."));

#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_TERM";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

/* fprintf(debugger, "0x%x: receiving TERM from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */

        DBUG_PRINT("MSG", ("Flag (0=father, 1=control) is %d !", spec_control[3]));

        spec_sched_status = 7;

        } else

      if (T_SPECMSG_FAIL) {   /********** T_SPECMSG_FAIL **********/
        /* oh, whowsers (Inspector Gadget) ! A failed spec process ! */
        DBUG_PRINT("MSG", ("SPECMSG_FAIL received !"));

        spec_control[0] = src;

/* fprintf(debugger, "0x%x: receiving FAIL from (0x%x,0x%d) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */
        
#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_FAIL";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif

        spec_sched_status = 8;
        } else

      if (T_SPECMSG_VITAL) {   /********** T_SPECMSG_VITAL **********/
        /* vitalize a process... */
        DBUG_PRINT("MSG", ("vitalizing a process..."));

/* fprintf(debugger, "0x%x: receiving VITAL from (0x%x,0x%x) for 0x%x\n", curr_pid, spec_control[2], src, spec_control[1]); fflush(debugger);  */
 
#ifdef D_MESS_ONLY 
        m_type = "MT_CONTROL_VITAL";
        m_pid = spec_control[2]; /* from which pid ?! */
        f_pid = spec_control[1]; /* for which pid ?! */
#endif 

        spec_sched_status = 10;
        }

         else {
          DBUG_PRINT("MSG", ("spec_control[0]: %x", spec_control[0]));
          post_mortem ("unknown control message ?!");
          }

/* fprintf(debugger, "Gebe buffer frei: %x (CONTROL)!\n", buffer); fflush(debugger); */

      nrelp((char *)buffer);
      free(received_list);

#else
      post_mortem ("no control messages allowed in non-SPEC version !");
#endif /* D_SPEC */

      break;

    case MT_TERMINATE:

/*  fprintf(debugger, "0x%x: receiving MT_TERMINATE from 0x%x\n", curr_pid, src); fflush(debugger);   */

    DBUG_PRINT ("PVM", ("MT_TERMINATE processing..."));

/* fprintf(debugger, "Exiting...!\n"); fflush(debugger); */

      exit(0);

    case MT_NEW_PROCESS:

/*  fprintf(debugger, "0x%x: receiving MT_NEW_PROCESS from 0x%x\n", curr_pid, src); fflush(debugger);   */

    DBUG_PRINT ("PVM", ("MT_NEW_PROCESS processing..."));

      /* receive the header */

      ppid         = (PROCESS_CONTEXT *)DO_RECEIVE();

#ifdef D_MESS_ONLY 
  m_type = "MT_NEW_PROCESS";
  m_pid = (int) ppid;
#endif 

#if D_SPEC
      new_pc       = /* (INSTR *) */ DO_RECEIVE();

      if (new_pc == -1) {

        DBUG_PRINT("MSG", ("this speculative process was spawned by specmap !"));
        new_pc = rout_specmap2; /* initiate microprogram */
        
/* fprintf(debugger, "%x: received CODE %x (specmap)\n", curr_pid, new_pc); fflush(debugger); */

        /* don't forget to move the stack items... or switch the stacks */

        } else {
        DBUG_PRINT("MSG", ("normal speculative process: new_pc = %x", new_pc));
        new_pc = (INSTR *)((int)new_pc*sizeof(int) + (int)GET_CODE_START());
        DBUG_PRINT("MSG", ("new_pc = %x, code_start is %x", new_pc, GET_CODE_START()));

/* fprintf(debugger, "%x: received CODE %x\n", curr_pid, new_pc); fflush(debugger); */

        }
#else
      new_pc       = (INSTR *)(DO_RECEIVE() + GET_CODE_START());
#endif
#if D_SPEC
      spec_father_slot = DO_RECEIVE();
      DBUG_PRINT("MSG", ("slot number is %d (0x%x)", spec_father_slot, spec_father_slot));
#endif
      a_stack_addr = (STACKELEM *)DO_RECEIVE();
      h_stack_addr = (STACKELEM *)DO_RECEIVE();
      new_redcnt      = DO_RECEIVE();

      DBUG_PRINT("MSG", ("received a_stack_addr (0x%x), h_stack_addr (0x%x), new_redcnt (%d)", a_stack_addr, h_stack_addr, new_redcnt));

#if D_SPEC
      spec_global = DO_RECEIVE(); 
      DBUG_PRINT("RECEIVE", ("spec_global: %x", spec_global));
#endif /* D_SPEC */

#if ADV_SCHED
      new_letpar_level = DO_RECEIVE();
#endif /* ADV_SCHED */

      /* create new process with data from header */

#if (D_SLAVE && D_MESS && D_MSCHED)
      new_pid = (*d_m_process_create)(new_pc,ppid,new_redcnt);
#else
      new_pid = process_create(new_pc,ppid,new_redcnt);
#endif

/* fprintf(debugger, "I was created by pid %x on node %d !\n", ppid, src), fflush(debugger); */

#if D_SPEC
      new_pid->father_slot = spec_father_slot;
   /*   if (new_pc == rout_specmap2) {
        DBUG_PRINT("MSG", ("OK, just  switch the stacks !"));
        new_pid->switched = W_A_SWITCHED; 
        } this crap does not work */
#endif

#if D_SPEC
      if (spec_global>0) {

        new_pid->father_pid = ppid;
        new_pid->father_node = src;
        new_pid->control_pid = DO_RECEIVE();
        new_pid->control_node = DO_RECEIVE();
#if D_SPEC_TICKETS
        new_pid->nesting = DO_RECEIVE();
#endif
        DBUG_PRINT("MSG", ("spec process created by node %d pid %x", src, ppid));
        DBUG_PRINT("MSG", ("control is at node %d pid %x", new_pid->control_node, new_pid->control_pid));
        PC_SET_SPEC_READY(new_pid);
        spec_control[0] = src;
        spec_control[1] = ppid;
        spec_control[2] = new_pid;
#if D_SPEC_TICKETS
        spec_control[3] = a_stack_addr;
        DBUG_PRINT("MSG", ("flag for non-Code-spawned-Process is %d, desc is %x", a_stack_addr, spec_father_slot));
        spec_control[4] = spec_father_slot;
#endif

        spec_sched_status = 9;
        }
#endif
  
#ifdef D_MESS_ONLY
      f_pid = (int) new_pid;
#endif

#if ADV_SCHED
      new_pid->start_letpar_level = new_pid->letpar_level = new_letpar_level;
      DBUG_PRINT ("ADV_SCHED", ("new letpar_levels received: %i", new_letpar_level));

#if ADV_SCHED2
      /* sort top-element of ready_queue deeper ;-) into the queue */

/*      fprintf(stderr,"*** %d sorting down...\n", proc_id); fflush(stderr); */

      tmp1 = NULL;
      tmp2 = ready_queue_first;
      tmp3 = ready_queue_first->next;
      dummy = 0; /***killme***/
      while ((tmp3!=NULL) && (tmp2->letpar_level>=tmp3->letpar_level)) {
        /* exchange */
        dummy++; /***killme***/
        if (tmp1 == NULL)
          ready_queue_first = tmp3;
        else
          tmp1->next = tmp3;
        tmp2->next = tmp3->next; /* tmp2 holds position */
        tmp3->next = tmp2; /* swap */
        tmp1 = tmp3;
        tmp3 = tmp2->next;
/*        fprintf (stderr, "exchanged %d and %d...", tmp2->letpar_level, tmp1->letpar_level); */
        }
      if (tmp3==NULL)
        ready_queue_last = tmp2;

/*      if (dummy>0) {
         fprintf(stderr, "swap'ed %d down...\n", dummy); fflush(stderr); } 
      fprintf(stderr,"\n"); fflush(stderr);  */
 
/*      fprintf(stderr,"*** %d sorted...\n", proc_id); fflush(stderr); */

#endif /* ADV_SCHED2 */

#endif /* ADV_SCHED */
      
      /* get addresses of new stacks */

#if WITHTILDE
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),h_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),a_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),(src << 1));

/* fprintf (debugger, "0x%x: pushing %d as return-node for pid 0x%x on Stack R !\n", curr_pid, src, new_pid); fflush(debugger); */

#else
      D_PUSHSTACK(PC_GET_H_STACK(new_pid),h_stack_addr);
      D_PUSHSTACK(PC_GET_H_STACK(new_pid),a_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),src);
#endif
      
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);

      stelem_number = DO_RECEIVE();
      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	{D_PUSHSTACK(PC_GET_REAL_I_STACK(new_pid),ST_DUMMY);
	 D_PUSHSTACK(PC_GET_H_STACK(new_pid),(int)D_TOP_ADDRESS(PC_GET_I_STACK(new_pid)) << 1);}

      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	D_PUSHSTACK(D_S_hilf,D_POPSTACK(PC_GET_H_STACK(new_pid)));

#if WITHTILDE
      stelem_number = DO_RECEIVE();
      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	{D_PUSHSTACK(PC_GET_H_STACK(new_pid),ST_DUMMY);
	 D_PUSHSTACK(PC_GET_R_STACK(new_pid),(int)D_TOP_ADDRESS(PC_GET_H_STACK(new_pid)) << 1);}

      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	D_PUSHSTACK(D_S_hilf,D_POPSTACK(PC_GET_R_STACK(new_pid)));
#endif /* WITHTILDE */

receive_loop:
      while((int)(rec_addr = (int *)D_POPSTACK(D_S_hilf)) != ST_END_MARKER)
	{ if ((int)rec_addr & 1)
	   {counter = D_POPSTACK(D_S_hilf);
            DBUG_PRINT("MSG", ("counter at start: %d", counter));
	    DBUG_ASSERT(counter > 0);
	    if (counter > 1)
	      {D_PUSHSTACK(D_S_hilf,counter - 1);
	       D_PUSHSTACK(D_S_hilf,rec_addr - 2);}}  /* minus 2, it's still shifted! */
	 rec_addr = (int *)((int)rec_addr >> 1);
/*	 if (isheap(rec_addr))
	   post_mortem("Trying to overwrite header of heapblock!"); */
	 rec_data = DO_RECEIVE();
	 if (rec_data != 0)
	   {DBUG_PRINT("MSG",("receiving non-pointer value VAL_INT %d (%x)",VAL_INT(rec_data),rec_data));

/* fprintf(debugger, "receiving NO pointer...\n"); fflush(debugger); */

	    *rec_addr = rec_data;}
	 else
	   {rec_data = DO_RECEIVE();
	    if (T_STAT_POINTER(rec_data))
	      {*rec_addr = (int)REAL_STATIC_ADDRESS((int)rec_data >> 4);
	       DBUG_PRINT("MSG",("receiving static descriptor pointer %x",*rec_addr));
	       DBUG_ASSERT(isdesc(*rec_addr));
	       INC_REFCNT((T_PTD)*rec_addr);
	       DBUG_PRINT("MSG",("refcount is now %d",((T_PTD)*rec_addr)->u.sc.ref_count));}
	    else
	      {DBUG_PRINT("MSG",("receiving dynamic descriptor pointer..."));
               if (T_DESC_INDEX(rec_data))
		 {*(T_PTD *)rec_addr = received_list[rec_data >> 4];
		  DBUG_PRINT("MSG",("receiving index for descriptor at %x",*rec_addr));
		  DBUG_ASSERT(isdesc(*rec_addr));
		  INC_REFCNT((T_PTD)*rec_addr);
		  DBUG_PRINT("MSG",("refcount is now %d",((T_PTD)*rec_addr)->u.sc.ref_count));}
	       else
		 {if (T_NULL_POINTER(rec_data))
		    {*rec_addr = NULL;}
		 else                    /* a new descriptor will follow */
		   {RECEIVE_2_SHORTS(new_class,new_type);
		    MAKEDESC(new_desc,1,new_class,new_type);
		    *rec_addr = (int)new_desc;
		    DBUG_PRINT("MSG",("receiving desc for address %x: class %d, type %d",rec_addr,new_class,new_type));
		    switch(new_class)
		      {case C_SCALAR:
                         DBUG_PRINT("MSG", ("C_SCALAR"));
			 L_SCALAR(*new_desc,vali) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr)) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr) + 1) = DO_RECEIVE();
			 break;
		       case C_DIGIT:
                         DBUG_PRINT("MSG", ("C_DIGIT"));
			 RECEIVE_2_SHORTS(L_DIGIT(*new_desc,sign),L_DIGIT(*new_desc,ndigit));
			 L_DIGIT(*new_desc,Exp) = DO_RECEIVE();
			 heap_elems = R_DIGIT(*new_desc,ndigit);
			 GET_HEAP(heap_elems,A_DIGIT(*new_desc,ptdv));
			 new_heap = R_DIGIT(*new_desc,ptdv);
			 for (heap_counter = 0;heap_counter < heap_elems;heap_counter++,new_heap++)
			   *new_heap = DO_RECEIVE();
			 break;
		       case C_LIST:
                         DBUG_PRINT("MSG", ("C_LIST"));
			 RECEIVE_2_SHORTS(L_LIST(*new_desc,special),L_LIST(*new_desc,dim));

/* fprintf(debugger, "receiving C_LIST..., dim is %d\n", R_LIST(*new_desc,dim)); fflush(debugger); */

                         DBUG_PRINT("MSG", ("dim is %d", R_LIST(*new_desc,dim)));

			 L_LIST(*new_desc,ptdd) = NULL;
			 D_PUSHSTACK(D_S_hilf,heap_elems = R_LIST(*new_desc,dim));
			 GET_HEAP(heap_elems,A_LIST(*new_desc,ptdv));
			 new_heap = R_LIST(*new_desc,ptdv);
			 D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			 break;
                       case C_MATCHING:
                         DBUG_PRINT("MSG", ("C_MATCHING"));
                         switch(new_type) {
                           case TY_SELECTION:
                             DBUG_PRINT("MSG", ("TY_SELECTION"));
                             L_SELECTION(*new_desc,nclauses) = (unsigned short) DO_RECEIVE();
                             D_PUSHSTACK(D_S_hilf,(int)A_SELECTION(*new_desc,clauses) << 1);
                             D_PUSHSTACK(D_S_hilf,(int)A_SELECTION(*new_desc,otherwise) << 1);
                             break;
                           case TY_CLAUSE:
                             DBUG_PRINT("MSG", ("TY_CLAUSE"));
                             D_PUSHSTACK(D_S_hilf,(int)A_CLAUSE(*new_desc,next) << 1);
                             heap_elems = 4;
                             D_PUSHSTACK(D_S_hilf,heap_elems);
                             GET_HEAP(heap_elems,A_CLAUSE(*new_desc,sons));
                             new_heap = R_CLAUSE(*new_desc,sons);
                             D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems-1) << 1) | 1);
                             break;
                           default:
                            DBUG_PRINT("MSG", ("unknown d_type")); break;
                           }
                         break;
                       case C_PATTERN:
                         DBUG_PRINT("MSG", ("C_PATTERN"));
                         L_PATTERN(*new_desc,following) = DO_RECEIVE();
                         D_PUSHSTACK(D_S_hilf,(int)A_PATTERN(*new_desc,pattern) << 1);
                         D_PUSHSTACK(D_S_hilf,(int)A_PATTERN(*new_desc,variable) << 1);
                         break;
		       case C_INTACT:
                         DBUG_PRINT("MSG", ("C_INTACT"));
                         L_INTACT(*new_desc,dim) = DO_RECEIVE();
			 D_PUSHSTACK(D_S_hilf,heap_elems = R_INTACT(*new_desc,dim));
			 GET_HEAP(heap_elems,A_INTACT(*new_desc,args));
			 new_heap = R_INTACT(*new_desc,args);
			 D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			 break;
                       case C_FRAME:
                          switch(new_type)
                          { case TY_FRAME:
                               DBUG_PRINT("MSG", ("C_FRAME/TY_FRAME"));
                               L_FRAME(*new_desc,dim) = DO_RECEIVE();
                               D_PUSHSTACK(D_S_hilf,heap_elems = R_FRAME(*new_desc,dim));
                               GET_HEAP(heap_elems,A_FRAME(*new_desc,slots));
                               new_heap = R_FRAME(*new_desc,slots);
                               D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
                               break;
                            case TY_SLOT:
                               DBUG_PRINT("MSG", ("C_FRAME/TY_SLOT"));
                               D_PUSHSTACK(D_S_hilf,(int)A_SLOT(*new_desc,name) << 1);
                               D_PUSHSTACK(D_S_hilf,(int)A_SLOT(*new_desc,value) << 1);
                               break;
                          }
                          break;
		       case C_MATRIX:
                         DBUG_PRINT("MSG", ("C_MATRIX"));
		       case C_VECTOR:
                         DBUG_PRINT("MSG", ("C_VECTOR"));
		       case C_TVECTOR:
                         DBUG_PRINT("MSG", ("C_TVECTOR ***"));
			 RECEIVE_2_SHORTS(L_MVT(*new_desc,nrows,new_class),L_MVT(*new_desc,ncols,new_class));
			 L_MVT(*new_desc,ptdd,new_class) = NULL;
			 if ((_formated == 1) && (new_type != TY_STRING))
			   {heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class));
			    if (new_type == TY_REAL)
			      heap_elems *= 2;}
			 else
			   D_PUSHSTACK(D_S_hilf,heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class)));
			 GET_HEAP(heap_elems,A_MVT(*new_desc,ptdv,new_class));
			 new_heap = R_MVT(*new_desc,ptdv,new_class);
			 if ((_formated == 1) && (new_type != TY_STRING))
			   for(;heap_elems >0;heap_elems--,new_heap++)
			     *new_heap = DO_RECEIVE();
			 else
			   D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems -1)) << 1) | 1);
			 break;
		       case C_EXPRESSION:
                         DBUG_PRINT("MSG", ("C_EXPRESSION"));
		       case C_CONSTANT:
                         DBUG_PRINT("MSG", ("C_CONSTANT ***"));
			 switch(new_type)
			   {case TY_REC:
                              DBUG_PRINT("MSG", ("TY_REC"));
			    case TY_ZF:
                              DBUG_PRINT("MSG", ("TY_ZF"));
			    case TY_SUB:
                              DBUG_PRINT("MSG", ("TY_SUB ***"));
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{
#if WITHTILDE
				  counter = DO_RECEIVE();
                                  DBUG_PRINT("MSG", ("heap_elems: %d, counter: %d", heap_elems, counter));
				  if (counter > 0)
				    D_PUSHSTACK(D_S_hilf,counter-2);
				  else
				    D_PUSHSTACK(D_S_hilf,heap_elems-1);
#else
				  if (heap_elems >0)
				    D_PUSHSTACK(D_S_hilf,heap_elems);
#endif
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,namelist));
				  new_heap = R_FUNC(*new_desc,namelist);
				  *new_heap = heap_elems;
#if WITHTILDE
				  *(++new_heap)=counter;
				  if (counter > 0)
				    {D_PUSHSTACK(D_S_hilf,((int)(new_heap + counter - 2) << 1) | 1);
				     for (;heap_elems>counter-1;heap_elems--)
				       *(new_heap + heap_elems - 1)=DO_RECEIVE();}
				  else
				    D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems - 1) << 1) | 1);
#else
				  if (heap_elems >0)
				    D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
#endif
				}
			      else
				L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
#if WITHTILDE
			      if (heap_elems >= 0)
				{
#endif
				  D_PUSHSTACK(D_S_hilf,heap_elems);
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
				  new_heap = R_FUNC(*new_desc,pte);
				  *new_heap = heap_elems;
				  D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
#if WITHTILDE
				}
			      else
				L_FUNC(*new_desc,pte) = NULL;
#endif
			      break;
#if WITHTILDE
			    case TY_SNSUB:
                              DBUG_PRINT("MSG", ("TY_SNSUB"));
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
			      new_heap = R_FUNC(*new_desc,pte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
#endif
			    case TY_ZFCODE:
                              DBUG_PRINT("MSG", ("TY_ZFCODE"));
			      RECEIVE_2_SHORTS(L_ZFCODE(*new_desc,zfbound),L_ZFCODE(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_ZFCODE(*new_desc,ptd) << 1);
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_ZFCODE(*new_desc,varnames));
			      new_heap = R_ZFCODE(*new_desc,varnames);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_COND:
                              DBUG_PRINT("MSG", ("TY_COND"));
			      L_COND(*new_desc,special) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptte));
			      new_heap = R_COND(*new_desc,ptte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptee));
			      new_heap = R_COND(*new_desc,ptee);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_VAR:
                              DBUG_PRINT("MSG", ("TY_VAR"));
			      L_VAR(*new_desc,nlabar) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_VAR(*new_desc,ptnd) << 1);
			      break;
			    case TY_EXPR:
                              DBUG_PRINT("MSG", ("TY_EXPR"));
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_EXPR(*new_desc,pte));
			      new_heap = R_EXPR(*new_desc,pte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NAME:
                              DBUG_PRINT("MSG", ("TY_NAME"));
			      heap_elems = DO_RECEIVE();
                              if (T_INT(heap_elems)) {
                                /* for description: see the sending of TY_NAME */ 
                                L_NAME(*new_desc,ptn) = (int) heap_elems;
                                DBUG_PRINT("MSG", ("It's an index: %d", VAL_INT(heap_elems)));
                              } else { 
                              heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_NAME(*new_desc,ptn));
			      new_heap = R_NAME(*new_desc,ptn);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
                              } 
			      break;
			    case TY_SWITCH:
                              DBUG_PRINT("MSG", ("TY_SWITCH"));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_SWITCH(*new_desc,nwhen),L_SWITCH(*new_desc,anz_args));
			      L_SWITCH(*new_desc,casetype) = DO_RECEIVE();
#else
			      RECEIVE_2_SHORTS(L_SWITCH(*new_desc,special),L_SWITCH(*new_desc,case_type));
			      L_SWITCH(*new_desc,nwhen) = DO_RECEIVE();
#endif
			      heap_elems = DO_RECEIVE();
			      GET_HEAP(heap_elems+1,A_SWITCH(*new_desc,ptse));
			      new_heap = R_SWITCH(*new_desc,ptse);
			      *new_heap = heap_elems;
#if WITHTILDE
			      *(new_heap+(heap_elems--)) = DO_RECEIVE();
#endif
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NOMAT:
                              DBUG_PRINT("MSG", ("TY_NOMAT"));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_NOMAT(*new_desc,act_nomat),L_NOMAT(*new_desc,reason));
#else
			      L_NOMAT(*new_desc,act_nomat) = DO_RECEIVE();
#endif
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_NOMAT(*new_desc,guard_body));
				 new_heap = R_NOMAT(*new_desc,guard_body);
				 *new_heap = heap_elems;
				 D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_NOMAT(*new_desc,guard_body) = NULL;
			      D_PUSHSTACK(D_S_hilf,(int)A_NOMAT(*new_desc,ptsdes) << 1);
			      break;
			    case TY_MATCH:
                              DBUG_PRINT("MSG", ("TY_MATCH"));
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,guard));
				 new_heap = R_MATCH(*new_desc,guard);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,guard) = NULL;
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,body));
				 new_heap = R_MATCH(*new_desc,body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,body) = NULL;
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{GET_HEAP(heap_elems+1,A_MATCH(*new_desc,code));
				 new_heap = R_MATCH(*new_desc,code);
				 *new_heap++ = heap_elems;
				 for (heap_counter = 0;heap_counter < heap_elems; heap_counter++)
				   {rec_data = DO_RECEIVE();
				    if (rec_data == PM_DESC_MARKER)
				      D_PUSHSTACK(D_S_hilf,((int)new_heap++) << 1);
				    else
				      *new_heap++ = rec_data;}}
			      else
				L_MATCH(*new_desc,code) = NULL;
			      break;
			    case TY_LREC:
                              DBUG_PRINT("MSG", ("TY_LREC"));
			      L_LREC(*new_desc,nfuncs) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,namelist));
			      new_heap = R_LREC(*new_desc,namelist);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_LREC(*new_desc,nfuncs));
#if WITHTILDE
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,ptdv));
#else
			      GET_HEAP(heap_elems,A_LREC(*new_desc,ptdv));
#endif
			      new_heap = R_LREC(*new_desc,ptdv);
#if WITHTILDE
			      *new_heap++ = R_LREC(*new_desc,nfuncs);
#endif
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_LREC_IND:
                              DBUG_PRINT("MSG", ("TY_LREC_IND"));
			      L_LREC_IND(*new_desc,index) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_IND(*new_desc,ptdd) << 1);
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_IND(*new_desc,ptf) << 1);
			      break;
			    case TY_LREC_ARGS:
                              DBUG_PRINT("MSG", ("TY_LREC_ARGS"));
			      L_LREC_ARGS(*new_desc,nargs) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_ARGS(*new_desc,ptdd) << 1);
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_LREC_ARGS(*new_desc,nargs));
			      GET_HEAP(heap_elems,A_LREC_ARGS(*new_desc,ptdv));
			      new_heap = R_LREC_ARGS(*new_desc,ptdv);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;}
			 break;
		       case C_FUNC:
                         DBUG_PRINT("MSG", ("C_FUNC"));
		       case C_CONS:
                         DBUG_PRINT("MSG", ("C_CONS ***"));
			 switch(new_type)
			   {case TY_CLOS:
                              DBUG_PRINT("MSG", ("TY_CLOS"));
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,args),L_CLOS(*new_desc,nargs));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,ftype),L_CLOS(*new_desc,nfv));
#else
			      L_CLOS(*new_desc,ftype) = DO_RECEIVE();
#endif
#if WITHTILDE
                              D_PUSHSTACK(D_S_hilf,heap_elems = R_CLOS(*new_desc,args) + R_CLOS(*new_desc,nfv) + 1);
#else
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_CLOS(*new_desc,args) + 1);
#endif
			      GET_HEAP(heap_elems,A_CLOS(*new_desc,pta));
			      new_heap = R_CLOS(*new_desc,pta);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_COMB:
                              DBUG_PRINT("MSG", ("TY_COMB"));
			      RECEIVE_2_SHORTS(L_COMB(*new_desc,args),L_COMB(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_COMB(*new_desc,ptd) << 1);
			      if ((type != MT_NEW_GRAPH) || code_received)
				L_COMB(*new_desc,ptc) = (PTR_HEAPELEM)(GET_CODE_START() + DO_RECEIVE());
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_COMB(*new_desc,ptc));
				 new_heap = R_COMB(*new_desc,ptc);
				 goto receive_code;}
			      break;
			    case TY_CONDI:
                              DBUG_PRINT("MSG", ("TY_CONDI"));
			      RECEIVE_2_SHORTS(L_CONDI(*new_desc,args),L_CONDI(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_CONDI(*new_desc,ptd) << 1);
			      if ((type != MT_NEW_GRAPH) || code_received)
				L_CONDI(*new_desc,ptc) = (PTR_HEAPELEM)GET_CODE_START() + DO_RECEIVE();
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_CONDI(*new_desc,ptc));
				 new_heap = R_CONDI(*new_desc,ptc);
				 goto receive_code;}
			      break;
			    case TY_CONS:
                              DBUG_PRINT("MSG", ("TY_CONS"));
			      D_PUSHSTACK(D_S_hilf,(int)A_CONS(*new_desc,hd) << 1);
			      D_PUSHSTACK(D_S_hilf,(int)A_CONS(*new_desc,tl) << 1);
			      break;
			    case TY_CASE:
                              DBUG_PRINT("MSG", ("TY_CASE"));
			    case TY_WHEN:
                              DBUG_PRINT("MSG", ("TY_WHEN"));
			    case TY_GUARD:
                              DBUG_PRINT("MSG", ("TY_GUARD"));
			    case TY_BODY:
                              DBUG_PRINT("MSG", ("TY_BODY ***"));
			      RECEIVE_2_SHORTS(L_CASE(*new_desc,args),L_CASE(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_CASE(*new_desc,ptd) << 1);
                              heap_elems = DO_RECEIVE();
                              if (T_INT(heap_elems)) { /* it's NO code-pointer !! */
                                DBUG_PRINT("MSG", ("no Code pointer ! Instead: %d", VAL_INT((int) heap_elems)));
                                L_CASE(*new_desc,ptc) = heap_elems;
                                break;
                                }
			      if ((type != MT_NEW_GRAPH) || code_received) {
                                DBUG_PRINT("MSG", ("receiving code-index..."));
				L_CASE(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE(); }
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_CASE(*new_desc,ptc));
				 new_heap = R_CASE(*new_desc,ptc);
			       receive_code:
				 code_address = new_heap;
                                 DBUG_PRINT("MSG", ("receiving Code..."));
				 for (heap_counter = 1;heap_counter < heap_elems; heap_counter ++)
				   {*new_heap = DO_RECEIVE();
				    rec_instr = instr_tab[*new_heap++];
				    switch(rec_instr.paramtype)
				      {case NUM_PAR:
					 /* Here is our special case again, see msg_send */
					 if ((rec_instr.instruction == I_DIST) || (rec_instr.instruction == I_DISTB))
					   {*(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					    *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					    *(new_heap++) = DO_RECEIVE();
					    *(new_heap++) = DO_RECEIVE();
#if WITHTILDE
					    *(new_heap++) = DO_RECEIVE();
					    *(new_heap++) = DO_RECEIVE();
					    heap_counter += 6;
#else
					    heap_counter += 4;
#endif
					    break;}

#if D_SPEC
                                         if (rec_instr.instruction == I_SPECDIST)
                                           {*(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                            *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                            *(new_heap++) = DO_RECEIVE();
                                            *(new_heap++) = DO_RECEIVE();
                                            heap_counter += 4;
                                            break;}
#endif

                                  /* special treatment for some pattern matching */
                                  /* instructions */

                                  if ((rec_instr.instruction >= I_APPEND) && (rec_instr.instruction <= I_MATCHDIGIT)) {
                                    switch(rec_instr.instruction) {
                                      case I_ATEND:
                                      case I_ATSTART:
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));
                                        *(new_heap++) = DO_RECEIVE();
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        heap_counter += 2;
                                        break;
                                      case I_MATCHARB:
                                      case I_MATCHARBS:
                                      case I_MATCHBOOL:
                                      case I_MATCHC:
                                      case I_MATCHIN:
                                      case I_MATCHINT:
                                      case I_MATCHLIST:
                                      case I_MATCHPRIM:
                                      case I_MATCHSTR:
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));
                                        *(new_heap++) = DO_RECEIVE();
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        heap_counter += 3;
                                        break;
                                      case I_MATCHREAL:
                                      case I_MATCHDIGIT:
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction)); 
                                        D_PUSHSTACK(D_S_hilf,(int)(new_heap++) << 1);
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        heap_counter += 3;
                                        break;
                                      case I_TGUARD:
                                        DBUG_PRINT("MSG", ("converting TGUARD PM instruction..."));
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        D_PUSHSTACK(D_S_hilf,(int)(new_heap++) << 1);
                                        *(new_heap++) = DO_RECEIVE();
                                        *(new_heap++) = DO_RECEIVE();
                                        *(new_heap++) = DO_RECEIVE();
                                        *(new_heap++) = DO_RECEIVE();
                                        heap_counter += 6;
                                        break;
                                      case I_MATCHTRUE:
                                      case I_MATCHFALSE:
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                        heap_counter += 2;
                                        break;
                                      default:
                                        for (param_counter = 0;param_counter < rec_instr.nparams; param_counter++)
                                          {*(new_heap++) = DO_RECEIVE();
                                            heap_counter ++;}
                                        break;
                                      }
                                    break;
                                    }

				       case ATOM_PAR:
				       case FUNC_PAR:
					 for (param_counter = 0;param_counter < rec_instr.nparams;param_counter++)
					   {*(new_heap++) = DO_RECEIVE();
					    heap_counter ++;}
					 break;
				       case DESC_PAR:
					 D_PUSHSTACK(D_S_hilf,(int)(new_heap++) << 1);
					 heap_counter++;
					 break;
				       case ADDR_PAR:
					 *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					 heap_counter++;
					 break;
				       }                                  /* switch */
				  }                                       /* for */
				 *new_heap = I_SYMTAB;
				 code_received = 1;
				 SET_CODE_START(code_address);
			       }                                          /* else {code not received} */
			      break;
			    }                                             /* switch (new_type) */
			 break;
		       }                                                  /* switch (new_class) */
		    received_list[received_index++] = new_desc;
		    if (received_index == rec_list_size)
		      {rec_list_size += 256;
		       received_list = (T_PTD *)realloc(received_list,rec_list_size * sizeof(T_PTD));
/* fprintf(debugger, "rec_list_size: %d\n", rec_list_size); fflush(debugger); */
		       if (!received_list)
			 post_mortem("insufficient memory for received_list");}
		  }                                                       /* else {descriptor not received yet} */
		}                                                       /* else {null pointer} */
	     }                                                          /* else {not in static heap} */
	  }                                                             /* else {extended pointer on stack} */
       }                                                                /* while */
#if MSG_CHKSUM
      rec_data = DO_RECEIVE();
      if (rec_data*2 != chksum) {
	post_mortem("Checksum error");
      }
#endif


      rec_data = DO_RECEIVE();
      if (rec_data != END_MARKER)
	post_mortem("END_MARKER expected but not found");
      DBUG_PRINT("MSG", ("END_MARKER found..."));
#if nCUBE

/* fprintf(debugger, "Gebe buffer frei: %x (GRAPH)!\n", buffer); fflush(debugger); */

      nrelp((char *)buffer);
#endif /* nCUBE */
      free(received_list);


  rel_heap();
  _heap_reserved = FALSE;

/* some additional speculative stuff */

#if D_SPEC

  if (new_pc == rout_specmap2) {
    DBUG_PRINT("MSG", ("OK, NOW switch the stacks !"));
    new_pid->switched = new_pid->switched | W_A_SWITCHED;
    } 

#if D_SPEC_TICKETS
  if ((spec_flag >0) && (type == MT_RESULT)) {
    DBUG_PRINT("MSG", ("copy speculative result to the appropriate destination..., spec_father_slot: %x", spec_father_slot));
    SPEC_SF_FIRSTF(*(PTR_DESCRIPTOR)spec_father_slot) = ((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_father_slot))->nesting;
    ((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_father_slot))->nesting = spec_help;

    DBUG_PRINT("MSG", ("entries for 0x%x: result 0x%x steps %d status 0x%x", spec_father_slot, SPEC_SF_FIRSTF(*(PTR_DESCRIPTOR)spec_father_slot), SPEC_SF_REDSTEPS(*(PTR_DESCRIPTOR)spec_father_slot), SPEC_SF_PSTATUS(*(PTR_DESCRIPTOR)spec_father_slot)));
    }
#endif

#endif /* D_SPEC */

      if (type == MT_NEW_GRAPH)
	{SET_STATIC_BORDER(highdesc);
	 DBUG_PRINT("STATHEAP",("new end of static heap: %08x",highdesc));
	 if (proc_id == 0)
	   PC_SET_PC(new_pid,R_COMB(*(T_PTD)D_READSTACK(PC_GET_H_STACK(new_pid)),ptc));
	 else
	   D_PPOPSTACK(D_S_hilf);}
      break;

 case MT_RESULT:

/*    fprintf(debugger, "0x%x: receiving MT_RESULT from 0x%x\n", curr_pid, src); fflush(debugger);      */

      /* initialize_fred_fish();   */

     DBUG_PRINT ("PVM", ("MT_RESULT processing..."));

#ifdef D_MESS_ONLY 
  m_type = "MT_RESULT";
#endif 

      rec_pid = (PROCESS_CONTEXT *)DO_RECEIVE();

      DBUG_PRINT("MSG", ("rec_pid = %x", rec_pid));

#ifdef D_MESS_ONLY
  f_pid = (int) rec_pid;
#endif

      DBUG_PRINT("MSG",("receiving result for process %x from %d",rec_pid,src));
      a_stack_addr = (STACKELEM *)DO_RECEIVE();
      DBUG_PRINT("MSG",("w_stack_addr: %x",a_stack_addr));
      h_stack_addr = (STACKELEM *)DO_RECEIVE();

#if (!D_SPEC)
      if (rec_pid == curr_pid)
	_redcnt -= DO_RECEIVE();
      else
	PC_SET_REDCNT(rec_pid,PC_GET_REDCNT(rec_pid) - DO_RECEIVE());

#else /* D_SPEC */
      spec_tmp3 = DO_RECEIVE(); /* this is the number of used red-steps */

      spec_flag = DO_RECEIVE(); /* this is 1 if it was a speculative result */
      spec_father_slot = DO_RECEIVE(); /* this is the slotnumber in the father's process context */

      DBUG_PRINT("MSG", ("spec_father_slot is 0x%x", spec_father_slot));

#if D_SPEC_TICKETS
      spec_help = ((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_father_slot))->nesting;
      a_stack_addr = &((PROCESS_CONTEXT *)SPEC_SF_PROCESS(*(PTR_DESCRIPTOR)spec_father_slot))->nesting;
#endif

      spec_tmp2 = DO_RECEIVE(); /* this is the source pid, if it's spec */
      if (spec_flag>0)  { /* it's a result of a speculative process */

      DBUG_PRINT("MSG", ("analysing status information of son process: %x", spec_flag));
      if (T_S_WINNER(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_WINNER"));
        }
      if (T_S_NO_STEPS(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_NO_STEPS"));
        }
      if (T_S_FAIL(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_FAIL"));
        }
      if (T_S_NO_BSTEPS(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_NO_BSTEPS"));
        }
      if (T_S_GUARD(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_GUARD"));
        }
      if (T_S_GUARD_SON(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_GUARD_SON"));
        }
      if (T_S_INSYNC(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... INSYNC"));
        }
      if (T_S_UNRED_GUARD(spec_flag)==1) {
        DBUG_PRINT("MSG", ("flags... SPEC_UNRED_GUARD"));
        }

/* fprintf(debugger, "receive: speculative process, T_WINNER %d, T_GRANDDAD %d\n", T_SPEC_WINNER(rec_pid), T_SPEC_GRANDDAD(rec_pid)); fflush(debugger);  */

#if D_SPEC_TICKETS
        DBUG_PRINT("MSG", ("receiving result for descriptor %x !", spec_father_slot));
#else
        DBUG_PRINT("MSG", ("receiving result for slot %d !", spec_father_slot));
#endif

#if D_SPEC_TICKETS
        DBUG_PRINT("MSG", ("counter for ALL results was %d", *(int*)SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot)));
        (*(int*)SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot))--;

/* fprintf(debugger, "0x%x: MESSAGE, COUNTER %x is now down to %d !\n", curr_pid, SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot), *(int*)SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot)); fflush(debugger); */

        DBUG_PRINT("MSG", ("counter for ALL results IS %d", *(int*)SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot)));
#else
        PC_SET_SPEC_SUM(rec_pid, PC_GET_SPEC_SUM(rec_pid)+1);

        DBUG_PRINT("MSG", ("results received: %d", PC_GET_SPEC_SUM(rec_pid)));
#endif

         /*  PC_SET_SPEC_FOLD(rec_pid); */ /* later ! If the process starts again ! */

/*          DBUG_PRINT("MSG", ("rec_pid: %x, rec_pid->son_status: %x", rec_pid, rec_pid->son_status)); */

#if D_SPEC_TICKETS
         SPEC_SF_PSTATUS(*(PTR_DESCRIPTOR)spec_father_slot) = spec_flag;
#else
         rec_pid->son_status[spec_father_slot] = spec_flag;
#endif

         DBUG_PRINT("MSG", ("status was stored... spec_flag is %x", spec_flag));

         if (T_S_GUARD_SON(spec_flag)==1) {

/* fprintf(debugger, "T_S_GUARD_SON set..., spec_tmp3=%d\n", spec_tmp3); fflush(debugger); */

           DBUG_PRINT("MSG", ("don't store reduction steps, GUARD_SON was set..., steps used: %d", spec_tmp3));
           if (spec_tmp3 != 0) {
             post_mortem("guard-terminated speculative process: redsteps ne 0 !"); }

           } else {
           DBUG_PRINT("MSG", ("store reduction steps..."));
#if D_SPEC_TICKETS
           SPEC_SF_REDSTEPS(*(PTR_DESCRIPTOR)spec_father_slot) = spec_tmp3;
#else
           rec_pid->son_redcnts[spec_father_slot] = spec_tmp3;
#endif
           }

         DBUG_PRINT("MSG", ("speculative \"result\": steps used=%d, status=%x, steps of the father=%d", spec_tmp3, spec_flag, rec_pid->red_count));

/*  fprintf (debugger, "receive: pid %x has %d steps left, %d were used by the son.\n", rec_pid, spec_tmp1, spec_tmp3); fflush(debugger);  */

        }
      else { /* THIS CODE IS COPIED FROM ABOVE */

/*  fprintf (debugger, "receive: pid %x is vital\n", rec_pid);  */

        if (rec_pid == curr_pid)
          _redcnt -= spec_tmp3;
        else
          PC_SET_REDCNT(rec_pid,PC_GET_REDCNT(rec_pid) - spec_tmp3);
        }
#endif /* D_SPEC */

#if ADV_SCHED
      if (!have_received_results) {
        have_received_results = init_tickets_cond = tickets_cond = 1;
        }
#endif /* ADV_SCHED */

      DBUG_PRINT("MSG", ("h_stack_addr %x, value %d", h_stack_addr, (*(int *)h_stack_addr)));

#if D_SPEC
      if (!spec_flag) {
        DBUG_PRINT("MSG", ("no speculative result..."));
#endif /* D_SPEC */

#if(!D_NN_SCHED)
      ticket_pool[src]++;
      ticket_pool_counter++;
#endif

/* fprintf(debugger, "no speculative result... wakeup...\n"); fflush(debugger); */

        DBUG_PRINT("MSG", ("rec_pid = %x, PC_STATUS(rec_pid) = %d, (PC_STATUS(rec_pid) == PS_WAITING) = %d", rec_pid, PC_STATUS(rec_pid), (PC_STATUS(rec_pid) == PS_WAITING)));
        
        if ((--(*(int *)h_stack_addr) == 0) && (PC_STATUS(rec_pid) == PS_WAITING))
#if (D_SLAVE && D_MESS && D_MSCHED)
          (*d_m_process_wakeup)(rec_pid);
#else
          process_wakeup(rec_pid);
#endif
#if D_SPEC
        } else {

#if D_SPEC_TICKETS
      spec_ticket_pool[src]++;
      spec_ticket_pool_counter++; 

/* fprintf(debugger, "0x%x: *RECEIVE* src: %d, spec_ticket_pool[src]: %d, spec_ticket_pool_counter: %d\n", curr_pid, src, spec_ticket_pool[src], spec_ticket_pool_counter);  */
/* fprintf(debugger, "0x%x: *RECEIVE* spec_ticket_pool[0] = %d, spec_ticket_pool[1] = %d, spec_ticket_pool[2] = %d, spec_ticket_pool[3] = %d\n", curr_pid, spec_ticket_pool[0], spec_ticket_pool[1], spec_ticket_pool[2], spec_ticket_pool[3]); fflush(debugger); */

      spec_control[0] = query_postpone();
      if (spec_control[0] != NULL) {
        DBUG_PRINT("MSG", ("ticket returned, some postponed expressions found..."));
        spec_sched_status = 11; /* post-process */
        }

      if (*(int*)SPEC_SF_ADDRCOUNT(*(PTR_DESCRIPTOR)spec_father_slot) == 0) {
        DBUG_PRINT("MSG", ("all speculative results have been gathered !"));

/* fprintf(debugger, "0x%x: MESSAGE, waking up suspended process, counter is down to 0.\n", curr_pid); fflush(debugger); */
         
        /* wake up the corresponding process */

#if (D_SLAVE && D_MESS && D_MSCHED)
          (*d_m_process_wakeup)(rec_pid);
#else
          process_wakeup(rec_pid);
#endif

        PC_CLEAR_SPEC_WAIT_WIN(rec_pid); /* clear GOON block */

        /* copy address of where to continue into the process context */

        rec_pid->pc = SPEC_SF_ASMMKNC(*(PTR_DESCRIPTOR)spec_father_slot);
        }

#else

        /* check here whether all results has been collected ! */
 
        DBUG_PRINT("MSG", ("SUM is %d, SPAWNED is %d", PC_GET_SPEC_SUM(rec_pid), PC_GET_SPEC_SPAWNED(rec_pid)));

        if (PC_GET_SPEC_SUM(rec_pid) == PC_GET_SPEC_SPAWNED(rec_pid)) {
          DBUG_PRINT("MSG", ("ALL results have been gathered ! Restart process !"));

#if (D_SLAVE && D_MESS && D_MSCHED)
          (*d_m_process_wakeup)(rec_pid);
#else
          process_wakeup(rec_pid);
#endif

          }
#endif /* D_SPEC_TICKETS */

        }
#endif /* D_SPEC */
 
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      D_PUSHSTACK(D_S_hilf,(int)a_stack_addr << 1);

#if ADV_SCHED

      /* check for suspend conditions */
      if (tickets_cond && curr_pid && ready_queue_first && (ticket_pool_counter >= 2)) {   
                                        /* restricted ticket handling is enabled */
                                        /* and there's a process running */
        jc=0;
        for (ic=0;ic<1<<cube_dim;ic++) {
          if (ticket_pool[ic]>0)
            jc++;
          if (jc >= 2)
            continue;
          } 
        /* setting sig_interrupt might be useless, but to be sure, let's set ;-) */
        sig_interrupt = sig_suspend = (jc >= 2) ? 1 : 0;

/*     fprintf (stderr, "insgesamt: %d, sig_suspend: %d\n", jc, sig_suspend); fflush(stderr); */

#if D_MESS
    /* counting the activation of white processes 'cause of another suspended process */
    if (sig_suspend) no_suspended++;
#endif /* D_MESS */
        }
#endif /* ADV_SCHED */
      goto receive_loop;

    case MT_NEW_GRAPH:

/*  fprintf(debugger, "0x%x: receiving MT_NEW_GRAPH from 0x%x\n", curr_pid, src); fflush(debugger);     */

     DBUG_PRINT ("PVM", ("MT_NEW_GRAPH processing..."));

#ifdef D_MESS_ONLY 
  m_type = "MT_NEW_GRAPH";
#endif 

      new_redcnt = DO_RECEIVE();

#if D_SPEC
      real_start_redcount = new_redcnt;
#endif

      if (proc_id==0)
	{
#if D_SPEC /* set global spec_variable to zero */
         spec_global = 0;
#endif

#if (D_SLAVE && D_MESS && D_MSCHED)
         new_pid = (*d_m_process_create)(0,-1,new_redcnt);
#else
         new_pid = process_create(0,-1,new_redcnt);
#endif

#if ADV_SCHED
         new_pid->start_letpar_level = 0;
         new_pid->letpar_level = 0;
         DBUG_PRINT ("ADV_SCHED", ("Initializing letpar_levels (0,0)..."));
#endif /* ADV_SCHED */

#if D_MESS_ONLY
         f_pid = (int) new_pid;
#endif

	 D_PUSHSTACK(PC_GET_H_STACK(new_pid),ST_DUMMY);
	 h_stack_addr = (STACKELEM *)D_TOP_ADDRESS(PC_GET_H_STACK(new_pid));}
      else
	{D_PUSHSTACK(D_S_hilf,ST_DUMMY);
	 h_stack_addr = (STACKELEM *)D_TOP_ADDRESS(D_S_hilf);}
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      D_PUSHSTACK(D_S_hilf,(int)h_stack_addr << 1);
      goto receive_loop;

 case MT_PARAM:

/*   fprintf(debugger, "0x%x: receiving MT_PARAM from 0x%x\n", curr_pid, src); fflush(debugger);   */

DBUG_PRINT ("PVM", ("MT_PARAM processing..."));

#if D_MESS
#if nCUBE
     sigaction (SIGMSGS,&act,NULL);   /* reinstall signal handler ... */
#endif
     sig_msg_flag = 0;
#if (ADV_SCHED || D_SPEC)
     sig_interrupt = 0;
#endif
#if ADV_SCHED
     have_received_results = 0;
     init_tickets_cond = 0;
     tickets_cond = 0;
#endif /* ADV_SCHED */
     m_curr_pid = -1;
#endif

#ifdef D_MESS_ONLY 
  m_type = "MT_PARAM";
#endif 

  _prec_mult        = DO_RECEIVE();
  _prec             = DO_RECEIVE();
  _prec10           = DO_RECEIVE();
  _formated         = DO_RECEIVE();
  LazyLists         = DO_RECEIVE();
  _digit_recycling  = DO_RECEIVE();
  _beta_count_only  = DO_RECEIVE();
  _trunc            = DO_RECEIVE();
  _count_reductions = DO_RECEIVE();
  _maxnum           = DO_RECEIVE();
  _base             = DO_RECEIVE();
  hs                = DO_RECEIVE();
  hd                = DO_RECEIVE();
  ss                = DO_RECEIVE();
  stack_seg_size    = DO_RECEIVE();
  d_sort_in         = DO_RECEIVE();

#if D_SPEC
  ack_red_count = DO_RECEIVE();
  max_spec_sons = DO_RECEIVE();
  max_spec_procs = DO_RECEIVE();
  spec_failcase = DO_RECEIVE();
  spec_multi_results = DO_RECEIVE();
  spec_mspecial = DO_RECEIVE();
  spec_kill_no_steps = DO_RECEIVE();
  spec_nondet = DO_RECEIVE();
  nospecnr = DO_RECEIVE();
  spec_retrans = DO_RECEIVE();
#endif /* D_SPEC */
   
#if D_MESS
      m_ackno           = DO_RECEIVE();
      m_merge_strat     = DO_RECEIVE();
      d_bib_mask        = DO_RECEIVE();
      DBUG_PRINT ("MSG", ("received real bitmask: %x", d_bib_mask));
#if nCUBE
      strcpy((char *)m_mesfilehea, (char *)buffer_p);
      strcpy(m_mesfilepath, ((char *)buffer_p)+strlen(m_mesfilehea)+1);
      buffer_p += ((int)(strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/(int)4)+1;
#endif
#if D_PVM
      pvm_upkstr(m_mesfilehea); 
      pvm_upkstr(m_mesfilepath); 
#endif
      DBUG_PRINT("MSG", ("m_mesfilehea: %s, strlen: %d, m_mesfilepath: %s, strlen: %d, int's: %d", m_mesfilehea, strlen(m_mesfilehea), \
m_mesfilepath, strlen(m_mesfilepath), ((int)(strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/(int)4)+1)); 

      d_init_mess(proc_id);
#endif

#if D_PVM
      DBUG_PRINT ("PVM", ("PARAMs received..."));
#endif  /* D_PVM */

      InitHeapManagement(hs,hd);
      alloc_stack_memory(ss);
      init_d_stack(&D_S_hilf);
      strcpy(D_S_hilf.name,"hilf");
      lookup_next = first_lookup;

#if D_SPEC_TICKETS
      spec_lookup_next = spec_first_lookup;
      DBUG_PRINT("MSG", ("spec_lookup_next set to %d", spec_lookup_next));
#endif

#if D_MESS

#if D_SPEC
#if (D_PVM && !D_PVM_NCUBE)
#if D_SPEC_TICKETS
      MPRINT_SPECIAL(D_MESS_FILE, M_TIMER, PROCESS_NR, -1, (spec_mspecial == 0) ? proc_tab_size+spec_proc_tab_size : proc_tab_size+spec_mspecial, m_base);
#else
      MPRINT_SPECIAL(D_MESS_FILE, M_TIMER, PROCESS_NR, -1, (spec_mspecial == 0) ? proc_tab_size : spec_mspecial, m_base);
#endif
#else
#if D_SPEC_TICKETS
      MPRINT_SPECIAL(D_MESS_FILE, 0.0, PROCESS_NR, -1, (spec_mspecial == 0) ? proc_tab_size+spec_proc_tab_size : proc_tab_size+spec_mspecial, m_base);
#else
      MPRINT_SPECIAL(D_MESS_FILE, 0.0, PROCESS_NR, -1, (spec_mspecial == 0) ? proc_tab_size : spec_mspecial, m_base);
#endif
#endif /* D_PVM */

#else /* D_SPEC */

#if (D_PVM && !D_PVM_NCUBE)
      MPRINT_SPECIAL(D_MESS_FILE, M_TIMER, PROCESS_NR, -1, proc_tab_size, m_base);
#else
      MPRINT_SPECIAL(D_MESS_FILE, 0.0, PROCESS_NR, -1, proc_tab_size, m_base);
#endif /* D_PVM */

#endif /* D_SPEC */

/* #if D_MHEAP  leave this out for measurements check */

      if (d_bib_mask & (1 << MINDEX_HEAP_ALLOC)) {
        DBUG_PRINT ("MSG", ("HEAP_ALLOC measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_newheap = m_newheap; }
      else 
        d_m_newheap = newheap;

      if (d_bib_mask & (1 << MINDEX_HEAP_FREE)) {
        DBUG_PRINT ("MSG", ("HEAP_FREE measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_freeheap = m_freeheap; }
      else
        d_m_freeheap = freeheap;

      if (d_bib_mask & (1 << MINDEX_HEAP_COMPACT)) {
        DBUG_PRINT ("MSG", ("HEAP_COMPACT measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_compheap = m_compheap; }
      else
        d_m_compheap = compheap;

      if (d_bib_mask & (1 << MINDEX_DESC_ALLOC)) {
        DBUG_PRINT ("MSG", ("DESC_ALLOC measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_newdesc = m_newdesc; }
      else
        d_m_newdesc = newdesc;

      if (d_bib_mask & (1 << MINDEX_DESC_FREE)) {
        DBUG_PRINT ("MSG", ("DESC_FREE measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_freedesc = m_freedesc; }
      else
        d_m_freedesc = freedesc;

/* #endif  D_MHEAP */ 

/* #if D_MSTACK */

      if (d_bib_mask & (1 << MINDEX_STACK_SEG_ALLOC)) {
        DBUG_PRINT ("MSG", ("STACK SEG ALLOC measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_allocate_d_stack_segment = m_allocate_d_stack_segment; }
      else
        d_m_allocate_d_stack_segment = allocate_d_stack_segment;

      if (d_bib_mask & (1 << MINDEX_STACK_SEG_FREE)) {
        DBUG_PRINT ("MSG", ("STACK SEG FREE measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_free_d_stack_segment = m_free_d_stack_segment; }
      else
        d_m_free_d_stack_segment = free_d_stack_segment;

      if (d_bib_mask & (1 << MINDEX_STACK_PUSH)) {
        DBUG_PRINT ("MSG", ("STACK PUSH measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_stack_push = m_stack_push; }
      else
        d_m_stack_push = 0;
 
      if (d_bib_mask & (1 << MINDEX_STACK_POP)) {
        DBUG_PRINT ("MSG", ("STACK POP measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_stack_pop = m_stack_pop; }
      else
        d_m_stack_pop = 0;

/* #endif D_MSTACK */

/* #if D_MSCHED */

      if (d_bib_mask & (1 << MINDEX_PROC_DIST)) {
        DBUG_PRINT ("MSG", ("PROC DIST measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_distribute = m_distribute; }
      else
        d_m_distribute = distribute;

      if (d_bib_mask & (1 << MINDEX_PROC_CREATE)) {
        DBUG_PRINT ("MSG", ("PROC CREATE measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_create = m_process_create; }
      else
        d_m_process_create = process_create;

      if (d_bib_mask & (1 << MINDEX_PROC_TERMINATE)) {
        DBUG_PRINT ("MSG", ("PROC TERMINATE measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_terminate = m_process_terminate; }
      else
        d_m_process_terminate = process_terminate;

      if (d_bib_mask & (1 << MINDEX_PROC_SLEEP)) {
        DBUG_PRINT ("MSG", ("PROC SLEEP measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_sleep = m_process_sleep; }
      else
        d_m_process_sleep = process_sleep;

      if (d_bib_mask & (1 << MINDEX_PROC_RUN)) {
        DBUG_PRINT ("MSG", ("PROC RUN measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_run = m_process_run; }
      else
        d_m_process_run = process_run; 

      if (d_bib_mask & (1 << MINDEX_PROC_WAKEUP)) {
        DBUG_PRINT ("MSG", ("PROC WAKEUP measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_wakeup = m_process_wakeup; }
      else
        d_m_process_wakeup = process_wakeup; 

/* #endif  D_MSCHED */

/* #if D_MCOMMU */
 
      d_m_msg_send = msg_send;
      d_m_msg_receive = msg_receive;

      if (d_bib_mask & (1 << MINDEX_SEND_BEGIN)) {
        DBUG_PRINT ("MSG", ("SEND BEGIN measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_send = m_msg_send; 
        m_msg_send_begin = 1; }
      else
        m_msg_send_begin = 0;

      if (d_bib_mask & (1 << MINDEX_SEND_END)) {
        DBUG_PRINT ("MSG", ("SEND END measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_send = m_msg_send; 
        m_msg_send_end = 1; }
      else
        m_msg_send_end = 0;

      if (d_bib_mask & (1 << MINDEX_RECEIVE_BEGIN)) {
        DBUG_PRINT ("MSG", ("RECEIVE BEGIN measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_receive = m_msg_receive; 
        m_msg_receive_begin = 1; }
      else
        m_msg_receive_begin = 0;

      if (d_bib_mask & (1 << MINDEX_RECEIVE_END)) {
        DBUG_PRINT ("MSG", ("RECEIVE END measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_receive = m_msg_receive; 
        m_msg_receive_end = 1; }
      else
        m_msg_receive_end = 0;

/* #endif  D_MCOMMU */ 

#endif

      if (DO_RECEIVE() != END_MARKER) {
        DBUG_PRINT ("MSG", ("END_MARKER expected but not found..."));
        post_mortem("* END_MARKER expected but not found...");
	/* error !!! */;}


      break;

#if D_MESS
    case MT_CLOSE_MEASURE:

/*******************************/

initialize_fred_fish();


/*  fprintf(debugger, "receiving MT_CLOSE_MEASURE\n"); fflush(debugger);  */

      DBUG_PRINT ("MSG", ("Receiving MT_CLOSE_MEASURE !"));

      /* switch off signal handler */

#if (nCUBE || D_PVM_NCUBE)
      if (!m_merge_strat)
        sigaction (SIGMSGS,NULL,NULL);
#endif

      /* close measure file */

      if (fclose (d_mess_dat) == EOF)
        fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);
 
      /* turn off all measure operations */

      d_m_newheap = newheap;
      d_m_freeheap = freeheap;
      d_m_compheap = compheap;
      d_m_newdesc = newdesc;
      d_m_freedesc = freedesc;
      d_m_allocate_d_stack_segment = allocate_d_stack_segment;
      d_m_free_d_stack_segment = free_d_stack_segment;
      d_m_stack_push = 0;
      d_m_stack_pop = 0;
      d_m_distribute = distribute;
      d_m_process_create = process_create;
      d_m_process_terminate = process_terminate;
      d_m_process_sleep = process_sleep;
      d_m_process_run = process_run;
      d_m_process_wakeup = process_wakeup;
      d_m_msg_send = msg_send;
      d_m_msg_receive = msg_receive;
      m_msg_send_begin = 0;
      m_msg_send_end = 0;
      m_msg_receive_begin = 0;
      m_msg_receive_end = 0;

      sleep(1);    /* wait for one second */

      if (proc_id == 0) {       /* i am no 0 ! */
        k = 0;
        for (i = 0; i < (1 << cube_dim); i++) {
          sprintf (name_line, "%s%s_uid%ld_%i", m_mesfilepath, m_mesfilehea, getuid(), i);         
          if (stat(name_line, &stbuf) == -1)
            fprintf(stderr, "node %d: warning, cannot access measurement file %d !\n", proc_id, i);
          DBUG_PRINT ("MERGE", ("length of file %d: %ld", i, stbuf.st_size));
          if ((int) stbuf.st_size == 0)
            fprintf (stderr, "node %d: warning, file %d empty !\n\n", proc_id, i);
          k = k + (int) stbuf.st_size; }
        DBUG_PRINT ("MERGE", ("total length: %d", k));
#if nCUBE
        nwrite(&k, 4, host, MT_CLOSE_MEASURE, &flags); 
#endif
#if D_PVM
        pvm_initsend(pvmcoding);
        pvm_pkint(&k, 1, 1);
        pvm_send(host, MT_CLOSE_MEASURE);
#endif
        }

      DBUG_PRINT ("MSG", ("measure file closed and measure actions terminated."));

      break;

    case MT_REMOTE_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_REMOTE_MEASURE !"));

      /* copy measure file nr to host */

#if nCUBE
      m_super_nr = *((int *)buffer);
#endif
#if D_PVM
      pvm_upkint(&m_super_nr, 1, 1);
      pvm_upkstr(m_remote_host);
      pvm_upkstr(m_remote_dir);

      DBUG_PRINT("S_MERGE", ("copying measurement file to host %s", m_remote_host));
#else
 
      DBUG_PRINT ("S_MERGE", ("copying measurement file no %d", m_super_nr));
#endif

      sprintf (d_mess_name, "%s%s_uid%ld_%i", m_mesfilepath, m_mesfilehea, getuid(), m_super_nr);
      sprintf (m_super_name, "/tmp/%s_uid%ld_%i", m_mesfilehea, getuid(), m_super_nr);

      DBUG_PRINT("S_MERGE", ("filenames:  %s to %s", d_mess_name, m_super_name));

#if (D_PVM && !D_PVM_NCUBE)
      sprintf (m_line,"rcp %s %s:%s", d_mess_name, m_remote_host, m_remote_dir );
      DBUG_PRINT ("S_MERGE", ("executing the command: %s", m_line));

      if (system(m_line) != 0)
        DBUG_PRINT("S_MERGE", ("couldn't execute the command ! :-("));
      
#else
      if ((m_super_file1 = fopen(d_mess_name, "r")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open measurement file for reading\n\n", proc_id);
      if ((m_super_file2 = fopen(m_super_name, "w")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open host measurement node file for writing\n\n", proc_id);

      while (!feof(m_super_file1)) {
        m_super_counter = fread (&m_super_buffer, 1, 4096, m_super_file1);
        fwrite (&m_super_buffer, 1, m_super_counter, m_super_file2);
        }

/* HAHAHAHAH great Code
      while ((m_super_c = fgetc(m_super_file1)) != EOF)
        fputc(m_super_c, m_super_file2); */
  
      if (fclose(m_super_file1) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measurement file\n\n", proc_id);
      if (fclose(m_super_file2) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measurement node file\n\n", proc_id);
#endif /* D_PVM */

#if nCUBE
      nwrite (&m_super_nr, 0, host, MT_CLOSE_MEASURE, &flags);
#endif
#if D_PVM
      pvm_initsend(pvmcoding);
      pvm_send(host, MT_CLOSE_MEASURE);
#endif

      break;

    case MT_OPEN_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_OPEN_MEASURE !"));

#ifndef M_OLD_MERGING
#if !D_PVM
      m_start_merge (buffer_p);
#endif

      break; 

/****** old merging *****/

#else

      if (fclose (d_mess_dat) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

      if ((d_mess_dat = fopen (d_mess_name, "r")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

      i = 0;
      if (fgets(mess_line, 120, d_mess_dat) == NULL) {
        DBUG_PRINT ("MSG", ("Messdatei schon hier leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

        if (fclose (d_mess_dat) == EOF)
           fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

        /* now open File again for more reductions !!!!! */
        if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
          fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

        d_mess_buff[0] = '\n';
        d_mess_buff[1] = '\0';
        i = 2;
        }
      else {              /* now read measure-file until the buffer is full */
        DBUG_PRINT ("MSG", ("Hiho, hir bin ich..."));

        while ((i + (k = strlen(mess_line))) < (D_MESS_MAXBUF - 4)) {  /* space left for delimiters */ 
          DBUG_PRINT ("MSG", ("Reading: %s; i = %d; k = %d", mess_line, i, k));
          memcpy(d_mess_buff+i, mess_line, k);
          i += k;
          d_mess_buff[i] = '\0';

          if (fgets(mess_line, 120, d_mess_dat) == NULL) {
            DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

            if (fclose (d_mess_dat) == EOF)
               fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

            /* now open File again for more reductions !!!!! */
            if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
              fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

            d_mess_buff[i++] = '\n';
            d_mess_buff[i++] = '\0';
            break;
            }
          }
        }
      
      d_mess_buff[i++] = '\0';

      DBUG_PRINT ("MSG", ("Sending %d Bytes !, strlen: %d ", i, strlen(d_mess_buff)));

      nwrite (d_mess_buff, i, host, MT_SEND_MEASURE, &flags);  

      DBUG_PRINT ("MSG", ("Hab MT_SEND_MEASURE weggesendet !"));

/*
      fprintf (stderr, "Prozessor %d sendet auf ein MT_OPEN_MEASURE hin %d Bytes !\n", proc_id, strlen(d_mess_buff));
      fflush (stderr); */

#endif /* M_OLD_MERGING */

      break;

/**************** end of old merging ************/

    case MT_SEND_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_SEND_MEASURE !"));
      
#ifdef M_OLD_MERGING

/************** OLD MERGING **************/    /* for ASCII only ! */

      i = 0;
      k = strlen (mess_line);
      memcpy(d_mess_buff, mess_line, k);
      i += k;

      if (fgets(mess_line, 120, d_mess_dat) == NULL) {
        DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

        if (fclose (d_mess_dat) == EOF)
           fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

        /* now open File again for more reductions !!!!! */
        if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
          fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

        d_mess_buff[0] = '\n';
        d_mess_buff[1] = '\0';
        i = 2;
        }
      else {              /* now read measure-file until the buffer is full */
        i = 0;
        while ((i + (k = strlen(mess_line))) < (D_MESS_MAXBUF - 4)) {  /* space left for del
imiters */ 
          memcpy(d_mess_buff+i, mess_line, k); 
          i += k;
          if (fgets(mess_line, 120, d_mess_dat) == NULL) {
            DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

            if (fclose (d_mess_dat) == EOF)
              fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

            /* now open File again for more reductions !!!!! */
            if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
              fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

            d_mess_buff[i] = '\n';
            d_mess_buff[i+1] = '\0';
            i += 2;
            break;
            }
          }
        }
       
      d_mess_buff[i++] = '\0';

      DBUG_PRINT ("MSG", ("Sending %d Bytes !, strlen: %d ", i, strlen(d_mess_buff)));

      nwrite (d_mess_buff, i, host, MT_SEND_MEASURE, &flags);

      DBUG_PRINT ("MSG", ("Hab MT_SEND_MEASURE weggesendet !"));

/*     fprintf (stderr, "Prozessor %d sendet auf ein MT_SEND_MEASURE hin %d Bytes !\n", proc_id, strlen(d_mess_buff));
     fflush (stderr); */

#endif /* M_OLD_MERGING */

/****************** end of old merging ******************/

      break;
#endif
    }                                                                   /* switch (type) */

/*  fprintf(debugger, "**** leaving msg_receive, having received %s from %d,%x for %x\n", m_type, src, m_pid, f_pid); fflush(debugger); */

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_receive_end)
    MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, src, m_pid, m_size, f_pid);
#endif
#endif

 DBUG_VOID_RETURN;
}

#ifndef D_MESS_ONLY

/***************************************************************/
/*                                                             */
/* msg_check()    checks for incoming messages and calls       */
/*                msg_receive if a message has arrived         */
/*                                                             */
/* called by:  scheduler or signal handler                     */
/*                                                             */
/***************************************************************/

void msg_check()
{
int source=-1;
int type=-1;
/*dmk*/ int erg=0;

 DBUG_ENTER ("msg_check");

#if nCUBE
 /*DBUG_PRINT("MSG_CHECK", ("ntest result: %d", ntest(&source,&type)));*/
 source=type=-1;
 while ((erg = ntest(&source,&type))>=0)
#endif /* nCUBE */

#if D_PVM
 while ((erg = pvm_probe (-1, -1))>0)
#endif /* D_PVM */
   {
#ifdef D_NN_SCHED
     DBUG_PRINT("NN",("msg_check - erg : %d / type : %d " , erg, type));
     if (type == MT_STAT_REQ) {
       proc_org=-1;
       nn_msg_receive(MT_STAT_REQ);
       nn_msg_send(proc_org, MT_STAT_SEND);
     } else if (type == MT_STAT_SEND) {
       nn_msg_receive(MT_STAT_SEND);
     } else {
#endif
#if (D_SLAVE && D_MESS && D_MCOMMU)
        (*d_m_msg_receive)();
#else
        msg_receive();
#endif
#if D_SPEC
  if (spec_sched_status > 0) {
   speculative_scheduler();
   }
#endif /* D_SPEC */
#ifdef D_NN_SCHED
    }
#endif
    source=type=-1;
   } /* of while */
 sig_msg_flag = 0;
 DBUG_VOID_RETURN;
}

/***************************************************************/
/*                                                             */
/* flush_queue()    waits until a message of type MT_PARAM is  */
/*                  received; dismisses any other incoming     */
/*                  message                                    */
/*                                                             */
/*   callled in case of post_mortem                            */
/*                                                             */
/***************************************************************/

void flush_queue()
{char *buffer;
#if D_PVM
 int pvm_bufid;
 int pvm_length;
#endif /* D_PVM */
 int src,type,flags;

 DBUG_ENTER("flush_queue");

 for(;;)
   {do
      src = type = -1;
#if nCUBE
    while (ntest(&src,&type) < 0);
#endif /* nCUBE */

#if D_PVM
    while ((pvm_bufid = pvm_probe (-1, -1)) > 0);
    pvm_bufinfo (pvm_bufid, &pvm_length, &type, &src);
#endif /* D_PVM */
    
    if ((type == MT_PARAM) || (type == MT_TERMINATE))
      {sig_msg_flag = 1;
       DBUG_VOID_RETURN;}
    
#if nCUBE
    nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags);

/* fprintf(debugger, "Gebe buffer frei: %x (flush_queue) !\n", buffer); fflush(debugger); */

    nrelp(buffer);
#endif /* nCUBE */
 }
}

#endif /* D_MESS_ONLY */
