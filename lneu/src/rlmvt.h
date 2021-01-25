/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */

/*****************************************************************************/
/*                INCLUDE - FILE   FOR  STRUCTURING FUNCTIONS                */
/*---------------------------------------------------------------------------*/
/* rlmvt.h                                                                   */
/*                 rlstruct.c / rmstruct.c / rvstruct.c                      */
/*                                                                           */
/*            l == List   m == Matrix   vt == Vector/TVector                 */
/*            -           -             --                                   */
/*****************************************************************************/


/*---------------------------------------------------------------------------*/
/*                            extern variables                               */
/*---------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;          /* <r> , jump-adress for heapoverflow */
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
#if LARGE
extern int conv_digit_int();           /* rdig-v1.c  */
#endif 
extern PTR_DESCRIPTOR gen_many_id();   /* rconvert.c */

/*---------------------------------------------------------------------------*/
/*                            defines                                        */
/*---------------------------------------------------------------------------*/
#define DESC(x)  (* (PTR_DESCRIPTOR) x)

/* Makros zum Abfangen eines heapoverflows: */
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) longjmp(_interruptbuf,0)
#define TEST_DESC         if (_desc == 0)             longjmp(_interruptbuf,0)

/* Einlesen eines Strukturierungsparameters in eine Variable */
#if LARGE
#define STRUCT_ARG(ARG,VAR)         \
switch( R_DESC(DESC(ARG),class) ) { \
  case C_SCALAR:                    \
       if (R_DESC(DESC(ARG),type) == TY_INTEGER) \
         VAR = R_SCALAR(DESC(ARG),vali);         \
       else      \
         if ((VAR = R_SCALAR(DESC(ARG),valr)) != R_SCALAR(DESC(ARG),valr)) \
            goto fail; \
       break;  \
 case C_DIGIT: \
      if (conv_digit_int(ARG,&VAR)==0) goto fail; \
        break; \
 default:       goto fail; }
#else
#define STRUCT_ARG(ARG,VAR)         \
switch( R_DESC(DESC(ARG),class) ) { \
  case C_SCALAR:                    \
       if (R_DESC(DESC(ARG),type) == TY_INTEGER) \
         VAR = R_SCALAR(DESC(ARG),vali);         \
       else      \
         if ((VAR = R_SCALAR(DESC(ARG),valr)) != R_SCALAR(DESC(ARG),valr)) \
            goto fail; \
       break;  \
 default:       goto fail; }
#endif 

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
