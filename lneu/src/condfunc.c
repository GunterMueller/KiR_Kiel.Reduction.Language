/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/**********************************************************************/
/*  condfunc.c  - external:  condfunc;                                */
/*       Aufgerufen durch :  subfunc;                                 */
/*                           lrecfunc;                                */
/*                           casefunc                                 */
/*  condfunc.c  - internal:                                           */
/**********************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "list.h"


/**********************************************************************/
/* Spezielle externe Routinen:                                        */
/**********************************************************************/


extern PTR_DESCRIPTOR          newdesc();             /* rheap.c      */
extern STACKELEM               st_list();             /* rstpre.c     */
extern STACKELEM               st_cond();            
extern int                      mvheap();             /* rhinout.c    */
extern void                    subfunc();             /* subfunc.c    */
extern void                   lrecfunc();             /* lrecfunc.c   */
extern void                   casefunc();             /* casefunc.c   */    
extern void                gen_BV_list();             /* hfunc.c      */
extern int                      anz_FV();          
extern void                  move_fv_e();                    
extern BOOLEAN    check_list_for_store();      
extern void                   list_end();
extern void                 fvlist_end();
extern void            free_funct_name();
extern void             free_bound_var();
extern void                free_fv_end();
extern void                   free_arg();
extern void                   test_var();
extern int                   test_cond();



/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

 extern list       *VAR;
 extern fvlist      *FV;
 extern arglist    *ARG;

/**********************************************************************/
/*  condfunc                                                          */
/*                                                                    */
/* Situation :  COND(2) then_expr else_expr      auf S_e              */  
/*                                                                    */
/*             Conditionals auf oberster Ebene eines Startausdruckes  */
/*             werden geschlossen ( siehe auch lrecfunc )             */
/*             das Conditional wird ausgelagert; die im then / else-  */
/*             Teil auftretenden relativ freien Variablen werden      */
/*             ueber den Applikator CAP an das Conditional gebunden   */    
/*                                                                    */
/**********************************************************************/
 

void condfunc() 
 {
    PTR_DESCRIPTOR                      pf;    /* Funktionsdeskriptor */
    STACKELEM                        *name;
    STACKELEM                          x,y;
    int           nbound, nfree, i , arity;


    START_MODUL("condfunc") ;          

    x=POPSTACK(S_e);
    PUSHSTACK(S_m,x);
    PUSHSTACK(S_m1,x);                            

    /* in VAR wird ein leerer Block eingehaengt */
       list_end(TRUE);
   
    main_e:

    x = READSTACK(S_e);
    
    /* im then/else - Teil auftretende Funktionen muessen geschlossen */
    /* werden => Aufruf von subfunc, etc mit dem Parameter close      */
    /* gleich TRUE ; Einhaengen eines Blockendezeichens in FV         */

    if(T_SUB(x))
    {
       fvlist_end();
       subfunc(TRUE);
       goto main_e;
    } 
 
    if (T_LREC(x))
    {
      if ( ARITY(x) == 3)
      {
         fvlist_end();
         lrecfunc(TRUE,1);
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
     casefunc(TRUE,FALSE,1); 
     goto main_e;
   }

   if (T_PM_WHEN(x))
   {
     fvlist_end();
     casefunc(FALSE,TRUE,1);
     goto main_e;
   }                    


   if (T_COND(x) )
   {
       fvlist_end();   
       condfunc();
       goto main_e;
   }  
                                                                      
   if ( T_PROTECT(x) ||                                   /* Variable */       
       ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
   {
      test_var(x);
      goto main_m ;                                     
   }

   if (T_CON(x))  
      if (ARITY(x) >0 ) /* bel. Konstruktor */
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
  
  post_mortem(" condfunc  : no match on main_e ") ;     

   main_m:
   x = READSTACK(S_m);                        /* Rumpf einer Funktion */
 

   arity = ARITY(x);

   if (arity > 1)           /* dh die Arity des Konstruktors war >= 2 */
   {
      WRITE_ARITY(S_m,arity-1);
      goto main_e;
   }

   if (arity > 0)                            /* DOLLAR hat arity = 0 */
   {
      PPOPSTACK(S_m);
      x = POPSTACK(S_m1);
      PUSHSTACK(S_a,x);


      if (T_LREC(x) )
      { 
        /* Startausdruck fertig bearbeitet   */
        /* Fkt-namen aus BV entfernen        */

           free_funct_name();
           free_arg();
      } 

      if (T_COND(x))                                             
      {
         y=st_cond();
         PUSHSTACK(S_e,y);

         nfree=anz_FV();  

        if(nfree>0)
        {
           /* sind relativ freie Variable gefunden worden , */     
           /* so wird auf dem E-Stack ein Ausdruck der Form */
           /*  snap(n+1) fv-n    fv-1 p-sub   aufgebaut     */
           /* dabei werden die relativ freien Variablen aus */
           /* der FV - Liste entfernt                       */

              move_fv_e();
              PUSHSTACK(S_e,SET_ARITY(SNAP,(nfree+1)));
        }  
        else
           /* Blockendezeichen entfernen   */
              free_fv_end();
 

        /* Listenendezeichen aus VAR entfernen */
           free_bound_var();

        END_MODUL("condfunc");
        return;
      } 
   
      if ((T_LIST(x)) || (T_STRING(x)))
         if (check_list_for_store()) /* darf Liste ausgelagert werden? */
         {                                                           
           if ((x = st_list() ) == 0)          /* Wenn ja, auslagern */
              post_mortem("condfunc : heap out of space in st_list");
           else
              PUSHSTACK(S_a,x);
         }
      goto main_m;
   }
}
 
