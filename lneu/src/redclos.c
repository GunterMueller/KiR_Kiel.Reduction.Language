/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/*************************************************************************/
/*                                                                       */
/* redclos.c                                                             */
/*                                                                       */
/*   internal:  red_clos, red_pclos.                                     */
/*                                                                       */
/*************************************************************************/

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



/*************************************************************************/
/*                                                                       */
/* Externe Variablen:                                                    */
/*                                                                       */
/*************************************************************************/


extern int _argsneeded;           /* state.c - Zahl der benoetigten Argumente */
                                  /*           fuer eine Funktion             */
extern BOOLEAN _beta_count_only;  /* <r>                                      */
extern BOOLEAN _digit_recycling;  /* <w>, fuer die Ausnahmebehandlung         */
extern BOOLEAN _interrupt;        
extern BOOLEAN _heap_reserved;    
extern jmp_buf _interruptbuf;   
extern PTR_DESCRIPTOR _desc;      /* <w>, der Ergebnisdeskriptor,auch         */
                                  /*      fuer die Ausnahmebehandlung         */


/*************************************************************************/
/*                                                                       */
/* Spezielle externe Routinen:                                           */
/*                                                                       */
/*************************************************************************/


extern  BOOLEAN pmredsw ();       /* pmredsw.c        */
extern  BOOLEAN pmmat_2 ();   
#if EAGER
extern  BOOLEAN pmredesw ();     
#endif

extern  BOOLEAN pmredwhen ();     /* pmredwhen.c      */
extern  BOOLEAN pmwhenmat_2 ();

extern  BOOLEAN pm_red_nomat ();  /* pmred-nomat.c    */

extern int red_dummy();           /* hfunc.c          */

extern PTR_DESCRIPTOR newdesc();  /* rheap.c          */

extern  BOOLEAN pm_red_nomat ();  /* pmred-nomat.c    */			    
extern  BOOLEAN pm_nomat_2 ();			    

extern  BOOLEAN pm_nomat_when (); /* red-nomat-when.c */
			    
extern  void beta_collect();      /* rear.c           */  
extern  void delta_collect();   

extern void pfail();             /* p-fail.c          */
extern STACKELEM st_clos();    
extern STACKELEM st_prim_clos(); 

extern int test_fname();         /* testf.c           */


extern STACKELEM store_l();      /* rhinout.c         */
extern STACKELEM st_arglist(); 
extern STACKELEM store_e();     
extern void load_expr();   

#if MEASURE
extern int minc;                 /* rbreak.c          */
#endif

extern void failure();           /* query.c           */




/*****************************************************************************/
/*                                                                           */
/* red_clos uebernimmt die Reduktion von Closures fuer benutzerdefinierte    */
/* Funktionen                                                                */
/*                                                                           */
/*****************************************************************************/



