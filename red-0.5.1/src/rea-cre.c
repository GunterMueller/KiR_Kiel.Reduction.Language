/* $Log: rea-cre.c,v $
 * Revision 1.2  1992/12/16  12:49:47  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */




/*-----------------------------------------------------------------------------
 *  ea-create.c  - external:  ea_create;
 *         Aufgerufen durch:  reduct;
 *  ea-create.c  - internal:  check_list_for_store;
 *-----------------------------------------------------------------------------
 */

/* last update  by     why
   25.05.88     ca     zuerst werden alle konstanten datenobjekte und variablen
                       ausgelagert (eac), deshalb auch zusaetzliches auslagern
                       von freien variablen.
                       (Durch diesen ersten Durchlauf vereinfachen sich alle
                       darauffolgenden PreProcessing-Algorithmen, da sie keine
                       konstanten Datenobjekte mehr vorfinden !)
   copied       by     to

 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>

/*---------------------------------------------------------------------------
 * Externe Variablen:
 *---------------------------------------------------------------------------
 */
extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern  void pmprepa ();				    
extern int       lrecfunc();                /* lrecfunc.c */
extern void      lst_lrec();                /* lr-stor.c */
extern void      lte_freev();               /* lte-frev.c */
                          /* schliesst parameterlose Funktionsdefinitionen */
                          /* in LREC-Konstrukten ab                        */
extern STACKELEM st_cond();                 /* rstpre.c   */
extern STACKELEM st_vare();                 /* rstpre.c   */
extern STACKELEM st_list();                 /* rstpre.c   */
extern int       recfunc();                 /* recfunc.c  */
extern void      subfunc();                 /* subfunc.c  */
static BOOLEAN check_list_for_store();    /* forward deklaration */
int             ptrav_a_hilf();

/* ach 29/10/92 */
extern void eac();                          /* reac.c */
extern void stack_error();                  /* rstack.c */
extern STACKELEM st_var();                  /* reac.c */
extern void mvheap();                       /* aeh... */
extern void trav();                         /* aeh... */
extern STACKELEM pm_st_switch();            /* aeh... */
extern STACKELEM pm_st_match();             /* aeh... */
extern STACKELEM st_expr();                 /* aeh... */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();               /* rstack.c */
extern void DescDump();                     /* redesc.c */
#endif
/* end of ach */

#ifdef AUTODIST
extern void d_autodist();
#endif

/*-----------------------------------------------------------------------------
 *  ea_create  --  Preprocessed einen auf dem E-Stack liegenden Ausdruck,
 *                 und liefert den Ergebnisausdruck auf dem A-Stack ab.
 *  globals    --  S_e,S_a     <w>
 *                 bindings    <w>
 *-----------------------------------------------------------------------------
 */
/*  globals    --  S_i         <w> [#if (!DORSY)]                           */
/*                 bound       <w>                                          */
/*--------------------------------------------------------------------------*/
void ea_create()
{
  register STACKELEM  x,y;
  VOLATILE STACKELEM hilf=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992, volatile: ach 10/11/92 */
  int i;
  register unsigned short  arity;
  extern int _bindings;                     /* reduct.c */
  extern jmp_buf _interruptbuf;             /* reduct.c */
  PTR_DESCRIPTOR desc;
  extern int _bound;                        /* state:c */
  int lrec,nbound;
  STACKELEM z;

  START_MODUL("ea_create");

  setjmp(_interruptbuf);                    /* Interruptbehandlung Preprocessing */
  if (_interrupt)                           /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("ea_create: Interrupt received");
  }

  eac();         /* lagere alle konstanten aus       */       /* ca */

#ifdef AUTODIST
  d_autodist();     /* syntactic changes for distributing expressions */
#endif 

  PUSHSTACK(S_m,DOLLAR);                             /* Ende-Symbol */
#if DORSY
#else
  PUSHSTACK(S_i,KLAA);                 /* fuer Pfad-Funktionsnamen */
#endif

  _bound = 0;             /* keine aktive Variablenbindung */
                          /* in _bound werden die relativ freien Variablen */
                          /* in LREC-konstrukten sowie in geschachtelten   */
                          /* Funktionen in LREC-konstrukten gezaehlt       */
  nbound = 0;             /* keine aktive Variablenbindung */
                          /* anzahl der bindungen einer funktion(sdefinition) */
  _bindings = 0;                                     /* keine aktive Variablen-*/
                                                     /* bindung */
main_e:
  x = READSTACK(S_e);
