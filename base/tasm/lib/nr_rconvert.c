/*****************************************************************************/
/* all decrements of the refcounts for the arguments removed                 */
/* original was rconvert.c, difference is that here won't be any             */
/* reference counting [sf 080595]                                            */
/*****************************************************************************/
/*                        MODULE FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* Conversion function      : converts the type of                           */
/* ========================== (1xn)-mat, (nx1)-mat, scalar, vector, tvector  */
/*                            n-vect, n-tvect,      (list , matrix)          */
/*                            into in function-header specified type         */
/*                                                                           */
/* rconvert.c  -- external  : nr_red_to_scal,   nr_red_to_vect,                    */
/*                            nr_red_to_tvect,  nr_red_to_mat                      */
/*                            nr_red_to_list,   nr_red_to_field                    */
/*                called in : rear.c                                         */
/*             -- internal  : gen_many_id                                    */
/*                external  : gen_many_id                                    */
/*                called in : rvalfunc.c , rmstruct.c                        */
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

#if D_MESS
#include "d_mess_io.h"
#endif

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
extern PTR_DESCRIPTOR gen_many_id();

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
extern void inc_idcnt(); /* TB, 4.11.1992 */       /* rheap.c */
#endif

/*---------------------------------------------------------------------------*/
/*                            defines                                        */
/*---------------------------------------------------------------------------*/
#define DESC(x)  (* (PTR_DESCRIPTOR) x)

