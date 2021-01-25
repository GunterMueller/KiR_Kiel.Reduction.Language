/*
 * $Log: rextern.h,v $
 * Revision 1.10  2001/07/02 14:53:11  base
 * compiler warnings eliminated
 *
 * Revision 1.9  1997/02/17 16:07:21  stt
 * new ptr_instream, ptr_outstream
 *
 * Revision 1.8  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.7  1995/11/27  09:28:48  stt
 * declaration of *ptr_start_dir
 *
 * Revision 1.6  1995/11/15  16:43:56  stt
 * declaration of external _issubexpr
 *
 * Revision 1.5  1995/05/22  09:16:19  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.4  1994/06/15  11:11:06  mah
 * CloseGoal added
 *
 * Revision 1.3  1993/12/09  08:29:16  mah
 * S_tilde added
 *
 * Revision 1.2  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */




/****************************************************************************/
/* rextern.h                                                                */
/*  In dieser Datei stehen alle Variablen und Funktionen als extern         */
/*  deklariert, welche durch die meisten Module gebraucht werden.           */
/*  (D.h wenn sie nur in ein oder zwei Dateien benoetigt werden, sollten    */
/*   sie hier nicht stehen.                                                 */
/*                                                                          */
/****************************************************************************/

#ifndef STACKMANAGER
#ifndef HEAPMANAGER       /* der Heapmanager kennt keine Stacks ! */
#if D_SLAVE
extern DStackDesc D_S_H,D_S_A,D_S_I,D_S_R;
#else
extern StackDesc S_e,S_a,S_m,S_m1,S_v,S_hilf,S_i;
       /* der V-Stack wird nur in der Preprocessing Phase verwendet */
#if WITHTILDE
extern StackDesc S_tilde; /* mah 190993 */
extern int CloseGoal;     /* method of closing lrecs */
#endif /* WITHTILDE */

#endif /* D_SLAVE */
#endif /* HEAPMANAGER */
#endif /* STACKMANAGER */



#if DEBUG

  extern void start_modul();       /* breakpoint:c */
  extern void   end_modul();       /* breakpoint:c */

#ifndef STACKMANAGER
  /* Im Stackmanager sind diese Rountinen natuerlich nicht extern */
#ifndef HEAPMANAGER
  extern STACKELEM readstack();       /* stack:c */
  extern void writestack();           /* stack:c */
  extern unsigned short read_arity(); /* stack:c */
  extern void write_arity();          /* stack:c */
  extern STACKELEM popstack();        /* stack:c */
  extern void pushstack();            /* stack:c */
  extern int sizeofstack();           /* stack:c */
#endif /* HEAPMANAGER  */
#endif /* STACKMANAGER */
#endif /* DEBUG        */

#ifndef RUNTIME
  extern int post_mortem();            /* runtime:c */
#endif /* RUNTIME      */


#ifndef HEAPMANAGER
/* ---------------  permanent descriptors in heap  ---------------*/
extern PTR_DESCRIPTOR _nilmat;    /* <r> , pointer to permanent desc.   */
                                  /*       for empty matrix             */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _nilvect;   /* <r> , pointer to permanent desc.   */
                                  /*       for empty vector             */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _niltvect;  /* <r> , pointer to permanent desc.   */
                                  /*       for empty transposed vector  */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _nil;       /* <r> , pointer to permanent desc.   */
                                  /*       for empty list               */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _nilstring; /* <r> , pointer to permanent desc.   */
                                  /*       for empty string             */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _nilstruct; /* <r> , pointer to permanent desc.   */
                                  /*       for empty frame              */
                                  /* <i>|<r>,<r>,<r>                    */
extern PTR_DESCRIPTOR _dig0;      /*   <i>|<r>,<r>,<r>                  */
extern PTR_DESCRIPTOR _dig1;      /*   <i>|<r>,<r>,<r>                  */
extern PTR_DESCRIPTOR _work;      /*   <i>|<w>,<w>,<w>                  */
extern PTR_DESCRIPTOR _digPI;     /* descriptor for number 'PI'         */
                                  /* (PI with relative error RGPI       */
extern PTR_DESCRIPTOR _digLN2;    /* descriptor for number LN2          */
extern PTR_DESCRIPTOR _digRG;     /* relative error descriptor          */
#endif /* HEAPMANAGER */


#ifndef HEAPMANAGER
/* da DEC_REFCNT im nicht Debugfall ein Makro ist, erfolgt ein Tabellensprung
 * in jeder Procedur, die dieses Makro verwenden. Deshalb muessen die
 * deallocroutinen ueberall bekannt sein.
 */
extern int dealloc_sc();   /* alle in heap:c */
extern int dealloc_di();
extern int dealloc_li();
extern int dealloc_mvt();
extern int dealloc_ex();
extern int dealloc_co();
extern FCTPTR dealloc_tab[]; /* in heap:c definiert */
#endif /* HEAPMANAGER */

extern short idcnt;              /* counter-variable for indirect descriptors */

extern short stdesccnt;          /* counter-variable for the                  */
                                 /* number of "standard-descriptors"          */

extern int      _prec10;         /* REDUMA-divisionprecision with base 10     */
                                 /* (parameter from EDITOR)                   */
                                 /* used in rbibfunc.c to eval.               */
                                 /* sin(x), ... , ln(x)                       */
extern int      _prec10_mult;    /* REDUMA-multiplicationprecision with       */
                                 /* base 10 (parameter from EDITOR)           */

extern int      _issubexpr;      /* stt The expr. to be reduced is a subexpr. */
extern char     *_ptr_start_dir; /* stt Pointer to a string which contains    */
                                 /*     inital directory.                     */
extern char     *_ptr_instream;  /* stt 17.02.97    Source and target files   */
extern char     *_ptr_outstream; /*     for interactions stdIO                */

