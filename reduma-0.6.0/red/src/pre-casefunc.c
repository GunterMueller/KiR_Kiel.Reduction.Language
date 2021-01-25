/* $Log: pre-casefunc.c,v $
 * Revision 1.13  1995/02/28  09:22:35  rs
 * removed some warnings...
 *
 * Revision 1.12  1995/02/08  15:34:19  rs
 * bugfix in single when
 *
 * Revision 1.11  1995/01/17  15:35:35  rs
 * another UH PM bugfix
 *
 * Revision 1.10  1995/01/13  16:40:56  rs
 * pre-casefunc.c
 *
 * Revision 1.9  1995/01/10  16:27:51  rs
 * UH PM bugfixes
 *
 * Revision 1.8  1995/01/05  16:22:11  rs
 * old switch-ptr integrated in UH pm structures
 *
 * Revision 1.7  1995/01/03  11:13:26  rs
 * more UH PM stuff
 *
 * Revision 1.6  1994/12/21  15:01:15  rs
 * more UH pm stuff
 *
 * Revision 1.5  1994/06/15  11:20:48  mah
 * both closing strategies in one binary: CloseGoal
 *
 * Revision 1.4  1994/05/03  07:31:34  mah
 * bug fix
 *
 * Revision 1.3  1994/03/29  17:12:57  mah
 * *** empty log message ***
 *
 * Revision 1.2  1994/03/13  15:51:10  mah
 * bug fix
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
/**********************************************************************/
/*                                                                    */
/*   casefunc.c : external :        casefunc;                         */
/*                           guard_body_func;                         */
/*        Aufgerufen durch :       ea-create;                         */
/*                                   subfunc;                         */
/*                                  lrecfunc;                         */
/*                                  casefunc;                         */
/*                                  condfunc;                         */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "list.h"
#include "case.h"


/**********************************************************************/
/* Spezielle externe Routinen:                                        */
/**********************************************************************/

extern void                 condfunc();             /* condfunc.c     */ 
extern void                  subfunc();             /* nsubfunc.c     */
extern void                 lrecfunc();             /* lrecfunc.c     */    

extern int                    mvheap();             /* rhinout.c      */  

extern T_HEAPELEM           *newbuff();             /* rheap.c        */      
extern PTR_DESCRIPTOR        newdesc();        
extern                       st_expr();             /* rstpro.c       */

extern void                  pm_ptfu();             /* pm-ptfu.c      */

extern void              gen_BV_list();             /* hfunc.c        */
extern int                    anz_FV();                
extern void                move_fv_e();     
extern void                 test_var();       
extern void                    hallo();      
extern BOOLEAN  check_list_for_store();
extern void                 list_end();    
extern void               fvlist_end();      
extern void          free_funct_name();     
extern void           free_bound_var();    
extern void              free_fv_end();           
extern void         gen_LREC_FV_list();
extern void            find_free_var();
extern void                 free_arg();
extern void                 test_var();             
extern int                 test_cond();
extern void                 DescDump();
extern void            gen_CRBV_list();
extern void             gen_arg_list();
extern void                 res_heap();
extern void                 rel_heap();
extern int                   newheap();
extern void                ppopstack();
extern void             ptrav_a_hilf();
extern void          test_dec_refcnt();
extern STACKELEM             st_cond();
extern STACKELEM             st_list();
extern void          test_inc_refcnt();
extern void                 trav_e_a();
extern void                ptrav_a_e();
extern void              stack_error();
extern void                snap_args();
extern void              trav_a_hilf();

extern void          guard_body_func();        /* forward declaration */
extern int                   combine();        /* forward declaration */

#if UH_ZF_PM
extern void   convert_pattern();
extern void load_expr();
extern midstack();
#endif /* UH_ZF_PATTERN */


/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

 extern list                      *VAR;
 extern fvlist                     *FV;
 extern arglist                   *ARG;
 extern free_var_list         *FV_LREC;

#if UH_ZF_PM
extern StackDesc S_pm;
extern StackDesc S_tilde;

#define MKDESC(p,r,c,t) if ((p = newdesc()) != 0) {  \
                              DESC_MASK(p,r,c,t);  \
                            }  \
                            else post_mortem("preproc: heap overflow")
#endif
                                                          


/*********************************************************************/
/*                                                                   */
/*      casefunc preprocessed einen auf dem E-Stack liegenden        */
/*                                                                   */
/*      Ausdruck der Form :                                          */
/*                                                                   */
/*      SWITCH(n) WHEN(m) pattern_1  guard_1  body_1                 */
/*                           ...                                     */
/*                WHEN(m) pattern_n  guard_n  body_n                 */
/*      [OTHERWISE  expr]                                            */
/*      END                                                          */
/*                                                                   */
/*                                                                   */
/*      oder der Form                                                */
/*                                                                   */
/*      WHEN(m) pattern guard body                                   */
/*                                                                   */
/*                                                                   */
/*********************************************************************/


