/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*****************************************************************************/
/*                        MODULE FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* Conversion function      : converts the type of                           */
/* ========================== (1xn)-mat, (nx1)-mat, scalar, vector, tvector  */
/*                            n-vect, n-tvect,      (list , matrix)          */
/*                            into in function-header specified type         */
/*                                                                           */
/* rconvert.c  -- external  : red_to_scal,   red_to_vect,                    */
/*                            red_to_tvect,  red_to_mat                      */
/*                            red_to_list,   red_to_field                    */
/*                called in : rear.c                                         */
/*             -- internal  : gen_many_id                                    */
/*                external  : gen_many_id                                    */
/*                called in : rvalfunc.c , rmstruct.c                        */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>

/*---------------------------------------------------------------------------*/
/*                            extern variables                               */
/*---------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;          /* <r> , jump-adress for heapoverflow */
extern PTR_DESCRIPTOR _desc;           /* <w> , pointer to resultdescriptor  */

/* Ausserdem: S_e,S_a  <w>. Auf dem E-Stack liegt das Stackelement fuer      */
/* die Strukturierungsfunktion.j Auf dem A-Stack liegt das letzte Argument   */
/* dieser Funktion, welches im Reduktionsfall durch das Ergebnis ueber-      */
/* schrieben wird.                                                           */

/*---------------------------------------------------------------------------*/
/*                            extern functions                               */
/*---------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();       /* rheap:c   */
extern int newheap();                  /* rheap:c   */
extern int conv_digit_int();           /* rdig-v1:c */

/*---------------------------------------------------------------------------*/
/*                            defines                                        */
/*---------------------------------------------------------------------------*/
#define DESC(x)  (* (PTR_DESCRIPTOR) x)

/* Macros for heap-overflow handling */
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
#define TEST_DESC         if (_desc == 0)             \
                            longjmp(_interruptbuf,0)


/*****************************************************************************/
/*               gen_many_id     (generate ind. desc. for many classes)      */
/* ------------------------------------------------------------------------- */
/* function :  generates indirect descriptor for dir_desc with class         */
/*             specified in ind_class.                                       */
/*             If ind_class is 'scalar' no ind. desc. will be created        */
/*             because heap-element of dir_desc has needed structure.        */
/*             If dir_class is 'scalar' no ind. desc. can be created in      */
/*             in cases TY_BOOL, TY_REAL, TY_INTEGER because there is no     */
/*             heap-element to point at.                                     */
/* prameter :  ind_class - class of indirect descr. that will be created     */
/*             ind_type  - type of indirect descr. that will be created      */
/*             dir_desc  - pointer to direct descriptor                      */
/* returns  :  pointer to indirect descriptor                                */
/*               '0'  if not enough heapspace                                */
/* called by:  red_to_????            (   this module   )                    */
/*             red_unite, red_select  (module: rstruct:c)                    */
/*             red_c                  (module: rval:c   )                    */
/*****************************************************************************/
PTR_DESCRIPTOR gen_many_id(ind_class,dir_desc)
char ind_class;                                /*   class of indirect desc.   */
register PTR_DESCRIPTOR dir_desc;
{
  PTR_DESCRIPTOR desc;                         /* pointer to result descriptor*/
  char dir_class,                              /*  class of direct descriptor */
       ind_type;                               /* type of indirect descriptor */

  START_MODUL("gen_many_id");
  NEWDESC(desc);
  if (desc == NULL)                            /*       case of trouble       */
  {
    END_MODUL("gen_many_id");
    return(0);
  }

  if ( (ind_class == (dir_class = R_DESC((*dir_desc),class))) &&
       (ind_class != C_MATRIX) )
    post_mortem
    ("gen_many_id: try to create ind. desc with same class as dir. desc.");

  ind_type = R_DESC((*dir_desc),type);         /*   same type for ind. desc   */
  MVT_DESC_MASK(desc,1,ind_class,ind_type);

  switch(ind_class)
  {
  /*---------------*/
    case C_SCALAR:
  /*---------------*/
        switch(ind_type)
        {
          case TY_BOOL:
              post_mortem
              ("gen_many_id: try to create scalar descriptor of type 'bool'");
          case TY_INTEGER:
          case TY_REAL:
              post_mortem
              ("gen_many_id: try to create indirect descriptor of class 'scalar'");
          default:
              post_mortem
              ("gen_many_id: try to create ind. desc. from dir. desc. of incompatible class/type");
        }
        break;
  /*---------------*/
    case C_LIST:                               /* that means with TY_STRING */
  /*---------------*/
        if (ind_type == TY_STRING)
          post_mortem
          ("gen_many_id: take string desc. itself as result of conversion!");
        else
          post_mortem
          ("gen_many_id: try to create ind. desc. from dir. desc. of incompatible class/type");
        break;
  /*---------------*/
    case C_DIGIT:
  /*---------------*/
        if (ind_type == TY_DIGIT)
          post_mortem
          ("gen_many_id: take digit desc. itself as result of conversion!");
        else
          post_mortem
          ("gen_many_id: try to create ind. desc. from dir. desc. of incompatible class/type");
        break;
  /*---------------*/
    case C_VECTOR:
    case C_TVECTOR:
    case C_MATRIX:
  /*---------------*/
        switch(dir_class)
        {
        /*~~~~~~~~~~~~~~~*/
          case C_SCALAR:
          case C_DIGIT:
          case C_LIST:                         /* that means with TY_STRING */
        /*~~~~~~~~~~~~~~~*/
              switch(R_DESC((*dir_desc),type))
              {
                case TY_REAL:
                    post_mortem
                    ("gen_many_id: try to create ind. desc. from scalar real");
                case TY_INTEGER:
                    post_mortem
                    ("gen_many_id: try to create ind. desc. from scalar integer");
                case TY_DIGIT:
                    post_mortem
                    ("gen_many_id: try to create ind. desc. from digit descriptor");
                case TY_STRING:
                    post_mortem
                    ("gen_many_id: try to create ind. desc. from string descriptor");
                default:
                    post_mortem
                    ("gen_many_id: unknown class/type of source descriptor");
              }
              break;
        /*~~~~~~~~~~~~~~~*/
          case C_VECTOR:
          case C_TVECTOR:
          case C_MATRIX:
        /*~~~~~~~~~~~~~~~*/
        /* ---- exchange nrows and ncols case vect <==> tvect conversion ---- */
              if (
                   ((ind_class == C_VECTOR) && (dir_class == C_TVECTOR)) ||
                   ((ind_class == C_TVECTOR) && (dir_class == C_VECTOR))
                 )
              {
                L_MVT((*desc),nrows,ind_class) =
                              R_MVT((*dir_desc),ncols,dir_class);
                L_MVT((*desc),ncols,ind_class) =
                              R_MVT((*dir_desc),nrows,dir_class);
              }
              else
              {
                L_MVT((*desc),nrows,ind_class) =
                              R_MVT((*dir_desc),nrows,dir_class);
                L_MVT((*desc),ncols,ind_class) =
                              R_MVT((*dir_desc),ncols,dir_class);
              }
              L_MVT((*desc),ptdv,ind_class) =
                            R_MVT((*dir_desc),ptdv ,dir_class);

              /* ---- source desc. is possibly indirect itself: ---- */
              if (
                   R_MVT((*dir_desc),ptdd,dir_class) == (T_PTD) 0
                 )
                L_MVT((*desc),ptdd,ind_class) = (T_PTD) dir_desc;
              else
                L_MVT((*desc),ptdd,ind_class) =
                      (T_PTD) R_MVT((*dir_desc),ptdd,dir_class);

              /* ---- for correct freeing of heap-element: ---- */
              INC_REFCNT((PTR_DESCRIPTOR) R_MVT((*desc),ptdd,ind_class));
              break;
        /*~~~~~~~~~~~~~~~*/
          default:
        /*~~~~~~~~~~~~~~~*/
              post_mortem
              ("gen_many_id: source descriptor of unexpected class");
        }  /* end of switch(dir_class) */
        break;
  /*---------------*/
    default:
  /*---------------*/
        post_mortem
        ("gen_many_id: target descriptor of unexpected class");
  }  /* end of switch(ind_class) */

  INC_IDCNT;                                  /* increment indirect-descrip- */
                                              /* tor-counter;                */
  END_MODUL("gen_many_id");
  return(desc);

} /**** end of function gen_many_id ****/

