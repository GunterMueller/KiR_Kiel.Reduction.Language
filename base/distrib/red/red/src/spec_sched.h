/***************************************************************************/
/*                                                                         */
/*   file     : spec_sched.h                                               */
/*                                                                         */
/*   contents : defines for time-slicing scheduling                        */
/*              errrr, more or less for speculative evaluation             */
/*              and process scheduling !                                   */
/*                                                                         */
/*   last change:                                                          */
/*                                                                         */
/*         13.09.1994 (created. REALLY !)                                  */
/*         xxxxxxxxxx see rcslog... ;-)                                    */
/*                                                                         */
/***************************************************************************/

#ifndef _spec_sched
#define _spec_sched

#include "rncsched.h"

/* extern variables */

#define SPEC_CONSTRUCTOR    "_PARTIAL_EVAL_SPEC"
#define SPEC_GUARD_CONSTRUCTOR "_SPEC_GUARDEXPR"

extern int spec_multi_results;
extern int spec_kill_no_steps;
extern int spec_nondet;
extern int nospecnr;
extern int spec_retrans;
extern int spec_message_size;
extern int spec_next_proc;
extern int spec_control[10];
extern int ack_red_count, max_spec_sons, max_spec_procs, spec_global, spec_sched_status;
extern int spec_failcase;
extern int spec_mspecial;
extern PROCESS_CONTEXT *spec_ready_queue_first; 
extern PROCESS_CONTEXT *spec_ready_queue_last; 
extern PROCESS_CONTEXT *spec_released_queue_first; 
extern PROCESS_CONTEXT *spec_released_queue_last;  
extern PROCESS_CONTEXT *spec_process_table;
extern PROCESS_CONTEXT *spec_in_use_queue_first; 
extern PROCESS_CONTEXT *spec_in_use_queue_last; 
extern int _redcnt;

#if D_SPEC_TICKETS
extern int spec_ticket_pool[];
extern int spec_lookup_next;
extern int spec_first_lookup;
extern int spec_proc_tab_size;
extern int spec_ticket_pool_counter;
extern char spec_ticket_file[];

extern int spec_get_distribute();
extern int spec_read_ticket_file();

extern PTR_DESCRIPTOR spec_frame;
extern int spec_post_proc;

extern tickets_desc;
#endif /* D_SPEC_TICKETS */

/* extern int ts_counter;
#define SCHED_TIME_OUT  (!--ts_counter)
#define RESET_TIMER     ts_counter=100
^
| this was the VERY old stuff, even before the UH-PM... fascinating ;-)
*/

/* extern functions */

/*
extern void spec_send_acknowledge();
extern void spec_send_goon();
extern void spec_send_hereami();
extern void spec_send_fail();
extern void spec_send_term();
extern void spec_send_rem(); */

extern void stackslen();

extern int query_postpone();

/* DEFINEs */

#define MESSAGE_SIZE_OLD 1024

#define MAX_SPEC_PROCS 500 /* hard coded because initialization takes place */
                           /* before the first communication */

#define SPEC_GRANDDAD 1      /* process is the root for a speculative thread */
#define SPEC_DAD      2      /* speculative process has spawned other processes */
#define SPEC_LEAF     4      /* speculative process is a leaf process */
#define HAVE_ACKNOWLEDGED 8  /* speculative process has run over the (first) barrier */
#define SPECWAIT     16      /* I_SPECWAIT reached */
#define SPEC_WINNER  32      /* this process has is dealing with a result */
#define SPEC_READY   64      /* newly created speculative process is ready for execution */
#define SPEC_SIGNOFF 128     /* speculative dad-process waits for newly spawned processes */
                             /* to acknowledge and to be terminated after that */
                             /* or: if it's a termination-message: terminate the shit ! */
#define SPEC_PROCESS 256     /* finally: this flag is set if it's a speculative process */
#define SPEC_CONF_WIN 512    /* flag that confirms that this process is chosen */
#define SPEC_WAIT_WIN  1024 /* confirms that the process is wating for a WIN acknowledge */
#define SPEC_NO_BSTEPS  2048   /* this is set if NO more barrier steps will be supplied */
#define SPEC_NO_STEPS 4096    /* this is set if NO more reduction steps are available */
#define SPEC_FOLD 8192        /* this flag is set when a process is participating in the re-folding of the search tree */
#define SPEC_FAIL 16384      /* this flag denotes that a speculative process has failed */
#define SPEC_CONF_FAIL 32768  /* confirms that the process has received the term message */

