/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */



/*-----------------------------------------------------------------------------
 *  digit-viel2:c  - external:  digit_abs, digit_frac, digit_trunc, digit_ceil,
 *                              digit_floor, digit_neg
 *         Aufgerufen durch:  red_abs, red_frac, red_trunc, usw.
 *  digit-viel2:c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"


/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
extern PTR_DESCRIPTOR newdesc();            /* heap:c       */
extern PTR_DESCRIPTOR digit_add();          /* digit-add:c  */
extern PTR_DESCRIPTOR digit_sub();          /* digit-sub:c  */
#if DEBUG
extern PTR_DESCRIPTOR digit_test();         /* digit-v1:c   */
#endif

/*-----------------------------------------------------------------------------
 *  digit_abs  --  liefert den Absolutbetrag einer Digit-Zahl zurueck.
 *  globals    --  _digit_recycling  <r>                state:c
 *                 _dig0             <r>
 *                 debconf           <r>    (nur DEBUG) runtime:c
 *-----------------------------------------------------------------------------
 */

PTR_DESCRIPTOR digit_abs(desc)
PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR  res;
   register int i, *data1, *data3, ndigit;
   extern BOOLEAN _digit_recycling;
#if DEBUG
   extern int debconf;
#endif

   if  (R_DIGIT((*desc),sign) == PLUS_SIGN)     /* Zahl bleibt unveraendert */
   {
      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_abs",desc,0));
   }

   if (R_DESC((*desc),ref_count) == 1   &&   _digit_recycling)
   {
      INC_REFCNT(desc);                              /* alter Deskriptor */
      L_DIGIT((*desc),sign) = PLUS_SIGN;             /* wiederverwendbar */
      return(DIGIT_TEST(desc,"digit_abs",desc,0));   /* d.h. lediglich Vor- */
   }                                                 /* zeichen vertauschen */

   ndigit = R_DIGIT((*desc),ndigit);
   DIGDESC(res,ndigit);                     /* Deskriptor und Heapplatz anfordern */
   L_DIGIT((*res),sign) = PLUS_SIGN;
   L_DIGIT((*res),ndigit) = ndigit;
   L_DIGIT((*res),Exp) = R_DIGIT((*desc),Exp);
   RES_HEAP;
   data1 = R_DIGIT((*desc),ptdv);           /* lokale Heapzeiger */
   data3 = R_DIGIT((*res),ptdv);

   for ( i=0; i<ndigit; i++)                /* Zahl kopieren */
       data3[i] = data1[i];
   REL_HEAP;
   return(DIGIT_TEST(res,"digit_abs",desc,0));
}


/*
----------------------------------------------------------------------------
* digit_frac  --  schneidet bei einer Digit-Zahl alle Vorkommastellen ab.
*
* globals:    _digit_recycling    <r>       state:c
*             _dig0               <r>       heap:c
*             debconf             <r>       runtime:c   (nur DEBUG)
*/

PTR_DESCRIPTOR digit_frac(desc)
PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR res;
   register int i, *data1, *data3;
   int ndigit, exp;
   extern BOOLEAN _digit_recycling;                   /* state:c */

#if DEBUG
  extern int debconf;                       /* runtime:c */

  if (debconf) printf("\n\ndigit-frac gestartet\n");
#endif

   if ( (exp = R_DIGIT((*desc),Exp)) - (ndigit = R_DIGIT((*desc),ndigit)) >= -1)
   {
      INC_REFCNT(_dig0);           /* Zahl hat keine Nachkommastellen */
      return(_dig0);
   }

   if (exp < 0)
   {                               /* Zahl hat nur Nachkommastellen */
      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_frac",desc,0));
   }

