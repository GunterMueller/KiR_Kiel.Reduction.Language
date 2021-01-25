/* $Log: rlstruct.c,v $
 * Revision 1.5  1996/03/29  16:00:12  stt
 * bad usage of macro T_POINTER fixed.
 *
 * Revision 1.4  1995/05/22  09:49:05  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:14  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*****************************************************************************/
/*                         MODUL FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rlstruct.c                                                                */
/*                                                                           */
/* special functions :                                                       */
/* -------------------                                                       */
/*    Functions which changing the structure of lists and strings            */
/*                                                                           */
/*    LIST and STRING functions :                                            */
/*                -- external:  red_lselect ,     red_substr , red_lrotate , */
/*                -- internal:  gen_id,  [called by functions which creating */
/*                              indirect list-descripotrs (pattern-match)]   */
/*                -- external:  red_lcut,        red_lunite  , red_lreverse  */
/*                              red_ltransform,  red_repstr  , red_lreplace  */
/*                                                                           */
/* called by : rear.c  and by universal functions in rmstruct.c              */
/*****************************************************************************/

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>

#include "dbug.h"



/* Include - File for  List- / Matrix- / Vector- / TVector-                  */
/* structuring functions : he contain extern variable , extern function      */
/* declarations and defines for the structuring functions.                   */
#include "rlmvt.h"
extern BOOLEAN _formated; /*uh*/  

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern void inc_idcnt(); /* TB, 4.11.1992 */       /* rheap.c */
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

PTR_DESCRIPTOR  gen_id();                       /* forward declaration      */
PTR_DESCRIPTOR  gen_ind();                      /* forward declaration      */

/* generation scavenging (tg) */
STACKELEM _scav1, _scav2, _scav3;               /* members of scav work set */
#if    SCAVENGE
#define SCAV_GET(x,y) (x)
#define SCAV_SET(x,y) x = (y)
#else
#define SCAV_GET(x,y) (y)
#define SCAV_SET(x,y)
#endif /* SCAVENGE       RS 1.12.1992 */

/*****************************************************************************/
/*                              red_lselect                                  */
/*---------------------------------------------------------------------------*/
/* function  : select one element of a list or a string.                     */
/*                                                                           */
/*             lselect : N  x  list       ==>  list                          */
/*                       N  x  substring  ==>  string                        */
/*                                                                           */
/* parameter : arg1  -  position of element to select                        */
/*             arg2  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_lselect(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("red_lselect");

  START_MODUL("red_lselect");
  if (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST) {
    int index;
    STRUCT_ARG(arg1,index);
    if (0 < index && index <= (int) R_LIST(DESC(arg2),dim)) {
                        /* int gecastet von RS 04/11/92 */ 
      /* index not out of range */
      _desc = (PTR_DESCRIPTOR)R_LIST(DESC(arg2),ptdv)[--index];
      if (T_STR_CHAR((STACKELEM)_desc)) {
        _desc = gen_ind(index,index,arg2);
      }
      else if (T_POINTER((STACKELEM)_desc)) INC_REFCNT(_desc);
      if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      END_MODUL("red_lselect");
      DBUG_RETURN(2);
    }
  }
fail:
  END_MODUL("red_lselect");
  DBUG_RETURN(0);
} /**** end of function red_lselect ****/


/*****************************************************************************/
/*                              red_substr                                   */
/*---------------------------------------------------------------------------*/
/* function  : get a substructure of a list or a string.                     */
/*                                                                           */
/*             substr : N  x  N  x  list    ==>  list                        */
/*                      N  x  N  x  string  ==>  string                      */
/*                                                                           */
/* parameter : arg1  -  first position of substructure                       */
/*             arg2  -  length of substructure                               */
/*             arg3  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_substr(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int first_index,                        /* first position of substructure  */
      last_index,                         /* length of substructure          */
      dim      ;                          /* dimension of the list           */

  DBUG_ENTER ("red_substr");

  START_MODUL("red_substr");

  if (T_POINTER(arg3) && R_DESC(DESC(arg3),class) == C_LIST) {
    STRUCT_ARG(arg1,first_index);
    STRUCT_ARG(arg2,last_index);
    dim = R_LIST(DESC(arg3),dim);
    if (0 < first_index && first_index <= dim) {
      last_index += (first_index - 1);        /* last position of substruct */
      if (first_index <= last_index && last_index <= dim) {
        _desc = gen_id(first_index,last_index,arg3);
        TEST_DESC;
      }
      else
      if (last_index == (first_index - 1)) {  /* 2. argument is null     */
        _desc = (R_DESC(DESC(arg3),type) == TY_STRING) ? _nilstring : _nil;
        INC_REFCNT(_desc);
      }
      else goto fail;
      /* success: */
      if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      if (T_POINTER(arg2)) DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      DEC_REFCNT((PTR_DESCRIPTOR)arg3);
      END_MODUL("red_substr");
      DBUG_RETURN(1);
    }
  }
fail:
  END_MODUL("red_substr");
  DBUG_RETURN(0);
} /**** end of function red_substr ****/

