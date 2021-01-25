/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/**********************************************************************/
/*  lrecfunc.c  - external:     lrecfunc;                             */
/*        Aufgerufen durch:    ea-create;                             */
/*                               subfunc;                             */
/*                              lrecfunc;                             */
/*                              casefunc;                             */
/*                              condfunc;                             */
/*  lrecfunc.c  - internal:                                           */
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

extern PTR_DESCRIPTOR         newdesc();       /* rheap.c    */
extern T_HEAPELEM            *newbuff();                             

extern STACKELEM              st_list();       /* rstpre.c   */
extern STACKELEM              st_cond();        
extern STACKELEM              st_expr();       

extern void                  condfunc();       /* condfunc.c */  
extern void                  casefunc();       /* casefunc.c */    
extern void                   subfunc();       /* nsubfunc.c */     

extern int                     mvheap();       /* rhinout.c  */    

extern void               gen_BV_list();       /* hfunc.c    */
extern void              gen_LBV_list();         
extern void              gen_arg_list();
extern int                     anz_FV();           
extern void                 move_fv_e();      
extern void                  test_var();         
extern void                     hallo();            
extern void                  list_end();
extern void                fvlist_end();
extern void           free_funct_name();
extern void            free_bound_var();
extern void               free_fv_end();
extern void                  free_arg();
extern BOOLEAN   check_list_for_store();  
extern int                  test_cond();
extern void             find_free_var();                          


/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

 extern list                       *VAR;
 extern fvlist                      *FV;
 extern arglist                    *ARG;
 extern free_var_list          *FV_LREC;
 extern BOOLEAN               _preproc ;


/**********************************************************************/
/*                                                                    */
/* lrecfunc                                                           */
/*                                                                    */
/* Situation : LREC(3) LIST(n) func_name_1 .. func_name_n             */
/*                     LIST(n) func_expr_1 .. func_expr_n             */
/*                     start_expr                          auf S_e    */            
/*                                                                    */
/*             fuer jeden Funktionsnamen wird ein LREC-IND - Descr.   */
/*             angelegt; die Namen werden dabei von dem E - Stack     */
/*             entfernt und in den anzulegenden LREC - Descriptor     */
/*             eingetragen; die Funktionsdefinitionen werden rekursiv */
/*             verpointert, die " Ergebnis " - Descriptoren werden    */
/*             in den LREC - Descriptor eingetragen;in den Funktions- */
/*             definitionen und im Startausdruck wird jedes angewandte*/
/*             Vorkommen einer Funktion durch den entsprechenden      */
/*             LREC-IND - Descriptor ersetzt;                         */
/*             lrecfunc liefert als Ergebnis einen LREC - Descriptor  */
/*             auf dem E-Stack ; ist die Funktion zu schliessen ,     */
/*             ( Aufruf von lrecfunc mit close=true ) so werden       */
/*             die gefundenen relativ freien Variablen plus einem     */
/*             SNAP ebenfalls auf den E-Stack gelegt ;                */
/*             der Startausdruck wird in der aufrufenden Funktion     */
/*             abgearbeitet                                           */                                                       
/*                                                                    */
/**********************************************************************/