/* jetzt muessen die obersten (= groessten) Ziffern abgeschnitten werden */

   if ( R_DESC((*desc),ref_count) == 1  &&  _digit_recycling)
   {                                        /* Deskriptor darf umgebaut werden */
      L_DIGIT((*desc),ndigit) = ndigit -= (exp + 1);

/* die oberen Ziffern werden abgeschnitten, indem einfach die Anzahl der Ziffern
   reduziert wird */
      L_DIGIT((*desc),Exp) = -1;
      RES_HEAP;
      while ((data3 = R_DIGIT((*desc),ptdv))[ndigit-1]  ==  0)  /* Normieren */
      {
         L_DIGIT((*desc),ndigit) = --ndigit;    /* Ziffernzahl verkleinern      */
         L_DIGIT((*desc),Exp)--;                /* die Ziffer mit dem kleinsten */
      }                                         /* Exponenten steht an der 0-ten */
      REL_HEAP;                                 /* Stelle. Die Vorkommaziffern  */
      INC_REFCNT(desc);                         /* bleiben, werden aber nicht   */
      return(DIGIT_TEST(desc,"digit_frac",desc,0));  /* mehr gebraucht          */
   }

   ndigit -= (exp + 1);                     /* neue Ziffernanzahl */
   exp = -1;
   DIGDESC(res,ndigit);                     /* Deskriptor und Heapplatz anfordern */

   RES_HEAP;
   data1 = R_DIGIT((*desc),ptdv);
   while (data1[ndigit-1] == 0)             /* Normieren, dass durch das */
   {                                        /* Ziffernabschneiden noetig */
      ndigit--;                             /* wird                      */
      exp--;
   }
   data3 = R_DIGIT((*res),ptdv);

   for ( i=0; i < ndigit; i++)              /* kopieren der benoetigten */
       data3[i] = data1[i];                 /* Ziffern                  */

   REL_HEAP;
   L_DIGIT((*res),ndigit) = ndigit;
   L_DIGIT((*res),sign) = R_DIGIT((*desc),sign);
   L_DIGIT((*res),Exp) = exp;
   return(DIGIT_TEST(res,"digit_frac",desc,0));
}



/*---------------------------------------------------------------------------
 *   digit_trunc  --  schneidet bei einer Digitzahl die Nachkommastellen ab.
 *
 *   globals:     _digit_recycling    <r>                  state:c
 *                _dig0               <r>                  heap:c
 *                debconf             <r>     (nur DEBUG)  runtime:c
 */

PTR_DESCRIPTOR digit_trunc(desc)

PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR res;
   register int i, *data1, *data3;
   int ndigit, exp, j, k;
   extern BOOLEAN _digit_recycling;                   /*  state:c */
#if DEBUG
   extern int debconf;                                /* runtime:c */
   if (debconf) printf("\n\n\n*** digit_trunc gestartet! ***\n");
#endif

   if ( (ndigit = R_DIGIT((*desc),ndigit)) <= ((exp = R_DIGIT((*desc),Exp)) + 1))
   {
      INC_REFCNT(desc);                     /* keine Nachkommastellen */
      return(DIGIT_TEST(desc,"digit_trunc",desc,0));
   }

   if (exp < 0)           /* keine Vorkommastellen vorhanden gewesen */
   {
      INC_REFCNT(_dig0);
      return(_dig0);
   }

/* jetzt muessen die untersten (= kleinsten) Ziffern abgeschnitten werden.
   Da die kleinste Ziffer an nullter Stelle steht, muss die Ziffernfolge neu
   ausgerichtet werden */
   if ( R_DESC((*desc),ref_count) == 1  &&  _digit_recycling)
   {                                     /* Deskriptor darf umgebaut werden */
      data1 = R_DIGIT((*desc),ptdv);
      j = ndigit - (exp + 1);            /* Ziffernzahl des Ergebnisses */
      k = exp + 1;
      while (data1[j] == 0)              /* Normieren, da durch das Ziffern- */
      {                                  /* abschneiden Nullen am Rand stehen */
         j++;                            /* koennen */
         k--;
         exp--;
      }
      for (i=0; i <= exp; i++,j++)       /* Ziffern verschieben, da die Ziffer */
      {                                  /* mit dem niedrigsten Exponent   */
         data1[i] = data1[j];            /* ganz vorne steht */
      }
      L_DIGIT((*desc),ndigit) = k;
      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_trunc",desc,0));
   }

   if ((res = newdesc() ) == NULL)
      return((PTR_DESCRIPTOR)0);
   DESC_MASK(res,1,C_DIGIT, TY_DIGIT);      /* Deskriptor initialisieren */
   L_DIGIT((*res),sign) = R_DIGIT((*desc),sign);
   L_DIGIT((*res),Exp) = exp;
   data1 = R_DIGIT((*desc),ptdv);
   j = (ndigit - (exp + 1));
   k = exp + 1;
   while (data1[j] == 0)           /* Normieren */
   {                               /* d.h. Kopiergrenzen modifizieren */
      j++;
      k--;
      exp--;
   }
   if (k == 0)              /* inzwischen keine Vorkommastellen mehr ueber */
   {
      INC_REFCNT(_dig0);
      DEC_REFCNT(res);
      return(_dig0);
   }
