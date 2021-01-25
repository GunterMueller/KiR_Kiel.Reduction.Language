extern int UseTypes;
extern int ReduceRefCnt;

#if    DEBUG
extern FILE    * TraceFp;          /* setup.c */
extern int     OutInstr;           /* setup.c ausgabe von instruktionen */
extern int     OutLen;             /* setup.c laenge des zyklischen ausgabebereiches */
extern int     debdisplay;         /* rruntime.c */
extern void    stackname_dr();     /* debug system */
extern void    cursorxy();         /* editor: cursor motion */
extern void    DescDump();
#endif

extern T_PTD   newdesc();          /* rheap.c allocate a new descriptor */           
extern int newheap();              /* rheap.c */                                     
extern T_PTD   gen_id();           /* rlstruct.c generate indirect list descriptor */
extern char    * stelname();       /* rdesc.c */
extern void printelements();       /* rextcode.c */
extern void test_inc_refcnt();             /* rheap.c */   
extern void rel_heap();                    /* rheap.c */   
extern void res_heap();                    /* rheap.c */   
extern void switchstacks();                /* rruntime.c */
extern void stack_error();                 /* rstack.c */
extern void test_dec_refcnt();             /* rheap.c */
extern int post_mortem();

extern char CodeFile2[];

#define IS_BOOLEAN(p)	( (R_DESC((*R_LINKNODE((*p), ptype)), class) == C_ATOMTYPE) \
			  && (R_DESC((*R_LINKNODE((*p), ptype)), type) == TY_BOOL) )
#define IS_UNBOXED(p)	( (R_DESC((*R_LINKNODE((*p), ptype)), class) == C_ATOMTYPE) \
			  && ((R_DESC((*R_LINKNODE((*p), ptype)), type) == TY_INTEGER) \
			     || (R_DESC((*R_LINKNODE((*p), ptype)), type) == TY_BOOL) ) )

typedef struct { int instrid;
                 char * name;
                 int nargs;
                 T_PTD (* restypefkt)();
                 T_PTD * resulttype;
                 T_PTD * argtype[4];
               } T_asm_primf_fkt;

extern T_asm_primf_fkt asm_primf_fkt[];
extern T_PTD undef, bool, integer, real, boolvect, intvect, realvect,
             booltvect, inttvect, realtvect, boolmat, intmat, realmat;

extern T_PTD lookupprimf(int);
extern T_PTD getretprimf(int, char *);
extern T_PTD lookupuserf(T_PTD);
extern T_PTD getretuserf(T_PTD);
extern char * type2str(T_PTD, char *);
extern int unify(T_PTD, T_PTD);
extern int equaltype(T_PTD, T_PTD);
extern void mkcode(char *, ...);
extern void mkdesc(T_PTD, char *, char *, int, int, T_PTD, char *, int *);
extern void mklabel(char *, T_PTD);
extern void mkcomment(char *, ...);

#define POP_W()           POPSTACK(*ps_w)
#define PPOP_W()          PPOPSTACK(*ps_w)
#define TOP_W()           READSTACK(*ps_w)
#define PUSH_W(x)         PUSHSTACK(*ps_w,(x))
#define PPUSH_W(x)        PPUSHSTACK(*ps_w,(x))
#define WRITE_W(x)        WRITESTACK(*ps_w,(x))
#define MID_W(n)          MIDSTACK(*ps_w,n)
#define UPDATE_W(n,x)     UPDATESTACK(*ps_w,n,(x))

#define POP_A()           POPSTACK(*ps_a)
#define PPOP_A()          PPOPSTACK(*ps_a)
#define TOP_A()           READSTACK(*ps_a)
#define PUSH_A(x)         PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        WRITESTACK(*ps_a,(x))
#define MID_A(x)          MIDSTACK(*ps_a,x)
#define UPDATE_A(n,x)     UPDATESTACK(*ps_a,n,(x))

#if WITHTILDE
#define POP_R()           POPSTACK(*ps_r)
#define PPOP_R()          PPOPSTACK(*ps_r)
#define TOP_R()           READSTACK(*ps_r)
#define PUSH_R(x)         PUSHSTACK(*ps_r,(x))
#define PPUSH_R(x)        PPUSHSTACK(*ps_r,(x))
#define WRITE_R(x)        WRITESTACK(*ps_r,(x))
#define MID_R(x)          MIDSTACK(*ps_r,(x))
#define UPDATE_R(n,x)     UPDATESTACK(*ps_r,n,(x))
#else
#define POP_R()           POPSTACK(S_m)
#define PPOP_R()          PPOPSTACK(S_m)
#define TOP_R()           READSTACK(S_m)
#define PUSH_R(x)         PUSHSTACK(S_m,(x))
#define PPUSH_R(x)        PPUSHSTACK(S_m,(x))
#define WRITE_R(x)        WRITESTACK(S_m,(x))
#define MID_R(x)          MIDSTACK(S_m,(x))
#define UPDATE_R(n,x)     UPDATESTACK(S_m,n,(x))
#endif /* WITHTILDE */

#define POP_E()           POPSTACK(S_e)
#define PPOP_E()          PPOPSTACK(S_e)
#define TOP_E()           READSTACK(S_e)
#define PUSH_E(x)         PUSHSTACK(S_e,(x))
#define PPUSH_E(x)        PPUSHSTACK(S_e,(x))
#define WRITE_E(x)        WRITESTACK(S_e,(x))
#define MID_E(x)          MIDSTACK(S_e,(x))
#define UPDATE_E(n,x)     UPDATESTACK(S_e,n,(x))

/* mah 261093 */
#if WITHTILDE
#define POP_T()           POPSTACK(*ps_t)
#define PPOP_T()          PPOPSTACK(*ps_t)
#define TOP_T()           READSTACK(*ps_t)
#define PUSH_T(x)         PUSHSTACK(*ps_t,(x))
#define PPUSH_T(x)        PPUSHSTACK(*ps_t,(x))
#define WRITE_T(x)        WRITESTACK(*ps_t,(x))
#define MID_T(x)          MIDSTACK(*ps_t,(x))
#define UPDATE_T(n,x)     UPDATESTACK(*ps_t,n,(x))
#endif /* WITHTILDE */

#if DEBUG
extern STACKELEM *ppopstack();             /* rheap.c */
extern STACKELEM midstack();               /* rstack.c */
extern STACKELEM readstack();              /* rstack.c */
extern void updatestack();                 /* rstack.c */
#endif /* DEBUG */

extern StackDesc *ps_w;
