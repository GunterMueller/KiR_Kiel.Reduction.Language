/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/**********************************************************************/
/*  ea-create.c  - external:  ea_create;                              */
/*         Aufgerufen durch:  reduct;                                 */
/*                            preproc                                 */
/*  ea-create.c  - internal:                                          */
/**********************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include <setjmp.h>
#include "list.h"
 

/**********************************************************************/
/* Spezielle externe Routinen:                                        */
/**********************************************************************/


extern PTR_DESCRIPTOR          newdesc();             /* rheap.c      */
extern STACKELEM               st_list();             /* rstpre.c     */   
extern STACKELEM               st_vare();
extern STACKELEM               st_cond();            
extern int                      mvheap();             /* rhinout.c    */     
extern void                    subfunc();             /* subfunc.c    */ 
extern void                   condfunc();             /* cond-func.c  */
extern void                   lrecfunc();             /* lrecfunc.c   */
extern void                   casefunc();             /* casefunc.c   */    
extern BOOLEAN    check_list_for_store();      
extern void                   list_end();
extern void                 fvlist_end();
extern void            free_funct_name();
extern void                   free_arg();
extern void                   test_var();


/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

#if LARGE
extern int          test_pre;     /* Marke fuer preproc */
#endif
extern BOOLEAN    _interrupt;     /* reduct.c */
extern jmp_buf _interruptbuf;     /* reduct.c */

extern list            *VAR;
extern fvlist           *FV;
extern arglist         *ARG;



/**********************************************************************/ 
/*  ea_create  --  Preprocessed einen auf dem E-Stack liegenden       */
/*                 Ausdruck, und liefert den Ergebnisausdruck auf     */
/*                 dem A-Stack ab.                                    */
/**********************************************************************/

void ea_create()
{
  register STACKELEM           x,y;
  register unsigned short    arity;
  extern   jmp_buf   _interruptbuf;                       /* reduct.c */
           int                   i;


  START_MODUL("ea_create");

  setjmp(_interruptbuf);         /* Interruptbehandlung Preprocessing */
  if (_interrupt)                                /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("ea_create: Interrupt received");
  }

  eac();                          /* lagere alle konstanten aus       */       

  PUSHSTACK(S_m,DOLLAR);                               /* Ende-Symbol */ 
  
  /* Initialiesieren der Listen, in denen die Bindungsstrukturen nach-*/
  /* gehalten werden                                                  */
                        
      VAR=NULL;
      FV=NULL;
      ARG=NULL;
      list_end(TRUE);
      fvlist_end();

   main_e:

   x = READSTACK(S_e);
                                                
   /*  aeussere Funktionen enthalten keine relativ freien Variablen   */       
   /*  und sind somit nicht zu schliessen => Aufruf von subfunc, etc  */
   /*  mit dem Parameter close gleich FALSE : in FV wird kein neuer   */
   /*  Block angelegt                                                 */

   if (T_SUB(x))
   {
       fvlist_end();
       subfunc(FALSE);
       goto main_e;
   }

   if (T_LREC(x))  
   {
     if (ARITY(x)==3)
     {
        fvlist_end();
        lrecfunc(FALSE,0);
     }
     else
     {
        POPSTACK(S_e);
        PUSHSTACK(S_m,x);
        PUSHSTACK(S_m1,x);
     }
     goto main_e;
   }

   if (T_PM_CASE(x)) 
   {
      fvlist_end();
      casefunc(FALSE,FALSE,0);
      goto main_e;
   }                    

   if (T_PM_WHEN(x))
   {
      fvlist_end();
      casefunc(FALSE,TRUE,0);
      goto main_e;
   }                    

   if (T_PROTECT(x) ||
       ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
   {
     if (T_KLAA((STACKELEM)VAR->var))
     {
        y=st_vare();
        PUSHSTACK(S_a,y);                                             
#if LARGE
        test_pre=1;              /* test_pre = 1 => Ausdruck enthaelt */
#endif
                                 /* absolut freie Variable            */
     }
     else
       test_var(x);
     goto main_m;
   } 

   if (T_CON(x)) 
   {
     if (ARITY(x) >0 )
     {
        PPOPSTACK(S_e);
        PUSHSTACK(S_m,x);
        PUSHSTACK(S_m1,x);
        goto main_e;
     }
     else
     {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;         
     }
   } 


   if (T_POINTER(x))  
   {
      PPOPSTACK(S_e);
      PUSHSTACK(S_a,x);
      goto main_m;
   }
    
   if (T_ATOM(x)) 
   {
      PPOPSTACK(S_e);
      PUSHSTACK(S_a,x);
      goto main_m;    
   }

   post_mortem("ea_create: No match successful on main_e");

   main_m:

   x = READSTACK(S_m);
   arity = ARITY(x);                                                   

   if (arity > 1)            /* dh die Arity des Konstruktors war >= 2 */
   {
     WRITE_ARITY(S_m,arity-1);
     goto main_e;
   }   

   if (arity > 0)                              /* DOLLAR hat arity = 0 */
   {
      PPOPSTACK(S_m);
      x = POPSTACK(S_m1);
      PUSHSTACK(S_a,x);

      if ( T_LREC(x) )
      { 
        /* lrec(1) auf m , d.h. Startausdruck fertig bearbeitet        */
        /* Fkt-namen aus der Liste der gebundenen Var aushaengen       */

           free_funct_name();
           free_arg();
      }
      
      if (T_COND(x))                          /* conditional auslagern */
      {
         y = st_cond(); 
         PUSHSTACK(S_a,y); 
      }
                          
      if ( (T_LIST(x))||(T_STRING(x)) )
       if (check_list_for_store())   /* darf Liste ausgelagert werden? */
       {
         if ((x = st_list() ) == 0)              /* Wenn ja, auslagern */
            post_mortem("ea_create: heap out of space in st_list");
         else
            PUSHSTACK(S_a,x);
       }
      goto main_m;
   }   

   /* Ein DOLLAR. Dann ist ea_create zu beenden */
      PPOPSTACK(S_m);                                        /* DOLLAR */

   /* restliche Listenstrukturen freigeben   */
    
      free(VAR);     
      free(FV);    
  
   /* nun den V-Stack von Namensdescriptoren befreien: */
   /* weitere Informationen in st_name.                */
   /* Refcountbehandlung ist nicht noetig.             */

      for (arity = SIZEOFSTACK(S_v); arity > 0; arity--)
      PPOPSTACK(S_v);

      END_MODUL("ea_create");
}

