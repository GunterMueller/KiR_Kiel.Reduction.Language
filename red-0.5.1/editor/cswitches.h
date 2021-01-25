/* include-file fuer den editor			cswitches.h	*/
/* letzte aenderung:	06.04.90				*/

/* hier werden die verschiedenen 'schalter' fuer den editor gesetzt	*/

/* default-betriebssystem: UNIX !!!!!!!!!!!	*/
#ifndef UNIX
#ifndef SINTRAN3
#define UNIX	1	/* VORSICHT !!!!	*/
#endif /* SINTRAN3 */
#endif /* UNIX */

#ifndef LETREC
#define LETREC 1
#endif
#ifndef DORSY
#define DORSY 1
#endif
#ifndef N_MAT
#define N_MAT 1
#endif
#ifndef N_STR
#define N_STR 1
#endif
#if N_STR
#ifndef N_U_STR
#define N_U_STR 1
#endif
#endif
#ifndef M_PATT
#define M_PATT 0
#endif
#ifndef LET_P
#define LET_P 1
#endif

#if UNIX
#define SINTRAN3 0
#endif

#if SINTRAN3 
#define UNIX 0
#define APOLLO 0
#define TRICK1_AP 0
#endif

#ifndef X_WINDOWS
#define X_WINDOWS	0
#endif
#ifndef KEDIT
#define KEDIT	0
#endif
/* fuer verschiedene versionen	*/
#ifndef TRACE
#define TRACE	0	/* fuers tracen der uebersetzungs-routinen	*/
#endif
#ifndef B_DEBUG
#define B_DEBUG 0	/* fuers tracen des backtracking (eingabe)	*/
#endif
#ifndef ClausR
#define ClausR 0	/* version fuer claus reincke		*/
#endif

#ifndef BATCH
#define BATCH 0		/* batch editor ?		*/
#endif


#define FULL	1	/* ctermio.c (required)	*/

/* end of	cswitches.h */
