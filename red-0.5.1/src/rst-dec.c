/* $Log: rst-dec.c,v $
 * Revision 1.2  1992/12/16  12:50:58  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*----------------------------------------------------------------------------
 * MODUL DER PREPROCESSING PHASE
 * st-dec:c - external: st_dec
 * st-dec:c - internal:  keine;
 *----------------------------------------------------------------------------
 */

#include "rstdinc.h"
#if nCUBE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen und Variablen:
 *-----------------------------------------------------------------------------
 */

extern BOOLEAN _formated;    /* <r>, entscheidet zwischen fix- und var-Format */
extern PTR_DESCRIPTOR newdesc();
extern int newheap();
extern int vali();                 /* val:c */
extern double valr();              /* val:c */

/* Basis ungleich 10000 : */
extern _prec;                      /* <w> */
extern PTR_DESCRIPTOR digit_add();
extern PTR_DESCRIPTOR digit_mul();
extern PTR_DESCRIPTOR digit_div();
extern BOOLEAN digit_eq();         /* digit-v1:c */

/* RS 30/10/92 */
extern void stack_error();         /* rstack.c */
/* END of RS 30/10/92 */

/* RS 03/11/92 */
extern void DescDump();           /* rdesc.c */
extern void test_dec_refcnt();    /* rheap.c */
extern void test_inc_refcnt();    /* rheap.c */
#if nCUBE
extern STACKELEM *d_ppopstack();
extern DStackDesc D_S_hilf;
#else
extern STACKELEM *ppopstack();     /* rstack.c */
#endif
/* END of RS 03/11/92 */ 
 
/* Basis gleich 10000 : */
/* S_hilf   <w>         */


#define NEWHEAP(size,adr)  if ( newheap(size,adr) == 0) post_mortem("st_dec: Heap out of space");
#define INIT_DESC          if ((desc = newdesc()) == 0) post_mortem("st_dec: Heap out of space");
#define TEST(desc)         if ((desc) == 0)             post_mortem("st_dec: Heap out of space");