/*****************************************************************************/
/*                     red_to_scal   (convert to scalar)                     */
/* ------------------------------------------------------------------------- */
/* function :  to_scal  : (1x1)-mat        -->  scal                         */
/*                        1-vect           -->  scal                         */
/*                        1-tvect          -->  scal                         */
/*                        scal             -->  scal    (identity)           */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/*                   (pointer to matrix / vector / tvector / scalar desc.)   */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_scal(arg)
register STACKELEM arg;
{
  char arg_class;                             /*      class of argument      */

  START_MODUL("red_to_scal");

  if (T_BOOLEAN(arg))                         /*  argument is no descriptor  */
  {
    _desc = (PTR_DESCRIPTOR) arg;
    END_MODUL("red_to_scal");
    return(1);                                /*  no decrement of argument   */
  }
  if (!T_POINTER(arg)) goto fail;

  switch(arg_class = R_DESC(DESC(arg),class))
  {
    case C_SCALAR:
    case C_DIGIT:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity   */
        END_MODUL("red_to_scal");
        return(1);
    case C_VECTOR:
    case C_TVECTOR:
    case C_MATRIX:
        if ( (R_MVT(DESC(arg),nrows,arg_class) != 1) ||
             (R_MVT(DESC(arg),ncols,arg_class) != 1) ||
             (R_DESC(DESC(arg),type)  == TY_MATRIX) )
          goto fail;

        switch(R_DESC(DESC(arg),type))
        {
          case TY_BOOL:
              /* ---- never create descriptor for boolean value ---- */
              _desc = (PTR_DESCRIPTOR) ((*(R_MVT(DESC(arg),ptdv,arg_class))) ?
                                                         SA_TRUE : SA_FALSE);
              goto success;
          case TY_INTEGER:
              NEWDESC(_desc); TEST_DESC;
              DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
              L_SCALAR((*_desc),vali) = *(R_MVT(DESC(arg),ptdv,arg_class));
              goto success;
          case TY_REAL:
              NEWDESC(_desc); TEST_DESC;
              DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
              L_SCALAR((*_desc),valr) = *(R_MVT(DESC(arg),ptdv,arg_class));
              goto success;
          case TY_DIGIT:
              _desc = (PTR_DESCRIPTOR) *(R_MVT(DESC(arg),ptdv,arg_class));
              INC_REFCNT(_desc);               /* compon. of struct. is result*/
              goto success;
          default:
              goto fail;
        }
        break;
    default:
        goto fail;
  }  /* end of switch */

success:
  DEC_REFCNT((PTR_DESCRIPTOR) arg);
  END_MODUL("red_to_scal");
  return(1);
fail:
  END_MODUL("red_to_scal");
  return(0);

} /**** end of function red_to_scal ****/

