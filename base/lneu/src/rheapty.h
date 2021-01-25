

/*****************************************************************************/
/*                                                                           */
/* In dieser Datei stehen die Definitionen der Heaptypen und Heapstrukturen, */
/* sowie die Zugriffsmakros des Heaps.                                       */
/*                                                                           */
/*****************************************************************************/

/* last update  by     why
   25.05.88     sj     Einfuegen von Kommentar
                       Einfuegen von siehe "BAR" fuer den Patternmatch
   30.05.88            Zugriff mit STORE_EDIT wird jetzt mit ueber die
                       L_ / R_ / A_Makros realisiert
                       Makro STORE_EDITS und READ_EDIT entfernt     SJ-ID
   31.05.88     sj     Descriptor Level : STORE_EDIT ...  ersetzt durch
                       (*(((int*)p)+1) = 0)  d.h.
                       Initialisieren von refcount / class / type
                       und des zweiten Wortes des Deskriptors       SJ-ID

   copied       by     to
   15.06.88     sj     (struck)iheapty:h
*/

#if MEASURE
#define MEAS_CALL(x) measure(x)
#else
#define MEAS_CALL(x) 0
#endif  /* MEASURE */

/******************************************************************************
 *                                                                   SJ-ID    *
 *   Ein Deskriptor hat eine Groesse von 16 Byte :                            *
 *   =============================================                            *
 *     Deskriptormaske :                                                      *
 *     -----------------                                                      *
 *       1 Byte fuer die Typdefinition der Deskriptor - Klassen               *
 *              (typedef char DESC_CLASS;)                                    *
 *       1 Byte fuer die Typdefinition der Deskriptor - Typen                 *
 *              (typedef char DESC_TYPE;)                                     *
 *       2 Byte fuer die Typdefinition des Deskriptor - Refcount              *
 *              (typedef unsigned short COUNT;)                               *
 *                                                                            *
 *     Spezifikation der unterschiedlichen DESCriptor_CLASS Definitionen :    *
 *     -------------------------------------------------------------------    *
 *       12 Byte fuer die Typdefinition der Strukturen der                    *
 *               unterschiedlichen DESCriptor CLASS Definitionen              *
 *                                                                            *
 *       Alle Strukturen wurden auf 12 Byte aufgefuellt.                      *
 *       Platz fuer eine Edit-Information wurde in Listen-,                   *
 *       Felder-,Conditional- und Funktionsstrukturen integriert.             *
 *                                                                            *
 *  MC-68000 :                                                                *
 *       char  8 Bit  |  short 16 Bit  |   int  32 Bit   |   double 64 Bit    *
 *                                                                            *
 *       PTR_HEAPELEM  ist als Typ von int definiert.                         *
 ******************************************************************************/

typedef char DESC_CLASS;              /* enum in 1 byte    */

    /* Wenn Bit 7,6 = 0,0  -> normale      Descriptor-Laenge ,  */
    /* Wenn Bit 7,6 = 0,1  -> doppelte     Descriptor-Laenge ,  */
    /* Wenn Bit 7,6 = 1,0  -> dreifache    Descriptor-Laenge ,  */
    /* Wenn Bit 7,6 = 1,1  -> undefinierte Descriptor-Laenge .  */

#define            C_SCALAR                '\1'  /* possible          */
#if LARGE
#define            C_DIGIT                 '\2'  /* descriptor classes*/
#endif
#define            C_LIST                  '\3'  /* for lists and strings */
#define            C_MATRIX                '\4'
#define            C_EXPRESSION            '\6'
#define            C_CONSTANT              '\7'  /* free variable desc*/
#if LARGE
#define            C_VECTOR                '\10'
#define            C_TVECTOR              '\11'
#endif
#define            C_LREC                 '\22'

typedef char DESC_TYPE;               /* enum in 1 byte     */

#define                TY_FREE              '\0'  /* Descriptor ist in*/
                                                  /* der Freiliste    */
#define                TY_UNDEF             '\1'
#define                TY_INTEGER           '\2'
#define                TY_REAL              '\3'
#define                TY_BOOL              '\5'
#define                TY_STRING            '\6'
#if LARGE
#define                TY_DIGIT             '\7'
#endif
#define                TY_REC              '\11' /* possible          */
#define                TY_SUB              '\12' /* type informations */
#define                TY_COND             '\13'
#define                TY_EXPR             '\15'
#define                TY_VAR              '\16'
#define                TY_NAME             '\17'
#if LARGE
#define                TY_MATRIX           '\21'  /* for matrix with more */
                                                  /* than two dimensions  */
#endif
#define                TY_SWITCH           '\22'  /* BAR */
#define                TY_MATCH            '\23'
#define                TY_NOMAT            '\25'
#define                TY_LREC             '\26'  /* letrec   *//* CS */
#define                TY_LREC_IND         '\27'  /* letrec   *//* CS */

#define                TY_FNAME            '\31' 
#define                TY_CLOS             '\32' 
#define                TY_P_CLOS           '\35'    
#define                TY_SPEC             '\36'

typedef unsigned short COUNT;         /*  Refcount  2 byte */


typedef char PROC_LEVEL;

#define     P_L_ATOM          '\0'
#define     P_L_HIERARCHICAL  '\1'

typedef char PROC_CLASS;

#define     P_C_SIMPLE        '\1'
#define     P_C_RECDEF        '\2'
#define     P_C_RECURSIVE     '\3'
#define     P_C_FIELD         '\4'
#define     P_C_MERGE_FIELD   '\5'
#define     P_C_ALTERNATE     '\6'
#define     P_C_SEM_INTERN    '\7'
#define     P_C_P_LOOP        '\10'
#define     P_C_C_LOOP        '\11'
#define     P_C_ALPHA_VAR     '\12'
#define     P_C_DOLLAR_VAR    '\13'

