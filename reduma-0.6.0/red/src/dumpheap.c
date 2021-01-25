/* $Log: dumpheap.c,v $
 * Revision 1.4  1994/01/18  08:31:23  mah
 * warning fix (gcc 2.5.6)
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:48:53  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* file dumpheap.c, last change: 25.03.92 by dg */

#include <stdio.h>
#include <string.h>                  /* RS 02/11/92 */ 
#include <stdlib.h>                   /* RS 02/11/92 */ 
#if (!APOLLO)
#include <malloc.h>                  /* ach 02/11/92 */
#endif
#include "rstdinc.h"
#include "rstelem.h"
#include "rheapty.h"
#include "rstackty.h"
#include "rinter.h"
#include "tags.h"
#include "dumpheap.h"
#include "rscavenge.h"

#define VERSION 1

/* --- makros --- */

#define RED_TAG(x,t)     (x) ? T_POINTER(x) ? HP_PTD : HP_ATOM : HP_DATA

#define IS_HW_PTR(x,t)   ((t) &  HW_POINTER_BIT)
#define IS_HW_INT(x,t)   ((t) == HW_INT_TAG)
#define IS_HW_BOOL(x,t)  ((t) == HW_BOOL_TAG)

#define HW_RED_CONV(x,t) IS_HW_PTR(x,t) ? HP_PTD : IS_HW_INT(x,t) ? HP_INT : IS_HW_BOOL(x,t) ? HP_BOOL : HP_ATOM
#define RED_CONVERT(x,t) (TagSystem == HARDWARETAGS) ? HW_RED_CONV(x,t) : RED_TAG(x,t)

#if ! SCAVENGE
#define CHECK_HEAP(p)    do {if ((int)(p) < (int)hpbase || (int)(p) > (int)hpend) { \
                           sprintf(buf,"write/read_heap (%s, line %d): invalid heap pointer %0x ",__FILE__,__LINE__,(unsigned int)p); \
                           fprintf(stdout,"\n %s. press CR to continue, other key to break ",buf); fflush(stdout); \
                           if (getchar() != '\n') POST_MORTEM(buf); \
                         }} while(0)
#define CHECK_DESC(p)    do {if ((int)(p) < (int)StHeapD || (int)(p) > (int)EnHeapD) { \
                           sprintf(buf,"write/read_heap (%s, line %d): invalid descriptor pointer %0x",__FILE__,__LINE__,(unsigned int)p); \
                           fprintf(stdout,"\n %s. press CR to continue, other key to break ",buf); fflush(stdout); \
                           if (getchar() != '\n') POST_MORTEM(buf); \
                         }} while (0)
#else
#define CHECK_HEAP(p)    do {if ((int)(p) < (int)hpbase || (int)(p) > (int)rs_end) { \
                           sprintf(buf,"write/read_heap (%s, line %d): invalid heap pointer %0x ",__FILE__,__LINE__,(unsigned int)p); \
                           fprintf(stdout,"\n %s. press CR to continue, other key to break ",buf); fflush(stdout); \
                           if (getchar() != '\n') POST_MORTEM(buf); \
                         }} while (0)
#define CHECK_DESC(p)    CHECK_HEAP(p)
#endif /* SCAVENGE auskommentiert RS 30.11.1992 */

/* --- makros, nur innerhalb von write_heap, read_heap, hw_val gueltig ! */

/* HW_PTH(p),HW_PTD(p),RED_PTH(p),RED_PTD(p) u.a. makros sind in dumpheap.h definiert */

#define MARK_HEAP0(p,c)  do {if (p) { \
                           PTR_HEAPELEM php = p; \
                           int i = c; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           for (; --i >= 0 ;) \
                             ptg[i] = RED_TAG(php[i],ptg[i]); \
                         }} while(0)
#define MARK_HEAP1(p)    do {if (p) { \
                           PTR_HEAPELEM php = p; \
                           int i = *php++; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           for (; --i >= 0 ;) \
                             ptg[i] = RED_TAG(php[i],ptg[i]); \
                           ptg[i] = HP_DATA; \
                         }} while(0)
#define MARK_HEAP2(p,c)  do {if (p) { \
                           PTR_HEAPELEM php = p; \
                           int i = c; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           for (; --i >= 0 ;) \
                             ptg[i] = HP_DATA; \
                         }} while(0)

#define MARK_HW_HEAP0(p,c) do {if (p) { \
                           PTR_HEAPELEM php = RED_PTH(p); \
                           int i = c; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           if (TagSystem == HARDWARETAGS) { \
                             for (; --i >= 0 ;) \
                               ptg[i] = HW_RED_CONV(php[i],ptg[i]); \
                           } else { \
                             for (; --i >= 0 ;) \
                               ptg[i] = RED_TAG(php[i],ptg[i]); \
                           } \
                         }} while(0)
#define MARK_HW_HEAP1(p) do {if (p) { \
                           PTR_HEAPELEM php = RED_PTH(p); \
                           int i = *php++; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           if (TagSystem == HARDWARETAGS) { \
                             for (; --i >= 0 ;) \
                               ptg[i] = HW_RED_CONV(php[i],ptg[i]); \
                           } else { \
                             for (; --i >= 0 ;) \
                               ptg[i] = RED_TAG(php[i],ptg[i]); \
                             ptg[i] = HP_DATA; \
                           } \
                         }} while(0)
#define MARK_HW_HEAP2(p,c) do {if (p) { \
                           PTR_HEAPELEM php = RED_PTH(p); \
                           int i = c; \
                           unsigned char *ptg = tagheap + HEAP_INDEX(php); \
                           CHECK_HEAP(php); \
                           for (; --i >= 0 ;) \
                             ptg[i] = HP_DATA; \
                         }} while(0)

/* --- definitionen --- */

#define BUFLEN 128

#define ASCII_FORMAT  0
#define BINARY_FORMAT 1
#define CAST_FORMAT   2

#define TAGSYSTEM(TagSysName) (!strcmp(TagSysName,HARDWARESTRING)) ? HARDWARETAGS : \
                              (!strcmp(TagSysName,REDUMASTRING))   ? REDUMATAGS   : \
                              OTHERTAGS
#define TAGSYSNAME(TagSystem) (TagSystem == HARDWARETAGS) ? HARDWARESTRING : \
                              (TagSystem == REDUMATAGS)   ? REDUMASTRING   : \
                              UNKNOWNSTRING

/* --- lokale variablen + initialisierungen --- */

static int  TagSystem              = HARDWARETAGS;   /* hardware tag system */
static int  OutputFormat           = ASCII_FORMAT;   /* binary, ascii */
static int  InputFormat            = ASCII_FORMAT;   /* binary, ascii */

/* --- externe variablen --- */

extern char TargetDir[BUFLEN]      ;  /* target directory for load_heap */
extern char HeapDumpFile[BUFLEN]   ;  /* no output */
extern char HeapLoadFile[BUFLEN]   ;  /* no input */
extern char TestDumpFile[BUFLEN]   ;  /* no test-output */
extern char HeapDumpFormat[BUFLEN] ;  /* format: ascii, binary, cast */
extern char HeapLoadFormat[BUFLEN] ;  /* format: ascii, binary, cast */
extern char ExecShell[BUFLEN]      ;  /* no shell to execute (in place of inter) */
extern char TagSysName[BUFLEN]     ;  /* hardware tag system */
extern int  HeapBase               ;  /* start at HEAPBASE (=0) */
extern int  HeapAlign              ;  /* byte alignment */

#if SCAVENGE
extern word *sta_adr, *sta_free;
extern word *ca_adr, *pss_adr, *fss_adr, *os_adr;
extern word *ca_free, *pss_free, *fss_free, *os_free;
extern word *ca_end, *pss_end, *fss_end, *os_end, *rs_end, *ns_end;
extern word *rs_adr; 
extern word *rs_free;
extern int maxage;

#endif /* SCAVENGE   auskommentiert RS 30.11.1992 */ 

/* --- externe funktionen (aus wb's backend) --- */

extern int *get_table();        /* wb, anfang der tabellen (dg: rdummies.h) */
extern int get_tablen();        /* wb, laenge der tabellen (dg: rdummies.h) */

/* --- sonstige externe variablen/funktionen --- */

extern StackDesc  S_a;          /* rstack.c */
extern INSTR_DESCR instr_tab[]; /* rinter.h */
extern T_PTD       code_desc;   /* rear.c */
extern T_CODEDESCR codedesc;    /* rear.c */
extern char *instrname();       /* rinter.h */
extern char *stelname();        /* rprimf.h */
extern int  _redcnt;            /* r?.c */
extern char *_errmes;           /* <r>, filename des editors */

