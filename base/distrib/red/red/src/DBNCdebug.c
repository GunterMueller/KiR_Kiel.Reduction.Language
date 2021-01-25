/*
 * DBNCdebug.c
 *
 * Message handler for debug properties
 * also functions that pick up informations about the
 * system (heap, stack, scheduler)
 *
 * car 30/03/93
 *
 */

#if NCDEBUG

#include <signal.h>
#include "rheapty.h"
#include "rstdinc.h"
#include "rstelem.h"
#include "rncstack.h"
#include "rncsched.h"
#include "rncmessage.h"
#include "dbug.h"
#include "DBdebug.h"
#include "DBMexport.h"

#define SWAP(a) (_tmp2 = (a),((_tmp2 << 24) | ((_tmp2 & 0xff00) << 8) | ((_tmp2 & 0xff0000) >> 8) | (_tmp2 >> 24)))

extern int		 proc_tab_size;
extern PROCESS_CONTEXT * process_table;
extern PROCESS_CONTEXT * ready_queue_first;    /* first element of the ready_queue      */
extern PROCESS_CONTEXT * released_queue_first; /* first element of the released_queue   */
extern PROCESS_CONTEXT * curr_pid;

static unsigned int _tmp2;
static volatile int dbnc_cont;
static int dbnc_distlevel = 0;
static void (*sa_handler)();
static char * proc_term;

static void DBNChandle_signal();

/* install signal handler for debug proposes
 * store default handler in sa_handler
 * to use it, for non debug messages
 */
void DBNCinit()
  {
  DBUG_ENTER("DBNCinit");
  if (proc_term)
    /* on second call, reallocate space */
    free(proc_term);
  else {
    /* do this only for the first time */
    sa_handler = signal(SIGMSGS, SIG_DFL);
    signal(SIGMSGS, DBNChandle_signal);
    }
  if (proc_tab_size) {
    proc_term = (char *)malloc(proc_tab_size);
    DBUG_ASSERT(proc_term);
    memset(proc_term, 0, proc_tab_size*sizeof(char));
    }
  DBUG_VOID_RETURN;
  }

/* reinitialize global debg parameters
 */
void DBNCtestcomplete()
  {
  DBUG_ENTER("DBNCtestcomplete");
  if (ready_queue_first == NULL)
    dbnc_distlevel = 0;
  DBUG_VOID_RETURN;
  }

/* signal the host that processor is
 * ready to be debugged, after that
 * block execution until permission is
 * given via setting the global variable
 * dbnc_cont (set in message handler)
 */
void DBNCclient()
  {
  int flag = 0;
  DBUG_ENTER("DBNCclient");

/*
 * test interrupt conditions
 */
  if (curr_pid)
    process_context_update();
  DBUG_PRINT("DEBUG", ("condition(0x%x) flag=%d steps=%d brkpt=%x pc=%x glob = %d",
      curr_pid, curr_pid->DBGcontext.distflag, curr_pid->DBGcontext.steps,
      curr_pid->DBGcontext.breakpt, curr_pid->pc, dbnc_distlevel));
  if (dbnc_distlevel)
    DBUG_VOID_RETURN;
  if (curr_pid != NULL)
    if (curr_pid->DBGcontext.distlevel || ( (--curr_pid->DBGcontext.steps > 0) &&
        (curr_pid->DBGcontext.breakpt != curr_pid->pc)))
      DBUG_VOID_RETURN;

  DBUG_PRINT("DEBUG", ("ready to enter loop"));

  DBUG_PRINT("DEBUG", ("sending DBG_READY"));
  dbnc_cont = 0;
  nwrite(NULL, 0, host, MT_DBG_READY, &flag);
  while(dbnc_cont == 0) {
    pause();
    /*
    sleep(1);
    DBUG_PRINT("DEBUG", ("alive"));
    */
    DBNChandle_signal();
    }
  DBUG_PRINT("DEBUG", ("DBG_READY cont"));
  DBUG_VOID_RETURN;
  }

/* reset process terminated status
 * initialize debug context to default values
 */
void DBNCcreate(pid)
  PROCESS_CONTEXT * pid;
  {
  DBUG_ENTER("DBNCcreate");
  proc_term[((int)pid-(int)process_table)/sizeof(PROCESS_CONTEXT)] = 0;
  pid->DBGcontext.steps      = 0;
  pid->DBGcontext.totalsteps = 1;
  pid->DBGcontext.breakpt    = NULL;
  pid->DBGcontext.distflag   = DISTstepwise;
  pid->DBGcontext.distlevel  = 0;
  DBUG_VOID_RETURN;
  }

