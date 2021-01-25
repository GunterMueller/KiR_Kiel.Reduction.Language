#ifndef _TINY
#define _TINY

#ifndef _LOCTYPES
#include "loctypes.h"
#endif

extern KNOWLEDGE know;

#ifndef DBUG_OFF
extern void stack_display();
extern void stelm_display(STELM *stelm, long i);
#endif

extern const char *prim_type_names[];
extern const char *s_prim_type_names[];
extern const char *ext_type_names[];
extern const char *s_ext_type_names[];
extern const char *other_type_names[];
extern const char *s_other_type_names[];

/************************************************************************/
/* calculates the internal value of the n given arguments on a STACK	*/
/* returns float value representing the arguments.			*/
/************************************************************************/
float calc_args_val(STELM *a,int n);

/************************************************************************/
/* searches for ord on ret-STACK and sets ret(branch) to ord(branch)	*/
/* and clears ord(branch).						*/
/************************************************************************/
void backupoldjcond();

/************************************************************************/
/* checks if function/closure stelm has to be dumped and builds up	*/
/* enviroment if needed.						*/
/************************************************************************/
void dump_list_func(STELM *stelm);

/************************************************************************/
/* checks on ret-stack for previous match with same fail-lable		*/
/* returns NULL if no previous match with same fail-label found else    */
/* returns returnadress of previous match				*/
/************************************************************************/
extern STELM *gotofail();

/************************************************************************/
/* clears functionresultentries for all depending not cleared functions */
/************************************************************************/
extern void clear_cache(FUNTYPES *ft);

/************************************************************************/
/* pops entries of RETURNADR-STACK until stelm is reached - clears all	*/
/* depending function-cache entries					*/
/************************************************************************/
extern void return_to_call(STELM *stelm);

/************************************************************************/
/* sets depend pointer in FUNLIST of des to function old_func		*/
/************************************************************************/
extern void set_depend(FUNDESC *des,STACK *w,STACK *t,STELM *old_func);

/************************************************************************/
/* displays n elements form array of TP_TYPES				*/
/************************************************************************/
extern void print_types(TP_TYPES * types, int n);

/************************************************************************/
/* searchs on ret STACK for ord						*/
/* returns NULL if ord not found, ord if no legal jcond found until ord */
/* or returns pointer to legal found jcond				*/
/************************************************************************/
extern ORDER *search_ret(ORDER *ord);

/************************************************************************/
/* searchs on ret STACK for functioncall with label "label"		*/
/************************************************************************/
extern STACK *findfcall(STELM *stelm);

/************************************************************************/
/* searchs on ret STACK for last function call				*/
/* returns STELM of ret from last function call				*/
/************************************************************************/
extern STELM *getfunc_ret();

/************************************************************************/
/* restores enviroment of function ord and cleans ret			*/
/************************************************************************/
extern void restore_ret(ORDER *ord);

/************************************************************************/
/* returns const pointer of short-type-string of type			*/
/************************************************************************/
extern const char *short_type_names(TP_TYPES tp);

/************************************************************************/
/* returns const pointer of type-string	of type				*/
/************************************************************************/
extern const char *long_type_names(TP_TYPES type);

/************************************************************************/
/* searches for object address in FUNLIST and DATA			*/
/* returns pointer to new build STELM					*/
/************************************************************************/
extern STELM *find_address(void *adr);

/************************************************************************/
/* checks for args hash                                                 */
/* returns 0 for false and 1 for true                                   */
/************************************************************************/
extern long check_hash_args(ORDER *ord);

/************************************************************************/
/* inserts args as hash in orderlist					*/
/************************************************************************/
extern void insert_args(STACK *w,long n,STACK *t,long nf,ORDER *ord);

/************************************************************************/
/* inserts restype as hash in orderlist					*/
/************************************************************************/
extern void insert_restype(STELM *res,ORDER *o);

/************************************************************************/
/* searches for matching functionresult in FUNLIST			*/
/* returns pointer to matching STELM located in FUNTYPE			*/
/* sets global variable 'know' to stored KNOWLEDGE			*/
/************************************************************************/
extern STELM *get_result(char *s,STACK *a,long n,STACK *t,long nt);

/************************************************************************/
/* frees subtypes structure						*/
/************************************************************************/
extern void free_subtype(STELM *stelm);