void casefunc(close,when, index )
BOOLEAN close; /* close = TRUE : die case-Funktion muss geschlossen werden */
               /* close = FALSE : die case-Funktion wird nicht geschlossen,*/
               /* d.h.die relativ freien Variablen werden an die umgebende */
               /* Funktion 'hochgereicht'.                                 */
BOOLEAN when;  /* when = TRUE : auf dem E-Stack liegt eine pi-Abstraktion */
               /* when = FALSE : auf dem E-Stack liegt eine case-Funktion */
                                
int        index;      /* index = 0 => case ist aeussere Funktion und */
                       /* enthaelt somit keine relativ freien Var.    */     
                       /* index = 2 : Aufruf aus lrecfunc             */
                       /* index = 1 : Aufruf aus g-b-func,subfunc,etc */

{
  STACKELEM              *when_list,x,y;
  STACKELEM          case_name=0, *name;
  PTR_HEAPELEM              offset,hilf;
  PTR_DESCRIPTOR           desc=NULL,pf;
  PTR_DESCRIPTOR          sub_desc=NULL;                     
  int       anzahl,arity,i,j,anz_when=0;
  int                          anz_bv=0;  
  int                         test = 0 ;
  int                       otherwise=0;
  BOOLEAN                   end = FALSE;    
                                        
#if UH_ZF_PM
         PTR_DESCRIPTOR pmpx,guardpx, bodypx;
         PTR_DESCRIPTOR clausepx=0, clausepxbak=0, patpx;
         PTR_DESCRIPTOR otherwisepx, othertmp;
         PTR_HEAPELEM patvar;
#endif

 START_MODUL("casefunc");

#if UH_ZF_PM
    if ((pmpx = newdesc()) == NULL)
       post_mortem("casefunc:Heap out of space ");
    DESC_MASK(pmpx,1,C_MATCHING,TY_SELECTION);
    L_SELECTION(*pmpx,otherwise)=NULL;
    L_SELECTION(*pmpx,clauses)=NULL;
#endif

if (!when)       
 /* also wird eine case-Funktion preprocessed */
   {
    otherwise = 0;

    x=READSTACK(S_e);
/* US */
    if ( T_SUB(x) )
    { 
       otherwise = combine();   
       sub_desc = (PTR_DESCRIPTOR)POPSTACK(S_e);
       test = 1 ;
    }

    if ( T_PM_REC(x) )
    {                                         
     /* Die case-Funktion ist rekursiv spezifiziert;   */ 

     /* index = 1 => bestimme die in der case-Funktion auftretenden rel. */
     /* freien Variablen und trage sie in FV ein                         */
     /* Kommentar siehe auch lrecfunc                                    */ 

     /* index = 2 : Aufruf aus lrecfunc                                  */
     /* FV_LREC wird gleich der Menge der relativ freien Var. der        */
     /* umgebenden  lrec-Struktur gesetzt                                */

        FV_LREC = NULL;
                                    
        switch ( index )
        {
          case 0 : break;

          case 1 : find_free_var(2);
                   if ( FV_LREC != NULL )
                        gen_LREC_FV_list( FV_LREC );  
                   break;

          case 2 : FV_LREC = ARG->fv_lrec;
        }
    }

    y = POPSTACK(S_e);   /* case-Konstruktor */
    PUSHSTACK(S_m,y);
    PUSHSTACK(S_m1,y);

   /* Switch-Deskriptor (unvollstaendig) anlegen: */

    if ((desc = newdesc()) == NULL)
       post_mortem("casefunc:Heap out of space ");

    DESC_MASK(desc,1,C_EXPRESSION,TY_SWITCH);

    /* anz_when := Anzahl der pi-Abstraktionen: */
    anz_when = READ_ARITY(S_m1);   

#if UH_ZF_PM
    L_SELECTION(*pmpx,nclauses) = anz_when-1;
#endif

    
    if ( T_PM_REC(y) )
        anz_when--;  /* da die Stelligkeit des case' auch   */
                     /* den Funktionsnamen umfasst.         */
    L_SWITCH((*desc),nwhen) = anz_when;  

    /* case-Konstruktor in den Deskriptor eintragen: */
    L_SWITCH ((*desc), casetype) = y;


    if (T_PM_REC(y))
     {
        case_name = POPSTACK(S_e);
       /* der Funktionsname */

        WRITE_ARITY(S_m,anz_when);
       /* Stelligkeit des CASE auf dem M-Stack dekrementieren */

       /*  gebundene Variable in die VAR - Liste eintragen:  */ 
                                     
            list_end(close); 
            gen_CRBV_list(desc,case_name);
       
      /*  fuer jeden CASEREC - Descriptor wird in der ARG - Liste ein    */
      /*  Element der Form                                               */
      /*       ( CASEREC - Desc , FV_LREC )                              */
      /* eingehaengt                                                     */

            gen_arg_list(desc);
     } 
     else
          list_end( close );

    /* Descriptor auf den A-Stack */
    PUSHSTACK(S_a,desc);    
   } /* Ende !when */
else {
   y = READSTACK(S_e);
#if UH_ZF_PM
  L_SELECTION(*pmpx,nclauses) = 1;
#endif /* UH_ZF_PM */
}



main_e:
  
  x = POPSTACK(S_e);     /*WHEN(3) || OTHERWISE(1) || END */

      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m1,x);
  
  if ( T_PM_WHEN(x))
     {
      /* Match-Deskriptor anlegen : */

      if ((pf = newdesc()) == NULL)
         post_mortem("casefunc:Heap out of space ");

      DESC_MASK(pf,1,C_EXPRESSION,TY_MATCH);
  
      /* Descriptor auf den A-Stack */
      PUSHSTACK(S_a,pf);        

      if (ARITY(x) > 3)                                    
        /* Es wurde ein n-stelliges case spezifiziert.                 */
        /* anzahl := Anzahl der Patternausdruecke der pi-Abstraktionen */
          anzahl = ARITY(x) - 2;                    
      else
          anzahl = 1; 

      if (!when)
          L_SWITCH((*desc),anz_args) = anzahl;
         /* = Anzahl der fuer einen Reduktion benoetigten Argumente */

      if (anzahl > 1)
        /* also handelt es sich um ein n-stelliges case bzw. when mit n=anzahl */
        /* Die Patternausdruecke werden zu einer Liste zusammengefasst:        */
         {
           WRITE_ARITY(S_m,3);	  /* Korrigieren der WHEN's */
           WRITE_ARITY(S_m1,3);
           PUSHSTACK(S_e,SET_ARITY(LIST,anzahl));
         }	
  
      pm_ptfu(pf,anzahl); /* Preprocessen des Patterns,dabei werden die im  */
                          /* Pattern auftretenden Variablen in den code-    */
                          /* Vektor eingetragen.                            */

     /* Pattern fertig bearbeitet => WHEN auf M-Stack dekrementieren:*/

        WRITE_ARITY(S_m,2);

       hilf = R_MATCH((*pf),code);   

       /* hilf verweist auf den code-Vektor des Match-Deskriptors . */
       /* Der code-Vektor hat folgenden Aufbau :                    */
       /*  hilf[VEKTORLAENGE] : Gesamtlaenge des Vektors;           */
       /*  hilf[STELLIGKEIT]  : Stelligkeit der Funktion;           */
       /*  hilf[NAMELIST]     : Offset fuer die Liste der           */
       /*                       Pattern-Variablen;                  */
       /*  hilf[OPAT]         : Verweis auf das Originalpattern;    */
       /*  dann               : code fuer das Pattern;              */
       /*  dann               : Anzahl der Pattern-Variablen;       */
       /*  dann               : Liste der Pattern-Variablen         */
          
       RES_HEAP;

       offset = (hilf + (int)hilf[NAMELIST]);
        /* offset verweist auf die Liste der gebundenen Variablen. */

       REL_HEAP;
       anz_bv = (int)offset[0] ;  /* = Anzahl der gebundenen Variablen */

       if ( ! when )
           list_end(FALSE);
       else 
           list_end(close);

       if ( anz_bv != 0 )
        /* Falls gebundene,d.h. im Pattern auftretende Variablen,vor- */
        /* handen sind ,werden diese in das Feld name eingetragen.    */
          {
           if ((name = newbuff(anz_bv+ 2)) == 0)
              post_mortem("casefunc:Heap out of space");

           RES_HEAP;

           for (j = 1; j <= anz_bv ; j++)
#if 1
	     /* adjustment to lred-compiler: */
	     /* reversing debruijn indices   */
	     /*                   mah 080294 */
#if UH_ZF_PM
             name[j+1] = MIDSTACK(S_tilde,anz_bv-j);
#else
	     name[j+1] = (STACKELEM)offset[anz_bv-j+1];
#endif
#else
	     name[j + 1] = (STACKELEM)offset[j];
#endif /* WITHTILDE */
      
#if UH_ZF_PM
          for (j = 1; j <= anz_bv ; j++)
            PPOPSTACK(S_tilde);
#endif /* UH_ZF_PM */

           REL_HEAP;
   
           /* Die gebundenen Variablen werden in VAR eingetragen: */
            
            gen_BV_list(anz_bv,name);                         

          }

        /* Bearbeitung des guard und des body :                     */
        /* anz_bv != 0 => in dem Pattern treten gebundene Variable  */
        /* auf => ist der guard oder der body der when-Klausel ein  */
        /* case,so wird dieses geschlossen.                         */
        /* Bei anz_bv = 0 wird ein auftretendes case nicht geschlos-*/
        /* sen,da die in diesem inneren case auftretenden relativ   */
        /* freien Variablen in dem aeusseren case ebenfalls relativ */
        /* frei sind.                                               */
        guard_body_func((anz_bv != 0));

        /* Da die betrachtete when-Klausel fertig preprocessed ist, */
        /* werden die in ihr gebundenen Variablen aus BV entfernt:  */

       free_bound_var();

#if UH_ZF_PM
       /* WARNING WARNING, hier wird teilweise nichts kopiert, sondern es 
          werden nur Verweise umgebogen !!! */

       START_MODUL("UH_PM_CONVERSION");
       MKDESC(clausepx,1,C_MATCHING,TY_CLAUSE);
       GET_HEAP(4,A_CLAUSE(*clausepx,sons));  /* Platz fuer das Pattern,Guard,Body Triple + ggf. OLD MATCH DESCRIPTOR */
       L_CLAUSE(*clausepx,sons)[3] = NULL;
       MKDESC(guardpx,1,C_EXPRESSION,TY_EXPR); 
       if (L_MATCH((*(PTR_DESCRIPTOR)READSTACK(S_a)),guard) == NULL) {     /* guard TRUE */
         GET_HEAP(2,A_EXPR(*guardpx,pte));
         *(PTR_HEAPELEM)R_EXPR(*guardpx,pte) = 1; 
         *((PTR_HEAPELEM)(R_EXPR(*guardpx,pte)+1)) = SA_TRUE;
         }
       else
         L_EXPR(*guardpx,pte) = R_MATCH((*(PTR_DESCRIPTOR)READSTACK(S_a)),guard);
       L_CLAUSE(*clausepx,sons)[1] = (int) guardpx;
       MKDESC(bodypx,1,C_EXPRESSION,TY_EXPR);
       L_EXPR(*bodypx,pte) = R_MATCH((*(PTR_DESCRIPTOR)READSTACK(S_a)),body);
       L_CLAUSE(*clausepx,sons)[2] = (int) bodypx;
       MKDESC(patpx,1,C_EXPRESSION,TY_SUB);
       L_CLAUSE(*clausepx,sons)[0] = (int)patpx;
       GET_HEAP(anz_bv+1, A_FUNC(*patpx,namelist));
       patvar = R_FUNC(*patpx,namelist);
       *patvar++=anz_bv;
       for (j=0; j<anz_bv; j++)
         *patvar++=offset[j+1];
       L_FUNC(*patpx,pte) = R_EXPR(*(T_PTD)hilf[OPAT], pte);
       L_FUNC(*patpx,nargs) = anz_bv;
       PUSHSTACK (S_a, clausepx);
       load_expr(A_FUNC(*patpx,pte));
       convert_pattern();
       TRAV_A_HILF; 
       if (mvheap(A_FUNC(*patpx,pte)) == 0)
         post_mortem ("UH_PM_CONVERSION out of heap !");
       END_MODUL("UH_PM_CONVERSION");
#endif /* UH_ZF_PM */

       goto main_m;
  
     } /* T_PM_WHEN */
  
 if (!when)
    {
     if (T_PM_END(x))                                    
     { 
       PPOPSTACK(S_m);
       PPOPSTACK(S_m1);
       end = TRUE;
       goto main_m;
     }

     if (T_PM_OTHERWISE(x))
        {
         /* Match-Deskriptor fuer den otherwise-Ausdruck anlegen: */ 
         if ((pf = newdesc()) == NULL)
            post_mortem("casefunc:Heap out of space ");

         DESC_MASK(pf,1,C_EXPRESSION,TY_MATCH);
                                                                        
         /* Da der otherwise-Ausdruck keinen Pattern- und keinen   */
         /* guard-Ausdruck 'enthaelt',werden guard und code in dem */
         /* Match-Deskriptor auf NULL gesetzt:                     */ 
         L_MATCH((*pf),guard) = (PTR_HEAPELEM)0;
         L_MATCH((*pf),code) = (PTR_HEAPELEM)0;

         /* Descriptor auf den A-Stack */
         PUSHSTACK(S_a,pf);

          list_end(FALSE);

         /* Ist der otherwise-Ausdruck ein case-Ausdruck,so wird dieses */
         /* case nicht geschlossen,daher wird guard_body_func mit dem   */
         /* Parameter FALSE aufgerufen:                                 */
          guard_body_func(FALSE);

          free_bound_var();

         goto main_m;
        }
    }
