/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */



/*
   rdigit.h
 */

/* last update  by     why

   copied       by     to

*/

#define DIGVAL(x)  (( (x) & 0x000f0000) >> 16)
    /* holt aus einem Ziffernstackelement die Ziffer heraus */

#define DIGSTACK(x)  (( (x) << 16) | 0x00300007)
 /* wandelt eine Ziffer in das zugehoerige Stackelement um */

#define PLUS_SIGN 0       /* fuer digit-Deskriptoren */
#define MINUS_SIGN 1

extern int _base;         /* das Zahlensystem fuer die Digitstrings */
extern int _maxnum;
#define BASE _base
#define MAXNUM _maxnum

#if DEBUG
#define DIGIT_TEST(x,y,d1,d2)  digit_test(x,y,d1,d2) /* Testroutine: ist der */
                                                     /* Deskriptor in Ordnung? */
#else
#define DIGIT_TEST(x,y,d1,d2)  x
#endif

#if DEBUG

#define DIGNULL   if (debconf) DescDump(stdout, _dig0, 1)
#define ABBRUCH   if (debconf) printf("\n\n**************  Berechnung abgebrochen ******\n")
#else
#define DIGNULL
#define ABBRUCH
#endif

#define DIGDESC(desc,ndigit)           \
      if ((desc = newdesc() ) == 0)    \
      {                                \
        ABBRUCH;                                  /* Meldung DEBUG-System */    \
        return((PTR_DESCRIPTOR)0); \
      }                             \
                                     \
      DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);  \
      if (newheap(ndigit, A_DIGIT((*desc),ptdv) ) == 0)       \
      {                                    \
         DEC_REFCNT(desc);                 \
         ABBRUCH;                          \
         return((PTR_DESCRIPTOR)0);        \
      }
