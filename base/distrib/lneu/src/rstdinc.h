

/*****************************************************************************/
/*  stdinc.h                                                                 */
/*  Diese Datei wird in jede Datei des Simulators einkopiert.                */
/*  Sie enthaelt ein Sammelsurium von Definitionen und Makros,               */
/*  die von allgemeinem Interesse sind.                                      */
/*                                                                           */
/*****************************************************************************/

/* last update  by     why

   copied       by     to

*/

#include <stdio.h>

/*********************************************************************/
/*  Die Schalter der Reduktionsmaschine sind:                        */
/*                                                                   */
/*  DEBUG   - Debug-Version oder nicht                               */
/*  MEASURE - Messsystem mit einbinden?                              */
/*  SINTRAN - ND-specials ?                                          */
/*  ODDSEX  - Orientierung von Strukturen ?                          */
/*            ND: ODDSEX = 0                                         */
/*********************************************************************/

#define  T_POINTER(x)  (((x) & 0x0000000F)==0)
/* Definition von T_POINTER wurde aus der stelemdef.h entfernt, damit
   nicht mit diesem Makro gleich die ganze Stackelementdefinition mit
   kopiert werden muss. */

/*********************************************************************/
/*  Standards                                                        */
/*********************************************************************/

#define BOOLEAN int
#define TRUE     1
#define FALSE    0

typedef int (* FCTPTR) ();

#if DEBUG
          /* Codierungen fuer display_stack, welche Stackop durchgefuehrt wurde */
#define  POP 100
#define  PUSH 101
#define  WRITE 102
#define  READ  103
#define  CREATE 104

#define START_MODUL(modulname)  start_modul(modulname)
#define   END_MODUL(modulname)    end_modul(modulname)
#else /* DEBUG */
#define START_MODUL(modulname)
#define   END_MODUL(modulname)
#endif /* DEBUG */

#if DEBUG
#define RES_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       res_heap(); \
                       _heap_reserved = TRUE;}

#define REL_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       rel_heap();  \
                       _heap_reserved = FALSE;}

#else

#define RES_HEAP

#define REL_HEAP

#endif /* DEBUG */

/****************************************************************************/
/* Makros zum Aufruf der verschiedenen Traversieralgorithmen                */
/****************************************************************************/

#define TRAV(Source,Sink)  trav(&Source,&Sink)

#define TRAV_E_A    {START_MODUL("trav_e_a");trav_e_a();END_MODUL("trav_e_a");}

#define TRAV_A_E    {START_MODUL("trav_a_e");trav_a_e();END_MODUL("trav_a_e");}
#define PTRAV_A_E    {START_MODUL("trav_a_e");ptrav_a_e();END_MODUL("trav_a_e");}

#define H_TRAV_A_HILF {START_MODUL("trav_a_hilf");h_trav_a_hilf();END_MODUL("trav_a_hilf");}
#define H_PTRAV_A_HILF {START_MODUL("trav_a_hilf");h_ptrav_a_hilf();END_MODUL("trav_a_hilf");}

#define TRAV_A_HILF {START_MODUL("trav_a_hilf");trav_a_hilf();END_MODUL("trav_a_hilf");}
#define PTRAV_A_HILF {START_MODUL("trav_a_hilf");ptrav_a_hilf();END_MODUL("trav_a_hilf");}
