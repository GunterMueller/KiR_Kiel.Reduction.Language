/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"



extern PTR_DESCRIPTOR newdesc();  /* rheap.c       */
/********************************************************************/
/*                                                                  */
/* closecond steuert das Schliessen eines Conditionals              */
/* Gruende :                                                        */
/*           Reduktionszaehler abgelaufen                           */
/*           Praedikatausdruck nicht auswertbar                     */
/*                                                                  */
/********************************************************************/

STACKELEM closecond()
{
 register PTR_DESCRIPTOR   desc;
          STACKELEM        x,hilf, *p_arg;
          int              i,anz_fv,anz_bv,anz;


   START_MODUL("closecond");
   if (( desc = newdesc() ) == (PTR_DESCRIPTOR)NULL )
    {
      END_MODUL("closecond");
      return((STACKELEM) NULL);
    }
 
    /* S_i     : @ x_(n-1) , .. , x_0                                */
    /* S_tilde : @ y_(m-1) , .. , y_0                                */
    /* traversieren der oberen Bloecke von S_i und S_tilde           */
    /* nach S_hilf                                                   */
    /* S_hilf : .. x_0 , .. , x_(n-1), y_0 , .. , y_(m-1)            */

     anz_bv=0;

     if ( SIZEOFSTACK_I > 0 )
       while ( !(T_KLAA( READSTACK(S_i))))
          {
           ++anz_bv;
           PUSHSTACK(S_hilf,POPSTACK(S_i));
          }
    
     anz_fv = 0;

     if ( SIZEOFSTACK_TILDE > 0 )
        while ( !(T_KLAA( READSTACK(S_tilde))))
           {
            ++anz_fv;
            PUSHSTACK(S_hilf,POPSTACK(S_tilde));
           }
      

     DESC_MASK(desc,1,C_EXPRESSION,TY_CLOS);

     L_CLOS((*desc),ndargs) =  0;
     L_CLOS((*desc),nfv) = anz_fv;
      x = POPSTACK(S_e); 
     L_CLOS((*desc),ptfd) = (PTR_DESCRIPTOR)x; /* p-cond eintragen */

     /* Anlegen der pta - Liste                                   */

     anz=anz_bv + anz_fv;
     if (newheap(anz+2,A_CLOS((*desc),pta)) == NULL)
     {
       PUSHSTACK(S_e,x);
       END_MODUL("closecond");
       return((STACKELEM)NULL);
     }
     RES_HEAP;

     /* p_arg wird derart aufgefuellt                              */ 
     /* p_arg[0]=anz_bv + anz_fv                                   */
     /* p_arg[1]= x_0          , .. , p_arg[anz_bv]= x_(n-1)       */
     /* p_arg[anz_bv + 1]= y_0 , .. , p_arg[anz_bv+anz_fv]= y_(m-1)*/ 
     /* p_arg[anz+1]=1;                                            */

     p_arg = (STACKELEM *)R_CLOS((*desc),pta);

     p_arg[0] =  anz;
     p_arg[anz + 1]=1;

     /* Eintragen der Argumente fuer die relativ freien Variablen */
     /* des Conditionals ; gleichzeitig wird auf S_tilde der     */
     /* Block wieder angelegt                                     */

     for (i = anz ; i > anz_bv; i-- )
         {
          hilf = POPSTACK(S_hilf);
          p_arg[i] = hilf;
          PUSHSTACK(S_tilde,hilf);
          if ( T_POINTER(hilf) )
            INC_REFCNT((PTR_DESCRIPTOR)hilf);
         }

     /* Eintragen der Argumente fuer die gebundenen Variablen des */
     /* Conditionals ; gleichzeitig wird auf S_i der Block wieder */
     /* angelegt                                                  */ 

     for (i = anz_bv; i > 0 ; i-- )
         {
          hilf = POPSTACK(S_hilf);
          p_arg[i] = hilf;
          PUSHSTACK(S_i,hilf);
          if ( T_POINTER(hilf) )
             INC_REFCNT((PTR_DESCRIPTOR)hilf);
         }

     REL_HEAP;

     END_MODUL("closecond");
     return((STACKELEM)desc);
   }
 

 STACKELEM close_cond(cdesc)
 STACKELEM cdesc;                   /* Cond - Descr. */
{
 register PTR_DESCRIPTOR   desc;
          STACKELEM        x,hilf, *p_arg;
          int              i,anz_fv,anz_bv,anz;


   START_MODUL("close_cond");
   if (( desc = newdesc() ) == (PTR_DESCRIPTOR)NULL )
    {
      END_MODUL("close_cond");
      return((STACKELEM) NULL);
    }
 
    /* S_i     : @ x_(n-1) , .. , x_0                                */
    /* S_tilde : @ y_(m-1) , .. , y_0                                */
    /* traversieren der oberen Bloecke von S_i und S_tilde           */
    /* nach S_hilf                                                   */
    /* S_hilf : .. x_0 , .. , x_(n-1), y_0 , .. , y_(m-1)            */

     anz_bv=0;

     if ( SIZEOFSTACK_I > 0 )
       while ( !(T_KLAA( READSTACK(S_i))))
          {
           ++anz_bv;
           PUSHSTACK(S_hilf,POPSTACK(S_i));
          }
    
     anz_fv = 0;

     if ( SIZEOFSTACK_TILDE > 0 )
        while ( !(T_KLAA( READSTACK(S_tilde))))
           {
            ++anz_fv;
            PUSHSTACK(S_hilf,POPSTACK(S_tilde));
           }
      

     DESC_MASK(desc,1,C_EXPRESSION,TY_CLOS);

     L_CLOS((*desc),ndargs) =  0;
     L_CLOS((*desc),nfv) = anz_fv;
     L_CLOS((*desc),ptfd) = (PTR_DESCRIPTOR)cdesc; /* p-cond eintragen */

     /* Anlegen der pta - Liste                                   */

     anz=anz_bv + anz_fv;
     if (newheap(anz+2,A_CLOS((*desc),pta)) == NULL)
     {
       END_MODUL("closecond");
       return((STACKELEM)NULL);
     }
     RES_HEAP;

     /* p_arg wird derart aufgefuellt                              */ 
     /* p_arg[0]=anz_bv + anz_fv                                   */
     /* p_arg[1]= x_0          , .. , p_arg[anz_bv]= x_(n-1)       */
     /* p_arg[anz_bv + 1]= y_0 , .. , p_arg[anz_bv+anz_fv]= y_(m-1)*/ 
     /* p_arg[anz+1]=1;                                            */

     p_arg = (STACKELEM *)R_CLOS((*desc),pta);

     p_arg[0] =  anz;
     p_arg[anz + 1]=1;

     /* Eintragen der Argumente fuer die relativ freien Variablen */
     /* des Conditionals ; gleichzeitig wird auf S_tilde der     */
     /* Block wieder angelegt                                     */

     for (i = anz ; i > anz_bv; i-- )
         {
          hilf = POPSTACK(S_hilf);
          p_arg[i] = hilf;
          PUSHSTACK(S_tilde,hilf);
          if ( T_POINTER(hilf) )
            INC_REFCNT((PTR_DESCRIPTOR)hilf);
         }

     /* Eintragen der Argumente fuer die gebundenen Variablen des */
     /* Conditionals ; gleichzeitig wird auf S_i der Block wieder */
     /* angelegt                                                  */ 

     for (i = anz_bv; i > 0 ; i-- )
         {
          hilf = POPSTACK(S_hilf);
          p_arg[i] = hilf;
          PUSHSTACK(S_i,hilf);
          if ( T_POINTER(hilf) )
             INC_REFCNT((PTR_DESCRIPTOR)hilf);
         }

     REL_HEAP;

     END_MODUL("close_cond");
     return((STACKELEM)desc);
   }
 


