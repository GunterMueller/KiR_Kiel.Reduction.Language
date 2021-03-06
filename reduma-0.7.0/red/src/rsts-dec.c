/* $Log: rsts-dec.c,v $
 * Revision 1.4  1995/05/22  11:29:30  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:51:12  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */





/*---------------------------------------------------------------------------*/
/* MODUL FOR ANYONE WHO NEEDS IT                                             */
/* rsts-dec:c - externals: sts_dec;   (STore String-DECimal)                 */
/*            - internals:  none;                                            */
/*---------------------------------------------------------------------------*/

#include <string.h>                    /* for strlen  routine */
#include <ctype.h>                     /* for isdigit routine */
#include <stdlib.h>                    /* for atoi, ach 30/10/92 */
#include "rstdinc.h"
#include "rheapty.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

/*---------------------------------------------------------------------------*/
/*                             extern functions                              */
/*---------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();       /* rheap:c     */
extern int newheap();                  /* rheap:c     */
/* base <> 10000 : */
extern PTR_DESCRIPTOR digit_add();     /* rdig-add:c  */
extern PTR_DESCRIPTOR digit_mul();     /* rdig-sub:c  */
extern PTR_DESCRIPTOR digit_div();     /* rdig-div:c  */
extern BOOLEAN digit_eq();             /* rdig-v1:c   */

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*---------------------------------------------------------------------------*/
/*                             extern variables                              */
/*---------------------------------------------------------------------------*/
extern int _prec;                        /* reduma division-precision        */

/*---------------------------------------------------------------------------*/
/*                               defines                                     */
/*---------------------------------------------------------------------------*/
#define NEWHEAP(size,adr)  if ( newheap(size,adr) == 0)                  \
                              post_mortem("sts_dec: Heap out of space");
#define INIT_DESC          if ((desc = newdesc()) == 0)                  \
                              post_mortem("sts_dec: Heap out of space");
#define TEST(desc)         if ((desc) == 0)                              \
                              post_mortem("sts_dec: Heap out of space");
#define IS_DIGIT(c)        isdigit(c)
#define IS_MINUS(c)        (((c) == '-') ? TRUE : FALSE)
#define READ_SIGN          (IS_MINUS(*string) ? (string++,MINUS_SIGN) : PLUS_SIGN)
#if DEBUG
#define READ_DIGIT         ((IS_DIGIT(x = *string))||      \
                            (x == '.')             ||      \
                            (x == '\0'))   ?               \
                           *string++ :                     \
                           (post_mortem("sts_dec: string is no valid decimal"))
#else
#define READ_DIGIT         (x = *string++)
#endif
#define CTOI(c)            ((c) -'0')




