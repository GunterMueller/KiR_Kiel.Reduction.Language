/*********************************************************************/
/*                                                                   */
/* rncsched.c --- scheduler for the nCube                            */
/*                                                                   */
/* ach 13/01/93                                                      */
/*                                                                   */
/*********************************************************************/

/* $LOG:$ */

#include "rstdinc.h"
#include "rncstack.h"
#include "rheapty.h"
#include "rncsched.h"
#include "rncmessage.h"
#include "rstelem.h"
#include "rextern.h"
#include <stdio.h>
#include "dbug.h"

#if D_SPEC
#include "spec_sched.h"
#endif

#if D_NN_SCHED
#include "nc_nn_sched.h"
#endif

#if D_MESS
#include "d_mess_io.h" 
#endif

#if D_SPEC
#include "spec_sched.h"
FILE *debugger;
#endif

#define NULL 0
#define MAXPROCESSORS 32

/* extern variables */
extern char ticket_file[];
extern INSTR *code;
extern DStackDesc D_S_A, D_S_H, D_S_R, D_S_I;
extern DStackDesc *ps_w, *ps_a;
#if WITHTILDE
extern DStackDesc *ps_t, *ps_r;
#endif
extern int _redcnt;
#if D_SPEC
extern int _spec_redcnt;
#endif
extern int proc_id;
extern int cube_dim;
extern int sig_msg_flag;

#if ADV_SCHED
extern int sig_suspend, tickets_cond;
extern take_yellow;
#endif /* ADV_SCHED */

/* global variables */

int d_sort_in;

#ifndef D_MESS_ONLY

unsigned int dist_stack_elems;           /* the size of the stack frame from DIST */
                                         /* or the bit field from DIST_B */
int dist_stack_free;                     /* the number of stackelements to be freed */
#if WITHTILDE
unsigned int dist_tilde_elems;           /* the size of the tilde frame from DIST */
int dist_tilde_free;                     /* the number of tildeelements to be freed */
#endif /* WITHTILDE */
DIST_TYPE dist_type;                     /* type of distribution */
INSTR *dist_instr;
#if (ADV_SCHED && D_MESS)
int no_suspended=0;
#endif

#else
extern unsigned int dist_stack_elems;
extern int dist_stack_free;
#if WITHTILDE
extern unsigned int dist_tilde_elems;
extern int dist_tilde_free;
#endif /* WITHTILDE */
extern DIST_TYPE dist_type;
extern INSTR *dist_instr;
#if (ADV_SCHED && D_MESS)
extern int no_suspended;
#endif
#endif

#ifndef D_MESS_ONLY
PROCESS_CONTEXT * curr_pid = NULL;              /* process_id of currently running process */
int ticket_pool[MAXPROCESSORS];
int lookup_next=-1;
int first_lookup;
int proc_tab_size;
int ticket_pool_counter = 0;
#else

extern PROCESS_CONTEXT * curr_pid;
extern ticket_pool[MAXPROCESSORS];
extern int lookup_next;
extern int proc_tab_size;
extern int ticket_pool_counter;

#endif

/* local variables */

#ifndef D_MESS_ONLY

#if ADV_SCHED

PROCESS_CONTEXT * ready_queue_red_first;     /* first element of the red ready_queue */
PROCESS_CONTEXT * ready_queue_red_last;      /* last element of the red ready-queue */
PROCESS_CONTEXT * ready_queue_yellow_first;  /* first element of the yellow ready_queue */
PROCESS_CONTEXT * ready_queue_yellow_last;   /* last element of the yellow ready-queue */

#endif /* ADV_SCHED */

PROCESS_CONTEXT * ready_queue_first;    /* first element of the ready_queue      */
PROCESS_CONTEXT * ready_queue_last;     /* last element of the  ready-queue      */

PROCESS_CONTEXT * released_queue_first; /* first element of the released_queue   */
PROCESS_CONTEXT * released_queue_last;  /* last element of the released_queue    */

PROCESS_CONTEXT * in_use_queue_first; /* first element of the in_use-queue */
PROCESS_CONTEXT * in_use_queue_last;  /* last element of the in_use-queue */

PROCESS_CONTEXT * process_table;

#else

#if ADV_SCHED

extern PROCESS_CONTEXT * ready_queue_red_first;
extern PROCESS_CONTEXT * ready_queue_red_last;
extern PROCESS_CONTEXT * ready_queue_yellow_first;
extern PROCESS_CONTEXT * ready_queue_yellow_last;

#endif /* ADV_SCHED */

extern PROCESS_CONTEXT * ready_queue_first;
extern PROCESS_CONTEXT * ready_queue_last;

extern PROCESS_CONTEXT * released_queue_first;
extern PROCESS_CONTEXT * released_queue_last;
extern PROCESS_CONTEXT * process_table;

extern PROCESS_CONTEXT * in_use_queue_first;
extern PROCESS_CONTEXT * in_use_queue_last;

#endif

#ifndef D_MESS_ONLY

/*********************************************************************/
/*                                                                   */
/* init_scheduler()  Initializes the scheduler: allocates the        */
/*                   process table and the queues, sets variables    */
/*                                                                   */
/* called by:        init (oder nc_init)                             */
/*                                                                   */
/*********************************************************************/

void init_scheduler()
{int i;
 PROCESS_CONTEXT *next;

 DBUG_ENTER("init_scheduler");

#if D_NN_SCHED
 proc_tab_size = init_nn() + ((proc_id == 0)?1:0);
#else
 proc_tab_size = read_ticket_file() + ((proc_id == 0)?1:0);
 for (i=0;i<1<<cube_dim;i++)
   ticket_pool_counter+=ticket_pool[i];
#endif

 DBUG_PRINT("SCHED",("Groesse der ProcTable: %d",proc_tab_size));
 process_table = released_queue_first = (PROCESS_CONTEXT*)malloc(proc_tab_size * sizeof(PROCESS_CONTEXT));

 if ((!process_table) && (proc_tab_size >0))
   post_mortem("could not allocate process table");
 DBUG_PRINT("SCHED",("Beginn der ProcTable:%x",released_queue_first));
 for (i=0;i<proc_tab_size-1;i++)
   released_queue_first[i].next = &released_queue_first[i+1];

 released_queue_first[i].next = NULL;
 released_queue_last = released_queue_first+i;

 ready_queue_first = ready_queue_last = NULL;

 in_use_queue_first = NULL;
 in_use_queue_last = NULL;

#if D_SPEC /* same thing for speculative processes */

  spec_in_use_queue_first = NULL;
  spec_in_use_queue_last = NULL;

#if D_SPEC_TICKETS

 spec_proc_tab_size = spec_read_ticket_file();

 for (i=0;i<1<<cube_dim;i++)
   spec_ticket_pool_counter+=spec_ticket_pool[i];
 
 DBUG_PRINT("SCHED",("Groesse der Spec_ProcTable: %d",spec_proc_tab_size));

/* fprintf(debugger, "0x%x: Groesse der Spec_ProcTable: %d\n", curr_pid, spec_proc_tab_size); fflush(debugger);*/

 spec_process_table = spec_released_queue_first = (PROCESS_CONTEXT*)malloc(spec_proc_tab_size * sizeof(PROCESS_CONTEXT));
 
 if ((!spec_process_table) && (spec_proc_tab_size >0))
   post_mortem("could not allocate spec process table");
 DBUG_PRINT("SCHED",("Beginn der Spec_ProcTable:%x",spec_released_queue_first));
 for (i=0;i<spec_proc_tab_size-1;i++)
   spec_released_queue_first[i].next = &spec_released_queue_first[i+1];
 
 spec_released_queue_first[i].next = NULL;
 spec_released_queue_last = spec_released_queue_first+i;
 
 spec_ready_queue_first = spec_ready_queue_last = NULL;

#else

  DBUG_PRINT("SCHED",("Size of process table for speculative processes: %d", MAX_SPEC_PROCS));
  spec_process_table = spec_released_queue_first = (PROCESS_CONTEXT*)malloc(MAX_SPEC_PROCS* sizeof(PROCESS_CONTEXT));

  if ((!spec_process_table) && (MAX_SPEC_PROCS>0))
    post_mortem("could not allocate speculative process table");

  for (i=0;i<MAX_SPEC_PROCS-1;i++)
    spec_released_queue_first[i].next = &spec_released_queue_first[i+1];

 DBUG_PRINT("SCHED", ("i is now %d", i));
 spec_released_queue_first[i].next = NULL;
 spec_released_queue_last = spec_released_queue_first+i;

 spec_ready_queue_first = spec_ready_queue_last = NULL;

 spec_next_proc = proc_id;

#endif /* D_SPEC_TICKETS */

#endif /* D_SPEC */

#if ADV_SCHED
 ready_queue_red_first = ready_queue_red_last = NULL;
 ready_queue_yellow_first = ready_queue_yellow_last = NULL;
#if D_MESS
 no_suspended = 0;
#endif /* D_MESS */
#endif /* ADV_SCHED */

 DBUG_VOID_RETURN;
} 

