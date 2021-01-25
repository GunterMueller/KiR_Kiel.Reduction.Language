/* $Log: rmessage.c,v $
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
#if D_DIST

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
#include <nhost.h>
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

#include "dbug.h"

#if D_DIST
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
int _tmp,_tmp2;
int cube;              /* file descriptor of the subcube */

/* extern variables */
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

#define SEND_2_SHORTS(data1,data2)  DO_SEND(((int)(data1) << 16) | (int)(data2))

#define RECEIVE_2_SHORTS(data1,data2) do {int tmp_data; \
					  tmp_data = DO_RECEIVE(); \
					  data1 = tmp_data >> 16; \
					  data2 = tmp_data & 0xffff;} while(0)

#define SWAP(a) (_tmp2 = (a),(((_tmp2 & 0xff) << 24) | ((_tmp2 & 0xff00) << 8) | ((_tmp2 & 0xff0000) >> 8) | ((_tmp2 & 0xff000000) >> 24)))

#define GET_CODE_START() (codedesc.codvec + 4)
#define IS_STATIC(addr)             (((addr) <= static_heap_upper_border) && ((addr) >= static_heap_lower_border))
#define STATIC_OFFSET(addr)         (static_heap_upper_border - (addr))
#define REAL_STATIC_ADDRESS(offset) (static_heap_upper_border - (offset)) 

typedef struct sent_list_node {PTR_DESCRIPTOR ptr;
                               int original_entry;
                               DESC_CLASS original_class;
                               struct sent_list_node * next;} SENT_LIST_NODE;

/***********************************************************/
/*                                                         */
/* init_ncube() - initializes the nCube                    */
/*                                                         */
/***********************************************************/