#define SPEC_GUARD 65536     /* signals that one or more guards ran into a problem */
                             /* and is used for instant constructor reconstruction */
#define SPEC_UNRED_GUARD 131072 /* a guard was not reduced to true or false */
#define SPEC_INBETWEEN  262144  /* this Flag is set by SPECINIT and is cleared by SPECWAIT */
#define SPEC_GUARD_SON 524288  /* this flag signals that the SON was created in an SCASE with an unreduced guard */
#define SPEC_INSYNC 1048576   /* this flag depict whether the process is (still) in barrier synchronisation */
#define SPEC_BARRSUS 2097152 /* this flag is set if a process was suspended due to the lack of barrier redsteps */

#define PC_SET_SPEC_BARRSUS(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_BARRSUS
#define T_SPEC_BARRSUS(pid)        (((pid)->specmask1 & SPEC_BARRSUS)==SPEC_BARRSUS)
#define PC_CLEAR_SPEC_BARRSUS(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_BARRSUS)
#define T_S_BARRSUS(pid)           (((pid) & SPEC_BARRSUS)==SPEC_BARRSUS)

#define PC_SET_SPEC_INSYNC(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_INSYNC
#define T_SPEC_INSYNC(pid)        (((pid)->specmask1 & SPEC_INSYNC)==SPEC_INSYNC)
#define PC_CLEAR_SPEC_INSYNC(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_INSYNC)
#define T_S_INSYNC(pid)           (((pid) & SPEC_INSYNC)==SPEC_INSYNC)

#define PC_SET_SPEC_GUARD_SON(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_GUARD_SON
#define T_SPEC_GUARD_SON(pid)        (((pid)->specmask1 & SPEC_GUARD_SON)==SPEC_GUARD_SON)
#define PC_CLEAR_SPEC_GUARD_SON(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_GUARD_SON)
#define T_S_GUARD_SON(pid)           (((pid) & SPEC_GUARD_SON)==SPEC_GUARD_SON)

#define PC_SET_SPEC_INBETWEEN(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_INBETWEEN
#define T_SPEC_INBETWEEN(pid)        (((pid)->specmask1 & SPEC_INBETWEEN)==SPEC_INBETWEEN)
#define PC_CLEAR_SPEC_INBETWEEN(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_INBETWEEN)
#define T_S_INBETWEEN(pid)           (((pid) & SPEC_INBETWEEN)==SPEC_INBETWEEN)

#define PC_SET_SPEC_UNRED_GUARD(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_UNRED_GUARD
#define T_SPEC_UNRED_GUARD(pid)        (((pid)->specmask1 & SPEC_UNRED_GUARD)==SPEC_UNRED_GUARD)
#define PC_CLEAR_SPEC_UNRED_GUARD(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_UNRED_GUARD)
#define T_S_UNRED_GUARD(pid)           (((pid) & SPEC_UNRED_GUARD)==SPEC_UNRED_GUARD)

#define PC_SET_SPEC_GUARD(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_GUARD
#define T_SPEC_GUARD(pid)        (((pid)->specmask1 & SPEC_GUARD)==SPEC_GUARD)
#define PC_CLEAR_SPEC_GUARD(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_GUARD)
#define T_S_GUARD(pid)           (((pid) & SPEC_GUARD)==SPEC_GUARD)

#define PC_SET_SPEC_CONF_FAIL(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_CONF_FAIL
#define T_SPEC_CONF_FAIL(pid)        (((pid)->specmask1 & SPEC_CONF_FAIL)==SPEC_CONF_FAIL)
#define PC_CLEAR_SPEC_CONF_FAIL(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_CONF_FAIL)
#define T_S_CONF_FAIL(pid)           (((pid) & SPEC_CONF_FAIL)==SPEC_CONF_FAIL)