/*********************************************************************/
/*                                                                   */
/* exit_scheduler()  Deinitializes the scheduler: deallocates the    */
/*                   process table                                   */
/*                                                                   */
/* called by:        exit (oder nc_exit)                             */
/*                                                                   */
/*********************************************************************/

void exit_scheduler()
{}

#endif

/*********************************************************************/
/*                                                                   */
/* process_create()  creates a new entry in the process table,       */
/*                   allocates the stacks and enqueues the process   */
/*                   into the ready_queue                            */
/*                                                                   */
/* arguments: pc         the value to be inserted in the pc-field    */
/*                       of the process context                      */
/*                                                                   */
/*            red_count  the reduction counter for the new process   */
/*                                                                   */
/* return value:     the new pid                                     */
/*                                                                   */
/* notes:            process_create only creates the process, the    */
/*                   initialization of the stacks has to be done by  */
/*                   the calling function (the message handling)     */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

PROCESS_CONTEXT * m_process_create(INSTR* pc, PROCESS_CONTEXT * ppid, int red_count)

#else

PROCESS_CONTEXT * process_create(INSTR* pc, PROCESS_CONTEXT * ppid, int red_count)

#endif

{PROCESS_CONTEXT *new_context;

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_process_create");
#else
 DBUG_ENTER("process_create");
#endif

#if D_SPEC
 if (spec_global==0) { /* no speculative process */
#endif /* D_SPEC */

 new_context = released_queue_first;

DBUG_PRINT("process_create", ("new_context is 0x%x", new_context));
 
 if (new_context == NULL)
   post_mortem("RUNTIME ERROR: no more processes ?! ;-)");

#if D_SPEC
 DBUG_PRINT("process_create", ("no speculative process"));
 new_context->specmask1 = new_context->processes = new_context->control = 0;
 new_context->specmask2 = 0;
 new_context->father_pid = new_context->father_node = 0;
 new_context->son_results = new_context->son_redcnts = new_context->son_status = 0;
 new_context->spec_red_count = -1;
#if D_SPEC_TICKETS
 new_context->ffound = 0; 
#endif
#endif /* D_SPEC */

/* fprintf(debugger, "0x%x: process_create: 0x%x, new_context->control: 0x%x\n", curr_pid, new_context, new_context->control); fflush(debugger);    */

 released_queue_first = released_queue_first->next;
 if (released_queue_last == new_context)
   released_queue_last = NULL;
 new_context->status = PS_READY;
 new_context->pc = pc;
 new_context->ppid = ppid;

DBUG_PRINT("SCHED", ("ppid: %x", new_context->ppid));
 
 new_context->red_count = red_count;
 new_context->start_red_count = red_count;
 new_context->switched = NULL;
 init_d_stacks(&(new_context->D_S_H),
               &(new_context->D_S_A),
               &(new_context->D_S_R),
               &(new_context->D_S_I));

 /* enqueue process in in_use-list */

 new_context->in_use_next = in_use_queue_first;
 in_use_queue_first = new_context;
 if (!in_use_queue_last)
   in_use_queue_last = new_context;

#if D_SPEC
 } else { /* new speculative process */
 DBUG_PRINT("SCHED", ("new speculative (leaf-) process; %x", spec_released_queue_first));

 new_context->spec_red_count = -1;
#if D_SPEC_TICKETS
 new_context->ffound = 0;   
#endif

/* fprintf(debugger, "0x%x: process_create; spec_released_queue_first is 0x%x !\n", curr_pid, spec_released_queue_first); fflush(debugger);  */

 if (spec_released_queue_first == NULL)
   post_mortem ("no more speculative process slots available...");

 new_context = spec_released_queue_first;

 new_context->specmask1 = new_context->processes = new_context->control = 0;
 new_context->specmask2 = 0;
 new_context->father_pid = new_context->father_node = 0;
 new_context->son_results = new_context->son_redcnts = new_context->son_status = 0;
 new_context->spec_red_count = -1;

/* fprintf(debugger, "0x%x: process_create: new speculative process 0x%x\n", curr_pid, new_context); fflush(debugger);      */

 spec_released_queue_first = spec_released_queue_first->next;
 if (spec_released_queue_last == new_context)
   spec_released_queue_last = NULL;

 /* enqueue process in in_use_list */

 new_context->in_use_next = spec_in_use_queue_first;
 spec_in_use_queue_first = new_context;
 if (!spec_in_use_queue_last)
   spec_in_use_queue_last = new_context;

 new_context->status = PS_READY;
 new_context->pc = pc;
 new_context->ppid = ppid;

DBUG_PRINT("SCHED", ("ppid: %x", new_context->ppid));

 new_context->red_count = red_count;
 new_context->spec_red_count = 0;
 new_context->start_red_count = red_count;
 new_context->switched = NULL;
 init_d_stacks(&(new_context->D_S_H),
               &(new_context->D_S_A),
               &(new_context->D_S_R),
               &(new_context->D_S_I));
 new_context->next = NULL;
 new_context->specmask1 = 0; /* initialize */

#if D_SPEC_TICKETS
 new_context->nesting = 0;
 new_context->spec_postpone_start = NULL;
 new_context->spec_postpone_end = NULL;
#endif /* D_SPEC_TICKETS */

 PC_SET_SPEC_PROCESS(new_context);
 PC_SET_SPEC_LEAF(new_context);
 DBUG_PRINT("SCHED", ("specmask1: %x", new_context->specmask1));
 /* allocate space for the son processes */
 if (!(new_context->processes = malloc(2*sizeof(int)*max_spec_sons)))
   post_mortem ("could not allocate space for speculative son processes...");

 /* allocate space for the son results */
 if (!(new_context->son_results = malloc(sizeof(int)*max_spec_sons)))
   post_mortem ("could not allocate space for speculative son results...");
 
 /* allocate space for the son redcnts */
 if (!(new_context->son_redcnts = malloc(sizeof(int)*max_spec_sons)))
   post_mortem ("could not allocate space for speculative son redcnts...");
 
 /* allocate space for the son status */
 if (!(new_context->son_status = malloc(sizeof(int)*max_spec_sons)))
   post_mortem ("could not allocate space for speculative son status...");

 }
#endif /* D_SPEC */


#if !ADV_SCHED2

/* fprintf(stderr,"*** %d new process...\n", proc_id); fflush(stderr); */

#if D_SPEC
 if (spec_global==0) {
 DBUG_PRINT("process_create", ("still no speculative process"));
#endif /* D_SPEC */

 new_context->next = NULL;
 if (!ready_queue_first)
   ready_queue_first = new_context;
 if (ready_queue_last)
   ready_queue_last->next = new_context;
 ready_queue_last = new_context;

#if D_SPEC
 } else { /* speculative task creation */
 DBUG_PRINT("process_create", ("still a speculative process"));
/*  if (!spec_ready_queue_first)
   spec_ready_queue_first = new_context;
 if (spec_ready_queue_last)
   spec_ready_queue_last->next = new_context;
 spec_ready_queue_last = new_context; */
/*** don't enqueue this process, wait for an acknowledge message from the control
     process ***/
   new_context->status = PS_WAITING;
 }
#endif /* D_SPEC */

/*  fprintf(stderr,"*** %d new process done ...\n", proc_id); fflush(stderr); */

#else /* ADV_SCHED2 set */

 if (!ready_queue_first) {
   ready_queue_first = new_context;
   ready_queue_last = new_context;
   }
 else {
   new_context->next = ready_queue_first;
   ready_queue_first = new_context;
   }

#endif /* !ADV_SCHED2 */

 DBUG_PRINT("SCHED",("Habe Prozess %x erzeugt...",new_context));

#if D_NN_SCHED
  proc_stat[P_READY]++;
#endif


#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
#if D_SPEC
 MPRINT_PROC_CREATE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) new_context, spec_global);
