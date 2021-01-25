/* $Log: rstackty.h,v $
 * Revision 1.5  1994/01/04  13:15:54  mah
 * READ_E,READ_M,READ_V added
 *
 * Revision 1.4  1994/01/04  12:45:35  mah
 * tilde stack manager added
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:51:03  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*****************************************************************************/
/*                                                                           */
/*   In dieser Datei stehen die Stacktypen und alle Makros sowie als extern  */
/*   deklarierten Stackmodule, und sie ist zu includen, falls in einer Datei */
/*   der Stackmanager benoetigt wird.                                        */
/*                                           Hurck                           */
/*****************************************************************************/

/* last update  by     why

   copied       by     to

*/

/* die Stacktypen */

typedef  /*unsigned*/ int STACKELEM;

typedef  struct {
          STACKELEM  * TopofStack;
          STACKELEM  * MarTopPtr;
          STACKELEM  * MarBotPtr;
               char    Name[8];
                }  StackDesc;


#if WITHTILDE
#define  READ_E(pos,var)  (var = *(S_e.TopofStack - pos))
#define  READ_M(pos,var)  ( var = *(S_m.TopofStack - pos ))
#define  READ_V(pos,var)  (var = *(S_v.TopofStack - pos))
#endif /* WITHTILDE */
#if    DEBUG

#define READSTACK(Onstack)        readstack(&(Onstack))
#define WRITESTACK(Onstack,data)  writestack(&(Onstack),(STACKELEM)data)
#define POPSTACK(Onstack)         popstack(&(Onstack))
#define PPOPSTACK(Onstack)        ppopstack(&(Onstack)) /* TB, 30.10.92 */
#define PUSHSTACK(Onstack,data)   pushstack(&(Onstack),(STACKELEM)data)
#define PPUSHSTACK(Onstack,data)  pushstack(&(Onstack),(STACKELEM)data)
/* dg */
#define MIDSTACK(Onstack,Offset)  midstack(&(Onstack),Offset)
#define UPDATESTACK(Onstack,Offset,Data) updatestack(&(Onstack),Offset,Data)

#define SIZEOFSTACK(Onstack)      sizeofstack(&(Onstack))
#define READ_ARITY(Onstack)       read_arity(&(Onstack))
#define WRITE_ARITY(Onstack,data) write_arity(&(Onstack),(STACKELEM)data)

#else /* DEBUG */

/* Zu den Stackmakros  WRITESTACK, PUSHSTACK und PPUSHSTACK ist folgendes
   zu bemerken: Auf keinen Fall darf das Argument 'data' in irgendeiner
   Form den Stack manipulieren, auf das es gelegt werden soll !!!!!!!!!!!! */

#define READSTACK(Onstack)        *(Onstack).TopofStack
#define WRITESTACK(Onstack,data)  *(Onstack).TopofStack=(STACKELEM)(data)
#define POPSTACK(Onstack)         *((Onstack).TopofStack)--
#define PPOPSTACK(Onstack)        ((Onstack).TopofStack)--
#define PPUSHSTACK(Onstack,data)  *++((Onstack).TopofStack)=(STACKELEM)(data)
#define PUSHSTACK(Onstack,data)   if ((Onstack).TopofStack>=(Onstack).MarTopPtr) \
             stack_error("stackmanager: Stackoverflow on Stack",(Onstack).Name); \
             else  *++((Onstack).TopofStack)=(STACKELEM)(data)
/* dg */
#define MIDSTACK(Onstack,Offset)  *((Onstack).TopofStack-(Offset))
#define UPDATESTACK(Onstack,Offset,Data) *((Onstack).TopofStack-(Offset))=(STACKELEM)(Data)

#define SIZEOFSTACK(Onstack)      (int)((Onstack).TopofStack-(Onstack).MarBotPtr)
#if ODDSEX  /* hier werden die letzten Nanosekunden herausgequetscht */
#define READ_ARITY(Onstack)       ARITY(READSTACK(Onstack))
#define WRITE_ARITY(Onstack,data) WRITESTACK(Onstack,SET_ARITY(READSTACK(Onstack),data))
#else
#define READ_ARITY(Onstack)       *(short *)(Onstack).TopofStack
#define WRITE_ARITY(Onstack,data) *(short *)(Onstack).TopofStack=(data)
#endif /* ODDSEX */
#endif /* DEBUG  */

/* IS_SPACE gibt es nur als Makro. Es 'garantiert', dass auf einem Stack 'st'
   noch Platz fuer 'size' Stackelemente ist. Verwendet wird es fuer
   ptrav-Routinen und in load_expr.                                         */
#define IS_SPACE(st,size)  do {if (((st.TopofStack) + size) >= st.MarTopPtr) \
                              stack_error("IS_SPACE: Stackoverflow on Stack",st.Name);} while(0)

/*-----------------------------------------------------------------------------
 * Inkarnationsstackmanager:
 *-----------------------------------------------------------------------------
 */

#define SIZEOFSTACK_I       SIZEOFSTACK(S_i)

#if DEBUG
/* Bereitet das Anlegen eines Bindungsblockes auf dem I-Stack vor.
   Es 'garantiert' das den noetige Platz vorhanden ist, und legt
   einen Klammeraffen als Trennsymbol auf den I-Stack.            */

#if MEASURE
#define PUSH_I_BLOCK(size)  { if (((S_i.TopofStack) + size) >= S_i.MarTopPtr) \
     post_mortem("Inkarnationstackoverflow");    \
   else PUSHSTACK(S_i,KLAA); \
   if (size > minc) minc = size; }
