/* $Log: tags.h,v $
 * Revision 1.1  1992/11/04 18:12:30  base
 * Initial revision
 * */
/* file tags.h                        */
/*              author:   dg 21.08.90 */
/*              modified: dg 19.08.91 */

/*------------------ tag bits (hardware) -----------------*/
/*       7        6       5       4         3    2 -  0   */
/*   +-------------------------------------------------+  */
/* 0 | TAIL- |  ATOM   | DATA | STATIC  | NOLST | name |  */
/* 1 |  BIT  | POINTER | CODE | DYNAMIC | NLIST |  "   |  */
/*   +-------------------------------------------------+  */
/*--------------------------------------------------------*/

#define HW_TAIL_BIT    0x80

#define HW_POINTER_BIT 0x40
#define HW_ATOM_BIT    0x00

#define HW_CODE_BIT    0x20
#define HW_DATA_BIT    0x00

#define HW_DYNAMIC_BIT 0x10
#define HW_STATIC_BIT  0x00

#define HW_NLIST_BIT   0x08
#define HW_NOLST_BIT   0x00

/*--- tags (hardware) ---*/

/* atoms */

#define HW_INT_NR      0x00 /* integer */
#define HW_BOOL_NR     0x01 /* boolean */
#define HW_CHAR_NR     0x02 /* character */
#define HW_FLOAT_NR    0x03 /* float, 32 bit */
#define HW_ANIL_NR     0x04 /* atomic nil */
#define HW_PRIM_NR     0x05 /* primitive function */
#define HW_UNUSED0_NR  0x06 /* unused */
#define HW_OTHER_NR    0x07 /* others */

/* pointers */

#define HW_REAL_NR     0x00 /* real, 64 bit */
#define HW_UNUSED1_NR  0x01 /* unused */
#define HW_CONS_NR     0x02 /* binary list */
#define HW_UNUSED2_NR  0x03 /* unused */

#define HW_NIL_NR      0x04 /* nil pointer */
#define HW_LIST_NR     0x05 /* list pointer */
#define HW_STRI_NR     0x06 /* string pointer */
#define HW_UNUSED3_NR  0x07 /* unused */

#define HW_COMB_NR     0x00 /* combinator */
#define HW_CASE_NR     0x01 /* case */
#define HW_COND_NR     0x02 /* conditinal */
#define HW_UNUSED4_NR  0x03 /* unused */
#define HW_UNUSED5_NR  0x04 /* unused */
#define HW_UNUSED6_NR  0x05 /* unused */
#define HW_CLOS_NR     0x06 /* closure */
#define HW_UNUSED7_NR  0x07 /* unused */

#ifndef IMM
#define IMM(x)      (x)
#endif

#define HW_NIL_NLIST_PAT IMM(0xe4) /* hier noch modularisieren! */
#define HW_NIL_NLIST_TST IMM(0x44) /* hier noch modularisieren! */

#define HW_POINTER_TAG IMM(HW_POINTER_BIT)
#define HW_OTHP_TAG    IMM(HW_POINTER_BIT | HW_OTHER_NR)

#define HW_INT_TAG     IMM(HW_ATOM_BIT    | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_INT_NR)
#define HW_BOOL_TAG    IMM(HW_ATOM_BIT    | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_BOOL_NR)
#define HW_PRIM_TAG    IMM(HW_ATOM_BIT    | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_PRIM_NR)
#define HW_OTHA_TAG    IMM(HW_ATOM_BIT    | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_OTHER_NR)

#define HW_REAL_TAG    IMM(HW_POINTER_BIT | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_REAL_NR)

#define HW_NIL_TAG     IMM(HW_POINTER_BIT | HW_DATA_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_NIL_NR)
#define HW_CONS_TAG    IMM(HW_POINTER_BIT | HW_DATA_BIT | HW_DYNAMIC_BIT | HW_NOLST_BIT | HW_CONS_NR)
#define HW_LIST_TAG    IMM(HW_POINTER_BIT | HW_DATA_BIT | HW_DYNAMIC_BIT | HW_NLIST_BIT | HW_LIST_NR)
#define HW_STRI_TAG    IMM(HW_POINTER_BIT | HW_DATA_BIT | HW_DYNAMIC_BIT | HW_NLIST_BIT | HW_STRI_NR)

#define HW_NLIST_TAG   HW_LIST_TAG

#define HW_COMB_TAG    IMM(HW_POINTER_BIT | HW_CODE_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_COMB_NR)
#define HW_CASE_TAG    IMM(HW_POINTER_BIT | HW_CODE_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_CASE_NR)
#define HW_CONDI_TAG   IMM(HW_POINTER_BIT | HW_CODE_BIT | HW_STATIC_BIT  | HW_NOLST_BIT | HW_COND_NR)
#define HW_CLOS_TAG    IMM(HW_POINTER_BIT | HW_CODE_BIT | HW_DYNAMIC_BIT | HW_NOLST_BIT | HW_CLOS_NR)

/* end of tags.h */