/*****************************************************************************/
/*                              red_lrotate                                  */
/*---------------------------------------------------------------------------*/
/* function  :                                                               */
/*  Die Funktion lrotate erzeugt eine zyklisch rotierte  Kopie  von 'list',  */
/*  bzw. 'string' . Der Parameter 'index' bestimmt, das  wievielte  Element  */
/*  nach dem ersten das neue erste Element werden soll. Bei einem negativen  */
/*  Parameter wird die Kopie entgegengesetzt rotiert.                        */
/*                                                                           */
/*             lrotate : index  x  list    ==>  list                         */
/*                       index  x  string  ==>  string                       */
/*                                                                           */
/* parameter : arg1  -  index                                                */
/*             arg2  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_lrotate(arg1,arg2)
STACKELEM arg1,arg2;
{
  register int dim,count;
  int pos, rotation;
  int *ipnew,*ip,element;

  DBUG_ENTER ("red_lrotate");

  START_MODUL("red_lrotate");

  if (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST) {
    dim = R_LIST(DESC(arg2),dim);
    if (dim > 0) {
      STRUCT_ARG(arg1,rotation);
      rotation = - rotation;  /* wurde nachtraeglich noetig */
      if ((rotation = rotation % dim) == 0) goto nothingtodo;
      SCAV_SET(_scav1,arg2);
      NEWDESC(_desc); TEST_DESC;
      SCAV_SET(arg2,_scav1);
      LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg2),type));
      L_LIST((*_desc),dim) = dim;
      /* get edit field */
      if (R_LIST(DESC(arg2),ptdd) != NULL)
        L_LIST(*_desc,special) = R_LIST(*R_LIST(DESC(arg2),ptdd),special);
      else L_LIST(*_desc,special) = R_LIST(DESC(arg2),special);
      NEWHEAP(dim,A_LIST(*_desc,ptdv));
      SCAV_SET(arg2,_scav1);
      RES_HEAP;
      ipnew = (int *) R_LIST(*_desc,ptdv);
      ip    = (int *) R_LIST(DESC(arg2),ptdv);
      if (rotation > 0)
         pos = rotation;
      else
         pos = dim + rotation;
      for (count = 0; count < dim; count++) {
        ipnew[pos] = element = ip[count];
        if T_POINTER(element)
          INC_REFCNT((PTR_DESCRIPTOR)element);
         ++pos;
         if (pos == dim) pos = 0;
      }
      REL_HEAP;
      /*  success: */
      if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      SCAV_SET(_scav1,0);
      END_MODUL("red_lrotate");
      DBUG_RETURN(1);
    }
    /* else nil | nilstring */
  nothingtodo:
    if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_lrotate");
    DBUG_RETURN(1);
  }
fail:
  END_MODUL("red_lrotate");
  DBUG_RETURN(0);
} /**** end of function red_lrotate ****/

/******************************************************************************/
/*                                gen_id                                      */
/*----------------------------------------------------------------------------*/
/* function  : generates new indirect list descriptor                         */
/*             returns a PTR_DESCRIPTOR of new indirect descriptor            */
/*             returns with NULL in case of trouble                           */
/*                                                                            */
/*             Note: SPECIAL-component contains OFFSET.                       */
/*                                                                            */
/* parameter : first_index  - 1.element of new list                           */
/*             last-index   - last element of new list                        */
/*             dir_desc     - pointer of direct descriptor                    */
/******************************************************************************/
PTR_DESCRIPTOR  gen_id(first_index,last_index,dir_desc)
 COUNT    first_index,                 /* 1. elem. of new list   */
          last_index;                  /* last elem. of new list */
 PTR_DESCRIPTOR dir_desc;              /* direct descriptor      */
{
   PTR_DESCRIPTOR ind_desc;            /* indirect descriptor    */

   DBUG_ENTER ("gen_id");

   START_MODUL("gen_id");

   SCAV_SET(_scav3,(STACKELEM)dir_desc);
   NEWDESC(ind_desc);

   if (ind_desc == NULL) {             /* case of trouble */
     SCAV_SET(_scav3,0);
     END_MODUL("gen_id");
     DBUG_RETURN((PTR_DESCRIPTOR)NULL);
   }
   SCAV_SET(dir_desc,(PTR_DESCRIPTOR)_scav3);

   INC_IDCNT;                          /* increment indirect-descriptor-counter */

   LI_DESC_MASK(ind_desc,1,C_LIST,R_DESC(DESC(dir_desc),type));
   L_LIST((*ind_desc),dim) = last_index - first_index + 1;

   if ((R_LIST((*dir_desc),ptdd)) == (T_PTD)0)
   {
       L_LIST((*ind_desc),special) = first_index - 1;
       L_LIST((*ind_desc),ptdd) = (T_PTD)dir_desc;
       L_LIST((*ind_desc),ptdv) = R_LIST((*dir_desc),ptdv) +
                                  R_LIST((*ind_desc),special);
   }
   else
       /* dir_desc in fact indirect */
   {
       L_LIST((*ind_desc),special) = R_LIST((*dir_desc),special) +
                                     first_index - 1;
       L_LIST((*ind_desc),ptdd) = (T_PTD)R_LIST((*dir_desc),ptdd);
       L_LIST((*ind_desc),ptdv) = R_LIST((*dir_desc),ptdv) +
                                  first_index - 1;
   }
   INC_REFCNT((PTR_DESCRIPTOR)(R_LIST((*ind_desc),ptdd)));
                            /* incr. refcount of descriptor, pointed at */
   SCAV_SET(_scav3,0);
   END_MODUL("gen_id");
   DBUG_RETURN(ind_desc);
}  /**** end of function gen_id  ****/

