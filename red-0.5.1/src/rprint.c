/* $Log: rprint.c,v $
 * Revision 1.2  1992/12/16  12:50:34  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */





/*****************************************************************************/
/*     MODUL zur Darstellung von Stackelementen in Strings und auf Files     */
/*-------------------------------------------------------------------------- */
/* rprint.c   --  external:  printstel - calls prstel                        */
/*                           s_prstel  - calls prstel                        */
/*                           prstel    - calls prstrstel                     */
/*            --  internal:  prstrstel - calls prnconstr, prnsingleatom      */
/*                           prnconstr                                       */
/*                           prnsingleatom                                   */
/*****************************************************************************/

#if DEBUG
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <string.h>

#define         CTRLSTR1        " %-10.10s"        /* fuer DEMO     */
#define         CTRLSTR2        " %-.4s(%4d)"      /* fuer DEMO     */
#define         CTRLSTR3        " %-.4s(%1d,%2d)"  /* fuer DEMO     */
#define         CTRLSTR4        " #%8.8x "         /* fuer DEMO     */

#define         PRNTEST(x,pname,vname ) ((x.element & pname) == vname )
#define UNDEF   0x00000c01
#define SHORT   1

typedef union
{
int   element;
struct

#if ODDSEX
{
  char  class;
  char  edit_info;
  short   value;
}
#else
{
  short   value;
  char  edit_info;
  char  class;
}
#endif

          feld;
}
I_STACKELEM;


static void prstrstel (); /* TB, 4.11.1992 */
static void prnsingleatom (); /* TB, 4.11.1992 */
void prnstring (); /* TB, 4.11.1992 */
static void prnconstr (); /* TB, 4.11.1992 */

/* ..... */

/*------------------------------------------------------------------------
 * Lokal verwendete globale Variablen:
 *------------------------------------------------------------------------
 */

static char line[133];    /* Die gewuenschte Stackelementbeschreibung */
static char hilf[133];    /* Ein Hilfsstring zum Aufbau von 'line'.   */

/*$P                  */
/*********************************************************************/
/*                                                                   */
/*  Die externen Funktionen:                                         */
/*                                                                   */
/*********************************************************************/

/*-----------------------------------------------------------------------
 * prstel -- eine kurze oder lange Beschreibung eines Stackelementes
 *           wird auf einen File geschrieben.
 * parameter: XFile - der Filepointer,
 *            Conv  - das Stackelement,
 *            Flag  - bei Flag gleich eins gibt es eine kurze Beschreibung,
 *                    sonst eine Lange
 * calls:     prstrstel.
 *-------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
prstel (XFile, Conv, Flag)
FILE * XFile;
int   Conv;
int   Flag;
{
  prstrstel (Conv, Flag);
  fprintf (XFile ,"%s", line);

  return; /* TB, 4.11.1992 */
}

/*-----------------------------------------------------------------------
 * s_prstel -- eine kurze oder lange Beschreibung eines Stackelementes
 *             wird in einen String geschrieben.
 * parameter: Xline - der Stringanfang,
 *            Conv  - das Stackelement,
 *            Flag  - bei Flag gleich eins gibt es eine kurze Beschreibung,
 *                    sonst eine Lange
 * calls:     prstrstel.
 *-------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
s_prstel (Xline, Conv, Flag)
char * Xline;
int   Conv;
int   Flag;
{
  prstrstel (Conv, Flag);
  sprintf (Xline, "%s", line);

  return; /* TB, 4.11.1992 */
}

/*-----------------------------------------------------------------------
 * printstel -- eine kurze oder lange Beschreibung eines Stackelementes
 *              wird auf 'stdout' geschrieben.
 * parameter: Conv  - das Stackelement,
 *            Flag  - bei Flag gleich eins gibt es eine kurze Beschreibung,
 *                    sonst eine Lange
 * calls:     prstrstel.
 *-------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
printstel (Conv, flag)
int   Conv;
int   flag;             /* nur name falls 1   */
{

  prstel (stdout, Conv, flag);
  fflush(stdout);         /*   damit der Puffer geleert wird */

  return; /* TB, 4.11.1992 */
}

/****************************************************************************/
/*                                                                          */
/*  Nur intern verwendete Hilffunktionen.                                   */
/*                                                                          */
/****************************************************************************/


