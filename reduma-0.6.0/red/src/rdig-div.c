/* $Log: rdig-div.c,v $
 * Revision 1.5  1995/05/22  09:06:03  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.4  1994/01/18  09:26:42  mah
 * warning fix
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:35  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/***************************************************************************/
/*                DIGIT - DIVISION        (digit-arithmetic)               */
/*-------------------------------------------------------------------------*/
/*  rdig-div.c  - external :  digit_div;                                   */
/*                called by:  red_div,     digit_mvt_op, mvt_mvt_op,       */
/*                            digit_mod                                    */
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
extern PTR_DESCRIPTOR newdesc();
extern PTR_DESCRIPTOR digit_mul();
extern PTR_DESCRIPTOR digit_sub();
extern BOOLEAN digit_lt();
#if DEBUG
extern PTR_DESCRIPTOR digit_test();
#endif

/* ach 30/10/92 */
extern int newheap();                       /* rheap.c */
extern void cutheap();                      /* rheap.c */
/* end of ach */

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*-----------------------------------------------------------------------------
 *  digit_div  --  fuehrt die Division auf Digit-Strings durch.
 *                 Dabei wird nach dem Divisor unterschieden:
 *                 Divisor einstellig: Die Division erfolgt genauso, wie man
 *                   schriftliche Division durchfuehrt.
 *                 Divisor mehrstellig: Division erfolgt, wie in
 *                 Knuth, Algorithm Bd.2  Seite 237 beschrieben.
 *                 Dazu werden erst beide Zahlen mit einem Faktor multipliziert,
 *                 damit die erste Ziffer des Divisors groesser gleich BASE/2
 *                 ist, wobei BASE die Basis des benutzten Zahlensystems ist.
 *                 (Gruende dafuer siehe Knuth.)
 *                 Dann wird die Division aehnlich wie im obigen Fall durchge-
 *                 fuehrt, jedoch wird beim Divisor nur die erste Ziffer
 *                 beruecksichtigt und damit ein Schaetzwert ermittelt.
 *                 Mithilfe der zweiten Ziffer wird dieser korrigiert.
 *                 In seltenen Faellen (3/BASE) kann auch noch dieser Wert um
 *                 eins zu gross sein. Dies wird daran erkannt, dass der
 *                 abzuziehende Betrag groesser ist als der aktuelle Rest.
 *
 *
 *  globals    --  _digit_recycling  <r>
 *-----------------------------------------------------------------------------
 */

