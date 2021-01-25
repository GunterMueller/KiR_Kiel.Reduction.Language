#ifndef _DBMexport_h
#define _DBMexport_h

#include "rheapty.h"
#include "rinter.h"
#include "rncsched.h"

#define MAXPROC 32
#define MAXPID 6
#define MAXSTACK 4

typedef void * PID;
typedef void * STACK;

typedef struct schedinfo{ PID         active;
			 int         free;
			 int         nxt_proc;
			 PID	     ready[MAXPID];
			 PID	     wait[MAXPID];
			} SCHEDINFO;

typedef struct pidinfo{ PROCESS_STATUS status;
			INSTR *  pc;
			int      red_count;
			int      start_red_count;
			STACK *	 stackadr[MAXSTACK];
			int	 stacksize[MAXSTACK];
			int      steps;
			INSTR *  codeadr;
			int      distflag;
		       } PIDINFO;

extern void DBMinit ();
extern int DBMncready ();
extern int DBMcont ();
extern int DBMsetparms ();
extern int DBMgetstack ();
extern int DBMgetdesc ();
extern int DBMgetheap ();
extern int DBMgetcode ();
extern int DBMgetschedinfo ();
extern int DBMgetpidinfo ();
extern int DBMncterminated ();
extern int DBMncpidterminated ();
extern int DBMsetdbug ();

#endif /* _DBMexport_h */