#else
 MPRINT_PROC_CREATE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) new_context, 0);
#endif
#endif
#endif

#if NCDEBUG
 DBNCcreate(new_context);
#endif

 DBUG_RETURN(new_context);}

/*********************************************************************/
/*                                                                   */
/* process_terminate()  removes a process from the process table,    */
/*                      enqueues its context into the released_queue */
/*                      and frees the stacks                         */
/*                                                                   */
/* argument: pid        pid of the process                           */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

void m_process_terminate(PROCESS_CONTEXT *pid)

#else

void process_terminate(PROCESS_CONTEXT *pid)

#endif

{int i;
 STACKELEM top;
 PROCESS_CONTEXT *help;
#if D_SPEC
 PROCESS_CONTEXT *help2;
#endif /* D_SPEC */

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_process_terminate");
#else
 DBUG_ENTER("process_terminate");
#endif

/* fprintf(debugger, "terminate 0x%x\n", pid); fflush(debugger);   */
/* stackslen(pid); */

if (curr_pid == pid) {
 pid->D_S_A=D_S_A;
 pid->D_S_H=D_S_H;
 pid->D_S_I=D_S_I;
 pid->D_S_R=D_S_R;
}

/* fprintf(debugger, "0x%x: process_terminate: starting...\n", pid); fflush(debugger);    */

 /* remove remaining elements from the I-Stack */
/* for (i=0;i<d_stacksize(&(pid->D_S_I));i++)
   {top = D_POPSTACK(pid->D_S_I);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from I-stack",top,top));
    if (isdesc(top))
      T_DEC_REFCNT((T_PTD)top);}
*/

#if D_SPEC
  DBUG_PRINT("SCHED", ("terminate pid 0x%x, T_SPEC_PROCESS(pid): %d", pid, T_SPEC_PROCESS(pid)));

  if ((pid->son_results!=0) && (pid->control==0)) {  /* speculative process */
  DBUG_PRINT("SCHED", ("killing speculative process"));

/* debugger -Stuff, COMMENT OUT ! *************************/
 /*  if (proc_id==0)
    spec_check_pid(pid); */

/* fprintf(debugger, "0x%x: process_terminate: killing speculative process 0x%x !\n", curr_pid, pid); fflush(debugger);  */

/* #define CHECK_REFCOUNT  */
#define CHECK_REFCOUNT if (((T_PTD)top)->u.sc.ref_count == 0) post_mortem("************* refcount tooooooo decreased ! ;-)"); 

  /* freeing stack_system */

/* ************************

DBUG_PRINT("SCHED", ("d_stacksize(&(pid->D_S_H)): %d, TopofStack: %x", d_stacksize(&(pid->D_S_H)), pid->D_S_H.TopofStack));

  for (i=0;i<d_stacksize(&(pid->D_S_H));i++)
   {top = D_POPSTACK(pid->D_S_H);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from H-stack, pointer %d",top,top, T_POINTER(top)));
    if (isdesc(top)) {
      DBUG_PRINT("TERM", ("refcount: %d", ((T_PTD)top)->u.sc.ref_count));
      CHECK_REFCOUNT;
      T_DEC_REFCNT((T_PTD)top);}}

DBUG_PRINT("SCHED", ("d_stacksize(&(pid->D_S_A)): %d, TopofStack: %x", d_stacksize(&(pid->D_S_A)), pid->D_S_A.TopofStack));

  for (i=0;i<d_stacksize(&(pid->D_S_A));i++)
   {top = D_POPSTACK(pid->D_S_A);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from A-stack, pointer %d",top,top,T_POINTER(top)));
    if (isdesc(top)) {
      DBUG_PRINT("TERM", ("refcount: %d", ((T_PTD)top)->u.sc.ref_count));
      CHECK_REFCOUNT;
      T_DEC_REFCNT((T_PTD)top);}}

DBUG_PRINT("SCHED", ("d_stacksize(&(pid->D_S_R)): %d, TopofStack: %x", d_stacksize(&(pid->D_S_R)), pid->D_S_R.TopofStack));

  for (i=0;i<d_stacksize(&(pid->D_S_R));i++)
   {top = D_POPSTACK(pid->D_S_R);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from R-stack, pointer %d",top,top,T_POINTER(top)));
    if (isdesc(top)) {
      DBUG_PRINT("TERM", ("refcount: %d", ((T_PTD)top)->u.sc.ref_count));
      CHECK_REFCOUNT;
      T_DEC_REFCNT((T_PTD)top);}}

DBUG_PRINT("SCHED", ("d_stacksize(&(pid->D_S_I)): %d, TopofStack: %x", d_stacksize(&(pid->D_S_I)), pid->D_S_I.TopofStack));

  for (i=0;i<d_stacksize(&(pid->D_S_I));i++)
   {top = D_POPSTACK(pid->D_S_I);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from I-stack, pointer %d",top,top,T_POINTER(top)));
    if (isdesc(top)) {
      DBUG_PRINT("TERM", ("refcount: %d", ((T_PTD)top)->u.sc.ref_count));
      CHECK_REFCOUNT;
      T_DEC_REFCNT((T_PTD)top);}}

*********************** freeing is no longer necessary, 'cause processes are no longer instantly terminated ****/

/*  fprintf(debugger, "0x%x: pid->D_S_H.BotofSeg(%s) %x, pid->D_S_A.BotofSeg(%s) %x, pid->D_S_R.BotofSeg(%s) %x, pid->D_S_I.BotofSeg(%s) %x\n", curr_pid, pid->D_S_H.name, *(pid->D_S_H.BotofSeg), pid->D_S_A.name, *(pid->D_S_A.BotofSeg), pid->D_S_R.name, *(pid->D_S_R.BotofSeg), pid->D_S_I.name, *(pid->D_S_I.BotofSeg)); fflush(debugger);*/

/* fprintf(debugger, "0x%x: terminate; stacksizes H: %d, A: %d, R: %d, I: %d\n", curr_pid, d_stacksize(&(pid->D_S_H)), d_stacksize(&(pid->D_S_A)), d_stacksize(&(pid->D_S_R)), d_stacksize(&(pid->D_S_I))); fflush(debugger); */
 
  free_d_stacks(&(pid->D_S_H),
                &(pid->D_S_A),
                &(pid->D_S_R),
                &(pid->D_S_I)); 

/* fprintf(debugger, "0x%x: terminate; stacks free'd !\n", curr_pid); fflush(debugger);  */

/*  fprintf(debugger, "0x%x: process_terminate: stacks are freed\n", curr_pid); fflush(debugger);*/

  DBUG_PRINT("process_terminate", ("updating pid %x in all queues...", pid));

/*   fprintf (debugger, "%d: SUCKER's status is %x\n", proc_id, PC_STATUS(pid)); fflush(debugger);    */

  /* clearing all counters and flags in the process context */

  pid->specmask1 = pid->specmask2 = 0;
  pid->control_pid = pid->control_node = pid->father_pid = pid->father_node = 0;
  pid->spec_red_count = pid->father_slot = 0;
#if D_SPEC_TICKETS
  pid->nesting = pid->ffound = 0;
#endif

/*   DBUG_PRINT("TERM", ("now deallocating sons-tab"));

  if (pid->processes) 
    free(pid->processes);

  if (pid->son_results)
    free(pid->son_results);
  if (pid->son_redcnts)
    free(pid->son_redcnts);
  if (pid->son_status)
    free(pid->son_status); */

  pid->son_results = pid->son_redcnts = pid->son_status = pid->processes = 0;

/* fprintf(debugger, "0x%x: terminate, before dequeueing the process off the in-use-list !\n"), fflush(debugger); */

  /* dequeue process in spec_in_use-list/queue BEFORE enqueing the process in the spec released queue */
 
  if (spec_in_use_queue_first == pid) {
    spec_in_use_queue_first = pid->in_use_next;
    DBUG_PRINT("process_terminate", ("process was first in spec_in_use_queue"));
    if (spec_in_use_queue_last == pid) {
      spec_in_use_queue_last = NULL;
      DBUG_PRINT("process_terminate", ("process was also last in spec_in_use_queue, so queue is empty"));
      }
    }
  else { /* search for the pid */
    help = spec_in_use_queue_first;
    while (help != NULL) {
      DBUG_PRINT("process_terminate", ("testing %x while searching for %x...", help->in_use_next, pid));
      if (help->in_use_next == pid) { /* found it ! */
        DBUG_PRINT("process_terminate", ("found it."));
        help->in_use_next = pid->in_use_next;
        if (pid == spec_in_use_queue_last) {
          DBUG_PRINT("process_terminate", ("it was the last element..."));
          spec_in_use_queue_last = help;
          }
        help = NULL; /* terminate WHILE */
        }
      else
        help = help->in_use_next;
      }
    }
  pid->in_use_next = NULL;

  DBUG_PRINT("process_terminate", ("inserting pid %x in spec_released_queue...", pid));

/* fprintf(debugger, "0x%x: before: spec_released_queue_first 0x%x, spec_released_queue_last 0x%x, ready_queue_first 0x%x\n", curr_pid, spec_released_queue_first, spec_released_queue_last, ready_queue_first); fflush(debugger);  */

  if (d_sort_in == 0) {
    DBUG_PRINT("process_terminate", ("insert pid as last element in the ready queue"));

    if (!spec_released_queue_first)
      spec_released_queue_first = pid;

    if (spec_released_queue_last)
      spec_released_queue_last->next = pid;
    spec_released_queue_last = pid;  

    } else {
    DBUG_PRINT("process_terminate", ("insert pid as first element in the ready queue"));

    pid->next = spec_released_queue_first;
    spec_released_queue_first = pid;
    if (!spec_released_queue_last)
      spec_released_queue_last = pid;
    
    }

/*  pid->next = spec_released_queue_first;
  spec_released_queue_first = pid; */

/* fprintf(debugger, "0x%x: terminate, process dequeued\n"), fflush(debugger); */

#if D_SPEC_TICKETS
  if (pid->spec_postpone_start != NULL) {

/* fprintf(debugger, "0x%x: terminate, postponed expressions are left !\n"), fflush(debugger); */

    post_mortem("Postponed speculative expressions left !"); }
#endif /* D_SPEC_TICKETS */
 
/* fprintf(debugger, "0x%x: after: spec_released_queue_first 0x%x, spec_released_queue_last 0x%x, ready_queue_first 0x%x\n", curr_pid, spec_released_queue_first, spec_released_queue_last, ready_queue_first); fflush(debugger); */

/* fprintf(debugger, "0x%x: leaving speculative termination\n", curr_pid); fflush(debugger);  */

    } else {

/* fprintf(debugger, "0x%x: process_terminate: terminating vital process 0x%x, pid->control ist 0x%x\n", curr_pid, pid, pid->control); fflush(debugger);      */

/*  if (pid->son_results != NULL) {

    DBUG_PRINT("process_terminate", ("freeing pid->son_* structures !"));

    free(pid->son_results);
    if (pid->son_redcnts)
      free(pid->son_redcnts);
    if (pid->son_status)
      free(pid->son_status);
    pid->son_results = pid->son_redcnts = pid->son_status = 0;
    } */

  if (T_SPEC_GRANDDAD(pid)==1) { /* it was a master-process ! */
    DBUG_PRINT("process_terminate", ("it was a GRANDDAD process, free'ing control structure !"));

/*  fprintf(debugger, "process_terminate: it was a spec master ! pid->control->data ist %x\n", pid->control->data); fflush(debugger);   */

    if (pid->control->data)
      free(pid->control->data);
    free(pid->control);

    pid->specmask1 = pid->specmask2 = 0;
    pid->control_pid = pid->control_node = pid->father_pid = pid->father_node = 0;
    }

#endif /* D_SPEC */

  free_d_stacks(&(pid->D_S_H),
                &(pid->D_S_A),
                &(pid->D_S_R),
                &(pid->D_S_I));

  /* dequeue process in in_use-list/queue BEFORE enqueing the process in the released queue */
 
  if (in_use_queue_first == pid) {
    in_use_queue_first = pid->in_use_next;
    DBUG_PRINT("process_terminate", ("process was first in in_use_queue"));
    if (in_use_queue_last == pid) {
      in_use_queue_last = NULL;
      DBUG_PRINT("process_terminate", ("process was also last in in_use_queue, so queue is empty"));
      }
    }
  else { /* search for the pid */
    help = in_use_queue_first;
    while (help != NULL) {
      DBUG_PRINT("process_terminate", ("testing %x while searching for %x...", help->in_use_next, pid));
      if (help->in_use_next == pid) { /* found it ! */
        DBUG_PRINT("process_terminate", ("found it."));
        help->in_use_next = pid->in_use_next;
        if (pid == in_use_queue_last) {
          DBUG_PRINT("process_terminate", ("it was the last element..."));
          in_use_queue_last = help;
          }
        help = NULL; /* terminate WHILE */
        }
      else
        help = help->in_use_next;
      }
    }
  pid->in_use_next = NULL;

 if (d_sort_in == 0) {
    DBUG_PRINT("process_terminate", ("insert pid as last element in the ready queue"));

    if (!released_queue_first)
      released_queue_first = pid;
    if (released_queue_last)
      released_queue_last->next = pid;
    released_queue_last = pid;

    } else {
    DBUG_PRINT("process_terminate", ("insert pid as first element in the ready queue"));
 
    pid->next = released_queue_first;
    released_queue_first = pid;
    if (!released_queue_last)
      released_queue_last = pid;
 
    }


#if D_SPEC
  }
#endif /* D_SPEC */

  if (pid == curr_pid) {
#if D_MESS
    m_curr_pid = -1;
#endif
    curr_pid = NULL;
    }
  
#if D_MESS
  if (pid == curr_pid)
    m_curr_pid = -1;
#endif
  
  DBUG_PRINT("SCHED",("Habe Prozess %x gekillt...",pid));

#if D_NN_SCHED
  proc_stat[P_RUN]--;
#endif

  
#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
  MPRINT_PROC_TERMINATE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) pid);
