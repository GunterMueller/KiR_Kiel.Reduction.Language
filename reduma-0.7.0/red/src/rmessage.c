/* $Log: rmessage.c,v $
 * Revision 1.56  1996/07/02  19:03:16  base
 * STORE=0 equivalent to NOSTORE
 *
 * Revision 1.55  1996/05/20  17:43:35  cr
 * some changes (quite a few;-) so that send/receive can be used
 * in tasm-generated executables, too (no code/graph only)
 * tasm uses different runtime-stacks, switched by macro here
 * expression to send is passed in _desc, not on stack
 * FLAG: TASM_STORE for tasm_store_send_graph/tasm_store_receive_result
 *
 * Revision 1.54  1996/04/25  15:20:48  rs
 * -DD_BENCH=1 => ncbench is called as ncube-bin
 *
 * Revision 1.53  1996/04/22  14:18:20  rs
 * D_BENCH for benchmark.ms-filename (greetings to cr btw)
 *
 * Revision 1.52  1996/04/19  16:31:51  cr
 * improved separation between compilation for rmessage.o and rstore.o
 *
 * Revision 1.50  1996/03/25  14:30:43  cr
 * dealing with multiple code-vectors, maintain list of sent vectors
 *
 * Revision 1.49  1996/03/22  10:33:20  rs
 * some problems concerning PM-instructions fixed
 *
 * Revision 1.48  1996/03/19  15:14:27  cr
 * do not send/receive _redcnt if STORE
 *
 * Revision 1.47  1996/03/19  10:57:38  rs
 * support for C_PATTERN descriptors added
 *
 * Revision 1.46  1996/03/07  18:45:04  cr
 * support C_INTACT
 *
 * Revision 1.45  1996/03/07  18:16:17  rs
 * C_FUNC TY_CASE ptc's might be used to indicate the number of the when-clause
 * (bugfix for the distributed version)
 *
 * Revision 1.44  1996/03/07  15:38:22  rs
 * another bugfix concerning the SUB-pattern-descriptor...
 *
 * Revision 1.43  1996/03/06  17:34:41  cr
 * always send code-vector completely (from I_TABSTART to I_SYMTAB)
 * create new T_CODEDESCR when receiving code
 * special treatment for some pattern matching instructions in
 * receive_code adopted from rncmessage.c
 *
 * Revision 1.42  1996/03/06  16:45:16  rs
 * sending/receiving CLAUSE and SELECTION descriptors
 * seems to work now
 *
 * Revision 1.41  1996/03/01  16:21:47  rs
 * still some work left ;-)
 *
 * Revision 1.40  1996/02/29  16:56:01  rs
 * still some work on sending MATCHING-descriptors left
 *
 * Revision 1.39  1996/02/28  15:49:44  rs
 * problems with TGUARD fixed...
 *
 * Revision 1.38  1996/02/28  15:17:02  rs
 * fixed label problems of the pattern matching instructions
 *
 * Revision 1.37  1996/02/23  15:28:33  cr
 * cannot override -D flags in red.cnf.
 * use flag NOSTORE to do it in rmessage.c
 *
 * Revision 1.36  1996/02/23  14:18:48  rs
 * some minor fixes and more DBUG-output
 *
 * Revision 1.35  1996/02/21  20:15:22  cr
 * don't use _tmp,_tmp2
 *
 * Revision 1.34  1996/02/21  19:57:43  cr
 * avoid multiple definitions of global variables
 *
 * Revision 1.33  1996/02/21  18:23:46  cr
 * added support for FRAME and SLOT
 * fixed a bug in receive_result (TY_SUB)
 * ------
 * prototype implementation of interactions for arbitrary expressions
 * uses modified send/receive-routines from rmessage.c
 * compile with -DSTORE=1 to get the prototype (interactions get/put)
 * or with -DSTORE=0 to get the original files.
 *  involved files so far: rheap.c riafunc.c rstelem.h rmessage.c
 * rmessage.c has to be compiled once with -DSTORE=1 to get the
 * modified send/receive-routines, and perhaps a second time with
 * -DSTORE=0 to get its original functionality for the distributed
 * versions.
 *
 * Revision 1.32  1996/02/21  17:18:50  rs
 * more DBUG-output
 *
 * Revision 1.31  1996/01/25  16:17:42  rs
 * some changes for ADV_SCHED (-uling)
 *
 * Revision 1.30  1995/12/07  16:47:50  rs
 * some pvm + ncube + measurement changes
 *
 * Revision 1.29  1995/12/06  10:26:02  rs
 * some (final :-) changes for the nCUBE pvm version...
 *
 * Revision 1.28  1995/10/20  13:22:31  rs
 * some ncube+pvm changes
 *
 * Revision 1.27  1995/10/17  15:33:27  rs
 * minor changes (pvm)
 *
 * Revision 1.26  1995/09/22  12:46:29  rs
 * additional pvm + measure changes
 *
 * Revision 1.25  1995/09/19  14:43:17  rs
 * changes for the pvm + measure version
 *
 * Revision 1.24  1995/09/18  13:39:52  rs
 * more DBUG output
 *
 * Revision 1.23  1995/09/12  12:26:06  rs
 * pvm + measure fixes
 *
 * Revision 1.22  1995/09/11  14:21:34  rs
 * some changes for the pvm measure version
 *
 * Revision 1.21  1995/09/01  14:31:06  rs
 * bugfix for the tilde-versions: send args+nfv in closures...
 *
 * Revision 1.20  1995/08/09  11:41:53  rs
 * improved shutdown of pvm slaver
 * err slaves ! ;-)
 *
 * Revision 1.19  1995/08/08  14:37:17  rs
 * bug fix in sending doubles (thanks mr. held)
 *
 * Revision 1.18  1995/07/13  13:13:44  rs
 * pvm bug fixes
 *
 * Revision 1.17  1995/07/12  15:23:25  rs
 * some pvm changes
 *
 * Revision 1.16  1995/07/10  14:09:53  rs
 * some minor pvm changes...
 *
 * Revision 1.15  1995/07/07  15:15:07  rs
 * additional pvm changes
 *
 * Revision 1.14  1995/07/05  14:20:47  rs
 * additional pvm changes
 *
 * Revision 1.13  1995/06/29  14:17:00  rs
 * additional pvm changes
 *
 * Revision 1.12  1995/06/28  14:42:01  rs
 * preparing for PVM version
 *
 * Revision 1.11  1995/05/22  09:51:13  rs
 * minor changes (preparing for pvm port)
 *
 * */

/***********************************************************/
/*                                                         */
/* rmessage.c --- message routines for the host            */
/*                                                         */
/* ach 01/03/93                                            */
/*                                                         */
/***********************************************************/

/*#ifdef NOSTORE  cr 23.02.96, no way to do this overriding in red.cnf? */
#if defined(NOSTORE) || (STORE==0)  /* cr/car 2.07.96 */

#undef STORE
#define STORE 0

#else /* NOSTORE cr 19.04.96, we are compiling rstore.o */

#undef D_DIST
#define D_DIST 0
#undef nCUBE
#define nCUBE 0
#undef D_MESS
#define D_MESS 0
#undef D_PVM
#define D_PVM 0
#undef ADV_SCHED
#define D_PVM 0
#undef D_PVM_NCUBE
#define D_PVM_NCUBE 0
#undef NCDEBUG
#define NCDEBUG 0
#undef M_OLD_MERGING
#undef M_BINARY

#endif

#if D_DIST || STORE

/* not working with debug-package, so : */
#if DEBUG
#undef DEBUG
#define DEBUG 0
#define IT_WAS_DEBUG 1
#else
#define IT_WAS_DEBUG 0
#endif 

#include "rstdinc.h"
#include "rstelem.h"
#include "rheapty.h"
#include "rstackty.h"
#include "rinter.h"
#include "rextern.h"
#if nCUBE
#include <nhost.h>
#endif /* nCUBE */
#include <malloc.h>
#if D_MESS
#include "d_mess_io.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
extern host_flush();
extern double host_time();
extern d_write_header();
extern clearscreen();
extern char *_errmes;
extern host_stat();
#endif

#if D_PVM
#include "pvm3.h"
extern int pvmspawn_flag;
extern char *pvmspawn_where;
extern int pvmspawn_ntask;
extern int pvm_tids[];
extern int pvmcoding;
extern int pvm_numt;
#endif /* D_PVM */

#include "dbug.h"

#if (D_DIST || STORE)
extern T_DESCRIPTOR *newdesc();
extern int newheap();
extern int isdesc();
#endif

#if D_MESS
typedef enum {MT_NEW_GRAPH=100, MT_NEW_PROCESS, MT_RESULT, MT_END_RESULT, MT_PARAM, MT_NCUBE_READY, MT_CLOSE_MEASURE, MT_OPEN_MEASURE, MT_SEND_MEASURE, \
MT_REMOTE_MEASURE, MT_POST_MORTEM, MT_ABORT, MT_NCUBE_FAILED, MT_TERMINATE} MESSAGE_TYPE;
#else
typedef enum {MT_NEW_GRAPH=100, MT_NEW_PROCESS, MT_RESULT, MT_END_RESULT, MT_PARAM, MT_NCUBE_READY, MT_POST_MORTEM, MT_ABORT, MT_NCUBE_FAILED, MT_TERMINATE} MESSAGE_TYPE;
#endif

#define MESSAGE_SIZE    1024
#define END_MARKER      0x42424242
#define FIRST_TYPE      MT_NEW_GRAPH
#define LAST_TYPE       MT_ABORT
#define PM_DESC_MARKER  0x26000001         /* for sending of pattern match code */

/* global variables */
#if STORE
extern T_HEAPELEM *newbuff();                       /* rheap.c */
extern PTR_DESCRIPTOR _desc; /* result descriptor (rstate.c) */
int store;             /* file descriptor of the store */

#if TASM_STORE
#ifdef __sparc__
#if !NO_STACK_REG
  register int *st_w asm ("g7");
#else
  extern int *st_w;
#endif
#else
  extern int *st_w;
#endif /* TASM_STORE */

#endif
#else

int _tmp,_tmp2;
int cube;              /* file descriptor of the subcube */

#if (ADV_SCHED && D_MESS)
int no_suspended = 0;
#endif /* ADV_SCHED && D_MESS */

#endif /* STORE */

/* extern variables */
extern char ticket_file[];
extern int _prec_mult;
extern int _prec;
extern int _prec10;
extern BOOLEAN _formated;
extern BOOLEAN LazyLists;
extern BOOLEAN _digit_recycling;
extern BOOLEAN _beta_count_only;
extern int _trunc;
extern BOOLEAN _count_reductions;
extern int _maxnum;
extern int _base;
extern unsigned int _redcnt;
extern void stack_error();
extern INSTR_DESCR instr_tab[];      /* rncinter.h */
extern T_CODEDESCR codedesc;
extern int nc_heaps,nc_heapd,nc_stacks;
extern int static_heap_upper_border;
extern int static_heap_lower_border;
extern int cube_dim;
extern int cube_stack_seg_size;

#if nCUBE
#if MSG_CHKSUM
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(nwrite(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),0xffff,type), \
			 buffer_p=buffer):0, \
			_tmp = (int)(data), \
			chksum += _tmp, \
			*buffer_p++ = SWAP(_tmp))

#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nread(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),(long *)&src,&type), \
			   buffer_p = buffer):0, \
			  chksum += SWAP(*buffer_p), \
			  SWAP(*buffer_p++)) 

#else

#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(nwrite(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),0xffff,type), \
			 buffer_p=buffer):0, \
			*buffer_p++ = SWAP((int)(data)))

#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nread(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),(long *)&src,&type), \
			   buffer_p = buffer):0, \
			  SWAP(*buffer_p++)) 
#endif
#endif /* nCUBE */

#if D_PVM
#define DO_SEND(data)  (_tmp=SWAP(data),(pvm_counter > MESSAGE_SIZE)? \
                        (pvm_mcast(pvm_tids, pvmspawn_ntask, type),\
                        pvm_counter = 0, \
                        pvm_initsend(pvmcoding)):0, \
                        pvm_pkint(&_tmp,1,1),\
                        pvm_counter++)

#define DO_RECEIVE()   ((pvm_counter > MESSAGE_SIZE)? \
                        (pvm_recv(src, type), \
                        pvm_counter = 0):0, \
                        pvm_upkint (&_tmp, 1, 1), \
		 	pvm_counter++,SWAP(_tmp))

#endif /* D_PVM */

#if (STORE && !(D_PVM_NCUBE || D_PVM || nCUBE))
#if TASM_STORE
#define PREFIX(f)        tasm_store_ ## f
#define STRING_PREFIX(f) "tasm_store_" f
#else
#define PREFIX(f)        store_ ## f
#define STRING_PREFIX(f) "store_" f
#endif

#define SWAP(a) a
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(write(store,(char *)buffer,MESSAGE_SIZE*sizeof(int)), \
			 buffer_p=buffer):0, \
			*buffer_p++ = SWAP((int)(data)))

#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (read(store,(char *)buffer,MESSAGE_SIZE*sizeof(int)), \
			   buffer_p = buffer):0, \
			  SWAP(*buffer_p++)) 
#else
#define PREFIX(f)        f
#define STRING_PREFIX(f) f
#endif

#define SEND_2_SHORTS(data1,data2)  DO_SEND(((int)(data1) << 16) | (int)(data2))

#define RECEIVE_2_SHORTS(data1,data2) do {int tmp_data; \
					  tmp_data = DO_RECEIVE(); \
					  data1 = tmp_data >> 16; \
					  data2 = tmp_data & 0xffff;} while(0)

#if (nCUBE || D_PVM_NCUBE)
#define SWAP(a) (_tmp2 = (a),(((_tmp2 & 0xff) << 24) | ((_tmp2 & 0xff00) << 8) | ((_tmp2 & 0xff0000) >> 8) | ((_tmp2 & 0xff000000) >> 24)))
#endif /* nCUBE */

