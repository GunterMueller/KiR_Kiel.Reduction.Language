/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */

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


/*---------------------------------------------------------------------
 * Allgemein verwendete externe Variablen und Funktionen
 *--------------------------------------------------------------------
 */


extern  STACKELEM st_expr ();             /* rstpro.c */
extern  STACKELEM st_clos ();             /* rstpro.c */
extern  BOOLEAN pmredsw ();               /* pmredswi.c */

extern int  mvheap ();                    /* mvheap:c */
extern PTR_DESCRIPTOR newdesc ();

extern int  _redcnt;                      /* state:c - der Reduktionszae hler
                                             */


/*-------------------------------------------------------------------*/
/*pm_red_nomat -  rechnet guard deskriptor des nomatch aus.          */
/*                guard UNDEC --> neuer Matchdeskriptor              */
/*                guard TRUE-- > laden des body der regel            */
/*                guard FALSE-- > aufruf von pmredsw fuer die        */
/*                               restlichen auszuprobierenden Regeln */
/*-------------------------------------------------------------------*/


BOOLEAN pm_red_nomat ()


{
    PTR_DESCRIPTOR n_nomat_desc;
    PTR_DESCRIPTOR nomat_desc;
    PTR_DESCRIPTOR swi_desc;
    PTR_DESCRIPTOR mat_desc;
    PTR_HEAPELEM hilf;
    int     anz_fv;




    START_MODUL ("pm_red_nomat");

    /*nomat_desc feststellen*/
    nomat_desc = (PTR_DESCRIPTOR) READSTACK(S_e);

    /*guard-expr holen*/
    hilf = (PTR_HEAPELEM) R_NOMAT ((*(PTR_DESCRIPTOR)nomat_desc), guard_body);
      PUSHSTACK(S_e, (STACKELEM) (hilf[1]));                      

    /*einlagern, falls der guard-ausdruck hinter einem*/
    /*expressionpointer weggeschrieben werden musste  */
     if  (R_DESC((*(PTR_DESCRIPTOR)(hilf[1])), type) == TY_EXPR)
     {
              /* den Pointer auf die Expression beseitigen */
              POPSTACK (S_e); 

              /* guard-expr laden */
              load_expr ((STACKELEM *) A_EXPR ((*(PTR_DESCRIPTOR)(hilf[1])), pte));
     }

    /*Kontrollsymbol auf den M-Stack*/
    PUSHSTACK(S_m, SET_ARITY(PM_NOMAT, (R_NOMAT ((*nomat_desc), act_nomat ))));
    
    END_MODUL ("pm_red_nomat");

   /*Weiter auf dem E-Stack des ear*/
    return(FALSE);
}

/*********************************************************************************/
/*-------------------------------------------------------------------------------*/
/*pm_nomat_2
/*-------------------------------------------------------------------------------*/
/*********************************************************************************/