extern FILE *fopen();
/* extern char *calloc();  */

/* RS 02/11/92 */ 
int write_heap();
int read_heap();
extern post_mortem();
extern int get_setup();            /* setup.c */
extern int instrindex();           /* aeh hihihi */
/* END of RS 02/11/92 */ 

extern int _filbuf();       /* RS 30.11.1992 */
extern void cfree();        /* mah 18.01.94  */

#if DEBUG
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern void writestack(); /* TB, 4.11.1992 */      /* rruntime.c */
#endif

/* --- globale funktionen (fuer wb's backend) --- */

#ifdef _ANSI_              /* ach 01/02/93 */
 
int newp(int p)  { return(HW_PTH(p)); }   
 
#else
 
int newp(p)
int p;
 { return(HW_PTH(p)); }   
 
#endif

int get_hpbase()   { return(HW_PTH(hpbase)); }
int get_hplast()   { return(HW_PTH(hplast)); }
int get_deslist()  { return(HW_PTD(deslist)); }
int get_nilreg()   { return(HW_PTD(_nil)); }

int get_tabstart()
{
#if SCAVENGE
   return(HW_PTH(rs_end));
#else
   return(HW_PTH(EnHeapD+1));
#endif /* SCAVENGE   auskommentiert RS 30.11.1992 */ 
}

/* ------------------------------------------------------------------------------- */
/* --- hw_ptr: rechnet reduma-heap-pointer in hardware- (o.a.) heap-pointer um --- */
/* ------------------------------------------------------------------------------- */

int hw_ptr(ptr,HeapBase,HeapAlign)
int *ptr;                       /* zeiger in (reduma) heap */
int  HeapBase;                  /* basisadresse (hierhin wird heap verschoben) */
int  HeapAlign;                 /* byte- (1) oder wort (4) -adressen */
{
  return(HW_PTH(ptr));
}

/* --------------------------------------------------------------------------- */
/* --- hw_val: rechnet reduma-stack-elemente in hardware-stack-elemente um --- */
/* --------------------------------------------------------------------------- */

int hw_val(x,HeapBase,HeapAlign)
int x;
int  HeapBase;
int  HeapAlign;
{
  if (x && T_POINTER(x)) return(HW_PTD(x));
  if (T_INT(x))          return(HW_ATOM(x));
  else                   return(x);
}

/* --------------------------------------------------------------------------- */
/* --- hw_tag: berechnet hardware-tags aus reduma-stack-elementen          --- */
/* --------------------------------------------------------------------------- */

int hw_tag(x)
int x;
{
  if (x && T_POINTER(x)) {
    switch (R_DESC(*(T_PTD)x,class)) {
      case C_LIST:
             if (R_LIST(*(T_PTD)x,dim) == 0) return(HW_NIL_TAG);
             else                            return(HW_NLIST_TAG);
      case C_FUNC:
             switch (R_DESC(*(T_PTD)x,type)) {
               case TY_COMB:  return(HW_COMB_TAG);
               case TY_CONDI: return(HW_CONDI_TAG);
               case TY_CLOS:  return(HW_CLOS_TAG);
               case TY_CASE:  return(HW_CASE_TAG);
               default:       return(HW_OTHP_TAG);
             }
      case C_SCALAR:          return(HW_REAL_TAG);
      default:                return(HW_OTHP_TAG);
    }
  }
  else {
    if (T_INT(x))     return(HW_INT_TAG);
    if (T_BOOLEAN(x)) return(HW_BOOL_TAG);
    if (T_FUNC(x))    return(HW_PRIM_TAG);
    else              return(HW_OTHA_TAG);
  }
}

/* ----------------------------------------------------------------------- */
/* --- init_print_heap: initialisierungen fuer print_heap              --- */
/* ----------------------------------------------------------------------- */

int init_print_heap()                    /* int eingefuegt von RS 02/11/92 */ 
{
  /* setups are read in by readsetups() (setup.c) */

  /* test ob "/" am ende von Targetdir steht */
  if (TargetDir[strlen(TargetDir)-1] != '/')
    strcat(TargetDir,"/");

  TagSystem = TAGSYSTEM(TagSysName);
  OutputFormat = !strcmp(HeapDumpFormat,"binary") ? BINARY_FORMAT :
                 !strcmp(HeapDumpFormat,"cast")   ? CAST_FORMAT : ASCII_FORMAT;
  InputFormat  = !strcmp(HeapLoadFormat,"binary") ? BINARY_FORMAT :
                 !strcmp(HeapLoadFormat,"cast")   ? CAST_FORMAT : ASCII_FORMAT;
  return (0);             /* RS 02/11/92 */ 
}

/* ----------------------------------------------------------------------- */
/* --- print_heap: schreibt den heap raus (mit standard-einstellungen) --- */
/* ----------------------------------------------------------------------- */

void print_heap(heapdumpfile)  /* void eingefuegt von RS 02/11/92 */ 
char *heapdumpfile;
{
  if (heapdumpfile) {
    /* heap in angegebene datei schreiben */
    write_heap(heapdumpfile,HeapBase,HeapAlign,TagSystem,get_table(),get_tablen());
  }
  else {
    /* falls HeapDumpFile oder TargetDir == /dev/null aussteigen */
    if (!strncmp(TargetDir,"/dev/null",BUFLEN))
      return;
    if (!strncmp(HeapDumpFile,"/dev/null",BUFLEN))
      return;
  
    if (!strncmp(HeapDumpFile,"<editor-file>",BUFLEN)) {
      /* dateinamen von editor (aus _errmes) nehmen */
      sprintf(HeapDumpFile,"%s%s",TargetDir,_errmes);
    }
  
    write_heap(HeapDumpFile,HeapBase,HeapAlign,TagSystem,get_table(),get_tablen());
  }
}

/* ----------------------------------------------------------------------- */
/* --- load_heap: liest den heap ein (mit standard-einstellungen)      --- */
/* ----------------------------------------------------------------------- */

void load_heap(heaploadfile)            /* void eingefuegt von RS 02/11/92 */ 
char *heaploadfile;
{
  if (heaploadfile) {
    /* heap einlesen aus angegebener datei */
    strncpy(HeapLoadFile,heaploadfile,BUFLEN);
  }

  /* falls HeapLoadFile oder TargetDir == /dev/null aussteigen */
  if (!strncmp(TargetDir,"/dev/null",BUFLEN))
    return;
  if (!strncmp(HeapLoadFile,"/dev/null",BUFLEN))
    return;

  if (!strncmp(HeapLoadFile,"<editor-file>",BUFLEN)
  ||  !strncmp(HeapLoadFile,"editor-file",BUFLEN)) {
    /* dateinamen von editor (aus _errmes) nehmen */
    sprintf(HeapLoadFile,"%s%s",TargetDir,_errmes);
  }

  /* shell wird per system-call ausgefuehrt, z.b. aufruf von CAST */
  if (strncmp(ExecShell,"none",BUFLEN) && strncmp(ExecShell,"-",BUFLEN)) {
    strcat(ExecShell," ");
    /* strcat(ExecShell,HeapLoadFile); */ /* vorl"aufig stets editorfile einlesen */
    strcat(ExecShell,TargetDir);
    strcat(ExecShell,_errmes);
    system(ExecShell);
  }

  /* (modifizierten heap einlesen */
  read_heap(HeapLoadFile,HeapBase,HeapAlign,TagSystem,(int *)0,0);

#ifndef NotUsed
  /* nur fuer testzwecke */
  get_setup(" TestDumpFile : %s", TestDumpFile,"/dev/null");
  if (strncmp(TestDumpFile,"/dev/null",BUFLEN)) {
    /* OutputFormat = ASCII_FORMAT; */
    write_heap(TestDumpFile,HeapBase,HeapAlign,TagSystem,get_table(),get_tablen());
  }
#endif /* NotUsed   auskommentiert RS 30.11.1992 */  
}

/* ----------------------------------------------------------------------- */
/* --- write_heap: schreibt den heap raus                              --- */
/* ----------------------------------------------------------------------- */

int write_heap(HeapDumpFile,HeapBase,HeapAlign,TagSystem,TabStart,TabLen)
      /* int eingefuegt von RS 02/11/92 */