/************************************************************************/
/* copies subtypes to new created subtype and				*/
/* returns pointer to new subtype or NULL if empty			*/
/************************************************************************/
extern SUBTYPE *subtype_cpy(SUBTYPE *subtype);

/************************************************************************/
/* compares two subtypes of lists, removes all unmatched subtypes of 	*/ 
/* subtype one								*/
/************************************************************************/
extern void subtypecmp(SUBTYPE *st1,SUBTYPE *st2);

/************************************************************************/
/* compares two arrays of STELM	type					*/
/* returns 0 if both arrays are equal, else -1				*/
/************************************************************************/
extern int stelmcmp(STELM *stelm1,STELM *stelm2,int n);

/************************************************************************/
/* searches for function in functionlist with matching a&t frames	*/
/* returns pointer to matching funtype or NULL for no match		*/
/************************************************************************/
extern FUNTYPES *searchftypes(FUNTYPES *ft,STELM *a,STELM *t,long n,long nt); 

/************************************************************************/
/* searches for matching name in FUNLIST				*/
/* returns pointer to matching FUNLIST entry or NULL			*/
/************************************************************************/
extern FUNLIST *searchfunlist(char *name);

/************************************************************************/
/* inserts new/old functiontype in FUNLIST or modifies storage		*/
/* return int != 0 if typeclash occures					*/
/************************************************************************/
extern int insert_functype(char *name,STACK *a,long n,STACK *t,long nt,
			   STELM *res,KNOWLEDGE kn);

/************************************************************************/
/* searches FUNLIST for function named s				*/
/* returns first ORDER of function or NULL				*/
/************************************************************************/
extern ORDER *find_func(FUNCTION *fun, char *s);

/************************************************************************/
/* searches for function named label in functiondescriptorlist 		*/
/* returns pointer of FUNDESC or NULL					*/
/************************************************************************/
extern FUNDESC *find_desc(FUNDESC *des, char *label);

/************************************************************************/
/* searches for functionlabel s in given orderlist ord			*/
/* returns pointer of first order after label s				*/
/************************************************************************/
extern ORDER *find_label(ORDER *ord, char *s);

/************************************************************************/
/* allocates and inititialises new stack element			*/
/* returns pointer of new STELM						*/
/************************************************************************/
extern STELM *new_stelm(TP_TYPES tp, void *data);

/************************************************************************/
/* copies stackelements with all possible structures			*/
/************************************************************************/
extern void stelm_cpy(STELM *stelm1,STELM *stelm2,long n);

/************************************************************************/
/* frees stackelements with all possible substructures			*/
/************************************************************************/
extern void stelm_free(STELM *stelm);

/************************************************************************/
/* frees stack with all elements 					*/
/************************************************************************/
extern void st_free(STACK *st);

/************************************************************************/
/* builds a stack with a copy of the first n elements of st		*/
/* returns new allocated an initialiesed STACK				*/
/************************************************************************/
extern STACK *st_make(STACK *st, long n);

/************************************************************************/
/* pops elemet top of stack st						*/
/* returns pointer of STELM top of stack st				*/
/************************************************************************/
extern STELM *st_pop(STACK *st);

/************************************************************************/
/* reads stack element number i counted form top of stack st		*/
/* returns pointer of STELM i counted form top of stack st		*/
/************************************************************************/
extern STELM *st_read(STACK *st, long i);

/************************************************************************/
/* pushes STELM vt on top of STACK st					*/
/* returns pointer of STACK st (may be changed)				*/
/************************************************************************/
extern STACK *st_push(STACK *st, STELM *vt);

/************************************************************************/
/* converts hexadecimal number placed in a string into integer		*/
/* returns converted int						*/
/************************************************************************/
extern int atox(const char *s);

/************************************************************************/
/* searches for '9876543210_' in given string search			*/
/* returns pointer of first char befor '9876543210_'			*/
/************************************************************************/
extern char *searchforfuncname(char *search);

/************************************************************************/
/* searches for '_' in given string search				*/
/* returns pointer of first char behind '_'				*/
/************************************************************************/
extern char *searchfor_(char *search);

/************************************************************************/
/* executes primitiv function primf					*/
/************************************************************************/
extern void exe_prim1(PRIMF primf);
extern void exe_prim2(PRIMF primf);
extern void exe_prim3(PRIMF primf);
extern void exe_prim4(PRIMF primf);

#endif