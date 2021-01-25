/* $Log: rscavenge.h,v $
 * Revision 1.2  1992/12/16  12:50:57  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* rscavenge.h */

#if SCAVENGE

/*** Bit-Flags fuer die Heap-Objekte ***/

#define FORWARD_FLAG    0x01
#define BLOCK_FLAG      0x02
#define COMPACT_FLAG    0x04
#define LIFE_FLAG       0x08

typedef unsigned char byte;
typedef unsigned short hword;
typedef unsigned long word;

#define CASTDATASIZE	48	/* length of info block */

#if ODDSEX

#define SCAV_HEADER \
  byte type; \
  byte class; \ 
  byte age; \
  byte flags

typedef struct
  {
  byte type;
  byte class;
  byte age;
  byte flags;
  }
  SC_HEAD;
 
#else

#define SCAV_HEADER \
  byte flags; \
  byte age; \
  byte class; \
  byte type

typedef struct
  {
  byte flags;
  byte age;
  byte class;
  byte type;
  }
  SC_HEAD;

#endif /* ODDSEX     auskommentiert RS 30.11.1992 */

typedef struct
  {
  SCAV_HEADER;
  hword size;     /* Anzahl der Nutzdaten */
  hword over;     /* Gesamtgroesse (incl. Verwaltung und Fuellworte) */
  word content[1];
  }
  BLOCK, *PBLOCK;

typedef struct
  {
  SCAV_HEADER;
  word forward;
  }
  FORWARD, *PFORWARD;

/* Pointer-Test */

#define Q_POINTER(p) (p && IS_POINTER (p))

/* Test ob Deskriptor oder Heapblock */

#define SCAV_IS_BLOCK(p) (((SC_HEAD*)(p))->flags & BLOCK_FLAG)
#define SCAV_IS_DESC(p)  (!IS_BLOCK(p))


extern word *sta_adr;

#ifdef NotUsed
/* Adressen von statischen und dynamische Allokierungsbereichen */

static word *sta_free, *dyn_free;
static word sta_size, dyn_size;

static byte bstatic;         /* 1 - static alloc, 0 - dynamic alloc */

/* creation area, past/future survivor space, old space */

word *ca_adr, *pss_adr, *fss_adr, *os_adr;
word *ca_free, *pss_free, *fss_free, *os_free;
word ca_size, pss_size, fss_size, os_size;

word *ca_end, *pss_end, *fss_end, *os_end, *rs_end, *ns_end;	/* fuer LOLEILA */

/* compact area */

word *co_adr;
word co_size;

/* remembered set */

word *rs_adr;
word *rs_free;
word rs_size;
word rs_block;	/* Groesse, um die rs waechst bzw. schrumpft */
word rs_used;	/* Anzahl der belegten rs-Eintraege */

/* sonstige Variablen */

int maxage;	/* maximales Alter im new space */
static word *pheap_ret; /* siehe get_heap */

/* externe Variablen */

extern StackDesc *ps_a, *ps_i;
extern word *_desc;
extern STACKELEM _scav1, _scav2, _scav3;
extern int CodeAreaPercent;



int init_scavenge ();
int reinit_scavenge ();
int exit_scavenge ();

void static_gc ();
void dynamic_gc ();

char *get_desc ();
int get_heap2 ();
char *get_heap ();
char *get_buff ();

extern int get_setup ();

static void init_rs ();
static int compact_rs ();
static int add_rs_elem ();
static int delete_rs_elem ();

static word *scav_alloc ();

static int scavenge ();

static int compact_ns ();
static int parse_stack ();
static int parse_object ();
static word *copy_and_forward ();
static word *copy_fss ();
static word *copy_os ();

static int compact_os ();

static int in_os ();
static int in_ns ();
static int in_src ();

#if SCAV_DEBUG
#define min(arg1,arg2) (arg1 <= arg2 ? arg1 : arg2)
#define max(arg1,arg2) (arg1 <= arg2 ? arg2 : arg1)

typedef struct
  {
  word count;
  word size;
  }
  REP;

static REP pr[20+1];
static word scav_count;
static REP fail;
static double scavtime;
static int rsmax;

void scav_out ();
void scav_write_log ();
void scav_write_spaces ();
void scav_write_ws ();
void scav_write_heap ();
void scav_write_report ();
char *scav_handle ();
static void init_scav_out ();
static void scav_write_data ();
static void scav_write_area ();
static void report ();
#endif

#endif /* NotUsed    auskommentiert RS 30.11.1992 */

#endif /* SCAVENGE   auskommentiert RS 30.11.1992 */
