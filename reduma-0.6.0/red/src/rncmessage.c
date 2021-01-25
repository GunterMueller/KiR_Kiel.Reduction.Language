/* $Log: rncmessage.c,v $
 * Revision 1.19  1995/05/22  09:55:52  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.18  1995/05/17  12:58:10  rs
 * another test check-in (IGNORE)
 *
 * */

/***************************************************************/
/*                                                             */
/* rncmessage.c --- message routines for the nCube             */
/*                                                             */
/* ach 26/01/93                                                */
/*                                                             */
/***************************************************************/

#ifndef D_MESS_ONLY  
#define RNCMESSAGE Message-System!
#endif
#ifdef D_NN_SCHED
extern void nn_msg_send();
extern void nn_msg_receive();
extern int proc_org;


#include "rheapty.h"


#endif

#include "rheapty.h"
#include "rncheap.h"
#include "rncstack.h"
#include "rstdinc.h"
#include "rncmessage.h"
#include "rncsched.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinter.h"
#include <stdio.h>
#include <nself.h>
#include <n_errno.h>
#include "dbug.h"
#include <malloc.h>
#include <string.h>
#include <setjmp.h>

#if D_MESS
#include <sys/stat.h>
#include <sys/signal.h>
#include "d_mess_io.h"
extern PROCESS_CONTEXT* curr_pid;
extern int newheap();
extern void freeheap();
extern T_DESCRIPTOR *newdesc();
extern void freedesc();
extern int compheap();
extern void allocate_d_stack_segment();
extern void free_d_stack_segment();
extern INSTR *distribute();
extern PROCESS_CONTEXT *process_create();
extern void process_terminate();
extern void process_sleep();
extern void process_wakeup();
extern void process_run();

extern int proc_tab_size;
#endif

#define MESSAGE_SIZE 1024

#if D_MESS
#define D_MESS_DISABLED(text) { sprintf(m_super_name, "sorry, %s measurements are disabled", text); \
                              post_mortem(m_super_name); }
#endif

#ifndef D_MESS_ONLY      /* CHANGES IN DO_SEND, PLEASE ALSO IN D_MESS_ONLY CASE !!!!!! */

#if MSG_CHKSUM
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
			 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
			 buffer_end=buffer + MESSAGE_SIZE):0, \
			 _tmp = (int)(data), \
			 chksum += _tmp, \
			 *buffer_p++ = _tmp) 
#else
#define DO_SEND(data)  ((buffer_p == buffer_end)? \
			(result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
			 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
			 buffer_end=buffer + MESSAGE_SIZE):0, \
			 *buffer_p++ = (int)(data)) 
#endif
#else    /* DON'T FORGET CHANGES HERE !!!! */

#define DO_SEND(data)  ((buffer_p == buffer_end)? \
                        (result = nwritep((char *)buffer,MESSAGE_SIZE*sizeof(int),dest,type,&flags), \
			 ((result < 0)?post_mortem("Error on nwrite"):0), \
                         m_size += MESSAGE_SIZE*sizeof(int), \
                         buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int)), \
                         buffer_end=buffer + MESSAGE_SIZE):0, \
			*buffer_p++ = (int)(data))

#endif /* D_MESS_ONLY */

#ifndef D_MESS_ONLY  /* CHANGES IN DO_RECEIVE, PLEASE ALSO IN D_MESS_ONLY CASE !!!!!! */

#if MSG_CHKSUM
#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nrelp((char *)buffer), \
			   result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags), \
			   ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
			   buffer_end = buffer_p + result/sizeof(int)):0, \
			   chksum += *buffer_p, \
			  *buffer_p++) 
#else
#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
			  (nrelp((char *)buffer), \
			   result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags), \
			   ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
			   buffer_end = buffer_p + result/sizeof(int)):0, \
			  *buffer_p++) 
#endif
#else     /* DON'T FORGET CHANGES HERE !!!! */

#define DO_RECEIVE()     ((buffer_p == buffer_end)? \
                          (nrelp((char *)buffer), \
                           m_size += (result = nreadp(&buffer,MESSAGE_SIZE*sizeof(int),&src,&type,&flags)), \
                           ((result < 0)?post_mortem("Error on nread"):0), \
			   buffer_p = buffer, \
                           buffer_end = buffer_p + result/sizeof(int)):0, *buffer_p++)

#endif /* D_MESS_ONLY */

#define SEND_2_SHORTS(data1,data2)  DO_SEND(((int)(data1) << 16) | (int)(data2))

#define RECEIVE_2_SHORTS(data1,data2) do {int tmp_data; \
					  tmp_data = DO_RECEIVE(); \
					  data1 = tmp_data >> 16; \
					  data2 = tmp_data & 0xffff;} while(0)

typedef struct sent_list_node {PTR_DESCRIPTOR ptr;
			       int original_entry;
			       DESC_CLASS original_class;
			       struct sent_list_node * next;} SENT_LIST_NODE;

extern unsigned int dist_stack_elems;        /* rncsched.c */
extern int dist_stack_free;          /* rncsched.c */
#if WITHTILDE
extern unsigned int dist_tilde_elems;        /* rncsched.c */
extern int dist_tilde_free;          /* rncsched.c */
#endif /* WITHTILDE */
extern DIST_TYPE dist_type;          /* rncsched.c */
extern INSTR * dist_instr;           /* rncsched.c */
extern DStackDesc D_S_H;             /* rncstack.c */
extern DStackDesc D_S_A;             /* rncstack.c */
extern DStackDesc D_S_R;             /* rncstack.c */
extern DStackDesc D_S_I;             /* rncstack.c */
extern DStackDesc D_S_hilf;          /* rncstack.c */
extern DStackDesc *ps_a;             /* rncinter.c */
extern DStackDesc *ps_w;             /* rncinter.c */
#if WITHTILDE
extern DStackDesc *ps_t;
extern DStackDesc *ps_r;
#endif /* WITHTILDE */
extern int proc_id;                  /* rncmain.c */
extern PROCESS_CONTEXT * curr_pid;   /* rncsched.c */
extern int ticket_pool[];
extern INSTR_DESCR instr_tab[];      /* rncinter.h */
extern int _redcnt;
extern INSTR* heap_code_start;       /* rncheap.c */
extern int _prec_mult;
extern int _prec;
extern int _prec10;
extern BOOLEAN _formated;
extern BOOLEAN LazyLists;
extern BOOLEAN _digit_recycling;
extern BOOLEAN _beta_count_only;
extern BOOLEAN _trunc;
extern BOOLEAN _count_reductions;
extern BOOLEAN _heap_reserved;
extern int _maxnum;
extern int _base;
extern int hd;
extern int hs;
extern int ss;
extern int stack_seg_size;
extern int sig_msg_flag;
extern int static_heap_upper_border;
extern int static_heap_lower_border;
extern int highdesc;
extern int stdesc_border;
extern int lookup_next;
extern int ticket_pool_counter;
extern int cube_dim;
extern char post_mortem_msg[256];
extern jmp_buf post_mortem_buffer;
extern int first_lookup;

extern PROCESS_CONTEXT *process_create();
extern void process_wakeup();

void flush_queue();

#if D_MESS
extern int host;
extern void handle_signal();
void msg_send();
void msg_receive();
#endif

#ifndef D_MESS_ONLY

int _tmp;

#else

extern int _tmp;

#endif

/***************************************************************/
/*                                                             */
/* msg_send()     sends a message                              */
/*                                                             */
/* arguments: dest   the number of the destination processor   */
/*                                                             */
/*            type   the type of the message                   */
/*                                                             */
/***************************************************************/

#ifdef D_MESS_ONLY

void m_msg_send(int dest, MESSAGE_TYPE type)

#else

void msg_send(int dest, MESSAGE_TYPE type)