/***********************************************************************/

PTR_DESCRIPTOR gen_ind(first_index,last_index,dir_desc)
COUNT          first_index;      /* first elem. : 0 .. dim-1           */
COUNT          last_index;       /* last elem.  : first_index .. dim-1 */
PTR_DESCRIPTOR dir_desc;         /* direct descriptor      */
{
  register PTR_DESCRIPTOR ind_desc;       /* indirect descriptor    */

  DBUG_ENTER ("gen_ind");

  START_MODUL("gen_ind");
  SCAV_SET(_scav3,(STACKELEM)dir_desc);
  NEWDESC(ind_desc);
  SCAV_SET(dir_desc,(PTR_DESCRIPTOR)_scav3);
  TESTDESC(ind_desc);      /* case of trouble */
  INC_IDCNT;               /* increment indirect-descriptor-counter */
  LI_DESC_MASK(ind_desc,1,C_LIST,R_DESC(DESC(dir_desc),type));
  L_LIST(*ind_desc,dim) = last_index - first_index + 1;
  L_LIST(*ind_desc,ptdv) = R_LIST(*dir_desc,ptdv) + first_index;
  if (R_LIST(*dir_desc,ptdd) == (T_PTD)0) {
    L_LIST(*ind_desc,special) = first_index;
    L_LIST(*ind_desc,ptdd) = (T_PTD)dir_desc;
  }
  else {
    /* dir_desc in fact indirect */
    L_LIST(*ind_desc,special) = R_LIST(*dir_desc,special) + first_index;
    L_LIST(*ind_desc,ptdd) = (T_PTD)R_LIST(*dir_desc,ptdd);
  }
  INC_REFCNT((PTR_DESCRIPTOR)R_LIST(*ind_desc,ptdd));
  SCAV_SET(_scav3,0);
  END_MODUL("gen_ind");
  DBUG_RETURN(ind_desc);
}  /**** end of function gen_ind  ****/


/******************************************************************************/
/*                                red_lcut                                    */
/*----------------------------------------------------------------------------*/
/*  Funktion :                                                                */
/*    Die Funktion lcut entfernt Elemente aus einer Liste bzw. Strings        */
/*    (siehe Beispiele). Das Vorzeichen des Parameters (arg1)                 */
/*    entscheidet darueber, ob ein 'cutfirst' (positives Vorzeichen)          */
/*    oder ein 'cutlast' durchgefuehrt wird.                                  */
/*    Der Absolutwert dieses Parameters gibt an, wieviele Elemente der        */
/*    Liste abgeschnitten werden sollen.                                      */
/*    Es duerfen nicht mehr Elemente abgeschnitten werden als vorhanden       */
/*    sind. Werden alle Elemente abgeschitten, so entsteht eine leere         */
/*    Liste.                                                                  */
/*    Die Funktion gen_id generiert einen neuen indirekten Listen-            */
/*    Deskriptor. Der PTDV des indirekten Deskriptors zeigt in den Daten-     */
/*    vektor des Heapspeicherplatzes des zugehoerigen direkten Deskriptor.    */
/*    Der PTDD des direkten Listen-Deskriptors wird auf 0 gesetzt.            */
/*    Der PTDD des indirekten Deskriptors zeigt auf den direkten              */
/*    Deskriptor. Der indirekte Deskriptor hat keine Rueckwaerts-             */
/*    refferenz von dem Heapspeicherplatz zum indirekten Deskriptor.          */
/*    Ein 'cutfirst' entfernt die ersten, ein 'cutlast' entfernt              */
/*    die letzten Elemente der Liste.                                         */
/*                                                                            */
/*               analoges gilt fuer strings                                   */
/*                                                                            */
/*               cutfirst  <==>  lcut( 2, <1,2,3> )  ==> <3>                  */
/*                               lcut(  4 , 'ich bin ich` )  =>  'bin ich`    */
/*               cutlast   <==>  lcut(~1, <1,2,3> )  ==> <1,2>                */
/*                               lcut(~3, <1,2,3> )  ==> nil                  */
/*                               lcut( 0, <1,2,3> )  ==> <1,2,3>              */
/*                               lcut( ~4 , 'ich bin ich` )  =>  'ich bin`    */
/*                                                                            */
/*  Parameter :  arg1 - (Strukturieungsparameter)                             */
/*               arg2 - (Liste)                                               */
/******************************************************************************/
int red_lcut(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("red_lcut");

  START_MODUL("red_lcut");
  if (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST) {
    register int dim;
    int cut;
    dim = R_LIST(DESC(arg2),dim);
    STRUCT_ARG(arg1,cut);
    if ((-dim < cut) && (cut < dim)) {
      /* cut > 0 : cutfirst ; cut < 0 : cutlast */
      _desc = (cut >= 0) ? gen_ind(cut,dim-1,(PTR_DESCRIPTOR)arg2) : gen_ind(0,dim+cut-1,(PTR_DESCRIPTOR)arg2);  /* casts von RS 1.12.1992 */
    }
    else
    if (cut == dim || cut == -dim) {
      _desc = (R_DESC(DESC(arg2),type) == TY_STRING) ? _nilstring : _nil;
      INC_REFCNT(_desc);
    }
    else goto fail;
    /* success: */
    if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_lcut");
    DBUG_RETURN(1);
  }
fail:
  END_MODUL("red_lcut");
  DBUG_RETURN(0);
}  /**** end of function red_lcut ****/


