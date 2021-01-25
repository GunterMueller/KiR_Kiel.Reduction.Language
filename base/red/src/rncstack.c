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

#if D_SPEC
extern FILE *debugger;
#endif
extern int curr_pid;

#ifndef D_MESS_ONLY

DStackDesc D_S_H,D_S_A,D_S_R,D_S_I,D_S_hilf; /* Die globalen Stacks */
STACKELEM *free_stack_list,*stack_area=0;
STACKELEM _stelem;            /* quick & dirty, car will das so */
int stack_seg_size;

#else

extern STACKELEM *free_stack_list;
extern STACKELEM *stack_area;
extern int stack_seg_size;

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
 real_size = ((size/stack_seg_size) + 1) * stack_seg_size;

 if (!(free_stack_list = stack_area = (STACKELEM *)malloc(real_size*sizeof(STACKELEM))))
   post_mortem("Dynamic Stackmanager: Unable to allocate space for dynamic stacks");

 /* Initialisieren der Liste */
 for (hilf=free_stack_list;(hilf+stack_seg_size) < (free_stack_list+real_size);hilf+=stack_seg_size)
   *hilf=(STACKELEM)(hilf+stack_seg_size);

 *hilf=(STACKELEM)NULL;
 DBUG_VOID_RETURN;}

void init_d_stacks(stack_e,stack_a,stack_m,stack_m1)
DStackDesc *stack_e,*stack_a,*stack_m,*stack_m1;

