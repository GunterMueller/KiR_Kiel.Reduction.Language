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
extern BOOLEAN _digit_recycling;          /* <w>, Steuerflag fuer die Digitroutinen */

/*--------------------------------------------------------------------------
 * Allgemein verwendete externe Routinen:
 *--------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();            /* rheap:c */
extern int newheap();                       /* rheap:c */
extern PTR_DESCRIPTOR  digit_add();
extern PTR_DESCRIPTOR  digit_sub();
extern PTR_DESCRIPTOR  digit_div();
extern PTR_DESCRIPTOR  digit_mul();
extern PTR_DESCRIPTOR  digit_mod();
extern PTR_DESCRIPTOR  digit_abs();         /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_neg();         /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_trunc();       /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_frac();        /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_floor();       /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_ceil();        /* rdig-v2:c */
extern int             conv_digit_int();    /* rdig-v1:c */
extern BOOLEAN         digit_eq();          /* rdig-v1:c */
extern BOOLEAN         digit_lt();          /* rdig-v1:c */
extern BOOLEAN         digit_le();          /* rdig-v1:c */

/* ---- ('mvt' steht fuer Matrix/Vektor/TVektor) ---- */
extern PTR_DESCRIPTOR scal_mvt_op();        /* rval-int:c */
extern PTR_DESCRIPTOR digit_mvt_op();       /* rval-int:c */
extern PTR_DESCRIPTOR mvt_mvt_op();         /* rval-int:c */
extern PTR_DESCRIPTOR mvt_op();             /* rval-int:c */

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

/* digit_add und digit_sub wird die Vorzeichenbetrachtung abgenommen: */
#define DI_ADD(x,y) (R_DIGIT(DESC(x),sign)==0  \
                    ? (R_DIGIT(DESC(y),sign)==0  \
                      ? digit_add(x,y) : digit_sub(x,y) ) \
                    : (R_DIGIT(DESC(y),sign)==0 \
                      ? digit_sub(y,x) : digit_add(x,y) ) )
#define DI_SUB(x,y)  (R_DIGIT(DESC(x),sign) == 0   \
                     ? (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_sub(x,y) : digit_add(x,y) ) \
                     : (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_add(x,y) : digit_sub(y,x) ) )

/* ---- Makro zur Klassenbestimmung in numerischen Skalaren: ---- */
#define NUM_CLASS(type)  ((type) == TY_DIGIT ? C_DIGIT : C_SCALAR)