#if  DEBUG
   if (k < 0) printf("\n\n trunc: negative Ziffernanzahl!\n");
#endif
   if (newheap (k, A_DIGIT((*res),ptdv) ) == 0)
   {
      DEC_REFCNT(res);
      return((PTR_DESCRIPTOR)0);
   }
   data3 = R_DIGIT((*res),ptdv);
   for (i=0; i<=exp; i++,j++)               /* Ziffern kopieren */
   {
      data3[i] = data1[j];
   }
   L_DIGIT((*res),ndigit) = k;              /* tatsaechliche neue Ziffernzahl */
   return(DIGIT_TEST(res,"digit_trunc",desc,0));
}

/*
-----------------------------------------------------------------------------
*   digit_floor  --  schneidet bei einer Digitzahl die Nachkommastellen ab,
*                    wobei abgerundet wird. Existieren keine Nachkommastellen,
*                    so bleibt die Zahl unveraendert.
*   globals:     _digit_recycling    <r>                 state:c
*                _dig0,_dig1         <r>                 heap:c
*                debconf             <r>  (nur DEBUG)    runtime:c
*/

PTR_DESCRIPTOR digit_floor(desc)
PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR res;                      /* Ergebnis */
   register int i, *data1, *data3;
   int ndigit, exp, j;
   COUNT sign;                              /* Vorzeichen */
   extern BOOLEAN _digit_recycling;         /* state:c   */

#if DEBUG
  extern int debconf;                       /* runtime:c  */

  if (debconf) printf("\n\ndigit-floor gestartet\n");
#endif

   if ( (ndigit = R_DIGIT((*desc),ndigit)) <= (exp = R_DIGIT((*desc),Exp)) + 1)
   {
      INC_REFCNT(desc);            /*  keine Nachkommastellen vorhanden */
      return(DIGIT_TEST(desc,"digit_floor",desc,0));
   }

   if (exp < 0)           /* keine Vorkommastellen vorhanden */
   {
      if (R_DIGIT((*desc),sign) == PLUS_SIGN)
      {
         INC_REFCNT(_dig0);
         return(_dig0);
      }
      else                /* bei negativen Vorzeichen muss die Zahl um 1 */
      {                   /* verkleinert werden -> (-1) erzeugen  */
         DIGDESC(res,1);
         L_DIGIT((*res),sign) == MINUS_SIGN;
         L_DIGIT((*res),Exp) = 0;
         L_DIGIT((*res),ndigit) = 1;
         R_DIGIT((*res),ptdv)[0] = 1;                /* die Ziffer */
         return(DIGIT_TEST(res,"digit_floor",desc,0));
      }
   }

   if ( R_DESC((*desc),ref_count) == 1  &&  _digit_recycling)
   {                                 /* Zahl kann im alten Deskriptor zusammengeschoben werden */
      data1 = R_DIGIT((*desc),ptdv);
      j = ndigit - (exp + 1);
      ndigit = exp + 1;
      while (data1[j] == 0)        /* Normieren */
      {
         j++;
         ndigit--;
         exp--;
      }
      for (i=0; i <=exp; i++,j++)
      {
         data1[i] = data1[j];      /* zurueckbleibende Ziffern verschieben */
      }
      L_DIGIT((*desc),ndigit) = ndigit;
      if (R_DIGIT((*desc),sign) == MINUS_SIGN)
      {
         data1[0]++;               /* im neg. Fall Zahl um ein verkleinern */
         if (data1[0] >= BASE)     /* im Ueberlauffall die Subtraktion aufrufen */
         {                         /* da sich der Uebertrag soweit fortpflanzen */
            data1[0]--;            /* kann, dass neuer Deskriptor faellig wird */
            res = (PTR_DESCRIPTOR) digit_sub(desc, _dig1);
            if (res == 0)
               return((PTR_DESCRIPTOR)0);
            return(DIGIT_TEST(res,"digit_floor",desc,0));
         }
      }

      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_floor",desc,0));
   }

   j = ndigit - (exp + 1);
   ndigit = exp + 1;
   DIGDESC(res, ndigit);
   L_DIGIT((*res),sign) = sign = R_DIGIT((*desc),sign);
   L_DIGIT((*res),Exp) = exp;
   data1 = R_DIGIT((*desc),ptdv);