#endif

{int stack_index=0;               /* number of sent stackelements  */
 int stack_elems;                 /* number of elements to be sent */
 int flags;                       /* for nwritep                   */
 int sent_index=0;                /* counter for sent descriptors  */
 int heap_elems,heap_counter;
 int code_sent=0;
 int param_counter;
 int result;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 INSTR_DESCR send_instr;
 DESC_CLASS d_class;              /* class of descriptor           */
 DESC_TYPE d_type;                /* type of descriptor            */
 STACKELEM send_data,result_stelem;
 PTR_HEAPELEM heap_ptr,code_address;
 int *buffer,*buffer_p,*buffer_end;

 SENT_LIST_NODE *sent_list = NULL,*new_node;

 int dummy,*dummy2,*dummy3; /* Debugging */

#ifdef D_MESS_ONLY
 int m_size = 0;
 char *m_type;
 int m_pid = -1;
#endif

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_msg_send");
#else
 DBUG_ENTER("msg_send");
#endif

 if ((type == MT_NCUBE_READY) || (type == MT_ABORT))
   {

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
    if (m_msg_send_begin)
      if (type == MT_NCUBE_READY)
        MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_NCUBE_READY", dest, -1);
      else
        MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, "MT_ABORT", dest, -1);
#endif
#endif /* D_MESS_ONLY */

    nwrite(NULL,0,dest,type,&flags);

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
    if (m_msg_send_end)
      if (type == MT_NCUBE_READY)
        MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, 0);
      else
        MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, 0);
#endif
#endif

    DBUG_VOID_RETURN;}

 if (type == MT_POST_MORTEM)
   {nwrite(post_mortem_msg,strlen(post_mortem_msg)+1,dest,type,&flags);
    DBUG_VOID_RETURN;}

 if (type == MT_NCUBE_FAILED)
   {nwrite(post_mortem_msg,strlen(post_mortem_msg)+1,dest,type,&flags);
    DBUG_VOID_RETURN;}


 /* prepare the message buffer */
 buffer=buffer_p=(int *)ngetp(MESSAGE_SIZE*sizeof(int));
 buffer_end=buffer + MESSAGE_SIZE;
 switch (type)
   {case MT_NEW_PROCESS:
      
#ifdef D_MESS_ONLY
      m_type = "MT_NEW_PROCESS";
#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin)
	MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif

      /* send the header */
      DO_SEND((int)curr_pid);                                 /* the pid of the sending process */
      DO_SEND((int)(dist_instr-GET_CODE_START()));            /* the pc of the subprocess */
      DO_SEND((int)D_TOP_ADDRESS(*ps_w));                     /* where to put the result */
#if WITHTILDE
      DO_SEND((int)D_TOP_ADDRESS(*ps_r));                     /* where to find the counter */
#else
      DO_SEND((int)D_TOP_ADDRESS(D_S_H));                     /* where to find the counter */
#endif
      DO_SEND((_redcnt)?_redcnt-1:0);                         /* how many reductions the process may use */

      stack_elems = dist_stack_elems;
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);     /* push marker on Hilfstack */     
      if (dist_type == DIST_N)              /* send full stack frame? */
	for (;stack_index<stack_elems;stack_index++)
	  D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_a,stack_index));
      else                                  /* send only needed args */
	{while (stack_elems>0)
	   {if (stack_elems & 1)
	      D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_a,stack_index));
	   else
	     D_PUSHSTACK(D_S_hilf,ST_DUMMY);
	    stack_elems >>= 1;
	    stack_index++;}
	 for (;stack_index<dist_stack_free;stack_index++)
	   D_PUSHSTACK(D_S_hilf,ST_DUMMY);}
      
      DO_SEND(stack_index);

#if WITHTILDE
      stack_index = 0;
      stack_elems = dist_tilde_elems;

      for (;stack_index<stack_elems;stack_index++)
	D_PUSHSTACK(D_S_hilf,D_MIDSTACK(*ps_t,stack_index));
      
      DO_SEND(stack_index);
#endif /* WITHTILDE */
      