main_m:

 if (!when)
    /* also ein case-Ausdruck */
     x = READSTACK(S_m);
  

     arity = ARITY(x);

     if ((arity == 1) || when || end )  /* d.h.,SWITCH(1) auf dem M_Stack ,oder */
                                        /* es war kein case,sondern ein when    */
                                        /* spezifiziert worden oder es wurde    */
                                        /* kein otherwise-Ausdruck spezifiziert */
        { 
         if (!when)
            {
             PPOPSTACK(S_m1);  /* SWITCH(n) */
             PPOPSTACK(S_m);   /* SWITCH(1) */

                /* Die Match-Deskriptoren hinter dem Switch-Deskriptor eintragen.*/
                /* Die Match-Deskriptoren liegen 'ueber' dem Switch-Deskriptor   */
                /* auf dem A_Stack.                                              */

                 if (T_PM_REC(y))        
                  /* in diesem Fall wird der Funktionsname in das 'letzte' Feld  */
                  /* der ptse-Liste eingetragen:                                 */
                    i = 2;
                 else
                    i = 1;           

                 if (newheap(anz_when + i + 2,A_SWITCH((*desc),ptse)) == 0)
                     post_mortem("casefunc:Heap out of space ");
      
                 /* when_list verweist auf ptse des Switch-Deskriptors: */
    
                 when_list = R_SWITCH((*desc),ptse);

                /* Die Match-Deskriptoren in when_list eintragen: */

                 RES_HEAP;            

                 if (i == 2)
                    /* Also liegt ein caserec vor.Den Funktionsnamen eintragen:  */
                    when_list[anz_when + 1] = case_name;

#if UH_ZF_PM
            /*      PUSHSTACK(S_pm,pmpx);         DBUG ONLY :-) */
                 othertmp = NULL;
#endif

                 if (end)
                      when_list[anz_when] = NULL;
                 else
                  /* es gibt einen otherwise-Ausdruck   */
                  /* der fuer diesen Ausdruck angelegte */
                  /* Deskriptor liegt auf dem A-Stack   */ 
#if UH_ZF_PM
                    { 
                      othertmp = (PTR_DESCRIPTOR) R_MATCH((*(PTR_DESCRIPTOR)READSTACK(S_a)),body);
#endif
                      when_list[anz_when] = POPSTACK(S_a);
#if UH_ZF_PM
                    }
#endif


                 for (j = anz_when - 1; j > 0 ; j--)
#if UH_ZF_PM
                   {   
                      clausepx = (PTR_DESCRIPTOR) POPSTACK(S_a);
                      if (j == (anz_when - 1)) {
                        L_CLAUSE(*clausepx,next) = NULL;
                        clausepxbak = clausepx;
                        }
                      else {
                        L_CLAUSE(*clausepx,next) = clausepxbak;
                        clausepxbak = clausepx;
                        }
#endif
                      when_list[j] = POPSTACK(S_a);
#if UH_ZF_PM
                   }
                 L_SELECTION(*pmpx,clauses) = clausepx;
#endif


                 REL_HEAP;

#if UH_ZF_PM
                 if (othertmp != NULL) {
                      MKDESC(otherwisepx,1,C_EXPRESSION,TY_EXPR);
                      L_EXPR(*otherwisepx,pte)= (PTR_HEAPELEM) othertmp;
                      L_SELECTION(*pmpx,otherwise)=otherwisepx; 
                      L_SELECTION(*pmpx,nclauses)=R_SELECTION(*pmpx,nclauses) + 1;
                  }
#endif

                 when_list[0] = anz_when + 1 + i ;  
/* US */
                 /* werden direkt aufeinander folgende Abstraktionen */
                 /* zusammengefasst , so wird die urspruengliche     */
                 /* Strukur im vorletzten Element der when_list      */
                 /* nachgehalten ;                                   */

                 if ( test == 0 )
                    when_list[anz_when + i ] = (STACKELEM)NULL;
                 else
                    when_list[anz_when + i ] = (STACKELEM)sub_desc;

                 if ( otherwise == 0 )
                    when_list[anz_when + i + 1 ] = (STACKELEM)NULL;
                 else
                    when_list[anz_when + i + 1 ] = (STACKELEM)1;

            }
#if UH_ZF_PM
            else {
              L_SELECTION(*pmpx,clauses) = (PTR_DESCRIPTOR) POPSTACK(S_a);
              }
#endif /* UH_ZF_PM */

#if UH_ZF_PM
         clausepx = R_SELECTION(*pmpx,clauses);
         L_CLAUSE(*clausepx,sons)[3] = READSTACK(S_a);
         PPOPSTACK(S_a);
         PUSHSTACK(S_e, pmpx);
#else
         PUSHSTACK(S_e,POPSTACK(S_a));
              /* Switch- bzw. Match-Deskriptor auf den E-Stack */
#endif

         anzahl = anz_FV();
             /* anzahl = Anzahl der relativ freien Variablen */

#if 1
         if (close || when)
#else
	   if (1)
#endif /* 0 */
            /* d.h.,der case- bzw. when-Ausdruck wird geschlossen */
            {
             if (anzahl > 0) /* d.h. es gibt relativ freie Variable. */
             {
	       PTR_DESCRIPTOR snsub;

	       /*  Funktionsdeskriptor fuer sub~ anlegen */
	       
	       if ((snsub = newdesc()) == NULL)
		 post_mortem("subfunc: Heap out of space");
	       

	       DESC_MASK(snsub,1,C_EXPRESSION,TY_SNSUB);                               
	       L_FUNC((*snsub),special) = 0;
	       L_FUNC((*snsub),nargs)   = anzahl; 
	       L_FUNC((*snsub),namelist)= NULL;
	       
	       PUSHSTACK(S_hilf, POPSTACK(S_e));             /* das Sub */
	       if (mvheap(A_FUNC((* snsub),pte)) == 0)       /* Auslagern */
		 post_mortem("subfunc: Heap out of space");
	       
	       PUSHSTACK(S_e, (STACKELEM) snsub);
#if 0
	       PUSHSTACK(S_e,SET_ARITY(SNSUB,1)); /* mah 271093 */
#endif /* 0 */
	       move_fv_e();
	       /* Die relativ freien Variablen werden zur eventuell */
	       /* weiteren Bearbeitung in der aeusseren Funktion    */
	       /* (falls es  Nummern- oder  Tilde-Variable sind)    */
	       /* auf den E-Stack gelegt.                           */

	       PUSHSTACK(S_e,SET_ARITY(SNAP,(anzahl + 1)));
	     }
             else
               free_fv_end();
            } /* Ende if(close || when) */
          
         if ( T_PM_REC(y) )
             free_arg();
        
          if ( ! when ) free_bound_var();

         END_MODUL("casefunc");

         return;
        }

     else  /* naechste when-Klausel */
       {
        WRITE_ARITY(S_m,arity-1);	
        goto main_e;
       }
}

