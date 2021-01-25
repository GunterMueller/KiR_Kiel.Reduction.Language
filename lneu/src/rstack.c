/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*****************************************************************************/
/*                                                                           */
/* Modul             : Stackmanager (in stack.c)                             */
/*                                                                           */
/* Untermodule       : InitStackmanagement()                                 */
/*                     FormatStack()                                         */
/*                     stack_error()                                         */
/*                                                                           */
/* Aufgerufen von    : reduce()                                              */
/*                                                                           */
/* Weitere Untermodule im Debugfall: readstack()                             */
/*                                   writestack()                            */
/*                                   sizeofstack()                           */
/*                                   pushstack()                             */
/*                                   popstack()                              */
/*                                                                           */
/* Aufgerufen von    : Alle Module die den Stackmanager brauchen.            */
/*                                                                           */
/* globale Variablen : die Stacks                                            */
/*                                                                           */
/* Funktion          : Bereitstellung der oben genannten Funktionen.         */
/*                                                                           */
/*****************************************************************************/

/* last update  by     why
   10.06.88     CS     v-stack ist jetzt ein zusaetzlicher stack
   15.06.88     ca     fehler der obigen korrektur behoben !
   copied       by     to

*/

#define STACKMANAGER  Hier ist der Stackmanager

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"


   
   
StackDesc  S_e,S_a,S_m,S_m1,S_v,S_hilf,S_i, S_pm,S_tilde; /* die globalen Stacks */

/*--------------------------------------------------------------------------
 * stack_error -- bastelt aus einer Nachricht und einem Stacknamen eine
 *                Beileitsnachricht fuer post_mortem.
 *--------------------------------------------------------------------------
 */
void stack_error(message,stackname)
char *message, *stackname;
{
 char deathmessage[80];

 strcpy(deathmessage,message);
 strcat(deathmessage," ");
 strcat(deathmessage,stackname);

 post_mortem(deathmessage);
}

/****************************************************************************/
/*                                                                          */
/*  I N I T S T A C K M A N A G E M E N T : Legt die globalen Stacks an.    */
/*                          Evtl. gehaltener Platz wird vorher freigegeben. */
/****************************************************************************/

