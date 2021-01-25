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

#if TIME_SLICE
#include "spec_sched.h"
#endif

#if D_NN_SCHED
#include "nc_nn_sched.h"
#endif

#if D_MESS
#include "d_mess_io.h" 
#endif

#define NULL 0
#define MAXPROCESSORS 32

/* extern variables */
extern INSTR *code;
extern DStackDesc D_S_A, D_S_H, D_S_R, D_S_I;
extern DStackDesc *ps_w, *ps_a;
#if WITHTILDE
extern DStackDesc *ps_t, *ps_r;
#endif
extern int _redcnt;
extern int proc_id;
extern int cube_dim;
extern int sig_msg_flag;

/* global variables */

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

#else
extern unsigned int dist_stack_elems;
extern int dist_stack_free;
#if WITHTILDE
extern unsigned int dist_tilde_elems;
extern int dist_tilde_free;
#endif /* WITHTILDE */
extern DIST_TYPE dist_type;
extern INSTR *dist_instr;
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

PROCESS_CONTEXT * ready_queue_first;    /* first element of the ready_queue      */
PROCESS_CONTEXT * ready_queue_last;     /* last element of the  ready-queue      */
PROCESS_CONTEXT * released_queue_first; /* first element of the released_queue   */
PROCESS_CONTEXT * released_queue_last;  /* last element of the released_queue    */
PROCESS_CONTEXT * process_table;

#else

extern PROCESS_CONTEXT * ready_queue_first;
extern PROCESS_CONTEXT * ready_queue_last;
extern PROCESS_CONTEXT * released_queue_first;
extern PROCESS_CONTEXT * released_queue_last;
extern PROCESS_CONTEXT * process_table;

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

 new_context = released_queue_first;
 released_queue_first = released_queue_first->next;
 if (released_queue_last == new_context)
   released_queue_last = NULL;
 new_context->status = PS_READY;
 new_context->pc = pc;
 new_context->ppid = ppid;
 new_context->red_count = red_count;
 new_context->start_red_count = red_count;
 new_context->switched = NULL;
 init_d_stacks(&(new_context->D_S_H),
               &(new_context->D_S_A),
               &(new_context->D_S_R),
               &(new_context->D_S_I));
 new_context->next = NULL;
 if (!ready_queue_first)
   ready_queue_first = new_context;
 if (ready_queue_last)
   ready_queue_last->next = new_context;
 ready_queue_last = new_context;
 DBUG_PRINT("SCHED",("Habe Prozess %x erzeugt...",new_context));

#if D_NN_SCHED
  proc_stat[P_READY]++;
#endif


#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSCHED)
 MPRINT_PROC_CREATE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) new_context);
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

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_process_terminate");
#else
 DBUG_ENTER("process_terminate");
#endif

 /* remove remaining elements from the I-Stack */
/* for (i=0;i<d_stacksize(&(pid->D_S_I));i++)
   {top = D_POPSTACK(pid->D_S_I);
    DBUG_PRINT("TERM",("throwing away stackelem %d (%x) from I-stack",top,top));
    if (isdesc(top))
      T_DEC_REFCNT((T_PTD)top);}
*/
  free_d_stacks(&(pid->D_S_H),
		&(pid->D_S_A),
		&(pid->D_S_R),
		&(pid->D_S_I));
  if (!released_queue_first)
    released_queue_first = pid;
  if (released_queue_last)
    released_queue_last->next = pid;
  released_queue_last = pid;
  curr_pid = NULL;
  
#if D_MESS
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

 ready_queue_first = pid->next;
 if (ready_queue_last == pid)
   ready_queue_last = NULL;
 pid->next = NULL;
 code = pid->pc;
 curr_pid = pid;

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
   {ps_w = &D_S_I;
    ps_a = &D_S_A;}
 else
   {ps_w = &D_S_A;
    ps_a = &D_S_I;}
#if WITHTILDE
 if (pid->switched & R_T_SWITCHED)
   {ps_r = &D_S_H;
    ps_t = &D_S_R;}
 else
   {ps_r = &D_S_R;
    ps_t = &D_S_H;}
#endif

 DBUG_PRINT("SCHED",("Habe Prozess %x gestartet; curr_pid: %x",pid,curr_pid));
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
 
 pid->pc = code;
 pid->red_count = _redcnt;
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
#ifdef D_MESS_ONLY
DBUG_ENTER("m_process_wakeup");
#else
DBUG_ENTER("process_wakeup");
#endif

 pid->status = PS_READY;
 if (!ready_queue_first)
   ready_queue_first = pid;
 if (ready_queue_last)
   ready_queue_last->next = pid;
 ready_queue_last = pid;
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

 while (!ready_queue_first)
   {while ((!ready_queue_first) && (counter++ < 100000))
      if (sig_msg_flag) {
	msg_check();
      }
    counter = 0;
    if (!ready_queue_first)
      msg_check();

#if NCDEBUG
    DBNCclient();
#endif

  }

#if TIME_SLICE
  RESET_TIMER;
#endif

#if (D_SLAVE && D_MESS && D_MSCHED)
 (*d_m_process_run)(ready_queue_first);
#else
 process_run(ready_queue_first);
#endif
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

#if NCDEBUG
 if (DBNCgetdistlevel())
   DBNCsetdistlevel(1);
 else
#endif
/* ------------------ dmk ------------------ */
#if D_NN_SCHED
   { if ((lookup_next=nn_decide())==-1)
#else
   {if (ticket_pool_counter == 0)
#endif
/* ------------------ dmk end -------------- */
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
       DBUG_RETURN(label1);}

/* ------------------ dmk ------------------ */
#if D_NN_SCHED
    else {
#else
    for(;;)
      {if ((ticket_pool[lookup_next] > 0) /*&& (lookup_next != proc_id)*/)
#endif
/* ------------------ dmk end -------------- */
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
	  DBUG_PRINT("DIST",("dummy pushed, Top of A-Stack: %x",ps_w->TopofStack));
	  
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

 list1 = (int *)malloc(cube_size*sizeof(int));
 list2 = (int *)malloc(cube_size*sizeof(int));
 ticket_list = (int *)malloc(cube_size*sizeof(int));

 if ((!list1) || (!list2) || (!ticket_list))
   abort_init("out of memory while reading ticket file");

 for (i=0;i<cube_size;i++)
   ticket_pool[i] = ticket_list[i] = 0;

 tf = fopen("red.tickets","r");
 if (!tf)
   {DBUG_PRINT("TICKET",("Ticket file not found..."));
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
 
 if (curr_pid)
   process_terminate(curr_pid);}

#endif /* D_MESS_ONLY */
