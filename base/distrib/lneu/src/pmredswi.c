


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
extern  STACKELEM st_expr ();             /* rstpro.c */
extern  STACKELEM st_clos ();             /* rstpro.c */
extern int  mvheap ();                    /* mvheap:c */
extern int  _redcnt;                      /* state:c - der Reduktionszae hler*/
extern  BOOLEAN _beta_count_only;      /* <r> */

/*---------------------------------------------------------------*/
/*  Schnittstelle zwischen pmmat und pmredswi                    */
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

#if !EAGER
void pm_start_eval_list (list_des)
PTR_DESCRIPTOR list_des;
{
STACKELEM h;

   /*Listendeskriptor auf den E-Stack retten */
   PUSHSTACK ( S_e, list_des);

   /*Listenelement laden*/
    h =  R_LIST((*list_des), ptdv)[0];

   /*--Einen Expressiondeskriptor zum Auswerten wieder Einlagern--*/
   if (  (T_POINTER( h))
       && ( (R_DESC ((*(PTR_DESCRIPTOR) h), type)) == TY_EXPR)
      )
   {
      /*Einlagern von Expression*/
      load_expr((STACKELEM **)A_EXPR((* (PTR_DESCRIPTOR) h),pte));

     /*Aufloesen des Expressiondeskriptors*/
      DEC_REFCNT((PTR_DESCRIPTOR) h);
   }
   else
   {
     /*Den Inhalt der Liste auf den Stack*/
     PUSHSTACK(S_e, h);
   }
   /*Trennsymbol auf den M-Stack */
     PUSHSTACK(S_m, SET_ARITY( PM_EAGER, 0));
    
     return;
}

#endif

/*********************************************************************/
/*-------------------------------------------------------------------*/
/*pmmat - steuert den Vergleich eines pattern mit dem Argument       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*********************************************************************/

#if !EAGER
void pmemat (mat_desc, arg , ppm_ta, pex, pbody)


PTR_DESCRIPTOR mat_desc;

STACKELEM arg;

int *ppm_ta;
STACKELEM *pex;
STACKELEM *pbody;
#else
void pmmat (mat_desc, arg , ppm_ta, pex, pbody)


PTR_DESCRIPTOR mat_desc;

STACKELEM arg;