send_loop:

      while((send_data = D_POPSTACK(D_S_hilf)) != ST_END_MARKER)
	{if ((!T_POINTER(send_data) || !isdesc(send_data)) && send_data)
	   DO_SEND(send_data);
	else
	  {if (!send_data)                                  /* NULL pointer? */
	     {DO_SEND(0);
	      DO_SEND(1);}
	  else
	    {if (IS_STATIC(send_data))                       /* pointer into static heap? */
	       {DBUG_PRINT("STATHEAP",("sending %08x as static heap",send_data));
		DO_SEND(0);
		DO_SEND(((int)STATIC_OFFSET(send_data) << 4) | 0x0a);}     /* ...1010 as tag for offset in static heap */
	    else
	      {if (R_DESC(*(T_PTD)send_data,class) == C_SENT)   /* descriptor already sent? */
		 {DO_SEND(0);
		  DO_SEND(((*((int *)send_data + 1)) << 4) | 0x0e);}
	      else
		{DO_SEND(0);
		 DO_SEND(0);                            /* tag for 'pointer to following descriptor' */
		 
		 /* send class and type of descriptor packed into one int */
		 
		 SEND_2_SHORTS((short)(d_class = R_DESC(*(T_PTD)send_data,class)),(short)(d_type = R_DESC(*(T_PTD)send_data,type)));
		 
		 /* refcount doesn`t need to be sent, will be set to 1 on dest processor */
		 /* and now: the main part of sending: the descriptor bodies */
		 switch(d_class)
		   {case C_SCALAR:
		      DO_SEND(R_SCALAR(*(T_PTD)send_data,vali));
		      if (type == MT_END_RESULT)
			{DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));
			 DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));}
		      else
			{DO_SEND(*(int *)A_SCALAR(*(T_PTD)send_data,valr));
			 DO_SEND(*((int *)A_SCALAR(*(T_PTD)send_data,valr) + 1));}
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
			 D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   DO_SEND(*heap_ptr);
		       else
			 for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			   D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
				   D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
#if WITHTILDE
			    if (R_FUNC(*(T_PTD)send_data,pte))
			      {
#endif
				heap_elems = *(int *)R_FUNC(*(T_PTD)send_data,pte);
				heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
				DO_SEND(heap_elems);
				for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				  D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
			    heap_ptr = R_FUNC(*(T_PTD)send_data,pte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
#endif
			  case TY_ZFCODE:
			    SEND_2_SHORTS(R_ZFCODE(*(T_PTD)send_data,zfbound),R_ZFCODE(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_ZFCODE(*(T_PTD)send_data,ptd));
			    heap_elems = *(int *)R_ZFCODE(*(T_PTD)send_data,varnames);
			    heap_ptr = R_ZFCODE(*(T_PTD)send_data,varnames) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_COND:
			    DO_SEND((int)R_COND(*(T_PTD)send_data,special));
			    heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptte);
			    heap_ptr = R_COND(*(T_PTD)send_data,ptte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    heap_elems = *(int *)R_COND(*(T_PTD)send_data,ptee);
			    heap_ptr = R_COND(*(T_PTD)send_data,ptee) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_VAR:
			    DO_SEND(R_VAR(*(T_PTD)send_data,nlabar));
			    D_PUSHSTACK(D_S_hilf,R_VAR(*(T_PTD)send_data,ptnd));
			    break;
			  case TY_EXPR:
			    heap_elems = *(int *)R_EXPR(*(T_PTD)send_data,pte);
			    heap_ptr = R_EXPR(*(T_PTD)send_data,pte) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_NAME:
			    heap_elems = *(int *)R_NAME(*(T_PTD)send_data,ptn);
			    heap_ptr = R_NAME(*(T_PTD)send_data,ptn) + 1;
			    DO_SEND(heap_elems);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
			    for (heap_counter = 0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    D_PUSHSTACK(D_S_hilf,R_NOMAT(*(T_PTD)send_data,ptsdes));
			    break;
			  case TY_MATCH:
			    if (R_MATCH(*(T_PTD)send_data,guard))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,guard);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,guard) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    if (R_MATCH(*(T_PTD)send_data,body))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,body);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,body) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 D_PUSHSTACK(D_S_hilf,*heap_ptr);}
			    else
			      DO_SEND(-1);
			    if (R_MATCH(*(T_PTD)send_data,code))
			      {heap_elems = *(int *)R_MATCH(*(T_PTD)send_data,code);
			       heap_ptr = R_MATCH(*(T_PTD)send_data,code) + 1;
			       DO_SEND(heap_elems);
			       for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
				 {if (T_POINTER(*heap_ptr) && isdesc(*heap_ptr))
				    {DO_SEND(PM_DESC_MARKER);
				     D_PUSHSTACK(D_S_hilf,*heap_ptr);}
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
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    heap_elems = R_LREC(*(T_PTD)send_data,nfuncs);
#if WITHTILDE
			    heap_ptr = R_LREC(*(T_PTD)send_data,ptdv)+1;
#else
			    heap_ptr = R_LREC(*(T_PTD)send_data,ptdv);
#endif
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_LREC_IND:
			    DO_SEND(R_LREC_IND(*(T_PTD)send_data,index));
			    D_PUSHSTACK(D_S_hilf,R_LREC_IND(*(T_PTD)send_data,ptdd));
			    D_PUSHSTACK(D_S_hilf,R_LREC_IND(*(T_PTD)send_data,ptf));
			    break;
			  case TY_LREC_ARGS:
			    DO_SEND(heap_elems = R_LREC_ARGS(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_LREC_ARGS(*(T_PTD)send_data,ptdd));
			    heap_ptr = R_LREC_ARGS(*(T_PTD)send_data,ptdv);
			    for (heap_counter=0;heap_counter < heap_elems;heap_counter++,heap_ptr++)
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
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
			      D_PUSHSTACK(D_S_hilf,*heap_ptr);
			    break;
			  case TY_COMB:
			    SEND_2_SHORTS(R_COMB(*(T_PTD)send_data,args),R_COMB(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_COMB(*(T_PTD)send_data,ptd));
			    heap_ptr = R_COMB(*(T_PTD)send_data,ptc);
			    goto send_code;
			  case TY_CONDI:
			    SEND_2_SHORTS(R_CONDI(*(T_PTD)send_data,args),R_CONDI(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_CONDI(*(T_PTD)send_data,ptd));
			    heap_ptr = R_CONDI(*(T_PTD)send_data,ptc);
			    goto send_code;
			  case TY_CONS:
			    D_PUSHSTACK(D_S_hilf,R_CONS(*(T_PTD)send_data,hd));
			    D_PUSHSTACK(D_S_hilf,R_CONS(*(T_PTD)send_data,tl));
			    break;
			  case TY_CASE:
			  case TY_WHEN:
			  case TY_GUARD:
			  case TY_BODY:
			    SEND_2_SHORTS(R_CASE(*(T_PTD)send_data,args),R_CASE(*(T_PTD)send_data,nargs));
			    D_PUSHSTACK(D_S_hilf,R_CASE(*(T_PTD)send_data,ptd));
			    heap_ptr = R_CASE(*(T_PTD)send_data,ptc);
send_code:
			    DO_SEND(heap_ptr - GET_CODE_START());
			    break;
			  }                 /* switch(d_type) */
		       break;
		     }                      /* switch(d_class) */
		  new_node = (SENT_LIST_NODE *)malloc(sizeof(SENT_LIST_NODE));
		  if (!new_node)
		    post_mortem("insufficient memory for sent_list");
		  new_node->ptr = (T_PTD)send_data;
		  new_node->original_entry = *((int *)send_data + 1);
		  new_node->original_class = d_class;
		  new_node->next = sent_list;
		  sent_list = new_node;
		  L_DESC(*(T_PTD)send_data,class) = C_SENT;
		  *((int *)send_data + 1) = sent_index++;
	       }                           /* else {descriptor not sent yet}*/
	     }                                 /* else {not in static heap} */
	   }                                  /* else {null pointer}        */
	 }                                       /* else {pointer on stack} */
       }                                                           /* while */
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

/*      if (type == MT_END_RESULT)
	post_mortem("Das war's!"); */
	
      break;
    case MT_RESULT:
      
#ifdef D_MESS_ONLY
      m_type = "MT_RESULT";
      m_pid = (int) PC_GET_PPID(curr_pid);
#if (D_SLAVE && D_MESS && D_MCOMMU)
      if (m_msg_send_begin)
	MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif
      
      DBUG_PRINT("MSG",("Result -> Node %d: class %d,type %d",dest,
			R_DESC(*((T_PTD)*(ps_w->TopofStack)),class),
			R_DESC(*((T_PTD)*(ps_w->TopofStack)),type)));

      /* header */
      DO_SEND(PC_GET_PPID(curr_pid));

#if WITHTILDE
      DO_SEND(D_POPSTACK(D_S_R));
      DO_SEND(D_POPSTACK(D_S_R));
#else
      DO_SEND(D_POPSTACK(D_S_H));
      DO_SEND(D_POPSTACK(D_S_H));
#endif
      DO_SEND(PC_GET_SREDCNT(curr_pid)-_redcnt);
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      result_stelem = D_POPSTACK(*ps_w);
      D_PUSHSTACK(D_S_hilf,result_stelem);
      goto send_loop;
    case MT_END_RESULT:

#ifdef D_MESS_ONLY
  m_type = "MT_END_RESULT";
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_send_begin)
    MPRINT_SEND_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, dest, m_pid);
#endif
#endif

      DBUG_PRINT("MSG",("End result: class %d,type %d",
			     R_DESC(*((T_PTD)*(ps_w->TopofStack)),class),
			     R_DESC(*((T_PTD)*(ps_w->TopofStack)),type)));

      DO_SEND(_redcnt);
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      result_stelem = D_POPSTACK(*ps_w);
      D_PUSHSTACK(D_S_hilf,result_stelem);
      SET_STATIC_BORDER(stdesc_border);
      DBUG_PRINT("STATHEAP",("new end of static heap: %08x",stdesc_border));
      goto send_loop;
    }                                       /* switch(type) */
 result = nwritep((char *)buffer,(buffer_p - buffer) * sizeof(int),dest,type,&flags);
 if (result < 0)
   post_mortem("error on nwrite");

#ifdef D_MESS_ONLY
 m_size += (buffer_p - buffer) * sizeof(int);
#endif

 if ((type == MT_RESULT) || (type == MT_END_RESULT))
    T_DEC_REFCNT((T_PTD)result_stelem);

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
 if (m_msg_send_end)
   MPRINT_SEND_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_size);
#endif
#endif

 DBUG_VOID_RETURN;
}


/***************************************************************/
/*                                                             */
/* msg_receive()  receives a message and calls msg_handle      */
/*                                                             */
/***************************************************************/

#ifdef D_MESS_ONLY

void m_msg_receive()

#else

void msg_receive()

#endif

