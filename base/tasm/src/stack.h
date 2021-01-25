#ifndef STACK_H
#define STACK_H

#include <malloc.h>

typedef int stack;
/* defines a new stack (also used in function headers) */
#define astack(st)      stack ** base_ ## st, * mem_ ## st = NULL, * st_ ## st
/* used to pass a stack as argument */
#define getstack(st)    (base_ ## st), (st_ ## st)              /* calling function */
#define vargstack(st, ap) (base_ ## st) = va_arg(ap, stack **),                         \
                        (st_ ## st) = va_arg(ap, stack *),                              \
                        (mem_ ## st) = NULL                /* pass stack as var-arg */
#define argstack(st)    stack ** base_ ## st, stack * st_ ## st /* function header  */

#define initstack(st, stacksize) ( mem_ ## st = (stack *)malloc(stacksize*sizeof(stack)),\
                                  (mem_ ## st)[0] = stacksize,                           \
                                  (mem_ ## st)[1] = BOTTOM,                              \
                                  (mem_ ## st)[stacksize-1] = BOTTOM,                    \
                                  base_ ## st = &(mem_ ## st),                           \
                                  st_ ## st = &((mem_ ## st)[1]))
#define reinitstack(st, stsize)  ( *(base_ ## st) = (stack *)realloc(*(base_ ## st), (stsize+STACKSIZE)*sizeof(stack)),                                                              \
                                  (*(base_ ## st))[0] += STACKSIZE,                      \
                                  st_ ## st = &(*(base_ ## st))[stsize-1],                \
                                  (*(base_ ## st))[(*(base_ ## st))[0]-1] = BOTTOM)
#define freestack(st)            free(mem_ ## st)


#define push(st,x)	((*++(st_ ## st) == BOTTOM ? (yywarn("stack " #st " reallocated"), \
                           reinitstack(st, (*(base_ ## st))[0])) : 0),                     \
                         (*(st_ ## st) = (x)))
#define peek(st, n)	(st_ ## st)[-(n)]
#define set(st, n, x)	(st_ ## st)[-(n)] = (x)
#define top(st)		peek(st, 0)
#define drop(st, n)	((st_ ## st) -= n)
#define pop(st)		(*(st_ ## st)--)
#define isempty(st)	(top(st)==BOTTOM)
/* remove used base_* warning
 */

#define BOTTOM          (stack)87654321
#define STACKSIZE       16384

extern int stacksize;
#endif /* STACK_H */