#if (D_PVM && !D_PVM_NCUBE)
#define SWAP(a) a
#endif /* D_PVM */

#if STORE
#define GET_CODE_START() code_start
#define SET_CODE_START_I(n) for (i=0, code_vector = sent_code; (i <= n) && (code_vector != NULL); i++, code_vector = code_vector->next ) code_start = code_vector->start;
#define SET_CODE_START(a) code_start = a
#else
#define GET_CODE_START() (codedesc.codvec + 4)
#endif /* STORE */

#define IS_STATIC(addr)             (((addr) <= static_heap_upper_border) && ((addr) >= static_heap_lower_border))
#define STATIC_OFFSET(addr)         (static_heap_upper_border - (addr))
#define REAL_STATIC_ADDRESS(offset) (static_heap_upper_border - (offset)) 

typedef struct sent_list_node {PTR_DESCRIPTOR ptr;
                               int original_entry;
                               DESC_CLASS original_class;
                               struct sent_list_node * next;} SENT_LIST_NODE;

#if STORE
typedef struct sent_code_vector {PTR_HEAPELEM start;
                                 int length;
                                 struct sent_code_vector * next;} SENT_CODE_VECTOR;
#endif /* STORE */

/* introduce one level of indirection in stack-macros */
/* and use appropriate stack-system in tasm and red   */
/* cr 07.05.96 */
#undef PUSH_E
#undef READ_E
#undef POP_E
#if STORE && TASM_STORE
#define PUSH_E(x)      (*++st_w = (x))
#define READ_E()       (st_w[0])
#define POP_E()        (*st_w--)
#else
#define PUSH_E(x)      PUSHSTACK(S_e,x)
#define READ_E()       READSTACK(S_e)
#define POP_E()        POPSTACK(S_e)
#define PUSH_A(x)      PUSHSTACK(S_a,x)
#define READ_A()       READSTACK(S_a)
#define POP_A()        POPSTACK(S_a)
#endif /* STORE && TASM_STORE */

#if !STORE
#if nCUBE

/***********************************************************/
/*                                                         */
/* init_ncube() - initializes the nCube                    */
/*                                                         */
/***********************************************************/

void init_ncube(int dim)
{int subcube;

 DBUG_ENTER ("init_ncube");

 if ((cube = nopen(dim)) < 0)
   post_mortem("error: could not allocate subcube!");
 subcube = nodeset(cube,"all");

#if NCDEBUG
 DBMinit(cube, dim);
#endif

 DBUG_PRINT("HOST", ("Ich installiere nCUBE-Binary !"));
#if D_BENCH
 if (rexecl(subcube,"ncbench","ncbench",0).np_pid == -1)
#else
 if (rexecl(subcube,D_CUBEBIN,D_CUBEBIN,0).np_pid == -1)
#endif
   post_mortem ("init_ncube: cannot install nCUBE binary !");

 DBUG_VOID_RETURN;
}

#endif /* nCUBE */

/***********************************************************/
/*                                                         */
/* send_params() - sends the parameters for execution to   */
/*                 the ncube                               */
/*                                                         */
/***********************************************************/

void send_params()

{int *buffer, *buffer_p, *buffer_end;
 MESSAGE_TYPE type=MT_PARAM;
#if MSG_CHKSUM
 int chksum = 0;
#endif
#if D_PVM
 int pvm_counter=0;
#endif /* D_PVM */

 DBUG_ENTER ("send_params");

 /* Prepare the message buffer */

 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;

#if D_PVM
 pvm_counter = 0;
 pvm_initsend(pvmcoding);
#endif /* D_PVM */

 DO_SEND(_prec_mult);
 DO_SEND(_prec);
 DO_SEND(_prec10);
 DO_SEND(_formated);
 DO_SEND(LazyLists);
 DO_SEND(_digit_recycling);
 DO_SEND(_beta_count_only);
 DO_SEND(_trunc);
 DO_SEND(_count_reductions);
 DO_SEND(_maxnum);
 DO_SEND(_base);
 DO_SEND(nc_heaps);
 DO_SEND(nc_heapd);
 DO_SEND(nc_stacks);
 DO_SEND(cube_stack_seg_size);

#if D_MESS
 /* now send Measure function flags */
 DO_SEND (m_ackno);
 DO_SEND (m_merge_strat);
 DO_SEND (d_bib_mask);
 /* now send measurement file names prefix */
#if nCUBE
 strcpy((char *)buffer_p, m_mesfilehea);
 strcpy(((char *)buffer_p)+strlen(m_mesfilehea)+1, m_mesfilepath);
 buffer_p += ((strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/4)+1;
#endif
#if D_PVM
 pvm_pkstr(m_mesfilehea); 
 pvm_pkstr(m_mesfilepath); 
#endif
 DBUG_PRINT("MSG", ("m_mesfilehea: %s, strlen: %d, m_mesfilepath: %s, strlen: %d, int's: %d", m_mesfilehea, strlen(m_mesfilehea), \
m_mesfilepath, strlen(m_mesfilepath), ((strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/4)+1));
#endif

 DO_SEND(0x42424242);

#if nCUBE
 nwrite(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),0xffff,type);
#endif /* nCUBE */

#if D_PVM
 pvm_mcast(pvm_tids, pvmspawn_ntask, type);
#endif /* D_PVM */

 free(buffer);

 DBUG_VOID_RETURN;
}
#endif /* STORE */

/**********************************************************************/
/*                                                                    */
/* send_graph() - sends the graph to the nCube       (-DSTORE=0)      */
/* store_send_graph() - sends the graph to the store (-DSTORE=1)      */
/*                                                                    */
/**********************************************************************/

void PREFIX(send_graph)()
{int sent_index=0;                /* counter for sent descriptors  */
 int heap_elems,heap_counter;
 int code_sent=0;
 int param_counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
#if D_PVM
 int pvm_counter=0;
#endif /* D_PVM */
 INSTR_DESCR send_instr;
 DESC_CLASS d_class;              /* class of descriptor           */
 DESC_TYPE d_type;                /* type of descriptor            */
 STACKELEM send_data;
 PTR_HEAPELEM heap_ptr,code_address;
#if STORE
  PTR_HEAPELEM code_start=(codedesc.codvec + 4);
  SENT_CODE_VECTOR *sent_code = NULL,**sent_code_last = &sent_code, *new_code;
#endif /* STORE */
 int *buffer,*buffer_p,*buffer_end;
 char *ngetp();
 SENT_LIST_NODE *sent_list = NULL,*new_node;
#if !STORE
 MESSAGE_TYPE type=MT_NEW_GRAPH;
#endif /* !STORE */

 DBUG_ENTER (STRING_PREFIX("send_graph"));

 START_MODUL("send_graph");

 /* prepare the message buffer */
 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;

#if D_PVM
 pvm_counter = 0;
 pvm_initsend(pvmcoding);
#endif /* D_PVM */
 
 /* send the header */
#if !STORE
 DO_SEND(_redcnt);
 send_data = READ_E();
#else
 send_data = (STACKELEM)_desc;
#endif /* !STORE */
 PUSH_E(ST_END_MARKER);
 PUSH_E(send_data);
 
 while((send_data = POP_E()) != ST_END_MARKER)
   {if ((!T_POINTER(send_data) || !isdesc(send_data)) && send_data) {
      DBUG_PRINT("MSG", ("simple stackelement"));
      DO_SEND(send_data);}
   else
     {if (!send_data)
	{DBUG_PRINT("MSG", ("no data..."));
         DO_SEND(0);
	 DO_SEND(1);}
     else
       {if (IS_STATIC(send_data))                       /* pointer into static heap? */
	  {DBUG_PRINT("MSG", ("pointer into static heap"));
           DO_SEND(0);	
	   DO_SEND(((int)STATIC_OFFSET(send_data) << 4) | 0x0a);}     /* ...1010 as tag for offset in static heap */
       else
	 {DBUG_PRINT("MSG", ("pointer into dynamic heap"));
         if (R_DESC(*(T_PTD)send_data,class) == C_SENT)   /* descriptor already sent? */
	    {DBUG_PRINT("MSG", ("already sent"));
             DO_SEND(0);
	     DO_SEND(((*((int *)send_data + 1)) << 4) | 0x0e);}
	 else
	   {DBUG_PRINT("MSG", ("not sent yet"));
            DO_SEND(0);
	    DO_SEND(0);                                  /* tag for 'pointer to following descriptor' */
	    
	    /* send class and type of descriptor packed into one int */
	    
	    SEND_2_SHORTS((short)(d_class = R_DESC(*(T_PTD)send_data,class)),(short)(d_type = R_DESC(*(T_PTD)send_data,type)));
	    
	    /* refcount doesn`t need to be sent, will be set to 1 on dest processor */
	    /* and now: the main part of sending: the descriptor bodies */
	    switch(d_class)
	      {case C_SCALAR:
                 DBUG_PRINT("MSG", ("C_SCALAR"));
		 DO_SEND(R_SCALAR(*(T_PTD)send_data,vali));
#if (nCUBE || D_PVM_NCUBE)
		 DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));
		 DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));
#else
                 DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));
                 DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));
#endif
		 break;
	       case C_DIGIT:
                 DBUG_PRINT("MSG", ("C_DIGIT"));
		 SEND_2_SHORTS(R_DIGIT(*(T_PTD)send_data,sign),R_DIGIT(*(T_PTD)send_data,ndigit));
		 heap_elems = (int)R_DIGIT(*(T_PTD)send_data,ndigit);
		 DO_SEND(R_DIGIT(*(T_PTD)send_data,Exp));
		 heap_ptr = R_DIGIT(*(T_PTD)send_data,ptdv);
		 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		   DO_SEND(*heap_ptr);
		 break;
	       case C_LIST:
                 DBUG_PRINT("MSG", ("C_LIST"));
		 SEND_2_SHORTS(R_LIST(*(T_PTD)send_data,special),R_LIST(*(T_PTD)send_data,dim));
		 heap_elems = (int)R_LIST(*(T_PTD)send_data,dim);
		 heap_ptr = R_LIST(*(T_PTD)send_data,ptdv);
		 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		   PUSH_E(*heap_ptr);
		 break;
	       case C_INTACT:
                 DBUG_PRINT("MSG", ("C_INTACT"));
		 heap_elems = (int)R_INTACT(*(T_PTD)send_data,dim);
                 DO_SEND(heap_elems);
		 heap_ptr = R_INTACT(*(T_PTD)send_data,args);
		 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		   PUSH_E(*heap_ptr);
		 break;
	       case C_FRAME:
                  switch(d_type)
                  { case TY_FRAME:
                       DBUG_PRINT("MSG", ("C_FRAME/TY_FRAME"));
                       heap_elems = (int)R_FRAME(*(T_PTD)send_data,dim);
                       DO_SEND(heap_elems);
                       heap_ptr = R_FRAME(*(T_PTD)send_data,slots);
                       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
                         PUSH_E(*heap_ptr);
                       break;
                    case TY_SLOT:
                       DBUG_PRINT("MSG", ("C_FRAME/TY_SLOT"));
                       PUSH_E(R_SLOT(*(T_PTD)send_data,name));
                       PUSH_E(R_SLOT(*(T_PTD)send_data,value));
                       break;
                  }
		  break;
	       case C_MATRIX:
                 DBUG_PRINT("MSG", ("C_MATRIX"));
	       case C_VECTOR:
                 DBUG_PRINT("MSG", ("C_VECTOR"));
	       case C_TVECTOR:
                 DBUG_PRINT("MSG", ("C_TVECTOR ***"));
		 SEND_2_SHORTS(R_MVT(*(T_PTD)send_data,nrows,class),R_MVT(*(T_PTD)send_data,ncols,class));
		 heap_elems = (int)R_MVT(*(T_PTD)send_data,nrows,class) * (int)R_MVT(*(T_PTD)send_data,ncols,class);
		 if ((_formated == 1) && (d_type == TY_REAL))
		   heap_elems *= 2;
		 heap_ptr = R_MVT(*(T_PTD)send_data,ptdv,class);
		 if ((_formated == 1) && (d_type != TY_STRING))
		   {if (d_type == TY_REAL)
		      for (heap_counter = 0; heap_counter < heap_elems; heap_counter+=2,heap_ptr+=2)
			{DO_SEND(*(heap_ptr+1));
			 DO_SEND(*heap_ptr);}
		   else
		     for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		       DO_SEND(*heap_ptr);}
		 else
		   for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		     PUSH_E(*heap_ptr);
		 break;
	       case C_EXPRESSION:
                 DBUG_PRINT("MSG", ("C_EXPRESSION"));
	       case C_CONSTANT:
                 DBUG_PRINT("MSG", ("C_CONSTANT ***"));
		 switch(d_type)
		   {case TY_REC:
                      DBUG_PRINT("MSG", ("TY_REC"));
		    case TY_ZF:
                      DBUG_PRINT("MSG", ("TY_ZF"));
		    case TY_SUB:
                      DBUG_PRINT("MSG", ("TY_SUB"));
		      SEND_2_SHORTS(R_FUNC(*(T_PTD)send_data,special),R_FUNC(*(T_PTD)send_data,nargs));
		      if (R_FUNC(*(T_PTD)send_data,namelist))
			{heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,namelist);
			 heap_ptr = R_FUNC(*(T_PTD)send_data,namelist) + 1;
			 DO_SEND(heap_elems);
#if WITHTILDE
			 param_counter = *(heap_ptr++);
			 DO_SEND(param_counter);
			 heap_elems--;
#endif
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
#if WITHTILDE
			   if ((param_counter > 0) && (heap_counter >= param_counter-2))
			     DO_SEND(*heap_ptr);
			   else
#endif
			     PUSH_E(*heap_ptr);}
		      else
			DO_SEND(-1);
