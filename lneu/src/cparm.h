/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */




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

#define D_STOP	0x0001	/* beachte stops im ausdruck		*/
#define D_ERROR	0x0002	/* stop bei irreduziblen ausdruecken	*/
#define D_STOPPED	0x0100	/* stop ist aufgetreten		*/
#define D_ERRORED	0x0200	/* error ist aufgetreten	*/
#define IS_INFO(deb,x)	(((x) & deb) != 0)
#define SET_INFO(deb,x)	x |= (deb)
#define CLR_INFO(deb,x)	x &= ~(deb)


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

        char     *errmes;                      /* error mess max. 60 */
        int      file_desc;         /* fuer preproc. ausdruck        */
	int	prec_mult;		/* precision fuer multiplikation	*/
	int	debug_info;		/* debug-info, kodierung s.o.		*/
	int	add_par1;		/* for future use	*/
	int	add_par2;		/* for future use	*/
      }
        T_UEBERGABE, * PTR_UEBERGABE ;

/* end of cparm.h  */

