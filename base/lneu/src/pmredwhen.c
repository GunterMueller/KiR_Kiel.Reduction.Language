
/*----------------------------------------------------------*/
/*  include files                                           */
/*                                                          */
/*----------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "pminstr.h"


/*--------------------------------------------------------------------
 * Allgemein verwendete externe Variablen und Funktionen
 *-------------------------------------------------------------------
 */
extern  STACKELEM st_expr ();             /* storexpr.c */
extern  STACKELEM st_clos ();             /* rstpro.c */
extern int  mvheap ();                    /* mvheap:c */
extern int  _redcnt;                      /* state:c - der Reduktionszae hler*/
extern  BOOLEAN _beta_count_only;      /* <r> */

                                               
/*---------------------------------------------------------------*/
/*  Schnittstelle zwischen pmwhenmat und pmredwhen               */
/*---------------------------------------------------------------*/
/* mit den in pmredsw deklarierten Variablen pm_ta, und
  ex erfolgt die Uebergabe der Ergebnisse von pmmat. pm_ta
  nimmt folgende Werte an */

#define SUCCESS       PM_SUCC
#define FAILURE       PM_FAIL
#define UNDECIDED     PM_UDC
#define GUARD_AUSRECHNEN 14525

/*********************************************************************/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*********************************************************************/


/*********************************************************************/
/*-------------------------------------------------------------------*/
/*pmwhenmat - steuert den Vergleich eines pattern mit dem Argument       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*********************************************************************/

void pmwhenmat (desc, arg , ppm_ta, pex, pbody)


PTR_DESCRIPTOR desc;

STACKELEM arg;

int *ppm_ta;
STACKELEM *pex;
STACKELEM *pbody;
{
    int     i;                            /* Laufvariable */
    STACKELEM h;
    int     snap_arity;
    int     size;
    STACKELEM x;
 /* zum Transport der Variablen vom Pm-stack auf den I-stack */
    STACKELEM hilf;


    int bool ;
    int     save_redcnt;         /* zum retten des redcounters bei
                                             guard-Behandl ung      */
    int oldredcnt;   
    PTR_HEAPELEM bcode;

    START_MODUL ("pmwhenmat");


    /*Heap reservieren wegen Parameter code. Der Heap wird*/
    /*wieder freigegeben in pm_am                         */
    RES_HEAP;
    bool = pm_am ((R_MATCH ((*desc), code)), arg);

    switch (bool)
    {



/*********************************************************************/
/* -----------------   P A T T E R N       P A S S T         --------*/
/*********************************************************************/


        case PM_PASST:             /* das pattern passt  */

           /*Testen ob der Guard nur aus einem true besteht*/
           if (R_MATCH((*desc),guard == NULL))
            
              /*---------------------------------------------------*/
              /*Guard besteht nur aus einem True, Laden des Rumpfes*/
              /*kann direkt erfolgen, ohne die Kontrolle an den    */
              /*ear abzugeben                                      */
              /*---------------------------------------------------*/
              {
                   /*Tag setzen fuer pmredwhen */
                   *ppm_ta = SUCCESS;
   
                   /*Match-Deskriptor vom Stack nehmen */
                     desc = (PTR_DESCRIPTOR) POPSTACK(S_e);
   
                   /* body des when auf den E-stack   */
                        load_expr ((STACKELEM *) A_MATCH ((*desc), body));
   
                   /* Match-Descr. freigeben, Reduktion hat geklappt */
                      DEC_REFCNT (desc);
                      END_MODUL("pmwhenmat");
                      return ;
              }
           else
              /*------------------------------------------------*/
              /*Der Guard Ausdruck muss erst ausgerechnet werden*/
              /*------------------------------------------------*/
              {
      
      
              /* guard auf den E-stack   */
               load_expr ((STACKELEM *) A_MATCH ((*desc), guard));
      
               *ppm_ta = GUARD_AUSRECHNEN;
      
               END_MODUL("pmwhenmat"); 
               
               return;
      
            }/*ende des Ausrechnen des Guard-Ausdrucks*/       


/*********************************************************************/
/*  P A T T E R N    E N T H A E L T  K O N S T A N T E  O D E R     */
/*  A P P L I C A T I O N  D I E   G E G E N   V A R I A B L E       */
/*  G E M A T C H T    W U R D E                                     */
/*********************************************************************/

        case PM_UNDEC:
    /* im Argument steht eine Variable, die gegen eine Konstante oder
       Listenkonstruktor gematcht wurde  */
            *ppm_ta = UNDECIDED;
            *pex = 0;

            END_MODUL ("pmwhenmat");

            break;





/*********************************************************************/
/* ---------   P A T T E R N       P A S S T     N I C H T   --------*/
/*********************************************************************/

        default:                   /* weder PM_PASST noch PM_UNDEC */
                                   /* also PM_FALSE                */
            *ppm_ta = FAILURE;
            END_MODUL("pmwhenmat");
            return ;
    }

}