void init_ncube(int dim)
{int subcube;

 if ((cube = nopen(dim)) < 0)
   post_mortem("error: could not allocate subcube!");
 subcube = nodeset(cube,"all");

#if NCDEBUG
 DBMinit(cube, dim);
#endif

#if nCUBE
 DBUG_PRINT("HOST", ("Ich installiere nCUBE-Binary !"));
 if (rexecl(subcube,D_CUBEBIN,D_CUBEBIN,0).np_pid == -1)
   post_mortem ("init_ncube: cannot install nCUBE binary !");
#endif /* nCUBE */
}

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

 /* Prepare the message buffer */

 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;

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
 strcpy((char *)buffer_p, m_mesfilehea);
 strcpy(((char *)buffer_p)+strlen(m_mesfilehea)+1, m_mesfilepath);
 DBUG_PRINT("MSG", ("m_mesfilehea: %s, strlen: %d, m_mesfilepath: %s, strlen: %d, int's: %d", m_mesfilehea, strlen(m_mesfilehea), \
m_mesfilepath, strlen(m_mesfilepath), ((strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/4)+1));
 buffer_p += ((strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/4)+1;
#endif

 DO_SEND(0x42424242);
 nwrite(cube,(char *)buffer,MESSAGE_SIZE*sizeof(int),0xffff,type);
 free(buffer);
}

/***********************************************************/
/*                                                         */
/* send_graph() - sends the graph to the nCube             */
/*                                                         */
/***********************************************************/

void send_graph()
{int sent_index=0;                /* counter for sent descriptors  */
 int heap_elems,heap_counter;
 int code_sent=0;
 int param_counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 INSTR_DESCR send_instr;
 DESC_CLASS d_class;              /* class of descriptor           */
 DESC_TYPE d_type;                /* type of descriptor            */
 STACKELEM send_data;
 PTR_HEAPELEM heap_ptr,code_address;
 int *buffer,*buffer_p,*buffer_end;
 char *ngetp();
 SENT_LIST_NODE *sent_list = NULL,*new_node;
 MESSAGE_TYPE type=MT_NEW_GRAPH;

 /* prepare the message buffer */
 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;
 
 /* send the header */
 DO_SEND(_redcnt);
 send_data = READSTACK(S_e);
 PUSHSTACK(S_e,ST_END_MARKER);
 PUSHSTACK(S_e,send_data);
 
 while((send_data = POPSTACK(S_e)) != ST_END_MARKER)
   {if ((!T_POINTER(send_data) || !isdesc(send_data)) && send_data)
      DO_SEND(send_data);
   else
     {if (!send_data)
	{DO_SEND(0);
	 DO_SEND(1);}
     else
       {if (IS_STATIC(send_data))                       /* pointer into static heap? */
	  {DO_SEND(0);	
	   DO_SEND(((int)STATIC_OFFSET(send_data) << 4) | 0x0a);}     /* ...1010 as tag for offset in static heap */
       else
	 {if (R_DESC(*(T_PTD)send_data,class) == C_SENT)   /* descriptor already sent? */
	    {DO_SEND(0);
	     DO_SEND(((*((int *)send_data + 1)) << 4) | 0x0e);}
	 else
	   {DO_SEND(0);
	    DO_SEND(0);                                  /* tag for 'pointer to following descriptor' */
	    
	    /* send class and type of descriptor packed into one int */
	    
	    SEND_2_SHORTS((short)(d_class = R_DESC(*(T_PTD)send_data,class)),(short)(d_type = R_DESC(*(T_PTD)send_data,type)));
	    
	    /* refcount doesn`t need to be sent, will be set to 1 on dest processor */
	    /* and now: the main part of sending: the descriptor bodies */
	    switch(d_class)
	      {case C_SCALAR:
		 DO_SEND(R_SCALAR(*(T_PTD)send_data,vali));
		 DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));
		 DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));
		 break;
	       case C_DIGIT:
		 SEND_2_SHORTS(R_DIGIT(*(T_PTD)send_data,sign),R_DIGIT(*(T_PTD)send_data,ndigit));
		 heap_elems = (int)R_DIGIT(*(T_PTD)send_data,ndigit);
		 DO_SEND(R_DIGIT(*(T_PTD)send_data,Exp));
		 heap_ptr = R_DIGIT(*(T_PTD)send_data,ptdv);
		 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		   DO_SEND(*heap_ptr);
		 break;
	       case C_LIST:
		 SEND_2_SHORTS(R_LIST(*(T_PTD)send_data,special),R_LIST(*(T_PTD)send_data,dim));
		 heap_elems = (int)R_LIST(*(T_PTD)send_data,dim);
		 heap_ptr = R_LIST(*(T_PTD)send_data,ptdv);
		 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
		   PUSHSTACK(S_e,*heap_ptr);
		 break;
	       case C_MATRIX:
	       case C_VECTOR:
	       case C_TVECTOR:
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
		     PUSHSTACK(S_e,*heap_ptr);
		 break;
	       case C_EXPRESSION:
	       case C_CONSTANT:
		 switch(d_type)
		   {case TY_REC:
		    case TY_ZF:
		    case TY_SUB:
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
			     PUSHSTACK(S_e,*heap_ptr);}
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
			    PUSHSTACK(S_e,*heap_ptr);
#if WITHTILDE
			}
		      else
			DO_SEND(-1);
#endif
		      break;
#if WITHTILDE
		    case TY_SNSUB:
		      SEND_2_SHORTS(R_FUNC(*(T_PTD)send_data,special),R_FUNC(*(T_PTD)send_data,nargs));
		      heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
		      DO_SEND(heap_elems);
		      heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
#endif
		    case TY_ZFCODE:
		      SEND_2_SHORTS(R_ZFCODE(*(T_PTD)send_data,zfbound),R_ZFCODE(*(T_PTD)send_data,nargs));
		      PUSHSTACK(S_e,R_ZFCODE(*(T_PTD)send_data,ptd));
		      heap_elems = *(int *)R_ZFCODE(*(T_PTD)send_data,varnames);
		      heap_ptr = R_ZFCODE(*(T_PTD)send_data,varnames) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_COND:
		      DO_SEND((int)R_COND(*(T_PTD)send_data,special));
		      heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptte);
		      heap_ptr = R_COND(*(T_PTD)send_data,ptte) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptee);
		      heap_ptr = R_COND(*(T_PTD)send_data,ptee) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_VAR:
		      DO_SEND(R_VAR(*(T_PTD)send_data,nlabar));
		      PUSHSTACK(S_e,R_VAR(*(T_PTD)send_data,ptnd));
		      break;
		    case TY_EXPR:
		      heap_elems = *(int *)R_EXPR(*(T_PTD)send_data,pte);
		      heap_ptr = R_EXPR(*(T_PTD)send_data,pte) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_NAME:
		      heap_elems = *(int *)R_NAME(*(T_PTD)send_data,ptn);
		      heap_ptr = R_NAME(*(T_PTD)send_data,ptn) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_SWITCH:
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
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_NOMAT:
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
			   PUSHSTACK(S_e,*heap_ptr);}
		      else
			DO_SEND(-1);
		      PUSHSTACK(S_e,R_NOMAT(*(T_PTD)send_data,ptsdes));
		      break;
		    case TY_MATCH:
		      if (R_MATCH(*(T_PTD)send_data,guard))
		       {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,guard);
			heap_ptr = R_MATCH(*(T_PTD)send_data,guard) + 1;
			DO_SEND(heap_elems);
			for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			  PUSHSTACK(S_e,*heap_ptr);}
		      else
			DO_SEND(-1);
		      if (R_MATCH(*(T_PTD)send_data,body))
			{heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,body);
			 heap_ptr = R_MATCH(*(T_PTD)send_data,body) + 1;
			 DO_SEND(heap_elems);
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   PUSHSTACK(S_e,*heap_ptr);}
		      else
			DO_SEND(-1);
		      if (R_MATCH(*(T_PTD)send_data,code))
			{heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,code);
			 heap_ptr = R_MATCH(*(T_PTD)send_data,code) + 1;
			 DO_SEND(heap_elems);
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   {if (T_POINTER(*heap_ptr) && isdesc(*heap_ptr))
			      {DO_SEND(PM_DESC_MARKER);
			       PUSHSTACK(S_e,*heap_ptr);}
			   else
			     DO_SEND(*heap_ptr);}}
		      else
			DO_SEND(-1);
		      break;
		    case TY_LREC:
		      DO_SEND(R_LREC(*(T_PTD)send_data,nfuncs));
		      heap_elems = *(int *)R_LREC(*(T_PTD)send_data,namelist);
		      heap_ptr = R_LREC(*(T_PTD)send_data,namelist) + 1;
		      DO_SEND(heap_elems);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      heap_elems = R_LREC(*(T_PTD)send_data,nfuncs);