/*---------------------------------------------------------------------------*/
/* Grundsaetzliche Bemerkungen zu den arithmetrischen Operationen:           */
/*   Zwei Argumente koennen verknuepft werden: wenn sie entweder vom         */
/*   Typ Digit sind, oder vom Typ Real und Integer. Digits und Reals bzw.    */
/*   Digits und Integer sind nicht kompatibel.                               */
/*   Falls Integer und Reals miteinander verknuepft werden so entsteht ein   */
/*   Datenobjekt des Typs Real.                                              */
/*   Matrizen, Vektoren, Tvektoren und Scalare bzw. Digits werden            */
/*   elementweise verknuepft. Falls eine Verknuepfung mit einem bezueglich   */
/*   dieser Verknuepfung neutralen Element geschieht, so wird dieses erkannt.*/
/*                                                                           */
/* Grundsaetzlich werden saemmtliche Operationen, welche Felder betreffen,   */
/*   an spezielle Feldroutinen weitergereicht, die die Operation als letzten */
/*   Parameter erhalten.                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* red_plus -- zwei Objekte werden addiert.                                  */
/*                                                                           */
/* Funktionsbeschreibung fuer  +, -, *, /, mod :                             */
/*                            -----------------                              */
/*                                                                           */
/*    Diese Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren von    */
/*    Zahlen anwendbar. Zahlen koennen mit Matrizen bzw. Vektoren von Zahlen */
/*    verknuepft werden. Es entsteht eine neue Matrix bzw. Vektor dessen     */
/*    Elemente aus der Verknuepfung der Zahl mit den einzelnen Komponenten   */
/*    hervorgehen. Werden zwei Matrizen bzw. Vektoren verknuepft, so muessen */
/*    beide die gleiche Zeilen- und Spaltenzahl haben. Die Modulo-           */
/*    Funktion ist auch auf Digit-Zahlen anwendbar. Ist der Parameter        */
/*    varformat gesetzt, werden alle Operationen, bis auf die Division       */
/*    und die Moduloberechnung exakt ausgefuehrt.                            */
/*    Bei fixformat erfolgt die Berechnung im Rahmen der                     */
/*    Integer- bzw. Realarithmetik (siehe auch Decimal).                     */
/*                                                                           */
/* Beispiele:                                                                */
/*                  ( 1.5 / 6)                  => 0.25                      */
/*            ( vect<1,2,~1> + 1 )              => vect<2,3,0>               */
/*            ( tvect<1,2,~1> + 1 )             => tvect<2,3,0>              */
/*           ( 4 - mat<1,2>,<3,4>. )            => mat<3,2>,<1,0>.           */
/*    ( mat<1,2>,<3,4>. * mat<1,2>,<3,4>. )     => mat<1,4>,<9,16>.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
red_plus(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;

  START_MODUL("red_plus");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
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
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if (_desc = scal_mvt_op(arg1,arg2,OP_ADD))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = DI_ADD(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig0,arg1))
                    goto res_arg2;
                  if (_desc = digit_mvt_op(arg1,arg2,OP_ADD))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (_desc = scal_mvt_op(arg2,arg1,OP_ADD))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig0,arg2))
                    goto res_arg1;
                  if (_desc = digit_mvt_op(arg2,arg1,OP_ADD))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = mvt_mvt_op(arg1,arg2,OP_ADD))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }
fail:
    END_MODUL("red_plus");
    return(0);
success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_plus");
    return(1);
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

/*---------------------------------------------------------------------------
 * red_minus -- zwei Datenobjekte werden voneinander subtrahiert.
 *              Die Aufgabe der Funktion wird bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
 red_minus(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i2;
  double r2;

  START_MODUL("red_minus");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
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
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if (_desc = scal_mvt_op(arg1,arg2,OP_SUB))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = DI_SUB(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = digit_mvt_op(arg1,arg2,OP_SUB))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (_desc = scal_mvt_op(arg2,arg1,OP_SUBI))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig0,arg2))
                    goto res_arg1;
                  if (_desc = digit_mvt_op(arg2,arg1,OP_SUBI))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = mvt_mvt_op(arg1,arg2,OP_SUB))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }
fail:
    END_MODUL("red_minus");
    return(0);
success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_minus");
    return(1);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_minus");
    return(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_minus");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_minus");
    return(1);
}

/*---------------------------------------------------------------------------
 * red_mult -- zwei Datenobjekte werden multipliziert.
 *              Die Aufgabe der Funktion wird bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
 red_mult(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2;
  double r1,r2;

  START_MODUL("red_mult");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 0)
                        goto res_arg1;
                      if (i1 == 1)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg2;
                      if (i2  == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = i1 * i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((i1 = R_SCALAR(DESC(arg1),vali)) == 0)
                        goto res_arg1;
                      if (i1 == 1)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg2;
                      if (r2 == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = i1 * r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 0.0)
                        goto res_arg1;
                      if (r1 == 1.0)
                        goto res_arg2;
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 0)
                        goto res_arg2;
                      if (i2  == 1)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 * i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r1 = R_SCALAR(DESC(arg1),valr)) == 0.0)
                        goto res_arg1;
                      if (r1 == 1.0)
                        goto res_arg2;
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                        goto res_arg2;
                      if (r2 == 1.0)
                        goto res_arg1;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = r1 * r2;
                      goto scalar_success;
                    }
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if (_desc = scal_mvt_op(arg1,arg2,OP_MULT))
                      goto success;
                    goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = digit_mul(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig1,arg1))
                    goto res_arg2;
                  if (_desc = digit_mvt_op(arg1,arg2,OP_MULT))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (_desc = scal_mvt_op(arg2,arg1,OP_MULT))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig1,arg2))
                    goto res_arg1;
                  if (_desc = digit_mvt_op(arg2,arg1,OP_MULT))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = mvt_mvt_op(arg1,arg2,OP_MULT))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }
fail:
    END_MODUL("red_mult");
    return(0);
success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mult");
    return(1);
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

/*---------------------------------------------------------------------------
 * red_div -- zwei Datenobjekte werden dividiert. Dabei ist zu beachten,
 *            dass das Ergebnis einer Division zweier ganzzahliger Zahlen
 *            in einer nicht ganzzahligem Zahlenformat abgelegt wird.
 *            Will man z.B. den Ganzzahligen Anteil von int3 / int2 haben,
 *            so ist anschliessend die Operation trunc anzuwenden.
 *            Der Grund fuer die Abweichung vom C-Standard liegt darin, dass
 *            digit 3 / digit2 = digit 1.5 ergibt.
 *            Ansonsten wird die Aufgabe der Funktion bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
 red_div(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i2;
  double r2;

  START_MODUL("red_div");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 1)
                        goto res_arg1;
                      if (i2 == 0) goto fail;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = ((double)R_SCALAR(DESC(arg1),vali)) / ((double)i2);
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 1.0)
                        goto res_arg1;
                      if (r2 == 0.0) goto fail;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),vali) / r2;
                      goto scalar_success;
                    }
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                    {
                      if ((i2 = R_SCALAR(DESC(arg2),vali)) == 1)
                       goto res_arg1;
                      if (i2 == 0) goto fail;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / i2;
                      goto scalar_success;
                    }
                    else
                    {
                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 1.0)
                        goto res_arg1;
                      if (r2 == 0.0) goto fail;
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / r2;
                      goto scalar_success;
                    }
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if (_desc = scal_mvt_op(arg1,arg2,OP_DIV))
                      goto success;
                    goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  if (_desc = digit_div(arg1,arg2))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = digit_mvt_op(arg1,arg2,OP_DIV))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (_desc = scal_mvt_op(arg2,arg1,OP_DIVI))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig1,arg2))
                    goto res_arg1;
                  if (_desc = digit_mvt_op(arg2,arg1,OP_DIVI))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = mvt_mvt_op(arg1,arg2,OP_DIV))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }

fail:
    END_MODUL("red_div");
    return(0);
success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_div");
    return(1);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_div");
    return(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_div");
    return(1);
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_div");
    return(1);
}

/*---------------------------------------------------------------------------*/
/* red_mod -- berechnet den Modulo zweier Datenobjekte vom Typ Integer und   */
/*            vom Typ Digit.                                                 */
/*            Die Aufgabe der Funktion wird bei red_plus erlaeutert.         */
/*---------------------------------------------------------------------------*/
red_mod(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int i1,i2,m;
  double x,y,help,fractal;

  START_MODUL("red_mod");

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((R_DESC(DESC(arg1),type) == TY_INTEGER) &&
                      (R_DESC(DESC(arg2),type) == TY_INTEGER))
                  {
                    i1 = R_SCALAR(DESC(arg1),vali);
                    i2 = R_SCALAR(DESC(arg2),vali);
                    if (i2 == 0) goto fail;

                    NEWDESC(_desc); TEST_DESC;
                    DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                    L_SCALAR((*_desc),vali) = i1 % i2;
                    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
                    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
                    END_MODUL("red_mod");
                    return(1);
                  }
                  else {
                    if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                      x = R_SCALAR(DESC(arg1),vali);
                    else
                      x = R_SCALAR(DESC(arg1),valr);

                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      y = R_SCALAR(DESC(arg2),vali);
                    else
                      y = R_SCALAR(DESC(arg2),valr);

                    help = x / y;
                    m    = help;                           /* trunc( (x/y) ) */
                    help = x - m * y;

                    fractal = help - (int)help;

                    if (fractal == 0) {
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
                      L_SCALAR((*_desc),vali) = (int)help; /* trunc(help)    */
                    }
                    else {
                      NEWDESC(_desc); TEST_DESC;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                      L_SCALAR((*_desc),valr) = help;
                    }

                    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
                    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
                    END_MODUL("red_mod");
                    return(1);
                  }
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if (_desc = scal_mvt_op(arg1,arg2,OP_MOD))
                      goto success;
                    goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  if (_desc = digit_mod(arg1,arg2))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = digit_mvt_op(arg1,arg2,OP_MOD))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if (_desc = scal_mvt_op(arg2,arg1,OP_MODI))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (_desc = digit_mvt_op(arg2,arg1,OP_MODI))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (_desc = mvt_mvt_op(arg1,arg2,OP_MOD))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }

