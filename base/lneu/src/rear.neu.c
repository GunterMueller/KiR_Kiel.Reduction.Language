/*****************************************************************************/
/*                                                                           */
/* MODULE DER PROCESSING PHASE                                               */
/* ear.c - external: ear;                                                    */
/* ear.c - internal:                                                         */
/*                                                                           */
/*****************************************************************************/


#define EAR

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "cparm.h"
#include "rextern.h"
#include "rusedeb.h"
#include "ct-tab.h"
#include "case.h"
#include <setjmp.h>
          
#include "dbug.h"



/*****************************************************************************/
/*                                                                           */
/*  Allgemein verwendete externe Variablen:                                  */
/*                                                                           */
/*****************************************************************************/

extern  BOOLEAN pmredsw ();       /* pmredsw.c     */
extern  BOOLEAN pmmat_2 (); 
#if EAGER
extern  BOOLEAN pmredesw ();     
#endif

extern  BOOLEAN pmredwhen ();     /* pmredwhen.c   */
extern  BOOLEAN pmwhenmat_2 ();

extern  BOOLEAN pm_red_nomat ();  /* pmred-nomat.c */

extern  BOOLEAN pm_red_nomat ();  /* pmred-nomat.c */
extern  BOOLEAN pm_nomat_2 ();			    

extern int red_dummy();           /* hfunc.c       */
extern PTR_DESCRIPTOR newdesc();  /* rheap.c       */
extern  BOOLEAN pm_eval_list();   /* pmredswi.c    */
extern  BOOLEAN pm_nomat_when (); /* red-nomat-when.c   */
 
extern int _argsneeded;           /* state.c - Zahl der benoetigten Argumente */
                                  /*           fuer eine Funktion             */
                                  /* type ? int, long, ?? (ca)                */
 




/*********************************************************************************/
/*                                                                               */
/* beta_collect -- versucht die in der globalen Variable _argsneeded bestimmte   */
/*                 Anzahl von Argumenten fuer eine Beta-Reduktion auf dem        */
/*                 M-Stacks zu finden.                                           */
/*                 _argsneeded wird fuer jedes gefundene Argument um 1 ver-      */
/*                 ringert. Die gefundenen Argumente liegen danach "lose"        */
/*                 auf dem A-Stack.                                              */
/*                 Der Reduktionszaehler wird fuer jeden gefundenen Applikator,  */
/*                 jedoch mindestens einmal, um 1 verringert.                    */
/*                 beta-collect war erfolgreich falls _argsneeded nicht groesser */
/*                 als 0 ist.                                                    */
/* called by    -- ear;                                                          */
/* globals      -- _argsneeded     <w>                                           */
/*                 _redcnt         <w>                                           */
/*                 S_m,S_m1        <w>                                           */
/*                                                                               */
/*********************************************************************************/

void beta_collect()
{
  register int args;
  register STACKELEM x ;
  START_MODUL("beta_collect");

  while (T_AP_1(x=READSTACK(S_m)) && (T_SYSAP(x) ? TRUE 
        : ((_redcnt > 0)? _redcnt-- :FALSE )))
  {
    args = READ_ARITY(S_m1) - 1;
    if (args == _argsneeded)
    {
      PPOPSTACK(S_m1);
      PPOPSTACK(S_m);
      _argsneeded = 0;
      END_MODUL("beta_collect");
      return;
    }
    else if (args < _argsneeded)
    {
      PPOPSTACK(S_m1);
      PPOPSTACK(S_m);
      _argsneeded -= args;
    }
    else
    {
      WRITE_ARITY(S_m1,args + 1 - _argsneeded);
      _argsneeded = - _argsneeded;
      END_MODUL("beta_collect");
      return;
    }
  }
  END_MODUL("beta_collect");
}

/************************************************************************************/
/*                                                                                  */
/* delta_collect -- versucht die in der globalen Variable _argsneeded bestimmte     */
/*                  Anzahl von Argumenten fuer eine Delta-Reduktion auf dem         */
/*                  M-Stacks zu finden.                                             */
/*                  _argsneeded wird fuer jedes gefundene Argument um 1 ver-        */
/*                  ringert. Die gefundenen Argumente liegen danach "lose"          */
/*                  auf dem A-Stack.                                                */
/*                  delta_collect war erfolgreich falls _argsneeded nicht groesser  */
/*                  als 0 ist.                                                      */
/* called by    --  ear;                                                            */
/* globals      --  _argsneeded     <w>                                             */
/*                  S_m,S_m1        <w>                                             */
/*                                                                                  */
/************************************************************************************/

void delta_collect()
{
  register int args;

  START_MODUL("delta_collect");

  while T_AP_1(READSTACK(S_m))
  {
    args = READ_ARITY(S_m1) - 1;
    if (args == _argsneeded)
    {
      PPOPSTACK(S_m1);
      PPOPSTACK(S_m);
      _argsneeded = 0;
      END_MODUL("delta_collect");
      return;
    }
    else if (args < _argsneeded)
    {
      PPOPSTACK(S_m1);
      PPOPSTACK(S_m);
      _argsneeded -= args;
    }
    else
    {
      WRITE_ARITY(S_m1,args + 1 - _argsneeded);
      _argsneeded = - _argsneeded;
      END_MODUL("delta_collect");
      return;
    }
  }
  END_MODUL("delta_collect");
}





/*****************************************************************************/
/*                                                                           */
/* ear    -- steuert die Processingphase,                                    */
/*           aufgerufen durch reduct.                                        */
/*                                                                           */
/* Globale Variablen :                                                       */
/*                                                                           */
/*****************************************************************************/

