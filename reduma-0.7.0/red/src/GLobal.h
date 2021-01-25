#ifndef _GLobal_h
#define _GLobal_h

typedef int INT4;
typedef int BOOLEAN;

#define TRUE    1
#define FALSE   0

typedef char * STRPTR;
typedef int (* FUNPTR)();

#define NULLFKT (FUNPTR)0
#define NULLSTR (STRPTR)0

#endif /* _GLobal_h */
