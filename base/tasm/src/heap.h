#ifndef HEAP_H
#define HEAP_H

#include "stack.h"
#include "mytypes.h"

typedef int heapelem;

extern heapelem * heapbase, * heapspare, * heap;
extern int heapsize;

typedef enum { TYreloc, TYshared, TYdetructive } headerid;

typedef enum { CLunknown = '0', CLnoref, CLref, CLtest } typeclass;

typedef struct desc * pdesc;

typedef struct {
  headerid   type;
  int        refcnt;
  ORDER    * prev;
  typeclass  mode;
  pdesc      newloc;
  } ST_NODE;

typedef struct {
  headerid   type;
  pdesc         dst;
  char          dummy[8];
  } ST_RELOC;


typedef struct desc {
  union {
    ST_NODE	nd;
    ST_RELOC    rel;
    } u;
  } desc;

#define freeheap()      (free(heapbase), free(heapspare))
#define newdesc()	((desc *)((_newdesc(), heap < heapbase ? heap = gcnewdesc(4, getstack(W), getstack(A), getstack(R), getstack(T)) : heap)))
#define _newdesc()	((desc *)(heap = (heapelem *)((int)heap-sizeof(desc))))
#define ndesc(t, n, m)   (newdesc(), lnode((desc *)heap, type) = (t), \
                          lnode((desc *)heap, refcnt) = (n), \
                          lnode((desc *)heap, prev) = 0, \
                          lnode((desc *)heap, mode) = m, \
                          lnode((desc *)heap, newloc) = NULL, \
                          (desc *)heap)
#define lnode(p,x)	((p)->u.nd.x)
#define rnode(p,x)	lnode(p, x)
#define lreloc(p,x)	((p)->u.rel.x)
#define rreloc(p,x)	lreloc(p, x)

#define HEAPSTAT	"%d bytes used.\n", (int)heapbase+heapsize-(int)heap

extern heapelem * initheap(int);
extern heapelem * swapheaps(int);
extern heapelem * gcnewdesc(int, argstack(W), argstack(A), argstack(R), argstack(T));

#endif /* HEAP_H */