/*****************************************************************************/
/*                    red_to_vect   (convert to vector)                      */
/* ------------------------------------------------------------------------- */
/* function :  to_vect  : (1xn)-mat        -->  vect                         */
/*                        tvect            -->  vect                         */
/*                        scal             -->  vect                         */
/*                        vect             -->  vect   (identity)            */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/*                   (pointer to matrix / vector / tvector / scalar desc.)   */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_vect(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  COUNT rows;

  START_MODUL("red_to_vect");

  if (T_BOOLEAN(arg))                         /*  argument is no descriptor  */
  {
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_VECTOR,TY_BOOL);
    L_MVT((*_desc),nrows,C_VECTOR) = 1;
    L_MVT((*_desc),ncols,C_VECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ?                /* store boolean in datavector */
                 TRUE : FALSE;
    REL_HEAP;
    END_MODUL("red_to_vect");
    return(1);                                /* no decrement on stackelement*/
  }

  if (!T_POINTER(arg)) goto fail;

  switch(R_DESC(DESC(arg),class))
  {
    case C_SCALAR:
        switch(R_DESC(DESC(arg),type))
        {
          case TY_INTEGER:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_VECTOR,TY_INTEGER);
              L_MVT((*_desc),nrows,C_VECTOR) = 1;
              L_MVT((*_desc),ncols,C_VECTOR) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
              ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
              RES_HEAP;
              /* ---- store integer in datavector: ---- */
              ip[0] = R_SCALAR(DESC(arg),vali);
              REL_HEAP;
              goto success;
          case TY_REAL:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_VECTOR,TY_REAL);
              L_MVT((*_desc),nrows,C_VECTOR) = 1;
              L_MVT((*_desc),ncols,C_VECTOR) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
              rp = (double *) R_MVT((*_desc),ptdv,C_VECTOR);
              RES_HEAP;
              /* ---- store real in datavector: ---- */
              rp[0] = R_SCALAR(DESC(arg),valr);
              REL_HEAP;
              goto success;
          default:
              goto fail;
        }
    case C_DIGIT:
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_VECTOR,TY_DIGIT);
        L_MVT((*_desc),nrows,C_VECTOR) = 1;
        L_MVT((*_desc),ncols,C_VECTOR) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
        ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int) arg;
        REL_HEAP;
        END_MODUL("red_to_vect");
        return(1);                            /*  no decrement of argument   */
    case C_LIST:
        if (R_DESC(DESC(arg),type) != TY_STRING) /*    arg is no string      */
          goto fail;

        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_VECTOR,TY_STRING);
        L_MVT((*_desc),nrows,C_VECTOR) = 1;
        L_MVT((*_desc),ncols,C_VECTOR) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
        ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int ) arg;
        REL_HEAP;
        END_MODUL("red_to_vect");
        return(1);                            /*  no decrement of argument   */
    case C_VECTOR:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity    */
        END_MODUL("red_to_vect");
        return(1);
    case C_TVECTOR:
        if ((rows = R_MVT(DESC(arg),nrows,C_TVECTOR)) > 1) /* tvect ==> vect */
         goto fail;
        if (rows == 1)
        {
          /* ---- create indirect descriptor: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR)arg);
          TEST_DESC;
          break;
        }
        else
        {
          _desc = _nilvect;
          INC_REFCNT(_nilvect);
          break;
        }
    case C_MATRIX:
        if ( ((rows = R_MVT(DESC(arg),nrows,C_MATRIX)) > 1) ||
             (   R_DESC(DESC(arg),type)  ==  TY_MATRIX  ) )
          goto fail;

        if (rows == 1)
        {
          /* ---- create indirect descriptor: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR)arg);
          TEST_DESC;
          break;
        }
        else
        {
          _desc = _nilvect;
          INC_REFCNT(_nilvect);
          break;
        }
    default:
        goto fail;
  }  /* end of switch */

success:
  DEC_REFCNT((PTR_DESCRIPTOR) arg);
  END_MODUL("red_to_vect");
  return(1);
fail:
  END_MODUL("red_to_vect");
  return(0);

} /**** end of function red_to_vect ****/



