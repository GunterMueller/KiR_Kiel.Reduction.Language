/***************************************************************************/
/*                                                                         */
/*   file     : spec_sched.h                                               */
/*                                                                         */
/*   contents : defines for time-slicing scheduling                        */
/*                                                                         */
/*   last change:                                                          */
/*                                                                         */
/*         13.09.1994                                                      */
/*                                                                         */
/***************************************************************************/

#ifndef _spec_sched
#define _spec_sched

extern int ts_counter;

#define SCHED_TIME_OUT  (!--ts_counter)
#define RESET_TIMER     ts_counter=100

#endif