/*---------------------------------------------------------------------------
 * prstrstel -- steuert die Stackelementidentifizierung. Die endgueltige
 *              Beschreibung des Stackelementes steht bei Ende des Moduls
 *              in 'line'.
 * parameter:  Conv - das Stackelement,
 *             flag - kurze oder lange Beschreibung ?
 * calls:      prnconstr, prnsingleatom.
 * comment:    prstrstel verwendet die Struktur I_STACKELEM, um so einfach
 *             an die Elementinformationen zu kommen.
 *---------------------------------------------------------------------------
 */
static void /* TB, 4.11.1992 */
     prstrstel (Conv, flag)
int   Conv;
int   flag;             /* nur name falls 1   */
{
  I_STACKELEM Top;

  hilf[0] = line[0] = '\0';

  Top.element = Conv ;    /* Konvertierung eines Stackelementes in eine Struktur */

/* 1. Fall: Das Stackelement UNDEF */
  if (Conv == UNDEF)
    if (flag != SHORT)
    {
      sprintf (hilf, "@0x%-8.8x                            U N D E F \n"
          ,Top.element);
      strcat (line, hilf);
      return;
    }
    else
    {
      sprintf (hilf, CTRLSTR1, "U N D E F");
      strcat (line, hilf);
      return;
    }

/* 2. Fall: Das Stackelement ist ein Pointer */
  if (PRNTEST (Top, P_POINTER, POINTER))
    if (flag != SHORT)
    {
      sprintf (hilf, "@0x%-8.8x                            Pointer   \n"
          ,Top.element);
      strcat (line, hilf);
      return;
    }
    else
    {
      sprintf (hilf, CTRLSTR4, (Top.element));
      strcat (line, hilf);
      return;
    }

/* 3. Fall: kein Pointer und nicht UNDEF */
  if (flag != SHORT)
  {     /* zusaetzlich wird das editfeld und das valuefeld mit ausgegeben */
    sprintf (hilf, " 0x%8.8x ", Top.element);
    strcat (line, hilf);
    sprintf (hilf, "Edit:%-5d Value:", Top.feld.edit_info);
    strcat (line, hilf);
    if (Top.feld.value >= 0)
    {
      sprintf (hilf, "+");
      strcat (line, hilf);
      sprintf (hilf, "%-8d", Top.feld.value);
      strcat (line, hilf);
    }
    else
    {
      sprintf (hilf, "-");
      strcat (line, hilf);
      sprintf (hilf, "%-8d", -Top.feld.value);
      strcat (line, hilf);
    }
  }

/* die grosse Fallunterscheidung: */
  if (PRNTEST (Top, P_INT, INT)) {
    prnsingleatom (Top, flag);
  }
  else {
    Top.element = Conv & (~F_EDIT) /* alles bis auf edit */;
    if (PRNTEST (Top, P_CON, CON))
      prnconstr (Top, flag);
    else
    if (PRNTEST (Top, P_MA, MA))
      prnstring (Top, flag);
    else
      prnsingleatom (Top, flag);
  }

  if (flag != SHORT)
  {
    sprintf (hilf, "%s", " \n");
    strcat (line, hilf);
  }

  return;
}

/*---------------------------------------------------------------------------
 * prnconstr -- bereitet einen Konstruktor auf.
 * parameter:  Top -  das Stackelement (breits als Struktur)
 *             flag - kurze oder lange Beschreibung ?
 *---------------------------------------------------------------------------
 */