#if WITHTILDE
		      heap_ptr = R_LREC(*(T_PTD)send_data,ptdv)+1;
#else
		      heap_ptr = R_LREC(*(T_PTD)send_data,ptdv);
#endif
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_LREC_IND:
		      DO_SEND(R_LREC_IND(*(T_PTD)send_data,index));
		      PUSHSTACK(S_e,R_LREC_IND(*(T_PTD)send_data,ptdd));
		      PUSHSTACK(S_e,R_LREC_IND(*(T_PTD)send_data,ptf));
		      break;
		    case TY_LREC_ARGS:
		      DO_SEND(heap_elems = R_LREC_ARGS(*(T_PTD)send_data,nargs));
		      PUSHSTACK(S_e,R_LREC_ARGS(*(T_PTD)send_data,ptdd));
		      heap_ptr = R_LREC_ARGS(*(T_PTD)send_data,ptdv);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;}
		 break;
	       case C_FUNC:
	       case C_CONS:
		 switch(d_type)
		   {case TY_CLOS:
		      SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,args),R_CLOS(*(T_PTD)send_data,nargs));
#if WITHTILDE
		      SEND_2_SHORTS(R_CLOS(*(T_PTD)send_data,ftype),R_CLOS(*(T_PTD)send_data,nfv));
#else
		      DO_SEND(R_CLOS(*(T_PTD)send_data,ftype));
