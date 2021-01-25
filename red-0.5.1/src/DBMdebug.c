#if NCDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nhost.h>
#include "dbug.h"
#include "rheapty.h"
#include "rinter.h"
#include "rncstack.h"
#include "rncmessage.h"
#include "DBMexport.h"
#include "DBdebug.h"

static int fdcube, cube_dim;
static int *dbg_ready = NULL;

/*
 * Allocate buffer to hold ready messages from NCUBE
 */

void DBMinit(cube, dim)
  int cube, dim;
  {
  int i;
  DBUG_ENTER("DBMinit");
  unlink("dbm");
  DBUG_PUSH("d,DEBUG:t:o,dbm");            /* FRED FISH DEBUG PAKET INITIALISIEREN */
  if (dbg_ready)
    free(dbg_ready);
  fdcube = cube;
  cube_dim = dim;
  dbg_ready = malloc(4<<cube_dim);
  for(i = 0; i < 1<<cube_dim; i++)
    dbg_ready[i] = 0;
  DBUG_ASSERT(dbg_ready);
  DBUG_VOID_RETURN;
  }

/*
 * determine if processor is ready to
 * receive debug-requests;
 * buffer received results
 * if block == 0: returns immediately, may return 0
 * if block != 0: wait for ready message of
 *   specified processor, always returns 1
 */

int DBMncreadyproc(proc, block)
  int	proc, block;
  {
  int type = MT_DBG_READY;

  DBUG_ENTER("DBMncreadyproc");
  DBUG_ASSERT((proc >= 0) && (proc < 1<<cube_dim));
  if (dbg_ready[proc]) {
    DBUG_PRINT("DEBUG", ("previous DBG_READY received from %d", proc));
    DBUG_RETURN(1);
    }
  if (block) {
    DBUG_PRINT("DEBUG", ("waiting for ready message"));
    nread(fdcube, NULL, 0, &proc, &type);
    dbg_ready[proc] = 1;
    DBUG_RETURN(type == MT_DBG_READY ? 1 : -1);
    }
  else if (ntest(fdcube, &proc, &type) >= 0) {
      DBUG_PRINT("DEBUG", ("receive"));
      nread(fdcube, NULL, 0, &proc, &type);
      dbg_ready[proc] = 1;
      DBUG_PRINT("DEBUG", ("DBG_READY received from %d", proc));
      DBUG_RETURN(1);
      }
  DBUG_PRINT("DEBUG", ("no DBG_READY from %d", proc));
  DBUG_RETURN(0);
  }

/* 
 * determine if all processors in subcube
 * are ready for debug
 * blocks until all processors are ready
 */
int DBMncready(proc)
  int	proc;
  {
  int	i, flag = 1;
  DBUG_ENTER("DBMncready");
  for(i = 0; i < (1<<cube_dim); i++)
    flag &= DBMncreadyproc(i, 1);
  DBUG_PRINT("DEBUG", ("return %d", flag));
  DBUG_RETURN(flag);
  }

/*
 * send continuation message to processor
 */

int DBMcont(proc)
  int proc;
  {
  DBUG_ENTER("DBMcont");
  DBUG_PRINT("DEBUG", ("sending DBG_CONT to %d", proc));
  dbg_ready[proc] = 0;
  nwrite(fdcube, NULL, 0, proc, MT_DBG_CONT);
  DBUG_RETURN(1);
  }

/*
 * send request for complete scheduler information
 * fills schedinfo structure
 */

int DBMgetschedinfo(proc, schedinfo)
  int		proc;
  SCHEDINFO   * schedinfo;
  {
  int sz, type = MT_DBG_SENDSCHEDSTAT;
  DBUG_ENTER("DBMgetschedinfo");
  nwrite(fdcube, NULL, 0, proc, MT_DBG_GETSCHEDSTAT);
  DBUG_PRINT("DEBUG", ("request for processor status sent to %d", proc));
  sz = nread(fdcube, (char *)schedinfo, sizeof(*schedinfo), &proc, &type);
  DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
  DBUG_RETURN(sz == sizeof(*schedinfo));
  }

/*
 * send request for specific process information
 * fills procinfo structure
 */