{int *buffer,*buffer_p,*buffer_end;
 int src=-1,type = -1,flags;
 STACKELEM *h_stack_addr, *a_stack_addr;
 int new_redcnt;
 int stelem_counter,stelem_number,heap_elems,heap_counter,param_counter;
 int received_index= 0;
 int *rec_addr, rec_data;
 int counter;
#if MSG_CHKSUM
 int chksum = 0;
#endif
 INSTR *new_pc;
 INSTR_DESCR rec_instr;
 PROCESS_CONTEXT *new_pid,*ppid,*rec_pid;
 DESC_CLASS new_class;
 DESC_TYPE new_type;
 PTR_HEAPELEM new_heap,code_address;
 T_PTD *received_list;
 T_PTD new_desc;
 int rec_list_size = 256;
 int code_received = 0;
 int result;
 
#if D_MESS
static struct sigaction act = {handle_signal,0,0};
#ifdef M_OLD_MERGING
 char mess_line[120];
#endif
 static char m_super_buffer[4096];
 int m_super_counter;
 struct stat stbuf;
 char name_line[120];
 int i,k;
 FILE *m_super_file1;
 FILE *m_super_file2;
 char m_super_name[120];
 int m_super_nr;
 int m_super_c;
#endif

#ifdef D_MESS_ONLY
 int m_size = 0;
 char *m_type;
 int m_pid = -1;
 int f_pid = -1;
#endif

#ifdef D_MESS_ONLY
 DBUG_ENTER ("m_msg_receive");
#else
 DBUG_ENTER("msg_receive");
#endif

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_receive_begin)
    MPRINT_RECEIVE_BEGIN(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);
#endif
#endif

 /* prepare the received list */
 received_list = (T_PTD *)malloc(sizeof(T_PTD)*rec_list_size);
 if (!received_list)
   post_mortem("insufficient memory for received_list");

 /* receive first block off message */
#ifdef D_MESS_ONLY
 m_size += ( result = nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags) );
#else
 result = nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags);
#endif
 if (result < 0)
   post_mortem("error on nread");

 buffer_p = buffer;
 buffer_end = buffer + result/sizeof(int);

/* if (type != MT_PARAM)
   hpchain2(); */

 res_heap();
 _heap_reserved = TRUE;

 switch(type)
   {case MT_ABORT:

#ifdef D_MESS_ONLY
      m_type = "MT_ABORT";
#endif /* D_MESS_ONLY */

      process_terminate_all();

      flush_queue();
      longjmp(post_mortem_buffer,1);
      break;

    case MT_TERMINATE:

      exit(0);

    case MT_NEW_PROCESS:

      /* receive the header */

      ppid         = (PROCESS_CONTEXT *)DO_RECEIVE();

#ifdef D_MESS_ONLY
  m_type = "MT_NEW_PROCESS";
  m_pid = (int) ppid;
#endif

      new_pc       = (INSTR *)(DO_RECEIVE() + GET_CODE_START());
      a_stack_addr = (STACKELEM *)DO_RECEIVE();
      h_stack_addr = (STACKELEM *)DO_RECEIVE();
      new_redcnt      = DO_RECEIVE();

      /* create new process with data from header */

#if (D_SLAVE && D_MESS && D_MSCHED)
      new_pid = (*d_m_process_create)(new_pc,ppid,new_redcnt);
#else
      new_pid = process_create(new_pc,ppid,new_redcnt);
#endif

#if D_MESS_ONLY
      f_pid = (int) new_pid;
#endif
      
      /* get addresses of new stacks */

#if WITHTILDE
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),h_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),a_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),(src << 1));
#else
      D_PUSHSTACK(PC_GET_H_STACK(new_pid),h_stack_addr);
      D_PUSHSTACK(PC_GET_H_STACK(new_pid),a_stack_addr);
      D_PUSHSTACK(PC_GET_R_STACK(new_pid),src);
#endif
      
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);

      stelem_number = DO_RECEIVE();
      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	{D_PUSHSTACK(PC_GET_REAL_I_STACK(new_pid),ST_DUMMY);
	 D_PUSHSTACK(PC_GET_H_STACK(new_pid),(int)D_TOP_ADDRESS(PC_GET_I_STACK(new_pid)) << 1);}

      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	D_PUSHSTACK(D_S_hilf,D_POPSTACK(PC_GET_H_STACK(new_pid)));

#if WITHTILDE
      stelem_number = DO_RECEIVE();
      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	{D_PUSHSTACK(PC_GET_H_STACK(new_pid),ST_DUMMY);
	 D_PUSHSTACK(PC_GET_R_STACK(new_pid),(int)D_TOP_ADDRESS(PC_GET_H_STACK(new_pid)) << 1);}

      for (stelem_counter = 0; stelem_counter < stelem_number; stelem_counter++)
	D_PUSHSTACK(D_S_hilf,D_POPSTACK(PC_GET_R_STACK(new_pid)));
#endif /* WITHTILDE */

receive_loop:
      while((int)(rec_addr = (int *)D_POPSTACK(D_S_hilf)) != ST_END_MARKER)
	{ if ((int)rec_addr & 1)
	   {counter = D_POPSTACK(D_S_hilf);
	    DBUG_ASSERT(counter > 0);
	    if (counter > 1)
	      {D_PUSHSTACK(D_S_hilf,counter - 1);
	       D_PUSHSTACK(D_S_hilf,rec_addr - 2);}}  /* minus 2, it's still shifted! */
	 rec_addr = (int *)((int)rec_addr >> 1);
/*	 if (isheap(rec_addr))
	   post_mortem("Trying to overwrite header of heapblock!"); */
	 rec_data = DO_RECEIVE();
	 if (rec_data != 0)
	   {DBUG_PRINT("MSG",("receiving non-pointer value %d (%x)",rec_data,rec_data));
	    *rec_addr = rec_data;}
	 else
	   {rec_data = DO_RECEIVE();
	    if (T_STAT_POINTER(rec_data))
	      {*rec_addr = (int)REAL_STATIC_ADDRESS((int)rec_data >> 4);
	       DBUG_PRINT("MSG",("receiving static descriptor pointer %x",*rec_addr));
	       DBUG_ASSERT(isdesc(*rec_addr));
	       INC_REFCNT((T_PTD)*rec_addr);
	       DBUG_PRINT("MSG",("refcount is now %d",((T_PTD)*rec_addr)->u.sc.ref_count));}
	    else
	      {if (T_DESC_INDEX(rec_data))
		 {*(T_PTD *)rec_addr = received_list[rec_data >> 4];
		  DBUG_PRINT("MSG",("receiving index for descriptor at %x",*rec_addr));
		  DBUG_ASSERT(isdesc(*rec_addr));
		  INC_REFCNT((T_PTD)*rec_addr);
		  DBUG_PRINT("MSG",("refcount is now %d",((T_PTD)*rec_addr)->u.sc.ref_count));}
	       else
		 {if (T_NULL_POINTER(rec_data))
		    {*rec_addr = NULL;}
		 else                    /* a new descriptor will follow */
		   {RECEIVE_2_SHORTS(new_class,new_type);
		    MAKEDESC(new_desc,1,new_class,new_type);
		    *rec_addr = (int)new_desc;
		    DBUG_PRINT("MSG",("receiving desc for address %x: class %d, type %d",rec_addr,new_class,new_type));
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
			 D_PUSHSTACK(D_S_hilf,heap_elems = R_LIST(*new_desc,dim));
			 GET_HEAP(heap_elems,A_LIST(*new_desc,ptdv));
			 new_heap = R_LIST(*new_desc,ptdv);
			 D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
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
			   D_PUSHSTACK(D_S_hilf,heap_elems = (R_MVT(*new_desc,nrows,new_class)*R_MVT(*new_desc,ncols,new_class)));
			 GET_HEAP(heap_elems,A_MVT(*new_desc,ptdv,new_class));
			 new_heap = R_MVT(*new_desc,ptdv,new_class);
			 if ((_formated == 1) && (new_type != TY_STRING))
			   for(;heap_elems >0;heap_elems--,new_heap++)
			     *new_heap = DO_RECEIVE();
			 else
			   D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems -1)) << 1) | 1);
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
				    D_PUSHSTACK(D_S_hilf,counter-2);
				  else
				    D_PUSHSTACK(D_S_hilf,heap_elems-1);
#else
				  if (heap_elems >0)
				    D_PUSHSTACK(D_S_hilf,heap_elems);
#endif
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,namelist));
				  new_heap = R_FUNC(*new_desc,namelist);
				  *new_heap = heap_elems;