extern BOOLEAN _beta_count_only;  /* <r> */
extern BOOLEAN _digit_recycling;  /* <w>, fuer die Ausnahmebehandlung */
extern BOOLEAN _interrupt;        /* <w>, fuer die Ausnahmebehandlung */
extern BOOLEAN _heap_reserved;    /* <w>, fuer die Ausnahmebehandlung */
extern jmp_buf _interruptbuf;     /* <w>, fuer die Ausnahmebehandlung */
extern PTR_DESCRIPTOR _desc;      /* <w>, der Ergebnisdeskriptor,auch */
                                  /*      fuer die Ausnahmebehandlung */

/*****************************************************************************/
/* spezielle externe Routinen:                                               */
/*****************************************************************************/

extern void load_expr();         /* rhinout.c */
extern BOOLEAN   red_clos();     /* redclos.c */
extern BOOLEAN   red_pclos();     
extern STACKELEM st_clos();      /* p_fail.c  */
extern STACKELEM st_prim_clos(); 
extern STACKELEM closecond();    /* cond-func.c */
extern int test_fname();         /* testf.c     */

extern STACKELEM store_l();      /* rstpro.c    */
extern STACKELEM st_arglist(); 
extern STACKELEM store_e();     

#if MEASURE
extern int minc;                 /* rbreak.c  */
#endif
extern void failure();           /* query.c     */
extern void pfail();             /* p-fail.c    */



/*****************************************************************************/