/*****************************************************************************/
/*                    sts_dec --  (store string-decimal)                     */
/* ------------------------------------------------------------------------- */
/*   function:  create digit-descriptor and corresponding heap-element       */
/*              from string and exponent;                                    */
/* parameters:  string - string-constant in valid decimal-fix-format         */
/*                       (without exponent nad pos. sign)                    */
/*              exp    - integer exponent                                    */
/*    returns:  pointer to descriptor                                        */
/*    example:  ptr = sts_dec("-1234.567",-12);                              */
/* ------------------------------------------------------------------------- */
/*       Note: Every  (successful)  call of  sts_dec  increments  the global */
/*       ===== variable stdesccnt (standard-descriptor counter)  because any */
/*             descriptor created by  sts_dec is permanent by definition. If */
/*             you want to release  any of these  you have to take care that */
/*             stdesccnt will be decremented appropriately.                  */
/*****************************************************************************/
PTR_DESCRIPTOR sts_dec(string,exp)
    register char *string;
    register int exp;
 {
    /* register int  ndigit;                          ach 30/10/92 */
    int  sign;
    char x;                    /* = head(string) (always set in READ_DIGIT!) */
    PTR_DESCRIPTOR desc = NULL;
    PTR_DESCRIPTOR hilf_desc;

    /* dg 13.05.92 START_MODUL("sts_dec"); */
    sign = READ_SIGN;                             /*   get sign from string  */

    /* ---- cut off leading zeros ---- */
    while ((READ_DIGIT) == '0')
       ;

    /*----------------------------------*/
    /* ----- BASE not equal 10000 ----- */
    /*----------------------------------*/
    if (BASE != 10000)
    {
      if (x == '\0')                              /* value of string is zero */
      {
        string -= 2;
        READ_DIGIT;                               /* store 0 in data vector! */
      }
      /* ---- initialize work-descriptor ---- */
      L_DIGIT((*_work),Exp) = 0;
      L_DIGIT((*_work),ndigit) = 1;
      L_DIGIT((*_work),sign) = PLUS_SIGN;

      if (x != '.')                               /* digit left-hand of point*/
      {
        INIT_DESC;                                /*  create new descriptor  */
        DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);       /* refcount = 1, so desc.  */
                                                  /* is never changed by     */
                                                  /* digit routines;         */
        L_DIGIT((*desc),Exp) = 0;
        L_DIGIT((*desc),ndigit) = 1;
        L_DIGIT((*desc),sign) = PLUS_SIGN;
        NEWHEAP(1,A_DIGIT((*desc),ptdv));
        L_DIGIT((*desc),ptdv)[0] = CTOI(x);          /*     first digit      */

        /* ---- get leaving digits up to decimal-point ---- */
        while (((READ_DIGIT) != '.')&&(x != '\0'))
        {
           L_DIGIT((*_work),ptdv)[0] = 10;
           TEST(hilf_desc = digit_mul(desc,_work));  /* multiply intermedium-*/
           DEC_REFCNT(desc);                         /* result with 10       */
           L_DIGIT((*_work),ptdv)[0] = CTOI(x);
           TEST(desc = digit_add(hilf_desc,_work));  /*    add next digit    */
           DEC_REFCNT(hilf_desc);
        }
      }
      if (x != '\0')                                 /* more dig. to be read */
      {
        /* ---- get leaving digits right of decimal-point ---- */
        while ((READ_DIGIT) != '\0')                 /* string not yet ended */
        {
          exp--;
          if (desc != NULL)                          /* desc. already created*/
          {
            L_DIGIT((*_work),ptdv)[0] = 10;
            TEST(hilf_desc = digit_mul(desc,_work)); /* multiply intermedium-*/
            DEC_REFCNT(desc);                        /* result with 10       */
            L_DIGIT((*_work),ptdv)[0] = CTOI(x);
            TEST(desc = digit_add(hilf_desc,_work)); /*    add next digit    */
            DEC_REFCNT(hilf_desc);
          }
          else /* desc == NULL */
            if (x != '0')                            /* otherwise there's no */
            {                                        /*need for new desc. yet*/
              INIT_DESC;                             /* create new descriptor*/
              DESC_MASK(desc,2,C_DIGIT,TY_DIGIT);    /* refcount = 2,so desc.*/
                                                     /* is never changed by  */
                                                     /* digit routines;      */
              L_DIGIT((*desc),Exp) = 0;
              L_DIGIT((*desc),ndigit) = 1;
              L_DIGIT((*desc),sign) = PLUS_SIGN;
              NEWHEAP(1,A_DIGIT((*desc),ptdv));
              L_DIGIT((*desc),ptdv)[0] = CTOI(x);
            }
        }
      }
      /********** all digits are read **********/


      L_DIGIT((*desc),sign) = sign;                  /* store sign in desc.  */

      /* ---- store expo. in desc. ---- */
      L_DIGIT((*_work),ptdv)[0] = 10;
      for ( ; exp > 0; exp--)
      {
        TEST(hilf_desc = digit_mul(desc,_work));
        DEC_REFCNT(desc);
        desc = hilf_desc;
      }

      for ( ; exp < 0; exp++)
      {
        int save_prec = _prec;
        /*-------------------------------------------------------------------*/
        /* What should be done, if (e.g.) _prec = 0 and 0.1 must be stored?  */
        /* Should  zero  value be stored?  I don't think so! We can expect,  */
        /* that  BASE is divisible  by 10 that means, the  amount of digits  */
        /* increases at most by one. Therefore:                              */
        /*------------------------                                       ----*/
        _prec = R_DIGIT((*desc),ndigit) + 1;   /* so we don't lose any digit */

        TEST(hilf_desc = digit_div(desc,_work));
        _prec = save_prec;                       /* restore reduma precision */
        DEC_REFCNT(desc);
        desc = hilf_desc;
      }

      INC_REFCNT(desc);

      stdesccnt++;
      /*---------------------------------------------------------------------*/
      /* Note: in stdesccnt the number of standard-descriptors is stored.Any */
      /* ===== descriptor created by  sts_dec is permanent by definition. If */
      /*       you want to release  any of these  you have to take care that */
      /*       stdesccnt will be decremented appropriately.                  */
      /*---------------------------------------------------------------------*/
      /* dg 13.05.92 END_MODUL("sts_dec"); */
      return((PTR_DESCRIPTOR) desc);
    }

    else

    /*---------------------------------*/
    /* ----- BASE   equal   10000 -----*/
    /*---------------------------------*/
    {
      register int i,                /*     index-variable for hlpstr        */
                   j,                /*          counter variable            */
                   dimv,
                   ndigit;
      int  *data/*, sum*/;           /* ach 30/10/92 */
      char hlpstr[130],              /* helpstring to convert number into    */
                                     /* base 10000                           */
           *substr;                  /* 4-digit extraction from hlpstr       */

      if (x == '\0')
        string -= 2;                 /*          restore 0 in string         */
      else
        string--;                    /* restore first non-zero digit; this   */
                                     /* is necessary  because  of  cut-off   */
                                     /* leading zeros.  (Digit  in fact is   */
                                     /* zero if zero is the only digit.)     */
      dimv = strlen(string);         /* length of string (incl. dec.-point)  */
      if (dimv > 125)
        post_mortem("sts_dec: try to create digit_string of illegal length");

      /* ---- get digits up to decimal-point ---- */
      for (i=0; ((READ_DIGIT) != '.')&&(x != '\0'); i++)
      {
         hlpstr[i] = x;
      }
      if (x != '\0')                            /*  more digits to be read   */
      {
         dimv--;                                /*   handle decimal-point    */

         /* ---- get leaving digits right to decimal-point ---- */
         for ( ; (READ_DIGIT) != '\0';i++)      /* string has not yet ended  */
         {
            hlpstr[i] = x;
            exp--;                              /*      adjust exponent      */
         }
      }
      /********** all digits are read **********/

      /* ---- cut off right-hand zeros ---- */
      while ((hlpstr[i-1] == '0') &&            /* do not decr. 'i' in this */
             (dimv > 1))                        /* statement, cause 'i' has */
                                                /* to be first free index ! */
                                                /* repl. 0 with term.symbol */
      {
         hlpstr[i] = '\0';
         dimv--;
         exp++;
         i--;
      }

      /* ---- adjust exponent ---- */
      if ((j = exp % 4) < 0) j += 4;
      for( ; j > 0; j--)
      {
        hlpstr[i++] = '0';
        dimv++;
        exp--;
      }
      hlpstr[i] = '\0';                         /*     terminate hlpstr !    */
      exp /=4;                                  /*        base = 10000       */

      ndigit = (dimv + 3) >> 2;                 /*    get number of digits   */

      /* ---- transform exponent from rightmost digit to leftmost digit ---- */
      exp += ndigit - 1;

      INIT_DESC;                                /*   create new descriptor   */
      DESC_MASK(desc,2,C_DIGIT,TY_DIGIT);       /* refcount = 2, so desc. is */
                                                /* never changed by digit    */
                                                /* routines;                 */
      L_DIGIT((*desc),sign)   = sign;
      L_DIGIT((*desc),Exp )   = exp;
      L_DIGIT((*desc),ndigit) = ndigit;
      NEWHEAP(ndigit, A_DIGIT((*desc),ptdv));
      data = (int*) R_DIGIT((*desc),ptdv);

      /* ---- create digits ---- */
      if ((substr = hlpstr+dimv-4) < hlpstr)   /* get last 4 digits of hlpstr*/
         substr = hlpstr;                      /* less than 4 digits availab.*/
      for ( j = 0; j < ndigit; j++)
      {
         data[j] = atoi(substr);               /* store digit with base 10000*/
         *substr = '\0';                       /* set end of next substring  */
         substr -= 4;                          /* get next 4 digits of hlpstr*/
         if (substr < hlpstr)                  /* less than four digits left */
            substr = hlpstr;
      }

      stdesccnt++;
      /*---------------------------------------------------------------------*/
      /* Note: in stdesccnt the number of standard-descriptors is stored.Any */
      /* ===== descriptor created by  sts_dec is permanent by definition. If */
      /*       you want to release  any of these  you have to take care that */
      /*       stdesccnt will be decremented appropriately.                  */
      /*---------------------------------------------------------------------*/
      /* dg 13.05.92 END_MODUL("sts_dec"); */
      return((PTR_DESCRIPTOR) desc);
    }
}
