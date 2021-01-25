/* pmanalyze.h */

/* In diesem File sind die Datenstrukturen und Macros definiert, die fuer die 
   Analyze eines Patterns benoetigt werden. */


typedef struct {
  int levels;         /* Schachtelungstiefe */
  int skskips;        /* # der ... auf dieser Ebene */
  int totalskskips;   /* # der ... im Pattern */
  int lastskskips;    /* # der letzten ... im Pattern */
  int elements;       /* # der Elemente auf dieser Ebene (... als 0 gezaehlt) */
  int variables;      /* # der Variablen im Pattern */
} listpattern_info;

typedef struct {
  int bindings;            /* Anzahl der Bindungen, die das Pattern macht */
  int sublist;             /* Listenpattern ist Teilstruktur? */
  listpattern_info list_info;  /* Infomationen ueber vorhandene Teil-Listenpattern */
} pattern_info;




/* #define MAKE_DESC(p,r,c,t) if ((p = newdesc()) != 0) { */ 
                             /* DESC_MASK(p,r,c,t); */ 
                           /* } */ 
                           /* else post_mortem("preproc: heap overflow") */

/* #define GET_HEAP(n,a)      if (newheap(n,a) == 0) */ 
                             /* post_mortem("preproc: heap overflow");*/ 
                           /* else */


/* Das MACRO ALLOC_INFO allokiert einen Speicherbereich im Heap, indem
   die Patterninformation untergebracht werden soll, und verankert diesen Bereich
   im PTDD des uebergebenen Descriptors. Pattern haben nur direkte Descriptoren.
   Deshalb ist das PTDD-Feld bei ihnen frei. */ 

#define ALLOC_INFO(ptdesc)  GET_HEAP(sizeof(listpattern_info),A_LIST(*(ptdesc),ptdd))

/* Das Macro FREE_INFO gibt den benoetigten Speicherbereich wieder frei
*/
#define FREE_INFO(ptdesc)  \
  freeheap(R_LIST(*(ptdesc),ptdd));\
  L_LIST(*(ptdesc),ptdd)=(T_PTD)NULL


/* Das Macro INFO greift auf die Komponente FIELD im durch PTDESC referenzierten 
   Descriptor zu. */ 

#define PATINFO(ptdesc)  (*((listpattern_info*)R_LIST(*(T_PTD)(ptdesc),ptdd)))

#define INFO(ptdesc,field) (PATINFO(ptdesc).field) 


#define INIT_INFO(ptdesc) { \
           int i;  char *c; \
           c=(char*)R_LIST(*(T_PTD)(ptdesc),ptdd); \
           for (i=sizeof(listpattern_info); i>0; i--) *c++=0; } \
         
#define MAKE_PATTERN(ptr,type)  MAKE_DESC(ptr,1,C_PATTERN,type) 

#define IS_AS(ptr) \
     (R_DESC(*(T_PTD)(ptr),type)==TY_EXPR) && (T_PM_AS(R_EXPR(*(T_PTD)(ptr),pte)[1]))

#define IS_IN(ptr) \
     (R_DESC(*(T_PTD)(ptr),type)==TY_EXPR) && (T_PM_IN(R_EXPR(*(T_PTD)(ptr),pte)[1]))

#define IS_LIST(ptr)  (R_DESC(*(T_PTD)(ptr),class)==C_LIST)

#define IS_VAR(ptr) ((R_DESC(*(T_PTD)ptr,type)==TY_VAR) || \
           (R_DESC(*(T_PTD)ptr,type)==TY_NAME)) 

#define IS_TAGGED_INT(n) (n & 1) 