/******************************************************************************/
/*                               red_lunite                                   */
/*----------------------------------------------------------------------------*/
/* Funktion:                                                                  */
/*    lunite kann nur Listen oder nur Strings vereinigen.                     */
/*                                                                            */
/* Beispiele:                                                                 */
/*    lunite( <> , <>  )  =>  <>       lunite( '`    , '`    )  => '`         */
/*    lunite( <> , <5> )  =>  <5>      lunite( '`    , 'B`   )  => 'B`        */
/*    lunite( <5>, <>  )  =>  <5>      lunite( 'str` , 'ing` ) => 'string`    */
/*    lunite( <7>, <9> )  =>  <7,9>                                           */
/*                                                                            */
/******************************************************************************/
int red_lunite(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int    i, element,
       *ip, *ipnew,
       dim,               /* Dimension von arg1 */
      dim2;               /* Dimension von arg2 */

  DBUG_ENTER ("red_lunite");

  START_MODUL("red_lunite");

  if ((T_POINTER(arg1) && (R_DESC(DESC(arg1),class) == C_LIST)) &&
      (T_POINTER(arg2) && (R_DESC(DESC(arg2),class) == C_LIST)))
  {
    if (R_DESC(DESC(arg1),type) != R_DESC(DESC(arg2),type))
      goto fail;                   /* Typ von arg1 ist ungleich Typ von arg2 */

    dim  = R_LIST(DESC(arg1),dim);
    dim2 = R_LIST(DESC(arg2),dim);

    if ((dim == 0) && (dim2 == 0)) {  /* arg1 und arg2               */
      _desc = (PTR_DESCRIPTOR)arg2;   /* sind nil's bzw. nilstring's */
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      goto success;
    }

    dim += dim2;                           /* Dimension of new descriptor    */
 
    SCAV_SET(_scav1,arg1);
    SCAV_SET(_scav2,arg2);
    NEWDESC(_desc); TEST_DESC;
    SCAV_SET(arg1,_scav1);
    SCAV_SET(arg2,_scav2);
    LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg2),type));
    L_LIST((*_desc),dim) = dim;
    if (R_LIST(DESC(arg1),ptdd) != NULL)                                     /* Edit-Feld */
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg1),ptdd)),special); /* Edit-Feld */
    else L_LIST((*_desc),special) = R_LIST(DESC(arg1),special);              /* Edit-Feld */
    NEWHEAP(dim,A_LIST((*_desc),ptdv));
    SCAV_SET(arg1,_scav1);
    SCAV_SET(arg2,_scav2);
    RES_HEAP;
    ipnew = (int *) R_LIST((*_desc),ptdv);
    ip =  (int *) R_LIST(DESC(arg1),ptdv);
    dim = R_LIST(DESC(arg1),dim);
    for (i = 0; i < dim; ++i)
    {
      ipnew[i] = element = ip[i];
      if T_POINTER(element)
        INC_REFCNT((PTR_DESCRIPTOR)element);
    }

    ipnew += dim;
    ip = (int *) R_LIST(DESC(arg2),ptdv);
    dim = R_LIST(DESC(arg2),dim);
    for (i = 0; i < dim; ++i)
    {
      ipnew[i] = element = ip[i];
      if T_POINTER(element)
        INC_REFCNT((PTR_DESCRIPTOR)element);
    }
    REL_HEAP;
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }  /* end of  arg1 = list  and  arg2 = list */
  else goto fail;

success:
  SCAV_SET(_scav1,0);
  SCAV_SET(_scav2,0);
  END_MODUL("red_lunite");
  DBUG_RETURN(1);
fail:
  SCAV_SET(_scav1,0);
  SCAV_SET(_scav2,0);
  END_MODUL("red_lunite");
  DBUG_RETURN(0);
}  /*** end of function red_lunite ***/

