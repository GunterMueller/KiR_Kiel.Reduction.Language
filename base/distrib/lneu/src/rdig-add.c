


/***************************************************************************/
/*                DIGIT - ADDITION       (digit-arithmetic)                */
/*-------------------------------------------------------------------------*/
/*  rdig-add.c  - external :  digit_add;                                   */
/*                called by:  red_plus,    digit_mvt_op, mvt_mvt_op,       */
/*                            digit_floor, digit_ceil                      */
/*              - internal :  none                                         */
/***************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

/*------------------------------------------------------- */
/* Spezielle externe Routinen:                            */
 extern PTR_DESCRIPTOR newdesc();           /* rheap.c    */
#if DEBUG
 extern PTR_DESCRIPTOR digit_test();        /* rdig-v1.c  */
 extern void DescDump();                    /* rdesc.c    */
#endif

/*--------------------------------------------------------------------------*/
/*  digit_add  --  Fuehrt die Addition zweier Digitstrings durch. Dabei     */
/*                 werden die Exponenten der vordersten und hintersten      */
/*                 Ziffer berechnet ( in up und low). Dann wird             */
/*                 verglichen, wie die relevanten Ziffern (dh. die im       */
/*                 Descriptor eingetragenen Ziffern) zueinander liegen, und */
/*                 in Abhaengigkeit davon wird zu einem entsprechenden Fall */
/*                 verzweigt, um eine moeglichst rationelle Addition zu     */
/*                 gewaehrleisten. Wenn _digit_recycling den Wert TRUE hat, */
/*                 wird in einigen Faellen versucht, Descriptoren mit dem   */
/*                 Refcount == 1 wiederzuverwenden. Dies geschieht jedoch   */
/*                 nur, wenn dazu kein NEWHEAP noetig ist. Im Extremfall    */
/*                 wird dies erst am Ende der Addition bemerkt (siehe       */
/*                 .._doch_neu). Dann wird ein neuer Deskriptor angefordert.*/
/*                 Als Abschluss wird das Ergebnis normiert.                */
/*                 Die Faelle 1 - 4 sind (fast) symmetrisch zu den Faellen  */
/*                 5 - 8. Dh. z.B. dass im Fall 3 die gleiche Konstellation */
/*                 auftritt wie im Fall 7, es ist lediglich Argument1 mit   */
/*                 Argument2 vertauscht. "Fast" besagt, dass bei Gleichheit */
/*                 der vorderen bzw der hinteren Exponenten nur einer von   */
/*                 beiden Faellen angesprochen wird.                        */
/*                 DIGIT_TEST ueberprueft im DEBUG-Fall, ob der Ergebnis-   */
/*                 deskriptor normiert und ob alle Ziffern im erlaubten     */
/*                 Bereich sind.                                            */
/* Die bei den einzelnen Faellen gezeigten darstellungen beziehen sich      */
/* auf die lage der zahlen zueinander. dabei ist das msb links (im gegen-   */
/* satz zu der abspeicherung im heap !!)                                    */
/*  globals    --  _digit_recycling   <r>                                   */
/*--------------------------------------------------------------------------*/

 PTR_DESCRIPTOR digit_add(desc1,desc2)
 PTR_DESCRIPTOR  desc1,desc2;
 {
    register int i;
    int j;
    int up1;        /* oberer Exponent der ersten Zahl */
    int low1;       /* unterer Exponent der ersten Zahl */
    int up2;        /* oberer Exponent der zweiten Zahl */
    int low2;       /* unterer Exponent der zweiten Zahl */
    int ndigit1,ndigit2;
    PTR_DESCRIPTOR res; /* Hier kommt das Ergebnis hinein */
    int *data1, *data2, *data3, *data3a;   /* lokale ptdv */
    extern BOOLEAN _digit_recycling;   /* darf alter Deskriptor modifiziert werden ? */
#if DEBUG
    extern int debconf;  /* zusaetzliche debugging Information gefordert ? */

 if (debconf) printf("\n\n\n *** digit_add gestartet ***\n\n");
 if (debconf) DescDump(stdout, desc1, 1);   /* druckt einen Deskriptor */
 if (debconf) DescDump(stdout, desc2, 1);
#endif

/*----------  Spezialfaelle  -------------------------------------------- */
    /* 1.: 1. Argument == 0 ?                                             */
    if (R_DIGIT((*desc1),ptdv)[0] == 0) {
      /* 1.1: 2. Argument positiv ?         */
      if (R_DIGIT((*desc2),sign) == PLUS_SIGN) {
        /* ist desc2 negativ, so lautet die urspruengliche Operation */
        /* 0 - (- zahl). Fuer das Ergebnis zaehlt nur das Vorzeichen */
        /* vom ersten Deskriptor! Durch das normale Addieren wird    */
        /* ein entsprechender positiver Deskriptor angelegt */
        INC_REFCNT(desc2);
#if DEBUG
        if (debconf) printf("1. Argument gleich 0\n");
#endif
        return(DIGIT_TEST(desc2,"digit_add",desc1,desc2));
       }
      else {     /* 1.2: 2. argument negativ !   */
        if (R_DESC((*desc2),ref_count) == 1  && _digit_recycling) {
          INC_REFCNT(desc2);                 /* wg. freigabe !         */
          L_DIGIT((*desc2),sign) = PLUS_SIGN;
          return(DIGIT_TEST(desc2,"digit_add",desc1,desc2));
         }
        else {   /* neuer descriptor */
          ndigit1 = R_DIGIT((*desc1),ndigit) ;
          DIGDESC(res,ndigit1);      /* Deskriptor und Heapplatz anfordern */
          L_DIGIT((*res),sign)   = R_DIGIT((*desc1),sign);
          L_DIGIT((*res),Exp)    = R_DIGIT((*desc1),Exp);
          L_DIGIT((*res),ndigit) = ndigit1;
          RES_HEAP;
          data3 = R_DIGIT((*res),ptdv);          /* lokale Heapzeiger */
          data2 = R_DIGIT((*desc2),ptdv);
          for (i = 0 ; i < ndigit1 ; i++)
            data3[i] = data2[i];                /* heapplatz kopieren */
          REL_HEAP;
          return(DIGIT_TEST(res,"digit_add",desc1,desc2));
        }
      }
    }   /* end if 1. arg == 0      */
    /* ------------------------------------------------------------- */
    /* 2.: 2. Argument gleich 0 ? */
    if (R_DIGIT((*desc2),ptdv)[0] == 0) {
       INC_REFCNT(desc1);
#if DEBUG
       if (debconf) printf("2. Argument gleich 0\n") ;
#endif
       return(DIGIT_TEST(desc1,"digit_add",desc1,desc2));
    }
    /* ---------- ende der spezialfaelle --------------------------- */

    up1     = R_DIGIT((*desc1),Exp);
    up2     = R_DIGIT((*desc2),Exp);
    ndigit1 = R_DIGIT((*desc1),ndigit) ;
    ndigit2 = R_DIGIT((*desc2),ndigit) ;
    low1    = up1 - ndigit1 ;
    low2    = up2 - ndigit2 ;
#if DEBUG
if (debconf)
  printf("up1:%d, low1:%d, up2:%d, low2:%d\n",up1,low1,up2,low2);
#endif
    /* Wie stehen die beiden Zahlen zueinander ? */
    if (low1 <= low2) {
      if (up1 >= up2) {
        if (R_DESC((*desc1),ref_count) == 1  && _digit_recycling &&
            up1 != up2     &&
            R_DIGIT((*desc1),ptdv)[R_DIGIT((*desc1),ndigit)-1] != MAXNUM) {
/* ---------------------------------------------------------------------- */
/*  1. Fall: low1 <= low2 && up1 > up2                                    */
/*      :1:--------       Refcount(:1:) = 1                               */
/*      :2:  ---                                                          */
/* d.h. eine Modifikation des ersten Deskriptors ist erlaubt, da er sonst */
/* nirgends gebraucht wird. Daher wird das Ergebnis hier hinein und nicht */
/* in einen neuen Deskriptor geschrieben. Deshalb werden die Faelle aus-  */
/* geschlossen, die das Anlegen eines neuen Deskriptors als wahrschein-   */
/* lich erscheinen lassen                                                 */
#if DEBUG
 if (debconf)  printf("\n\n1. Fall \n");
#endif
             RES_HEAP;
             data1 = (int *) ( R_DIGIT((*desc1),ptdv) + (low2 - low1));
             data2 = R_DIGIT((*desc2),ptdv); /* der Bereich, in dem addiert wird */
             j = ndigit2 - 1;
             for (i = 0; i < j;  ) {
               if ((data1[i] += data2[i]) >= BASE) { /* Addieren. Uebertrag? */
                 data1[i] -= BASE;                 /* Uebertrag weiterschieben */
                 i++;                              /* und naechste Ziffer */
                 data1[i]++;
                 }
                else i++;                              /* naechste Ziffer */
             }   /* end for i */
             if ((data1[i] += data2[i]) >= BASE) {
                 j = up2;
                 while (data1[i] >= BASE  &&  j < up1) {
                    data1[i] -= BASE;       /* Uebertrag vorhanden u. kann */
                    i++;                    /* weitergegeben werden? */
                    j++;
                    data1[i]++;
                 }
                 if (data1[i] >= BASE) {
#if DEBUG
 printf("dig-add: i:%d, data1[i]:%d, data2[i]:%d\n",i,data1[i],data2[i]);
#endif
 post_mortem("dig-add: uebertrag (1) !");
#ifdef UnBenutzt
                   REL_HEAP;
/* Dieser Fall wird gebraucht, falls ein Uebertrag */
/* soweit nach vorne gegeben werden muss, dass er  */
/* die Grenzen sprengt  Beispiel: 999+1 mit BASE = 10 */
                   ndigit = R_DIGIT((*desc1),ndigit) + 1;
                   DIGDESC(res,ndigit);     /* Deskriptor und Heapplatz anfordern */
                   L_DIGIT((*res),ndigit) = ndigit;
                   L_DIGIT((*res),sign) = R_DIGIT((*desc1),sign);
                   L_DIGIT((*res),Exp) = R_DIGIT((*desc1),Exp) + 1;
#if DEBUG
 if (debconf) printf("ndigit:%d\n",ndigit);
#endif
                   RES_HEAP;
                   data1 = R_DIGIT((*desc1),ptdv);   /* lokale Zeiger in den Heap */
                   data3 = R_DIGIT((*res),ptdv);
                   j = ndigit - 1;
                   for (i=0; i < j; i++)             /* Ziffern kopieren */
                      data3[i] = data1[i];
                   data3[i-1] -= BASE;     /* vordersten Uebertrag weitergeben */
                   data3[i] = 1;           /* Er ist vorhanden, sonst kommt */
                                           /* dieser Code nicht dran */
                   while (data3[0] == 0) {   /* Normieren hinten */
#if DEBUG
 if (debconf) printf("(1-add) schleife normieren !\n");
#endif
                      j = ndigit -1;
                      for (i=0; i<j; i++)
                          data3[i] = data3[i+1];     /* Ziffern verschieben */
                      L_DIGIT((*res),ndigit) = --ndigit;      /* eine Ziffer weniger */
                   }
                   REL_HEAP;
                   return(DIGIT_TEST(res,"digit_add",desc1,desc2));
#endif  /* UnBenutzt */
                }                           /* neuer Deskriptor noetig ? */
             }
             INC_REFCNT(desc1);
             /* Normieren hinten */
             j = 0;
             data1 = R_DIGIT((*desc1),ptdv);
             while (data1[j] == 0)
               j++;
             if (j) {
               for (i = 0 ; i < ndigit1 ; i++)
                 data1[i] = data1[i+j];
               L_DIGIT((*desc1),ndigit) -= j;
             }
             REL_HEAP;
             return (DIGIT_TEST(desc1,"digit_add",desc1,desc2));
           }
          else {
/* ---------------------------------------------------------------------- */
/*  2. Fall:     low1 <= low2 && up1 >= up2                               */
/*   :1:--------         Refcount(:1:) != 1                               */
/*   :2:  ----                                                            */
#if DEBUG
  if (debconf)  printf("\n\n2. Fall\n");
#endif
             /* Deskriptor und Heapplatz anfordern (+1 wg. ev. uebertrag) */
             DIGDESC(res,ndigit1+1);
             L_DIGIT((*res),sign)   = R_DIGIT((*desc1),sign);
             L_DIGIT((*res),Exp)    = up1;
             L_DIGIT((*res),ndigit) = ndigit1;
             RES_HEAP;
             data3a = R_DIGIT((*res),ptdv);          /* lokale Heapzeiger */
             data1  = R_DIGIT((*desc1),ptdv);
             data2  = R_DIGIT((*desc2),ptdv);
             data3a[ndigit1] = 0;                /* Reserve fuer Uebertrag */
             for (i = 0 ; i < ndigit1 ; i++)     /* erste Zahl in Ergebnis- */
                data3a[i] = data1[i];            /* deskriptor kopieren */
             /* jetzt beginnt das Addieren. Dazu muss data3 auf die erste */
             /* Ziffer positioniert werden, die veraendert wird           */
             data3 = (int *) (data3a + (low2 - low1));
             for (i = 0 ; i < ndigit2 ;  ) {
               if ((data3[i] += data2[i]) >= BASE) { /* Addieren. Uebertrag? */
                 data3[i] -= BASE;        /* Uebertrag nach vorn */
                 i++;                     /* naechste Ziffer */
                 data3[i]++;
                }
               else i++;                     /* naechste Ziffer */
             }
             while (data3[i] >= BASE) { /* Uebertraege nach vorn weitergeben */
                data3[i] -= BASE;  /* so weit wie noetig, nachdem das */
                i++;               /* Addieren abgeschlossen ist     */
                data3[i]++;
             }
             if (data3a[ndigit1]) {          /* ganz vorn Uebertrag ? */
                L_DIGIT((*res),ndigit)++;   /* eine Ziffer mehr */
                L_DIGIT((*res),Exp)++;      /* vorderste Exponent eins groesser */
             }
             /* Normieren hinten */
             j = 0;
             while (data3a[j] == 0)
               j++;
             if (j) {
               for (i = 0 ; i < ndigit1 ; i++)
                 data3a[i] = data3a[i+j];
               L_DIGIT((*res),ndigit) -= j;
             }
             REL_HEAP;
             return (DIGIT_TEST(res,"digit_add",desc1,desc2));
          }
       }
      else              /* up1 < up2 */
       if (low2 >= up1) {
/* ---------------------------------------------------------------------- */
/* 3. Fall:      up1 <= low2 < up2                                        */
/*  :1:    -----                                                          */
/*  :2:----                                                               */
/* Hier gibt es keine echte Addition, weil sich die Ziffern nicht ueber-  */
/* schneiden. Deshalb werden die beiden Zahlen geeignet zusammenkopiert   */
#if DEBUG
 if (debconf)  printf("\n\n3. Fall\n");
#endif
          ndigit1 = up2 - low1;             /* wird missbraucht !      */
          DIGDESC(res,ndigit1);
          L_DIGIT((*res),sign)   = R_DIGIT((*desc1),sign);
          L_DIGIT((*res),Exp)    = up2;
          L_DIGIT((*res),ndigit) = ndigit1;
          RES_HEAP;
          data3 = R_DIGIT((*res),ptdv);              /* lokale Heapzeiger */
          data1 = R_DIGIT((*desc1),ptdv);
          data2 = R_DIGIT((*desc2),ptdv);
          for ( i = low1 + 1; i <= up1; i++) {
             *data3++ = *data1++ ;                   /* Zahl1 kopieren */
          }
          for ( ;i <= low2; i++) {
             *data3++ = 0;               /* Zwischenraum mit Nullen fuellen */
          }
          for ( ;i <= up2; i++) {             /* Zahl2 kopieren */
             *data3++ = *data2++;
          }
          REL_HEAP;
          return(DIGIT_TEST(res,"digit_add",desc1,desc2));
       }
       else {  /* low2 < up1 */
/* ---------------------------------------------------------------------- */
/* 4. Fall:      low1 <= low2 < up1 < up2                                 */
/*  :1:  ----                                                             */
/*  :2:----                                                               */
#if DEBUG
  if (debconf)   printf("\n\n4. Fall\n");
#endif
          ndigit1 = up2 - low1;             /* wird missbraucht !      */
          DIGDESC(res,ndigit1 + 1);
          L_DIGIT((*res),sign)   = R_DIGIT((*desc1),sign);
          L_DIGIT((*res),Exp)    = up2;
          L_DIGIT((*res),ndigit) = ndigit1;  /* vorerst !               */
          RES_HEAP;
          data3 = R_DIGIT((*res),ptdv);              /* lokale Heapzeiger */
          data1 = R_DIGIT((*desc1),ptdv);
          data2 = R_DIGIT((*desc2),ptdv);
          data3a = data3;
          for (i = low1 + 1 ; i <= low2 ; i++) {
             *data3++ = *data1++ ;
          }
 L_ohne:         /* ohne Uebertrag */
          if ((*data3 = *data1++ + *data2++) >= BASE) {   /* Addieren. Uebertrag? */
             if (i == up1) goto L_schluss;             /* vorne angekommen? */
             *data3++ -= BASE;                             /* Uebertrag abziehen */
             i++;
             goto L_mit;
           }
          else {
             if (i == up1)  goto L_schluss;      /* vorn angekommenm? */
             data3++;
             i++;
             goto L_ohne;
          }
L_mit:           /* mit Uebertrag */
          if ((*data3 = *data1++ + *data2++  + 1) >= BASE) { /* Addieren plus alten Uebertrag. Uebertrag? */
             if ( i == up1) goto L_schluss;      /* vorn angekommen */
             *data3++ -= BASE;                       /* Uebertrag abziehen */
             i++;
             goto L_mit;
           }
          else {
             if ( i == up1) goto L_schluss;      /* vorn angekommen? */
             data3++;
             i++;
             goto L_ohne;
          }
/* eigentliche Addition zuende, aber noch sind nicht alle Ziffern von Zahl2 da */
L_schluss:
          if (*data3 >= BASE) {
             while (*data3 >= BASE && i < up2) {
                *data3++ -= BASE;  /* noch Uebertrag zu verrechnen? */
                i++;
                *data3 = *data2++ + 1;
             }
             if (*data3 >= BASE) {         /* immer noch Uebertrag, d.h. */
                *data3++ -= BASE;  /* Zahl2 laengst zuende */
                i++;               /* Uebertrag verschieben */
                *data3 = 1;
                L_DIGIT((*res),Exp)++;
                L_DIGIT((*res),ndigit)++;
              }
             else data3++;
           }
          else data3++;   /* kein Uebertrag */
          for ( ; i < up2 ; i++) {
             *data3++ = *data2++;      /* ev. restliche Ziffern von Zahl2 kopieren */
          }
          /* Normieren hinten */
          j = 0;
          while (data3a[j] == 0)
            j++;
          if (j) {
            for (i = 0 ; i < ndigit1 ; i++)
              data3a[i] = data3a[i+j];
            L_DIGIT((*res),ndigit) -= j;
          }
          REL_HEAP;
          return(DIGIT_TEST(res,"digit_add",desc1,desc2));
       }
     }
    else {       /* low1 > low2 */
      if (up2 >= up1) {
        if (R_DESC((*desc2),ref_count) == 1  && _digit_recycling     &&
            up1 != up2                                       &&
            R_DIGIT((*desc2),ptdv)[R_DIGIT((*desc2),ndigit)-1] != MAXNUM) {
/* ---------------------------------------------------------------------- */
/*  5. Fall      low1 > low2 , up1 < up2                                  */
/*  :1:  ---          und Refcount(:1:) = 1                               */
/*  :2:--------  (Spezialfall vom Fall 6)                                 */
/* d.h. eine Modifikation des ersten Deskriptors ist erlaubt, da er sonst    */
/* nirgends gebraucht wird. Daher wird das Ergebnis hier hinein und nicht in */
/* einen neuen Deskriptor geschrieben. Deshalb werden die Faelle ausge-      */
/* schlossen, die das Anlegen eines neuen Deskriptors als wahrscheinlich     */
/* erscheinen lassen.                                                        */
#if DEBUG
  if (debconf)   printf("\n\n5. Fall\n");
#endif
             RES_HEAP;
             data2 = (int *) (R_DIGIT((*desc2),ptdv) + (low1 - low2));
             data1 = R_DIGIT((*desc1),ptdv); /* derBereich, in dem addiert wird */
             j     = ndigit1 - 1;
             for (i = 0; i < j ;  ) {
               if ((data2[i] += R_DIGIT((*desc1),ptdv)[i]) >= BASE) {
                 data2[i] -= BASE;          /* Addieren. Uebertrag? */
                 i++;                       /* Uebertrag weiterschieben */
                 data2[i]++;                /* naechste Ziffer */
                }
               else i++;                             /* naechste Ziffer */
             }   /* end for i      */
             if ((data2[i] += data1[i]) >= BASE) {
                j = up1;
                while (data2[i] >= BASE  &&  j < up2) {
                   data2[i] -= BASE;  /* Uebertrag vorhanden und kann */
                   i++;               /* weitergegeben werden */
                   j++;
                   data2[i]++;
                }
                if (data2[i] >= BASE) {
#if DEBUG
 if (debconf) printf("dig-add: uebertrag: i:%d, data1[i]:%d, data2[i]:%d\n",i,data1[i],data2[i]);
#endif
 post_mortem("dig-add: uebertrag (2) !");
#ifdef UnBenutzt
                   REL_HEAP;
/* Dieser Fall wird gebraucht, falls ein           */
/* Uebertrag soweit nach vorn gegeben werden       */
/* muss, dass er die Grenzen sprengt. Bsp: 1 + 999 */
                   ndigit = R_DIGIT((*desc2),ndigit) + 1;
                   DIGDESC(res,ndigit);     /* Deskriptor und Heapplatz anfordern */
                   L_DIGIT((*res),ndigit) = ndigit;
                   L_DIGIT((*res),sign) = R_DIGIT((*desc2),sign);
                   L_DIGIT((*res),Exp) = R_DIGIT((*desc2),Exp) + 1;
                   RES_HEAP;
                   data3 = R_DIGIT((*res),ptdv);     /* lokale Zeiger in den Heap */
                   data2 = R_DIGIT((*desc2),ptdv);
                   j = ndigit - 1;
                   for (i=0; i < j; i++)
                      data3[i] = data2[i];           /* Ziffern kopieren */
                   data3[i-1] -= BASE;
                   data3[i] = 1;
                   while (data3[0] == 0) {   /* Normieren hinten */
#if DEBUG
 if (debconf)  printf("(5-add) schleife normieren !\n");
#endif
                      j = ndigit -1;
                      for (i=0; i<j; i++)
                          data3[i] = data3[i+1];
                      L_DIGIT((*res),ndigit) = --ndigit;
                   }
                   REL_HEAP;
                   return(DIGIT_TEST(res,"digit_add",desc1,desc2));
#endif  /* UnBenutzt      */
                }
             }   /* end if Uebertrag */
             INC_REFCNT(desc2);
             REL_HEAP;
             return (DIGIT_TEST(desc2,"digit_add",desc1,desc2));
           }
          else {
/* ---------------------------------------------------------------------- */
/*  6. Fall:     low1 > low2 , up1 <= up2                                 */
/*   :1:  ----     Refcount(:1:) != 1                                     */
/*   :2:---------                                                         */
#if DEBUG
 if (debconf)   printf("\n\n6. Fall\n");
#endif
             DIGDESC(res,ndigit2+1);       /* Deskriptor und Heapplatz anfordern */
             L_DIGIT((*res),sign)   = R_DIGIT((*desc1),sign);
             L_DIGIT((*res),Exp)    = up2;
             L_DIGIT((*res),ndigit) = ndigit2;
             RES_HEAP;
             data3a = R_DIGIT((*res),ptdv);          /* lokale Heapzeiger */
             data1 = R_DIGIT((*desc1),ptdv);
             data2 = R_DIGIT((*desc2),ptdv);
             data3a[ndigit2] = 0;                /* Reserve fuer Uebertrag */
             for ( i = 0 ; i < ndigit2 ; i++) { /* zweite Zahl in Ergebnis- */
                data3a[i] = data2[i];                /* deskriptor kopieren */
             }
             data3 = (int *) (data3a + (low1 - low2));
/* jetzt beginnt das Addieren. Dazu muss data3 auf die erste Ziffer    */
/* positioniert werden, die veraendert wird. */
             for ( i = 0 ; i < ndigit1 ; ) {
               if ((data3[i] += data1[i]) >= BASE) { /* Addieren. Uebertrag? */
                 data3[i] -= BASE;        /* Uebertrag nach vorn */
                 i++;
                 data3[i]++;
                 }
                else i++;
             }   /* end for i      */
             while (data3[i] >= BASE) {   /* Uebertraege nach vorn weitergeben */
                data3[i] -= BASE;         /* so weit wie noetig, nachdem das */
                i++;                      /* Addieren abgeschlossen ist */
                data3[i]++;
             }
             if (data3a[ndigit2]) {          /* Uebertrag ganz vorne? */
                L_DIGIT((*res),ndigit)++;   /* Eine Ziffer mehr */
                L_DIGIT((*res),Exp)++;      /* vorderste Exponent eins groesser */
             }
             REL_HEAP;
             return (DIGIT_TEST(res,"digit_add",desc1,desc2));
          }
       }
      else              /* up2 < up1 */
       if (low1 >= up2) {
/* ---------------------------------------------------------------------- */
/* 7. Fall:      up2 <= low1 < up1                                        */
/*  :1:----                                                               */
/*  :2:    -----                                                          */
/* Hier gibt es keine echte Addition, weil sich die Ziffern nicht ueber-  */
/* schneiden. Deshalb werden die beiden Zahlen geeignet zusammenkopiert   */
#if DEBUG
if (debconf)   printf("\n\n7. Fall\n");
#endif
          ndigit1 = up1 - low2;             /* wird missbraucht        */
          DIGDESC(res,ndigit1);      /* Deskriptor und Heapplatz anfordern */
          L_DIGIT((*res),sign) = R_DIGIT((*desc1),sign);
          L_DIGIT((*res),Exp) = up1;
          L_DIGIT((*res),ndigit) = ndigit1;
          RES_HEAP;
          data1 = R_DIGIT((*desc1),ptdv);        /* lokale Heapzeiger */
          data2 = R_DIGIT((*desc2),ptdv);
          data3 = R_DIGIT((*res),ptdv);
          for (i = low2 + 1 ; i <= up2 ; i++) { /* Zahl2 kopieren */
             *data3++ = *data2++ ;
          }
          for ( ; i <= low1 ; i++) { /* Mittelteil mit Nullen auffuellen */
             *data3++ = 0;
          }
          for ( ; i <= up1 ; i++) {  /* Zahl1 kopieren */
             *data3++ = *data1++;
          }
          if (*(--data3) == 0) {        /* erste Zahl war eine Null, und */
             L_DIGIT((*res),ndigit)--;  /* die zweite negativ und ihr    */
             L_DIGIT((*res),Exp)--;     /* Absolutbetrag kleiner Null    */
          }                             /* --> Normieren vorne           */
          REL_HEAP;
          return(DIGIT_TEST(res,"digit_add",desc1,desc2));
        }
       else {    /* low1 < up2 */
/* ---------------------------------------------------------------------- */
/* 8. Fall:      low1 < up2 < up1                                         */
/*  :1:----                                                               */
/*  :2:  -----                                                            */
#if DEBUG
  if (debconf)   printf("\n\n8. Fall\n");
#endif
          ndigit1 = up1 - low2;      /* wird missbraucht !    */
          DIGDESC(res,ndigit1 + 1);  /* Deskriptor und Heapplatz anfordern */
          L_DIGIT((*res),sign) = R_DIGIT((*desc1),sign);
          L_DIGIT((*res),Exp) = up1;
          L_DIGIT((*res),ndigit) = ndigit1;
          RES_HEAP;
          data3 = R_DIGIT((*res),ptdv);              /* lokale Heapzeiger */
          data1 = R_DIGIT((*desc1),ptdv);
          data2 = R_DIGIT((*desc2),ptdv);
          for (i = low2 + 1; i <= low1; i++) {
             *data3++ = *data2++ ;
          }
 M_ohne:         /* ohne Uebertrag */
          if ((*data3 = *data1++ + *data2++) >= BASE) {  /* Addieren, Uebertrag? */
             if (i == up2)  goto M_schluss;      /* vorn angekommen? */
             *data3++ -= BASE;                       /* Uebertrag abziehen */
             i++;                                    /* naechste Ziffer */
             goto M_mit;
          }
          else {
             if (i == up2)  goto M_schluss;      /* vorn angekommen? */
             data3++;
             i++;                                    /* naechste Ziffer */
             goto M_ohne;
          }
M_mit:           /* mit Uebertrag */
          if ((*data3 = *data1++ + *data2++  + 1) >= BASE) {
                                    /* Addieren plus alten Uebertrag. Uebertrag? */
             if ( i == up2) goto M_schluss;      /* vorn angekommen? */
             *data3++ -= BASE;                       /* Uebertrag abziehen */
             i++;
             goto M_mit;
          }
          else {
             if ( i == up2) goto M_schluss;      /* vorn angekommen? */
             data3++;
             i++;
             goto M_ohne;
          }
/* eigentliche Addition zuende, aber noch sind nicht alle Ziffern von Zahl1 da */
M_schluss:
          if (*data3 >= BASE) {
             while (*data3 >= BASE && i < up1) { /* noch Uebertrag zu verrechnen */
                *data3++ -= BASE;
                i++;
                *data3 = *data1++ + 1;
             }
             if (*data3 >= BASE) {         /* immer noch Uebertrag, d.h. Zahl1 */
                *data3++ -= BASE;           /* ist laengst zuende */
                i++;                        /* Uebertrag verschieben */
                *data3 = 1;
                L_DIGIT((*res),Exp)++;
                L_DIGIT((*res),ndigit)++;
              }
             else data3++;
           }
          else { /* keine Uebertrag         */
            data3++;
          }
          for ( ; i < up1; i++) {
             *data3++ = *data1++;           /* ev. restliche Ziffern von Zahl2 */
          }                                 /* kopieren */
          REL_HEAP;
          return(DIGIT_TEST(res,"digit_add",desc1,desc2));
       }
    }
 }
/* end of rdig-add.c */
