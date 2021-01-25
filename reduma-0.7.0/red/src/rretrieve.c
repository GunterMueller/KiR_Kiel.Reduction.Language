/* $Log: rretrieve.c,v $
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:49  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

/*----------------------------------------------------------------------------
 * MODULE DER POSTPROCESSING PHASE  fuer das Pattern Matching
 *----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rinstr.h"

 /*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern int  freeheap ();                                /* heap:c */
extern  void freedesc ();                               /* heap:c */
extern  void convi ();
extern  void convr ();
extern  void load_expr ();

/* RS 30/10/92 */ 
extern void stack_error();             /* rstack.c */
extern void trav();                    /* rtrav.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

#define TRICKY 0

 /*---------------------------------------------------------*/
 /* get_length of a heap-element                            */
 /*---------------------------------------------------------*/
int     get_length (ptr_to_heapelem)
        PTR_HEAPELEM ptr_to_heapelem;
{
    int     i;

    if ((i = (int) ptr_to_heapelem[0]) >= TRICKY)
    {
        i -= TRICKY;
    }
    else
    {
        fprintf (stderr, " internal error : this should not take place PM_MOVE/PM_LOAD - EXPR \n");
    }
    return (i);
}


 /*-----------------------------------------------------------------------------
 * pm_ret_switch erhaelt Switchdeskriptor und legt die entsprechenden Match-
 * deskriptoren  auf den E-Stack. Das letzte Element des Datenvektors
 * wird gesondert behandelt, weil dort entweder ein Verweis auf den
 * gesamten Switchdeskriptor oder einen Matchdeskriptor  steht.
 *-----------------------------------------------------------------------------
 */
