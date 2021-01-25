/* $Log: cparm.h,v $
 * Revision 1.6  1996/01/10  17:27:17  stt
 * RED_INFO command
 *
 * Revision 1.5  1995/11/27  09:28:48  stt
 * new entry ptr_start_dir
 *
 * Revision 1.4  1995/11/15  16:43:56  stt
 * new entry 'issubexpr' in T_UEBERGABE
 *
 * Revision 1.3  1993/10/27  12:49:05  ach
 * #if D_DIST around RED_STOP removed
 *
 * Revision 1.2  1993/10/27  09:50:24  ach
 * ach 27/10/93: RED_STOP eingefuegt
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/**************************************************** File cparm.h  **/
/* Modul             :  CPARM.H                                      */
/* Aufgerufen von    :                                               */
/* Untermodule       :  keine                                        */
/* C - Routinen      :  keine                                        */
/* globale Variablen :  keine                                        */
/* Funktion          :  Spezifikation der Uebergabe - Parameter      */
/*********************************************************************/

/* Commands for redcmd in parmlist                                   */

#define RED_INIT   0x01                        /* initialize         */
#define RED_NP     0x02                        /* next page follows  */
#define RED_RD     0x04                        /* last page, reduce  */
#define RED_NRP    0x08                        /* next reduced page  */
#define RED_RS     0x10                        /* restart            */
#define RED_LRP    0x20                        /* last reduced page  */
#define RED_PRE    0x40                        /* preprocess         */
#define RED_POST   0x80                        /* postprocess        */
#define RED_PURE  0x100                        /* only ear           */
#define RED_IALL  0x200                        /* read GRAPH all     */
#define RED_OS    0x400                        /* OS_fire reduziert  */
#define RED_PRELD	0x0800			/* preload	*/
#define RED_UNLOAD	0x1000			/* unload	*/
#define RED_STOP  0x2000                       /* terminate reduma */
#define RED_INFO        0x4000          /* show information only available */
                                        /* to reduma.         stt 10.01.96 */
#define D_STOP	0x0001	/* beachte stops im ausdruck		*/
#define D_ERROR	0x0002	/* stop bei irreduziblen ausdruecken	*/
#define D_STOPPED	0x0100	/* stop ist aufgetreten		*/
#define D_ERRORED	0x0200	/* error ist aufgetreten	*/
#define IS_INFO(deb,x)	(((x) & deb) != 0)
#define SET_INFO(deb,x)	x |= (deb)
#define CLR_INFO(deb,x)	x &= ~(deb)

/* dg 22.05.92 : Changes_default, pi_red_plus */
#define P_PI_RED_PLUS     01
#define P_CHANGES_DEFAULT 02

#define CLR_PAR1(p,mask) (p)->add_par1  &= ~(mask)
#define SET_PAR1(p,mask) (p)->add_par1  |=  (mask)
#define GET_PAR1(p,mask) ((p)->add_par1 &   (mask))

#define ERR_LEN	80


/* typedef for parmlist                                              */

typedef struct
      {
        int      pg_size;                      /* page size          */
        int      redcmd ;                      /* reduce command     */
        unsigned redcnt;                       /* reduction counter  */
        int      precision ;                   /* refer to strdiv    */
        int      truncation ;                  /* refer to normiere  */
        int      heapdes;                      /* no. of heapdescr.  */
        int      heapsize;                     /* heapsize (byte)    */
        /* COMBINATOR     */
        int      qstacksize;                   /* e,a,h stack size   */
        int      mstacksize;                   /* m,m1  stack size   */
        int      istacksize;                   /* i     stack size   */
        int      formated;                     /* fix/var (0/1)      */
        int      base;                         /* for decimal arith. */
        int      beta_count;                   /* on / off           */
        int      errflag;                      /* error occured      */
                                       /* neu : Zeitmessung 05.05.88 */
        double   preproc_time;                 /* time used (sec.)   */
        double   process_time;                 /* time used (sec.)   */
        double   postproc_time;                /* time used (sec.)   */
        double   total_time;                   /* time used (sec.)   */

        char     errmes[ERR_LEN];              /* error mess		*/
        int      file_desc;         /* fuer preproc. ausdruck        */
	int	prec_mult;		/* precision fuer multiplikation	*/
	int	debug_info;		/* debug-info, kodierung s.o.		*/
	int	add_par1;		/* dg 22.05.92 Changes_default, pi_red_plus */
	int	add_par2;		/* for future use	*/
        int     issubexpr;              /* stt The expr. to be reduced is a */
                                        /*     subexpression.               */
        char   *ptr_start_dir;          /* stt Pointer to a string which  */
                                        /* contains the inital directory. */
      }
        T_UEBERGABE, * PTR_UEBERGABE ;

/* end of cparm.h  */