#endif
		      heap_elems = R_CLOS(*(T_PTD)send_data,args)+1;
		      heap_ptr = R_CLOS(*(T_PTD)send_data,pta);
		      for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			PUSHSTACK(S_e,*heap_ptr);
		      break;
		    case TY_COMB:
		      SEND_2_SHORTS(R_COMB(*(T_PTD)send_data,args),R_COMB(*(T_PTD)send_data,nargs));
		      PUSHSTACK(S_e,R_COMB(*(T_PTD)send_data,ptd));
		      heap_ptr = R_COMB(*(T_PTD)send_data,ptc);
		      goto send_code;
		    case TY_CONDI:
		      SEND_2_SHORTS(R_CONDI(*(T_PTD)send_data,args),R_CONDI(*(T_PTD)send_data,nargs));
		      PUSHSTACK(S_e,R_CONDI(*(T_PTD)send_data,ptd));
		      heap_ptr = R_CONDI(*(T_PTD)send_data,ptc);
		      goto send_code;
		    case TY_CONS:
		      PUSHSTACK(S_e,R_CONS(*(T_PTD)send_data,hd));
		      PUSHSTACK(S_e,R_CONS(*(T_PTD)send_data,tl));
		      break;
		    case TY_CASE:
		    case TY_WHEN:
		    case TY_GUARD:
		    case TY_BODY:
		      SEND_2_SHORTS(R_CASE(*(T_PTD)send_data,args),R_CASE(*(T_PTD)send_data,nargs));
		      PUSHSTACK(S_e,R_CASE(*(T_PTD)send_data,ptd));
		      heap_ptr = R_CASE(*(T_PTD)send_data,ptc);
		   send_code:
		      if (code_sent)
			DO_SEND(heap_ptr - GET_CODE_START());
		      else
			{
			  /* count length of code, the receiving node must know the size */
			  /* before the code is sent                                     */
			  
			  for (heap_elems=1,code_address=heap_ptr;*code_address++ != I_SYMTAB;heap_elems++);
			  DO_SEND(heap_elems);
			  code_address = heap_ptr;
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
				case ATOM_PAR:
				case FUNC_PAR:
				  for (param_counter = 0;param_counter < send_instr.nparams;param_counter++)
				    DO_SEND(*heap_ptr++);
				  break;
				case DESC_PAR:
				  PUSHSTACK(S_e,*(heap_ptr++));
				  break;
				case ADDR_PAR:
				  DO_SEND((PTR_HEAPELEM)*(heap_ptr++) - code_address);
				  break;
				}      /* switch */
			   }           /* while */
			  code_sent = 1;
			}              /* else {code not sent yet} */
		      break;
		    }                 /* switch(d_type) */
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
 nwrite(cube,(char *)buffer,(buffer_p - buffer)*sizeof(int),0xffff,type);
 free(buffer);}

/***********************************************************/
/*                                                         */
/* receive_result() - receives the result from the nCube   */
/*                                                         */
/***********************************************************/