BOOLEAN pm_nomat_2()
{
STACKELEM bool;
PTR_DESCRIPTOR mat_desc;
PTR_DESCRIPTOR swi_desc;
PTR_DESCRIPTOR nomat_desc;
PTR_DESCRIPTOR new_nm_desc;
int regel;
int  i;                            /* Laufvariable */
STACKELEM h;
int     snap_arity;
int     size;
STACKELEM x;
STACKELEM hilf;
PTR_HEAPELEM g_b;

START_MODUL("pm_nomat_2");

/* die Regel feststellen, deren Guard-Ausdruck          */
/* ausgewertet wurde.Auf dem M-stack liegt ein PM_NOMAT */
regel =  ARITY(POPSTACK(S_m));

/*Referenz auf den Nomat_desc vom Stack e nehmen   */
/*und nach nomat_desc schreiben                    */
nomat_desc = (PTR_DESCRIPTOR) POPSTACK(S_e);

/*der Wert des Ausgerechneten Guard-Ausdrucks       */
bool = POPSTACK(S_a);


switch ( bool & ~F_EDIT)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  -----       g u a r d  i s t  t r u e             --------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 case SA_TRUE:



            /* Beseitigen des Arguments fuer die Case-Funktion*/
             POPSTACK(S_a);
                   
            /* Beseitigen des Applikators fuer die Case-Funktion*/
              if (T_AP_2 (READSTACK (S_m1)))
              {
                  PPOPSTACK (S_m);
                  PPOPSTACK (S_m1);
              }
              else
              {
                  WRITE_ARITY (S_m1, READ_ARITY (S_m1) - 1);
              }

               g_b =   (PTR_HEAPELEM) R_NOMAT ((*(PTR_DESCRIPTOR)nomat_desc), guard_body);

             /*einlagern, falls der body-ausdruck hinter einem*/
             /*expressionpointer weggeschrieben werden musste  */
               if  (R_DESC((*(PTR_DESCRIPTOR)(g_b[2])), type) == TY_EXPR)
                  load_expr ((STACKELEM *) A_EXPR ((*(PTR_DESCRIPTOR)(g_b[2])), pte));
               else
               {
                  /*Falls kein Expressionpointer im Nomatchdeskriptor steht*/
                  /*Inhalt auf den E-Stack bringen und Refcnt erhoehen     */
                    PUSHSTACK(S_e, g_b[2]);
                    INC_REFCNT( (PTR_DESCRIPTOR) g_b[2]);
               }
               
           /*Reduktion hat geklappt, Nomatchdeskriptor nomat_desc*/
           /*beseitigen                                       */
             DEC_REFCNT((PTR_DESCRIPTOR) nomat_desc);

            END_MODUL("pm_nomat_2");

          /*weiter auf dem e-Stack; Ausrechnen des Rumpfausdrucks*/
            return(FALSE);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   ------- g u a r d   i s t   f a l s e       ----------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 case SA_FALSE:

                /*switchdeskriptor auf den e-stack legen*/
                swi_desc = (PTR_DESCRIPTOR)R_NOMAT ((*nomat_desc), ptsdes);
                PUSHSTACK(S_e, swi_desc);

                /*Refcntbehandlung*/
                INC_REFCNT(swi_desc);
                DEC_REFCNT(nomat_desc);

                END_MODUL("pm_nomat_2");

                return(pmredsw( READSTACK(S_e), READSTACK(S_a), regel+1));       

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* --- g u a r d  i s t  w e d e r  t r u e  n o c h  f a l s e  --*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                 default:

                   /*Nomatchdeskriptor anlegen*/
                     if ((new_nm_desc = (PTR_DESCRIPTOR) newdesc ()) == 0)
                              post_mortem ("pmredsw: Heap out of Space");
                     DESC_MASK (new_nm_desc, 1,  C_EXPRESSION, TY_NOMAT);
           
                    /*Platz besorgen*/
                     if (newheap( 4,A_NOMAT ((*new_nm_desc), guard_body)) == 0) 
                     post_mortem("pmred-nomat: es konnte kein neuer switchdescr angelegt werden ");

                     /*switchdeskriptor feststellen*/
                     swi_desc = (PTR_DESCRIPTOR)R_NOMAT ((*nomat_desc), ptsdes);

                    /* Kopieren des switchdeskriptors */
                     L_NOMAT((*new_nm_desc),ptsdes)=(T_PTD)swi_desc;
                     INC_REFCNT((PTR_DESCRIPTOR) swi_desc);

                   /* eintragen der Zahl der ueberprueften whens */
                     L_NOMAT ((*new_nm_desc), act_nomat) = regel ;
                   
                    /* eintragen,aus welchem grund der Deskriptor angelegt wird: */
                    /* reason = 3 oder reason = 4 bedeutet,dass der guard weder  */
                    /* zu TRUE noch zu FALSE reduziert wurde.                    */
                    /* Es wird eine 4 eingetragen,falls der Reduktionszaehler    */
                    /* abgelaufen ist,eine 3, falls eine weitere Reduktion des   */
                    /* Ausdruckes in keinem Fall mehr moeglich ist.              */
                      
                      if ( _redcnt == 0 )
                         L_NOMAT ((*new_nm_desc), reason) = 4 ;
                      else
                         L_NOMAT ((*new_nm_desc), reason) = 3 ;
                   
                   /*guard-body ausfuellen                       */
                   /*Dazu die Laenge wegschreiben                */
                    L_NOMAT ((*new_nm_desc), guard_body)[0] = 3;

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
	                        L_NOMAT ((*new_nm_desc), guard_body)[1] = x;
                           }  
                          else
                              post_mortem (" pm_mat: Ausdruck konnte nicht ausgelagert werden ");
                     }
                     else
                     {
                        /*guard wegschreiben */
	                  L_NOMAT ((*new_nm_desc), guard_body)[1] = POPSTACK(S_a);
                     }

                    /* Beseitigen des Inkarnationsblocks fuer den Guard   */
                     if (T_DOLLAR (READSTACK (S_m)))
                     {
                          POP_I_BLOCK (hilf);
                          POPSTACK (S_m); /* das T_DOLLAR */
                     }

                     /*body in den nomatchdescr. wegschreiben*/
                     /*der alte body kann kopiert werden     */
                     L_NOMAT ((*new_nm_desc), guard_body)[2] =
                             R_NOMAT((*nomat_desc), guard_body)[2];
                     INC_REFCNT( (PTR_DESCRIPTOR) R_NOMAT((*nomat_desc), guard_body)[2]);


                     /* Argument wegschreiben und Entfernen des Applikators */
          
	                        L_NOMAT ((*new_nm_desc), guard_body)[3] = POPSTACK(S_a);

                            POPSTACK(S_m); 
                            POPSTACK(S_m1); 
 
                     /* Schliessen des Nomatch-Deskriptors */
                                               
                      if (SIZEOFSTACK_TILDE > 0)
                      { 
                        PUSHSTACK(S_e, new_nm_desc);
                        if ( ( x = st_clos(0,0) ) == NULL )
                            post_mortem("pmredsw: Heap out of space ");
                        else
                            PUSHSTACK(S_a,x);
                      }
                      else
                        PUSHSTACK(S_a, new_nm_desc);

                     /*Reduktion hat geklappt, Nomatchdeskriptor nomat_desc*/
                     /*beseitigen                                          */
                       DEC_REFCNT((PTR_DESCRIPTOR) nomat_desc);  
    
                     END_MODUL("pm_nomat_2");

                     /*weiter auf der M-Linie des ear*/
                     return(TRUE) ;

}/*ende switch*/
}