/* aufruf von lte_freev() */
  if (!(T_SUB(x))) {
     if ((_bound) && (T_LIST(READSTACK(S_m)))) {
                     /* falls ein LREC-Konstrukt relativ freie Variablen */
                     /* enthaelt, werden alle Funktionsdefinitionen ab-  */
                     /* geschlossen und als SUB-Strukturen dargestellt   */
        STACKELEM z;
        z = POPSTACK(S_m);         /* listenkonstruktor */
        if ((T_LREC(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) > 1)) {
           PUSHSTACK(S_m,z);          /* listenkonstruktor */
                              /* alle Teilausdruecke des Listenkonstruktors */
                              /* muessen mit _bound zusaetzlichen           */
                              /* Parametern auf das heap ausgelagert werden */
           lte_freev(_bound);           /* Anlegen eines TY_SUB Deskriptors */
                      /* "nbound" fuer stelligkeit des PRELIST-konstruktors */
        }
        else
           PUSHSTACK(S_m,z);          /* listenkonstruktor */
     }  /* else ;    auskommentiert RS 18.1.1993 */
  }
  /* else ;  / fuer T_SUB(x) wird kein zusaetzlicher TY_SUB-deskriptor angelegt / auskommentiert RS 18.1.1993 */

  if (T_CON(x))
  {
    if (T_SUB(x))                           /* Funktion gefunden ? */
    {
      if (T_LIST(READSTACK(S_m))) {
         z = POPSTACK(S_m);
         if ((T_LREC(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) > 1))
            lrec = 1;          /* in subfunc() kein closefunc() ausfuehren */
         else
            lrec = 0;
         PUSHSTACK(S_m,z);
      }
      else
        lrec = 0;
      PUSHSTACK(S_m,SET_ARITY(PRELIST,_bound)); /* Kopie von nbound */
      subfunc(lrec);             /* Funktion preprocessen */
      nbound = ARITY(x) - 1;            /* Parameter fuer lrecfunc */
      goto main_e;
    }

    if (T_REC(x))         /* rekursive Funktion */
    {
       if (T_LIST(READSTACK(S_m))) {
          z = POPSTACK(S_m);
          if ((T_LREC(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) > 1))
             lrec = 1;         /* in recfunc() kein closefunc() ausfuehren */
          else
             lrec = 0;
          PUSHSTACK(S_m,z);
       }
       else
         lrec = 0;
       PUSHSTACK(S_m,SET_ARITY(PRELIST,_bound)); /* Kopie von nbound */
       recfunc();            /* Preprocessen dieser Funktion. Hier wird der */
                             /* PRELIST erst von recfunc erzeugt (siehe oben) */
       goto main_e;
    }

    if (T_LREC(x))        /* LETREC */
    {
       STACKELEM fname;

#if DORSY
       fname = KLAA;               /* fuer lrecfunc() */
#else
       fname = READSTACK(S_i);     /* func_name */
#endif
       lrecfunc(fname);
       goto main_m;
    }
    if (T_PROTECT(x)) {
      y = st_vare();
      PUSHSTACK(S_a,y);                   /* gebundene Variable werden von */
      goto main_m;                        /* recfunc bzw. subfunc behandelt */
    }
    if (ARITY(x) > 0)
    {
       PPOPSTACK(S_e);       /* sonstiger Konstruktor mit Stelligkeit > 0 */
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }

    PPOPSTACK(S_e);          /* sonst. Konstruktor mit Stelligkeit 0 */
    PUSHSTACK(S_a,x);
    goto main_m;
  }