#if WITHTILDE
				  *(++new_heap)=counter;
				  if (counter > 0)
				    {D_PUSHSTACK(D_S_hilf,((int)(new_heap + counter - 2) << 1) | 1);
				     for (;heap_elems>counter-1;heap_elems--)
				       *(new_heap + heap_elems - 1)=DO_RECEIVE();}
				  else
				    D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems - 1) << 1) | 1);
#else
				  if (heap_elems >0)
				    D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
#endif
				}
			      else
				L_FUNC(*new_desc,namelist) = NULL;
			      heap_elems = DO_RECEIVE();
#if WITHTILDE
			      if (heap_elems >= 0)
				{
#endif
				  D_PUSHSTACK(D_S_hilf,heap_elems);
				  GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
				  new_heap = R_FUNC(*new_desc,pte);
				  *new_heap = heap_elems;
				  D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
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
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_FUNC(*new_desc,pte));
			      new_heap = R_FUNC(*new_desc,pte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
#endif
			    case TY_ZFCODE:
			      RECEIVE_2_SHORTS(L_ZFCODE(*new_desc,zfbound),L_ZFCODE(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_ZFCODE(*new_desc,ptd) << 1);
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_ZFCODE(*new_desc,varnames));
			      new_heap = R_ZFCODE(*new_desc,varnames);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_COND:
			      L_COND(*new_desc,special) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptte));
			      new_heap = R_COND(*new_desc,ptte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_COND(*new_desc,ptee));
			      new_heap = R_COND(*new_desc,ptee);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_VAR:
			      L_VAR(*new_desc,nlabar) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_VAR(*new_desc,ptnd) << 1);
			      break;
			    case TY_EXPR:
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_EXPR(*new_desc,pte));
			      new_heap = R_EXPR(*new_desc,pte);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NAME:
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_NAME(*new_desc,ptn));
			      new_heap = R_NAME(*new_desc,ptn);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
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
			      *(new_heap+(heap_elems--)) = DO_RECEIVE();
#endif
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      break;
			    case TY_NOMAT:
#if WITHTILDE
			      RECEIVE_2_SHORTS(L_NOMAT(*new_desc,act_nomat),L_NOMAT(*new_desc,reason));
#else
			      L_NOMAT(*new_desc,act_nomat) = DO_RECEIVE();
#endif
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_NOMAT(*new_desc,guard_body));
				 new_heap = R_NOMAT(*new_desc,guard_body);
				 *new_heap = heap_elems;
				 D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_NOMAT(*new_desc,guard_body) = NULL;
			      D_PUSHSTACK(D_S_hilf,(int)A_NOMAT(*new_desc,ptsdes) << 1);
			      break;
			    case TY_MATCH:
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,guard));
				 new_heap = R_MATCH(*new_desc,guard);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,guard) = NULL;
			      heap_elems = DO_RECEIVE();
			      if (heap_elems >= 0)
				{if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,heap_elems);
				 GET_HEAP(heap_elems+1,A_MATCH(*new_desc,body));
				 new_heap = R_MATCH(*new_desc,body);
				 *new_heap = heap_elems;
				 if (heap_elems > 0)
				   D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);}
			      else
				L_MATCH(*new_desc,body) = NULL;
			      if ((heap_elems = DO_RECEIVE()) >= 0)
				{GET_HEAP(heap_elems+1,A_MATCH(*new_desc,code));
				 new_heap = R_MATCH(*new_desc,code);
				 *new_heap++ = heap_elems;
				 for (heap_counter = 0;heap_counter < heap_elems; heap_counter++)
				   {rec_data = DO_RECEIVE();
				    if (rec_data == PM_DESC_MARKER)
				      D_PUSHSTACK(D_S_hilf,((int)new_heap++) << 1);
				    else
				      *new_heap++ = rec_data;}}
			      else
				L_MATCH(*new_desc,code) = NULL;
			      break;
			    case TY_LREC:
			      L_LREC(*new_desc,nfuncs) = DO_RECEIVE();
			      heap_elems = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,heap_elems);
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,namelist));
			      new_heap = R_LREC(*new_desc,namelist);
			      *new_heap = heap_elems;
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + heap_elems) << 1) | 1);
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_LREC(*new_desc,nfuncs));
#if WITHTILDE
			      GET_HEAP(heap_elems+1,A_LREC(*new_desc,ptdv));
#else
			      GET_HEAP(heap_elems,A_LREC(*new_desc,ptdv));
#endif
			      new_heap = R_LREC(*new_desc,ptdv);
#if WITHTILDE
			      *new_heap++ = R_LREC(*new_desc,nfuncs);
#endif
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_LREC_IND:
			      L_LREC_IND(*new_desc,index) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_IND(*new_desc,ptdd) << 1);
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_IND(*new_desc,ptf) << 1);
			      break;
			    case TY_LREC_ARGS:
			      L_LREC_ARGS(*new_desc,nargs) = DO_RECEIVE();
			      D_PUSHSTACK(D_S_hilf,(int)A_LREC_ARGS(*new_desc,ptdd) << 1);
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_LREC_ARGS(*new_desc,nargs));
			      GET_HEAP(heap_elems,A_LREC_ARGS(*new_desc,ptdv));
			      new_heap = R_LREC_ARGS(*new_desc,ptdv);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
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
			      D_PUSHSTACK(D_S_hilf,heap_elems = R_CLOS(*new_desc,args) + 1);
			      GET_HEAP(heap_elems,A_CLOS(*new_desc,pta));
			      new_heap = R_CLOS(*new_desc,pta);
			      D_PUSHSTACK(D_S_hilf,((int)(new_heap + (heap_elems - 1)) << 1) | 1);
			      break;
			    case TY_COMB:
			      RECEIVE_2_SHORTS(L_COMB(*new_desc,args),L_COMB(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_COMB(*new_desc,ptd) << 1);
			      if ((type != MT_NEW_GRAPH) || code_received)
				L_COMB(*new_desc,ptc) = (PTR_HEAPELEM)(GET_CODE_START() + DO_RECEIVE());
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_COMB(*new_desc,ptc));
				 new_heap = R_COMB(*new_desc,ptc);
				 goto receive_code;}
			      break;
			    case TY_CONDI:
			      RECEIVE_2_SHORTS(L_CONDI(*new_desc,args),L_CONDI(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_CONDI(*new_desc,ptd) << 1);
			      if ((type != MT_NEW_GRAPH) || code_received)
				L_CONDI(*new_desc,ptc) = (PTR_HEAPELEM)GET_CODE_START() + DO_RECEIVE();
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_CONDI(*new_desc,ptc));
				 new_heap = R_CONDI(*new_desc,ptc);
				 goto receive_code;}
			      break;
			    case TY_CONS:
			      D_PUSHSTACK(D_S_hilf,(int)A_CONS(*new_desc,hd) << 1);
			      D_PUSHSTACK(D_S_hilf,(int)A_CONS(*new_desc,tl) << 1);
			      break;
			    case TY_CASE:
			    case TY_WHEN:
			    case TY_GUARD:
			    case TY_BODY:
			      RECEIVE_2_SHORTS(L_CASE(*new_desc,args),L_CASE(*new_desc,nargs));
			      D_PUSHSTACK(D_S_hilf,(int)A_CASE(*new_desc,ptd) << 1);
			      if ((type != MT_NEW_GRAPH) || code_received)
				L_CASE(*new_desc,ptc) = GET_CODE_START() + DO_RECEIVE();
			      else
				{heap_elems = DO_RECEIVE();
				 GET_HEAP(heap_elems+1,A_CASE(*new_desc,ptc));
				 new_heap = R_CASE(*new_desc,ptc);
			       receive_code:
				 code_address = new_heap;
				 for (heap_counter = 1;heap_counter < heap_elems; heap_counter ++)
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
				       case ATOM_PAR:
				       case FUNC_PAR:
					 for (param_counter = 0;param_counter < rec_instr.nparams;param_counter++)
					   {*(new_heap++) = DO_RECEIVE();
					    heap_counter ++;}
					 break;
				       case DESC_PAR:
					 D_PUSHSTACK(D_S_hilf,(int)(new_heap++) << 1);
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
			      break;
			    }                                             /* switch (new_type) */
			 break;
		       }                                                  /* switch (new_class) */
		    received_list[received_index++] = new_desc;
		    if (received_index == rec_list_size)
		      {rec_list_size += 256;
		       received_list = (T_PTD *)realloc(received_list,rec_list_size * sizeof(T_PTD));
		       if (!received_list)
			 post_mortem("insufficient memory for received_list");}
		  }                                                       /* else {descriptor not received yet} */
		}                                                       /* else {null pointer} */
	     }                                                          /* else {not in static heap} */
	  }                                                             /* else {extended pointer on stack} */
       }                                                                /* while */