#if WITHTILDE
		      if (R_FUNC(*(T_PTD)send_data,pte))
			{
#endif
			  heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
			  DO_SEND(heap_elems);
			  heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
			  for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			    PUSH_E(*heap_ptr);
#if WITHTILDE
			}
		      else
			DO_SEND(-1);
#endif
		      break;
#if WITHTILDE
		    case TY_SNSUB:
                      DBUG_PRINT("MSG", ("TY_SNSUB"));
		      SEND_2_SHORTS(R_FUNC(*(T_PTD)send_data,special),R_FUNC(*(T_PTD)send_data,nargs));
		      heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
		      DO_SEND(heap_elems);
		      heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
#endif
		    case TY_ZFCODE:
                      DBUG_PRINT("MSG", ("TY_ZFCODE"));
		      SEND_2_SHORTS(R_ZFCODE(*(T_PTD)send_data,zfbound),R_ZFCODE(*(T_PTD)send_data,nargs));
		      PUSH_E(R_ZFCODE(*(T_PTD)send_data,ptd));
		      heap_elems = *(int *)R_ZFCODE(*(T_PTD)send_data,varnames);
		      heap_ptr = R_ZFCODE(*(T_PTD)send_data,varnames) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_COND:
                      DBUG_PRINT("MSG", ("TY_COND"));
		      DO_SEND((int)R_COND(*(T_PTD)send_data,special));
		      heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptte);
		      heap_ptr = R_COND(*(T_PTD)send_data,ptte) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptee);
		      heap_ptr = R_COND(*(T_PTD)send_data,ptee) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_VAR:
                      DBUG_PRINT("MSG", ("TY_VAR"));
		      DO_SEND(R_VAR(*(T_PTD)send_data,nlabar));
		      PUSH_E(R_VAR(*(T_PTD)send_data,ptnd));
		      break;
		    case TY_EXPR:
                      DBUG_PRINT("MSG", ("TY_EXPR"));
		      heap_elems = *(int *)R_EXPR(*(T_PTD)send_data,pte);
		      heap_ptr = R_EXPR(*(T_PTD)send_data,pte) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_NAME:
                      DBUG_PRINT("MSG", ("TY_NAME"));
                      if (T_INT((int)R_NAME(*(T_PTD)send_data,ptn))) {
                        /* sorry ;-), but this is the special case for NAME- */
                        /* descriptors in a (UH-) pattern graph, because */
                        /* the (real) name is irrelevant and so the index */
                        /* is stored in the ptn field */ 
                        DBUG_PRINT("MSG", ("It's an index: %d", VAL_INT((int)R_NAME(*(T_PTD)send_data,ptn))));
                        DO_SEND(R_NAME(*(T_PTD)send_data,ptn));
                      } else { 
                      DO_SEND(0); /* T_INT will fail on that (hopefully) */
		      heap_elems = (*(int *)R_NAME(*(T_PTD)send_data,ptn));
                      DBUG_PRINT("MSG", ("heap_elems: %d", heap_elems));
		      heap_ptr = R_NAME(*(T_PTD)send_data,ptn) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
                      } 
		      break;
		    case TY_SWITCH:
                      DBUG_PRINT("MSG", ("TY_SWITCH"));
#if WITHTILDE
		      SEND_2_SHORTS(R_SWITCH(*(T_PTD)send_data,nwhen),R_SWITCH(*(T_PTD)send_data,anz_args));
		      DO_SEND(R_SWITCH(*(T_PTD)send_data,casetype));
#else
		      SEND_2_SHORTS(R_SWITCH(*(T_PTD)send_data,special),R_SWITCH(*(T_PTD)send_data,case_type));
		      DO_SEND(R_SWITCH(*(T_PTD)send_data,nwhen));
#endif
		      heap_elems = *(int *)R_SWITCH(*(T_PTD)send_data,ptse);
		      heap_ptr = R_SWITCH(*(T_PTD)send_data,ptse) + 1;
		      DO_SEND(heap_elems);
#if WITHTILDE
		      DO_SEND(*(heap_ptr+heap_elems-1));
		      heap_elems--;
#endif
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_NOMAT:
                      DBUG_PRINT("MSG", ("TY_NOMAT"));
#if WITHTILDE
		      SEND_2_SHORTS(R_NOMAT(*(T_PTD)send_data,act_nomat),R_NOMAT(*(T_PTD)send_data,reason));
#else
		      DO_SEND(R_NOMAT(*(T_PTD)send_data,act_nomat));
#endif
		      if (R_NOMAT(*(T_PTD)send_data,guard_body))
			{heap_elems = *(int *)R_NOMAT(*(T_PTD)send_data,guard_body);
			 heap_ptr = R_NOMAT(*(T_PTD)send_data,guard_body) + 1;
			 DO_SEND(heap_elems);
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   PUSH_E(*heap_ptr);}
		      else
			DO_SEND(-1);
		      PUSH_E(R_NOMAT(*(T_PTD)send_data,ptsdes));
		      break;
		    case TY_MATCH:
                      DBUG_PRINT("MSG", ("TY_MATCH"));
		      if (R_MATCH(*(T_PTD)send_data,guard))
		       {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,guard);
			heap_ptr = R_MATCH(*(T_PTD)send_data,guard) + 1;
			DO_SEND(heap_elems);
			for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			  PUSH_E(*heap_ptr);}
		      else
			DO_SEND(-1);
		      if (R_MATCH(*(T_PTD)send_data,body))
			{heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,body);
			 heap_ptr = R_MATCH(*(T_PTD)send_data,body) + 1;
			 DO_SEND(heap_elems);
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   PUSH_E(*heap_ptr);}
		      else
			DO_SEND(-1);
		      if (R_MATCH(*(T_PTD)send_data,code))
			{heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,code);
			 heap_ptr = R_MATCH(*(T_PTD)send_data,code) + 1;
			 DO_SEND(heap_elems);
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   {if (T_POINTER(*heap_ptr) && isdesc(*heap_ptr))
			      {DO_SEND(PM_DESC_MARKER);
			       PUSH_E(*heap_ptr);}
			   else
			     DO_SEND(*heap_ptr);}}
		      else
			DO_SEND(-1);
		      break;
		    case TY_LREC:
                      DBUG_PRINT("MSG", ("TY_LREC"));
		      DO_SEND(R_LREC(*(T_PTD)send_data,nfuncs));
		      heap_elems = *(int *)R_LREC(*(T_PTD)send_data,namelist);
		      heap_ptr = R_LREC(*(T_PTD)send_data,namelist) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      heap_elems = R_LREC(*(T_PTD)send_data,nfuncs);
#if WITHTILDE
		      heap_ptr = R_LREC(*(T_PTD)send_data,ptdv)+1;
#else
		      heap_ptr = R_LREC(*(T_PTD)send_data,ptdv);
#endif
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_LREC_IND:
                      DBUG_PRINT("MSG", ("TY_LREC_IND"));
		      DO_SEND(R_LREC_IND(*(T_PTD)send_data,index));
		      PUSH_E(R_LREC_IND(*(T_PTD)send_data,ptdd));
		      PUSH_E(R_LREC_IND(*(T_PTD)send_data,ptf));
		      break;
		    case TY_LREC_ARGS:
                      DBUG_PRINT("MSG", ("TY_LREC_ARGS"));
		      DO_SEND(heap_elems = R_LREC_ARGS(*(T_PTD)send_data,nargs));
		      PUSH_E(R_LREC_ARGS(*(T_PTD)send_data,ptdd));
		      heap_ptr = R_LREC_ARGS(*(T_PTD)send_data,ptdv);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
                    default:
                      DBUG_PRINT("MSG", ("unknown d_type desc: %d", (int) d_type));
                      post_mortem("unknown d_type desc");
                      break;}
		 break;
	       case C_FUNC:
                 DBUG_PRINT("MSG", ("C_FUNC"));
	       case C_CONS:
                 DBUG_PRINT("MSG", ("C_CONS ***"));
		 switch(d_type)
		   {case TY_CLOS:
                      DBUG_PRINT("MSG", ("TY_CLOS"));
		      SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,args),R_CLOS(*(T_PTD)send_data,nargs));
#if WITHTILDE
		      SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,ftype),R_CLOS(*(T_PTD)send_data,nfv));
#else
		      DO_SEND(R_CLOS(*(T_PTD)send_data,ftype));
#endif
#if WITHTILDE
                      heap_elems = R_CLOS(*(T_PTD)send_data,args)+R_CLOS(*(T_PTD)send_data,nfv)+1;
#else
		      heap_elems = R_CLOS(*(T_PTD)send_data,args)+1;
#endif
		      heap_ptr = R_CLOS(*(T_PTD)send_data,pta);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSH_E(*heap_ptr);
		      break;
		    case TY_COMB:
                      DBUG_PRINT("MSG", ("TY_COMB"));
		      SEND_2_SHORTS(R_COMB(*(T_PTD)send_data,args),R_COMB(*(T_PTD)send_data,nargs));
		      PUSH_E(R_COMB(*(T_PTD)send_data,ptd));
		      heap_ptr = R_COMB(*(T_PTD)send_data,ptc);
		      goto send_code;
		    case TY_CONDI:
                      DBUG_PRINT("MSG", ("TY_CONDI"));
		      SEND_2_SHORTS(R_CONDI(*(T_PTD)send_data,args),R_CONDI(*(T_PTD)send_data,nargs));
		      PUSH_E(R_CONDI(*(T_PTD)send_data,ptd));
		      heap_ptr = R_CONDI(*(T_PTD)send_data,ptc);
		      goto send_code;
		    case TY_CONS:
                      DBUG_PRINT("MSG", ("TY_CONS"));
		      PUSH_E(R_CONS(*(T_PTD)send_data,hd));
		      PUSH_E(R_CONS(*(T_PTD)send_data,tl));
		      break;
		    case TY_CASE:
                      DBUG_PRINT("MSG", ("TY_CASE"));
		    case TY_WHEN:
                      DBUG_PRINT("MSG", ("TY_WHEN"));
		    case TY_GUARD:
                      DBUG_PRINT("MSG", ("TY_GUARD"));
		    case TY_BODY:
                      DBUG_PRINT("MSG", ("TY_BODY ***"));
		      SEND_2_SHORTS(R_CASE(*(T_PTD)send_data,args),R_CASE(*(T_PTD)send_data,nargs));
		      PUSH_E(R_CASE(*(T_PTD)send_data,ptd));
		      heap_ptr = R_CASE(*(T_PTD)send_data,ptc);
                      DO_SEND(heap_ptr);
                      if (T_INT((int)heap_ptr)) { /* it's NO code-Pointer ! */
                        DBUG_PRINT("MSG", ("no code pointer"));
                        break;
                        } 
                      else if ((int)heap_ptr < 10) {
                        DBUG_PRINT("MSG", ("quite small for a code pointer, isn't it? %d",heap_ptr));
                        break;
                        }
                        
		   send_code:
#if STORE
                          /* find start of code-vektor */
			  for (code_address=heap_ptr;*code_address != I_TABSTART;code_address--);
                          SET_CODE_START(code_address);

                          /* compute length of code-vektor from start to SYMTAB */
                          /* arg of I_TABSTART = &SYMTAB */
                          heap_elems = (*(INSTR**)(code_address+1)) - code_address;

                          /* scan the list of already sent code-vectors */
                          for (new_code = sent_code, code_sent=0 ; new_code != NULL ; 
                               new_code = new_code->next , code_sent++ )
                            if ((heap_ptr - new_code->start) < new_code->length)
                              break;    /* heap_ptr points into a code-vector that has been sent before */

		      if (new_code != NULL) {
                        DBUG_PRINT("MSG", ("sending code-vector-index %d",code_sent));
			DO_SEND(code_sent);
#else /* STORE */
		      if (code_sent) {
#endif /* STORE */
                        DBUG_PRINT("MSG", ("code-vector %d heap_ptr %x code_start %x code_length %d index %d",code_sent,heap_ptr,GET_CODE_START(),(*(INSTR**)(GET_CODE_START()+1) - GET_CODE_START()),heap_ptr - GET_CODE_START()));
			DO_SEND(heap_ptr - GET_CODE_START());
                        }
		      else
			{
		          DBUG_PRINT("MSG", ("Sending Code..."));	  

#if STORE
                          /* new code vector, add to list of sent vectors */
                          if ((new_code = (SENT_CODE_VECTOR*)malloc(sizeof(SENT_CODE_VECTOR))) != NULL)
                          {
                            new_code->start = code_address;
                            new_code->length = heap_elems;
                            new_code->next = NULL;
                            *sent_code_last = new_code;
                            sent_code_last = &(new_code->next);
                          }
                          else
                            post_mortem("send_code: malloc for new_code failed");

                          DO_SEND(code_sent);
			  DO_SEND(heap_elems);
                          DBUG_PRINT("MSG", ("code-vector %d heap_ptr %x code_address %x code_length %d index %d",code_sent,heap_ptr,code_address,heap_elems,heap_ptr - code_address));
			  DO_SEND(heap_ptr - code_address);

                          heap_ptr = code_address + 2; /* first (pseudo-)instruction */
#else /* STORE */
			  /* count length of code, the receiving node must know the size */
			  /* before the code is sent                                     */
			  for (heap_elems=1,code_address=heap_ptr;*code_address++ != I_SYMTAB;heap_elems++);
			  DO_SEND(heap_elems);
			  code_address = heap_ptr;
#endif /* STORE */
			  while(*heap_ptr != I_SYMTAB)
			    {DO_SEND(*heap_ptr);
			     send_instr=instr_tab[*heap_ptr++];
			     switch(send_instr.paramtype)
			       {case NUM_PAR:
				  /* Special case, because the distribution instruction needs */
				  /* two ADDR_PARs and two NUM_PARs                           */
				  if ((send_instr.instruction == I_DIST) || (send_instr.instruction == I_DISTB))
				    {DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
				     DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
				     DO_SEND(*heap_ptr++);
				     DO_SEND(*heap_ptr++);
#if WITHTILDE
				     DO_SEND(*heap_ptr++);
				     DO_SEND(*heap_ptr++);
#endif
				     break;}

                                  /* special treatment for some pattern matching */
                                  /* instructions */
                                  
                                  if ((send_instr.instruction >= I_APPEND) && (send_instr.instruction <= I_MATCHDIGIT)) {
                                    switch(send_instr.instruction) {
                                      case I_ATEND:
                                      case I_ATSTART:  /* one index and one label */
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", send_instr.instruction));
                                        DO_SEND(*heap_ptr++);
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        break; 
                                      case I_MATCHARB:
                                      case I_MATCHARBS:
                                      case I_MATCHBOOL:
                                      case I_MATCHC:
                                      case I_MATCHIN:
                                      case I_MATCHINT:
                                      case I_MATCHLIST:
                                      case I_MATCHPRIM:
                                      case I_MATCHSTR:  /* one index and two labels */
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", send_instr.instruction));
                                        DO_SEND(*heap_ptr++);
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        break;
                                      case I_MATCHREAL:
                                      case I_MATCHDIGIT: /* one descriptor and two labels */
                                        DBUG_PRINT("MSG", ("converting PM instruction %i", send_instr.instruction));
                                        PUSH_E(*(heap_ptr++));
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        break;
                                      case I_TGUARD:
                                        DBUG_PRINT("MSG", ("converting TGUARD PM instruction..."));
                                        DO_SEND(((PTR_HEAPELEM)(*heap_ptr++)) - code_address);
                                        PUSH_E(*(heap_ptr++));
                                        DO_SEND(*heap_ptr++);
                                        DO_SEND(*heap_ptr++);
                                        DO_SEND(*heap_ptr++);
                                        break; 
                                      default: 
                                        for (param_counter = 0;param_counter < send_instr.nparams; param_counter++)
                                        DO_SEND(*heap_ptr++);
                                        break;
                                      }
                                    break;
                                    }

				case ATOM_PAR:
				case FUNC_PAR:
				  for (param_counter = 0;param_counter < send_instr.nparams;param_counter++)
				    DO_SEND(*heap_ptr++);
				  break;
				case DESC_PAR:
				  PUSH_E(*(heap_ptr++));
				  break;
				case ADDR_PAR:
				  DO_SEND((PTR_HEAPELEM)*(heap_ptr++) - code_address);
				  break;
				}      /* switch */
			   }           /* while */
			  code_sent = 1;
			}              /* else {code not sent yet} */
		      break;
		    default:
                      DBUG_PRINT("MSG", ("unknown C_FUNC C_CONS desc: %d", (int) d_type));
                      post_mortem("unknown C_FUNC C_CONS desc");
                      break;}
                                      /* switch(d_type) */
		 break;
               case C_MATCHING:
                 DBUG_PRINT("MSG", ("C_MATCHING"));
                 switch(d_type) {
                   case TY_SELECTION:
                     DBUG_PRINT("MSG", ("TY_SELECTION"));
                     DO_SEND((int)R_SELECTION(*(T_PTD)send_data, nclauses));
                     PUSH_E(R_SELECTION(*(T_PTD)send_data, clauses));
                     PUSH_E(R_SELECTION(*(T_PTD)send_data, otherwise));
                     break;
                   case TY_CLAUSE:
                     DBUG_PRINT("MSG", ("TY_CLAUSE"));
                     PUSH_E(R_CLAUSE(*(T_PTD)send_data, next));
                     heap_ptr = R_CLAUSE(*(T_PTD)send_data, sons);
                     for (heap_counter=0;heap_counter < 4;heap_counter++,heap_ptr++) {
                       PUSH_E(*heap_ptr);
                       DBUG_PRINT("MSG", ("*heap_ptr = %x", *heap_ptr)); }
                     break;
                   default:
                     DBUG_PRINT("MSG", ("unknown d_type")); break;
                   }
                 break;
               case C_PATTERN:
                 DBUG_PRINT("MSG", ("C_PATTERN"));
                 DO_SEND((int)R_PATTERN(*(T_PTD)send_data, following)); 
                 PUSH_E(R_PATTERN(*(T_PTD)send_data, pattern));
                 PUSH_E(R_PATTERN(*(T_PTD)send_data, variable)); 
                 break;
               default:
                 DBUG_PRINT("MSG", ("unknown d_class desc: %d", (int) d_class ));
                 post_mortem("unknown d_class desc");
                 break;
	       }                      /* switch(d_class) */
	    new_node = (SENT_LIST_NODE *)malloc(sizeof(SENT_LIST_NODE));
	    new_node->ptr = (T_PTD)send_data;
	    new_node->original_entry = *((int *)send_data + 1);
	    new_node->original_class = d_class;
	    new_node->next = sent_list;
	    sent_list = new_node;
	    L_DESC(*(T_PTD)send_data,class) = C_SENT;
	    *((int *)send_data + 1) = sent_index++;
	  }                            /* else {descriptor not sent yet}*/
       }                              /* else {no static pointer} */
      }                              /* else {null pointer} */
    }                                 /* else {pointer on stack} */
  }                                    /* while */
#if MSG_CHKSUM
 DO_SEND(chksum);
#endif
 DO_SEND(END_MARKER);
 while (sent_list)
   {SENT_LIST_NODE *tmp;
    
    L_DESC(*(sent_list->ptr),class) = sent_list->original_class;
    *((int *)sent_list->ptr + 1) = sent_list->original_entry;
    tmp = sent_list;
    sent_list = sent_list->next;
    free(tmp);}

#if nCUBE
 nwrite(cube,(char *)buffer,(buffer_p - buffer)*sizeof(int),0xffff,type);
#endif /* nCUBE */

#if STORE
 write(store,(char *)buffer,(buffer_p - buffer)*sizeof(int));
#endif /* STORE */

#if D_PVM
 DBUG_PRINT ("PVM", ("multicasting..."));
 pvm_mcast(pvm_tids, pvmspawn_ntask, type);
#endif /* D_PVM */

 free(buffer);

 END_MODUL("send_graph");

 DBUG_VOID_RETURN;
}

/*******************************************************************************/
/*                                                                             */
/* receive_result() - receives the result from the nCube          (-DSTORE=0)  */
/* store_receive_result() - receives an expression from the store (-DSTORE=1)  */
/*                                                                             */
/*******************************************************************************/

void PREFIX(receive_result)()
{int *buffer,*buffer_p,*buffer_end;
#if STORE
 int i, type = -1;
#else
 int src = -1,type = -1;
#endif /* STORE */
 int heap_elems = 0,heap_counter;
 int received_index= 0;
 int *rec_addr, rec_data;
 int counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
#if D_PVM
 int pvm_bufid;
 int pvm_length;
 int pvm_counter=0;
#endif /* D_PVM */
 DESC_CLASS new_class;
 DESC_TYPE new_type;
 PTR_HEAPELEM new_heap;
 T_PTD *received_list;
 T_PTD new_desc;
 int rec_list_size = 256;
#if STORE
  INSTR **pptc;
  int code_received = 0, next_code_vector = 0;
  PTR_HEAPELEM code_address,code_start;
  INSTR_DESCR rec_instr;
  int param_counter;
  SENT_CODE_VECTOR *sent_code = NULL,**sent_code_last = &sent_code, *new_code, *code_vector;
#endif /* STORE */

 DBUG_ENTER (STRING_PREFIX("receive_result"));

 /* prepare the received list */
 received_list = (T_PTD *)malloc(sizeof(T_PTD)*rec_list_size);

 /* receive first block off message */
 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;

#if nCUBE 
 nrange(cube,FIRST_TYPE,LAST_TYPE);

 nread(cube,(char *)buffer,MESSAGE_SIZE * sizeof(int),(long *)&src,&type);
#endif /* nCUBE */

#if STORE
 read(store,(char *)buffer,MESSAGE_SIZE * sizeof(int));
#endif /* STORE */

#if D_PVM
 DBUG_PRINT("PVM", ("waiting for a message..."));
 pvm_bufid = pvm_recv (-1,-1);
 pvm_bufinfo (pvm_bufid, &pvm_length, &type, &src);
 DBUG_PRINT("PVM", ("(first part of) message received..."));
#endif /* D_PVM */

 if (type == MT_POST_MORTEM)
/*   {exit_ncube(); */
   {

#if D_PVM
    pvm_upkstr(buffer);
    exit_slaves();
#endif /* D_PVM */

   post_mortem((char *)buffer);
   }
 
 /* receive the header */
 
#if !STORE
 _redcnt      = DO_RECEIVE();
#endif /* !STORE */

#if (ADV_SCHED && D_MESS)
 no_suspended = DO_RECEIVE();
#endif /* ADV_SCHED && D_MESS */
 
 PUSH_E(ST_END_MARKER);
#if STORE
 _desc = (PTR_DESCRIPTOR)ST_DUMMY;
 PUSH_E((int)&_desc << 1);
#else
 PUSH_A(ST_DUMMY);
 PUSH_E((int)S_a.TopofStack << 1);
#endif /* STORE */

 while((int)(rec_addr = (int *)POP_E()) != ST_END_MARKER)
   {if ((int)rec_addr & 1)
	   {counter = POP_E();
	    if (counter > 1)
	      {PUSH_E(counter - 1);
	       PUSH_E(rec_addr - 2);}}
	 (int)rec_addr >>= 1;
	 rec_data = DO_RECEIVE();
	 if (rec_data != 0)
	   *rec_addr = rec_data;
	 else
	   {rec_data = DO_RECEIVE();
	    if (T_STAT_POINTER(rec_data))
	      {DBUG_PRINT("MSG", ("static pointer"));
               *rec_addr = REAL_STATIC_ADDRESS(rec_data >> 4);
	       INC_REFCNT((T_PTD)*rec_addr);}
	    else
	      {DBUG_PRINT("MSG", ("dynamic pointer"));
               if (T_DESC_INDEX(rec_data))
		 {DBUG_PRINT("MSG", ("already received"));
                  *rec_addr = (int) received_list[rec_data >> 4];
		  INC_REFCNT((T_PTD)*rec_addr);}
	       else
		 {if (T_NULL_POINTER(rec_data))
		    {*rec_addr = NULL;}
		 else                    /* a new descriptor will follow */
		   {DBUG_PRINT("MSG", ("a new pointer"));
                    RECEIVE_2_SHORTS(new_class,new_type);
		    MAKEDESC(new_desc,1,new_class,new_type);
		    *rec_addr = (int) new_desc;
		    switch(new_class)
		      {case C_SCALAR:
                         DBUG_PRINT("MSG", ("C_SCALAR"));
			 L_SCALAR(*new_desc,vali) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr)) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr) + 1) = DO_RECEIVE();
			 break;
		       case C_DIGIT:
                         DBUG_PRINT("MSG", ("C_DIGIT"));
			 RECEIVE_2_SHORTS(L_DIGIT(*new_desc,sign),L_DIGIT(*new_desc,ndigit));
			 L_DIGIT(*new_desc,Exp) = DO_RECEIVE();
			 heap_elems = R_DIGIT(*new_desc,ndigit);
			 GET_HEAP(heap_elems,A_DIGIT(*new_desc,ptdv));
			 new_heap = R_DIGIT(*new_desc,ptdv);
			 for (heap_counter = 0;heap_counter < heap_elems;heap_counter++,new_heap++)
			   *new_heap = DO_RECEIVE();
			 break;
		       case C_LIST:
                         DBUG_PRINT("MSG", ("C_LIST"));
			 RECEIVE_2_SHORTS(L_LIST(*new_desc,special),L_LIST(*new_desc,dim));
			 L_LIST(*new_desc,ptdd) = NULL;
			 PUSH_E(heap_elems = R_LIST(*new_desc,dim));
			 GET_HEAP(heap_elems,A_LIST(*new_desc,ptdv));
			 new_heap = R_LIST(*new_desc,ptdv);
			 PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			 break;
		       case C_INTACT:
                         DBUG_PRINT("MSG", ("C_INTACT"));
                         L_INTACT(*new_desc,dim) = DO_RECEIVE();
			 PUSH_E(heap_elems = R_INTACT(*new_desc,dim));
			 GET_HEAP(heap_elems,A_INTACT(*new_desc,args));
			 new_heap = R_INTACT(*new_desc,args);
			 PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			 break;
                       case C_FRAME:
                          switch(new_type)
                          { case TY_FRAME:
                               DBUG_PRINT("MSG", ("C_FRAME/TY_FRAME"));
                               L_FRAME(*new_desc,dim) = DO_RECEIVE();
                               PUSH_E(heap_elems = R_FRAME(*new_desc,dim));
                               GET_HEAP(heap_elems,A_FRAME(*new_desc,slots));
                               new_heap = R_FRAME(*new_desc,slots);
                               PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
                               break;
                            case TY_SLOT:
                               DBUG_PRINT("MSG", ("C_FRAME/TY_SLOT"));
                               PUSH_E((int)A_SLOT(*new_desc,name) << 1);
                               PUSH_E((int)A_SLOT(*new_desc,value) << 1);
                               break;
                          }
                          break;
		       case C_MATRIX:
                         DBUG_PRINT("MSG", ("C_MATRIX"));
		       case C_VECTOR:
                         DBUG_PRINT("MSG", ("C_VECTOR"));
		       case C_TVECTOR:
                         DBUG_PRINT("MSG", ("C_TVECTOR ***"));
			 RECEIVE_2_SHORTS(L_MVT(*new_desc,nrows,new_class),L_MVT(*new_desc,ncols,new_class));
			 L_MVT(*new_desc,ptdd,new_class) = NULL;
			 if ((_formated == 1) && (new_type != TY_STRING))
			   {heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class));
			    if (new_type == TY_REAL)
			      heap_elems *= 2;}
			 else
			   PUSH_E(heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class)));
			 GET_HEAP(heap_elems,A_MVT(*new_desc,ptdv,new_class));
			 new_heap = R_MVT(*new_desc,ptdv,new_class);
			 if ((_formated == 1) && (new_type != TY_STRING))
			   {if (new_type == TY_REAL)
			      for (;heap_elems >0;heap_elems-=2,new_heap+=2)
				{*(new_heap+1) = DO_RECEIVE();
				 *new_heap = DO_RECEIVE();}
			   else
			     for(;heap_elems >0;heap_elems--,new_heap++)
			       *new_heap = DO_RECEIVE();}
			 else
			   PUSH_E(((int)(new_heap + (heap_elems -1)) << 1) | 1);
			 break;
                       case C_MATCHING:
                         DBUG_PRINT("MSG", ("C_MATCHING"));
                         switch(new_type) {
                           case TY_SELECTION:
                             DBUG_PRINT("MSG", ("TY_SELECTION"));
                             L_SELECTION(*new_desc,nclauses) = (unsigned short) DO_RECEIVE();
                             PUSH_E((int) A_SELECTION(*new_desc, clauses) << 1);
                             PUSH_E((int) A_SELECTION(*new_desc, otherwise) << 1);
                             break;
                           case TY_CLAUSE:
                             DBUG_PRINT("MSG", ("TY_CLAUSE"));
                             PUSH_E((int)A_CLAUSE(*new_desc,next) << 1);
                             heap_elems = 4;
                             PUSH_E(heap_elems); 
                             GET_HEAP(heap_elems,A_CLAUSE(*new_desc,sons));
                             new_heap = R_CLAUSE(*new_desc,sons);
                             PUSH_E(((int)(new_heap + heap_elems-1) << 1) | 1);
                             break;
                           default:
                             DBUG_PRINT("MSG", ("unknown d_type")); break;
                           }
                         break;
                       case C_PATTERN:
                         DBUG_PRINT("MSG", ("C_PATTERN")); 
                         L_PATTERN(*new_desc,following) = DO_RECEIVE();
                         PUSH_E((int) A_PATTERN(*new_desc, pattern) << 1);
                         PUSH_E((int) A_PATTERN(*new_desc, variable) << 1);
                         break;
		       case C_EXPRESSION:
                         DBUG_PRINT("MSG", ("C_EXPRESSION"));
		       case C_CONSTANT:
                         DBUG_PRINT("MSG", ("C_CONSTANT ***"));
			 switch(new_type)
			   {case TY_REC:
                              DBUG_PRINT("MSG", ("TY_REC"));
			    case TY_ZF:
                              DBUG_PRINT("MSG", ("TY_ZF"));
			    case TY_SUB:
                              DBUG_PRINT("MSG", ("TY_SUB ***"));
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{
#if WITHTILDE
				  counter = DO_RECEIVE();
				  if (counter > 0)
				    PUSH_E(counter-2);
				  else
				    PUSH_E(heap_elems -1);
#else
				  if (heap_elems > 0)
				    PUSH_E(heap_elems);
#endif
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,namelist));
				  new_heap = R_FUNC(*new_desc,namelist);
				  *new_heap = heap_elems;
