/* $Log: rfstruct.c,v $
 * Revision 1.11  1996/02/20  12:50:59  cr
 * now one file for implicit and explicit refcounting versions (flag NOREF)
 *
 * Revision 1.10  1996/02/15  15:36:49  cr
 * modified red_update and red_delete for use with tasm
 * don't use stacks
 *
 * Revision 1.9  1995/12/15  17:41:47  cr
 * use W as working stack :-)
 *
 * Revision 1.8  1995/12/14  16:52:09  rs
 * including: rstack.h
 *
 * Revision 1.7  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.6  1995/12/11  13:13:46  cr
 * two more operations on frames: FR_SLOTS,FR_DELETE
 *
 * Revision 1.5  1995/11/20  17:07:04  cr
 * no NEWDESC while RES_HEAP ..
 *
 * Revision 1.4  1995/05/22  09:17:12  rs
 * chan..err.. changed nCUBE to D_SLAVE !
 *
 * Revision 1.3  1995/03/28  14:29:02  cr
 * bug fixed in red_update
 *
 * Revision 1.2  1995/03/22  18:55:41  cr
 * added red_test
 * */


/*****************************************************************************/
/*                         MODULE FOR PROCESSING PHASE                       */
/* ------------------------------------------------------------------------- */
/* rfstruct.c  (also used as nr_rfstruct.c for tasm-lib)                     */
/*                                                                           */
/* special functions :                                                       */
/* -------------------                                                       */
/*  Functions for manipulation of frames                                     */
/*                                                                           */
/*  FRAME functions :                                                        */
/*  -- external:  red_fselect, red_test, red_update, red_delete, red_slots   */
/*                                                                           */
/*  Variants that don't do reference counting for their arguments are also   */
/*      generated from this source file (FLAG NOREF):                        */
/*  nr_red_fselect, nr_red_test, nr_red_update, nr_red_delete, nr_red_slots  */
/*                                                                           */
/* (Frames are simple slot-lists now.                                        */
/*  Better implementation -hashing- needed?)                                 */
/*****************************************************************************/

/* set prefix for function names and control refcounting for arguments */
#if NOREF
#define PREFIX(f)  nr_red ## f
#define ARG_DEC_REFCNT(arg)
#else
#define PREFIX(f)  red ## f
#define ARG_DEC_REFCNT(arg) DEC_REFCNT(arg)
#endif

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
extern  BOOLEAN str_eq();         /* rlogfunc.c */
extern  void stack_error();       /* rstack.c */

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

/*****************************************************************************/
/*                              red_test                                     */
/*---------------------------------------------------------------------------*/
/* function  : test for existence of named slot in given frame               */
/*                                                                           */
/*             lselect : frame  x  string  ==>  boolean                      */
/*                                                                           */
/* parameter : arg1  -  C_FRAME with TY_FRAME                                */
/*             arg2  -  name of slot to test for                             */
/*****************************************************************************/
int PREFIX(_test)(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("PREFIX(_test)");

  START_MODUL("PREFIX(_test)");
  if (   (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                          && R_DESC(DESC(arg1),type) == TY_FRAME)
      && (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST
                          && R_DESC(DESC(arg2),type) == TY_STRING))
  {
    int index;
    PTR_DESCRIPTOR slot;

    for(index=0; index < (int) R_FRAME(DESC(arg1),dim); index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        if (str_eq(R_SLOT(*slot,name),(PTR_DESCRIPTOR)arg2))
        {
            _desc = (PTR_DESCRIPTOR)SA_TRUE;
            ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
            ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg2);

            END_MODUL("PREFIX(_test)");
            DBUG_RETURN(1);
        }
    }
    _desc = (PTR_DESCRIPTOR)SA_FALSE;
    ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg2);

    END_MODUL("PREFIX(_test)");
    DBUG_RETURN(1);
  }
  /* fail */
  END_MODUL("PREFIX(_test)");
  DBUG_RETURN(0);
} /**** end of function red_test ****/


