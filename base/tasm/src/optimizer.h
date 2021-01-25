#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

extern void optimize_delta(FUNCTION *);
extern int am_i_insertable(FUNCTION *);
extern ORDER *inline_function(FUNCTION *, ORDER *, int);
extern void insert_functions(FUNCTION *);
extern void reset_inlined_flags(void);
extern void overwrite_func(char *, ORDER *);
#endif