void receive_result()
{int *buffer,*buffer_p,*buffer_end;
 int src = -1,type = -1;
 int heap_elems = 0,heap_counter;
 int received_index= 0;
 int *rec_addr, rec_data;
 int counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 DESC_CLASS new_class;
 DESC_TYPE new_type;
 PTR_HEAPELEM new_heap;
 T_PTD *received_list;
 T_PTD new_desc;
 int rec_list_size = 256;

 /* prepare the received list */
 received_list = (T_PTD *)malloc(sizeof(T_PTD)*rec_list_size);

 /* receive first block off message */
 buffer=buffer_p=(int *)malloc(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;

 nrange(cube,FIRST_TYPE,LAST_TYPE);

 nread(cube,(char *)buffer,MESSAGE_SIZE * sizeof(int),(long *)&src,&type);

 if (type == MT_POST_MORTEM)
/*   {exit_ncube(); */
   post_mortem((char *)buffer);
/*}*/
 
 /* receive the header */
 
 _redcnt      = DO_RECEIVE();
 
 PUSHSTACK(S_e,ST_END_MARKER);
 PUSHSTACK(S_a,ST_DUMMY);
 PUSHSTACK(S_e,(int)S_a.TopofStack << 1);

 while((int)(rec_addr = (int *)POPSTACK(S_e)) != ST_END_MARKER)
   {if ((int)rec_addr & 1)
	   {counter = POPSTACK(S_e);
	    if (counter > 1)
	      {PUSHSTACK(S_e,counter - 1);
	       PUSHSTACK(S_e,rec_addr - 2);}}
	 (int)rec_addr >>= 1;
	 rec_data = DO_RECEIVE();
	 if (rec_data != 0)
	   *rec_addr = rec_data;
	 else
	   {rec_data = DO_RECEIVE();
	    if (T_STAT_POINTER(rec_data))
	      {*rec_addr = REAL_STATIC_ADDRESS(rec_data >> 4);
	       INC_REFCNT((T_PTD)*rec_addr);}
	    else
	      {if (T_DESC_INDEX(rec_data))
		 {*rec_addr = (int) received_list[rec_data >> 4];
		  INC_REFCNT((T_PTD)*rec_addr);}
	       else
		 {if (T_NULL_POINTER(rec_data))
		    {*rec_addr = NULL;}
		 else                    /* a new descriptor will follow */
		   {RECEIVE_2_SHORTS(new_class,new_type);
		    MAKEDESC(new_desc,1,new_class,new_type);
		    *rec_addr = (int) new_desc;
		    switch(new_class)
		      {case C_SCALAR:
			 L_SCALAR(*new_desc,vali) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr)) = DO_RECEIVE();
			 *((int *)A_SCALAR(*new_desc,valr) + 1) = DO_RECEIVE();
			 break;
		       case C_DIGIT:
			 RECEIVE_2_SHORTS(L_DIGIT(*new_desc,sign),L_DIGIT(*new_desc,ndigit));
			 L_DIGIT(*new_desc,Exp) = DO_RECEIVE();
			 heap_elems = R_DIGIT(*new_desc,ndigit);
			 GET_HEAP(heap_elems,A_DIGIT(*new_desc,ptdv));
			 new_heap = R_DIGIT(*new_desc,ptdv);
			 for (heap_counter = 0;heap_counter < heap_elems;heap_counter++,new_heap++)
			   *new_heap = DO_RECEIVE();
			 break;
		       case C_LIST:
			 RECEIVE_2_SHORTS(L_LIST(*new_desc,special),L_LIST(*new_desc,dim));
			 L_LIST(*new_desc,ptdd) = NULL;
			 PUSHSTACK(S_e,heap_elems = R_LIST(*new_desc,dim));
			 GET_HEAP(heap_elems,A_LIST(*new_desc,ptdv));
			 new_heap = R_LIST(*new_desc,ptdv);
			 PUSHSTACK(S_e,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			 break;
		       case C_MATRIX:
		       case C_VECTOR:
		       case C_TVECTOR:
			 RECEIVE_2_SHORTS(L_MVT(*new_desc,nrows,new_class),L_MVT(*new_desc,ncols,new_class));
			 L_MVT(*new_desc,ptdd,new_class) = NULL;
			 if ((_formated == 1) && (new_type != TY_STRING))
			   {heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class));
			    if (new_type == TY_REAL)
			      heap_elems *= 2;}
			 else
			   PUSHSTACK(S_e,heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class)));
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
			   PUSHSTACK(S_e,((int)(new_heap + (heap_elems -1)) << 1) | 1);
			 break;
		       case C_EXPRESSION:
		       case C_CONSTANT:
			 switch(new_type)
			   {case TY_REC:
			    case TY_ZF:
			    case TY_SUB:
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{
#if WITHTILDE
				  counter = DO_RECEIVE();
				  if (counter > 0)
				    PUSHSTACK(S_e,counter-2);
				  else
				    PUSHSTACK(S_e,heap_elems -1);
#else
				  if (heap_elems > 0)
				    PUSHSTACK(S_e,heap_elems);
#endif
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,namelist));
				  new_heap = R_FUNC(*new_desc,namelist);
				  *new_heap = heap_elems;
#if WITHTILDE
				  *(++new_heap)=counter;
				  if (counter > 0)
				    {PUSHSTACK(S_e,((int)(new_heap + counter - 2) << 1) | 1);
				     for (;heap_elems>counter-1;heap_elems--)
				       *(new_heap + heap_elems - 1)=DO_RECEIVE();}
				  else
				    PUSHSTACK(S_e,((int)(new_heap + heap_elems - 1) << 1) | 1);
#else
				  if (heap_elems > 0)
				    PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
#endif
				}
			      else
				L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
#if WITHTILDE
			      if (heap_elems >= 0)
				{
#endif
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
				  new_heap = R_FUNC(*new_desc,pte);
				  *new_heap = heap_elems;
				  PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
#if WITHTILDE
				}
			      else
				L_FUNC(*new_desc,pte) = NULL;
#endif
			      break;
#if WITHTILDE
			    case TY_SNSUB:
			      RECEIVE_2_SHORTS(L_FUNC(*new_desc,special),L_FUNC(*new_desc,nargs));
			      L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
			      new_heap = R_FUNC(*new_desc,pte);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
#endif
			    case TY_ZFCODE:
			      RECEIVE_2_SHORTS(L_ZFCODE(*new_desc,zfbound),L_ZFCODE(*new_desc,nargs));
			      PUSHSTACK(S_e,(int)A_ZFCODE(*new_desc,ptd) << 1);
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_ZFCODE(*new_desc,varnames));
			      new_heap = R_ZFCODE(*new_desc,varnames);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_COND:
			      L_COND(*new_desc,special) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptte));
			      new_heap = R_COND(*new_desc,ptte);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptee));
			      new_heap = R_COND(*new_desc,ptee);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_VAR:
			      L_VAR(*new_desc,nlabar) = DO_RECEIVE();
			      PUSHSTACK(S_e,(int)A_VAR(*new_desc,ptnd) << 1);
			      break;
			    case TY_EXPR:
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_EXPR(*new_desc,pte));
			      new_heap = R_EXPR(*new_desc,pte);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NAME:
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_NAME(*new_desc,ptn));
			      new_heap = R_NAME(*new_desc,ptn);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_SWITCH:
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
			      PUSHSTACK(S_e,heap_elems);
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NOMAT:
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_NOMAT(*new_desc,act_nomat),L_NOMAT(*new_desc,reason));
#else
			      L_NOMAT(*new_desc,act_nomat) = DO_RECEIVE();