void pm_ret_switch (pdesc)
STACKELEM pdesc;
{
    PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
    register    STACKELEM * ptsex;
    register int    i;

    START_MODUL ("pm_ret_switch");

    ptsex = (STACKELEM *) R_SWITCH ((*desc), ptse);
    i = get_length (ptsex);
    IS_SPACE (S_e, i);

    if (R_DESC ((*desc), ref_count) == 1)
    {
      /* keine Refcountbehandlg. noetig */

        if (R_DESC ((*((PTR_DESCRIPTOR) ptsex[i])), type) == TY_MATCH)
        {
            load_expr ((STACKELEM *) A_MATCH ((*(PTR_DESCRIPTOR) ptsex[i]), body));

            PUSHSTACK (S_e, SET_ARITY (PM_OTHERWISE, 1));
            WRITE_ARITY (S_e, 1);
        }
        else
            if (R_DESC ((*(PTR_DESCRIPTOR) ptsex[i]), type) == TY_NOMAT)
            {
                 DEC_REFCNT (((PTR_DESCRIPTOR) ptsex[i]));
                 PUSHSTACK (S_e, PM_END);
            }
            else
                 post_mortem ("pm_st_switch: kein match oder switch Deskriptor in ptse");

      /* i inkrem. weil letztes Argument des Datenvektors schon oben  */
      /* behandelt wurde                                              */
        --i;   

        for (; i > 0;)
            PPUSHSTACK (S_e, ptsex[i--]);

        freeheap ((PTR_HEAPELEM) ptsex);
#if PI_RED_PLUS
        /* (COUNT) von TB, 9.11.1992 */
        if  ((COUNT)R_SWITCH((*desc), case_type) < (COUNT)2)
        {/* es liegt ein  normales case vor */

           PUSHSTACK (S_e, (STACKELEM)(PM_SWITCH | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
        }
        else
          {
            /* es liegt ein n-stelliges  case vor */
          
            PUSHSTACK (S_e, (STACKELEM)(PM_NCASE | SET_EDIT( (int)R_SWITCH( (*desc), special))));
            WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
          }
#else
        if  (R_SWITCH((*desc), case_type) == 0)
        {/* es liegt ein  normales case vor */

           PUSHSTACK (S_e, (STACKELEM)(PM_SWITCH | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
        }
        else
          if  (R_SWITCH((*desc), case_type) == 1)
          {/* es liegt ein eager  case vor */
          
           PUSHSTACK (S_e, (STACKELEM)(PM_ECASE | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
          }
        else
        { /* es liegt ein caserec vor*/

          /* Stackelement auf den E-Stack und Ausfuellen des*/
          /* Editfelds                                      */
           PUSHSTACK (S_e, (STACKELEM)(PM_REC | SET_EDIT( (int)R_SWITCH( (*desc), special))));
          /* Arity um eins fuer den Rekursiven Parameter erhoehen*/
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen) +1);

        }
#endif /* PI_RED_PLUS       auskommentiert RS 3.12.1992 */
        WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
        freedesc (desc);
    }
    else
     /*Refcntbehandlung notwendig*/
    {
        if (R_DESC ((*(PTR_DESCRIPTOR) ptsex[i]), type) == TY_MATCH)
        {
            load_expr ((STACKELEM *) A_MATCH ((*(PTR_DESCRIPTOR) ptsex[i]), body));
            PUSHSTACK (S_e, SET_ARITY (PM_OTHERWISE, 1));
            WRITE_ARITY (S_e, 1);
        }
        else
            if (R_DESC ((*(PTR_DESCRIPTOR) ptsex[i]), type) == TY_NOMAT)
            {
                 PUSHSTACK (S_e, PM_END);
            }
            else
                 post_mortem ("kein match oder switch Deskriptor in ptsex[ende]");

      /* i inkrem. weil letztes Argument des Datenvektors schon oben  */
      /* behandelt wurde                                              */
        --i;             

        for (; i > 0;)
        {
            if (T_POINTER (ptsex[i]))
                 INC_REFCNT (((PTR_DESCRIPTOR) ptsex[i]));
            PPUSHSTACK (S_e, ptsex[i--]);
        }

        DEC_REFCNT (desc);

#if PI_RED_PLUS
	/* (COUNT) von TB, 9.11.1992 */
        if  ((COUNT)R_SWITCH((*desc), case_type) < (COUNT)2)
        {/* es liegt ein  normales case vor */
          
           PUSHSTACK (S_e, (STACKELEM)(PM_SWITCH | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
        }
        else
          {
            /* es liegt ein n-stelliges  case vor */
          
            PUSHSTACK (S_e, (STACKELEM)(PM_NCASE | SET_EDIT( (int)R_SWITCH( (*desc), special))));
            WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
          }
#else
        if  (R_SWITCH((*desc), case_type) == 0)
        {/* es liegt ein  normales case vor */
          
           PUSHSTACK (S_e, (STACKELEM)(PM_SWITCH | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
        }
        else
          if  (R_SWITCH((*desc), case_type) == 1)
          {/* es liegt ein eager  case vor */
          
           PUSHSTACK (S_e, (STACKELEM)(PM_ECASE | SET_EDIT( (int)R_SWITCH( (*desc), special))));
           WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen));
          }
        else
        { /* es liegt ein caserec vor*/

          /*rekursiver Parameter auf den E-Stack*/
          PUSHSTACK(S_e,  (R_MATCH ((*(PTR_DESCRIPTOR) ptsex[1]), code))[4]);
          INC_REFCNT((PTR_DESCRIPTOR) READSTACK(S_e));

          /* Stackelement auf den E-Stack und Ausfuellen des*/
          /* Editfelds                                      */
          PUSHSTACK (S_e, (STACKELEM)(PM_REC | SET_EDIT( (int)R_SWITCH( (*desc), special))));
          /* Arity um eins fuer den Rekursiven Parameter erhoehen*/
          WRITE_ARITY (S_e, R_SWITCH ((*desc), nwhen) +1);

        }
#endif /* PI_RED_PLUS   auskommentiert RS 3.12.1992 */
    }
    END_MODUL ("pm_ret_switch");
}

 /*-----------------------------------------------------------------------------
 * pm_ret_match erhaelt einen Matchdeskriptor und legt das Pattern, den guard
 *             und den body auf den E-stack
 *-----------------------------------------------------------------------------
 */
void pm_ret_match (pdesc)
STACKELEM pdesc;
{
/* STACKELEM hilf;       RS 30/10/92 */ 
    PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;

    START_MODUL ("pm_ret_match");



    load_expr ((STACKELEM *) A_MATCH ((*desc), body));
    if ( (R_MATCH ((*desc), guard)) == 0 )
    {
       /*der guard besteht au einem  true */
       PUSHSTACK(S_e, SA_TRUE);
    }
    else
    {
      /* der guard ist ein Reduktionssprachenausdruck*/
      load_expr ((STACKELEM *) A_MATCH ((*desc), guard));
    }

   /* vorhergende Zeile sucht im code  Eintrag an der Stelle 1 im ptdv      */
   /* Vektor den fuer das Postprocessing geretteten Deskriptor und legt ihn */
   /* auf den E-stack                                                       */
    PUSHSTACK (S_e, (STACKELEM) * (*(A_MATCH ((*desc), code)) + 1));

   /* Refcnt fuer diesen Desc. erhoehen                 */
    if(T_POINTER(READSTACK(S_e)))
    INC_REFCNT ((PTR_DESCRIPTOR) * (*(A_MATCH ((*desc), code)) + 1));

    PUSHSTACK (S_e, (STACKELEM) SET_ARITY (PM_WHEN, 3));

    DEC_REFCNT (desc);

    END_MODUL ("pm_ret_match");
}


/*-----------------------------------------------------------------*/
/* pm_del_e loescht einen Ausdruck in dem Pointer und Konstruktoren*/
/* stehen vom E-stack. pm_del_e wird in pm_ret_nomatch aufgerufen  */
/*-----------------------------------------------------------------*/

void pm_del_e ()
{
    STACKELEM x = POPSTACK (S_e);
    int     i;

    if (T_CON (x))
        for (i = ARITY (x); --i >= 0;)
            pm_del_e ();
    else
        if (T_POINTER (x))
            DEC_REFCNT((PTR_DESCRIPTOR) x);
}


/*--------------------------------------------------------------*/
/* Funktion  zum kopieren eines Ausdrucks vom A-Stack auf den   */
/* E-Stack und Hilfs-Stack                                      */
/*--------------------------------------------------------------*/
void cp()
{
    register STACKELEM  x = POPSTACK(S_a);
    register int        i;

    if (T_CON(x))
      for (i = ARITY(x); --i>=0; )
        cp();
    PUSHSTACK(S_e,x);
    if (T_POINTER(x)) INC_REFCNT((PTR_DESCRIPTOR) x);
    PUSHSTACK(S_hilf,x);
}

/*----------------------------------------------------------
 * pm_ret_nomatch erhaelt einen Nomatchdeskriptor
 * legt den Switchausdrucka
 *             auf den E-stack
 * globals --  S_e           <w>
 * called by -- reduct, ear, ea_retrieve.
 *------------------------------------------------------------
 */
void pm_ret_nomatch (pdesc)

STACKELEM pdesc;
{
    PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
/*  STACKELEM * pm_ptse;      RS 30/10/92 */
    STACKELEM  hilf;
    STACKELEM  swik;
    PTR_DESCRIPTOR pm_mat_desc;
    PTR_HEAPELEM pm_guard;
    int     pm_nwhen;
    int     i;
    int     pm_act_nomat;
    PTR_DESCRIPTOR pm_swidesc;
 /* STACKELEM pm_rec_par;        RS 30/10/92 */ 

    START_MODUL ("pm_ret_nomatch");

    pm_swidesc = (PTR_DESCRIPTOR)R_NOMAT ((*desc), ptsdes);
    pm_act_nomat = R_NOMAT ((*desc), act_nomat);
    pm_guard     = R_NOMAT ((*desc), guard_body);
    pm_nwhen = R_SWITCH ((*pm_swidesc), nwhen);

 /* act_nomat = 0 falls kein otherwise Ausdruck existierte, und waehrend
    des Processing die Casefunktion nicht angewendet werden konnte.
    Falls die Fortsetzung der Casefunktion mit einem UNDECIDED abgebrochen
    wird, weil ein guard Ausdruck weder true noch false ergibt oder eine
    Konstante oder Listenkonstruktor im Pattern mit einer Applikation oder
    Variablen verglichen wird, enthaelt act_nomat die Nummer des Pattern-Match
    der fehlschlug. D.h.  act_nomat = 1 falls gleich der guard des ersten 
    when sich nicht zu TRUE oder FALSE evaluiert hat */

    if ((pm_act_nomat <= 0) || ((1 == pm_act_nomat ) &&  (0 == pm_guard)))
    {
        PUSHSTACK (S_e, (STACKELEM) pm_swidesc);
        INC_REFCNT (pm_swidesc);
    }
    else
    {
       /* pm_ret_switch decr refcnt. Es gibt aber weiterhin vom 
           Nomatch-Deskriptor eine Refernz auf den Switcdescriptor */
        INC_REFCNT (pm_swidesc);

       /* alten Switchdesk. einlagern */
        pm_ret_switch ((STACKELEM) pm_swidesc);   /* cast eingefuegt RS 3.12.1992 */

#if PI_RED_PLUS
        /* es liegt ein normales oder n-stelliges case vor */
        /* Switchkonstr beseitigen */
        swik = POPSTACK (S_e);
#else
        if ( (R_SWITCH((*pm_swidesc), case_type) == 0)
            || (R_SWITCH((*pm_swidesc), case_type) == 1)  )
        {/* es liegt ein eager oder normales case vor */

         /* Switchkonstr beseitigen */
          swik = POPSTACK (S_e);
       }
       else
       { /* es liegt ein caserec vor*/

         /* Switchkonstr beseitigen */
          swik = POPSTACK (S_e);
         
         /* rekrusiven Parameter retten */
         pm_rec_par = POPSTACK(S_e);
       }
#endif /* PI_RED_PLUS   auskommentiert RS 3.12.1992 */


    /* patterni, die nicht passeni, koennen weggepoppt werden */
        for (i = 1; i < pm_act_nomat; i++)
        {
            pm_mat_desc = (PTR_DESCRIPTOR)POPSTACK (S_e);
            DEC_REFCNT (pm_mat_desc);
        }


      /*entfernen der ueberfluessigen Matchdeskriptoren */
        for(i = pm_act_nomat; i < pm_nwhen; i++)
             PUSHSTACK(S_hilf, POPSTACK(S_e)); 

        if(T_PM_END(READSTACK(S_e)))
        {
         PPOPSTACK(S_e); /* TB, 30.10.92 */ /* das end */ 
        PUSHSTACK(S_e, pm_swidesc);
        INC_REFCNT(pm_swidesc);

        /* Kopieren des Argument der Case-Funktion vom A-Stack*/
        /* auf den E-Stack, damit die Originalapplikation     */
        /* dargestellt werden kann                            */
        /* NCASE: multiple pattern */
        for (i = 0; i < (int) R_SWITCH(*pm_swidesc,case_type); i++)
                   /* int gecastet von RS 5.11.1992 */ 
          cp();
        PUSHSTACK(S_e,(STACKELEM) SET_ARITY(AP, i+1));
        for ( ; --i >=0 ; )
          trav(&S_hilf, &S_a);

        PUSHSTACK (S_e, SET_ARITY (PM_OTHERWISE, 1));
        WRITE_ARITY (S_e, 1);
        }
        else /* Es gibt einen otherwise Ausdruck */
        {
        /*otherwise kann so auf dem Stack stehen bleiben*/
        }

       /*zurueckbringen der beiseite geraeumten Matchdeskriptoren */
        for(i = pm_act_nomat; i < pm_nwhen; i++)
             PUSHSTACK(S_e, POPSTACK(S_hilf));  

       /*Berichtigung des guards im letzten Deskriptor auf dem E-stack
         der nicht  zu true oder false ausgewertet werden konnte         */
        if(     R_NOMAT ((*desc), guard_body) != 0)
        {
        pm_ret_match (POPSTACK (S_e));
        PPOPSTACK (S_e); /* TB, 30.10.92 */ /* das when vom E-stacK */
        hilf = POPSTACK(S_e); /* das Pattern */
        if (T_POINTER(hilf))
        {
           DEC_REFCNT((PTR_DESCRIPTOR)hilf);
           }
           else
            if (T_SA(hilf))
             /*void */ ;
             else
           post_mortem("pm_ret_nomat: an dieser Stelle wird ein Pointer erwartet ");

        pm_del_e ();               /* den alten guard enfernen  */

        pm_del_e();             /* den alten body entfernen */

       /* den body, in dem Pattern-Variablen ersetzt sind auf den*/
       /* Stack bringen                                          */
        PUSHSTACK(S_e, (STACKELEM)(R_NOMAT ((* desc), guard_body)[2]));
	INC_REFCNT ((PTR_DESCRIPTOR) READSTACK (S_e));

       /* den teilweise reduzierten Guard darstellen              */
        PUSHSTACK(S_e,(STACKELEM) (R_NOMAT ((*desc ), guard_body)[1]));
	T_INC_REFCNT ((PTR_DESCRIPTOR) READSTACK (S_e));
         /* RS 28.7.1993: INC_REF changed to T_INC cause of fix int's => crash */

        /* NCASE: multiple pattern */
        for (i = 0; i < (int) R_SWITCH(*pm_swidesc,case_type); i++)
           /* int gecastet von RS 5.11.1992 */ 
          PUSHSTACK(S_e,PM_SKIP); /* skip statt altem pattern */
        if (i > 1)
          PUSHSTACK(S_e,SET_ARITY(LIST,i));
        PUSHSTACK (S_e, (STACKELEM) SET_ARITY (PM_WHEN, 3));
        }

#if PI_RED_PLUS
        /* es liegt ein normales oder n-stelliges case vor */
        /* switch  Konstruktor auf den E-stack */
        PUSHSTACK (S_e, swik);
        WRITE_ARITY (S_e, pm_nwhen - pm_act_nomat + 1);
#else
        if ( (R_SWITCH((*pm_swidesc), case_type) == 0)
            || (R_SWITCH((*pm_swidesc), case_type) == 1)  )
        {/* es liegt ein eager oder normales case vor */

         /* switch  Konstruktor auf den E-stack */
          PUSHSTACK (S_e, swik);
          WRITE_ARITY (S_e, pm_nwhen - pm_act_nomat + 1);
       }
       else
       { /* es liegt ein caserec vor*/

         /* den geretteten Parameter zurueck*/
         PUSHSTACK(S_e, pm_rec_par);

         /* switch  Konstruktor auf den E-stack */
          PUSHSTACK (S_e, swik);
          WRITE_ARITY (S_e, pm_nwhen - pm_act_nomat + 2);
       }
#endif /* PI_RED_PLUS        auskommentiert RS 3.12.1992 */


    }
    DEC_REFCNT ((PTR_DESCRIPTOR) pdesc);
    END_MODUL ("pm_ret_nomatch");
}

