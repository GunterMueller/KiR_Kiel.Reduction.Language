/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*****************************************************************************/
/*                      MODUL DER POSTPROCESSING PHASE                       */
/*---------------------------------------------------------------------------*/
/*  ret-dec.c  - external:  ret_dec                                          */
/*  called by  - ae_retrieve;                                                */
/*  ret-dec.c  - internal:  keine                                            */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rdigit.h"


/*---------------------------------------------------------------------------*/
/*                          external declarations                            */
/*---------------------------------------------------------------------------*/
extern int _trunc;                   /* <r> , rstate.c */
extern int _prec;                    /* <w> , rstate.c */

extern PTR_DESCRIPTOR newdesc();     /* rheap.c        */
extern PTR_DESCRIPTOR digit_mul();   /* rdig-mul.c     */
extern PTR_DESCRIPTOR digit_div();   /* rdig-div.c     */
extern PTR_DESCRIPTOR digit_trunc(); /* rdig-v2.c      */
extern BOOLEAN        digit_eq();    /* rdig-v1.c      */

/*---------------------------------------------------------------------------*/
/*                               defines                                     */
/*---------------------------------------------------------------------------*/
#define TEST(desc)  if ((desc) == NULL) post_mortem("ret-dec: Heap out of space");

/* ist Zahl ganzzahlig ? */
#define T_INT_DIG(x)  ((R_DIGIT((*(x)),Exp)+1) >= R_DIGIT((*(x)),ndigit))

/* die letzte 10er Ziffer */
#define LAST_DIG(x) (((R_DIGIT((*(x)),Exp)+1) == R_DIGIT((*(x)),ndigit)) ? \
                     ((R_DIGIT((*(x)),ptdv))[0] % 10) : 0)