/*****************************************************************************/
/*                              red_reverse                                  */
/*---------------------------------------------------------------------------*/
/* function  :                                                               */
/*  Die Funktion lrotate erzeugt eine zyklisch rotierte  Kopie  von 'list',  */
/*  bzw. 'string' . Der Parameter 'index' bestimmt, das  wievielte  Element  */
/*  nach dem ersten das neue erste Element werden soll. Bei einem negativen  */
/*  Parameter wird die Kopie entgegengesetzt rotiert.                        */
/*                                                                           */
/*             reverse :  list   ==>  list                                   */
/*                        string ==>  string                                 */
/*                                                                           */
/* parameter : arg1  -  list        /   string                               */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_reverse(arg1)
register STACKELEM arg1;
{
  register int i,j,dim;
  int *ip,*ipnew;

  DBUG_ENTER ("red_reverse");

  START_MODUL("red_reverse");

  if (T_POINTER(arg1) && (R_DESC(DESC(arg1),class) == C_LIST))
  {
    if (R_LIST(DESC(arg1),dim) == 0) {
      NILSTRING_NIL(arg1);
    }

    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg1),type));
    L_LIST((*_desc),dim) = dim = R_LIST(DESC(arg1),dim);
    if (R_LIST(DESC(arg1),ptdd) != NULL)                                     /* Edit-Feld */
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg1),ptdd)),special); /* Edit-Feld */
    else L_LIST((*_desc),special) = R_LIST(DESC(arg1),special);              /* Edit-Feld */
    NEWHEAP(dim,A_LIST((*_desc),ptdv));
    RES_HEAP;
    ipnew = (int *) R_LIST((*_desc),ptdv);
    ip = (int *) R_LIST(DESC(arg1),ptdv);

    dim--;
    for (i = 0; i <= dim; i++)
    {
      if T_POINTER((j = ip[dim - i]))
        INC_REFCNT((PTR_DESCRIPTOR) j);
      ipnew[i] = j;
    }
    REL_HEAP;
    goto success;
  } /* Ende von T_POINTER */

/*fail:                             RS 30/10/92 */
  END_MODUL("red_reverse");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_reverse");
  DBUG_RETURN(1);
} /**** end of red_reverse ****/

/*****************************************************************************/
/*                             red_ltransform                                */
/* ------------------------------------------------------------------------- */
/* Funktion:                                                                 */
/*  Mit Hilfe von ltransform kann aus einer Liste eine Matrix erzeugt werden.*/
/*  'rowpar' gibt die Anzahl der Zeilen und 'columnpar' gibt die  Anzahl  der*/
/*  Spalten der zu erzeugenden Matrix an. Die Anzahl der  Elemente der  Liste*/
/*  muss  mit der Angabe von  ('rowpar' * 'columpar')  uebereinstimmen.  Alle*/
/*  Elemente der Liste muessen  vom  gleichen Typ sein. Wenn  ein Vektor oder*/
/*  ein  TVektor  als  Ergebnis  der  Funktion  gewuenscht  wird,  muss  eine*/
/*  (1xm)-mat bzw. eine (nx1)-mat erzeugt werden und diese mit Hilfe der Kon-*/
/*  versions-Funktionen to_vect bzw. to_tvect in einen  Vektor  bzw.  TVektor*/
/*  konvertiert werden.                                                      */
/*                                                                           */
/* Beispiele:  ltransform(0,0,     <>     )  =>  nilmat                      */
/*             ltransform(2,2, <1,2,3,4>  )  =>  mat<1,2>,                   */
/*                                                  <3,4>.                   */
/* Parameter:  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - list                                                   */
/*****************************************************************************/
int red_ltransform(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int nrows,ncols,i,dim,type,class,element;
  int *ip,*ipnew;
  double *rpnew;

  DBUG_ENTER ("red_ltransform");

  START_MODUL("red_ltransform");

  if (T_POINTER(arg3) && R_DESC(DESC(arg3),class) == C_LIST &&
      R_DESC(DESC(arg3),type) == TY_UNDEF) {
    STRUCT_ARG(arg1,nrows);
    STRUCT_ARG(arg2,ncols);
    /* Dimensionsueberpruefung : */
    if ((dim = R_LIST(DESC(arg3),dim)) != (nrows * ncols)) goto fail;
    if ((nrows == 0) && (ncols == 0)) {
      if (dim == 0) {
        _desc = _nilmat;
        INC_REFCNT(_nilmat);
        goto success;
      }
      else  goto fail;
    }
    if ((nrows <= 0) || (ncols <= 0))
      goto fail;
    /* Typ- und Klassenueberpruefung: */
    ip = (int *) R_LIST(DESC(arg3),ptdv);
    if T_POINTER((element = ip[0])) {
      switch(class = R_DESC(DESC(element),class)) {
         case C_MATRIX : type = TY_MATRIX;
                         break;
         case C_LIST   : type = TY_STRING;
                         if (R_DESC(DESC(element),type) == TY_UNDEF)
                           goto fail;
                         break;
         case C_DIGIT  :
         case C_SCALAR : type =  R_DESC(DESC(element),type);
                         break;
         default       : goto fail;
      }
      for (i = 1; i < dim; i++) {
        element = ip[i];
        if (T_POINTER(element) && R_DESC(DESC(element),class) == class) {
          if (type != R_DESC(DESC(element),type))
            if (class == C_SCALAR)
              type = TY_REAL;  /* es wird eine Matrix von Reals erstellt */
            else
              if (class != C_MATRIX) goto fail;
        }
        else
        if (T_INT(element)) {
          if (class != C_SCALAR) goto fail;
        }
        else goto fail;
      }
    }
    else {
      if (T_INT(element)) {
        type = TY_INTEGER;
        for (i = 1; i < dim; i++)
          if (!T_INT(ip[i]))
            if (T_POINTER(ip[i]) && R_DESC(DESC(ip[i]),class) == C_SCALAR)
              type = TY_REAL;
            else goto fail;
      }
      else
      if (T_BOOLEAN(element)) {
        type = TY_BOOL;
        for (i = 1; i < dim; i++)
          if (!T_BOOLEAN(ip[i]))
            goto fail;
      }
      else goto fail;
    }
    /* Aufbau einer Matrix */
    switch(type) {
      case TY_INTEGER:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; ++i)
             ipnew[i] = VAL_INT(ip[i]);
           REL_HEAP;
           goto success;
      case TY_REAL:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; ++i) {
             element = ip[i];
             if (T_INT(element))
               rpnew[i] = VAL_INT(element);
             else
               rpnew[i] = R_SCALAR(DESC(element),valr);
           }
           REL_HEAP;
           goto success;
      case TY_DIGIT:
      case TY_STRING:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,type);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; ++i) {
             ipnew[i] = element = ip[i];
             INC_REFCNT((PTR_DESCRIPTOR)element);
           }
           REL_HEAP;
           goto success;
      case TY_BOOL:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_BOOL);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; ++i)
             ipnew[i] = (T_SA_TRUE(ip[i]) ? 1 : 0);
           REL_HEAP;
           goto success;
      case TY_MATRIX:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_MATRIX);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; ++i) {
             INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
             ipnew[i] = ip[i];
           }
           REL_HEAP;
           goto success;
      default:
           goto fail;
    }
