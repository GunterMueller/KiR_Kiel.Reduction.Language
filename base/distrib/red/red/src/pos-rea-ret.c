/*
 * $Log: pos-rea-ret.c,v $
 * Revision 1.14  2001/07/02 15:32:29  base
 * compiler warning eliminated
 *
 * Revision 1.13  1996/07/01 13:11:00  rs
 * commented out some statements on postprocessing a case closure (see lines 673 ff)
 *
 * Revision 1.12  1995/11/15  15:04:10  cr
 * postprocessing for frames (ret_frame) and slots (ret_slot)
 *
 * Revision 1.11  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.10  1995/02/28  09:22:23  rs
 * removed some warnings...
 *
 * Revision 1.9  1995/02/22  15:50:22  rs
 * SELECTION-desctiptors used only in preprocessing
 *
 * Revision 1.8  1995/01/26  13:14:33  rs
 * bug in UH pm closures fixed
 *
 * Revision 1.7  1995/01/16  15:09:12  rs
 * UH PM bugfixes
 *
 * Revision 1.6  1995/01/13  16:40:41  rs
 * uh pm bugfixes
 *
 * Revision 1.5  1994/11/16  14:01:22  mah
 * bugfix for compatibility with reduce_to_nf
 *
 * Revision 1.4  1994/06/19  13:23:06  mah
 * reduce to normal form added
 *
 * Revision 1.3  1994/06/15  11:19:32  mah
 * both closing strategies in one binary: CloseGoal
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 */

/*************************************************************************/
/*                                                                       */
/* ea-ret.c                                                              */
/*  ea-retrieve:c  - external:  ea_retrieve;                             */
/*           Aufgerufen durch:  reduct;                                  */
/*  ea-retrieve:c  - internal:  ae_retrieve;                             */
/*                                                                       */
/*************************************************************************/

#include <malloc.h>
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include <setjmp.h>
#include "list.h"
#include "case.h"
#include "dbug.h"

/*************************************************************************/
/*                                                                       */
/* Externe Variablen:                                                    */
/*                                                                       */
/*************************************************************************/

extern BOOLEAN _interrupt;    /* reduct:c */
extern jmp_buf _interruptbuf; /* reduct:c */
extern postlist  *ILIST;
extern postlist_1 *TLIST;

extern BOOLEAN xxx;
extern BOOLEAN part_sub_var;
extern int anz_sub_bv;
extern STACKELEM *argumentlist;
extern int otherwise;


/*************************************************************************/
/*                                                                       */
/* Spezielle externe Routinen:                                           */
/*                                                                       */
/*************************************************************************/

extern void ret_name();            /* retrieve.c */
extern void ret_lname();          
extern void ret_fname();  
extern void ret_scal();     
extern void ret_cond();     
extern void ret_var();     
extern void ret_list();     
extern void ret_frame();     
extern void ret_slot();     
extern void ret_mvt();             /* rhinout.c  */
extern void load_expr();     
extern STACKELEM st_expr();        /* rstpro     */ 
extern void trav_e_a();            /* trav.c     */
extern void switch_post();         /* casepost.c */
extern void nomat_post();     
extern STACKELEM st_switch_post(); /* p-fail.c   */
extern void  ILIST_end();          /* postfunc.c */   
extern void TLIST_end();
extern void gen_ILIST_block();
extern void gen_ILIST_lrec();
extern void gen_ILIST_caserec();
extern void gen_TLIST_block();
extern void gen_TLIST_clos();
extern void inc_protects();
extern void dec_protects();
extern void free_TLIST_block();
extern void free_ILIST_block();
extern int  test_LIND();
extern int  test_CASE();
extern void p_load_expr();
extern void p_ret_expr();
extern void test_NUM();
extern void test_TILDE();
extern void test_inc_refcnt();
extern void test_dec_refcnt();
extern void DescDump();
extern void res_heap();
extern void rel_heap();
extern void ppopstack();
extern void convi();
extern void ret_dec();
extern void stack_error();
extern T_HEAPELEM            *newbuff();
extern STACKELEM st_switch_clos();
extern void ret_expr();
extern void ret_intact();       /* rhinout.c */

static void ae_retrieve();      /* forward declaration */




/******************************************************************************************/
/*                                                                                        */
/*  ea_retrieve  --  Postprocessed einen auf dem E-Stack liegenden Ausdruck,              */
/*                   und liefert den Ergebnisausdruck auf dem E-Stack ab.                 */
/*                   Dabei wird eine partielle Substitution durchgefuehrt,                */
/*                   d.h. es werden alle SNAP's aufgeloest. part_sub_var() bzw.               */
/*                   part_rec() legen den Inkarnationsblock an. Die eigentliche           */
/*                   Substitution der Nummernvariablen fuehrt dann ea_retrieve            */
/*                   durch. Anschliessend erfolgt die Umbenennung der ver-                */
/*                   bliebenen Nummernvariablen in ihre urspruenglichen Namen.            */
/*                   Diese Umbenennung wird erst gestartet, wenn ea_retrieve              */
/*                   die zugehoehrige Funktion auf den A-Stack traversiert hat.           */
/*                   Das Einlagern der Datenstrukturen erfolgt durch ae_retrieve.         */
/*  globals    --    S_e,S_a,S_m,S_m1   <w>                                               */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/