/* Ende casefunc  */



/**********************************************************************/
/*                                                                    */
/*       guard_body_func : preprocessed  den guard-,body- bzw. den    */
/*           otherwise-Ausdruck einer when-Klausel und benennt        */
/*           die auftretenden relativ freien Variablen in             */
/*           Tilde-Variablen und die gebundenen Variablen (d.h. die   */
/*           im Pattern auftretenden Variablen) in Nummernvariable um.*/
/*                                                                    */
/**********************************************************************/



void guard_body_func(close_case)
BOOLEAN close_case; /* Der Wert von close_case gibt an,ob ein        */
                    /* auftretender case-Ausdruck geschlossen wird   */
                    /* (TRUE) oder nicht (FALSE).                    */


{
  PTR_DESCRIPTOR desc;
  STACKELEM       y,x;
  int           arity;

  START_MODUL("guard_body_func");

main_e:

  x = READSTACK(S_e);

  if (T_SUB(x))
    {
     /* rel freie Var. sollen gesammelt werden,daher kein         */
     /* Aufruf von fvlist_end();                                  */
        if ( close_case )
            fvlist_end();    

     subfunc(close_case);
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
          PPOPSTACK(S_e);
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
     }
      goto main_e;
     }

if (T_PM_CASE(x))
   {

    if ( T_PM_REC(x))
    {
       fvlist_end();
       casefunc( TRUE , FALSE , 1 );
    }
    else
    {

      if (close_case)
       /* d.h. das innere case soll geschlossen werden,daher muss    */
       /* an die Liste der relativ freien Variablen ein Listenende-  */
       /* Zeichen angehaengt werden.                                 */
       fvlist_end();

       casefunc( close_case , FALSE , 1 );
       /* Aufruf von casefunc fuer das innere case */
     }

    goto main_e;
   }