static void /* TB, 4.11.1992 */
prnconstr (Top, flag)
I_STACKELEM Top;
int   flag;
{
  I_STACKELEM temp;

  temp = Top;    /* warum weiss keiner so genau, vielleicht fuer NC-Compiler */

  if (flag != SHORT)
  {
    if ((temp.element & P_VAR_CONSTR) == VAR_CONSTR)
    {
      sprintf (hilf, " var   ");
      strcat (line, hilf);
      if ((temp.element & P_CHAINED_CONSTR) == CHAINED_CONSTR)
      {
        sprintf (hilf, "chained ");
        strcat (line, hilf);
      }
      else
      {
        sprintf (hilf, "no-chain");
        strcat (line, hilf);
      }
    }
    else
    {
      sprintf (hilf, " fix   ");
      strcat (line, hilf);
      if ((temp.element & P_CHAINED_CONSTR) == GROUP1_FIXCON)
        sprintf (hilf, "group1  ");
      else
        sprintf (hilf, "group2  ");
      strcat (line, hilf);
    }
  }

  /* ein Var- oder Fixconstruktor ? */
  if ((temp.element & P_VAR_CONSTR) == VAR_CONSTR)
    if (T_PM_NCASE(temp.element)) {
        sprintf (hilf, CTRLSTR3, "Ncase  ", ARITY (temp.element),1);
        strcat (line, hilf);
        return;
    }
    else
    switch ((temp.element & F_CONSTR_NAME) >> O_CONSTR_NAME)
    {
      case _AP:
#if D_DIST
        if (T_DAP(temp.element))
          sprintf (hilf, CTRLSTR2, "DApply ", ARITY (temp.element));
        else
#endif
          sprintf (hilf, CTRLSTR2, "Apply  ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _SNAP:
        sprintf (hilf, CTRLSTR2, "Snap   ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _AP_TIC: /* not used in husch-version */
        sprintf (hilf, CTRLSTR2, "Ap_tic ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _SUB:
        sprintf (hilf, CTRLSTR2, "Sub    ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _SUBPRE:       /* not used in husch-version */
        sprintf (hilf, CTRLSTR2, "Subpre ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _SUBPOST:      /* not used in husch-version */
        sprintf (hilf, CTRLSTR2, "Subpost", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _LIST:
        sprintf (hilf, CTRLSTR2, "List   ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _MATRIX:
        sprintf (hilf, CTRLSTR2, "Matrix ", ARITY (temp.element));
        strcat (line, hilf);
        return;
/* H */
      case _PM_SWITCH:                                    /* pattern - match */
        sprintf (hilf, CTRLSTR2, "Case   ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _PM_AS:                                        /* pattern - match */
        sprintf (hilf, CTRLSTR3, "As     ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
   /* case _PM_NCASE :
        sprintf (hilf, CTRLSTR3, "Ncase  ", ARITY (temp.element),1);
        strcat (line, hilf);
        return; */
      case _PM_LIST:                                      /* pattern - match */
        sprintf (hilf, CTRLSTR3, "Plist  ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
/* CS */
      case _LREC:
        sprintf (hilf, CTRLSTR2, "Lrec   ", ARITY (temp.element));
        strcat (line, hilf);
        break;
      case _ZF_UH : /* zf-expression */
        sprintf (hilf, CTRLSTR2, "Zf     ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _ZFIN_UH : /* zf-in Konstruktor fuer ZF-Generatoren */
        sprintf (hilf, CTRLSTR2, "Zfin   ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _ZFINT_UH : /* interner Konstruktor in eapre fuer ZF-Filter */
        sprintf (hilf, CTRLSTR2, "ZFil   ", ARITY (temp.element));
        strcat (line,hilf);
        return;
      default:
        sprintf (hilf, CTRLSTR2, "var-con?", ARITY (temp.element));
        strcat (line, hilf);
        return;
    }
  else {                        /* fix constr         */
   if ((temp.element & P_UNCHAINED_CONSTR) == GROUP1_FIXCON)
    switch ((temp.element & F_CONSTR_NAME) >> O_CONSTR_NAME)
    {
      case _PRELIST:
        sprintf (hilf, CTRLSTR3, "Prelist", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _PROTECT:
        sprintf (hilf, CTRLSTR3, "Protect", ARITY (temp.element), 1);
        strcat (line, hilf);
        return;
      case _PLUS:
        sprintf (hilf, CTRLSTR3, "Plus   ", ARITY (temp.element), 1);
        strcat (line, hilf);
        return;
      case _MINUS:
        sprintf (hilf, CTRLSTR3, "Minus  ", ARITY (temp.element), 1);
        strcat (line, hilf);
        return;
      case _SET:          /* not used in husch-version */
        sprintf (hilf, CTRLSTR3, "Set    ", ARITY (temp.element), 1);
        strcat (line, hilf);
        return;
      case _REC:
        sprintf (hilf, CTRLSTR3, "Rec    ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _COND:
        sprintf (hilf, CTRLSTR3, "Cond   ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _EXP:                                      /* Exponent ==>  1e10 */
        sprintf (hilf, CTRLSTR3, "Expo   ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _VECTOR:
        sprintf (hilf, CTRLSTR3, "Vector ", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _TVECTOR:
        sprintf (hilf, CTRLSTR3, "Tvector", ARITY (temp.element), 2);
        strcat (line, hilf);
        return;
      case _PM_OTHERWISE:                                 /* pattern - match */
        sprintf (hilf, CTRLSTR3, "Oth    ", ARITY (temp.element), 1);
        strcat (line, hilf);
        return;
      case _PM_WHEN:                                      /* pattern - match */
        sprintf (hilf, CTRLSTR3, "When   ", ARITY (temp.element), 3);
        strcat (line, hilf);
        return;
      case _PM_MATCH:                                     /* pattern - match */
        sprintf (hilf, CTRLSTR3, "Match  ", ARITY (temp.element), 3);
        strcat (line, hilf);
        return;

      default:
        sprintf (hilf, CTRLSTR2, "fix-con?", ARITY (temp.element));
        strcat (line, hilf);
        return;
    }                           /* switch             */
   else
    switch ((temp.element & F_CONSTR_NAME) >> O_CONSTR_NAME) {
      case _STRING:
        sprintf (hilf, CTRLSTR2, "String ", ARITY (temp.element));
        strcat (line, hilf);
        return;
      case _PM_IN :                                       /* pattern - match */
        sprintf (hilf, CTRLSTR3, "In     ", ARITY (temp.element),2);
        strcat (line, hilf);
        return;
/* ak *//* UNPROTECT macht genau ein PROTECT ungueltig */
      case _UNPROTECT :                       
        sprintf (hilf, CTRLSTR3, "Unprot ", ARITY (temp.element),1);
        strcat (line, hilf);
        return;
      default:
        sprintf (hilf, CTRLSTR2, "constru?", ARITY (temp.element));
        strcat (line, hilf);
        return;
    } /** switch **/
  } /** else fix-constr **/

  return;
}

/*---------------------------------------------------------------------------
 * prnstring -- bereitet das Element eines Zahlen- oder Ziffern-Strings auf.
 * parameter:  Top -  das Stackelement (breits als Struktur)
 *             flag - kurze oder lange Beschreibung ?
 *---------------------------------------------------------------------------
 */
#include <ctype.h>

#define S_PRCHAR(str,ch,quot) \
           if (isprint(ch)) \
             sprintf (str, " %c%c        ", ch, quot); \
           else \
             sprintf (str, " " #ch "(%02x)%c   ", ch, quot)

void /* TB, 4.11.1992 */
prnstring (Top, flag)
I_STACKELEM Top;
int flag;
{
  char  ch;

  ch = (char) Top.feld.value;

  switch ((P_LET1 & Top.element)) {
    case LET0:
      if (flag != SHORT) {
        sprintf (hilf, " last letter   ");
        strcat (line, hilf);
      }
      S_PRCHAR(hilf,ch,'\'');
      strcat (line, hilf);
      break;
    case LET1:
      if (flag != SHORT) {
        sprintf (hilf, " letter        ");
        strcat (line, hilf);
      }
      S_PRCHAR(hilf,ch,' ');
      strcat (line, hilf);
      break;
    case STR_CHAR :
      if (flag != SHORT) {
        sprintf (hilf, " character     ");
        strcat (line, hilf);
      }
      S_PRCHAR(hilf,ch,' ');
      strcat (line, hilf);
      break;
    case DIG0:
      if (flag != SHORT) {
        sprintf (hilf, " last digit    ");
        strcat (line, hilf);
      }
      S_PRCHAR(hilf,ch,'\'');
      strcat (line, hilf);
      break;
    case DIG1:
      if (flag != SHORT) {
        sprintf (hilf, " digit         ");
        strcat (line, hilf);
      }
      S_PRCHAR(hilf,ch,' ');
      strcat (line, hilf);
      break;
    default:
        sprintf (hilf, CTRLSTR1, "str-elem ?");
        strcat (line, hilf);
  }

  return; /* TB, 4.11.1992 */
}

/*---------------------------------------------------------------------------
 * prnsingleatom -- bereitet ein single atom  auf.
 * parameter:  Top -  das Stackelement (breits als Struktur)
 *             flag - kurze oder lange Beschreibung ?
 *---------------------------------------------------------------------------
 */
static void /* TB, 4.11.1992 */
prnsingleatom (Top, flag)
I_STACKELEM Top;
int   flag;
{
  if (T_INT(Top.element)) {
    sprintf (hilf,"%+10d ",VAL_INT(Top.element));
    strcat (line, hilf);
    return;
  }
  else
  switch ((P_FUNC & Top.element))
  {
    case FUNC:                                             /* eine  Funktion */
    {
      if (flag != SHORT)
      {
        sprintf (hilf, " function      ");
        strcat (line, hilf);
      }
      switch (Top.element)
      {
        case M_NOT:
             sprintf (hilf, CTRLSTR1, "NOT       ");
             strcat (line, hilf);
             return;
        case M_ABS:
             sprintf (hilf, CTRLSTR1, "ABS       ");
             strcat (line, hilf);
             return;
        case M_NEG:
             sprintf (hilf, CTRLSTR1, "NEG       ");
             strcat (line, hilf);
             return;
        case M_TRUNCATE:
             sprintf (hilf, CTRLSTR1, "TRUNC     ");
             strcat (line, hilf);
             return;
        case M_FRAC:
             sprintf (hilf, CTRLSTR1, "FRAC      ");
             strcat (line, hilf);
             return;
        case M_FLOOR:
             sprintf (hilf, CTRLSTR1, "FLOOR     ");
             strcat (line, hilf);
             return;
        case M_CEIL:
             sprintf (hilf, CTRLSTR1, "CEIL      ");
             strcat (line, hilf);
             return;
        case M_EMPTY:
             sprintf (hilf, CTRLSTR1, "EMPTY     ");
             strcat (line, hilf);
             return;
        case M_SIN:
             sprintf (hilf, CTRLSTR1, "SIN       ");
             strcat (line, hilf);
             return;
        case M_COS:
             sprintf (hilf, CTRLSTR1, "COS       ");
             strcat (line, hilf);
             return;
        case M_TAN:
             sprintf (hilf, CTRLSTR1, "TAN       ");
             strcat (line, hilf);
             return;
        case M_LN:
             sprintf (hilf, CTRLSTR1, "LN        ");
             strcat (line, hilf);
             return;
        case M_EXP:
             sprintf (hilf, CTRLSTR1, "EXP       ");
             strcat (line, hilf);
             return;
        case D_PLUS:
             sprintf (hilf, CTRLSTR1, "PLUS      ");
             strcat (line, hilf);
             return;
        case D_MINUS:
             sprintf (hilf, CTRLSTR1, "MINUS     ");
             strcat (line, hilf);
             return;
        case D_MULT:
             sprintf (hilf, CTRLSTR1, "MULT      ");
             strcat (line, hilf);
             return;
        case D_DIV:
             sprintf (hilf, CTRLSTR1, "DIV       ");
             strcat (line, hilf);
             return;
        case D_OR:
             sprintf (hilf, CTRLSTR1, "OR        ");
             strcat (line, hilf);
             return;
        case D_AND:
             sprintf (hilf, CTRLSTR1, "AND       ");
             strcat (line, hilf);
             return;
        case D_XOR:
             sprintf (hilf, CTRLSTR1, "XOR       ");
             strcat (line, hilf);
             return;
        case D_EQ:
             sprintf (hilf, CTRLSTR1, "EQ        ");
             strcat (line, hilf);
             return;
        case D_NEQ:
             sprintf (hilf, CTRLSTR1, "NE        ");
             strcat (line, hilf);
             return;
        case D_LESS:
             sprintf (hilf, CTRLSTR1, "LT        ");
             strcat (line, hilf);
             return;
        case D_LEQ:
             sprintf (hilf, CTRLSTR1, "LE        ");
             strcat (line, hilf);
             return;
        case D_GREAT:
             sprintf (hilf, CTRLSTR1, "GT        ");
             strcat (line, hilf);
             return;
        case D_GEQ:
             sprintf (hilf, CTRLSTR1, "GE        ");
             strcat (line, hilf);
             return;
        case D_MIN:
             sprintf (hilf, CTRLSTR1, "MIN       ");
             strcat (line, hilf);
             return;
        case D_MAX:
             sprintf (hilf, CTRLSTR1, "MAX       ");
             strcat (line, hilf);
             return;
        case D_MOD:
             sprintf (hilf, CTRLSTR1, "MOD       ");
             strcat (line, hilf);
             return;
        case IP:                                     /* inner product       */
             sprintf (hilf, CTRLSTR1, "IP        ");
             strcat (line, hilf);
             return;
        case CLASS:
             sprintf (hilf, CTRLSTR1, "CLASS     ");
             strcat (line, hilf);
             return;
        case TYPE:
             sprintf (hilf, CTRLSTR1, "TYPE      ");
             strcat (line, hilf);
             return;
        case LDIMENSION:
             sprintf (hilf, CTRLSTR1, "LDIM      ");
             strcat (line, hilf);
             return;
        case DIMENSION:                               /* uni - function      */
             sprintf (hilf, CTRLSTR1, "DIM       ");
             strcat (line, hilf);
             return;
        case VDIMENSION:                              /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VDIM      ");
             strcat (line, hilf);
             return;
        case MDIMENSION:                              /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MDIM      ");
             strcat (line, hilf);
             return;
        case C_PLUS:
             sprintf (hilf, CTRLSTR1, "C_PLUS    ");
             strcat (line, hilf);
             return;
        case C_MINUS:
             sprintf (hilf, CTRLSTR1, "C_MINUS   ");
             strcat (line, hilf);
             return;
        case C_MULT:
             sprintf (hilf, CTRLSTR1, "C_MULT    ");
             strcat (line, hilf);
             return;
        case C_DIV:
             sprintf (hilf, CTRLSTR1, "C_DIV     ");
             strcat (line, hilf);
             return;
        case C_MIN:
             sprintf (hilf, CTRLSTR1, "C_MIN     ");
             strcat (line, hilf);
             return;
        case C_MAX:
             sprintf (hilf, CTRLSTR1, "C_MAX     ");
             strcat (line, hilf);
             return;
        case VC_PLUS:                                 /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_PLUS   ");
             strcat (line, hilf);
             return;
        case VC_MINUS:                                /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_MINUS  ");
             strcat (line, hilf);
             return;
        case VC_MULT:                                 /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_MULT   ");
             strcat (line, hilf);
             return;
        case VC_DIV:                                  /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_DIV    ");
             strcat (line, hilf);
             return;
        case VC_MIN:                                  /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_MIN    ");
             strcat (line, hilf);
             return;
        case VC_MAX:                                  /*  VECTOR / TVECTOR   */
             sprintf (hilf, CTRLSTR1, "VC_MAX    ");
             strcat (line, hilf);
             return;
        case F_EQ:
             sprintf (hilf, CTRLSTR1, "F_EQ      ");
             strcat (line, hilf);
             return;
        case F_NE:
             sprintf (hilf, CTRLSTR1, "F_NE     ");
             strcat (line, hilf);
             return;
        case LCUT:                                    /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LCUT      ");
             strcat (line, hilf);
             return;
        case LTRANSFORM:                              /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LTRANSFORM");
             strcat (line, hilf);
             return;
        case LROTATE:                                 /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LROTATE   ");
             strcat (line, hilf);
             return;
        case LSELECT:                                 /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LSELECT   ");
             strcat (line, hilf);
             return;
        case LREPLACE:                                /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LREPLACE  ");
             strcat (line, hilf);
             return;
        case REVERSE :                                /*  LIST               */
             sprintf (hilf, CTRLSTR1, "REVERSE   ");
             strcat (line, hilf);
             return;
        case LUNI:                                    /*  LIST               */
             sprintf (hilf, CTRLSTR1, "LUNITE  ");
             strcat (line, hilf);
             return;
        case SUBSTR:                                  /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "SUBSTR  ");    /*  list / string      */
             strcat (line, hilf);
             return;
        case LGEN_UH: /* list generation */
             sprintf (hilf, CTRLSTR1, "LGEN    ");
             strcat (line, hilf);
             return;
        case REPSTR:                                  /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "REPSTR  ");    /*  list / string      */
             strcat (line, hilf);
             return;
        case CUT:                                     /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "CUT       ");
             strcat (line, hilf);
             return;
        case ROTATE:                                  /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "ROTATE    ");
             strcat (line, hilf);
             return;
        case SELECT:                                  /*  uni - function   */
             sprintf (hilf, CTRLSTR1, "SELECT    ");
             strcat (line, hilf);
             return;
        case REPLACE:                                 /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "REPLACE   ");
             strcat (line, hilf);
             return;
        case UNI:                                     /*  uni - function     */
             sprintf (hilf, CTRLSTR1, "UNITE     ");
             strcat (line, hilf);
             return;
        case MCUT:                                    /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MCUT      ");
             strcat (line, hilf);
             return;
        case MROTATE:                                 /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MROTATE   ");
             strcat (line, hilf);
             return;
        case MSELECT:                                 /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MSELECT    ");
             strcat (line, hilf);
             return;
        case MREPLACE:                                /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MREPLACE   ");
             strcat (line, hilf);
             return;
        case MREPLACE_C:                              /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MREPLACE_C");
             strcat (line, hilf);
             return;
        case MREPLACE_R:                              /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MREPLACE_R");
             strcat (line, hilf);
             return;
        case MUNI:                                    /*  MATRIX             */
             sprintf (hilf, CTRLSTR1, "MUNITE    ");
             strcat (line, hilf);
             return;
        case VCUT:                                   /*  VECTOR / TVECTOR    */
             sprintf (hilf, CTRLSTR1, "VCUT      ");
             strcat (line, hilf);
             return;
        case VROTATE:                                /*  VECTOR / TVECTOR    */
             sprintf (hilf, CTRLSTR1, "VROTATE   ");
             strcat (line, hilf);
             return;
        case VSELECT:                                /*  VECTOR / TVECTOR    */
             sprintf (hilf, CTRLSTR1, "VSELECT   ");
             strcat (line, hilf);
             return;
        case VREPLACE:                               /*  VECTOR / TVECTOR    */
             sprintf (hilf, CTRLSTR1, "VREPLACE  ");
             strcat (line, hilf);
             return;
        case VUNI:                                   /*  VECTOR / TVECTOR    */
             sprintf (hilf, CTRLSTR1, "VUNITE  ");
             strcat (line, hilf);
             return;
        case TRANSPOSE:                       /* VECTOR , TVECTOR and MATRIX */
             sprintf (hilf, CTRLSTR1, "TRANSPOSE ");
             strcat (line, hilf);
             return;
        case TRANSFORM:                       /* VECTOR , TVECTOR and MATRIX */
             sprintf (hilf, CTRLSTR1, "TRANSFORM ");
             strcat (line, hilf);
             return;
        case TO_MAT:                                 /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_MAT    ");
             strcat (line, hilf);
             return;
        case TO_VECT:                                /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_VECT   ");
             strcat (line, hilf);
             return;
        case TO_TVECT:                               /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_TVECT  ");
             strcat (line, hilf);
             return;
        case TO_SCAL:                               /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_SCAL   ");
             strcat (line, hilf);
             return;
        case TO_LIST:                               /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_LIST   ");
             strcat (line, hilf);
             return;
        case TO_FIELD:                              /*  type conversion     */
             sprintf (hilf, CTRLSTR1, "TO_FIELD  ");
             strcat (line, hilf);
             return;
        case MK_STRING:                             /*  make hirarchical    */ 
             sprintf (hilf, CTRLSTR1, "MKSTRING  ");/*  string              */
             strcat (line, hilf);
             return;    
        default:
             sprintf (hilf, CTRLSTR1, "function ?");
             strcat (line, hilf);
             return;
      } /** end of switch Top.element **/
    }  /** end of case FUNC **/

    case CONSTANT:
    {
      if ((P_DOLLAR  & Top.element) == DOLLAR)
      {
        if (flag != SHORT)
        {
          sprintf (hilf, " $ Ink-Elemente");
          strcat (line, hilf);
        }
        sprintf (hilf, CTRLSTR2, " $     ", ARITY (Top.element));
        strcat (line, hilf);
        return;
      }

      if ((P_NUM & Top.element) == NUM)
      {
        if (flag != SHORT)
        {
          sprintf (hilf, " NUM-Variable   ");
          strcat (line, hilf);
        }
        sprintf (hilf, CTRLSTR2, "NUM     ", ARITY (Top.element));
        strcat (line, hilf);
        return;
      }

      if (flag != SHORT)
      {
        sprintf (hilf, " Constant      ");
        strcat (line, hilf);
      }

      /* die eigentliche Fallunterscheidung */
      switch (Top.element)
      {
          case ESET :     /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "empty set ");
               strcat (line, hilf);
               return;
          case DOL_CONST :
               sprintf (hilf, CTRLSTR1, "   $      ");
               strcat (line, hilf);
               return;
          case PLACEH:    /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "PLACEH    ");
               strcat (line, hilf);
               return;
          case HASH:      /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "   #      ");
               strcat (line, hilf);
               return;
          case KLAA:
               sprintf (hilf, CTRLSTR1, "   @      ");
               strcat (line, hilf);
               return;
          case NIL:                                  /*  empty list         */
               sprintf (hilf, CTRLSTR1, "NIL       ");
               strcat (line, hilf);
               return;
          case NILMAT:                               /*  empty matrix       */
               sprintf (hilf, CTRLSTR1, "NILMAT    ");
               strcat (line, hilf);
               return;
          case NILVECT:                              /*  empty vector       */
               sprintf (hilf, CTRLSTR1, "NILVECT   ");
               strcat (line, hilf);
               return;
          case NILTVECT:                             /*  empty tvector      */
               sprintf (hilf, CTRLSTR1, "NILTVECT  ");
               strcat (line, hilf);
               return;
          case NILSTRING:                            /*  empty string       */
               sprintf (hilf, CTRLSTR1, "NILSTRING ");
               strcat (line, hilf);
               return;
          case CL_FUNCTION:
               sprintf (hilf, CTRLSTR1, "CL_FUNCTION");
               strcat (line, hilf);
               return;
          case CL_SCALAR:
               sprintf (hilf, CTRLSTR1, "CL_SCALAR ");
               strcat (line, hilf);
               return;
          case CL_VECTOR:                            /*  class - Vector      */
               sprintf (hilf, CTRLSTR1, "CL_VECTOR ");
               strcat (line, hilf);
               return;
          case CL_TVECTOR:                           /*  class - TVector     */
               sprintf (hilf, CTRLSTR1, "CL_TVECTOR");
               strcat (line, hilf);
               return;
          case CL_MATRIX:                            /*  class - Matrix      */
               sprintf (hilf, CTRLSTR1, "CL_MATRIX ");
               strcat (line, hilf);
               return;
          case CL_SET:    /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "CL_SET    ");
               strcat (line, hilf);
               return;
          case CL_TREE:                              /*  class - List        */
               sprintf (hilf, CTRLSTR1, "CL_LIST   ");
               strcat (line, hilf);
               return;
          case USERFUNC:
               sprintf (hilf, CTRLSTR1, "USERFUNC  ");
               strcat (line, hilf);
               return;
          case PRIMFUNC:
               sprintf (hilf, CTRLSTR1, "PRIMFUNC  ");
               strcat (line, hilf);
               return;
          case LAMBDA :   /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "LAMBDA    ");
               strcat (line, hilf);
               return;
          case CONDITIONAL:        /* not used in husch-version */
               sprintf (hilf, CTRLSTR1, "CONDITIONAL");
               strcat (line, hilf);
               return;
          case BOOL:
               sprintf (hilf, CTRLSTR1, "BOOL      ");
               strcat (line, hilf);
               return;
          case DECIMAL:
               sprintf (hilf, CTRLSTR1, "NUMBER    ");
               strcat (line, hilf);
               return;
          case EXPR:
               sprintf (hilf, CTRLSTR1, "EXPR      ");
               strcat (line, hilf);
               return;
          case TMATRIX:                              /*  Type - Matrix       */
               sprintf (hilf, CTRLSTR1, "TMATRIX   ");
               strcat (line, hilf);
               return;
          case CHAR:
               sprintf (hilf, CTRLSTR1, "STRING    ");
               strcat (line, hilf);
               return;
          case SA_TRUE:
               sprintf (hilf, CTRLSTR1, "TRUE      ");
               strcat (line, hilf);
               return;
          case SA_FALSE:
               sprintf (hilf, CTRLSTR1, "FALSE     ");
               strcat (line, hilf);
               return;
          case PM_FAIL:                                   /* pattern - match */
               sprintf (hilf, CTRLSTR1, "FAIL      ");
               strcat (line, hilf);
               return;
          case PM_SUCC:                                   /* pattern - match */
               sprintf (hilf, CTRLSTR1, "OK        ");
               strcat (line, hilf);
               return;
          case PM_END :                                   /* pattern - match */
               sprintf (hilf, CTRLSTR1, "END       ");
               strcat (line, hilf);
               return;
          case PM_SKIP:                                   /* pattern - match */
               sprintf (hilf, CTRLSTR1, "SKIP      ");
               strcat (line, hilf);
               return;
          case PM_SKSKIP:                                 /* pattern - match */
               sprintf (hilf, CTRLSTR1, "SKIPSKIP      ");
               strcat (line, hilf);
               return;
          case PM_DOLLAR:                                 /* pattern - match */
               sprintf (hilf, CTRLSTR1, "DOLLAR      ");
               strcat (line, hilf);
               return;

          case PM_GGG:                                 /* pattern - match */
               sprintf (hilf, CTRLSTR1, "PM_GGG      ");
               strcat (line, hilf);
               return;

          case PM_GGG_N:                                 /* pattern - match */
               sprintf (hilf, CTRLSTR1, "PM_GGG_N      ");
               strcat (line, hilf);
               return;

          case PM_GGG_E:                                 /* pattern - match */
               sprintf (hilf, CTRLSTR1, "PM_GGG_E      ");
               strcat (line, hilf);
               return;
          case GNULL:                                     /* GRAPH */  
               sprintf (hilf, CTRLSTR1, "NULL        ");
               strcat (line, hilf);
               return;
          case TERMINATE:                                 /* GRAPH */  
               sprintf (hilf, CTRLSTR1, "TERMINATE   ");
               strcat (line, hilf);
               return;
          default:
               sprintf (hilf, CTRLSTR1, "constant?");
               strcat (line, hilf);
               return;
        }                     /* switch             */
      }                         /* CONSTANT           */
    default:
      sprintf (hilf, CTRLSTR1, "singleatom?");
      strcat (line,hilf);
      return;
  }
}
#else
#if !sun
/* static int breakdummy()
{}                           RS 02/11/92 */ 
#endif	/* !sun	*/
#endif
