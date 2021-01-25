/**************************************************************************/
/*                                                                        */
/* rncsched.h --- header file for the nCube-scheduler                     */
/*                                                                        */
/* ach 13/01/92                                                           */
/*                                                                        */
/**************************************************************************/

#ifndef _rncsched_h_included /* finally done by RS */
#define _rncsched_h_included

#include "DBdebug.h"

typedef enum {PS_READY,PS_WAITING,PS_ACTIVE} PROCESS_STATUS;

#if D_SPEC

typedef struct spec_control_struc {int      leaves;
                            int     result;
                            int      have_ack;
                            int     *data;
                              } SPEC_CONTROL_STRUC;
#endif

typedef struct process_context{PROCESS_STATUS         status;
			       int                    red_count;
			       int                    start_red_count;
                               int                    switched;
                               INSTR                  *pc;
			       struct process_context *ppid;
                               DStackDesc             D_S_A;
                               DStackDesc             D_S_H;
                               DStackDesc             D_S_I;
			       DStackDesc             D_S_R;
#if NCDEBUG
                               DBGcontext             DBGcontext;
#endif /* NCDEBUG */
#if ADV_SCHED
                               int                    start_letpar_level;
                               int                    letpar_level;
#endif /* ADV_SCHED */

#if D_SPEC
                               int                    specmask1, specmask2;
                               int                    spec_red_count;
                               int                    father_slot;
                               int                    control_pid, control_node,
                                                      father_pid,  father_node;
                               int                   *processes;
                               int                   *son_results, *son_redcnts, *son_status;
                               SPEC_CONTROL_STRUC    *control;
#if D_SPEC_TICKETS
                               int                    nesting, ffound;
                               int                   *spec_postpone_start;
                               int                   *spec_postpone_end;
#endif /* D_SPEC_TICKETS */

#endif
                               struct process_context *in_use_next;
			       struct process_context *next;} PROCESS_CONTEXT;

typedef enum {DIST_N,DIST_B} DIST_TYPE;

#define PC_STATUS(pid)           (pid)->status
#define PC_GET_A_STACK(pid)      (((pid)->switched)?(pid)->D_S_I:(pid)->D_S_A)
#define PC_GET_I_STACK(pid)      (((pid)->switched)?(pid)->D_S_A:(pid)->D_S_I)
#define PC_GET_REAL_I_STACK(pid) ((pid)->D_S_I)
#define PC_GET_H_STACK(pid)      ((pid)->D_S_H)
#define PC_GET_R_STACK(pid)      ((pid)->D_S_R)
#define PC_GET_PC(pid)           (pid)->pc
#define PC_GET_PPID(pid)         (pid)->ppid
#define PC_GET_REDCNT(pid)       (pid)->red_count
#define PC_GET_SREDCNT(pid)      (pid)->start_red_count

#define PC_SET_REDCNT(pid,redcnt) (pid)->red_count = (redcnt)
#define PC_SET_PC(pid,npc)        (pid)->pc = (npc)
#define PC_SET_STACK(pid,stack,value) ((pid)->stack) = (value)
#define PC_SET_SWITCHED(pid,sw)   (pid)->switched = (sw)
#define W_A_SWITCHED 0x1
#define R_T_SWITCHED 0x2

#endif