/*****************************************************************************/
/*                              red_fselect                                  */
/*---------------------------------------------------------------------------*/
/* function  : get value from named slot in given frame                      */
/*                                                                           */
/*             lselect : frame  x  string  ==>  expr                         */
/*                                                                           */
/* parameter : arg1  -  C_FRAME with TY_FRAME                                */
/*             arg2  -  name of slot to select                               */
/*****************************************************************************/
int PREFIX(_fselect)(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("PREFIX(_fselect)");

  START_MODUL("PREFIX(_fselect)");
  if (   (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                          && R_DESC(DESC(arg1),type) == TY_FRAME)
      && (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST
                          && R_DESC(DESC(arg2),type) == TY_STRING))
  {
    int index;
    PTR_DESCRIPTOR slot;

    for(index=0; index < (int) R_FRAME(DESC(arg1),dim); index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        if (str_eq(R_SLOT(*slot,name),(PTR_DESCRIPTOR)arg2))
        {
            _desc = (PTR_DESCRIPTOR)R_SLOT(*slot,value);
            if (T_POINTER((STACKELEM)_desc)) INC_REFCNT(_desc);
            ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
            ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg2);

            END_MODUL("PREFIX(_fselect)");
            DBUG_RETURN(2);
        }
    }
  }
  /* fail */
  END_MODUL("PREFIX(_fselect)");
  DBUG_RETURN(0);
} /**** end of function red_fselect ****/

/*****************************************************************************/
/*                           red_update                                      */
/*---------------------------------------------------------------------------*/
/* function  : replace value of named slot in frame                          */
/*                                                                           */
/*             update : frame x string x expr -> frame                       */
/*                                                                           */
/* parameter : arg1 - frame to be updated                                    */
/*             arg2 - name of slot                                           */
/*             arg3 - new value                                              */
/*                                                                           */
/* (slot will be added if missing)                                           */
/*****************************************************************************/
int PREFIX(_update)(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{

  DBUG_ENTER ("PREFIX(_update)");

  START_MODUL("PREFIX(_update)");
  if (   (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                          && R_DESC(DESC(arg1),type) == TY_FRAME)
      && (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST
                          && R_DESC(DESC(arg2),type) == TY_STRING))
  {
    int index,count=0,exists=-1;
    PTR_DESCRIPTOR newslot,slot;

    NEWDESC(newslot); TESTDESC(newslot);
    DESC_MASK(newslot,1,C_FRAME,TY_SLOT);
    L_SLOT(*newslot,name) = (PTR_DESCRIPTOR)arg2;
    L_SLOT(*newslot,value) = (T_HEAPELEM)arg3;

    count = (int)R_FRAME(DESC(arg1),dim);

    for(index=0; index < count ; index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        if (str_eq(R_SLOT(*slot,name),(PTR_DESCRIPTOR)arg2))
          exists = index; 
    }

    if ((R_DESC(DESC(arg1),ref_count) == 1) && (exists != -1))
    {
        _desc = (PTR_DESCRIPTOR)arg1;

#if NOREF
        INC_REFCNT(_desc); /* to correct implicit refcounting of arg1 */
#endif

        DEC_REFCNT((PTR_DESCRIPTOR)R_FRAME(DESC(arg1),slots)[exists]);
        L_FRAME(*_desc,slots)[exists] = (T_HEAPELEM)newslot;
    }
    else
    {
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_FRAME,TY_FRAME);

        L_FRAME(*_desc,dim) = ( (exists == -1) ? count+1 : count );
        NEWHEAP(R_FRAME(*_desc,dim),A_FRAME(*_desc,slots));

        for(index=0; index < count ; index++)
        {
            slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
            INC_REFCNT(slot);
            L_FRAME(*_desc,slots)[index] = (T_HEAPELEM)slot;
        }

        if (exists == -1)
        {
            L_FRAME(*_desc,slots)[count] = (T_HEAPELEM)newslot;
        }
        else
        {
            DEC_REFCNT((PTR_DESCRIPTOR)R_FRAME(DESC(arg1),slots)[exists]);
            L_FRAME(*_desc,slots)[exists] = (T_HEAPELEM)newslot;
        }

        ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    }
/*    RES_HEAP; REL_HEAP; not used here */

    END_MODUL("PREFIX(_update)");
    DBUG_RETURN(2);

  }
  /* fail */
  END_MODUL("PREFIX(_update)");
  DBUG_RETURN(0);
} /**** end of function red_update ****/