if (T_PM_WHEN(x))
   {
    fvlist_end();
#if 0
    casefunc(FALSE,TRUE,1);
#else
    casefunc(close_case,TRUE,1); /* mah 160394 */
#endif
    goto main_e;
   }                    





if ( T_PROTECT(x) ||                                
    ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
   {
     test_var(x); /* handelt es sich um eine gebundene,eine relativ */
                  /* freie oder um eine freie Variable?             */

     /* Funktionsanwendungen auf oberster Ebene des */
     /* Startausdrucks muessen geschlossen werden   */

     if (!CloseGoal) {
       x = READSTACK(S_a);
       if (T_POINTER(x) && (R_DESC(*(PTR_DESCRIPTOR)x,type) == TY_LREC_IND))
       {
	 int i;

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
     PPOPSTACK(S_e);
     PUSHSTACK(S_m,x);
     PUSHSTACK(S_m1,x);
     goto main_e;
   }

 
if (T_CON(x))  
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
       
post_mortem("guard_body_func: No match successful on main_e");



main_m:

  x = READSTACK(S_m);


  arity = ARITY(x);

  if (T_PM_WHEN(x) || T_PM_OTHERWISE(x))
     {
      if (arity > 1)
          /* d.h.,auf dem M-Stack liegt WHEN(2),also muss     */
          /* der body der when-Klausel noch bearbeitet werden */
        {
          /*  Guard hinter den Match-Deskriptor legen: */
         
         if (T_SA_TRUE(READSTACK(S_a)))
            {
             PPOPSTACK(S_a);
             desc = (PTR_DESCRIPTOR) READSTACK(S_a); /* Match-Deskriptor */
             L_MATCH((*desc),guard) = (PTR_HEAPELEM)0;
             /* besteht der guard-Ausdruck nur aus einem TRUE, so wird */
             /* in den Deskriptor ein Null-Zeiger eingetragen          */ 
            }
         else
            {                 
#if HEAP
             H_PTRAV_A_HILF;
#else
             PTRAV_A_HILF;
             /* Ausdruck zum Auslagern auf den Hilf-Stack traversieren */
#endif
             desc = (PTR_DESCRIPTOR) READSTACK(S_a); /* Match-Deskriptor */

             if (mvheap(A_MATCH((*desc),guard)) == 0)                    
                /* Auslagern */
                {
                 DEC_REFCNT(desc);
                 post_mortem("guard_body_func:Heap out of space ");
                }

             WRITE_ARITY(S_m,arity - 1);
             goto main_e;
            } 
        }
      
      else  /* d.h.,auf dem M-Stack liegt WHEN(1) bzw. OTHERWISE(1),*/
            /* die when-Klausel bzw der otherwise-Ausdruck ist also */
            /* fertig bearbeitet                                    */
        {
         /* Body hinter den Match-Deskriptor legen: */

#if HEAP
             H_PTRAV_A_HILF;
#else
             PTRAV_A_HILF;     
             /* Zum Auslagern auf den Hilf-Stack traversieren */
#endif
         desc = (PTR_DESCRIPTOR) READSTACK(S_a); /* Match-Deskriptor */

         if (mvheap(A_MATCH((*desc),body)) == 0)                
            /* Auslagern */
            {
             DEC_REFCNT(desc);
             post_mortem("guard_body_func:Heap out of space ");
            }

         PPOPSTACK(S_m);    /* WHEN(1) bzw OTHERWISE(1) */
         PPOPSTACK(S_m1);   /* WHEN(3) bzw OTHERWISE(1) */


         END_MODUL("guard_body_func");
         return;
        }
    }


  if (arity > 1)  /* d.h. es war kein when-Konstruktor und die */
                  /* Stelligkeit war >= 2                      */
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
         /* Startausdruck fertig bearbeitet => Funktionsnamen aus BV */
         /* entfernen:                                               */

          free_funct_name();
          free_arg();
       }

    if (T_COND(x))   { y = st_cond(); PUSHSTACK(S_a,y); } 
                          /* Conditional auslagern */

    if ((T_LIST(x)) || (T_STRING(x)))
       if (check_list_for_store())  /* darf Liste ausgelagert werden? */
          {
           if ((x = st_list() ) == 0)   /* Wenn ja, auslagern */
              post_mortem("guard_body_func: heap out of space in st_list");
           else
              PUSHSTACK(S_a,x);
          }
    goto main_m;

  }

}