/* set process terminated status
 */
void DBNCterminated(pid)
  PROCESS_CONTEXT * pid;
  {
  DBUG_ENTER("DBNCterminated");
  proc_term[((int)pid-(int)process_table)/sizeof(PROCESS_CONTEXT)] = 1;
  DBUG_PRINT("DEBUG", ("process %x == %d terminated", pid,
      ((int)pid-(int)process_table)/sizeof(PROCESS_CONTEXT)));
  DBUG_VOID_RETURN;
  }

/* simulate a stack for nested callings of ASM-DIST
 * implemented via a counter that can be modified externally,
 * parameter = 0: initialize distlevel := 0
 *           > 0: increment distlevel by parameter
 *           < 0: decrement distlevel by parameter
 * 
 */
int DBNCsetdistlevel(i)
  int i;
  {
  int j;
  DBUG_ENTER("DBNCsetdistlevel");
  DBUG_PRINT("DEBUG", ("old distlevel = %d", curr_pid->DBGcontext.distlevel));
  if (i == 0 || (curr_pid->DBGcontext.distlevel == 0))
    curr_pid->DBGcontext.distlevel = 0;
  else
    curr_pid->DBGcontext.distlevel += i;
  DBUG_RETURN(curr_pid->DBGcontext.distlevel);
  }

/* get global dist_level variable
 */
int DBNCgetdistlevel()
  {
  DBUG_ENTER("DBNCgetdistlevel");
  DBUG_RETURN((curr_pid->DBGcontext.distflag == DISToneshot) || dbnc_distlevel);
  }

/* message handler for debug mode
 * always active when processor wait for
 * debug instructions
 *
 * all debug messages are filtered out
 * of the message stream, normal messages
 * are left untouched and are processed by
 * the regular message handler
 */