#if MSG_CHKSUM
      rec_data = DO_RECEIVE();
      if (rec_data*2 != chksum) {
	post_mortem("Checksum error");
      }
#endif


      rec_data = DO_RECEIVE();
      if (rec_data != END_MARKER)
	post_mortem("END_MARKER expected but not found");
      nrelp((char *)buffer);
      free(received_list);


  rel_heap();
  _heap_reserved = FALSE;

      if (type == MT_NEW_GRAPH)
	{SET_STATIC_BORDER(highdesc);
	 DBUG_PRINT("STATHEAP",("new end of static heap: %08x",highdesc));
	 if (proc_id == 0)
	   PC_SET_PC(new_pid,R_COMB(*(T_PTD)D_READSTACK(PC_GET_H_STACK(new_pid)),ptc));
	 else
	   D_PPOPSTACK(D_S_hilf);}
      break;

 case MT_RESULT:

#ifdef D_MESS_ONLY
  m_type = "MT_RESULT";
#endif

      rec_pid = (PROCESS_CONTEXT *)DO_RECEIVE();

#if D_MESS_ONLY
  f_pid = (int) rec_pid;
#endif

      DBUG_PRINT("MSG",("receiving result for process %x from %d",rec_pid,src));
      a_stack_addr = (STACKELEM *)DO_RECEIVE();
      DBUG_PRINT("MSG",("a_stack_addr: %x",a_stack_addr));
      h_stack_addr = (STACKELEM *)DO_RECEIVE();
      if (rec_pid == curr_pid)
	_redcnt -= DO_RECEIVE();
      else
	PC_SET_REDCNT(rec_pid,PC_GET_REDCNT(rec_pid) - DO_RECEIVE());
#if(!D_NN_SCHED)
      ticket_pool[src]++;
      ticket_pool_counter++;
#endif
      if ((--(*(int *)h_stack_addr) == 0) && (PC_STATUS(rec_pid) == PS_WAITING))
#if (D_SLAVE && D_MESS && D_MSCHED)
        (*d_m_process_wakeup)(rec_pid);
#else
	process_wakeup(rec_pid);
#endif
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      D_PUSHSTACK(D_S_hilf,(int)a_stack_addr << 1);
      goto receive_loop;

    case MT_NEW_GRAPH:

#ifdef D_MESS_ONLY
  m_type = "MT_NEW_GRAPH";
#endif

      new_redcnt = DO_RECEIVE();
      if (proc_id==0)
	{
#if (D_SLAVE && D_MESS && D_MSCHED)
         new_pid = (*d_m_process_create)(0,0,new_redcnt);
#else
         new_pid = process_create(0,0,new_redcnt);
#endif

#if D_MESS_ONLY
         f_pid = (int) new_pid;
#endif

	 D_PUSHSTACK(PC_GET_H_STACK(new_pid),ST_DUMMY);
	 h_stack_addr = (STACKELEM *)D_TOP_ADDRESS(PC_GET_H_STACK(new_pid));}
      else
	{D_PUSHSTACK(D_S_hilf,ST_DUMMY);
	 h_stack_addr = (STACKELEM *)D_TOP_ADDRESS(D_S_hilf);}
      D_PUSHSTACK(D_S_hilf,ST_END_MARKER);
      D_PUSHSTACK(D_S_hilf,(int)h_stack_addr << 1);
      goto receive_loop;

 case MT_PARAM:


#if D_MESS
     sigaction (SIGMSGS,&act,NULL);   /* reinstall signal handler ... */
     sig_msg_flag = 0;
     m_curr_pid = -1;
#endif

#ifdef D_MESS_ONLY
  m_type = "MT_PARAM";
#endif

  _prec_mult        = DO_RECEIVE();
  _prec             = DO_RECEIVE();
  _prec10           = DO_RECEIVE();
  _formated         = DO_RECEIVE();
  LazyLists         = DO_RECEIVE();
  _digit_recycling  = DO_RECEIVE();
  _beta_count_only  = DO_RECEIVE();
  _trunc            = DO_RECEIVE();
  _count_reductions = DO_RECEIVE();
  _maxnum           = DO_RECEIVE();
  _base             = DO_RECEIVE();
  hs                = DO_RECEIVE();
  hd                = DO_RECEIVE();
  ss                = DO_RECEIVE();
  stack_seg_size    = DO_RECEIVE();
   