#endif
#endif
  
#if NCDEBUG
  DBNCterminated(pid);
  DBNCtestcomplete();
#endif

/* fprintf(debugger, "termination done !\n"); fflush(debugger);  */

/* fprintf(debugger, "0x%x: leaving process_terminate\n", curr_pid); fflush(debugger);     */


  DBUG_VOID_RETURN;}

/*********************************************************************/
/*                                                                   */
/* process_run()    activates a process                              */
/*                  The global pc is set to the process' pc, and     */
/*                  the global stacks to the process' stacks         */
/*                                                                   */
/* argument: pid    pid of the process                               */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

void m_process_run(PROCESS_CONTEXT *pid)

#else

void process_run(PROCESS_CONTEXT *pid)

#endif

{
#ifdef D_MESS_ONLY
DBUG_ENTER ("m_process_run");
#else
DBUG_ENTER("process_run");
#endif

/* fprintf(debugger, "0x%x R\n", pid); fflush(debugger); */

/* fprintf(debugger, "0x%x: process_run: activate pid 0x%x\n", curr_pid, pid); fflush(debugger);  */

#if ADV_SCHED

  /* choose next active process out of three ready queues */
  if (sig_suspend) { /* activate the next white process */
    ready_queue_first = pid->next;
    if (ready_queue_last == pid)
      ready_queue_last = NULL;
    pid->next = NULL;
    sig_suspend = 0;
    
/*    fprintf(stderr, "*** %d: sig_suspend reset\n", proc_id); fflush(stderr); */

    tickets_cond = 0; /* let it run */
    }
  else {
  if (ready_queue_red_first && !take_yellow) {  /* choose red process or force yellow */
    pid = ready_queue_red_first;
    ready_queue_red_first = pid->next; 
    if (ready_queue_red_last == pid)
      ready_queue_red_last = NULL;
    pid->next = NULL;
    }
  else 

#if ADV_SCHED2

    if (take_yellow || !ready_queue_first || (ready_queue_yellow_first && 
        (ready_queue_yellow_first->letpar_level <= ready_queue_first->letpar_level))) {

#else /* ADV_SCHED2 */

    if (ready_queue_yellow_first) {  /* choose yellow process */

#endif /* ADV_SCHED2 */
 
    take_yellow = 0; /* force taking a yellow process reset */

    pid = ready_queue_yellow_first;
    ready_queue_yellow_first = pid->next; 
    if (ready_queue_yellow_last == pid)
      ready_queue_yellow_last = NULL;
    pid->next = NULL;
    }
  else { /* choose white process */

    ready_queue_first = pid->next;
    if (ready_queue_last == pid)
      ready_queue_last = NULL;
    pid->next = NULL; 
    } }
#else /* ADV_SCHED */

#if D_SPEC
 DBUG_PRINT("SCHED", ("specmask1 is %x, spec: %x", pid->specmask1, T_SPEC_PROCESS(pid)));
 if ((pid->son_results!=0) && (pid->control==0)) {
    DBUG_PRINT("SCHED", ("starting speculative process..."));

/* fprintf(debugger, "process_run: spec_ready_queue_first = %x vorher\n", spec_ready_queue_first);   */

    spec_ready_queue_first = pid->next;

/* fprintf(debugger, "process_run: spec_ready_queue_first = %x nachher\n", spec_ready_queue_first); fflush(debugger); */

    if (spec_ready_queue_last == pid)
      spec_ready_queue_last = NULL;
    pid->next = NULL;
    _spec_redcnt = pid->spec_red_count;
   } else {
#endif /* D_SPEC */

 DBUG_PRINT("SCHED", ("starting vital process..."));

/* fprintf(debugger, "process_run: starting vital process\n"); fflush(debugger);   */

/* fprintf(debugger, "process_run: ready_queue_first = %x vorher\n", ready_queue_first);*/

 ready_queue_first = pid->next;

/* fprintf(debugger, "process_run: ready_queue_first = %x nachher\n", ready_queue_first); fflush(debugger);*/

 if (ready_queue_last == pid)
   ready_queue_last = NULL;
 pid->next = NULL;

#if D_SPEC
  }
#endif /* D_SPEC */

#endif /* ADV_SCHED */

 code = pid->pc;
 curr_pid = pid;
 /* hey, why has Mr. Held forgotten this... ??? Is he mad ? Stupid ? Great ? Yeah... */
 curr_pid->status = PS_ACTIVE;

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
 MPRINT_PROC_RUN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) pid);