typedef char PROC_STATUS;

#define     P_S_ACTIVATED     '\1'
#define     P_S_ACTIVE        '\2'
#define     P_S_SUSPENDED     '\3'
#define     P_S_NOT_ACTIVATED '\4'
#define     P_S_ERROR         '\5'

typedef char PROC_IL;

#define     P_IL_FALSE        '\0'
#define     P_IL_TRUE         '\1'

typedef char PROC_LOOP;

#define     P_L_FALSE         '\0'
#define     P_L_TRUE          '\1'

typedef char PROC_PP;

#define     P_PP_FALSE        '\0'
#define     P_PP_TRUE         '\1'

typedef char PROC_FLAG;

#define     P_F_STREAM        '\0'
#define     P_F_VALUE         '\1'

/* Working-Definition */
typedef int T_HEAPELEM ;
typedef int * PTR_HEAPELEM ;
typedef struct T_descriptor * T_PTD;

/************************************************************************
 *                        A C H T U N G !                               *
 *  Die Strukturkomponente 'special' wird in einigen der folgenden      *
 *  Definitionen fuer zwei unterschiedliche Zwecke verwendet:           *
 *     1) Bei Benutzung in einem "direkten" Deskriptor:                 *
 *          als "edit"-Feld                                             *
 *     2) Bei Benutzung in einem "indirekten" Deskriptor                *
 *        (wird nur bei Listen verwendet):                              *
 *          als "offset"-Feld.                                          *
 *  Dadurch wird ein einheitlicher Zugriff auf dieses Feld durch die    *
 *  L-,R- und A-Macros gewaehrleistet.                                  *
 ************************************************************************/

typedef struct {
        int vali;
        float valr;
}               ST_SCALAR;

typedef struct {
    COUNT        sign;
    COUNT      ndigit;
    int           Exp;  /* wegen include der Standardbibliothek <"math.h">  */
    PTR_HEAPELEM ptdv;  /* in rbibfunc.c  und L_ exp = R_ exp Markozuweisung*/
}               ST_DIGIT;

typedef struct {            /* SJ-ID  Typdefinition fuer direkte   */
    COUNT          special; /*        und indirekte Listenstruktur */
    COUNT          dim;
    T_PTD          ptdd;
    PTR_HEAPELEM   ptdv;
}                 ST_LIST;


typedef struct {
    COUNT       nrows;
    COUNT       ncols;
    T_PTD        ptdd;
    PTR_HEAPELEM ptdv;
}               ST_MVT;              /* matrix / vector / transposed vector */


typedef struct {
    COUNT       special;       /* SJ-ID */
    char freespace[2];
    PTR_HEAPELEM  ptte;
    PTR_HEAPELEM  ptee;
}               ST_COND;

typedef struct {
    char freespace[8];
    PTR_HEAPELEM  pte;
}               ST_EXPR;    

typedef struct {
    T_PTD 
      i_stack, tilde_stack,     /* die Umgebung */
      ptdd;                     /* der Ausdruck */
}             ST_SPEC;


typedef struct {
    char freespace[4];
    int        nlabar;
    T_PTD        ptnd;   /* Pointer auf Namensdescriptor */
}               ST_VAR;

typedef struct {
    char freespace[8];
    PTR_HEAPELEM  ptn;   /* Heapadresse des Namenstrings */
}               ST_NAME;

typedef struct {
    char freespace[8];
    PTR_HEAPELEM  ptn;   /* Heapadresse des Namenstrings */
}               ST_FNAME;

typedef struct {
    COUNT         special;          /* SJ-ID */
    COUNT           nargs;
    PTR_HEAPELEM namelist;
    PTR_HEAPELEM      pte;
}               ST_FUNC;
typedef struct {
    COUNT          ndargs;
    COUNT             nfv;
    PTR_HEAPELEM      pta;
    T_PTD            ptfd;
}               ST_CLOS;

typedef struct {
    char     freespace[2];
    COUNT          ndargs;
    int          primfunc;
    PTR_HEAPELEM      pta;
}               ST_P_CLOS;



typedef struct {                 /* BAR  Patternmatch */
    int             casetype;
    COUNT              nwhen;
    COUNT           anz_args;     
    PTR_HEAPELEM        ptse;
}               ST_SWITCH;

typedef struct {                /* BAR  Patternmatch */
    PTR_HEAPELEM     code;
    PTR_HEAPELEM    guard;
    PTR_HEAPELEM     body;
}             ST_MATCH;

typedef struct {              /*BAR  Patternmatch*/
    PTR_HEAPELEM  guard_body; /*act_nom steht auf 0, falls es sich*/
    COUNT         act_nomat;  /*um otherwise Ausdruck handelt*/
    COUNT            reason;
    T_PTD     ptsdes;
}               ST_NOMAT;

typedef struct {                                                       
    COUNT         special;  
    COUNT          nfuncs;                                             
    PTR_HEAPELEM namelist;                                             
    PTR_HEAPELEM     ptdv;                                             
}               ST_LREC;                                               

typedef struct {                                                       
    int             index;                                             
    T_PTD            ptdd;                                             
    T_PTD             ptf;
}               ST_LREC_IND;                                           

typedef struct {                                           /* AE */
   T_PTD       rueck;                                      /* AE */
   char        freespace[8];                               /* AE */
}             ST_FREE;

