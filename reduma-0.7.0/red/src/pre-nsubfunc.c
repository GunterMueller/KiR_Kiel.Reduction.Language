/* $Log: pre-nsubfunc.c,v $
 * Revision 1.2  1994/06/15  11:22:47  mah
 * both closing strategies in one binary: CloseGoal
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
/**********************************************************************/
/*  nsubfunc.c  - external:  subfunc;                                 */
/*        Aufgerufen durch:  ea-create;                               */
/*                           lrecfunc;                                */
/*                           casefunc;                                */
/*                           condfunc;                                */     
/*  nsubfunc.c  - internal:                                           */
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
extern void                   condfunc();             /* cond-func.c  */
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
extern int                  test_abstr();  
extern void                   DescDump();
extern int                     newheap();
extern void                  snap_args();
extern STACKELEM            *ppopstack();
extern void               ptrav_a_hilf();
extern void                trav_a_hilf();
extern void                stack_error();

/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

 extern list     *VAR;
 extern fvlist    *FV;
 extern arglist  *ARG;


/**********************************************************************/
/*  n_subfunc                                                         */
/*                                                                    */
/* Situation :  SUB(n+1) x_1 .. x_n  body_expr   auf S_e              */
/*                                                                    */
/*              der Funktionsrumpf wird von E nach A traversiert ,    */
/*              dabei werden auftretende gebundene Variable durch     */
/*              Nummervariable und  relativ freie Variable durch      */
/*              Tildevariable ersetzt ;                               */
/*              nsubfunc liefert als Ergebnis einen SUB - Descriptor  */
/*              auf dem E-Stack ; ist die Funktion zu schliessen ,    */
/*              ( Aufruf von nsubfunc mit close=true ) so werden      */
/*              die gefundenen relativ freien Variablen plus einem    */
/*              SNAP ebenfalls auf den E-Stack gelegt .               */
/*                                                                    */
/**********************************************************************/