#if WITHTILDE
				  *(++new_heap)=counter;
				  if (counter > 0)
				    {PUSH_E(((int)(new_heap + counter - 2) << 1) | 1);
				     for (;heap_elems>counter-1;heap_elems--)
				       *(new_heap + heap_elems - 1)=DO_RECEIVE();}
				  else
				    PUSH_E(((int)(new_heap + heap_elems - 1) << 1) | 1);
#else
				  if (heap_elems > 0)
				    PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
#endif
				}
			      else
				L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
#if WITHTILDE
			      if (heap_elems >= 0)
				{
#endif
                                  PUSH_E(heap_elems); /* cr 21.02.96, 
                                      moved this below the test 'if (heap_elems >= 0)' */
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
				  new_heap = R_FUNC(*new_desc,pte);
				  *new_heap = heap_elems;
				  PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
#if WITHTILDE
				}
			      else
				L_FUNC(*new_desc,pte) = NULL;
#endif
			      break;
#if WITHTILDE
			    case TY_SNSUB:
                              DBUG_PRINT("MSG", ("TY_SNSUB"));
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
			      new_heap = R_FUNC(*new_desc,pte);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      break;
#endif
			    case TY_ZFCODE:
                              DBUG_PRINT("MSG", ("TY_ZFCODE"));
			      RECEIVE_2_SHORTS(L_ZFCODE(*new_desc,zfbound),L_ZFCODE(*new_desc,nargs));
			      PUSH_E((int)A_ZFCODE(*new_desc,ptd) << 1);
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_ZFCODE(*new_desc,varnames));
			      new_heap = R_ZFCODE(*new_desc,varnames);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_COND:
                              DBUG_PRINT("MSG", ("TY_COND"));
			      L_COND(*new_desc,special) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptte));
			      new_heap = R_COND(*new_desc,ptte);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptee));
			      new_heap = R_COND(*new_desc,ptee);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_VAR:
                              DBUG_PRINT("MSG", ("TY_VAR"));
			      L_VAR(*new_desc,nlabar) = DO_RECEIVE();
			      PUSH_E((int)A_VAR(*new_desc,ptnd) << 1);
			      break;
			    case TY_EXPR:
                              DBUG_PRINT("MSG", ("TY_EXPR"));
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_EXPR(*new_desc,pte));
			      new_heap = R_EXPR(*new_desc,pte);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NAME:
                              DBUG_PRINT("MSG", ("TY_NAME"));
			      heap_elems = DO_RECEIVE();
                              if (T_INT(heap_elems)) {
                                /* for description: see the sending of TY_NAME */
                                L_NAME(*new_desc,ptn) = (int) heap_elems;
                                DBUG_PRINT("MSG", ("It's an index: %d", VAL_INT(heap_elems)));
                              } else {
                              heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_NAME(*new_desc,ptn));
			      new_heap = R_NAME(*new_desc,ptn);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
                              }
			      break;
			    case TY_SWITCH:
                              DBUG_PRINT("MSG", ("TY_SWITCH"));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_SWITCH(*new_desc,nwhen),L_SWITCH(*new_desc,anz_args));
			      L_SWITCH(*new_desc,casetype) = DO_RECEIVE();
#else
			      RECEIVE_2_SHORTS(L_SWITCH(*new_desc,special),L_SWITCH(*new_desc,case_type));
			      L_SWITCH(*new_desc,nwhen) = DO_RECEIVE();
#endif
			      heap_elems = DO_RECEIVE();
			      GET_HEAP(heap_elems+1,A_SWITCH(*new_desc,ptse));
			      new_heap = R_SWITCH(*new_desc,ptse);
			      *new_heap = heap_elems;
#if WITHTILDE
			      *(new_heap+(heap_elems--))=DO_RECEIVE();
#endif
			      PUSH_E(heap_elems);
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NOMAT:
                              DBUG_PRINT("MSG", ("TY_NOMAT"));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_NOMAT(*new_desc,act_nomat),L_NOMAT(*new_desc,reason));
#else
			      L_NOMAT(*new_desc,act_nomat) = DO_RECEIVE();
#endif
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{if (heap_elems > 0)
				   PUSH_E(heap_elems);
				 GET_HEAP(heap_elems+1,A_NOMAT(*new_desc,guard_body));
				 new_heap = R_NOMAT(*new_desc,guard_body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_NOMAT(*new_desc,guard_body) = NULL;
			      PUSH_E((int)A_NOMAT(*new_desc,ptsdes) << 1);
			      break;
			    case TY_MATCH:
                              DBUG_PRINT("MSG", ("TY_MATCH"));
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   PUSH_E(heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,guard));
				 new_heap = R_MATCH(*new_desc,guard);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,guard) = NULL;
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   PUSH_E(heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,body));
				 new_heap = R_MATCH(*new_desc,body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,body) = NULL;
			      if ((heap_elems = DO_RECEIVE())>= 0)
				{GET_HEAP(heap_elems+1,A_MATCH(*new_desc,code));
				 new_heap = R_MATCH(*new_desc,code);
				 *new_heap++ = heap_elems;
				 for (heap_counter = 0;heap_counter < heap_elems; heap_counter++)
				   {rec_data = DO_RECEIVE();
				    if (rec_data == PM_DESC_MARKER)
				      PUSH_E(((int)new_heap++) << 1);
				    else
				      *new_heap++ = rec_data;}}
			      else
				L_MATCH(*new_desc,code) = NULL;
			      break;
			    case TY_LREC:
                              DBUG_PRINT("MSG", ("TY_LREC"));
			      L_LREC(*new_desc,nfuncs) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      PUSH_E(heap_elems);
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,namelist));
			      new_heap = R_LREC(*new_desc,namelist);
			      *new_heap = heap_elems;
			      PUSH_E(((int)(new_heap + heap_elems) << 1) | 1);
			      PUSH_E(heap_elems = R_LREC(*new_desc,nfuncs));
#if WITHTILDE
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,ptdv));
#else
			      GET_HEAP(heap_elems,A_LREC(*new_desc,ptdv));
#endif
			      new_heap = R_LREC(*new_desc,ptdv);
#if WITHTILDE
			      *new_heap++ = R_LREC(*new_desc,nfuncs);
#endif
			      PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_LREC_IND:
                              DBUG_PRINT("MSG", ("TY_LREC_IND"));
			      L_LREC_IND(*new_desc,index) = DO_RECEIVE();
			      PUSH_E((int)A_LREC_IND(*new_desc,ptdd) << 1);
			      PUSH_E((int)A_LREC_IND(*new_desc,ptf) << 1);
			      break;
			    case TY_LREC_ARGS:
                              DBUG_PRINT("MSG", ("TY_LREC_ARGS"));
			      L_LREC_ARGS(*new_desc,nargs) = DO_RECEIVE();
			      PUSH_E((int)A_LREC_ARGS(*new_desc,ptdd) << 1);
			      PUSH_E(heap_elems = R_LREC_ARGS(*new_desc,nargs));
			      GET_HEAP(heap_elems,A_LREC_ARGS(*new_desc,ptdv));
			      new_heap = R_LREC_ARGS(*new_desc,ptdv);
			      PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;}
			 break;
		       case C_FUNC:
                         DBUG_PRINT("MSG", ("C_FUNC"));
		       case C_CONS:
                         DBUG_PRINT("MSG", ("C_CONS ***"));
			 switch(new_type)
			   {case TY_CLOS:
                              DBUG_PRINT("MSG", ("TY_CLOS"));
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,args),L_CLOS(*new_desc,nargs));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,ftype),L_CLOS(*new_desc,nfv));
#else
			      L_CLOS(*new_desc,ftype) = DO_RECEIVE();
#endif
#if WITHTILDE
                              PUSH_E(heap_elems = R_CLOS(*new_desc,args) + R_CLOS(*new_desc,nfv) + 1);
#else
			      PUSH_E(heap_elems = R_CLOS(*new_desc,args) + 1);
