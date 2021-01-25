#ifndef HEAP_C
#define HEAP_C

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "heap.h"
#include "stack.h"
#include "error.h"
#include "dbug.h"

heapelem * heapbase, *heapspare;
heapelem * heap;

heapelem * initheap(int heapsize)
  {
  if ((heapbase = (heapelem *)malloc(heapsize)) == NULL) {
    yyfail("can not allocate heap");
    exit(1);
    }
  if ((heapspare = (heapelem *)malloc(heapsize)) == NULL) {
    yyfail("can not allocate spare heap");
    exit(1);
    }
  heap = (heapelem *)((int)heapbase+heapsize);
  return heap;
  }

heapelem * swapheaps(int heapsize)
  {
  heapelem * pheap;

  pheap = heapbase;
  heapbase = heapspare;
  heapspare = pheap;
  return((heapelem *)((int)heapbase+heapsize));
  }

static desc * evacuate(desc * p)
  {
  desc * np;

  if ((int)p < lastcmd)
    return p;
  if (lnode(p, type) == TYreloc)
    return lreloc(p, dst);
  else {
    /* evacuate descriptor
     */
    np = _newdesc();
    *np = *p;
    lreloc(p, type) = TYreloc;
    lreloc(p, dst) = np;
    return np;
    }
  }

int gctime=0;

heapelem * gcnewdesc(int n, argstack(W), argstack(A), argstack(R), argstack(T))
  {
  void printstelem(int mode, int val);

  DBUG_ENTER("gcnewdesc");
  gctime -= clock();
  DBUG_PRINT("GC", ("heapbase=%p, heapspare=%p\n", heapbase, heapspare));
  heap = swapheaps(heapsize);
  /* copy all active structures from A stack
   */
    DBUG_PRINT("GC", ("evacuating stack S[%d]", n));
    while(!isempty(W)) {
      DBUG_PRINT("GC", ("(%p)->", (void *)top(W)));
      peek(W, -1) = (stack)evacuate((desc *)pop(W));
      }
    while(!isempty(A)) {
      DBUG_PRINT("GC", ("(%p)->", (void *)top(A)));
      peek(A, -1) = (stack)evacuate((desc *)pop(A));
      }
    while(!isempty(R)) {
      DBUG_PRINT("GC", ("(%p)->", (void *)top(R)));
      peek(R, -1) = (stack)evacuate((desc *)pop(R));
      }
    while(!isempty(T)) {
      DBUG_PRINT("GC", ("(%p)->", (void *)top(T)));
      peek(T, -1) = (stack)evacuate((desc *)pop(T));
      }
  DBUG_PRINT("GC", ("heapbase=%p, heap=%p heapspare=%p size=%d\n",
    heapbase, heap, heapspare, n));
  if ((int)heap-sizeof(desc) < (int)heapbase) {
    yyfail("heap overflow");
    exit(1);
    }
  yymessage("garbage collection completed, %d Bytes (%d%%) free",
    ((int)heap-(int)heapbase) & ~0xf, (((int)heap-(int)heapbase) & ~0xf)*100/heapsize);
  gctime += clock();
  DBUG_RETURN((heapelem *)((int)heap-sizeof(desc)));
  }

#endif /* HEAP_C */
