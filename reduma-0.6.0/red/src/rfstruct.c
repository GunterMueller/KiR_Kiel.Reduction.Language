/* $Log: rfstruct.c,v $
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
/* rfstruct.c                                                                */
/*                                                                           */
/* special functions :                                                       */
/* -------------------                                                       */
/*    Functions for manipulation of frames                                   */
/*                                                                           */
/*    FRAME functions :                                                      */
/*                -- external:  red_fselect, red_test, red_update            */
/*                                                                           */
/* (Frames are simple slot-lists now.                                        */
/*  Better implementation -hashing- needed?)                                 */
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
extern  BOOLEAN str_eq();         /* rlogfunc.c */

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
/*                              red_test                                     */
/*---------------------------------------------------------------------------*/
/* function  : test for existence of named slot in given frame               */
/*                                                                           */
/*             lselect : frame  x  string  ==>  boolean                      */
/*                                                                           */
/* parameter : arg1  -  C_FRAME with TY_FRAME                                */
/*             arg2  -  name of slot to test for                             */
/*****************************************************************************/
int red_test(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("red_test");

  START_MODUL("red_test");
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
            DEC_REFCNT((PTR_DESCRIPTOR)arg1);
            DEC_REFCNT((PTR_DESCRIPTOR)arg2);

            END_MODUL("red_test");
            DBUG_RETURN(1);
        }
    }
    _desc = (PTR_DESCRIPTOR)SA_FALSE;
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);

    END_MODUL("red_test");
    DBUG_RETURN(1);
  }
  /* fail */
  END_MODUL("red_test");
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
int red_fselect(arg1,arg2)
register STACKELEM arg1,arg2;
{

  DBUG_ENTER ("red_fselect");

  START_MODUL("red_fselect");
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
            DEC_REFCNT((PTR_DESCRIPTOR)arg1);
            DEC_REFCNT((PTR_DESCRIPTOR)arg2);

            END_MODUL("red_fselect");
            DBUG_RETURN(2);
        }
    }
  }
  /* fail */
  END_MODUL("red_fselect");
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
/* (slot won't be added if missing -> should be changed later)               */
/*****************************************************************************/
int red_update(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{

  DBUG_ENTER ("red_update");

  START_MODUL("red_update");
  if (   (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_FRAME
                          && R_DESC(DESC(arg1),type) == TY_FRAME)
      && (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_LIST
                          && R_DESC(DESC(arg2),type) == TY_STRING))
  {
    int index;
    PTR_DESCRIPTOR slot;

    NEWDESC(_desc); TEST_DESC;
    DESC_MASK(_desc,1,C_FRAME,TY_FRAME);
    L_FRAME(*_desc,dim) = R_FRAME(DESC(arg1),dim);
    NEWHEAP(R_FRAME(*_desc,dim),A_FRAME(*_desc,slots));
    RES_HEAP;

    for(index=0; index < (int) R_FRAME(DESC(arg1),dim); index++)
    {
        slot = (PTR_DESCRIPTOR)(R_FRAME(DESC(arg1),slots)[index]); 
        if (str_eq(R_SLOT(*slot,name),(PTR_DESCRIPTOR)arg2))
        {
            NEWDESC(slot); TEST_DESC;
            DESC_MASK(slot,1,C_FRAME,TY_SLOT);
            INC_REFCNT((PTR_DESCRIPTOR)arg2);
            if (T_POINTER(arg3)) INC_REFCNT((PTR_DESCRIPTOR)arg3);
            L_SLOT(*slot,name) = (PTR_DESCRIPTOR)arg2;
            L_SLOT(*slot,value) = (T_HEAPELEM)arg3;

        } else { INC_REFCNT(slot); }
        L_FRAME(*_desc,slots)[index] = (T_HEAPELEM)slot;
    }
    REL_HEAP;

    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    if (T_POINTER(arg3)) DEC_REFCNT((PTR_DESCRIPTOR)arg3);

    END_MODUL("red_update");
    DBUG_RETURN(2);

  }
  /* fail */
  END_MODUL("red_update");
  DBUG_RETURN(0);
} /**** end of function red_update ****/


/***************************** end of file rfstruct.c *************************/