typedef struct T_descriptor {
/*  struct T_descriptor  *heap_chain;                                 */
/*  Der Heapchainverweis wird, falls der Descriptor frei ist, auf die */
/*  letzten vier Byte des Descriptors gelegt.   (siehe rheap.c)       */

#if  ODDSEX
    DESC_TYPE   type;
    DESC_CLASS class;
    COUNT  ref_count;
#else
    COUNT ref_count;
    DESC_CLASS class;
    DESC_TYPE type;
#endif
    union { /* die moeglichen Deskriptoren: */
            ST_SCALAR     sc;
#if LARGE
            ST_DIGIT      di;
#endif
            ST_LIST       li;
#if LARGE
            ST_MVT        mvt; /* matrix / vector / transposed vector */
#endif
            ST_VAR        va;  /* die Expressiondescriptoren: */
            ST_COND       co;
            ST_EXPR       ex;    
            ST_SPEC       sp;
            ST_FUNC       fu;
            ST_NAME       na;
            ST_FNAME      fn;
            ST_SWITCH     sw;  /* BAR  Patternmatch */
            ST_MATCH      ma;  /* BAR  Patternmatch */
            ST_NOMAT      no;  /* BAR  Patternmatch */
            ST_CLOS       cl;
            ST_P_CLOS    pcl;
            ST_LREC       lr;                              /* CS */
            ST_LREC_IND   ri;                              /* CS */
            ST_FREE       fr; /* Freier Descriptor */
    }      u;
} *PTR_DESCRIPTOR, T_DESCRIPTOR;

/*******************************************************************/
/* Lange Descriptoren fuer GRAPH                                   */
/*******************************************************************/

/* Working Definition */
typedef struct T_process_desc *T_PTP;
typedef struct T_Gfield_desc *T_PTF;

typedef struct {
    T_PTP        proc;
    short        flag;
} * PTPF , T_PROC_FLAG;

typedef struct {
    T_PTP        proc1;
    T_PTP        proc2;
    char         flag;
} * PTP2F , T_PROC2_FLAG;

typedef struct {
    int          pos_x;
    int          pos_y;
    int          segment_id;
} *PTEdit , T_Edit;


typedef struct {
    char         arity;
    char         sem_flag;
    short        act_ind;
    T_PTP        crit_proc;
#if ODDSEX
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
    char         freespace1[3];
#else
    char         freespace1[3];
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
#endif       /* ODDSEX */
    T_PTP      * entry_proc;     /* liegt im Heap */
    char         freespace2[8];
} *PTR_SEMAPHOR , T_SEMAPHOR;


typedef struct T_Gfield_desc {
    short        maxdim;
    short        dim;
    short        l_used;
    short        l_term;
#if ODDSEX
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
    char         freespace[3];
#else
    char         freespace[3];
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
#endif       /* ODDSEX */
    T_PTF        masterdesc;
    T_PTP        master;
    PTPF         proc_list;      /* liegt im Heap */
} *PTR_GFIELD , T_GFIELD;


typedef struct {
    short        maxdim;
    short        dim;
    short        l_used;
    char         freespace1[2];
#if ODDSEX
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
    char         freespace[3];
#else
    char         freespace[3];
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
#endif       /* ODDSEX */
    T_PTF        masterdesc;
    T_PTP        master;
    PTPF         proc_list;      /* liegt im Heap */
} *PTR_MERGE_F , T_MERGE_F;


typedef struct {
    short        size;
    short        first_index;
    short        dim;
    char         prod_index;
    char         cons_index;
#if ODDSEX
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
    char         freespace1[3];
#else
    char         freespace1[3];
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
#endif       /* ODDSEX */
    T_PTP        producer;
    T_PTP        consumer;
    PTR_HEAPELEM data;
    PTR_HEAPELEM name;
    char         freespace2[12];
} *PTR_STREAM , T_STREAM;


typedef struct {
    short        arity;
    short        merge_index;
    char         cons_index;
    char         freespace1[3];
#if ODDSEX
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
    char         freespace2[3];
#else
    char         freespace2[3];
    char         busymark;       /* Bit 0 oder Bit 1 = 1 ! */
#endif       /* ODDSEX */
    T_PTP        consumer;
    PTR_HEAPELEM name;
    PTR_HEAPELEM streamlist;    /* liegt im Heap */
} *PTR_MERGE_S , T_MERGE_S;


typedef struct T_process_desc {
    long         terminted_flag;
    short        act_in;
    short        act_out;
#if ODDSEX
    unsigned     mark    : 2;
    unsigned     free    : 3;
    PROC_PP      ppflag  : 1;
    PROC_LOOP    loopfl  : 1;
    PROC_IL      ilflag  : 1;
    PROC_STATUS  status  : 3;
    PROC_CLASS   class   : 4;
    PROC_LEVEL   level   : 1;
    unsigned     ari_out : 8;
    unsigned     ari_in  : 8;
#else
    unsigned     ari_in  : 8;
    unsigned     ari_out : 8;
    PROC_LEVEL   level   : 1;
    PROC_CLASS   class   : 4;
    PROC_STATUS  status  : 3;
    PROC_IL      ilflag  : 1;
    PROC_LOOP    loopfl  : 1;
    PROC_PP      ppflag  : 1;
    unsigned     free    : 3;
    unsigned     mark    : 2;
#endif  /* ODDSEX */
    short        act_childs;
    short        nr_sub_proc;
    T_PTP        father;
    PTR_HEAPELEM name;
    T_PROC_FLAG  inlist;
    T_PROC_FLAG  outlist;
    int          express;    /* irgendein Pointer */
    int          multi_verw; /* irgendein Pointer */
} *PTR_PROCESS , T_PROCESS; 


typedef struct Long2_Desc {
#if ODDSEX
    DESC_CLASS   class;
    char         semaphor;
    COUNT        ref_count;
#else
    COUNT        ref_count;
    char         semaphor;
    DESC_CLASS   class;
#endif       /* ODDSEX */
    PTEdit       edit;
  union {
    T_SEMAPHOR   se;
    T_GFIELD     fi;
    T_MERGE_F    mf;
    T_MERGE_S    ms;
  } u;
} *PTR_2_DESC , T_2_DESC;