#define PC_SET_SPEC_FAIL(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_FAIL
#define T_SPEC_FAIL(pid)        (((pid)->specmask1 & SPEC_FAIL)==SPEC_FAIL)
#define PC_CLEAR_SPEC_FAIL(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_FAIL)
#define T_S_FAIL(pid)           (((pid) & SPEC_FAIL)==SPEC_FAIL)

#define PC_SET_SPEC_FOLD(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_FOLD
#define T_SPEC_FOLD(pid)        (((pid)->specmask1 & SPEC_FOLD)==SPEC_FOLD)
#define PC_CLEAR_SPEC_FOLD(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_FOLD)

#define PC_SET_SPEC_NO_BSTEPS(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_NO_BSTEPS
#define T_SPEC_NO_BSTEPS(pid)        (((pid)->specmask1 & SPEC_NO_BSTEPS)==SPEC_NO_BSTEPS)
#define PC_CLEAR_SPEC_NO_BSTEPS(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_NO_BSTEPS)
#define T_S_NO_BSTEPS(pid)           (((pid) & SPEC_NO_BSTEPS)==SPEC_NO_BSTEPS)

#define PC_SET_SPEC_NO_STEPS(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_NO_STEPS
#define T_SPEC_NO_STEPS(pid)        (((pid)->specmask1 & SPEC_NO_STEPS)==SPEC_NO_STEPS)
#define PC_CLEAR_SPEC_NO_STEPS(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_NO_STEPS)
#define T_S_NO_STEPS(pid)           (((pid) & SPEC_NO_STEPS)==SPEC_NO_STEPS)

#define PC_SET_SPEC_WAIT_WIN(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_WAIT_WIN
#define T_SPEC_WAIT_WIN(pid)        (((pid)->specmask1 & SPEC_WAIT_WIN)==SPEC_WAIT_WIN)
#define PC_CLEAR_SPEC_WAIT_WIN(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_WAIT_WIN)

#define PC_SET_SPEC_CONF_WIN(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_CONF_WIN
#define T_SPEC_CONF_WIN(pid)        (((pid)->specmask1 & SPEC_CONF_WIN)==SPEC_CONF_WIN)
#define PC_CLEAR_SPEC_CONF_WIN(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_CONF_WIN)

#define PC_SET_SPEC_PROCESS(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_PROCESS
#define T_SPEC_PROCESS(pid)         (((pid)->specmask1 & SPEC_PROCESS)==SPEC_PROCESS)
#define PC_CLEAR_SPEC_PROCESS(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_PROCESS)

#define PC_SET_SPEC_WINNER(pid)   (pid)->specmask1 = (pid)->specmask1 | SPEC_WINNER
#define T_SPEC_WINNER(pid)         (((pid)->specmask1 & SPEC_WINNER)==SPEC_WINNER)
#define PC_CLEAR_SPEC_WINNER(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_WINNER)
#define T_S_WINNER(pid)         (((pid) & SPEC_WINNER)==SPEC_WINNER)

#define PC_SET_SPEC_READY(pid)    (pid)->specmask1 = (pid)->specmask1 | SPEC_READY
#define T_SPEC_READY(pid)         (((pid)->specmask1 & SPEC_READY)==SPEC_READY)
#define PC_CLEAR_SPEC_READY(pid)  (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_READY)

#define PC_SET_SIGNOFF(pid)     (pid)->specmask1 = (pid)->specmask1 | SPEC_SIGNOFF
#define T_SPEC_SIGNOFF(pid)          (((pid)->specmask1 & SPEC_SIGNOFF)==SPEC_SIGNOFF)
#define PC_CLEAR_SPEC_SIGNOFF(pid)   (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_SIGNOFF)

#define PC_SET_SPECWAIT(pid)      (pid)->specmask1 = (pid)->specmask1 | SPECWAIT
#define T_SPECWAIT(pid)           (((pid)->specmask1 & SPECWAIT)==SPECWAIT)
#define PC_CLEAR_SPECWAIT(pid)    (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPECWAIT)

