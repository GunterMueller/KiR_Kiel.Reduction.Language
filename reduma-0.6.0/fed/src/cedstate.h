/*					cedstate.h	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* dieser file enthaelt die schnittstellen-definition	*/
/* fuer die editoren					*/

#define EDSTATE 1

/* fuer beide editoren	*/
struct Editstate {
  long		* Bottom_;
  T_UEBERGABE	Parms_;
  char		** argv_;
  int		Changes_,
		Counter_,
		init_,
		neu_,
		argc_,
		result_;
  long		* edit_;
};

typedef struct Editstate * P_EditState;

#ifndef EDITSTATE                    /* vorsicht !     */
extern P_EditState storeedst(),createdst();
extern void assgnparms();
#endif

/* aufruf des editors: belegung der variablen neu_	*/
#define ED_NEU		1	/* neuen editor				*/
#define ED_RED_FROM	2	/* rueckgabe eines ausdrucks von reduma	*/
#define ED_OLD		5	/* alten ausdruck aus backupstack holen	*/
#define ED_INIT_TTY	100	/* nur initialisieren des editors	*/
/* belegung der variablen init_				*/
/* <>0: reduma noch nicht initialisiert			*/
/* 0: reduma bereits initialisiert			*/

/* ergebnisse von editor() (als return oder ->result_) */
#define ED_EXIT  0
#define OK_NOTC  2
#define CALL_RED 3
#define SAVE_GRAPH 10
#define EXIT_GRAPH 11


/* end of       cedstate.h */