typedef struct Long3_Desc {
#if ODDSEX
    DESC_CLASS   class;
    char         semaphor;
    COUNT        ref_count;
#else
    COUNT        ref_count;
    char         semaphor;
    DESC_CLASS   class;
#endif       /* ODDSEX */
    PTEdit       edit;
  union {
    T_STREAM     st;
    T_PROCESS    pr;
  } u;
} *PTR_3_DESC , T_3_DESC;

/*---------------------------------------------------------------------------
 * allgemeine Makros
 *--------------------------------------------------------------------------
 */

#define NEWDESC(desc)  (desc = newdesc())
/* wird nur in der Processing Phase verwendet. */

#if NOREFCNTNG

#define INC_IDCNT
#define DEC_IDCNT

#define INC_REFCNT(id)
#define DEC_REFCNT(id)
#define DEC_REFCNT_SCALAR(id)

#else /* NOREFCNTNG */

#if DEBUG

#define INC_IDCNT             inc_idcnt() /* increment indirect descriptor counter */
#define DEC_IDCNT             dec_idcnt() /* decrement indirect descriptor counter */

#define INC_REFCNT(id)        test_inc_refcnt(id,__FILE__,__LINE__)
#define DEC_REFCNT(id)        test_dec_refcnt(id,__FILE__,__LINE__)
#define DEC_REFCNT_SCALAR(id) DEC_REFCNT(id)

#else /* DEBUG */

#define INC_IDCNT             idcnt++;    /* increment indirect descriptor counter */
#define DEC_IDCNT             idcnt--;    /* decrement indirect descriptor counter */

#define INC_REFCNT(id)        (++((id)->ref_count))
#define DEC_REFCNT(id)        if (--((id)->ref_count) == 0) \
                                 (* dealloc_tab[(id)->class])(id); else
#define DEC_REFCNT_SCALAR(id) if (--((id)->ref_count) == 0) \
                                 freedesc(id); else
                              /* wird nur in valfunc:c verwendet. */
#endif  /* DEBUG */

#endif /* NOREFCNTNG */

/*********************************************************************/
/*                                                                   */
/*                 P R A E F I X  -  M A K R O S                     */
/*                                                                   */
/*********************************************************************/
/*  SJ-ID                                                            *
 *         p = Pointer auf entsprechenden Deskriptor                 *
 *         x = gewuenschte Komponente                                *
 *         L = sollte auf der linken Seite einer Zuweisung stehen    *
 *             Zugriffe haben zuweisenden Charakter                  *
 *         R = sollte auf der rechten Seite einer Zuweisung stehen   *
 *             im wesentlichen lesende Zugriffe                      *
 *         A = Adresse des Deskriptors                               *
 *********************************************************************/

#if DEBUG

/*********************************************************************/
/* Descriptor Level                                                  */
/*********************************************************************/

#define DESC_MASK(p,ref,cl,ty)                   \
  MEAS_CALL(MS_DESCREF),                         \
  (*(int*) p) = ((ref <<16) | (cl << 8) | ty),   \
  (*(((int*)p)+1) = 0)
/* Benutzung : alle Deskriptor-Initialisierungen ausser                    */
/*             der Initialisierungen von Listen-Deskriptoren               */
/* Das Makro DESC_MASK initialisiert                                       */
/*           das erste Wort (refcount / class / type)  und                 */
/*           das zweite Worte des Deskriptors.                             */
/* Bemerkung:                                                              */
/*      Die Or- und Shift-operationen werden zur Compilezeit durchgefuehrt,*/
/*      da die Argumente des Makros fast immer Konstanten sein werden.     */