BOOLEAN red_clos(desc)
PTR_DESCRIPTOR desc;            
  /* desc: betrachteter Closure-Deskriptor */

  /* ist das Ergebnis von red_clos                         */
  /* = FALSE, so wird im ear auf der M-Linie fortgefahren, */
  /* = TRUE, auf der E-Linie                               */
{

           STACKELEM                         hilf;
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

START_MODUL("red_clos");
     


 args = (int)R_CLOS((*desc),ndargs);           
    /* = Anzahl der benoetigiten Argumente */

 pf = R_CLOS((*desc),ptfd);                  
    /* Verweis auf die Funktion, fuer die die Closure angelegt wurde */


 if ( (R_DESC((*pf),type) != TY_COND ) ) 
 { 
   p_arg = (STACKELEM*)R_CLOS((*desc),pta);

   _argsneeded = args ;

   beta_collect();

   if (_argsneeded > 0) 
    /*eine Reduktion ist nicht moeglich */
  
   {   
      if (((R_DESC((*desc),type) == TY_SWITCH) ||
           (R_DESC((*desc),type) == TY_MATCH)))
      /* in diesem Fall muss der Reduktionszaehler 'zurueckgesetzt' */
      /* werden.                                                    */
         _redcnt += (args - _argsneeded); 

      if (R_DESC((*desc),ref_count) > 1)
      {
       /* einen neuen CLOSURE-Deskriptor anlegen: */

        if ((pf=newdesc()) == NULL)
            post_mortem("ear:heap out of space!");
            
        DESC_MASK(pf,1,C_EXPRESSION,TY_CLOS);

        fun_desc = (PTR_DESCRIPTOR)R_CLOS((*desc),ptfd);

        L_CLOS((*pf),ptfd) = fun_desc;

        INC_REFCNT(fun_desc);

        L_CLOS((*pf),nfv) = R_CLOS((*desc),nfv);  
                    
        index = p_arg[0];

        if (newheap(index+2,A_CLOS((*pf),pta)) ==NULL)
           post_mortem("ear:heap out of space!");

        RES_HEAP;
        p_arg_1= (STACKELEM *)R_CLOS((*pf),pta);

        /* Kopieren der Argumentliste: */

        for (i=R_CLOS((*desc),ndargs) + 1;i <= index ; i++)
        {
           y = p_arg[i];
           p_arg_1[i] = y;
           if (T_POINTER(y))
               INC_REFCNT((PTR_DESCRIPTOR)y);
        }

        p_arg_1[0] = p_arg[0];
        p_arg_1[index+1]=p_arg[index+1];
        REL_HEAP;

        DEC_REFCNT(desc);
      
        /* Ab hier weiter mit dem neuen Deskriptor */
        desc = pf;

     }  /* Ende neuen Closure-Deskriptor anlegen */
                    

    /* Zusaetzliche Argumente in pta eintragen */

    L_CLOS((*desc),ndargs) = _argsneeded;     

    RES_HEAP; 

    p_arg = R_CLOS((*desc),pta);

    for (i=args ; i > _argsneeded ; i--)
        p_arg[i] = POPSTACK(S_a);

    REL_HEAP;

    /* Closure-Deskriptor auf den A-Stack legen: */
    PUSHSTACK(S_a,(STACKELEM)desc);

    END_MODUL("red_clos");

    /* im ear weiter auf der M-Linie: */
    return(FALSE); 
    
    } /* Ende _argsneeded > 0 */

    else 
   /* Fuer eine Reduktion sind genuegend Argumente vorhanden. */
   /* Die Argumente fuer die relativ freien Variablen hinter  */
   /* pta werden aufden Tilde-Stack gelegt.                   */
   
    { 
      /* Reduktion jetzt moeglich */
      anz_fv = R_CLOS((*desc),nfv);

      if (T_DOLLAR(READSTACK(S_m)))
      {
         POPSTACK(S_m);  /* $ */
         POP_I_BLOCK(hilf);  
      }                                 

      if (anz_fv > 0)
      /* Relativ freie Variable (falls vorhanden)             */
      /* auf den Tilde-Stack und ein SNAP(0) auf den M-Stack: */
      {
         for (arity = 0; T_AP_1(READSTACK(S_m));
              PPOPSTACK(S_m),arity++);

         PUSHSTACK(S_m,SET_ARITY(SNAP,0));

         for ( ; arity > 0; --arity)
              PUSHSTACK(S_m,(STACKELEM) AP_1);
 
         PUSH_TILDE_BLOCK(anz_fv);

         anz_bv = p_arg[0] - anz_fv;    

         RES_HEAP;   

         for (i = p_arg[0]; i > anz_bv; i--)
         {
           y = p_arg[i];
           PUSH_TILDE(y);
           if (T_POINTER(y))
              INC_REFCNT((PTR_DESCRIPTOR)y);
         } 

         REL_HEAP;

       } /* anz_fv > 0 */


 
       if (R_DESC((*pf),type) == TY_SUB )   /* Closure fuer ein SUB */ 
                      
       { 
        /* Ein Dollar unter alle AP_1 auf dem M-Stack legen */

         anz_fv = R_CLOS((*desc),nfv);

         for (arity = 0; T_AP_1(READSTACK(S_m));
             PPOPSTACK(S_m),arity++);

         PUSHSTACK(S_m,DOLLAR);

         for ( ; arity > 0; --arity)
             PUSHSTACK(S_m,(STACKELEM) AP_1);

   
        /* Inkarnationsblock anlegen und fuellen: p_arg verweist auf die Liste der bisher */
        /* vorhandenen Argumente. anz_bv ist die Anzahl der benoetigten Argumente.        */

        RES_HEAP;
        anz_bv = p_arg[0] - anz_fv;    

       /* Argumente aus pta auf den I-Stack:*/

        PUSH_I_BLOCK(anz_bv); 

        for (i=anz_bv;i > args ; i--)
        {
          y = p_arg[i];
          PUSH_I(y);
          if (T_POINTER(y))
             INC_REFCNT((PTR_DESCRIPTOR)y);
        }   

        REL_HEAP;

       /* Jetzt hinzugekommene Argumente auf den I_Stack */
        for (i = args; i > 0; i--)
        {                  

#if DEBUG
         if (T_CON(READSTACK(S_a)))  
            post_mortem("ear: function receives constructor");
#endif

         PUSH_I(POPSTACK(S_a));
        }

       /* Funktion einlagern: */    

        load_expr((STACKELEM **)A_FUNC((*pf),pte));
  
        DEC_REFCNT(desc);
        END_MODUL("red_clos");

        /* weiter auf der E-Linie des ear */
        return(TRUE);

       } /* Ende Closure fuer ein SUB */
                                      


       if (R_DESC((*pf),type) == TY_SWITCH) 
          /* Closure fuer ein Case */
       {
         if (R_SWITCH((*pf),anz_args) > 1) 
            /* d.h. es handelt sich um ein n-stelliges case */
         {
           anz_bv = R_SWITCH((*pf),anz_args);
           /* = Anzahl der benoetigten Argumente */   

           anz_vorh = anz_bv - args;
           /* = Anzahl der in der Closure eingetragenen Argumente */

           /* Argumente aus dem Closure-Deskriptor zusaetzlich auf den A-Stack: */
           for (i = anz_vorh - 1 ; i >= 0 ; i--) 
           {
              y = p_arg[anz_bv - i];
              if (T_POINTER(y))
                 INC_REFCNT((PTR_DESCRIPTOR)y);   
              PUSHSTACK(S_a,y);
           }

           /* Eintragen der Argumente in einen Listendeskriptor */
           PUSHSTACK(S_a,SET_ARITY(LIST,anz_bv));

           if ((y = st_arglist()) != NULL)
              PUSHSTACK(S_a,y);
           else
              post_mortem("ear:Heap out of space!");

         } /* anz_args > 1 */

 
         /* Switch-Deskriptor auf den E-Stack: */
         PUSHSTACK(S_e,pf);
         INC_REFCNT(pf);

         PUSHSTACK(S_m,SET_ARITY(AP,1));      /* Die AP's auf den m-Stacks werden */
         PUSHSTACK(S_m1,SET_ARITY(AP,2));     /* in pmredswi benoetigt.           */

         if ( pmredsw (pf, READSTACK (S_a), 1))   
           /* auf dem A-Stack liegt ein Nomatch-Deskriptor */
         { 
           END_MODUL("red_clos");
           /* weiter auf der M-Linie des ear */
           return(FALSE);
         }

         else
         {
           END_MODUL("red_clos");
           return(TRUE);
         }

       } /* Ende Closure fuer ein Case */                                              



       if (R_DESC((*pf),type) == TY_MATCH) 
         /* Closure fuer ein When */
       {
         if ((int)(R_MATCH((*pf),code)[STELLIGKEIT]) > 1) 
            /* d.h. es handelt sich um ein n-stelliges when */
         {
           anz_bv = (int)(R_MATCH((*pf),code)[STELLIGKEIT]);
           /* = Anzahl der benoetigten Argumente */   
 
           anz_vorh = anz_bv - args;
           /* = Anzahl der in der Closure eingetragenen Argumente */

           /* Argumente aus dem Closure-Deskriptor zusaetzlich auf den A-Stack: */
           for (i = anz_vorh - 1 ; i >= 0 ; i--) 
           {
             y = p_arg[anz_bv - i];
             if (T_POINTER(y))
                INC_REFCNT((PTR_DESCRIPTOR)y);   
             PUSHSTACK(S_a,y);
           }

          /* Eintragen der Argumente in einen Listendeskriptor */
           PUSHSTACK(S_a,SET_ARITY(LIST,args));

           if ((y = st_arglist()) != NULL)
              PUSHSTACK(S_a,y);
           else
              post_mortem("ear:Heap out of space!");

         } /* n-stelliges when */
 
         PUSHSTACK(S_e,pf); /* den Match-Deskriptor */

         INC_REFCNT(pf);

         PUSHSTACK(S_m,SET_ARITY(AP,1));      /* Die AP's auf den m-Stacks werden */
         PUSHSTACK(S_m1,SET_ARITY(AP,2));     /* in pmredswi benoetigt.           */

         if ( pmredwhen (pf, READSTACK (S_a), 1))   
           /* auf dem A-Stack liegt ein Nomatch-Deskriptor */
         {
            END_MODUL("red_clos");

            /* weiter auf der M-Linie des ear */  
            return(FALSE);
         }
         else
         {
           END_MODUL("red_clos");               
       
           /* weiter auf der E-Linie dedes ear */
           return(TRUE);
         }

       } /* Ende Closure fuer ein when */        



       if (R_DESC((*pf),type) == TY_NOMAT) 
      /* Closure fuer einen Nomatch-Deskriptor */
      {  
        if ( R_NOMAT((*pf),reason) != 4 )
        /* keine weitere Reduktion moeglich, Closure auf A */ 
        { 
           PUSHSTACK(S_a,(STACKELEM)desc);
           END_MODUL("red_clos");
           return(FALSE);
        }
        else
        {   
           /* Verweis auf den Nomatch-Deskriptor auf E, */
           /* weiter auf der E-Linie des ear            */ 
           PUSHSTACK(S_e,pf);
           INC_REFCNT(pf); 

           DEC_REFCNT(desc);
           END_MODUL("red_clos");
           return(TRUE);
        }

      } /* Ende Closure fuer einen Nomatch-Deskriptor */                                                                                
                                    
    }  /* Ende Reduktion moeglich */  

 } /* (R_DESC((*pf),type) != TY_COND ) */


 else
  /* Closure fuer ein Conditional =>  trifft man nur beim */
  /* verpointerten Wiederaufsetzen an                     */
   
 {
   /* ist dieses Conditional reduzierbar,so wird die Closure */
   /* aufgeloest und der then bzw der else Teil des          */
   /* Conditionals eingelagert                               */

   if ( (T_AP_1(READSTACK(S_m))) &&    /* Reduktion eines conditional */
        (T_BOOLEAN(READSTACK(S_a))) &&                           
        ((_redcnt > 0) || _beta_count_only))                           
   { 
      /* Conditional reduzierbar */                                     
                             
     if (! _beta_count_only)   /* dies ist keine Beta-Reduktion */
        _redcnt--;

     if (T_AP_2(READSTACK(S_m1)))
     {
        PPOPSTACK(S_m);
        PPOPSTACK(S_m1);
     }
     else
     {
       arity = READ_ARITY(S_m1) - 1;
       WRITE_ARITY(S_m1,arity);
     }

     /* Closure aufloesen */
                                
  
     /* Verweis auf Argumentliste */

     p_arg = (STACKELEM*)R_CLOS((*desc),pta);

     /* Verweis auf Conditional Deskriptor */

     pf = R_CLOS((*desc),ptfd);

     anz_fv = R_CLOS((*desc),nfv);
   
     anz_bv = p_arg[0] - anz_fv;    
   
    /* Relativ freie Variable (falls vorhanden)*/
    /* auf den Tilde-Stack :                   */

     if ( anz_fv > 0 )
     {
        PUSH_TILDE_BLOCK(anz_fv);
        RES_HEAP;   

        for (i = p_arg[0]; i > anz_bv; i--)
        {
           y = p_arg[i];
           PUSH_TILDE(y);
           if (T_POINTER(y))
              INC_REFCNT((PTR_DESCRIPTOR)y);
        } 

        REL_HEAP;

        /* SNAP(0) unter alle AP(1) auf dem M-Stack: */

        for (arity = 0; T_AP_1(READSTACK(S_m));
             PPOPSTACK(S_m),arity++);

        PUSHSTACK(S_m,SET_ARITY(SNAP,0));

        for ( ; arity > 0; --arity)
            PUSHSTACK(S_m,(STACKELEM) AP_1);
 
     } /* anz_fv > 0 */

                             
     /* Inkarnationsblock anlegen und fuellen: */

     if ( anz_bv > 0 )
     {
       /* Argumente aus pta auf den I-Stack:*/
   
       RES_HEAP;

       PUSH_I_BLOCK(anz_bv);      

       for (i=anz_bv;i > 0 ; i--)
       {
         y = p_arg[i];
         PUSH_I(y);
         if (T_POINTER(y))
            INC_REFCNT((PTR_DESCRIPTOR)y);
       }   
 
       REL_HEAP;

        PUSHSTACK(S_m,DOLLAR);

    } /* anz_bv > 0 */
         

    /* Einlagern des then bzw else Teils ders conditionals */

    if (T_SA_TRUE(POPSTACK(S_a)))
       load_expr((STACKELEM **)A_COND((*pf),ptte));
    else
       load_expr((STACKELEM **)A_COND((*pf),ptee));

    DEC_REFCNT(desc);
    END_MODUL("red_clos");

    /* weiter auf der E-Linie des ear */
    return(TRUE);                       

  } /* Ende 'Conditional reduzierbar' */

  else
  {
    PUSHSTACK(S_a,(STACKELEM)desc); /* und auf den A - Stack legen */
    END_MODUL("red_clos");
    return(FALSE);
  } 

 } /* Ende Closure in Funktionsposition */

} /* Ende Closure fuer Conditional */

                               