/*****************************************************************************/
/*                red_to_tvect   (convert to transposed vector)              */
/* ------------------------------------------------------------------------- */
/* function :  to_tvect : (nx1)-mat        -->  tvect                        */
/*                        vect             -->  tvect                        */
/*                        scal             -->  tvect                        */
/*                        tvect            -->  tvect   (identity)           */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/*                   (pointer to matrix / vector / tvector / scalar desc.)   */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_tvect(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  COUNT cols;                                 /*   number of columns of arg  */

  START_MODUL("red_to_tvect");

  if (T_BOOLEAN(arg))                         /*  argument is no descriptor  */
  {
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_BOOL);
    L_MVT((*_desc),nrows,C_TVECTOR) = 1;
    L_MVT((*_desc),ncols,C_TVECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ?                /* store boolean in datavector */
                 TRUE : FALSE;
    REL_HEAP;
    END_MODUL("red_to_tvect");
    return(1);                                /* no decrement on stackelement*/
  }

  if (!T_POINTER(arg)) goto fail;

  switch(R_DESC(DESC(arg),class))
  {
    case C_SCALAR:
        switch(R_DESC(DESC(arg),type))
        {
          case TY_INTEGER:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_INTEGER);
              L_MVT((*_desc),nrows,C_TVECTOR) = 1;
              L_MVT((*_desc),ncols,C_TVECTOR) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
              ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
              RES_HEAP;
              /* ---- store integer in datavector: ---- */
              ip[0] = R_SCALAR(DESC(arg),vali);
              REL_HEAP;
              goto success;
          case TY_REAL:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_REAL);
              L_MVT((*_desc),nrows,C_TVECTOR) = 1;
              L_MVT((*_desc),ncols,C_TVECTOR) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
              rp = (double *) R_MVT((*_desc),ptdv,C_TVECTOR);
              RES_HEAP;
              /* ---- store real in data-vector: ---- */
              rp[0] = R_SCALAR(DESC(arg),valr);
              REL_HEAP;
              goto success;
          default:
              goto fail;
        }
    case C_DIGIT:
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_DIGIT);
        L_MVT((*_desc),nrows,C_TVECTOR) = 1;
        L_MVT((*_desc),ncols,C_TVECTOR) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
        ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int) arg;
        REL_HEAP;
        END_MODUL("red_to_tvect");
        return(1);                            /*  no decrement of argument   */
    case C_LIST:
        if (R_DESC(DESC(arg),type) != TY_STRING) /*    arg is no string      */
          goto fail;

        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_STRING);
        L_MVT((*_desc),nrows,C_TVECTOR) = 1;
        L_MVT((*_desc),ncols,C_TVECTOR) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
        ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int) arg;
        REL_HEAP;
        END_MODUL("red_to_tvect");
        return(1);                             /*  no decrement of argument   */
    case C_VECTOR:
        if ((cols = R_MVT(DESC(arg),ncols,C_VECTOR)) > 1)   /* vect ==> tvect */
         goto fail;
        if (cols == 1)                         /*  vect ==> tvect             */
        {
          /* ---- create indirect descriptor: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR)arg);
          TEST_DESC;
          break;
        }
        else
        {
          _desc = _niltvect;
          INC_REFCNT(_niltvect);
          break;
        }
    case C_TVECTOR:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity    */
        END_MODUL("red_to_tvect");
        return(1);
    case C_MATRIX:
        if ( ((cols = R_MVT(DESC(arg),ncols,C_MATRIX)) > 1) ||
             (   R_DESC(DESC(arg),type)  ==  TY_MATRIX  ) )
          goto fail;

        if (cols == 1)
        {
          /* ---- create indirect descriptor: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR)arg);
          TEST_DESC;
          break;
        }
        else
        {
          _desc = _niltvect;
          INC_REFCNT(_niltvect);
          break;
        }
    default:
        goto fail;
  }  /* end of switch */

success:
  DEC_REFCNT((PTR_DESCRIPTOR) arg);
  END_MODUL("red_to_tvect");
  return(1);
fail:
  END_MODUL("red_to_tvect");
  return(0);

} /**** end of function red_to_tvect ****/




/*****************************************************************************/
/*                     red_to_mat     (convert to matrix)                    */
/* ------------------------------------------------------------------------- */
/* function :  to_mat :   vect             -->  mat                          */
/*                        tvect            -->  mat                          */
/*                        scal             -->  mat                          */
/*                        mat              -->  mat     (identity)           */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/*                   (pointer to matrix / vector / tvector / scalar desc.)   */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_mat(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  char arg_class;                             /*      class of argument      */

  START_MODUL("red_to_mat");

  if (T_BOOLEAN(arg))                         /*  argument is no descriptor  */
  {
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_MATRIX,TY_BOOL);
    L_MVT((*_desc),nrows,C_MATRIX) = 1;
    L_MVT((*_desc),ncols,C_MATRIX) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
    ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ?                /* store boolean in datavector */
                 TRUE : FALSE;
    REL_HEAP;
    END_MODUL("red_to_mat");
    return(1);                                /* no decrement on stackelement*/
  }

  if (!T_POINTER(arg)) goto fail;

  switch(arg_class = R_DESC(DESC(arg),class))
  {
    case C_SCALAR:
        switch(R_DESC(DESC(arg),type))
        {
          case TY_INTEGER:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
              L_MVT((*_desc),nrows,C_MATRIX) = 1;
              L_MVT((*_desc),ncols,C_MATRIX) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
              ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
              RES_HEAP;
              /* ---- store integer in datavector: ---- */
              ip[0] = R_SCALAR(DESC(arg),vali);
              REL_HEAP;
              goto success;
          case TY_REAL:
              NEWDESC(_desc); TEST_DESC;
              MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
              L_MVT((*_desc),nrows,C_MATRIX) = 1;
              L_MVT((*_desc),ncols,C_MATRIX) = 1;
              NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
              rp = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
              RES_HEAP;
              /* ---- store real in datavector: ---- */
              rp[0] = R_SCALAR(DESC(arg),valr);
              REL_HEAP;
              goto success;
          default:
              goto fail;
        }
    case C_DIGIT:
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_MATRIX,TY_DIGIT);
        L_MVT((*_desc),nrows,C_MATRIX) = 1;
        L_MVT((*_desc),ncols,C_MATRIX) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
        ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int) arg;
        REL_HEAP;
        END_MODUL("red_to_mat");
        return(1);                            /*  no decrement of argument   */
    case C_LIST:
        if (R_DESC(DESC(arg),type) != TY_STRING) /*    arg is no string      */
          goto fail;

        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_MATRIX,TY_STRING);
        L_MVT((*_desc),nrows,C_MATRIX) = 1;
        L_MVT((*_desc),ncols,C_MATRIX) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
        ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
        RES_HEAP;
        /* ---- store pointer to descriptor in datavector: ---- */
        ip[0] = (int) arg;
        REL_HEAP;
        END_MODUL("red_to_mat");
        return(1);                            /*  no decrement of argument   */
    case C_VECTOR:
    case C_TVECTOR:
        if (R_MVT(DESC(arg),nrows,arg_class) > 0)
        {
          /* ---- create indirect descriptor: ---- */
          _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg);
          TEST_DESC;
          break;
        }
        else
        {
          _desc = _nilmat;
          INC_REFCNT(_nilmat);
          break;
        }
    case C_MATRIX:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity    */
        END_MODUL("red_to_mat");
        return(1);
    default:
        goto fail;
  }  /* end of switch */