/* Macros for heap-overflow handling */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define NEWHEAP(size,adr) if ((*d_m_newheap)(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
#else
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
#endif
#define TEST_DESC         if (_desc == 0)             \
                            longjmp(_interruptbuf,0)



/*****************************************************************************/
/*                     nr_red_to_scal   (convert to scalar)                     */
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
int nr_red_to_scal(arg)
register STACKELEM arg;
{
  char arg_class;                             /*      class of argument      */

  DBUG_ENTER ("nr_red_to_scal");

  START_MODUL("nr_red_to_scal");

  if (T_INT(arg) || T_BOOLEAN(arg)) /*  argument is no descriptor  */
  {
    _desc = (PTR_DESCRIPTOR) arg;
    END_MODUL("nr_red_to_scal");
    DBUG_RETURN(1);                                /*  no decrement of argument   */
  }
  if (!T_POINTER(arg)) goto fail;

  switch(arg_class = R_DESC(DESC(arg),class))
  {
    case C_SCALAR:
    case C_DIGIT:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity   */
        END_MODUL("nr_red_to_scal");
        DBUG_RETURN(1);
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
              _desc = (PTR_DESCRIPTOR)TAG_INT(*(R_MVT(DESC(arg),ptdv,arg_class)));
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
        /* break;      auskommentiert RS 3.12.1992 */
    default:
        goto fail;
  }  /* end of switch */

success:
  END_MODUL("nr_red_to_scal");
  DBUG_RETURN(1);
fail:
  END_MODUL("nr_red_to_scal");
  DBUG_RETURN(0);

} /**** end of function nr_red_to_scal ****/

/*****************************************************************************/
/*                    nr_red_to_vect   (convert to vector)                      */
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
int nr_red_to_vect(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  COUNT rows;

  DBUG_ENTER ("nr_red_to_vect");

  START_MODUL("nr_red_to_vect");

  if (T_BOOLEAN(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_VECTOR,TY_BOOL);
    L_MVT((*_desc),nrows,C_VECTOR) = 1;
    L_MVT((*_desc),ncols,C_VECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ? TRUE : FALSE; /* store boolean in datavector */
    REL_HEAP;
    END_MODUL("nr_red_to_vect");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (T_INT(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_VECTOR,TY_INTEGER);
    L_MVT((*_desc),nrows,C_VECTOR) = 1;
    L_MVT((*_desc),ncols,C_VECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_VECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
    RES_HEAP;
    ip[0] = VAL_INT(arg);
    REL_HEAP;
    END_MODUL("nr_red_to_vect");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (!T_POINTER(arg)) goto fail;
  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR:
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
        END_MODUL("nr_red_to_vect");
        DBUG_RETURN(1);                            /*  no decrement of argument   */
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
        END_MODUL("nr_red_to_vect");
        DBUG_RETURN(1);                            /*  no decrement of argument   */
    case C_VECTOR:
        _desc = (PTR_DESCRIPTOR) arg;         /*     function is identity    */
        END_MODUL("nr_red_to_vect");
        DBUG_RETURN(1);
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
  END_MODUL("nr_red_to_vect");
  DBUG_RETURN(1);
fail:
  END_MODUL("nr_red_to_vect");
  DBUG_RETURN(0);

} /**** end of function nr_red_to_vect ****/



/*****************************************************************************/
/*                nr_red_to_tvect   (convert to transposed vector)              */
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
int nr_red_to_tvect(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  COUNT cols;                                 /*   number of columns of arg  */

  DBUG_ENTER ("nr_red_to_tvect");

  START_MODUL("nr_red_to_tvect");

  if (T_BOOLEAN(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_BOOL);
    L_MVT((*_desc),nrows,C_TVECTOR) = 1;
    L_MVT((*_desc),ncols,C_TVECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ? TRUE : FALSE; /* store boolean in datavector */
    REL_HEAP;
    END_MODUL("nr_red_to_tvect");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (T_INT(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_INTEGER);
    L_MVT((*_desc),nrows,C_TVECTOR) = 1;
    L_MVT((*_desc),ncols,C_TVECTOR) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
    ip = (int *) R_MVT((*_desc),ptdv,C_TVECTOR);
    RES_HEAP;
    ip[0] = VAL_INT(arg);
    REL_HEAP;
    END_MODUL("nr_red_to_tvect");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (!T_POINTER(arg)) goto fail;
  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR:
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_REAL);
        L_MVT((*_desc),nrows,C_TVECTOR) = 1;
        L_MVT((*_desc),ncols,C_TVECTOR) = 1;
        NEWHEAP(1,A_MVT((*_desc),ptdv,C_TVECTOR));
        rp = (double *) R_MVT((*_desc),ptdv,C_TVECTOR);
        RES_HEAP;
        /* ---- store real in datavector: ---- */
        rp[0] = R_SCALAR(DESC(arg),valr);
        REL_HEAP;
        goto success;
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
        END_MODUL("nr_red_to_tvect");
        DBUG_RETURN(1);                            /*  no decrement of argument   */
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
        END_MODUL("nr_red_to_tvect");
        DBUG_RETURN(1);                             /*  no decrement of argument   */
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
        END_MODUL("nr_red_to_tvect");
        DBUG_RETURN(1);
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
  END_MODUL("nr_red_to_tvect");
  DBUG_RETURN(1);
fail:
  END_MODUL("nr_red_to_tvect");
  DBUG_RETURN(0);

} /**** end of function nr_red_to_tvect ****/




/*****************************************************************************/
/*                     nr_red_to_mat     (convert to matrix)                    */
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
int nr_red_to_mat(arg)
register STACKELEM arg;
{
  int  *ip;                                   /*       integer pointer       */
  double  *rp;                                /*         real pointer        */
  char arg_class = '\0';                             /*      class of argument      */

  DBUG_ENTER ("nr_red_to_mat");

  START_MODUL("nr_red_to_mat");

  if (T_BOOLEAN(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_MATRIX,TY_BOOL);
    L_MVT((*_desc),nrows,C_MATRIX) = 1;
    L_MVT((*_desc),ncols,C_MATRIX) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
    ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    RES_HEAP;
    ip[0] = (T_SA_TRUE(arg)) ? TRUE : FALSE; /* store boolean in datavector */
    REL_HEAP;
    END_MODUL("nr_red_to_mat");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (T_INT(arg)) { /*  argument is no descriptor  */
    NEWDESC(_desc); TEST_DESC;
    MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
    L_MVT((*_desc),nrows,C_MATRIX) = 1;
    L_MVT((*_desc),ncols,C_MATRIX) = 1;
    NEWHEAP(1,A_MVT((*_desc),ptdv,C_MATRIX));
    ip = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    RES_HEAP;
    ip[0] = VAL_INT(arg);
    REL_HEAP;
    END_MODUL("nr_red_to_mat");
    DBUG_RETURN(1);                                /* no decrement on stackelement*/
  }
  if (!T_POINTER(arg)) goto fail;
  switch(arg_class = R_DESC(DESC(arg),class)) {
    case C_SCALAR:
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
        END_MODUL("nr_red_to_mat");
        DBUG_RETURN(1);                            /*  no decrement of argument   */
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
        END_MODUL("nr_red_to_mat");
        DBUG_RETURN(1);                            /*  no decrement of argument   */
    case C_VECTOR:
    case C_TVECTOR:
	/* (COUNT) von TB, 9.11.1992 */
        if ((COUNT)R_MVT(DESC(arg),nrows,arg_class) > (COUNT)0)
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
        END_MODUL("nr_red_to_mat");
        DBUG_RETURN(1);
    default:
        goto fail;
  }  /* end of switch */

success:
  END_MODUL("nr_red_to_mat");
  DBUG_RETURN(1);
fail:
  END_MODUL("nr_red_to_mat");
  DBUG_RETURN(0);

} /**** end of function nr_red_to_mat ****/


/*****************************************************************************/
/*                     nr_red_to_list    (convert to list)                      */
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
int nr_red_to_list(arg1)
register STACKELEM arg1;
{
  PTR_DESCRIPTOR hilf_desc;
  PTR_DESCRIPTOR row_desc;           /* zeilenweise einen Deskriptor anlegen */
  int i,j,k,
   /* dim,   RS 30/10/92 */                    /* Dimension des Datenvektors */
      element,
      *ip,*ipnew,
      rows,cols;
  /*double *rpnew;  RS 30/10/92 */
  char  class1;

  DBUG_ENTER ("nr_red_to_list");

  START_MODUL("nr_red_to_list");

  if (  !T_POINTER(arg1) ||
      (((class1 = R_DESC(DESC(arg1),class)) != C_MATRIX) &&
       (class1                             != C_VECTOR) &&
       (class1                             != C_TVECTOR)))
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
                 L_LIST(*row_desc,ptdv)[j] =
                         TAG_INT(R_MVT(DESC(arg1),ptdv,class1)[k++]);
               }
               L_LIST(*_desc,ptdv)[i] = (int)row_desc;
               REL_HEAP;
           }
         else { /* class == vector or class == tvector */
            RES_HEAP;
            for (i = rows ; --i >= 0;) {
              L_LIST(*_desc,ptdv)[i] =
                         TAG_INT(R_MVT(DESC(arg1),ptdv,class1)[i]);
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
#if (D_SLAVE && D_MESS && D_MHEAP)
                 if ((hilf_desc = (*d_m_newdesc)()) == 0) longjmp(_interruptbuf,0);
#else
                 if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
#endif
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
#if (D_SLAVE && D_MESS && D_MHEAP)
             if ((hilf_desc = (*d_m_newdesc)()) == 0) longjmp(_interruptbuf,0);
#else
             if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
#endif
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
  END_MODUL("nr_red_to_list");
  DBUG_RETURN(0);

success:
   END_MODUL("nr_red_to_list");
   DBUG_RETURN(1);
} /**** end of function nr_red_to_list ****/


/*****************************************************************************/
/*          nr_red_to_field   (convert hiearchical list to mat or vect)         */
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
int nr_red_to_field(arg1)
register STACKELEM arg1;
{
  int dim,rows,cols,j,i,k=0,type,class=0; /* Initialisierung von TB, 6.11.1992 */
  int elem, element;
  int *ip,*ipnew,*ip1;
/*int substructdim;           RS 30/10/92 */
  double *rpnew;
  char class1;

  DBUG_ENTER ("nr_red_to_field");

  START_MODUL("nr_red_to_field");

  if (T_POINTER(arg1)
   && R_DESC(DESC(arg1),class) == C_LIST
   && R_DESC(DESC(arg1),type) == TY_UNDEF) {
    dim = R_LIST(DESC(arg1),dim);
    if (dim == 0) {
      /* empty list ==> nilvect */
      _desc = _nilvect;
      INC_REFCNT(_nilvect);
      goto success;
    }
    /* Typ- und Klassenueberpruefung: */
    ip = (int *)R_LIST(DESC(arg1),ptdv);
    element = ip[0];
    if (T_POINTER(element)) {
      class = R_DESC(DESC(element),class);
      switch (class) {
         case C_LIST:
           if (R_DESC(DESC(element),type) == TY_UNDEF) {
             /* first element is a list  <<1,2>,<3,4>>, make a matrix */
             cols = R_LIST(DESC(element),dim);
             if (cols == 0) {
               if (dim == 1) {
                 _desc = _nilmat;
                 INC_REFCNT(_nilmat);
                 goto success;
               }
               else goto fail;
             }
             /* non empty matrix */
             ip1 = (int *)R_LIST(DESC(element),ptdv);
             elem = ip1[0];
             if (T_INT(elem)) {
               class  = C_SCALAR; /* ein = weggenommen von RS 30/10/92 */
               type = TY_INTEGER;
             }
             else
             if (T_BOOLEAN(elem)) {
               class   = C_SCALAR;
               type  = TY_BOOL;  /* beidesmal ein = weggenommen von RS 30/10/92 */
             }
             else
             if (T_POINTER(elem)) {
               class = R_DESC(DESC(elem),class);
               type = R_DESC(DESC(elem),type);
             }
             else goto fail;
             /* test dimensions, classes and types */
             for (i = dim; --i >= 0;) {
               element = ip[i];
               if (T_POINTER(element)) {
                 if (R_DESC(DESC(element),class) == C_LIST
                  && R_DESC(DESC(element),type) == TY_UNDEF
                  && R_LIST(DESC(element),dim) == cols) {
                   /* dimension is ok */
                   ip1 = (int *)R_LIST(DESC(element),ptdv);
                   elem = ip1[0];
                   if (T_INT(elem)) {
                     if (type == TY_INTEGER || type == TY_REAL)
                       continue;
                     else goto fail;
                   }
                   if (T_BOOLEAN(elem) && type == TY_BOOL)
                     continue;
                   if (T_POINTER(elem) && R_DESC(DESC(elem),class) == class) {
                     if (R_DESC(DESC(elem),type) == type)
                       continue;
                     if (R_DESC(DESC(elem),type) == TY_REAL && type == TY_INTEGER) {
                       type = TY_REAL;
                       continue;
                     }
                   }
                 }
               }
               goto fail;
             } /* end for */
             if (class == C_MATRIX)
               type = TY_MATRIX;
             class1 = C_MATRIX;     /* class of result descriptor   */
             rows = dim;            /* rows of the result matrix    */
             dim  = cols * rows;    /* dimension of data-vector of  */
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
      } /* end switch class */

      for (i = 1; i < dim; i++) {        /********  list --> vect ************/
        element = ip[i];
        if (T_POINTER(element)) {
          if (R_DESC(DESC(element),class) == class) {
            if (R_DESC(DESC(element),type) != type)
              if (class == C_SCALAR)
                type = TY_REAL;              /* make a vector of type real */
              else if (class == C_MATRIX) goto fail;
          }
          else goto fail;
        }
        else goto fail;
      }
    }
    else {                               /********  list --> vect ************/
      if (T_INT(element)) {
        type = TY_INTEGER;    /* make a vector of type integer */
        for (i = 1; i < dim; i++) {
          if (T_INT(ip[i]))
            continue;
          if (T_POINTER(ip[i]) && R_DESC(DESC(ip[i]),class) == C_SCALAR) {
            type = TY_REAL;
            continue;
          }
          goto fail;
        }
      }
      else
      if (T_BOOLEAN(element)) {
        type = TY_BOOL;    /* make a vector of type bool */
        for (i = 1; i < dim; i++) {
          if (T_BOOLEAN(ip[i]))
            continue;
          goto fail;
        }
      }
      else goto fail;
    }
    class1 = C_VECTOR;     /* class of result descriptor */
    rows   = 1;            /* vector has only one row    */
    cols   = dim;

matrix:                    /* used, if list ==> matrix   */

    switch(type) {
     /*----------------*/
      case TY_INTEGER:
     /*----------------*/
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,class1,TY_INTEGER);
           L_MVT(*_desc,nrows,class1) = rows;
           L_MVT(*_desc,ncols,class1) = cols;
           NEWHEAP(dim,A_MVT(*_desc,ptdv,class1));
           RES_HEAP;
           ipnew = (int *) R_MVT(*_desc,ptdv,class1);
           if (class1 == C_MATRIX) {          /***** result is a matrix ****/
             for(i = 0; i < rows; ++i) {
               ip1 = (int *) R_LIST(DESC(ip[i]),ptdv);
               for(j = 0; j < cols; ++j)
                ipnew[k++] = VAL_INT(ip1[j]);
             }
           }
           else {     /* class1 == C_VECTOR */
             for (i = 0; i < cols; ++i)
               ipnew[i] = VAL_INT(ip[i]);
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
               element = ip1[j];
               if (class == C_SCALAR)
                 if (T_POINTER(element))
                   rpnew[k++] = R_SCALAR(DESC(element),valr);
                 else rpnew[k++] = VAL_INT(element);
             }
           }
           else {     /* class1 == C_VECTOR */
             for (i = 0; i < cols; ++i) {
               element = ip[i];
               if (class == C_SCALAR)
                 if (T_POINTER(element))
                   rpnew[k++] = R_SCALAR(DESC(element),valr);
                 else rpnew[k++] = VAL_INT(element);
             }
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
success:
    END_MODUL("nr_red_to_field");
    DBUG_RETURN(1);
  } /** if T_POINTER arg1 **/
fail:
  END_MODUL("nr_red_to_field");
  DBUG_RETURN(0);
} /**** nr_red_to_field ****/





/*****************************************************************************/
/*           nr_red_mk_string  (convert to string to hirarchical string)        */
/* ------------------------------------------------------------------------- */
/* function :  mk_string : string           -->  string                      */
/* example  :              'abcdf`          -->  ''abcdf``                   */
/*                                                                           */
/* parameter:  arg - stackelement:                                           */
/* returns  :  '0' : if reduction wasn't successful                          */
/*             '1' : if reduction was successful                             */
/* called by:  rear.c                                                        */
/*****************************************************************************/
int nr_red_mk_string(arg1)
register STACKELEM arg1;
{

  DBUG_ENTER ("nr_red_mk_string");

  START_MODUL("nr_red_mk_string");

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
  END_MODUL("nr_red_mk_string");
  DBUG_RETURN(1);
fail:
  END_MODUL("nr_red_mk_string");
  DBUG_RETURN(0);

  } /**** nr_red_mk_string ****/


/*************************  end of file  rconvert.c  *************************/