/************************************************************************/
/*----------------------------------------------------------------------*/
/*pmwhenmat_2 nimmt die Behandlung des Case nach Ausrechnen des Guard-  */
/*Ausdrucks wieder auf.                                                 */
/*----------------------------------------------------------------------*/
/************************************************************************/


BOOLEAN pmwhenmat_2()
{
STACKELEM bool;
PTR_DESCRIPTOR mat_desc;
PTR_DESCRIPTOR nomat_desc;
int regel,arity;
int  i;                            /* Laufvariable */
STACKELEM h;
int     snap_arity;
int    anz_fv,size;
STACKELEM x;
STACKELEM hilf;

START_MODUL("pmwhenmat_2");

POPSTACK(S_m); /* GUARD */
/*der Wert des Ausgerechneten Guard-Ausdrucks       */
bool = POPSTACK(S_a);

/*Referenz auf die pi-Abstraktion vom E-Stack nehmen */
/*und nach mat_desc schreiben                        */
mat_desc = (PTR_DESCRIPTOR) POPSTACK(S_e);
/* body des when auf den E-stack   */

switch ( bool & ~F_EDIT)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  -----       g u a r d  i s t  t r u e             --------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 case SA_TRUE:

            /* body des when auf den E-stack   */
                     load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));
                                                    
            /* Inkarnationsblock darf nicht entfernt werden, er wird fuer die */
            /* Reduktion des body-Ausdruckes benoetigt.                       */

                     if (T_DOLLAR (READSTACK (S_m)))
                     {
                          arity = 1;
                          POPSTACK (S_m); /* das T_DOLLAR */
                     }                                                          
                     else
                          arity = 0;


                   /* Beseitigen des Argumentes */
                   if (T_POINTER (READSTACK (S_a)))
                      {
                       register    PTR_DESCRIPTOR x;
                       x = ((PTR_DESCRIPTOR) POPSTACK (S_a));
                       DEC_REFCNT (x);
                      }
                   else if (T_ATOM(READSTACK(S_a)))
                           POPSTACK(S_a);
                        else
                           post_mortem("pmredwhen: Argument fuer das Switch war kein Single-Atom oder Pointer");
                   
                   /* Beseitigen des Applikators */
                      if (T_AP_2 (READSTACK (S_m1)))
                      {
                          PPOPSTACK (S_m);
                          PPOPSTACK (S_m1);
                      }
                      else
                         WRITE_ARITY (S_m1, READ_ARITY (S_m1) - 1);

                      if (arity == 1)
                      {
                         for (arity = 0; T_AP_1(READSTACK(S_m));  arity++) 
                             POPSTACK(S_m);

                         PUSHSTACK(S_m,DOLLAR);

                         for ( ; arity > 0; arity-- )
                             PUSHSTACK(S_m,AP_1);

                      }

                   /*Reduktion hat geklappt, Matchdeskriptor beseitigen */
                     DEC_REFCNT((PTR_DESCRIPTOR) mat_desc);

                     END_MODUL("pmwhenmat_2");

                     /*weiter auf dem E-Stack; Ausrechnen des Rumpfausdrucks*/
                     return(FALSE);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   ------- g u a r d   i s t   f a l s e       ----------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 case SA_FALSE:


                     /* i-stack aufraeumen */
                          if (T_DOLLAR(READSTACK(S_m)))
                          {
                          POP_I_BLOCK (hilf);
                          POPSTACK(S_m); /* das Dollar fuer dieses
                                              Blockelement */
                          }

                          if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmwhenmat_2: Heap out of Space");
                 DESC_MASK (nomat_desc, 1,  C_EXPRESSION, TY_NOMAT);

                   
                   
                 /*Matching des Pattern hat nicht geklappt*/
	             L_NOMAT ((*nomat_desc), guard_body) = 0;

                 /* kopieren des Match-Deskriptors */
                 L_NOMAT((*nomat_desc),ptsdes)=(T_PTD)mat_desc;
                 
                 /* Increment Refcnt fuer Match-Deskriptor nicht noetig, */
                 /* da der Pointer vom Stack genommen wird               */
                 /* und es stattdessen eine Referenz vom neu             */
                 /* angelegten Nomatchdescr. auf den Match-Deskriptor    */
                 /* gibt                                                 */

                 L_NOMAT ((*nomat_desc), act_nomat) = -1 ;

                 L_NOMAT ((*nomat_desc), reason) = 1 ;


                 /* schliessen des nomatch */

   

                 if (SIZEOFSTACK_TILDE > 0)
                 {
                   PUSHSTACK(S_e, nomat_desc);
                   if ( ( x = st_clos(0,0) ) == NULL )
                       post_mortem("pmredsw: Heap out of space ");
                   else
                       PUSHSTACK(S_a,x);
                 }
                 else
                   PUSHSTACK(S_a, nomat_desc);


   
                 END_MODUL("pmwhenmat_2");

                 /*weiter auf der M-Linie des ear*/
                 return(TRUE) ;





