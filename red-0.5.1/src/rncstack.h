#ifndef _rncstack_h_included  /* finally done by RS */
#define _rncstack_h_included

typedef  /*unsigned*/ int STACKELEM;

typedef struct { STACKELEM * TopofStack;    /* Das letzte belegte Element */
		 STACKELEM * TopofSeg;      /* Die letzte Adresse, die noch zum Segment gehoert */
                 STACKELEM * BotofSeg;      /* An dieser Adresse steht der Zeiger auf das vorige Segment */
		 char        name[8];}
        DStackDesc;

extern STACKELEM _stelem;                   /* rncstack.c */

#define D_TOP_ADDRESS(Onstack)        (Onstack).TopofStack

#if DEBUG

#define D_READSTACK(Onstack)          d_readstack_debug(&(Onstack))
#define D_WRITESTACK(Onstack,Data)    d_writestack_debug(&(Onstack),(STACKELEM)Data)
#define D_POPSTACK(Onstack)           d_popstack_debug(&(Onstack))
#define D_PPOPSTACK(Onstack)          d_ppopstack_debug(&(Onstack))
#define D_PUSHSTACK(Onstack,Data)     d_pushstack_debug(&(Onstack),(STACKELEM)Data)
#define D_PPUSHSTACK(Onstack,Data)    d_pushstack_debug(&(Onstack),(STACKELEM)Data)
#define D_UPDATESTACK(Onstack,Offset,Data)        d_updatestack_debug(&(Onstack),Offset,(STACKELEM)Data))
#define D_MIDSTACK(Onstack,Offset)    d_midstack_debug(&(Onstack),(Offset))

#else  /* DEBUG */

#define D_READSTACK(Onstack)          *(Onstack).TopofStack
#define D_WRITESTACK(Onstack,Data)    *(Onstack).TopofStack = (STACKELEM)Data

#if (D_MESS && nCUBE && D_MSTACK)
#define D_POPSTACK(Onstack)           (_stelem = *(Onstack).TopofStack-- , \
                                      (d_m_stack_pop == m_stack_pop) ? \
                                      MPRINT_STACK_POP (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID,(Onstack).name) : 1 , \
                                      ((Onstack).TopofStack == (Onstack).BotofSeg) ? \
                                      ((*d_m_free_d_stack_segment)(&(Onstack)) , _stelem) : _stelem)
#else
#define D_POPSTACK(Onstack)           (_stelem = *(Onstack).TopofStack-- , ((Onstack).TopofStack == (Onstack).BotofSeg) ? \
                                      (free_d_stack_segment(&(Onstack)) , _stelem) : _stelem)
#endif

/*#define D_POPSTACK(Onstack)           *(Onstack).TopofStack--*/

#if (D_MESS && nCUBE && D_MSTACK)
#define D_PPOPSTACK(Onstack)          do { if (d_m_stack_pop == m_stack_pop) \
                                             MPRINT_STACK_POP(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID,(Onstack).name); \
                                         if (--((Onstack).TopofStack) == (Onstack).BotofSeg) (*d_m_free_d_stack_segment)(&(Onstack)); } while(0) 
#else
#define D_PPOPSTACK(Onstack)          do if (--((Onstack).TopofStack) == (Onstack).BotofSeg) free_d_stack_segment(&(Onstack)); while(0)
#endif

/*#define D_PPOPSTACK(Onstack)          (Onstack).TopofStack--*/

#if (D_MESS && nCUBE && D_MSTACK)
/* #define D_PPUSHSTACK(Onstack,Data)     (d_m_stack_push == m_stack_push) ?  \
                                       MPRINT_STACK_PUSH (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID,(Onstack).name) : 1 ,  \ */

#define D_PPUSHSTACK(Onstack,Data)    ((Onstack).TopofStack == (Onstack).TopofSeg) ? \
                                      ( (d_m_stack_push == m_stack_push) ? \
                                          MPRINT_STACK_PUSH (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID,(Onstack).name) : 1) , \
                                      ((*d_m_allocate_d_stack_segment)(&(Onstack)) , *++((Onstack).TopofStack) = (STACKELEM)(Data)) : \
                                      ( (d_m_stack_push == m_stack_push) ? \
                                          MPRINT_STACK_PUSH (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID,(Onstack).name) : 1) , \
                                      (*++((Onstack).TopofStack) = (STACKELEM)(Data))

#else
#define D_PPUSHSTACK(Onstack,Data)    ((Onstack).TopofStack == (Onstack).TopofSeg) ? \
                                      (allocate_d_stack_segment(&(Onstack)) , *++((Onstack).TopofStack) = (STACKELEM)(Data)) : \
                                      (*++((Onstack).TopofStack) = (STACKELEM)(Data))
#endif

/*#define D_PPUSHSTACK(Onstack,Data)    *++((Onstack).TopofStack) = (STACKELEM)(Data)*/
#define D_PUSHSTACK(Onstack,Data)     D_PPUSHSTACK(Onstack,Data)
#define D_UPDATESTACK(Onstack,Offset,Data)  *((((Onstack).TopofStack - (Offset)) <= (Onstack).BotofSeg) ? \
                                              ((STACKELEM *)(*(Onstack).BotofSeg) - ((Offset) - ((Onstack).TopofStack - (Onstack).BotofSeg))) : \
                                              ((Onstack).TopofStack-(Offset))) = (STACKELEM)(Data)
#define D_MIDSTACK(Onstack,Offset)    ((((Onstack).TopofStack - (Offset)) <= (Onstack).BotofSeg) ? \
                                       (*((STACKELEM *)(*(Onstack).BotofSeg) - ((Offset) - ((Onstack).TopofStack - (Onstack).BotofSeg)))) : \
                                       (*((Onstack).TopofStack-(Offset))))
/*#define D_UPDATESTACK(Onstack,Offset,Data) *((Onstack).TopofStack-(Offset)) = (STACKELEM)(Data)
#define D_MIDSTACK(Onstack,Offset)         (*((Onstack).TopofStack-(Offset)))*/

#endif /* DEBUG */
#define D_SIZEOFSTACK(Onstack)        d_stacksize(&(Onstack))

#define IS_SPACE(Onstack,size)

#endif