#if nCUBE
#define POP_H()        D_POPSTACK(D_S_H)
#define PUSH_HILF(arg) D_PUSHSTACK(D_S_hilf,arg)
#define READ_HILF()    D_READSTACK(D_S_hilf)
#define POP_HILF()     D_POPSTACK(D_S_hilf)
#define PPOP_HILF()    D_PPOPSTACK(D_S_hilf)
#else
#define POP_H()        POPSTACK(S_e)
#define PUSH_HILF(arg) PUSHSTACK(S_hilf,arg)
#define READ_HILF()    READSTACK(S_hilf)
#define POP_HILF()     POPSTACK(S_hilf)
#define PPOP_HILF()    PPOPSTACK(S_hilf)
#endif

 /*-----------------------------------------------------------------------------
  *  st_dec  --  legt einen auf dem E-Stack liegenden Zahlenstring in den Heap.
  *              Ist das Fix-Format vorgeschrieben, so entsteht ein Deskriptor
  *              der Klasse scalar. Die eigentliche Auslagerung gescheiht durch
  *              vali und valr. Sonst ensteht ein Deskriptor der Klasse digit.
  *              Der Ergebnisdeskriptor wird zurueckgegeben. Das Abspeichern
  *              im Var-Format ist besonders einfach, falls die Basis 10000
  *              gewaehlt wurde. Dann wird mittels des Hilfstacks die Zahl
  *              gleich von hier aus in den Heap geschaufelt. Sonst muessen
  *              die Digit-Routinen zur Hilfe genommen werden.
  *-----------------------------------------------------------------------------
  */

 STACKELEM st_dec()
 {
    register int exp/*,ndigit*/;            /* RS 30/10/92 */
    int  sign;
    STACKELEM x;
    PTR_DESCRIPTOR desc = NULL;
    PTR_DESCRIPTOR hilf_desc;

#if DEBUG
    extern int debconf;
#endif

    START_MODUL("st_dec");

    if (_formated)  /* dh. Darstellung der Decimalzahlen als Integer und Doubles */
    {
      int format;
      double r;

      r = valr(&format);  /* einlesen als Double */
      if (format == REAL)
      {
        INIT_DESC;
        DESC_MASK(desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*desc),valr) = r;
        END_MODUL("st_dec");				/* BM */
        return((STACKELEM) desc);			/* BM */
      }
      else
      {
        END_MODUL("st_dec");   				/* BM */
        return( (STACKELEM) TAG_INT( (int)r ) ) ; 	/* BM */
      }
    }

    if (BASE != 10000)
    {
#if DEBUG
      if (debconf)
      {
          printf("\n\n*********  st_dec gestartet  *********\n\n");
      }
#endif

      /* Hilfsvariable initialisieren */
      L_DIGIT((*_work),Exp) = 0;
      L_DIGIT((*_work),ndigit) = 1;
      L_DIGIT((*_work),sign) = PLUS_SIGN;

      if T_EXP((x = POP_H()))
      {
         exp = vali();
         x = POP_H();
      }
      else
         exp = 0;

#if DEBUG
      if (debconf)
            printf("exp: %d\n",exp);
#endif

      /* das Vorzeichen festhalten */
      sign = (T_PLUS(x) ? PLUS_SIGN : MINUS_SIGN);

      /* fuehrende Nullen entfernen */
      while (T_DIG1(x = POP_H()) && (DIGVAL(x) == 0));

      if (! T_DECPT(x)) /* eine echte Ziffer vor dem Komma */
      {
        /* neuen Descriptor anlegen */
        INIT_DESC;
        DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);
        L_DIGIT((*desc),Exp) = 0;
        L_DIGIT((*desc),ndigit) = 1;
        L_DIGIT((*desc),sign) = PLUS_SIGN;
        NEWHEAP(1,A_DIGIT((*desc),ptdv));
        L_DIGIT((*desc),ptdv)[0] = DIGVAL(x);        /* die erste Ziffer */

        /* die restlichen Ziffern bis zum Dezimalpunkt einlesen */
        while (! T_DECPT((x = POP_H())))
        {
           L_DIGIT((*_work),ptdv)[0] = 10;
           TEST(hilf_desc = digit_mul(desc,_work));  /* Zwischenergebnis mit */
           DEC_REFCNT(desc);                         /* 10 multiplizieren    */
           L_DIGIT((*_work),ptdv)[0] = DIGVAL(x);
           TEST(desc = digit_add(hilf_desc,_work));  /* die naechste Ziffer  */
           DEC_REFCNT(hilf_desc);                    /* dazuaddieren         */
        }
      }
      /* die restlichen Ziffern nach dem Komma einlesen */
      while (!T_DIG0(x))  /* falls die vorherige Ziffer nicht die letzte war */
      {
        x = POP_H();         /* die naechste Ziffer */
        exp--;
        if (desc != NULL)    /* Vielleicht haben wir noch keinen Deskriptor */
        {
          L_DIGIT((*_work),ptdv)[0] = 10;
          TEST(hilf_desc = digit_mul(desc,_work));   /* Zwischenergebnis mit */
          DEC_REFCNT(desc);                          /* 10 multiplizieren    */
          L_DIGIT((*_work),ptdv)[0] = DIGVAL(x);
          TEST(desc = digit_add(hilf_desc,_work));   /* die naechste Ziffer  */
          DEC_REFCNT(hilf_desc);                     /* dazuaddieren         */
        }
        else /* desc == NULL */
          if (DIGVAL(x) != 0)  /* sonst brauchen wir noch keinen Deskriptor */
          {
            /* neuen Descriptor anlegen */
            INIT_DESC;
            DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);
            L_DIGIT((*desc),Exp) = 0;
            L_DIGIT((*desc),ndigit) = 1;
            L_DIGIT((*desc),sign) = PLUS_SIGN;
            NEWHEAP(1,A_DIGIT((*desc),ptdv));
            L_DIGIT((*desc),ptdv)[0] = DIGVAL(x);
          }
      }
      /* alle Ziffern wurden eingelesen */

      /* den Fall der Null abfangen */
      if (desc == NULL)
      {
        INC_REFCNT(_dig0);  /* Die Zahl 0 hat nur einen Deskriptor */
        END_MODUL("st_dec");
        return((STACKELEM)_dig0);
      }

#if DEBUG
      if (debconf)
         printf("exp: %d\n", exp);
