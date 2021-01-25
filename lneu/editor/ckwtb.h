/* keyword-table                       ckwtb.h	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#define PREDEF   4	/* vordef. fkt		*/
#define PRINT    8	/* bei ausgabe naechsten eintrag benutzen	*/
#define PRIM_FKT 16	/* ist primitve fkt		*/
#define VERTIKAL 32	/* hat vertikale darstellung	*/
#define VERT1 64	/* hat vertikale darstellung	*/
#define VERT2 128	/* hat vertikale darstellung	*/

#define K_KLZU '}'	/* rechte klammer fuer us_kon	*/
#define C_KLZU ']'	/* rechte klammer fuer call	*/
#define F_KLZU ']'	/* rechte klammer fuer function-decl	*/
#define F_TRENN '='	/* trennzeichen fuer function-decl	*/
#define LBARSYMB '\\'	/* lambda-bar-symbol		*/
#define NO_PARC '@'	/* no-parameter-symbol		*/

/* ausdruckstyp-kodierungen	*/
#define PLBIT           0200	/* fuer ev. leere listen  (parameter)	*/
#define VERTBIT         0100	/* fuer vertikale darstellung von listen	*/
#define STRBIT          040	/* fuer strings				*/
#define CONT1BIT        020	/* ev. mit naechstem ausdruck weiter	*/
#define CONTBIT         010	/* ausdruck setzt sich mit naechstem fort	*/
#define LISTBIT         04	/* ist liste				*/
#define LIMBIT          02	/* displ: maxcol einhalten		*/
#define REMBIT          01	/* ldispl: folgt noch eine subexpr	*/
/* dann wird bei der listen-darstellung 1 von der arity abgezogen	*/
#define plbit(c)        (((c) & PLBIT) != 0)
#define vertbit(c)      (((c) & VERTBIT) != 0)
#define strbit(c)       (((c) & STRBIT) != 0)
#define cont1bit(c)     (((c) & CONT1BIT) != 0)
#define contbit(c)      (((c) & CONTBIT) != 0)
#define listbit(c)      (((c) & LISTBIT) != 0)
#define limbit(c)       (((c) & LIMBIT) != 0)
#define rembit(c)       ((c) & REMBIT)

struct kwrd {
 EXTYP etp;		/* auf welche ausdruecke passt 'keyword'	*/
 STACKELEM elem;	/* darstellung auf stack (zzgl. edit-info's	*/
 char *dsp;		/* darzustellender string (mit subexpr=ESC)	*/
 int rows,cols;		/* anzahl benoetigter zeilen und spalten	*/
 int kwtbinfo;		/* fuer version	*/
};

struct predtb {
  char *predfct;	/* bezeichnung der funktion	*/
  STACKELEM predelem;	/* kodierung	*/
  int pred_ar;		/* stelligkeit	*/
};
typedef struct predtb predefined;

#ifndef iskwrdtb

extern struct kwrd kwtb[],rkwtb[];
extern char *kwrds[];
extern int kwtblen,rkwtblen,nkwrds,nfunct;
extern predefined functions[];

#endif	/* iskwrdtb	*/

/* ist eingabe mit kwtbinfo (=i) erlaubt ?	*/
#if DORSY	/*	*/
#define allowed(i)       TRUE
#else
#define allowed(i)       (((i) & edit_vers) == 0)
#endif
/* ist eintrag eine vordefinierte Funktion ?	*/
#define is_predef(i)	(((i) & PREDEF) != 0)
#define is_print(i)	(((i) & PRINT) != 0)
#define is_prim(i)	(((i) & PRIM_FKT) != 0)
#define is_vert(i)	(((i) & VERTIKAL) != 0)
#define is_vert1(i)	(((i) & VERT1) != 0)
#define is_vert2(i)	(((i) & VERT2) != 0)

/* end of      ckwtb.h	*/
