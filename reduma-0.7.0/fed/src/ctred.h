/* include file fuer transformation/reduktion          ctred.h	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    10.06.88	*/

#if LETREC
#define P_DEF  0		/* positionen bei aufruf von redc	*/
#define P_GOAL 1		/* auf goal	*/
#define I_GOAL 2		/* im goal	*/
#define P_FUNC 3		/* in funktion	*/
#endif

/* achtung: fehler-feld in redc	*/
/* bei < -1 : fehler, d.h. abbruch	*/
/* bei >  1 : warning, d.h. reduzieren	*/

#define UND_FIL	(2)
#define EXCALL	(3)
#define EXPLACE	(-4)
#define EXNOPAR	(-5)
#define WRONGCE	(-6)
#define EXDOPP	(-7)
#define DEFPART	(-8)
#define OVERKL	(-9)
#define OVERAL	(-10)
#define OVERST	(-11)
#define TMLEVEL	(-12)
#define TMLOCAL	(-13)
#define FCTLEN	(-14)
#define FCTGL	(-15)
#define FCTLOC	(-16)
#define TMFCT	(-17)
#define TMPFCT	(-18)
#define WR_WHEN	(-19)
#define NO_M_PATT	(-20)

/* interne symbole	*/
#define ENDSYMB         0x00230003L		/* end-symbol for travers #	*/
#define VARIABLE        0x00400003L		/* nil-atom of svl      @	*/
#define SLC             0x000200ffL		/* list-constr. of svl	*/
#define _SLC            0x000000ffL		/* vorsicht: vgl. stelemdef.h !	*/

/* markierungen fuer transformation in edit	*/
#define MARKE   0x00008000L		/* markierung der ce (bit in edit)	*/
#define PARAM   0x4000		/* fuer kennzeichnung von parametern	*/
/* fktsnr und param in naechsten stackelement (per chain)	*/
#define FCN     30		/* fuer free: freie Variablen ist fcn	*/
#define FKT     28		/* rec stammt von funktion (s.u. notprot)	*/
#define ISREC   26		/* ist recursiver aufruf	*/
/* hier wichtig	*/
#define N_FKT   200		/* max. anzahl von funktionen ( <= 2^15-1 s.u.)	*/

#define INF_FIELD       0x00001f00L		/* informationsfeld	*/
#define O_INF_FIELD              8
#define P_INF_FIELD     0x0000001fL
#define inform(m)       (((m) >> O_INF_FIELD) & P_INF_FIELD)
#define toinf(m)        (((STACKELEM)(m) & P_INF_FIELD) << O_INF_FIELD)
#define noinf(m)        ((m) & ~INF_FIELD)
/* fuer funktionsnummern	*/
#define CHAIN           0x00002000L		/* chain-flag fuer t/fred	*/
#define chained(m)      (((m)& CHAIN) != 0L)
#define unchained(m)    (((m)& CHAIN) == 0L)
#define setchain(m)     ((m)| CHAIN)
#define clrchain(m)     ((m)& ~CHAIN)
/* falls chain == 1: fktsnr ex. und ist im naechsten stackelement	*/
#define FKTSNR          0x00007fffL		/* 16-bit mit vorzeichen (int)	*/
#define getfnr(m)       ((int)((m) & FKTSNR))		/* fast ueberfluessig	*/
#define tofnr(n)        ((STACKELEM)((n) & FKTSNR))
#define toparam(m)      ((m)| PARAM)
#define noparam(m)      ((m)& ~PARAM)
#define isparam(m)      (((m)& PARAM) != 0L)
#define tofunkt(m)      ((m)| PARAM)
#define nofunkt(m)      ((m)& ~PARAM)
#define isfunkt(m)      (((m)& PARAM) != 0L)
#define ismce(m)        (((m)&MARKE) != 0L)		/* ist markierte ce	*/
#define markce(m)       ((STACKELEM)((m)|MARKE))		/* markiere ce	*/
#define notprot(ed)     ((ed) == FKT)		/* nicht schuetzen mit r3eb	*/
#define isbound(ed)     ((ed) == ISREC)		/* ist gebunden	*/
#define is_bind(el)     (class(el) == AR0(SUB) || class(el) == AR0(REC))
#define mark(el)        ((el) | CHAIN_FLAG)
#define unmark(el)      ((el) & ~CHAIN_FLAG)
#define is_marked(el)   (((el) & CHAIN_FLAG) != 0L)
#define isnf(i,j)       isset(nachf,i,j,(int)N_FKT)
#define isused(i,j)     isset(used_in,i,j,(int)N_FKT)
#define ruft(i,j)       isset(ruf,i,j,(int)N_FKT)