#endif

      /* das Vorzeichen einbauen */
      L_DIGIT((*desc),sign) = sign;

      /* den Exponenten einbauen */
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

        /* was soll geschehen, falls beispielsweise _prec gleich Null, und
           eine 0.1 eigegeben wurde. Soll daraus eine Null werden? Wohl nein !
           Da davon ausgegangen werden kann, dass die Basis durch 10 teilbar
           ist, kann sich bei der Division durch 10, die Zahl der Ziffern
           hoechstens um einen erhoehen. Deshalb:                           */

        _prec = R_DIGIT((*desc),ndigit) + 1;
        /* so verlieren wir keine Ziffer ! */

        TEST(hilf_desc = digit_div(desc,_work));

        _prec = save_prec;         /* urspruengliche Praezision restaurieren */
        DEC_REFCNT(desc);
        desc = hilf_desc;
      }

      /* fuer die Zahl 1 gibt es auch schon einen Deskriptor ! */
      if (digit_eq(desc,_dig1))
      {
        DEC_REFCNT(desc);
        INC_REFCNT(_dig1);
        desc = _dig1;
      }
      END_MODUL("st_dec");
      return((STACKELEM) desc);
    }

    else /* BASE == 10000 */

    {
      register int i,dimv,ndigit;
      int  *data, sum;

#if DEBUG
      if (debconf)
      {
          printf("\n\n*********  st_dec gestartet  *********\n\n");
      }
#endif

      x = POP_H();  /* Vorzeichen oder Exponentzeich moeglich */

      if (T_EXP(x))
      {
         exp = vali();
         x = POP_H();
      }
      else
         exp = 0;

#if DEBUG
      if (debconf)
            printf("exp: %d\n",exp);
#endif

      /* das Vorzeichen festhalten */
      sign = T_PLUS(x) ? PLUS_SIGN : MINUS_SIGN;

      /* fuehrende Nullen entfernen */
      while (T_DIG1(x = POP_H()) && (DIGVAL(x) == 0));

      /* Ziffern bis zum Decimalpunkt einlesen */
      dimv = 0;
      while (! T_DECPT(x))
      {
         dimv++;
         PUSH_HILF(x);
         x = POP_H();
      }

      /* die restlichen Ziffern einlesen */
      while (!T_DIG0(x)) /* solange die vorherige Ziffern nicht die letzte war */
      {
        x = POP_H();         /* die naechste Ziffer */
        PUSH_HILF(x);
        dimv++;
        exp--;
      }
        /* jetzt liegt die Zahl vollstaendig auf dem Hilfstack */

      /* Nachfolgende Nullen entfernen */
      while ((dimv > 0) && (DIGVAL(READ_HILF()) == 0))
      {
        PPOP_HILF();
        dimv--;
        exp++;
      }

      /* den Fall der Null abfangen */
      if (dimv == 0)
      {
        INC_REFCNT(_dig0);
        END_MODUL("st_dec");
        return((STACKELEM)_dig0);
      }
#if DEBUG
      if (debconf)
         printf("dimv: %d, exp: %d\n",dimv, exp);
#endif

      /* Exponenten ausrichten */
      if ((i = exp % 4) < 0) i += 4;
      for( ; i > 0; i--)
      {
        PUSH_HILF(DIGSTACK(0));
        dimv++;
        exp--;
      }
      exp /=4;


      /* Zahl der benoetigten Ziffern ermitteln */
      ndigit = (dimv + 3) >> 2;

      /* den Exponenten so umrechnen, dass er von Exponenten der letzten 10000er
         Ziffer zum Exponenten der ersten 10000er Ziffer wird. */
      exp += ndigit - 1;

      /* den neuen Descriptor anlegen */
      INIT_DESC;
      DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);
      L_DIGIT((*desc),sign)   = sign;
      L_DIGIT((*desc),Exp )   = exp;
      L_DIGIT((*desc),ndigit) = ndigit;
      NEWHEAP(ndigit, A_DIGIT((*desc),ptdv));
      data = (int*) R_DIGIT((*desc),ptdv);

      /* Die Ziffern fuellen */
      for ( i = 0; i < ndigit; i++)
      {
         sum = DIGVAL(POP_HILF());
         dimv--;
         if (dimv > 0) sum += DIGVAL(POP_HILF()) * 10;
         dimv--;
         if (dimv > 0) sum += DIGVAL(POP_HILF()) * 100;
         dimv--;
         if (dimv > 0) sum += DIGVAL(POP_HILF()) * 1000;
         dimv--;
         data[i] = sum;
#if DEBUG
         if (debconf)
         {
             printf("Element: %d, Inhalt: %d\n",i,data[i]);
         }
#endif
      }

      while (R_DIGIT((*desc),ptdv)[ndigit-1] == 0)    /* Normieren vorne */
      {
         L_DIGIT((*desc),ndigit) = --ndigit;
         L_DIGIT((*desc),Exp)--;
      }

      END_MODUL("st_dec");
      return((STACKELEM) desc);
    }
}