void ea_retrieve()
{
  register STACKELEM                             z, x, y;
  STACKELEM                 casetype, *namelist, * p_arg;
  STACKELEM                              arg_desc, *name;
  int                snapargs,narg,ndargs,nfv,index,test;
  VOLATILE int                             resolve=0,i=0;
  int                            anzahl,j,anz_bv,anz_ges;
  int                                 reason,n_args,ende;
  VOLATILE PTR_DESCRIPTOR                  sub_desc=NULL;
  PTR_DESCRIPTOR                         desc,pf,sw_desc;
  PTR_HEAPELEM                  *code,*offset,*func_list;
  postlist                                            *p;
  char                                            class1; 
         /* for 2nd parameter of function call ret_mvt */
  BOOLEAN                    when, ncase, combine, nwhen;
  VOLATILE STACKELEM                                pred; /* Praedikat des Conditionals */ 
 

  START_MODUL("ea_retrieve");               

  setjmp(_interruptbuf);           /* Interruptbehandlung neu initialisieren */
  if (_interrupt)                  /* Interrupt aufgetreten?                 */
  {
    _interrupt = FALSE;            /* zuruecksetzen */
    post_mortem("ea_retrieve: Interrupt received");   /* alles abbrechen        */
  }                                                   /* und zurueck zum Editor */

  PUSHSTACK(S_m,KLAA);             /* Endemarkierung @ */
                                                                              

  /* Initialisieren der Listen */

     ILIST=NULL;
     ILIST_end();
     TLIST=NULL;
     TLIST_end();                        

  /* Standard-Belegung globaler Variablen */

     otherwise = 0;
     anz_sub_bv = 0;
     part_sub_var = FALSE;
     xxx = FALSE;
             


main_e:

  x = POPSTACK(S_e);

    if (T_NUM(x))
    {
     test_NUM( ARITY(x) );
     goto main_e;
    }

  if (T_TILDE(x))
    {
     test_TILDE( ARITY(x) );
     goto main_e;
    }     

    if (T_SNSUB(x))
    {
      post_mortem("ea_retrieve: SNSUB not a descriptor"); /* mah 231193 darf nicht sein */
      goto main_e;
    }

    if ( ( T_LREC(x))  && ( ARITY(x)== 2) )
    {
      desc = (PTR_DESCRIPTOR) POPSTACK(S_e);  /* LREC-Descriptor */

        /* = Anzahl der Funktionsdefinitionen */

      anzahl = (int)R_LREC((*desc),nfuncs);

        /* Verweis auf Namensliste */
     
      name=(STACKELEM*)R_LREC((*desc),namelist);

        /* Verweis auf Funktion-ruempfe */

      func_list = (PTR_HEAPELEM *)R_LREC((*desc),ptdv);

        /* Funktion-namen plus LIST(anzahl) auf S_a legen */

      for ( i = 1 ; i <= anzahl ; i++ )
      {
         PUSHSTACK(S_a,name[i]);
         INC_REFCNT((PTR_DESCRIPTOR)name[i]);
      }

      PUSHSTACK(S_a,SET_ARITY(LIST,anzahl));


       /* Funktionsnamen und LREC-Deskriptor in ILIST eintragen: */

      gen_ILIST_lrec(name,desc);

       /* ersten Block von TLIST daraufhin ueberpruefen,ob  */
       /* Protects hochgezaehlt werden muessen:             */

      inc_protects();

       /* Markierung auf S_m legen  */

      y=READSTACK(S_m);

       /* liegt auf S_m ein SNAP(0) , so lege den DOLLAR darunter */
       /* => die relativ freien Var des LREC's muessen aus TLIST  */
       /*    entfernt werden , wenn die Funktionsruempfe wieder   */
       /*    aufgebaut sind   , die Funktionsnamen werden noch    */
       /*    fuer den Startausdruck benoetigt                     */

      if ( T_SNAP(y) )
      {
        PPOPSTACK(S_m);
        PUSHSTACK(S_m,DOLLAR);
        PUSHSTACK(S_m,y);      
      }
      else
        PUSHSTACK(S_m,DOLLAR);

      /* Funktionsdefinitionen auf S_e legen   : */

      for ( i = anzahl ; i > 0 ; i-- )
      {
         PUSHSTACK(S_e,(STACKELEM)func_list[i]);
         INC_REFCNT((PTR_DESCRIPTOR)func_list[i]);
      }

      /* LREC(2) auf S_m und LREC(3) auf S_m1 legen */

      PUSHSTACK (S_m, (STACKELEM)(LREC /* | SET_EDIT( (int)R_LREC( (*desc), special))*/));
      WRITE_ARITY (S_m, 2);

      PUSHSTACK (S_m1, (STACKELEM)(LREC /* | SET_EDIT( (int)R_LREC( (*desc), special))*/));
      WRITE_ARITY (S_m1, 3);

     /* LIST(anzahl) auf S_m und S_m1 legen */

      PUSHSTACK(S_m,SET_ARITY(LIST,anzahl));
      PUSHSTACK(S_m1,SET_ARITY(LIST,anzahl));

      goto main_e; 

    } /* LREC(2) */


    if ( T_PM_WHEN(x) )
    {
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m1,x);
      inc_protects();
      goto main_e;
    }

    if ( T_PM_OTHERWISE(x) )
    {
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m1,x);
      inc_protects();
      goto main_e;
    }                              

    if ( T_PM_NOMAT(x) )
    {
    /* auf dem E-Stack liegt ein Nomatch-Deskriptor; dieser wird  */
    /* in nomat_post aufgeloest                                   */

      x = POPSTACK(S_e);
      nomat_post((PTR_DESCRIPTOR)x);
      goto main_e;
    }                            


    if ( T_PM_GUARD(x) )
    {
     /* Kommentar siehe TY_CLOS , Closure fuer ein SWITCH */

      x = POPSTACK(S_e);         /* Closure-Deskriptor */
      desc = (PTR_DESCRIPTOR)x;

      pf = (PTR_DESCRIPTOR)R_CASE(
	   *((PTR_DESCRIPTOR)(R_CLOS((*desc),pta)[0])),ptd);/* SWITCH descriptor */

#if UH_ZF_PM
      if (R_DESC(*pf,type) == TY_SELECTION)
        pf = (PTR_DESCRIPTOR) R_CLAUSE(*(T_PTD)R_SELECTION(*pf,clauses),sons)[3];
#endif /* UH_ZF_PM */

      nfv = R_CLOS((*desc),nfv);
         /* nfv = Anzahl der relativ freien Variablen.   */

      p_arg = R_CLOS((*desc),pta);
           /* p_arg verweist auf die Liste der Argumente   */
           /* und der relativ freien Variablen .           */

      anz_ges = R_CLOS((*desc),args) + R_CLOS((*desc),nfv);

             /* anzahl der moeglichen Eintraege              */              

      casetype=(STACKELEM)R_SWITCH((*pf),casetype);      

     /* index wird als offset benoetigt, um das vorletzte  */
     /* Element der ptse-Liste des Switch-Deskriptors zu   */
     /* bestimmen                                          */

      index=(int)R_SWITCH((*pf),nwhen) + 1;
      if( T_PM_REC(casetype) )  index++;

      sub_desc = (PTR_DESCRIPTOR )( R_SWITCH((*pf),ptse) )[index]; 

      anz_sub_bv = R_FUNC((*sub_desc),nargs);
      /* namelist = (STACKELEM *)R_FUNC((*sub_desc),namelist);   not used */
      otherwise = (int)( R_SWITCH((*pf),ptse) )[index + 1];


     /* argumentliste anlegen und mit den Argumenten aus der */
     /* Closure fuellen                                      */ 

      if ( ( argumentlist = (STACKELEM *)newbuff( anz_sub_bv + 1 ) ) == (STACKELEM)NULL )
         post_mortem("ea_retrieve: heap out of space ");
 
      argumentlist[0] = anz_sub_bv;          
                
      j = anz_sub_bv;   /* Laufvariable fuer argumentlist */

      ende = anz_sub_bv;

      for ( i = ende ; i >= 1 ; i--)
      {
         argumentlist[j] = p_arg[ anz_sub_bv - i + 1];
         j--;
      }

     /* setzen einer Marke fuer nomat_post und switch_post */

      part_sub_var = TRUE;

      INC_REFCNT(pf);
      switch_post(pf);

     /* zuruecksetzen der globalen Variablen */

      part_sub_var = FALSE;
      anz_sub_bv = 0;
      otherwise = 0;
                                
      DEC_REFCNT(desc);     /* Closure-Deskriptor */
      goto main_e;
    } /* PM_GUARD */



    if (T_CON(x))
    {
      if ( T_SNAP(x) || T_POST_SNAP(x))
      {
        snapargs=ARITY(x)-1;
        /* snap(m+1) arg_m-1    arg_0 p-sub bzw p-switch auf S_e */
        /*  Argumante fuer rel. freie Variable werden in die     */
        /*  T-LIST eingetragen                                   */

        gen_TLIST_block(snapargs);

        /* snap(0) auf S_m legen */

        if (T_AP_1(READSTACK(S_m)))
        {
          PPOPSTACK(S_m);
          PUSHSTACK(S_m,SET_ARITY(SNAP,0));
          PUSHSTACK(S_m,SET_ARITY(AP,1));
        }
        else
          PUSHSTACK(S_m,SET_ARITY(SNAP,0));

        goto main_e;

      } /* T_SNAP */

      else  /* sonstiger Konstruktor */
      {
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

      } /* sonstiger Konstruktor */

    } /* T_CON */
                 


    if (T_POINTER(x))
    { 

      switch(R_DESC((* ((PTR_DESCRIPTOR)x)),type))
      {
/* mah :TY_CONS und TY_COMB eigefuegt, fuer Code-Deskriptoren */
      case TY_CONS:
                    desc = R_CONS(*(T_PTD)x,tl);
                    if (T_POINTER(desc)) INC_REFCNT(desc);
                    PUSHSTACK(S_e,desc);
                    desc = R_CONS(*(T_PTD)x,hd);
                    if (T_POINTER(desc)) INC_REFCNT(desc);
                    PUSHSTACK(S_e,desc);
                    DEC_REFCNT((T_PTD)x);
                    PUSHSTACK(S_m,SET_ARITY(CONS,2));
                    PUSHSTACK(S_m1,SET_ARITY(CONS,2));
                    goto main_e;
      case TY_COMB:
                    desc = R_COMB((*(T_PTD)x),ptd);
                    /* desc zeigt jetzt entweder auf einen */
                    /* LREC_IND oder einen SUB descriptor  */
                    /* ohne relativ freien variablen !     */
                    INC_REFCNT(desc);
                    PUSHSTACK(S_e,desc);
                    DEC_REFCNT((T_PTD)x);
                    goto main_e;
      case TY_CONDI:
		    desc = R_CONDI((*(T_PTD)x),ptd);
		    /* desc zeigt jetzt auf das Conditional */
		    INC_REFCNT(desc);
		    PUSHSTACK(S_e,desc);
		    DEC_REFCNT((T_PTD)x);
		    goto main_e;
#if UH_ZF_PM
       case TY_SELECTION:
                    DBUG_PRINT("ea_ret",("SELECTION 1"));
                    desc = (PTR_DESCRIPTOR) R_CLAUSE(*(T_PTD)R_SELECTION(*(T_PTD)x,clauses),sons)[3];
                    INC_REFCNT(desc);
                    PUSHSTACK(S_e,desc);   /* original case descriptor */
                    goto main_e;
#endif /* UH_ZF_PM */
       case TY_CASE:
                    desc = R_CASE((*(T_PTD)x),ptd);
                    /* desc zeigt jetzt auf einen SWITCH descriptor */
                    /* ohne relativ freien variablen !     */
                    DEC_REFCNT((T_PTD)x);
#if UH_ZF_PM
          /*          DBUG_PRINT("ea_ret",("SELECTION 2")); */
                /*    desc = R_CLAUSE(*(T_PTD)R_SELECTION(*desc,clauses),sons)[3]; */ /* original CASE descriptor */ 
#endif /* UH_PM_ZF */
                    INC_REFCNT(desc);
                    PUSHSTACK(S_e,desc);
                    goto main_e;

        case TY_VAR: ret_var(x);                 /* ganz freie Variable    */
                     goto main_e;                /* durch Namenspointer    */
                                                 /* und PROTECT's ersetzen */

      case TY_SNSUB:  desc = (PTR_DESCRIPTOR)x;  /* mah 231193 */

		      p_load_expr((STACKELEM **)A_FUNC((*desc),pte));

		      DEC_REFCNT(desc);

		      goto main_e;

        case TY_SUB:  desc = (PTR_DESCRIPTOR)x;
    
                      /* narg = Anzahl der gebundenen Variablen: */

                      narg = (int)R_FUNC((*desc),nargs);

                      name = (STACKELEM *)R_FUNC((*desc),namelist);
                       
                      /* gebundenen Var. in ILIST einhaengen */

                      gen_ILIST_block(narg,name);
              
        
                      /* ersten Block von TLIST daraufhin ueber- */
                      /* pruefen , ob protects hochgezaehlt      */
                      /* werden muessen                          */

                      inc_protects();

                      /* Markierung auf S_m legen  */

                      PUSHSTACK(S_m,DOLLAR);

                      /* Funktionsrumpf einlagern: */

                      p_load_expr((STACKELEM **)A_FUNC((*desc),pte));
    

                      if ( name[1] != 0 )
                      { 
                        /* waehrend der Preprocessing Phase wurden mehrere     */
                        /* direkt aufeinander folgende sub's zusammengefasst   */ 
                        /* die urspruengliche Struktur ,welche in der namelist */
                        /* des SUB - Deskriptors nachgehalten wurde , wird     */
                        /* rekonstruiert                                       */

                        index = (int)name[0];
                        j = name[1];

                        for( ; j <= index ; j++ )
                        {
                          anzahl = name[j] ;    /* Stelligkeit des sub's */
                                                                 
                          /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */

                           PUSHSTACK(S_m,SET_ARITY(SUB,1));
                           PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
                        }

                      }
                      else /* namw[1] = 0 */
                      {   
                        /* SUB(1) auf S_m , SUB(narg+1) auf S_m1 */

                        PUSHSTACK(S_m,SET_ARITY(SUB,1));
                        PUSHSTACK(S_m1,SET_ARITY(SUB,narg + 1 ));
                      }    
                       
                     /* Gebundene Variablen auf S_a legen  */           

                      for ( i = narg ; i > 0 ; i--)
                      {
                        PUSHSTACK(S_a, name[ i + 1 ]);
                        INC_REFCNT((PTR_DESCRIPTOR)name[ i + 1 ]);
                      }

/* problems with red to nf */
/*                      DEC_REFCNT(desc); */
                      goto main_e;


        case TY_CLOS: desc = (PTR_DESCRIPTOR)x;

                      ndargs = R_CLOS((*desc),nargs);
                      /* ndargs = Anzahl der fehlenden Argumente.     */

                      nfv = R_CLOS((*desc),nfv);
                      /* nfv = Anzahl der relativ freien Variablen.   */

                      p_arg = R_CLOS((*desc),pta);
                      /* p_arg verweist auf die Liste der Argumente   */
                      /* und der relativ freien Variablen .           */

                      anz_ges = R_CLOS((*desc),args) + R_CLOS((*desc),nfv);
                      /* anz_ges = Anzahl der gebundenen + Anzahl der */
                      /* relativ freien Variablen .                   */

		      switch(R_CLOS((*desc),ftype))
		      {
		      case TY_NOMATBODY:
			/* this closure type is only used if a guard cannot */
			/* be decided. a closure has been build to replace  */
			/* bound variables in the body expression by their  */
			/* values. the variables are artificially bound by  */
			/* a superflous sub to keep postprocessing easy     */
			/* this was not necessary in the lneu reduction     */
			/* machine because the bound variables have then    */
			/* been replaced in the interpreter stage.          */
			resolve = 1;
		      case TY_COMB:
			pf = (PTR_DESCRIPTOR)R_COMB(
			     *((PTR_DESCRIPTOR)(R_CLOS((*desc),pta)[0])),ptd);
                      /* pf verweist auf den hinter dem Closure-Deskriptor */
                      /* liegenden Funktionsdeskriptor: SUB oder LREC_IND  */
			
			if ((R_DESC((*pf),type) == TY_LREC_IND) && (ndargs > 0))
			  /* Die Funktion, nun ist es ein sub kein lrec_ind*/
			  /* Nur wenn die closure nicht wegen redcnt = 0   */
			  /* entstanden ist.                               */
			  pf = (PTR_DESCRIPTOR)R_LREC_IND(*pf,ptf);

			break;
		      case TY_CONDI:
			/* das praedikat steht ganz hinten */
			if (ndargs == 0) {
			  pred = (STACKELEM) p_arg[anz_ges];
			  anz_ges--;
			}
			pf = (PTR_DESCRIPTOR)R_CONDI(
		             *((PTR_DESCRIPTOR)(R_CLOS((*desc),pta)[0])),ptd);
			break;
		      case TY_CLOS:
		      case TY_PRIM:
		      case TY_INTACT: /* stt 11.09.95 */
			/* Primitive Funktionen muessen hier abgefangen werden, */
			/* da sie nicht als Deskriptoren vorliegen              */

			for (i = 0 ; i <= anz_ges - nfv ; i++)
			{
			  if T_POINTER(p_arg[i])
			    INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
			  PUSHSTACK(S_e, p_arg[i]);                      /* primitive funktion und */
			}                                                /* ihre argumente auf e   */
			PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1)); /* funktion anwenden      */

			goto main_e;
		      case TY_CASE:
                      case TY_SELECTION:
			pf = (PTR_DESCRIPTOR)R_CLOS((*desc),pta)[0];
		        if (R_DESC(*pf,type) != TY_NOMAT) {
			  pf = (PTR_DESCRIPTOR)R_CASE(
			          *((PTR_DESCRIPTOR)(pf)),ptd);           /* SWITCH descriptor */
#if UH_ZF_PM
                          DBUG_PRINT("ea_ret",("SELECTION 3"));
                          if (R_DESC(*pf,type) == TY_SELECTION)
                            pf = (PTR_DESCRIPTOR) R_CLAUSE(*(T_PTD)R_SELECTION(*pf,clauses),sons)[3];
#endif

   /* what was this for ?! ne1 knows ?! */
   /* it didn't work with def F [   ] = letp A = 1, B = 2 in 3 in F [   ] (single step) */
   /* commenting the following if out => it works */

/*			  if (R_SWITCH(*pf,casetype) == WHEN_WITHOUT_CASE)
			    pf = (PTR_DESCRIPTOR)(R_SWITCH(*pf,ptse))[1]; */ /* MATCH descriptor  */

                          DBUG_PRINT("ea_ret",("SELECTION 3 ready"));
			}
			break;
		      default:
			pf = (PTR_DESCRIPTOR)(R_CLOS((*desc),pta)[0]);
		      }

                      /* Argumente fuer relativ freie Variable in TLIST */
                      /* eintragen                                      */

                      if (nfv > 0)
                        gen_TLIST_clos(nfv,anz_ges-nfv,p_arg);



                     /*************      Closure fuer ein SUB   ********************/

		    if (R_DESC((*pf),type) == TY_SUB)  
                    { 
		      
		      if (nfv > 0)
			PUSHSTACK(S_m,SET_ARITY(SNAP,0));
		      
		      RES_HEAP;
		      
		      /* Trennsymbol generieren */
		      
		      ILIST_end();

		      /* vorhandene Argumente in ILIST eintragen  */

		      for (i = 1; i <= anz_ges - nfv; i++)
                      {
			y = p_arg[i];
			if (T_POINTER(y))
			  INC_REFCNT((PTR_DESCRIPTOR)y);
			
			p = (postlist*)malloc(sizeof(postlist));
			p->var = y; 
			p->link = (postlist *) ILIST;
			p->next = (postlist *) ILIST;
			ILIST = p;
		      }

		      REL_HEAP;
                      
		      /* Statt der fehlenden Argumente die Variablen */
		      /* in ILIST eintragen                          */

		      RES_HEAP;
		      name = (STACKELEM *)R_FUNC((*pf),namelist);
		      for ( i = ndargs ; i > 0  ; i-- )
		      {
			y = name[i + 1];
			INC_REFCNT((PTR_DESCRIPTOR)y);
			
			p = (postlist*)malloc(sizeof(postlist));
			  p->var = y; 
			p->link = (postlist *) ILIST;
			p->next = ILIST;
			ILIST = p;
		      }
             
		      REL_HEAP;
		      
		      /* ersten Block von TLIST daraufhin ueber- */
		      /* pruefen , ob protects hochgezaehlt      */
		      /* werden muessen                          */

		      inc_protects();
		      
		      /* Markierung auf S_m legen  */

		      PUSHSTACK(S_m,DOLLAR);

		      /* Funktionsrumpf einlagern: */

		      if ((ndargs > 0) || (resolve == 1))
			p_load_expr((STACKELEM **)A_FUNC((*pf),pte));
                         
		      if ( name[1] != (STACKELEM) 0 )
                      {

			/* waehrend der Preprocessing Phase wurden mehrere     */
			/* direkt aufeinander folgende sub's zusammengefasst   */ 
			/* die urspruengliche Struktur ,welche in der namelist */
			/* des SUB - Deskriptors nachgehalten wurde, wird      */
			/* teilweise rekonstruiert                             */
                                                               
			j = (int)name[1];
			anzahl = (int)name[j] -1 ;   /* Anzahl der durch das aeusserste */
			                             /* sub geb. Variablen              */

			anz_bv = (int) R_FUNC((*pf),nargs) ;   /* Anzahl der insgesamt */
                                                               /* geb. Variablen       */

			/* ermittle diejenige Teilstruktur , ab der gebundene */
			/* nicht substituierte Variable existieren            */


			if (ndargs > 0) {
			  while ( (anz_bv - anzahl) >= ndargs )
			  {
			    anz_bv -= anzahl;
			    ++j;
			    anzahl = (int)name[j] - 1;
			  }  

			  if ( anz_bv > ndargs )
			  {
			    /* Durch das betrachtete sub gebundene Var. werden   */
			    /* teilweise substituiert                            */
			    /* Stelligkeit des sub's      */

			    anzahl = (int)name[j] - ( anz_bv -ndargs ); 
			    ++j;

			    /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */
			    PUSHSTACK(S_m,SET_ARITY(SUB,1));
			    PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
			  }

			  /* restliche sub's aufbauen */

			  index = (int)name[0];
			
			  for( ; j <= index ; j++ )
			  {
			    anzahl = name[j] ;    /* Stelligkeit des sub's */
                                                                 
			    /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */

			    PUSHSTACK(S_m,SET_ARITY(SUB,1));
			    PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
			  }

			}
		      }
		      else /* name[1] = 0*/
		      {
			/* SUB(1) auf S_m , SUB(ndargs+1) auf S_m1 */
			/*    nicht bei vollstaendiger Anwendung   */
			if (ndargs > 0)
			{
			  PUSHSTACK(S_m,SET_ARITY(SUB,1));
			  PUSHSTACK(S_m1,SET_ARITY(SUB,ndargs+1));
			}
		      }
		      /* Gebundene ,nicht substituierte Variable  */
		      /* auf S_a legen                            */
 
		      for ( i = ndargs ; i > 0 ; i--)
		      {
			PUSHSTACK(S_a, name[i + 1]);
			INC_REFCNT((PTR_DESCRIPTOR)name[i + 1]);
		      }

		      /* Vollstaendige Anwendung, Closure weil red_count = 0 */
		      
		      if ((ndargs == 0) && (resolve == 0))
		      {
			INC_REFCNT((PTR_DESCRIPTOR)p_arg[0]);
			PUSHSTACK(S_e,p_arg[0]);
			for (i = 1 ; i <= anz_ges - nfv ; i++)
			{
			  if T_POINTER(p_arg[i])
			    INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
			  PUSHSTACK(S_e, p_arg[i]);
			}

			PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1));
		      }

		      resolve = 0;

		      DEC_REFCNT(desc);
		      goto main_e;

                      }  /*************     ENDE Closure fuer SUB      ********************/
                        

		    /************* Closure fuer ein SWITCH *******************/

                      if (R_DESC((*pf),type) == TY_SWITCH)
                      {
                       if (nfv > 0)
                          PUSHSTACK(S_m,SET_ARITY(SNAP,0));
                                              
                       casetype=(STACKELEM)R_SWITCH((*pf),casetype);      
                           /* case- bzw. caserec-Konstruktor */                                   

                       /* index wird als offset benoetigt, um das letzte   */
                       /* Element der ptse-Liste des Switch-Deskriptors zu */
                       /* bestimmen                                        */

                       DBUG_PRINT("ea_ret", ("next SWITCH..."));
                       index=(int)R_SWITCH((*pf),nwhen) + 1;
                       DBUG_PRINT("ea_ret", ("next SWITCH done !"));

                       if( T_PM_REC(casetype) )  index++;

                       sub_desc = (PTR_DESCRIPTOR )( R_SWITCH((*pf),ptse) )[index]; 
                       otherwise = (int)( R_SWITCH((*pf),ptse) )[index + 1];

                       combine = ( sub_desc != NULL );
                      
                       if ( ! combine )
                       {
                         /* keine "zusammengefasste" case-Funktion =>          */
                         /* Switch-Deskriptor und vorhandene Argumente auf den */
                         /* E-Stack legen                                      */
                                                                          
                         PUSHSTACK(S_e,pf);/* Switch-Deskriptor */
                         INC_REFCNT(pf);

			 if (ndargs > 0) {
			   if (R_SWITCH((*pf),anz_args) > 1)                   
			   {
			     /* d.h.,es handelt sich um ein n-stelliges case */

			     for (i = anz_ges - nfv ; i >= 1 ; i-- )
                             {
#if 1
                               y = p_arg[(anz_ges - nfv) - i + 1];
#else
                               y = p_arg[i];
#endif
                               PUSHSTACK(S_e,y);
                               if (T_POINTER(y))
				 INC_REFCNT((PTR_DESCRIPTOR)y);
			     }
			     anz_ges = R_SWITCH((*pf),anz_args);
			     /* = Anzahl der benoetigten Argumente */
			     test = anz_ges - ndargs + 1;
			     if (test > 1)
			     {
                               PUSHSTACK(S_m,SET_ARITY(AP,test));
                               PUSHSTACK(S_m1,SET_ARITY(AP,test));
			     }
			   }  /* n-stelliges case */
			 }
			 /* Vollstaendige Anwendung, Closure weil red_count = 0 */

			 else /* ndargs == 0 */
			 {
			   /*INC_REFCNT(pf);*/
			   /*PUSHSTACK(S_e,p_arg[0]);*/
			   for (i = 1 ; i <= anz_ges - nfv ; i++)
			   {
			     if T_POINTER(p_arg[i])
			       INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
			     PUSHSTACK(S_e, p_arg[i]);
			   }

			   PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1));
			 }

                         DEC_REFCNT(desc);
                         goto main_e;  

                       } /* !combine */

                       else
                       { 
                        /* in der Preprocessing Phase wurden eine oder mehrere   */
                        /* benutzerdefinierte Funktionen und eine Case-Funktion  */
                        /* zu einer mehrstelligen case-Funktion zusammengefasst  */

                         if ( ( anz_ges - nfv ) == NULL )
                         {
                           /* keine Argumente fuer gebundene Variable vorhanden */
                           /* aufloesen der case-Funktion siehe TY_SWITCH       */

                           PUSHSTACK(S_e,pf);/* Switch-Deskriptor */
                           INC_REFCNT(pf);

                           DEC_REFCNT(desc);
                           goto main_e; 
                         }

                         else /* Argumente fuer durch sub's gebundene Variable vorhanden */
                         {      
                           /* anz_ges    : Anzahl der moeglichen Eintraege(Argumente */     
                           /*              fuer gebundene und relativ freie Variable)*/
                           /* nfv        : Anzahl der Argumente fuer relativ freie   */
                           /*              Variable                                  */
                           /* ndargs     : Anzahl der zur Reduktion noch benoetigten */
                           /*              Argumente                                 */

                           /* n_args     : Anzahl der Argumente , die das CASE zur   */
                           /*              Reduktion benoetigt                       */
                           /* anz_sub_bv : Anzahl der urspruenglich durch sub's ge - */
                           /*              bundenen Variablen                        */

                           n_args = (int)R_SWITCH((*pf),anz_args);
                           anz_sub_bv = R_FUNC((*sub_desc),nargs);
                           namelist = (STACKELEM *)R_FUNC((*sub_desc),namelist);
                           otherwise = (int)( R_SWITCH((*pf),ptse) )[index + 1];
                                                          
                           /* ( anz_ges - nfv - ndargs )  : Anzahl der Argumente  */
                           /*  fuer gebundene Variable                            */

                           if (( anz_ges - nfv/* - ndargs */ ) <= anz_sub_bv )
                           {
                            /* der eigentlichen case-Funktion werden keine Argumente */
                            /* zugefuehrt; die durch sub gebundenen Var. muessen     */
                            /* zum Teil durch Argumente substituiert werden, die     */
                            /* restlichen Variablenbindungen muessen restauriert     */
                            /* werden                                                */   

                            /* teilweise Substitution der durch sub gebundenen Variablen  */
 
                            /* argumentliste anlegen und mit den Argumenten aus der       */
                            /* Closure und Variablennamen fuellen                         */ 

                            if ( ( argumentlist = (STACKELEM *)newbuff( anz_sub_bv + 1 ) ) == (STACKELEM)NULL )
                               post_mortem("ea_retrieve: heap out of space ");
 
                            argumentlist[0] = anz_sub_bv;     
                     
                            j = anz_sub_bv;        /* Laufvariable fuer argumentlist */

                            /* Argumente aus der Closure in argumentlist eintragen:  */

			    for ( i = anz_ges - nfv ; i >= 1 ; i--)
			    {
#if 1
                              argumentlist[j] = p_arg[(anz_ges - nfv) - i + 1]; 
#else
                              argumentlist[j] = p_arg[i]; 
#endif
                              j--;
                            }
                               
                            if ( ( n_args - anz_sub_bv ) != ndargs ) 
                            {
                              /* " sub " rekonstruieren */
 
                              /* restliche Variablennamen aus namelist */

                              for ( i = anz_sub_bv - ( n_args - ndargs) ; i > 0 ; i-- )
                              {
                                argumentlist[j] = namelist[i + 1];
                                j--;
                              }

                              /* ermittle diejenige Teilstruktur , ab der gebundene */
                              /* nicht substituierte Variable existieren            */

                              anz_bv = anz_sub_bv;
                              ndargs = anz_bv - ( n_args - ndargs );

                              j = (int)namelist[1];
                              anzahl = (int)namelist[j] -1 ;   /* Anzahl der durch das aeusserste */
                                                               /* sub gebundenen Variablen        */

                              while ( (anz_bv - anzahl) >=  ndargs )
                              {
                                anz_bv -= anzahl;
                                ++j;
                                anzahl = (int)namelist[j] - 1;
                              }  

                              if ( anz_bv > ndargs )
                              {
                                /* Durch das betrachtete sub gebundene Variablen */
                                /* werden teilweise substituiert                 */
                                 
                                /* Stelligkeit des sub's      */
                                anzahl = (int)namelist[j] - ( anz_bv - ndargs ); 
                                ++j;

                                /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */
                            
                                PUSHSTACK(S_m,SET_ARITY(SUB,1));
                                PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
                              }
                               
                              /* restliche sub's aufbauen */

                              index = (int)namelist[0];

                              for( ; j <= index ; j++ )
                              {
                                anzahl = namelist[j] ;    /* Stelligkeit des sub's */
                                                                 
                                /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */

                                PUSHSTACK(S_m,SET_ARITY(SUB,1));
                                PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
                              }

                              /* Gebundene ,nicht substituierte Variable  */
                              /* auf S_a legen                            */

                              for ( i = ndargs ; i > 0 ; i--)
                              {
                                 PUSHSTACK(S_a, namelist[i + 1]);
                                 INC_REFCNT((PTR_DESCRIPTOR)namelist[i + 1]);
                              }
 
                            } /* ende if ( ( n_args - anz_sub_bv ) != ndargs )    */


                           /* setzen einer Marke fuer nomat_post und switch_post */
                            part_sub_var = TRUE;    
                                          
                           /* rekonstruieren der case - Funktion */
 
                            INC_REFCNT(pf);
                            switch_post(pf);
                                                       
                           /* zuruecksetzen der globalen Variablen */

                             part_sub_var = FALSE;
                             anz_sub_bv = 0;
                             otherwise = 0;
#if 0
			    /* Vollstaendige Anwendung, Closure weil red_count = 0 */

			    if (ndargs == 0)
			    {
			      INC_REFCNT(pf);
			      PUSHSTACK(S_e,p_arg[0]);
			      for (i = 1 ; i <= anz_ges - nfv ; i++)
			      {
				if T_POINTER(p_arg[i])
				  INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
				PUSHSTACK(S_e, p_arg[i]);
			      }

			      PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1));
			    }
#endif /* 0 */
                               
                             DEC_REFCNT(desc);     /* Closure-Deskriptor */
                             goto main_e;                              

                           } /* anz_ges - nfv - ndargs ) <= anz_sub_bv */

                           else /* der urspruenglichen case-Funktion wurden Argumente */
                                /* zugefuehrt.                                        */
                           {
                             /* saemtliche durch sub gebundenen Variable muessen substituiert */
                             /* werden; der eigentliche case-Funktion werden Argumente        */
                             /* zugefuehrt; diese Argumente muessen aufgeloest werden,        */
                             /* bevor die case-Funktion rekonstruiert werden darf             */

                             /* Closure plus einer Marke werden auf den E - Stack gelegt;   */
                             /* trifft man im Verlaufe der Abarbeitung auf diese Marke, so  */
                             /* liegen die Argumente der case-Funktion in aufgeloester Form */
                             /* auf dem A-Stack => die case-Funktion darf nun rekonstruiert */
                             /* werden                                                      */

                             PUSHSTACK(S_e,(STACKELEM)desc);
                             PUSHSTACK(S_e,SET_ARITY(PM_GUARD,1));

                             /* Argumente auf den E-Stack legen   */

			     ende = anz_ges - nfv;

			     for ( i = anz_sub_bv + 1 ; i <= ende ; i++)
                             {
                                PUSHSTACK(S_e,(STACKELEM)p_arg[i]);
                                if ( T_POINTER(p_arg[i]) )
                                    INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
                             }  

                             /* Applikationen auf M und  M1 legen */
          
                             PUSHSTACK(S_m,SET_ARITY(AP,(ende - anz_sub_bv + 1)));
                             PUSHSTACK(S_m1,SET_ARITY(AP,(ende - anz_sub_bv + 1)));

                             goto main_e;                                       

                           } /* der case-Funktion wurden Argumente zugefuehrt */


                         } /* Argumente fuer durch sub's gebundene Variable vorhanden */
                        

                       } /* ende combine = TRUE */


                      }  /*************     ENDE Closure fuer SWITCH   ********************/

                      /************* Closure fuer ein MATCH     ********************/

		    if (R_DESC((*pf),type) == TY_MATCH)
                      {

                        if (nfv > 0)
                           PUSHSTACK(S_m,SET_ARITY(SNAP,0));

                        PUSHSTACK(S_e,pf);/* Match-Deskriptor */
                        INC_REFCNT(pf);

                        if ((int)(R_MATCH((*pf),code)[STELLIGKEIT]) > 1)
                           /* d.h.,es handelt sich um ein n-stelliges when */
                        {  
                          /* Argumente aus der Closure auf den E-Stack legen: */

                          for (i = 1 ; i <= anz_ges - nfv ; i++)
                          {
                            y = p_arg[i];
                            PUSHSTACK(S_e,y);
                            if (T_POINTER(y))
                                INC_REFCNT((PTR_DESCRIPTOR)y);
                          }                                                     

                          anz_ges = (int)(R_MATCH((*pf),code))[STELLIGKEIT];
                             /* = Anzahl der benoetigten Argumente */

                          PUSHSTACK(S_m,SET_ARITY(AP,anz_ges - ndargs + 1));
                          PUSHSTACK(S_m1,SET_ARITY(AP,anz_ges - ndargs + 1));
                        }

			/* Vollstaendige Anwendung, Closure weil red_count = 0 */

			if (ndargs == 0)
			{
			  /*INC_REFCNT(pf);*/
			  /*PUSHSTACK(S_e,p_arg[0]);*/
			  for (i = 1 ; i <= anz_ges - nfv ; i++)
			  {
			    if T_POINTER(p_arg[i])
			      INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
			    PUSHSTACK(S_e, p_arg[i]);
			  }

			  PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1));
			}

                        DEC_REFCNT(desc);
                        goto main_e;

		      }  /*************     ENDE Closure fuer MATCH   ********************/

                    /************* Closure fuer ein NOMATCH     ********************/


                    if (R_DESC((*pf),type) == TY_NOMAT)
                    {
                      if (nfv > 0)             
                      {
                        if ( T_AP_1(READSTACK(S_m)))
                        {
                          PPOPSTACK(S_m);
                          PUSHSTACK(S_m,SET_ARITY(SNAP,0));
                          PUSHSTACK(S_m,SET_ARITY(AP,1));
                        }
                        else
                          PUSHSTACK(S_m,SET_ARITY(SNAP,0));
                      }

                      PUSHSTACK(S_e,pf);/* Nomatch-Deskriptor */
                      INC_REFCNT(pf);         

                      DEC_REFCNT(desc);
                      goto main_e;

                    }      /*************     ENDE Closure fuer NOMATCH   ********************/


                    /************* Closure fuer ein Conditional    ********************/


                    if (R_DESC((*pf),type) == TY_COND)
                    {
                      if (nfv > 0)  
                          PUSHSTACK(S_m,SET_ARITY(SNAP,0));
 
                      RES_HEAP;
            
                     /* Trennsymbol generieren */


		      ILIST_end();

		      /* Argumente fuer geb. Var in  ILIST eintragen  */
		      /* der ty_condi-deskriptor nicht mit in varliste */ 

		      for (i = 1; i <= anz_ges - nfv; i++)
		      {
			y = p_arg[i];
			if (T_POINTER(y))
			  INC_REFCNT((PTR_DESCRIPTOR)y);
			p = (postlist*)malloc(sizeof(postlist));
			p->var = y;           
			p->link = (postlist *)ILIST;
			p->next = (postlist *)ILIST;
			ILIST=p;
		      }

                      REL_HEAP;

                      /* ersten Block von TLIST daraufhin ueber- */
                      /* pruefen , ob protects hochgezaehlt      */
                      /* werden muessen                          */

                      inc_protects();

                      /* Markierung auf S_m legen  */

                      PUSHSTACK(S_m,DOLLAR);

                      /* Conditional einlagern */

                      INC_REFCNT(pf);
                      ret_cond((STACKELEM) pf);

		      if (ndargs == 0) {
			/* Praedikat oben drauf und AP */
			if (T_POINTER(pred))
			  INC_REFCNT((PTR_DESCRIPTOR)pred);
			PUSHSTACK(S_e,pred); /* das praedikat */
			PUSHSTACK(S_e,SET_ARITY(AP, 2)); 
		      }

                      DEC_REFCNT(desc);
                      goto main_e;

                    }   /*************  Ende Closure fuer ein Conditional    ********************/

		      
		    /********** closure fuer ein Lrec-ind  ****************/

		    if (R_DESC((*pf),type) == TY_LREC_IND)
		    {
		      INC_REFCNT(pf);
		      i = test_LIND(pf, anz_ges - nfv, p_arg);

		      if (i == 0)
			goto main_e;
		      else 
			goto main_m;
		    }

		    /**************** closure fuer eine closure ******************/

		    if (R_DESC((*pf),type) == TY_CLOS)
		    {
		      /* erster versuch: ap erzeugen, argumente auf e */

		      if T_POINTER(p_arg[0])
			INC_REFCNT((PTR_DESCRIPTOR)p_arg[0]);
		      PUSHSTACK(S_e,p_arg[0]);
		      for (i = 1 ; i <= anz_ges - nfv ; i++)
		      {
			if T_POINTER(p_arg[i])
			  INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);
			PUSHSTACK(S_e, p_arg[i]);
		      }
		      PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - nfv + 1));

		      goto main_e;
		    }

		    /*********** closure fuer eine primitive funktion **********/

		    if ((R_DESC((*pf),type) == TY_PRIM) ||
		        (R_DESC((*pf),type) == TY_INTACT) ) /* stt 11.09.95 */
		    {
		      for (i = 0 ; i <= anz_ges - nfv ; i++)
		      {
			if T_POINTER(p_arg[i])
			  INC_REFCNT((PTR_DESCRIPTOR)p_arg[i]);			    
			PUSHSTACK(S_e, p_arg[i]);                      /* primitive funktion und */
		      }                                                /* ihre argumente auf e   */
		      PUSHSTACK(S_e,SET_ARITY(AP, anz_ges - ndargs + 1)); /* funktion anwenden      */

		      goto main_e;
		    }
	

		    post_mortem("ea_retrieve: unknown closure type"); /* Notbremse */


        case TY_COND : ret_cond(x);
                       goto main_e;
                    


        case TY_LREC_IND: desc = (PTR_DESCRIPTOR)x;
                          i=test_LIND(desc, 0, NULL);
                          if ( i == 0 )
                             goto main_e; 
                          else
                             goto main_m; 

        case TY_SWITCH: desc = (PTR_DESCRIPTOR)x;
                        casetype=(STACKELEM)R_SWITCH((*desc),casetype);      
                        /* case- bzw. caserec-Konstruktor */

                        /* index wird als offset benoetigt, um das letzte   */
                        /* Element der ptse-Liste des Switch-Deskriptors zu */
                        /* bestimmen                                        */

                        index=(int)R_SWITCH((*desc),nwhen) + 1;             

                        if( T_PM_REC(casetype) )  index++;

                        sub_desc = (PTR_DESCRIPTOR)( R_SWITCH((*desc),ptse) )[index]; 
                        otherwise = (int)( R_SWITCH((*desc),ptse) )[index + 1];
             
                        combine = ( sub_desc != NULL );

                        if ( combine )
                        {
                          namelist = (STACKELEM *)R_FUNC((*sub_desc),namelist);
                          otherwise = (int)( R_SWITCH((*desc),ptse) )[index + 1];

                          /* namelist != NULL => in der Preprocessing Phase wurden ein */
                          /* oder mehrere sub's plus einem case zusammengefasst; die   */
                          /* urspruengliche Struktur kann mittels der in namelist      */
                          /* nachgehaltenen Informationen rekonstruiert werden         */
                          /* part_sub_var = FALSE => der betrachtete Switch-Deskriptor     */
                          /* war nicht als Funktion in einer Closure eingetragen       */
                           
                          /* Anzahl der durch sub's gebundenen Variablen: */

                          anz_sub_bv = namelist[1] - 2;

                          /* Markierung fuer switchpost: */

                          xxx = TRUE;

                          /* Rekonstruktion der urspruenglichen Struktur */

                          index = (int)namelist[0];
                          j = namelist[1];

                          for( ; j <= index ; j++ )
                          {
                            anzahl = namelist[j] ;    /* Stelligkeit des sub's */
                                                                 
                            /* SUB(1) auf S_m , SUB(anzahl) auf S_m1 */

                            PUSHSTACK(S_m,SET_ARITY(SUB,1));
                            PUSHSTACK(S_m1,SET_ARITY(SUB,anzahl));
                          }
                       
                          /* Gebundene Variablen auf S_a legen  */

                          for ( i = anz_sub_bv ; i > 0 ; i--)
                          {
                            PUSHSTACK(S_a, namelist[ i + 1 ]);
                            INC_REFCNT((PTR_DESCRIPTOR)namelist[ i + 1 ]);
                          }            

                        } /* combine */


                        if( T_PM_REC(casetype) )
                        {
                          if((i=test_CASE(desc)) == 0 )
                          /* Aufbau der caserec-Funktion */
                          {
                            /* Case-Deskriptor und Funktionsnamen in ILIST  */
                            /* eintragen ;                                  */
                            gen_ILIST_caserec(desc);                                
                
                            /* ersten Block von TLIST daraufhin ueber- */
                            /* pruefen , ob protects hochgezaehlt      */
                            /* werden muessen                          */
                            inc_protects();

                            /* Markierung auf S_m legen  */
                            PUSHSTACK(S_m,DOLLAR);       

                            /* Funktion aufbauen: */
                            switch_post(desc);
                             
                            goto main_e;
                          }

                          else  /* Case-Deskriptor wurde durch Funktionsnamen ersetzt */
                            goto main_e;

                        } /* PM_REC */

                        else
                        { 
                          switch_post(desc);

                          if ( xxx )
                          {
                            /* zuruecksetzen der Werte */

                             xxx = FALSE;
                             anz_sub_bv = 0;
                             otherwise = NULL;
                          }

                          goto main_e;
                        }

        case TY_MATCH: desc = (PTR_DESCRIPTOR)x; 

                       code = (PTR_HEAPELEM *)R_MATCH((*desc),code);
                       /* code entspricht dem Code-Vektor von pf,in dem das         */
                       /* Originalpattern,die Patternvariablen (= die in der        */
                       /* when-Klausel gebundenen Variablen) und der Match-Code     */
                       /* eingetragen sind.                                         */
         
                       nwhen = ( (int)code[STELLIGKEIT] > 1 );
                         /* n-stelliges when? */

                       offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
                         /* offset verweist auf die Liste der Patternvariablen: */

                       anz_bv = (int)offset[0];  /* = Anzahl der Patternvariablen */

                       /* Trennsymbol generieren */

                       ILIST_end();

                       /* Patternvariable in ILIST eintragen*/

                       for (j = anz_bv ; j > 0 ; j--)
                       {
#if 1
			 /* inverse namelist as generated by preprocessor */
                        pf = (PTR_DESCRIPTOR)offset[anz_bv-j+1];
#else
			pf = (PTR_DESCRIPTOR)offset[j];
#endif

                        INC_REFCNT(pf);
        
                        p = (postlist *)malloc(sizeof(postlist));
                        p->var = (STACKELEM)pf;
                        p->link = (postlist *)ILIST;
                        p->next = (postlist *)ILIST;
                        ILIST = p;
                       }
        
                       /* body-Ausdruck einlagern: */
                       p_load_expr((STACKELEM **)A_MATCH((*desc),body));
         
                       if (R_MATCH((*desc),guard) != (PTR_HEAPELEM)0)
                       /* guard-Ausdruck einlagern: */
                          p_load_expr((STACKELEM **)A_MATCH((*desc),guard));
                       else
                       /* guard-Ausdruck besteht aus dem Stackelement TRUE */  
                          PUSHSTACK(S_e,SA_TRUE); 

                       if (nwhen)
                       /* n-stelliges when */ 
                       {
                         pf = (PTR_DESCRIPTOR)code[OPAT];
                         /* Verweis auf das 'Original'-pattern; in der Preprocessing- */  
                         /* Phase wurden die verschiedenen Pattern einer when-Klausel */
                         /* als Listenstruktur ausgelagert.                           */

                         INC_REFCNT(pf);
                         /* da in ret_list der refcount dekrementiert wird */

                         ret_expr(pf);

                         /* Listenkonstruktor entfernen: */
                         if (T_LIST(READSTACK(S_e)))
                            PPOPSTACK(S_e);                

                         PUSHSTACK(S_e,SET_ARITY(PM_WHEN,(int)code[STELLIGKEIT] + 2));
                       }
                       else /* 3-stelliges when */                                      
                       {
                         PUSHSTACK(S_e,(PTR_DESCRIPTOR)code[OPAT]);
                             /* Pointer auf Originalpattern */
     
                         INC_REFCNT((PTR_DESCRIPTOR)code[OPAT]);    
          
                         DEC_REFCNT(desc);
                 
                         /* jetzt noch WHEN(3) auf den E-Stack : */
   
                         PUSHSTACK(S_e,SET_ARITY(PM_WHEN,3));
                       } 

                       goto main_e;

        case TY_NOMAT: desc = (PTR_DESCRIPTOR)x;  

                       reason = (int) R_NOMAT((*desc),reason); 
                         /* reason gibt an, aus welchem Grund der Nomatch-Deskriptor */
                         /* angelegt wurde                                           */

                       sw_desc =  (PTR_DESCRIPTOR)R_NOMAT((*desc),ptsdes); /* mah 170294 */

		       /* mah 010394 */
		       if (R_SWITCH(*sw_desc,casetype) == WHEN_WITHOUT_CASE)
			 sw_desc = (PTR_DESCRIPTOR)(R_SWITCH(*sw_desc,ptse))[1]; /* MATCH */

                         /* Verweis auf den Case- bzw. Match-Deskriptor, fuer den */
                         /* der Nomatch-Deskriptor angelegt wurde.                */ 

                       if ( R_DESC((*sw_desc),type) == TY_SWITCH )
                       {
                          when = FALSE;
                          /* Nomatch-Deskriptor fuer ein case */

                         arg_desc = (STACKELEM) R_NOMAT((*desc),guard_body)[3];
                         /* Verweis auf das Argument */

                          ncase = ( R_SWITCH((*sw_desc),anz_args) > 1 );            
                            /* n-stelliges case? */

                          if ( ncase )
                          {
                           /* es handelt sich um eine mehrstellige case-Funktion       */
                           /* teste, ob in der Preprocessing Phase benutzerdefinierte  */
                           /* Funktionen und eine case-Funktion zusammengefasst wurden */ 
                           /* wenn ja => combine = TRUE                                */

                           casetype=(STACKELEM)R_SWITCH((*sw_desc),casetype);      
                            /* case- bzw. caserec-Konstruktor */

                           index=(int)R_SWITCH((*sw_desc),nwhen) + 1;

                           if( T_PM_REC(casetype) )  index++;

                           sub_desc = (PTR_DESCRIPTOR )( R_SWITCH((*sw_desc),ptse) )[index];   
                           otherwise = (int)( R_SWITCH((*sw_desc),ptse) )[index + 1];

                           combine = ( sub_desc != NULL );
                          }
                          else
                              combine = FALSE;
                       }
                       else  /* Nomatch-Deskriptor fuer eine pi-Abstraktion */
                       {
                          when = TRUE;
                          arg_desc = (STACKELEM) R_NOMAT((*desc),guard_body)[3];
/* ----> sbs <--- (vorherige Zeile ergaenzt)  */
                          ncase = ( ( ( R_MATCH((*sw_desc),code) )[STELLIGKEIT] ) > 1 );
                          combine = FALSE;
                       }

                       if ( ! combine )
                       {
                          if ( ( reason == 1 ) || ( ( when == TRUE ) && ( reason <= 4 ) ) )
                          {
                            /* es ist ein Ausdruck der Form ap argument p_switch bzw.     */
                            /* ap argument p_match aufzubauen, da in der Processing Phase */
                            /* das Matching fuer alle pi-Abstraktionen fehlschlug         */
                               
                               PUSHSTACK(S_e,(STACKELEM)sw_desc); 
                               INC_REFCNT(sw_desc);
                               if ( ( !ncase ) && !( T_AP_1( READSTACK(S_m) ) ) )
                               /* das Pattern hat nicht gepasst, daher wurde kein */
                               /* Ausdruck der Form ap arg nomat hinter einem     */
                               /* Expression-Deskriptor ausgelagert.              */
                                 {
                                 arg_desc = (STACKELEM) R_NOMAT((*desc),guard_body)[3];
                                 /* Verweis auf das Argument */
/* --->   sbs   <--- */          if (when) {
                                   PUSHSTACK( S_e, arg_desc );
                                   if ( T_POINTER( arg_desc ) )
                                     INC_REFCNT( (PTR_DESCRIPTOR)arg_desc );
                                   PUSHSTACK( S_m1, SET_ARITY( AP, 2 ) );
                                   PUSHSTACK( S_m, SET_ARITY( AP, 2 ) );
                                   }
                                 }

                          }
                          else
                          {
                            /* es wird ein Ausdruck der Form ap argument nomat(1) p_nomat */
                            /* aufgebaut, trifft man auf der E - Linie auf nomat(1), so   */
                            /* ist der Nomatch-Deskriptor aufzuloesen                     */

                            PUSHSTACK(S_e, (STACKELEM)desc);
                            PUSHSTACK(S_e,SET_ARITY(PM_NOMAT,1));
                          }

                          if ( ncase )
                          {
                            /* in der Processing Phase wurden die Argument fuer die mehrstellige */
                            /* case-Funktion / pi-Abstraktion zu einer Liste zusammengefasst.    */
                            /* diese Liste wird aufgeloest                                       */
                               
                            i = R_LIST((*(PTR_DESCRIPTOR)arg_desc),dim);
                            for ( j = 0 ; j < i ; j++ )
                            {
                               PUSHSTACK(S_e,(x = R_LIST((*(PTR_DESCRIPTOR)arg_desc),ptdv)[j]));
                               if (T_POINTER(x))
                                  INC_REFCNT((PTR_DESCRIPTOR)x);
                            }     
                              PUSHSTACK(S_m1,SET_ARITY(AP,i+1));
                              PUSHSTACK(S_m,SET_ARITY(AP,i+1));
/* ----> sbs <--- (vorherige 2 Zeilen ergaenzt)  */

                          }
                          else
                          {
                            if ( !when )
                            {
                            /* arg_desc auf S_e legen */

                            PUSHSTACK(S_e,arg_desc);
                            if (T_POINTER(arg_desc))
                               INC_REFCNT((PTR_DESCRIPTOR)arg_desc);
                            i = 1;
                            }
                          }

                          /* Applikatoren auf den M - und den M1 - Stack legen */

/* ---> sbs <--- */       if ( !when && (!ncase))  /* hier stand vorher : if (!when) */
                          {
                            PUSHSTACK(S_m ,SET_ARITY(AP,i +1));
                            PUSHSTACK(S_m1,SET_ARITY(AP,i +1));
                          }

                          goto main_e ;

                        } /* ende if( ! combine )  */

                        else
                        {  
                          /* es handelt sich um eine zusammengefasste case-Funktion        */
                          /* die duch sub gebundenen Variable muessen substituiert werden  */ 
                          /* die Argumente der case-Funktion werden auf den E-Stack gelegt */

                          if ( reason == 1 )
                          {
                            /* Closure anlegen; Switch-Deskriptor und Argumente eintragen */
                               
                            if ( ( x = st_switch_clos(desc) ) == NULL )
                                post_mortem("ea_retrieve: no more space available");
                            else
                                PUSHSTACK(S_e,x);
                             
                            goto main_e;
                          }
                          else
                          {
                            /* es wird ein Ausdruck der Form ap argument nomat(1) p_nomat */
                            /* aufgebaut, trifft man auf der E-Linie auf nomat(1), so     */
                            /* ist der Nomatch-Deskriptor aufzuloesen                     */

                            PUSHSTACK(S_e, (STACKELEM)desc);       
                            PUSHSTACK(S_e,SET_ARITY(PM_NOMAT,1));

                            /* die Argumente fuer die Case-Funktion werden aud S_e gelegt */

                            anz_sub_bv = R_FUNC((*sub_desc),nargs);
                            i = R_LIST((*(PTR_DESCRIPTOR)arg_desc),dim);

                            for ( j = anz_sub_bv ; j < i ; j++ )
                            {
                               PUSHSTACK(S_e,(x = R_LIST((*(PTR_DESCRIPTOR)arg_desc),ptdv)[j]));
                               if (T_POINTER(x)) 
                                  INC_REFCNT((PTR_DESCRIPTOR)x);
                            }    

                          /* Applikatoren auf den M- und den M1-Stack legen */

                            PUSHSTACK(S_m ,SET_ARITY(AP,(i- anz_sub_bv) +1));
                            PUSHSTACK(S_m1,SET_ARITY(AP,(i - anz_sub_bv)+1));

                            goto main_e;

                          } /* reason != 1 */

                        } /* combine */

	case TY_EXPR :  p_ret_expr(x);    /* einlagern */
                          goto main_e;


        case TY_STRING :
        case TY_UNDEF:   switch(class1 = R_DESC((*((PTR_DESCRIPTOR)x)),class)) 
	                  {
                             case C_LIST    :   /* Liste einlagern  incl. <>  */
                                                 ret_list(x);
                                                 goto main_e;
                             case C_MATRIX  :        /* nilmat   bzw.         */
                             case C_TVECTOR :        /* niltvect bzw.         */
                             case C_VECTOR  :        /* nilvect  einlagern    */
                                              ret_mvt(x,class1);
                                              goto main_e;
                           }

        case TY_SLOT:    ret_slot(x);  /* cr 14.11.95 */
                         goto main_e;
        case TY_FRAME:   ret_frame(x);  /* cr 14.11.95 */
                         goto main_e;

        case TY_INTACT:  ret_intact(x); /* Interaktion einlagern stt 11.09.95 */
                         goto main_e;

        default:         PUSHSTACK(S_a,x);     /* Datenpointer, wird erst in */
                             goto main_m;      /* ae-retrieve retrieved      */

      } /* switch(R_DESC((* ((PTR_DESCRIPTOR)x)),type)) */

    } /* T_POINTER */



    if (T_ATOM(x))                     /* sonstiges Single-atom traversieren */
    {
      PUSHSTACK(S_a,x);
      goto main_m;
    }

    post_mortem("ea_retrieve: No match successful on main_e");
  