fail:
    END_MODUL("red_mod");
    return(0);
success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mod");
    return(1);
}

/*------------------------------------------------------------------------
 * red_abs --
 *
 * Anwendung:                 abs(argument)
 *
 * Funktion:
 *
 *    abs liefert den Absolutwert von Zahlen,Matrizen und Vektoren.
 *    Der Absolutwert einer Struktur ist eine Struktur. Ihre Komponenten
 *    sind die Absolutwerte der Komponenten der Argumentstruktur.
 *
 *    Beispiele:
 *
 *        abs( 1 )               =>  1
 *        abs( ~2 )              =>  2
 *        abs( vect<1,~2,~3> )   =>  vect<1,2,3>
 *        abs( tvect<1,~2,~3> )  =>  tvect<1,2,3>
 *        abs( mat<1,~2,~3>. )   =>  mat<1,2,3>.
 *
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
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_abs(arg1));
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_ABS)) == 0) goto fail;
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      default: goto fail;
    }
  }
fail:
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
 *
 * Funktion:
 *
 *    neg ist nur auf Zahlen und Matrizen bzw. Vektoren von Zahlen anwendbar.
 *    Er erzeugt eine Kopie des Argumentes, in der alle Vorzeichen
 *    umgekehrt worden sind.
 *
 *    Beispiele:
 *
 *        neg( 1 )               =>  ~1
 *        neg( ~2 )              =>  2
 *        neg( vect<1,~2,~3> )   =>  vect<~1,2,3>
 *        neg( tvect<1,~2,~3> )  =>  tvect<~1,2,3>
 *        neg( mat<1,~2,~3>. )   =>  mat<~1,2,3>.
 *
 *------------------------------------------------------------------------
 */
 red_neg(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_neg");

  if T_POINTER(arg1)
  {
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_neg(arg1));
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_NEG)) == 0) goto fail;
           goto success;
      default: goto fail;
    }
  }
fail:
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
 *    Beide Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren
 *    von Zahlen definiert.
 *    Die Funktion trunc erzeugt eine Kopie des Argumentes ohne Nach-
 *    kommastellen, waehrend frac eine ohne Vorkommastellen erzeugt.
 *
 * Beispiele:
 *    trunc( 1.23 )           =>  1
 *    trunc( vect<1,2.4> )    =>  vect<1,2>
 *    trunc( tvect<1,2.4> )   =>  tvect<1,2>
 *    trunc( mat<1,2.4>. )    =>  mat<1,2>.
 *     frac( 1.23 )           =>  0.23
 *     frac( vect<1,2.4> )    =>  vect<0,0.4>
 *     frac( tvect<1,2.4> )   =>  tvect<0,0.4>
 *     frac( mat<1,2.4>. )    =>  mat<0,0.4>.
 *
 *------------------------------------------------------------------------
 */
 red_trunc(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_trunc");

  if T_POINTER(arg1)
  {
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_trunc(arg1));
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_TRUNC)) == 0) goto fail;
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      default: goto fail;
    }
  }
fail:
  END_MODUL("red_trunc");
  return(0);
success:
  END_MODUL("red_trunc");
  return(1);
}

/*------------------------------------------------------------------------
 * red_frac -- berechnet den faktional Part.
 *             Ihre Funktion wird in red_trunc erlaeutert.
 *------------------------------------------------------------------------
 */
 red_frac(arg1)
register STACKELEM arg1;
{
  double r;

  START_MODUL("red_frac");

  if T_POINTER(arg1)
  {
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_frac(arg1));
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_FRAC)) == 0) goto fail;
           goto success;
      default: goto fail;
    }
  }