/********************************************************************/
/*                                                                  */
/* c_closecond steuert das Schliessen eines Conditionals , welches  */
/* auf oberster Ebene in einen Startausdruck eines LREC 's          */
/* auftritt , und rel. freie Var enthaelt                           */
/* then/else Teil enthalten keine geb. Var.                         */
/* Situation : CAP(m+1) y_(m-1)   y_0 p-cond auf S_e                */
/* Gruende :                                                        */
/*           Reduktionszaehler abgelaufen                           */
/*           Praedikatausdruck nicht auswertbar                     */
/*                                                                  */
/********************************************************************/

STACKELEM c_closecond()
{
          STACKELEM        x,hilf, *p_arg;
 register PTR_DESCRIPTOR   desc;
          int              i,anz_fv,anz_bv,anz;


   START_MODUL("c_closecond");
   if (( desc = newdesc() ) == (PTR_DESCRIPTOR)NULL )
    {
      END_MODUL("c_closecond");
      return((STACKELEM) NULL);
    }
 
    /* S_i     : @ x_n , .. , x_0                                */
    /* traversieren des obersten Blockes von S_i nach S_hilf     */
    /* S_hilf : .. x_0 , .. , x_m                                */

     anz_bv=0;                                         

 /*    if ( SIZEOFSTACK_I > 0 ) * /
   /*    while ( !(T_KLAA( READSTACK(S_i))) )     */
     /*     {     */
       /*    ++anz_bv;     */
         /*  PUSHSTACK(S_hilf,POPSTACK(S_i));     */
         /* }     */
    
     x=POPSTACK(S_e);              /* CAP(m+1) entfernen  */
     anz_fv=ARITY(x) - 1 ;

     DESC_MASK(desc,1,C_EXPRESSION,TY_CLOS);

     L_CLOS((*desc),ndargs) =  0;
     L_CLOS((*desc),nfv) = anz_fv;

     /* Anlegen der pta - Liste                                   */

     anz=anz_bv+anz_fv;
     if (newheap(anz+2,A_CLOS((*desc),pta)) == NULL)
     {
       PUSHSTACK(S_e,x);
       END_MODUL("c_closecond");
       return((STACKELEM)NULL);
     }
     RES_HEAP;

     /* p_arg wird derart aufgefuellt                              */ 
     /* p_arg[0]=anz_bv + anz_fv                                   */
     /* p_arg[1]= x_0          , .. , p_arg[anz_bv]= x_(n-1)       */
     /* p_arg[anz_bv + 1]= y_0 , .. , p_arg[anz_bv + anz_fv]= y_(m-1)*/ 
     /* p_arg[anz+1]=2                                              */

     p_arg = (STACKELEM *)R_CLOS((*desc),pta);

     p_arg[0] =  anz;
     p_arg[anz+1]=2;

     /* Eintragen der Argumente fuer die relativ freien Variablen */
     /* des Conditionals ;                                       */

     for (i = anz ; i > anz_bv; i-- )
         {
          hilf = POPSTACK(S_e);
          p_arg[i] = hilf;
         }

     /* Eintragen der Argumente fuer die gebundenen Variablen des */
     /* Conditionals ; gleichzeitig wird auf S_i der Block wieder */
     /* angelegt                                                  */ 

    /* for (i = anz_bv; i > 0 ; i-- )  */
     /*    {  */
      /*    hilf = POPSTACK(S_hilf);  */
       /*   p_arg[i] = hilf;  */
        /*  PUSHSTACK(S_i,hilf);  */
         /* INC_REFCNT((PTR_DESCRIPTOR)hilf);  */
       /*  }  */

     REL_HEAP;

     x = POPSTACK(S_e);   
     L_CLOS((*desc),ptfd) = (PTR_DESCRIPTOR)x;   /* p-cond eintragen */

     END_MODUL("c_closecond");
     return((STACKELEM)desc);
   }
 