/* "if (T_POINTER(x))" ist doppelt spezifiziert; */
/* st_vare() wird ebenfalls entfernt             */

  if (T_POINTER(x))                         /* einfach traversieren, es sei */
  {                                         /* denn, es ist ein Namenspointer */
    PPOPSTACK(S_e);                         /* Dann ist es ganz freie Variable */
    PUSHSTACK(S_a,x);                       /* siehe oben */
    if (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME)
     {
        y = st_var();                       /* Deskriptor fuer ganz freie */
        PUSHSTACK(S_a,y);                   /* Variable erzeugen */
     }
    goto main_m;
  }
  if (T_ATOM(x))
  {                                         /*         sonstige atoms         */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  post_mortem("ea_create: No match successful on main_e");

main_m:
  x = READSTACK(S_m);
                                                     /* Rumpf einer Funktion */
  if (T_PRELIST(x))                                  /* fertig bearbeitet -> */
  {                                                  /* auslagern            */
     PTRAV_A_HILF;
     desc = (PTR_DESCRIPTOR) READSTACK(S_a);
     _bindings -= ARITY(x);                       /* Anzahl offener Bindungen */
                                            /* Steht im PRELIST */
     mvheap(A_FUNC((*((PTR_DESCRIPTOR)READSTACK(S_a))),pte)); /* Auslagern */
     PPOPSTACK(S_m);     /* PRELIST  weg */
     _bound = ARITY(POPSTACK(S_m));       /* zweites PRELIST */
     goto main_m;
  }

  arity = ARITY(x);

  if (arity > 1)  /* dh die Arity des Konstruktors war >= 2 */
  {
    WRITE_ARITY(S_m,arity-1);
#if DORSY
#else   /* !DORSY */
    if (T_LIST(x)) {      /* listenkonstruktor fuer func_def's ? */
       STACKELEM u;

       u = POPSTACK(S_m);
       if ((T_LREC(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) > 1)) {
          if (POPSTACK(S_i) == KLAA) {       /* func_name */
             post_mortem("ea_create: Read on empty stack i");
          }
       }
       PUSHSTACK(S_m,u);
    }
#endif  /* !DORSY */
    goto main_e;
  }
  if (arity > 0)          /* DOLLAR hat arity = 0 */
  {
    PPOPSTACK(S_m);
    x = POPSTACK(S_m1);
	if (T_PM_CASE (x))	/* switch ist traversiert und */
	{			/* preprocessing hat stattgefunden */


    /* Abfrage ob ein caserec vorliegt, dann muss der*/
    /* Parameter zusaetlich in die Matchdeskriptoren */
    /* geschrieben werden                            */
    if (T_PM_REC( x))
    {
       /* situation auf dem A- stack: entweder ein otherwise Ausdruck*/
       /* oder ein end                                               */
       if (T_PM_OTHERWISE(READSTACK(S_a)))
       {
          /*otherwise nach e */
          trav(&S_a, &S_e);  

          hilf = POPSTACK(S_a);

          /*otherwise zurueck */
          trav(&S_e, &S_a);  
       }
       else
       {
          PPOPSTACK(S_a); /* das end */
          hilf = POPSTACK(S_a);  /* der rekursive Parameter*/

          /* das end zurueck*/
          PUSHSTACK(S_a, PM_END);
       }
    }

    if (T_PM_REC( x))
    {
       /* arity reduzieren, da der rekursive */
       /* Parameter fortan wegfaellt         */
       arity = ARITY(x) -1;
    }
    else
    {
       /* normales case*/
       arity = ARITY(x) ;
    }
#if PI_RED_PLUS
    if (T_PM_NCASE(x)) {
      arity = ARITY(x) - 1;
      y =  pm_st_switch (arity );
      arity = VAL_INT(POPSTACK(S_a)); /* Stelligkeit des multiplen CASEs */
    }
    else
#endif /* PI_RED_PLUS         RS 1.12.1992 */

	    y =  pm_st_switch (arity );
	    PUSHSTACK (S_a, y );
    /* Ausdruck in den switch      */
    /* Deskriptor schreiben, other */
    /* wise behandlung uebernimmt  */
    /* st_switch                   */

    /* Ausfuellen der freigelassen Stellen */
    /* im matchcode durch den Befehl       */
    /* PM_PUSH p-f falls es sich um ein    */
    /* caserec handelt                     */  
    if (T_PM_REC( x))
    {
       for ( i = ARITY(x)-2 ; i > 0; i--)
       {
        register    STACKELEM * ptsex;
        ptsex = (STACKELEM *) R_SWITCH ((*(PTR_DESCRIPTOR)y), ptse);
        desc = (PTR_DESCRIPTOR) ptsex[i];
        (L_MATCH ((*desc), code))[3] = y;
        (L_MATCH ((*desc), code))[4] = hilf;
        /*Refcnt fuer hilf und y nicht hochzaehlen, dann braucht*/
        /*im Code nichts freigegeben werden und es gibt keinen  */ 
        /*Aerger mit den zyklischen Pointerstrukturen           */
        /*INC_REFCNT( (PTR_DESCRIPTOR) hilf);                   */    
        /*INC_REFCNT( (PTR_DESCRIPTOR) y);                      */
       }
     }

    /*Editfeld in den Switchdescr. eintragen*/
    L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), special) = 
				EXTR_EDIT(x);

    /* Unterscheidung welches case vorliegt und Speichern im Switchdesc.*/
#if PI_RED_PLUS
    /* case_type enthaelt die stelligkeit (anzahl der argumente) des CASE */
    if (T_PM_NCASE(x)) /* n-stelliges case*/
       L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), case_type) =  arity; 
    else
    if (T_PM_SWITCH(x)) /* normales case*/
       L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), case_type) = 1; 
    else
       post_mortem("ea_create: invalid CASE type");