main_m:     

    x = READSTACK(S_m);

    if (T_DOLLAR(x))              /* obersten Block des Inkarnationsstacks  */
    {                             /* entfernen                              */

      PPOPSTACK(S_m);            /* DOLLAR */
      dec_protects();
      free_ILIST_block();
      goto main_m;
    }


    if (T_SNAP(x))
    {
      PPOPSTACK(S_m);
      free_TLIST_block();
      goto main_m;
    }
    

    if (T_CON(x))
    {
       if (ARITY(x) > 1)  /* dh die Arity des Konstruktors auf M war >= 2 */
       {
         if(( T_LREC(x)) && (ARITY(x) == 2 ))
         {
           /* auf S_e liegt nun der Startausdruck    */
           /* liegt unter dem LREC ein SNAP , so wird der erste  */
           /* Block von TLIST entfernt ( er enthaelt die Argumente */
           /* fuer die relativ freien Variablen des LREC's )      */

           y = POPSTACK(S_m);   /* LREC entfernen */

           z = READSTACK(S_m);

	   if (!CloseGoal)
	     if ( T_SNAP(z) )
	     {
	       PPOPSTACK(S_m);      /* SNAP entfernen */
	       free_TLIST_block();
	       inc_protects();
	     }

           PUSHSTACK(S_m,y);   /* LREC wieder auf S_m legen */

          } /* LREC(2) */
           
          WRITESTACK(S_m,DEC(x));            /* das uebliche */
          goto main_e;
       }
       else /* ARITY = 1 */
       {
         if( ( T_PM_WHEN(x) ) || T_PM_OTHERWISE(x) )
         {
            /* WHEN / OTHERWISE von S_e nach S_a traversiert ;    */
            /* der erste Block aus ILIST wird ausgehaengt ,       */ 
            /* der erste Block in TLIST wird daraufhin ueber -    */
            /* prueft , ob protects heruntergezaehlt werden       */
            /* muessen                                            */

            dec_protects();
            free_ILIST_block();
         }

         PPOPSTACK(S_m);
         PUSHSTACK(S_a,POPSTACK(S_m1));
         goto main_m;

       } /* ARITY = 1 */
    }


    if (T_KLAA(x))     /* @ bedeutet, ea_retrieve ist zuende! */
    {
      ae_retrieve();  /* Vom A-Stack auf den E-Stack traversieren und restliche */
                      /* Pointer einlagern.                                     */
      free(ILIST);
      free(TLIST);
      PPOPSTACK(S_m);   /* KLAA */
      END_MODUL("ea_retrieve");
      return;
    }

} /* Ende ea_retrieve */







