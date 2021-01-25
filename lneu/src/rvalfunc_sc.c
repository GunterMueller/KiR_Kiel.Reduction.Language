/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*****************************************************************************/
/*                          MODUL FOR PROCESSING PHASE                       */
/*---------------------------------------------------------------------------*/
/* Diese Datei enthaelt alle werttransformierden Funktionen, mit Ausnahme    */
/* derjenigen, welche auf boolean arbeiten, bzw. denen Vergleiche zugrunde   */
/* liegen, sowie die tranzendenten Funktionen                                */
/*                      red_sin,  red_cos,  red_tan,  red_ln,  red_exp       */
/*                      welche sich in dem File rbibfunc:c befinden.         */
/*                                                                           */
/* rvalfunc.c  -- externals : red_add,  red_minus,  red_mult,  red_div,      */
/*                            red_mod,  red_ip,     red_abs,   red_trunc,    */
/*                            red_frac, red_floor,  red_ceil,  red_c,        */
/*                            red_vc                                         */
/*             -- called by : rear.c                                         */
/*             -- internals : sind in rval-int.c zu finden.                  */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rops.h"
#include "rmeasure.h"
#include <setjmp.h>

/*--------------------------------------------------------------------------
 * Allgemein verwendete externe Variablen:
 *--------------------------------------------------------------------------
 */
extern jmp_buf _interruptbuf;             /* <r>, Sprungadresse bei heapoverflow */
extern PTR_DESCRIPTOR _desc;              /* <w>, der Ergebnisdeskriptor */

/*--------------------------------------------------------------------------
 * Allgemein verwendete externe Routinen:
 *--------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();            /* rheap:c */
extern int newheap();                       /* rheap:c */

/* ---- Routine zur Erzeugung indirekter Deskriptoren: ---- */
extern PTR_DESCRIPTOR gen_many_id();        /* rconvert:c */

/*--------------------------------------------------------------------------
 * Allgemein verwendete Makros und Definitionen:
 *--------------------------------------------------------------------------
 */
/* Makros zum Abfangen von heapoverflows */
#define NEWHEAP(size,adr)  {if (newheap(size,adr) == 0)  longjmp(_interruptbuf,0);}
#define TEST_DESC          {if (_desc == NULL)           longjmp(_interruptbuf,0);}
#define TEST(desc)         {if ((desc) == NULL)          longjmp(_interruptbuf,0);}

#define DESC(x)  (* (PTR_DESCRIPTOR) x)
#define IPNEW    ((int *) R_MVT((*_desc),ptdv,class))
#define IP2      ((int *) R_MVT(DESC(arg2),ptdv,C_MATRIX))

/* ---- Makro zur Klassenbestimmung in numerischen Skalaren: ---- */
#define NUM_CLASS(type)  ((type) == TY_DIGIT ? C_DIGIT : C_SCALAR)




/*---------------------------------------------------------------------------*/
/* red_plus -- zwei Objekte werden addiert.                                  */
/*---------------------------------------------------------------------------*/
red_plus(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;
  char  class1, class2;

  START_MODUL("red_plus");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class2==C_SCALAR))
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 0)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = i1 + i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 0)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = i1 + r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 0.0)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 + i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 0.0)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 + r2;
                      goto scalar_success;
                    }
  }
fail:
    END_MODUL("red_plus");
    return(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_plus");
    return(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_plus");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_plus");
    return(1);
}


/*---------------------------------------------------------------------------*/
/* red_minus -- zwei Objekte werden subtrahiert                               */
/*---------------------------------------------------------------------------*/
red_minus(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;
  char  class1, class2;

  START_MODUL("red_minus");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class2==C_SCALAR))
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = R_SCALAR(DESC(arg1),vali) - i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),vali) - r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) - i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) - r2;
                      goto scalar_success;
                    }
  }
fail:
    END_MODUL("red_minus");
    return(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_minus");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_minus");
    return(1);
}


/*---------------------------------------------------------------------------*/
/* red_mult -- zwei Objekte werden multipliziert                             */
/*---------------------------------------------------------------------------*/
red_mult(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;
  char  class1, class2;

  START_MODUL("red_mult");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class2==C_SCALAR))
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 1)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = i1 * i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 1)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = i1 * r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 1.0)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 * i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 1.0)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 * r2;
                      goto scalar_success;
                    }
  }
fail:
    END_MODUL("red_mult");
    return(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_mult");
    return(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_mult");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mult");
    return(1);
}

/*---------------------------------------------------------------------------*/
/* red_div  -- zwei Objekte werden dividiert                                 */
/*---------------------------------------------------------------------------*/
red_div(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;
  char  class1, class2;

  START_MODUL("red_div");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class2==C_SCALAR))
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto fail;
                      if (i2 == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = R_SCALAR(DESC(arg1),vali) / i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto fail;
                      if (r2 == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),vali) / r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto fail;
                      if (i2 == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto fail;
                      if (r2 == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / r2;
                      goto scalar_success;
                    }
  }
fail:
    END_MODUL("red_div");
    return(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_div");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_div");
    return(1);
}


/*---------------------------------------------------------------------------*/
/* red_mod  -- der Modulo zweier Objekte wird gebildet                       */
/*---------------------------------------------------------------------------*/
red_mod(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;
  char  class1, class2;

  START_MODUL("red_mod");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class2==C_SCALAR))
          if ((R_DESC(DESC(arg1),type) == TY_INTEGER) && (R_DESC(DESC(arg2),type) == TY_INTEGER))
               {
               if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                   goto fail;
               NEWDESC(_desc); TEST_DESC;
               DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
               L_SCALAR((*_desc),vali) = R_SCALAR(DESC(arg1),vali) % i2;
               goto scalar_success;
               }
  }