#if D_MESS
      m_ackno           = DO_RECEIVE();
      m_merge_strat     = DO_RECEIVE();
      d_bib_mask        = DO_RECEIVE();
      DBUG_PRINT ("MSG", ("received real bitmask: %x", d_bib_mask));
      strcpy((char *)m_mesfilehea, (char *)buffer_p);
      strcpy(m_mesfilepath, ((char *)buffer_p)+strlen(m_mesfilehea)+1);
      buffer_p += ((int)(strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/(int)4)+1;
      DBUG_PRINT("MSG", ("m_mesfilehea: %s, strlen: %d, m_mesfilepath: %s, strlen: %d, int's: %d", m_mesfilehea, strlen(m_mesfilehea), \
m_mesfilepath, strlen(m_mesfilepath), ((int)(strlen(m_mesfilehea)+strlen(m_mesfilepath)+2)/(int)4)+1)); 

      d_init_mess(proc_id);
#endif

      InitHeapManagement(hs,hd);
      alloc_stack_memory(ss);
      init_d_stack(&D_S_hilf);
      strcpy(D_S_hilf.name,"hilf");
      lookup_next = first_lookup;

#if D_MESS

      MPRINT_SPECIAL(D_MESS_FILE, 0.0, PROCESS_NR, -1, proc_tab_size, m_base);

/* #if D_MHEAP  leave this out for measurements check */

      if (d_bib_mask & (1 << MINDEX_HEAP_ALLOC)) {
        DBUG_PRINT ("MSG", ("HEAP_ALLOC measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_newheap = m_newheap; }
      else 
        d_m_newheap = newheap;

      if (d_bib_mask & (1 << MINDEX_HEAP_FREE)) {
        DBUG_PRINT ("MSG", ("HEAP_FREE measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_freeheap = m_freeheap; }
      else
        d_m_freeheap = freeheap;

      if (d_bib_mask & (1 << MINDEX_HEAP_COMPACT)) {
        DBUG_PRINT ("MSG", ("HEAP_COMPACT measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_compheap = m_compheap; }
      else
        d_m_compheap = compheap;

      if (d_bib_mask & (1 << MINDEX_DESC_ALLOC)) {
        DBUG_PRINT ("MSG", ("DESC_ALLOC measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_newdesc = m_newdesc; }
      else
        d_m_newdesc = newdesc;

      if (d_bib_mask & (1 << MINDEX_DESC_FREE)) {
        DBUG_PRINT ("MSG", ("DESC_FREE measure initiated"));
#if (!D_MHEAP) 
        D_MESS_DISABLED("heap");
#endif
        d_m_freedesc = m_freedesc; }
      else
        d_m_freedesc = freedesc;

/* #endif  D_MHEAP */ 

/* #if D_MSTACK */

      if (d_bib_mask & (1 << MINDEX_STACK_SEG_ALLOC)) {
        DBUG_PRINT ("MSG", ("STACK SEG ALLOC measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_allocate_d_stack_segment = m_allocate_d_stack_segment; }
      else
        d_m_allocate_d_stack_segment = allocate_d_stack_segment;

      if (d_bib_mask & (1 << MINDEX_STACK_SEG_FREE)) {
        DBUG_PRINT ("MSG", ("STACK SEG FREE measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_free_d_stack_segment = m_free_d_stack_segment; }
      else
        d_m_free_d_stack_segment = free_d_stack_segment;

      if (d_bib_mask & (1 << MINDEX_STACK_PUSH)) {
        DBUG_PRINT ("MSG", ("STACK PUSH measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_stack_push = m_stack_push; }
      else
        d_m_stack_push = 0;
 
      if (d_bib_mask & (1 << MINDEX_STACK_POP)) {
        DBUG_PRINT ("MSG", ("STACK POP measure initiated"));
#if (!D_MSTACK)
        D_MESS_DISABLED("stack");
#endif
        d_m_stack_pop = m_stack_pop; }
      else
        d_m_stack_pop = 0;

/* #endif D_MSTACK */

/* #if D_MSCHED */

      if (d_bib_mask & (1 << MINDEX_PROC_DIST)) {
        DBUG_PRINT ("MSG", ("PROC DIST measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_distribute = m_distribute; }
      else
        d_m_distribute = distribute;

      if (d_bib_mask & (1 << MINDEX_PROC_CREATE)) {
        DBUG_PRINT ("MSG", ("PROC CREATE measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_create = m_process_create; }
      else
        d_m_process_create = process_create;

      if (d_bib_mask & (1 << MINDEX_PROC_TERMINATE)) {
        DBUG_PRINT ("MSG", ("PROC TERMINATE measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_terminate = m_process_terminate; }
      else
        d_m_process_terminate = process_terminate;

      if (d_bib_mask & (1 << MINDEX_PROC_SLEEP)) {
        DBUG_PRINT ("MSG", ("PROC SLEEP measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_sleep = m_process_sleep; }
      else
        d_m_process_sleep = process_sleep;

      if (d_bib_mask & (1 << MINDEX_PROC_RUN)) {
        DBUG_PRINT ("MSG", ("PROC RUN measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_run = m_process_run; }
      else
        d_m_process_run = process_run; 

      if (d_bib_mask & (1 << MINDEX_PROC_WAKEUP)) {
        DBUG_PRINT ("MSG", ("PROC WAKEUP measure enabled !"));
#if (!D_MSCHED)
        D_MESS_DISABLED("scheduler");
#endif
        d_m_process_wakeup = m_process_wakeup; }
      else
        d_m_process_wakeup = process_wakeup; 

/* #endif  D_MSCHED */

/* #if D_MCOMMU */
 
      d_m_msg_send = msg_send;
      d_m_msg_receive = msg_receive;

      if (d_bib_mask & (1 << MINDEX_SEND_BEGIN)) {
        DBUG_PRINT ("MSG", ("SEND BEGIN measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_send = m_msg_send; 
        m_msg_send_begin = 1; }
      else
        m_msg_send_begin = 0;

      if (d_bib_mask & (1 << MINDEX_SEND_END)) {
        DBUG_PRINT ("MSG", ("SEND END measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_send = m_msg_send; 
        m_msg_send_end = 1; }
      else
        m_msg_send_end = 0;

      if (d_bib_mask & (1 << MINDEX_RECEIVE_BEGIN)) {
        DBUG_PRINT ("MSG", ("RECEIVE BEGIN measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_receive = m_msg_receive; 
        m_msg_receive_begin = 1; }
      else
        m_msg_receive_begin = 0;

      if (d_bib_mask & (1 << MINDEX_RECEIVE_END)) {
        DBUG_PRINT ("MSG", ("RECEIVE END measure enabled !"));
#if (!D_MCOMMU)
        D_MESS_DISABLED("communication");
#endif
        d_m_msg_receive = m_msg_receive; 
        m_msg_receive_end = 1; }
      else
        m_msg_receive_end = 0;

/* #endif  D_MCOMMU */ 

#endif

      if (DO_RECEIVE() != END_MARKER)
	/* error !!! */;


      break;

#if D_MESS
    case MT_CLOSE_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_CLOSE_MEASURE !"));

      /* switch off signal handler */

      if (!m_merge_strat)
        sigaction (SIGMSGS,NULL,NULL);

      /* close measure file */

      if (fclose (d_mess_dat) == EOF)
        fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);
 
      /* turn off all measure operations */

      d_m_newheap = newheap;
      d_m_freeheap = freeheap;
      d_m_compheap = compheap;
      d_m_newdesc = newdesc;
      d_m_freedesc = freedesc;
      d_m_allocate_d_stack_segment = allocate_d_stack_segment;
      d_m_free_d_stack_segment = free_d_stack_segment;
      d_m_stack_push = 0;
      d_m_stack_pop = 0;
      d_m_distribute = distribute;
      d_m_process_create = process_create;
      d_m_process_terminate = process_terminate;
      d_m_process_sleep = process_sleep;
      d_m_process_run = process_run;
      d_m_process_wakeup = process_wakeup;
      d_m_msg_send = msg_send;
      d_m_msg_receive = msg_receive;
      m_msg_send_begin = 0;
      m_msg_send_end = 0;
      m_msg_receive_begin = 0;
      m_msg_receive_end = 0;

      sleep(1);    /* wait for one second */

      if (proc_id == 0) {       /* i am no 0 ! */
        k = 0;
        for (i = 0; i < (1 << cube_dim); i++) {
          sprintf (name_line, "%s%s_uid%ld_%i", m_mesfilepath, m_mesfilehea, getuid(), i);         
          if (stat(name_line, &stbuf) == -1)
            fprintf(stderr, "node %d: warning, cannot access measurement file %d !\n", proc_id, i);
          DBUG_PRINT ("MERGE", ("length of file %d: %ld", i, stbuf.st_size));
          if ((int) stbuf.st_size == 0)
            fprintf (stderr, "node %d: warning, file %d empty !\n\n", proc_id, i);
          k = k + (int) stbuf.st_size; }
        DBUG_PRINT ("MERGE", ("total length: %d", k));
        nwrite(&k, 4, host, MT_CLOSE_MEASURE, &flags); 
        }

      DBUG_PRINT ("MSG", ("measure file closed and measure actions terminated."));

      break;

    case MT_REMOTE_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_REMOTE_MEASURE !"));

      /* copy measure file nr to host */

      m_super_nr = *((int *)buffer);
 
      DBUG_PRINT ("S_MERGE", ("copying measurement file no %d", m_super_nr));

      sprintf (d_mess_name, "%s%s_uid%ld_%i", m_mesfilepath, m_mesfilehea, getuid(), m_super_nr);
      sprintf (m_super_name, "/tmp/%s_uid%ld_%i", m_mesfilehea, getuid(), m_super_nr);

      if ((m_super_file1 = fopen(d_mess_name, "r")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open measurement file for reading\n\n", proc_id);
      if ((m_super_file2 = fopen(m_super_name, "w")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open host measurement node file for writing\n\n", proc_id);

      while (!feof(m_super_file1)) {
        m_super_counter = fread (&m_super_buffer, 1, 4096, m_super_file1);
        fwrite (&m_super_buffer, 1, m_super_counter, m_super_file2);
        }

/* HAHAHAHAH great Code
      while ((m_super_c = fgetc(m_super_file1)) != EOF)
        fputc(m_super_c, m_super_file2); */
  
      if (fclose(m_super_file1) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measurement file\n\n", proc_id);
      if (fclose(m_super_file2) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measurement node file\n\n", proc_id);

      nwrite (&m_super_nr, 0, host, MT_CLOSE_MEASURE, &flags);

      break;

    case MT_OPEN_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_OPEN_MEASURE !"));

#ifndef M_OLD_MERGING
      m_start_merge (buffer_p);

      break; 

/****** old merging *****/

#else

      if (fclose (d_mess_dat) == EOF)
        fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

      if ((d_mess_dat = fopen (d_mess_name, "r")) == NULL)
        fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

      i = 0;
      if (fgets(mess_line, 120, d_mess_dat) == NULL) {
        DBUG_PRINT ("MSG", ("Messdatei schon hier leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

        if (fclose (d_mess_dat) == EOF)
           fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

        /* now open File again for more reductions !!!!! 
        if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
          fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

        d_mess_buff[0] = '\n';
        d_mess_buff[1] = '\0';
        i = 2;
        }
      else {              /* now read measure-file until the buffer is full */
        DBUG_PRINT ("MSG", ("Hiho, hir bin ich..."));

        while ((i + (k = strlen(mess_line))) < (D_MESS_MAXBUF - 4)) {  /* space left for delimiters */ 
          DBUG_PRINT ("MSG", ("Reading: %s; i = %d; k = %d", mess_line, i, k));
          memcpy(d_mess_buff+i, mess_line, k);
          i += k;
          d_mess_buff[i] = '\0';

          if (fgets(mess_line, 120, d_mess_dat) == NULL) {
            DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

            if (fclose (d_mess_dat) == EOF)
               fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

            /* now open File again for more reductions !!!!! 
            if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
              fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

            d_mess_buff[i++] = '\n';
            d_mess_buff[i++] = '\0';
            break;
            }
          }
        }
      
      d_mess_buff[i++] = '\0';

      DBUG_PRINT ("MSG", ("Sending %d Bytes !, strlen: %d ", i, strlen(d_mess_buff)));

      nwrite (d_mess_buff, i, host, MT_SEND_MEASURE, &flags);  

      DBUG_PRINT ("MSG", ("Hab MT_SEND_MEASURE weggesendet !"));

/*
      fprintf (stderr, "Prozessor %d sendet auf ein MT_OPEN_MEASURE hin %d Bytes !\n", proc_id, strlen(d_mess_buff));
      fflush (stderr); */

#endif /* M_OLD_MERGING */

      break;

/**************** end of old merging ************/

    case MT_SEND_MEASURE:
      DBUG_PRINT ("MSG", ("Receiving MT_SEND_MEASURE !"));
      
#ifdef M_OLD_MERGING

/************** OLD MERGING **************/    /* for ASCII only ! */

      i = 0;
      k = strlen (mess_line);
      memcpy(d_mess_buff, mess_line, k);
      i += k;

      if (fgets(mess_line, 120, d_mess_dat) == NULL) {
        DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

        if (fclose (d_mess_dat) == EOF)
           fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

        /* now open File again for more reductions !!!!! 
        if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
          fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

        d_mess_buff[0] = '\n';
        d_mess_buff[1] = '\0';
        i = 2;
        }
      else {              /* now read measure-file until the buffer is full */
        i = 0;
        while ((i + (k = strlen(mess_line))) < (D_MESS_MAXBUF - 4)) {  /* space left for del
imiters */ 
          memcpy(d_mess_buff+i, mess_line, k); 
          i += k;
          if (fgets(mess_line, 120, d_mess_dat) == NULL) {
            DBUG_PRINT ("MSG", ("Messdatei leer !"));

/* fprintf (stderr, "Prozessor %d meldet: Messdatei leer !\n", proc_id);
fflush (stderr); */

            if (fclose (d_mess_dat) == EOF)
              fprintf(stderr, "node %d: warning, cannot close measure file\n\n", proc_id);

            /* now open File again for more reductions !!!!! 
            if ((d_mess_dat = fopen (d_mess_name, "w")) == NULL)
              fprintf (stderr, "node %d: warning, cannot open measure file\n\n", proc_id);

            d_mess_buff[i] = '\n';
            d_mess_buff[i+1] = '\0';
            i += 2;
            break;
            }
          }
        }
       
      d_mess_buff[i++] = '\0';

      DBUG_PRINT ("MSG", ("Sending %d Bytes !, strlen: %d ", i, strlen(d_mess_buff)));

      nwrite (d_mess_buff, i, host, MT_SEND_MEASURE, &flags);

      DBUG_PRINT ("MSG", ("Hab MT_SEND_MEASURE weggesendet !"));

/*     fprintf (stderr, "Prozessor %d sendet auf ein MT_SEND_MEASURE hin %d Bytes !\n", proc_id, strlen(d_mess_buff));
     fflush (stderr); */

#endif /* M_OLD_MERGING */

/****************** end of old merging ******************/

      break;
#endif
    }                                                                   /* switch (type) */

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MCOMMU)
  if (m_msg_receive_end)
    MPRINT_RECEIVE_END(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, m_type, src, m_pid, m_size, f_pid);
#endif
#endif

 DBUG_VOID_RETURN;
}

#ifndef D_MESS_ONLY

/***************************************************************/
/*                                                             */
/* msg_check()    checks for incoming messages and calls       */
/*                msg_receive if a message has arrived         */
/*                                                             */
/* called by:  scheduler or signal handler                     */
/*                                                             */
/***************************************************************/

void msg_check()
{
int source=-1;
int type=-1;
/*dmk*/ int erg=0;

 while ((erg = ntest(&source,&type))>=0)
   {
     DBUG_PRINT("NN",("msg_check - erg : %d / type : %d " , erg, type)); 
#ifdef D_NN_SCHED
     if (type == MT_STAT_REQ) {
       proc_org=-1;
       nn_msg_receive(MT_STAT_REQ);
       nn_msg_send(proc_org, MT_STAT_SEND);
     } else if (type == MT_STAT_SEND) {
       nn_msg_receive(MT_STAT_SEND);
     } else {
#endif
#if (D_SLAVE && D_MESS && D_MCOMMU)
        (*d_m_msg_receive)();
#else
        msg_receive();
#endif
#ifdef D_NN_SCHED
    }
#endif
    source=type=-1;
   } /* of while */
 sig_msg_flag = 0;
}

/***************************************************************/
/*                                                             */
/* flush_queue()    waits until a message of type MT_PARAM is  */
/*                  received; dismisses any other incoming     */
/*                  message                                    */
/*                                                             */
/*   callled in case of post_mortem                            */
/*                                                             */
/***************************************************************/

void flush_queue()
{char *buffer;
 int src,type,flags;

 DBUG_ENTER("flush_queue");

 for(;;)
   {do
      src = type = -1;
    while (ntest(&src,&type) < 0);
    
    if ((type == MT_PARAM) || (type == MT_TERMINATE))
      {sig_msg_flag = 1;
       DBUG_VOID_RETURN;}
    
    nreadp(&buffer,MESSAGE_SIZE * sizeof(int),&src,&type,&flags);
    nrelp(buffer);}}
      
#endif /* D_MESS_ONLY */