#define PC_SET_SPEC_GRANDDAD(pid)  (pid)->specmask1 = (pid)->specmask1 | SPEC_GRANDDAD
#define T_SPEC_GRANDDAD(pid)       (((pid)->specmask1 & SPEC_GRANDDAD)==SPEC_GRANDDAD)
#define PC_CLEAR_SPEC_GRANDDAD(pid) (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_GRANDDAD)
#define PC_SET_SPEC_LEAF(pid)      (pid)->specmask1 = (pid)->specmask1 | SPEC_LEAF
#define T_SPEC_LEAF(pid)           (((pid)->specmask1 & SPEC_LEAF)>0)
#define PC_CLEAR_SPEC_LEAF(pid)    (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_LEAF)
#define PC_SET_SPEC_DAD(pid)       (pid)->specmask1 = (pid)->specmask1 | SPEC_DAD
#define T_SPEC_DAD(pid)            (((pid)->specmask1 & SPEC_DAD)==SPEC_DAD)
#define PC_CLEAR_SPEC_DAD(pid)     (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - SPEC_DAD) 

#define SET_ACKNOWLEDGE(pid)       (pid)->specmask1 = (pid)->specmask1 | HAVE_ACKNOWLEDGED
#define CLEAR_ACKNOWLEDGE(pid)     (pid)->specmask1 = (pid)->specmask1 & (0xffffffff - HAVE_ACKNOWLEDGED)
#define T_ACKNOWLEDGE(pid)         (((pid)->specmask1 & HAVE_ACKNOWLEDGED)>0)
#define T_S_ACKNOWLEDGE(pid)       (((pid) & HAVE_ACKNOWLEDGED)==HAVE_ACKNOWLEDGED)

#define T_DATA_ACK(x)      (((x) & (1<<31)) == (1<<31))
#define SET_DATA_ACK(x)    (x) = (x) | (1<<31)
#define CLEAR_DATA_ACK(x)  (x) = (x) & 0x7fffffff
#define GET_DATA_NODE(x)   ((x) & 0x7fffffff)

/* more macros to access the enlarged process context */

/* specmask 2 is for counters... */

/* counter how many specdists were executed */
#define PC_GET_SPEC_SPAWNED(pid)   ((pid)->specmask2 >> 24)
#define PC_SET_SPEC_SPAWNED(pid,wert) (pid)->specmask2 = ((pid)->specmask2 & 0x00ffffff) | ((wert) << 24)

/* counter how many speculative sons have acknowledged yet */
#define PC_GET_SPEC_ACKS(pid)      (((pid)->specmask2 & 0x00ff0000) >> 16)
#define PC_SET_SPEC_ACKS(pid,wert) (pid)->specmask2 = ((pid)->specmask2 & 0xff00ffff) | ((wert) << 16)

/* counter how many results are collected */
#define PC_GET_SPEC_SUM(pid)      ((pid)->specmask2 & 0x000000FF)
#define PC_SET_SPEC_SUM(pid,wert) (pid)->specmask2 = ((pid)->specmask2 & 0xffffff00) | (wert)

/* the following code is inserted if there are no more barrier steps available in rinter.c */

