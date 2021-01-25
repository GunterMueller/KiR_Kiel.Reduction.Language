/****************************************************************/
/*                                                              */
/* rncstack.c --- stack manager for the nCube                   */
/*                                                              */
/* ach 30/12/92                                                 */
/*                                                              */
/****************************************************************/
#include "rstdinc.h"
#include "rncstack.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

#include <stdlib.h>
#include <string.h>
#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#ifndef STACK_SEG_SIZE
#define STACK_SEG_SIZE 9000     /* Groesse eines Stacksegments */
#endif

#ifndef D_MESS_ONLY

DStackDesc D_S_H,D_S_A,D_S_R,D_S_I,D_S_hilf; /* Die globalen Stacks */
DStackDesc *ps_a,*ps_i;
STACKELEM *free_stack_list,*stack_area=0;
STACKELEM _stelem;            /* quick & dirty, car will das so */

#else

extern STACKELEM *free_stack_list;
extern STACKELEM *stack_area;

#endif

#if DEBUG
extern void display_stack();
#endif

#ifndef D_MESS_ONLY

void alloc_stack_memory(size)
int size;

{int real_size;
 STACKELEM *hilf;

 DBUG_ENTER("alloc_stack_memory");

 /* free old stack area, if any */

 if (stack_area)
   free(stack_area);

 /* Der Speicherbereich soll vollstaendig in Segmente zerlegt werden koennen */
 real_size = ((size/STACK_SEG_SIZE) + 1) * STACK_SEG_SIZE;

 if (!(free_stack_list = stack_area = (STACKELEM *)malloc(real_size*sizeof(STACKELEM))))
   post_mortem("Dynamic Stackmanager: Unable to allocate space for dynamic stacks");

 /* Initialisieren der Liste */
 for (hilf=free_stack_list;(hilf+STACK_SEG_SIZE) < (free_stack_list+real_size);hilf+=STACK_SEG_SIZE)
   *hilf=(STACKELEM)(hilf+STACK_SEG_SIZE);

 *hilf=(STACKELEM)NULL;
 DBUG_VOID_RETURN;}

void init_d_stacks(stack_e,stack_a,stack_m,stack_m1)
DStackDesc *stack_e,*stack_a,*stack_m,*stack_m1;

