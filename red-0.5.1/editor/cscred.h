/* include file fuer cscred.c u.a.             cscred.h */

#define LINEMODE	FALSE
#define RAWMODE		TRUE

/* div. Koordinaten */
#define FAROW  3
#define FACOL  0
#define INROW  1
#define INCOL 10
#define MROW   0
#define MCOL  10
#define MMROW  0
#define MMCOL 59
#define LBUFLEN 1000                   /* NEU    */
#define INROWLEN (co-INCOL-1)          /* co statt COLS !         */

#define putceu() { if (!batch) fputc((char)class(topof(EADRST)),stdout); }
#define putce()  { if (!batch) { fputc((char)class(topof(EADRST)),stdout); fflush(stdout); }}
#define putrce() { if (!batch) putrev((char)class(topof(EADRST))); }

/* tastatur-codes */
#define PUT     0x01      /* CTRL-A         */
#define BOTTOM  0x02      /* CTRL-B         */
#define PICK    0x04      /* CTRL-D         */
#define _DELEOL 0x04      /* CTRL-D         */
#define PICK1   0x05      /* CTRL-E         */
#define _INSERT 0x05      /* CTRL-E         */
#define CMD     0x06      /* CTRL-F         */
#define _DOWN   0x06      /* CTRL-F         */
#define _GOAL   0x07      /* CTRL-G         */
#define _HOME   0x0e      /* CTRL-N         */
#define TRACEN  0x0f      /* CTRL-O         */
#define DISP    0x10      /* CTRL-P         */
#define DISPF   0x11      /* CTRL-Q         */
#define _RED1   0x12      /* CTRL-R         */
#define READ    0x1e      /* CTRL-^         */
#define WRITE   0x1f      /* CTRL-_         */

#define T_FKT ':'         /* fuer simulation von fkttasten    */
#define _COMMAND '\\'     /* fuer commandos                   */

#ifdef scred
 int srow,scol,freerows,lpos,lmxpos;
 char lbuf[LBUFLEN];
#else
 extern int srow,scol,freerows,lpos,lmxpos;
 extern char lbuf[];
 extern void waitkey();
#endif

extern int co,li;

/* end of      cscred.h */

