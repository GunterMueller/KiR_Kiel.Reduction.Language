/* $Log: d_mess_io.h,v $
 * Revision 1.6  1995/10/20  13:22:02  rs
 * some ncube+pvm changes
 *
 * Revision 1.5  1995/10/02  16:46:25  rs
 * some more pvm + measure modifications
 *
 * Revision 1.4  1995/09/22  12:46:20  rs
 * additional pvm + measure changes
 *
 * Revision 1.3  1995/09/11  14:21:44  rs
 * some changes for the pvm measure version
 *
 * Revision 1.2  1995/05/22  08:59:10  rs
 * changed nCUBE to D_SLAVE
 *
 * */

/***************************************************************************/
/*                                                                         */
/*   Datei  : d_mess_io.h                                                  */
/*                                                                         */
/*   Inhalt : Makros fuer Mess-IO                                          */ 
/*                                                                         */
/*   Letzte Aenderung :                                                    */
/*                                                                         */
/*         04.08.1993                                                      */
/*                                                                         */
/***************************************************************************/

#ifndef _d_mess_io
#define _d_mess_io

#include "dbug.h"
#if (D_SLAVE && (nCUBE || D_PVM_NCUBE))
#include <ntime.h>
#endif
#if (D_SLAVE && D_PVM)
#include <sys/time.h>
#endif

#define  D_MESS_FILE       d_mess_dat
#define  D_MESS_INIT       "INIT"
#define  D_BITMASK         d_bitmask

#define D_MESS_MAXBUF 60000   
extern char *d_mess_buff;
#if D_PVM
extern gettimeofday();
extern struct timeval pvm_time;
extern struct timeval pvm_time_host, pvm_time_delta;
extern long pvm_dsec, pvm_dusec;
#endif /* D_PVM */

#if D_SLAVE
#include "rncstack.h"
#include "rncsched.h"
#include <time.h>

/* some defines for the measure-file */

#if (nCUBE || D_PVM_NCUBE)
#define M_TIMER                    amicclk()
#endif
#if (D_PVM && !D_PVM_NCUBE)
#define M_TIMER                    (gettimeofday (&pvm_time, NULL), (double)((pvm_time.tv_sec+pvm_dsec)*1000000.0 + (pvm_time.tv_usec+pvm_dusec))) 
/* #define M_TIMER                    (gettimeofday (&pvm_time, NULL), (double)((pvm_time.tv_sec)*1000000.0 + (pvm_time.tv_usec))) */
#endif
#define M_TIMER_TYPE               double
#define M_TIMER_TYPE_BUF           _d_ddummy

#define PROCESS_NR                 proc_id
#define PROCESS_NR_TYPE            char
#define PROCESS_NR_BUF             _d_cdummy

#define PROCESS_PID                m_curr_pid
#define PROCESS_PID_TYPE           int
#define PROCESS_PID_BUF            _d_dummy

#define M_INT_BUF                  _d_dummy
#define M_CHAR_BUF                 _d_cdummy
#define M_PCHAR_BUF                _d_sdummy

#define M_BINARY_COUNT_TYPE        char
#define M_BINARY_COUNT             _d_cdummy

extern int _d_dummy;
extern double _d_ddummy;
extern char _d_cdummy;
extern char *_d_sdummy;
extern int _m_tmp;

#define M_SWAP(a) (_m_tmp = (a),(((_m_tmp & 0xff) << 24) | ((_m_tmp & 0xff00) << 8) | ((_m_tmp & 0xff0000) >> 8) | ((_m_tmp & 0xff000000) >> 24)))

#else
#include <sys/param.h>
#define M_TIMER clock()/HZ
#endif

extern int m_base;
extern char d_bitmask[];
extern int d_bib_mask;

#if (!D_SLAVE)
extern char mess_file[]; /* real name measure file on host */
extern char m_remote_host[], m_target_dir[];
extern char m_remote_bin[];   /* see setup.c */
extern char m_mesfilehea[]; /* see setup.c */ 
extern char m_mesfilepath[]; /* see setup.c */
extern char m_remote_dir[];
extern int m_ackno;   /* acknowledge measuring data */
extern int m_merge_strat;
extern int m_del_files;
extern void m_remote_copy();
extern int m_heapdes, m_heapsize;
#else

/* extern merging variables */

extern int m_sendto;        /* nr. of node to send the measure result to (or host-file) */
extern int m_max_files;     /* nr. of measure files to read from */
extern int m_max_message;   /* nr. of pre-sorted measure files to read from */

extern char m_mesfilehea[];
extern char m_mesfilepath[];
extern int m_ackno;
extern int m_merge_strat;

extern void  m_start_merge();

/* extern used measure variables for MPRINT-Makros */

extern proc_id;
extern m_curr_pid;

/* extern function pointers for heap measure */

extern int (*d_m_newheap)();
extern void (*d_m_freeheap)();
extern T_DESCRIPTOR *(*d_m_newdesc)();
extern void (*d_m_freedesc)();
extern int (*d_m_compheap)();

/* extern function pointers (and variables) for stack measure */

extern void (*d_m_allocate_d_stack_segment)();
extern void (*d_m_free_d_stack_segment)();
extern int d_m_stack_pop;
extern int d_m_stack_push;

/* extern function pointers for scheduler measure */

extern INSTR *(*d_m_distribute)();
extern PROCESS_CONTEXT *(*d_m_process_create)();
extern void (*d_m_process_terminate)();
extern void (*d_m_process_run)();
extern void (*d_m_process_sleep)();
extern void (*d_m_process_wakeup)();

/* extern function pointers for communication measure */

extern void (*d_m_msg_send)();
extern void (*d_m_msg_receive)();

/* extern function declarations for heap measure functions */

extern int m_newheap();
extern void m_freeheap();
extern T_DESCRIPTOR *m_newdesc();
extern void m_freedesc();
extern int m_compheap();

/* extern function declarations for stack measure functions */

extern void m_allocate_d_stack_segment();
extern void m_free_d_stack_segment();
extern int m_stack_pop;
extern int m_stack_push;

/* extern function declarations for scheduler measure functions */

extern INSTR *m_distribute();
extern PROCESS_CONTEXT *m_process_create();
extern void m_process_terminate();
extern void m_process_run();
extern void m_process_sleep();
extern void m_process_wakeup();

/* extern function declaration for communication measure functions */

extern void m_msg_send();
extern void m_msg_receive();
extern int m_msg_send_begin, m_msg_send_end, m_msg_receive_begin, m_msg_receive_end;

/* some global measure stuff */

/* extern void get_em_merged(); */
extern char d_mess_name[];
#endif

#define NCUBE_MAXPROCS  32

extern void d_read_init();
extern void m_merging();
extern void m_merge_initialize();
extern FILE *d_mess_dat;

#include "d_mess_groups.h"

#endif