success:
    if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    if (T_POINTER(arg2)) DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    DEC_REFCNT((PTR_DESCRIPTOR)arg3);
    END_MODUL("red_ltransform");
    DBUG_RETURN(1);
  } /** if T_POINTER arg3 **/
fail:
  END_MODUL("red_ltransform");
  DBUG_RETURN(0);
}  /**** end of function ltransform ****/

/*****************************************************************************/
/*                     red_repstr  (replace-sub-structure)                   */
/*---------------------------------------------------------------------------*/
/* function :  replace a substructure of a string or a list at position      */
/*             'position' and length 'length'                                */
/*     repstr( position  x  length  x  List1    x  List1   )  ==> List       */
/*     repstr( position  x  length  x  String1  x  String2 )  ==> String     */
/*                                                                           */
/* parameter : arg1  -  index / position                                     */
/*             arg2  -  length                                               */
/*             arg3  -  List1       /   String1                              */
/*             arg4  -  List2       /   String2                              */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_repstr(arg1,arg2,arg3,arg4)
STACKELEM arg1,arg2,arg3,arg4;
{
  register int i,j,
               dim3,  /* dimension of arg3 */
               dim4;  /* dimension of arg4 */
  int position,       /* first position for replacing in structure */
      length,         /* length of replaceing in structure */
      type4,          /* Type of arg4 */
      *ip,
      *ipnew,
      *iparg3;

  DBUG_ENTER ("red_repstr");

  START_MODUL("red_repstr");

  if T_CON(arg3)             /* zwar kann repstr beliebige Aus-       */
  {                          /* druecke in eine Liste einbauen, aber  */
    END_MODUL("red_repstr"); /* falls ear fuer Konstruktorausdruecke  */
    DBUG_RETURN(0);               /* keinen expression-Pointer anlegt, so  */
  }                          /* kann das nur bedeuten, das _redcnt    */
  /* gleich Null ist und keine Beta-Reduktionen gezaehlt werden sollen*/
  /* lreplace darf diesen Ausdruck aber auch nicht selbst auslagern,da*/
  /* sonst evtl. ein Redex in den Heap gelangt. Deshalb muss hier die */
  /* Beta-Reduktion als gescheitert erklaert werden.    */

  if (T_POINTER(arg4) && R_DESC(DESC(arg4),class) == C_LIST) {
    STRUCT_ARG(arg1,position);
    if (position <= 0) /* negative or zero structuring parameter arg1 */
      goto fail;
    STRUCT_ARG(arg2,length);
    if (length < 0)    /* negative structuring parameter arg2 */
      goto fail;
    dim4 = R_LIST(DESC(arg4),dim); /* dimension if replace one element */
    if ((length + position) > (dim4 + 1)) /* check of arguments */
      goto fail;
    type4 = R_DESC(DESC(arg4),type);
    if ((T_POINTER(arg3)) && (R_DESC(DESC(arg3),class) == C_LIST)) {
      if (type4 != R_DESC(DESC(arg3),type))
        goto fail;           /* not equal types in arguments */
      dim3 = R_LIST(DESC(arg3),dim); /* dimension of arg3 */
      dim4 = dim4 - length + dim3;  /* dimension new descriptor */
      if ((dim3 == 0) && (R_DESC(DESC(arg3),type) == TY_UNDEF)) {
        if (dim4 == 0) {
          INC_REFCNT(_nil); /* the result of the reduction is a <>   */
          _desc = _nil;
          goto success;
        }
      }
      if ((dim4 == 0) && (type4 == TY_STRING)) {
        INC_REFCNT(_nilstring); /* the result of the reduction is a '` */
        _desc = _nilstring;
        goto success;
      }
    }
    else
      goto fail; /* arg3 is not a list or only a stackelem*/
    position--;  /* first position in data-vector         */
                 /* position in data-vector starts with 0 */
    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,type4);
    L_LIST((*_desc),dim) = dim4;
    if (R_LIST(DESC(arg4),ptdd) != NULL)  /* Edit-Feld */
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg4),ptdd)),special);
    else L_LIST((*_desc),special) = R_LIST(DESC(arg4),special);
    NEWHEAP(dim4,A_LIST((*_desc),ptdv));
    RES_HEAP;
    ip = (int *) R_LIST(DESC(arg4),ptdv);
    ipnew = (int *) R_LIST((*_desc),ptdv);
    for (i = 0; i < position; i++)   /* copy if it is possible until position  */
      if T_POINTER((ipnew[i] = ip[i])) /* is reached  */
        INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
    iparg3 = (int *) R_LIST(DESC(arg3),ptdv);
    for (j = 0; j < dim3; j++)     /* copy the argument 3 */
    {
      if (T_POINTER(ipnew[i] = iparg3[j]))
        INC_REFCNT((PTR_DESCRIPTOR)iparg3[j]);
      i++;
    }
    switch(length) {  /* first position of the rest in data-vector */
     case 1  : j = i - j;         /* REPLACE one element */
               break;
     case 0  : j = i - dim3 - 1 ; /* INSERT / APPEND  */
               break;
     default : j = i - dim3 - 1 + length;/* DROP / REPLACE a structure */
               break;
    } /** end switch length **/
    for ( ;i < dim4; i++) {    /* copy the rest, if there is one */
      ++j;
      if T_POINTER((ipnew[i] = ip[j]))
        INC_REFCNT((PTR_DESCRIPTOR)ip[j]);
    }
    REL_HEAP;