#if DEBUG
 if (ndigit <= 0)  printf ("\n\n\n floor: ndigit <= 0!!!\n");
#endif

   data3 = R_DIGIT((*res),ptdv);
   for (i=0; i<=exp; i++,j++)
   {
      data3[i] = data1[j];
   }
   if (sign)
   {
      if ( ++data3[0] >= BASE)   /* eins subtrahieren, da Zahl negativ ist */
      {
         data3[0]--;               /* Wieder rueckgaengig machen wegen Uebertrag */
         L_DIGIT((*res),ndigit) = ndigit;
         desc = (PTR_DESCRIPTOR) digit_sub(res,_dig1);   /* vollstaendige Subtraktion */
         DEC_REFCNT(res);                                /* wegen der komplexen Moeglich- */
         if (desc == 0)                                  /* keiten, wie Uebertraege anfallen */
            return((PTR_DESCRIPTOR)0);
         return(DIGIT_TEST(desc,"digit_floor",desc,0));
      }
   }

   while (data3[0] == 0)            /* Normieren */
   {
#if DEBUG
  if (debconf) printf("Normieren\n");
#endif
      j = ndigit - 1;
      for (i=0; i<j; i++)
          data3[i] = data3[i+1];
      ndigit--;
   }
   L_DIGIT((*res),ndigit) = ndigit;
   return(DIGIT_TEST(res,"digit_floor",desc,0));
}


/*
-----------------------------------------------------------------------------
*   digit_ceil  --  schneidet bei einer Digitzahl die Nachkommastellen ab,
*                    wobei abgerundet wird.
*   globals:     _digit_recycling    <r>
*                _dig0,_dig1         <r>
*/

PTR_DESCRIPTOR digit_ceil(desc)
PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR res;
   register int i, *data1, *data3;
   int ndigit, exp, j;
   COUNT sign;
   extern BOOLEAN _digit_recycling;

#if DEBUG
  extern int debconf;

  if (debconf) printf("\n\ndigit-ceil gestartet\n");
