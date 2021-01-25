


/*-----------------------------------------------------------------------------
 *  digit-viel:c  - external:  conv_digit_int, conv_int_digit, digit_eq,
 *                             digit_le, digit_lt, digit_test
 *         Aufgerufen durch:
 *  diverse red_<Module>
 *-----------------------------------------------------------------------------
 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

extern PTR_DESCRIPTOR newdesc();

/*--------------------------------------------------------------------------------
 *  conv_digit_int  --  Konvertiert eine Dezimalzahl in eine Integerzahl.
 *                      Dies wird fuer die Parameter einiger Funktionen benoetigt.
 *                      Das Ergebnis wird in p hineingeschrieben, und bei
 *                      erfolgreicher Konvertierung wird TRUE zurueckgegeben,
 *                      sonst FALSE.
 *  globals         --              <w>
 *--------------------------------------------------------------------------------
 */

BOOLEAN conv_digit_int(desc,p)
PTR_DESCRIPTOR desc;
int *p;          /* Zeiger auf das Ergebnis */
{
int *data;

    RES_HEAP;
   if ((data = R_DIGIT((*desc),ptdv))[0] == 0)
   {
      REL_HEAP;
      *p = 0;
      return (TRUE);
   }

   if (R_DIGIT((*desc),ndigit) == 1)        /* Zahl hat nur eine Ziffer */
   {
      if (R_DIGIT((*desc),Exp) == 0)
      {
         *p = data[0];
         REL_HEAP;
         if (R_DIGIT((*desc),sign) != PLUS_SIGN)
            *p = - *p;
         return(TRUE);
      }
      if (R_DIGIT((*desc),Exp) == 1)
      {
          *p = data[0] * BASE;
          REL_HEAP;
         if (R_DIGIT((*desc),sign) != PLUS_SIGN)
            *p = - *p;
          return(TRUE);
      }
      REL_HEAP;
      return (FALSE);
   }

   if (R_DIGIT((*desc),ndigit) == 2  &&  R_DIGIT((*desc),Exp) == 1)
   {                                        /* Zahl hat zwei digit-Ziffern */
      *p = data[0] + data[1] * BASE;
      REL_HEAP;
      if (R_DIGIT((*desc),sign) != PLUS_SIGN)
          *p = - *p;
      return(TRUE);
   }

   REL_HEAP;          /* eine andere Zahl kann keinen Sinn geben */
   return(FALSE);
}


/*---------------------------------------------------------------------------*/
/*   conv_int_digit  --  konvertiert einen unsigned integer in eine          */
/*                       Dezimalzahl.                                        */
/*--------------------------------------------------------------------------*/

PTR_DESCRIPTOR  conv_int_digit(p)
unsigned int p;
{
   PTR_DESCRIPTOR desc;
   int *data;
   int a[20], i,j, n;

   extern int _base, _maxnum;

   if (p == 0)                     /* Spezialfaelle 0 und 1 */
   {
      INC_REFCNT(_dig0);
      return(_dig0);
   }
   if (p == 1)
   {
      INC_REFCNT(_dig1);
      return(_dig1);
   }
   if ((desc = newdesc() ) == 0)
      return((PTR_DESCRIPTOR)0);
   DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);      /* Deskriptor initialisieren */
   L_DIGIT((*desc),sign) = PLUS_SIGN;

   n = 0;
   while (p != 0)
   {
      a[n] = p % BASE;    /* Zahl wird in Digit-Darstellung gebracht */
      n++;                /* Damit man weiss, wieviel Ziffern noetig sind */
      p /= BASE;
   }
   j = 0;                 /* Normieren */
   while (a[j] == 0)  j++;               /* jetzt kommt der Deskriptor */

   if (newheap(n,A_DIGIT((*desc),ptdv)) == NULL)
   {
      DEC_REFCNT(desc);
      return((PTR_DESCRIPTOR)0);
   }
   L_DIGIT((*desc),ndigit) = n-- - j;
   L_DIGIT((*desc),Exp) = n;

   RES_HEAP;
   data = R_DIGIT((*desc),ptdv);
   for (i=0; j <= n; i++, j++)
       data[i] = a[j];
   REL_HEAP;
   return(desc);
}



/*---------------------------------------------------------------------------
    digit-eq  --  prueft, ob zwei Dezimalzahlen gleich sind
  ---------------------------------------------------------------------------
*/

BOOLEAN digit_eq(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   int *data1, *data2, i;

   if (desc1 == desc2)
      return(TRUE);
   RES_HEAP;
   if ((data1 = R_DIGIT((*desc1),ptdv))[0] != (data2 = R_DIGIT((*desc2),ptdv))[0])
   {
      REL_HEAP;
      return(FALSE);
   }
   if (R_DIGIT((*desc1),ndigit) != R_DIGIT((*desc2),ndigit) )
   {
      REL_HEAP;
      return(FALSE);
   }
   if (R_DIGIT((*desc1),Exp ) != R_DIGIT((*desc2),Exp ) )
   {
      REL_HEAP;
      return(FALSE);
   }
   if (R_DIGIT((*desc1),sign) != R_DIGIT((*desc2),sign) )
   {
      REL_HEAP;
      return(FALSE);
   }

   for (i=1; R_DIGIT((*desc1),ndigit) > i; i++)      /* Ziffern vergleichen */
       if (data1[i] != data2[i])
       {
          REL_HEAP;
          return(FALSE);
       }

   REL_HEAP;
   return(TRUE);
}