int *ppm_ta;
STACKELEM *pex;
STACKELEM *pbody;
#endif
{
    int     i;                            /* Laufvariable */
    STACKELEM h;
    int     snap_arity;
    int     size;
    PTR_DESCRIPTOR   swi_desc;
    STACKELEM x;
 /* zum Transport der Variablen vom Pm-stack auf den I-stack */
    STACKELEM hilf;


    int bool ;
    int     save_redcnt;         /* zum Retten des redcounters bei guard-Behandlung */
    int oldredcnt;   
    PTR_HEAPELEM bcode;

    START_MODUL ("pmmat");


    /* Heap reservieren wegen Parameter code. Der Heap wird */
    /* wieder freigegeben in pm_am                          */
    RES_HEAP;
    bool = pm_am ((R_MATCH ((*mat_desc), code)), arg);

 /* Aufruf von pm_am mit verschiedenen Matchcodes bis bool anzeigt, */
 /* dass ein Pattern passt                                          */
    switch (bool)
    {



/*********************************************************************/
/* -----------------   P A T T E R N       P A S S T         --------*/
/*********************************************************************/


        case PM_PASST:             /* das pattern passt  */

    
           /*Testen ob der Guard nur aus einem true besteht*/
           if (R_MATCH((*mat_desc),guard) == NULL)
            
              /*---------------------------------------------------*/
              /*Guard besteht nur aus einem True, Laden des Rumpfes*/
              /*kann direkt erfolgen, ohne die Kontrolle an den    */
              /*ear abzugeben                                      */
              /*---------------------------------------------------*/
              {
                   /*Tag setzen fuer pmredsw            */
                   *ppm_ta = SUCCESS;

#if EAGER
                   /*Switchdeskriptor vom Stack nehmen */
                     swi_desc = (PTR_DESCRIPTOR) POPSTACK(S_e);
#endif
   
                   /* body des when auf den E-stack   */
                        load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));

#if EAGER
                   /* Switchdescr. freigeben, Reduktion hat geklappt */
                      DEC_REFCNT (swi_desc);
#endif

#if !EAGER
               /*--Parameter setzen die pmredeswi in den Listendeskriptor--*/
               /*--der Ergebnisliste schreibt                            --*/ 

               /*Tag setzen*/
               *ppm_ta = SUCCESS;
    
               /*Guard setzen*/
               *pex    = SA_TRUE;
   
               /*--Rumpfausdruck setzen--*/
   
                  /*zum Auslagern auf den A-Stack traversieren*/
                  trav(&S_e, &S_a);
           
                  /*Ausdruck auslagern*/
                  if ((x = st_expr ()) != 0)
                  {
                    PUSHSTACK(S_a, x);
   
                  /*Rumpf wegschreiben */
                   *pbody = POPSTACK(S_a);
                  }  
                  else
                     post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
#endif
                END_MODUL("pmmat");
                return ;
           }
           else
           /*------------------------------------------------*/
           /*Der Guard Ausdruck muss erst ausgerechnet werden*/
           /*------------------------------------------------*/
           {

            /* guard auf den E-stack   */
            load_expr ((STACKELEM *) A_MATCH ((*mat_desc), guard));


#if EAGER
            *ppm_ta = GUARD_AUSRECHNEN;
#else
            /*--Parameter setzen die pmredeswi in den Listendeskriptor--*/
            /*--der Ergebnisliste schreibt                            --*/ 

            /*Tag setzen*/
            *ppm_ta = SUCCESS;
 

            /*--Guardausdruck setzen--*/

               /*zum Auslagern auf den A-Stack traversieren*/
               trav(&S_e, &S_a);
        
               /*Ausdruck auslagern*/
               if ((x = st_expr ()) != 0)
               {
                 PUSHSTACK(S_a, x);

               /*Guard wegschreiben */
                *pex = POPSTACK(S_a);
               }  
               else
                 post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
            
            /*--Rumpfausdruck setzen---*/

            
            /* Beseitigen des Inkarnationsblocks fuer den Guard   */
                     if (T_DOLLAR (READSTACK (S_m)))
                     {
                          POP_I_BLOCK (hilf);
                          POPSTACK (S_m); /* das T_DOLLAR */
                     }


            /* body des when auf den E-stack   */
                     load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));


            /*--Rumpfausdruck setzen--*/

               /*zum Auslagern auf den A-Stack traversieren*/
               trav(&S_e, &S_a);
        
               /*Ausdruck auslagern*/
               if ((x = st_expr ()) != 0)
               {
                 /*Umgebung retten*/
                 PUSHSTACK(S_a, x);

               /*Rumpf wegschreiben */
                *pbody = POPSTACK(S_a);
               }  
               else
                 post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
#endif

            END_MODUL("pmmat"); 
            
            return;

       }/*ende des Ausrechnen des Guard-Ausdrucks*/       


/*********************************************************************/
/*  P A T T E R N    E N T H A E L T  K O N S T A N T E  O D E R     */
/*  A P P L I C A T I O N  D I E   G E G E N   V A R I A B L E       */
/*  G E M A T C H T    W U R D E                                     */
/*********************************************************************/

        case PM_UNDEC:
        /* im Argument steht eine Variable, die gegen eine Konstante oder */
        /* Listenkonstruktor gematcht wurde                               */
            *ppm_ta = UNDECIDED;
            *pex = 0;

            END_MODUL ("pmmat");

            break;





/*********************************************************************/
/* ---------   P A T T E R N       P A S S T     N I C H T   --------*/
/*********************************************************************/

        default:                   /* weder PM_PASST noch PM_UNDEC */
                                   /* also PM_FALSE                */
            *ppm_ta = FAILURE;
            END_MODUL("pmmat");
            return ;
    }

}