void subfunc(close)
BOOLEAN close;
 {
    PTR_DESCRIPTOR                       pf;    /* Funktionsdeskriptor */
    STACKELEM                         *name;
    STACKELEM                           x,y;
    int            nbound, nfree, i , arity;
    int                j,tiefe,anz_bv,anz_l;
    START_MODUL("subfunc");
    
    /* teste , ob mehrere lambda - Abstraktionen direkt aufeinander  */
    /* folgen                                                        */
         
     i=test_abstr();
   
       if ( i == 2 ) 
       {                        
         /* ein oder mehrere sub's gefolgt von einem case */
            casefunc(close,FALSE);     

         END_MODUL("subfunc");    
         return;
       }
   
                                                              
       if ( i == 1 )
       {
         /* mehrere geschachtelte sub's */
            
           tiefe = 0;
           anz_bv = 0; 
           anz_l = 0;   

         /* bestimme sie Anzahl der sub's und die Anzahl der */
         /* insgesamt gebundenen Variablen                   */

           READ_E( tiefe , x );
   
           while ( T_SUB(x) )
           {
             arity=ARITY(x);
             ++anz_l;
             tiefe += arity;
             anz_bv = anz_bv + arity -1;
             READ_E( tiefe , x);
           }   

         /*  Funktionsdeskriptor anlegen */

           if ((pf = newdesc()) == NULL)                                      
                post_mortem("subfunc: Heap out of space");

           PUSHSTACK(S_m,SET_ARITY(PRELIST,anz_bv));                     

           DESC_MASK(pf,1,C_EXPRESSION,TY_SUB);                               

           L_FUNC((*pf),special) = EXTR_EDIT(x);
           L_FUNC((*pf),nargs)=anz_bv; 

#if DEBUG                                      /*1 in 0 geaendert wg. odd address  */
           L_FUNC((*pf),pte) = (PTR_HEAPELEM)0;/* Damit DescDump() nicht bei       */
#endif                                         /* diesem halbfertigen Deskriptor   */
                                               /* abstuerzt                        */

           if (newheap(2 + anz_bv+anz_l, A_FUNC((*pf),namelist)) == 0)         
	     post_mortem("subfunc: Heap out of space");

         /* Anlegen einer Liste ,welche mit den Variablennamen */
         /* und den Stelligkeiten der sub's aufgefuellt wird   */

           name = (STACKELEM *) R_FUNC((*pf),namelist);   
           name[0] = (STACKELEM) anz_bv + 1 + anz_l;                
           name[1] = (STACKELEM) anz_bv + 2;
                                           
           i = anz_bv + 1;
           j = anz_bv + 2 ;

           for (  ; anz_l > 0 ; anz_l-- )
           {
             nbound = READ_ARITY(S_e);
             PPOPSTACK(S_e);              /* SUB(nbound) von S_e entfernen */
             
             name[j] = nbound ;          /* Stelligkeit merken */
             ++j;
             for( ; nbound > 1 ; nbound-- )
             {
                name[i] = POPSTACK(S_e);  /* Variablennamen eintragen */
                --i;
             }
           } 

          /* gebundene Variable in die VAR - Liste eintragen */
             list_end(close);
             gen_BV_list(anz_bv,name);            
                                    
      }
      else
      {

        /*  Funktionsdeskriptor anlegen */

        if ((pf = newdesc()) == NULL)                                      
             post_mortem("subfunc: Heap out of space");

        x = POPSTACK(S_e);                     /* n-stelliges SUB    */
        nbound = ARITY(x) - 1;
        PUSHSTACK(S_m,SET_ARITY(PRELIST,nbound));                     

        DESC_MASK(pf,1,C_EXPRESSION,TY_SUB);                               

        L_FUNC((*pf),special) = EXTR_EDIT(x);  
        L_FUNC((*pf),nargs)=nbound;
#if DEBUG                    /*1 in 0 geaendert wg. odd address       */
        L_FUNC((*pf),pte) = (PTR_HEAPELEM)0;    /* Damit DescDump() nicht bei    */
#endif                                          /* diesem halbfertigen Deskri-   */
                                                /* ptor abstuerzt */

        if (newheap(2 + nbound, A_FUNC((*pf),namelist)) == 0)
             post_mortem("subfunc: Heap out of space");

        /* Anlegen einer Liste ,welche mit den Variablennamen */
        /* aufgefuellt wird                                   */

        name = (STACKELEM *) R_FUNC((*pf),namelist);   
        name[0] = (STACKELEM) nbound + 1;
        name[1] = (STACKELEM) 0;

        for(i=nbound + 1 ; i > 1 ; i-- ) 
             name[i]=POPSTACK(S_e);
   
        /* gebundene Variable in die VAR - Liste eintragen */
           list_end(close);
           gen_BV_list(nbound,name);            
 
     }   

    PUSHSTACK(S_a,(STACKELEM)pf);          



    main_e:

    x = READSTACK(S_e);

    /* tritt in dem Fkt'srumpf  ein SUB/LREC/CASE auf , so muessen  */
    /* diese geschlossen werden => Aufruf der entsprechenden Fkt.   */
    /* mit dem Parameter TRUE und Einhaengen eines Blockendezeichens*/
    /* in der FV - Liste                                            */

       if (T_SUB(x))
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
         /* d.h. lrec(2) p-lrec start-expr auf E */

           PPOPSTACK(S_e);
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




      if ( T_PROTECT(x) ||                          /* Variable */       
          ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
      {
        test_var(x);

	/* Funktionsanwendungen auf oberster Ebene des */
	/* Startausdrucks muessen geschlossen werden   */

	if (!CloseGoal)
	{
	  x = READSTACK(S_a);
	  if (T_POINTER(x) && (R_DESC(*(PTR_DESCRIPTOR)x,type) == TY_LREC_IND))
	  {
	    if ((i=test_cond()) == 0)
	    {
	      PPOPSTACK(S_a); /* das lrec_ind */
	      snap_args(x);
	    }
	  }
	} /* !CloseGoal */

        goto main_m ;                                     
      }

      if (T_COND(x) )
      {
	/* Conditionals on top of a goal expression are NOT! closed */
	/* by condfunc(). mah 210194                                */
#if 0	
         if ( ( i=test_cond() ) == 0 )
         {
           /* conditional auf oberster Ebene des Startausdrucks */
           /* wird mittels condfunc() geschlossen               */

             fvlist_end();
             condfunc();
             goto main_e;
         }
         else 
#endif
         {
             PPOPSTACK(S_e);
             PUSHSTACK(S_m,x);
             PUSHSTACK(S_m1,x);
             goto main_e;
         }

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
  
    post_mortem(" subfunc  : no match on main_e ") ;     

    main_m:

      x = READSTACK(S_m);

      /* Rumpf einer Funktion fertig bearbeitet => auslagern  */

      if (T_PRELIST(x))                                 
      {                                                
#if HEAP
         H_PTRAV_A_HILF;
#else
         PTRAV_A_HILF;
#endif
         pf= (PTR_DESCRIPTOR) READSTACK(S_a);
         if (mvheap(A_FUNC((* pf),pte)) == 0)       /* Auslagern */
	   post_mortem("subfunc: Heap out of space");
         PPOPSTACK(S_m);                            /* PRELIST  weg */

        /* p-sub auf e legen  */

         PUSHSTACK(S_e,POPSTACK(S_a));

          if ( close ) 
          {
	    PTR_DESCRIPTOR snsub;

            /* d.h. der Ausdruck soll geschlossen werden  */
 
              nfree=anz_FV();
              if(nfree>0)
              {
                /* sind relativ freie Variable gefunden worden , */     
                /* so wird auf dem E-Stack ein Ausdruck der Form */
                /*  snap(n+1) fv-n    fv-1 p-sub   aufgebaut     */
                /* dabei werden die relativ freien Variablen aus */
                /* der FV - Liste entfernt                       */

                /*  Funktionsdeskriptor fuer sub~ anlegen */

		if ((snsub = newdesc()) == NULL)
		  post_mortem("subfunc: Heap out of space");


		DESC_MASK(snsub,1,C_EXPRESSION,TY_SNSUB);                               
		L_FUNC((*snsub),special) = 0;
		L_FUNC((*snsub),nargs)   = nfree; 
		L_FUNC((*snsub),namelist)= NULL;

		PUSHSTACK(S_hilf, POPSTACK(S_e));             /* das Sub */
		if (mvheap(A_FUNC((* snsub),pte)) == 0)       /* Auslagern */
		  post_mortem("subfunc: Heap out of space");

		PUSHSTACK(S_e, (STACKELEM) snsub);
		move_fv_e();
		PUSHSTACK(S_e,SET_ARITY(SNAP,(nfree+1)));
              }  
              else
              /* Blockendezeichen entfernen   */
                 free_fv_end();
          } /* close */


         /* beim Verlassen des Bindungsbereiches werden die gebundenen */
         /*  Variablen aus der VAR - Liste entfernt                     */

           free_bound_var();
 
         END_MODUL("subfunc");           

         return;
      }

      arity = ARITY(x);

      if (arity > 1)  /* dh die Arity des Konstruktors war >= 2 */
      {
        WRITE_ARITY(S_m,arity-1);
        goto main_e;
      }
      if (arity > 0)          /* DOLLAR hat arity = 0 */
      {
	/* T_SNSUB auslagern: TY_SNSUB-Deskriptor erzeugen */
	/* mah 231193 */

	if (T_SNSUB(x))       /* arity ist immer 1 */
	{
	  PTR_DESCRIPTOR snsub;
	  int nfree;

	  PPOPSTACK(S_m);
	  PPOPSTACK(S_m1);

	  nfree = (ARITY(READSTACK(S_m1)) - 1); /* snap */

	  /* mah 231193: Funktionsdeskriptor fuer sub~ anlegen */

	  if ((snsub = newdesc()) == NULL)
	    post_mortem("subfunc: Heap out of space");

	  DESC_MASK(snsub,1,C_EXPRESSION,TY_SNSUB);                               
	  L_FUNC((*snsub),special) = 0;
	  L_FUNC((*snsub),nargs)   = nfree; 
	  L_FUNC((*snsub),namelist)= NULL;

	  trav_a_hilf();

	  if (mvheap(A_FUNC((* snsub),pte)) == 0)       /* Auslagern */
	    post_mortem("subfunc: Heap out of space");
	  
	  PUSHSTACK(S_a, (STACKELEM) snsub);
	}
	else
	{
	  PPOPSTACK(S_m);
	  x = POPSTACK(S_m1);
	  PUSHSTACK(S_a,x);
	}
       if (T_LREC(x) )
       {
         /* Startausdruck fertig bearbeitet   */
         /* Fkt-namen aus  der VAR _-Liste entfernen */

           free_funct_name();
           free_arg();
      }

      if (T_COND(x))
      { 
        /* conditional auslagern */
           y = st_cond();
           PUSHSTACK(S_a,y);
      }    

      if ((T_LIST(x)) || (T_STRING(x)))
         if (check_list_for_store())  /* darf Liste ausgelagert werden? */
         {
           if ((x = st_list() ) == 0)   /* Wenn ja, auslagern */
              post_mortem("subfunc: heap out of space in st_list");
           else
             PUSHSTACK(S_a,x);
         }
      goto main_m;
    }
}
 




    
