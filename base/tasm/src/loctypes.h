#ifndef _LOCTYPES
#define _LOCTYPES

#ifndef _MYTYPES
#include "mytypes.h"
#endif

/* an stack is array of char with length in the first 4 bytes (long)	*/
/* and top of stack in the second 4 bytes (long)			*/
#define SZ_LONG		sizeof(long)
#define MAX_LINE_LENGTH	120
#define MIN_STACK_SIZE	16

/* status for dump functions */
#define CLOS_EXE	1
#define NEW_PARAMS	2
#define MUST_EXE	4

#define BETAFUNCS(x)	(						\
                          (						\
			    (						\
			      (beta==(x)) || (tail==(x))		\
			    ) || 					\
                            (						\
			      (gammabeta==(x)) || (apply==(x))		\
			    )						\
			  ) || 						\
			  (						\
			    (						\
			      (Case==(x)) || (gammacase==(x)) 		\
			    ) ||					\
			    (Gamma==(x))				\
			  )						\
                        )
#define JCONDX(x)	(						\
                          (						\
                            (						\
			      (jcond==(x)) || (jcond2==(x))		\
			    ) ||					\
                            (						\
			      (jfalse==(x)) || (jfalse2==(x))		\
			    )						\
			  ) ||						\
  			  (						\
			    (jtrue==(x)) || (jtrue2==(x))		\
			  )						\
                        )
#define MATCHX(x)	(						\
                          (						\
			    (						\
			      (						\
			        (matcharb==(x)) || (matchlist==(x))	\
			      ) ||					\
  			      (						\
			        (tguard==(x)) || (matchint==(x))	\
			      )						\
			    ) || 					\
			    (						\
			      (						\
			        (matcharbs==(x)) || (matchbool==(x))	\
			      ) ||					\
			      (						\
			        (matchin==(x)) || (matchprim==(x))	\
			      )						\
			    )						\
			  ) || 						\
                          (matchstr==(x))				\
                        )
#define JCONDS(x)	( 						\
                          ( 						\
			    JCONDX(x)					\
			  ) ||						\
                          ( 						\
			    MATCHX(x)					\
			  )						\
  			)
  
#define GETSTELMTYPE(x)		((x)->type)
#define SETSTELMTYPE(x) 	GETSTELMTYPE(x)
#define GETSTELMDATA(x,data)	((x)->dat.data)
#define SETSTELMDATA(x,data)	GETSTELMDATA(x,data)
#define GETTOPOFSTACK(x)	((x)->topofstack)
#define SETTOPOFSTACK(x)	GETTOPOFSTACK(x)
#define GETSTACKENTRY(x,e)	((x)->e)
#define SETSTACKENTRY(x,e)	GETSTACKENTRY(x,e)
#define GETTOPELM(x)		((x)->stack[GETTOPOFSTACK(x)-1]
#define SETTOPELM(x)		GETTOPELM(x)

typedef enum { kn_no, kn_partly, kn_depend, kn_sure,
	       kn_clear				/* needed for dependencies */
	     } KNOWLEDGE;

#define VTP_NONE	4.0
#define VTP_INT		0.1
#define VTP_BOOL	0.1
#define VTP_REAL	0.4
#define VTP_PRIM	0.8
#define VTP_FUNC	1.0
#define VTP_CLOS	2.5
#define VTP_LIST	1.4
#define VTP_STRING	1.1
#define VTP_VECT	1.0
#define VTP_MATRIX	1.2
#define VTP_ATOM	1.1
#define VTP_VAR		1.4

struct tagfuntypes;
struct tagfunlist;
struct tagstelm;
struct tagstack;
struct tagclosure;
struct tagreturnadr;
struct tagprimlist;
struct tagsubtype;
struct tagdepend;

typedef struct tagsubtype
  {
  TP_TYPES type;
  struct tagsubtype *next;
  } SUBTYPE;

typedef struct tagprimlist
  {
  PRIMF prim;
  struct tagprimlist *next;
  } PRIMLIST;

typedef struct tagreturnadr
  {
  ORDER *ord;
  struct tagstack *w,*a,*t,*r;
  int	status,branch;
  } RETURNADR;

typedef struct tagclosure
  {
  FUNDESC *desc;				/* if desc==NULL then primfunc */
  int i,j,k;
  struct tagstelm *w;
  struct tagstelm *t;
  } CLOSURE;

typedef struct tagstelm
  {
  TP_TYPES type;
  SUBTYPE subtype;
  union
    {
    FUNDESC *desc;
    struct tagclosure *clos;
    PRIMF prim;
    RETURNADR *ret;
    } dat;
  } STELM;

typedef struct tagstack
  {
  long topofstack;
  long length;
  STELM *stack;
  } STACK;

typedef struct tagdepend
  {
  struct tagfuntypes *func;
  struct tagdepend   *next;
  } DEPEND;

typedef struct tagfuntypes
  {
  STELM *params;
  STELM *tparams;
  STELM result;
  KNOWLEDGE knowledge;
  FUNDESC *des;
  DEPEND *depend;
  ORDER *ord;
  char *label;
  struct tagfuntypes *next;	
  } FUNTYPES;

typedef struct tagfunlist
  {
  char *name;					/* function label */
  int a,t;					/* no. elements on a,t stack */
  FUNTYPES *funtypes;				/* all types of this func */
  struct tagfunlist *org;			/* original funlistentry */
  struct tagfunlist *next;
  } FUNLIST;

#endif