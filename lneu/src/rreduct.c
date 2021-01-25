/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*----------------------------------------------------------------------------
 * reduct -- steuert die drei Phasen der Reduktionsmaschine, und sorgt
 *           fuer ein evtl. verpointertes Wiederaufsetzen.
 *           Der zu reduzierende Ausdruck wird auf dem E-Stack erwartet.
 *           Der Ergebnisausdruck wird auf den E-Stack zurueckgeliefert.
 *           Es erfolgt fuer die einzelnen Phasen eine Zeitmessung, die an den
 *           Editor uebergeben wird.
 * globals --
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 * reduct:c external -- reduct
 * reduct:c internal -- none
 *-----------------------------------------------------------------------------
 */

/* last update  by     why

   copied       by     to

*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "list.h"

#if DEBUG
extern int showdisplay;            /* runtime:c */
#endif
extern PTR_DESCRIPTOR _old_expr_p;
/* Pointer auf eine verpointerte Kopie, des zuletzt bearbeiteten Ausdrucks */
extern BOOLEAN         _preproc ;
#if LARGE
extern PTR_DESCRIPTOR  PRE_LIST;
#endif

double tvorher;
double tpre;
double tpro;
double tpos;
double tges;

/* calls --  */
extern _formated;
extern void ptrav_a_e();           /* trav:c */
extern STACKELEM st_expr();        /* storepro:c */
extern void ret_expr();            /* retrieve:c */

#if LARGE
extern void start_PRE_LIST();
#endif

extern void ea_create();           /* nea-create.c*/
extern void ear();                 /* ear:c      */
extern void ea_retrieve();         /* ea_ret:c   */
/*---------------------------------------------------------------------------*/
void reduct()
{
#if  SINTRAN3
  extern double tused();
#define TIMER() tused()
#else
  extern long clock();
#define TIMER() (clock() / 1000000.0)
#endif

#if LARGE
#else
  _formated=1;
#endif

  tvorher = 0.;
  tpre    = 0.;
  tpro    = 0.;
  tpos    = 0.;
  tges    = 0.;
                         
  _preproc=FALSE;

  START_MODUL("reduct");

   tvorher = TIMER();
   tvorher = TIMER();

  if (T_KLAA(READSTACK(S_e)))   /* verpointertes Wiederaufsetzen. */
  {
/*---------------------------------------------------------------------------*/
/*                  Verpointertes Wiederaufsetzen                            */
/*---------------------------------------------------------------------------*/
    if (_old_expr_p == NULL)
      post_mortem("reduct: There is no preprocessed Expression.");
#if DEBUG
    showdisplay = FALSE;  /* Bildschirmmanipulationen werden unterdrueckt */
#endif
    PPOPSTACK(S_e);       /* Klammeraffen entfernen */
    ret_expr(_old_expr_p);     /* Ausdruck wird auf den E-Stack gelegt */
#if DEBUG
    showdisplay = TRUE;
    setdisplay();         /* Stackbildaufbau */
    monitor_call();       /* Benutzer kann interagieren */
#endif
  }
  else
  {
/*---------------------------------------------------------------------------*/
/*                        Preprocessing                                      */
/*---------------------------------------------------------------------------*/
#if DEBUG
    showdisplay = TRUE;
    setdisplay();         /* Stackbildaufbau */
    monitor_call();       /* Benutzer kann interagieren */
#endif
    _old_expr_p = NULL;        /* keine alter Ausdruck gespeichert */

#if LARGE
     start_PRE_LIST();     /* initialisieren von PRE_LIST, anz_PRE; */
#endif

      ea_create();       /* Steuermodul der Preprocessingphase */


    IS_SPACE(S_e,SIZEOFSTACK(S_a));
    PTRAV_A_E;            /* Ausdruck auf den E-Stack traversieren */
  }
  tpre = TIMER();

/*---------------------------------------------------------------------------*/
/*                              Processing                                   */
/*---------------------------------------------------------------------------*/

  ear();                  /* Steuermodul der Processing Phase */

  tpro = TIMER();

  /* das Kopieren des Ausdrucks fuer ein verpointertes Wiederaufsetzen. */
  if ((_old_expr_p = (PTR_DESCRIPTOR)st_expr()) != NULL)
  {
    INC_REFCNT(((PTR_DESCRIPTOR)_old_expr_p));   /* Refcount wird 2 */
    ret_expr(_old_expr_p);
  }
  else        /* kein Heapplatz mehr fuer ein verpointertes Wiederaufsetzen. */
  {
    IS_SPACE(S_e,SIZEOFSTACK(S_a));
    PTRAV_A_E;            /* Ausdruck auf den E-Stack traversieren */
  }
/*---------------------------------------------------------------------------*/
/*                              Postprocessing                               */
/*---------------------------------------------------------------------------*/

  ea_retrieve();          /* Steuermodul der Postprocessing Phase */

  tpos = TIMER();
  tpos -= tpro;
  tpro -= tpre;
  tpre -= tvorher;
  tges = tpre + tpro + tpos;

#if DEBUG
  showdisplay = FALSE;    /* Stackbildmanipulation unterdruecken */
#endif
  END_MODUL("reduct");
}
/* end of       rreduct.c */
