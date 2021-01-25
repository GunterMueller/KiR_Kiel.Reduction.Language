/******************************************************* cportab.h     ****/
/* Symbolische Werte                                                      */
#define NO	0
#define FAIL	0
#define FALSE	0
/* #define NULL	0	*/
#define YES	1
#define OKAY	1
#define TRUE	1
#define TRUE2	2
#define ERROR	(-1)
#define NOTFOUND	(-1)
#define SUCCESS		0
#define FAILURE		(-1)
#define NULLPTR		((char *)0)
#define NULLFILE	((FILE *)0)
#define NILCHAR		((char *)(-1))
#if UNIX
#define VOID	void
#else
#define VOID	/* */
#endif

#define FOREVER for	( ; ; )

#define WORD    short           /*	16 bit integer !     */

#ifdef __STDC__
#else
extern char *strcpy();
extern exit();
#endif

/* end of cportab.h */
