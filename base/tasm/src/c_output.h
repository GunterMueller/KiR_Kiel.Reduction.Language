#ifndef __C_OUTPUT__
#define __C_OUTPUT__

void c_out_order_list(FUNCTION *);
void c_out_functable(CODE *, int);
void c_build_func_prot(PROGRAM *, const char *);
void c_out_data(PROGRAM *);
void build_makefile(void);
void call_makefile(void);
void clean_up(void);
int search_funcdesc(int);
int searchdata(int);

#endif