success:
  DEC_REFCNT((PTR_DESCRIPTOR) arg);
  END_MODUL("red_to_mat");
  return(1);
fail:
  END_MODUL("red_to_mat");
  return(0);

} /**** end of function red_to_mat ****/


/*****************************************************************************/
/*                     red_to_list    (convert to list)                      */
/* ------------------------------------------------------------------------- */
/* function :  to_list:   mat              -->  list                         */
/*                        vect             -->  list                         */
/*                        tvect            -->  list                         */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_list(arg1)
register STACKELEM arg1;
{
  PTR_DESCRIPTOR hilf_desc;
  PTR_DESCRIPTOR row_desc;           /* zeilenweise einen Deskriptor anlegen */
  int i,j,k,
      dim,                                     /* Dimension des Datenvektors */
      element,
      *ip,*ipnew,
      rows,cols;
  double *rpnew;
  char  class1;

  START_MODUL("red_to_list");

  if (  !T_POINTER(arg1) ||
      ((class1 = R_DESC(DESC(arg1),class)) != C_MATRIX) &&
       (class1                             != C_VECTOR) &&
       (class1                             != C_TVECTOR))
   goto fail;

  rows = R_MVT(DESC(arg1),nrows,class1);
  cols = R_MVT(DESC(arg1),ncols,class1);

  if ((rows == 0) && (cols ==0)) {        /* arg1 ist eine nilmat oder     */
    _desc = (PTR_DESCRIPTOR)NIL;          /* ein nilvect oder ein niltvect */
    goto success;                         /* ==> result: leere Liste       */
  }

  if (class1 == C_VECTOR)
    rows = cols;

  k = 0;

  NEWDESC(_desc); TEST_DESC;
  LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
  L_LIST((*_desc),dim) = rows;
  L_LIST((*_desc),special) = LIST;        /* Edit-Feld */
  NEWHEAP(rows,A_LIST((*_desc),ptdv));

  switch(R_DESC(DESC(arg1),type)) {
    case TY_INTEGER:
         if (class1 == C_MATRIX)
            for (i = 0 ; i < rows; i++) {
               NEWDESC(row_desc); TEST_DESC;
               LI_DESC_MASK(row_desc,1,C_LIST,TY_UNDEF);
               L_LIST((*row_desc),dim) = cols;
               L_LIST((*row_desc),special) = LIST;        /* Edit-Feld */
               NEWHEAP(cols,A_LIST((*row_desc),ptdv));

               RES_HEAP;
               for (j = 0; j < cols; j++ ) {
                 if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
                 DESC_MASK(hilf_desc,1,C_SCALAR,TY_INTEGER);
                 L_SCALAR((*hilf_desc),vali) =
                                   ((int *)R_MVT(DESC(arg1),ptdv,class1))[k++];
                 ((int *) L_LIST((*row_desc),ptdv))[j] = (int) hilf_desc;
               }

               ((int *) L_LIST((*_desc),ptdv))[i] = (int) row_desc;
               REL_HEAP;
           }
         else { /* class == vector or class == tvector */
            RES_HEAP;
            for (i = rows ; --i >= 0;) {
              if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
              DESC_MASK(hilf_desc,1,C_SCALAR,TY_INTEGER);
              L_SCALAR((*hilf_desc),vali) =
                                ((int *)R_MVT(DESC(arg1),ptdv,class1))[i];
              ((int *) L_LIST((*_desc),ptdv))[i] = (int) hilf_desc;
            }
            REL_HEAP;
         }
         goto success;

    case TY_REAL:
         if (class1 == C_MATRIX)
            for (i = 0 ; i < rows; i++) {
               NEWDESC(row_desc); TEST_DESC;
               LI_DESC_MASK(row_desc,1,C_LIST,TY_UNDEF);
               L_LIST((*row_desc),dim) = cols;
               L_LIST((*row_desc),special) = LIST;        /* Edit-Feld */
               NEWHEAP(cols,A_LIST((*row_desc),ptdv));

               RES_HEAP;
               for (j = 0; j < cols; j++ ) {
                 if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
                 DESC_MASK(hilf_desc,1,C_SCALAR,TY_REAL);
                 L_SCALAR((*hilf_desc),valr) =
                               ((double *)R_MVT(DESC(arg1),ptdv,class1))[k++];
                 ((int *) L_LIST((*row_desc),ptdv))[j] = (int) hilf_desc;
               }

               ((int *) L_LIST((*_desc),ptdv))[i] = (int) row_desc;
               REL_HEAP;
            }
         else { /* class == vector or class == tvector */
           RES_HEAP;
           for (i = rows; --i >= 0; ) {
             if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
             DESC_MASK(hilf_desc,1,C_SCALAR,TY_REAL);
             L_SCALAR((*hilf_desc),valr) =
                              ((double *)R_MVT(DESC(arg1),ptdv,class1))[i];
             ((int *) L_LIST((*_desc),ptdv))[i] = (int) hilf_desc;
           }
           REL_HEAP;
         }
         goto success;

    case TY_BOOL:
         if (class1 == C_MATRIX) {
            for (i = 0 ; i < rows; i++) {
               NEWDESC(row_desc); TEST_DESC;
               LI_DESC_MASK(row_desc,1,C_LIST,TY_UNDEF);
               L_LIST((*row_desc),dim) = cols;
               L_LIST((*row_desc),special) = LIST;        /* Edit-Feld */
               NEWHEAP(cols,A_LIST((*row_desc),ptdv));

               RES_HEAP;
               for (j = 0; j < cols; j++ ) {
                 ((int *) R_LIST((*row_desc),ptdv))[j] =
                     (((int *) R_MVT(DESC(arg1),ptdv,class1))[k++] ? SA_TRUE : SA_FALSE);
               }

               ((int *) L_LIST((*_desc),ptdv))[i] = (int) row_desc;
               REL_HEAP;
            }
         }
         else { /* class == vector or class == tvector */
            RES_HEAP;
            ip    = (int *) R_MVT(DESC(arg1),ptdv,class1);
            ipnew = (int *) R_LIST((*_desc),ptdv);
            for (i = rows; --i >= 0; )
              ipnew[i] = (ip[i] ? SA_TRUE : SA_FALSE);
            REL_HEAP;
         }
         goto success;
    default:
         if (class1 == C_MATRIX) {
            for (i = 0 ; i < rows; i++) {
               NEWDESC(row_desc); TEST_DESC;
               LI_DESC_MASK(row_desc,1,C_LIST,TY_UNDEF);
               L_LIST((*row_desc),dim) = cols;
               L_LIST((*row_desc),special) = LIST;        /* Edit-Feld */
               NEWHEAP(cols,A_LIST((*row_desc),ptdv));

               RES_HEAP;
               for (j = 0; j < cols; j++ ) {
                 ((int *) R_LIST((*row_desc),ptdv))[j] = element =
                     (((int *) R_MVT(DESC(arg1),ptdv,class1))[k++]);
                 INC_REFCNT((PTR_DESCRIPTOR)element);
               }

               ((int *) L_LIST((*_desc),ptdv))[i] = (int) row_desc;
               REL_HEAP;
            }
         }
         else { /* class == vector or class == tvector */
           RES_HEAP;
           ip = (int *)R_MVT(DESC(arg1),ptdv,class1);
           ipnew = (int *) R_LIST((*_desc),ptdv);
           for (i = rows; --i >= 0; ) {
             ipnew[i] = element = ip[i];
             INC_REFCNT((PTR_DESCRIPTOR)element);
           }
           REL_HEAP;
         }
         goto success;
  } /** Ende der Fallunterscheidung des Typs des Argumentes **/

fail:
  END_MODUL("red_to_list");
  return(0);

success:
   DEC_REFCNT((PTR_DESCRIPTOR)arg1);
   END_MODUL("red_to_list");
   return(1);
} /**** end of function red_to_list ****/