#endif
			      GET_HEAP(heap_elems,A_CLOS(*new_desc,pta));
			      new_heap = R_CLOS(*new_desc,pta);
			      PUSH_E(((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_COMB:
                              DBUG_PRINT("MSG", ("TY_COMB"));
			      RECEIVE_2_SHORTS(L_COMB(*new_desc,args),L_COMB(*new_desc,nargs));
			      PUSH_E((int)A_COMB(*new_desc,ptd) << 1);
#if !STORE
			      L_COMB(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
#else /* !STORE */
                              code_received = DO_RECEIVE();   /* index of code-vector */
                              DBUG_PRINT("MSG", ("received code-vector-index %d",code_received));

			      if (code_received < next_code_vector)
                              {
                                SET_CODE_START_I(code_received);  /* get start of this vector */
				L_COMB(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
                                DBUG_PRINT("MSG", ("relative ptc %d",R_COMB(*new_desc,ptc) - GET_CODE_START()));
                              }
			      else if (code_received == next_code_vector)
				{ pptc = A_COMB(*new_desc,ptc); 
				  goto receive_code;}
#endif /* !STORE */
			      break;
			    case TY_CONDI:
                              DBUG_PRINT("MSG", ("TY_CONDI"));
			      RECEIVE_2_SHORTS(L_CONDI(*new_desc,args),L_CONDI(*new_desc,nargs));
			      PUSH_E((int)A_CONDI(*new_desc,ptd) << 1);
#if !STORE
			      L_CONDI(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
#else /* !STORE */
                              code_received = DO_RECEIVE();   /* index of code-vector */
                              DBUG_PRINT("MSG", ("received code-vector-index %d",code_received));

			      if (code_received < next_code_vector)
                              {
                                SET_CODE_START_I(code_received);  /* get start of this vector */
				L_CONDI(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
                                DBUG_PRINT("MSG", ("relative ptc %d",R_CONDI(*new_desc,ptc) - GET_CODE_START()));
                              }
			      else if (code_received == next_code_vector)
				{ pptc = A_CONDI(*new_desc,ptc); 
				  goto receive_code;}
#endif /* !STORE */
			      break;
			    case TY_CONS:
                              DBUG_PRINT("MSG", ("TY_CONS"));
			      PUSH_E((int)A_CONS(*new_desc,hd) << 1);
			      PUSH_E((int)A_CONS(*new_desc,tl) << 1);
			      break;
			    case TY_CASE:
                              DBUG_PRINT("MSG", ("TY_CASE"));
			    case TY_WHEN:
                              DBUG_PRINT("MSG", ("TY_WHEN"));
			    case TY_GUARD:
                              DBUG_PRINT("MSG", ("TY_GUARD"));
			    case TY_BODY:
                              DBUG_PRINT("MSG", ("TY_BODY ***"));
			      RECEIVE_2_SHORTS(L_CASE(*new_desc,args),L_CASE(*new_desc,nargs));
			      PUSH_E((int)A_CASE(*new_desc,ptd) << 1);
                              heap_elems = DO_RECEIVE();
                              if (T_INT(heap_elems)) { /* it's NO code-pointer !! */
                                DBUG_PRINT("MSG", ("no code pointer"));
                                L_CASE(*new_desc,ptc) = heap_elems;
                                break;
                                }
                              else if (heap_elems < 10) {
                                DBUG_PRINT("MSG", ("quite small for a code pointer, isn't it? %d",heap_elems));
                                L_CASE(*new_desc,ptc) = heap_elems;
                                break;
                                }
                              DBUG_PRINT("MSG", ("code ..."));
#if !STORE
			      L_CASE(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
			      break;
#else /* !STORE */
                              code_received = DO_RECEIVE();   /* index of code-vector */
                              DBUG_PRINT("MSG", ("received code-vector-index %d",code_received));

			      if (code_received < next_code_vector)
                              {
                                SET_CODE_START_I(code_received);  /* get start of this vector */
				L_CASE(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
                                DBUG_PRINT("MSG", ("relative ptc %d",R_CASE(*new_desc,ptc) - GET_CODE_START()));
                              }
			      else if (code_received == next_code_vector)
				{ pptc = A_CASE(*new_desc,ptc); 
                                 
			       receive_code:
                                 DBUG_PRINT("MSG", ("receiving Code..."));

                                 /* new code-vector, add to list */
                                 if ((new_code = malloc(sizeof(SENT_CODE_VECTOR))) != NULL)
                                 {
                                   heap_elems = DO_RECEIVE();
                                   if ((new_code->start = (INSTR*)newbuff(heap_elems+3)) != NULL)
                                   {
                                     new_code->length = (int)heap_elems;
                                     new_code->next = NULL;
                                     *sent_code_last = new_code;
                                     sent_code_last = &(new_code->next);
                                     next_code_vector++;
                                   }
                                   else
                                    post_mortem("PREFIX(receive_result): no heap for codevector");
                                 }
                                 else
                                  post_mortem("PREFIX(receive_result): malloc for ptr_codedesc failed");
                                
				 code_address = new_code->start;
                                 *pptc = code_address + DO_RECEIVE(); /* ptc, relative to start of code-vector */
                                 DBUG_PRINT("MSG", ("relative ptc %d",(*pptc)-code_address));

				 new_heap = code_address;
                                 *new_heap++ = I_TABSTART;                            /* keep format similar to */
                                 *new_heap++ = (INSTR)(code_address + heap_elems) ;   /* original code-vector */

				 for (heap_counter = 2;heap_counter < heap_elems; heap_counter ++)
				   {*new_heap = DO_RECEIVE();
				    rec_instr = instr_tab[*new_heap++];
				    switch(rec_instr.paramtype)
				      {case NUM_PAR:
					 /* Here is our special case again, see msg_send */
					 if ((rec_instr.instruction == I_DIST) || (rec_instr.instruction == I_DISTB))
					   {*(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					    *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					    *(new_heap++) = DO_RECEIVE();
					    *(new_heap++) = DO_RECEIVE();
#if WITHTILDE
					    *(new_heap++) = DO_RECEIVE();
					    *(new_heap++) = DO_RECEIVE();
					    heap_counter += 6;
#else
					    heap_counter += 4;
#endif
					    break;}

                                            /* special treatment for some pattern matching */
                                            /* instructions */

                                            if ((rec_instr.instruction >= I_APPEND) && (rec_instr.instruction <= I_MATCHDIGIT)) {
                                              switch(rec_instr.instruction) {
                                                case I_ATEND:
                                                case I_ATSTART:
                                                  DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));
                                                  *(new_heap++) = DO_RECEIVE();
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  heap_counter += 2;
                                                  break;
                                                case I_MATCHARB:
                                                case I_MATCHARBS:
                                                case I_MATCHBOOL:
                                                case I_MATCHC:
                                                case I_MATCHIN:
                                                case I_MATCHINT:
                                                case I_MATCHLIST:
                                                case I_MATCHPRIM:
                                                case I_MATCHSTR:
                                                  DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));
                                                  *(new_heap++) = DO_RECEIVE();
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  heap_counter += 3;
                                                  break;
                                                case I_MATCHREAL:
                                                case I_MATCHDIGIT:
                                                  DBUG_PRINT("MSG", ("converting PM instruction %i", rec_instr.instruction));           
                                                  PUSH_E((int)(new_heap++) << 1);
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  heap_counter += 3;
                                                  break;
                                                case I_TGUARD:
                                                  DBUG_PRINT("MSG", ("converting TGUARD PM instruction..."));
                                                  *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
                                                  PUSH_E((int)(new_heap++) << 1);
                                                  *(new_heap++) = DO_RECEIVE();
                                                  *(new_heap++) = DO_RECEIVE();
                                                  *(new_heap++) = DO_RECEIVE();
                                                  heap_counter += 5;
                                                  break;
                                                default:
                                                  for (param_counter = 0;param_counter < rec_instr.nparams; param_counter++)
                                                    {*(new_heap++) = DO_RECEIVE();
                                                      heap_counter ++;}
                                                  break;
                                                }
                                              break;
                                              }

				       case ATOM_PAR:
				       case FUNC_PAR:
					 for (param_counter = 0;param_counter < rec_instr.nparams;param_counter++)
					   {*(new_heap++) = DO_RECEIVE();
					    heap_counter ++;}
					 break;
				       case DESC_PAR:
					 PUSH_E((int)(new_heap++) << 1);
					 heap_counter++;
					 break;
				       case ADDR_PAR:
					 *(new_heap++) = (T_HEAPELEM)(code_address + DO_RECEIVE());
					 heap_counter++;
					 break;
				       }                                  /* switch */
				  }                                       /* for */
				 *new_heap = I_SYMTAB;
				 code_received = 1;
				 SET_CODE_START(code_address);
			       }                                          /* else {code not received} */
                               else
                                post_mortem("receive_code: missing code-vector?");
			      break;
#endif /* !STORE */
			    }                                             /* switch (new_type) */
			 break;
		       }                                                  /* switch (new_class) */
		    received_list[received_index++] = new_desc;
		    if (received_index == rec_list_size)
		      {rec_list_size *= 2;
		       received_list = realloc(received_list,rec_list_size * sizeof(int));}
		  }                                                       /* else {descriptor not received yet} */
		}                                                       /* else {null pointer} */
	     }                                                          /* else {not in static heap} */
	  }                                                             /* else {extended pointer on stack} */
  }                                                                /* while */
#if MSG_CHKSUM
 rec_data = DO_RECEIVE();
 if (rec_data*2 != chksum)
   post_mortem("Checksum error!");
#endif
 rec_data = DO_RECEIVE();
 if (rec_data != END_MARKER)
 {
   DBUG_PRINT("MSG",("found %d instead of END_MARKER",rec_data));
   post_mortem("END_MARKER expected but not found");
 }
 free(received_list);
 free(buffer);

 DBUG_VOID_RETURN;
}

#if !STORE

/***********************************************************/
/*                                                         */
/* wait_for_ncube     waits for the NCUBE_READY message    */
/*                    from 2^dim nodes                     */
/*                    NEW: sends absolute path for the     */
/*                    tickets-configuration-file           */
/*                                                         */
/***********************************************************/

void wait_for_ncube(dim)
int dim;
{int counter = 0,src,type,dest;
 char error_message[256];
#if D_PVM
 int pvm_length, pvm_bufid, temp, i, temp_tids[128]; 
#endif /* D_PVM */

 DBUG_ENTER ("wait_for_ncube");

/* don't forget to send the absolute path for the red.tickets-file ! */

#if nCUBE
 DBUG_PRINT ("wait_for_ncube", ("Sending ticket_file_name !"));
 dest = 0xffff;
 type = 1;
 nwrite (cube,ticket_file,strlen(ticket_file)+1,dest,type);
 DBUG_PRINT ("wait_for_ncube", ("It's away !"));
#endif

/* check all nodes */

 while(counter < (1 << dim))
   {

#if D_PVM
    /* our slaves must know about the pvm message encoding and who's slave no. 0 */
    /* so we must send a message to the slaves first... */

    pvm_initsend(pvmcoding);
    temp=SWAP(pvmcoding);
    pvm_pkint(&temp, 1, 1);
    temp=SWAP(cube_dim);
    pvm_pkint(&temp, 1, 1);
    temp=SWAP(pvmspawn_ntask);
    pvm_pkint(&temp, 1, 1);
    for (i=0; i<pvmspawn_ntask; i++)
      temp_tids[i]=SWAP(pvm_tids[i]);
    pvm_pkint(temp_tids, pvmspawn_ntask, 1);
    pvm_pkstr(ticket_file);
    pvm_send(pvm_tids[counter],1);
    DBUG_PRINT ("PVM", ("checking node %d !", counter));

#endif /* D_PVM */

    src = type = -1;
#if nCUBE
    nread(cube,error_message,256,(long *)&src,(int *) &type);
#endif /* nCUBE */
#if D_PVM
    pvm_bufid = pvm_recv (pvm_tids[counter],-1);
    pvm_bufinfo (pvm_bufid, &pvm_length, &type, &src);

    DBUG_PRINT ("PVM", ("ready signaled by node t%x", pvm_tids[counter]));
#endif /* D_PVM */

    if (type == MT_NCUBE_READY)
      counter++;
    if (type == MT_NCUBE_FAILED)
      {
#if nCUBE
       nclose(cube);
       cube = NULL;
#endif /* nCUBE */
#if D_PVM
       pvm_upkstr(error_message);
       /* terminate Slaves ! */
       exit_slaves();
#endif /* D_PVM */
       post_mortem(error_message);}}
 DBUG_VOID_RETURN;
}

#if nCUBE

/***********************************************************/
/*                                                         */
/* exit_ncube     closes the connection to the nCube       */
/*                                                         */
/***********************************************************/

void exit_ncube()
{
 DBUG_ENTER ("exit_ncube");

 nwrite(cube,NULL,0,0xffff,MT_TERMINATE);
 nclose(cube);
 cube=NULL;
 
 DBUG_VOID_RETURN;
}

#endif /* nCUBE */

#if D_PVM

/***********************************************************/
/*                                                         */
/* exit_slaves   terminates the pvm slaves                 */
/*                                                         */
/***********************************************************/

void exit_slaves()
{
 int i;

 DBUG_ENTER ("exit_slaves");

 if (pvm_numt) {
   pvm_initsend(pvmcoding);
   pvm_mcast(pvm_tids, pvmspawn_ntask, MT_TERMINATE);
   pvm_numt=0;
 
   sleep(1);

   for (i=0; i<pvmspawn_ntask; i++)
     pvm_kill(pvm_tids[i]);
   }

 DBUG_VOID_RETURN;
}

#endif /* D_PVM */

/***********************************************************/
/*                                                         */
/* flush_queue    flushes the message queue                */
/*                                                         */
/***********************************************************/

void flush_queue()
{
#if D_PVM
 int pvm_bufid;
#else /* D_PVM */
 int src = -1, type = -1;
#endif

  DBUG_ENTER ("flush_queue");

 /* char *buffer; */

#if nCUBE
 while (ntest(cube,(long *)&src,&type) >= 0)
   {nread(cube,NULL,0,(long *)&src,&type);
    src = type = -1;}
#endif /* nCUBE */

#if D_PVM
  while ((pvm_bufid = pvm_probe (-1, -1)) > 0)
    pvm_recv (-1,-1);
#endif /* D_PVM */

  DBUG_VOID_RETURN;
}


#if D_MESS

#ifdef M_OLD_MERGING

/****************************************************************************/

/* OLD MERGING */

/* FOR ASCII FILES ONLY */

/* NODES SEND THEIR STUFF TO THE HOST, THE HOST MERGES */

/***************************************************************************/
/*                                                                         */
/* function : get_em_merged                                                */
/*                                                                         */
/* work     : merges measure files                                         */
/*            pure ascii file merger                                       */
/*                                                                         */
/* last change      :                                                      */
/*                                                                         */
/*       30.03.1993 RS                                                     */
/*                                                                         */
/***************************************************************************/

void get_em_merged()