#endif

   if ( (ndigit = R_DIGIT((*desc),ndigit)) <= (exp = R_DIGIT((*desc),Exp)) + 1)
   {                               /* keine Nachkommastellen */
      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_ceil",desc,0));
   }

   if (exp < 0)     /* nur Nachkommastellen */
   {
      if (R_DIGIT((*desc),sign) == MINUS_SIGN)
      {
         INC_REFCNT(_dig0);
         return(_dig0);
      }
      else
      {
         INC_REFCNT(_dig1);
         return(_dig1);
      }
   }

   if ( R_DESC((*desc),ref_count) == 1  &&  _digit_recycling)
   {
      data1 = R_DIGIT((*desc),ptdv);
      j = ndigit - (exp + 1);
      ndigit = exp + 1;           /* neue Ziffernzahl */
      while (data1[j] == 0)       /* Normieren */
      {
         j++;
         ndigit--;
         exp--;
      }
      for (i=0; i <= exp; i++,j++)   /* Ziffern verschieben. Die kleinste steht */
      {                              /* in data1[0] ! */
         data1[i] = data1[j];
      }
      L_DIGIT((*desc),ndigit) = ndigit;
      if (R_DIGIT((*desc),sign) == PLUS_SIGN)
      {
         data1[0]++;                /* eins Addieren wegen Aufrunden */
         if (data1[0] >= BASE)      /* Uebertrag !!! */
         {
            data1[0]--;             /* Addieren zuruecknehmen */
            res = (PTR_DESCRIPTOR) digit_add(desc,_dig1);  /* echtes Addieren mit */
            if (res == 0)                                  /* Uebertragbehandlung */
               return((PTR_DESCRIPTOR)0);
            return(DIGIT_TEST(res,"digit_ceil",desc,0));
         }
      }
      INC_REFCNT(desc);
      return(DIGIT_TEST(desc,"digit_ceil",desc,0));
   }

   j = ndigit - (exp + 1);
   ndigit = exp + 1;               /* neue Ziffernzahl bestimmen */
   DIGDESC(res,ndigit);            /* Deskriptor und Heapplatz anfordern */
   L_DIGIT((*res),sign) = sign = R_DIGIT((*desc),sign);
   L_DIGIT((*res),Exp) = exp;
   data1 = R_DIGIT((*desc),ptdv);
   data3 = R_DIGIT((*res),ptdv);
   for (i=0; i<ndigit; i++,j++)      /* Ziffern kopieren */
   {
      data3[i] = data1[j];
   }
   if (sign == PLUS_SIGN)
   {
      if ( ++data3[0] >= BASE)       /* eins addieren und auf Ueberlauf testen */
      {
         data3[0]--;                 /* Addition zuruecknehmen */
         L_DIGIT((*res),ndigit) = ndigit;
         desc = (PTR_DESCRIPTOR) digit_add(res, _dig1);  /* "richtige" Addition */
         DEC_REFCNT(res);
         if (desc == 0)
            return((PTR_DESCRIPTOR)0);
         return(DIGIT_TEST(desc,"digit_ceil",desc,0));
      }
   }


   while (data3[0] == 0)              /* Normieren */
   {
#if DEBUG
  if (debconf) printf("Normieren\n");
#endif
      j = ndigit - 1;
      for (i=0; i<j; i++)
          data3[i] = data3[i+1];
      ndigit--;
   }

   L_DIGIT((*res),ndigit) = exp + 1;
   return(DIGIT_TEST(res,"digit_ceil",desc,0));
}

/*
-----------------------------------------------------------------------------
*   digit_neg  --  invertiert eine Digitzahl
*   globals:     _digit_recycling    <r>                state:c
*                _dig0               <r>                heap:c
*                debconf             <r>  (nur DEBUG)   runtime:c
*/

PTR_DESCRIPTOR digit_neg(desc)
 PTR_DESCRIPTOR desc;
{
   PTR_DESCRIPTOR res;
   int i,ndigit;
   STACKELEM *data1, *data3;
   extern BOOLEAN _digit_recycling;                  /* state:c */
#if DEBUG
   extern BOOLEAN debconf;                           /* runtime:c */
#endif

   if (R_DIGIT((*desc),ptdv)[0] == 0) /* Die Null bekommt kein anderes Vorzeichen */
   {
      INC_REFCNT(desc);
      return(desc);
   }

   if (_digit_recycling  &&  R_DESC((*desc),ref_count) == 1)
   {
      INC_REFCNT(desc);
      if (R_DIGIT((*desc),sign) == PLUS_SIGN)
         L_DIGIT((*desc),sign) = MINUS_SIGN;
      else
         L_DIGIT((*desc),sign) = PLUS_SIGN;
      return(DIGIT_TEST(desc,"digit_neg",desc,0));
   }

   ndigit = R_DIGIT((*desc),ndigit);
   DIGDESC(res,ndigit);
   L_DIGIT((*res),sign) = (R_DIGIT((*desc),sign) ? PLUS_SIGN : MINUS_SIGN);
   L_DIGIT((*res),Exp) = R_DIGIT((*desc),Exp);       /* PLUS_SIGN ist 0, waehrend MINUS_SIGN */
   L_DIGIT((*res),ndigit) = ndigit;                  /* von 0 verschieden ist  */

   RES_HEAP;
   data3 = R_DIGIT((*res),ptdv);
   data1 = R_DIGIT((*desc),ptdv);

   for (i=0; i<ndigit; i++)
       data3[i] = data1[i];

   REL_HEAP;
   return(DIGIT_TEST(res,"digit_neg",desc,0));
}