/*****************************************************************************/
/*                                                                           */
/* red_pclos uebernimmt die Reduktion von Closures fuer primitive Funktionen */
/*                                                                           */
/*****************************************************************************/


BOOLEAN red_pclos(desc)
PTR_DESCRIPTOR desc;
  /* desc: betrachteter Closure-Deskriptor */

  /* ist das Ergebnis von red_pclos                        */
  /* = FALSE, so wird im ear auf der M-Linie fortgefahren, */
  /* = TRUE, auf der E-Linie                               */

{

           STACKELEM                         hilf;
  register unsigned short                   arity;
  register int                               args; 
           int                           i,ndargs;
           int                     anz_vorh,index;
           STACKELEM          arg1,arg2,arg3,arg4;
           STACKELEM                       arg[4];
           STACKELEM                            y;
           STACKELEM              *p_arg,*p_arg_1;
           PTR_DESCRIPTOR                      pf; 
           FCTPTR                       prim_func;

START_MODUL("red_pclos");

args = R_P_CLOS((*desc),ndargs);  
   /* Anzahl der benoetigten Argumente */

_argsneeded = args;

delta_collect();

if (_argsneeded > 0)
  /*eine Reduktion ist nicht moeglich */

{   
  if (R_DESC((*desc),ref_count) > 1)
  {
     /* einen neuen Closure-Deskriptor anlegen: */
 
    if ((pf=newdesc()) == NULL)
         post_mortem("ear:heap out of space!");
 
    DESC_MASK(pf,1,C_EXPRESSION,TY_P_CLOS);
    L_P_CLOS((*pf),primfunc) = R_P_CLOS((*desc),primfunc);

    p_arg = (STACKELEM*)R_P_CLOS((*desc),pta);
    arity = p_arg[0];

    if (newheap(arity + 1 , A_P_CLOS((*pf),pta)) ==NULL)
       post_mortem("ear:heap out of space!");

    RES_HEAP;
    p_arg_1= (STACKELEM *)R_P_CLOS((*pf),pta);

    /* Kopieren der Argumentliste: */
 
    for (i = R_P_CLOS((*desc),ndargs) + 1;i <= arity ; i++)
    {
      y = p_arg[i];
      p_arg_1[i] = y;
      if (T_POINTER(y))
         INC_REFCNT((PTR_DESCRIPTOR)y);
    }

    p_arg_1[0] = arity;
    p_arg_1[1]=p_arg[1];

    REL_HEAP;
  
    DEC_REFCNT(desc);

   /* Ab hier weiter mit dem neuen Deskriptor */
    desc = pf;

 } /* ref_count > 1 */
 
                   
 /* Zusaetzliche Argumente hinter pta eintragen */
  L_P_CLOS((*desc),ndargs) = _argsneeded;     

  RES_HEAP; 

  p_arg = (STACKELEM *)R_P_CLOS((*desc),pta);

  for (i=args ; i > _argsneeded ; i--)
   /* args = Anzahl der bisher noch benoetigten Argumente */
      p_arg[i] = POPSTACK(S_a);

  REL_HEAP;

  PUSHSTACK(S_a,(STACKELEM)desc);
   /* den Closure-Deskriptor von E nach A */
  
  END_MODUL("red_pclos");
  return(FALSE);

} /* _argsneeded > 0 */


else   /* Reduktion moeglich */
{
   int result;


   p_arg = (STACKELEM*)R_P_CLOS((*desc),pta);
   arity = (int)p_arg[0];

   ndargs = (int)R_P_CLOS((*desc),ndargs);

   anz_vorh = arity - ndargs;
     /* = Anzahl der in der Closure eingetragen Argumente */
                                    
   /* Die in der Closure eingetragenen Argumente in arg eintragen: */

   RES_HEAP;

   for ( i = 0 ; i < anz_vorh ; i++ )
   {
       arg[i] = p_arg[arity - i];
 
       if (T_POINTER(arg[i]))
           INC_REFCNT((PTR_DESCRIPTOR)arg[i]);
         /* da in den Routinen zur Berechnung der primitiven Funktionen */
         /* die Referenzzaehler der Argumente dekrementiert werden,     */
         /* muessen sie hier inkrementiert werden.                      */
   }

   REL_HEAP;


 /* Die auf dem A-Stack liegenden Argumente in arg eintragen: */

   for ( i = anz_vorh  ; i < arity ; i++)
       arg[i] = POPSTACK(S_a);

   index = R_P_CLOS((*desc),primfunc);
      /* Index der entsprechenden Routine in red_func_tab1 */

   prim_func = red_func_tab1[index];
     /* 'Aufruf' der entsprechenden C-Routine */

   _desc = NULL;  
      /* fuer die Interruptbehandlung */

   switch(arity)       /* Argumente aufsammeln */
   {
       case 2:  if ((result = (*prim_func)(arg[0],arg[1])) == 1)
                   break;
                if (result == 2)
                   /* lselect wurde aufgerufen */
                {
/* changed WRITESTACK -> PUSHSTACK              car (bzw. cr) */
                   PUSHSTACK(S_e,(STACKELEM)_desc);
                   if (!_beta_count_only) _redcnt--;
                   {
                       END_MODUL("red_pclos");
                       return(TRUE);
                   }
                }
       case 3: if ((result = (*prim_func)(arg[0],arg[1],arg[2])) == 1)
                   break;
       case 1: if ((result = (*prim_func)(arg[0])) == 1)
                    break;
       default: if ((result = (*prim_func)(arg[0],arg[1],arg[2],arg[3])) == 1)
                    break;
   }

   if (result == 0) 
      /* die primitive Funktion konnte nicht ausgewertet werden */
   {
/* inserted FUNC_INDEX(...)                    car (bzw. cr) */
      index = FUNC_INDEX(p_arg[1]);
      /* = der Index der primitiven Funktion in red_func_tab. */

      pfail(arity,arg,index,anz_vorh);
      END_MODUL("red_pclos");
      return(FALSE);

   } 
  
   DEC_REFCNT(desc);  
    /* da der Closure-Deskriptor an dieser Stelle nicht mehr */
    /* benoetigt wird.                                       */


    /* die Delta-Redukion hat geklappt: */
   PUSHSTACK(S_a,(STACKELEM)_desc);

   if (!_beta_count_only) _redcnt--;

#if DEBUG
   if (READSTACK(S_a) == NULL)
       post_mortem("ear: primitive function returns a null");
#endif

   END_MODUL("red_pclos");

   /* weiter auf der M-Linie des ear */
   return(FALSE);

} /* Reduktion moeglich */
}


