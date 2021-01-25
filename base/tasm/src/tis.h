#ifndef _TIS
#define _TIS

#ifndef _LOCTYPES
#include "loctypes.h"
#endif

extern STELM *exe_func();
extern void execution();
extern int tis();

extern FUNLIST  *flbase;
extern FUNDESC  *funpointer;
extern STELM    CONST_NONE;
extern int	do_dupe,dup_count,dup_num,max_desc_adr;
extern ORDER 	*ord;
extern ORDER 	CONST_EXT;
extern STACK 	*func_dump;
extern STACK	*w,*a,*t,*r,*ret;
extern int	new_params,clos_exe,arg_check;

#define DBUG_ORDERS(str)	DBUG_PRINT("ORDERS",(str,			\
					(NULL==w) ? NULL : GETTOPOFSTACK(w),	\
			   		(NULL==a) ? NULL : GETTOPOFSTACK(a),	\
			   		(NULL==r) ? NULL : GETTOPOFSTACK(r),	\
			   		(NULL==t) ? NULL : GETTOPOFSTACK(t),	\
			   		(NULL==ret) ? NULL : GETTOPOFSTACK(ret)))


#endif