{
  time_t now;
  struct tm *date;
  FILE *init_file;   /* INIT file */
  FILE *mess_host;   /* measure file */
  char line[120];
  char mess_ncube_lines[NCUBE_MAXPROCS][120];   /* next lines */
  int used_nodes = 1 << cube_dim;             /* size of sub(n)cube */
  int i, topvalue, counter[NCUBE_MAXPROCS];
  double toptime = 0.0;
  double node_values[NCUBE_MAXPROCS];
  int flags[NCUBE_MAXPROCS];
  int superflag = 1;
  int rec_type = MT_SEND_MEASURE;      
  char *d_mess_buff[NCUBE_MAXPROCS];
  
  double zeit1, zeit2;
  int gesamt = 0;

  DBUG_ENTER ("get_em_merged");

  DBUG_PRINT ("MERGE", ("Merging %d files...", used_nodes));

  zeit1 =  host_time();                     

  now = time(NULL);      /* getting time for filename (oops, nCUBE-time !?!?) */
  date = localtime(&now);

  sprintf (mess_file, "/tmp/ncube_%s_%d_%02d%02d%02d%02d%02d_uid%d.ms", _errmes, 1 << cube_dim,
  date->tm_mday, date->tm_mon, date->tm_year, date->tm_hour, date->tm_min, getuid());

  if ((mess_host = fopen (mess_file, "w")) == NULL)
    post_mortem ("merger: cannot open host-measure-file");

  d_write_header(mess_host);     /* generating header... */

/******* inserting INIT-file  *****/

  if ((init_file = fopen ("INIT", "r")) == NULL)
    post_mortem ("merger: cannot open host-INIT-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merger: cannot close host-INIT-file");

  fprintf (mess_host, "---\n");

/****** inserting ticket-file *****/

  if ((init_file = fopen (ticket_file, "r")) == NULL)
    post_mortem ("merger: cannot open host-red.tickets-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merger: cannot close host-red.tickets-file");

  fprintf (mess_host, "---\n");

   DBUG_PRINT ("MERGE", ("jetzt gehts los Buam, used_nodes=%d", used_nodes)); 

  for (i = 0; i < used_nodes; i++) {
    if ((d_mess_buff[i] = malloc(used_nodes * D_MESS_MAXBUF)) == NULL)
      post_mortem ("get_em_merged: cannot allocate measure-merge-buffer !");

/* printf ("Hol neues Segment von Prozessor %d !\n", i);
host_flush(); */

    nwrite (cube, NULL, 0, i, MT_OPEN_MEASURE);
    counter[i]=0;
    mess_ncube_lines[i][0] = '\0';

     DBUG_PRINT ("MERGE", ("Signal MT_OPEN_MEASURE gesendet !")); 

    nread (cube, d_mess_buff[i], D_MESS_MAXBUF, &i, &rec_type);

    DBUG_PRINT ("MERGE", ("Signal MT_SEND_MEASURE erhalten von %d, %d Bytes erhalten !", i, strlen(d_mess_buff[i]))); 

    if (strlen(d_mess_buff[i]) == 1) {
      flags[i] = 0;
      
/* printf ("Prozessor %d meint, seine Messdatei waer zu Ende !\n", i);
host_flush(); */
 
/*       DBUG_PRINT ("MERGE", ("Node %d gibt nix zurueck.", i)); */
      }
    else {
      flags[i] = 1;
      sscanf (d_mess_buff[i]+counter[i], "%[^\n]", mess_ncube_lines[i]);
      sscanf (mess_ncube_lines[i], "%lf", &node_values[i]);
      counter[i] += strlen(mess_ncube_lines[i]) + 1;
       DBUG_PRINT ("MERGE", ("Node %d gibt zurueck: %s, Zeitmarke: %.lf", i, mess_ncube_lines[i], node_values[i]));  
      }
    }

/*   DBUG_PRINT ("MERGE", ("Alle Nodes OK !")); */
 
/*   DBUG_PRINT ("MERGE", ("Starting Merging !")); */

/* attention !!!!!!!!! merging */

  while (superflag) {       /* there are still open files */
    superflag = 0;

/*     DBUG_PRINT ("MERGE", ("Next loop...")); */

    /* now determine next input-line (next measure file nr is topvalue) */
    /* check all time marks */

    topvalue = -1;     /* initialize next node-line to be wrote to host */

    for (i = 0; i < used_nodes; i++)
      if (flags[i]) {                   /* file not yet empty */
/*         DBUG_PRINT ("MERGE", ("Yups: working on file %d !", i)); */
        if (!superflag) {               /* initialize comparison */
          toptime = node_values[i];
          superflag = 1;
          }
        if (toptime >= node_values[i]) { /* compare values */
          toptime = node_values[i];
          topvalue = i;
          }
        }

    /* now write best time-mark and line to host */

    if (topvalue >= 0) {
/*       DBUG_PRINT ("MERGE", ("At last: File Nr. %d taken !", topvalue)); */
/*      supercounter++;
      if (supercounter > 10000) {
        supercounter = 0;
        printf (" Another 10000 measure lines...\n");
        host_flush();
        } */

      /* write line back to host */
      fprintf (mess_host, "%s\n", mess_ncube_lines[topvalue]);
      gesamt += strlen(mess_ncube_lines[topvalue]);

      if (d_mess_buff[topvalue][counter[topvalue]] == '\n')  /* end of File */
{
        flags[topvalue] = 0;

/* printf ("Prozessor %d meint, seine Messdatei waer zu Ende !\n", topvalue);
host_flush();  */

}
      else if (d_mess_buff[topvalue][counter[topvalue]] == '\0') {  /* get new block */

/* printf ("Hol neues Segment von Prozessor %d !\n", topvalue);
host_flush(); */

        nwrite (cube, NULL, 0, topvalue, MT_SEND_MEASURE);
        nread (cube, d_mess_buff[topvalue], D_MESS_MAXBUF, &topvalue, &rec_type);
        counter[topvalue] = 0;
        /* check wheter file empty */
        if (d_mess_buff[topvalue][0] == '\n') {

/* printf ("Diesmal hab ich Dich Nr. %d !\n", topvalue);
host_flush(); */

/* printf ("Prozessor %d meint, seine Messdatei waer zu Ende !\n", topvalue);
host_flush(); */

          flags[topvalue] = 0;            
          }
        else {
          sscanf (d_mess_buff[topvalue]+counter[topvalue], "%[^\n]", mess_ncube_lines[topvalue]);
          sscanf (mess_ncube_lines[topvalue], "%lf", &node_values[topvalue]);
          counter[topvalue] += strlen(mess_ncube_lines[topvalue]) + 1;
           DBUG_PRINT ("MERGE", ("Node %d gibt zurueck: %s, Zeitmarke: %.lf", topvalue, mess_ncube_lines[topvalue], node_values[topvalue])); 
          }
        }
      else { /* just next line */
        sscanf (d_mess_buff[topvalue]+counter[topvalue], "%[^\n]", mess_ncube_lines[topvalue]);
        sscanf (mess_ncube_lines[topvalue], "%lf", &node_values[topvalue]);
        counter[topvalue] += strlen(mess_ncube_lines[topvalue]) + 1;
/*         DBUG_PRINT ("MERGE", ("Naechste Zeile von %d ist : %s", topvalue, mess_ncube_lines[topvalue])); */
        }
      }
    }

/* merging finished, puuh.... */

  DBUG_PRINT ("MERGE", ("merging finished !!"));

  zeit2 = host_time();

  clearscreen();
  host_flush();
  printf (" Merge-Time: %4.2f, Bytes merged: %ld, Rate: %4.2f KB/s\n", (zeit2 - zeit1), gesamt, (gesamt/(1000.0*(zeit2 - zeit1))));
  host_flush();

  if (fclose (mess_host) == EOF)
    post_mortem ("merger: cannot close host-measure-file");
  DBUG_VOID_RETURN;
}

#endif /* M_OLD_MERGING */

#if !D_PVM

/***************************************************************************/
/*                                                                         */
/* function : m_merging                                                    */
/*                                                                         */
/* work     : merges measure files distributed on node-processors          */
/*            write header, initialize nodes, wait for finish              */
/*                                                                         */
/* last change      :                                                      */
/*                                                                         */
/*       18.05.1993 RS                                                     */
/*                                                                         */
/***************************************************************************/

void m_merging()

{
  time_t now;
  struct tm *date;
  FILE *init_file;   /* INIT file */
  FILE *mess_host;   /* measure file */
  int used_nodes = 1 << cube_dim;             /* size of sub(n)cube */
  /* int rec_type = MT_SEND_MEASURE; */
  char line[120];
  double zeit1, zeit2;
  long gesamt = 0;
  int buffer, src = -1, type = MT_CLOSE_MEASURE;

  DBUG_ENTER ("m_merging");

  DBUG_PRINT ("MERGE", ("Merging %d files...", used_nodes));

  zeit1 =  host_time();      /* include problems, so function here */

  now = time(NULL);      /* getting time for filename (oops, nCUBE-time !?!?) */
  date = localtime(&now);

  sprintf (mess_file, "/tmp/ncube_%s_%d_%02d%02d%02d%02d%02d_uid%d.ms", _errmes, 1 << cube_dim,
  date->tm_mday, date->tm_mon, date->tm_year, date->tm_hour, date->tm_min, getuid());

  if ((mess_host = fopen (mess_file, "w")) == NULL)
    post_mortem ("merge: cannot open host-measure-file");

  d_write_header(mess_host);     /* generating header... */

/******* inserting INIT-file  *****/

  if ((init_file = fopen (D_MESS_INIT, "r")) == NULL)
    post_mortem ("merge: cannot open host-INIT-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merge: cannot close host-INIT-file");

  fprintf (mess_host, "---\n");

/****** inserting ticket-file *****/

  if ((init_file = fopen (ticket_file, "r")) == NULL)
    post_mortem ("merger: cannot open host-red.tickets-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merger: cannot close host-red.tickets-file");

  fprintf (mess_host, "---\n");

/***** close measure file *****/

  if (fclose (mess_host) == EOF)
    post_mortem ("merger: cannot close host-measure-file");

/***** initialize nodes and start merging *****/

  m_merge_initialize();

/***** wait for merging finish *****/
 
  DBUG_PRINT ("MERGE", ("Waiting for termination signal from nCUBE."));
  src = -1; type = -1;

  nread(cube,(char *)&buffer, 0, (int *)&src, &type);

  if (type == MT_POST_MORTEM)
    post_mortem ((char *)&buffer);
 
  if (type != MT_CLOSE_MEASURE)
    post_mortem ("Merging could not be normally terminated...");
   
  DBUG_PRINT ("MERGE", ("Termination signal received !"));

/***** print merging time usage *****/

  DBUG_PRINT ("MERGE", ("merging finished !!"));

  /* free send-measure buffer */

  free (d_mess_buff);

  /* take merging-time */

  zeit2 = host_time();

  gesamt = host_stat(mess_file);

  /* clearscreen(); */
  if (m_ackno) {
    host_flush();
    printf (" merging-time: %4.2f s, length of measurement file: %ld, %4.2f kb/s\n", (zeit2 - zeit1), gesamt, (gesamt/(1000.0*(zeit2 - zeit1))));
    host_flush();
    }

  DBUG_VOID_RETURN;
}

/***************************************************************************/
/*                                                                         */
/* function : m_merge_initialize                                           */
/*                                                                         */
/* work     : sends initializing-messages to node-processors               */
/*            (algorithm or setup-file is recommended)                     */
/*                                                                         */
/* last change      :                                                      */
/*                     14.5.1993                                           */
/*                                                                         */
/***************************************************************************/

void m_merge_initialize()