fail:
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
 *    Diese beiden Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren
 *    von Zahlen definiert.
 *    ceil angewendet auf eine Zahl liefert die naechst
 *    groessere ganze Zahl. floor dagegen liefert die naechst kleinere
 *    Zahl. Auf Strukturen werden die Funktionen elementweise angewendet.
 *
 * Beispiele:
 *     ceil( 5.7 )               =>  6
 *     ceil( vect<~0.1,0,0.2> )  => vect<0,0,1>
 *     ceil( tvect<~0.1,0,0.2> ) => tvect<0,0,1>
 *     ceil( mat<~0.1,0,0.2>. )  => mat<0,0,1>.
 *    floor( 5.7 )               =>  5
 *    floor( vect<~0.1,0,0.2> )  => vect<~1,0,0>.
 *    floor( tvect<~0.1,0,0.2> ) => tvect<~1,0,0>.
 *    floor( mat<~0.1,0,0.2>. )  => mat<~1,0,0>.
 *
 *------------------------------------------------------------------------
 */
 red_floor(arg1)
register STACKELEM arg1;
{
  double r;

  START_MODUL("red_floor");

  if T_POINTER(arg1)
  {
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_floor(arg1));
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_FLOOR)) == 0) goto fail;
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      default: goto fail;
    }
  }
fail:
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
    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
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
      case C_DIGIT:
           TEST(_desc = digit_ceil(arg1));
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           if ((_desc = mvt_op(arg1,OP_CEIL)) == 0) goto fail;
           DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           goto success;
      default: goto fail;
    }
  }
fail:
  END_MODUL("red_ceil");
  return(0);
success:
  END_MODUL("red_ceil");
  return(1);
}


/*--------------------------------------------------------------------------
 * red_ip --
 *
 * Anwendung:             (... ip ...)
 *
 * Funktion:
 *
 *    ip bildet das innere Produkt zwischen Feldern und Matrizen vom Typ Decimal.
 *        (siehe Jacobsen, Struck: "Zur Diskussion der Einfuehrung des Datentyps
 *                                  Vektor in die HUSCH-Reduktionsmaschine"
 *                                  (Interner Bericht 5.Oktober '88, S.24 ff) )
 *
 * Beispiele:
 *                 ...
 *
 *--------------------------------------------------------------------------
 */
 red_ip(arg1,arg2)