/********************************************************************/
/*------------------------------------------------------------------*/
/*pmmat_2 nimmt die Behandlung des Case nach Ausrechnen des Guard-  */
/*Ausdrucks wieder auf. pmmat_2 analysiert  Stacktop des Stack A    */
/*und ruft im Falle eines Stackelements False wieder der pmredswi   */
/*auf                                                               */
/*------------------------------------------------------------------*/
/********************************************************************/

#if EAGER

BOOLEAN pmmat_2()
{
STACKELEM bool;
PTR_DESCRIPTOR mat_desc;
PTR_DESCRIPTOR swi_desc;
PTR_DESCRIPTOR nomat_desc;
int regel;
int  i,arity;                            /* Laufvariable */
STACKELEM h;
int     snap_arity;
int    anz_fv,size;
STACKELEM x,y;
STACKELEM hilf;

START_MODUL("pmmat_2");

/*die Regel feststelle, deren Guard-Ausdruck zu true*/
/*ausgewertet wurde.Auf dem M-stacl liegt ein PM_GUARD*/
    regel =  ARITY(POPSTACK(S_m));

/*der Wert des Ausgerechneten Guard-Ausdrucks       */
bool = POPSTACK(S_a);


switch ( bool & ~F_EDIT)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  -----       g u a r d  i s t  t r u e             --------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 case SA_TRUE:


            /*Referenz auf die Case-Funktion vom Stack e nehmen*/
            /*und nach swi_desc schreiben                      */
            swi_desc = (PTR_DESCRIPTOR) POPSTACK(S_e);

            /*matchdeskriptor feststellen                         */
             mat_desc = (PTR_DESCRIPTOR) (R_SWITCH ((*swi_desc), ptse))[regel];

            /* body des when auf den E-stack   */
                     load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));


                   /* Beseitigen des Arguments fuer die Case-Funktion*/
                   if (T_POINTER (READSTACK (S_a)))
                      {
                       register    PTR_DESCRIPTOR x;
                       x = ((PTR_DESCRIPTOR) POPSTACK (S_a));
                       DEC_REFCNT (x);
                      }
                   else if (T_ATOM(READSTACK(S_a)))
                           POPSTACK(S_a);
                        else
                           post_mortem("pmredswi: Argument fuer das Switch war kein Single-Atom oder Pointer");
                   
                  /* Der Inkarnationsblock wird noch bei der Reduktion des */
                  /* body-Ausdruckes benoetigt.                            */
                  
                     /* NOMATCH muss geschlossen werden */
                  if (T_DOLLAR(READSTACK(S_m)))
                  {
                     POPSTACK(S_m);
                     arity = 1;
                  }   
                  else
                     arity = 0;

                 /* Beseitigen des Applikators fuer die Case-Funktion */

                 if (T_AP_2(READSTACK (S_m1)))
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
                   /*Reduktion hat geklappt, Switchdeskriptor swi_desc*/
                   /*beseitigen                                       */
                     DEC_REFCNT((PTR_DESCRIPTOR) swi_desc);

                     END_MODUL("pmmat_2");

                     /*weiter auf dem e-Stack; Ausrechnen des Rumpfausdrucks*/
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


                     END_MODUL("pmmat_2");

                     /*Rueckgabe des Wertes von pmredsw an den ear wie bei*/
                     /*einem direkten Aufruf des pmredsw aus dem ear      */
                     return(pmredsw( READSTACK(S_e), READSTACK(S_a), regel+1));       