{DBUG_ENTER("init_d_stacks");

/* fprintf(debugger, "0x%x: *STACK* allocating a new stack system\n", curr_pid); fflush(debugger); */

 if (!(stack_e->BotofSeg = stack_e->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_e->BotofSeg) = (STACKELEM) NULL;
 stack_e->TopofSeg = stack_e->BotofSeg+stack_seg_size-1;
 strcpy(stack_e->name,"H");

/* fprintf(debugger, "0x%x: *STACK* first segment of stack H (%x) is %x\n", curr_pid, stack_e, stack_e->BotofSeg); fflush(debugger); */

 if (!(stack_a->BotofSeg = stack_a->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_a->BotofSeg) = (STACKELEM) NULL;
 stack_a->TopofSeg = stack_a->BotofSeg+stack_seg_size-1;
 strcpy(stack_a->name,"A"); 

/* fprintf(debugger, "0x%x: *STACK* first segment of stack A (%x) is %x\n", curr_pid, stack_a, stack_a->BotofSeg); fflush(debugger); */
 
 if (!(stack_m->BotofSeg = stack_m->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_m->BotofSeg) = (STACKELEM) NULL;
 stack_m->TopofSeg = stack_m->BotofSeg+stack_seg_size-1;
 strcpy(stack_m->name,"R"); 

/* fprintf(debugger, "0x%x: *STACK* first segment of stack R (%x) is %x\n", curr_pid, stack_m, stack_m->BotofSeg); fflush(debugger); */

 if (!(stack_m1->BotofSeg = stack_m1->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack_m1->BotofSeg) = (STACKELEM) NULL;
 stack_m1->TopofSeg = stack_m1->BotofSeg+stack_seg_size-1;
 strcpy(stack_m1->name,"I"); 

/* fprintf(debugger, "0x%x: *STACK* first segment of stack I (%x) is %x\n", curr_pid, stack_m1, stack_m1->BotofSeg); fflush(debugger); */

 DBUG_VOID_RETURN;}

void init_d_stack(stack)
DStackDesc *stack;

{DBUG_ENTER("init_d_stack");

 if (!(stack->BotofSeg = stack->TopofStack = free_stack_list))
   post_mortem("Dynamic Stackmanager: List of free stack segments empty");
 free_stack_list = (STACKELEM *)*free_stack_list;
 *(stack->BotofSeg) = (STACKELEM) NULL;
 stack->TopofSeg = stack->BotofSeg+stack_seg_size-1;

/* fprintf(debugger, "0x%x: *STACK* first segment of stack (%s, %x) is %x\n", curr_pid, stack->name, stack, stack->BotofSeg); fflush(debugger); */

 DBUG_VOID_RETURN;}

void free_d_stacks(stack_e,stack_a,stack_m,stack_m1)
DStackDesc *stack_e,*stack_a,*stack_m,*stack_m1;

{DBUG_ENTER("free_d_stacks");

/* fprintf(debugger, "0x%x: *STACK* free_d_stacks stacksizes stack_e (%s, %x): %d, stack_a (%s, %x): %d, stack_m (%s, %x): %d, stack_m1 (%s, %x): %d\n", curr_pid, stack_e->name, stack_e, d_stacksize(stack_e), stack_a->name, stack_a, d_stacksize(stack_a), stack_m->name, stack_m, d_stacksize(stack_m), stack_m1->name, stack_m1, d_stacksize(stack_m1)); fflush(debugger); */

 while (stack_e->BotofSeg)
  {stack_e->TopofStack = (STACKELEM *)(*(stack_e->BotofSeg));
   *(stack_e->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_e->BotofSeg;
   stack_e->BotofSeg = (stack_e->TopofStack)?(STACKELEM *)((((stack_e->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area):NULL;}

/* fprintf(debugger, "0x%x: *STACK* stack_e cleared !\n", curr_pid); fflush(debugger); */

 while (stack_a->BotofSeg)
  {stack_a->TopofStack = (STACKELEM *)(*(stack_a->BotofSeg));
   *(stack_a->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_a->BotofSeg;
   stack_a->BotofSeg = (stack_a->TopofStack)?(STACKELEM *)((((stack_a->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area):NULL;}

/* fprintf(debugger, "0x%x: *STACK* stack_a cleared !\n", curr_pid); fflush(debugger); */

 while (stack_m->BotofSeg)
  {
 
/* fprintf(debugger, "0x%x: *STACK* stack_m->BotofSeg %x\n", curr_pid, stack_m->BotofSeg); fflush(debugger); */

   stack_m->TopofStack = (STACKELEM *)(*(stack_m->BotofSeg));
   *(stack_m->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_m->BotofSeg;
   stack_m->BotofSeg = (stack_m->TopofStack)?(STACKELEM *)((((stack_m->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area):NULL;}

/* fprintf(debugger, "0x%x: *STACK* stack_m cleared !\n", curr_pid); fflush(debugger); */

 while (stack_m1->BotofSeg)
  {stack_m1->TopofStack = (STACKELEM *)(*(stack_m1->BotofSeg));
   *(stack_m1->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack_m1->BotofSeg;
   stack_m1->BotofSeg = (stack_m1->TopofStack)?(STACKELEM *)((((stack_m1->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area):NULL;}

/* fprintf(debugger, "0x%x: *STACK* stack_m1 cleared !\n", curr_pid); fflush(debugger); */

 DBUG_VOID_RETURN;}

void free_d_stack(stack)
DStackDesc *stack;

{DBUG_ENTER("free_d_stack");

post_mortem ("whaaat ?!");

/* fprintf(debugger, "0x%x: *STACK* free_d_stack, freeing stack %s (bot_seg is %x)!\n", curr_pid, stack->name, stack->BotofSeg); fflush(debugger); */

 while (stack->BotofSeg)
  {stack->TopofStack = (STACKELEM *)(*(stack->BotofSeg));
   *(stack->BotofSeg) = (STACKELEM)free_stack_list;
   free_stack_list = stack->BotofSeg;
   stack->BotofSeg = (stack->TopofStack)?(STACKELEM *)((((stack->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area):NULL;}
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
#if (D_SLAVE && D_MESS && D_MSTACK)
  MPRINT_STACK_SEG_ALLOC (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, stack->name);
#endif
#endif

 free_stack_list = (STACKELEM *)*free_stack_list;
 *hilf = (STACKELEM)stack->TopofSeg;
 stack->TopofStack = stack->BotofSeg = hilf;

/* fprintf(debugger, "0x%x: *STACK* allocating new segment %x for stack %s (%x)\n", curr_pid, hilf, stack->name, stack); fflush(debugger);  */

 stack->TopofSeg = hilf+stack_seg_size-1;
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

/* fprintf(debugger, "0x%x: *STACK* trying to free a segment of stack %s (%x), BotofSeg: %x\n", curr_pid, stack->name, stack, stack->BotofSeg); fflush(debugger);  */


 if (!(*(stack->BotofSeg)))           {         /* Wenn das einzige Segment freigegeben werden soll, wird nichts getan */

/* fprintf(debugger, "0x%x: trying to free a last segment of %s, don't do it...\n", curr_pid, stack->name); fflush(debugger);  */

   DBUG_PRINT("free", ("It was the first segment, don't free it !"));
   DBUG_VOID_RETURN;  }
 
 DBUG_PRINT("STACK",("Segment wech!"));

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MSTACK)
  MPRINT_STACK_SEG_FREE (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, stack->name);
#endif
#endif

 stack->TopofStack = (STACKELEM *)(*(stack->BotofSeg));
 *(stack->BotofSeg) = (STACKELEM)free_stack_list;
 free_stack_list = stack->BotofSeg;
 stack->BotofSeg = (STACKELEM *)((((stack->TopofStack-stack_area)/stack_seg_size)*stack_seg_size)+stack_area);

/* fprintf(debugger, "0x%x: *STACK* segment free'd, new BotofSeg is: %x (value %x)\n", curr_pid, stack->BotofSeg, *(stack->BotofSeg)); fflush(debugger);  */

 stack->TopofSeg = stack->BotofSeg+stack_seg_size-1;
 DBUG_VOID_RETURN;}

#ifndef D_MESS_ONLY

int d_stacksize(stack)
DStackDesc *stack;

{STACKELEM *pointer, *bottom;
 int size=0;

 DBUG_ENTER("d_stacksize");
 pointer = stack->TopofStack;

/* fprintf(debugger, "0x%x: *STACK* d_stacksize was called... for stack %x (%s)\n", curr_pid, stack, stack->name); fflush(debugger); */

/* fprintf(debugger, "*STACK*: TopOfSegment %x, TopOfStack %x, Bottom %x (*Bottom %x)\n", stack->TopofSeg, stack->TopofStack, stack->BotofSeg, *(stack->BotofSeg)); fflush(debugger); */

 while (pointer)
   {bottom = (STACKELEM *)((((pointer - stack_area)/stack_seg_size)*stack_seg_size)+stack_area);

/* fprintf(debugger, "*STACK*: inside loop, bottom is %x, pointer-bottom is %d\n", bottom, pointer-bottom); fflush(debugger); */

    size += pointer - bottom;
    pointer =(STACKELEM *) *bottom;

/* fprintf(debugger, "*STACK*: inside loop, new pointer is %x\n", pointer); fflush(debugger); */

    }

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
    bottom = (STACKELEM *)((((pointer - stack_area)/stack_seg_size)*stack_seg_size)+stack_area);
    seg_size = pointer - bottom;}

 DBUG_RETURN(*(pointer - offset));}


 
#endif /* D_MESS_ONLY */