static void DBNChandle_signal(sig)
  int sig;
  {
  int source=-1, type = -1, flag = 0;
  int i, offset, size, redo;
  int * p;
  INSTR * pc;
  PTR_DESCRIPTOR desc;
  PTR_HEAPELEM heap;
  DStackDesc * stack;
  PROCESS_CONTEXT * pid;
  static int buf[MT_MAXLEN];
  static char str[MAXDBUGLEN];
  static PIDINFO pidinfo;
  static SCHEDINFO procstat;

  /* eliminate handshake messages
   * after printing to the terminal
   */
  if (ntest(&source, &type), type & 0x8000) {
    return;
    }
  signal(SIGMSGS, SIG_DFL);

  /* loop while messages are pending
   */
  do {
    redo = 0;

    /* set parameters for further reductions
     * messagebody: steps breakpt distflag
     */
    source = -1; type = MT_DBG_SENDPARMS;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_PARMS"));
      nread(buf, 4*sizeof(int), &source, &type, &flag); 
      pid				= (PROCESS_CONTEXT *)SWAP(buf[0]);
      pid->DBGcontext.totalsteps	= SWAP(buf[1]);
      pid->DBGcontext.breakpt	= (INSTR *)SWAP(buf[2]);
      pid->DBGcontext.distflag	= SWAP(buf[3]);
      DBUG_PRINT("DEBUG", ("parameter set to: steps %d, break at 0x%x, distflag %d",
	  curr_pid->DBGcontext.totalsteps, curr_pid->DBGcontext.breakpt,
	  curr_pid->DBGcontext.distflag));
      if (pid->DBGcontext.distflag == DISToneshot)
        DBNCsetdistlevel(0);
      }

    /* detach all processes on processor
     * set parameters to one-shot execution
     * and continue reduction
     * this may only be done if there is an
     * active process, or any other process or
     * a pending message that will create a
     * process
     */
    source = -1; type = MT_DBG_DETACH;
    while(ntest(&source, &type) >= 0) {
      DBUG_PRINT("DEBUG", ("received DBG_DETACH"));
      nread(NULL, 0, &source, &type, &flag); 
      source = -1; type = MT_NEW_GRAPH;
      for(i = proc_tab_size, pid = released_queue_first; pid; pid = pid->next, i--);
      DBUG_PRINT("DEBUG", ("i %d curr 0x%x ready 0x%x mess %d", i, curr_pid,
          ready_queue_first, ((proc_id  == 0) && (ntest(&source, &type) >= 0))));
      if (i || ((proc_id  == 0) && (ntest(&source, &type) >= 0))) {
        dbnc_distlevel = 1;
	DBUG_PRINT("DEBUG", ("detach successful"));
	}
      dbnc_cont = 1;
      source = -1; type = MT_DBG_DETACH;
      }

    /* continue reduction no body
     * use MT_DBG_SETPARMS to set parameters
     * messagebody: empty
     */
    source = -1; type = MT_DBG_CONT;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_CONT"));
      nread(NULL, 0, &source, &type, &flag); 
      curr_pid->DBGcontext.steps = curr_pid->DBGcontext.totalsteps;
      dbnc_cont = 1;
      }

    /* call for processor status
     * send active process, ready queue, (compacted) wait table
     * and free process slots, next distribution processor to host
     * messagebody: empty
     */
    source = -1; type = MT_DBG_GETSCHEDSTAT;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_GETSCHEDSTAT"));
      nread(NULL, 0, &source, &type, &flag); 
      procstat.active=(PID)SWAP((int)curr_pid);
      procstat.free=0;
      for(pid = released_queue_first; pid; pid = pid->next, procstat.free++);
      procstat.free=SWAP(procstat.free);
      procstat.nxt_proc = SWAP(get_distribute());
      for(i = 0; i < proc_tab_size; i++)
        procstat.ready[i] = procstat.wait[i] = NULL;
      for(i = 0, pid = ready_queue_first; pid; pid = pid->next, i++)
        procstat.ready[i] = (PID)SWAP((int)pid);
      for(i = size = 0, pid = process_table; size < proc_tab_size; pid++, size++)
        if (pid->status == PS_WAITING)
          procstat.wait[i++] = (PID)SWAP((int)pid);
      nwrite((char *)&procstat, sizeof(procstat), host, MT_DBG_SENDSCHEDSTAT);
      }

    /* call for process context
     * send status, pc, redcount(s), stacks to host
     * messagebody: procadr
     */
    source = -1; type = MT_DBG_GETPIDINFO;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_GETPIDINFO"));
      nread((char *)&pid, sizeof(pid), &source, &type, &flag); 
      pid=(PROCESS_CONTEXT *)SWAP((int)pid);
      if (pid == curr_pid)
        pidinfo.status = SWAP(PS_ACTIVE);
      else
        pidinfo.status = SWAP(pid->status);
      pidinfo.pc = (INSTR *)SWAP((int)pid->pc);
      pidinfo.red_count = SWAP(pid->red_count);
      pidinfo.start_red_count = SWAP(pid->start_red_count);
      pidinfo.stackadr[0] = (STACK *)SWAP((int)&pid->D_S_A);
      pidinfo.stackadr[1] = (STACK *)SWAP((int)&pid->D_S_H);
      pidinfo.stackadr[2] = (STACK *)SWAP((int)&pid->D_S_I);
      pidinfo.stackadr[3] = (STACK *)SWAP((int)&pid->D_S_R);
      pidinfo.stacksize[0] = SWAP(D_SIZEOFSTACK(pid->D_S_A));
      pidinfo.stacksize[1] = SWAP(D_SIZEOFSTACK(pid->D_S_H));
      pidinfo.stacksize[2] = SWAP(D_SIZEOFSTACK(pid->D_S_I));
      pidinfo.stacksize[3] = SWAP(D_SIZEOFSTACK(pid->D_S_R));
      DBUG_PRINT("DEBUG", ("stacksizes A:%d H:%d I:%d R:%d",
        D_SIZEOFSTACK(pid->D_S_A), D_SIZEOFSTACK(pid->D_S_H),
        D_SIZEOFSTACK(pid->D_S_I), D_SIZEOFSTACK(pid->D_S_R)));
      pidinfo.steps = SWAP(1);
      pidinfo.codeadr = (INSTR *)SWAP(0);
      pidinfo.distflag = SWAP(FALSE);
      nwrite((char *)&pidinfo, sizeof(pidinfo), host,MT_DBG_SENDPIDINFO);
      }

    /* call for reduma stack slice
     * messagebody: stackadr, offset, size
     * send an array of stack elements
     */
    source = -1; type = MT_DBG_GETSTACK;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("request DBG_GETSTACK"));
      nread((char *)buf, 12, &source, &type, &flag); 
      for(i=0; i<3; i++)
        buf[i] = SWAP(buf[i]);
      DBUG_PRINT("DEBUG", ("request stackoff %d size %d", buf[1], buf[2]));
      stack = (DStackDesc *)buf[0];
      size = d_stacksize(stack);
      if (buf[1] > size)
        { offset = 0; size = 0; }
      else if (buf[1]+buf[2] > size)
          { offset = 0; size = size - buf[1]; }
        else
          { offset = size - buf[1] - buf[2]; size = buf[2]; }
      DBUG_PRINT("DEBUG", ("sending from %d size %d", offset, size));
      p = (int *)malloc(size*4);
      DBUG_ASSERT(p);
      for(i = 0; i < size; offset++, i++) {
        p[size-i-1] = SWAP(D_MIDSTACK(*stack, offset));
        DBUG_PRINT("DEBUG", ("%d. stackelem %x=%d", i, D_MIDSTACK(*stack, offset),
	    D_MIDSTACK(*stack, offset)));
        }
      DBUG_PRINT("DEBUG", ("sending size %d", i));
      nwrite((char *)p, i*sizeof(int), host, MT_DBG_SENDSTACK, &flag);
      free(p);
      }

    /* call for piece of code
     * messagebody: codeadr size
     * send an array of code of requested size
     */
    source = -1; type = MT_DBG_GETCODE;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_GETCODE"));
      nread((char *)buf, 8, &source, &type, &flag); 
      pc = (INSTR *)SWAP(buf[0]);
      size = SWAP(buf[1]);
      DBUG_PRINT("DEBUG", ("sending from %x size %d", pc, size));
      p = (int *)malloc(size*sizeof(INSTR));
      DBUG_ASSERT(p);
      for(i = 0; i<size; i++)
        p[i] = SWAP(pc[i]);
      nwrite((char *)p, size*sizeof(int), host, MT_DBG_SENDCODE, &flag);
      free(p);
      }

    /* call for descriptor entries
     * messagebody: descadr
     */
    source = -1; type = MT_DBG_GETDESC;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_GETDESC"));
      nread((char *)&desc, 4, &source, &type, &flag); 
      desc = (PTR_DESCRIPTOR)SWAP((int)desc);
      DBUG_PRINT("DEBUG", ("sending descriptor %x", desc));
      p = (int *)malloc(sizeof(T_DESCRIPTOR));
      DBUG_ASSERT(p);
      for(i = 0; i < sizeof(T_DESCRIPTOR)/sizeof(int); i++)
        p[i] = SWAP(((int *)desc)[i]);
      nwrite((char *)p, sizeof(T_DESCRIPTOR), host, MT_DBG_SENDDESC, &flag);
      free(p);
      }

    /* call for heap segment
     * messagebody: heapadr size
     */
    source = -1; type = MT_DBG_GETHEAP;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_GETHEAP"));
      nread((char *)buf, 8, &source, &type, &flag); 
      heap = (PTR_HEAPELEM)SWAP(buf[0]);
      size = SWAP(buf[1]);
      DBUG_PRINT("DEBUG", ("sending from %08x size %d", heap, size));
      p = (int *)malloc(size*sizeof(T_HEAPELEM));
      DBUG_ASSERT(p);
      for(i = 0; i<size; i++)
        p[i] = SWAP((int)heap[i]);
      nwrite((char *)p, size*sizeof(int), host, MT_DBG_SENDHEAP, &flag);
      free(p);
      }

    /* call for informoation about process termination status
     * messagebody: pid
     */
    source = -1; type = MT_DBG_ISTERMINATED;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_ISTERMINATED"));
      nread((char *)&pid, sizeof(PROCESS_CONTEXT *), &source, &type, &flag); 
      pid = (PROCESS_CONTEXT *)SWAP((int)pid);
      i = proc_term[((int)pid-(int)process_table)/sizeof(PROCESS_CONTEXT)];
      DBUG_PRINT("DEBUG", ("sending %d", i));
      i = SWAP(i);
      nwrite((char *)&i, sizeof(int), host, MT_DBG_TERMINATED, &flag);
      }

    /* set new dbug parameters
     * messagebody: dbugparms
     */
    source = -1; type = MT_DBG_SETDBUG;
    while(ntest(&source, &type) >= 0) {
      redo = 1;
      DBUG_PRINT("DEBUG", ("received DBG_SETDBUG"));
      nread((char *)str, MAXDBUGLEN, &source, &type, &flag); 
      DBUG_PRINT("DEBUG", ("setting DBUG to '%s'", str));
      DBUG_PUSH(str);
      DBUG_PRINT("DEBUG", ("set DBUG to '%s'", str));
      }

  } while(redo);

  source = -1; type = -1;
  if (ntest(&source, &type) >= 0) {
    /*
    DBUG_PRINT("DEBUG", ("message type %d received, calling default handler", type));
    */
    sa_handler();
    }

  signal(SIGMSGS, DBNChandle_signal);
  }

#endif /* NCDEBUG */