/******************************************************************************/
/*                                                                            */
/*  ae_retrieve  --  traversiert eine Ausdruck vom A-Stack auf den E-Stack.   */
/*                   Dabei werden die letzten verblieben Pointer fuer Daten-  */
/*                   Strukturen und Namen wieder eingelagert.                 */
/*  globals    --    S_e,S_a     <w>                                          */
/*                                                                            */
/******************************************************************************/


static void ae_retrieve()
{
  register STACKELEM           x;
/*  register unsigned short  arity; */
  char class1;                /* for 2nd parameter of function call ret_mvt */


  START_MODUL("ae_retrieve");

main_a:                        

  x = POPSTACK(S_a);

  if (T_CON(x))                             /* Konstruktor nur traversieren */
  {       
     /* LREC(1) auf S_a =>                                   */
     /* statt func_name wird @func_name.PRE auf S_e gelegt   */

     if ( ( ARITY(x) == 1 ) && ( T_LREC(x) ) )
     {
       x=POPSTACK(S_a);                      /* Namensdescriptor */
       ret_lname(x);
       goto main_m;
     }    

     if (ARITY(x) > 0)
     {
#if RED_TO_NF
       /* Streichen von PROTECTS, die durch UNPROTECTS aufgehoben werden */
       /* Vor.: UNPROTECTS stehen auf dem A-Stack (vorher auf E-Stack)   */
       /*       "unter" den PROTECTS.                                    */
       if (T_UNPROTECT(x))
       {
         if (T_PROTECT(READSTACK (S_m)))
         {
           PPOPSTACK(S_m); /* TB, 30.10.92 */
           PPOPSTACK(S_m1); /* TB, 30.10.92 */
         }
         else /* sollte nie ausgefuehrt werden, */
              /* da es nie mehr UNPROTECT als PROTECT */
              /* Konstruktoren geben darf. */
         {
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
         }
       }
       else
       if (T_PROTECT(x))
       { 
         if (T_UNPROTECT(READSTACK (S_m)))
         {
           PPOPSTACK(S_m); /* TB, 30.10.92 */
           PPOPSTACK(S_m1); /* TB, 30.10.92 */
         }
         else 
         {
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
         }
       }
       else
#endif /* RED_TO_NF */
       {
	 PUSHSTACK(S_m,x);
	 PUSHSTACK(S_m1,x);
       }
       goto main_a;
     }
     else
     {
       PUSHSTACK(S_e,x);
       goto main_m;
     }
  }

  /* mah 141093 T_INT eingefuegt fuer TAG_INT-integer */

  if (T_INT(x))					/* BM */
     {						/* BM */
      convi(VAL_INT(x)) ;			/* BM */
      goto main_m ;				/* BM */
     }						/* BM */

  if (T_POINTER(x))                         /* Pointer aufloesen! Es darf keiner */
  switch(class1 = R_DESC( (* ((PTR_DESCRIPTOR)x) ),class))/* uebrig bleiben */
    {
      case C_EXPRESSION:           /* z.Z. nur Namenspointer */
                         switch (R_DESC( (* ((PTR_DESCRIPTOR)x) ),type))
                         {
                           case TY_NAME: ret_name(x);    /* Namen einlagern auf E */
                                         goto main_m;
                           case TY_FNAME: ret_fname(x);    /* Namen einlagern auf E */
                                         goto main_m;
                           default:      post_mortem("ae_retrieve:Unexpected descriptortype");
                         }
                         break;
      case C_SCALAR:     ret_scal(x);       /* formatierte Zahl einlagern */
                         goto main_m;
      case C_DIGIT :     ret_dec(x);        /* Digitstring einlagern */
                         goto main_m;
      case C_MATRIX :                       /* Matrix  bzw.      */
      case C_VECTOR :                       /* Vektor  bzw.      */
      case C_TVECTOR:    ret_mvt(x,class1); /* TVektor einlagern */
                         goto main_m;

      default:           post_mortem("ae_retrieve: Unexpected descriptorclass");
    } /* Ende von T_POINTER */

  if (T_ATOM(x))            /* sonstiger Singleatom */
  {
    PUSHSTACK(S_e,x);
    goto main_m;
  }
  post_mortem("ae_create: No match successful on main_a");

main_m:
  x = READSTACK(S_m);

  if (T_CON(x))
  {
     if (ARITY(x) > 1)  /* dh die Arity des Konstruktors war >= 2 */
     {
       WRITESTACK(S_m,DEC(x));              /* das uebliche */
       goto main_a;
     }
     else
     {
       PPOPSTACK(S_m);
       PUSHSTACK(S_e,POPSTACK(S_m1));
       goto main_m;
     }
  }

  if (T_KLAA(x))                            /* ae_retrieve ist zu beenden */
  {
     END_MODUL("ae_retrieve");
     return;
  }
  post_mortem("ae_retrieve main_m: no match");
}


 
