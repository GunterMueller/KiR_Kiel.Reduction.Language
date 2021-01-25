/***************************************************************************/
/*                                                                         */
/*   file     : spec_sched.c                                               */
/*                                                                         */
/*   contents : functions for time-slicing scheduling                      */
/*                                                                         */
/*   last change:                                                          */
/*                                                                         */
/*         13.09.1994  (creation... really)                                */
/*         see rcs-logs for any further details                            */
/*                                                                         */
/***************************************************************************/

#if D_SPEC

/* includes */

#include "rstdinc.h"
#include "rncstack.h"
#include "rheapty.h"
#include "rncsched.h"
#include "rncmessage.h"
#include "rstelem.h"
#include "rextern.h"
#include <stdio.h>
#include "dbug.h"
#include "spec_sched.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/* extern variables */

extern int _spec_redcnt;
extern int spec_sched_status;
extern PROCESS_CONTEXT *in_use_queue_first, *spec_in_use_queue_first;

void speculative_scheduler(); /* well that's a variable ! */
void spec_remove_control_pid();
void spec_send_vital();

/* extern void initialize_fred_fish(); */

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
extern unsigned int dist_stack_elems;
extern int dist_stack_free; 
extern PROCESS_CONTEXT *curr_pid;
#if WITHTILDE
extern unsigned int dist_tilde_elems;        /* rncsched.c */
extern int dist_tilde_free;          /* rncsched.c */
#endif /* WITHTILDE */
extern DIST_TYPE dist_type;          /* rncsched.c */
extern INSTR * dist_instr;           /* rncsched.c */
extern FILE *debugger;

/* functions */

void next_node_for_speculative_process();
void dump_processes();
void abdul();
void spec_gather_results();
#if D_SPEC_TICKETS
void spec_tickets_distribute();
PTR_DESCRIPTOR dequeue_postpone();
#endif

/* variables */

#if D_SPEC_TICKETS
int tickets_desc;
#endif

int real_start_redcount;
int spec_kill_no_steps; 
int spec_nondet;
int nospecnr;
int spec_retrans;
int spec_multi_results; /* for multiple speculative results */
int spec_sched_status; /* status of the (new) speculative scheduler after receiving a message */
int spec_next_proc;

#if D_SPEC_TICKETS
#define MAXPROCESSORS 32
int spec_ticket_pool[MAXPROCESSORS];
int spec_lookup_next=-1;
int spec_first_lookup;
int spec_proc_tab_size;
int spec_ticket_pool_counter = 0;
extern char spec_ticket_file[];

PTR_DESCRIPTOR spec_frame;
int spec_post_proc;
#endif /* D_SPEC_TICKETS */

PROCESS_CONTEXT *spec_ready_queue_first; /* first element of the ready_queue      */
PROCESS_CONTEXT *spec_ready_queue_last;     /* last element of the  ready-queue      */
PROCESS_CONTEXT *spec_released_queue_first; /* first element of the released_queue   */
PROCESS_CONTEXT *spec_released_queue_last;  /* last element of the released_queue    */
PROCESS_CONTEXT *spec_process_table;

PROCESS_CONTEXT *spec_in_use_queue_first; /* first element of the released_queue   */
PROCESS_CONTEXT *spec_in_use_queue_last;  /* last element of the released_queue    */

int ack_red_count, max_spec_sons, max_spec_procs, spec_global, spec_failcase, spec_mspecial;

int spec_father_slot;

int spec_control[10];

/* int ts_counter=0;          somewhat very old declaration */

/* functions */

/*************************************************************************************/
/* spec_distribute starts a speculative process on a remote processor/node */
/* the major difference between this function and the (conservative) distribute */
/* is that in spec_distribute, it doesn't matter if tickets are available, the */
/* distributions will be performed in a kind of round robin manner */