/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* --- g u a r d  i s t  w e d e r  t r u e  n o c h  f a l s e  --*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 default:

                   /*Nomatchdeskriptor anlegen*/
                     if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmredsw: Heap out of Space");
                     DESC_MASK (nomat_desc, 1,  C_EXPRESSION, TY_NOMAT);
           
                    /*Platz besorgen*/
                     if (newheap( 4,A_NOMAT ((*nomat_desc), guard_body)) == 0) 
                     post_mortem("pmred-nomat: es konnte kein neuer switchdescr angelegt werden ");

                    /*Referenz auf die Case-Funktion vom Stack e nehmen*/
                    /*und nach swi_desc schreiben                      */
                      swi_desc = (PTR_DESCRIPTOR) POPSTACK(S_e);

                    /* Matchdeskriptor feststellen                     */
                      mat_desc = (PTR_DESCRIPTOR) (R_SWITCH ((*swi_desc), ptse))[regel];

                    /* kopieren des switchdesciptors */
                     L_NOMAT((*nomat_desc),ptsdes)=(T_PTD)swi_desc;

                    /* eintragen der Zahl der ueberprueften whens */
                     L_NOMAT ((*nomat_desc), act_nomat) = regel ;

                   /* eintragen,aus welchem grund der Deskriptor angelegt wird: */
                   /* reason = 3 oder reason = 4 bedeutet,dass der guard weder  */
                   /* zu TRUE noch zu FALSE reduziert wurde.                    */
                   /* Es wird eine 4 eingetragen,falls der Reduktionszaehler    */
                   /* abgelaufen ist,eine 3, falls eine weitere Reduktion des   */
                   /* Ausdruckes in keinem Fall mehr moeglich ist.              */
                     
                     if ( _redcnt == 0 )
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
                              post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
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
                              post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
                     }
                     else
                     {
                            /*body in den nomatchdescr. wegschreiben*/
	                        L_NOMAT ((*nomat_desc), guard_body)[2] = POPSTACK(S_a);
                     }                                         


                    /* Beseitigen des Inkarnationsblocks  */
                     if (T_DOLLAR (READSTACK (S_m)))
                     {
                          POP_I_BLOCK (hilf);
                          POPSTACK (S_m);   /* $ */
                     }                             

                     /* Argument wegschreiben und korrigieren der Applikatoren */
          
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
                     /* NOMATCH muss geschlossen werden */
                      {  
                        PUSHSTACK(S_e, nomat_desc);
                        if ( ( x = st_clos(0,0) ) == NULL )
                            post_mortem("pmredsw: Heap out of space ");
                        else
                            PUSHSTACK(S_a,x);
                      }                                   
                      else 
                      PUSHSTACK(S_a, nomat_desc);
                      

                     END_MODUL("pmmat_2");

                     /*weiter auf der M-Linie des ear*/
                     return(TRUE) ;

}/*ende switch*/
}
#else
BOOLEAN pm_eval_list()
{
PTR_DESCRIPTOR list_des;
STACKELEM h;
int pm_dim;

START_MODUL("pm_eval_list");

/*Listendeskriptor in die Variable Schreiben*/
list_des = (PTR_DESCRIPTOR) READSTACK(S_e);

/*Stelligkeit des PM_GUARD*/
pm_dim = ARITY( POPSTACK(S_m));

/*Ergebnis wegschreiben*/
L_LIST ((*list_des), ptdv)[pm_dim] = POPSTACK(S_a);

/*gibt es noch ein weiteres Listenelement?*/
if ( (R_LIST( (* list_des), dim)) > (pm_dim + 1) )
{
      /*naechstes Listenelement laden*/
        h =  R_LIST((*list_des), ptdv)[pm_dim+1];

       /*--Einen Expressiondeskriptor zum Auswerten wieder Einlagern--*/
        if (  (T_POINTER( h))
            && ( (R_DESC ((*(PTR_DESCRIPTOR) h), type)) == TY_EXPR)
           )
        {
           /*Einlagern von Expression*/
             load_expr((STACKELEM **)A_EXPR((* (PTR_DESCRIPTOR) h),pte));

           /*Aufloesen des Expressiondeskriptors*/
             DEC_REFCNT((PTR_DESCRIPTOR) h);
        }
        else
        {
          /*Den Inhalt des Listenelements auf den Stack*/
             PUSHSTACK(S_e, h);
        }

   /*Trennsymbol auf den M-Stack */
     PUSHSTACK(S_m, SET_ARITY( PM_EAGER, pm_dim + 1));

    END_MODUL("pm_eval_list");

    /*Kontrolle zurueck auf den E-Stack*/
    return(FALSE);
}
else
{
   /*Deskriptor vom E-Stack nehmen*/
   POPSTACK(S_e);

   /*Als Resultat list_des auf den A-Stack legen*/
   PUSHSTACK(S_a,list_des);

   END_MODUL("pm_eval_list");

   /*Kontrolle zurueck auf die M-Linie*/
   return(TRUE);
}

}