/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* --- g u a r d  i s t  w e d e r  t r u e  n o c h  f a l s e  --*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 default:

                   /*Nomatchdeskriptor anlegen*/
                     if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmredwhen: Heap out of Space");
                     DESC_MASK (nomat_desc, 1,  C_EXPRESSION, TY_NOMAT);
           
                    /*Platz besorgen*/
                     if (newheap( 4,A_NOMAT ((*nomat_desc), guard_body)) == 0) 
                     post_mortem("pmwhenmat_2: es konnte kein neuer descr angelegt werden ");

                    /* kopieren des Matchdesciptors */
                     L_NOMAT((*nomat_desc),ptsdes)=(T_PTD)mat_desc;

                   /* eintragen der Zahl der ueberprueften whens */
                     L_NOMAT ((*nomat_desc), act_nomat) = -1 ;

                     if (_redcnt == 0)
                        L_NOMAT ((*nomat_desc), reason) = 4 ;
                     else 
                        L_NOMAT ((*nomat_desc), reason) = 3 ;
                   
                   /*guard-body ausfuellen                       */
                   /*Dazu die Laenge wegschreiben                */
                     L_NOMAT ((*nomat_desc), guard_body)[0] = 3;

                   /*ersten Teil des Guards hinter bool wieder auf*/
                   /*den Stack legen                              */
                    PUSHSTACK(S_a, bool);

                   /* guard Ausdruck auslagern, falls er kein Pointer oder Single
                      Atom ist */
                     if (!(T_POINTER (READSTACK (S_a))))
                     {
                          if ((x = st_expr ()) != 0)
                           {
                              /*guard wegschreiben */
	                        L_NOMAT ((*nomat_desc), guard_body)[1] = x;
                           }  
                          else
                              post_mortem (" pmwhenmat_2: Ausdruck konnte nicht ausgelagert werden ");
                     }
                     else
                        /*guard wegschreiben */
	                  L_NOMAT ((*nomat_desc), guard_body)[1] = POPSTACK(S_a);



                    /* body des when auf den E-stack   */
                     load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));


                     /* Body-Ausdruck zum Auslagern auf den A-Stack*/
                     /* traversieren                               */
                     trav(&S_e, &S_a);

                     if (!(T_POINTER (READSTACK (S_a))))
                     {
                          if ((x = st_expr ()) != 0)
                          {
                             /*body in den nomatchdescr. wegschreiben*/
	                        L_NOMAT ((*nomat_desc), guard_body)[2] = x;
                          }
                          else
                              post_mortem (" pmwhenmat_2: Ausdruck konnte nicht ausgelagert werden ");
                     }
                     else
                     {
                            /*body in den nomatchdescr. wegschreiben*/
	                        L_NOMAT ((*nomat_desc), guard_body)[2] = POPSTACK(S_a);
                     }
                                                               

                    /* Beseitigen des Inkarnationsblocks fuer die pi-Abstraktion */
                     if (T_DOLLAR (READSTACK (S_m)))
                     {
                          POP_I_BLOCK (hilf);
                          POPSTACK (S_m); /* das T_DOLLAR */
                     }



                 /* Argument wegschreiben und Entfernen des Applikators */
        
                    L_NOMAT ((*nomat_desc), guard_body)[3] = POPSTACK(S_a);

                    arity = READ_ARITY(S_m1);
                           
                    if (  arity == 2 ) 
                       {
                         POPSTACK(S_m); 
                         POPSTACK(S_m1); 
                       } 
                    else                              
                     /* Ueberversorgung der case-Funktion mit Argumenten */
                         WRITE_ARITY(S_m1, arity - 1 );
                               
 

                     /* schliessen des nomatch und nomatch auf den A-Stack*/
                             


                    if (SIZEOFSTACK_TILDE > 0)
                    {
                       PUSHSTACK(S_e, nomat_desc);                                 
                       if ( ( x = st_clos(0,0) ) == NULL )
                           post_mortem("pmredsw: Heap out of space ");
                       else
                           PUSHSTACK(S_a,x);
                    }
                    else
                       PUSHSTACK(S_a, nomat_desc);                                 


                     END_MODUL("pmwhenmat_2");

                     /*weiter auf der M-Linie des ear*/
                     return(TRUE) ;

}/*ende switch*/
}