/*****************************************************************************/
/*          red_to_field   (convert hiearchical list to mat or vect)         */
/* ------------------------------------------------------------------------- */
/* function :  to_field : list             -->  mat                          */
/*                        list             -->  vect                         */
/* example  :             <<1,2>,<3,4>>    -->  mat<1,2>,                    */
/*                                                 <3,4>.                    */
/*                        < <> >           -->  nilmat                       */
/*                        <1,2,3,4>        -->  vect<1,2,3,4>                */
/*                        <>               -->  nilvect                      */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_to_field(arg1)
register STACKELEM arg1;
{
  int dim,rows,cols,j,i,k,type,class;
  int elem, element;
  int *ip,*ipnew,*ip1;
  int substructdim;
  double *rpnew;
  char class1;

  START_MODUL("red_to_field");

  if (T_POINTER(arg1))
  {
    if ((R_DESC(DESC(arg1),class) != C_LIST) ||
        (R_DESC(DESC(arg1),type)  != TY_UNDEF))  /* kein String */
      goto fail;

    if ((cols = dim = R_LIST(DESC(arg1),dim)) == 0) {  /* empty list ==> nilvect */
      _desc = _nilvect;
      INC_REFCNT(_nilvect);
      goto success;
    }

    /* Typ- und Klassenueberpruefung: */
    ip = (int *) R_LIST(DESC(arg1),ptdv);

    if T_POINTER((element = ip[0])) {

      switch(class = R_DESC(DESC(element),class)) {
         case C_LIST   :
           if (R_DESC(DESC(element),type) == TY_UNDEF) {
                                /* first element is a list  <<1,2>,<3,4>> */
                                /* make a matrix                          */
             if ((R_LIST(DESC(R_LIST(DESC(arg1),ptdv)[0]),dim) == 0) && (dim  > 1)) {
               goto fail;
             }
             else {
               if (dim == 1) {
                 _desc = _nilmat;
                 INC_REFCNT(_nilmat);
                 goto success;
               }
             }

             cols = R_LIST(DESC(R_LIST(DESC(arg1),ptdv)[0]),dim);
                        /* cols of the result matrix                  */
                        /* dimension of the first substructure (list) */
                   
             ip1 = (int*) R_LIST(DESC(element),ptdv);
             if T_POINTER((elem = ip1[0])) {
               switch(class = R_DESC(DESC(elem),class)) {
                 case C_DIGIT  :
                 case C_SCALAR :
                   type = R_DESC(DESC(elem),type);
                   break;
                 case C_LIST   :
                   type = TY_STRING;
                   break;
                 case C_MATRIX :
                   type = TY_MATRIX;
                   break;
                 default : goto fail;
               }

               for (i = 0; i < dim; ++i) {
                 if T_POINTER((element = ip[i])) {
                   if ((C_LIST != R_DESC(DESC(element),class))  ||
                       (TY_UNDEF != R_DESC(DESC(element),type)) ||
                       (cols != R_LIST(DESC(element),dim)))
                     goto fail;
                   ip1 = R_LIST(DESC(element),ptdv);
                   for ( j = 0; j < cols; ++j) {
                     if (T_POINTER(elem = ip1[j])) {
                       if (class == R_DESC(DESC(elem),class)) {
                         if (type != R_DESC(DESC(elem),type))
                           if (class == C_SCALAR)
                            type = TY_REAL;
                       }
                       else goto fail;
                     }
                     else goto fail;
                   } /** end for j * */
                 }
                 else goto fail;   /** not T_POINTER **/
               } /** end for i **/

             }
             else {    /* the substructure of the list may be a list with boolean's */
               for (i = 0; i < dim; ++i) {
                 element = ip[i];
                 if ((C_LIST != R_DESC(DESC(element),class))  ||
                     (TY_UNDEF != R_DESC(DESC(element),type)) ||
                     (cols != R_LIST(DESC(element),dim)))
                   goto fail;
                 ip1 = R_LIST(DESC(element),ptdv);
                 for (j = 0; j < cols; ++j)
                   if (T_POINTER(ip1[j]) || !T_BOOLEAN(ip1[j]))
                     goto fail;
               }
               type = TY_BOOL;
             }

             rows = dim;            /* rows of the result matrix    */
             dim  = cols * rows;    /* dimension of data-vector of  */
                                    /* the result matrix            */
             class1 = C_MATRIX;     /* class of result descriptor   */
             k = 0;                 /* initialization of variable k */
             goto matrix;
           }
           type = TY_STRING;  /* make a vector of type string */
           break;

         case C_DIGIT  :
         case C_SCALAR :
           type =  R_DESC(DESC(element),type); /* ==> make a vector  */
           break;

         default       : goto fail;
      } /* switch class of first element in list */

      for (i = 1; i < dim; i++)          /********  list --> vect ************/
        if T_POINTER((element = ip[i])) {
           if (class == R_DESC(DESC(element),class))
           {
             if (type  != R_DESC(DESC(element),type))
               if (class == C_SCALAR)
                 type = TY_REAL;              /* make a vector of type real */
               else if (class == C_MATRIX) goto fail;
           }
           else
             goto fail;
        }
        else goto fail;
    }
    else {                               /********  list --> vect ************/
      if (!T_BOOLEAN(element)) goto fail;
        type = TY_BOOL;    /* make a vector of type bool */
      for (i = 1; i < dim; i++)
        if (T_POINTER(ip[i]) || !T_BOOLEAN(ip[i]))
          goto fail;
    }
    class1 = C_VECTOR;     /* class of result descriptor */
    rows   = 1;            /* vector has only one row    */

matrix:                    /* used, if list ==> matrix   */

    switch(type) {
     /*----------------*/
      case TY_INTEGER:
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,TY_INTEGER);
           L_MVT((*_desc),nrows,class1) = rows;
           L_MVT((*_desc),ncols,class1) = cols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,class1);
           if (class1 == C_MATRIX) {          /***** result is a matrix ****/
             for(i = 0; i < rows; ++i) {
               ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
               for(j = 0; j < cols; ++j)
                ipnew[k++] = R_SCALAR(DESC(ip1[j]),vali);
             }
           }
           else {     /* class1 == C_VECTOR */
             ipnew = (int *) R_MVT((*_desc),ptdv,class1);
             for (i = 0; i <cols; ++i)
               ipnew[i] = R_SCALAR(DESC(ip[i]),vali);
           }
           REL_HEAP;
           goto success;

     /*----------------*/
      case TY_REAL:
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,TY_REAL);
           L_MVT((*_desc),nrows,class1) = rows;
           L_MVT((*_desc),ncols,class1) = cols;
           NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
           RES_HEAP;
           rpnew = (double *) R_MVT((*_desc),ptdv,class1);
           if (class1 == C_MATRIX) {         /**** result is a matrix ****/
             for(i = 0; i < rows; ++i) {
               ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
               for(j = 0; j < cols; ++j)
               if (R_DESC(DESC((element = ip1[j])),type) == TY_REAL)
                 rpnew[k++] = R_SCALAR(DESC(element),valr);
               else
                 rpnew[k++] = R_SCALAR(DESC(element),vali);
             }
           }
           else {     /* class1 == C_VECTOR */
             for (i = 0; i < cols; ++i)
               if (R_DESC(DESC((element = ip[i])),type) == TY_REAL)
                 rpnew[i] = R_SCALAR(DESC(element),valr);
               else
                 rpnew[i] = R_SCALAR(DESC(element),vali);
           }
           REL_HEAP;
           goto success;
     /*----------------*/
      case TY_DIGIT:
      case TY_STRING:
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,type);
           L_MVT((*_desc),nrows,class1) = rows;
           L_MVT((*_desc),ncols,class1) = cols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,class1);
           if (class1 == C_MATRIX) {        /**** result is a matrix ****/
             for(i = 0; i < rows; ++i) {
               ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
               for(j = 0; j < cols; ++j) {
                ipnew[k++] = element = ip1[j];
                INC_REFCNT((PTR_DESCRIPTOR)element);
               }
             }
           }
           else {     /* class1 == C_VECTOR */
             for (i = 0; i < cols; ++i) {
               ipnew[i] = element = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)element);
             }
           }
           REL_HEAP;
           goto success;
     /*----------------*/
      case TY_BOOL:
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,TY_BOOL);
           L_MVT((*_desc),nrows,class1) = rows;
           L_MVT((*_desc),ncols,class1) = cols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,class1);
           if (class1 == C_MATRIX) {       /**** result is a matrix ****/
             for(i = 0; i < rows; ++i) {
               ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
               for(j = 0; j < cols; ++j)
                ipnew[k++] = (T_SA_TRUE(ip1[j]) ? 1 : 0);
             }
           }
           else {     /* class1 == C_VECTOR */
             for (i = 0; i < cols; ++i)
               ipnew[i] = (T_SA_TRUE(ip[i]) ? 1 : 0);
           }
           REL_HEAP;
           goto success;
     /*----------------*/
      case TY_MATRIX:       /**** result is a matrix ; no class vector ******/
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,TY_MATRIX);
           L_MVT((*_desc),nrows,class1) = rows;
           L_MVT((*_desc),ncols,class1) = cols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,class1);
           for(i = 0; i < rows; ++i) {
             ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
             for(j = 0; j < cols; ++j) {
               INC_REFCNT((PTR_DESCRIPTOR)ip1[j]);
               ipnew[k++] = ip1[j];
             }
           }
           REL_HEAP;
           goto success;
    }
  } /** if T_POINTER arg1 **/