#endif


/*********************************************************************/
/*-------------------------------------------------------------------*/
/*pmredswi - steuert den Vergleich der pattern mit dem Argument      */
/*                  und ruft dabei das Programm pmmat auf.           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*********************************************************************/

#if EAGER
BOOLEAN pmredsw (swi_desc, arg, i_ma)
#else
BOOLEAN pmredesw (swi_desc, arg, i_ma)
#endif

PTR_DESCRIPTOR swi_desc;

STACKELEM arg;

int     i_ma;  /* Regel, die gematcht wird */

{


    STACKELEM x,y;
    STACKELEM hilf;

    PTR_DESCRIPTOR nomat_desc;
    PTR_DESCRIPTOR mat_desc;


   int anz_fv,arity;
   int     pm_ta = 0;
   STACKELEM ex =0;
   STACKELEM body =0;
   PTR_DESCRIPTOR result;
   int reg_nwhen;
   int i ; 

    START_MODUL ("pmredswi");
    /* redcnt vorsorglich schon hier decr. */
    /* wird incr. falls pmmat UNDECIDED liefert*/
    /* oder keines der Pattern passt und es keinen*/
    /* otherwise Ausdruck gibt                    */
    /*if(!(T_SYSAP(READSTACK(S_m))))*/
    _redcnt--;
/*------------------------------------------------------------------------*/
#if EAGER
    while (i_ma < R_SWITCH ((*swi_desc), nwhen))
    {
        mat_desc = (PTR_DESCRIPTOR) (R_SWITCH ((*swi_desc), ptse)[i_ma]);

       /* Aufruf von pm_am mit verschiedenen Matchdeskriptoren */
       /* Seiteneffekt von pmmat : Setzen der Werte fuer       */
       /* pm_ta und ex                                         */
	pmmat (mat_desc, arg , &pm_ta, &ex, &body);
#else

START_MODUL("EAGER");
reg_nwhen = (R_SWITCH((*swi_desc), nwhen));
       if (( result = (PTR_DESCRIPTOR)newdesc() ) == 0)
         post_mortem(" rredswi: kein Platz fuer Anlegen eines Decr.");
       if ( newheap( 3 *(reg_nwhen - 1) , A_LIST((*result), ptdv)) == 0 )
         post_mortem(" rredswi: kein Platz fuer Listdescr.");
       LI_DESC_MASK( result, 1, C_LIST, TY_UNDEF);
       L_LIST((*result),dim) =(3 *(reg_nwhen-1));
        
for( i= i_ma; i <reg_nwhen; i++)
{
   mat_desc = (PTR_DESCRIPTOR) (R_SWITCH ((*swi_desc), ptse)[i]);
#define INDEX(x) (3*((x) - 1))
        L_LIST((*result), ptdv)[ INDEX(i)] = (T_HEAPELEM)mat_desc ;;
        L_LIST((*result), ptdv)[ INDEX(i)+1 ] = arg;
        L_LIST((*result), ptdv)[ INDEX(i)+2 ] = SA_TRUE ;
}   

#if DEBUG
PUSHSTACK(S_e,result);
POPSTACK(S_e);
#endif 

for( i= i_ma; i < reg_nwhen; i++)
{
        PTR_DESCRIPTOR mat_desc ;

        STACKELEM ex=SA_TRUE ;
        STACKELEM body=SA_TRUE ;
        STACKELEM arg ;

        int pm_ta = 0;
        mat_desc = (PTR_DESCRIPTOR) R_LIST((*result), ptdv)[INDEX(i)];
        arg = (STACKELEM) R_LIST((*result), ptdv)[INDEX(i)+1];
    	pmemat (mat_desc, arg , &pm_ta, &ex, &body);

        L_LIST((*result), ptdv)[ INDEX(i) ] = pm_ta;
        L_LIST((*result), ptdv)[ INDEX(i)+1 ] = ex;
        L_LIST((*result), ptdv)[ INDEX(i)+2 ] = body;
}
END_MODUL("EAGER");

/* im Falle der eager Evaluation nur einen Listendeskriptor abliefern, der Rest des code*/
/* wird nicht mehr gebraucht.                                                           */  

        /* Applikator   des ap auf den m-stacks entfernen
           und wegpoppen des Arguments weil die Reduktion klappt*/
                 if (T_AP_2(READSTACK (S_m1)))
                 {
                     PPOPSTACK (S_m);
                     PPOPSTACK (S_m1);

                 }
                 else
                   WRITE_ARITY (S_m1, READ_ARITY (S_m1) - 1);
        


         /* Argument abraeumen*/ 
                 if (T_POINTER (READSTACK (S_a)))
                 {
                     register    PTR_DESCRIPTOR x;
                     x = ((PTR_DESCRIPTOR) POPSTACK (S_a));
                     DEC_REFCNT (x);
                 }
                 else if (T_ATOM(READSTACK(S_a)))
                               POPSTACK(S_a);
        else
        post_mortem("pmredswi: Argument fuer das Switch war kein Single-Atom oder Pointer");

        /* Reduktion hat geklappt, Switchdescr. freigeben */
        /* und vom Stack nehmen                           */
                   PPOPSTACK(S_e); /*swidesc*/
                   DEC_REFCNT (swi_desc);


/*Bereitet das Auswerten der Guard und Rumpfausdrueck vor*/
pm_start_eval_list(result);

END_MODUL ("pmredswi");
return( FALSE); /*weiter auf dem e-stack des ear*/
#endif
#if EAGER
/*------------------------------------------------------------------------*/

        switch (pm_ta)
        {

              
           case GUARD_AUSRECHNEN:

           /*Kontrollsymbol auf den M-Stack*/
           PUSHSTACK(S_m, SET_ARITY( PM_GUARD, i_ma));
           
           /* Rumpf des guard ist in pmmat auf den Stack e gelegt worden*/

           END_MODUL("pmredswi");

           /* Rueckgabe der Kontrolle an den EAR */
           /* weiter auf den E linie             */
           return (FALSE);



            case SUCCESS:          /* das pattern passt  */
        /* guard ist true     */

        /*Rumpf des entsprechenden whens ist in pmmat auf den*/
        /*E-Stack geladen worden; jedenfalls im nicht Eager  */
        /*Fall                                               */    



                  /* Der Inkarnationsblock wird noch bei der Reduktion des */
                  /* body-Ausdruckes benoetigt.                            */
                  
                 if (T_DOLLAR(READSTACK(S_m)))
                 {
                     POPSTACK(S_m); 
                     arity = 1;    
                 }   
                 else
                     arity = 0;
 
                 /* Applikator   des ap auf den m-stacks entfernen        */
                 /* und wegpoppen des Arguments weil die Reduktion klappt */
           
                 if (T_AP_2(READSTACK (S_m1)))
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
        post_mortem("pmredswi: Argument fuer das Switch war kein Single-Atom oder Pointer");

        /*Der Switchdeskriptor wird schon in pmmat */
        /*vom E-Stack genommen, damit dort auch    */
        /*der Rumpf geladen werden kann            */
              
          
 
                 END_MODUL ("pmredswi");
                 return(FALSE); /* weiter auf e, Body ist */
                               /* nicht reduziert        */

            case FAILURE:
                 i_ma++;
                 break;

            case UNDECIDED:
                  
                 /*Switchdeskiptor vom Stack nehmen*/
                 PPOPSTACK(S_e);

                if(!(T_SYSAP(READSTACK(S_m))))
                    _redcnt++;


                          if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmredsw: Heap out of Space");
                 DESC_MASK (nomat_desc, 1,  C_EXPRESSION, TY_NOMAT);

                   
                   

                 /* kopieren des switchdesciptors */
                 L_NOMAT((*nomat_desc),ptsdes)=(T_PTD)swi_desc;
                 
                 /* Increment Refcnt fuer switch nicht noetig*/
                 /* da der Pointer vom Stack genommen wird   */
                 /* und es stattdessen eine Referenz vom neu */
                 /* angelegten Nomatchdescr. auf den Switchd.*/
                 /* gibt                                     */

                 /* eintragen der Zahl der ueberprueften whens */
                 L_NOMAT ((*nomat_desc), act_nomat) = i_ma ;

                                                    
                 /* eintragen,warum der Deskriptor angelegt wird */
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

                                                                          
                 /* Inkarnationsblock entfernen */
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
                               


                 /* schliessen des nomatch und nomatch auf den A-Stack*/
                            
                 /* NOMATCH muss geschlossen werden */


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

                 END_MODUL ("pmredswi");
                 return(TRUE); /* weiter auf m*/

           default:
              post_mortem("pmredsw: pmmat liefert weder FAILURE \
                             SUCCESS noch UNDECIDED ");
        }/* ende switch */


    }/*ende  while*/



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*         keine der Regeln darf angewendet werden              */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* samtliche pattern wurden getestet, keines passt  Behandlung des */
 /* otherwise oder Wiederherstellung des pattern                    */

        /*Switchdeskriptor vom Stack nehmen*/
          PPOPSTACK(S_e);

        mat_desc = (PTR_DESCRIPTOR) R_SWITCH ((*swi_desc), ptse)[i_ma];

        if (mat_desc == NULL)

        {                          /* es gibt keine otherwise-expr */

                 /*if(!(T_SYSAP(READSTACK(S_m))))*/
                      _redcnt++;

        if ((nomat_desc = (PTR_DESCRIPTOR) newdesc ()) == 0) /* Anlegen eines */
            post_mortem ("pm_st_switch: Heap out of Space"); /* Deskriptors vom */
                                                             /* Typ nomatch, der */
        DESC_MASK (nomat_desc, 1, C_EXPRESSION, TY_NOMAT);   /* den urspruenglichen */
                                                             /* Switch Deskriptor */


        L_NOMAT ((*nomat_desc), ptsdes) = (T_PTD)swi_desc;
        /* kein Increment fuer swi_desc, da swi_desc vom */
        /* Stack genommen wird                           */
        L_NOMAT ((*nomat_desc), act_nomat) = 0;

        L_NOMAT ((*nomat_desc), reason) = 1;
         
       /*Platz besorgen*/
        if (newheap( 4,A_NOMAT ((*nomat_desc), guard_body)) == 0) 
        post_mortem("pmred-nomat: es konnte kein neuer switchdescr angelegt werden ");


        /*guard-body ausfuellen                       */
        /*Dazu die Laenge wegschreiben                */
        L_NOMAT ((*nomat_desc), guard_body)[0] = 3;


        /* da das Matching nicht geklappt hat,bleiben der guard- */
        /* und der body-Ausdruck unveraendert.                   */ 

        L_NOMAT ((*nomat_desc), guard_body)[1] = NULL;
        L_NOMAT ((*nomat_desc), guard_body)[2] = NULL;


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


         END_MODUL ("pmredswi");
         return(TRUE);  /* weiter auf m */
        }
        else                       /* otherwise expression */
        {



            load_expr ((STACKELEM *) A_MATCH ((*mat_desc), body));


/* Berichtigung des ap auf den m-stacks und wegpoppen
des Arguments weil   die Reduktion klappt
 dh. otherwise Ausdruck das Ergebnis ist.        */
            if (T_AP_2
                     (READSTACK (S_m1)))
            {
                 PPOPSTACK (S_m);
                 PPOPSTACK (S_m1);
            }
            else
            {
                 WRITE_ARITY (S_m1, READ_ARITY (S_m1) - 1);
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
        post_mortem("pmredswi: Argument fuer das Switch war kein Single-Atom");
    /* Argument des switch vom A-stack und decr. */

            DEC_REFCNT (swi_desc);

    /* Reduktion hat geklappt, Switchdescr. freigeben */


            END_MODUL ("pmredswi");
            return(FALSE);
        }
#endif
}