/*****************************************************************************/
/*                               ret_dec                                     */
/*---------------------------------------------------------------------------*/
/* Funktion :  Eine Zahl hinter einem Digit-Deskriptor wird als digitstring  */
/*             auf den E-Stack gelegt. Ueberfluessige Nachkommastellen werden*/
/*             abgeschnitten. Falls die interne Zahlenbasis ungleich 10000   */
/*             ist, so werden fuer diese Aufgabe die Digitroutinen benoetigt.*/
/*             Dadurch wird diese Routine fuer grosse Zahlen langsam.        */
/*                                                                           */
/* Paramter : desc - ein Digitdeskriptor                                     */
/*                                                                           */
/* Globale Variablen :                                                       */
/*****************************************************************************/
void ret_dec(desc)
PTR_DESCRIPTOR desc;
{
  PTR_DESCRIPTOR hilf_desc,hilf_desc2;
  int exp,sign;
#if DEBUG
  extern int debconf;
#endif

/*------------------*/
  if (BASE != 10000)
/*------------------*/
  {
    int save_prec = _prec;

    START_MODUL("ret_dec");

    /* Hilfsvariable intitialisieren */
    L_DIGIT((*_work),sign) = PLUS_SIGN;
    L_DIGIT((*_work),ndigit) = 1;
    if (BASE == 10)
    {
      L_DIGIT((*_work),Exp)  = 1;
      L_DIGIT((*_work),ptdv)[0] = 1;
    }
    else
    {
      L_DIGIT((*_work),Exp)  = 0;
      L_DIGIT((*_work),ptdv)[0] = 10;
    }

    _prec = 0;  /* Division wird gleich mit Truncation verbunden. */
    sign = R_DIGIT((*desc),sign);
    exp = 0;

    while (! T_INT_DIG(desc)) /* keine ganze Zahl */
    /* trunc bestimmt die Zahl der Nachkommastellen, falls trunc gleich Null
       so sollen alle Nachkommastellen ausgegeben werden.                   */
      if ((exp < _trunc) || (_trunc == 0))
      {
        exp++;
        TEST(hilf_desc = digit_mul(desc,_work));
        DEC_REFCNT(desc);
        desc = hilf_desc;
      }
      else
      {
        /* Rest abschneiden */
        TEST(hilf_desc = digit_trunc(desc));
        DEC_REFCNT(desc);
        desc = hilf_desc;
      }

    while ((exp > 0) && (LAST_DIG(desc) == 0)) /* normieren */
    /* beispielsweise soll kein 12.2000 ausgegeben werden.  */
    {
      exp--;
      TEST(hilf_desc = digit_div(desc,_work));
      DEC_REFCNT(desc);
      TEST(hilf_desc2 = digit_trunc(hilf_desc));     /* trunc ist leider nicht */
      DEC_REFCNT(hilf_desc);                         /* garantiert */
      desc = hilf_desc2;
    }

    if (exp == 0)         /* die Behandlung der Stellen einer ganzen Zahl */
      PUSHSTACK(S_e,DECPT);        /* erfolgt erst spaeter.               */
    else  /* Behandlung der Nachkommastellen: */
    {
      int n;
      /* letzte Ziffer auf den Stack legen */
      n = LAST_DIG(desc);
      PUSHSTACK(S_e,SWITCH(DIGSTACK(n)));

      /* restliche Nachkommastellen auf den Stack legen */
      for( ; ; )
      {
        exp--;
        TEST(hilf_desc = digit_div(desc,_work));
        DEC_REFCNT(desc);
        TEST(hilf_desc2 = digit_trunc(hilf_desc));
        DEC_REFCNT(hilf_desc);
        desc = hilf_desc2;
        if (exp == 0) break;
        n = LAST_DIG(desc);
        PUSHSTACK(S_e,DIGSTACK(n));
      }

      /* den Dezimalpunkt auf den Stack legen */
      PUSHSTACK(S_e,SWITCH(DECPT));
    } /* Ende der Behandlung der Nachkommastellen */

    /* Behandlung der Stellen vor dem Komma */
    do
    {
      int n;
      n = LAST_DIG(desc);
      PUSHSTACK(S_e,DIGSTACK(n));

      TEST(hilf_desc = digit_div(desc,_work));
      DEC_REFCNT(desc);
      desc = hilf_desc;
    }
    while (!digit_eq(desc,_dig0));

    /* zuletzt das Vorzeichen */
    PUSHSTACK(S_e, (sign ? SET_ARITY(MINUS,1) : SET_ARITY(PLUS,1)));
    DEC_REFCNT(desc);
    _prec = save_prec;
    END_MODUL("ret_dec");
    return;
  }

/*-------- BASE == 10000 ---*/
  else
/*--------------------------*/
  {
    int ndigit, count, countmax, i, j, k ,rest;
    /* int correct = 0;       siehe Zeile 246 */
    int zeiger = 0;
    int *data,digit;

    START_MODUL("ret_dec");
    exp = R_DIGIT((*desc),Exp);
    ndigit = R_DIGIT((*desc),ndigit);
    data = (int*) R_DIGIT((*desc),ptdv);
    countmax = ndigit - 1;
    j = (exp + 1) * 4 + _trunc;    /* Anzahl zu druckender Ziffern */
    if (j <= 0)
    {
       PUSHSTACK(S_e,DECPT);
       PUSHSTACK(S_e,DIGSTACK(0));
       PUSHSTACK(S_e, (R_DIGIT((*desc),sign) ? SET_ARITY(MINUS,1) : SET_ARITY(PLUS,1)));
       DEC_REFCNT(desc);
       END_MODUL("ret_dec");
       return;
    }
    k = j / 4 + 1;
    rest = j % 4;
    if (rest == 0)
    {
       rest = 4;
       k--;
    }
    if (ndigit >= k)
       count = ndigit - k;
    else
    {
       count = 0;
       rest = 4;
    }
#if DEBUG
    if (debconf)
    {
       printf("**********   ret_dec   **********\n\n");
       printf("exp: %d, ndigit: %d, countmax: %d, count: %d\n",exp,ndigit,countmax,count);
       printf("j: %d, k: %d, rest: %d\n",j,k,rest);
    }
#endif

_nochmal:
    if (countmax - count > exp  &&  countmax >= count)
    {
#if DEBUG
       if (debconf)
          printf("Wert: %d,  count: %d\n",data[count],count);
#endif
       digit = data[count];
       PUSHSTACK(S_hilf,  DIGSTACK(digit/1000));
       digit %= 1000;
       PUSHSTACK(S_hilf, DIGSTACK(digit/100));
       digit %= 100;
       PUSHSTACK(S_hilf, DIGSTACK(digit/10));
       digit %= 10;
       PUSHSTACK(S_hilf, DIGSTACK(digit));

       i = 4;
       while (i > rest)
       {
          PPOPSTACK(S_hilf);
          i--;
       }
       while ( i >= 1  &&  DIGVAL(READSTACK(S_hilf)) == 0)
       {
          PPOPSTACK(S_hilf);
          i--;
       }
       if (i == 0)
       {
          count++;
          rest = 4;
       /*   correct++;        siehe Zeile 299 */
          goto _nochmal;
       }
       zeiger = 1;
       if (i > 0)
          WRITESTACK(S_hilf,SWITCH(READSTACK(S_hilf)));
       for ( ; i>=1; i--)
       {
          PUSHSTACK(S_e,POPSTACK(S_hilf));
       }


       count++;

       while ((countmax - count) > exp)
       {
#if DEBUG
       if (debconf)
          printf("Wert: %d,  count: %d\n",data[count],count);
#endif
          if (countmax >= count)
          {
             digit = data[count];
             for (i=1; i<=4; i++)
             {
                PUSHSTACK(S_e,DIGSTACK(digit % 10));
                digit /= 10;
             }
             count++;
          }
          else
          {
             for (i=1; i<=4; i++)
                 PUSHSTACK(S_e,DIGSTACK(0));
             count++;
          }
       }
    }

    if (count == countmax + 1)
    {
       if (zeiger)
          PUSHSTACK(S_e,SWITCH(DECPT));
       else
          PUSHSTACK(S_e,DECPT);
       PUSHSTACK(S_e,DIGSTACK(0));
       PUSHSTACK(S_e, (R_DIGIT((*desc),sign) ? SET_ARITY(MINUS,1) : SET_ARITY(PLUS,1)));
       DEC_REFCNT(desc);
       END_MODUL("ret_dec");
       return;
    }
    else
    {
/*    if (count > correct)         */
      if (zeiger)
         PUSHSTACK(S_e,SWITCH(DECPT));
      else
         PUSHSTACK(S_e,DECPT);
    }

                                        /* Nachkommastellen abgeschlossen */
    while (exp >= ndigit)
    {
       for (i=1; i <= 4; i++)
          PUSHSTACK(S_e,DIGSTACK(0));
       exp-- ;
    }

    while (count <= countmax)
    {
#if DEBUG
       if (debconf)
          printf("Wert: %d,  count: %d\n",data[count],count);
#endif
       digit = data[count];
       for (i = 1; i <= 4; i++)
       {
          PUSHSTACK(S_e,DIGSTACK(digit%10));
          digit /= 10;
       }
       count++;
    }
    while (DIGVAL(READSTACK(S_e)) == 0)
    {
       PPOPSTACK(S_e);
    }
    if T_DECPT(READSTACK(S_e))   /* die Zahl selbst eine Null war */
      PUSHSTACK(S_e,DIGSTACK(0));

    PUSHSTACK(S_e, (R_DIGIT((*desc),sign) ? SET_ARITY(MINUS,1) : SET_ARITY(PLUS,1)));
    DEC_REFCNT(desc);
    END_MODUL("ret_dec");
  }
}
/**********************  end of file rret-dec.c ******************************/