{
 int i, src, buffer, counter = 0;
 int node, k, j, l, n;
 int type = MT_CLOSE_MEASURE;
 int m_lower, m_upper, m_actual;
 
 DBUG_ENTER ("m_merge_initialize");

 /* read from a file or specify algorithm */

 /* AN ALLE: MESSUNGEN AUSSCHALTEN UND MESSDATEI SCHLIESSEN ! */

 DBUG_PRINT ("MERGE", ("Schicke MT_CLOSE_MEASURE an alle nodes !"));

 /* no broadcast, node 0 will count file-lens */

 for (i = ((1 << cube_dim)-1); i >= 0; i--) {
   nwrite(cube,(char *)&i, 0, i, type);
   DBUG_PRINT ("MERGE", ("Schicke MT_CLOSE_MEASURE an node %d !", i)); }

 /* read file len from node 0 */

 src = 0;
 type = MT_CLOSE_MEASURE;

 nread(cube,(char *)&buffer, 100, (int *)&src, &type);

 DBUG_PRINT ("MERGE", ("es sind %d bytes !", SWAP(buffer)));

 if (m_ackno) {
   printf (" merging %d bytes ...\n", SWAP(buffer));
   host_flush(); 
   }

 /* now finally start merging ! */

 type = MT_OPEN_MEASURE;

 if (cube_dim == 0) {
   DBUG_PRINT ("MERGE", ("Initializing Merging for dim 0."));
   *(((int *) d_mess_buff)+counter++) = SWAP(-1);
   *(((int *) d_mess_buff)+counter++) = SWAP(1);
   *(((int *) d_mess_buff)+counter++) = SWAP(0);
   *(((int *) d_mess_buff)+counter++) = SWAP(0);
   strcpy ((char *)(((int *) d_mess_buff)+counter), mess_file);
   counter = counter*sizeof(int) + strlen(mess_file) + 1;
   nwrite (cube, d_mess_buff, counter, 0, type);

   DBUG_PRINT ("MERGE", ("initializing node %d: %d, %d, %d, %d + filename", 0, -1, 1, 0, 0));
   }
 else {

/* stupid initialization: 

   DBUG_PRINT ("MERGE", ("initializing node %d: 1 1 0 0 !", 0));
   *(((int *) d_mess_buff)+counter++) = SWAP(1);   initialize first node 
   *(((int *) d_mess_buff)+counter++) = SWAP(1);
   *(((int *) d_mess_buff)+counter++) = SWAP(0);
   *(((int *) d_mess_buff)+counter++) = SWAP(0);
   nwrite (cube, d_mess_buff, counter * sizeof(int), 0, type);
   counter = 0;

   for (i = 1; i < ((1 << cube_dim)-1); i++) {
     *(((int *) d_mess_buff)+counter++) = SWAP(i+1);
     *(((int *) d_mess_buff)+counter++) = SWAP(1);
     *(((int *) d_mess_buff)+counter++) = SWAP(1);
     *(((int *) d_mess_buff)+counter++) = SWAP(i);
     *(((int *) d_mess_buff)+counter++) = SWAP(i-1);
     DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d", i, i+1, 1, 1, i, i-1));
     nwrite (cube, d_mess_buff, counter * sizeof(int), i, type);
     counter = 0;
     }

   DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d", ((1 << cube_dim)-1), -1, 1, 1, ((1 << cube_dim)-1), ((1 << cube_dim)-2)));
   *(((int *) d_mess_buff)+counter++) = SWAP(-1);
   *(((int *) d_mess_buff)+counter++) = SWAP(1);
   *(((int *) d_mess_buff)+counter++) = SWAP(1);
   *(((int *) d_mess_buff)+counter++) = SWAP(((1 << cube_dim)-1));
   *(((int *) d_mess_buff)+counter++) = SWAP(((1 << cube_dim)-1)-1);
   strcpy ((char *)(((int *) d_mess_buff)+counter), mess_file);
   counter = counter*sizeof(int) + strlen(mess_file) + 1;
   nwrite (cube, d_mess_buff, counter, (1 << cube_dim)-1, type);
*/

/******************* binary tree distributed merging ***********************/

  l = (1 << cube_dim);  /* dimension */
  node = 0;

  if (l == 2) {
    counter = 0;
    *(((int *) d_mess_buff)+counter++) = SWAP(-1);
    *(((int *) d_mess_buff)+counter++) = SWAP(2); 
    *(((int *) d_mess_buff)+counter++) = SWAP(0);
    *(((int *) d_mess_buff)+counter++) = SWAP(0);
    *(((int *) d_mess_buff)+counter++) = SWAP(1);
    strcpy ((char *)(((int *) d_mess_buff)+counter), mess_file);
    counter = counter*sizeof(int) + strlen(mess_file) + 1;
    DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d + filename", 0, -1, 2, 0, 0, 1));
    l = 0;
    nwrite (cube, d_mess_buff, counter, l, type);
    }
  else {
  
  for (k = 0; k < (l/2); k++) {  /* initialize k file nodes */
    counter = 0;
    *(((int *) d_mess_buff)+counter++) = SWAP((l/2) + (k / 2) );
    *(((int *) d_mess_buff)+counter++) = SWAP(2);
    *(((int *) d_mess_buff)+counter++) = SWAP(0);
    *(((int *) d_mess_buff)+counter++) = SWAP(k*2);
    *(((int *) d_mess_buff)+counter++) = SWAP((k*2)+1);
    nwrite (cube, d_mess_buff, counter * sizeof(int), k, type);
    DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d", k, (l/2) + k/2, 2, 0, k*2, (k*2)+1));
   }
  m_lower = 0;
  m_upper = (l/2)+(l/4);
  m_actual = (l/2);
  j = l/4;
  node = l/2;
  n = 0;    /* start of old nodes */
  for (k = j; k >= 2; k /= 2) {     /* initialize channel merging nodes */
    for (l = 0; l < k; l++) {       /* initialize nodes */
      counter = 0;
      *(((int *) d_mess_buff)+counter++) = SWAP(m_upper+l/2);
      *(((int *) d_mess_buff)+counter++) = SWAP(0);
      *(((int *) d_mess_buff)+counter++) = SWAP(2);
      *(((int *) d_mess_buff)+counter++) = SWAP(m_lower++);
      *(((int *) d_mess_buff)+counter++) = SWAP(m_lower++);
      nwrite (cube, d_mess_buff, counter * sizeof(int), m_actual, type);
      DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d", m_actual, m_upper+l/2, 0, 2, m_lower-2, m_lower-1));
      m_actual++;
      }
    m_upper = m_upper + k/2;
    }

  /* last node */
 
  counter = 0;

  *(((int *) d_mess_buff)+counter++) = SWAP(-1);
  *(((int *) d_mess_buff)+counter++) = SWAP(0);
  *(((int *) d_mess_buff)+counter++) = SWAP(2);
  *(((int *) d_mess_buff)+counter++) = SWAP(m_lower++);
  *(((int *) d_mess_buff)+counter++) = SWAP(m_lower++);
  strcpy ((char *)(((int *) d_mess_buff)+counter), mess_file);
  counter = counter*sizeof(int) + strlen(mess_file) + 1;
  nwrite (cube, d_mess_buff, counter, m_actual, type);
  DBUG_PRINT ("MERGE", ("initializing node %d: %d %d %d %d %d + filename", m_actual, -1, 0, 2, m_lower-2, m_lower-1)); 
  } 
 }

 DBUG_VOID_RETURN;
}

#endif /* !D_PVM */

/* here starts the SUPER Merging ! */

/***************************************************************************/
/*                                                                         */
/* function : m_super_merging                                              */
/*                                                                         */
/* work     : merges measure files on host                                 */
/*            write header, initialize nodes, wait for finish              */
/*                                                                         */
/* last change      :                                                      */
/*                                                                         */
/*       26.05.1993 RS                                                     */
/*                                                                         */
/***************************************************************************/

void m_super_merging()

{
  time_t now;
  struct tm *date;
  FILE *init_file;   /* INIT file */
  FILE *mess_host;   /* measure file */
#ifndef DBUG_OFF
  int used_nodes = 1 << cube_dim;             /* size of sub(n)cube */
#endif
  char line[120];
  double zeit1, zeit2;
  /* long gesamt = 0; */
  int buffer, src = -1, type = MT_CLOSE_MEASURE;
  int i,j,/*k,*/ m_super_value;
  int m_dirty_trick = 0;
  char m_standard[255] = "/tmp/"; 

/* the dirty trick is to generate the measurement files on the host, so 
   there's no transport of files from nCUBE to host necessary, that's 
   a great speedup in merging time ! */

  DBUG_ENTER ("m_super_merging");

#if (!D_PVM || D_PVM_NCUBE)
  if (strncmp(m_mesfilepath, "//d02/", 6) != 0) {
    DBUG_PRINT ("SMERGE", ("dirty trick used !"));
    m_dirty_trick=1;
    strcpy (m_standard, m_mesfilepath);
    }
#endif

  DBUG_PRINT ("S_MERGE", ("Merging %d files...", used_nodes));

  zeit1 =  host_time();      /* include problems, so function here */

  now = time(NULL);      /* getting time for filename (oops, nCUBE-time !?!?) */
  date = localtime(&now);

  DBUG_PRINT ("S_MERGE", ("Preparing filename..."));

  DBUG_PRINT ("S_MERGE", ("_errmes: %s", _errmes));

#if D_BENCH
  sprintf (mess_file, "/tmp/benchmark.ms");
#else
  sprintf (mess_file, "/tmp/ncube_%s_%d_%02d%02d%02d%02d%02d_uid%d.ms", _errmes, 1 << cube_dim,
  date->tm_mday, date->tm_mon, date->tm_year, date->tm_hour, date->tm_min, getuid());
#endif

  DBUG_PRINT ("S_MERGE", ("Filename: %s", mess_file));

  if ((mess_host = fopen (mess_file, "w")) == NULL)
    post_mortem ("merge: cannot open host-measurement-file");

  d_write_header(mess_host);     /* generating header... */

/******* inserting INIT-file  *****/

  if ((init_file = fopen (D_MESS_INIT, "r")) == NULL)
    post_mortem ("merge: cannot open host-INIT-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merge: cannot close host-INIT-file");

  fprintf (mess_host, "---\n");

/****** inserting ticket-file *****/


  if ((init_file = fopen (ticket_file, "r")) == NULL)
    post_mortem ("merger: cannot open host-red.tickets-file");

  while (fgets(line, 120, init_file) != NULL)
    fputs (line, mess_host);

  if (fclose(init_file) == EOF)
    post_mortem ("merger: cannot close host-red.tickets-file");

  fprintf (mess_host, "---\n");

/***** close measure file *****/

  if (fclose (mess_host) == EOF)
    post_mortem ("merger: cannot close host-measure-file");

/***** initialize nodes and start merging *****/

 /* TO ALL: SWITCH OFF MEASURE ACTIONS */

 DBUG_PRINT ("S_MERGE", ("Schicke MT_CLOSE_MEASURE an alle nodes !"));

 /* no broadcast, node 0 will count file-lengths */

 for (i = ((1 << cube_dim)-1); i >= 0; i--) {
#if nCUBE
   nwrite(cube,(char *)&i, 0, i, type);
#endif
#if D_PVM
   pvm_initsend(pvmcoding);
   pvm_send(pvm_tids[i], type);
#endif
   DBUG_PRINT ("S_MERGE", ("Schicke MT_CLOSE_MEASURE an node %d !", i)); }

 /* read file len from node 0 */

 src = 0;
 type = MT_CLOSE_MEASURE;

#if nCUBE
 nread(cube,(char *)&buffer, 100, (int *)&src, &type);
#endif
#if D_PVM
  pvm_recv(pvm_tids[0], -1);
  pvm_upkint(&buffer, 1, 1);
#endif

 DBUG_PRINT ("S_MERGE", ("es sind %d bytes !", SWAP(buffer)));

 if (m_ackno) {
#if (!D_PVM || D_PVM_NCUBE)
   printf (" merging %d bytes ...\n", SWAP(buffer));
#endif /* (!D_PVM || D_PVM_NCUBE) */
   host_flush();
   printf (" copying measurement files to remote host ... ");
   host_flush();
   }

 /* sending copy-request to nodes */

 if (!m_dirty_trick) {
   buffer = SWAP(0);
   type = MT_REMOTE_MEASURE;
   DBUG_PRINT ("S_MERGE", ("Sending request for copying file nr. 0 !"));
#if nCUBE
   nwrite(cube,(char *)&buffer, 4, 0, type);
#endif
#if D_PVM
   for (i = 0; i < (1 << cube_dim); i++) {
   DBUG_PRINT("S_MERGE", ("sending params to node %d, remote host is %s", i, m_remote_host));
   pvm_initsend(pvmcoding);
   j=SWAP(i);
   pvm_pkint(&j, 1, 1);
   pvm_pkstr(m_remote_host);
   pvm_pkstr(m_remote_dir);
   pvm_send(pvm_tids[i], type);

   /* wait for acknowledge */

   pvm_recv(pvm_tids[i], -1);
   DBUG_PRINT("S_MERGE", ("node %d acknowledged !",i));
   } 
#endif /* D_PVM */
   }
 
#if !D_PVM
 /* wait for acknowledge */

 if (!m_dirty_trick) {
   src = 0; type = MT_CLOSE_MEASURE;
   nread(cube,(char *)&buffer, 100, (int *)&src, &type);
   DBUG_PRINT ("S_MERGE", ("node 0 acknowledged."));
   }

 for (i = 1; i < (1 << cube_dim); i++) {
   if (!m_dirty_trick) {
     buffer = SWAP(i);
     type = MT_REMOTE_MEASURE;
     DBUG_PRINT ("S_MERGE", ("Sending request for copying file nr. %d !", i));
     nwrite(cube,(char *)&buffer, 4, 0, type);
     }
#endif /* !D_PVM */

#if (!D_PVM || D_PVM_NCUBE)
#if D_PVM_NCUBE
 for (i = 1; i <= (1 << cube_dim); i++) {
#endif

   DBUG_PRINT ("S_MERGE", ("now copy file to remote host"));
  
   sprintf (line, "rcp %s%s_uid%ld_%i %s:%s", m_standard, m_mesfilehea, getuid(), i-1, m_remote_host, m_remote_dir);
   DBUG_PRINT ("S_MERGE", ("copy command : %s", line));
   if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }
#if D_PVM_NCUBE
  }
#endif /* D_PVM_NCUBE */
#endif /*(!D_PVM || D_PVM_NCUBE) */

#if !D_PVM
   /* wait for acknowledge */

   if (!m_dirty_trick) {
     src = 0; type = MT_CLOSE_MEASURE;
#if nCUBE
     nread(cube,(char *)&buffer, 100, (int *)&src, &type);
#endif
#if D_PVM
     pvm_recv(pvm_tids[0], -1);
#endif
     DBUG_PRINT ("S_MERGE", ("node 0 acknowledged."));
     } 
   }

   /* copy last file */
 
   sprintf (line, "rcp %s%s_uid%ld_%i %s:%s", m_standard, m_mesfilehea, getuid(), (1 << cube_dim)-1, m_remote_host, m_remote_dir);
   DBUG_PRINT ("S_MERGE", ("copy command : %s", line));
   if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }

#endif /* D_PVM */

  sprintf (line, "rcp %s %s:%s", mess_file, m_remote_host, m_remote_dir);

  if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }

  if (m_del_files) {
#if (D_PVM && !D_PVM_NCUBE)
    sprintf (line, "rm %s", mess_file);
#endif
#if (nCUBE || D_PVM_NCUBE)
    sprintf (line, "rm %s /tmp/%s_uid%ld_*", mess_file, m_mesfilehea, getuid());
#endif
  
    if ((m_super_value = system(line)) != 0) {
       printf (" command failed: %s, return: %d\n", line, m_super_value);
       host_flush();
     }
 
     DBUG_PRINT ("S_MERGE", ("command: %s", line));
     }

  zeit2 = host_time();

  if (m_ackno) {
    printf ("%4.2f s\n merging ... ", (zeit2 - zeit1));
    host_flush(); 
    }

/*   zeit1 = host_time(); */

#ifdef M_BINARY
  sprintf (line, "rsh %s %s %s %s %s %s %d 1 %d", m_remote_host, m_remote_bin, &mess_file[5], m_remote_dir, m_target_dir, m_mesfilehea, (1 << cube_dim), m_del_files);
#else
  sprintf (line, "rsh %s %s %s %s %s %s %d 0 %d", m_remote_host, m_remote_bin, &mess_file[5], m_remote_dir, m_target_dir, m_mesfilehea, (1 << cube_dim), m_del_files);
#endif

  DBUG_PRINT ("S_MERGE", ("merge command : %s", line));
  
  if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }

/***** print merging time usage *****/

  DBUG_PRINT ("S_MERGE", ("merging finished !!"));

  /* clearscreen(); */
  host_flush();

  DBUG_VOID_RETURN;
}

#endif /* D_MESS */

#endif /* STORE */

#endif /* D_DIST */