#endif
#endif

#if D_MESS
 m_curr_pid = (int) pid;
#endif

 _redcnt = pid->red_count;
 D_S_A = pid->D_S_A;
 D_S_H = pid->D_S_H;
 D_S_R = pid->D_S_R;
 D_S_I = pid->D_S_I;
 if (pid->switched & W_A_SWITCHED)
   {
    DBUG_PRINT("wakeup", ("switch W and A..."));
    ps_w = &D_S_I;
    ps_a = &D_S_A;
   }
 else
   {
    DBUG_PRINT("wakeup", ("don't switch W and A..."));
    ps_w = &D_S_A;
    ps_a = &D_S_I;
   }
#if WITHTILDE
 if (pid->switched & R_T_SWITCHED)
   {
    DBUG_PRINT("wakeup", ("switch R and T..."));
    ps_r = &D_S_H;
    ps_t = &D_S_R;
   }
 else
   {
    DBUG_PRINT("wakeup", ("don't switch R and T..."));
    ps_r = &D_S_R;
    ps_t = &D_S_H;
   }
#endif

 DBUG_PRINT("SCHED",("Habe Prozess %x gestartet; curr_pid: %x",pid,curr_pid));

/*  fprintf(debugger, "0x%x: process_run: node 0x%x started PID 0x%x !!\n", curr_pid, proc_id, curr_pid); */
/* fprintf(debugger, "0x%x: Stacksystem: ps_w 0x%x (size %d)\n", curr_pid, ps_w, d_stacksize(ps_w)); */
/* fprintf(debugger, "0x%x: Stacksystem: ps_a 0x%x (size %d)\n", curr_pid, ps_a, d_stacksize(ps_a)); */
/*  fprintf(debugger, "0x%x: Stacksystem: ps_t 0x%x (size %d)\n", curr_pid, ps_t, d_stacksize(ps_t)); */
/* fprintf(debugger, "0x%x: Stacksystem: ps_r 0x%x (size %d)\n", curr_pid, ps_r, d_stacksize(ps_r)); fflush(debugger); */


#if D_NN_SCHED
  proc_stat[P_RUN]++;
  proc_stat[P_READY]--;
#endif

 DBUG_VOID_RETURN;
}

/*********************************************************************/
/*                                                                   */
/* process_sleep()  sets a process' status to 'waiting'              */
/*                                                                   */
/* argument: pid    pid of the process                               */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

void m_process_sleep(PROCESS_CONTEXT *pid)

#else

void process_sleep(PROCESS_CONTEXT *pid)

#endif

{
#ifdef D_MESS_ONLY
DBUG_ENTER("m_process_sleep");
#else
DBUG_ENTER("process_sleep");
#endif

/* fprintf(debugger, "0x%x: process sleep called...\n", curr_pid);   */
/*  fprintf(debugger, "0x%x: Stacksystem: ps_w 0x%x (size %d)\n", curr_pid, ps_w, d_stacksize(ps_w)); */
/* fprintf(debugger, "0x%x: Stacksystem: ps_a 0x%x (size %d)\n", curr_pid, ps_a, d_stacksize(ps_a)); */
/*  fprintf(debugger, "0x%x: Stacksystem: ps_t 0x%x (size %d)\n", curr_pid, ps_t, d_stacksize(ps_t)); */
/*  fprintf(debugger, "0x%x: Stacksystem: ps_r 0x%x (size %d)\n", curr_pid, ps_r, d_stacksize(ps_r)); fflush(debugger); */

 
 pid->pc = code;
 pid->red_count = _redcnt;
#if D_SPEC
 if (pid->son_results!=0) {
   DBUG_PRINT("SCHED", ("storing spec_redcnt !"));
    pid->spec_red_count = _spec_redcnt;
   }
#endif
 pid->status = PS_WAITING;
 pid->switched = 0;
 if (ps_w != &D_S_A)
   pid->switched = W_A_SWITCHED;
#if WITHTILDE
  if (ps_r != &D_S_R)
    pid->switched |= R_T_SWITCHED;
#endif
 pid->D_S_A=D_S_A;
 pid->D_S_H=D_S_H;
 pid->D_S_I=D_S_I;
 pid->D_S_R=D_S_R;
 curr_pid = NULL;

#if D_MESS
 m_curr_pid = -1;
#endif

/* fprintf(debugger, "0x%x S\n", pid); fflush(debugger);  */

/* fprintf(debugger, "0x%x: process_sleep: pid 0x%x\n", curr_pid, pid); fflush(debugger);    */

 DBUG_PRINT("SCHED",("Habe Prozess %x eingeschlaefert...",pid));

#if D_NN_SCHED
  proc_stat[P_WAIT]++;
  proc_stat[P_RUN]--;
#endif
#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
 MPRINT_PROC_SLEEP(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) pid);