char *HeapDumpFile;          /* ausgabedatei */
int  HeapBase;               /* basisadresse (hierhin wird heap verschoben) */
int  HeapAlign;              /* byte- (1) oder wort (4) -adressen */
int  TagSystem;              /* tagsystem: reduma, hardware */
int  *TabStart;              /* tabellenanfang */
int  TabLen;                 /* tabellenlaenge */
{
  int                  i,cnt;
  unsigned             len;
  T_DESCRIPTOR         *ptd;
  T_HEAPELEM           *pth;
#if ! SCAVENGE                       /* #if #endif eingefuegt von RS 6.11.1992 */ 
  T_HBLOCK             *ptb;
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  
  char                 buf[BUFLEN];
  unsigned char        *taghp,*ptt;
  static unsigned char *tagheap = NULL;
  static FILE          *fp = NULL;

#if    SCAVENGE
  /* POST_MORTEM("scavenge and dumpheap conflict!"); */
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  

  if (HeapBase < 0) {
    HeapBase = (int)hpbase;  /* no relocation */
  }

  if (HeapAlign != HEAPALIGNB && HeapAlign != HEAPALIGNW) {
    /* 1: byte- 4: word-addressing */
    POST_MORTEM("write_heap: HeapAlign not in {1,4}");
  }

  if (HeapBase % (HEAPALIGNW/HeapAlign) != 0) {
    POST_MORTEM("write_heap: HeapBase is misaligned");
  }

#if SCAVENGE
  len = HEAP_INDEX(rs_end);
#else
  if ((HW_PTD(StHeapD) % sizeof(T_DESCRIPTOR)) != 0) {
    POST_MORTEM("write_heap: HeapBase (StHeapD) is misaligned");
  }

  /* length of heap (words) */
  len = HEAP_INDEX(EnHeapD+1);
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  

  /* allocate tag vector */
/*  if (tagheap) cfree((char *)tagheap);       ach 02/11/92 */
  if (tagheap) free((char *)tagheap);
  if ((tagheap = (unsigned char *)calloc(len,sizeof(*tagheap))) == NULL) {
    POST_MORTEM("write_heap: calloc failed");
  }

  /* initialize tag vector */
  for (cnt = len ; --cnt >= 0 ;)
    tagheap[cnt] = HP_DATA;

#if ! SCAVENGE
  /* run through heap chain */
  ptb = hpbase;                                     /* heap start */
  do {
    tagheap[HEAP_INDEX(&ptb->next)] = HP_NEXT;      /* heapblock */
    tagheap[HEAP_INDEX(&ptb->ref)] =
      (ptb->ref == 0 || (int)ptb->ref == -1) ? HP_DATA : HP_REF;
  } while ((ptb = ptb->next) > hpbase);
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  

  /* run through code area (resides in the heap) */
  for (pth = CODEVEC; *pth != I_SYMTAB; ) {
    int i = instrindex(*pth);
    tagheap[HEAP_INDEX(pth)] = HP_INSTR;
    for (cnt = 1 ; cnt <= instr_tab[i].nparams; ++cnt) {
      switch (instr_tab[i].paramtype) {
        case DESC_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_PTD;
          break;
        case ADDR_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_PTH;
          break;
        case ATOM_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_ATOM;
          break;
        case FUNC_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_ATOM;
          break;
      }
    }
    pth += (1 + instr_tab[i].nparams);
  }

#if ! SCAVENGE
  /* check descriptor freelist */
  for (ptd = deslist ; ptd != NULL ; ptd = (T_PTD)NEXTFREE(ptd)) {
    if (R_DESC(*ptd,ref_count) != 0) {
      WARNING("write_heap: invalid freelist. I'll fix it");
      L_DESC(*ptd,ref_count) = 0;
    }
  }

  /* run through descriptor area */
  taghp = tagheap + HEAP_INDEX(StHeapD);            /* beginn descriptor tags */
  ptd = StHeapD;
  while (ptd <= EnHeapD) {
    *taghp++ = HP_DESC;                             /* descriptor header */
    if (R_DESC(*ptd,ref_count) == 0) {              /* in der freiliste */
      *taghp++ = HP_DATA;                           /* irgendwas */
      *taghp++ = HP_DATA;                           /* irgendwas */
      *taghp++ = NEXTFREE(ptd) ? HP_PTD : HP_DATA;  /* next free */
      ptd++;     /* RS 24.5.1993 */
    }
    else {
#else
  /* scavenge: run through remembered set */
  for (pth = (PTR_HEAPELEM)rs_adr; pth < (PTR_HEAPELEM)rs_end; pth++ ) {
    tagheap[HEAP_INDEX(pth)] = (*pth) ? HP_PTH : HP_DATA;
  }
  /* run through static area; descriptors and blocks are mixed up */
  ptd = (T_PTD)sta_adr;
  while (ptd < (T_PTD)sta_free) {
    if (SCAV_IS_BLOCK(ptd)) {
      /* zeiger auf naechsten block/descriptor */
      pth = (PTR_HEAPELEM)ptd + ((PBLOCK)ptd)->over;
      ptd = (T_PTD)pth;
      continue;
    }
    else {
      /* jetzt ham wer'n descriptor (nau wi h"awa descriptor) */
      taghp = tagheap + HEAP_INDEX(ptd);            /* beginn descriptor tags */
      *taghp++ = HP_DESC;                           /* descriptor header */
#endif
      switch (R_DESC(*ptd,class)) {
        case C_LIST:
          *taghp++ = HP_DATA;                          /* special|dim */
          *taghp++ = R_LIST(*ptd,ptdd)?HP_PTD:HP_DATA; /* ptdd */
          *taghp++ = R_LIST(*ptd,ptdv)?HP_PTH:HP_DATA; /* ptdv */
          if (!R_LIST(*ptd,ptdd)) {
            MARK_HEAP0(R_LIST(*ptd,ptdv),R_LIST(*ptd,dim));
          }   
          break;
          case C_CONS:
            *taghp++ = RED_TAG(R_CONS(*ptd,hd),*taghp); /* head */
            *taghp++ = RED_TAG(R_CONS(*ptd,hd),*taghp); /* tail */
            *taghp++ = HP_DATA;                         /* not used */
            break;
        case C_EXPRESSION:
          switch (R_DESC(*ptd,type)) {
#if WITHTILDE
	    case TY_SNSUB:
#endif /* WITHTILDE */
            case TY_SUB:
              *taghp++ = HP_DATA;                   /* special|nargs */
              *taghp++ = HP_PTH;                    /* namelist */
              *taghp++ = HP_PTH;                    /* pte */
              MARK_HEAP1(R_FUNC(*ptd,namelist));
              MARK_HEAP1(R_FUNC(*ptd,pte));
              break;
            case TY_LREC:
              *taghp++ = HP_DATA;                  /* nfuncs */
              *taghp++ = HP_PTH;                   /* namelist */
              *taghp++ = HP_PTH;                   /* ptdv */
              MARK_HEAP1(R_LREC(*ptd,namelist));
              MARK_HEAP0(R_LREC(*ptd,ptdv),R_LREC(*ptd,nfuncs));
              break;
            case TY_LREC_IND:
              *taghp++ = HP_DATA;                  /* index */
              *taghp++ = HP_PTD;                   /* ptdd */
              *taghp++ = RED_TAG(R_LREC_IND(*ptd,ptf),*taghp);
              break;
            case TY_COND:
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_PTH;                   /* ptte */
              *taghp++ = HP_PTH;                   /* ptee */
              MARK_HEAP1(R_COND(*ptd,ptte));
              MARK_HEAP1(R_COND(*ptd,ptee));
              break;
            case TY_EXPR:
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_PTH;                   /* pte */
              MARK_HEAP1(R_EXPR(*ptd,pte));
              break;
            case TY_NAME:
              *taghp++ = HP_DATA;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTH;
              MARK_HEAP1(R_NAME(*ptd,ptn));
              break;
            case TY_ZF:
              *taghp++ = HP_DATA;                  /* special */
              *taghp++ = HP_PTH;                   /* namelist */
              *taghp++ = HP_PTH;                   /* pte */
              MARK_HEAP1(R_ZF(*ptd,namelist));
              MARK_HEAP1(R_ZF(*ptd,pte));
              break;
           case TY_ZFCODE:
              *taghp++ = HP_DATA;                 /* special */
              *taghp++ = HP_PTD;                  /* ptd */
              *taghp++ = HP_PTH;                  /* varnames */
              MARK_HEAP1(R_ZFCODE(*ptd,varnames));
              break;
            case TY_SWITCH:
              *taghp++ = HP_DATA;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTH;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("write_heap: SWITCH not implemented");
              break;
            case TY_MATCH:
              *taghp++ = HP_PTH;
              *taghp++ = HP_PTH;
              *taghp++ = HP_PTH;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("write_heap: MATCH not implemented");
              break;
            case TY_NOMAT:
              *taghp++ = HP_PTH;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTD;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("write_heap: NOMAT not implemented");
              break;
            default: 
              sprintf(buf,"write_heap: unknown descriptor type %d",
                          R_DESC(*ptd,type));
              POST_MORTEM(buf);
          }
          break;
        case C_FUNC:
          *taghp++ = HP_DATA;            /* args|nargs */
          if (R_DESC(*ptd,type) == TY_CLOS) {
            *taghp++ = HP_PTH;           /* pta */
            *taghp++ = HP_DATA;          /* ftype */
            MARK_HEAP0(R_CLOS(*ptd,pta),R_CLOS(*ptd,args)+1);
          }
          else {
            *taghp++ = HP_CODE;          /* ptc */
            *taghp++ = HP_PTD;           /* ptd */
          }
          break;
        case C_MATCHING:
          switch (R_DESC(*ptd,type)) {
            case TY_CLAUSE:
              *taghp++ = HP_DATA;  /* free */
              *taghp++ = R_CLAUSE(*ptd,next) ? HP_PTD : HP_DATA; /* next */
              *taghp++ = R_CLAUSE(*ptd,sons) ? HP_PTH : HP_DATA; /* sons */
              MARK_HEAP0(R_CLAUSE(*ptd,sons),3);                 /* pat, guard,body */
              break;
            case TY_SELECTION:
              *taghp++ = HP_DATA;  /* nclauses */
              *taghp++ = R_SELECTION(*ptd,clauses)   ? HP_PTD : HP_DATA; /* clauses */
              *taghp++ = R_SELECTION(*ptd,otherwise) ? HP_PTD : HP_DATA; /* otherwise */
              break;
            default:
              sprintf(buf,"write_heap: unknown descriptor type %d",
                          R_DESC(*ptd,type));
              POST_MORTEM(buf);
          }
          break;
        case C_PATTERN:
          *taghp++ = HP_DATA;                                     /* following */
          *taghp++ = R_PATTERN(*ptd,pattern)  ? HP_PTD : HP_DATA; /* pattern */
          *taghp++ = R_PATTERN(*ptd,variable) ? HP_PTD : HP_DATA; /* variable */
          break;
        case C_CONSTANT:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_PTD;
          break;
        case C_DIGIT:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_PTH;
          MARK_HEAP2(R_DIGIT(*ptd,ptdv),R_DIGIT(*ptd,ndigit));
          break;
        case C_SCALAR:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          break;
        case C_MATRIX:
        case C_VECTOR:
        case C_TVECTOR:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = (R_MVT(*ptd,ptdv,R_DESC(*ptd,class)))?HP_PTH:HP_DATA;
          break;
        default:
          sprintf(buf,"write_heap: unknown descriptor class %d",
                      R_DESC(*ptd,class));
          POST_MORTEM(buf);
      } /* end switch */
    } /* end else */
    /* hochzaehlen nicht vergessen! */
    ptd++;
  } /* end while */

  /* write heapdump info into first heapblock */
  pth = (T_HEAPELEM *)hpbase;
  ptt = tagheap;
  pth[ 2] = (T_HEAPELEM)code_desc;      ptt[ 2] = HP_PTD;
  pth[ 3] = (T_HEAPELEM)0; /* status */ ptt[ 3] = HP_DATA;
#if ! SCAVENGE
  pth[ 4] = (T_HEAPELEM)deslist;        ptt[ 4] = HP_PTD;
  pth[ 5] = (T_HEAPELEM)hplast;         ptt[ 5] = HP_PTH;
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  
  pth[ 6] = (T_HEAPELEM)_redcnt;        ptt[ 6] = HP_DATA;
  pth[ 7] = (T_HEAPELEM)len;            ptt[ 7] = HP_DATA;
  pth[ 8] = (T_HEAPELEM)_nil;           ptt[ 8] = HP_PTD;
  pth[ 9] = (T_HEAPELEM)TabLen;         ptt[ 9] = HP_DATA;
  pth[10] = (T_HEAPELEM)0; /* unused */ ptt[10] = HP_DATA;
  pth[11] = (T_HEAPELEM)HeapBase;       ptt[11] = HP_DATA;
  pth[12] = (T_HEAPELEM)HeapAlign;      ptt[12] = HP_DATA;
  pth[13] = (T_HEAPELEM)TagSystem;      ptt[13] = HP_DATA;
  pth[14] = (T_HEAPELEM)hpbase;         ptt[14] = HP_PTH;
#if ! SCAVENGE
  pth[15] = (T_HEAPELEM)hpend;          ptt[15] = HP_PTH;
  pth[16] = (T_HEAPELEM)StHeapD;        ptt[16] = HP_PTD;
  pth[17] = (T_HEAPELEM)EnHeapD;        ptt[17] = HP_PTD;
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  

#if SCAVENGE
  /* register des scavengers */
  pth[18] = (T_HEAPELEM)ca_adr;         ptt[18] = HP_PTH;
  pth[19] = (T_HEAPELEM)ca_free;        ptt[19] = HP_PTH;
  pth[20] = (T_HEAPELEM)ca_end;         ptt[20] = HP_PTH;
  pth[21] = (T_HEAPELEM)fss_adr;        ptt[21] = HP_PTH;
  pth[22] = (T_HEAPELEM)fss_free;       ptt[22] = HP_PTH;
  pth[23] = (T_HEAPELEM)fss_end;        ptt[23] = HP_PTH;
  pth[24] = (T_HEAPELEM)pss_adr;        ptt[24] = HP_PTH;
  pth[25] = (T_HEAPELEM)pss_free;       ptt[25] = HP_PTH;
  pth[26] = (T_HEAPELEM)pss_end;        ptt[26] = HP_PTH;
  pth[27] = (T_HEAPELEM)os_adr;         ptt[27] = HP_PTH;
  pth[28] = (T_HEAPELEM)os_free;        ptt[28] = HP_PTH;
  pth[29] = (T_HEAPELEM)os_end;         ptt[29] = HP_PTH;
  pth[30] = (T_HEAPELEM)rs_adr;         ptt[30] = HP_PTH;
  pth[31] = (T_HEAPELEM)rs_free;        ptt[31] = HP_PTH;
  pth[32] = (T_HEAPELEM)rs_end;         ptt[32] = HP_PTH;
  pth[33] = (T_HEAPELEM)maxage;         ptt[33] = HP_DATA;
  pth[34] = (T_HEAPELEM)ns_end;         ptt[34] = HP_PTH;
  /* ... */
  /* pth[47] = 0;                          ptt[47] = HP_DATA; */
  /* und jetzt nix mehr! */
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  

  /* open outfile */
  if (fp) fclose(fp);
  if ((fp = fopen(HeapDumpFile,"w")) == NULL) {
    sprintf(buf,"write_heap: can't open \"%s\"",HeapDumpFile);
    POST_MORTEM(buf);
  }

  if (OutputFormat == BINARY_FORMAT) {
    /* write total length, heap length and table length in BINARY-format */
    cnt = len + TabLen;
    if (fwrite((char *)&cnt,sizeof(int),1,fp) != 1)                        /* total length */
      POST_MORTEM("write_heap: binary write error (total len)");
    if (fwrite((char *)&len,sizeof(int),1,fp) != 1)                        /* heap length */
      POST_MORTEM("write_heap: binary write error (heap len)");
    if (fwrite((char *)&TabLen,sizeof(int),1,fp) != 1)                     /* table length */
      POST_MORTEM("write_heap: binary write error (tab len)");
  }
  else
  if (OutputFormat == CAST_FORMAT) {
    ; /* nix */
  }
  else {
    /* write heap area, tag vector and tables in ASCII-format */
    fprintf(fp,"# DumpHeap Version %d\n", VERSION);
    fprintf(fp,"# =======================================\n");
    fprintf(fp,"# HeapDumpFile : %s\n", HeapDumpFile);
    fprintf(fp,"# HeapBase     : %0x\n",HeapBase);
    fprintf(fp,"# HeapAlign    : %d\n", HeapAlign);
    fprintf(fp,"# TagSystem    : %d (%s)\n",TagSystem,TAGSYSNAME(TagSystem));
    fprintf(fp,"# ---------------------------------------\n");
    fprintf(fp,"# hpbase       : %08x (old: %08x)\n",HW_PTH(hpbase),(unsigned int)hpbase);
#if ! SCAVENGE
    fprintf(fp,"# hplast       : %08x (old: %08x)\n",HW_PTH(hplast),(unsigned int)hplast);
    fprintf(fp,"# hpend        : %08x (old: %08x)\n",HW_PTH(hpend),(unsigned int)hpend);
    fprintf(fp,"# StHeapD      : %08x (old: %08x)\n",HW_PTD(StHeapD),(unsigned int)StHeapD);
    fprintf(fp,"# EnHeapD      : %08x (old: %08x)\n",HW_PTD(EnHeapD),(unsigned int)EnHeapD);
    fprintf(fp,"# deslist      : %08x (old: %08x)\n",HW_PTD(deslist),(unsigned int)deslist);
#endif /* SCAVENGE      auskommentiert RS 30.11.1992 */  
    fprintf(fp,"# _nil         : %08x (old: %08x)\n",HW_PTD(_nil),(unsigned int)_nil);
    fprintf(fp,"# =======================================\n");
    fprintf(fp,"# TotalLength HeapLength TableLength (first line!)\n");
    fprintf(fp,"%10d %10d %10d\n",len+TabLen,len,TabLen);
    fprintf(fp,"# =======================================\n");
    fprintf(fp,"# Address Heapelem Tag Comment        \n");
    fprintf(fp,"# ---------------------------------------\n");
  }
  
/* lokale makrodefinition */
#define FPRINT(addr,hpel,hptg,comm) \
    if (OutputFormat == BINARY_FORMAT) { \
      int   i = hpel; \
      *ptt = hptg;     /* wichtig da die tags erst nachher rausgeschrieben werden */ \
      if (fwrite((char *)&i,sizeof(i),1,fp) != 1) \
        POST_MORTEM("write_heap: binary write error (heap element)"); \
    } else \
    if (OutputFormat == CAST_FORMAT) { \
      short aux[3]; \
      /* swap half-words ! */ \
      aux[0] = (hpel) & 0x0000ffff; \
      aux[1] = ((hpel) >> 16) & 0x0000ffff; \
      aux[2] = hptg; \
      if (fwrite((char *)aux,2,3,fp) != 3) \
        POST_MORTEM("write_heap: cast write error (heap element)"); \
    } \
    else \
      fprintf(fp,"%8x: %08x %02x  # %s\n",addr,hpel,hptg,comm)
                            /*   \# in nur # umgewandelt von RS 02/11/92 */
/* ende der makrodefinition */

  /* write heap */
  pth = (T_HEAPELEM *)hpbase;
  ptt = tagheap;
  /* omage an die effizienz: zwei schleifen */
  if (TagSystem == REDUMATAGS) {
    for (cnt = HeapBase, i = len; --i >= 0; cnt+=(HEAPALIGNW/HeapAlign), pth++, ptt++) {
      switch (*ptt) {
        case HP_DATA:
          sprintf(buf,"%d",*pth);
          FPRINT(cnt,*pth,HP_DATA,buf);
          break;
        case HP_NEXT:
          CHECK_HEAP(*pth);
          FPRINT(cnt,HW_PTH(*pth),HP_NEXT,"pth->next");
          break;
        case HP_REF:
          CHECK_DESC(*pth);
          FPRINT(cnt,HW_PTH(*pth),HP_REF,"pth->ref");
          break;
        case HP_ATOM:
          FPRINT(cnt,*pth,HP_ATOM,stelname(*pth));
          break;
        case HP_CODE:
          FPRINT(cnt,*pth,HP_CODE,"label");
          break;
        case HP_INSTR:
          FPRINT(cnt,*pth,HP_INSTR,instrname(*pth));
          break;
        case HP_DESC:
          CHECK_DESC(pth);
          sprintf(buf,"desc: %s",stelname((int)pth));
          FPRINT(cnt,*pth,HP_DESC,buf);
          break;
        case HP_PTH:
          CHECK_HEAP(*pth);
          sprintf(buf,"pth(0x%08x)",*pth);
          FPRINT(cnt,HW_PTH(*pth),HP_PTH,buf);
          break;
        case HP_PTD:
          CHECK_DESC(*pth);
          FPRINT(cnt,HW_PTD(*pth),HP_PTD,stelname(*pth));
          break;
        default:
          sprintf(buf,"write_heap: unknown tag %d",*ptt);
          POST_MORTEM(buf);
      }
    } /* end for */
  } /* end REDUMATAGS */
  else
  if (TagSystem == HARDWARETAGS) {
    for (cnt = HeapBase, i = len; --i >= 0; cnt+=(HEAPALIGNW/HeapAlign), pth++, ptt++) {
      switch (*ptt) {
        case HP_DATA:
          sprintf(buf,"%d",*pth);
          FPRINT(cnt,*pth,HW_INT_TAG,buf);
          break;
        case HP_NEXT:
          CHECK_HEAP(*pth);
          FPRINT(cnt,HW_PTH(*pth),HW_INT_TAG,"pth->next");
          break;
        case HP_REF:
          CHECK_DESC(*pth);
          FPRINT(cnt,HW_PTH(*pth),HW_INT_TAG,"pth->ref");
          break;
        case HP_ATOM:
          FPRINT(cnt,HW_ATOM(*pth),hw_tag(*pth),stelname(*pth));
          break;
        case HP_CODE:
          FPRINT(cnt,*pth,HP_CODE,"label");
          break;
        case HP_INSTR:
          FPRINT(cnt,*pth,HP_INSTR,instrname(*pth));
          break;
        case HP_DESC:
          CHECK_DESC(pth);
          sprintf(buf,"desc: %s",stelname((int)pth));
          FPRINT(cnt,*pth,HW_INT_TAG,buf);
          break;
        case HP_PTH:
          CHECK_HEAP(*pth);
          sprintf(buf,"pth(0x%08x)",*pth);
          FPRINT(cnt,HW_PTH(*pth),HW_INT_TAG,buf);
          break;
        case HP_PTD:
          CHECK_DESC(*pth);
          FPRINT(cnt,HW_PTD(*pth),hw_tag(*pth),stelname(*pth));
          break;
        default:
          sprintf(buf,"write_heap: unknown tag %d",*ptt);
          POST_MORTEM(buf);
      }
    } /* end for */
  } /* end HARDWARETAGS */
  else {
    POST_MORTEM("write_heap: unknown tag-system");
  }
  
  if (OutputFormat == BINARY_FORMAT) {
    if (fwrite((char *)tagheap,sizeof(*tagheap),(int)len,fp) != len)    /* tag vector */
      POST_MORTEM("write_heap: binary write error (tag vector)");
    if (fwrite((char *)TabStart,sizeof(*TabStart),TabLen,fp) != TabLen) /* tables */
      POST_MORTEM("write_heap: binary write error (table)");
  }
  else
  if (OutputFormat == CAST_FORMAT) {
    if (fwrite((char *)TabStart,sizeof(*TabStart),TabLen,fp) != TabLen) /* tables */
      POST_MORTEM("write_heap: cast write error (table)");
  }
  else {
    pth = (T_HEAPELEM *)TabStart;
    fprintf(fp,"# =======================================\n");
    for (i = TabLen ; --i >= 0 ; cnt+=(HEAPALIGNW/HeapAlign), pth++) {
      /* VORSICHT: tabellen koennen referenzen enthalten !!! */
      /* diese muessen von dem tabellenhalter (bloedes wort) korrigiert werden! */
      FPRINT(cnt,*pth,HW_INT_TAG,"tab");
    }
  }

  /* release tagvector */
  if (tagheap) {
    cfree((char *)tagheap);
    tagheap = NULL;
  }

  /* close dumpfile */
  if (fp) {
    fclose(fp);
    fp = NULL;
  }
  return (0);         /* RS 02/11/92 */ 
} /* end write_heap */

/* ----------------------------------------------------------------------- */
/* --- read_heap: liest veraenderten heap ein                          --- */
/* ----------------------------------------------------------------------- */

int read_heap(HeapLoadFile,HeapBase,HeapAlign,TagSystem,TabStart,TabLen)
    /* int eingefuegt von RS 02/11/92 */
char *HeapLoadFile;          /* quelldatei */
int  HeapBase;               /* basisadresse (hierhin wurde der heap verschoben) */
int  HeapAlign;              /* byte- (1) oder wort (4) -adressen */
int  TagSystem;              /* tagsystem: reduma, hardware */
int  *TabStart;              /* tabellenanfang, wird nicht benutzt */
int  TabLen;                 /* tabellenlaenge, wird nicht benutzt */
{
  int                  i,cnt;
  unsigned             len;
  T_DESCRIPTOR         *ptd;
  T_HEAPELEM           *pth;
#if ! SCAVENGE                   /* ach 06/11/92 */
  T_HBLOCK             *ptb;
#endif
  char                 buf[BUFLEN];
  unsigned char        *taghp,*ptt;
  static unsigned char *tagheap = NULL;
  static FILE          *fp = NULL;

#if    SCAVENGE
  /* POST_MORTEM("scavenge and dumpheap conflict!"); */
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  

  if (HeapBase < 0) {
    HeapBase = (int)hpbase;                         /* no relocation */
  }
  if (HeapAlign != HEAPALIGNB && HeapAlign != HEAPALIGNW) {
    /* 1: byte- 4: word-addressing */
    POST_MORTEM("read_heap: HeapAlign not in {1,4}");
  }
  if (HeapBase % (HEAPALIGNW/HeapAlign) != 0) {
    POST_MORTEM("read_heap: HeapBase is misaligned");
  }

  /* open heaploadfile */
  if (fp)
    fclose(fp);
  if ((fp = fopen(HeapLoadFile,"r")) == NULL) {
    sprintf(buf,"read_heap: can't open \"%s\"",HeapLoadFile);
    WARNING(buf);
    return(0);             /* RS 02/11/92 */
  }

  if (InputFormat == BINARY_FORMAT) {
    /* read length line */
    if (fread((char *)&cnt,sizeof(int),1,fp) != 1)                       /* total length */
      POST_MORTEM("read_heap: binary read error (total len)");
    if (fread((char *)&len,sizeof(int),1,fp) != 1)                       /* heap length */
      POST_MORTEM("read_heap: binary read error (heap len)");
    if (fread((char *)&cnt,sizeof(int),1,fp) != 1)                       /* table length */
      POST_MORTEM("read_heap: binary read error (tab len)");
  }
  else
  if (InputFormat == CAST_FORMAT) {
#if ! SCAVENGE
    len = HEAP_INDEX(EnHeapD+1);
#else
    len = HEAP_INDEX(rs_end);
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  
  }
  else {
    /* check version */
    if (!fgets(buf,BUFLEN,fp) || sscanf(buf,"# DumpHeap Version %d ",&cnt) != 1) {
      sprintf(buf,"read_heap: invalid input file \"%s\"",HeapLoadFile);
      POST_MORTEM(buf);
    }
    if (cnt != VERSION) {
      WARNING("read_heap: invalid version number");
    }
  
    /* skip comment & empty lines */
    while (fgets(buf,BUFLEN,fp)) {
      if (*buf != '#' && *buf != '\0') break;
    }
  
    /* read length line */
    if (sscanf(buf," %*d %d %*d ",&len) != 1)
      POST_MORTEM("read_heap: invalid length line format");
  }

  /* check length */
#if SCAVENGE
  if (len != HEAP_INDEX(rs_end))
#else
  if (len != HEAP_INDEX(EnHeapD+1))
#endif /* SCAVENGE         auskommentiert RS 30.11.1992 */  
    POST_MORTEM("read_heap: heapsize not equal file length");

  /* now len contains the actual heap length */
  
  /* allocate tag vector */
  if (tagheap) (void)cfree((char *)tagheap);
  if ((tagheap = (unsigned char *)calloc(len,sizeof(*tagheap))) == NULL) {
    POST_MORTEM("read_heap: calloc failed");
  }

  /* read heap and tag area */
  if (InputFormat == BINARY_FORMAT) {
    if (fread((char *)hpbase,sizeof(int),(int)len,fp) != len)        /* heap area */
      POST_MORTEM("read_heap: binary read error (heap area)");
    if (fread((char *)tagheap,sizeof(*tagheap),(int)len,fp) != len)  /* tag vector */
      POST_MORTEM("read_heap: binary read error (tag vector)");
    if (getc(fp) != EOF)
      WARNING("read_heap: more items in file then expected");
  }
  else
  if (InputFormat == CAST_FORMAT) {
    short aux[3];
    pth = (T_HEAPELEM *)hpbase;
    ptt = tagheap;
    for (cnt = len ; --cnt >= 0 ; pth++, ptt++) {
      if (fread((char *)aux,2,3,fp) != 3)
        POST_MORTEM("read_heap: cast read error");
      /* swap half-words ! */
      *pth = ((((int)aux[1]) & 0x0000ffff) << 16) | (((int)aux[0]) & 0x0000ffff);
      *ptt = aux[2];
    }
  }
  else {
    /* AsciiFormat */
    pth = (T_HEAPELEM *)hpbase;
    ptt = tagheap;
    for (cnt = len; --cnt >= 0; pth++, ptt++) {
      /* skip comment & empty lines */
      while (fgets(buf,BUFLEN,fp)) {
        if (*buf != '#' && *buf != '\0') break;
      }
      /* read heapelement and tag */
      if (sscanf(buf,"%*x : %x %x",pth,&i) != 2)
        POST_MORTEM("read_heap: input file format error");
      *ptt = (unsigned char)i;
    }
  }

  fclose(fp);
  fp = NULL;

  /* check heapdump info from first heapblock */
  pth = (T_HEAPELEM *)hpbase;
  ptt = tagheap;
  ptt[ 2] = RED_CONVERT(pth[2],ptt[2]);    /* result    = pth[ 2]; */  
  ptt[ 3] = HP_DATA;                       /* status    = pth[ 3]; */  
#if ! SCAVENGE
  ptt[ 4] = HP_PTD;  deslist        =             RED_PTD(pth[ 4]);
  ptt[ 5] = HP_PTH;  hplast         = (T_HBLOCK *)RED_PTH(pth[ 5]);
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  
  ptt[ 6] = HP_DATA; _redcnt        =                (int)pth[ 6];
  ptt[ 7] = HP_DATA; if (len       !=                (int)pth[ 7])  WARNING("read_heap: invalid len entry");
  ptt[ 8] = HP_PTD;  if (_nil      !=             RED_PTD(pth[ 8])) WARNING("read_heap: invalid _nil entry");
  ptt[ 9] = HP_DATA; /* TabLen                            pth[ 9] */
  ptt[10] = HP_DATA; /* unused                            pth[10] */
  ptt[11] = HP_DATA; if (HeapBase  !=                (int)pth[11])  WARNING("read_heap: invalid HeapBase entry");
  ptt[12] = HP_DATA; if (HeapAlign !=                (int)pth[12])  WARNING("read_heap: invalid Heap_Align entry");
  ptt[13] = HP_DATA; if (TagSystem !=                (int)pth[13])  WARNING("read_heap: invalid TagSystem entry");
  ptt[14] = HP_PTH;  if (hpbase    != (T_HBLOCK *)RED_PTH(pth[14])) WARNING("read_heap: invalid hpbase entry");
#if ! SCAVENGE
  ptt[15] = HP_PTH;  if (hpend     != (T_HBLOCK *)RED_PTH(pth[15])) WARNING("read_heap: invalid hpend entry");
  ptt[16] = HP_PTD;  if (StHeapD   !=             RED_PTD(pth[16])) WARNING("read_heap: invalid StHeapD entry");
  ptt[17] = HP_PTD;  if (EnHeapD   !=             RED_PTD(pth[17])) WARNING("read_heap: invalid EnHeapD entry");
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  

#if SCAVENGE
  /* register des scavengers */
  ptt[18] = HP_PTH;  if (ca_adr   != (word*)RED_PTH(pth[18])) WARNING("read_heap: invalid ca_adr entry");
  ptt[19] = HP_PTH;      ca_free   = (word*)RED_PTH(pth[19]);
  ptt[20] = HP_PTH;  if (ca_end   != (word*)RED_PTH(pth[20])) WARNING("read_heap: invalid ca_end entry");
  ptt[21] = HP_PTH;      fss_adr   = (word*)RED_PTH(pth[21]);
  ptt[22] = HP_PTH;      fss_free  = (word*)RED_PTH(pth[22]);
  ptt[23] = HP_PTH;      fss_end   = (word*)RED_PTH(pth[23]);
  ptt[24] = HP_PTH;      pss_adr   = (word*)RED_PTH(pth[24]);
  ptt[25] = HP_PTH;      pss_free  = (word*)RED_PTH(pth[25]);
  ptt[26] = HP_PTH;      pss_end   = (word*)RED_PTH(pth[26]);
  ptt[27] = HP_PTH;  if (os_adr   != (word*)RED_PTH(pth[27])) WARNING("read_heap: invalid os_adr entry");
  ptt[28] = HP_PTH;      os_free   = (word*)RED_PTH(pth[28]);
  ptt[29] = HP_PTH;      os_end    = (word*)RED_PTH(pth[29]);
  ptt[30] = HP_PTH;      rs_adr    = (word*)RED_PTH(pth[30]);
  ptt[31] = HP_PTH;      rs_free   = (word*)RED_PTH(pth[31]);
  ptt[32] = HP_PTH;  if (rs_end   != (word*)RED_PTH(pth[32])) WARNING("read_heap: invalid rs_end entry");
  ptt[33] = HP_DATA; if (maxage   != (T_HEAPELEM)pth[33]) WARNING("read_heap: invalid maxage entry");
  ptt[34] = HP_PTH;  if (ns_end   != (word*)RED_PTH(pth[34])) WARNING("read_heap: invalid ns_end entry");
  /* ... */
  /* ptt[47] = HP_DATA;  pth[47] = 0); */
  /* und jetzt nix mehr! */
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  

  /* check status */
  if (pth[3]) {
    sprintf(buf,"read_heap: Reduction ended with errors (status %d)",pth[3]);   /* pth[3] geraten von ach: 03/02/93 */
    /* POST_MORTEM(buf); */
    WARNING(buf);
  }

#if ! SCAVENGE
  /* check freelist */
  for (ptd = deslist; NEXTFREE(ptd); ptd = RED_PTD(NEXTFREE(ptd))) {
    CHECK_DESC(ptd);
    if (R_DESC(*ptd,ref_count) != 0) {
      WARNING("read_heap: invalid freelist. I'll fix it");
      L_DESC(*ptd,ref_count) = 0;
    }
  }
#endif /* SCAVENGE    auskommentiert RS 30.11.1992 */  

  /* run through code area (resides in the heap) */
  for (pth = CODEVEC; *pth != I_SYMTAB; ) {
    int i = instrindex(*pth);
    tagheap[HEAP_INDEX(pth)] = HP_INSTR;
    for (cnt = 1 ; cnt <= instr_tab[i].nparams; ++cnt) {
      switch (instr_tab[i].paramtype) {
        case DESC_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_PTD;
          break;
        case ADDR_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_PTH;
          break;
        case ATOM_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = RED_CONVERT(*pth,tagheap[HEAP_INDEX(pth+cnt)]);
          break;
        case FUNC_PAR:
          tagheap[HEAP_INDEX(pth+cnt)] = HP_ATOM;
          break;
      }
    }  
    pth += (1 + instr_tab[i].nparams);
  }

#if ! SCAVENGE
  /* run through heap chain */
  ptb = hpbase;                                     /* heap start */
  for ( ; ; ) {
    tagheap[HEAP_INDEX(&ptb->next)] = HP_NEXT;      /* heapblock */
    tagheap[HEAP_INDEX(&ptb->ref)] =
      (ptb->ref == 0 || (int)ptb->ref == -1) ? HP_DATA : HP_REF;
    if ((T_HBLOCK *)RED_PTH(ptb->next) == hpbase)
      break;
    if (ptb->ref == 0) {
      /* set tags of free heap to HP_DATA! */
      for (pth = (PTR_HEAPELEM)(ptb+1); pth < (PTR_HEAPELEM)RED_PTH(ptb->next); pth++)
        tagheap[HEAP_INDEX(pth)] = HP_DATA;
    }
    /* test heap chain */
    if (ptb >= (T_HBLOCK *)RED_PTH(ptb->next)) {
      sprintf(buf,"read_heap: invalid heap chain at position %0x ",HEAP_INDEX(ptb));
      POST_MORTEM(buf);
    }
    /* next heap block */
    ptb = (T_HBLOCK *)RED_PTH(ptb->next);
  }
  /* ptb now points to hpend! */

  /* compute start of descriptor area */
  if ((ptd = (T_PTD)(ptb+1)) != StHeapD) {
    WARNING("read_heap: ptd != StHeapD");
  }

  /* run through descriptor area */
  taghp = tagheap + HEAP_INDEX(ptd);     /* start descriptor tags */
  while (ptd <= EnHeapD) {
    *taghp++ = HP_DESC;                             /* descriptor header */
    if (R_DESC(*ptd,ref_count) == 0) {              /* in der freiliste */
      *taghp++ = HP_DATA;                           /* irgendwas */
      *taghp++ = HP_DATA;                           /* irgendwas */
      *taghp++ = NEXTFREE(ptd) ? HP_PTD : HP_DATA;  /* next free */
    }
    else {
#else
  /* scavenge: run through remembered set */
  for (pth = (PTR_HEAPELEM)rs_adr; pth < (PTR_HEAPELEM)rs_end; pth++ ) {
    tagheap[HEAP_INDEX(pth)] = (*pth) ? HP_PTH : HP_DATA;
  }
  /* run through heap area; descriptors and blocks are mixed up */
  ptd = (T_PTD)ca_adr;
  while (1) {
    /* hier hoellisch aufpassen; mehrere bereiche muessen durchlaufen werden */
    if (ptd == (T_PTD)ca_free)  ptd = (T_PTD)pss_adr; /* hier keine elses! */
    if (ptd == (T_PTD)pss_free) ptd = (T_PTD)sta_adr; /* hier keine elses! */
    if (ptd == (T_PTD)sta_free) ptd = (T_PTD)os_adr;  /* hier keine elses! */
    /* abbruchbedingung; die muss genau hier stehen! */
    if (ptd >= (T_PTD)os_free)
      break;
    if (SCAV_IS_BLOCK(ptd)) {
      /* zeiger auf naechsten block/descriptor */
      PTR_HEAPELEM php = (PTR_HEAPELEM)ptd + ((PBLOCK)ptd)->over;
      ptd = (T_PTD)php;
      continue;
    }
    else {
      /* jetzt ham wer'n descriptor */
      taghp = tagheap + HEAP_INDEX(ptd);            /* beginn descriptor tags */
      *taghp++ = HP_DESC;                           /* descriptor header */
#endif /* SCAVENGE      auskommentiert RS 30.11.1992 */  
      switch (R_DESC(*ptd,class)) {
        case C_LIST:
          *taghp++ = HP_DATA;                          /* special|dim */
          *taghp++ = R_LIST(*ptd,ptdd)?HP_PTD:HP_DATA; /* ptdd */
          *taghp++ = R_LIST(*ptd,ptdv)?HP_PTH:HP_DATA; /* ptdv */
          if (!R_LIST(*ptd,ptdd)) {
            MARK_HW_HEAP0(R_LIST(*ptd,ptdv),R_LIST(*ptd,dim));
          }   
          break;
          case C_CONS:
            *taghp = RED_CONVERT(R_CONS(*ptd,hd),*taghp); taghp++; /* head */
            *taghp = RED_CONVERT(R_CONS(*ptd,tl),*taghp); taghp++; /* tail */
            *taghp++ = HP_DATA;                                    /* not used */
            break;
        case C_EXPRESSION:
          switch (R_DESC(*ptd,type)) {
#if WITHTILDE
	    case TY_SNSUB:
#endif /* WITHTILDE */
            case TY_SUB:
              *taghp++ = HP_DATA;                   /* special|nargs */
              *taghp++ = HP_PTH;                    /* namelist */
              *taghp++ = HP_PTH;                    /* pte */
              MARK_HW_HEAP1(R_FUNC(*ptd,namelist));
              MARK_HW_HEAP1(R_FUNC(*ptd,pte));
              break;
            case TY_LREC:
              *taghp++ = HP_DATA;                  /* nfuncs */
              *taghp++ = HP_PTH;                   /* namelist */
              *taghp++ = HP_PTH;                   /* ptdv */
              MARK_HW_HEAP1(R_LREC(*ptd,namelist));
              MARK_HW_HEAP0(R_LREC(*ptd,ptdv),R_LREC(*ptd,nfuncs));
              break;
            case TY_LREC_IND:
              *taghp++ = HP_DATA;                  /* index */
              *taghp++ = HP_PTD;                   /* ptdd */
              *taghp   = RED_CONVERT(R_LREC_IND(*ptd,ptf),*taghp); taghp++;
              break;
            case TY_COND:
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_PTH;                   /* ptte */
              *taghp++ = HP_PTH;                   /* ptee */
              MARK_HW_HEAP1(R_COND(*ptd,ptte));
              MARK_HW_HEAP1(R_COND(*ptd,ptee));
              break;
            case TY_EXPR:
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_DATA;                  /* free */
              *taghp++ = HP_PTH;                   /* pte */
              MARK_HW_HEAP1(R_EXPR(*ptd,pte));
              break;
            case TY_NAME:
              *taghp++ = HP_DATA;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTH;
              MARK_HW_HEAP1(R_NAME(*ptd,ptn));
              break;
            case TY_ZF:
              *taghp++ = HP_DATA;                  /* special */
              *taghp++ = HP_PTH;                   /* namelist */
              *taghp++ = HP_PTH;                   /* pte */
              MARK_HW_HEAP1(R_ZF(*ptd,namelist));
              MARK_HW_HEAP1(R_ZF(*ptd,pte));
              break;
           case TY_ZFCODE:
              *taghp++ = HP_DATA;                 /* special */
              *taghp++ = HP_PTD;                  /* ptd */
              *taghp++ = HP_PTH;                  /* varnames */
              MARK_HW_HEAP1(R_ZFCODE(*ptd,varnames));
              break;
            case TY_SWITCH:
              *taghp++ = HP_DATA;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTH;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("read_heap: SWITCH not implemented");
              break;
            case TY_MATCH:
              *taghp++ = HP_PTH;
              *taghp++ = HP_PTH;
              *taghp++ = HP_PTH;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("read_heap: MATCH not implemented");
              break;
            case TY_NOMAT:
              *taghp++ = HP_PTH;
              *taghp++ = HP_DATA;
              *taghp++ = HP_PTD;
              /* HIER FEHLT NOCH WAS!!! */
              POST_MORTEM("read_heap: NOMAT not implemented");
              break;
            default: 
              sprintf(buf,"read_heap: unknown descriptor type %d",
                          R_DESC(*ptd,type));
              POST_MORTEM(buf);
          }
          break;
        case C_FUNC:
          *taghp++ = HP_DATA;            /* args|nargs */
          if (R_DESC(*ptd,type) == TY_CLOS) {
            *taghp++ = HP_PTH;           /* pta */
            *taghp++ = HP_DATA;          /* ftype */
            MARK_HW_HEAP0(R_CLOS(*ptd,pta),R_CLOS(*ptd,args)+1);
          }
          else {
            *taghp++ = HP_CODE;          /* ptc */
            *taghp++ = HP_PTD;           /* ptd */
          }
          break;
        case C_MATCHING:
          switch (R_DESC(*ptd,type)) {
            case TY_CLAUSE:
              *taghp++ = HP_DATA;  /* free */
              *taghp++ = R_CLAUSE(*ptd,next) ? HP_PTD : HP_DATA; /* next */
              *taghp++ = R_CLAUSE(*ptd,sons) ? HP_PTH : HP_DATA; /* sons */
              MARK_HW_HEAP0(R_CLAUSE(*ptd,sons),3);              /* pat, guard,body */
              break;
            case TY_SELECTION:
              *taghp++ = HP_DATA;                                        /* nclauses */
              *taghp++ = R_SELECTION(*ptd,clauses)   ? HP_PTD : HP_DATA; /* clauses */
              *taghp++ = R_SELECTION(*ptd,otherwise) ? HP_PTD : HP_DATA; /* otherwise */
              break;
            default:
              sprintf(buf,"read_heap: unknown descriptor type %d",
                          R_DESC(*ptd,type));
              POST_MORTEM(buf);
          }
          break;
        case C_PATTERN:
          *taghp++ = HP_DATA;                                     /* following */
          *taghp++ = R_PATTERN(*ptd,pattern)  ? HP_PTD : HP_DATA; /* pattern */
          *taghp++ = R_PATTERN(*ptd,variable) ? HP_PTD : HP_DATA; /* variable */
          break;
        case C_CONSTANT:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_PTD;
          break;
        case C_DIGIT:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_PTH;
          MARK_HW_HEAP2(R_DIGIT(*ptd,ptdv),R_DIGIT(*ptd,ndigit));
          break;
        case C_SCALAR:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          break;
        case C_MATRIX:
        case C_VECTOR:
        case C_TVECTOR:
          *taghp++ = HP_DATA;
          *taghp++ = HP_DATA;
          *taghp++ = (R_MVT(*ptd,ptdv,R_DESC(*ptd,class)))?HP_PTH:HP_DATA;
          break;
        default:
          sprintf(buf,"read_heap: unknown descriptor class %d",
                      R_DESC(*ptd,class));
          POST_MORTEM(buf);
      } /* end switch */
      /* hochzaehlen nicht vergessen! */
      ptd++;
    } /* end else */
  } /* end while */

  /*  read tags and correkt entrys */
  pth = (T_HEAPELEM *)hpbase;
  ptt = tagheap;
  if (TagSystem == REDUMATAGS) {
    for (cnt = len; --cnt >= 0; pth++, ptt++) {
      switch (*ptt) {
        case HP_DATA:
        case HP_DESC:
        case HP_INSTR:
        case HP_CODE:
        case HP_ATOM:
        case HP_INT:
        case HP_BOOL:
          break;
        case HP_NEXT:
        case HP_PTH:
          *pth = (T_HEAPELEM)RED_PTH(*pth);
          CHECK_HEAP(*pth);
          break;
        case HP_PTD:
          *pth = (T_HEAPELEM)RED_PTD(*pth);
          CHECK_DESC(*pth);
          break;
        case HP_REF:
          *pth = (T_HEAPELEM)RED_PTH(*pth);
          /* CHECK_DESC(*pth); */
          break;
        default:
          sprintf(buf,"read_heap: unknown tag %d",*ptt);
          WARNING(buf);
          /* POST_MORTEM(buf); */
      }
    }
  } /* end REDUMATAGS */
  else
  if (TagSystem == HARDWARETAGS) {
    for (cnt = len; --cnt >= 0; pth++, ptt++) {
      switch (*ptt) {
        case HP_DATA:
        case HP_DESC:
        case HP_INSTR:
        case HP_CODE:
        case HP_ATOM:
          break;
        case HP_INT:
          *pth = TAG_INT(*pth);
          break;
        case HP_BOOL:
          *pth = *pth ? SA_TRUE : SA_FALSE;
          break;
        case HP_NEXT:
        case HP_PTH:
          *pth = (T_HEAPELEM)RED_PTH(*pth);
          CHECK_HEAP(*pth);
          break;
        case HP_PTD:
          *pth = (T_HEAPELEM)RED_PTD(*pth);
          CHECK_DESC(*pth);
          break;
        case HP_REF:
          *pth = (T_HEAPELEM)RED_PTH(*pth);
          CHECK_DESC(*pth);
          break;
        default:
          sprintf(buf,"read_heap: unknown tag %d",*ptt);
          WARNING(buf);
          /* POST_MORTEM(buf); */
      }
    }
  } /* end HARDWARETAGS */
  else {
    POST_MORTEM("read_heap: unknown tag-system");
  }

  /* push result onto stack a */
  WRITESTACK(S_a,((PTR_HEAPELEM)hpbase)[2]);
  return (0);             /* RS 02/11/92 */ 
}

/* ---------------------------------------- */
/* --- dump_heap: dumpt den reduma-heap --- */
/* ---------------------------------------- */

int dump_heap(filename)               /* int eingefuegt von RS 02/11/92 */ 
char *filename;
{
#if !SCAVENGE             /* von hier .... */
  int         *pth;
  static char buf[BUFLEN];
  FILE        *fp;
#endif /* SCAVENGE        auskommentiert RS 30.11.1992 */  
  /* FILE        *fopen();*/ /* bis hier #if #endif eingefuegt und umsortiert von RS 6.11.1992 */ 

#if ! SCAVENGE
  if ((fp = fopen(filename,"w")) != NULL) {
    /* dump heap area */
    for (pth = (int *)hpbase ; pth < (int *)(EnHeapD+1) ; pth++) {
      fprintf(fp,"%8x: %08x\n",(unsigned int)pth,*pth);
    }
    fclose(fp);
  }
  else {
    sprintf(buf,"heapdump: can't open \"%s\"",filename);
    POST_MORTEM(buf);
  }
#endif /* SCAVENGE     auskommentiert RS 30.11.1992 */  
 return (0);             /* RS 02/11/92 */ 
}

/* end of file */

