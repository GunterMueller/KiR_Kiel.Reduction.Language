/* DBdebug.h
 *
 * declarations for the interactive debug package
 * on the NCUBE
 *
 */

#ifndef DBDEBUG_H
#define DBDEBUG_H

#include "rheapty.h"
#include "rinter.h"

/* declare message type for NCUBE-host communication
 * request(instruction) messages 200 ..  send from host to nCUBE
 * and answer messages (250 ..) send from nCUBE
 */
typedef enum { MT_DBG_CONT = 200, MT_DBG_GETSTACK, MT_DBG_GETDESC, MT_DBG_ISTERMINATED,
  MT_DBG_GETHEAP, MT_DBG_GETCODE, MT_DBG_GETPC, MT_DBG_GETSCHEDSTAT, MT_DBG_GETPIDINFO,
  MT_DBG_SETDBUG, MT_DBG_SENDPARMS, MT_DBG_DETACH,
  MT_DBG_READY = 250, MT_DBG_SENDSTACK, MT_DBG_SENDDESC, MT_DBG_TERMINATED,
  MT_DBG_SENDHEAP, MT_DBG_SENDCODE, MT_DBG_SENDPC, MT_DBG_SENDSCHEDSTAT, MT_DBG_SENDPIDINFO
  } DBG_MESSAGE_TYPE;

/* maximum size for messagebuffer on ncube side
 * for receiving messages
 */
#define MAXDBUGLEN		1024
#define MT_MAXLEN		4

extern int proc_id;
extern int host;
extern int cube_dim;

/* additional entries for process context
 * linked into process_context via pointer
 */
typedef struct DBGcontext {
  int		steps;
  int		totalsteps;
  INSTR	      * breakpt;
  int		distflag;
  int		distlevel;
  } DBGcontext;

/* possible values for distflag
 */
#define DISTstepwise	1 /* stepwise execution, interrupt reduction */
#define DISToneshot	2 /* execute stepwise, but DIST-command cause
			   * one shot execution until matching DIST_END
			   * (see variable dbnc_distlevel */

#endif
