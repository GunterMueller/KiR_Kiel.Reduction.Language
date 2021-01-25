#ifndef __COMP_SUPPORT__
#define __COMP_SUPPORT__

extern void insert_labels();
extern void compile_typed_primf(FUNCTION *);
extern void compile_delta(FUNCTION *);
extern void convert_tail(FUNCTION *);
extern void insert_labels(FUNCTION *);
extern ORDER *dup_func(ORDER *,int flag);
extern int count_commands(FUNCTION *);
extern void make_one_function(PROGRAM *);
extern ORDER *last_push_r_before(ORDER *);

#endif