#endif
#endif

 DBUG_VOID_RETURN;}

/*********************************************************************/
/*                                                                   */
/* process_wakeup()  enqueues a waiting process into the ready_queue */
/*                                                                   */
/* argument: pid     pid of the process                              */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

void m_process_wakeup(PROCESS_CONTEXT *pid)

#else

void process_wakeup(PROCESS_CONTEXT *pid)

#endif

{
#if (ADV_SCHED2 || D_SPEC)
 PROCESS_CONTEXT *tmp1,*tmp2,*tmp3;
#endif /* ADV_SCHED2 */

#ifdef D_MESS_ONLY
DBUG_ENTER("m_process_wakeup");
#else
DBUG_ENTER("process_wakeup");
#endif

 if (pid->status == PS_READY)
   post_mortem("RUNTIME ERROR: double wakeup ?!");

 pid->status = PS_READY;

#if ADV_SCHED

 if ((pid->start_letpar_level == pid->letpar_level) && !sig_suspend) { /* enqueue a red process */
   if (!ready_queue_red_first)
     ready_queue_red_first = pid;
   if (ready_queue_red_last)
     ready_queue_red_last->next = pid;
   ready_queue_red_last = pid; 
   }
 else { /* enqueue a yellow process */

#if ADV_SCHED2

/*   fprintf(stderr,"*** %d enqueue ...\n", proc_id); fflush(stderr); */

  /* enqueue process at the BEGINNING */

  if (!ready_queue_yellow_first) {
     ready_queue_yellow_first = pid;
     ready_queue_yellow_first->next = NULL;
     ready_queue_yellow_last = pid;
     }
  else {
    pid->next = ready_queue_yellow_first;
    ready_queue_yellow_first = pid;
    } 

  if (!sig_suspend) { /* sort queue */

/*     fprintf(stderr, "*** %d sorting...\n", proc_id); fflush(stderr); */

    tmp1 = NULL;
    tmp2 = ready_queue_yellow_first;
    tmp3 = ready_queue_yellow_first->next;

    while ((tmp3!=NULL) && (tmp2->letpar_level>=tmp3->letpar_level)) {
      
/*      fprintf(stderr, "*** %d exchanging...\n", proc_id); fflush(stderr); */

      /* exchange */
      if (tmp1 == NULL)
        ready_queue_yellow_first = tmp3;
      else
        tmp1->next = tmp3;

      tmp2->next = tmp3->next; /* tmp2 holds position */
      tmp3->next = tmp2; /* swap */
      tmp1 = tmp3;
      tmp3 = tmp2->next;

/*      fprintf (stderr, "*** %d exchanged %d and %d...\n", proc_id, tmp2->letpar_level, tmp1->letpar_level); fflush(stderr); */

      }
    
/*    fprintf(stderr, "*** %d no sorting or sorting done...\n", proc_id); fflush(stderr); */

/*    if (tmp3==NULL) 
      ready_queue_yellow_last = tmp2; */
    }

#else

   if (sig_suspend) { /* enqueue process at the BEGINNING */
     if (!ready_queue_yellow_first) {
       ready_queue_yellow_first = pid;
       ready_queue_yellow_last = pid;
       }
     else {
       pid->next = ready_queue_yellow_first;
       ready_queue_yellow_first = pid;
       } }
    else { /* NO sig_suspend, enqueue at the ending */
      if (!ready_queue_yellow_first)
        ready_queue_yellow_first = pid;
      if (ready_queue_yellow_last)
        ready_queue_yellow_last->next = pid;
      ready_queue_yellow_last = pid;
     } 
#endif /* ADV_SCHED2 */

   }

#else /* no ADV_SCHED */

/* fprintf(debugger, "0x%x W\n", pid); fflush(debugger);  */

#if D_SPEC
 if ((pid->son_results!=0) && (pid->control == 0)) {
   DBUG_PRINT("SCHED",("activating speculative process"));
   DBUG_PRINT("SCHED", ("spec_ready_queue_first = %x, pid = %x", spec_ready_queue_first, pid));
   DBUG_PRINT("SCHED", ("spec_ready_queue_last = %x", spec_ready_queue_last));

/* fprintf(debugger, "0x%x: process_wakeup: speculative process pid 0x%x\n", curr_pid, pid); fflush(debugger);    */

   if (!spec_ready_queue_first)
     spec_ready_queue_first = pid;
   if (spec_ready_queue_last)
     spec_ready_queue_last->next = pid;
   spec_ready_queue_last = pid;

/********************* searching in the queue for double-entry of pid ! ****************/

/*   tmp1 = spec_ready_queue_first;
   while (tmp1 !=spec_ready_queue_last)
     if (tmp1 == pid)
       post_mortem("speculative process enqueued twice !!!!");
     else
       tmp1 = tmp1->next; */

   DBUG_PRINT("SCHED", ("spec_ready_queue_first = %x, pid = %x", spec_ready_queue_first, pid));
   DBUG_PRINT("SCHED", ("spec_ready_queue_last = %x", spec_ready_queue_last));
   } else {
#endif /* D_SPEC */
  
 DBUG_PRINT("SCHED",("activating non speculative process"));

/*fprintf(debugger, "0x%x: process_wakeup: vital wakeup pid 0x%x\n", curr_pid, pid); fflush(debugger);  */

 if (!ready_queue_first)
   ready_queue_first = pid;
 if (ready_queue_last)
   ready_queue_last->next = pid;
 ready_queue_last = pid;

#if D_SPEC
  }
#endif /* D_SPEC */

#endif /* ADV_SCHED */


 DBUG_PRINT("SCHED",("Habe Prozess %x aufgeweckt...",pid));

#if D_NN_SCHED
  proc_stat[P_WAIT]--;
  proc_stat[P_READY]++;
#endif


#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
 MPRINT_PROC_WAKEUP(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) pid);
#endif
#endif

 DBUG_VOID_RETURN;}

#ifndef D_MESS_ONLY

/*********************************************************************/
/*                                                                   */
/* schedule()       activates the next process from the ready_queue  */
/*                  (if any), else calls the message checking        */
/*                  function                                         */
/*                                                                   */
/*********************************************************************/