#endif
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{if (heap_elems > 0)
				   PUSHSTACK(S_e,heap_elems);
				 GET_HEAP(heap_elems+1,A_NOMAT(*new_desc,guard_body));
				 new_heap = R_NOMAT(*new_desc,guard_body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_NOMAT(*new_desc,guard_body) = NULL;
			      PUSHSTACK(S_e,(int)A_NOMAT(*new_desc,ptsdes) << 1);
			      break;
			    case TY_MATCH:
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   PUSHSTACK(S_e,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,guard));
				 new_heap = R_MATCH(*new_desc,guard);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,guard) = NULL;
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   PUSHSTACK(S_e,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,body));
				 new_heap = R_MATCH(*new_desc,body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,body) = NULL;
			      if ((heap_elems = DO_RECEIVE())>= 0)
				{GET_HEAP(heap_elems+1,A_MATCH(*new_desc,code));
				 new_heap = R_MATCH(*new_desc,code);
				 *new_heap++ = heap_elems;
				 for (heap_counter = 0;heap_counter < heap_elems; heap_counter++)
				   {rec_data = DO_RECEIVE();
				    if (rec_data == PM_DESC_MARKER)
				      PUSHSTACK(S_e,((int)new_heap++) << 1);
				    else
				      *new_heap++ = rec_data;}}
			      else
				L_MATCH(*new_desc,code) = NULL;
			      break;
			    case TY_LREC:
			      L_LREC(*new_desc,nfuncs) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      PUSHSTACK(S_e,heap_elems);
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,namelist));
			      new_heap = R_LREC(*new_desc,namelist);
			      *new_heap = heap_elems;
			      PUSHSTACK(S_e,((int)(new_heap + heap_elems) << 1) | 1);
			      PUSHSTACK(S_e,heap_elems = R_LREC(*new_desc,nfuncs));