#else
#define PUSH_I_BLOCK(size)  if (((S_i.TopofStack) + size) >= S_i.MarTopPtr) \
                              post_mortem("Inkarnationstackoverflow");    \
                            else PUSHSTACK(S_i,KLAA)
#endif

#define PUSH_I(arg)      PUSHSTACK(S_i,arg)

/* ein POP_I wird nicht benoetigt */

/* READ_I ist fuer die Refcountbehandlung zustaendig. */
#define READ_I(pos,var) {display_stack(&S_i,READ,0);        \
                          (SIZEOFSTACK(S_i) == 0 ? post_mortem("read_i: Inkarnationstack is empty"):0); \
                         if (T_POINTER(var = *(S_i.TopofStack - pos)))  \
                          INC_REFCNT((PTR_DESCRIPTOR)var);  }

#else /* DEBUG */
#define PUSH_I_BLOCK(size)  if (((S_i.TopofStack) + size) >= S_i.MarTopPtr) \
                              post_mortem("Inkarnationstackoverflow");   \
                            else *(++S_i.TopofStack) = KLAA

#define PUSH_I(arg)      (*(++S_i.TopofStack) = arg)

#define READ_I(pos,var)  do {if (T_POINTER(var = *(S_i.TopofStack - pos)))   \
                          INC_REFCNT((PTR_DESCRIPTOR)var);} while(0)

#endif /* DEBUG */

/* POP_I_BLOCK entfernt den obersten Bindungsblock von I_Stack, und
   kuemmert sich auch um die Refcounts.                              */
#define POP_I_BLOCK(var)  for (;;) \
                            if (T_POINTER(var = POPSTACK(S_i)))     \
                                DEC_REFCNT((PTR_DESCRIPTOR) var); \
                            else     \
                                if ((var) != KLAA) ; else break 

#if WITHTILDE
/* mah 081193 */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Tildestackmanager:                                                */
/*-------------------------------------------------------------------*/

#define SIZEOFSTACK_TILDE   SIZEOFSTACK(S_tilde)

#if DEBUG
/* Bereitet das Anlegen eines Bindungsblockes auf dem Tilde-Stack vor.*/
/* Es 'garantiert' das den noetige Platz vorhanden ist, und legt      */
/* einen Klammeraffen als Trennsymbol auf den Tilde-Stack.            */

#if HEAP

#define PUSH_TILDE_BLOCK(size)  if (((S_tilde.TopofStack) + size) >= S_tilde.MarTopPtr) \
                              post_mortem("Tildestackoverflow");    \
                            else { LastTopofStack = S_tilde.TopofStack;  \
                                   PUSHSTACK(S_tilde,KLAA) ; }


#else
#define PUSH_TILDE_BLOCK(size)  if (((S_tilde.TopofStack) + size) >= S_tilde.MarTopPtr) \
                              post_mortem("Tildestackoverflow");    \
                            else PUSHSTACK(S_tilde,KLAA)
#endif     

#define PUSH_TILDE(arg)      PUSHSTACK(S_tilde,arg)

/* ein POP_TILDE wird nicht benoetigt */
/* READ_TILDE ist fuer die Refcountbehandlung zustaendig. */  

#if HEAP 
#define READ_LAST_TILDE(pos,var) {display_stack(&S_tilde,READ,0);        \
                          (SIZEOFSTACK(S_tilde) == 0 ? post_mortem("read_tilde: Tildestack is empty"):0); \
                         if (T_POINTER(var = *(LastTopofStack - pos)))  \
                          INC_REFCNT((PTR_DESCRIPTOR)var);  }
#endif

#define READ_TILDE(pos,var) {display_stack(&S_tilde,READ,0);        \
                          (SIZEOFSTACK(S_tilde) == 0 ? post_mortem("read_tilde: Tildestack is empty"):0); \
                         if (T_POINTER(var = *(S_tilde.TopofStack - pos)))  \
                          INC_REFCNT((PTR_DESCRIPTOR)var);  }

#else /* DEBUG */                                  

#if HEAP 

#define PUSH_TILDE_BLOCK(size)  if (((S_tilde.TopofStack) + size) >= S_tilde.MarTopPtr) \
                              post_mortem("Tildestackoverflow");   \
                            else { LastTopofStack = S_tilde.TopofStack;  \
                                   *(++S_tilde.TopofStack) = KLAA ; }


#else

#define PUSH_TILDE_BLOCK(size)  if (((S_tilde.TopofStack) + size) >= S_tilde.MarTopPtr) \
                              post_mortem("Tildestackoverflow");   \
                            else *(++S_tilde.TopofStack) = KLAA

#endif

#define PUSH_TILDE(arg)      (*(++S_tilde.TopofStack) = arg)         

#if HEAP

#define READ_LAST_TILDE(pos,var)  if (T_POINTER(var = *(LastTopofStack - pos)))   \
                          INC_REFCNT((PTR_DESCRIPTOR)var);else


#endif

#define READ_TILDE(pos,var)  if (T_POINTER(var = *(S_tilde.TopofStack - pos)))   \
                          INC_REFCNT((PTR_DESCRIPTOR)var);else

#endif /* DEBUG */

/*POP_TILDE_BLOCK entfernt den obersten Bindungsblock von Tilde_Stack,*/
/*und kuemmert sich auch um die Refcounts.                            */
#define POP_TILDE_BLOCK(var) for (;;) \
                            if T_POINTER(var = POPSTACK(S_tilde))     \
                                DEC_REFCNT((PTR_DESCRIPTOR) var); \
                            else     \
                                if (var == KLAA) break;  else


#endif /* WITHTILDE */



