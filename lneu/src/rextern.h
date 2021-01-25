/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
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
extern StackDesc S_e,S_a,S_m,S_m1,S_v,S_hilf,S_i,S_pm,S_tilde;
       /* der V-Stack wird nur in der Preprocessing Phase verwendet */
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
  extern post_mortem();            /* runtime:c */
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
extern PTR_DESCRIPTOR _dig0;      /*   <i>|<r>,<r>,<r>            */
extern PTR_DESCRIPTOR _dig1;      /*   <i>|<r>,<r>,<r>            */
extern PTR_DESCRIPTOR _work;      /*   <i>|<w>,<w>,<w>            */
extern PTR_DESCRIPTOR _digPI;     /* descriptor for number 'PI'   */
                                  /* (PI with relative error RGPI */
extern PTR_DESCRIPTOR _digLN2;    /* descriptor for number LN2          */
extern PTR_DESCRIPTOR _digRG;     /* relative error descriptor    */
#endif /* HEAPMANAGER */


#ifndef HEAPMANAGER
/* da DEC_REFCNT im nicht Debugfall ein Makro ist, erfolgt ein Tabellensprung
 * in jeder Procedur, die dieses Makro verwenden. Deshalb muessen die
 * deallocroutinen ueberall bekannt sein.
 */
extern int dealloc_sc();   /* alle in heap:c */
extern int dealloc_di();
extern int dealloc_li();
extern int dealloc_fi();
extern int dealloc_ex();
extern int dealloc_co();
extern FCTPTR dealloc_tab[]; /* in heap:c definiert */
#endif /* HEAPMANAGER */

extern unsigned short idcnt;     /* counter-variable for indirect descriptors */

extern unsigned short stdesccnt; /* counter-variable for the                  */
                                 /* number of "standard-descriptors"          */


extern int      _prec10;         /* REDUMA-divisionprecision with base 10     */
                                 /* (parameter from EDITOR)                   */
                                 /* used in rbibfunc.c to eval.               */
                                 /* sin(x), ... , ln(x)                       */
extern int      _prec10_mult;    /* REDUMA-multiplicationprecision with       */