success:
    if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    if (T_POINTER(arg2)) DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    DEC_REFCNT((PTR_DESCRIPTOR)arg3);
    DEC_REFCNT((PTR_DESCRIPTOR)arg4);
    END_MODUL("red_repstr");
    DBUG_RETURN(1);
  }
fail:
  END_MODUL("red_repstr");
  DBUG_RETURN(0);
} /**** end of function red_repstr ****/

/*****************************************************************************/
/*                              red_lreplace                                 */
/*---------------------------------------------------------------------------*/
/* Funktion :                                                                */
/*     lreplace ersetzt Unterstrukturen eines Strings 'string' (einer Liste  */
/*     'list'). Der Parameter 'n' bestimmt, welches Stringelement  (Listen-  */
/*     element) durch das mit 'substitute' spezifizierte  Argument  ersetzt  */
/*     werden soll. Bei Anwendung der Funktion auf Listen kann 'substitute'  */
/*     ein beliebiger Ausdruck der Reduktionssprache  sein.  Bei  Anwendung  */
/*     der Funktion auf Strings muss 'substitute'  ein  String  mit der Di-  */
/*     mension 1 oder ein leerer String sein.                                */
/*                                                                           */
/*          lreplace( position , substitute , list   )   ==>  list           */
/*          lreplace( position , string1    , string )   ==>  string         */
/*                                                                           */
/* parameter : arg1  -  position                                             */
/*             arg2  -  substitute                                           */
/*             arg3     list        /   string                               */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_lreplace(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int i,dim;
  int index, *ip, *ipnew/*, *iparg2*/;     /* RS 30/10/92 */ 
  PTR_DESCRIPTOR repl;

  DBUG_ENTER ("red_lreplace");

  START_MODUL("red_lreplace");

  if T_CON(arg2)              /* zwar kann lreplace beliebige Aus-   */
  {                           /* druecke in eine Liste einbauen, aber*/
    END_MODUL("red_lreplace");/* falls ear fuer Konstruktorausdruecke*/
    DBUG_RETURN(0);                /* keinen expression-Pointer anlegt, so*/
  }                           /* kann das nur bedeuten, das _redcnt  */
  /* gleich Null ist und keine Beta-Reduktionen gezaehlt werden sollen*/
  /* lreplace darf diesen Ausdruck aber auch nicht selbst auslagern,da*/
  /* sonst evtl. ein Redex in den Heap gelangt. Deshalb muss hier die */
  /* Beta-Reduktion als gescheitert erklaert werden. */

  if (T_POINTER(arg3) && R_DESC(DESC(arg3),class) == C_LIST) {
    if (R_DESC(DESC(arg3),type) == TY_STRING) {
      /* (COUNT) von TB, 9.11.1992 */
      if (T_POINTER(arg2)
       && R_DESC(DESC(arg2),type) == TY_STRING && 
	  (COUNT)R_LIST(DESC(arg2),dim) <= (COUNT)1) {
         /* wenn arg3 ein string ist, so muss arg2 ein string der Dimension */
         /* 1 oder ein leerer string sein */
         if (R_LIST(DESC(arg2),dim) == 0)
           repl = (PTR_DESCRIPTOR)arg2;
         else 
           repl = (PTR_DESCRIPTOR)R_LIST(DESC(arg2),ptdv)[0];
         if (T_POINTER((STACKELEM)repl)) INC_REFCNT(repl);
      }
      else goto fail;
    }
    else repl = (PTR_DESCRIPTOR)arg2;

    dim = R_LIST(DESC(arg3),dim);
    STRUCT_ARG(arg1,index);
    if (0 < index && index <= dim) {
      index--;
#if SCAVENGE
      if (0) {
        ;
#else
      if (R_DESC(DESC(arg3),ref_count) == 1
       && (R_LIST(DESC(arg3),ptdd) == NULL
        || R_DESC(*R_LIST(DESC(arg3),ptdd),ref_count) == 1)) {
         /* destructive update possible */
         _desc = (PTR_DESCRIPTOR)arg3;
         INC_REFCNT(_desc);
         arg2 = R_LIST(*_desc,ptdv)[index];
         if (T_POINTER(arg2)) DEC_REFCNT((PTR_DESCRIPTOR)arg2);
         R_LIST(*_desc,ptdv)[index] = (T_HEAPELEM)repl;
#endif /* SCAVENGE      RS 1.12.1992 */
      }
      else {
        SCAV_SET(_scav1,arg3);
        NEWDESC(_desc); TEST_DESC;
        SCAV_SET(arg3,_scav1);
        LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg3),type));
        L_LIST(*_desc,dim) = dim;
        if (R_LIST(DESC(arg3),ptdd) != NULL)  /* Edit-Feld */
          L_LIST(*_desc,special) = R_LIST(*R_LIST(DESC(arg3),ptdd),special);
        else L_LIST(*_desc,special) = R_LIST(DESC(arg3),special);
        NEWHEAP(dim,A_LIST(*_desc,ptdv));
        SCAV_SET(arg3,_scav1);
        RES_HEAP;
        ip    = (int *) R_LIST(DESC(arg3),ptdv);
        ipnew = (int *) R_LIST(*_desc,ptdv);
        for (i = 0; i < index; i++)
          if T_POINTER((ipnew[i] = ip[i]))
            INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
        ipnew[i++] = (T_HEAPELEM)repl;
        for ( ;i < dim; i++)
          if T_POINTER((ipnew[i] = ip[i]))
            INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
        REL_HEAP;
      }
      /* success: */
      if (T_POINTER(arg1)) DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      DEC_REFCNT((PTR_DESCRIPTOR)arg3);
      SCAV_SET(_scav1,0);
      END_MODUL("red_lreplace");
      DBUG_RETURN(1);
    }
  }