/*
----------------------------------------------------------------------------
   digit_lt  --  vergleicht zwei Dezimalzahlen und gibt TRUE zurueck, wenn
                 die erste kleiner ist als die zweite.
----------------------------------------------------------------------------
*/

BOOLEAN digit_lt(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   int j1, j2;
   int *data1, *data2;
   int exp1,exp2;
#if DEBUG
extern int debconf;
#endif

   if (desc1 == desc2)
      return(FALSE);
   if (R_DIGIT((*desc1),sign) == PLUS_SIGN)
   {
      if (R_DIGIT((*desc2),sign) == PLUS_SIGN)
      {
         if ( (exp1 = R_DIGIT((*desc1),Exp)) != (exp2 = R_DIGIT((*desc2),Exp)) )
            if (exp1 < exp2 )
               if (R_DIGIT((*desc2),ptdv)[0] == 0)
                  return(FALSE);
               else
                  return(TRUE);
            else
               if (R_DIGIT((*desc1),ptdv)[0] == 0)
                   return(TRUE);
               else
                   return(FALSE);

         data1 = (int *) R_DIGIT((*desc1),ptdv);     /* jetzt muessen die Ziffern */
         data2 = (int *) R_DIGIT((*desc2),ptdv);     /* verglichen werden */
         j1 = R_DIGIT((*desc1),ndigit);
         j2 = R_DIGIT((*desc2),ndigit);

         do
         {
            j1--;
            j2--;
            if (data1[j1] > data2[j2])
   {
#if DEBUG
     if (debconf) printf("digit_lt: richtiger Ausgang\n");
#endif
              return(FALSE);
   }
            if (data1[j1] < data2[j2])
              return(TRUE);
         }
         while (j1 != 0   &&   j2 != 0);

         return(j2);
      }
      return(FALSE);
   }
   else
      if (R_DIGIT((*desc2),sign) == PLUS_SIGN)    /* andere Zahl negativ */
         return(TRUE);
      else       /* beide Zahlen negativ */
      {
         if ((exp1 = R_DIGIT((*desc1),Exp)) != (exp2 = R_DIGIT((*desc2),Exp)) )
            if (exp1 < exp2 ) return (FALSE);
            else return (TRUE);

         data1 = R_DIGIT((*desc1),ptdv);
         data2 = R_DIGIT((*desc2),ptdv);
         j1 = R_DIGIT((*desc1),ndigit);
         j2 = R_DIGIT((*desc2),ndigit);

         do
         {
            j1--;
            j2--;
            if (data1[j1] > data2[j2])   return(TRUE);
            if (data1[j1] < data2[j2])   return(FALSE);
         }
         while (j1 != 0   &&   j2 != 0);

         return(j1) ;
      }
}


/*
----------------------------------------------------------------------------
 digit_le  --  vergleicht zwei Dezimalzahlen und gibt TRUE zurueck, wenn
               die erste kleiner gleich als die zweite ist.
----------------------------------------------------------------------------
*/

BOOLEAN digit_le(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   int j1, j2;
   int *data1, *data2;
   int exp1,exp2;

   if (desc1 == desc2)
      return(TRUE);
   if (R_DIGIT((*desc1),sign) == PLUS_SIGN)
   {
      if (R_DIGIT((*desc2),sign) == PLUS_SIGN)   /* beide Zahlen positiv */
      {
         if ((exp1 = R_DIGIT((*desc1),Exp)) != (exp2 = R_DIGIT((*desc2),Exp)) )
            if (exp1 < exp2)                /* Vergleich der Exponenten */
                if (R_DIGIT((*desc2),ptdv)[0] == 0)
                   return(FALSE);
                else
                   return(TRUE);
            else
                if (R_DIGIT((*desc1),ptdv)[0] == 0)
                   return(TRUE);
                else
                   return(FALSE);

         data1 = R_DIGIT((*desc1),ptdv);    /* jetzt muessen die Ziffern verglichen */
         data2 = R_DIGIT((*desc2),ptdv);    /* werden */
         j1 = R_DIGIT((*desc1),ndigit);
         j2 = R_DIGIT((*desc2),ndigit);

         do
         {
            j1--;
            j2--;
            if (data1[j1] > data2[j2])   return(FALSE);
            if (data1[j1] < data2[j2])   return(TRUE);
         }
         while (j1 != 0   &&   j2 != 0);

         return(j1==0);            /* welche Zahl hat weniger Ziffern? */
      }
      return(FALSE);
   }
   else
      if (R_DIGIT((*desc2),sign) == PLUS_SIGN)    /* erste Zahl ist negativ */
         return(TRUE);
      else                /* beide Zahlen negativ */
      {
         if ((exp1 = R_DIGIT((*desc1),Exp)) != (exp2 = R_DIGIT((*desc2),Exp)) )
            if (exp1 < exp2 ) return (FALSE);
            else return(TRUE);

         data1 = R_DIGIT((*desc1),ptdv);    /* jetzt ziffernweises Vergleichen */
         data2 = R_DIGIT((*desc2),ptdv);
         j1 = R_DIGIT((*desc1),ndigit);
         j2 = R_DIGIT((*desc2),ndigit);

         do
         {
            j1--;
            j2--;
            if (data1[j1] > data2[j2])   return(TRUE);
            if (data1[j1] < data2[j2])   return(FALSE);
         }
         while (j1 != 0   &&   j2 != 0);

         return(j2 == 0);      /* welche Zahl hat weniger Ziffern? */
      }
}