fail:
    END_MODUL("red_mod");
    return(0);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mod");
    return(1);
}


/*------------------------------------------------------------------------
 * red_abs --
 *
 * Anwendung:                 abs(argument)
 *------------------------------------------------------------------------
 */
 red_abs(arg1)
register STACKELEM arg1;
{
  int i;
  double r;

  START_MODUL("red_abs");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
             if ((i = R_SCALAR(DESC(arg1),vali)) > 0)
             {
               _desc = (PTR_DESCRIPTOR)arg1;
               goto success;
             }
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             L_SCALAR((*_desc),vali) = -i;
             DEC_REFCNT((PTR_DESCRIPTOR)arg1);
             goto success;
           }
           else
           {
             if ((r = R_SCALAR(DESC(arg1),valr)) > 0.0)
             {
               _desc = (PTR_DESCRIPTOR)arg1;
               goto success;
             }
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
             L_SCALAR((*_desc),valr) = -r;
             DEC_REFCNT((PTR_DESCRIPTOR)arg1);
             goto success;
           }
  }
  END_MODUL("red_abs");
  return(0);
success:
  END_MODUL("red_abs");
  return(1);
}

/*------------------------------------------------------------------------
 * red_neg --
 *
 * Anwendung:                 neg(argument)
 *------------------------------------------------------------------------
 */
 red_neg(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_neg");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             L_SCALAR((*_desc),vali) = - R_SCALAR(DESC(arg1),vali);
             goto success;
           }
           else
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
             L_SCALAR((*_desc),valr) = - R_SCALAR(DESC(arg1),valr);
             goto success;
           }
  }
  END_MODUL("red_neg");
  return(0);
success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_neg");
  return(1);
}

/*------------------------------------------------------------------------
 * red_trunc --
 *
 * Anwendung:      trunc(argument)  oder  frac(argument)
 *
 * Funktion:
 *
 *    Die Funktion trunc erzeugt eine Kopie des Argumentes ohne Nach-
 *    kommastellen, waehrend frac eine ohne Vorkommastellen erzeugt.
 *
 * Beispiele:
 *    trunc( 1.23 )           =>  1
 *     frac( 1.23 )           =>  0.23
 *------------------------------------------------------------------------
 */
 red_trunc(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_trunc");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
               _desc = (PTR_DESCRIPTOR)arg1;
               goto success;
           }
           else
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             L_SCALAR((*_desc),vali) =  R_SCALAR(DESC(arg1),valr);
             DEC_REFCNT((PTR_DESCRIPTOR)arg1);
             goto success;
           }
  }
  END_MODUL("red_trunc");
  return(0);
success:
  END_MODUL("red_trunc");
  return(1);
}

/*------------------------------------------------------------------------
 * red_frac -- berechnet den faktional Part.
 *------------------------------------------------------------------------
 */
 red_frac(arg1)
register STACKELEM arg1;
{
  double r;

  START_MODUL("red_frac");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             L_SCALAR((*_desc),vali) = 0;
             goto success;
           }
           else
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
             r = R_SCALAR(DESC(arg1),valr);
             L_SCALAR((*_desc),valr) = r - (int) r;
             goto success;
           }
  }
  END_MODUL("red_frac");
  return(0);
success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_frac");
  return(1);
}

/*------------------------------------------------------------------------
 * red_floor --
 *
 * Anwendung:    ceil(argument)   oder   floor(argument)
 *
 * Funktion:
 *
 *    ceil angewendet auf eine Zahl liefert die naechst
 *    groessere ganze Zahl. floor dagegen liefert die naechst kleinere
 *    Zahl. Auf Strukturen werden die Funktionen elementweise angewendet.
 *
 * Beispiele:
 *     ceil( 5.7 )               =>  6
 *    floor( 5.7 )               =>  5
 *------------------------------------------------------------------------
 */
 red_floor(arg1)
register STACKELEM arg1;
{
  double r;

  START_MODUL("red_floor");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
               _desc = (PTR_DESCRIPTOR)arg1;
               goto success;
           }
           else
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             r = R_SCALAR(DESC(arg1),valr);
             L_SCALAR((*_desc),vali) = (r >= 0 ? r : r - 1);
             DEC_REFCNT((PTR_DESCRIPTOR)arg1);
             goto success;
           }
  }
  END_MODUL("red_floor");
  return(0);
success:
  END_MODUL("red_floor");
  return(1);
}

/*------------------------------------------------------------------------
 * red_ceil -- berechnet die zu einer Zahl gehoerige naechst groessere
 *             ganze Zahl.
 *             Ihre Funktion wird bei red_floor erlaeutert.
 *------------------------------------------------------------------------
 */
 red_ceil(arg1)
register STACKELEM arg1;
{
  int i;
  double r;

  START_MODUL("red_ceil");

  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) == C_SCALAR)
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
           {
               _desc = (PTR_DESCRIPTOR)arg1;
               goto success;
           }
           else
           {
             NEWDESC(_desc); TEST_DESC;
             DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
             i = r = R_SCALAR(DESC(arg1),valr);
             L_SCALAR((*_desc),vali) = (r <= 0.0 ? i : i + 1);
             DEC_REFCNT((PTR_DESCRIPTOR)arg1);
             goto success;
           }
  }
  END_MODUL("red_ceil");
  return(0);
success:
  END_MODUL("red_ceil");
  return(1);
}

/*****************************  end of file rvalfunc.c  **********************/
