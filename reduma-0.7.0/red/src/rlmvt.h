/* $Log: rlmvt.h,v $
 * Revision 1.3  1995/05/22  09:32:24  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.2  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

/*****************************************************************************/
/*                INCLUDE - FILE   FOR  STRUCTURING FUNCTIONS                */
/*---------------------------------------------------------------------------*/
/* rlmvt.h                                                                   */
/*                 rlstruct.c / rmstruct.c / rvstruct.c                      */
/*                                                                           */
/*            l == List   m == Matrix   vt == Vector/TVector                 */
/*            -           -             --                                   */
/*****************************************************************************/


#if D_MESS
#include "d_mess_io.h"
#endif

/*---------------------------------------------------------------------------*/
/*                            extern variables                               */
/*---------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;          /* <r> , jump-adress for heapoverflow */
extern int _interrupt;
extern PTR_DESCRIPTOR _desc;           /* <w> , pointer to resultdescriptor  */

/* Ausserdem: S_e,S_a  <w>. Auf dem E-Stack liegt das Stackelement fuer      */
/* die Strukturierungsfunktion.j Auf dem A-Stack liegt das letzte Argument   */
/* dieser Funktion, welches im Reduktionsfall durch das Ergebnis ueber-      */
/* schrieben wird.                                                           */

/*---------------------------------------------------------------------------*/
/*                            extern functions                               */
/*---------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();       /* rheap.c    */
extern int newheap();                  /* rheap.c    */
extern int conv_digit_int();           /* rdig-v1.c  */
extern PTR_DESCRIPTOR gen_many_id();   /* rconvert.c */

/*---------------------------------------------------------------------------*/
/*                            defines                                        */
/*---------------------------------------------------------------------------*/
#define DESC(x)  (* (PTR_DESCRIPTOR) x)

/* see rheapty.h */
#ifndef NEWHEAP
#if (D_SLAVE && D_MESS && D_MHEAP)
#define NEWHEAP(size,adr) if ((*d_m_newheap)(size,adr) == 0) (_interrupt = 1,longjmp(_interruptbuf,0))
#else
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) (_interrupt = 1,longjmp(_interruptbuf,0))
#endif
#endif
#ifndef TEST_DESC
#define TEST_DESC         if (_desc == 0) (_interrupt = 1,longjmp(_interruptbuf,0))
#endif
#ifndef TESTDESC
#define TESTDESC(p)       if (p == 0)     (_interrupt = 1,longjmp(_interruptbuf,0))
#endif

/* Einlesen eines Strukturierungsparameters in eine Variable */
#define STRUCT_ARG(ARG,VAR)         \
  if (T_INT(ARG)) \
    VAR = VAL_INT(ARG); \
  else \
  if (T_POINTER((STACKELEM)ARG)) \
    switch( R_DESC(DESC(ARG),class) ) { \
      case C_SCALAR: /* REAL */         \
        if ((VAR = R_SCALAR(DESC(ARG),valr)) != R_SCALAR(DESC(ARG),valr)) \
          goto fail; \
        break;  \
      case C_DIGIT: \
       if (conv_digit_int(ARG,&VAR)==0) goto fail; \
         break; \
      default:       goto fail;\
    } \
  else goto fail

/* ein leerer String (_nilstring)  oder eine leere Liste (_nil) ? */
#define  NILSTRING_NIL(ARG)                    \
    switch(R_DESC(DESC(ARG),type)) {           \
      case TY_UNDEF : _desc = _nil;            \
                      INC_REFCNT(_nil);        \
                      break;                   \
      case TY_STRING: _desc = _nilstring;      \
                      INC_REFCNT(_nilstring);  \
                      break;                   \
    }                                          \
    goto success


/*********************** end of include file rlmvt.h *************************/
