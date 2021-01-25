/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/**********************************************************************/  
/*                                                                    */
/*  preproc.c   - external:      preproc;                             */
/*                                                                    */
/*        Aufgerufen durch:       reduce;                             */
/*                                                                    */
/**********************************************************************/


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include <fcntl.h>     


#if DEBUG
extern int showdisplay;            /* runtime:c */
#endif

double tvorher;
double tpre;
double tpro;
double tpos;
double tges;


/**********************************************************************/
/* Spezielle externe Routinen:                                        */
/**********************************************************************/

extern int       OS_send();        /* os-send  */    
extern STACKELEM st_expr();        /* storepro:c */
extern void n_ea_create();        /* nea-create.c  */



/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

int test_pre;
extern BOOLEAN _preproc ;          /* rstate.c */ 
 


/*********************************************************************/
/* preproc steuert die Umwandlung eines Ausdruckes in die interne    */
/* Darstellung der Reduktionsmaschine und das Schreiben dieses Aus-  */   
/* druckes in eine von einem Anwender spezifizierte Datei ( gekenn-  */   
/* zeichnet durch fd ).                                              */ 
/*********************************************************************/

void preproc(fd) 
int fd;
{
STACKELEM  x;   

#if  SINTRAN3
  extern double tused();
#define TIMER() tused()
#else
  extern long clock();
#define TIMER() (clock() / 1000000.0)
#endif
  tvorher = 0.;
  tpre    = 0.;
  tpro    = 0.;
  tpos    = 0.;
  tges    = 0.;

  START_MODUL("preproc");

  tvorher = TIMER();
  tvorher = TIMER();
  if (T_KLAA(READSTACK(S_e)))    /* verpointertes Wiederaufsetzen. */
    post_mortem("preproc: wegschreiben bereits verpointerter Ausdruecke nicht moeglich");
  else
  {

/***********************  Preprocessing  *****************************/

#if DEBUG
    showdisplay = TRUE;
    setdisplay();                                 /* Stackbildaufbau */
    monitor_call();                    /* Benutzer kann interagieren */
#endif
      
    test_pre=0;
    _preproc=TRUE;
    ea_create();               /* Steuermodul der Preprocessingphase */
    _preproc=FALSE;

  tpre = TIMER();

  if (test_pre == 0)
  {
   /* Ausdruck enthaelt keine freien Variablen => */
   /* Ausdruck in Datei abspeichern               */
            
     if ((x = (STACKELEM)st_expr()) != NULL)
     {

       if ( OS_send( fd, x) <= 0 )
          post_mortem(" preproc : can't write preproc. expression to file "); 

     }
     else                                     /* kein Heapplatz mehr */
       post_mortem("preproc : heap out of space ");
  }
  else                           /* Ausdruck enthaelt freie Variable */
    post_mortem("preproc : Expression contains free variables "); 

  tpos = TIMER();
  tpos -= tpro;
  tpro -= tpre;
  tpre -= tvorher;
  tges = tpre + tpro + tpos;

#if DEBUG
  showdisplay = FALSE;       /* Stackbildmanipulation unterdruecken */
#endif
  END_MODUL("preproc");
  return;
}
}