/*****************************************************************************/
/*                           red_delete                                      */
/*---------------------------------------------------------------------------*/
/* function  : delete named slot in frame                                    */
/*                                                                           */
/*             delete : frame x string -> frame                              */
/*                                                                           */
/* parameter : arg1 - frame to be modified                                   */
/*             arg2 - name of slot                                           */
/*                                                                           */
/*****************************************************************************/
int PREFIX(_delete)(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("PREFIX(_delete)");

  START_MODUL("PREFIX(_delete)");
  if (   (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                          && R_DESC(DESC(arg1),type) == TY_FRAME)
      && (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST
                          && R_DESC(DESC(arg2),type) == TY_STRING))
  {
    int index,dim,exists=-1;
    PTR_DESCRIPTOR slot;

    dim = (int) R_FRAME(DESC(arg1),dim);

    for(index=0; index < dim ; index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        if (str_eq(R_SLOT(*slot,name),(PTR_DESCRIPTOR)arg2))
          exists = index;
    }

    ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg2);

    if (exists == -1)
    {
        _desc = (PTR_DESCRIPTOR)arg1;
#if NOREF
        INC_REFCNT(_desc); /* to correct implicit refcounting of arg1 */
#endif
        END_MODUL("PREFIX(_delete)");
        DBUG_RETURN(1);
    }
    else
    {
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_FRAME,TY_FRAME);
        L_FRAME(*_desc,dim) = dim-1;
        NEWHEAP(R_FRAME(*_desc,dim),A_FRAME(*_desc,slots));

        for(index=0; index < exists ; index++)
        {
            slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
            INC_REFCNT(slot);
            L_FRAME(*_desc,slots)[index] = (T_HEAPELEM)slot;
        }

        for(index=exists+1; index < dim ; index++)
        {
            slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
            INC_REFCNT(slot);
            L_FRAME(*_desc,slots)[index-1] = (T_HEAPELEM)slot;
        }

        ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    }

/*    RES_HEAP; REL_HEAP; not used here */

    END_MODUL("PREFIX(_delete)");
    DBUG_RETURN(2);

  }
  /* fail */
  END_MODUL("PREFIX(_delete)");
  DBUG_RETURN(0);
} /**** end of function red_delete ****/


/*****************************************************************************/
/*                           red_slots                                       */
/*---------------------------------------------------------------------------*/
/* function  : return list of slotnames in frame                             */
/*                                                                           */
/*             update : frame -> list of strings                             */
/*                                                                           */
/* parameter : arg1 - frame to be inspected                                  */
/*                                                                           */
/*****************************************************************************/
int PREFIX(_slots)(arg1)
register STACKELEM arg1;
{

  DBUG_ENTER ("PREFIX(_slots)");

  START_MODUL("PREFIX(_slots)");
  if (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                      && R_DESC(DESC(arg1),type) == TY_FRAME)
  {
    int index,dim;
    PTR_DESCRIPTOR slot,name;

    dim = (int) R_FRAME(DESC(arg1),dim);

    if (dim == 0)
    {
      ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      _desc = _nil;
      INC_REFCNT(_nil);
      END_MODUL("PREFIX(_slots)");
      DBUG_RETURN(1);
    }


    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
    L_LIST(*_desc,dim) = dim;
    NEWHEAP(R_LIST(*_desc,dim),A_LIST(*_desc,ptdv));

    RES_HEAP;

    for(index=0; index < dim ; index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        name = (PTR_DESCRIPTOR)(R_SLOT(*slot,name)); 
        INC_REFCNT(name);
        L_LIST(*_desc,ptdv)[index] = (T_HEAPELEM)name;
    }
    REL_HEAP;

    ARG_DEC_REFCNT((PTR_DESCRIPTOR)arg1);

    END_MODUL("PREFIX(_slots)");
    DBUG_RETURN(2);

  }
  /* fail */
  END_MODUL("PREFIX(_slots)");
  DBUG_RETURN(0);
} /**** end of function red_slots ****/



/***************************** end of file rfstruct.c *************************/