fail:
  END_MODUL("red_to_field");
  return(0);

success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_to_field");
    return(1);
} /**** red_to_field ****/





/*****************************************************************************/
/*           red_mk_string  (convert to string to hirarchical string)        */
/* ------------------------------------------------------------------------- */
/* function :  mk_string : string           -->  string                      */
/* example  :              'abcdf`          -->  ''abcdf``                   */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int red_mk_string(arg1)
register STACKELEM arg1;
{

  START_MODUL("red_mk_string");

  if (T_POINTER(arg1) && (R_DESC(DESC(arg1),class) == C_LIST) &&
                         (R_DESC(DESC(arg1),type) == TY_STRING))
  {
    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,TY_STRING);
    L_LIST((*_desc),dim) = 1;
    if (R_LIST(DESC(arg1),ptdd) != NULL)
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg1),ptdd)),special);
    else
      L_LIST((*_desc),special) = R_LIST(DESC(arg1),special);

    NEWHEAP(1,A_LIST((*_desc),ptdv));
    RES_HEAP;
    L_LIST((*_desc),ptdv)[0] = arg1;
    REL_HEAP;
    goto success;

  } /** if T_POINTER arg1 **/
  else goto fail;

success:
  END_MODUL("red_mk_string");
  return(1);
fail:
  END_MODUL("red_mk_string");
  return(0);

} /**** red_mk_string ****/


/*************************  end of file  rconvert.c  *************************/