#if WITHTILDE
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,ptdv));
#else
			      GET_HEAP(heap_elems,A_LREC(*new_desc,ptdv));
#endif
			      new_heap = R_LREC(*new_desc,ptdv);
#if WITHTILDE
			      *new_heap++ = R_LREC(*new_desc,nfuncs);
#endif
			      PUSHSTACK(S_e,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_LREC_IND:
			      L_LREC_IND(*new_desc,index) = DO_RECEIVE();
			      PUSHSTACK(S_e,(int)A_LREC_IND(*new_desc,ptdd) << 1);
			      PUSHSTACK(S_e,(int)A_LREC_IND(*new_desc,ptf) << 1);
			      break;
			    case TY_LREC_ARGS:
			      L_LREC_ARGS(*new_desc,nargs) = DO_RECEIVE();
			      PUSHSTACK(S_e,(int)A_LREC_ARGS(*new_desc,ptdd) << 1);
			      PUSHSTACK(S_e,heap_elems = R_LREC_ARGS(*new_desc,nargs));
			      GET_HEAP(heap_elems,A_LREC_ARGS(*new_desc,ptdv));
			      new_heap = R_LREC_ARGS(*new_desc,ptdv);
			      PUSHSTACK(S_e,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;}
			 break;
		       case C_FUNC:
		       case C_CONS:
			 switch(new_type)
			   {case TY_CLOS:
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,args),L_CLOS(*new_desc,nargs));
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_CLOS(*new_desc,ftype),L_CLOS(*new_desc,nfv));
#else
			      L_CLOS(*new_desc,ftype) = DO_RECEIVE();
