/* $Log: rstdinc.h,v $
 * Revision 1.2  1992/12/16  12:51:06  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



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
/*  UNIX        - UNIX-System   oder nicht                           */
/*  DORSY       - DORSY System  oder nicht (Dupl. von Fkt.Def.)      */
/*  PI_RED_PLUS - Compiled Version pi-red+                           */
/*  DEBUG       - Debug-Version oder nicht                           */
/*  MEASURE     - Messsystem mit einbinden?                          */
/*  SINTRAN3    - ND-specials ? ND: ODDSEX = 0                       */
/*  ODDSEX      - Orientierung von Strukturen ?                      */
/*  SCAVENGE    - Generation Scavenging Garbage Collector            */
/*  SCAV_DEBUG  - Generation Scavenging Garbage Collector            */
/*  RED_TO_NF   - Reduce to Normal Form (behind lambdas)             */
/*  UH_ZF_PM    - Ulli Hoffmanns ZF-Expressions and Pattern Matching */
/*                                                                   */
/*********************************************************************/

/* operating system unix / other */
#ifndef UNIX
#define UNIX 1
#endif
                                    /* V fehlte (?) TB & RS, 28.10.92 */
/* operating system sintran3 (horror!) */ 
#ifndef SINTRAN3
#define SINTRAN3 0
#endif

/* DORSY-System, nur dieses wird noch unterst"utzt (dg.27.04.92) */
#ifndef DORSY
#define DORSY 1
#endif

/* compiled version (pi-red+) of the reduction machine */
#ifndef PI_RED_PLUS
#define PI_RED_PLUS 1
#endif

/* big endian (0) / little endian (1) addressing */
#ifndef ODDSEX
#define ODDSEX 0
#endif

/* debug / nodebug version */
#ifndef DEBUG
#define DEBUG 0
#endif

/* Messystem, veraltet, nicht mehr gepflegt (dg.27.04.92) */
#ifndef MEASURE
#define MEASURE 0
#endif

/* generation scavenging garbage collector */
#ifndef SCAVENGE
#define SCAVENGE 0
#endif

/* debug facilities for generation scavenging */
#ifndef SCAV_DEBUG
#define SCAV_DEBUG 0
#endif

/* reduce to normal form instead of head normal form */
#ifndef RED_TO_NF
#define RED_TO_NF 0
#endif

/* ulli hoffmanns zf-expressions and pattern matching */
#ifndef UH_ZF_PM
#define UH_ZF_PM 0
#endif

#define  T_POINTER(x)  (((int)(x) & 0x0000000F)==0)
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

#if nCUBE
typedef enum {IR_EXT, IR_WAIT, IR_DIST_END} INTER_RESULT;
#endif

#if DEBUG
          /* Codierungen fuer display_stack, welche Stackop durchgefuehrt wurde */
#define  POP 100
#define  PUSH 101
#define  WRITE 102
#define  READ  103
#define  CREATE 104
#define  UPDATE 105
#define  MID 106

#define START_MODUL(modulname)  start_modul(modulname)
#define   END_MODUL(modulname)    end_modul(modulname)
#else /* DEBUG */
#define START_MODUL(modulname)
#define   END_MODUL(modulname)
#endif /* DEBUG */

#if SCAVENGE
#if DEBUG
#define RES_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       res_heap(); \
                       disable_scav(); \
                       _heap_reserved = TRUE;}
#define REL_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       rel_heap();  \
                       enable_scav(); \
                       _heap_reserved = FALSE;}
#else
#define RES_HEAP     disable_scav()
#define REL_HEAP     enable_scav()
#endif /* DEBUG    auskommentiert von RS 30.11.1992 */

#else

#if (DEBUG || D_DIST ) 
#define RES_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       res_heap(); \
                       _heap_reserved = TRUE;}
#define REL_HEAP     { extern BOOLEAN _heap_reserved; /* state:c */ \
                       rel_heap();  \
                       _heap_reserved = FALSE;}
#else

#define RES_HEAP
#define REL_HEAP

#endif /* DEBUG || D_DIST*/
#endif /* SCAVENGE       auskommentiert von RS 30.11.1992 */

/****************************************************************************/
/* Makros zum Aufruf der verschiedenen Traversieralgorithmen                */
/****************************************************************************/

#define TRAV(Source,Sink)  trav(&Source,&Sink)

#define TRAV_E_A    {START_MODUL("trav_e_a");trav_e_a();END_MODUL("trav_e_a");}

#define TRAV_A_E    {START_MODUL("trav_a_e");trav_a_e();END_MODUL("trav_a_e");}
#define PTRAV_A_E    {START_MODUL("trav_a_e");ptrav_a_e();END_MODUL("trav_a_e");}

#define TRAV_A_HILF {START_MODUL("trav_a_hilf");trav_a_hilf();END_MODUL("trav_a_hilf");}
#define PTRAV_A_HILF {START_MODUL("trav_a_hilf");ptrav_a_hilf();END_MODUL("trav_a_hilf");}

/* Makros fuer die ANSI-Version - ach 01/02/93 */
 
#ifdef _ANSI_
#define VOLATILE volatile
#else
#define VOLATILE
#endif