void schedule()
{int counter = 0;

 DBUG_ENTER("schedule");

/* fprintf(debugger, "0x%x: ready_queue_first: 0x%x, spec_ready_queue_first: 0x%x\n", curr_pid, ready_queue_first, spec_ready_queue_first); fflush(debugger);  */

#if ADV_SCHED
 while (!ready_queue_first && !ready_queue_yellow_first && !ready_queue_red_first)
   {while ((!ready_queue_first && !ready_queue_yellow_first && !ready_queue_red_first) && 
           (counter++ < 100000))
#else

#if D_SPEC
 while (!ready_queue_first && !spec_ready_queue_first)
   {while ((!ready_queue_first && !spec_ready_queue_first) && (counter++ < 100000))
#else /* D_SPEC */
 while (!ready_queue_first)
   {while ((!ready_queue_first) && (counter++ < 100000))
#endif /* D_SPEC */

#endif /* ADV_SCHED */

      if (sig_msg_flag) {
	msg_check();
      }
    counter = 0;
#if nCUBE
#if ADV_SCHED
    if (!ready_queue_first && !ready_queue_yellow_first && !ready_queue_red_first)
#else /* ADV_SCHED */
#if D_SPEC
    if (!ready_queue_first && !spec_ready_queue_first)
#else
    if (!ready_queue_first)
#endif
#endif /* ADV_SCHED */
      msg_check();
#endif /* nCUBE */

#if D_PVM
#if (D_SLAVE && D_MESS && D_MCOMMU)
        (*d_m_msg_receive)();
#else
        msg_receive();
#endif
#endif /* D_PVM */

#if NCDEBUG
    DBNCclient();
#endif

  }

#if TIME_SLICE
  RESET_TIMER;
#endif

#if D_SPEC
 if (ready_queue_first) {
#endif

/* fprintf(debugger, "0x%x: schedule; ready_queue_first: 0x%x\n", curr_pid, ready_queue_first); fflush(debugger);   */

#if (D_SLAVE && D_MESS && D_MSCHED)
 (*d_m_process_run)(ready_queue_first);
#else
 process_run(ready_queue_first);
#endif

#if D_SPEC
  } else {

/* fprintf(debugger, "0x%x: schedule; spec_ready_queue_first: 0x%x\n", curr_pid, spec_ready_queue_first); fflush(debugger);  */

#if (D_SLAVE && D_MESS && D_MSCHED)
 (*d_m_process_run)(spec_ready_queue_first);
#else
 process_run(spec_ready_queue_first);
#endif
  }
#endif /* D_SPEC */

  DBUG_VOID_RETURN;
}

#endif /* D_MESS_ONLY */

/*********************************************************************/
/*                                                                   */
/* distribute()    checks if an expression can be distributed to     */
/*                 another processor and, in case of distribution,   */
/*                 calls the message sending routine                 */
/*                                                                   */
/* arguments: label1            the adress of the code to be         */
/*                              distributed                          */
/*            label2            the adress of the code following     */
/*                              the distributed part                 */
/*            send__frame_size  the size of the I-stack frame that   */
/*                              has to be sent to the destination    */
/*                              processor                            */
/*            free_frame_size   the number of stack elements to be   */
/*                              removed in case of distribution      */
/*            type              type of stack sending (full frame    */
/*                              or bit field selection)              */                
/*                                                                   */
/* return value:   label2           in case of distribution          */
/*                 label1           else                             */
/*                                                                   */
/*********************************************************************/

#ifdef D_MESS_ONLY

#if WITHTILDE

INSTR *m_distribute(INSTR *label1, INSTR *label2,int send_frame_size,int free_frame_size,int tilde_frame_size, int free_tilde_size,DIST_TYPE type)

#else

INSTR *m_distribute(INSTR *label1, INSTR *label2,int send_frame_size,int free_frame_size,DIST_TYPE type)

#endif /* WITHTILDE */
#else
#if WITHTILDE

INSTR *distribute(INSTR *label1, INSTR *label2,int send_frame_size,int free_frame_size,int tilde_frame_size, int free_tilde_size, DIST_TYPE type)

#else

INSTR *distribute(INSTR *label1, INSTR *label2,int send_frame_size,int free_frame_size,DIST_TYPE type)

#endif /* WITHTILDE */
#endif /* D_MESS_ONLY */

{int i;
 STACKELEM free_elem;

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_distribute");
#else
 DBUG_ENTER("distribute");
#endif

/* in the PVM version, we have to decide when to check for incoming messages */
/* in the current version we have two options: before distributing (here) */
/* and before a context switch */

#if WITHTILDE
  DBUG_PRINT ("distribute", ("send_frame_size %d, free_frame_size %d, tilde_frame_size %d, free_tilde_size %d", send_frame_size, free_frame_size, tilde_frame_size, free_tilde_size));
#endif /* WITHTILDE */

#if D_PVM
 msg_check();
#endif /* D_PVM */

#if NCDEBUG
 if (DBNCgetdistlevel())
   DBNCsetdistlevel(1);
 else
#endif

#if ADV_SCHED
   {if ((ticket_pool_counter == 0) || (tickets_cond && ready_queue_first && (ticket_pool_counter == 1))) 
/*     {if ((ticket_pool_counter == 0) || (tickets_cond && !ready_queue_first && !ready_queue_yellow_first)) */
#else /* ADV_SCHED */
   {if (ticket_pool_counter == 0)
#endif /* ADV_SCHED */

#if WITHTILDE
      {D_PUSHSTACK(*ps_r,(-1<<1));
#else
      {D_PUSHSTACK(D_S_R,-1);
#endif

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
       MPRINT_PROC_DIST(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, -1, send_frame_size);
#endif
#endif
       DBUG_PRINT("DIST", ("No distribution..."));
       DBUG_RETURN(label1);}

    for(;;)
      {if ((ticket_pool[lookup_next] > 0) /*&& (lookup_next != proc_id)*/)
	 {dist_stack_elems = send_frame_size;
	  dist_stack_free = free_frame_size;
#if WITHTILDE
	  dist_tilde_elems = tilde_frame_size;
	  dist_tilde_free = free_tilde_size;
#endif /* WITHTILDE */
	  dist_type = type;
	  dist_instr = label1;
	  D_PUSHSTACK(*ps_w,ST_DUMMY);
	  DBUG_PRINT("DIST",("distributing to node %d",lookup_next));
	  DBUG_PRINT("DIST",("dummy pushed, Top of W-Stack: %x",ps_w->TopofStack));
	  
#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
	  MPRINT_PROC_DIST(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, lookup_next, send_frame_size);
#endif
#endif
	  
#if (D_SLAVE && D_MESS && D_MCOMMU)
	  (*d_m_msg_send)(lookup_next,MT_NEW_PROCESS);
#else
	  msg_send(lookup_next,MT_NEW_PROCESS);
#endif

	  ticket_pool[lookup_next++]--;
	  ticket_pool_counter--;

	  if (lookup_next >= (1 << cube_dim))
	    lookup_next = 0;
	  for (i=0;i<free_frame_size;i++) {
	    free_elem = D_POPSTACK(*ps_a);
      	    T_DEC_REFCNT((T_PTD)free_elem);
          }

#if WITHTILDE
	  for (i=0;i<free_tilde_size;i++)
	    {free_elem = D_POPSTACK(*ps_t);
	     T_DEC_REFCNT((T_PTD)free_elem);}
#endif /* WITHTILDE */
	  DBUG_RETURN(label2);}
       lookup_next++;
       if (lookup_next >= (1 << cube_dim))
	 lookup_next = 0;}}
}

#ifndef D_MESS_ONLY

/*********************************************************************/
/*                                                                   */
/* get_distribute() - gets the number of the processor that would    */
/*                    be the destination of a distribution           */
/*                                                                   */
/* return value:  the number of the processor if distribution is     */
/*                 possible                                          */
/*		  -1 if distribution is not possible                 */
/*                                                                   */
/* !! also needed by measure-sys to determine distribution !!!       */
/*********************************************************************/

int get_distribute()
{int lookup;

 DBUG_ENTER("get_distribute");

 if (ticket_pool_counter == 0)
   DBUG_RETURN(-1);

 lookup = lookup_next;

 for(;;)
   {if ((ticket_pool[lookup] > 0) && (lookup != proc_id))
      DBUG_RETURN(lookup);
    lookup++;
    if (lookup == (1 << cube_dim))
      lookup = 0;}}
 
/*********************************************************************/
/*                                                                   */
/* process_context_update()   updates the context of the currently   */
/*                            active process                         */
/*                                                                   */
/*********************************************************************/

void process_context_update()
{DBUG_ENTER("process_context_update");
 
 curr_pid->pc = code;
 curr_pid->red_count = _redcnt;
 curr_pid->switched = 0;
 if (ps_w != &D_S_A)
   curr_pid = W_A_SWITCHED;
#if WITHTILDE
 if (ps_r != &D_S_R)
   curr_pid = R_T_SWITCHED;
#endif
 curr_pid->D_S_A=D_S_A;
 curr_pid->D_S_H=D_S_H;
 curr_pid->D_S_I=D_S_I;
 curr_pid->D_S_R=D_S_R;

 DBUG_VOID_RETURN;}

/*********************************************************************/
/*                                                                   */
/* read_ticket_file()  reads the file 'red.tickets' and initializes  */
/*                     the ticket pool                               */
/*                                                                   */
/* return value:       the size of the process table                 */
/*                                                                   */
/*********************************************************************/

int read_ticket_file()
{FILE *tf;
 int number,high,low,ch,i,j,found,tab_size = 0,nb;
 int list1_index,list2_index,list_flag;
 int *list1, *list2, *ticket_list, cube_size;

 DBUG_ENTER("read_ticket_file");

 cube_size = 1<<cube_dim;

 DBUG_PRINT("TICKET", ("cube_size: %d", cube_size));

 list1 = (int *)malloc(cube_size*sizeof(int));
 list2 = (int *)malloc(cube_size*sizeof(int));
 ticket_list = (int *)malloc(cube_size*sizeof(int));

 if ((!list1) || (!list2) || (!ticket_list))
   abort_init("out of memory while reading ticket file");

 for (i=0;i<cube_size;i++)
   ticket_pool[i] = ticket_list[i] = 0;

 tf = fopen(ticket_file,"r");
 if (!tf)
   {DBUG_PRINT("TICKET",("Ticket file not found..."));
    fprintf(stderr, "*** warning: Could not open ticket file >%s<***\n",ticket_file);
    for (i=0;i<cube_dim;i++)
      ticket_pool[proc_id ^ (1 << i)] = 1;
    first_lookup = lookup_next = (proc_id + 1) % cube_size;
    DBUG_RETURN(cube_dim);}


 DBUG_PRINT("TICKET",("Ticket file open"));
 do
   {while ((ch = getc(tf)) != '*')
      {if ((ch == '.') || (ch == EOF))
	 {for (i=0;i<cube_dim;i++)
	    ticket_pool[proc_id ^ (1 << i)] = 1;
	  first_lookup = lookup_next = (proc_id + 1) % cube_size;
	  DBUG_PRINT("TICKET",("ticket spec for subcube dim %d not found",cube_dim));
	  DBUG_RETURN(cube_dim);}}
    fscanf(tf,"%d",&number);}
 while (number != cube_dim);

 getc(tf);           /* here should follow a '\n' in the file */
 
 for(;;)
   {list1_index = 0;
    if (getc(tf) != '@')
      break;
    do
      {fscanf(tf,"%d",&number);
       DBUG_PRINT("TICKET",("found @ for %d",number));
       ch = getc(tf);
       if (ch == '-')
	 {fscanf(tf,"%d",&high);
	  for (i=number;i<=high;i++)
	    {for (found=j=0;j<list1_index;j++)
	       if (list1[j] == i)
		 found = 1;
	     if (found == 0)
	       {DBUG_PRINT("TICKET",("inserting %d into list1",i));
		list1[list1_index++] = i;}}
	  ch = getc(tf);}
       else
	 {for (found=j=0;j<list1_index;j++)
	    if (list1[j] == number)
	      found = 1;
	  if (found == 0)
	    {DBUG_PRINT("TICKET",("inserting %d into list1",number));
	     list1[list1_index++] = number;}}}
    while (ch == ',');
    
    if (ch != '(')
      abort_init("syntax error in ticket file");
    
    list2_index = 0;
    list_flag = 0;
    do
      {fscanf(tf,"%d",&number);
       ch = getc(tf);
       switch (ch)
	 {case '-':
	    if (list_flag)
	      abort_init("syntax error in ticket file");
	    else
	      {low = number;
	       list_flag = 1;}
	    break;
	  case ':':
	    if (list_flag)
	      {for (i=low;i<=number;i++)
		 {for (found=j=0;j<list2_index;j++)
		    if (list2[j] == i)
		      found = 1;
		  if (found == 0)
		    {DBUG_PRINT("TICKET",("inserting %d into list2",i));
		     list2[list2_index++] = i;}}
	       list_flag = 0;}
	    else
	      {for (found=j=0;j<list2_index;j++)
		 if (list2[j] == number)
		   found = 1;
	       if (found == 0)
		 {DBUG_PRINT("TICKET",("inserting %d into list2",number));
		  list2[list2_index++] = number;}}
	    fscanf(tf,"%d",&number);
	    for (i=0;i<list1_index;i++)
	      for (j=0;j<list2_index;j++)
		{if (list1[i] == proc_id)
		   ticket_pool[list2[j]] = number;
		 if (list2[j] == proc_id)
		   ticket_list[list1[i]] = number;}
	    ch = getc(tf);
	    list2_index = 0;
	    break;
	  case ';':
	  case ')':
	    for (j=0;j<list1_index;j++)
	      for (i=0;i<cube_dim;i++)
		{nb = list1[j] ^ (1 << i);
		 if (list1[j] == proc_id)
		   ticket_pool[nb] = number;
		 if (nb == proc_id)
		   ticket_list[list1[j]] = number;}
	    break;
	  case ',':
	    if (list_flag)
	      {for (i=low;i<=number;i++)
		 {for (found=j=0;j<list2_index;j++)
		    if (list2[j] == i)
		      found = 1;
		  if (found == 0)
		    {DBUG_PRINT("TICKET",("inserting %d into list2",i));
		     list2[list2_index++] = i;}}
	       list_flag = 0;}
	    else
	      {for (found=j=0;j<list2_index;j++)
		 if (list2[j] == number)
		   found = 1;
	       if (found == 0)
		 {DBUG_PRINT("TICKET",("inserting %d into list2",number));
		  list2[list2_index++] = number;}
	    break;}}}
    while (ch != ')');
    
    if (ch != ')')
      abort_init("syntax error in ticket file");
    
    for(i=0;i<cube_size;i++)
      {DBUG_PRINT("TICKET",("can receive %d processes from proc %d",ticket_list[i],i));
       tab_size += ticket_list[i];
       ticket_list[i]=0;}
    
    while ((ch = getc(tf)) == ';')
      {fscanf(tf,"%d",&number);
       if (list1[list2_index++] == proc_id)
	 first_lookup = lookup_next = number;}
    
    if (ch != '\n')
      abort_init("syntax error in ticket file");}
 
 if (lookup_next == -1)
   first_lookup = lookup_next = (proc_id + 1) % cube_size;
 
 DBUG_RETURN(tab_size);}

/*********************************************************************/
/*                                                                   */
/* process_terminate_all        terminates all processes             */
/*                                                                   */
/*********************************************************************/

void process_terminate_all()
{int i;
 PROCESS_CONTEXT * pid;

 for (i = 0;i < proc_tab_size;i++)
   if (PC_STATUS(process_table + i) == PS_WAITING)
     process_terminate(process_table + i);
 
 for(pid = ready_queue_first;pid;pid = pid -> next)
   process_terminate(pid);

#if ADV_SCHED
 for(pid = ready_queue_red_first;pid;pid = pid -> next)
   process_terminate(pid);
 for(pid = ready_queue_yellow_first;pid;pid = pid -> next)
   process_terminate(pid);
#endif /* ADV_SCHED */
 
 if (curr_pid)
   process_terminate(curr_pid);}

#endif /* D_MESS_ONLY */