PTR_DESCRIPTOR digit_div(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   int d;                          /* Normierungsfaktor */
   int maxdigit;                   /* maximal moegliche Ziffernzahl des Ergebnisses */
   int u1;                         /* Exponent der hoechstwertigen Ziffer */
   int d1;                         /* Exponent der niederstwertigen Ziffer */
   unsigned long v1,v2;            /* erste bzw. zweite Ziffer des Divisors */
   unsigned long uj;               /* erste Ziffer des "aktuellen" Dividenden */
   unsigned long uj1, uj2;         /* zweite und dritte Ziffer...  */
   unsigned long q;                /* "vorlaeufige" Ergebnisziffer */
   BOOLEAN flag;                   /* beim nachtraeglichen Wechsel von Mehrziffer- */
                                   /* fall zu Einzifferfall sind die Deskriptoren */
                                   /* nur intern, also erfolgt Refcountbehandlung */
   COUNT sign1, sign2;             /* Nachhalten der urspruenglichen Vorzeichen, */
                                   /* da der Algorithmus nur fuer pos. Zahlen definiert ist */

   PTR_DESCRIPTOR res, zw1, zw2, hilf1, hilf2;
   int i,j;
   int ndigit1, ndigit2, cneu, calt, exp;
   int exp_lauf, ndigit;
   COUNT sign;
   unsigned int help;
   unsigned long wert;
   unsigned long *data1, *data3;
   register unsigned long temp;

   extern int _prec;

#if DEBUG
   extern int debconf;

   DBUG_ENTER ("digit_div");

   if (debconf) printf("\n\n\n*** digit-div gestartet! ***\n");
   if (debconf) DescDump(stdout,desc1,1);
   if (debconf) DescDump(stdout,desc2,1);
#else
   DBUG_ENTER ("digit_div");
#endif

   flag = FALSE;

   if (desc2 == _dig0)                     /* Division nicht erlaubt */
   {
      ABBRUCH;
      DBUG_RETURN((PTR_DESCRIPTOR)0);
   }

   if (desc1 == _dig0)                      /* erstes Argument Null */
   {
      INC_REFCNT(_dig0);
      DIGNULL;
      DBUG_RETURN(_dig0);
   }

                    /*  1 - Faktor einbauen  */

   sign1 = R_DIGIT((*desc1),sign);         /* Retten der urspruenglichen Vorzeichen */
   sign2 = R_DIGIT((*desc2),sign);

/****************************************************************************
 *         Divisor einziffrig                                               *
 ***************************************************************************/

L_eine_ziffer:
   if (( ndigit2 = R_DIGIT((*desc2),ndigit)) == 1)   /* Divisor einstellig */
   {
      ndigit1 = R_DIGIT((*desc1),ndigit);
      wert = R_DIGIT((*desc2),ptdv)[0];

#if DEBUG
  if (debconf) printf("div: Divisor einstellig. ndigit1:%d, ndigit2:%d, wert:%ld\n",ndigit1,ndigit2,wert);
#endif

      maxdigit = (exp = (R_DIGIT((*desc1),Exp) - R_DIGIT((*desc2),Exp))) + _prec + 1;
      if (maxdigit <= _prec)   maxdigit = _prec + 1;
#if DEBUG
 if (debconf) printf("div: maxdigit:%d\n",maxdigit);
#endif

      if (maxdigit <= 0)
      {
         if (flag)                                   /* Hier sind desc1 und  */
         {                                           /* desc2 erst von digit-*/
             DEC_REFCNT(desc1);                      /* div angelegt worden  */
             DEC_REFCNT(desc2);                      /* siehe "mehrziffrig"  */
         }
         INC_REFCNT(_dig0);
         DIGNULL;
         DBUG_RETURN(_dig0);
      }

#if (D_SLAVE && D_MESS && D_MHEAP)
      if ((res = (*d_m_newdesc)() ) == 0)
#else
      if ((res = newdesc() ) == 0)
#endif
      {
         if (flag)                                   /* Hier sind desc1 und  */
         {                                           /* desc2 erst von digit-*/
             DEC_REFCNT(desc1);                      /* div angelegt worden  */
             DEC_REFCNT(desc2);                      /* siehe "mehrziffrig"  */
         }
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      DESC_MASK(res,1,C_DIGIT,TY_DIGIT);             /* Deskriptor initialisieren */
#if (D_SLAVE && D_MESS && D_MHEAP)
      if ((*d_m_newheap) (maxdigit,A_DIGIT((*res),ptdv) ) == 0)
#else
      if (newheap (maxdigit,A_DIGIT((*res),ptdv) ) == 0)
#endif
      {
         DEC_REFCNT(res);
         if (flag)                                  /*  siehe oben */
         {
            DEC_REFCNT(desc1);
            DEC_REFCNT(desc2);
         }
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      cneu = maxdigit - 1;
      calt = ndigit1 - 1;
      RES_HEAP;
      data3 = (unsigned long *)R_DIGIT((*res),ptdv);
      data1 = (unsigned long *)R_DIGIT((*desc1),ptdv);
      if ((temp = data1[calt]) >= wert)       /* Ziffernzahl und Exponenten */
      {                                       /* korrigieren */
         data3[cneu] = temp / wert;
         temp = (temp % wert) * BASE;
#if DEBUG
 if (debconf) printf("div: 1. Ergebnis:%ld, Rest:%ld\n",data3[cneu],temp);
#endif
         cneu--; calt--;
      }
      else
      {
         temp *= BASE;
         maxdigit--;
         exp--;
#if DEBUG
 if (debconf) printf("div: 2. Ergebnis:0, Rest:%ld\n",temp);
#endif
         cneu--; calt--;
      }

      while (calt >= 0 && cneu >= 0)              /* Noch Ziffern im Dividenden */
      {
         data3[cneu] = (temp += data1[calt]) / wert;
         temp = (temp % wert) * BASE;
#if DEBUG
 if (debconf) printf("div: 3. Ergebnis:%ld, Rest:%ld\n",data3[cneu],temp);
#endif
         cneu--; calt--;
      }

      while (temp && cneu >= 0)             /* Keine Ziffer mehr im Dividend */
      {                                     /* aber Rest vorhanden und nicht */
         data3[cneu] = temp / wert;         /* alle Ziffern ausgeschoepft    */
         temp = (temp % wert) * BASE;
#if DEBUG
 if (debconf) printf("div: 4. Ergebnis:%ld, Rest:%ld\n",data3[cneu],temp);
#endif
         cneu--;                            /* cneu enthaelt die Anzahl der */
      }                                     /* nicht benutzten ("leeren")   */
                                            /* Ziffern. Dementsprechend:    */
      if (cneu < 0)                         /* Normieren, indem entsprech-  */
         while (data3[cneu+1] == 0 &&  (cneu+1)<maxdigit)         /* ende Ziffern als leer de-    */
            cneu++;                         /* klariert werden              */

      if (cneu >= 0)                        /* Verschieben der Zahl         */
      {
         for (i=0, j=cneu+1; j<maxdigit; i++,j++)
             data3[i] = data3[j];
         maxdigit += (i - j);
#if DEBUG
  if (debconf) printf("div: Division endlich. cneu:%d, maxdigit:%d\n",cneu,maxdigit);
#endif
           if (cneu > 9 )                     /* Freigabe der "leeren" Ziffern */
           {                                  /* sofern es sich lohnt          */
              cutheap(data3,maxdigit);
#if DEBUG
    if (debconf) printf("div: cutheap durchgefuehrt\n");
#endif
           }
      }
      REL_HEAP;
      if (maxdigit == 0)             /* es ist eine Null entstanden, z.B   */
      {                              /* durch Abschneiden wegen _prec      */
         if (flag)                                   /* Hier sind desc1 und  */
         {                                           /* desc2 erst von digit-*/
             DEC_REFCNT(desc1);                      /* div angelegt worden  */
             DEC_REFCNT(desc2);                      /* siehe "mehrziffrig"  */
         }
         DEC_REFCNT(res);
         INC_REFCNT(_dig0);
         DIGNULL;
         DBUG_RETURN(_dig0);
      }

      L_DIGIT((*res),ndigit) = maxdigit;
      if (R_DIGIT((*desc1),sign) )
          L_DIGIT((*res),sign) = ( R_DIGIT((*desc2),sign) ? PLUS_SIGN : MINUS_SIGN);
      else
          L_DIGIT((*res),sign) = R_DIGIT((*desc2),sign);
      L_DIGIT((*res),Exp) = exp;
      if (flag)                             /* urspruenglich war Divisor mehr- */
      {                                     /* ziffrig, ist aber normiert   */
         DEC_REFCNT(desc1);                 /* worden. Daher sind desc1 und */
         DEC_REFCNT(desc2);                 /* desc2 Hilfsdescriptoren.     */
      }
      DBUG_RETURN(DIGIT_TEST(res,"digit_div",desc1,desc2));
   }                              /* Ende desc2 einstellig */


/*****************************************************************************
 *         Divisor mehrziffrig                                               *
 *****************************************************************************/

#if DEBUG
  if (debconf) printf("div: desc2 mehrziffrig\n");
#endif
   L_DIGIT((*_work),ndigit) = 1;
   L_DIGIT((*_work),Exp) = 0;
   L_DIGIT((*_work),sign) = PLUS_SIGN;
   ndigit1 = R_DIGIT((*desc1),ndigit);

      d = BASE / (R_DIGIT((*desc2),ptdv)[ndigit2 - 1] +1); /* Normierungsfaktor */
#if DEBUG
  if (debconf) printf("div: d:%d\n",d);
#endif

                                            /* Normieren */
   if (d != 1)
   {
      L_DIGIT((*_work),ptdv)[0] = d;
      zw1 = (PTR_DESCRIPTOR) digit_mul(desc1,_work);
      if (zw1 == NULL)
      {
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      zw2 = (PTR_DESCRIPTOR) digit_mul(desc2,_work);
      if (zw2 == NULL)
      {
         DEC_REFCNT(zw1);
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      ndigit1 = R_DIGIT((*zw1),ndigit);
      ndigit2 = R_DIGIT((*zw2),ndigit);
      if (ndigit2 == 1)                     /* durch das Normieren ist Divisor */
      {                                     /* nun einziffrig               */
         desc1 = zw1;
         desc2 = zw2;
         flag = TRUE;
#if DEBUG
  if (debconf) printf("durch Normieren einziffrig geworden!\n");
#endif
         goto L_eine_ziffer;
      }
   }
   else
   {
      zw1 = desc1;
      zw2 = desc2;
      INC_REFCNT(desc1);
      INC_REFCNT(desc2);
   }

#if DEBUG
  if (debconf) printf("div: Eingabezahlen normiert\n");
#endif
   INC_REFCNT(zw2);                         /* Verhindern, dass zw2 recyclet */
                                            /* wird */
   if ( sign1  == PLUS_SIGN)
      sign = sign2;
   else
      sign = ( sign2  ? PLUS_SIGN : MINUS_SIGN);
   L_DIGIT((*zw1),sign) = PLUS_SIGN;
   L_DIGIT((*zw2),sign) = PLUS_SIGN;

   maxdigit = (exp = R_DIGIT((*zw1),Exp) - R_DIGIT((*zw2),Exp) ) + _prec + 1;
   if (maxdigit <= _prec)   maxdigit = _prec + 1;
   if (maxdigit <= 0)                       /* Ergebnis hat keine Ziffern   */
   {                                        /* -> Ergebnis = 0              */
      L_DIGIT((*zw1),sign) = sign1;
      L_DIGIT((*zw2),sign) = sign2;
      DEC_REFCNT(zw1);
      DEC_REFCNT(zw2);                      /* zw2 hat einen refcount zuviel */
      DEC_REFCNT(zw2);                      /* um das recycling in einer     */
      DIGNULL;
      INC_REFCNT(_dig0);
      DBUG_RETURN(_dig0);                        /* anderen Digit-Routine zu ver- */
   }                                        /* hindern.                      */

#if (D_SLAVE && D_MESS && D_MHEAP)
   if ((res = (*d_m_newdesc)() ) == 0)
#else
   if ((res = newdesc() ) == 0)
#endif
   {
      L_DIGIT((*zw1),sign) = sign1;
      L_DIGIT((*zw2),sign) = sign2;
      DEC_REFCNT(zw1);
      DEC_REFCNT(zw2);                      /* zw2 hat einen refcount zuviel */
      DEC_REFCNT(zw2);                      /* um das recycling in einer     */
      ABBRUCH;
      DBUG_RETURN((PTR_DESCRIPTOR)0);            /* anderen Digit-Routine zu ver- */
   }                                        /* hindern.                      */
   DESC_MASK(res,1,C_DIGIT,TY_DIGIT);       /* Deskriptor initialisieren */
#if (D_SLAVE && D_MESS && D_MHEAP)
   if ((*d_m_newheap)( maxdigit, A_DIGIT((*res),ptdv)) == 0)
#else
   if (newheap( maxdigit, A_DIGIT((*res),ptdv)) == 0)
#endif
   {
      L_DIGIT((*zw1),sign) = sign1;
      L_DIGIT((*zw2),sign) = sign2;
      DEC_REFCNT(res);
      DEC_REFCNT(zw1);                      /* siehe oben */
      DEC_REFCNT(zw2);
      DEC_REFCNT(zw2);
      ABBRUCH;
      DBUG_RETURN((PTR_DESCRIPTOR)0);
   }

   cneu = maxdigit - 1;
#if DEBUG
 if (debconf) printf ("div: _dig0:%x, zw1:%x, zw2:%x, ndigit1:%d, ndigit2:%d\n",(unsigned int)_dig0,(unsigned int)zw1,(unsigned int)zw2,ndigit1,ndigit2);
#endif
   {
   }
   calt = R_DIGIT((*zw1),Exp) + 1;

   v1 = R_DIGIT((*zw2),ptdv)[ R_DIGIT((*zw2),ndigit) - 1];    /* vordersten beiden */
   v2 = R_DIGIT((*zw2),ptdv)[ R_DIGIT((*zw2),ndigit) - 2];    /* Ziffern des */
                                                              /* Divisors    */
   if (R_DIGIT((*zw1),ptdv)[R_DIGIT((*zw1),ndigit)-1] < v1)
   {                                                 /* vorderste Ziffer des  */
      exp--;                                         /* Dividenden < v1       */
      maxdigit--;                                    /* dann Ergebnis eine    */
      cneu--;                                        /* Ziffer kleiner        */
      calt--;
   }

   if (maxdigit <= 0)                       /* Ergebnis hat keine Ziffern   */
   {                                        /* -> Ergebnis = 0              */
      DEC_REFCNT(zw1);
      DEC_REFCNT(zw2);                      /* zw2 hat einen refcount zuviel */
      DEC_REFCNT(zw2);                      /* um das recycling in einer     */
      DEC_REFCNT(res);
      DIGNULL;
      INC_REFCNT(_dig0);
      DBUG_RETURN(_dig0);                        /* anderen Digit-Routine zu ver- */
   }                                        /* hindern.                      */

   exp_lauf = exp;
#if DEBUG
  if (debconf) printf("div: maxdigit:%d, exp_lauf:%d,cneu:%d, calt:%d\n",maxdigit,exp_lauf,cneu,calt);
  if (debconf) printf("div: v1:%ld, v2:%ld\n",v1,v2);
#endif

   while (cneu >= 0  &&   zw1 != _dig0)     /* solange "leere" Ziffern vor- */
   {                                        /* handen u. Rest != null       */
      ndigit = R_DIGIT((*zw1),ndigit);
      u1 = R_DIGIT((*zw1),Exp);
      d1 = u1 - ndigit + 1;

      if (calt <= u1  &&  calt >= d1)
         uj = R_DIGIT((*zw1),ptdv)[calt-d1];
      else
         uj = 0;

      if (calt - 1 <= u1  &&   calt - 1  >= d1)
         uj1 = R_DIGIT((*zw1),ptdv)[calt-d1-1];
      else
         uj1 = 0;

      if (calt - 2 <= u1  &&   calt - 2 >= d1)
         uj2 = R_DIGIT((*zw1),ptdv)[calt-d1-2];
      else
         uj2 = 0;

      help = uj * BASE + uj1;
      if (uj == v1)
         q = MAXNUM;
      else
         q = help  / v1;                           /* vorlaeufige Ergebnis- */

      while (v2 * q  >  ((help - v1 * q) * BASE + uj2))
      {                                              /* Ziffer korrigieren */
         q--;
#if DEBUG
 if (debconf) printf("div: q wird minimiert: q:%ld\n",q);
#endif
      }

#if DEBUG
  if (debconf) printf("div: uj:%ld, uj1:%ld, uj2:%ld, q:%ld, ndigit:%d\n",uj,uj1,uj2,q,ndigit);
#endif

      L_DIGIT((*_work),Exp) = exp_lauf;
      L_DIGIT((*_work),ptdv)[0] = q;                 /* q*Divisor = abzuziehender */
      hilf1 = (PTR_DESCRIPTOR) digit_mul(zw2,_work); /* Betrag. exp_lauf bestimmt */
                                                     /* den Exponenten des        */
                                                     /* abzuziehenden Betrages    */
                                                     /* falls Rest negativ, war   */
                                                     /* das vorlaeufige Ergebnis  */
                                                     /* zu gross. Ansonsten ist   */
                                                     /* das Ergebnis jetzt end-   */
                                                     /* gueltig                   */
      if (hilf1 == 0)
      {
         DEC_REFCNT(res);
         DEC_REFCNT(zw1);
         DEC_REFCNT(zw2);
         DEC_REFCNT(zw2);
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      while (digit_lt(zw1,hilf1) )
      {
#if DEBUG
 if (debconf)
 {
     printf("Nochmal der Deskriptor zw1\n");
     DescDump(stdout,zw1,1);
     printf("\n\n und hilf1, der im TRUEfall groesser sein soll\n");
     DescDump(stdout,hilf1,1);
 }
#endif
         DEC_REFCNT(hilf1);
         L_DIGIT((*_work),Exp) = exp_lauf;
         L_DIGIT((*_work),ptdv)[0] = --q;
         hilf1 = (PTR_DESCRIPTOR) digit_mul(zw2,_work);

         if (hilf1 == 0)
         {
            DEC_REFCNT(res);
            DEC_REFCNT(zw1);
            DEC_REFCNT(zw2);
            DEC_REFCNT(zw2);
            ABBRUCH;
            DBUG_RETURN((PTR_DESCRIPTOR)0);
         }
      }

      hilf2 = (PTR_DESCRIPTOR) digit_sub(zw1,hilf1); /* Ermitteln des Restes */
      DEC_REFCNT(hilf1);
      DEC_REFCNT(zw1);
      if (hilf2 == 0)
      {
         DEC_REFCNT(zw2);
         DEC_REFCNT(zw2);
         DEC_REFCNT(res);
         ABBRUCH;
         DBUG_RETURN((PTR_DESCRIPTOR)0);
      }
      zw1 = hilf2;

      L_DIGIT((*res),ptdv)[cneu] = q;                /* Ergebnisziffer eintragen */
#if DEBUG
  if (debconf) printf("**** div: Eingetragen q:%ld, cneu:%d, calt:%d\n",q,cneu,calt);
#endif
      cneu--;
      calt--;
      exp_lauf--;
   }       /* while */             /* eigentliches Dividieren beendet */

#if DEBUG
  if (debconf) printf("div: while Ende: cneu:%d, maxdigit:%d\n",cneu,maxdigit);
#endif
   cneu++;
   RES_HEAP;
   data3 = (unsigned long *) R_DIGIT((*res),ptdv);

   if ((j = cneu))                          /* Verschieben des Ausdrucks wegen */
   {                                        /* nichtausgefuellter Ziffern  */
      maxdigit -= cneu;
      for (i=0; i < maxdigit; i++, cneu++)
          data3[i] = data3[cneu];
   }
   while (data3[0] == 0)             /* Normieren hinten */
   {
      j = --maxdigit;
      for (i=0; i<j; i++)
          data3[i] = data3[i+1];
   }
   if (maxdigit)                            /* Normieren vorne */
      while (data3[maxdigit-1] == 0)
      {
           maxdigit--;
           exp--;
      }
   REL_HEAP;

   if (j > 9)
      cutheap(data3, maxdigit);



   DEC_REFCNT(zw1);
   DEC_REFCNT(zw2);
   DEC_REFCNT(zw2);                         /* Da zw2 gegen recycling ge- */
                                            /* schuetzt */
   if (maxdigit == 0)
   {
     INC_REFCNT(_dig0);
     DEC_REFCNT(res);
     DIGNULL;
     DBUG_RETURN(_dig0);
   }

#if DEBUG
 if (debconf) printf("div: geschafft\n");
#endif
   L_DIGIT((*res),sign) = sign;
   L_DIGIT((*res),Exp) = exp;
   L_DIGIT((*res),ndigit) = maxdigit;

   L_DIGIT((*desc1),sign) = sign1;
   L_DIGIT((*desc2),sign) = sign2;

   DBUG_RETURN(DIGIT_TEST(res,"digit_div",desc1,desc2));
}