#if DEBUG

/*----------------------------------------------------------------------------
 * digit_test --  dient zum Testen von Digit-Zahlen auf Konsistenz.
 *                Geprueft wird, ob die Zahl normiert ist, und ob die Ziffern
 *                bezueglich der Basis erlaubt sind. Wenn nein, wird die
 *                Reduktion gestoppt und der Deskriptor gezeigt.
 *                Mit der Eingabe c kann man die Reduktion fortsetzen.
 *                Es werden nur Ergebnisse von digit_Routinen getestet.
 *                Auf besondere Anforderung werden auf jeden Fall Argument und
 *                Ergebnisdeskriptoren gezeigt. Dieser Test findet nur im
 *                DEBUG-Fall statt. Ausserdem wird geprueft, ob noch illegaler-
 *                weise die _interrupt Variable gesetzt ist.
 *-----------------------------------------------------------------------------
 */
PTR_DESCRIPTOR digit_test(desc,c,d1,d2)
PTR_DESCRIPTOR desc;
char *c;
PTR_DESCRIPTOR d1,d2;
{
  int i, *data;
  char d;
  extern BOOLEAN _interrupt;
  extern int debconf;

   if (_interrupt)        /* Interruptmeldung nicht zurueckgesetzt? */
  {
     printf("digit_test: _interrupt ist gesetzt !!!\n    bitte <ret> druecken\n");
     getchar();
  }
   if (desc == 0)         /* Kein Deskriptor, wo einer erwartet wird? */
  {
     printf("digit_test: Nullpointer an falscher Stelle: %s !!!\n    bitte <ret> druecken\n",c);
     getchar();
  }

  data = R_DIGIT((*desc),ptdv);
  i = R_DIGIT((*desc),ndigit) - 1;

  if (desc == _dig0)               /* hier braucht und darf(!) nicht unter- */
     return (desc);                /* sucht werden */

  if (data[i] == 0   ||  data[0] == 0)      /* Zahl nicht normiert? */
  {
     printf("\n\n\nNormierungsfehler in %s: %x\n",c,desc);
     printf("Ziffer %d mit Wert %d\n",i,data[i]);
     DescDump (stdout, desc, 1);            /* kaputten Deskriptor zeigen */
     printf("bitte <return> druecken\n");
     d = getchar();
     if (d == 'd')
     {
        DescDump (stdout, d1,1);            /* Deskriptor 1. Arg zeigen */
        if (d2)
           DescDump (stdout, d2,1);         /* Deskriptor 2. Arg zeigen */
     }
     if (d != 'c')                          /* nicht sofort abbrechen? */
        post_mortem("digit-test: Normierungsfehler");
  }

  for (   ; i>=0; i--)
     if (data[i] >= BASE  || data[i] < 0)            /* Ziffer ausserhalb der */
     {                                               /* zulaessigen Grenzen */
        printf("\n\n\nFehler in %s: %x\n",c,desc);
        printf("Ziffer %d mit Wert %d\n",i,data[i]);
        DescDump(stdout, desc, 1);                   /* Deskriptor zeigen */
        printf("bitte <return> druecken\n");
        d = getchar();
        if (d == 'd')
        {
           DescDump (stdout, d1,1);         /* Deskriptor 1.Arg zeigen */
           if (d2)
              DescDump (stdout, d2,1);      /* Deskriptor 2.Arg zeigen */
        }
        if (d != 'c')              /* nicht sofort abbrechen ? */
           post_mortem("digit-test: illegale Ziffer gefunden");
     }
  if (debconf) DescDump(stdout, desc, 1);   /* Ergebnisdeskriptor zeigen */
  hpchain();                                /* heap-chain in Ordnung? */
  deschain();                               /* Deskriptor-chain in Ordnung? */
  return(desc);                             /* beide in heap:c */
}
#endif