void spec_distribute(INSTR *label, int send_frame_size, int tilde_frame_size)
{
  DBUG_ENTER("spec_distribute");

  dist_stack_elems = send_frame_size;
  dist_stack_free = 0;
#if WITHTILDE
  dist_tilde_elems = tilde_frame_size;
  dist_tilde_free = 0;
#endif /* WITHTILDE */
  dist_type = DIST_N;
  dist_instr = label;

  /* initializing speculative processes, mark this current process as speculative */
  /* if this wasn't done already */

  DBUG_PRINT("SPEC_DIST", ("dist_instr = %x", dist_instr));

/* fprintf(debugger, "_redcnt is %d !\n", _redcnt); fflush(debugger); */

  DBUG_PRINT("SPEC_DIST", ("status of process is %x", curr_pid->specmask1));

  if ((T_SPEC_FAIL(curr_pid)==1) && !T_SPEC_NO_BSTEPS(curr_pid)) {
    post_mortem ("unsupported: spawning again with FAIL set...");
    }

  if (T_SPEC_UNRED_GUARD(curr_pid)==1) {
    post_mortem ("unsupported: spawning again with UNRED_GUARD set...");
    }

  if (T_SPEC_WINNER(curr_pid)==1) {
    post_mortem ("unsupported: spawning again with WINNER set...");
    }

  if ((T_SPEC_PROCESS(curr_pid)==1) && !T_SPEC_INSYNC(curr_pid))  {
    post_mortem ("unsupported: spawning again while already out of sync...");
    }

  if (!T_SPEC_PROCESS(curr_pid) && !T_SPEC_GRANDDAD(curr_pid)) {
    /**************************/
    DBUG_PRINT("SPEC_DIST", ("formerly vital process goes granddad-speculative"));
    PC_SET_SPEC_GRANDDAD(curr_pid);
 
    PC_SET_SPEC_SUM(curr_pid,0);

    /* allocate space for the son processes */
    if (!(curr_pid->processes = malloc(2*sizeof(int)*max_spec_sons)))
      post_mortem ("could not allocate space for speculative son processes...");

    if (curr_pid->son_results == NULL) {

    /* allocate space for the son results */
    if (!(curr_pid->son_results = malloc(sizeof(int)*max_spec_sons)))
      post_mortem ("could not allocate space for speculative son results...");

    /* allocate space for the son redcnts */
    if (!(curr_pid->son_redcnts = malloc(sizeof(int)*max_spec_sons)))
      post_mortem ("could not allocate space for speculative son redcnts...");

    /* allocate space for the son status */
    if (!(curr_pid->son_status = malloc(sizeof(int)*max_spec_sons)))
      post_mortem ("could not allocate space for speculative son status...");

    }

    /* allocate space for the controller */
    if (!(curr_pid->control = malloc(sizeof(SPEC_CONTROL_STRUC))))
      post_mortem ("could not allocate space for speculative control...");

    if (!(curr_pid->control->data = malloc(sizeof(int)*2*max_spec_procs)))
      post_mortem ("could not allocate space for speculative control data...");
    curr_pid->control->leaves = 0;
    curr_pid->control->have_ack = 0;
    curr_pid->control->result = 0;

    curr_pid->control_pid = curr_pid;
    curr_pid->control_node = proc_id;

    DBUG_PRINT("SPEC_DIST", ("space allocated..."));
    }
  else if (T_SPEC_GRANDDAD(curr_pid)==1){
    DBUG_PRINT("SPEC_DIST", ("granddad spawns another process")); 
    }
  else if (T_SPEC_LEAF(curr_pid)==1) {

/* fprintf (debugger, "Hey bastards ! Evolving !\n"); fflush(debugger); */

    DBUG_PRINT("SPEC_DIST", ("leaf spec process evolves to dad spec"));
    PC_CLEAR_SPEC_LEAF(curr_pid);
    PC_SET_SPEC_DAD(curr_pid);
    PC_SET_SPEC_SUM(curr_pid,0);
    } 

  /* increase counter for speculative sons */
  DBUG_PRINT("SPEC_DIST", ("specmask1 is %x, value %d", curr_pid->specmask1, PC_GET_SPEC_SPAWNED(curr_pid)));

  spec_father_slot = PC_GET_SPEC_SPAWNED(curr_pid);

  DBUG_PRINT("SPEC_DIST", ("slot number is %d", spec_father_slot));

  PC_SET_SPEC_SPAWNED(curr_pid,PC_GET_SPEC_SPAWNED(curr_pid)+1);

  if (PC_GET_SPEC_SPAWNED(curr_pid) == max_spec_sons)
    post_mortem ("Too many speculative sons...");

  DBUG_PRINT("SPEC_DIST", ("specmask1 is %x", curr_pid->specmask1));
  
  /* generate the target node */

  DBUG_PRINT("SPEC_DIST", ("next_proc was: %d", spec_next_proc));

  next_node_for_speculative_process();
  
  DBUG_PRINT("SPEC_DIST", ("next_proc is now: %d", spec_next_proc));

  DBUG_PRINT("SPEC_DIST",("distributing to node %d",spec_next_proc));

#if (D_SLAVE && D_MESS && D_MCOMMU)
  (*d_m_msg_send)(spec_next_proc,MT_NEW_PROCESS);
#else
  msg_send(spec_next_proc,MT_NEW_PROCESS);
#endif 

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* choosing the next processor for a speculative node is extracted from */
/* the function above, maybe it'll changed sometime */

void next_node_for_speculative_process()
{
  DBUG_ENTER("next_node_for_speculative_process");

  if (cube_dim > 0) {
    if (++spec_next_proc >=(1 << cube_dim))
      spec_next_proc = 0;
    if (spec_next_proc == proc_id)
      if (++spec_next_proc == (1 << cube_dim))
        spec_next_proc = 0;
    }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_acknowledge sends a message to the controller of this speculative thread */
/* for registering ALL leaf-processes and deleting the new (born ;-) father process */

#if D_SPEC_TICKETS
void spec_send_acknowledge(target, controlpid, father, flag, desc)
#else
void spec_send_acknowledge(target, controlpid, father)
#endif
int target, controlpid;

#if D_SPEC_TICKETS
int flag; PTR_DESCRIPTOR desc;
#endif

PROCESS_CONTEXT *father;
{  
 int i,k;
 int tmp1, tmp2, tmp3;

#if D_SPEC_TICKETS
 int tmp6, tmp7;
#endif

 DBUG_ENTER("spec_send_acknowledge");

/*****/
/* if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_acknowledge: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_acknowledge: spec_sched_status!=0 !!!");
   } */
/*****/


 DBUG_PRINT("ssa", ("sending acknowledge to node %d, pid %x, process pid is %x", target, controlpid, father));

 TAG_SPECMSG_ACK;
 spec_control[1] = controlpid;
 spec_control[2] = father;
 spec_control[3] = PC_GET_SPEC_SPAWNED(father);
#if D_SPEC_TICKETS
 spec_control[4] = !T_SPEC_SIGNOFF(father);
#else
 spec_control[4] = T_SPEC_GRANDDAD(father);
#endif

/* fprintf(debugger, "0x%x: ssa, spec_control[4] (SIGNOFF or not) is %d (0 remove, 1 don't remove)\n", curr_pid, spec_control[4]); fflush(debugger);*/

 spec_control[5] = T_ACKNOWLEDGE(father);
#if D_SPEC_TICKETS
 if (flag == 1)
   spec_control[4] = 1; /* don't remove the father process */
 spec_control[6] = flag;
 spec_control[7] = desc;
#endif
 
 if (target == proc_id) { /* don't send control messages to local processes */

/* fprintf(debugger, "0x%x: sending ACKNOWLEDGE locally to (0x%x, 0x%x) from 0x%x (%d processes)\n", curr_pid, controlpid, target, father, spec_control[3]); fflush(debugger); */
/* fprintf(debugger, "0x%x: receiving ACKNOWLEDGE locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, controlpid, target, father); fflush(debugger); */

   /* don't forget to measure this thing... */
#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_ACK", proc_id, controlpid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc fr_pid  len  to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_ACK", proc_id, father, 24, controlpid);
#endif

   DBUG_PRINT("ssa", ("processing acknowledge locally..."));
   DBUG_PRINT("ssa", ("spec_multi_results: %d", spec_multi_results));
   DBUG_PRINT("MSG", ("results already stored: %d", ((PROCESS_CONTEXT *)controlpid)->control->result));

   tmp1 = spec_control[1];
   tmp3 = spec_control[3];
#if D_SPEC_TICKETS
   tmp6 = spec_control[6];
   tmp7 = spec_control[7];
#endif

   if (spec_control[4]==1) { /* it was a granddad speculative process */

/*fprintf(debugger, "0x%x: (spec_control[4]==1) is true !\n"); fflush(debugger);*/

     DBUG_PRINT("ssa", ("it was a granddad speculative process..."));
     } else {

/*fprintf(debugger, "0x%x: (spec_control[4]==1) is false !\n", curr_pid); fflush(debugger);*/

     DBUG_PRINT("ssa", ("normal dad speculative process, remove..."));
     spec_remove_control_pid(spec_control[1], spec_control[2], proc_id, 0);
     }

   if ((((PROCESS_CONTEXT *)tmp1)->control->leaves+tmp3)>max_spec_procs)
           post_mortem("Too many speculative processes in syncronisation !");

#if D_SPEC_TICKETS
   if (tmp6 == 1) {
     DBUG_PRINT("ssa", ("sending an postpone-acknowledge to the control locally !"));
     ((PROCESS_CONTEXT *)tmp1)->control->data[(((PROCESS_CONTEXT *)tmp1)->control->leaves*2)+0] = SPEC_SF_NUMB_T(*(PTR_DESCRIPTOR)tmp7);
     ((PROCESS_CONTEXT *)tmp1)->control->data[(((PROCESS_CONTEXT *)tmp1)->control->leaves*2)+1] = SPEC_SF_NUMB_A(*(PTR_DESCRIPTOR)tmp7);

     ((PROCESS_CONTEXT *)tmp1)->control->leaves++;

/* fprintf(debugger, "0x%x: ACK; pp-registering 0x%x from node %d, altogether %d\n", curr_pid, SPEC_SF_NUMB_T(*(PTR_DESCRIPTOR)tmp7), SPEC_SF_NUMB_A(*(PTR_DESCRIPTOR)tmp7), ((PROCESS_CONTEXT *)tmp1)->control->leaves); fflush(debugger); */

/* fprintf(debugger, "lAp %d %d\n", ((PROCESS_CONTEXT *)tmp1)->control->leaves, ((PROCESS_CONTEXT *)tmp1)->control->have_ack); fflush(debugger);*/

     } else {
     DBUG_PRINT("ssa", ("registering normal acknowledge..."));
#endif
   for (i=0; i<tmp3; i++) {
     ((PROCESS_CONTEXT *)tmp1)->control->data[(((PROCESS_CONTEXT *)
         tmp1)->control->leaves*2)+2*i] = father->processes[(i*2)];
     ((PROCESS_CONTEXT *)tmp1)->control->data[(((PROCESS_CONTEXT *)
         tmp1)->control->leaves*2)+2*i+1] = father->processes[(i*2)+1];
     DBUG_PRINT("ssa", ("registering pid %x of node %d as number %d",
         father->processes[(i*2)], father->processes[(i*2)+1], i+
         ((PROCESS_CONTEXT *)tmp1)->control->leaves));

/* fprintf(debugger, "0x%x: ACK; registering 0x%x from node %d as number %d\n", curr_pid, father->processes[(i*2)], father->processes[(i*2)+1], i+((PROCESS_CONTEXT *)tmp1)->control->leaves); fflush(debugger); */

     }
   ((PROCESS_CONTEXT *)tmp1)->control->leaves += PC_GET_SPEC_SPAWNED(father);

/* fprintf(debugger, "lA %d %d\n", ((PROCESS_CONTEXT *)tmp1)->control->leaves, ((PROCESS_CONTEXT *)tmp1)->control->have_ack); fflush(debugger); */

#if D_SPEC_TICKETS
    }
#endif
   DBUG_PRINT("ssa", ("total processes: %d", ((PROCESS_CONTEXT *)tmp1)->
                      control->leaves));

   spec_control[1] = proc_id;
   spec_control[2] = father;
   spec_control[4] = controlpid;
#if D_SPEC_TICKETS
   spec_control[8] = tmp6;
   spec_control[9] = tmp7;
#endif

/*fprintf(debugger, "0x%x: ACK processed, %d leaves registered, %d have acknowledged\n", curr_pid, ((PROCESS_CONTEXT *)tmp1)->control->leaves, ((PROCESS_CONTEXT *)tmp1)->control->have_ack); fflush(debugger);*/

   spec_sched_status = 2;
   speculative_scheduler();
   }

 else {
#if (D_SLAVE && D_MESS && D_MCOMMU)
   (*d_m_msg_send)(target,MT_CONTROL);
#else
   msg_send(target,MT_CONTROL);
#endif
   }

 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* dump_processes is a debug function */

void dump_processes(control)
PROCESS_CONTEXT *control;
{
  int i,k;

  DBUG_ENTER("dump_processes");

/* fprintf(debugger, "************** dump_processes: control %x, control->control %x\n", control, control->control); fflush(debugger);*/

/*fprintf (debugger, "** %d processes are stored, %d ack:\n", control->control->leaves, control->control->have_ack);  */

  for (i=0; i<control->control->leaves; i++) {
/*  fprintf (debugger, "** pid %x on node %d, ack: %d\n", control->control->data[(i*2)], GET_DATA_NODE(control->control->data[(i*2)+1]), T_DATA_ACK(control->control->data[(i*2)+1])); */
    }
/* fflush(debugger); */
  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_goon sends a message to a leaf process for more available reduction steps */

void spec_send_goon(target, targetpid, mypid)
int target, targetpid, mypid;
{
 DBUG_ENTER("spec_send_goon");

 DBUG_PRINT("ssg", ("sending goon message to node %d pid %x", target, targetpid));

/*****/
/*  if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_goon: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_goon: spec_sched_status!=0 !!!");
   } */
/*****/

 TAG_SPECMSG_GOON;
 spec_control[1] = targetpid;
 spec_control[2] = mypid;

/* fprintf(debugger, "sG 0x%x,%d\n", targetpid, target); fflush(debugger); */

/* fprintf(debugger, "0x%x: sending GOON to (0x%x, 0x%x) from 0x%x\n", curr_pid, targetpid, target, mypid); fflush(debugger);*/

 if (target == proc_id) { /* don't send control messages to local processes */

/*  fprintf(debugger, "0x%x: sending GOON locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, targetpid, target, mypid); fflush(debugger); */
/*  fprintf(debugger, "0x%x: receiving GOON locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, targetpid, target, mypid); fflush(debugger);  */

   /* don't forget to measure this thing... */
#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_GOON", proc_id, targetpid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc fr_pid  len  to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_GOON", proc_id, mypid, 24, targetpid);
#endif

   DBUG_PRINT("ssg", ("processing goon locally..."));
   spec_sched_status = 4;
   speculative_scheduler();
   } else {
  spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
   (*d_m_msg_send)(target, MT_CONTROL);
#else
   msg_send(target, MT_CONTROL);
#endif
   spec_message_size = MESSAGE_SIZE_OLD;
   }

 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_hereami sends a message to the direct father process for registering the */
/* created speculative leaf process */

#if D_SPEC_TICKETS
void spec_send_hereami(target, fatherpid, mypid, flaggy, desc)
#else
void spec_send_hereami(target, fatherpid, mypid)
#endif
int target, fatherpid, mypid;
#if D_SPEC_TICKETS
int flaggy;
PTR_DESCRIPTOR desc;
#endif
{
 DBUG_ENTER("spec_send_hereami");

 DBUG_PRINT("ssh", ("sending hereami-message to node %d pid %x from pid %x", target, fatherpid, mypid));

/*****/
/*  if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_hereami: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_hereami: spec_sched_status!=0 !!!");
   } */
/*****/

 TAG_SPECMSG_HEREAMI;
 spec_control[1] = fatherpid;
 spec_control[2] = mypid;
 spec_control[3] = proc_id;
#if D_SPEC_TICKETS
 spec_control[4] = flaggy;
 spec_control[5] = desc;
#endif

 if (target == proc_id) { /* don't send control messages to local processes */

/* fprintf(debugger, "0x%x: sending HEREAMI locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, fatherpid, target, mypid); fflush(debugger);*/
/* fprintf(debugger, "0x%x: receiving HEREAMI locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, fatherpid, target, mypid); fflush(debugger); */

#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_HERE", proc_id, fatherpid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc from_pid len to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROLHERE", proc_id, mypid, 24, fatherpid);
#endif

   DBUG_PRINT("ssh", ("processing hereami locally..."));
   spec_sched_status = 1;
   speculative_scheduler();
   } else {
   spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
   (*d_m_msg_send)(target, MT_CONTROL);
#else
   msg_send(target, MT_CONTROL);
#endif
   spec_message_size = MESSAGE_SIZE_OLD;
   }

 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_make_vital sends a vital message to the one left leaf process */

void spec_make_vital(controlpid)
PROCESS_CONTEXT *controlpid;
{
 int vnode, vpid;

 DBUG_ENTER("spec_make_vital");

/* initialize_fred_fish(); */

 DBUG_PRINT("smv", ("vitalize a process..."));

 /* hmmmm... first ex-GRANDDAD the originating process */

 PC_CLEAR_SPEC_GRANDDAD(controlpid); /* for WHAT reason ?! */

 vpid = controlpid->control->data[0];
 vnode = controlpid->control->data[1];

 DBUG_PRINT("smv", ("vitalizing pid %x on node %x", vpid, vnode));

 spec_send_vital(vnode, vpid, controlpid, controlpid->control->result);

 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_vital sends a message to a leaf process making it vital ;) */

void spec_send_vital(target, targetpid, mypid, results)
int target, targetpid, mypid;
{
 DBUG_ENTER("spec_send_vital");

/* initialize_fred_fish(); */

 DBUG_PRINT("ssg", ("sending vital message to node %d pid %x", target, targetpid));

/* fprintf(debugger, "vitalizing process %x on node %d, I have %d results...\n", targetpid, target, results); fflush(debugger); */

/*****/
/*  if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_vital: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_vital: spec_sched_status!=0 !!!");
   } */
/*****/

 TAG_SPECMSG_VITAL;
 spec_control[1] = targetpid;
 spec_control[2] = mypid;
 spec_control[3] = results;
 
 if (target == proc_id) { /* don't send control messages to local processes */
 
/*fprintf(debugger, "0x%x: sending VITAL locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, targetpid, target, mypid); fflush(debugger); */
/*fprintf(debugger, "0x%x: receiving VITAL locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, targetpid, target, mypid); fflush(debugger); */
 
   /* don't forget to measure this thing... */
#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_VITAL", proc_id, targetpid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc fr_pid  len  to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_VITAL", proc_id, mypid, 24, targetpid);
#endif
 
   DBUG_PRINT("ssg", ("processing vital locally..."));
   spec_sched_status = 10;
   speculative_scheduler();
   } else {
  spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
   (*d_m_msg_send)(target, MT_CONTROL);
#else
   msg_send(target, MT_CONTROL);
#endif
   spec_message_size = MESSAGE_SIZE_OLD;
   }
 
 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_barrier sends a message to the control process and announces that */
/* this process has passed the first barrier */

void spec_send_barrier(mypid)
PROCESS_CONTEXT* mypid;
{
  DBUG_ENTER("spec_send_barrier");
  
  DBUG_PRINT("ssb", ("sending barrier message from pid %x to node %d", mypid, mypid->control_node));

/*****/
/* if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_barrier: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_barrier: spec_sched_status!=0 !!!");
   } */
/*****/

  TAG_SPECMSG_BARR;
  spec_control[1] = mypid->control_pid;
  spec_control[2] = mypid;
  spec_control[3] = proc_id;

/* fprintf(debugger, "0x%x: sending BARR, source is (0x%x,%d), target is control (0x%x,%d)\n", curr_pid, mypid, proc_id, mypid->control_pid, mypid->control_node); fflush(debugger);  */

  if (mypid->control_node == proc_id) { /* don't send control messages to local processes */

/*  fprintf(debugger, "0x%x: sending BARR locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, mypid->control_pid, mypid->control_node, mypid); fflush(debugger);*/
/*  fprintf(debugger, "0x%x: receiving BARR locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, mypid->control_pid, mypid->control_node, mypid); fflush(debugger); */

#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_BARR", proc_id, mypid->control_pid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc from_pid len to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_BARR", proc_id, mypid, 24, mypid->control_pid);
#endif

    DBUG_PRINT("ssb", ("processing barrier locally..."));
    spec_sched_status = 3;
    speculative_scheduler();
    } else {
    spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
    (*d_m_msg_send)(mypid->control_node, MT_CONTROL);
#else
    msg_send(mypid->control_node, MT_CONTROL);
#endif
    spec_message_size = MESSAGE_SIZE_OLD;
    }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_term sends a message to a leaf (or father node) to terminate the process */

void spec_send_term(node, pid, mypid, flag)
int node, pid, mypid, flag;
{
  DBUG_ENTER("spec_send_term");

  DBUG_PRINT("sst", ("sending termination message to node %d pid %x", node, pid));

/*****/
/* if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_term: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_term: spec_sched_status!=0 !!!");
   } */
/*****/

  TAG_SPECMSG_TERM;
  spec_control[1] = pid;
  spec_control[2] = mypid;
  spec_control[3] = flag;

/* fprintf(debugger, "0x%x: sending TERM to 0x%x,%d !\n", curr_pid, pid, node); fflush(debugger); */

  if (node == proc_id) { /* don't send control messages to local processes */

/* fprintf(debugger, "0x%x: sending TERM locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, pid, node, mypid); fflush(debugger); */
/* fprintf(debugger, "0x%x: receiving TERM locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, pid, node, mypid); fflush(debugger);*/

#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], "MT_CONTROL_TERM", proc_id, pid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[2], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc from_pid len to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_TERM", proc_id, mypid, 24, pid);
#endif

    DBUG_PRINT("sst", ("processing term control locally..."));
    spec_sched_status = 7;
    speculative_scheduler();
    } else {
    spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
    (*d_m_msg_send)(node, MT_CONTROL);
#else
    msg_send(node, MT_CONTROL);
#endif
    spec_message_size = MESSAGE_SIZE_OLD;
    }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_start sends a message to a leaf process giving the permission to start working */

#if D_SPEC_TICKETS
void spec_send_start(node, pid, redsteps, specbarrier, mypid, specredcount, flag, desc)
#else
void spec_send_start(node, pid, redsteps, specbarrier, mypid, specredcount)
#endif
int node, pid, redsteps, specbarrier, mypid, specredcount;
#if D_SPEC_TICKETS
int flag; 
PTR_DESCRIPTOR desc;
#endif
{
  DBUG_ENTER("spec_send_start");

  DBUG_PRINT("sss", ("sending start message to node %d pid %x, redsteps %d, specbarrier %d, mypid %x, specredcount %d", node, pid, redsteps, specbarrier, mypid, specredcount));

/*****/
/* if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_start: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_start: spec_sched_status!=0 !!!");
   } */
/*****/

  TAG_SPECMSG_START;
  spec_control[1] = pid;
  spec_control[2] = redsteps;
  spec_control[3] = specbarrier;
  spec_control[4] = mypid;
  spec_control[5] = specredcount;
  spec_control[6] = ((PROCESS_CONTEXT *)mypid)->control_pid;
  spec_control[7] = ((PROCESS_CONTEXT *)mypid)->control_node;
#if D_SPEC_TICKETS
  spec_control[8] = flag;
  spec_control[9] = desc;
#endif

/*  fprintf(debugger, "%x: sending START to (%x,%d), redsteps %d, specredsteps %d\n", mypid, pid, node, redsteps, specredcount); fflush(debugger); */

/*  fprintf(debugger, "sending START: control_pid is %x, control_node is %d !\n", spec_control[6], spec_control[7]); fflush(debugger);*/

  if (node == proc_id) { /* don't send control messages to local processes */

/*  fprintf(debugger, "0x%x: sending START locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, pid, node, mypid); fflush(debugger); */
/* fprintf(debugger, "0x%x: receiving START locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, pid, node, mypid); fflush(debugger); */

#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[4], "MT_CONTROL_START", proc_id, pid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, spec_control[4], 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc from_pid len to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_START", proc_id, mypid, 24, pid);
#endif

    DBUG_PRINT("ssb", ("processing start locally..."));
    spec_sched_status = 6;
    speculative_scheduler();
    } else {
    spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
    (*d_m_msg_send)(node, MT_CONTROL);
#else
    msg_send(node, MT_CONTROL);
#endif
    spec_message_size = MESSAGE_SIZE_OLD;
    }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_send_start_sons sends a spec_send_start message to all newly created son 
   proceses */

#if D_SPEC_TICKETS
void spec_send_start_sons(pid, flag, desc)
#else
void spec_send_start_sons(pid)
#endif
PROCESS_CONTEXT *pid;
#if D_SPEC_TICKETS
int flag;
PTR_DESCRIPTOR desc;
#endif
{
  int i,k, rsteps, sbits;

  DBUG_ENTER("spec_send_start_sons");

  DBUG_PRINT("ssss", ("sending to all sons of pid %x the START message...", pid));

#if D_SPEC_TICKETS
  if (flag == 0) {
  
  DBUG_PRINT("ssss", ("no post_proc message..."));

  if (T_SPEC_SIGNOFF(pid)!=1) {
    DBUG_PRINT("spec_send_start_sons", ("process is NOT signing off, don't clear INSYNC !"));
    PC_SET_SPEC_INSYNC(pid); /* it wasn't even set if the process is carrying the central control */
    } else {
#endif

  PC_CLEAR_SPEC_INSYNC(pid); /* the father is no longer controlled by central */

#if D_SPEC_TICKETS
    DBUG_PRINT("spec_send_start_sons", ("process IS signing off, clear INSYNC !"));
    }
    } else {
    DBUG_PRINT("ssss", ("post_proc message, ignore some stuff..."));
    }
#endif

  sbits = 0;
  if (T_ACKNOWLEDGE(pid)==1) {  
    DBUG_PRINT("ssss", ("transmitting ACK-bit !"));
    sbits+=HAVE_ACKNOWLEDGED;
    } 
  if (T_SPEC_NO_BSTEPS(pid)==1) {
    DBUG_PRINT("ssss", ("transmitting NO_BSTEPS !"));

/* fprintf(debugger, "0x%x: transmitting NO_BSTEPS to sons !\n", curr_pid); fflush(debugger); */

    sbits+=SPEC_NO_BSTEPS;
    }
#if D_SPEC_TICKETS
  if ((flag == 1) && (pid->ffound == 1) && (spec_multi_results == 0)) {
    
/* fprintf(debugger, "0x%x: set son of 0x%x to fail, 'cause ffound is 'set ! ;-)\n", curr_pid, pid); fflush(debugger); */
   sbits+=SPEC_NO_BSTEPS;

    }
  if (T_SPEC_FAIL(pid)==1) {  
    DBUG_PRINT("ssss", ("transmitting FAIL also..."));

/* fprintf(debugger, "0x%x: transmitting FAIL to sons !\n", curr_pid); fflush(debugger); */

    sbits+=SPEC_FAIL;
    }
  if (flag == 1) {
    DBUG_PRINT("ssss", ("post_proc, ignore GUARD_SON !"));

/* fprintf(debugger, "0x%x: starting postponed process (0x%x,%d) !\n", curr_pid, SPEC_SF_NUMB_T(*desc), SPEC_SF_NUMB_A(*desc)); fflush(debugger); */

    }
  else
#endif
  if (T_SPEC_GUARD(pid)==1) {
    DBUG_PRINT("ssss", ("transmitting GUARD_SON, GUARD was set !"));

#if D_SPEC_TICKETS
    PC_CLEAR_SPEC_GUARD(pid); /* has to be cleared AFTER executing SPECWAIT */
#endif

/*  fprintf(debugger, "ssss: transmitting GUARD_SON, GUARD was set !\n"); fflush(debugger); */

    sbits+=SPEC_GUARD_SON;
    }

  sbits+=SPEC_INSYNC; /* sons are under the control of the central...err... control */

/* fprintf(debugger, "ssss: specmask1(pid) is %x\n", pid->specmask1); fflush(debugger); */

  DBUG_PRINT("ssss", ("sbits is %x", sbits));

/*  fprintf(debugger, "**************** %x sends to his sons\n", pid); fflush(debugger); */
  
#if D_SPEC_TICKETS
  if (flag == 1 ) {
    DBUG_PRINT("ssss", ("send a special start message to the one process"));
    DBUG_PRINT("ssss", ("sending start to pid %x on node %d", SPEC_SF_NUMB_T(*desc), SPEC_SF_NUMB_A(*desc)));
    if (curr_pid == pid)
      spec_send_start(SPEC_SF_NUMB_A(*desc), SPEC_SF_NUMB_T(*desc), 0, sbits, pid, _spec_redcnt, flag, desc);
    else
      spec_send_start(SPEC_SF_NUMB_A(*desc), SPEC_SF_NUMB_T(*desc), 0, sbits, pid, pid->spec_red_count, flag, desc);
    } else
#endif 
  for (i=0; i<PC_GET_SPEC_SPAWNED(pid); i++) {
    DBUG_PRINT("ssss", ("sending start to pid %x on node %d", pid->processes[(i*2)], 
               pid->processes[(i*2)+1]));
   
/* fprintf(debugger, "**************** %x on node %d \n", pid->processes[(i*2)], pid->processes[(i*2)+1]); */

#if D_SPEC_TICKETS
    spec_send_start(pid->processes[(i*2)+1], pid->processes[(i*2)],
                    pid->red_count,
                    sbits, pid, pid->spec_red_count, 0, 0);
#else
    spec_send_start(pid->processes[(i*2)+1], pid->processes[(i*2)], 
                    pid->red_count,
                    sbits, pid, pid->spec_red_count);
#endif
    }

/*  fprintf(debugger, "0x%x: *********** (initialized with %d redsteps and %d barriersteps, sbits %x)\n", curr_pid, pid->red_count, pid->spec_red_count, sbits); fflush(debugger);  */

  DBUG_VOID_RETURN;
}

void stacklen(keule)
DStackDesc *keule;
{
/*   fprintf(debugger, "stacklen of stack %s is %d !\n", keule->name, d_stacksize(keule)); fflush(debugger); */
}

void stackslen(pid)
PROCESS_CONTEXT* pid;
{
if (curr_pid == pid) { /* process is active */
stacklen(ps_w);
stacklen(ps_a);
stacklen(ps_r);
stacklen(ps_t);
} else {
stacklen(&(((PROCESS_CONTEXT *)pid)->D_S_A));
stacklen(&(((PROCESS_CONTEXT *)pid)->D_S_H));
stacklen(&(((PROCESS_CONTEXT *)pid)->D_S_R));
stacklen(&(((PROCESS_CONTEXT *)pid)->D_S_I));
}
}

/*************************************************************************************/
/* spec_send_winner sends a message to the control process that this speculative */
/* process has reached a SPECRTF ASM-command */

void spec_send_winner(target, controlpid, mypid,real_win,destina,remove)
int target, controlpid, mypid, real_win,destina,remove;
{
 DBUG_ENTER("spec_send_winner");

/* initialize_fred_fish(); */

 DBUG_PRINT("ssw", ("sending winner message to node %d, controlpid %x, mypid %x", target, controlpid, mypid));

/* fprintf(debugger,"Ich bin der Winner !\n"); fflush(debugger); */

/**** debugger ****/
/* if (real_win == 1) {
fprintf(debugger, "printing stacksizes of 0x%x !\n", mypid); fflush(debugger);
stackslen(mypid);
} */

/*****/
/* if (spec_sched_status!=0) {
   fprintf(debugger, "spec_send_win: spec_sched_status!=0 !!!\n"); fflush(debugger);
   post_mortem("spec_send_win: spec_sched_status!=0 !!!");
   } */
/*****/

 TAG_SPECMSG_WIN;
 spec_control[1] = controlpid;
 spec_control[2] = mypid;
 spec_control[3] = proc_id;
 spec_control[4] = real_win;
 spec_control[5] = destina;
 spec_control[6] = remove;

 DBUG_PRINT("spec_send_winner", ("to control %d, remove %d", spec_control[5], spec_control[6]));

/* fprintf(debugger, "0x%x: SEND winner to %d, controlpid 0x%x, mypid 0x%x, real_win %d, destina %d, !remove %d\n", curr_pid, target, controlpid, mypid, real_win, destina, remove); */

  if (target == proc_id) { /* don't send control messages to local processes */

/*  fprintf(debugger, "0x%x: sending WIN locally to (0x%x, 0x%x) from 0x%x\n", curr_pid, controlpid, target, mypid); fflush(debugger); */
/* fprintf(debugger, "0x%x: receiving WIN locally for (0x%x, 0x%x) from 0x%x\n", curr_pid, controlpid, target, mypid); fflush(debugger); */

    DBUG_PRINT("ssw", ("processing winner locally..."));

#if (D_SLAVE && D_MESS && D_MCOMMU) /*                                           to_proc  to_pid */
  MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, mypid, "MT_CONTROL_WIN", proc_id, controlpid);
  MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, mypid, 24);
  MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID); /*         fr_proc from_pid len to_pid */
  MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_CONTROL_WIN", proc_id, mypid, 24, controlpid);
#endif

    spec_sched_status = 5;
    speculative_scheduler();
    } else {
    spec_message_size = 10;
#if (D_SLAVE && D_MESS && D_MCOMMU)
    (*d_m_msg_send)(target, MT_CONTROL);
#else
    msg_send(target, MT_CONTROL);
#endif
    spec_message_size = MESSAGE_SIZE_OLD;
    }

 DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_destination add_son adds the informations to a process context of a node/father process */

void spec_add_son(p_pid, m_pid, m_proc)
PROCESS_CONTEXT *p_pid, *m_pid;
int m_proc;
{
  DBUG_ENTER("spec_add_son");
  
  DBUG_PRINT("sas", ("registering pid %x of node %d", m_pid, m_proc));

/*   printf("\nadding son pid %x on node %d as number %d...\n", m_pid, m_proc, PC_GET_SPEC_ACKS(p_pid)); fflush(stdout); */

  p_pid->processes[(2*PC_GET_SPEC_ACKS(p_pid))] = m_pid;
  p_pid->processes[(2*PC_GET_SPEC_ACKS(p_pid))+1] = m_proc;
  PC_SET_SPEC_ACKS(p_pid,PC_GET_SPEC_ACKS(p_pid)+1);
  
  if (PC_GET_SPEC_SPAWNED(p_pid)==PC_GET_SPEC_ACKS(p_pid)) {

/*  fprintf(debugger, "* (spec_add_son) we've got all !\n"); fflush(debugger); */

    /* cool, all data about the son processes is available */
    DBUG_PRINT("sas", ("we've got all, now let's register them in the control process"));
    /* first check whether we have reached spec_wait */

    if (!T_SPECWAIT(p_pid)) {
/*      fprintf(debugger, "* no SPECWAIT yet!\n"); fflush(debugger);*/
      DBUG_PRINT("sas", ("nope, no SPECWAIT-command yet, so we have to wait !!!!"));
      } else {

      DBUG_PRINT("sas", ("check whether the sons have to be registered..."));

/*      fprintf(debugger, "* send ACK to GRANDDAD p_pid->control_node %x, p_pid->control_pid %x, p_pid %x !\n", p_pid->control_node, p_pid->control_pid, p_pid); fflush(debugger); */

#if D_SPEC_TICKETS
      spec_send_acknowledge(p_pid->control_node, p_pid->control_pid, p_pid, 0, 0);
#else
      spec_send_acknowledge(p_pid->control_node, p_pid->control_pid, p_pid); 
#endif

/*      fprintf(debugger, "* send ACK to GRANDDAD DONE !\n"); fflush(debugger);*/
      }
    }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_broadcast_sons sends an GOON-message to all speculative leaves */

void spec_broadcast_sons(controlpid)
PROCESS_CONTEXT *controlpid;
{
  int i;

  DBUG_ENTER("spec_broadcast_sons");
  
/* fprintf(debugger, "0x%x: ************************ broadcast *********************\n", curr_pid); fflush(debugger);  */

  /* don't forget to clear the node-tags ! */

/* fprintf(debugger, "*** B %d\n", controlpid->control->leaves); fflush(debugger); */

/* fprintf(debugger, "0x%x: starting to transmit **BROADCAST** GOON to %d sons...\n", curr_pid, controlpid->control->leaves); fflush(debugger);   */

/* fprintf(debugger, "0x%x: send broadcast to %d sons...\n", curr_pid, controlpid->control->leaves); fflush(debugger);*/

  for (i = 0; i<controlpid->control->leaves; i++) {
    CLEAR_DATA_ACK(controlpid->control->data[2*i+1]);
    }

  for (i = 0; i<controlpid->control->leaves; i++) {
    DBUG_PRINT("sbs", ("sending to pid %x on node %d", controlpid->control->data[2*i], controlpid->control->data[2*i+1]));

/* fprintf(debugger, "0x%x: *** %d: sending to pid %x on node %d\n", curr_pid, i, controlpid->control->data[2*i], controlpid->control->data[2*i+1]); fflush(debugger);  */

    spec_send_goon(controlpid->control->data[2*i+1], controlpid->control->data[2*i], controlpid); }

/*  fprintf(debugger, "***************** GOON broadcast finished.\n", controlpid->control->leaves); fflush(debugger);*/

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_remove_control_pid removes a (leaf-) process from the control table */

void spec_remove_control_pid(pid_control, pid, node, flag) /* if flag = 1 we check ! */
PROCESS_CONTEXT *pid_control;
int pid, node;
{
  int i;

  DBUG_ENTER("spec_remove_control_pid");

  DBUG_PRINT("srcp", ("trying to find pid %x on node %d, total is %d", pid, node, pid_control->control->leaves));

/* fprintf(debugger, "0x%x: spec_remove_control_pid, pid_control %x, pid %x, node %d\n", curr_pid, pid_control, pid, node); fflush(debugger);  */

/* fprintf(debugger, "rCP 0x%x,%d %d %d %d ", pid, node, pid_control->control->leaves, pid_control->control->have_ack, flag); */

/* fprintf(debugger, "0x%x: spec_remove_control_pid, pid_control 0x%x, pid 0x%x, node %d, leaves before %d, have ack %d, flag %d, ", curr_pid, pid_control, pid, node, pid_control->control->leaves, pid_control->control->have_ack, flag);   */

 /* dump_processes(pid_control);   */

  for (i = 0; i<pid_control->control->leaves; i++) {
    DBUG_PRINT("srcp", ("comparing (%x,%d)", pid_control->control->data[(i*2)], 
               GET_DATA_NODE(pid_control->control->data[(i*2)+1])));
    if ((pid_control->control->data[(i*2)] == pid) && (GET_DATA_NODE(pid_control->control->data[(i*2)+1]) == node)) {
      DBUG_PRINT("srcp", ("found pid %x for node %d, entry %d", pid, node, i));
      break;
      }
    }
  if (i == pid_control->control->leaves) {
    DBUG_PRINT("srcp", ("what a pity, entry not found... :-("));

/*    fprintf(debugger, "spec_remove_control_pid, pid_control %x, pid %x, node %d\n", pid_control, pid, node); fflush(debugger);*/

/*    fprintf(debugger, "T_SPEC_GRANDDAD(pid)=%d !\n", T_SPEC_GRANDDAD((PROCESS_CONTEXT *)pid)); fflush(debugger); */
    /*dump_processes(pid_control);*/

    
    post_mortem("speculative leaf-process not found ?!");
    }

  DBUG_PRINT("srcp", ("The flag was %d.", T_DATA_ACK(pid_control->control->data[(i*2)+1])));
  DBUG_PRINT("srcp", ("%d processes have acknowledged.", pid_control->control->have_ack));

  if (T_DATA_ACK(pid_control->control->data[(i*2)+1])==1) {
    pid_control->control->have_ack--;

/* fprintf(debugger, "0x%x: remove pid 0x%x from pipe, already had acknowledged, %d left, %d have_ack !\n", curr_pid, pid, pid_control->control->leaves-1, pid_control->control->have_ack); fflush(debugger);*/

    } else {

/* fprintf(debugger, "0x%x: remove pid 0x%x from pipe, had NOT acknowledged, %d left, %d have_ack !\n", curr_pid, pid, pid_control->control->leaves-1, pid_control->control->have_ack); fflush(debugger); */

    }
  DBUG_PRINT("srcp", ("Now %d processes have acknowledged.", pid_control->control->have_ack));

  pid_control->control->data[(i*2)] = pid_control->control->data[(pid_control->control->leaves-1)*2];
  pid_control->control->data[(i*2)+1] = pid_control->control->data[(pid_control->control->leaves-1)*2+1];
  pid_control->control->leaves--;

  /* shouldn't acks == leaves be tested here ???? I think so ! */

/*  fprintf(debugger, "have ack new %d\n", pid_control->control->have_ack); fflush(debugger); */

 if (flag == 1 ) { /* only allow this test if the process is not substituted by his sons */

 if ((pid_control->control->have_ack >= pid_control->control->leaves) && (pid_control->control->have_ack > 0)) {
      DBUG_PRINT("spfc", ("all leaves seem to have acknowledged"));
      pid_control->control->have_ack = 0;
      
/* fprintf(debugger, "*** pid_control->control->leaves == pid_control->control->have_ack !\n"); fflush(debugger); */

#if !D_SPEC_TICKETS
      if (((spec_multi_results==0) && (pid_control->control->result>=1)) || ((spec_multi_results>0) && (pid_control->control->result >= spec_multi_results))) { 
        DBUG_PRINT("srcp", ("Already enough results available, don't broadcast anything... ;-)"));

/*    fprintf(debugger, "* enough results, don't broadcast anything...\n"); fflush(debugger);*/
      
        } else {
#endif

/*    fprintf(debugger, "* broadcast ...\n"); fflush(debugger);*/

      spec_broadcast_sons(pid_control);
#if !D_SPEC_TICKETS
      } 
#endif
     }
     
     } 

  
/* fprintf(debugger, "spec_remove_control_pid afterwards:\n"); fflush(debugger); */
 /* dump_processes(pid_control);   */

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
/* spec_control_terminate sends a terminate message to all leaves */
/* spec_control doesn't remove the PIDs... that's quite fascinating... */
/* that's ok, because terminate does not REALLY terminate the processes, */
/* instead all will run until they read SPECWIN, THEN they will de-register themselves */

void spec_control_terminate(control_pid)
PROCESS_CONTEXT *control_pid;
{
  int i;
  DBUG_ENTER("spec_control_terminate");

  for (i = 0; i<control_pid->control->leaves; i++) {
   DBUG_PRINT("sct", ("terminating pid %x on node %d", control_pid->control->data[(2*i)], GET_DATA_NODE(control_pid->control->data[(2*i)+1])));

/* fprintf(debugger, "0x%x: terminate pid %x on node %d as nr. %d\n", curr_pid, control_pid->control->data[(2*i)], GET_DATA_NODE(control_pid->control->data[(2*i)+1]), i); fflush(debugger); */

   spec_send_term(GET_DATA_NODE(control_pid->control->data[(2*i)+1]), control_pid->control->data[(2*i)], control_pid, 1);
   }

  DBUG_VOID_RETURN;
}

/*************************************************************************************/
void abdul(blubber)
PROCESS_CONTEXT *blubber;
{
 int i;
 for (i=0; i<10; i++) {
   DBUG_PRINT("abdul", ("* %d: pid or desc %x, node %d *", i, blubber->processes[i*2], blubber->processes[(i*2)+1]));
/*   fprintf(debugger, "* %d: pid or desc %x, node %d *\n", i, blubber->processes[i*2], blubber->processes[(i*2)+1]);*/
   }
 DBUG_PRINT("abdul", ("* PC_GET_SPEC_SPAWNED is %d, specmask2 is %d", PC_GET_SPEC_SPAWNED(blubber), blubber->specmask2));

/* fprintf(debugger, "* PC_GET_SPEC_SPAWNED is %d, specmask2 is %d\n", PC_GET_SPEC_SPAWNED(blubber), blubber->specmask2); fflush(debugger);*/
}

/*************************************************************************************/
/* spec_tag_data tags a specified process in the control's data table/record/whatever */

void spec_tag_data(c_pid, pid, node)
PROCESS_CONTEXT *c_pid, *pid;
int node;
{
  int i;

  DBUG_ENTER("spec_tag_data");

  DBUG_PRINT("std", ("tagging process %x from node %d...", pid, node));

  for (i=0; i<c_pid->control->leaves; i++) {
    DBUG_PRINT("std", ("comparing pid %x on node %d", c_pid->control->data[i*2], GET_DATA_NODE(c_pid->control->data[(i*2)+1])));
    if ((pid == c_pid->control->data[i*2]) && (c_pid->control->data[(i*2)+1] == node)) {
      SET_DATA_ACK(c_pid->control->data[(i*2)+1]);
      DBUG_PRINT("std", ("found it..."));
      break;
      }
    }
  if (i == c_pid->control->leaves) {
 
/*fprintf(debugger, "0x%x: spec_tag_data: c_pid: %x, pid: %x, node: %d\n", curr_pid, c_pid, pid, node); fflush(debugger);  */
/* dump_processes(c_pid); */

    post_mortem("std: pid not found :-("); }

  DBUG_VOID_RETURN;
}

/* so, finally, here's the speculative scheduler, which acts more or less like a kind of
   post-processing of incoming messages, so that there won't be any recursive call
   of msg_send in msg_receive (hopefully) */

/*************************************************************************************/
void speculative_scheduler()

{
  PROCESS_CONTEXT *spec_tmp1;
  int spec_tmp2, spec_tmp3, spec_tmp4;

  DBUG_ENTER("speculative_scheduler");

  /* "heeeey, it's meeeeee, Maaaaaario" (Mario 64 on Nintendo 64, just another cult) */

  /* fprintf(debugger, "** entering speculative_scheduler\n"); fflush(debugger);  */

  switch (spec_sched_status) {

    case 1: /*** HEREAMI ***/

      /* spec_control[1] = pid of the father (the local target process)
                     [2] = pid of the registrating process
                     [3] = node_id of the registrating process */
      /* for D_SPEC_TICKETS, [4] is the flag if the process was NOT generated by code, and [5] is the descriptor */

/* fprintf(debugger, "0x%x: *SPECSCHED* HEREAMI for 0x%x from (0x%x,%d)\n", curr_pid, spec_control[1], spec_control[2], spec_control[3]); fflush(debugger); */

      DBUG_PRINT("specsched", ("HEREAMI message processing"));
      DBUG_PRINT("specsched", ("the message is for pid : %x", spec_control[1]));

      DBUG_PRINT("specsched", ("add the son..."));

      spec_sched_status = 0;

#if D_SPEC_TICKETS
      if (spec_control[4] == 1) {
        SPEC_SF_NUMB_T(*(PTR_DESCRIPTOR)spec_control[5]) = spec_control[2];
        SPEC_SF_NUMB_A(*(PTR_DESCRIPTOR)spec_control[5]) = spec_control[3];

        DBUG_PRINT("specsched", ("registrate non-CODE process 0x%x from node %d", spec_control[2], spec_control[3]));

        spec_send_acknowledge(((PROCESS_CONTEXT *)spec_control[1])->control_node,((PROCESS_CONTEXT *)spec_control[1])->control_pid, spec_control[1], 1, spec_control[5]);

        } else
#endif
      spec_add_son(spec_control[1],spec_control[2],spec_control[3]);
  
      break;
     
      case 2: /*** ACK ***/

        /* [1] is the node of the father process
           [2] is the pid of the father process */
    
        DBUG_PRINT("specsched", ("after acknowledging, send OK back to the father"));
        DBUG_PRINT("specsched", ("father pid %x on node %d", spec_control[2],
                   spec_control[1]));
        DBUG_PRINT("specsched", ("what's up with spec_control[4]: %x", spec_control[4]));

/* fprintf(debugger, "0x%x: *SPECSCHED* processing ACK ! father is (%x,%d), flag is %x\n", curr_pid, spec_control[2], spec_control[1], spec_control[4]); fflush(debugger); */

        spec_sched_status = 0;
#if D_SPEC_TICKETS
        spec_send_start(spec_control[1], spec_control[2], 0, 0, spec_control[4], 0, spec_control[8], spec_control[9]);
#else
        spec_send_start(spec_control[1], spec_control[2], 0, 0, spec_control[4], 0);
#endif
        
        break;

      case 3: /*** BARR ***/

        /* [1] is the pid of the controller */
        /* [2] is the pid of the son */
        /* [3] is the pid of the son's node */

        DBUG_PRINT("specsched", ("BARR-message received, processing"));
        DBUG_PRINT("specsched", ("the message is for pid : %x", spec_control[1]));

/* fprintf(debugger, "Br 0x%x,%d\n", spec_control[2], spec_control[3]); fflush(debugger); */

/* fprintf(debugger, "0x%x: *SPECSCHED* BARR received from (0x%x,%d) for 0x%x\n", curr_pid, spec_control[2], spec_control[3], spec_control[1]); fflush(debugger);  */

/******/
/*         if (((PROCESS_CONTEXT *)spec_control[1])->control->result>0) {
          break; */
/*           fprintf(debugger,"BARR received, result already registered !\n"); fflush(debugger); }  */


        ((PROCESS_CONTEXT *)spec_control[1])->control->have_ack++;

/* fprintf (debugger, "0x%x: BARR, Now %d have acknowledged, %d altogether !\n", curr_pid, ((PROCESS_CONTEXT *)spec_control[1])->control->have_ack, ((PROCESS_CONTEXT *)spec_control[1])->control->leaves); fflush(debugger);  */

        DBUG_PRINT("specsched", ("leaves: %d, ack: %d", ((PROCESS_CONTEXT *)
                   spec_control[1])->control->leaves, ((PROCESS_CONTEXT *)
                   spec_control[1])->control->have_ack));

        if (((PROCESS_CONTEXT *)spec_control[1])->control->have_ack >=
            ((PROCESS_CONTEXT *)spec_control[1])->control->leaves) {

/*fprintf (debugger, "0x%x: BARR, tagging 0x%x, ENOUGH acknowledges !\n", curr_pid, spec_control[2]); fflush(debugger); */

            DBUG_PRINT("specsched", ("all leaves have acknowledged !"));
            ((PROCESS_CONTEXT *)spec_control[1])->control->have_ack = 0;
            DBUG_PRINT("specsched", ("broadcasting barrier messages..."));
            spec_sched_status = 0;

#if D_SPEC_TICKETS

/*           fprintf (debugger, "0x%x: BARR, sending broadcast !\n", curr_pid); fflush(debugger); */
 
           spec_broadcast_sons(spec_control[1]);

#else
            if (((spec_multi_results==0) && (((PROCESS_CONTEXT *)spec_control[1])->control->result>=1)) || 
                ((spec_multi_results>0) && (((PROCESS_CONTEXT *)spec_control[1])->control->result >= spec_multi_results))) {
              DBUG_PRINT("srcp", ("Already enough results available, don't broadcast anything ! ;-)"));
 
/*               fprintf(debugger, "* enough results, don't broadcast anything !\n"); fflush(debugger);  */

/*fprintf (debugger, "0x%x: BARR, don't send a broadcast any more !\n", curr_pid); fflush(debugger); */
 
            } else {
 
/*              fprintf(debugger, "* broadcasting ...\n"); fflush(debugger);*/

/*fprintf (debugger, "0x%x: BARR, sending broadcast !\n", curr_pid); fflush(debugger); */
 
              spec_broadcast_sons(spec_control[1]);
            }

            /* spec_broadcast_sons(spec_control[1]);  UAAAAAAAAAAAAAAAAAAAAAAH ?! */
#endif /* D_SPEC_TICKETS */

          }
        else {

/* fprintf (debugger, "0x%x: BARR, just tagging 0x%x !\n", curr_pid, spec_control[2]); fflush(debugger); */

          spec_tag_data(spec_control[1], spec_control[2], spec_control[3]); 
          }

        break;

      case 4: /*** GOON ***/
       
        /* [1] is the pid of the concerning process */

        /* give the process more redsteps, err, more or less */
        /* first check whether it's not a waiting father or not */
 
        /* ignore the message if it's a waiting dad process, if
           that's a winning process and if it's a terminating process */

/* fprintf(debugger, "0x%x: *SPECSCHED* receiving GOON for 0x%x !\n", curr_pid, spec_control[1]); fflush(debugger); */

        if (T_SPEC_READY((PROCESS_CONTEXT *)spec_control[1])==1)
          post_mortem("GOON-Message for SPEC_READY process !");

        if (T_SPECWAIT((PROCESS_CONTEXT *)spec_control[1])==1) {

/*          fprintf(debugger, "*** father is receiving !\n"); fflush(debugger);  */
 
          DBUG_PRINT("specsched", ("father hasn't started the sons yet, toggle the flag..."));
          CLEAR_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_control[1]);

          break;
          }

#if D_SPEC_TICKETS

#else
        if (!T_SPEC_FOLD((PROCESS_CONTEXT *)spec_control[1])) {
#endif
          
/*          fprintf(debugger, "'drin !\n"); fflush(debugger); */

          spec_tmp1 = spec_control[1];

          if (spec_control[1] == curr_pid) { /* process is active */

            DBUG_PRINT("specsched", ("process is active"));
            CLEAR_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_tmp1);

/*            fprintf(debugger, "'is active !\n"); fflush(debugger); */

            } else {
            DBUG_PRINT("specsched", ("process is not active"));

/* fprintf(debugger, "0x%x: GOON for 0x%x, 'is not active, SPEC_WAIT_WIN is %d !\n", curr_pid, spec_control[1], T_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_tmp1)); fflush(debugger);  */
 
            /* check for the amount of reduction steps... */

#if D_SPEC_TICKETS
            if ((PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING) && (T_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_tmp1) == 0) /* && (T_SPEC_BARRSUS((PROCESS_CONTEXT *)spec_tmp1) == 1) */ ) {
#else
            if ((PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING) /* && (T_SPEC_BARRSUS((PROCESS_CONTEXT *)spec_tmp1) == 1) */) {
#endif
/*              PC_CLEAR_SPEC_BARRSUS((PROCESS_CONTEXT *)spec_tmp1); */

              DBUG_PRINT("specsched", ("spec_redcnt old value: %d", ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count));
              ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count += ack_red_count;

              if (((PROCESS_CONTEXT *)spec_control[1])->spec_red_count > ack_red_count)
                post_mortem("spec_red_count > ack_red_count !");

             /*  if (!T_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_control[1]))
                post_mortem("speculative process acknowledge bit not set !"); */

              /* maybe a security check is needed if spec_red_count is greater than ack_red_count ??? */

              DBUG_PRINT("specsched", ("spec_redcnt new value: %d", ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count));

            /* reductionsteps-management later, VERY FUNNY (searching for the bug */
            /* for 2 days now... :-( */
  
            /* so this process must have been waiting, let's wake this process up */
            /* hey, it could be suspended... */

              DBUG_PRINT("specsched", ("waking up the process..."));

#if (D_SLAVE && D_MESS && D_MSCHED)
              (*d_m_process_wakeup)(spec_tmp1);
#else
              process_wakeup(spec_tmp1);
#endif

              DBUG_PRINT("specsched", ("sending BARR message back to the control"));

              spec_sched_status = 0;

/* fprintf(debugger, "0x%x: wakeup't, now sending barrier... (hopefully not recursive !)\n", curr_pid); fflush(debugger);*/

              spec_send_barrier(spec_tmp1);  

/* fprintf(debugger, "0x%x: back from spec_send_barrier !\n", curr_pid); fflush(debugger);*/

              } else {

/* fprintf(debugger, "0x%x: just toggle flag (if anything, WAITING %d, !WAIT_WIN %d, BARRSUS %d)\n", curr_pid,(PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING), (T_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_tmp1) == 0), (T_SPEC_BARRSUS((PROCESS_CONTEXT *)spec_tmp1) == 1)); fflush(debugger); */

#if D_SPEC_TICKETS
              if (T_SPEC_NO_BSTEPS((PROCESS_CONTEXT *)spec_tmp1) == 1) {
                DBUG_PRINT("specsched", ("DON't clear ack, 'cause NO_BSTEPS is set already"));
                
/* fprintf(debugger, "0x%x: ************************* GOON received and BSTEPS set, ignore !\n", curr_pid); fflush(debugger);*/

                }
              else
#endif
                CLEAR_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_tmp1);
                DBUG_PRINT("specsched", ("acknowledge cleared, specmask1 of %x is %x",
                   spec_tmp1, ((PROCESS_CONTEXT *)spec_tmp1)->specmask1)); }
            } 
#if !D_SPEC_TICKETS
          } else  /* ignore */ {

          DBUG_PRINT("specsched", ("it's a folding process, ignore GOON"));

/*          fprintf(debugger, "******** Ignoring !!! :-(\n"); fflush(debugger);  */
          }
#endif

/* fprintf(debugger, "* leaving this !\n"); fflush(debugger); */

        break;

      case 5: /*** WINNER ***/

        /* [1] is the pid of the control
           [2] is the pid of the calling process
           [3] is the number of the calling node 
           [4] is TRUE if it's really a result, FALSE otherwise (for the CONTROL, for a process, it's TRUE if it was the first result, FALSE otherwise)
           [5] is true if the message is for the CONTROL
           [6] is an auxiliary flag for D_SPEC_TICKETS */

/* fprintf(debugger, "0x%x: *SPECSCHED* WINNER from <%d, 0x%x> for 0x%x, flag %x\n", curr_pid, spec_control[3], spec_control[2], spec_control[1], spec_control[4]); fflush(debugger); */

/* initialize_fred_fish(); */

        DBUG_PRINT("specsched", ("processing winner message"));

        DBUG_PRINT("specsched", ("is it for the control: %d, should it be be removed from the pipe: %d", spec_control[5], spec_control[6]));

        if (spec_control[5]==1) {
          DBUG_PRINT("specsched", ("control is receiving a result..."));

          spec_tmp1 = spec_control[2];
          spec_tmp2 = spec_control[3];
          spec_tmp3 = spec_control[1];
          spec_tmp4 = spec_control[4];

          spec_sched_status = 0;

#if D_SPEC_TICKETS
          if (spec_control[6]!=1) {
            DBUG_PRINT("specsched", ("remove pid from the pipe !"));

/*fprintf(debugger, "0x%x: SPECSCHED WIN received, remove PID 0x%x from the pipe !\n", curr_pid, spec_tmp1); fflush(debugger); */

#endif
          spec_remove_control_pid(spec_control[1], spec_control[2], spec_control[3], 1);
#if D_SPEC_TICKETS
            } else {
              DBUG_PRINT("specsched", ("don't remove the pid from the pipe !"));

/*fprintf(debugger, "0x%x: SPECSCHED WIN received, DO NOT remove PID 0x%x from the pipe !\n", curr_pid, spec_tmp1); fflush(debugger); */
              }
#endif

          if (spec_tmp4==1) {
 
            DBUG_PRINT("specsched", ("it REALLY was a successfull result !"));
            ((PROCESS_CONTEXT *)spec_tmp3)->control->result++; /* mask it */

/*  fprintf(debugger, "0x%x: control %x has received a REAL result from pid %x on node %d\n", curr_pid, spec_tmp3, spec_tmp1, spec_tmp2); fflush(debugger);   */
 
            if (((spec_multi_results==0) && (((PROCESS_CONTEXT *)spec_tmp3)->control->result==1)) || ((spec_multi_results>0) && (((PROCESS_CONTEXT *)spec_tmp3)->control->result >= spec_multi_results))) {

/* fprintf(debugger, "** control %x terminates all leaves...\n", spec_tmp3); fflush(debugger);   */

              DBUG_PRINT("specsched", ("terminate all other leaves !"));

              spec_sched_status = 0;
              spec_control_terminate(spec_tmp3);

              }
            } else {
            DBUG_PRINT("specsched", ("just a SPECWIN acknowledge..."));

/* fprintf(debugger, "0x%x: control has received a SON-HAS-REACHED-SPECWIN-message from (0x%x,%d)\n", curr_pid, spec_tmp1, spec_tmp2); fflush(debugger); */
            }

            /* send the confirmation message to the node */

#if D_SPEC_TICKETS
            if (spec_nondet != 1)
              post_mortem("spec_nondet not set for SPEC_TICKETS !");
#endif
            spec_sched_status = 0;
            if ((spec_nondet == 1) && (spec_multi_results==0)) {
              DBUG_PRINT("specsched", ("produce nondeterministic solution..."));
              if (((PROCESS_CONTEXT *)spec_tmp3)->control->result>1) {
                DBUG_PRINT("specsched", ("first solution has already acknowledged... mutate all all others to fail"));

/* fprintf(debugger, "0x%x: confirm (0x%x, %d) ... (mutate to fail)\n", curr_pid, spec_tmp1, spec_tmp2); fflush(debugger); */

                spec_send_winner(spec_tmp2, spec_tmp1, spec_tmp3,1,0,0);
                }
              else {
                DBUG_PRINT("specsched", ("it was the first result or no result at all, don't mutate the process to fail !"));

/* fprintf(debugger, "0x%x: confirm (0x%x, %d) ... (first result or no result)\n", curr_pid, spec_tmp1, spec_tmp2); fflush(debugger); */

                spec_send_winner(spec_tmp2, spec_tmp1, spec_tmp3,0,0,0);
                }
              } else {

/* fprintf(debugger, "0x%x: confirm (0x%x, %d) ... (no nondet result)\n", curr_pid, spec_tmp1, spec_tmp2); fflush(debugger); */

              spec_send_winner(spec_tmp2, spec_tmp1, spec_tmp3,0,0,0);
              DBUG_PRINT("specsched", ("deterministic solution..."));
              }

          } else {

          DBUG_PRINT("specsched", ("process can goon processing..."));

#if D_SPEC_TICKETS
            /* debugger 
            if (T_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_control[1]) != 1)
              post_mortem("no WAIT_WIN was set... ?!"); */

            PC_CLEAR_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_control[1]);
#endif

          PC_SET_SPEC_CONF_WIN((PROCESS_CONTEXT *)spec_control[1]);

          if ((spec_nondet==1) && (spec_multi_results==0) && (spec_control[4]==1)) {
            DBUG_PRINT("specsched", ("produce nondeterministic result, ignore reduction steps etc."));
            PC_CLEAR_SPEC_WINNER((PROCESS_CONTEXT *)spec_control[1]);
            PC_SET_SPEC_FAIL((PROCESS_CONTEXT *)spec_control[1]);
            } 
         
/* debugger */
   /*       if (T_SPEC_WINNER((PROCESS_CONTEXT *)spec_control[1]) == 1) */
/*   fprintf (debugger, "0x%x: 0x%x stays the winning process !\n", curr_pid, spec_control[1]); fflush(debugger); */

           
 
          DBUG_PRINT("specsched", ("setting CONF_WIN for pid %x !", spec_control[1]));

          if (PC_STATUS((PROCESS_CONTEXT *)spec_control[1])==PS_WAITING) {
            DBUG_PRINT("specsched", ("The process is not running, waking up !"));

/* fprintf(debugger, "0x%x: it's the WIN Wakeup for 0x%x ! (spec_control[4] = %d)\n", curr_pid, spec_control[1], spec_control[4]); fflush(debugger); */

#if (D_SLAVE && D_MESS && D_MSCHED)
          (*d_m_process_wakeup)(spec_control[1]);
#else
          process_wakeup(spec_control[1]); /* that's it ?! I think so... */
#endif
            } else {
            DBUG_PRINT("specsched", ("The process is running, no need for waking up !"));
            /* post_mortem("WINNER: process is already running ?!"); */
            }

          }

        break;

      case 6: /*** START ***/

        /* [1] is the concerning pid 
           [2] reduction steps
           [3] barrier flag
           [4] source pid
           [5] remaining barrier-steps
           [6] (new) control pid
           [7] (new) control node */

        /* [8] is a flag for D_SPEC_TICKETS whether or not this process was created by post_proc
           [9] is the corresponding descriptor */

        DBUG_PRINT("specsched", ("SPECMSG_START received..."));

        /* if this message is for a father process, SPECWAIT would be set */

#if D_SPEC_TICKETS
        if (spec_control[3] != 0) {
#else
        if (!T_SPECWAIT((PROCESS_CONTEXT *)spec_control[1])) {
#endif
          DBUG_PRINT("specsched", ("it's a newly created leaf process"));

#if D_SPEC_TICKETS
/*        if (((PROCESS_CONTEXT *)spec_control[1])->ffound != 0) {
          spec_control[3] = spec_control[3] | SPEC_FAIL;
           post_mortem("newly created leaf process already ffound=1 !"); 
          } */
#endif

/*  fprintf(debugger, "0x%x: *SPECSCHED* receiving start message for the son 0x%x from (0x%x,???)\n", curr_pid, spec_control[1], spec_control[4]); fflush(debugger);*/

          if (PC_STATUS((PROCESS_CONTEXT *)spec_control[1])==PS_ACTIVE) {
/*            fprintf(debugger, "%x: leaf received START but is not waiting !\n", spec_control[1]); fflush(debugger);*/
            post_mortem("leaf received START but is not waiting !"); }

/* fprintf(debugger, "0x%x: START is for a newly created LEAF-process...\n", curr_pid); fflush(debugger); */
 
          DBUG_PRINT("specsched", ("target pid: %x, steps: %d, barrier: %d, source pid: %x, barrier steps: %d, new control pid: %x, new control node: %d", spec_control[1], spec_control[2],spec_control[3],spec_control[4],spec_control[5],spec_control[6],spec_control[7]));

#if D_SPEC_TICKETS
        if (spec_control[8] != 1) {
          DBUG_PRINT("specsched", ("post_proc is not set, initializing remaining reduction steps properly"));
#endif
          ((PROCESS_CONTEXT *)spec_control[1])->red_count = spec_control[2];
          ((PROCESS_CONTEXT *)spec_control[1])->start_red_count = spec_control[2];
#if D_SPEC_TICKETS
          } else {
          DBUG_PRINT("specsched", ("post_proc is set, reduction steps already initialized"));
          }
#endif
#if D_SPEC_TICKETS
          if (T_SPEC_NO_BSTEPS((PROCESS_CONTEXT *)spec_control[1]) == 1) {
            DBUG_PRINT("specsched", ("don't touch spec_red_count if BSTEPS was set already"));
            } else
#endif
          ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count = spec_control[5];

/*  fprintf(debugger, "** %x: red_count: %d, spec_red_count: %d\n", spec_control[1], spec_control[2], spec_control[5]); fflush(debugger);  */

/* fprintf(debugger, "START processing, old control_pid %x, old control_node %d\n", ((PROCESS_CONTEXT *)spec_control[1])->control_pid, ((PROCESS_CONTEXT *)spec_control[1])->control_node); fflush(debugger); */

          ((PROCESS_CONTEXT *)spec_control[1])->control_pid = spec_control[6];
          ((PROCESS_CONTEXT *)spec_control[1])->control_node = spec_control[7];

/* fprintf(debugger, "START processing, new control_pid %x, new control_node %d\n", ((PROCESS_CONTEXT *)spec_control[1])->control_pid, ((PROCESS_CONTEXT *)spec_control[1])->control_node); fflush(debugger); */

          DBUG_PRINT("specsched", ("initialization complete for red_count=%d, start_red_count=%d, spec_red_count=%d", ((PROCESS_CONTEXT *)spec_control[1])->red_count, ((PROCESS_CONTEXT *)spec_control[1])->start_red_count, ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count));
          /* something has to be inserted for red. steps. counting... */
          spec_tmp1 = spec_control[1];

/*  fprintf(debugger, "START received, bits are %x\n", spec_control[3]); fflush(debugger); */

          PC_CLEAR_SPEC_READY((PROCESS_CONTEXT *)spec_tmp1);

/* fprintf (debugger, "*** it's this wakeup !\n"); fflush(debugger); */
 
#if (D_SLAVE && D_MESS && D_MSCHED)
          (*d_m_process_wakeup)(spec_tmp1);
#else
          process_wakeup(spec_tmp1);
#endif
 
/* fprintf (debugger, "*** or not ?!\n"); fflush(debugger);*/

          if (spec_control[3]>0) {

/*  fprintf(debugger, "START received, bits are >0\n", spec_control[3]); fflush(debugger); */

            DBUG_PRINT("specsched", ("* bits are %x", spec_control[3]));

#if D_SPEC_TICKETS
            if ((spec_control[3] & SPEC_NO_BSTEPS) || (spec_control[3] & SPEC_FAIL)) {
#else
            if ((spec_control[3] & SPEC_NO_BSTEPS)) {
#endif
              DBUG_PRINT("specsched", ("SPEC_NO_BSTEPS flag is set..."));
              PC_SET_SPEC_NO_BSTEPS((PROCESS_CONTEXT *)spec_control[1]);
              } else {
              DBUG_PRINT("specsched", ("no_bsteps flag not set..."));
              }

#if D_SPEC_TICKETS
            if (((T_SPEC_NO_BSTEPS((PROCESS_CONTEXT *)spec_control[1]) == 1) || 
                (T_SPEC_FAIL((PROCESS_CONTEXT *)spec_control[1]) == 1)) &&
                (spec_nondet == 1)) {
#else
            if ((T_SPEC_NO_BSTEPS((PROCESS_CONTEXT *)spec_control[1]) == 1) && (spec_nondet == 1)) {
#endif
              DBUG_PRINT("specsched", ("immediately set spec reduction steps to 0 !"));
              ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count = 0;
              }


/*  fprintf(debugger, "START received, spec_control[3] & 65536: %x\n", spec_control[3] & 65536); fflush(debugger); */

            DBUG_PRINT("specsched", ("spec_control[3]: %x, SPEC_GUARD_SON: %x, (spec_control[3] & SPEC_GUARD_SON): %x, ((spec_control[3] & SPEC_GUARD_SON)!=0) : %x", spec_control[3], SPEC_GUARD_SON, (spec_control[3] & SPEC_GUARD_SON), ((spec_control[3] & SPEC_GUARD_SON)!=0)));

            if ((spec_control[3] & SPEC_GUARD_SON)!=0) {

/*  fprintf(debugger, "START received, set GUARD\n"); fflush(debugger); */

              DBUG_PRINT("specsched", ("SPEC_GUARD_SON flag is set..."));
              PC_SET_SPEC_GUARD_SON((PROCESS_CONTEXT *)spec_control[1]);
              ((PROCESS_CONTEXT *)spec_control[1])->red_count = 0;
              ((PROCESS_CONTEXT *)spec_control[1])->start_red_count = 0;
              ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count = 0;
              /* let this process terminate automatically */
              } else {

/* fprintf(debugger, "START received, GUARD_SON NOT set\n"); fflush(debugger); */
              PC_CLEAR_SPEC_GUARD_SON((PROCESS_CONTEXT *)spec_control[1]);
              DBUG_PRINT("specsched", ("guard flag not set..."));
              }

            if ((spec_control[3] & SPEC_INSYNC)!=0) {
              DBUG_PRINT("specsched", ("SPEC_INSYNC flag is set..."));
              PC_SET_SPEC_INSYNC((PROCESS_CONTEXT *)spec_control[1]);

/* fprintf(debugger, "0x%x: setting starting process to INSYNC\n", curr_pid); fflush(debugger);*/

              } else {
              DBUG_PRINT("specsched", ("SPEC_INSYNC not set..."));
              PC_CLEAR_SPEC_INSYNC((PROCESS_CONTEXT *)spec_control[1]);
             }

            if ((spec_control[3] & HAVE_ACKNOWLEDGED)) {
              DBUG_PRINT("specsched", ("barrier flag is set..."));

/*fprintf(debugger, "0x%x: START received, BARR was set, send the flag !\n"); fflush(debugger);*/

              SET_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_control[1]);
              /* sending BARRIER-message ! */

              spec_sched_status = 0;

/* fprintf(debugger, "0x%x: 0x%x sets ACK-Bit after receiving the START message (and sending BARR)\n", curr_pid, spec_control[1]); fflush(debugger);*/

/* fprintf(debugger, "sACK\n"); fflush(debugger); */

              spec_send_barrier(spec_tmp1);
              } else {
              DBUG_PRINT("specsched", ("barrier flag not set..."));
              CLEAR_ACKNOWLEDGE((PROCESS_CONTEXT *)spec_control[1]);
              }

            }

#if D_SPEC_TICKETS
          if (((PROCESS_CONTEXT *)spec_tmp1)->ffound != 0) {
            DBUG_PRINT("specsched", ("the son already has received a TERM message..."));
            ((PROCESS_CONTEXT *)spec_tmp1)->spec_red_count = 0;
            }
#endif

           }
        else {
          DBUG_PRINT("specsched", ("it's the father again, so NOW send details to the sons"));

#if D_SPEC_TICKETS
     /*    if (((PROCESS_CONTEXT *)spec_control[1])->ffound != 0) */
/* fprintf(debugger, "0x%x: FATHER 0x%x receiving start, ffound set !\n", curr_pid, spec_control[1]); */
#endif


/*  fprintf(debugger, "0x%x: *SPECSCHED* START: sending start to the sons...\n", curr_pid); fflush(debugger); */

#if D_SPEC_TICKETS
        if (spec_control[8] != 1)  {
          DBUG_PRINT("specsched", ("check for first spawning and the initialization of the pipe"));
#endif
          if (((PROCESS_CONTEXT *)spec_control[1])->control != NULL) {
            DBUG_PRINT("specsched", ("the father was the granddad ! Initialize barrier !"));
            ((PROCESS_CONTEXT *)spec_control[1])->spec_red_count = ack_red_count;
            } else { 
            DBUG_PRINT("specsched", ("the father wasn't the granddad"));
            }
#if D_SPEC_TICKETS
          } else {
          DBUG_PRINT("specsched", ("it was a post_proc message..."));
          }
#endif

#if D_SPEC_TICKETS
          if (spec_control[8] != 1) {
          if ((((PROCESS_CONTEXT *)spec_control[1])->spec_postpone_start != NULL) || (T_SPEC_SIGNOFF((PROCESS_CONTEXT *)spec_control[1])!=1)) {
              DBUG_PRINT("specsched", ("restart process 0x%x and clear SPECWAIT !", spec_control[1]));
     
            PC_CLEAR_SPECWAIT((PROCESS_CONTEXT *)spec_control[1]);

/* fprintf (debugger, "*** it's that wakeup !\n"); fflush(debugger);*/

#if (D_SLAVE && D_MESS && D_MSCHED)
            (*d_m_process_wakeup)((PROCESS_CONTEXT *)spec_control[1]);
#else
            process_wakeup((PROCESS_CONTEXT *)spec_control[1]);
#endif

            } else {
              DBUG_PRINT("specsched", ("process 0x%x has no postponed expressions..."));
            }
             } else {
            DBUG_PRINT("specsched", ("post_proc message, ignore additional stuff...")); }
#endif

          spec_sched_status = 0;
#if D_SPEC_TICKETS
          spec_send_start_sons(spec_control[1], spec_control[8], spec_control[9]);
#else
          spec_send_start_sons(spec_control[1]);
#endif
          }

        break;

      case 7: /*** TERMINATE ***/

        /* [1] is the pid that should be terminated 
           [2] is the source pid
           [3] is a flag (see DBUG-message below */

/* fprintf(debugger, "0x%x: *SPECSCHED* TERMINATE from <%d, %x> for %x\n", curr_pid, -1, spec_control[2], spec_control[1]); fflush(debugger);  */


        DBUG_PRINT("specsched", ("termination of process %x initiated ...", spec_control[1]));
        DBUG_PRINT("specsched", ("the message is from the father (=0) or the control (=1): %d", spec_control[3])); 

        /* if (T_SPEC_READY((PROCESS_CONTEXT *) spec_control[1]) == 1)
          post_mortem("TERMINATE: message to a READY process => will be overwritten..."); */

        /* I don't think that this distinction is relevant any more */
        /* OH YES IT IS ! */
        /* no, it is not, because as of now, nobody sends termination messages to the control anymore */

        if (spec_control[3]==1) {
          DBUG_PRINT("specsched", ("Message is from the control process !"));
 
          /* don't do anything if it's just the confirmation to proceed */
          /* otherwise set NO_BSTEPS flag */
  
          spec_tmp1 = spec_control[1];

#if D_SPEC_TICKETS
          if (spec_multi_results == 0) {
            spec_tmp1->ffound = 1;

/* fprintf(debugger, "0x%x: setting ffound for 0x%x !\n", curr_pid, spec_tmp1); fflush(debugger); */

            DBUG_PRINT("specsched", ("set ffound to 1 => a solution has been found !"));
            }
#endif          
     
#if D_SPEC_TICKETS
          if ((T_SPEC_FAIL(spec_tmp1)==1) /* || (T_SPEC_GUARD_SON(spec_tmp1)==1) DON'T KNOW */) {
#else
          if ((T_SPEC_WINNER(spec_tmp1)==1) || (T_SPEC_NO_STEPS(spec_tmp1)==1) ||
              (T_SPEC_FAIL(spec_tmp1)==1) || (T_SPEC_GUARD_SON(spec_tmp1)==1)) {
#endif

             /* in all these cases, a TERM-message is ignored, because there are no more */
             /* reduction steps executed (NO_STEPS, SPEC_FAIL, _GUARD_SON) or SPECWIN was */
             /* already reached anyways */

 fprintf(debugger, "0x%x: TERM, nothing is changed for no_tickets and 0x%x !\n", curr_pid, spec_tmp1); fflush(debugger); 

#if D_SPEC_TICKETS
            PC_SET_SPEC_NO_BSTEPS(spec_tmp1); /* this cannot do any harm... */
#endif
            DBUG_PRINT("specsched", ("ignore termination ! NOOOOOOOOO :-)"));

#if D_SPEC_TICKETS
            }
          else if (T_SPEC_WINNER(spec_tmp1)==1) {

            DBUG_PRINT("specsched", ("don't touch, it's a winning process already"));

/* fprintf(debugger, "0x%x: don't do anything, WINNER already...\n", curr_pid); fflush(debugger); */

            /* don't wakeup since the SPECWIN message will wake the process up */

#endif

            } else {

/* fprintf(debugger, "0x%x: TERM, settint NO_BSTEPS for 0x%x...\n", curr_pid, spec_tmp1); fflush(debugger); */

            PC_SET_SPEC_NO_BSTEPS(spec_tmp1);
            DBUG_PRINT("specsched", ("set NO_BSTEPS !"));

            if (spec_nondet == 1) {

            if (curr_pid == spec_tmp1) {
              DBUG_PRINT("specsched", ("process is active, clear reduction steps..."));
              /* ((PROCESS_CONTEXT *)spec_tmp1)->start_red_count = ((PROCESS_CONTEXT *)spec_tmp1)->start_red_count - _redcnt; */
              _spec_redcnt = 0;
              } else {
              DBUG_PRINT("specsched", ("process is not active, clear reduction steps..."));
/*               ((PROCESS_CONTEXT *)spec_tmp1)->start_red_count = ((PROCESS_CONTEXT *)spec_tmp1)->start_red_count - ((PROCESS_CONTEXT *)spec_tmp1)->red_count;
              ((PROCESS_CONTEXT *)spec_tmp1)->red_count = 0; */
              ((PROCESS_CONTEXT *)spec_tmp1)->spec_red_count = 0;
              }

/*             PC_SET_SPEC_FAIL(spec_tmp1);
            PC_SET_SPEC_FOLD(spec_tmp1); */

              }
              
            /* yeah, this is right, because a process might be suspended while waiting for another */
            /* amount of reduction steps */

#if D_SPEC_TICKETS
            if ((PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING) && !T_SPEC_READY((PROCESS_CONTEXT *)spec_tmp1)
               && !T_SPECWAIT((PROCESS_CONTEXT *)spec_tmp1) && !T_SPEC_WAIT_WIN((PROCESS_CONTEXT *)spec_tmp1)) {
#else
            if ((PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING) && !T_SPEC_READY((PROCESS_CONTEXT *)spec_tmp1)
               && !T_SPECWAIT((PROCESS_CONTEXT *)spec_tmp1)) {
#endif
              DBUG_PRINT("specsched", ("process is sleeping..."));

              /* if (T_SPEC_BARRSUS(spec_tmp1)==1)
                PC_CLEAR_SPEC_BARRSUS(spec_tmp1); */

#if (D_SLAVE && D_MESS && D_MSCHED)
              (*d_m_process_wakeup)(spec_tmp1);
#else
              process_wakeup(spec_tmp1);
#endif
              }
            }
          } else {
          DBUG_PRINT("specsched", ("Message is from the father process !"));
          post_mortem("unidentified TERM message received !");
          }

        break;

       case 9: /* send hereami back to the father... extracted from rncmessage.c... */

         /* [0] target node
            [1] target pid
            [2] source pid */

         /* [3] flag which is 1 (D_SPEC_TICKETS) if the process is NOT spawned by code */
         /* [4] is the descriptor if [3] is set */
         
/* fprintf(debugger, "0x%x: *SPECSCHED* sending hereami back to the father, from 0x%x to (0x%x, %d)...\n", curr_pid, spec_control[2], spec_control[1], spec_control[0]); fflush(debugger);*/

         spec_sched_status = 0;
#if D_SPEC_TICKETS
         spec_send_hereami(spec_control[0], spec_control[1], spec_control[2], spec_control[3], spec_control[4]);
#else
         spec_send_hereami(spec_control[0], spec_control[1], spec_control[2]);
#endif
         break;

       case 10: /* making a process vital */

         /* [1] target pid
            [2] source pid (the old control 
            [3] results */

         post_mortem("VITALIZATION disabled !!!!");

/* fprintf(debugger, "0x%x: *SPECSCHED* spec_sched: VITAL from <%d, %x> for %x\n", curr_pid, -1, spec_control[2], spec_control[1]); fflush(debugger); */

/* initialize_fred_fish(); */

         DBUG_PRINT("specsched", ("process VITAL message... pid %x", spec_control[1]));

/*  fprintf (debugger, "Vitalizing process %x, source was %x, results is %x\n", spec_control[1], spec_control[2], spec_control[3]); fflush(debugger); */
/*  fprintf (debugger, "Vitalizing process %x, source was %x, results is %x\n", spec_control[1], spec_control[2], spec_control[3]); fflush(debugger); */

         spec_tmp1 = spec_control[1];

         /* allocate space for the control structure */

         if (!(spec_tmp1->control = malloc(sizeof(SPEC_CONTROL_STRUC))))
           post_mortem ("could not allocate space for new speculative control...");

         if (!(spec_tmp1->control->data = malloc(sizeof(int)*2*max_spec_procs)))
           post_mortem ("could not allocate space for new speculative control data...");
         spec_tmp1->control->leaves = 0;
         spec_tmp1->control->have_ack = 0;
         spec_tmp1->control->result = spec_control[3];

         spec_tmp1->control_pid = spec_tmp1;
         spec_tmp1->control_node = proc_id;

         /* red counter, leave synchronization  */
         /* doesn't matter anymore */

         spec_tmp1->ppid = 0;     /* tag the process context */

         if ((PC_STATUS((PROCESS_CONTEXT *)spec_tmp1)==PS_WAITING) && !T_SPECWAIT((PROCESS_CONTEXT *)spec_tmp1)) {
           DBUG_PRINT("specsched", ("waking up the process..."));
 
#if (D_SLAVE && D_MESS && D_MSCHED)
           (*d_m_process_wakeup)(spec_tmp1);
#else
           process_wakeup(spec_tmp1);
#endif
           }

         /* set SPEC_GRANDDAD ! */

         PC_SET_SPEC_GRANDDAD(spec_tmp1);

         if (T_SPEC_LEAF(spec_tmp1)==1) {
           DBUG_PRINT("specsched", ("It was a SPEC LEAF !"));

/* fprintf (debugger, "It was a spec leaf process...\n"); fflush(debugger);   */

           PC_CLEAR_SPEC_PROCESS(spec_tmp1);
           PC_CLEAR_SPEC_LEAF(spec_tmp1);
           } else { /* ok, it's a DAD process alright */
           DBUG_PRINT("specsched", ("It was a SPEC DAD !"));

/* fprintf (debugger, "It was a spec dad process...\n"); fflush(debugger);  */

           PC_CLEAR_SPEC_DAD(spec_tmp1); /* can this cause any trouble ?! */
           
           if ((T_SPECWAIT(spec_tmp1)==1) && (PC_GET_SPEC_SPAWNED(spec_tmp1) ==
                PC_GET_SPEC_ACKS(spec_tmp1))) { /* acknowledge already sent, send again */

/* fprintf (debugger, "Send acknowledge again...\n"); fflush(debugger);  */

             DBUG_PRINT("specsched", ("send acknowledge again !"));

             spec_sched_status = 0;
#if D_SPEC_TICKETS
             spec_send_acknowledge(proc_id, spec_tmp1, spec_tmp1, 0, 0);
#else
             spec_send_acknowledge(proc_id, spec_tmp1, spec_tmp1);
#endif
             }
           }
   
         break;

#if D_SPEC_TICKETS

       case 11:

         /* [0] the pid which's got a postponed expression */


/*  fprintf(debugger, "0x%x: *SPECSCHED* distribute a postponed expression of 0x%x\n", curr_pid, spec_control[0]); fflush(debugger); */

         spec_tmp2 = dequeue_postpone(0,spec_control[0]);
       
         DBUG_PRINT("specsched", ("dequeuing expression of pointer 0x%x", spec_tmp2));

         spec_tickets_distribute(spec_get_distribute(), spec_tmp2, 1);

         break;

#endif /* D_SPEC_TICKETS */

       default: post_mortem ("speculative scheduler: unknown return code");
                break;
    }

  spec_sched_status = 0;

  /* fprintf(debugger, "** leaving speculative_scheduler...\n"); fflush(debugger);  */

  DBUG_VOID_RETURN;
}

#if D_SPEC_TICKETS

/***************************************************************************/
/*                                                                         */
/* spec_read_ticket_file()  reads the file 'spec.tickets' and initializes  */
/*                          the ticket pool                                */
/*                                                                         */
/* return value:       the size of the process table                       */
/*                                                                         */
/***************************************************************************/
 
int spec_read_ticket_file()
{FILE *tf;
 int number,high,low,ch,i,j,found,tab_size = 0,nb;
 int list1_index,list2_index,list_flag;
 int *list1, *list2, *ticket_list, cube_size;
 
 DBUG_ENTER("spec_read_ticket_file");
 
 cube_size = 1<<cube_dim;
 
 DBUG_PRINT("TICKET_SPEC", ("cube_size: %d", cube_size));
 
 list1 = (int *)malloc(cube_size*sizeof(int));
 list2 = (int *)malloc(cube_size*sizeof(int));
 ticket_list = (int *)malloc(cube_size*sizeof(int));
 
 if ((!list1) || (!list2) || (!ticket_list))
   abort_init("out of memory while reading spec ticket file");
 
 for (i=0;i<cube_size;i++)
   spec_ticket_pool[i] = ticket_list[i] = 0;
 
 tf = fopen(spec_ticket_file,"r");
 if (!tf)
   {DBUG_PRINT("TICKET",("Ticket file not found..."));
    fprintf(stderr, "*** warning: Could not open spec ticket file ***\n");
    for (i=0;i<cube_dim;i++)
      spec_ticket_pool[proc_id ^ (1 << i)] = 1;
    spec_first_lookup = spec_lookup_next = (proc_id + 1) % cube_size;
    DBUG_PRINT("spec_read_ticket_file", ("spec_lookup_next is %d", spec_lookup_next));
    DBUG_RETURN(cube_dim);}
 
 
 DBUG_PRINT("TICKET",("spec ticket file open"));
 do
   {while ((ch = getc(tf)) != '*')
      {if ((ch == '.') || (ch == EOF))
         {for (i=0;i<cube_dim;i++)
            spec_ticket_pool[proc_id ^ (1 << i)] = 1;
          spec_first_lookup = spec_lookup_next = (proc_id + 1) % cube_size;
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
                   spec_ticket_pool[list2[j]] = number;
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
                   spec_ticket_pool[nb] = number;
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
         spec_first_lookup = spec_lookup_next = number;}
 
    if (ch != '\n')
      abort_init("syntax error in ticket file");}
 
 if (spec_lookup_next == -1)
   spec_first_lookup = spec_lookup_next = (proc_id + 1) % cube_size;
 
 DBUG_RETURN(tab_size);}

/*********************************************************************/
/*                                                                   */
/* spec_get_distribute() - gets the number of the processor that     */
/*                     would be the destination of a distribution    */
/*                     AND it already decreases the spec_ticket_pool */
/*                                                                   */
/* return value:  the number of the processor if distribution is     */
/*                 possible                                          */
/*                -1 if distribution is not possible                 */
/*                                                                   */
/*********************************************************************/
 
int spec_get_distribute()
{int lookup;
 
 DBUG_ENTER("spec_get_distribute");
 
 if (spec_ticket_pool_counter == 0)
   DBUG_RETURN(-1);
 
 DBUG_PRINT("spec_get_distribute", ("spec_lookup_next/lookup: %d", spec_lookup_next));

 lookup = spec_lookup_next;

 for(;;)
   {if ((spec_ticket_pool[lookup] > 0) && (lookup != proc_id)) {

/* fprintf(debugger, "0x%x: *DISTRIBUTE before* lookup: %d, spec_ticket_pool[lookup]: %d, spec_lookup_next: %d\n", curr_pid, lookup, spec_ticket_pool[lookup], spec_lookup_next); */
/* fprintf(debugger, "0x%x: spec_ticket_pool[0] = %d, spec_ticket_pool[1] = %d, spec_ticket_pool[2] = %d, spec_ticket_pool[3] = %d\n", curr_pid, spec_ticket_pool[0], spec_ticket_pool[1], spec_ticket_pool[2], spec_ticket_pool[3]); fflush(debugger); */


      DBUG_PRINT("spec_get_distribute", ("lookup: %d, spec_ticket_pool[lookup]: %d, proc_id: %d", lookup, spec_ticket_pool[lookup], proc_id));
      spec_ticket_pool[lookup]--;
      spec_ticket_pool_counter--;

      spec_lookup_next = lookup+1;

/*fprintf(debugger, "0x%x: *DISTRIBUTE afterwards* lookup (target): %d, spec_ticket_pool_counter: %d, spec_ticket_pool[lookup]: %d, spec_lookup_next: %d\n", curr_pid, lookup, spec_ticket_pool_counter, spec_ticket_pool[lookup], spec_lookup_next); fflush(debugger); */
/*fprintf(debugger, "0x%x: spec_ticket_pool[0] = %d, spec_ticket_pool[1] = %d, spec_ticket_pool[2] = %d, spec_ticket_pool[3] = %d\n", curr_pid, spec_ticket_pool[0], spec_ticket_pool[1], spec_ticket_pool[2], spec_ticket_pool[3]); fflush(debugger);*/

      if (spec_lookup_next ==  (1 << cube_dim))
        spec_lookup_next = 0;
      DBUG_RETURN(lookup); }
    lookup++;
    if (lookup == (1 << cube_dim))
      lookup = 0;
    DBUG_PRINT("spec_get_distribute", ("inside loop, lookup: %d", lookup));
}}

/***************************************************************/
/* create_spec_frame creates the appropriate frame in the HEAP */

PTR_DESCRIPTOR create_spec_frame(asmcode,t_elems,a_elems)
int asmcode, t_elems, a_elems;
{
  PTR_DESCRIPTOR desc;
  PTR_HEAPELEM pth;
  int i;

  DBUG_ENTER("create_spec_frame");

  DBUG_PRINT("create_spec_frame", ("generating FRAME with %d T and %d A elements", t_elems, a_elems));

  GET_DESC(desc);
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  GET_HEAP(SPEC_SF_DATA+t_elems+a_elems,A_LIST(*desc,ptdv));

  DBUG_PRINT("create_spec_frame", ("desc is %x, ptdv for %d elements is %x !", desc, SPEC_SF_DATA+t_elems+a_elems, R_LIST(*desc,ptdv)));

  L_LIST(*desc,dim) = SPEC_SF_DATA+t_elems+a_elems;
 
  DBUG_PRINT("create_spec_frame", ("dim is %d !", R_LIST(*desc,dim)));

  RES_HEAP;

  /* initialize and copy frames */

  SPEC_SF_PROCESS(*desc) = curr_pid;
  SPEC_SF_ASMADDR(*desc) = asmcode;

  SPEC_SF_NUMB_T(*desc) = t_elems;
  SPEC_SF_NUMB_A(*desc) = a_elems;

  pth = &SPEC_SF_DATA_T(*desc);

  DBUG_PRINT("create_spec_frame", ("start of frames is %x !", pth));

  for (i=0; i<t_elems; i++) {
    *pth = D_MIDSTACK(*ps_t,i);
    DBUG_PRINT("create_spec_frame", ("copying %d T element %x to the frame", i, *pth));
    if (T_POINTER((PTR_DESCRIPTOR)*pth))
      INC_REFCNT((PTR_DESCRIPTOR)*pth);
    pth++;
    }

  for (i=0; i<a_elems; i++) {
    *pth = D_MIDSTACK(*ps_a,i);
    DBUG_PRINT("create_spec_frame", ("copying %d A element %x to the frame", i, *pth));
    if (T_POINTER((PTR_DESCRIPTOR)*pth))
      INC_REFCNT((PTR_DESCRIPTOR)*pth);
    pth++;
    }

  REL_HEAP;

  DBUG_RETURN(desc);
}

/*************************************************************/
/* erase_spec_frame clears and frees a spec frame descriptor */

void erase_spec_frame(desc)
PTR_DESCRIPTOR desc;
{
  int i;
  int *help;

  DBUG_ENTER("erase_spec_frame");

  for (i=SPEC_SF_DATA; i<SPEC_SF_DATA+SPEC_SF_NUMB_T(*desc)+SPEC_SF_NUMB_A(*desc); i++) {
    desc->u.li.ptdv[i] = T_INT(42);
    }

  for (i=0; i<SPEC_SF_DATA; i++)
    desc->u.li.ptdv[i] = T_INT(42);

  DEC_REFCNT(desc); 

  DBUG_PRINT("erase_spec_frame", ("descriptor free'd"));

  DBUG_VOID_RETURN;
}

/************************************************************************/
/* spec_insert_frame inserts the stackframes in the current stacksystem */

void spec_insert_frame(desc)
PTR_DESCRIPTOR desc;
{
  int i;

  DBUG_ENTER("spec_insert_frame");

  DBUG_PRINT("spec_insert_frame", ("inserting %x elements on stack T", SPEC_SF_NUMB_T(*desc)));

  for (i=SPEC_SF_DATA+SPEC_SF_NUMB_T(*desc)-1; i>=SPEC_SF_DATA; i--) {
    DBUG_PRINT("spec_insert_frame", ("pushing #%d: 0x%x (INT: %d) on T", i, desc->u.li.ptdv[i], VAL_INT(desc->u.li.ptdv[i])));
    D_PUSHSTACK(*ps_t,(desc->u.li.ptdv[i]));
    if (T_POINTER((PTR_DESCRIPTOR) desc->u.li.ptdv[i])) {
      DBUG_PRINT("spec_insert_frame", ("increasing refcount for element %d (0x%x)", i, (PTR_DESCRIPTOR) desc->u.li.ptdv[i]));
      INC_REFCNT((PTR_DESCRIPTOR) desc->u.li.ptdv[i]);
      }   
    }

  DBUG_PRINT("spec_insert_frame", ("inserting %x elements on stack A", SPEC_SF_NUMB_A(*desc)));

  for (i=SPEC_SF_DATA+SPEC_SF_NUMB_T(*desc)+SPEC_SF_NUMB_A(*desc)-1; i>=SPEC_SF_DATA+SPEC_SF_NUMB_T(*desc); i--) {
    DBUG_PRINT("spec_insert_frame", ("pushing #%d: 0x%x (INT: %d) on A", i, desc->u.li.ptdv[i], VAL_INT(desc->u.li.ptdv[i])));
    D_PUSHSTACK(*ps_a,(desc->u.li.ptdv[i]));
    if (T_POINTER((PTR_DESCRIPTOR) desc->u.li.ptdv[i])) {
      DBUG_PRINT("spec_insert_frame", ("increasing refcount for element %d (0x%x)", i, (PTR_DESCRIPTOR) desc->u.li.ptdv[i]));
      INC_REFCNT((PTR_DESCRIPTOR) desc->u.li.ptdv[i]);
      }
    }

  DBUG_VOID_RETURN;
}

/************************************************************************/
/* enqueue_postpone creates a list descriptor for a postponed specframe */

PTR_DESCRIPTOR enqueue_postpone(fdesc, nesting)
PTR_DESCRIPTOR fdesc;
int nesting;
{
  PTR_DESCRIPTOR desc;

  DBUG_ENTER("enqueue_postpone");

  GET_DESC(desc);
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  GET_HEAP(4,A_LIST(*desc,ptdv));
  L_LIST(*desc,dim) = 4;
  RES_HEAP;

  SPEC_PP_PREV(*desc) = curr_pid->spec_postpone_end;
  SPEC_PP_NEXT(*desc) = NULL;
  SPEC_PP_NESTING(*desc) = nesting;
  SPEC_PP_FRAME(*desc) = fdesc;

  REL_HEAP;

  if (curr_pid->spec_postpone_end) {
    DBUG_PRINT("enqueue_postpone", ("it is not the last element..."));
    SPEC_PP_NEXT(*(PTR_DESCRIPTOR)curr_pid->spec_postpone_end) = desc;
    } else {
    DBUG_PRINT("enqueue_postpone", ("it is the last element..."));
    }
  curr_pid->spec_postpone_end = desc;
  if (!curr_pid->spec_postpone_start) {
    DBUG_PRINT("enqueue_postpone", ("it is the first element..."));
    curr_pid->spec_postpone_start = desc;
    } else {
    DBUG_PRINT("enqueue_postpone", ("it is not the first element..."));
    }

  DBUG_PRINT("enqueue_postpone", ("*** STATUS after enqueue_postpone: ***"));
  DBUG_PRINT("enqueue_postpone", ("spec_postpone_start: 0x%x, spec_postpone_end: 0x%x", curr_pid->spec_postpone_start, curr_pid->spec_postpone_end));
  DBUG_PRINT("enqueue_postpone", ("entries of postpone descriptor 0x%x:", desc));
  DBUG_PRINT("enqueue_postpone", ("PREV: %x, NEXT: %x, NESTING: %d, FRAME: 0x%x", SPEC_PP_PREV(*desc), SPEC_PP_NEXT(*desc), SPEC_PP_NESTING(*desc), SPEC_PP_FRAME(*desc)));

  DBUG_RETURN(desc);
}

/**************************************************************************************/
/* dequeue_postpone dequeues an entry, free's the descriptor and returns the frameptr */

PTR_DESCRIPTOR dequeue_postpone(flag,pid)
int flag; /* 1 if the last element has to be evaluated and 0 if the first... err... */
PROCESS_CONTEXT *pid;

{
  PTR_DESCRIPTOR help, result;

  DBUG_ENTER("dequeue_postpone");

  if (pid->spec_postpone_start == NULL) { /* no expressions enqueued */
    DBUG_PRINT("dequeue_postpone", ("no expressions enqueued..."));
    DBUG_RETURN(NULL);
    }

  if (pid->spec_postpone_start == pid->spec_postpone_end) {
    DBUG_PRINT("dequeue_postpone", ("there's only one entry"));
    help = pid->spec_postpone_start;
    pid->spec_postpone_start = NULL;
    pid->spec_postpone_end = NULL;
    } else {

    DBUG_PRINT("dequeue_postpone", ("there are several entries..."));
  
    if (flag) {
      DBUG_PRINT("dequeue_postpone", ("choose the last element of the list..."));
  
      help = pid->spec_postpone_end;
      SPEC_PP_NEXT(*(PTR_DESCRIPTOR)(int)SPEC_PP_PREV(*help)) = NULL;
      pid->spec_postpone_end = SPEC_PP_PREV(*help);
  
      } else {
      DBUG_PRINT("dequeue_postpone", ("choose the first element of the list..."));
  
      help = pid->spec_postpone_start;
      SPEC_PP_PREV(*(PTR_DESCRIPTOR)SPEC_PP_NEXT(*help)) = NULL;
      pid->spec_postpone_start = SPEC_PP_NEXT(*help);
  
      }
    }

  DBUG_PRINT("dequeue_postpone", ("chosen was %x, nesting %d", help, SPEC_PP_NESTING(*help)));

  /* dealloc list descriptor */
  
  result = SPEC_PP_FRAME(*help);

  SPEC_PP_NEXT(*help) = TAG_INT(42);
  SPEC_PP_PREV(*help) = TAG_INT(42);
  SPEC_PP_NESTING(*help) = TAG_INT(42);
  SPEC_PP_FRAME(*help) = TAG_INT(42);
 
  DEC_REFCNT(help); 

  DBUG_PRINT("dequeue_postpone", ("returning %x as frame-pointer", result));

  DBUG_RETURN(result);
}

/**********************************************************************************/
/* sort_postpone creates a list descriptor for a postponed specframe and sorts it */
/*               into the queue                                                   */
 
PTR_DESCRIPTOR sort_postpone(fdesc, nesting)
PTR_DESCRIPTOR fdesc;
int nesting;
{
  PTR_DESCRIPTOR desc, help1, help2;
 
  DBUG_ENTER("sort_postpone");
 
  GET_DESC(desc);
  LI_DESC_MASK(desc,1,C_LIST,TY_UNDEF);
  GET_HEAP(4,A_LIST(*desc,ptdv));
  L_LIST(*desc,dim) = 4;
  RES_HEAP;
 
  SPEC_PP_NESTING(*desc) = nesting;
  SPEC_PP_FRAME(*desc) = fdesc;
 
  REL_HEAP;
 
  /* sort it in top down */

  if (curr_pid->spec_postpone_start == NULL) {
    DBUG_PRINT("sort_postpone", ("there was no entry yet..."));
    curr_pid->spec_postpone_start = desc;
    curr_pid->spec_postpone_end = desc;
    SPEC_PP_NEXT(*desc) = NULL;
    SPEC_PP_PREV(*desc) = NULL;
    }

  if (curr_pid->spec_postpone_start == curr_pid->spec_postpone_end) {

    DBUG_PRINT("sort_postpone", ("there is only one entry, nesting: %d, new nesting: %d", SPEC_PP_NESTING(*(PTR_DESCRIPTOR)curr_pid->spec_postpone_start), nesting));

    if (SPEC_PP_NESTING(*(PTR_DESCRIPTOR)curr_pid->spec_postpone_start) > nesting) {
      DBUG_PRINT("sort_postpone", ("enqueue new entry as first"));
      SPEC_PP_PREV(*desc) = NULL;
      SPEC_PP_NEXT(*desc) = curr_pid->spec_postpone_start;
      SPEC_PP_PREV(*(PTR_DESCRIPTOR)curr_pid->spec_postpone_start) = desc;
      curr_pid->spec_postpone_start = desc;
      } else {
      DBUG_PRINT("sort_postpone", ("enqueue new entry as last"));
      SPEC_PP_NEXT(*desc) = NULL;
      SPEC_PP_PREV(*desc) = curr_pid->spec_postpone_start;
      SPEC_PP_NEXT(*(PTR_DESCRIPTOR)curr_pid->spec_postpone_start) = desc;
      curr_pid->spec_postpone_end = desc;
      }
    } else {

    DBUG_PRINT("sort_postpone", ("there are several entries..."));

    help1 = NULL; /* previous entry */
    help2 = curr_pid->spec_postpone_start; /* comparing entry */

    DBUG_PRINT("sort_postpone", ("compare nesting %d with stored nesting %d (%x)", nesting, SPEC_PP_NESTING(*help2), help2));

    while ((SPEC_PP_NESTING(*help2)<=nesting) && (SPEC_PP_NEXT(*help2) != NULL)) {
      help1 = help2;
      help2 = SPEC_PP_NEXT(*help1); /* shift one entry forward */
      DBUG_PRINT("sort_postpone", ("compare nesting %d with stored nesting %d (%x)", nesting, SPEC_PP_NESTING(*help2), help2));
      }

    if (SPEC_PP_NEXT(*help2) == NULL) { /* the last element was reached */
      DBUG_PRINT("sort_postpone", ("the last element was reached"));

      if (SPEC_PP_NESTING(*help2) > nesting) {
        DBUG_PRINT("sort_postpone", ("enqueue entry before the last one"));

        SPEC_PP_NEXT(*help1) = desc;
        SPEC_PP_PREV(*desc) = help1;
        SPEC_PP_NEXT(*desc) = help2;
        SPEC_PP_PREV(*help2) = desc;
        } else {
        DBUG_PRINT("sort_postpone", ("enqueue entry as the last one"));

        SPEC_PP_NEXT(*help2) = desc;
        SPEC_PP_PREV(*desc) = help2;
        SPEC_PP_NEXT(*desc) = NULL;
        curr_pid->spec_postpone_end = desc;
        }
      } else {
      DBUG_PRINT("sort_postpone", ("point of insertion was reached..."));

      SPEC_PP_NEXT(*desc) = help2;
      SPEC_PP_PREV(*desc) = help1;
      SPEC_PP_PREV(*help2) = desc;
      if (help2 = curr_pid->spec_postpone_start) {
        DBUG_PRINT("sort_postpone", ("enqueue as first element"));

        curr_pid->spec_postpone_start = desc;
        } else {
        DBUG_PRINT("sort_postpone", ("enqueue as some mid element"));

        SPEC_PP_NEXT(*help1) = desc;
        }
      }
    }
 
  DBUG_RETURN(desc);
}

/****************************************************************/
/* spec_tickets_distribute spawns a speculative process         */

void spec_tickets_distribute(node, fdesc, flag)
int node;
PTR_DESCRIPTOR fdesc;
int flag; /* this is 1 if the process is created by the spec scheduler and not by code */
{
 int i;

 DBUG_ENTER("spec_tickets_distribute");

 DBUG_PRINT("spec_tickets_distribute", ("spawn descriptor %x to node %d !", fdesc, node));
 DBUG_PRINT("spec_tickets_distribute", ("ptdv is %x !", fdesc->u.li.ptdv));

 DBUG_PRINT("spec_tickets_distribute", ("entries as follows:"));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_PROCESS: %x", SPEC_SF_PROCESS(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_PSTATUS: %x", SPEC_SF_PSTATUS(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_ASMADDR: %x", SPEC_SF_ASMADDR(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_ASMMKNC: %x", SPEC_SF_ASMMKNC(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_REDSTEPS: %d", SPEC_SF_REDSTEPS(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_NESTING: %x", SPEC_SF_NESTING(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_FIRSTF: %d", SPEC_SF_FIRSTF(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_ADDRCOUNT: %x", SPEC_SF_ADDRCOUNT(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_NUMB_T: %d", SPEC_SF_NUMB_T(*fdesc)));
 DBUG_PRINT("spec_tickets_distribute", ("SPEC_SF_NUMB_A: %d", SPEC_SF_NUMB_A(*fdesc)));


 dist_stack_elems = SPEC_SF_NUMB_A(*fdesc);
 dist_stack_free = 0;
 dist_tilde_elems = SPEC_SF_NUMB_T(*fdesc);
 dist_tilde_free = 0;
 dist_type = DIST_N;
 dist_instr = SPEC_SF_ASMADDR(*fdesc);

 spec_frame = fdesc;

 if (flag == 1) {
   DBUG_PRINT("spec_tickets_distribute", ("activating spec_post_proc !"));
   spec_post_proc = 1;
   } else
   spec_post_proc = 0;

/* fprintf(debugger, "0x%x: spawning process to node %d ! (spec_post_proc = %d)\n", curr_pid, node, spec_post_proc); fflush(debugger);*/
  
#if (D_SLAVE && D_MESS && D_MCOMMU)
 (*d_m_msg_send)(node,MT_NEW_PROCESS);
#else
 msg_send(node,MT_NEW_PROCESS);
#endif

 spec_frame = NULL;
 spec_post_proc = 0;

 /* release stackframe-elements */

 for (i=SPEC_SF_DATA; i<SPEC_SF_DATA+SPEC_SF_NUMB_T(*fdesc)+SPEC_SF_NUMB_A(*fdesc); i++) {
   DBUG_PRINT("spec_tickets_distribute", ("freeing %x", fdesc->u.li.ptdv[i]));
   if (T_POINTER((PTR_DESCRIPTOR)fdesc->u.li.ptdv[i]))
     DEC_REFCNT((PTR_DESCRIPTOR)fdesc->u.li.ptdv[i]);
   }

 DBUG_VOID_RETURN;
}

/******************************************************************************************************************/
/* query_postpone: searches through the spec_in_use_list and returns a pid if there are any expressions postponed */

int query_postpone()
{
 int found_one;
 int found_nesting;
 int found_pid;
 PROCESS_CONTEXT *dummy;

 DBUG_ENTER("query_postpone");

 found_one = 0;

 dummy = in_use_queue_first;

 DBUG_PRINT("query_postpone", ("searching in_use_queue_first first..."));

 while (dummy != NULL) {
  DBUG_PRINT("query_postpone", ("searching process 0x%x (postpone_start: 0x%x) !", dummy, dummy->spec_postpone_start));

  if (dummy->spec_postpone_start != NULL) {
    if (!found_one) {
      DBUG_PRINT("query_postpone", ("found the FIRST entry at all"));

      found_one = 1; 
      found_pid = dummy;
      found_nesting = SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start));
      DBUG_PRINT("query_postpone", ("found the FIRST entry at all: pid 0x%x, nesting %d", found_pid, found_nesting));

      } else {
      if (SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start)) < found_nesting) {
        found_pid = dummy;
        found_nesting = SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start));
        DBUG_PRINT("query_postpone", ("found a new entry: pid 0x%x, nesting %d", found_pid, found_nesting));

        } else {
        DBUG_PRINT("query_postpone", ("nothing changes... nesting was too high (%d >= %d)", SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start)), found_nesting));

        }
      }
    }
    dummy = dummy->in_use_next;
  }

 dummy = spec_in_use_queue_first;

 DBUG_PRINT("query_postpone", ("searching spec_in_use_queue_first next..."));
 
 while (dummy != NULL) {
  DBUG_PRINT("query_postpone", ("searching process 0x%x (postpone_start: 0x%x) !", dummy, dummy->spec_postpone_start));

  if (dummy->spec_postpone_start != NULL) {
    if (!found_one) {
      DBUG_PRINT("query_postpone", ("found the FIRST entry at all"));
 
      found_one = 1;
      found_pid = dummy;
      found_nesting = SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start));
      DBUG_PRINT("query_postpone", ("found the FIRST entry at all: pid 0x%x, nesting %d", found_pid, found_nesting));
 
      } else {
      if (SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start)) < found_nesting) {
        found_pid = dummy;
        found_nesting = SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start));
        DBUG_PRINT("query_postpone", ("found a new entry: pid 0x%x, nesting %d", found_pid, found_nesting));
 
        } else {
        DBUG_PRINT("query_postpone", ("nothing changes... nesting was too high (%d >= %d)", SPEC_PP_NESTING(*((PTR_DESCRIPTOR)dummy->spec_postpone_start)), found_nesting));
 
        }
      }
    }
    dummy = dummy->in_use_next;
  }

 if (found_one == 1) {
/*   fprintf(debugger, "0x%x: query_postpone: distribute a process off pid 0x%x !\n", curr_pid, found_pid); fflush(debugger);*/

   DBUG_RETURN(found_pid);
   }
 else {
/*   fprintf(debugger, "0x%x: query_postpone: no distribution possible...\n", curr_pid); fflush(debugger);*/
   DBUG_RETURN(0);
   }
}

/*****************************************************************************************/
/* spec_check_pid debug function, checks whether pid is still in sync when terminated... */

void spec_check_pid(pid)
PROCESS_CONTEXT *pid;
{
  int i;
  PROCESS_CONTEXT *control_pid;

  DBUG_ENTER("spec_check_pid");

  control_pid = pid->control_pid;

  for (i=0; i<control_pid->control->leaves; i++) {
    if (control_pid->control->data[2*i] == pid) {

/* fprintf(debugger, "0x%x: process 0x%x still in pipe while terminating it !\n", curr_pid, pid); fflush(debugger);*/
      exit(99);
    }
   }

  DBUG_VOID_RETURN;
}

#endif /* D_SPEC_TICKETS */

#endif /* D_SPEC */