#define LI_DESC_MASK(p,ref,cl,ty)                \
  MEAS_CALL(MS_DESCREF),                         \
  (*(int*) p) = ((ref <<16) | (cl << 8) | ty),   \
  (*(((int*)p)+1) = 0),                          \
  (*(((int*)p)+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von Listen-Deskriptoren          */
/* Das Makro initialisiert                                                 */
/*              das erste Wort (refcount / class / type)                   */
/*              das zweite Wort (special / dim) und                        */
/*              das dritte Wort (ptdd) des Listen-Deskriptors.             */
/* Bemerkung:                                                              */
/*      Die Or- und Shift-operationen werden zur Compilezeit durchgefuehrt,*/
/*      da die Argumente des Makros fast immer Konstanten sein werden.     */

#define MVT_DESC_MASK(p,ref,cl,ty)               \
  MEAS_CALL(MS_DESCREF),                         \
  (*(int*) (p)) = (((ref) <<16) | ((int)(cl) << 8) | (int)(ty)),   \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von                              */
/*             Matrizen-, Vektor- und TVektor-Deskriptoren                 */
/*             --------   ------      --------------------                 */
/* Intialisierung des ersten und dritten Wortes der Deskriptoren           */

#define L_DESC(p,x)                \
  p.ref_count > 65000 ?            \
  ( DescDump(stdout,&p,0) ,        \
  post_mortem("inconsistent refcount in Descriptor") ) \
  : MEAS_CALL(MS_DESCREF),         \
  p.x

#define R_DESC(p,x) \
  ( MEAS_CALL(MS_DESCREF),         \
  p.x)



/*********************************************************************/
/* Scalar Descriptor  - SCALAR                                       */
/*********************************************************************/

#define L_SCALAR(p,x)            \
  ( p.ref_count > 65000     ||   \
    p.ref_count ==  0       ||   \
    p.class   != C_SCALAR    ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries L_SCALAR") ) \
   : MEAS_CALL(MS_DESCREF),       \
  p.u.sc.x

#define R_SCALAR(p,x)            \
 (( p.class   != C_SCALAR    ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries R_SCALAR") ) \
   : MEAS_CALL(MS_DESCREF),       \
  p.u.sc.x)

#define A_SCALAR(p,x)             \
  ( MEAS_CALL(MS_DESCREF),        \
  &p.u.sc.x)


#if LARGE
/*********************************************************************/
/* Digit Descriptor   - DIGIT                                       */
/*********************************************************************/

#define L_DIGIT(p,x)             \
  ( p.ref_count > 65000     ||   \
    p.ref_count ==  0       ||   \
    p.class   != C_DIGIT     ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries L_DIGIT") ) \
  : MEAS_CALL(MS_DESCREF),       \
  p.u.di.x

#define R_DIGIT(p,x)            \
 (( p.type    != TY_DIGIT   ||   \
    p.class   != C_DIGIT     ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries R_DIGIT") ) \
  : MEAS_CALL(MS_DESCREF),       \
  p.u.di.x)

#define A_DIGIT(p,x)             \
  ( MEAS_CALL(MS_DESCREF),        \
  &p.u.di.x)
#endif


/*********************************************************************/
/* List Descriptor    - LIST                                         */
/*********************************************************************/

#define L_LIST(p,x)            \
  ( p.ref_count > 65000     ||   \
    p.ref_count ==  0       ||   \
    p.class   != C_LIST    ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries L_LIST") ) \
  : MEAS_CALL(MS_DESCREF),       \
  p.u.li.x

#define R_LIST(p,x)            \
 ((((p.type   != TY_UNDEF) &&  \
   (p.type    != TY_STRING))|| \
   (p.class   != C_LIST)   ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries R_LIST") ) \
  : MEAS_CALL(MS_DESCREF),       \
  p.u.li.x)

#define A_LIST(p,x)             \
  ( MEAS_CALL(MS_DESCREF),       \
  &p.u.li.x)


#if LARGE
/*********************************************************************/
/* Matrix / Vektor / TVector  Descriptor  ----- MVT                  */
/*********************************************************************/

#define L_MVT(p,x,class1)        \
  ( p.ref_count > 65000     ||   \
    p.ref_count ==  0       ||   \
    p.class   != class1      ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries L_MVT") ) \
   : MEAS_CALL(MS_DESCREF),      \
  p.u.mvt.x

#define R_MVT(p,x,class1)         \
 (( p.class   != class1      ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries R_MVT") ) \
   : MEAS_CALL(MS_DESCREF),      \
  p.u.mvt.x)

#define A_MVT(p,x,class1)        \
 (( p.class   != class1      ) ? \
  ( DescDump(stdout,&p,0) ,      \
  post_mortem("inconsistent descriptor entries A_MVT") ) \
   : MEAS_CALL(MS_DESCREF),      \
 &p.u.mvt.x)


#endif


/*********************************************************************/
/* Expression Descriptor - EXPR                                      */
/*********************************************************************/

#define L_EXPR(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_EXPR    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_EXPR"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.ex.x

#define R_EXPR(p,x)                                           \
 (( p.type    != TY_EXPR  ||                                  \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_EXPR"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.ex.x)

#define A_EXPR(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.ex.x)

 
/*********************************************************************/
/* Speculative Descriptor - SPEC                                     */
/*********************************************************************/

#define L_SPEC(p,x) \
  ( p.ref_count > 65000 || \
    p.ref_count == 0    || \
    p.type  != TY_SPEC  || \
    p.class != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0), \
  post_mortem("inconsistent descriptor entries L_SPEC")) \
  : MEAS_CALL(MS_DESCREF), \
  (p).u.sp.x

#define R_SPEC(p,x) \
 (( p.type  != TY_SPEC || \
    p.class != C_EXPRESSION )? \
  ( DescDump(stdout,&(p),0), \
  post_mortem("inconsistent descriptor entries R_SPEC")) \
  : MEAS_CALL(MS_DESCREF), \
  (p).u.sp.x)

#define A_SPEC(p,x) \
  ( MEAS_CALL(MS_DESCREF), \
  &(p).u.sp.x)


/*********************************************************************/
/* Function Descriptor   - FUNC                                      */
/*********************************************************************/

#define L_FUNC(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    ((p.type != TY_SUB) && (p.type != TY_REC)) ||             \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_FUNC"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.fu.x

#define R_FUNC(p,x)                                           \
 (( ((p.type != TY_SUB) && (p.type != TY_REC)) ||             \
    p.class   != C_EXPRESSION ) ?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_FUNC"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.fu.x)

#define A_FUNC(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.fu.x)

/*********************************************************************/ 
/* Expression Descriptor - CLOS
/*********************************************************************/

#define L_CLOS(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    (p.type != TY_CLOS )  ||                                  \
    p.class   != C_EXPRESSION )?                              \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries _CLOS"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.cl.x

#define R_CLOS(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  p.u.cl.x)

#define A_CLOS(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.cl.x)


/*********************************************************************/ 
/* Expression Descriptor - P_CLOS
/*********************************************************************/

#define L_P_CLOS(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    (p.type != TY_P_CLOS )  ||                                  \
    p.class   != C_EXPRESSION )?                              \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries _P_CLOS"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.pcl.x

#define R_P_CLOS(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  p.u.pcl.x)

#define A_P_CLOS(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.pcl.x)

/*********************************************************************/
/* Conditional Descriptor - COND                                     */
/*********************************************************************/

#define L_COND(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_COND ||                              \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_COND"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.co.x

#define R_COND(p,x)                                           \
 (( p.type    != TY_COND ||                              \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_COND"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.co.x)

#define A_COND(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.co.x)


/*********************************************************************/
/* Variable Descriptor - VAR                                         */
/*********************************************************************/

#define L_VAR(p,x)                                            \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_VAR     ||                                \
    p.class   != C_CONSTANT   )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_VAR") )       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.va.x

#define R_VAR(p,x)                                            \
  (( p.type    != TY_VAR     ||                                \
    p.class   != C_CONSTANT   )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_VAR") )       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.va.x)

#define A_VAR(p,x)                                            \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.va.x)


/*********************************************************************/
/* Name Descriptor - NAME                                            */
/*********************************************************************/

#define L_NAME(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_NAME    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_NAME"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.na.x

#define R_NAME(p,x)                                           \
 (( p.type    != TY_NAME    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_NAME"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.na.x)

#define A_NAME(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.na.x)



/*********************************************************************/
/* Funktionname Descriptor FNAME                                     */
/*********************************************************************/

#define L_FNAME(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_FNAME    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_FNAME"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.fn.x

#define R_FNAME(p,x)                                           \
 (( p.type    != TY_FNAME    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_FNAME"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.fn.x)

#define A_FNAME(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.fn.x)


/*********************************************************************/
/* Switch Descriptor - SWITCH                             BAR        */
/*********************************************************************/

#define L_SWITCH(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_SWITCH    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_SWITCH"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.sw.x

#define R_SWITCH(p,x)                                 \
 (( p.type    != TY_SWITCH    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_SWITCH"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.sw.x)

#define A_SWITCH(p,x)                                 \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.sw.x)


/*********************************************************************/
/* Match Descriptor - MATCH                               BAR        */
/*********************************************************************/

#define L_MATCH(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_MATCH    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_MATCH"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.ma.x

#define R_MATCH(p,x)                                 \
 (( p.type    != TY_MATCH    ||                                \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_MATCH"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.ma.x)

#define A_MATCH(p,x)                                 \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.ma.x)


/*********************************************************************/
/* Nomat(ch) Descriptor - NOMAT                           BAR        */
/*********************************************************************/

#define L_NOMAT(p,x)                                            \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    p.type    != TY_NOMAT     ||                                \
    p.class   != C_EXPRESSION   )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries L_NOMAT") )       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.no.x

#define R_NOMAT(p,x)                                            \
 (( p.type    != TY_NOMAT     ||                                \
    p.class   != C_EXPRESSION   )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries R_NOMAT") )       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.no.x)

#define A_NOMAT(p,x)                                            \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.no.x)

/*********************************************************************/
/* Expression Descriptor - LETREC                            */ /* CS */
/*********************************************************************/

#define L_LREC(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    (p.type != TY_LREC)   ||             \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries _LREC"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.lr.x

#define R_LREC(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  p.u.lr.x)

#define A_LREC(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.lr.x)

/*********************************************************************/
/* Expression Descriptor - LETREC_IND                         *//* CS */
/*********************************************************************/

#define L_LREC_IND(p,x)                                           \
  ( p.ref_count > 65000   ||                                  \
    p.ref_count == 0      ||                                  \
    (p.type != TY_LREC_IND)   ||             \
    p.class   != C_EXPRESSION )?        \
  ( DescDump(stdout,&p,0) ,                                   \
  post_mortem("inconsistent descriptor entries _LREC_IND"))       \
  : MEAS_CALL(MS_DESCREF),                                    \
  p.u.ri.x

#define R_LREC_IND(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  p.u.ri.x)

#define A_LREC_IND(p,x)                                           \
  ( MEAS_CALL(MS_DESCREF),                                    \
  &p.u.ri.x)




#define L_FREE(p,x)                                         \
  (!(isdesc(&p)))  ?                                        \
  ( DescDump (stdout,p,0) ,                                 \
  post_mortem("not a pointer to descriptor L_FREE"))        \
  : MEAS_CALL(MS_DESCREF),                                  \
  p.u.fr.x

#define R_FREE(p,x)                                         \
  (MEAS_CALL(MS_DESCREF),                                   \
  p.u.fr.x)

#define A_FREE(p,x)                                         \
  &p.u.fr.x

/********************************************************************/
/* Process descriptor                                       *//* AE */
/********************************************************************/

#define L_PROCESS(p,x)                                             \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_PROCESS)) ?                                       \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_PROCESS"))        \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.pr.x

#define R_PROCESS(p,x)                                             \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.pr.x)

#define A_PROCESS(p,x)                                             \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.pr.x)

#define L_PROCESS_FLAGS(p,x,n)                                     \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    n > 15               ||                                        \
    n < 0                ||                                        \
    (p.class != C_PROCESS)) ?                                       \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_PROCESS"))        \
  : MEAS_CALL(MS_DESCREF),                                         \
  (p.u.pr.x |= (1 << n))

#define R_PROCESS_FLAGS(p,x,n)                                     \
( MEAS_CALL(MS_DESCREF),                                           \
  (((p.u.pr.x)>>n) & 1)
/********************************************************************/
/* Stream descriptor                                        *//* AE */
/********************************************************************/

#define L_STREAM(p,x)                                              \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_STREAM)) ?                                        \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_STREAM"))         \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.st.x

#define R_STREAM(p,x)                                              \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.st.x)

#define A_STREAM(p,x)                                              \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.st.x)

/********************************************************************/
/* field descriptor                                         *//* AE */
/********************************************************************/

#define L_GFIELD(p,x)                                              \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_GFIELD)) ?                                        \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_GFIELD"))          \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.fi.x

#define R_GFIELD(p,x)                                              \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.fi.x)

#define A_GFIELD(p,x)                                              \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.fi.x)

/********************************************************************/
/* semaphor descriptor                                      *//* AE */
/********************************************************************/

#define L_SEMAPHOR(p,x)                                            \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_SEMAPHOR)) ?                                      \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_SEMAPHOR"))       \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.se.x

#define R_SEMAPHOR(p,x)                                            \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.se.x)

#define A_SEMAPHOR(p,x)                                            \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.se.x)

/********************************************************************/
/* merge-field descriptor                                   *//* AE */
/********************************************************************/

#define L_MERGE_FIELD(p,x)                                         \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_MERGE_FIELD)) ?                                   \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_MERGE_FIELD"))    \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.mf.x

#define R_MERGE_FIELD(p,x)                                         \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.mf.x)

#define A_MERGE_FIELD(p,x)                                         \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.mf.x)

/********************************************************************/
/* merge-field descriptor                                   *//* AE */
/********************************************************************/

#define L_MERGE_STREAM(p,x)                                        \
  ( p.ref_count > 65000  ||                                        \
    p.ref_count == 0     ||                                        \
    (p.class != C_MERGE_STREAM)) ?                                  \
  ( DescDump (stdout,&p,0) ,                                       \
  post_mortem("inconsistent descriptor entries L_MERGE_STREAM"))   \
  : MEAS_CALL(MS_DESCREF),                                         \
  p.u.ms.x

#define R_MERGE_STREAM(p,x)                                        \
  ( MEAS_CALL(MS_DESCREF),                                         \
  p.u.ms.x)

#define A_MERGE_STREAM(p,x)                                        \
  ( MEAS_CALL(MS_DESCREF),                                         \
 &p.u.ms.x)

      /* end DEBUG      */
#else
      /* start NO-DEBUG */

/*********************************************************************/
/* Descriptor Level                                                  */
/*********************************************************************/

/*  Die Or- und Shift-operationen aller DESC_MASK Makros werden zur        */
/*  Compilezeit durchgefuehrt, da die Argumente des Makros fast immer      */
/*  Konstanten sein werden.                                                */

#define LI_DESC_MASK(p,ref,cl,ty)             \
  (*(int*) p) = (ref <<16) | (cl << 8) | ty , \
  (*(((int*)p)+1) = 0),                       \
  (*(((int*)p)+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von Listen-Deskriptoren          */
/* Das Makro initialisiert                                                 */
/*              das erste Wort (refcount / class / type)                   */
/*              das zweite Wort (special / dim) und                        */
/*              das dritte Wort (ptdd) des Listen-Deskriptors.             */
/* Bemerkung:                                                              */
/*      Die Or- und Shift-operationen werden zur Compilezeit durchgefuehrt,*/
/*      da die Argumente des Makros fast immer Konstanten sein werden.     */

#define MVT_DESC_MASK(p,ref,cl,ty)            \
  (*(int*) (p)) = ((ref) <<16) | ((int)(cl) << 8) | (int)(ty) , \
  (*(((int*)(p))+2) = 0)
/* Benutzung : Nur fuer Initialisierungen von                              */
/*             Matrizen-, Vektor- und TVektor-Deskriptoren                 */
/*             --------   ------      --------------------                 */
/* Intialisierung des ersten und dritten Wortes der Deskriptoren           */

#define DESC_MASK(p,ref,cl,ty)                \
  (*(int*) (p)) = ((ref) <<16) | ((int)(cl) << 8) | (int)(ty) , \
  (*(((int*)(p))+1) = 0)
/* Benutzung : Initialisierungen aller sonstigen Deskriptoren              */
/* Das Makro DESC_MASK initialisiert                                       */
/*           das erste Wort (refcount / class / type)  und                 */
/*           das zweite Worte des Deskriptors.                             */

#define L_DESC(p,x) \
  p.x

#define R_DESC(p,x) \
  p.x

/*********************************************************************/
/* Scalar Descriptor  - SCALAR                                       */
/*********************************************************************/

#define L_SCALAR(p,x)            \
  p.u.sc.x

#define R_SCALAR(p,x)            \
  p.u.sc.x

#define A_SCALAR(p,x)             \
  &p.u.sc.x


/*********************************************************************/
/* Digit Descriptor  - DIGIT                                        */
/*********************************************************************/

#define L_DIGIT(p,x)             \
  p.u.di.x

#define R_DIGIT(p,x)            \
  p.u.di.x

#define A_DIGIT(p,x)             \
  &p.u.di.x


/*********************************************************************/
/* List Descriptor                                                   */
/*********************************************************************/

#define L_LIST(p,x)            \
  p.u.li.x

#define R_LIST(p,x)            \
  p.u.li.x

#define A_LIST(p,x)             \
  &p.u.li.x


/*********************************************************************/
/* Matrix / Vektor / TVector  Descriptor  ----- MVT                  */
/*********************************************************************/

#define L_MVT(p,x,class1)        \
  p.u.mvt.x

#define R_MVT(p,x,class1)         \
  p.u.mvt.x

#define A_MVT(p,x,class1)         \
  &p.u.mvt.x




/*********************************************************************/
/* Expression Descriptor - EXPR                                      */
/*********************************************************************/

#define L_EXPR(p,x)                                           \
  p.u.ex.x

#define R_EXPR(p,x)                                           \
  p.u.ex.x

#define A_EXPR(p,x)                                           \
  &p.u.ex.x               

  
/*********************************************************************/
/* Speculative Descriptor - SPEC                                     */
/*********************************************************************/

#define L_SPEC(p,x) (p).u.sp.x
#define R_SPEC(p,x) (p).u.sp.x
#define A_SPEC(p,x) &(p).u.sp.x


/*********************************************************************/
/* Speculative Descriptor - SPEC                                     */
/*********************************************************************/

#define L_SPEC(p,x) (p).u.sp.x
#define R_SPEC(p,x) (p).u.sp.x
#define A_SPEC(p,x) &(p).u.sp.x


/*********************************************************************/
/* Function Descriptor - FUNC                                        */
/*********************************************************************/

#define L_FUNC(p,x)                                           \
  p.u.fu.x

#define R_FUNC(p,x)                                           \
  p.u.fu.x

#define A_FUNC(p,x)                                           \
  &p.u.fu.x


/*********************************************************************/ 
/* Expression Descriptor - CLOS                                      */
/*********************************************************************/

#define L_CLOS(p,x)                                           \
  p.u.cl.x

#define R_CLOS(p,x)                                           \
  p.u.cl.x

#define A_CLOS(p,x)                                           \
  &p.u.cl.x

   
/*********************************************************************/ 
/* Expression Descriptor - P_CLOS                                      */
/*********************************************************************/

#define L_P_CLOS(p,x)                                           \
  p.u.pcl.x

#define R_P_CLOS(p,x)                                           \
  p.u.pcl.x

#define A_P_CLOS(p,x)                                           \
  &p.u.pcl.x

   
/*********************************************************************/
/* Conditional Descriptor - COND                                     */
/*********************************************************************/

#define L_COND(p,x)                                           \
  p.u.co.x

#define R_COND(p,x)                                           \
  p.u.co.x

#define A_COND(p,x)                                           \
  &p.u.co.x


/*********************************************************************/
/* Variable Descriptor - VAR                                         */
/*********************************************************************/

#define L_VAR(p,x)                                            \
  p.u.va.x

#define R_VAR(p,x)                                            \
  p.u.va.x

#define A_VAR(p,x)                                            \
  &p.u.va.x


/*********************************************************************/
/* Funktionname Descriptor FNAME                                     */
/*********************************************************************/

#define L_FNAME(p,x)                                           \
  p.u.fn.x

#define R_FNAME(p,x)                                           \
  p.u.fn.x

#define A_FNAME(p,x)                                           \
  &p.u.fn.x


/*********************************************************************/
/* Name Descriptor - NAME                                            */
/*********************************************************************/

#define L_NAME(p,x)                                           \
  p.u.na.x

#define R_NAME(p,x)                                           \
  p.u.na.x

#define A_NAME(p,x)                                           \
  &p.u.na.x


/*********************************************************************/
/* Switch Descriptor - SWITCH                             BAR        */
/*********************************************************************/

#define L_SWITCH(p,x)                \
  p.u.sw.x

#define R_SWITCH(p,x)                \
  p.u.sw.x

#define A_SWITCH(p,x)                 \
  &p.u.sw.x

/*********************************************************************/
/* Match Descriptor - MATCH                               BAR        */
/*********************************************************************/

#define L_MATCH(p,x)                  \
  p.u.ma.x

#define R_MATCH(p,x)                   \
  p.u.ma.x

#define A_MATCH(p,x)                   \
  &p.u.ma.x


/*********************************************************************/
/* Nomat(ch) Descriptor - NOMAT                           BAR        */
/*********************************************************************/

#define L_NOMAT(p,x)                    \
  p.u.no.x

#define R_NOMAT(p,x)                    \
  p.u.no.x

#define A_NOMAT(p,x)                     \
  &p.u.no.x

/*********************************************************************/
/* Expression Descriptor - LETREC                             *//* CS */
/*********************************************************************/

#define L_LREC(p,x)                                           \
  p.u.lr.x

#define R_LREC(p,x)                                           \
  p.u.lr.x

#define A_LREC(p,x)                                           \
  &p.u.lr.x

/*********************************************************************/
/* Expression Descriptor - LETREC_IND                         *//* CS */
/*********************************************************************/

#define L_LREC_IND(p,x)                                           \
  p.u.ri.x

#define R_LREC_IND(p,x)                                           \
  p.u.ri.x

#define A_LREC_IND(p,x)                                           \
  &p.u.ri.x

/********************************************************************/
/* Freizellen Descriptor FREE                               *//* AE */
/********************************************************************/

#define L_FREE(p,x)                                                \
  p.u.fr.x

#define R_FREE(p,x)                                                \
  p.u.fr.x

#define A_FREE(p,x)                                                \
  &p.u.fr.x

/********************************************************************/
/* Process descriptor                                       *//* AE */
/********************************************************************/

#define L_PROCESS(p,x)                                             \
  p.u.pr.x

#define R_PROCESS(p,x)                                             \
  p.u.pr.x

#define A_PROCESS(p,x)                                             \
 &p.u.pr.x

#define L_PROCESS_FLAGS(p,x,n)                                     \
  (p.u.pr.x |= (1 << n))

#define R_PROCESS_FLAGS(p,x,n)                                     \
  (((p.u.pr.x)>>n) & 1)
/********************************************************************/
/* Stream descriptor                                        *//* AE */
/********************************************************************/

#define L_STREAM(p,x)                                              \
  p.u.st.x

#define R_STREAM(p,x)                                              \
  p.u.st.x

#define A_STREAM(p,x)                                              \
 &p.u.st.x

/********************************************************************/
/* field descriptor                                         *//* AE */
/********************************************************************/

#define L_GFIELD(p,x)                                              \
  p.u.fi.x

#define R_GFIELD(p,x)                                              \
  p.u.fi.x

#define A_GFIELD(p,x)                                              \
 &p.u.fi.x

/********************************************************************/
/* semaphor descriptor                                      *//* AE */
/********************************************************************/

#define L_SEMAPHOR(p,x)                                            \
  p.u.se.x

#define R_SEMAPHOR(p,x)                                            \
  p.u.se.x

#define A_SEMAPHOR(p,x)                                            \
 &p.u.se.x

/********************************************************************/
/* merge-field descriptor                                   *//* AE */
/********************************************************************/

#define L_MERGE_FIELD(p,x)                                         \
  p.u.mf.x

#define R_MERGE_FIELD(p,x)                                         \
  p.u.mf.x

#define A_MERGE_FIELD(p,x)                                         \
 &p.u.mf.x

/********************************************************************/
/* merge-field descriptor                                   *//* AE */
/********************************************************************/

#define L_MERGE_STREAM(p,x)                                        \
  p.u.ms.x

#define R_MERGE_STREAM(p,x)                                        \
  p.u.ms.x

#define A_MERGE_STREAM(p,x)                                        \
 &p.u.ms.x

#endif     /* DEBUG  , NODEBUG  */

/* end of RHEAPTY.H */