void InitStackmanagement(Q_stacksize,M_stacksize,I_stacksize)
int Q_stacksize,M_stacksize,I_stacksize;
{
  static char * E_space = NULL;
  static char * A_space = NULL;
  static char * M_space = NULL;
  static char * M1_space = NULL;
  static char * I_space = NULL;
  static char * V_space = NULL;                                        /* CS */
  static char * Hilf_space = NULL;
  static char * Tilde_space = NULL;
  static char * Pm_space = NULL;

  if (E_space != (char *) NULL)    /* Freigabe von evtl. schon angefordertem */
  {                                /* Platz.                                 */
    free(E_space);
    E_space = NULL;
  }
  if (A_space != (char *) NULL)
  {
    free(A_space);
    A_space = NULL;
  }
  if (I_space != (char *) NULL)
  {
    free(I_space);
    I_space = NULL;
  }
  if (Pm_space != (char *) NULL)
  {
    free(Pm_space);
    Pm_space = NULL;
  }

  if (Tilde_space != (char *) NULL)
  {
    free(Tilde_space);
    Tilde_space = NULL;
  }

  if (Pm_space != (char *) NULL)
  {
    free(Pm_space);
    Pm_space = NULL;
  }

  if (V_space != (char *) NULL)    /* CS */
  {
    free(V_space);
    V_space = NULL;
  }
  if (M_space != (char *) NULL)
  {
    free(M_space);
    M_space = NULL;
  }
  if (M1_space != (char *) NULL)
  {
    free(M1_space);
    M1_space = NULL;
  }
  if (Hilf_space != (char *) NULL)
  {
    free(Hilf_space);
    Hilf_space = NULL;
  }

  if (Q_stacksize <= 1)   /* Parameterueberpruefung */
    post_mortem("Stackmanager: requested Q_stacksize too small");

  if (M_stacksize <= 1)
    post_mortem("Stackmanager: requested M_stacksize too small");

  if (I_stacksize <= 1)
    post_mortem("Stackmanager: requested I_stacksize too small");

  /* die Platzanforderung: */
  E_space    = (char *) malloc(Q_stacksize * sizeof(STACKELEM));
  A_space    = (char *) malloc(Q_stacksize * sizeof(STACKELEM));
  Hilf_space = (char *) malloc(Q_stacksize * sizeof(STACKELEM));
  M_space    = (char *) malloc(M_stacksize * sizeof(STACKELEM));
  M1_space   = (char *) malloc(M_stacksize * sizeof(STACKELEM));
  I_space    = (char *) malloc(I_stacksize * sizeof(STACKELEM));
  V_space    = (char *) malloc(I_stacksize * sizeof(STACKELEM));    /* CS */
  Pm_space   = (char *) malloc(I_stacksize * sizeof(STACKELEM));

  Tilde_space   = (char *) malloc(I_stacksize * sizeof(STACKELEM));

  Pm_space   = (char *) malloc(I_stacksize * sizeof(STACKELEM));


  if (E_space == (char *) NULL)    /* Pruefung ob Platz wirklich da */
    post_mortem("Stackmanager: Unable to allocate stackspace for E-Stack");

  if (A_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for A-Stack");

  if (M_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for M-Stack");

  if (I_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for I-Stack");

  if (V_space == (char *) NULL)                               /* CS */
    post_mortem("Stackmanager: Unable to allocate stackspace for V-Stack");

  if (M1_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for M1-Stack");
  if (Pm_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for Pm-Stack");

  if (Tilde_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for Tilde-Stack");

  if (Pm_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for Pm-Stack");


  if (Hilf_space == (char *) NULL)
    post_mortem("Stackmanager: Unable to allocate stackspace for Hilf-Stack");

        /* Initialisierung der Stackdeskriptoren */
  S_e.TopofStack    = S_e.MarBotPtr    = (STACKELEM *) E_space;
  S_a.TopofStack    = S_a.MarBotPtr    = (STACKELEM *) A_space;
  S_m.TopofStack    = S_m.MarBotPtr    = (STACKELEM *) M_space;
  S_m1.TopofStack   = S_m1.MarBotPtr   = (STACKELEM *) M1_space;
  S_hilf.TopofStack = S_hilf.MarBotPtr = (STACKELEM *) Hilf_space;
  S_i.TopofStack    = S_i.MarBotPtr    = (STACKELEM *) I_space;
  S_v.TopofStack    = S_v.MarBotPtr    = (STACKELEM *) V_space;        /* CS */
  S_pm.TopofStack    = S_pm.MarBotPtr    = (STACKELEM *) Pm_space;

  S_tilde.TopofStack    = S_tilde.MarBotPtr    = (STACKELEM *) Tilde_space;

  S_pm.TopofStack    = S_pm.MarBotPtr    = (STACKELEM *) Pm_space;


  /* da der V-Stack nur in der Preprocessingphase, und der I-Stack nur in
     der Pro- sowie Postprocessingphase gebraucht wird bietet sich an, dass
     der Platz des V-Stack durch den I-Stack mitgebraucht wird.
     diese Optimierung wurde heute entfernt   10.06.88                  * CS *
   */

  /* die Speicherzelle auf die ....MarTopPtr zeigt, gehoert noch zum Stack */
  S_e.MarTopPtr    = S_e.MarBotPtr    + (Q_stacksize - 1);
  S_a.MarTopPtr    = S_a.MarBotPtr    + (Q_stacksize - 2);
  S_m.MarTopPtr    = S_m.MarBotPtr    + (M_stacksize - 1);
  S_i.MarTopPtr    = S_i.MarBotPtr    + (I_stacksize - 1);
  S_v.MarTopPtr    = S_v.MarBotPtr    + (I_stacksize - 1);
  S_m1.MarTopPtr   = S_m1.MarBotPtr   + (M_stacksize - 1);
  S_hilf.MarTopPtr = S_hilf.MarBotPtr + (Q_stacksize - 1);
  S_pm.MarTopPtr    = S_pm.MarBotPtr    + (I_stacksize - 1);

  S_tilde.MarTopPtr    = S_tilde.MarBotPtr    + (I_stacksize - 1);

  S_pm.MarTopPtr    = S_pm.MarBotPtr    + (I_stacksize - 1);


  strcpy(S_e.Name,"e");
  strcpy(S_a.Name,"a");
  strcpy(S_v.Name,"v");
  strcpy(S_i.Name,"i");
  strcpy(S_tilde.Name,"tilde");
  strcpy(S_m.Name,"m");
  strcpy(S_m1.Name,"m1");
  strcpy(S_hilf.Name,"hilf");
  strcpy(S_pm.Name,"pm");

  strcpy(S_pm.Name,"pm");


#if DEBUG        /* dem Ueberwachungssystem wird die Schaffung der Stacks */
                 /* mitgeteilt.                                           */
  display_stack(&S_e,CREATE, (STACKELEM) 0);
  display_stack(&S_a,CREATE, (STACKELEM) 0);
  display_stack(&S_m,CREATE, (STACKELEM) 0);
  display_stack(&S_i,CREATE, (STACKELEM) 0);
  display_stack(&S_tilde,CREATE, (STACKELEM) 0);
  display_stack(&S_v,CREATE, (STACKELEM) 0);
  display_stack(&S_hilf,CREATE, (STACKELEM) 0);
  display_stack(&S_pm,CREATE, (STACKELEM) 0);
  display_stack(&S_m1,CREATE, (STACKELEM) 0);
  display_stack(&S_pm,CREATE, (STACKELEM) 0);

#endif

#if DEBUG
  printf(" *** Init Stack  ok! *** \n\r");
#endif
}

/****************************************************************************/
/*                                                                          */
/*  F O R M A T S T A C K :  Reinitialisiert den Stackmanager .             */
/*                                                                          */
/****************************************************************************/

void FormatStack()
{
  S_e.TopofStack = S_e.MarBotPtr;  /* zuruecksetzen der Pointer */
  S_a.TopofStack = S_a.MarBotPtr;
  S_v.TopofStack = S_v.MarBotPtr;
  S_i.TopofStack = S_i.MarBotPtr;
  S_m.TopofStack = S_m.MarBotPtr;
  S_m1.TopofStack = S_m1.MarBotPtr;
  S_hilf.TopofStack = S_hilf.MarBotPtr;
  S_pm.TopofStack = S_pm.MarBotPtr;
  S_tilde.TopofStack = S_tilde.MarBotPtr;
  S_pm.TopofStack = S_pm.MarBotPtr;

#if DEBUG        /* die Stacks beim Ueberwachungssystem anmelden */
  display_stack(&S_e,CREATE, (STACKELEM) 0);
  display_stack(&S_a,CREATE, (STACKELEM) 0);
  display_stack(&S_m,CREATE, (STACKELEM) 0);
  display_stack(&S_i,CREATE, (STACKELEM) 0);
  display_stack(&S_tilde,CREATE, (STACKELEM) 0);
  display_stack(&S_v,CREATE, (STACKELEM) 0);
  display_stack(&S_hilf,CREATE, (STACKELEM) 0);
  display_stack(&S_pm,CREATE, (STACKELEM) 0);
  display_stack(&S_m1, CREATE, (STACKELEM)0);
  display_stack(&S_pm,CREATE, (STACKELEM) 0);
  printf(" *** Reinit Stack ok!*** \n\r");
#endif
}

                              /*$P */
/****************************************************************************/
/*                                                                          */
/*  Die ueberallhin exportierten Stackoperationen :                         */
/*    readstack, writestack, pushstack, popstack, sizeofstack.              */
/*                                                                          */
/****************************************************************************/

/*            Im Debugfall sind die Stackoperationen Funktionen !             */
/* Die Makros muessen leider etwas anders aussehen als die Funktionen.        */
/* Das liegt daran, dass die Funktionen nur mit Zeigern auf Stackdeskriptoren */
/* und nicht mit den Deskriptoren selbst arbeiten koennen                     */
#if DEBUG

/*--------------------------------------------------------------------------
 * readstack -- gibt das oberste Stackelement zurueck.
 *--------------------------------------------------------------------------
 */
STACKELEM readstack(Onstack)
register StackDesc * Onstack;
{
  if ((*Onstack).TopofStack <= (*Onstack).MarBotPtr)
    stack_error("Stackmanager: Read on an empty Stack",Onstack->Name);
  display_stack(Onstack,READ, (STACKELEM) 0);
  return(*(*Onstack).TopofStack);
}

/*--------------------------------------------------------------------------
 * writestack -- ueberschreibt das oberste Stackelement mit "data".
 *--------------------------------------------------------------------------
 */
void writestack(Onstack,data)
register StackDesc * Onstack;
STACKELEM data;
{
  if ((*Onstack).TopofStack <= (*Onstack).MarBotPtr)
    stack_error("Stackmanager: Write on an empty Stack",Onstack->Name);
  *(*Onstack).TopofStack=data;
  display_stack(Onstack,WRITE,data);
}

/*--------------------------------------------------------------------------
 * read_arity -- gibt den Aritywert des obersten Stackelementes zurueck.
 *--------------------------------------------------------------------------
 */
unsigned short read_arity(Onstack)
register StackDesc * Onstack;
{
  if ((*Onstack).TopofStack <= (*Onstack).MarBotPtr)
    stack_error("Stackmanager: Read on an empty Stack",Onstack->Name);
  display_stack(Onstack,READ, (STACKELEM) 0);
#if ODDSEX
  return(ARITY(readstack(Onstack)));
#else
  return(*((unsigned short *)(*Onstack).TopofStack));
#endif
}

/*--------------------------------------------------------------------------
 * write_arity -- ueberschreibt den Arity-Wert des obersten Stackelementes.
 *--------------------------------------------------------------------------
 */
void write_arity(Onstack,data)
register StackDesc * Onstack;
unsigned short          data;
{
  if ((*Onstack).TopofStack <= (*Onstack).MarBotPtr)
    stack_error("Stackmanager: Write on an empty Stack",Onstack->Name);
#if ODDSEX
  writestack(Onstack,SET_ARITY(readstack(Onstack),((int)data)));
#else
  *(unsigned short *)(*Onstack).TopofStack=data;
#endif
  display_stack(Onstack,WRITE,*(Onstack->TopofStack));
}

/*--------------------------------------------------------------------------
 * popstack -- nimmt das oberste Stackelement vom Stack, und gibt es zurueck.
 *--------------------------------------------------------------------------
 */
STACKELEM popstack(Onstack)
register StackDesc * Onstack;
{
  if ((*Onstack).TopofStack <= (*Onstack).MarBotPtr)
    stack_error("Stackmanager: Pop on an empty Stack",Onstack->Name);
  (*Onstack).TopofStack--;
  display_stack(Onstack,POP, (STACKELEM) 0);
  return(*((*Onstack).TopofStack+1));
}

/*--------------------------------------------------------------------------
 * pushstack -- legt 'data' als oberstes Stackelement auf den Stack.
 *--------------------------------------------------------------------------
 */
void pushstack(Onstack,data)
register StackDesc * Onstack;
register STACKELEM data;
{
  if ((*Onstack).TopofStack >= (*Onstack).MarTopPtr)
     stack_error("stackmanager: Stackoverflow on Stack",Onstack->Name);
  else
     *(++((*Onstack).TopofStack)) = data;
#if MEASURE
  messta((*Onstack).Name,(int)((*Onstack).TopofStack - (*Onstack).MarBotPtr));
#endif
  display_stack(Onstack,PUSH,data);
}

/*--------------------------------------------------------------------------
 * sizeofstack -- errechnet die Anzahl der Stackelemente auf einem Stack.
 *--------------------------------------------------------------------------
 */
int sizeofstack(Onstack)
register StackDesc * Onstack;
{
  return((int)((*Onstack).TopofStack - (*Onstack).MarBotPtr));
}

#endif /* DEBUG */
/* end of file stack.c */

