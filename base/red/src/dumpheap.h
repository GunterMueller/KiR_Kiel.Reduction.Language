/* $Log: dumpheap.h,v $
 * Revision 1.1  1992/11/04 18:12:30  base
 * Initial revision
 * */
/* file dumpheap.h, last change 12.12.91 by dg */

/* --- vorklassifizierung der heapelemente --- */

#define HP_DATA  0
#define HP_DESC  1
#define HP_PTH   2
#define HP_NEXT  3
#define HP_REF   4
#define HP_ATOM  5
#define HP_PTD   6
#define HP_CODE  7
#define HP_INSTR 8
#define HP_INT   9
#define HP_BOOL  10

/* --- symbolische konstanten --- */

#define HEAPBASE     0
#define HEAPALIGNB   1
#define HEAPALIGNW   4

#define HARDWARETAGS 0
#define REDUMATAGS   1
#define OTHERTAGS    3

#define HARDWARESTRING "hardware"
#define REDUMASTRING   "reduma"
#define UNKNOWNSTRING  "unknown"

/* --- globale variablen aus rheap.c --- */

extern T_DESCRIPTOR *deslist;                     /* descriptor chain */
extern T_DESCRIPTOR *StHeapD;                     /* initial ptr *global */
extern T_DESCRIPTOR *EnHeapD;                     /* arena top *global */
extern T_HBLOCK     *hpbase;                      /* initial arena */
extern T_HBLOCK     *hplast;                      /* last heappointer */
extern T_HBLOCK     *hpend;                       /* arena top */
extern T_DESCRIPTOR *_nil;                        /* nil pointer */

/* --- makros --- */

#define POST_MORTEM(str)  post_mortem(str)
#define WARNING(str)      fprintf(stderr,"\n Warning: %s. Wait ... ",str), sleep(2)
#define PNEXTFREE(ptd)    ((int *)(ptd) + 3)
#define NEXTFREE(ptd)     *PNEXTFREE(ptd) 
#define HEAP_INDEX(p)     ((T_HEAPELEM *)(p) - (T_HEAPELEM *)hpbase)
 
/* umrechnung in hardware (oder auch andere) adressen */

#define HW_PTH(p)         (HeapBase + (((int)(p) - (int)hpbase) / HeapAlign))
#define HW_PTD(p)         (HeapBase + (((int)(p) - (int)hpbase) / HeapAlign))

#define HW_ATOM(x)        T_INT(x) ? VAL_INT(x) : T_SA_TRUE(x) ? 1 : T_SA_FALSE(x) ? 0 : (x)

/* umrechnung in reduma adressen */

#define RED_PTH(p)        ((PTR_HEAPELEM)((int)hpbase + (((int)p - (int)HeapBase) * HeapAlign)))
#define RED_PTD(p)        ((T_PTD)((int)hpbase + (((int)p - (int)HeapBase) * HeapAlign)))

/* end of file */