/*  Ende guard_body_func  */
     

 /* US */                                                              
/**********************************************************************/
/* combine fasst ein oder mehrere benutzerdefinierte Funktionen und   */
/* eine direkt folgende Case-Funktion zu einer mehrstelligen Case-    */
/* Funktion zusammen; die urspruengliche Struktur wird in einem       */
/* SUB - Descriptor nachgehalten                                      */  
/**********************************************************************/

int combine()
{  
   int              tiefe,anz_bv,anz_l;
   int              anz_when,arity,i,j;
   int                          nbound;
   STACKELEM                       x,y;
   STACKELEM                 *namelist;
   PTR_DESCRIPTOR                 desc;
   int                     otherwise=0;

          START_MODUL("combine");
                       

         /* mehrere geschachtelte sub's */
            
           tiefe = 0;
           anz_bv = 0; 
           anz_l = 0;   

         /* bestimme die Anzahl der sub's und die Anzahl der */
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

           if ((desc = newdesc()) == NULL)                                      
                post_mortem("combine: Heap out of space");


           DESC_MASK(desc,1,C_EXPRESSION,TY_SUB);                               

           L_FUNC((*desc),nargs)=anz_bv; 

           L_FUNC((*desc),pte) = (PTR_HEAPELEM)0;    /* Damit DescDump() nicht bei       */
                                                     /* diesem halbfertigen Deskriptor   */
                                                     /* abstuerzt                        */

           if (newheap(2 + anz_bv+anz_l, A_FUNC((*desc),namelist)) == 0)              
                  post_mortem("combine: Heap out of space");

         /* Anlegen einer Liste ,welche mit den Variablennamen */
         /* und den Stelligkeiten der sub's aufgefuellt wird   */

           namelist = (STACKELEM *) R_FUNC((*desc),namelist);   
           namelist[0] = (STACKELEM) anz_bv + 1 + anz_l;                
           namelist[1] = (STACKELEM) anz_bv + 2;
                                           
           i = anz_bv + 1;
           j = anz_bv + 2 ;

           for (  ; anz_l > 0 ; anz_l-- )
           {
             nbound = READ_ARITY(S_e);
             PPOPSTACK(S_e);              /* SUB(nbound) von S_e entfernen */
             
             namelist[j] = nbound ;          /* Stelligkeit merken */
             ++j;
             for( ; nbound > 1 ; nbound-- )
             {
                namelist[i] = POPSTACK(S_e);  /* Variablennamen eintragen */
                --i;
             }
           } 
          
         /* auf dem E - Stack liegt ein  Ausdruck der Form                    */
         /*                                                                   */
         /*      SWITCH(n) WHEN(m) pattern_1  guard_1  body_1                 */
         /*                           ...                                     */
         /*                WHEN(m) pattern_n  guard_n  body_n                 */
         /*      OTHERWISE  expr / END                                        */
            
         /* pattern_1  .. pattern_n werden um die in der namelist             */
         /* enthaltenen Variablen erweitert =>                                */
         /* es entsteht ein mehrstelliges case                                */

           y=POPSTACK(S_e);              /* SWITCH(n) */
           anz_when=ARITY(y);    
           arity = READ_ARITY(S_e);      /* Stelligkeit der pi - Abstraktionen */

           for ( j = anz_when ; j > 0 ; j-- )
           {
             x=POPSTACK(S_e);            /* WHEN(m) bzw OTHERWISE bzw END */

             if ( T_PM_WHEN(x) )
             {
               /* das pattern der when - Klausel wird erweitert     */
               /* die modifizierte Klausel wird auf den A - Stack   */
               /* traversiert                                       */

                  for ( i = anz_bv +1 ; i > 1 ; i-- )
                  {
                    PUSHSTACK(S_a,namelist[i]);
                    INC_REFCNT((PTR_DESCRIPTOR)namelist[i]);
                  }           
                 
                 for ( i = arity ; i > 0 ; i-- )
                      TRAV_E_A; 

                 PUSHSTACK(S_a,SET_ARITY(PM_WHEN,( arity + anz_bv ) ) );

             }

             if ( T_PM_END(x) )
                 PUSHSTACK(S_a,x);                   

              /* return - Wert initialisieren      */
                 otherwise = 0;


             if ( T_PM_OTHERWISE(x) )
             {
              /* OTHERWISE expr wird in                             */
              /* WHEN(anz_bv + arity) pattern_n+1  true expr        */
              /* umgewandelt ; pattern_n+1 enthaelt die in          */
              /* namelist auftretenden Variablen                    */
              /* zusaetzlich muss noch ein PM_END generiert         */
              /* werden => Stelligkeit des SWITCH incrementieren    */

                  for ( i = anz_bv +1 ; i > 1 ; i-- )
		  {
                    PUSHSTACK(S_a,namelist[i]);
                    INC_REFCNT((PTR_DESCRIPTOR)namelist[i]);
                  }                                     

                  for ( i = arity - 2  ; i > 0 ; i--)
                      PUSHSTACK(S_a,(STACKELEM)PM_SKIP);
                                                 
                  PUSHSTACK(S_a,(STACKELEM)SA_TRUE);  /* guard */

                  TRAV_E_A;               /* expr */

                 PUSHSTACK(S_a,SET_ARITY(PM_WHEN,( arity + anz_bv ) ) );

                 PUSHSTACK(S_a,(STACKELEM)PM_END);
                 otherwise=1;
             }                                  

           } /* Ende for( j = anz_when ; .. ) */
        
          /* SWITCH - Konstruktor auf S_a legen , evtl. Stellligkeit */
          /* veraendern                                              */
                               
          PUSHSTACK(S_a,SET_ARITY(PM_SWITCH,(anz_when + otherwise )));

         /* Case - Ausdruck wieder auf dem E - Stack aufbauen        */

           PTRAV_A_E;
         PUSHSTACK(S_e,(STACKELEM)desc);

         END_MODUL("combine");
         return(otherwise);
}