fail:
  END_MODUL("red_lreplace");
  DBUG_RETURN(0);
} /**** end of function red_lreplace ****/

/*uh*/ /*****************************************************************************/
/*uh*/ /*                              LGEN                                         */
/*uh*/ /*---------------------------------------------------------------------------*/
/*uh*/ /*****************************************************************************/
/*uh*/ int red_lgen(arg1,arg2,arg3)
/*uh*/ STACKELEM arg1,arg2,arg3;
/*uh*/ {
/*uh*/   int dim=0,i; /* Initialisierung von TB, 6.11.1992 */
/*uh*/   PTR_HEAPELEM list; 

         DBUG_ENTER ("red_lgen");

/*uh*/   START_MODUL("red_lgen");
/*uh*/   if (_formated) {
/*uh*/     if (T_2INT(arg1,arg2)) {
/*uh*/       if (!T_INT(arg3)) {
/*uh*/          if ( T_POINTER(arg3) && (R_DESC(*(PTR_DESCRIPTOR)arg3,class)==C_SCALAR) ) {
/*uh*/             arg3 = R_SCALAR(DESC(arg3),valr);
/*uh*/          } else {
/*uh*/             END_MODUL("red_lgen");
/*uh*/             DBUG_RETURN(0);
/*uh*/          }
/*uh*/       } else {
/*uh*/          arg3 = VAL_INT(arg3);
/*uh*/       }
/*uh*/       arg1 = VAL_INT(arg1);
/*uh*/       arg2 = VAL_INT(arg2);
/*uh*/       /* Hier haben ARG1, ARG2 und ARG3 integer Werte */   
/*uh*/       if (arg2) {
/*uh*/          dim = (arg3-arg1) / arg2;
/*uh*/          if (dim < 0 )  dim = 0;  else dim++;
/*uh*/       } else {
/*uh*/          if (arg1==arg3) {
/*uh*/            post_mortem("lgen: Cannot produce infinite List");
/*uh*/          } else 
/*uh*/          dim = 0;   /* dann auch NIL-Desc _nil zurueckgeben. */
/*uh*/       }
/*uh*/       if (!dim) {
/*uh*/         _desc=_nil;
/*uh*/         INC_REFCNT(_nil);
/*uh*/         END_MODUL("red_lgen");
/*uh*/         DBUG_RETURN(1);
/*uh*/       }
/*uh*/       NEWDESC(_desc); TEST_DESC;
/*uh*/       LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
/*uh*/       L_LIST(*_desc,dim) = dim;
/*uh*/       NEWHEAP(dim,A_LIST(*_desc,ptdv));
/*uh*/       RES_HEAP;
/*uh*/       list = (PTR_HEAPELEM) R_LIST(*_desc,ptdv);
/*uh*/       for (i=0; i<dim;i++) {
/*uh*/          list[i]=TAG_INT(arg1);
/*uh*/          arg1 += arg2;
/*uh*/       }
/*uh*/       REL_HEAP;
/*uh*/     } else {
/*uh*/       post_mortem("lgen: Can currently only generate lists from (small) integers.");
/*uh*/     }
/*uh*/   } else {
/*uh*/     post_mortem("lgen: Can currently only generate lists from formatted numbers.");	
/*uh*/   } ;
/*uh*/   END_MODUL("red_lgen"); 
/*uh*/   DBUG_RETURN(1);
/*uh*/ }
/***************************** end of file lstruct.c *************************/