int DBMgetpidinfo(proc, pid, procinfo)
  int proc;
  int pid;
  PIDINFO * procinfo;
  {
  int sz, type = MT_DBG_SENDPIDINFO;
  DBUG_ENTER("DBMgetpidinfo");
  nwrite(fdcube, (char *)&pid, sizeof(pid), proc, MT_DBG_GETPIDINFO);
  DBUG_PRINT("DEBUG", ("request for process info sent, to %d", proc));
  sz = nread(fdcube, (char *)procinfo, sizeof(*procinfo), &proc, &type);
  DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
  DBUG_RETURN(sz == sizeof(*procinfo));
  }

/*
 * send request for stack slice
 * stack reference previously got by DBMgetpidinfo
 * message format: stackadr offset size
 * fills array with stack elements
 * returns number of received elements
 */

int DBMgetstack(proc, stack, off, size, stackbuf)
  int		proc;
  STACK	      * stack;
  int		off;
  int		size;
  STACKELEM   * stackbuf;
  {
  int sz, buf[3], type = MT_DBG_SENDSTACK;
  DBUG_ENTER("DBMgetstack");
  if (size == 0) {
    DBUG_PRINT("DEBUG", ("request for stack (from %d size %d) slice not sent to %d",
        off, size, proc));
    sz = 0;
    }
  else {
    buf[0] = (int)stack; buf[1] = off; buf[2] = size;
    nwrite(fdcube, (char *)buf, sizeof(buf), proc, MT_DBG_GETSTACK);
    DBUG_PRINT("DEBUG", ("request for stack (from %d size %d) slice sent to %d",
        off, size, proc));
    sz = nread(fdcube, (char *)stackbuf, size * sizeof(STACKELEM), &proc, &type);
    DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
    }
  DBUG_RETURN(sz/sizeof(STACKELEM));
  }

/*
 * send request for piece of code
 * code pointer(PC) previously got by DBMgetpidinfo
 * message format: codeadr size
 * fills array with code
 * returns size of received code
 * all sizes are words not ASM-instructions!
 */

int DBMgetcode(proc, code, size, codebuf)
  int proc;
  INSTR * code;
  int size;
  INSTR * codebuf;
  {
  int sz, buf[2], type = MT_DBG_SENDCODE;
  DBUG_ENTER("DBMgetcode");
  if (size == 0) {
    DBUG_PRINT("DEBUG", ("request for code (from %x size %d) not sent to %d",
        code, size, proc));
    sz = 0;
    }
  else {
    buf[0] = (int)code; buf[1] = size;
    nwrite(fdcube, (char *)buf, sizeof(buf), proc, MT_DBG_GETCODE);
    DBUG_PRINT("DEBUG", ("request for code (from %x size %d) sent to %d",
        code, size, proc));
    sz = nread(fdcube, (char *)codebuf, size * sizeof(STACKELEM), &proc, &type);
    DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
    }
  DBUG_RETURN(sz/sizeof(INSTR));
  }

/*
 * send request for one descriptor
 * message format: descadr
 * fills array with descriptor contents
 */

int DBMgetdesc(proc, desc, descbuf)
  int	proc;
  PTR_DESCRIPTOR desc;
  PTR_DESCRIPTOR descbuf;
  {
  int sz, type = MT_DBG_SENDDESC;
  DBUG_ENTER("DBMgetdesc");
  nwrite(fdcube, (char *)&desc, sizeof(PTR_DESCRIPTOR), proc, MT_DBG_GETDESC);
  DBUG_PRINT("DEBUG", ("request for desc %x sent to %d", desc, proc));
  sz = nread(fdcube, (char *)descbuf, sizeof(T_DESCRIPTOR), &proc, &type);
  DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
  DBUG_RETURN(sz == sizeof(T_DESCRIPTOR));
  }

/*
 * send request for one slice of heap
 * message format: heapadr size
 * fills array with heap elements
 * returns count of returned heap elements
 */

int DBMgetheap(proc, heap, size, heapbuf)
  int	proc;
  PTR_HEAPELEM heap;
  int	size;
  PTR_HEAPELEM heapbuf;
  {
  int sz, buf[2], type = MT_DBG_SENDHEAP;
  DBUG_ENTER("DBMgetheap");
  if (size == 0) {
    DBUG_PRINT("DEBUG", ("request for heap %x, size %d not sent to %d", heap, size, proc));
    sz = 0;
    }
  else {
    buf[0] = (int)heap; buf[1] = size;
    nwrite(fdcube, (char *)buf, sizeof(buf), proc, MT_DBG_GETHEAP);
    DBUG_PRINT("DEBUG", ("request for heap %x, size %d sent to %d", heap, size, proc));
    sz = nread(fdcube, (char *)heapbuf, size * sizeof(PTR_HEAPELEM), &proc, &type);
    DBUG_PRINT("DEBUG", ("answer received, size %d", sz));
    }
  DBUG_RETURN(sz/sizeof(PTR_HEAPELEM));
  }