register PTR_DESCRIPTOR arg1,arg2;
{
  register int i,j,k;
  int pos,nrows1,nrows2,ncols1,ncols2;
  int     isum,*ipnew,*ip1,*ip2;
  double  rsum,*rpnew,*rp1,*rp2;
  char    class1,                             /*        class of arg1        */
          class2,                             /*        class of arg2        */
          res_class;                          /*       class of result       */
  PTR_DESCRIPTOR digit_sum,digit1,digit2;

  START_MODUL("red_ip");

  if (!T_POINTER((STACKELEM)arg1))
    goto fail;
  if (!T_POINTER((STACKELEM)arg2))
    goto fail;

  class1 = R_DESC((*arg1),class);
  class2 = R_DESC((*arg2),class);

  nrows1 = R_MVT((*arg1),nrows,class1);
  nrows2 = R_MVT((*arg2),nrows,class2);
  ncols1 = R_MVT((*arg1),ncols,class1);
  ncols2 = R_MVT((*arg2),ncols,class2);

  if (ncols1 != nrows2) goto fail;

  switch(class1)                              /* Bestimmung der Klasse des   */
  {                                           /* Resultats; Erzeugung der    */
  /*---------------*/                         /* Resultatsdeskriptoren;      */
    case C_VECTOR:
  /*---------------*/
        switch(class2)
        {
          case C_VECTOR:
          case C_TVECTOR:
              res_class = NUM_CLASS( R_DESC((*arg1),type) );
              break;
          case C_MATRIX:
              if (ncols2 == 1)
                res_class = NUM_CLASS( R_DESC((*arg1),type) );
              else
                res_class = C_VECTOR;
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    case C_TVECTOR:
  /*---------------*/
        switch(class2)
        {
          case C_VECTOR:
          case C_MATRIX:
              res_class = C_MATRIX;
              break;
          case C_TVECTOR:
              res_class = NUM_CLASS( R_DESC((*arg1),type) );
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    case C_MATRIX:
  /*---------------*/
        switch(class2)
        {
          case C_MATRIX:
          case C_VECTOR:
              res_class = C_MATRIX;
              break;
          case C_TVECTOR:
              if (nrows1 == 1)
                res_class = NUM_CLASS( R_DESC((*arg1),type) );
              else
                res_class = C_TVECTOR;
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    default:
  /*---------------*/
        goto fail;
  }

  switch(R_DESC((*arg1),type))
  {
    case TY_INTEGER:
         switch(R_DESC((*arg2),type))
         {
           case TY_INTEGER:
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                ip2   = (int *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_INTEGER);
                  isum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    isum += ip1[k] * ip2[k];
                  L_SCALAR((*_desc),vali) = isum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_INTEGER);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP(nrows1 * ncols2,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  ipnew = (int *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      isum = ip1[0] * ip2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        isum += ip1[k] * ip2[pos];
                      }
                      ipnew[j] = isum;
                    }
                    ip1   += ncols1;
                    ipnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           case TY_REAL:
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += ip1[k] * rp2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = ip1[0] * rp2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += ip1[k] * rp2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    ip1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           default: goto fail;
         }
    case TY_REAL:
         switch(R_DESC((*arg2),type))
         {
           case TY_INTEGER:
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                ip2   = (int *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += rp1[k] * ip2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = rp1[0] * ip2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += rp1[k] * ip2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    rp1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           case TY_REAL:
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += rp1[k] * rp2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = rp1[0] * rp2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += rp1[k] * rp2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    rp1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           default: goto fail;
         }
    case TY_DIGIT:
         if (R_DESC((*arg2),type) != TY_DIGIT)
           goto fail;
         {
           register int pos1,pos2;

           _digit_recycling = FALSE;

           if (res_class == C_DIGIT)           /*     Resultat ist Skalar     */
           {
             digit_sum = _dig0;
             INC_REFCNT(_dig0);
             for (k=0; k < ncols1; ++k)
             {
               TEST(digit1 = digit_mul(R_MVT((*arg1),ptdv,class1)[k],R_MVT((*arg2),ptdv,class2)[k]));
               _digit_recycling = TRUE;
               TEST(digit2 = DI_ADD(digit_sum,digit1));
               _digit_recycling = FALSE;
               DEC_REFCNT(digit1);
               DEC_REFCNT(digit_sum);
               digit_sum = digit2;
             }
             _desc = digit_sum;
           }
           else                                /*    Resultat ist Struktur    */
           {
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,res_class,TY_DIGIT);
             L_MVT((*_desc),nrows,res_class) = nrows1;
             L_MVT((*_desc),ncols,res_class) = ncols2;
             NEWHEAP(nrows1 * ncols2,A_MVT((*_desc),ptdv,res_class));

             for (i = 0; i < nrows1; ++i)
               for (j = 0; j < ncols2; ++j)
               {
                 pos1 = i * ncols1;
                 pos2 = j;
                 TEST(digit_sum = digit_mul(R_MVT((*arg1),ptdv,class1)[pos1],R_MVT((*arg2),ptdv,class2)[pos2]));
                 for (k = 1; k < ncols1; ++k)
                 {
                   TEST(digit1 = digit_mul(R_MVT((*arg1),ptdv,class1)[++pos1],R_MVT((*arg2),ptdv,class2)[pos2+=ncols2]));
                   _digit_recycling = TRUE;
                   TEST(digit2 = DI_ADD(digit_sum,digit1));
                   _digit_recycling = FALSE;
                   DEC_REFCNT(digit1);
                   DEC_REFCNT(digit_sum);
                   digit_sum = digit2;
                 }
                 L_MVT((*_desc),ptdv,res_class)[i*ncols2 + j] = (int) digit_sum;
               }
           }
           _digit_recycling = TRUE;
         }
         goto success;
    default: goto fail;
  }

fail:
  END_MODUL("red_ip");
  return(0);

success:
  DEC_REFCNT(arg1);
  DEC_REFCNT(arg2);
  END_MODUL("red_ip");
  return(1);
}

/*---------------------------------------------------------------------------
 * red_c -- ist die Sammelfunktion fuer die Koordinatenfunktionen
 *          c_+,c_*,c_-,c_div,c_max,c_min.
 *
 * Anwendung:   ap c_? [coor,matrix]
 *
 * Funktion:
 *
 *    Dieses sind die dyadischen arithmetischen Funktionen, die entlang
 *    einer Koordinate einer Matrix ausgefuehrt werden. Ist 'coor' gleich
 *    Eins, so werden die Zeilen und bei 'coor' gleich Zwei die Spalten
 *    verknuepft. Das Ergebnis ist entweder ein Vektor oder
 *    ein transponierter Vektor.
 *    Waehrend mit Hilfe von c_+ Zeilen (bzw. Spalten) addiert werden, so
 *    werden durch c_- dieselben abwechselnd subtrahiert und addiert.
 *    Analoges gilt fuer c_* und c_/.
 *
 * Beispiele:
 *    ap c_+  [1, mat<1,2>,<3,4>.]  =>  vect<4,6>
 *    ap c_-  [2, mat<1,2>,<3,4>.]  =>  tvect<~1,~1>
 *    ap c_*  [1, mat<1>,<2>,<3>.]  =>  vect<6>
 *    ap c_/  [2, mat<1,2,3>.    ]  =>  tvect<1.5>
 *    ap c_+  [1, mat<7>.        ]  =>  vect<7>
 *
 *-------------------------------------------------------------------------
 */
red_c(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2,digit3;
  int nrows,ncols,dim,pos;
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
  int op;
  char class;                            /*    Klasse des Resultats    */

  START_MODUL("red_c");

  op   = READSTACK(S_e) & P_FUNCCOMPL;

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    switch(R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           if (R_DESC(DESC(arg1),type) == TY_INTEGER)
             coordinate = R_SCALAR(DESC(arg1),vali);
           else
             if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
               goto fail;
           break;
      case C_DIGIT:
           if (conv_digit_int(arg1,&coordinate) == 0)
             goto fail;
           break;
      default: goto fail;
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             if (op == C_DIV)
             {
               NEWDESC(_desc); TEST_DESC;
               MVT_DESC_MASK(_desc,1,class,TY_REAL);
               NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
               L_MVT((*_desc),ncols,class) = ncols;
               L_MVT((*_desc),nrows,class) = 1;

               RES_HEAP;
               rpnew = (double *)R_MVT((*_desc),ptdv,class);
               ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               /* 1.Zeile kopieren */
               for (i = 0; i < ncols; ++i)
                 rpnew[i] = (double) ip[i];

               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   rpnew[i] /= (double) ip[i];
                 if (++j < nrows)
                 {
                   ip +=ncols;
                   for (i = 0; i < ncols; ++i)
                     rpnew[i] *= (double) ip[i];
                 }
               }
               REL_HEAP;
               goto success;
             }
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

             switch(op)
             {
               case C_PLUS:
                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                       for (i = 0; i < ncols; ++i)
                      ipnew[i] += ip[i];
                    }
                    break;
               case C_MULT:
                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        ipnew[i] *= ip[i];
                    }
                    break;
               case C_MINUS:
                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        ipnew[i] -= ip[i];
                      if (++j < nrows)
                      {
                        ip +=ncols;
                        for (i = 0; i < ncols; ++i)
                          ipnew[i] += ip[i];
                      }
                    }
                    break;
               case C_MAX:
                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (ip[i] > ipnew[i])
                           ipnew[i] = ip[i];
                    }
                    break;
               case C_MIN:
                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (ip[i] < ipnew[i])
                           ipnew[i] = ip[i];
                    }
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

             switch(op)
             {
               case C_PLUS:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] += rp[i];
                    }
                    break;
               case C_MULT:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] *= rp[i];
                    }
                    break;
               case C_MINUS:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] -= rp[i];
                      if (++j < nrows)
                      {
                        rp +=ncols;
                        for (i = 0; i < ncols; ++i)
                          rpnew[i] += rp[i];
                      }
                    }
                    break;
               case C_DIV:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] /= rp[i];
                      if (++j < nrows)
                      {
                        rp +=ncols;
                        for (i = 0; i < ncols; ++i)
                          rpnew[i] *= rp[i];
                      }
                    }
                    break;
               case C_MAX:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (rp[i] > rpnew[i])
                           rpnew[i] = rp[i];
                    }
                    break;
               case C_MIN:
                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (rp[i] < rpnew[i])
                           rpnew[i] = rp[i];
                    }
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

             switch(op)
             {
               case C_PLUS:
                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                        TEST(digit3 = DI_ADD(digit1,digit2));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                      }
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MULT:
                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos]));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                      }
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MINUS:
                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {            /* eine Zeile abziehen */
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                        TEST(digit3 = DI_SUB(digit1,digit2));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                      }
                      if (++j < nrows)
                      {
                        pos += ncols;
                        for (i = 0; i < ncols; ++i)  /* Zeile addieren */
                        {
                          digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                          digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                          TEST(digit3 = DI_ADD(digit1,digit2));
                          DEC_REFCNT(digit1);
                          L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                        }
                      }
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_DIV:
                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;  /* da durch Null geteilt werden kann !*/
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i) /* mit einer Zeile multiplizieren */
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        if ((digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos])) == 0)
                        {
                          for (i= 0; i < ncols; ++i)
                            DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                          _digit_recycling = TRUE;
                          DEC_REFCNT(_desc);
                          goto fail;
                        }
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                      }
                      if ( ++j < nrows)
                      {
                        pos += ncols;
                        for (i = 0; i < ncols; ++i)  /* durch eine Zeile teilen */
                        {
                          digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                          if ((digit2 = digit_div(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos])) == 0)
                          {
                            for (i= 0; i < ncols; ++i)
                              DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                            _digit_recycling = TRUE;
                            DEC_REFCNT(_desc);
                            goto fail;
                          }
                          DEC_REFCNT(digit1);
                          L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                        }
                      }
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MAX:
                    REL_HEAP;
                    pos = 0;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (digit_lt((IPNEW[i]),IP2[i+pos]))
                        {
                          DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                          INC_REFCNT((PTR_DESCRIPTOR)(IP2[i+pos]));
                          (IPNEW[i]) = IP2[i+pos];
                        }
                    }
                    break;
                case C_MIN:
                    pos = 0;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (digit_lt(IP2[i+pos],(IPNEW[i])))
                        {
                          DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                          INC_REFCNT((PTR_DESCRIPTOR)(IP2[i+pos]));
                          (IPNEW[i]) = IP2[i+pos];
                        }
                    }
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             goto success;
        case TY_BOOL:
             if (op != C_MAX && op != C_MIN) goto fail;
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
                ipnew[i] = ip[i];

             if (op == C_MAX)
               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (ip[i] > ipnew[i])
                      ipnew[i] = ip[i];
               }
             else
               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (ip[i] < ipnew[i])
                     ipnew[i] = ip[i];
               }
             REL_HEAP;
             goto success;
        case TY_STRING:
             if (op != C_MAX && op != C_MIN) goto fail;
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               (IPNEW[i]) = IP2[i];
               INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
             }
             pos = 0;
             if (op == C_MAX)
               for (j = 1; j < nrows; ++j)
               {
                 pos += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (str_lt((IPNEW[i]),IP2[i+pos]))
                   {
                     DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                     INC_REFCNT((PTR_DESCRIPTOR)IP2[i+pos]);
                   (IPNEW[i]) = IP2[i+pos];
                   }
               }
              else
                for (j = 1; j < nrows; ++j)
                {
                  pos += ncols;
                  for (i = 0; i < ncols; ++i)
                    if (str_lt(IP2[i+pos],(IPNEW[i])))
                    {
                      DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                      INC_REFCNT((PTR_DESCRIPTOR)IP2[i+pos]);
                      (IPNEW[i]) = IP2[i+pos];
                    }
                }
               REL_HEAP;
               goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             if (op == C_DIV)
             {
               NEWDESC(_desc); TEST_DESC;
               MVT_DESC_MASK(_desc,1,class,TY_REAL);
               NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
               L_MVT((*_desc),ncols,class) = 1;
               L_MVT((*_desc),nrows,class) = nrows;

               RES_HEAP;
               rpnew = (double *)R_MVT((*_desc),ptdv,class);
               ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 valr = (double) ip[0];
                 for (j = 1; j < ncols; ++j)
                 {
                   valr /= (double) ip[j];
                   if (++j >= ncols) break;
                   valr *= (double) ip[j];
                 }
                 rpnew[i] = valr;
                 ip += ncols;
               }
               REL_HEAP;
               goto success;
             }
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             switch(op)
             {
               case C_PLUS:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        vali += ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }
                    break;
               case C_MULT:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        vali *= ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }
                    break;
               case C_MINUS:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        vali -= ip[j];
                        if (++j >= ncols) break;
                        vali += ip[j];
                      }
                      ipnew[i] = vali;
                      ip += ncols;
                    }
                    break;
               case C_MAX:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (vali < ip[j])
                          vali = ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }
                    break;
               case C_MIN:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (vali > ip[j])
                          vali = ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             switch(op)
             {
               case C_PLUS:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        valr += rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               case C_MULT:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        valr *= rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               case C_MINUS:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        valr -= rp[j];
                        if (++j >= ncols) break;
                        valr += rp[j];
                      }
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               case C_DIV:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        valr /= rp[j];
                        if (++j >= ncols) break;
                        valr *= rp[j];
                      }
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               case C_MAX:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        if (valr < rp[j])
                          valr = rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               case C_MIN:
                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        if (valr > rp[j])
                          valr = rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             switch(op)
             {
               case C_PLUS:
                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[j+pos];
                        TEST(digit3 = DI_ADD(digit1,digit2));
                        DEC_REFCNT(digit1);
                        digit1 = digit3;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MULT:
                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j]));
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MINUS:
                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j];
                        TEST(digit3 = DI_SUB(digit1,digit2));
                        DEC_REFCNT(digit1);
                        digit1 = digit3;
                        if (++j >= ncols) break;
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j];
                        TEST(digit3 = DI_ADD(digit1,digit2));
                        DEC_REFCNT(digit1);
                        digit1 = digit3;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_DIV:
                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        if ((digit2 = digit_div(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j])) == 0)
                        {
                          DEC_REFCNT(digit1);
                          for (--i; i>=0; --i)
                            DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                          _digit_recycling = TRUE;
                          DEC_REFCNT(_desc);
                          goto fail;
                        }
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                        if (++j >= ncols) break;
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[j+pos]));
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
                    break;
               case C_MAX:
                    RES_HEAP;
                    ipnew = (int *)R_MVT((*_desc),ptdv,class);
                    ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (digit_lt(digit1,ip[j]))
                          digit1 = (PTR_DESCRIPTOR) ip[j];
                      INC_REFCNT((PTR_DESCRIPTOR)digit1);
                      ipnew[i] = (int) digit1;
                      ip += ncols;
                    }
                    REL_HEAP;
                    break;
               case C_MIN:
                    RES_HEAP;
                    ipnew = (int *)R_MVT((*_desc),ptdv,class);
                    ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (digit_lt(ip[j],digit1))
                          digit1 = (PTR_DESCRIPTOR)ip[j];
                      INC_REFCNT((PTR_DESCRIPTOR)digit1);
                      ipnew[i] = (int) digit1;
                      ip += ncols;
                    }
                    REL_HEAP;
                    break;
               default:
                    post_mortem("red_c: Unknown operation");
             }
             goto success;
        case TY_BOOL:
             if (op != C_MAX && op != C_MIN) goto fail;
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             if (op == C_MAX)
               for (i = 0; i < nrows; ++i)
               {
                 vali = ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (vali < ip[j])
                     vali = ip[j];
                 ipnew[i] = vali;
                 ip += ncols;
               }
             else
               for (i = 0; i < nrows; ++i)
               {
                 vali = ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (vali > ip[j])
                     vali = ip[j];
                 ipnew[i] = vali;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        case TY_STRING:
             if (op != C_MAX && op != C_MIN) goto fail;
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             if (op == C_MAX)
               for (i = 0; i < nrows; ++i)
               {
                 digit1 = (PTR_DESCRIPTOR) ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (str_lt(digit1,ip[j]))
                     digit1 = (PTR_DESCRIPTOR) ip[j];
                 INC_REFCNT((PTR_DESCRIPTOR)digit1);
                 ipnew[i] = (int) digit1;
                 ip += ncols;
               }
             else
               for (i = 0; i < nrows; ++i)
               {
                 digit1 = (PTR_DESCRIPTOR) ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (str_lt(ip[j],digit1))
                     digit1 = (PTR_DESCRIPTOR)ip[j];
                 INC_REFCNT((PTR_DESCRIPTOR)digit1);
                 ipnew[i] = (int) digit1;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c");
  return(1);
}

/*---------------------------------------------------------------------------
 * red_vc -- ist die Sammelfunktion fuer die Koordinatenfunktionen
 *           vc_+,vc_*,vc_-,vc_div,vc_max,vc_min.
 *
 * Anwendung:   ap vc_? [vector]
 *              ap vc_? [tvector]
 *
 * Funktion:
 *
 *    Dieses sind die dyadischen arithmetischen Funktionen, die entlang
 *    eines Vektors oder transponierten Vektors ausgefuehrt werden.
 *    Das Ergebnis ist ein Skalar.
 *    Waehrend mit Hilfe von c_+ alle Komponenten addiert werden, so
 *    werden durch c_- dieselben abwechselnd subtrahiert und addiert.
 *    Analoges gilt fuer c_* und c_/.
 *
 * Beispiele:
 *    ap vc_+  [vect<1,2,3,4>]   =>  10
 *    ap vc_-  [tvect<1,2,3,4>]  =>  ~2
 *    ap vc_*  [vect<1,2,3>]     =>   6
 *    ap vc_/  [tvect<1,2,3>]    =>   1.5
 *    ap vc_+  [tvect<7>]        =>   7
 *
 *-------------------------------------------------------------------------
 */
 red_vc(arg)
register STACKELEM arg;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2,digit3;
  int nrows,ncols,dim,pos;
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
  int op, type1;
  char class;                                   /*      Klasse von arg      */

  START_MODUL("red_vc");

  op = READSTACK(S_e) & P_FUNCCOMPL;

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

         switch(op)
         {
           case VC_PLUS:
                for (i = 1; i < dim; ++i)
                  vali += ip[i];
                break;
           case VC_MULT:
                for (i = 1; i < dim; ++i)
                  vali *= ip[i];
                break;
           case VC_MINUS:
                for (i = 1; i < dim; ++i)
                {
                  vali -= ip[i];
                  if (++i >= dim) break;
                  vali += ip[i];
                }
                break;
           case VC_DIV:
                DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                valr = (double) vali;
                for (i = 1; i < dim; ++i)
                {
                  valr /= (double) ip[i];
                  if (++i >= dim) break;
                  valr *= (double) ip[i];
                }
                REL_HEAP;
                L_SCALAR((*_desc),valr) = valr;
                goto success;
           case VC_MAX:
                for (i = 1; i < dim; ++i)
                  if (vali < ip[i])
                    vali = ip[i];
                break;
           case VC_MIN:
                for (i = 1; i < dim; ++i)
                  if (vali > ip[i])
                    vali = ip[i];
                break;
           default:
                post_mortem("red_c: Unknown operation");
         }
         REL_HEAP;
         L_SCALAR((*_desc),vali) = vali;
         goto success;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

         switch(op)
         {
           case VC_PLUS:
                for (i = 1; i < dim; ++i)
                  valr += rp[i];
                break;
           case VC_MULT:
                for (i = 1; i < dim; ++i)
                  valr *= rp[i];
                break;
           case VC_MINUS:
                for (i = 1; i < dim; ++i)
                {
                  valr -= rp[i];
                  if (++i >= dim) break;
                  valr += rp[i];
                }
                break;
           case VC_DIV:
                for (i = 1; i < dim; ++i)
                {
                  valr /= rp[i];
                  if (++i >= dim) break;
                  valr *= rp[i];
                }
                break;
           case VC_MAX:
                for (i = 1; i < dim; ++i)
                  if (valr < rp[i])
                    valr = rp[i];
                break;
           case VC_MIN:
                for (i = 1; i < dim; ++i)
                  if (valr > rp[i])
                    valr = rp[i];
                break;
           default:
                post_mortem("red_c: Unknown operation");
         }
         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

         switch(op)
         {
           case VC_PLUS:
                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = DI_ADD(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
                break;
           case VC_MULT:
                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = digit_mul(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
                break;
           case VC_MINUS:
                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = DI_SUB(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                  if (++i >= dim) goto success;
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = DI_ADD(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
                break;
           case VC_DIV:
                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  if ((digit1 = digit_div(_desc,R_MVT(DESC(arg),ptdv,class)[i])) == 0)
                  {
                    _digit_recycling = TRUE;
                    DEC_REFCNT(_desc);
                    goto fail;
                  }
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                  if (++i >= dim) goto success;
                  TEST(digit1 = digit_mul(_desc,R_MVT(DESC(arg),ptdv,class)[i]));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
                break;
           case VC_MAX:
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (digit_lt(_desc,digit1))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                break;
           case VC_MIN:
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (digit_lt(digit1,_desc))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                break;
           default:
                post_mortem("red_c: Unknown operation");
         }
         goto success;
    case TY_BOOL:
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);
         vali = ip[0];

         switch (op)
         {
           case VC_MAX:
                for (i = 1; i < dim; ++i)
                  if (vali < ip[i])
                    vali = ip[i];
                break;
           case VC_MIN:
                for (i = 1; i < dim; ++i)
                  if (vali > ip[i])
                    vali = ip[i];
                break;
           default:
                REL_HEAP;
                goto fail;
         }
         REL_HEAP;
         _desc = (PTR_DESCRIPTOR) (vali ? SA_TRUE : SA_FALSE);
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

         switch(op)
         {
           case VC_MAX:
                RES_HEAP;
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (str_lt(_desc,digit1))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                REL_HEAP;
                goto success;
           case VC_MIN:
                RES_HEAP;
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (str_lt(digit1,_desc))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                REL_HEAP;
                goto success;
           default:
                DEC_REFCNT(_desc);
                goto fail;
         }
       goto success;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc");
  return(1);
}

/*****************************  end of file rvalfunc.c  **********************/