#endif
			      PUSHSTACK(S_e,heap_elems = R_CLOS(*new_desc,args) + 1);
			      GET_HEAP(heap_elems,A_CLOS(*new_desc,pta));
			      new_heap = R_CLOS(*new_desc,pta);
			      PUSHSTACK(S_e,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_COMB:
			      RECEIVE_2_SHORTS(L_COMB(*new_desc,args),L_COMB(*new_desc,nargs));
			      PUSHSTACK(S_e,(int)A_COMB(*new_desc,ptd) << 1);
			      L_COMB(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
			      break;
			    case TY_CONDI:
			      RECEIVE_2_SHORTS(L_CONDI(*new_desc,args),L_CONDI(*new_desc,nargs));
			      PUSHSTACK(S_e,(int)A_CONDI(*new_desc,ptd) << 1);
			      L_CONDI(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
			      break;
			    case TY_CONS:
			      PUSHSTACK(S_e,(int)A_CONS(*new_desc,hd) << 1);
			      PUSHSTACK(S_e,(int)A_CONS(*new_desc,tl) << 1);
			      break;
			    case TY_CASE:
			    case TY_WHEN:
			    case TY_GUARD:
			    case TY_BODY:
			      RECEIVE_2_SHORTS(L_CASE(*new_desc,args),L_CASE(*new_desc,nargs));
			      PUSHSTACK(S_e,(int)A_CASE(*new_desc,ptd) << 1);
			      L_CASE(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
			      break;
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
   post_mortem("END_MARKER expected but not found");
 free(received_list);
 free(buffer);
}

/***********************************************************/
/*                                                         */
/* wait_for_ncube     waits for the NCUBE_READY message    */
/*                    from 2^dim nodes                     */
/*                                                         */
/***********************************************************/

void wait_for_ncube(dim)
int dim;
{int counter = 0,src,type;
 char error_message[256];

 while(counter < (1 << dim))
   {src = type = -1;
    nread(cube,error_message,256,(long *)&src,(int *) &type);
    if (type == MT_NCUBE_READY)
      counter++;
    if (type == MT_NCUBE_FAILED)
      {nclose(cube);
       cube = NULL;
       post_mortem(error_message);}}}

/***********************************************************/
/*                                                         */
/* exit_ncube     closes the connection to the nCube       */
/*                                                         */
/***********************************************************/

void exit_ncube()
{nwrite(cube,NULL,0,0xffff,MT_TERMINATE);
 nclose(cube);
 cube=NULL;}

/***********************************************************/
/*                                                         */
/* flush_queue    flushes the message queue                */
/*                                                         */
/***********************************************************/

void flush_queue()
{int src = -1, type = -1;
 /* char *buffer; */

 while (ntest(cube,(long *)&src,&type) >= 0)
   {nread(cube,NULL,0,(long *)&src,&type);
    src = type = -1;}}


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

  if ((init_file = fopen ("red.tickets", "r")) == NULL)
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

  if ((init_file = fopen ("red.tickets", "r")) == NULL)
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
  int i,/* j,k,*/ m_super_value;
  int m_dirty_trick = 0;
  char m_standard[255] = "/tmp/"; 

/* the dirty trick is to generate the measurement files on the host, so 
   there's no transport of files from nCUBE to host necessary, that's 
   a great speedup in merging time ! */

  DBUG_ENTER ("m_super_merging");

  if (strncmp(m_mesfilepath, "//d02/", 6) != 0) {
    DBUG_PRINT ("SMERGE", ("dirty trick used !"));
    m_dirty_trick=1;
    strcpy (m_standard, m_mesfilepath);
    }

  DBUG_PRINT ("S_MERGE", ("Merging %d files...", used_nodes));

  zeit1 =  host_time();      /* include problems, so function here */

  now = time(NULL);      /* getting time for filename (oops, nCUBE-time !?!?) */
  date = localtime(&now);

  sprintf (mess_file, "/tmp/ncube_%s_%d_%02d%02d%02d%02d%02d_uid%d.ms", _errmes, 1 << cube_dim,
  date->tm_mday, date->tm_mon, date->tm_year, date->tm_hour, date->tm_min, getuid());

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


  if ((init_file = fopen ("red.tickets", "r")) == NULL)
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

 /* no broadcast, node 0 will count file-lens */

 for (i = ((1 << cube_dim)-1); i >= 0; i--) {
   nwrite(cube,(char *)&i, 0, i, type);
   DBUG_PRINT ("S_MERGE", ("Schicke MT_CLOSE_MEASURE an node %d !", i)); }

 /* read file len from node 0 */

 src = 0;
 type = MT_CLOSE_MEASURE;

 nread(cube,(char *)&buffer, 100, (int *)&src, &type);

 DBUG_PRINT ("S_MERGE", ("es sind %d bytes !", SWAP(buffer)));

 if (m_ackno) {
   printf (" merging %d bytes ...\n", SWAP(buffer));
   host_flush();
   printf (" copying measurement files to remote host ... ");
   host_flush();
   }

 /* sending copy-request to nodes */

 if (!m_dirty_trick) {
   buffer = SWAP(0);
   type = MT_REMOTE_MEASURE;
   DBUG_PRINT ("S_MERGE", ("Sending request for copying file nr. 0 !"));
   nwrite(cube,(char *)&buffer, 4, 0, type);
   }
 
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

   DBUG_PRINT ("S_MERGE", ("now copy file to remote host"));
  
   sprintf (line, "rcp %s%s_uid%ld_%i %s:%s", m_standard, m_mesfilehea, getuid(), i-1, m_remote_host, m_remote_dir);
   DBUG_PRINT ("S_MERGE", ("copy command : %s", line));
   if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }

   /* wait for acknowledge */

   if (!m_dirty_trick) {
     src = 0; type = MT_CLOSE_MEASURE;
     nread(cube,(char *)&buffer, 100, (int *)&src, &type);
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

  sprintf (line, "rcp %s %s:%s", mess_file, m_remote_host, m_remote_dir);

  if ((m_super_value = system(line)) != 0) {
     printf (" command failed: %s, return: %d\n", line, m_super_value);
     host_flush();
     }

  if (m_del_files) {
    sprintf (line, "rm %s /tmp/%s_uid%ld_*", mess_file, m_mesfilehea, getuid());
  
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

#endif /* D_DIST */
