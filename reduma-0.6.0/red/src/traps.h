/* $Log: traps.h,v $
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* file traps.h */

/* trap nummern (werden u.a. benutzt um in traptab zu greifen) */

#define TRAP_RESET	0
#define TRAP_ILL_INSTR	1
#define TRAP_ILL_ADDR	2
#define TRAP_UNUSED	3
#define TRAP_UNDER_M	4
#define TRAP_UNDER_H	5
#define TRAP_UNDER_A	6
#define TRAP_UNDER_I	7
#define TRAP_OVER_M	8
#define TRAP_OVER_H	9
#define TRAP_OVER_A	10
#define TRAP_OVER_I	11
#define TRAP_INTV	12
#define TRAP_BOOL	13
#define TRAP_LIST	14
#define TRAP_PRED	15

/* adressen der trap-handler in Runtime.laf */

#define not_implemented 6

#define traph_reset	not_implemented
#define traph_ill_instr	not_implemented
#define traph_ill_addr	not_implemented
#define traph_unused	not_implemented
#define traph_under_m	not_implemented
#define traph_under_h	not_implemented
#define traph_under_a	not_implemented
#define traph_under_i	not_implemented
#define traph_over_m	not_implemented
#define traph_over_h	not_implemented
#define traph_over_a	not_implemented
#define traph_over_i	not_implemented
#define traph_intv	2
#define traph_bool	3
#define traph_list	4
#define traph_pred	5

/* tabelle mit adressen der trap_handler */

static int traptab[] = {
  traph_reset		/* 00 reset */
, traph_ill_instr	/* 01 illegal instruction */
, traph_ill_addr	/* 02 illegal address */
, traph_unused		/* 03 unused */
, traph_under_m		/* 04 underflow stack M */
, traph_under_h		/* 05 underflow stack H (E) */
, traph_under_a		/* 06 underflow stack A */
, traph_under_i		/* 07 underflow stack I */
, traph_over_m		/* 08 overflow stack M */
, traph_over_h		/* 09 overflow stack H (E) */
, traph_over_a		/* 10 overflow stack A */
, traph_over_i		/* 11 overflow stack I */
, traph_intv		/* 12 Int_tag error */
, traph_bool		/* 13 Bool_tag error */
, traph_list		/* 14 List_tag error */
, traph_pred		/* 15 predicate eror */
};