/*
 * send continuation parameter to nCUBE-process
 * either steps or codeadr nust be set to 0/NULL
 * (codeadr is set to NULL if steps != 0)
 * distflag may be 0 (not set) else distflag is set
 */

int DBMsetparms(proc, pid, steps, breakpt, distflag)
  int proc, pid, steps, breakpt, distflag;
  {
  int buf[4];
  DBUG_ENTER("DBMsetparms");
  DBUG_PRINT("DEBUG", ("DBMsetparms (proc %d, pid %08x, steps %d, breakpt %08x, flag %d)",
      proc, pid, steps, breakpt, distflag));
  buf[0] = pid; buf[1] = steps; buf[2] = breakpt;
  buf[3] = (distflag ? DISToneshot : DISTstepwise);
  DBUG_PRINT("DEBUG", ("sending distflag = %d", buf[3]));
  nwrite(fdcube, (char *)buf, sizeof(buf), proc, MT_DBG_SENDPARMS);
  DBUG_RETURN(1);
  }

/*
 * determine if process on specific processor terminated
 */

int DBMncpidterminated(proc, pid)
  int proc;
  PID pid;
  {
  int isterm, type = MT_DBG_TERMINATED;

  DBUG_ENTER("DBMncterminated");
  DBUG_PRINT("DEBUG", ("sending terminate request to %d %08x", pid, proc));
  nwrite(fdcube, (char *)&pid, sizeof(PID), proc, MT_DBG_ISTERMINATED);
  nread(fdcube, (char *)&isterm, sizeof(isterm), &proc, &type);
  DBUG_PRINT("DEBUG", ("%x at %d has terminate status %d", pid, proc, isterm));
  DBUG_RETURN(isterm);
  }

/* test if host received a message that signals completion
 * of the whole calculation (end result sent or abnormal
 * programm termination via post mortem)
 */

int DBMncterminated()
  {
  int proc = -1, type;

  DBUG_ENTER("DBMncterminated");
  type = MT_POST_MORTEM;
  if (ntest(fdcube, &proc, &type) >= 0) {
    DBUG_PRINT("DEBUG", ("POST_MORTEM send by %d", proc));
    DBUG_RETURN(1);
    }
  type = MT_END_RESULT;
  if (ntest(fdcube, &proc, &type) >= 0) {
    DBUG_PRINT("DEBUG", ("END_RESULT send by %d", proc));
    DBUG_RETURN(1);
    }
  DBUG_RETURN(0);
  }

/*
 * change DBUG parameter on nCUBE
 */

int DBMsetdbug(proc, str)
  int	 proc;
  char * str;
  {
  int i;
  static char buf[MAXDBUGLEN];

  DBUG_ENTER("DBMsetdbug");
  for(i = (proc < 0 ? 0 : proc); i < (proc < 0 ? (1<<cube_dim) : proc); i++) {
    sprintf(buf, "dbnc%02d", i);
    unlink(buf);
    sprintf(buf, "%s:o,dbnc%02d", str, i);
    DBUG_PRINT("DEBUG", ("sending %d DBUG parameter '%s' to %d", i, buf, proc));
    nwrite(fdcube, buf, strlen(buf)+1, i, MT_DBG_SETDBUG);
    }
  DBUG_RETURN(1);
  }

/* send detach message to all processors
 * which cause every processor to switch to
 * one shot execution mode and complete the hole
 * reduction
 */

int DBMdetach()
  {
  int i;
  DBUG_ENTER("DBMdetach");
  for(i = 0; i < (1<<cube_dim); i++) {
    nwrite(fdcube, NULL, 0, i, MT_DBG_DETACH);
    DBUG_PRINT("DEBUG", ("sent detach to %d", i));
    dbg_ready[i] = 0;
    }
  DBMncready();
  DBUG_RETURN(1);
  }

#endif /* NCDEBUG */