{DBUG_ENTER("init_d_stacks");

 if (!(stack_e->BotofSeg = stack_e->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_e->BotofSeg) = (STACKELEM) NULL;
 stack_e->TopofSeg = stack_e->BotofSeg+STACK_SEG_SIZE-1;
 strcpy(stack_e->name,"H");

 if (!(stack_a->BotofSeg = stack_a->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_a->BotofSeg) = (STACKELEM) NULL;
 stack_a->TopofSeg = stack_a->BotofSeg+STACK_SEG_SIZE-1;
 strcpy(stack_a->name,"A"); 
 
 if (!(stack_m->BotofSeg = stack_m->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_m->BotofSeg) = (STACKELEM) NULL;
 stack_m->TopofSeg = stack_m->BotofSeg+STACK_SEG_SIZE-1;
 strcpy(stack_m->name,"R"); 

 if (!(stack_m1->BotofSeg = stack_m1->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_m1->BotofSeg) = (STACKELEM) NULL;
 stack_m1->TopofSeg = stack_m1->BotofSeg+STACK_SEG_SIZE-1;
 strcpy(stack_m1->name,"I"); 

 DBUG_VOID_RETURN;}

void init_d_stack(stack)
DStackDesc *stack;

{DBUG_ENTER("init_d_stack");

 if (!(stack->BotofSeg = stack->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack->BotofSeg) = (STACKELEM) NULL;
 stack->TopofSeg = stack->BotofSeg+STACK_SEG_SIZE-1;
 DBUG_VOID_RETURN;}

void free_d_stacks(stack_e,stack_a,stack_m,stack_m1)
DStackDesc *stack_e,*stack_a,*stack_m,*stack_m1;

{DBUG_ENTER("free_d_stacks");

 while (stack_e->BotofSeg)
  {stack_e->TopofStack = (STACKELEM *)(*(stack_e->BotofSeg));
   *(stack_e->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_e->BotofSeg;
   stack_e->BotofSeg = (stack_e->TopofStack)?(STACKELEM *)((((stack_e->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area):NULL;}

 while (stack_a->BotofSeg)
  {stack_a->TopofStack = (STACKELEM *)(*(stack_a->BotofSeg));
   *(stack_a->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_a->BotofSeg;
   stack_a->BotofSeg = (stack_a->TopofStack)?(STACKELEM *)((((stack_a->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area):NULL;}

 while (stack_m->BotofSeg)
  {stack_m->TopofStack = (STACKELEM *)(*(stack_m->BotofSeg));
   *(stack_m->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_m->BotofSeg;
   stack_m->BotofSeg = (stack_m->TopofStack)?(STACKELEM *)((((stack_m->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area):NULL;}

 while (stack_m1->BotofSeg)
  {stack_m1->TopofStack = (STACKELEM *)(*(stack_m1->BotofSeg));
   *(stack_m1->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_m1->BotofSeg;
   stack_m1->BotofSeg = (stack_m1->TopofStack)?(STACKELEM *)((((stack_m1->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area):NULL;}
 DBUG_VOID_RETURN;}

void free_d_stack(stack)
DStackDesc *stack;

{DBUG_ENTER("free_d_stack");
 while (stack->BotofSeg)
  {stack->TopofStack = (STACKELEM *)(*(stack->BotofSeg));
   *(stack->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack->BotofSeg;
   stack->BotofSeg = (stack->TopofStack)?(STACKELEM *)((((stack->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area):NULL;}
 DBUG_VOID_RETURN;}

#endif /* D_MESS_ONLY */

#ifdef D_MESS_ONLY

void m_allocate_d_stack_segment(stack)
DStackDesc *stack;

#else

void allocate_d_stack_segment(stack)
DStackDesc *stack;

#endif

{STACKELEM *hilf;

#ifdef D_MESS_ONLY
 DBUG_ENTER("m_allocate_d_stack_segment");
#else
 DBUG_ENTER("allocate_d_stack_segment");
#endif

 if (!(hilf=free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");

#ifdef D_MESS_ONLY
#if (nCUBE && D_MESS && D_MSTACK)
  MPRINT_STACK_SEG_ALLOC (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, stack->name);
#endif
#endif

 free_stack_list = (STACKELEM *)*free_stack_list;
 *hilf = (STACKELEM)stack->TopofSeg;
 stack->TopofStack = stack->BotofSeg = hilf;
 stack->TopofSeg = hilf+STACK_SEG_SIZE-1;
 DBUG_VOID_RETURN;}

#ifdef D_MESS_ONLY

void m_free_d_stack_segment(stack)
DStackDesc *stack;

#else

void free_d_stack_segment(stack)
DStackDesc *stack;

#endif

{
#ifdef D_MESS_ONLY 
 DBUG_ENTER("m_free_d_stack_segment");
#else
 DBUG_ENTER("free_d_stack_segment");
#endif

 DBUG_PRINT("STACK",("TopofStack: %x; BotofSeg: %x",stack->TopofStack,stack->BotofSeg));


 if (!(*(stack->BotofSeg)))                     /* Wenn das einzige Segment freigegeben werden soll, wird nichts getan */
   DBUG_VOID_RETURN;  
 
 DBUG_PRINT("STACK",("Segment wech!"));

#ifdef D_MESS_ONLY
#if (nCUBE && D_MESS && D_MSTACK)
  MPRINT_STACK_SEG_FREE (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, stack->name);
#endif
#endif

 stack->TopofStack = (STACKELEM *)(*(stack->BotofSeg));
 *(stack->BotofSeg) = (STACKELEM)free_stack_list;
 free_stack_list = stack->BotofSeg;
 stack->BotofSeg = (STACKELEM *)((((stack->TopofStack-stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area);
 stack->TopofSeg = stack->BotofSeg+STACK_SEG_SIZE-1;
 DBUG_VOID_RETURN;}

#ifndef D_MESS_ONLY

int d_stacksize(stack)
DStackDesc *stack;

{STACKELEM *pointer, *bottom;
 int size=0;

 DBUG_ENTER("d_stacksize");
 pointer = stack->TopofStack;

 while (pointer)
   {bottom = (STACKELEM *)((((pointer - stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area);
    size += pointer - bottom;
    pointer =(STACKELEM *) *bottom;}

 DBUG_RETURN(size);}

STACKELEM d_midstack_debug(stack,offset)
DStackDesc *stack;
int offset;

{STACKELEM *pointer,*bottom;
 int seg_size;

 DBUG_ENTER("d_midstack_debug");

 pointer = stack->TopofStack;
 bottom = stack->BotofSeg;
 seg_size = pointer - bottom;

 while (seg_size <= offset)
   {offset -= seg_size;
    pointer =(STACKELEM *) *bottom;
    bottom = (STACKELEM *)((((pointer - stack_area)/STACK_SEG_SIZE)*STACK_SEG_SIZE)+stack_area);
    seg_size = pointer - bottom;}

 DBUG_RETURN(*(pointer - offset));}


 
#endif /* D_MESS_ONLY */






