/*********************************************************************/
/*-------------------------------------------------------------------*/
/*pmredwhen - steuert den Vergleich der pattern mit dem Argument     */
/*                  und ruft dabei das Programm pmwhenmat auf.       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*********************************************************************/

BOOLEAN pmredwhen (desc, arg, i_ma)

PTR_DESCRIPTOR desc;

STACKELEM arg;

int     i_ma;                             /* zeigt Regel, die gematcht wird */

{


    STACKELEM x;
    STACKELEM hilf;

   int anz_fv;
   int     pm_ta = 0;
   STACKELEM ex =0;
   STACKELEM body =0;
   PTR_DESCRIPTOR nomat_desc,result;
   int reg_nwhen;
   int i,arity ; 

    START_MODUL ("pmredwhen");
    /* redcnt vorsorglich schon hier decr. */
    /* wird incr. falls pmwhenmat UNDECIDED liefert*/
    /* oder keines der Pattern passt und es keinen*/
    /* otherwise Ausdruck gibt                    */
    /*if(!(T_SYSAP(READSTACK(S_m))))*/
    _redcnt--;
/*------------------------------------------------------------------------*/

       /* Aufruf von pm_am */
       /* Seiteneffekt von pmmat : Setzen der Werte fuer       */
       /* pm_ta und ex                                         */
	pmwhenmat (desc, arg , &pm_ta, &ex, &body);
/*------------------------------------------------------------------------*/

        switch (pm_ta)
        {

              
           case GUARD_AUSRECHNEN:

           /*Kontrollsymbol auf den M-Stack*/
           PUSHSTACK(S_m, SET_ARITY( PM_GUARD, 0));
           
           /* Rumpf des guard ist in pmwhenmat auf den Stack e gelegt worden*/

           END_MODUL("pmredwhen");

           /* Rueckgabe der Kontrolle an den EAR */
           /* weiter auf den E linie             */
           return (FALSE);



            case SUCCESS:          /* das pattern passt  */
                                   /* guard ist true     */

        /*Rumpf des when ist in pmwhenmat auf den*/
        /*E-Stack geladen worden */
                           
                 if (T_DOLLAR(READSTACK(S_m)))
                 {
                     POPSTACK(S_m); 
                     arity = 1;    
                 }   
                 else
                     arity = 0;
 

        /* Applikator   des ap auf den M-Stacks entfernen        */
        /* und wegpoppen des Arguments weil die Reduktion klappt */

                 if (T_AP_2
                          (READSTACK (S_m1)))
                 {
                     PPOPSTACK (S_m);
                     PPOPSTACK (S_m1);
                 }
                 else
                     WRITE_ARITY (S_m1, READ_ARITY (S_m1) - 1);

                 if (arity == 1)
                 {
                    for (arity = 0; T_AP_1(READSTACK(S_m));  arity++) 
                        POPSTACK(S_m);

                    PUSHSTACK(S_m,DOLLAR);

                    for ( ; arity > 0; arity-- )
                        PUSHSTACK(S_m,AP_1);

                 }

                 if (T_POINTER (READSTACK (S_a)))
                 {
                     register    PTR_DESCRIPTOR x;
                     x = ((PTR_DESCRIPTOR) POPSTACK (S_a));
                     DEC_REFCNT (x);
                 }
                 else if (T_ATOM(READSTACK(S_a)))
                               POPSTACK(S_a);
        else
        post_mortem("pmredwhen: Argument fuer das when war kein Single-Atom oder Pointer");

        /* Der Match-Deskriptor wird schon in pmwhenmat */
        /* vom E-Stack genommen, damit dort auch        */
        /* der Rumpf geladen werden kann                */

                 END_MODUL ("pmredwhen");
                 return(FALSE); /* weiter auf e, Body ist */
                                /* nicht reduziert        */

            case FAILURE:

            case UNDECIDED:
                  
                 /*Match-Deskriptor vom Stack nehmen*/
                 PPOPSTACK(S_e);

                if(!(T_SYSAP(READSTACK(S_m))))
                    _redcnt++;


                          if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmredwhen: Heap out of Space");
                 DESC_MASK (nomat_desc, 1,  C_EXPRESSION, TY_NOMAT);

                   
                   
                 /* kopieren des Match-Deskriptors */
                   L_NOMAT((*nomat_desc),ptsdes)=(T_PTD)desc;
                 
                 /* Increment Refcnt fuer Match-Deskriptor nicht noetig, */
                 /* da der Pointer vom Stack genommen wird               */
                 /* und es stattdessen eine Referenz vom neu             */
                 /* angelegten Nomatchdescr. auf den Match-Deskriptor    */
                 /* gibt                                                 */

                 L_NOMAT ((*nomat_desc), act_nomat) = -1 ;

                 if (pm_ta == FAILURE)
                     L_NOMAT ((*nomat_desc), reason) = 1 ;
                 else                                   
                    /* also UNDECIDED */
                     L_NOMAT ((*nomat_desc), reason) = 2 ;

                                         
                /*Platz besorgen*/
                 if (newheap( 4,A_NOMAT ((*nomat_desc), guard_body)) == 0) 
                 post_mortem("pmred-nomat: es konnte kein neuer switchdescr angelegt werden ");


                /*guard-body ausfuellen                       */
                /*Dazu die Laenge wegschreiben                */
                 L_NOMAT ((*nomat_desc), guard_body)[0] = 3;

                /* da das Matching nicht geklappt hat,bleiben der guard- */
                /* und der body-Ausdruck unveraendert.                       */ 

                 L_NOMAT ((*nomat_desc), guard_body)[1] = NULL;
                 L_NOMAT ((*nomat_desc), guard_body)[2] = NULL;
                                
                 /* Inkarnationsblock entfernen: */
                 if (T_DOLLAR(READSTACK(S_m)))
                 {
                   POPSTACK(S_m); /* $ */
                   POP_I_BLOCK(hilf);
                 }

                 /* Argument wegschreiben und Entfernen des Applikators */
          
                 L_NOMAT ((*nomat_desc), guard_body)[3] = POPSTACK(S_a);

                 arity = READ_ARITY(S_m1);
                            
                 if (  arity == 2 ) 
                    {
                      POPSTACK(S_m); 
                      POPSTACK(S_m1); 
                    } 
                 else                              
                 /* Ueberversorgung der case-Funktion mit Argumenten */
                      WRITE_ARITY(S_m1, arity - 1 );
                               
                 

                 /* schliessen des nomatch */


                  if (SIZEOFSTACK_TILDE > 0)
                  {
                    PUSHSTACK(S_e, nomat_desc);
                    if ( ( x = st_clos(0,0) ) == NULL )
                        post_mortem("pmredsw: Heap out of space ");
                    else
                        PUSHSTACK(S_a,x);
                  }
                  else
                    PUSHSTACK(S_a, nomat_desc);


                 END_MODUL ("pmredwhen");
                 return(TRUE); /* weiter auf m*/

           default:
              post_mortem("pmredwhen: pmwhenmat liefert weder FAILURE \
                             SUCCESS noch UNDECIDED ");
        }/* ende switch */


            END_MODUL ("pmredwhen");
            return(TRUE);  /* weiter auf m */
}