#define SPEC_CODE_1(wert,befehl) \
     if (T_ACKNOWLEDGE(curr_pid)==1) { \
       if (T_SPEC_NO_BSTEPS(curr_pid)==1) { /* no more barriersteps will come */ \
         DBUG_PRINT("INTER", ("acknowledge is set, but no more steps will come ;)"));\
         DBUG_PRINT("specsched", ("start_red_count: %d, red_count: %d", curr_pid->start_red_count, curr_pid->red_count));\
         curr_pid->start_red_count = curr_pid->start_red_count - curr_pid->red_count;\
         curr_pid->red_count = 0;\
         _redcnt = 0; \
         /* post_mortem("out of BSTEPS must NOT occur !");  */ \
         DBUG_PRINT("specsched", ("start_red_count: %d, red_count: %d", curr_pid->start_red_count, curr_pid->red_count));\
         PC_SET_SPEC_FAIL(curr_pid); \
         DBUG_PRINT("specsched", ("setting FOLD bit")); \
         PC_SET_SPEC_FOLD(curr_pid);  \
         code = ((INSTR *)code-wert); \
         goto loop; \
         } else { \
         DBUG_PRINT("INTER", ("acknowledge is set, let's suspend the process...")); \
         /* fprintf(debugger, "%x: go to sleep...\n", curr_pid); fflush(debugger); */ \
         code = ((INSTR *)code-wert); \
         DBUG_PRINT("INTER", ("code is now %x", code)); \
         /* PC_SET_SPEC_BARRSUS(curr_pid); */ \
         /* fprintf(debugger, "0x%x: suspending process 'cause out of spec_red_steps (specmask1 = 0x%x)\n", curr_pid, curr_pid->specmask1); fflush(debugger); */  \
         /* fprintf(debugger, "0x%x: sleep 6\n", curr_pid); fflush(debugger); */ \
         DBUG_RETURN(IR_WAIT); } \
       } else { \
         DBUG_PRINT("INTER", ("not yet acknowledged, re-initialize _spec_redcnt !")); \
         _spec_redcnt = ack_red_count; \
         SET_ACKNOWLEDGE(curr_pid); \
	 /* fprintf(debugger, "0x%x: out of barrier steps and flag was NOT set yet\n", curr_pid); fflush(debugger); */ \
	 _spec_redcnt = ack_red_count; \
         spec_send_barrier(curr_pid); \
         if ((spec_nondet == 1) && (T_SPEC_NO_BSTEPS(curr_pid)==1)) { \
         DBUG_PRINT("INTER", ("nevertheless stop this process...")); \
         curr_pid->start_red_count = curr_pid->start_red_count - curr_pid->red_count;\
         curr_pid->red_count = 0;\
         _redcnt = 0; \
         DBUG_PRINT("specsched", ("start_red_count: %d, red_count: %d", curr_pid->start_red_count, curr_pid->red_count));\
         PC_SET_SPEC_FAIL(curr_pid); \
         DBUG_PRINT("specsched", ("setting FOLD bit")); \
         PC_SET_SPEC_FOLD(curr_pid); } \
         DBUG_PRINT("INTER", ("repeat last ASM command, code %x, %d", code,wert)); \
         code = ((INSTR *)code-wert); \
         DBUG_PRINT("INTER", ("code is now %x", code)); \
         goto loop; \
       } 

/* the following code is inserted if there are no more (global) steps left */

/* there has to be an exit in this code, this means that processing can go on right away ;-) */

#define SPEC_CODE_2(wert,befehl)\
   if ((T_SPEC_PROCESS(curr_pid)==1) && !T_SPEC_FOLD(curr_pid)) { \
     if (T_SPEC_NO_BSTEPS(curr_pid)==1) { \
       DBUG_PRINT("INTER", ("speculative: no more barrier steps are coming... no more sync")); \
       } else { \
       if (spec_kill_no_steps == 1) { \
         DBUG_PRINT("INTER", ("spec_kill_no_steps is set !")); \
         PC_SET_SPEC_FAIL(curr_pid); } \
       else { DBUG_PRINT("INTER", ("spec_kill_no_steps not set...")); \
/* fprintf(debugger,"0x%x: ******************** really no steps ! ?!", curr_pid); fflush(debugger);*/ \
         PC_SET_SPEC_NO_STEPS(curr_pid); } \
       DBUG_PRINT("INTER", ("speculative: all steps are gone... status: %x", curr_pid->specmask1)); \
       PC_SET_SPEC_FOLD(curr_pid); \
       } \
     } 

#define IS_RAW_SPEC(pid) ((T_SPEC_LEAF(pid)==1) && !T_SPEC_WINNER(pid))

/* control messages:

   ACKNOWLEDGE: a father process announces, that he will replaced by a number
                of new leaf processes, this goes to a control process and 
                originated from a speculative node process

   GOON       : (leaf-) process gets another bunch of reduction steps available

   HEREAMI    : goes to the direct father process to register the child process in 
                his process context

   BARRIER    : this process has passed the first (redsteps) barrier and is 
                approaching the second one

   FAIL       : a child process is failed (sending to the global controller if 
                it's a leaf, otherwise only to the direct father process)

   TERMINATE  : terminate the speculative process; this means that the process
                will send a FAIL-message ( ahaeh ?! )

   WIN        : I'm the winner, I've got a result ! Double-usage: to the control
                and to a node-process

   START      : the control process sends a start command to a speculative thread

*/