lrecfunc(close,l_index)
BOOLEAN close;
int      l_index;      /* index = 0 => LREC ist aeussere Funktion und */
                       /* enthaelt somit keine relativ freien Var.    */     
                       /* index = 2 : Aufruf aus lrecfunc             */
                       /* index = 1 : Aufruf aus subfunc,etc          */
{

 register int   i,nfree,index ,arity;

 PTR_DESCRIPTOR             desc,ldesc;
 STACKELEM     *name,x,y,z,*LBV,*daten;
 int                            anzahl;
 int                          m1_arity;
 free_var_list                   *hilf;

   START_MODUL("lrecfunc");

   if ((desc = newdesc()) == NULL)
       post_mortem("lrecfunc: Heap out of space ");

   FV_LREC=NULL;

   /* ( index =1  und close = TRUE)                                   */            
                                                                      
   /* es gilt                                                         */
   /* FV( lrec_expr ) = FV( func_expr_1 ) u .. u FV( func_expr_n )    */
   /*                   u FV( start_expr )                            */

   /* Um die angewandten Vorkommen der  durch das lrec gebunden Fkt.  */
   /* reduzieren zu koennen, werden nur die Argumente fuer die in den */
   /* Fkt-definitionen auftretenden rel. freien Variablen , nicht     */
   /* jedoch die Argumente fuer die im Startausdruck auftretenden rel */
   /* freien Var. benoetigt.                                          */
   /* Daher werden nur die in den Fkt-def. auftretenden rel. freien   */
   /* Variablen bestimmt und in FV_LREC eingetragen                   */

   /* Die in FV_LREC eingetragenen Variablen werden an die lrec-Struk-*/
   /* tur und an zu schliessende angewandte Vorkommen gebunden.       */

   /* Nachteil : auf oberster Ebene des Startausdruckes auftretende   */
   /* Funktionen muessen geschlossen werden, da zur Reduktion des     */
   /* Startausdruckes auf dem Tildestack die Umgebung fuer die Fkt-   */
   /* definitionen, nicht jedoch fuer den Startausdruck liegt.        */
   /* Insbesondere muessen in dieser Situation auch CONDITIONALS      */
   /* geschlossen werden !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!              */    


   /* index = 2                   : Aufruf aus lrecfunc               */    
   /* FV_LREC wird gleich der Menge der rel. freien Variablen der     */
   /* umgebenden lrec-Struktur gesetzt                                */
                                                          
   switch ( l_index )
   {
    case 0 : break;
   
    case 1 :                
            find_free_var(1);                     
                                                                        
            /* die gefundenen relativ freien Variablen werden in der  */
            /* Reihenfolge ihres Auftretens in FV eingetragen         */
            /* dies ist notwendig, da durch das Schliessen von ange-  */
            /* wandten Vorkommen rekursiver Funktionen die Reihen-    */
            /* folge, in der rel. freie Variable auftreten, veraendert*/
            /* werden kann                                            */

                  if ( FV_LREC != NULL )
                       gen_LREC_FV_list(FV_LREC);
                  break;

    case 2:  FV_LREC = ARG->fv_lrec;

   }
                                                                

   x = POPSTACK(S_e);                           /* LREC(3) entfernen  */
   y = READSTACK(S_e);                                   /* LIST(n)   */
                                             /* es folgen n Fkt-namen */

   anzahl = ARITY(y);

   /* Anlegen eines LREC - Descriptors */

      DESC_MASK(desc,1,C_EXPRESSION,TY_LREC);

      L_LREC((*desc),nfuncs) = anzahl;
      


   /* Retten des Edit-Feldes des LREC-Konstruktors: */

       L_LREC ((*desc), special) = EXTR_EDIT(x);

   if (newheap(anzahl+1,A_LREC((*desc),namelist)) == 0)
        post_mortem("lrecfunc: Heap out of space");

   POPSTACK(S_e);                                /* LIST(n) entfernen */  


   /* Namensliste des LREC - Descriptors auffuellen */

      RES_HEAP;
      name = (STACKELEM * ) R_LREC((*desc),namelist);
      name[0] = (STACKELEM)anzahl;
      for ( i = 1 ; i <= anzahl ; i++ )
        name[i] = POPSTACK(S_e);
      REL_HEAP;

   if ((LBV=newbuff(anzahl+1)) == 0)
       post_mortem("lrecfunc: Heap out of space");
  
   /* Anlegen der LREC-IND - Descriptoren */

      for ( i = 1 ; i<= anzahl ; i++ )
      {
        if ((ldesc = newdesc()) == NULL )
            post_mortem("lrecfunc: Heap out of space");

        DESC_MASK(ldesc,1,C_EXPRESSION,TY_LREC_IND);
        L_LREC_IND((*ldesc),index) = i;
        L_LREC_IND((*ldesc),ptdd) = (T_PTD)desc;
        LBV[i] = (STACKELEM)ldesc;
      }                                                                
    
   PUSHSTACK(S_a,(STACKELEM) desc);              /* p-lrec auf a legen */

   /*  fuer jeden LREC - Descriptor wird in der ARG - Liste ein        */
   /*  Element der Form                                                */
   /*       ( LREC - Desc , FV_LREC )                                  */
   /* eingehaengt                                                      */

      gen_arg_list(desc);

   /* gebundene Variable in die VAR - Liste eintragen                  */ 

      list_end(close);
      gen_LBV_list(desc,name,LBV);


   /* auf dem E - Stack liegt ein Ausdruck der Form                    */
   /*   list(n) fkt-def-1 ...  fkt-def-n  start-expr.                  */
 
      x=POPSTACK(S_e);                            /* LIST(n) entfernen */
      PUSHSTACK(S_m,SET_ARITY(LREC,ARITY(x)+1));
      PUSHSTACK(S_m1,SET_ARITY(LREC,3));


   main_e:

  x=READSTACK(S_e);

   /* die durch das LREC definierten Funktionen werden nicht   */
   /* geschlossen , die relativ freien Variablen aller Fkt's - */
   /* ruempfe werden gesammelt und die Variablen konsistent in */
   /* Tildevariable umbenannt => Aufruf der entsprechenden     */
   /* Funktionen mit dem Parameter FALSE , in die FV -Liste    */
   /* wird kein Blockendezeichen eingehaengt                   */

      if (T_SUB(x))
      {
        subfunc(FALSE);
        goto main_e;
      } 
 
     if (T_LREC(x))
     {
       if ( ARITY(x) == 3)
       {
       lrecfunc(FALSE,2);
       }
       else
       {
        /* d.h. lrec(2) p-lrec start-expr auf E */

           POPSTACK(S_e);
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
       }
       goto main_e;
     }

     if (T_PM_CASE(x))
     {
        casefunc(FALSE,FALSE,2);                                           
        goto main_e;
     }

     if (T_PM_WHEN(x))
     {
        fvlist_end();
        casefunc(FALSE,TRUE,2);
        goto main_e;
     }                    

                                                                       
     if ( T_PROTECT(x) ||                                  /* Variable */       
       ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
     {
        test_var(x);
        goto main_m ;                                     
     }

     if (T_COND(x) )
     {   
       if ( ( i=test_cond() ) == 0 )
       {                                                        
         /* conditional auf oberster Ebene des Startausdrucks   */
         /* wird mittels condfunc() geschlossen                 */


            fvlist_end();
            condfunc();
            goto main_e;
       }
       else 
       {     
           POPSTACK(S_e);
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
           goto main_e;
       }  

     }
  
     if (T_CON(x))  
        if (ARITY(x) >0 )                   /* bel. Konstruktor */
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
       
   post_mortem("lrecfunc: No match successful on main_e");

   main_m:

   x = READSTACK(S_m);
   arity = ARITY(x);

   y = READSTACK(S_m1);
   m1_arity = ARITY(y);
   
   if (T_LREC(x))
   {
     if (m1_arity == 3)
     {                                                              
       /* Fkt-definition fertig bearbeitet;                     */
         z = READSTACK(S_a);

      /* auf dem A -Stack liegt entweder ein Descriptor oder    */
      /* ein Ausdruck ( z.B. eine Nummer oder Tildevariable     */
      /*                    oder eine Applikation           )   */
      /* im zweiten Fall wird ein Expression - Descriptor       */
      /* angelegt und auf den A - Stack gelegt                  */

        if (! T_POINTER(z))
           if ((y = st_expr()) == NULL )
               post_mortem("lrecfunc ; heap out of space");
           else
              PUSHSTACK(S_a,y);
        
        if ( arity == 2 )
        {                                             
          /* d.h. alle Fkt-def. bearbeitet                      */
          /* Anlegen einer Liste , welche die Verweise auf die  */
          /* Fkt's-ruempfe aufnehmen soll                       */

           if (newheap(anzahl+1,A_LREC((*desc),ptdv)) == NULL )
               post_mortem("lrecfunc: Heap out of space");
  
           RES_HEAP;

           daten = (STACKELEM * )R_LREC((*desc),ptdv);
           daten[0] = (STACKELEM)anzahl;

           for ( i = anzahl ; i > 0 ; i-- )
           {
              daten[i] = (x = POPSTACK(S_a)); 

              /* die Verweise auf die Funktionsruempfe werden in*/
              /* die LREC_IND - Descriptoren eingetragen        */
                      
                 ldesc = (PTR_DESCRIPTOR)LBV[i];
                 L_LREC_IND((*ldesc),ptf) = (T_PTD)x;
           } 

           REL_HEAP;
          

          PUSHSTACK(S_e,POPSTACK(S_a));   /* p-lrec auf e legen */
       
          PUSHSTACK(S_e,SET_ARITY(LREC,2));   /* lrec(2) auf e  */
          POPSTACK(S_m);
          POPSTACK(S_m1);

          if ( close )
          {
            /* d.h. der Ausdruck muss geschlossen werden */

              if (( nfree = anz_FV()) > 0)
              {
                /* wenn vorhanden,rel. freie Var. samt SNAP auf e  */
                   move_fv_e();
                   PUSHSTACK(S_e,SET_ARITY(SNAP,nfree+1));
              }
              else
                /* Blockende-zeichen entfernen  */
                   free_fv_end();

          }
          hallo();

          END_MODUL("lrecfunc");
          return;
        }
        else
        {
           WRITE_ARITY(S_m,arity - 1);
           goto main_e;
        }
     }
     else    /* lrec(2) auf m1  */
     {
       if (arity == 1)
       { 
         /* d.h. Startausdruck fertig bearbeitet ; Fkt-namen  */
         /* entfernen                                         */

            free_funct_name();
            free_arg();

          POPSTACK(S_m);
          PUSHSTACK(S_a,POPSTACK(S_m1));
          goto main_m;
       }
       else
       {
          WRITE_ARITY(S_m,arity - 1);
          goto main_e;
       }
      } 
   }


   if (arity > 1 )                  /* dh die Arity des Konstruktors war >= 2 */
   {
      WRITE_ARITY(S_m,arity-1);
      goto main_e;
   }           

   if (arity > 0)                                     /* DOLLAR hat arity = 0 */
   {
      PPOPSTACK(S_m);
      x = POPSTACK(S_m1);
      PUSHSTACK(S_a,x);

      if (T_COND(x))                                  /* conditional auslagern */
      { 
         y = st_cond();
         PUSHSTACK(S_a,y); 
      } 
                         
      if ((T_LIST(x)) || (T_STRING(x)))
       if (check_list_for_store())          /* darf Liste ausgelagert werden? */
       {
          if ((x = st_list() ) == 0)                    /* Wenn ja, auslagern */
             post_mortem("ea_create: heap out of space in st_list");
          else
             PUSHSTACK(S_a,x);
       }
      goto main_m;
   }

}
/* Ende von lrecfunc */
  


    