#define ziff(w) (((w)%10) + '0')

/* groesse des nachfolgerfeldes (dreiecksmatrix ohne diagonale)	*/
#define MNF     (((N_FKT*N_FKT) >> 1) >> 4)

/* groesse des nachfolger- und is-def_in-feldes fuer fred	*/
#define MATG    ((N_FKT*N_FKT) >> 4)
#define DIMFKT  (2*N_FKT)		/* dimension des fkt[]-feldes	*/

/* diese werte werden in beiden transformationen benutzt	*/
#define SLEN            2000		/* laenge des string-storage	*/
#define ALEN            200		/* laenge der alpha-konv.liste	*/
#define FNLEN           256		/* max. laenge eines funktionsnamens	*/
#define M_EBENEN        15		/* max. anzahl ebenen	*/
		/* achtung: siehe info !	*/
/* diese werte werden nur in fuer die hin-transformation benutzt	*/
#define FLEN            400		/* laenge der funktionsname-'liste'	*/
#define KLEN            400		/* laenge der konversions-liste	*/
#define PLEN            40		/* max. anzahl 'paralleler' fktionen	*/
#define PLENF           100		/* max. anzahl 'paralleler' fktionen	*/
#define DLENG           500		/* max. laenge globaler fktsnamen	*/
#define DLENL           500		/* max. laenge 'paralleler' fktsnamen	*/

#if !LETREC
/* datenstruktur fuer alpha-konversionen in tored	*/
struct a_list {
        int f_n_a,f_n_n,		/* verweise auf strstor: alter, neuer name	*/
            fnr;		/* nummer der funktion	*/
  };

/* konversionsliste: fktsnr, nr der alpha-konversion (in a_l)	*/
struct k_list {
        int f_nr,a_nr;
  };
#endif

#ifdef istr
#if !LETREC
struct a_list a_l[ALEN];
struct k_list k_l[KLEN];
int str_ind;
char strstor[SLEN];		/* string-storage	*/
#endif

/* fcount:fktszaehler, n_a,n_k: naechster index in a_l bzw k_l,	*/
/* str_ind:naechster index in strstor, fkt[]:verweis auf name in strstor	*/
int fkt[DIMFKT];
/* liste der 'aktiven' fkten, zaehler (fuer fred2, tored4)	*/
#if !LETREC
int flist[N_FKT],fcount,n_a,n_k,n_fl;
#endif

char *tormsg[] = {
   "das war wohl nix !"			/* existiert nicht !	*/
   ,"den fehler gibt's nicht ?"					/* -1	*/
   ,"Can't load file '%s' in expression"			/* -2	*/
   ,"Unknown function in tored"					/* -3	*/
   ,"Can't reduce #"						/* -4	*/
   ,"Exists NOPAR"						/* -5	*/
   ,"Illegal cursor-expression"					/* -6	*/
   ,"Double-defined function"					/* -7	*/
   ,"You can't reduce (in) the definition-part"			/* -8	*/
   ,"Internal error: konversion-list too small"			/* -9	*/
   ,"Internal error: alpha-konversion-list too small"		/* -10	*/
   ,"Internal error: strstor too small"				/* -11	*/
   ,"Internal error: Too much definition-levels"		/* -12	*/
   ,"Internal error: Too much local functions"			/* -13	*/
   ,"Internal error: Functionname too long"			/* -14	*/
   ,"Internal error: Global functionnames too long"		/* -15	*/
   ,"Internal error: Local functionnames too long"		/* -16	*/
   ,"Internal error: Too many functions"			/* -17	*/
   ,"Internal error: Too many parallel functions"		/* -18	*/
   ,"WHEN's are incompatible"					/* -19	*/
   ,"multiple pattern not allowed in this reduma-version"	/* -20	*/
  };
#else
# ifdef isft
#  if !LETREC
extern struct a_list a_l[];
extern struct k_list k_l[];
#  endif
extern int str_ind,fkt[];
extern char strstor[],*tormsg[];
#  if !LETREC
extern int flist[N_FKT],fcount,n_a,n_k,n_fl;
#  endif
# endif

/* makros zur entschluesselung von info (in fred)	*/
#define get_ebene(c)    ((c) & 0x0f)		/* ebene in info (Vorsicht !)	*/
#define toebene(c)      ((c) & 0x0f)
#define INTERNAL        0x10		/* kennzeichnung fuer interne fkten	*/
#define isintern(c)     (((c) & INTERNAL) != 0)
#define BEC_INT         0x20		/* kennzeichnung fuer wird interne fkten	*/
#define bec_int(c)      (((c) & BEC_INT) != 0)
#endif
/* end of      ctred.h	*/