#define SPEC_MSG_ACK    1
#define SPEC_MSG_GOON   2
#define SPEC_MSG_HA     4
#define SPEC_MSG_FAIL   8
#define SPEC_MSG_TERM   16
#define SPEC_MSG_BARR   32
#define SPEC_MSG_WIN    64
#define SPEC_MSG_START  128
#define SPEC_MSG_VITAL  256

#define TAG_SPECMSG_VITAL    spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_VITAL
#define TAG_SPECMSG_ACK      spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_ACK
#define TAG_SPECMSG_GOON     spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_GOON
#define TAG_SPECMSG_HEREAMI  spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_HA
#define TAG_SPECMSG_FAIL     spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_FAIL
#define TAG_SPECMSG_TERM     spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_TERM
#define TAG_SPECMSG_BARR     spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_BARR
#define TAG_SPECMSG_WIN      spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_WIN
#define TAG_SPECMSG_START    spec_control[0] = (spec_control[0] & 0xffff0000) | SPEC_MSG_START

#define T_SPECMSG_VITAL      ((spec_control[0] & SPEC_MSG_VITAL) == SPEC_MSG_VITAL)
#define T_SPECMSG_ACK        ((spec_control[0] & SPEC_MSG_ACK) == SPEC_MSG_ACK)
#define T_SPECMSG_GOON       ((spec_control[0] & SPEC_MSG_GOON) == SPEC_MSG_GOON)
#define T_SPECMSG_HEREAMI    ((spec_control[0] & SPEC_MSG_HA) == SPEC_MSG_HA)
#define T_SPECMSG_FAIL       ((spec_control[0] & SPEC_MSG_FAIL) == SPEC_MSG_FAIL)
#define T_SPECMSG_TERM       ((spec_control[0] & SPEC_MSG_TERM) == SPEC_MSG_TERM)
#define T_SPECMSG_BARR       ((spec_control[0] & SPEC_MSG_BARR) == SPEC_MSG_BARR)
#define T_SPECMSG_WIN        ((spec_control[0] & SPEC_MSG_WIN) == SPEC_MSG_WIN)
#define T_SPECMSG_START      ((spec_control[0] & SPEC_MSG_START) == SPEC_MSG_START)

/* special macros for D_SPEC_TICKETS */

#define SPEC_SF_PROCESS(desc)      ((desc).u.li.ptdv[0])
#define SPEC_SF_PSTATUS(desc)      ((desc).u.li.ptdv[1])
#define SPEC_SF_ASMADDR(desc)      ((desc).u.li.ptdv[2])
#define SPEC_SF_ASMMKNC(desc)      ((desc).u.li.ptdv[3])
#define SPEC_SF_REDSTEPS(desc)     ((desc).u.li.ptdv[4])
#define SPEC_SF_NESTING(desc)      ((desc).u.li.ptdv[5])
#define SPEC_SF_FIRSTF(desc)       ((desc).u.li.ptdv[6])
#define SPEC_SF_NUMB_T(desc)       ((desc).u.li.ptdv[7])
#define SPEC_SF_NUMB_A(desc)       ((desc).u.li.ptdv[8])
#define SPEC_SF_ADDRCOUNT(desc)    ((desc).u.li.ptdv[9])
#define SPEC_SF_GUARD(desc)        ((desc).u.li.ptdv[10])

#define SPEC_SF_DATA               11     

#define SPEC_SF_DATA_T(desc)       ((desc).u.li.ptdv[SPEC_SF_DATA])
#define SPEC_SF_DATA_A(desc)       ((desc).u.li.ptdv[SPEC_SF_DATA+(desc).u.li.ptdv[6]])

#define SPEC_PP_NEXT(desc)         ((desc).u.li.ptdv[0])
#define SPEC_PP_PREV(desc)         ((desc).u.li.ptdv[1])
#define SPEC_PP_NESTING(desc)      ((desc).u.li.ptdv[2])
#define SPEC_PP_FRAME(desc)        ((desc).u.li.ptdv[3])

#endif