#else
    if (T_PM_SWITCH(x)) /* normales case*/
       L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), case_type) = 0; 
    else
    if (T_PM_ECASE(x)) /* eager case */
       L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), case_type) = 1; /*PM_EAG*/
    else
    if (T_PM_REC(x)) /* caserec  */
    {
      /* den rekursiven Parameter des caserec in das case_type Feld retten*/
      L_SWITCH ((*( (PTR_DESCRIPTOR) READSTACK(S_a))), case_type) = hilf;
    }
#endif /* PI_RED_PLUS     RS 1.12.1992 */
    goto main_m;

  }

	if (T_PM_MATCH (x))	/* when expression ist traversiert und */
	{			/* guard sowie body sind verpointert   */

            hilf = pm_st_match();
	    PUSHSTACK (S_a, hilf );

	    goto main_m;
	}
    PUSHSTACK(S_a,x);

    if (T_COND(x))   { y = st_cond(); PUSHSTACK(S_a,y); }
                          /* conditional auslagern */
    if ( (T_LIST(x))||(T_STRING(x)) )
    {                     /* listenkonstruktor fuer func_def's */
      if ((T_LREC(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) > 1)) {
#if DORSY
#else   /* !DORSY */
         if (POPSTACK(S_i) == KLAA) {        /* func_name */
            post_mortem("ea_create: Read on empty stack i");
         }
#endif  /* !DORSY */
         lst_lrec();
         WRITE_ARITY(S_m,2);
         WRITE_ARITY(S_m1,2);
      }
      else {
      if (check_list_for_store())  /* darf Liste ausgelagert werden? */
      {
         if ((x = st_list() ) == 0)   /* Wenn ja, auslagern */
            post_mortem("ea_create: heap out of space in st_list");
         else
            PUSHSTACK(S_a,x);
      }
      }
    }
    goto main_m;
  }
  /* Ein DOLLAR. Dann ist ea_create zu beenden */
  PPOPSTACK(S_m);         /* DOLLAR */
#if DORSY
#else   /* !DORSY */
  PPOPSTACK(S_i);         /* KLAA */        /* (func_names) */
#endif  /* !DORSY */

/* dg */     if ((x = st_expr()) == 0)
/* dg */       post_mortem("ea_create: st_expr failed");
/* dg */     PUSHSTACK(S_e,x);

  /* nun den V-Stack von Namensdescriptoren befreien: */
  /*    weitere Informationen in st_name.             */
  for (arity = SIZEOFSTACK(S_v); arity > 0; arity--)
    PPOPSTACK(S_v);
  /* Refcountbehandlung ist nicht noetig. */
  END_MODUL("ea_create");
}

/*----------------------------------------------------------------------------
 * check_list_for_store -- analysiert eine Liste auf dem A-Stack.
 *                         Ein TRUE wird zurueckgegeben, falls die Liste
 *                         weder Konstruktoren noch Nummernvariablen ent-
 *                         haelt.
 * globals              -- S_hilf,S_a  <w>
 *----------------------------------------------------------------------------
 */
static BOOLEAN check_list_for_store()
{
  register unsigned short arity;
  register STACKELEM list;
  register STACKELEM x;
  BOOLEAN  result;

  START_MODUL("check_list");

  list = POPSTACK(S_a);            /* LIST */
  arity = ARITY(list);

  for ( arity = ARITY(list); arity > 0; arity --)
  {
    x = READSTACK(S_a);
    if T_CON(x) break;    /* alles Gruende, das eine Liste nicht ausgelagert */
    if T_NUM(x) break;    /* werden darf */
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),class) == C_EXPRESSION)) break;
    /* der Rest ist redundant
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_REC)) break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_LREC_IND))
       break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_COND)) break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_SWITCH)) break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_NOMAT)) break;
       ende Redundanz */
    PUSHSTACK(S_hilf,POPSTACK(S_a));
  }
  result = (arity == 0);  /* result == TRUE heisst: darf ausgelagert werden */

  while ( (int) arity < ARITY(list))  /* auf int gecastet von RS 04/11/92 */ 
  {
    PUSHSTACK(S_a,POPSTACK(S_hilf));     /* Liste wieder korrekt auf A legen */
    arity++;
  }
  PUSHSTACK(S_a,list);             /* LIST */
  END_MODUL("check_list");

  return(result);
}
/* end of ea-create.c */
