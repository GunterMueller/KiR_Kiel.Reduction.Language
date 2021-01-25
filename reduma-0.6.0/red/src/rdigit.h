/* $Log: rdigit.h,v $
 * Revision 1.3  1995/05/22  09:12:01  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.2  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*
   rdigit.h
 */

/* last update  by     why

   copied       by     to

*/


#define DIGVAL(x)  (VALUE(x) - '0')
    /* holt aus einem Ziffernstackelement die Ziffer heraus */

#define DIGSTACK(x)  SET_VALUE(DIG1,x + '0')
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

#if (D_SLAVE && D_MESS && D_MHEAP)
#define DIGDESC(desc,ndigit)           \
      if ((desc = (*d_m_newdesc)() ) == 0)    \
      {                                \
        ABBRUCH;                                  /* Meldung DEBUG-System */    \
        return((PTR_DESCRIPTOR)0); \
      }                             \
                                     \
      DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);  \
      if ((*d_m_newheap)(ndigit, A_DIGIT((*desc),ptdv) ) == 0)       \
      {                                    \
         DEC_REFCNT(desc);                 \
         ABBRUCH;                          \
         return((PTR_DESCRIPTOR)0);        \
      }
#else
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
#endif