void ear()
{
  register STACKELEM                            x;
           STACKELEM                       z,hilf;
  register PTR_DESCRIPTOR                    desc;
  register unsigned short                   arity;
  register int                               args; 
           int                           i,ndargs;
           int              anz_vorh,nbound,index;
           STACKELEM          arg1,arg2,arg3,arg4;
           STACKELEM                       arg[4];
           int               save_redcnt,snapargs;  
           STACKELEM                            y;
           STACKELEM              *p_arg,*p_arg_1;
           PTR_DESCRIPTOR pf,fun_desc,ldesc,adesc; 
           COUNT                    anz_fv,anz_bv;
           FCTPTR                       prim_func;
           PTR_HEAPELEM                      code;

DBUG_ENTER("EAR");

START_MODUL("ear");
     
DBUG_PUSH("d:t:F:L:o,log");

  PUSHSTACK(S_m,KLAA);

  if (setjmp(_interruptbuf) || _interrupt)
      /* eine primitive Funktion ist nicht durchfuehrbar */
  {
    END_MODUL("?");                  /* welches ist ja nicht bekannt */
    if (_heap_reserved)    /* durch unterbrochene primitive Funktion */
      REL_HEAP;
    /* wiederherstellen der Applikation */
    args = FUNC_ARITY(READSTACK(S_e));               /* ca */
    failure(args,arg1,arg2,arg3,arg4);
    /* evtl. halb erstellte Descriptoren freigeben */
    if (_desc != NULL)
      DEC_REFCNT(_desc);
    _interrupt = FALSE;
    _digit_recycling = TRUE;
    goto main_m;
  } 


main_e:

  x = POPSTACK(S_e);  

DBUG_PRINT("S_e",("on line e : %8.8x  ",x));

#if DEBUG
  if (x == NULL)      /* Empfiehlt sich aus Sicherheitsgruenden */
     post_mortem("ear: null on main_e line");
#endif


 if ( _redcnt > NULL )

{
/******************************************************************************/
/*                                                                            */
/*                   REDUKTIONSZAEHLER ungleich NULL                          */
/*                                                                            */
/******************************************************************************/

  if T_POINTER(x)
  {
    desc = (PTR_DESCRIPTOR) x;     /* mit desc zu arbeiten ist einfacher */

    switch((R_DESC((*desc),class)))
    {

     case C_EXPRESSION :
     switch(R_DESC((*desc),type))
     {  

      case TY_LREC_IND :    

                        /* Verweis auf den Fkt-rumpf*/

                            fun_desc=(PTR_DESCRIPTOR)(R_LREC_IND((*desc),ptf));
                            DBUG_PRINT("S_e",("TY_LREC_IND  %8.8x ",fun_desc));
                            INC_REFCNT(fun_desc);


                            if ( R_DESC((*fun_desc),type) == TY_SUB )
                               {
                                  args  = R_FUNC((*fun_desc),nargs);
                                  _argsneeded = args;
                                  beta_collect();
                                  if (_argsneeded > 0)
                                  {
                                    /* eine Reduktion war also nicht moeglich */
                                    /* bisher gefundene Argumente und die     */
                                    /* Funktion hinter Closure-Deskriptor     */

                                        PUSHSTACK(S_e,(STACKELEM)fun_desc);
                                       if ((x = st_clos(args,_argsneeded)) != NULL)
                                       {    
                                          --_redcnt;
                                          PUSHSTACK(S_a,x);
                                          goto main_m; 
                                       }
                                       else    
                                           post_mortem("ear : heap out of space ");

                                  }
                                  /* Reduktion moeglich */
                                  /* Ein Dollar unter alle AP_1 auf dem M-Stack legen */
                                     for (arity = 0; T_AP_1(READSTACK(S_m));
                                                PPOPSTACK(S_m),arity++);  

                                       if (T_DOLLAR(READSTACK(S_m)))
                                          POP_I_BLOCK(hilf);
                                       
                                       else
                                         PUSHSTACK(S_m,DOLLAR);

                                      for ( ; arity > 0; --arity)
                                      PUSHSTACK(S_m,(STACKELEM) AP_1);

                                     /* Inkarnationblock anlegen und fuellen. */
                                         PUSH_I_BLOCK(args);

                                     while (--args >= 0)
                                     {
#if DEBUG
                                        if T_CON(READSTACK(S_a))
                                           post_mortem("ear: function receives constructor");
#endif
                                        PUSH_I(( x= POPSTACK(S_a))); 
                                        DBUG_PRINT("S_e",("Nummer  %8.8x ",x));
                                      }

                                      load_expr((STACKELEM **)A_FUNC((*fun_desc),pte));

                                     DEC_REFCNT(fun_desc);    
                                     goto main_e;

                               }
                            else
                               {
                                 _redcnt--;
                              
                                 if ( (R_DESC((*fun_desc),type))==TY_EXPR ) 
                                    {
                                      load_expr((STACKELEM **)A_EXPR((*fun_desc),pte));
                                      DEC_REFCNT(fun_desc); 

                                      goto main_e;
                                    }
                                else
                                    {
                                      PUSHSTACK(S_e,(STACKELEM)fun_desc);
                                      goto main_e;
                                    }
                              }



 

      case TY_SWITCH:     PUSHSTACK(S_e,x);
                          if (T_AP_1 (READSTACK (S_m)) )
                          {	/* Switch (evtl.) reduzierbar */

                            args = R_SWITCH((*desc),anz_args);
                            if (args > 1)
                            /* d.h.,es handelt sich um ein n-stelliges case */
                            {
                              _argsneeded = args;
                              save_redcnt = _redcnt;
                              beta_collect();
                              if (_argsneeded > 0)
                               /* es sind nicht genuegend Argumente fuer die */
                               /* Reduktion der Funktion vorhanden.          */
                               /* Anlegen einer Closure:                     */ 
                              {                  
                                _redcnt = save_redcnt - 1;
                                if ((x = st_clos(args,_argsneeded)) != NULL)
                                    PUSHSTACK(S_a,x);   /* auslagern */
                                else
                                    post_mortem("ear:Heap out of space!");
 
                                goto main_m;
                              }                      
                              else                  
                              /* _argsneeded = 0, Reduktion moeglich */
                              {    
                                 _redcnt++;
                                   /* Der Reduktionszaehler muss an dieser Stelle inkrementiert */
                                   /* werden,da er in pmredswi dekrementiert wird.              */
                                 if (_redcnt > 0)
                                 {
                                   /* Eintragen der Argumente in einen Listendeskriptor */
                                   PUSHSTACK(S_a,SET_ARITY(LIST,args));
                                   if ((y = st_arglist()) != NULL)
                                      PUSHSTACK(S_a,y);
                                   else
                                      post_mortem("ear:Heap out of space!");

                                   /* An dieser Stelle muessen AP's auf den m- und den m1-Stack */
                                   /* gelegt werden,da diese von beta_collect entfernt wurden:  */
                                   PUSHSTACK(S_m1,SET_ARITY(AP,2));
                                   PUSHSTACK(S_m,SET_ARITY(AP,1));
                                 }                   
   
                              } /* _argsneeded = 0 */

                            } /* Ende n-stelliges case */

                            /* pmredsw steuert den Vergleich des Arguments          */
                            /* auf dem A - Stack mit den Pattern im Matchdeskriptor */

                            if (T_PM_ECASE(R_SWITCH((*desc), casetype)))  
                            {
                             /*Aufruf von pmredeswi zum Anlegen eines */
                             /*Listendeskriptors, in dem die Ergebnisse*/
                             /*des Matching der Guard und Rumpfausdruecke*/
                             /*stehen                                    */
      
                             pmredesw (x, READSTACK (S_a), 1);

                             /*Kontrolle zurueck auf die E-linie, damit der*/
                             /*erste Ausdruck auf dem E-Stack ausgewertet  */
                             /*werden kann                                 */
           
                             goto main_e; 
                            }
                            else
                            {
                              if( pmredsw (x, READSTACK (S_a), 1))
                              /* switch abgearbeitet, Ergebnis liegt auf dem */
                              /* A_stack, d.h. entweder der reduzierte Rumpf */
                              /* oder ein Expressiondeskriptor, hinter dem   */
                              /* ein ap Arg p-nomatchdesc steht */
                                 goto main_m;
                              else
                             /* es wurde ein otherwise Ausdruck angegegben, der*/
                             /* noch reduziert werden muss */
                                 goto main_e;
                            }  

                          }/* T_AP_1 */

                          else           
                          /* case nicht in Funktionsposition  */
                          {
                               if ( SIZEOFSTACK_TILDE > 0 )
                               /* Argumente fuer relativ freie Variable */
                               /* vorhanden => Closure anlegen:         */
                               {            
                                 args =  R_SWITCH((*desc),anz_args);                                           
                                 if ( ( x = st_clos(args,args) ) != NULL )
                                     PUSHSTACK(S_a,x);   /* auslagern */
                                 else
                                     post_mortem("ear : heap out of space ");
                               }
                               else
                               {
                                 POPSTACK(S_e);
                                 PUSHSTACK(S_a,x);
                                 goto main_m;
                               }
                               goto main_m;
                        }     
                                                  

      case TY_COND: if (T_AP_1(READSTACK(S_m)) &&    /* Reduktion eines conditional */
                        T_BOOLEAN(READSTACK(S_a)) )                           
                    { 
                           /* Conditional reduzierbar */    

                           if (! _beta_count_only)   /* dies ist keine Beta-Reduktion */
                             _redcnt--;

                           if T_AP_2(READSTACK(S_m1))
                           {
                              PPOPSTACK(S_m);
                              PPOPSTACK(S_m1);
                           }
                           else
                             {
                              arity = READ_ARITY(S_m1) - 1;
                              WRITE_ARITY(S_m1,arity);
                             }

                           if T_SA_TRUE(POPSTACK(S_a))
                              load_expr((STACKELEM **)A_COND((*desc),ptte));
                           else
                              load_expr((STACKELEM **)A_COND((*desc),ptee));
                          
                           DEC_REFCNT(desc);

                           goto main_e;
                      }

                         /* Conditional nicht reduzierbar  =>  */
                         /* Conditional schliessen             */

                         if ((SIZEOFSTACK_I > 0)  ||    
                             (SIZEOFSTACK_TILDE > 0 ))
                          {
                           /* Argumente fuer gebundene und rel. freie */
                           /* Var. werden zusammen mit p-cond in eine */
                           /* Closure eingetragen                     */
                           /* ( das letzte Feld der pta -Liste dieses */
                           /*   Closure - Desc. wird mit einer 1 be - */
                           /*   legt )                                */
                            
                            PUSHSTACK(S_e,x);
                            if( ( x=closecond() ) == (STACKELEM) NULL)
                              post_mortem("ear:heap out of space ");

                            else

                              /* Closure auf S_a legen */

                                 PUSHSTACK(S_a,x); 
                           }
                           else

                             /* Cond. enthaelt weder gebundene noch */
                             /* rel. freie Variable =>              */
                             /* p-cond auf S_a legen                */
                                
                                PUSHSTACK(S_a,x);

                         /* if (_redcnt) do_fail_cond(1); car's patch entfernt 
			  * wegen ap sub[x] in x[true] to [ then 1 else 2] 
                          * ging nicht! RS
		          */

                         goto main_m;

      case TY_SUB:       args  = R_FUNC((*desc),nargs);
                         _argsneeded = args;
                         beta_collect();
                         if (_argsneeded > 0)
                         {
                          /* eine Reduktion war also nicht moeglich */
                          /* bisher gefundene Argumente und die     */
                          /* Funktion hinter Closure-Deskriptor     */

                           PUSHSTACK(S_e,x);
                           if ((x = st_clos(args,_argsneeded)) != NULL)
                              /* auslagern */
                            {
                             PUSHSTACK(S_a,x);
                             goto main_m;
                            }
                           else    
                              post_mortem("ear : heap out of space ");


                         }
                         /* Reduktion moeglich */
                         /* Ein Dollar unter alle AP_1 auf dem M-Stack legen */
                         for (arity = 0; T_AP_1(READSTACK(S_m));
                                                PPOPSTACK(S_m),arity++);
                         if (T_DOLLAR(READSTACK(S_m)))
                             POP_I_BLOCK(hilf);
                         else
                             PUSHSTACK(S_m,DOLLAR);

                         for ( ; arity > 0; --arity)
                           PUSHSTACK(S_m,(STACKELEM) AP_1);

                         /* Inkarnationblock anlegen und fuellen. */
                         PUSH_I_BLOCK(args);

                         while (--args >= 0)
                         {
#if DEBUG
                           if T_CON(READSTACK(S_a))
                             post_mortem("ear: function receives constructor");
#endif
                             PUSH_I(POPSTACK(S_a));
                         }

                         load_expr((STACKELEM **)A_FUNC((*desc),pte));

                         DEC_REFCNT(desc);    
                         goto main_e;



      case TY_FNAME :  
                         i=test_fname(desc);    
                         if ( i== 0 ) 
                         {
                            DEC_REFCNT(desc);
                            goto main_e;    /* Funktion eingelagert */
                         }
                         else
                            goto main_m;    /* Desc. traversiert  */



      case TY_MATCH:     PUSHSTACK(S_e,x);
                         if (T_AP_1 (READSTACK (S_m)) )
                         /* pi-Abstraktion (evtl.) reduzierbar */
                         {
                            code = R_MATCH((*desc),code);
                            args = code[STELLIGKEIT];
                            if (args > 1)
                               /* d.h.,es handelt sich um ein n-stelliges when */
                            {
                              _argsneeded = args;
                              save_redcnt = _redcnt;
                              beta_collect();
                              if (_argsneeded > 0)
                               /* es sind nicht genuegend Argumente fuer die */
                               /* Reduktion der Funktion vorhanden.          */
                               /* Closure anlegen:                           */
                              {                  
                                 _redcnt = save_redcnt - 1;
                                 if ((x = st_clos(args,_argsneeded)) != NULL)
                                    PUSHSTACK(S_a,x);   /* auslagern */
                                 else
                                    _redcnt = 0;        /* Reduktion abbrechen */

                                 goto main_m;
                              }                      
                              else
                              /* Reduktion moeglich */
                              {    
                                _redcnt++;
                                /* Der Reduktionszaehler muss an dieser Stelle inkrementiert */
                                /* werden,da er in pmredwhen dekrementiert wird.              */
                                if (_redcnt > 0)
                                {
                                   /* Eintragen der Argumente in einen Listendeskriptor */
                                   PUSHSTACK(S_a,SET_ARITY(LIST,args));
                                   if ((y = st_arglist()) != NULL)
                                      PUSHSTACK(S_a,y);
                                   else
                                      post_mortem("ear:Heap out of space!");

                                   /* An dieser Stelle muessen AP's auf den m- und den m1-Stack */
                                   /* gelegt werden,da diese von beta_collect entfernt wurden:  */

                                   PUSHSTACK(S_m1,SET_ARITY(AP,2));
                                   PUSHSTACK(S_m,SET_ARITY(AP,1));
                                 }             
                              }

                            } /* Ende n-stelliges when */

                            if( pmredwhen (x, READSTACK (S_a), 1))
                            /* auf dem A-Stack liegt ein Nomatch-Deskriptor */ 
                               goto main_m;
                            else                                              
                            /* der body-Ausdruck liegt auf dem E-Stack  */
                               goto main_e;

                         } /* Ende when reduzierbar */

                         else           
                         {
                           if ( SIZEOFSTACK_TILDE > 0 )
                           /* Argumente fuer relativ freie Variable */
                           /* vorhanden => Closure anlegen:         */
                           {            
                              args = R_MATCH((*desc),code)[STELLIGKEIT];
                               /* Stelligkeit der pi-Abstraktion */
                              if ( ( x = st_clos(args,args) ) != NULL )
                                  PUSHSTACK(S_a,x);   /* auslagern */
                              else
                                  post_mortem("ear : heap out of space ");
                           }
                           else
                           {
                              POPSTACK(S_e);
                              PUSHSTACK(S_a,x);
                              goto main_m;
                           }

                           goto main_m;
                         }

      case TY_NOMAT:     if (R_NOMAT((*desc),reason) == 4)
                        /* guard-Berechnung wurde abgebrochen,da der Reduktions- */
                        /* zaehler abgelaufen war.Restaurierung der Applikation: */
                         {
                           PUSHSTACK(S_e,x);
                           PUSHSTACK(S_m,SET_ARITY(AP,1));
                           PUSHSTACK(S_m1,SET_ARITY(AP,2));
                           x = (STACKELEM)(R_NOMAT((*desc),guard_body)[3]); 
                            /* das Argument */
                           if (T_POINTER(x))
                              INC_REFCNT((PTR_DESCRIPTOR)x); 
                           PUSHSTACK(S_a,x);
      
                           if ((R_NOMAT((*desc), guard_body) != 0) &&
                               (T_AP_1 (READSTACK (S_m)))          &&
                               (_redcnt > 0)                    )
                           {
                              if (pm_red_nomat())
                              /* auf dem A-Stack liegt ein neuer Nomatch-Deskriptor */
                                 goto main_m;
                              else                                                    
                              /* auf dem E-Stack liegt der zu reduzierende body-Ausdruck */ 
                                 goto main_e;
                           }
                           else
                           {
                             if ( (_redcnt > 0) && (SIZEOFSTACK_TILDE > 0) )
                             {
                               if ( (x = st_clos(0,0)) == NULL)
                                  post_mortem("ear:Heap out of space!");
                               PUSHSTACK(S_a,x);                        
                             }
                             else
                             {                                
                                POPSTACK(S_e);          /* der Nomatchdesc */
                                PUSHSTACK (S_a, desc);  /* der Nomatchdesc */
                             } 

                             goto main_m;
                           }

                         } /* reason == 4 */

                         else /* der case-Ausdruck bzw. die pi-Abstraktion */
                              /* kann nicht weiter reduziert werden        */
                         {
                           PUSHSTACK(S_a,x);
                           goto main_m;
                         }


      case TY_CLOS : pf = R_CLOS((*desc),ptfd);
                     if ( ( !T_AP_1(READSTACK(S_m)) ) && ( R_DESC((*pf),type) != TY_NOMAT )  )
                     {
                      /* Closure nicht in Funktionsposition  */                       
                       PUSHSTACK(S_a,x); /* und auf den A - Stack legen */
                       goto main_m;
                     }
                     else
                       if (red_clos(desc))
                       /* Reduktion der Funktion, fuer die die Closure angelegt wurde */
                          goto main_e;
                       else                                                             
                       /* Closure kann nicht reduziert werden, der Deskriptor wurde  */
                       /* auf den A-Stack gelegt                                     */
                          goto main_m;  

  
      case TY_P_CLOS: if (!T_AP_1(READSTACK(S_m)) )  
                      {
                        PUSHSTACK(S_a,x); /* Closure-Descriptor traversieren */ 
                        goto main_m;
                      }
                      else
                        if (red_pclos(desc))
                            goto main_e;
                        else
                            goto main_m;  
      

      case TY_EXPR:      if (T_AP_1(READSTACK(S_m)))
                         /* d.h. Expressionpoinster an Funktionsposition :*/
                         {
                           load_expr((STACKELEM **)A_EXPR((*desc),pte));
                           DEC_REFCNT(desc);
                           goto main_e;
                         }
                         else    /* sonst nicht einlagern!!!!! */
                         {
                           PUSHSTACK(S_a,x);
                         }        /* end if */
                         goto main_m;
      default: /* post_mortem("ear: Unexpected pointertype"); */ 
               PUSHSTACK(S_a,x);
               goto main_m;


    }  /* case C_EXPRESSION */
   
    default:
         if ( R_DESC((*desc),class) != C_DIGIT )
         {
            DBUG_PRINT("S_e",(" xxx  %8.8x ",x));  
         }
         else
         {
            DBUG_PRINT("S_e",(" Zahl  %8.8x ",x));
         } 

         PUSHSTACK(S_a,x);
         goto main_m;

   }   /* end of switch */

  } /* Ende von T_POINTER */


  if T_CON(x)
  {  
    if ( T_AP(x) )
    {             
       DBUG_PRINT("S_e",(" Ap  %8.8x ",x));
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }

    if (T_SNAP(x))
      {        
              DBUG_PRINT("S_e",("snap  %8.8x ",x));
              snapargs = ARITY(x) - 1;

              for( i = 0 ; T_AP_1(READSTACK(S_m)) ; i++ , POPSTACK(S_m) );
                                
              y = READSTACK(S_m);
              while ( (T_DOLLAR(y)) || ( T_SNAP(y)) )
              { 
                POPSTACK(S_m);          

                if ( T_DOLLAR(y) )
                  POP_I_BLOCK(hilf);
                else
                  POP_TILDE_BLOCK(hilf);

                y = READSTACK(S_m);
              }

              PUSHSTACK(S_m,SET_ARITY(SNAP,0));

              for(  ; i> 0 ; i-- ) PUSHSTACK(S_m,SET_ARITY(AP,1));

              /* Block anlegen */
              PUSH_TILDE_BLOCK(snapargs);

              for (i = snapargs;i > 0 ; i--)
              {
                  PUSH_TILDE(( x = POPSTACK(S_e))); 
                  DBUG_PRINT("S_e",("tilde  %8.8x ",x));
               }
              goto main_e;


    } /* T_SNAP */
 
    if T_LREC(x)
    { 
       DBUG_PRINT("S_e",("lrec  %8.8x ",x));
       POPSTACK(S_e);     /* p-lrec */
       goto main_e;
    }

/* tritt im Startausdruck eines LREC 's auf oberer Ebene ein        */
/* Conditional auf ,so wird dieses geschlossen , da beim Einlagern  */
/* die entsprechenden Tilde - Var nicht im obersten Block zu        */
/* finden sind(dieser enthaelt die Argumente fuer die rel. freien   */
/* Var des LREC 's) . Es wird kurzfristig auf dem Tilde-Stack ein   */
/* Block aufgebaut , welcher die Argumente fuer das Conditional     */
/* enthaelt ; dieser wird nach dem Einlagern sofort entfernt        */

 if ( T_CAP(x) )
  {
    if (T_AP_1(READSTACK(S_m)) &&    /* Reduktion eines conditional */
        T_BOOLEAN(READSTACK(S_a)))                            
    {  
        if (! _beta_count_only)   /* dies ist keine Beta-Reduktion */
             _redcnt--;   

        if T_AP_2(READSTACK(S_m1))
         {
           PPOPSTACK(S_m);
           PPOPSTACK(S_m1);
         }
        else
           WRITE_ARITY(S_m1,READ_ARITY(S_m1) - 1);

       /* Block auf dem Tilde-Stack aufbauen  */

        snapargs=ARITY(x) - 1;
        PUSH_TILDE_BLOCK(snapargs);
        for ( i=snapargs ; i > 0 ; i-- )
             PUSH_TILDE(POPSTACK(S_e));

       /* CAP(0) auf m  */

  /*   PUSHSTACK(S_m,SET_ARITY(CAP,0));  */

       /* p-cond entfernen */
   
       desc=(PTR_DESCRIPTOR)POPSTACK(S_e);
              

       if T_SA_TRUE(POPSTACK(S_a))
               load_expr((STACKELEM **)A_COND((*desc),ptte));
       else
              load_expr((STACKELEM **)A_COND((*desc),ptee));
       DEC_REFCNT(desc);

      /* CAP(0) und obersten Block vom Tilde-Stack entfernen */

 /*    POPSTACK(S_m);  */
       POP_TILDE_BLOCK(hilf);
       goto main_e;
    }
    else
      /* Conditional nicht reduzierbar  =>  */
     /* CAP(n) ... p_cond von E nach A traversieren */

     { 

        PUSHSTACK(S_e,x);           /* CAP wieder auf S_e legen */
        TRAV_E_A;

        goto main_m;
     }
    }  /* Ende T_CAP */

  
    if (T_K_COMM (x)) 
    {
        x = POPSTACK(S_e);		/* comment oder aehnliches	*/
        if (T_POINTER(x))
           DEC_REFCNT((PTR_DESCRIPTOR)x);
        goto main_e;
    }



    /* alle sonstigen Konstruktoren: */
    if (ARITY(x) > 0)
    {
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }
    else
    {
       PUSHSTACK(S_a,x);
       goto main_m;
    }
  } /* Ende von T_CON */

  if (T_FUNC(x))
  {                           
    int result;  /* lokale Variable */
    PUSHSTACK(S_e,x);       
    args = _argsneeded = FUNC_ARITY(x);              /* ca */

    delta_collect();      /* Applikatoren aufsammeln */

    if (_argsneeded > 0)
    {
     /* Eine Reduktion war also nicht moeglich.                     */
     /* Bisher gefundene Argumente und die Funktion hinter Closure-Deskriptor.*/

     if (args != _argsneeded) /* es ist mindestens ein Argument vorhanden */
        { 
         if ((x = st_prim_clos(args)) != NULL)
            /* auslagern */
             PUSHSTACK(S_a,x);
         else
            post_mortem("ear : heap out of space ");
         goto main_m;
         }  
      else  
       /* es ist kein Argument vorhanden,also braucht keine Closure angelegt */
       /* zu werden.                                                         */

         {
          PUSHSTACK(S_a,POPSTACK(S_e));  
          goto main_m; 
         }

    }

    _desc = NULL;         /* fuer die Interruptbehandlung */

    /*---------------------------------------------------------------------
     *  Der Tabellensprung zur entsprechenden C-Routine :
     *  Die Argumente der primitiven Funktion werden in ear von dem A-Stack
     *  geholt und als Paramter an diese uebergeben. Die Funktionen, die
     *  mehrfach in der Tabelle auftauchen, lesen auf dem E-Stack, um her-
     *  auszufinden, wer sie beauftragt hat. Scheitert eine Funktion
     *  so liefert sie eine Null zurueck. Mit Hilfe der Funktion failure
     *  wird dann von ear die Applikation restauriert. Liefert die primitive
     *  Funktion eine Eins zurueck, so hat die Reduktion geklappt. Das
     *  Ergebnis steht dann in der globalen Variable _desc. Es wird auf den
     *  A-Stack geschieben, das Funktionssymbol wird von E-Stack entfernt,
     *  und falls Delta-Reduktionen gezaehlt werden sollen, wird der Reduktions-
     *  zaehler um eins vermindert.
     *  Eine besondere Behandlung erfaehrt die Funktion red_lselect. Bei
     *  Erfolg liefert sie keine Eins sondern eine Zwei zurueck. Das aus-
     *  gewaehlte Listenelement wird dann auf den E-Stack gelegt und nicht
     *  auf den A-Stack. Ausserdem setzt ear seine Taetigkeit auf der
     *  E-Linie und nicht auf der M-Linie fort.
     *  Bei Interrupt oder Heapueberlauf wird die Interruptbehandlung von
     *  ear (siehe oben) angesprungen.
     *-------------------------------------------------------------------------
     */
    switch(args)       /* Argumente aufsammeln */
    {
      case 2:  arg1 = POPSTACK(S_a);
               arg2 = READSTACK(S_a);
               if ((result = (* red_func_tab1[FUNC_INDEX(x)])(arg1,arg2)) == 1)
                 break;
               if (result == 0)
               {
                 failure(2,arg1,arg2,0,0);                       
                 goto main_m;
               }
               /* vorsicht mit ansonsten ! */
/* ca
#if DEBUG
               if (result != 2)
                 post_mortem("ear: result of primitive function != 0,1,2");
#endif
*/
               /* ansonsten: lselect wurde aufgerufen */
               PPOPSTACK(S_a); /* das letzte Argument */
               WRITESTACK(S_e,(STACKELEM)_desc);
               if (!_beta_count_only) _redcnt--;
               goto main_e;
      case 3:  arg1 = POPSTACK(S_a);
               arg2 = POPSTACK(S_a);
               arg3 = READSTACK(S_a);
           if ((result = (* (red_func_tab1[FUNC_INDEX(x)]))(arg1,arg2,arg3)) == 1)
                 break;
               if (result == 0)
               {
                 failure(3,arg1,arg2,arg3,0);
                 goto main_m;
               }
               /* ansonsten: select wurde aufgerufen  */
               PPOPSTACK(S_a); /* das letzte Argument */
               WRITESTACK(S_e,(STACKELEM)_desc);
               if (!_beta_count_only) _redcnt--;
               goto main_e;
      case 1:  arg1 = READSTACK(S_a);
               if ((result = (* (red_func_tab1[FUNC_INDEX(x)]))(arg1)) == 1)
                 break;
               failure(1,arg1,0,0,0);
               goto main_m;
/* ca
#if DEBUG
      case 4:  ....

      default: post_mortem("ear: delta-reduction: args > 4 od < 1");
#endif
*/
      default: arg1 = POPSTACK(S_a);
               arg2 = POPSTACK(S_a);
               arg3 = POPSTACK(S_a);
               arg4 = READSTACK(S_a);
       if ((result = (* (red_func_tab1[FUNC_INDEX(x)]))(arg1,arg2,arg3,arg4)) == 1)
                 break;
               failure(4,arg1,arg2,arg3,arg4);
               goto main_m;
    }

    /* die Delta-Redukion hat geklappt: */
    PPOPSTACK(S_e); /* das Funktionssymbol */
    WRITESTACK(S_a,(STACKELEM)_desc);
    if (!_beta_count_only) _redcnt--;
#if DEBUG
    if (READSTACK(S_a) == NULL)
      post_mortem("ear: primitive function returns a null");
#endif
     goto main_m;

  } /* Ende der Behandlung der primitiven Funktionen */

/*
  if (T_DIS_ON(x))
     {
       POPSTACK(S_e);
       y = POPSTACK(S_e);
       if (T_POINTER(y))
          DEC_REFCNT( (PTR_DESCRIPTOR) y); 
       arity = READ_ARITY(S_m);
       WRITE_ARITY(S_m,arity - 2);
       WRITE_ARITY(S_m1,arity - 2);
       goto main_e;
     }                                           

*/

  /* sonstige Stackelemente auf dem E-Stack */
  PUSHSTACK(S_a,x);
  goto main_m;

}
else
{
/******************************************************************************/
/*                                                                            */
/*                    REDUKTIONSZAEHLER gleich NULL                           */
/*                                                                            */
/******************************************************************************/

  /* Reduktionszaehler ist abgelaufen => */
  /* der auf dem E - Stack liegende Ausruck wird nach A traversiert , */
  /* Funktionen werden hierbei geschlossen                            */  

  if T_POINTER(x)
  {
    desc = (PTR_DESCRIPTOR) x;     /* mit desc zu arbeiten ist einfacher */

    if ( (R_DESC((*desc),class)) != C_EXPRESSION )
    {                              /* ein Daten- oder Variablendeskriptor */
      PUSHSTACK(S_a,x);
      goto main_m;
    }
    switch(R_DESC((*desc),type))
    {
     case TY_SUB :
     case TY_LREC_IND :
     case TY_SWITCH :
     case TY_MATCH :
     case TY_NOMAT :
                           if ( SIZEOFSTACK_TILDE > 0 )
                           {  
                               /* der oberer Block von S_tilde wird kopiert */
                               /* die Argumente plus einem SNAP werden auf  */
                               /* S_e gelegt                                */
                                
                               PUSHSTACK(S_e,x);
                               i=0;
                               READ_TILDE(i,y);
                               while( ! ( T_KLAA(y) ) )
                                    {
                                      PUSHSTACK(S_e,y);
                                      i++;
                                      READ_TILDE(i,y);

                                    }
                               PUSHSTACK(S_e,SET_ARITY(SNAP,i+1));   
                               TRAV_E_A;
                               goto main_m;
                           }
                           else
                           {
                               PUSHSTACK(S_a,x);
                               goto main_m;
                           }       

     case TY_COND :      /* Conditional nicht reduzierbar  =>  */
                         /* Conditional schliessen             */

                         if ((SIZEOFSTACK_I > 0)  ||    
                             (SIZEOFSTACK_TILDE > 0 ))
                          {
                           /* Argumente fuer gebundene und rel. freie */
                           /* Var. werden zusammen mit p-cond in eine */
                           /* Closure eingetragen                     */
                             
                            PUSHSTACK(S_e,x);

                            if( ( x=closecond() ) == (STACKELEM) NULL)
                              post_mortem("ear:heap out of space ");

                            else
                              /* Closure auf S_a legen */
                                 PUSHSTACK(S_a,x); 
                           }
                           else

                             /* Cond. enthaelt weder gebundene noch */
                             /* rel. freie Variable =>              */
                             /* p-cond auf S_a legen                */
                                
                                PUSHSTACK(S_a,x);

                         goto main_m;

     default :           PUSHSTACK(S_a,x);
                         goto main_m;
    }
  } /* Ende if T_POINTER */

  if( T_CON(x) )                    
  { 
    if ( T_LREC(x) )
    {                           
       PUSHSTACK(S_e,x);                  

       if ( SIZEOFSTACK_TILDE > 0 )
       {  
         /* der oberer Block von S_tilde wird kopiert */
         /* die Argumente plus einem SNAP werden auf  */
         /* S_e gelegt                                */
                               
            i=0;
            READ_TILDE(i,y);
            while( ! ( T_KLAA(y) ) )
            {
              PUSHSTACK(S_e,y);
              i++;
              READ_TILDE(i,y);
            }
            PUSHSTACK(S_e,SET_ARITY(SNAP,i+1));   
        }

       TRAV_E_A;
       goto main_m;    
    } 

    if (  T_SNAP(x) || T_CAP(x)  )
    {
       PUSHSTACK(S_e,x);
       TRAV_E_A;
       goto main_m;    
    }
    else
    {

      /* alle sonstigen Konstruktoren: */
        if (ARITY(x) > 0)
        {
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
          goto main_e;
        }
        else
        {
          PUSHSTACK(S_a,x);
          goto main_m;
        }

    }
  }
  else
  {
    if ( T_DIS_ON(x) )
    {
       PUSHSTACK(S_e,x);
       TRAV_E_A;
       goto main_m;
    }
    else
    {
      PUSHSTACK(S_a,x);
      goto main_m;
    }
  }
} /* Ende _redcount == NULL */



main_m:
  x = READSTACK(S_m);

weiter:  
  if (T_CON(x))
  {
    if ( T_UNCHAINED_CONSTR(x) )
    {
        arity = ARITY(x);
        if (arity > 1)
        {
          WRITE_ARITY(S_m,arity - 1);
          goto main_e;
        }
        else if ( arity == 1 )
             {
               PPOPSTACK(S_m);
               PUSHSTACK(S_a,POPSTACK(S_m1));
    
               if (_redcnt > 0) /* nur dann wird der Ausdurck auf dem A-Stack ausgelagert,
                            da sonst evtl. ein Redex in den Heap gelangt        */
               {
                  if ((T_LIST(x)) || (T_STRING(x)))  /* eine Liste wird  ausgelagert, */
                  {
                    if ((x = store_l()) != NULL)
                         PUSHSTACK(S_a,x);
                    else
                        _redcnt = 0;  /* da die Liste nicht ausgelagert werden kann, muss */
                    goto main_m;     /* der Abbruch der Reduktionsphase erzwungen werden */
                  }

                  if ((T_AP(x)) || ( (T_SNAP(x)) && (ARITY(x) > 0)))  /* ein Ausdruck wird  ausgelagert, */
                  {
                    if ((x = st_expr()) != NULL)
                        PUSHSTACK(S_a,x);
                    else
                       _redcnt = 0;  /* da der Ausdruck nicht ausgelagert werden kann, muss */
                    goto main_m;     /* der Abbruch der Reduktionsphase erzwungen werden */
                  }

                  post_mortem("ear: unexpected constructor on A-stack");

               } /* _redcnt > 0 */

               else         /* Reduktionszaehler gleich Null */
                 goto main_m;

             } /* arity == 1 */

             else
             {
               /* also arity == 0  => snap(0) auf S_m */
                  if (T_SNAP(x))
                  {
                     POPSTACK(S_m);
                     POP_TILDE_BLOCK(hilf);
                     goto main_m;
                  }
                  else 
                     post_mortem("ear: unexpected constructor on A-stack");
             } /* arity == 0 */

    } /* UNCHAINED-Konstruktor */
    
    else  /* also CHAINED- oder Fixkonstruktor */
    {

      /*Abfrage ob PM_GUARD vorliegt. Falls ja liegt */
      /*der Guard-Ausdrucks einer lambda-Abstraktion */
      /*auf dem A-Stack                              */

       if (T_PM_GUARD(x))
       { 
        arity = ARITY(x);

        if ( arity > 0)
           {
            if (   pmmat_2()   )
              /*Kontrolle zurueck auf den M-Stack*/
              goto main_m;
            else
             /*Kontrolle zurueck auf den E-Stack*/
               goto main_e;
           } 

        else 
           { 
            if (   pmwhenmat_2()   )
              /*Kontrolle zurueck auf den M-Stack*/
              goto main_m;
            else
             /*Kontrolle zurueck auf den E-Stack*/
               goto main_e;
           }

       } /* PM_GUARD */

#if EAGER
       if (T_PM_EAGER(x))
       {
           if (   pm_eval_list()   )
           {
             /*Kontrolle zurueck auf den M-Stack*/
             goto main_m;
           }
           else
           {
            /*Kontrolle zurueck auf den E-Stack*/
              goto main_e;
           }
       }
#endif

       if (T_PM_NOMAT(x))
       {   
         if (ARITY(x) > 0)
           if (   pm_nomat_2()   )
           {
             /*Kontrolle zurueck auf den E-Stack*/
             goto main_m;
           }
           else
           {
            /*Kontrolle zurueck auf den E-Stack*/
              goto main_e;
           }                                 
         else
           /* also liegt hinter dem Nomatch-Deskriptor */
           /* ein Match-Deskriptor                     */  
           if (   pm_nomat_when()   )
           {
             /*Kontrolle zurueck auf den E-Stack*/
             goto main_m;
           }
           else
           {
            /*Kontrolle zurueck auf den E-Stack*/
              goto main_e;
           }                                 
       }
    
       if (T_K_COMM (x)) 
       /* Der Kommentarstring liegt auf dem A-Stack, den Konstruktor */
       /* ebenfalls auf den A-Stack legen:                           */
       {
          arity = ARITY(x);
          if (arity > 1)
          {
            WRITE_ARITY(S_m,arity - 1);
            goto main_e;
          }
          else 
          {
           PPOPSTACK(S_m);
           PUSHSTACK(S_a, POPSTACK(S_m1));
           goto main_m;
          }
       }

  } /* CHAINED- oder Fixkonstruktor */

} /* T_CON */

  /* also ein DOLLAR oder KLAA : */
  PPOPSTACK(S_m);

  if T_DOLLAR(x)
  {
    POP_I_BLOCK(hilf);    /* hilf ist eine Hilfsvariable des Makros */
    goto main_m;
  }
  END_MODUL("ear");
  DBUG_VOID_RETURN;
}
 /* end of rear.c */

