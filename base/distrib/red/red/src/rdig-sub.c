/*
 * $Log: rdig-sub.c,v $
 * Revision 1.5  2001/07/02 14:53:11  base
 * compiler warnings eliminated
 *
 * Revision 1.4  1995/05/22 09:10:08  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:40  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */




/***************************************************************************/
/*                DIGIT - SUBTRACTION    (digit-arithmetic)                */
/*-------------------------------------------------------------------------*/
/*  rdig-sub.c  - external :  digit_add;                                   */
/*                called by:  red_minus,   digit_mvt_op, mvt_mvt_op,       */
/*                            digit_div,   digit_mod                       */
/*              - internal :  none                                         */
/***************************************************************************/

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
 extern int newheap(); /* TB, 30.10.92 */   /* heap.c */
 extern PTR_DESCRIPTOR newdesc();           /* heap:c    */
#if DEBUG
 extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
 extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
 extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
 extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
 extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
 extern PTR_DESCRIPTOR digit_test();        /* digit-v1:c */
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

/*-----------------------------------------------------------------------------
 *  digit_sub  --  fuehrt die Addition von zwei Digitstrings durch.
 *                 Erst wird durch eventuelles Tauschen dafuer gesorgt,
 *                 dass das 1. Argument die betragsmaessig groessere Zahl
 *                 ist. Dann wird (wie bei der Addition) die Exponenten der
 *                 jeweils vordersten und hintersten Ziffer bestimmt.
 *                 Anhand dieser Werte wird ermittelt, wie die beiden Zahlen
 *                 zueinander stehen und der zugehoerige Fall wird ange-
 *                 sprungen.
 *                 Aehnlich wie bei der Addition wird versucht, Descriptoren
 *                 wiederzuverwenden, wenn deren Refcount == 1 ist.
 *                 Voraussetzung dafuer ist, dass _digit_recycling == TRUE
 *                 und kein NEWHEAP erforderlich ist.
 *  globals    --  _digit_recycling <r>
 *                 _dig0            <r>
 *                 debconf          <r>   (nur DEBUG)
 *-----------------------------------------------------------------------------
 */

 PTR_DESCRIPTOR digit_sub(desc1,desc2)
 PTR_DESCRIPTOR desc1, desc2;
 {
    int up_exp1;  /* groesster Exponent der ersten Zahl */
    int low_exp1;    /* kleinster Exponent der ersten Zahl */
    int up_exp2;  /* groesster Exponent der zweiten Zahl */
    int low_exp2;    /* kleinster Exponent der zweiten Zahl */
    int j1,j2;       /* Hilfsvariable zum Bestimmen, ob getauscht werden muss */
    PTR_DESCRIPTOR res;   /* Ergebnis */
    int *data1, *data2, *data3, *data1a;  /* lokale ptdv's */
    int i,j,k,sign,ndigit1,ndigit2,ndigit3;
    extern BOOLEAN _digit_recycling;        /* darf recyclet werden ? */
#if DEBUG
    extern int debconf;

    DBUG_ENTER ("digit_sub");

    if (debconf) printf("\n\n\n******   digit-sub  gestartet! ******\n\n");
    if (debconf) DescDump(stdout,desc1,1);     /* Deskriptor ausdrucken */
    if (debconf) DescDump(stdout,desc2,1);
#else
    DBUG_ENTER ("digit_sub");
#endif

    {
       ndigit1 = j1 = R_DIGIT((*desc1),ndigit);
       ndigit2 = j2 = R_DIGIT((*desc2),ndigit);

       RES_HEAP;

   /* die groessere Zahl muss desc1 sein, aus diesem Grunde wird geprueft, */
   /* ob Tauschen erforderlich ist */

       if ((up_exp1 = R_DIGIT((*desc1),Exp)) != (up_exp2 = R_DIGIT((*desc2),Exp)) )
       {
         if (up_exp1 < up_exp2 )            /* die eine Zahl hat den groesseren Exponenten */
         {
            if (R_DIGIT((*desc2),ptdv)[0] == 0)      /* zweite Zahl == 0? */
            {
               goto L_weiter;                        /* kein Tauschen */
            }
            else
            {
               goto L_tauschen;
            }
         }
         else {
            if (R_DIGIT((*desc1),ptdv)[0] == 0)      /* erste Zahl == 0? */
            {
               goto L_tauschen;
            }
            else
            {
               goto L_weiter;
            }
         }
       }

/* Die Zahlen haben den gleichen hoechsten Exponenten. Also beginnt jetzt */
/* ziffernweises Vergleichen */

       data1 = R_DIGIT((*desc1),ptdv);
       data2 = R_DIGIT((*desc2),ptdv);

       while (j1 != 0   &&   j2 != 0)            /* ziffernweises Vergleichen */
       {
          j1--;
          j2--;
          if (data1[j1] > data2[j2])   goto L_weiter;
          if (data1[j1] < data2[j2])   goto L_tauschen;

       }
   /* jetzt ist eine Zahl zuende. Welche ist es ? */

       if (j2)  goto L_tauschen;
       else
          goto L_weiter;
    }


L_tauschen:
    {
       res = desc1;
       desc1 = desc2;
       desc2 = res;
       data3 = data1;
       data1 = data2;
       data2 = data3;
       sign =  MINUS_SIGN;         /* wegen dem Tauschen muss das Ergebnis */
       ndigit3 = ndigit1;          /* negativ sein. Das haengt mit dem Verteiler */
       ndigit1 = ndigit2;          /* in red_minus bzw red_plus zusammen */
       ndigit2 = ndigit3;
       ndigit3 = up_exp1;
       up_exp1 = up_exp2;
       up_exp2 = ndigit3;
       goto L_start;
    }
L_weiter:
    sign = PLUS_SIGN;              /* Ergebnis wird positiv. Das haengt mit */
                         /* dem Verteiler in red_minus bzw. red_plus zusammen */
     /* Tauschen abgeschlossen */

L_start:
    REL_HEAP;


    if (R_DIGIT((*desc2),ptdv)[0] == 0)     /* zweite Zahl ist eine Null */
    {
       if (sign == R_DIGIT((*desc1),sign))
       {
          INC_REFCNT(desc1);
          DBUG_RETURN(DIGIT_TEST(desc1,"digit_sub",desc1,desc2));
       }
       else
       {
          if (R_DESC((*desc1),ref_count) == 1  &&  _digit_recycling)
          {
             L_DIGIT((*desc1),sign) = sign;
             INC_REFCNT(desc1);
             DBUG_RETURN(DIGIT_TEST(desc1,"digit_sub",desc1,desc2));
          }
          else
          {      /* neuer Deskriptor noetig */
             DIGDESC(res,ndigit1);          /* Deskriptor und Heapplatz anfordern */
             L_DIGIT((*res),ndigit) = ndigit1;
             L_DIGIT((*res),Exp) = up_exp1;
             L_DIGIT((*res),sign) = sign;
             RES_HEAP;
             data3 = R_DIGIT((*res),ptdv);
             data1 = R_DIGIT((*desc1),ptdv);
             for (i=0; i<ndigit1; i++)               /* Ziffern kopieren */
                data3[i] = data1[i];
             REL_HEAP;
             DBUG_RETURN(DIGIT_TEST(res,"digit_sub",desc1,desc2));
          }
       }
    }


    low_exp1 = up_exp1 - ndigit1;    /* up_exp1 und up_exp2 bereits vorne definiert */
    low_exp2 = up_exp2 - ndigit2;

#if DEBUG
 if (debconf) printf("up_exp1:%d,  up_exp2:%d,  low_exp1:%d, low_exp2:%d\n",up_exp1,up_exp2,low_exp1,low_exp2);
 if (debconf) printf("ndigit1:%d,  ndigit2:%d\n",ndigit1,ndigit2);
#endif


    if (low_exp1 <= low_exp2)      /* zweite Zahl passt in die erste hinein */
    {
#if DEBUG
 if (debconf) printf(" Fall low_exp1 <= low_exp2 \n");
#endif

       if ( R_DESC((*desc1),ref_count) != 1  || !(_digit_recycling) )
       {
          DIGDESC(res,ndigit1);            /* Deskriptor und Heapplatz anfordern */
          L_DIGIT((*res),sign) = sign;
          L_DIGIT((*res),Exp) = up_exp1;
          L_DIGIT((*res),ndigit) = ndigit1;

          RES_HEAP;
          data3 = R_DIGIT((*res),ptdv);     /* lokale Heappointer */
          data1 = R_DIGIT((*desc1),ptdv);
          data2 = R_DIGIT((*desc2),ptdv);
          for (i=low_exp1; i<low_exp2; i++)
             *data3++ = *data1++;               /* untersten Teil von desc1 kopieren */
          i++;

   /* eigentliche Subtraktion */
L_ohne:          /* ohne Uebertrag */
          if ((*data3 = *data1 - *data2) < 0)
          {
             if ( i == up_exp2) goto L_schluss_mit;  /* Zahl durch */
             *data3++ +=BASE;             /* negativer Uebertrag verschieben */
             i++;                         /* naechste Ziffer */
             data1++;
             data2++;
             goto L_mit;                  /* naechste mal mit Uebertrag */
          }
          else
          {
             if (i == up_exp2) goto L_schluss_ohne;  /* Zahl durch */
             data3++;                       /* naechste Ziffer */
             i++;
             data1++;
             data2++;
             goto L_ohne;                   /* naechste mal ohne Uebertrag */
          }

L_mit:           /* mit Uebertrag */
          if ((*data3 = *data1 - *data2 -1) < 0)     /* Subtrahieren */
          {
             if ( i== up_exp2) goto L_schluss_mit;   /* vorne angekommen? */
             *data3++ += BASE;                       /* neg. Uebertrag */
             i++;                                    /* naechste Ziffer */
             data1++;
             data2++;
             goto L_mit;                        /* naechste mal mit Uebertrag */
          }
          else
          {
             if (i == up_exp2) goto L_schluss_ohne;  /* vorn angekommen */
             data3++;                                /* neue Ziffer */
             i++;
             data1++;
             data2++;
             goto L_ohne;                   /* naechste mal ohne Uebertrag */
          }

L_schluss_mit:                     /* Subtraktion zuende, aber mit Uebertrag */
          *data3++ += BASE;        /* Uebertrag abarbeiten */
          data1++;
          *data3 = *data1 - 1;
          i++;

          while (*data3 < 0)       /* sich weiterschiebende Uebertraege */
          {                        /* abarbeiten */
             *data3++ += BASE;
             data1++;
             *data3 = *data1 - 1;
             i++;
          }

L_schluss_ohne:;
                    /* eigentliche Subtraktion jetzt abgeschlossen */

          if ( i < up_exp1)
          {
             data3++;              /* uebrig gebliebenen Uebertrag einbauen */
             i++;
             data1++;

             for ( ; i <= up_exp1; i++)     /* restliche Ziffen kopieren */
                *data3++ = *data1++;
          }

          while ((data3 = R_DIGIT((*res),ptdv))[ndigit1-1] == 0)   /* Normieren vorne */
          {
             L_DIGIT((*res),ndigit) = --ndigit1;   /* eine Ziffer weniger */
             if (ndigit1)                          /* dadurch wird vorderste */
             {                                     /* Ziffer ignoriert */
                L_DIGIT((*res),Exp)--;             /* Exponent anpassen */
             }
             else
             {
                REL_HEAP;
                INC_REFCNT(_dig0);                 /* es gibt keine Ziffer */
                DEC_REFCNT(res);                   /* mehr -> wir haben eine */
                                                   /* NULL!!! */
                DIGNULL;                           /* Meldung DEBUG-System */
                DBUG_RETURN(_dig0);
             }
          }
          while (data3[0] == 0)          /* Normieren hinten */
          {
             j = --ndigit1;
             for (i=0; i< j; i++)
                data3[i] = data3[i+1];
             L_DIGIT((*res),ndigit) = ndigit1;
          }
          REL_HEAP;
          DBUG_RETURN(DIGIT_TEST(res,"digit_sub",desc1,desc2));
       }
       else    /* refcount == 1 und die zweite Zahl passt in die erste hinein */
       {
#if DEBUG
  if (debconf) printf("Fall refcnt == 1\n");
#endif

          j = up_exp2 - low_exp2;                       /* bei dem die eigentliche */
          RES_HEAP;
          data1 = R_DIGIT((*desc1),ptdv);   /* lokale Heapzeiger */
          data2 = R_DIGIT((*desc2),ptdv);
          data1a = (int*) data1 + (low_exp2 - low_exp1);   /* neuen Anfang suchen */
          j--;                                             /* Subtraktion anfaengt */
          i = 0;

      /* Subtrahieren */

 K_ohne:         /* ohne Uebertrag */
          if ((data1a[i] -= data2[i]) < 0)  /* Subtrahieren */
          {
             if ( i == j) goto K_schluss_mit;        /* Zahl zuende? */
             data1a[i] += BASE;                      /* Uebertrag abarbeiten */
             i++;
             goto K_mit;                        /* naechste mal mit Uebertrag */
          }
          else
          {
             if (i == j) goto K_schluss_ohne;        /* Zahl zuende? */
             i++;
             goto K_ohne;           /* naechste mal ohne Uebertrag */
          }

K_mit:           /* mit Uebertrag */
          if ((data1a[i] -= (data2[i] + 1)) < 0)     /* Subtrahieren incl. */
          {                                          /* Uebertrag */
             if (i == j) goto K_schluss_mit;         /* Zahl zuende */
             data1a[i] += BASE;                      /* Uebertrag verschieben */
             i++;
             goto K_mit;                        /* naechste mal mit Uebertrag */
          }
          else
          {
             if (i == j) goto K_schluss_ohne;        /* Zahl zuende */
             i++;
             goto K_ohne;                  /* naechste mal ohne Uebertrag */
          }

K_schluss_mit:                             /* Uebertrag nachziehen */
          data1a[i] += BASE;
          i++;
          data1a[i]--;

          while (data1a[i] < 0)
          {
             data1a[i] += BASE;
             i++;
             data1a[i]--;
          }

K_schluss_ohne:

  /* Subtrahieren abgeschlossen */

          while ((data3 = R_DIGIT((*desc1),ptdv))[ndigit1-1] == 0)   /* Normieren vorne */
          {
             L_DIGIT((*desc1),ndigit) = --ndigit1;   /* Anzahl Ziffern herunter- */
             if (ndigit1)                            /* setzen. Dadurch verschwindet */
             {                                       /* die vorderste Ziffer */
                L_DIGIT((*desc1),Exp)--;
             }
             else
             {
                REL_HEAP;
                                            /* keine Ziffer mehr uebrig */
                INC_REFCNT(_dig0);          /* -> wir haben eine Null!!! */
                DIGNULL;                   /* Meldung DEBUG-System */
                DBUG_RETURN(_dig0);
             }
          }
          while (data3[0] == 0)   /* Normieren hinten */
          {
             j = --ndigit1;
             for (i=0; i< j; i++)
                data3[i] = data3[i+1];      /* Ziffern verschieben */
             L_DIGIT((*desc1),ndigit) = ndigit1;
          }
          REL_HEAP;
          L_DIGIT((*desc1),sign) = sign;   /* Vorzeichen eintragen */
          INC_REFCNT(desc1);
          DBUG_RETURN(DIGIT_TEST(desc1,"digit_sub",desc1,desc2));
       }
    }

    if (up_exp2 <= low_exp1)    /* keine Zahl ist in der anderen enthalten */
                                   /*  :1:       ----  */
    {                              /*  :2:  ---        */
#if DEBUG
  if (debconf) printf("Fall up_exp2 <= low_exp1\n");
#endif
       ndigit3 =  up_exp1 - low_exp2;
       DIGDESC(res,ndigit3);                /* Deskriptor und Heapplatz anfordern */
       L_DIGIT((*res),sign) = sign;
       L_DIGIT((*res),Exp) = up_exp1;
       L_DIGIT((*res),ndigit) = ndigit3;

       RES_HEAP;
       data1 = R_DIGIT((*desc1),ptdv) + (low_exp2 - low_exp1);
       data2 = R_DIGIT((*desc2),ptdv);      /* lokale Heapzeiger */
       data3 = R_DIGIT((*res),ptdv);


       data3[0] = BASE - data2[0];

       j = up_exp2 - low_exp2;           /* ueberhaengenden Rest der zweiten */
       j--;                                 /* Zahl abziehen */
       for (i=1; i <= j; i++)
       {
          data3[i] = MAXNUM - data2[i];     /* Null - Ziffer - Uebertrag */
       }

       j = low_exp1 - up_exp2;
       for (k=0 ; k < j; k++)
       {                                    /* das Zwischenloch zwischen beiden */
          data3[i] = MAXNUM;                /* Zahlen */
          i++;
       }

       data3[i] = data1[i] - 1;
       i++;

       if (low_exp1 >= up_exp2)             /* Rest bestimmen */
          j = up_exp1 - low_exp1 - 1;
       else
          j = up_exp1 - up_exp2 - 1;

       for (k=0 ; k < j; k++)               /* Rest aus erster Zahl kopieren */
       {
          data3[i] = data1[i];
          i++;
       }

       while (data3[ndigit3 - 1] == 0)   /* Normieren vorne */
       {

          L_DIGIT((*res),ndigit) = --ndigit3;        /* Heruntersetzen um eine Ziffer */
          L_DIGIT((*res),Exp)--;                     /* Dadurch wird die vorderste */
       }                                             /* Ziffer geschluckt */

       REL_HEAP;
       DBUG_RETURN(DIGIT_TEST(res,"digit_sub",desc1,desc2));
    }

                  /* der letzte u. sonstige Fall */

    {
#if DEBUG
  if (debconf) printf("sonstiger Fall: low_exp2low_exp1up_exp2up_exp1\n");
#endif
       ndigit3 =  up_exp1 - low_exp2;
       DIGDESC(res,ndigit3);
       L_DIGIT((*res),sign) = sign;
       L_DIGIT((*res),Exp) = up_exp1;
       L_DIGIT((*res),ndigit) = ndigit3;

       RES_HEAP;
       data1 = R_DIGIT((*desc1),ptdv) + (low_exp2 - low_exp1);
       data3 = R_DIGIT((*res),ptdv);        /* lokale Heappointer */
       data2 = R_DIGIT((*desc2),ptdv);

       data3[0] = BASE - data2[0];          /* Null - Ziffer */
       j = low_exp1 - low_exp2;
       for (i=1; i < j; i++)
       {
          data3[i] = MAXNUM - data2[i];     /* Null - Ziffer - Uebertrag */
       }

       j = up_exp2 - low_exp2;
       j--;

M_mit:         /* mit Uebertrag */
       if ((data3[i] = data1[i] - data2[i] -1) < 0)
       {
          if ( i == j) goto M_schluss_mit;  /* Zahl durch? */
          data3[i] += BASE;                 /* Uebertrag */
          i++;
          goto M_mit;                      /* naechste mal mit Uebertrag */
       }
       else
       {
          if (i == j) goto M_schluss_ohne;  /* Zahl durch? */
          i++;
          goto M_ohne;                      /* naechste mal ohne Uebertrag */
       }

M_ohne:          /* ohne Uebertrag */
       if ((data3[i] = data1[i] - data2[i]) < 0)
       {                                    /* Subtrahieren */
          if ( i == j) goto M_schluss_mit;
          data3[i] +=BASE;
          i++;
          goto M_mit;
       }
       else
       {
#if DEBUG
  if (debconf) printf("6. result:%d,  i:%d\n",data3[i],i+1);
#endif
          if (i == j) goto M_schluss_ohne;
          i++;
          goto M_ohne;
       }


M_schluss_mit:
       data3[i] += BASE;
#if DEBUG
  if (debconf) printf("7. result:%d,  i:%d\n",data3[i],i);
#endif
       i++;
       data3[i] = data1[i] - 1;

       while (data3[i] < 0)
       {
          data3[i] += BASE;
#if DEBUG
  if (debconf) printf("8. result:%d,  i:%d\n",data3[i],i);
#endif
          i++;
          data3[i] = data1[i] - 1;
       }

M_schluss_ohne:;
       j = up_exp1 - low_exp2;
       j--;
       if ( i < j)
       {
          i++;

          for ( ; i<=j; i++)
          {
             data3[i] = data1[i];
#if DEBUG
  if (debconf) printf("9. result:%d,  i:%d\n",data3[i],i);
#endif
          }
       }

       while (data3[ndigit3 - 1] == 0)   /* Normieren vorne */
       {
          L_DIGIT((*res),ndigit) = --ndigit3;
          L_DIGIT((*res),Exp)--;
#if DEBUG
  if (debconf) printf("Normieren vorne\n");
#endif
       }

       REL_HEAP;
       DBUG_RETURN(DIGIT_TEST(res,"digit_sub",desc1,desc2));
    }
 }
