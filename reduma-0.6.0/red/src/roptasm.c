/* $Log roptasm.c,v $
 */

/* file roptasm.c
 * --------------
 * 
 * car  1.06.94
 *
 * produce optimized ASM-code by abstract interpretation of ASM-code
 *
 * implemented optimizations (V1.0):
 * - reduce reference counting to a minimum
 * - infer types to the code by analyzing each function seperately
 */

#define INTER "Interpreter fuer abstrakten Code"

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinter.h"
#include "roptasm.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "dbug.h"

/* ATTENTION: also used for printstelem call
 */
static FILE *codefp = 0;

/* static atomic type descriptors */
T_PTD  undef, bool, integer, real,
  boolvect, intvect, realvect,
  booltvect, inttvect, realtvect,
  boolmat, intmat, realmat;

StackDesc      *ps_w;              /* zeiger auf aktuellen a-stack */
StackDesc      *ps_a;              /* zeiger auf aktuellen i-stack */

#if WITHTILDE
StackDesc      *ps_t;              /* zeiger auf aktuellen tilde-stack  mah 261093 */
StackDesc      *ps_r;              /* zeiger auf aktuellen return-stack mah 171193 */
#endif /* WITHTILDE */

static char    buf[128];            /* puffer fuer fehlermeldungen */

/* --- stackoperationen --- */


#define TAIL_FLAG            1
#define POP_RET()            (INSTR *)(POP_R() & ~TAIL_FLAG)
#define TEST_TAIL_FLAG()     ((TOP_R() & TAIL_FLAG) == TAIL_FLAG)
#define SET_TAIL_FLAG()      WRITE_R(TOP_R() | TAIL_FLAG)
#define DEL_TAIL_FLAG()      WRITE_R(TOP_R() & ~TAIL_FLAG)
#define INVERT_TAIL_FLAG()   WRITE_R(TOP_R() ^ TAIL_FLAG)
#define isundef(p)           ( (R_DESC(*R_LINKNODE((*(p)), ptype), class) == C_ATOMTYPE)   \
                                 && (R_DESC(*R_LINKNODE((*(p)), ptype), type) == TY_UNDEF) )

/* beta,tail, cond und jtrue/jfalse: parameter sind absolute sprungziele */
#define GRAB_DESC(p)      p = ((T_PTD *)p)[-1]
#define COMB_CODE(p)      p = (T_PTD)R_COMB(*p,ptc)

/* --- debug: ausgabe der auszufuehrenden instruktion --- */

#if     DEBUG

#define SWITCHSTACKS()    {register StackDesc *x = ps_w; ps_w = ps_a; ps_a = x;} \
                           switchstacks(ps_w,ps_a); \
                           fprintf(TraceFp,"_switchstacks();\n")
#define SWITCHSTACKS_T()  {register StackDesc *x = ps_r; ps_r = ps_t; ps_t = x;} \
                           switchstacks(ps_r,ps_t); \
                           fprintf(TraceFp,"_switchstacks_t();\n")

#define POST_MORTEM(mes)  {if (TraceFp) { fclose(TraceFp); TraceFp=0; } post_mortem(mes); }

extern void     printelements();

#else /* DEBUG */

#define SWITCHSTACKS()    {register StackDesc *x = ps_w; ps_w = ps_a; ps_a = x;}
#define SWITCHSTACKS_T()  {register StackDesc *x = ps_r; ps_r = ps_t; ps_t = x;}

#define POST_MORTEM(mes)  post_mortem(mes)

#endif/* DEBUG */

/* --- operators on return adresses combined with tail end recursion --- */

/* -------------------------------------------------------------------- */
/* ---                  miscellaneous functions                     --- */
/* -------------------------------------------------------------------- */

#if DEBUG
static int  instrcnt = 0;
static int  relpos   = 0;
static int  baserow  = 2;
static int  basecol  = 0;
#endif DEBUG

#define NEXTINSTR()       cursorxy(baserow+(relpos=(relpos+1)%OutLen),basecol); \
                          printf("%d ",++instrcnt)

/* echo ASM code to codefile and (if DEBUG) to terminal
 */
void mkcode(char * fmt, ...)
  {
  va_list ap;

  va_start(ap, fmt);
#if DEBUG
  if (OutInstr && debdisplay) {
    NEXTINSTR();
    printf("      ");
    vprintf(fmt, ap);
    printf("                             \n");
    }
#endif /* DEBUG */
  fprintf(codefp, "\t");
  vfprintf(codefp, fmt, ap);
  fprintf(codefp, "\n");
  va_end(ap);
  }

/* echo ASM code descriptor to codefile and (if DEBUG) to terminal
 */
void mkdesc(T_PTD desc, char * cl, char * ty, int args, int nargs,
            T_PTD ptd, char * prefix, int * code)
  {
#if DEBUG
  if (OutInstr && debdisplay) {
    NEXTINSTR();
    printf("      _desc(0x%08x,%s,%s,%d,%d,%x,%s_%p)\n",
      (int)desc, cl, ty, args, nargs, (int)ptd,
      (strcmp(prefix, "cond") == 0 ? "jcond" :
          (strcmp(prefix, "start") == 0 ? "func" : prefix)), desc);
    NEXTINSTR();
    printf("%s_%p:                        \n", prefix, desc);
    }
#endif /* DEBUG */
  fprintf(codefp, "\t_desc(0x%08x,%s,%s,%d,%d,%x,%s_%p)\n",
    (int)desc, cl, ty, args, nargs, (int)ptd,
    (strcmp(prefix, "cond") == 0 ? "jcond" :
        (strcmp(prefix, "start") == 0 ? "func" : prefix)), desc);
  fprintf(codefp, "%s_%p:\n", prefix, desc);
  }

/* echo ASM code label to codefile and (if DEBUG) to terminal
 */
void mklabel(char * prefix, T_PTD desc)
  {
#if DEBUG
  if (OutInstr && debdisplay) {
    NEXTINSTR();
    printf("%s_%p:                        \n", prefix, desc);
    }
#endif /* DEBUG */
  fprintf(codefp, "%s_%p:\n", prefix, desc);
  }

/* echo comment to codefile and (if DEBUG) to terminal
 */
void mkcomment(char * fmt, ...)
  {
  va_list ap;

  va_start(ap, fmt);
#if DEBUG
  if (OutInstr && debdisplay) {
    NEXTINSTR();
    printf("# ");
    vprintf(fmt, ap);
    printf("                        \n");
    }
#endif /* DEBUG */
  fprintf(codefp, "# ");
  vfprintf(codefp, fmt, ap);
  fprintf(codefp, "\n");
  va_end(ap)
  }

/* enclose a type into a fresh link node
 */
T_PTD enclosetype(int mode, int refcnt, T_PTD ptype)
  {
  T_PTD node;

  MAKEDESC(node, 1, C_LINKNODE, mode);
  L_LINKNODE((*node), refcnt) = refcnt;
  L_LINKNODE((*node), ptype) = (UseTypes ? ptype : undef);
  return node;
  }

/* test if a function has been already evaluated
 */
int testnewfunc(T_PTD pdesc)
  {
  int i;

  for(i = SIZEOFSTACK(S_v)-1; i>=0; i--)
    if ((T_PTD)MIDSTACK(S_v, i) == pdesc)
      break;
  if (i < 0) {
    PUSHSTACK(S_i, pdesc);
    INC_REFCNT(pdesc);
    }
  return (i < 0);
  }

/* push type of constant to W-Stack, allocate new link node
 */
typedef struct {
  int class;
  int type;
  T_PTD *pptype;
  } ptr_type;

ptr_type ptr_types[] = {
  { C_SCALAR, TY_REAL, &real },
  { C_VECTOR, TY_BOOL, &boolvect },
  { C_VECTOR, TY_INTEGER, &intvect },
  { C_VECTOR, TY_REAL, &realvect },
  { C_TVECTOR, TY_BOOL, &booltvect },
  { C_TVECTOR, TY_INTEGER, &inttvect },
  { C_TVECTOR, TY_REAL, &realtvect },
  { C_MATRIX, TY_BOOL, &boolmat },
  { C_MATRIX, TY_INTEGER, &intmat },
  { C_MATRIX, TY_REAL, &realmat },
  };

T_PTD typeconst(int stelem)
  {
  int i, n;
  int cl, ty;
  T_PTD ptype = NULL;

  /* during the type detection the refcount of the type descriptor
   * has to be 2
   * otherwise it isn't possible to compute the type of a primfunc/list!
   */
  if (T_INT(stelem)) {
    ptype = integer;
    INC_REFCNT(ptype);
    }
  else if (T_BOOLEAN(stelem)) {
    ptype = bool;
    INC_REFCNT(ptype);
    }
  else if (T_FUNC(stelem))
    ptype = lookupprimf(FUNC_INDEX(stelem));
  else if (T_POINTER(stelem)) {
    cl = R_DESC((*(T_PTD)stelem), class);
    ty = R_DESC((*(T_PTD)stelem), type);
    if ( (cl == C_FUNC) && (ty == TY_COMB) ) {
      ptype = lookupuserf((T_PTD)stelem);
      INC_REFCNT(ptype);
      testnewfunc((T_PTD)stelem);
      }
    else if (cl == C_LIST) {
      T_PTD node, node2;

      n = R_LIST((*(T_PTD)stelem), dim);
      MAKEDESC(ptype, 1, C_COMPTYPE, TY_PROD);
      if (n) {
        GET_HEAP(1, A_COMPTYPE((*ptype), ptypes));
        L_COMPTYPE((*ptype), cnt) = 1;
          L_COMPTYPE((*ptype), ptypes)[0] = (T_HEAPELEM)node
              = typeconst((int)((T_PTD)R_LIST((*(T_PTD)stelem), ptdv)[0]));
        for(i = n-1;  i > 0; i--) {
          if (equaltype(node,
              node2 = typeconst((int)((T_PTD)R_LIST((*(T_PTD)stelem), ptdv)[i]))) == 0) {
            DEC_REFCNT(node);
            L_COMPTYPE((*ptype), ptypes)[0] = (T_HEAPELEM)undef;
            INC_REFCNT(undef);
            break;
            }
          DEC_REFCNT(node2);
          }
        }
      else {
        L_COMPTYPE((*ptype), ptypes) = NULL;
        L_COMPTYPE((*ptype), cnt) = 0;
        }
      }
    else {
      n = sizeof(ptr_types)/sizeof(ptr_type);
      for(i = 0; i < n; i++)
        if ( (cl == ptr_types[i].class) && (ty == ptr_types[i].type) ) {
          ptype = *ptr_types[i].pptype;
          INC_REFCNT(ptype);
          break;
          }
      if (i == n) {
        fprintf(stdout, "unknown pointer type found (%d/%d).\n", cl, ty);
#if DEBUG
        DescDump(stdout, (T_PTD)stelem, 0);
#endif /* DEBUG */
        }
      }
    }
  else
    post_mortem("attempted to push unknown contant type");
  return ptype;
  }
 
/* install most general argument frame on top of the argument stack
 */
int installfunc(T_PTD func)
  {
  int   i;
  T_PTD ptype, * ptypes;

  if ((ptype = lookupuserf(func))) {
    ptypes = (T_PTD *)&(R_COMPTYPE((*ptype), ptypes)[R_COMPTYPE((*ptype), cnt)-1]);
    for(i = R_COMPTYPE((*ptype), cnt)-1; i > 0; i--, ptypes--) {
      PUSH_A(enclosetype(TY_UNDEF, 0, *ptypes));
      INC_REFCNT(*ptypes);
      }
    return 1;
    }
  else {
    for(i = R_COMB((*func), nargs); i > 0; i--) {
      PUSH_A(enclosetype(TY_UNDEF, 0, undef));
      INC_REFCNT(undef);
      }
    return 0;
    }
  }

/* -------------------------------------------------------------------- */
/* ---                    abstract interpreter                      --- */
/* -------------------------------------------------------------------- */

#define CON_COND1	AP
#define CON_COND2	SNAP
#define CON_JUMP	PRELIST
#define CON_END		PROTECT

#define T_COND1(x)	T_AP(x)
#define T_COND2(x)	T_SNAP(x)
#define T_JUMP(x)	T_PRELIST(x)
#define T_END(x)	T_PROTECT(x)

void optasm(INSTR * VOLATILE code)
  {
  int     i, n, ntype = -1, opcode = -1;
  T_PTD   pdesc, node, node2;
  char  * cmd;
  char    name[32];
  DBUG_ENTER ("optasm");
  START_MODUL("optasm");

  ps_w = &S_a;          /* initial working  stack is mapped on S_a  */
  ps_a = &S_m1;         /* initial argument stack is mapped on S_m1 */
# if WITHTILDE
  ps_t = &S_tilde;      /* initial tilde stack is mapped on S_tilde     */
  ps_r = &S_m;          /* initial return/tilde2stack is mapped on S_m */
# endif

# if (DEBUG && !D_SLAVE)
  strcpy(S_e.Name,     "H"); stackname_dr(&S_e);
  strcpy(S_a.Name,     "W"); stackname_dr(&S_a);
  strcpy(S_m1.Name,    "A"); stackname_dr(&S_m1);
  strcpy(S_m.Name,     "R"); stackname_dr(&S_m);
#   if WITHTILDE
  strcpy(S_tilde.Name, "T"); stackname_dr(&S_tilde); /* mah 251093 */
#   endif
  /* initialize some debug variables */
  instrcnt = 0;         /* zaehler fuer ausgefuerte instruktionen */
  relpos   = 0;         /* zyklischer positionszahler fuer bildschirmausgabe */
# endif /* (DEBUG && !D_SLAVE) */

  MAKEDESC(undef, 1, C_ATOMTYPE, TY_UNDEF);
  MAKEDESC(bool, 1, C_ATOMTYPE, TY_BOOL);
  MAKEDESC(integer, 1, C_ATOMTYPE, TY_INTEGER);
  MAKEDESC(real, 1, C_ATOMTYPE, TY_REAL);
  MAKEDESC(boolvect, 1, C_COMPTYPE, TY_VECT);
    GET_HEAP(1, A_COMPTYPE((*boolvect), ptypes));
    L_COMPTYPE((*boolvect), cnt) = 1;
    L_COMPTYPE((*boolvect), ptypes)[0] = (T_HEAPELEM)bool;
    INC_REFCNT(bool);
  MAKEDESC(booltvect, 1, C_COMPTYPE, TY_TVECT);
    GET_HEAP(1, A_COMPTYPE((*booltvect), ptypes));
    L_COMPTYPE((*booltvect), cnt) = 1;
    L_COMPTYPE((*booltvect), ptypes)[0] = (T_HEAPELEM)bool;
    INC_REFCNT(bool);
  MAKEDESC(boolmat, 1, C_COMPTYPE, TY_MAT);
    GET_HEAP(1, A_COMPTYPE((*boolmat), ptypes));
    L_COMPTYPE((*boolmat), cnt) = 1;
    L_COMPTYPE((*boolmat), ptypes)[0] = (T_HEAPELEM)bool;
    INC_REFCNT(bool);
  MAKEDESC(intvect, 1, C_COMPTYPE, TY_VECT);
    GET_HEAP(1, A_COMPTYPE((*intvect), ptypes));
    L_COMPTYPE((*intvect), cnt) = 1;
    L_COMPTYPE((*intvect), ptypes)[0] = (T_HEAPELEM)integer;
    INC_REFCNT(integer);
  MAKEDESC(inttvect, 1, C_COMPTYPE, TY_TVECT);
    GET_HEAP(1, A_COMPTYPE((*inttvect), ptypes));
    L_COMPTYPE((*inttvect), cnt) = 1;
    L_COMPTYPE((*inttvect), ptypes)[0] = (T_HEAPELEM)integer;
    INC_REFCNT(integer);
  MAKEDESC(intmat, 1, C_COMPTYPE, TY_MAT);
    GET_HEAP(1, A_COMPTYPE((*intmat), ptypes));
    L_COMPTYPE((*intmat), cnt) = 1;
    L_COMPTYPE((*intmat), ptypes)[0] = (T_HEAPELEM)integer;
    INC_REFCNT(integer);
  MAKEDESC(realvect, 1, C_COMPTYPE, TY_VECT);
    GET_HEAP(1, A_COMPTYPE((*realvect), ptypes));
    L_COMPTYPE((*realvect), cnt) = 1;
    L_COMPTYPE((*realvect), ptypes)[0] = (T_HEAPELEM)real;
    INC_REFCNT(real);
  MAKEDESC(realtvect, 1, C_COMPTYPE, TY_TVECT);
    GET_HEAP(1, A_COMPTYPE((*realtvect), ptypes));
    L_COMPTYPE((*realtvect), cnt) = 1;
    L_COMPTYPE((*realtvect), ptypes)[0] = (T_HEAPELEM)real;
    INC_REFCNT(real);
  MAKEDESC(realmat, 1, C_COMPTYPE, TY_MAT);
    GET_HEAP(1, A_COMPTYPE((*realmat), ptypes));
    L_COMPTYPE((*realmat), cnt) = 1;
    L_COMPTYPE((*realmat), ptypes)[0] = (T_HEAPELEM)real;
    INC_REFCNT(real);

  PUSH_R(CON_END);

  if ((codefp = fopen(CodeFile2, "w")) == NULL)
    post_mortem("optasm: can't open codefile");

  mkcomment("check outfile!!");
  pdesc = (T_PTD)code;
  GRAB_DESC(pdesc);
  mkdesc(pdesc, "FUNC", "COMB", 0, 0, R_COMB(*pdesc,ptd), "start", code);
loop:
  cmd = NULL;
  n=0;
  switch (*code++) {
    case I_EXT: {
      DBUG_PRINT("PROTO", (" I_EXT"));
      if (UseTypes)
        mkcomment("return type: %s", type2str(R_LINKNODE((*(T_PTD)TOP_W()), ptype), NULL));
      mkcode("_ext();");
      ntype = POP_R();
      if (!T_END(ntype))
        post_mortem("nesting error in main function");
      goto loop;
      }
    case I_END: {
      DBUG_PRINT("PROTO", (" I_END"));
      /* drop result */
      mkcode("_end();");
      pdesc = (T_PTD)POP_W();
      DEC_REFCNT(pdesc);
      if (SIZEOFSTACK(S_i) > 0) {
	pdesc = (T_PTD)POPSTACK(S_i);
        PUSHSTACK(S_v, pdesc);
	code = R_COMB((*pdesc), ptc);
        if (UseTypes)
          mkcomment("%s", type2str(lookupuserf(pdesc), NULL));
	mkdesc(pdesc, "FUNC", "COMB", R_COMB((*pdesc), args),
            R_COMB((*pdesc), nargs), R_COMB(*pdesc,ptd), "func", code);
        PUSH_R(CON_END);
	installfunc(pdesc);
	goto loop;
	}
      break;
      }
    case I_PUSH_W: cmd = "push_w";
    case I_PUSHC_W: {
      DBUG_PRINT("PROTO", (" I_PUSH(C)_W"));
      node = enclosetype(TY_UNDEF, 1, typeconst(*code));
      mkcode("_%s(%s);", (cmd ? cmd : "pushc_w"), stelname(*code));
      if (cmd)
        printelements(codefp, *code, 0);
      PUSH_W(node);
      code++;
      goto loop;
      }
    case I_PUSH_AW: {
      DBUG_PRINT("PROTO", ("I_PUSH_AW"));
      mkcode("_push_aw(%d);", *code);
      node=(T_PTD)MID_A(*code);
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_W(node);
      INC_REFCNT(node);
      code++;
      goto loop;
      }
    case I_PUSH_AW0: {
      DBUG_PRINT("PROTO", ("I_PUSH_AW0"));
      mkcode("_push_aw0();");
      node=(T_PTD)TOP_A();
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_W(node);
      INC_REFCNT(node);
      goto loop;
      }
    case I_MOVE_AW: {
      DBUG_PRINT("PROTO", ("I_MOVE_AW"));
      PUSH_W(POP_A());
      mkcode("_move_aw();");
      goto loop;
      }
    case I_ADD:
      if (n == 0) { n = 2; opcode = _D_PLUS; DBUG_PRINT("PROTO", ("I_ADD")); }
    case I_SUB:
      if (n == 0) { n = 2; opcode = _D_MINUS; DBUG_PRINT("PROTO", ("I_SUB")); }
    case I_MUL:
      if (n == 0) { n = 2; opcode = _D_MULT; DBUG_PRINT("PROTO", ("I_MUL")); }
    case I_DIV:
      if (n == 0) { n = 2; opcode = _D_DIV; DBUG_PRINT("PROTO", ("I_DIV")); }
    case I_MOD:
      if (n == 0) { n = 2; opcode = _D_MOD; DBUG_PRINT("PROTO", ("I_MOD")); }
    case I_EQ:
      if (n == 0) { n = 2; opcode = _D_EQ; DBUG_PRINT("PROTO", ("I_EQ")); }
    case I_NE:
      if (n == 0) { n = 2; opcode = _D_NEQ; DBUG_PRINT("PROTO", ("I_NE")); }
    case I_LT:
      if (n == 0) { n = 2; opcode = _D_LESS; DBUG_PRINT("PROTO", ("I_LT")); }
    case I_LE:
      if (n == 0) { n = 2; opcode = _D_LEQ; DBUG_PRINT("PROTO", ("I_LE")); }
    case I_GT:
      if (n == 0) { n = 2; opcode = _D_GREAT; DBUG_PRINT("PROTO", ("I_GT")); }
    case I_GE:
      if (n == 0) { n = 2; opcode = _D_GEQ; DBUG_PRINT("PROTO", ("I_GE")); }
    case I_NEG:
      if (n == 0) { n = 2; opcode = _M_NEG; DBUG_PRINT("PROTO", ("I_NEG")); }
    case I_NOT:
      if (n == 0) { n = 2; opcode = _M_NOT; DBUG_PRINT("PROTO", ("I_NOT")); }
    case I_OR:
      if (n == 0) { n = 2; opcode = _D_OR; DBUG_PRINT("PROTO", ("I_OR")); }
    case I_AND:
      if (n == 0) { n = 2; opcode = _D_AND; DBUG_PRINT("PROTO", ("I_AND")); }
    case I_XOR:
      if (n == 0) { n = 2; opcode = _D_XOR; DBUG_PRINT("PROTO", ("I_XOR")); }
    case I_LSEL:
      if (n == 0) { n = 2; opcode = _LSELECT; DBUG_PRINT("PROTO", ("I_LSEL")); }
    case I_DELTA1:
      if (n == 0)
        { n = 1; opcode = FUNC_INDEX(*code++); DBUG_PRINT("PROTO", ("I_DELTA1")); }
    case I_DELTA2:
      if (n == 0)
        { n = 2; opcode = FUNC_INDEX(*code++); DBUG_PRINT("PROTO", ("I_DELTA2")); }
    case I_DELTA3:
      if (n == 0)
        { n = 3; opcode = FUNC_INDEX(*code++); DBUG_PRINT("PROTO", ("I_DELTA3")); }
    case I_DELTA4: {
      if (n == 0)
        { n = 4; opcode = FUNC_INDEX(*code++); DBUG_PRINT("PROTO", ("I_DELTA4")); }
      if (UseTypes && ((pdesc = getretprimf(opcode, name)) >= 0)) {
        mkcode("t = _delta%d(%s);", n, name);
        for(i = 0; i < n; i++) {
          node = (T_PTD)POP_W();
          if ((R_LINKNODE((*node), refcnt) == 0) && ! IS_UNBOXED(node))
            if (R_DESC((*node), type) == TY_FREE)
              mkcode("_%sdeallocw(%d);", isundef(node) ? "t" : "", i);
            else
              mkcode("_%sdecw(%d, 1);", isundef(node) ? "t" : "", i);
          L_LINKNODE((*node), refcnt) = 0;
          DEC_REFCNT(node);
          }
        mkcode("_freew(%d);", n);
        mkcode("_pushw(t);");
        }
      else {
        pdesc = undef;
        INC_REFCNT(undef);
        mkcode("delta%d(%s);", n, name);
        for(i = 0; i < n; i++) {
          node = (T_PTD)POP_W();
          DEC_REFCNT(node);
          }
        }
      node = enclosetype(TY_FREE, 0, pdesc);
      PUSH_W(node);
      goto loop;
      }
    case I_PUSHRET: {
      DBUG_PRINT("PROTO", ("I_PUSHRET"));
      mkcode("_pushret_aw(0x%08x);", *code);
      PUSH_R(*code);
      PUSH_R(CON_JUMP);
      code++;
      goto loop;
      }
    case I_JFALSE: cmd = "jfalse";
    case I_JTRUE: if (cmd == NULL) cmd = "jtrue";
    case I_JCOND: {
      DBUG_PRINT("PROTO", ("I_JFALSE/I_JTRUE/I_JCOND"));
      if (cmd == NULL) cmd = "jcond";
      node = (T_PTD)POP_W();
      pdesc = (T_PTD)*(int *)code; /* get reference to conditional alternative code */
      GRAB_DESC(pdesc);              /* reference to conditional descriptor */
      if (IS_BOOLEAN(node)) {
        mklabel("jcond", pdesc);
        mkcode("_%s_b(cond_%p);", cmd, pdesc);
        }
      else {
        if ((R_LINKNODE(*node, refcnt) == 0) && ! IS_UNBOXED(node)) {
          mkcode("_%sdecw(0, 1);", isundef(node) ? "t" : "");
	  L_LINKNODE(*node, refcnt) = R_LINKNODE(*node, refcnt)-1;
	  }
        mklabel("jcond", pdesc);
        mkcode("%s(cond_%p);", cmd, pdesc);
	}
      DEC_REFCNT(node);
      PUSH_R((INSTR *)*code);
      n = R_CONDI(*pdesc,nargs)-2; /* number of arguments (including predicate) */
      PUSH_R(SET_ARITY(CON_COND1,n));
      for(i = n; i >= 0; i--) {
        node = enclosetype(TY_UNDEF,
			   R_LINKNODE((*(T_PTD)MID_A(n)), refcnt),
			   R_LINKNODE((*(T_PTD)MID_A(n)), ptype));
        INC_REFCNT(R_LINKNODE((*node), ptype));
	PUSH_A(node);
	}
      code++;
      goto loop;
      }
    case I_JTAIL:
      { DBUG_PRINT("PROTO", ("I_JTAIL")); cmd = "jtail"; }
    case I_JTAIL0:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_JTAIL0")); cmd = "jtail0"; }
    case I_JTAIL1:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_JTAIL1")); cmd = "jtail1"; }
    case I_BETAQ:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETAQ")); cmd = "betaq"; }
    case I_BETAQ0:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETAQ0")); cmd = "betaq0"; }
    case I_BETAQ1:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETAQ1")); cmd = "betaq1"; }
    case I_JTAILQ:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_TAILQ")); cmd = "jtailq"; }
    case I_JTAILQ0:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_TAILQ0")); cmd = "jtailq0"; }
    case I_JTAILQ1:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_TAILQ1")); cmd = "jtailq1"; }
    case I_BETA0:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETA0")); cmd = "beta0"; }
    case I_BETA1:
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETA1")); cmd = "beta1"; }
    case I_BETA: {
      if (cmd == NULL) { DBUG_PRINT("PROTO", ("I_BETA")); cmd = "beta"; }
      pdesc = (T_PTD)*code++;
      GRAB_DESC(pdesc);
      n = R_COMB((*pdesc), nargs);
      for(i = 0; i < n; i++) {
	node = (T_PTD)MID_W(i);
        if ((R_LINKNODE((*node), refcnt) > 0) && ! IS_UNBOXED(node))
          mkcode("_%sincw(%d, %d);", isundef(node) ? "t" : "", i,
              R_LINKNODE((*node), refcnt));
        L_LINKNODE((*node), refcnt) = 0;
	L_DESC((*node), type) = TY_UNDEF;
	}
      testnewfunc(pdesc);
      node = enclosetype(TY_UNDEF, 0, getretuserf(pdesc));
      INC_REFCNT(R_LINKNODE((*node), ptype));
      mkcode("_%s(0x%08x)", cmd, pdesc);
      for(i = 0; i < n; i++) {
        node2 = (T_PTD)POP_W();
        DEC_REFCNT(node2);
	}
      PUSH_W(node);
      goto loop;
      }
    case I_MKLIST: {
      DBUG_PRINT("PROTO", ("I_MKLIST"));
      n = *code++;
      MAKEDESC(pdesc, 1, C_COMPTYPE, TY_PROD);
      GET_HEAP(1, A_COMPTYPE((*pdesc), ptypes));
      L_COMPTYPE((*pdesc), cnt) = 1;
      node2 = (T_PTD)TOP_W();
      INC_REFCNT(node2);
      for(i = n-1;  i >= 0; i--) {
        node = (T_PTD)POP_W();
        if ((R_LINKNODE((*node), refcnt) > 0) && ! IS_UNBOXED(node))
          mkcode("_%sincw(0, %d);", isundef(node) ? "t" : "",
              R_LINKNODE((*node), refcnt));
        L_LINKNODE((*node), refcnt) = 0;
        L_DESC((*node), type) = TY_UNDEF;
        if (equaltype(R_LINKNODE((*node2), ptype), R_LINKNODE((*node), ptype)) == 0) {
          INC_REFCNT(undef);
          DEC_REFCNT(node2);
          node2 = undef;
          }
        DEC_REFCNT(node);
        }
      L_COMPTYPE((*pdesc), ptypes)[0] = (T_HEAPELEM)R_LINKNODE((*node2), ptype);
      mkcode("_mklist(%d);", n);
      node = enclosetype(TY_FREE, 0, pdesc);
      PUSH_W(node);
      goto loop;
      }
    case I_FREE_A: {
      DBUG_PRINT("PROTO", ("I_FREE_A"));
      n = *code++;
      for(i = 0; i < n; i++) {
        node = (T_PTD)POP_A();
	if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	  mkcode("_%sdeca(%d, 1);", isundef(node) ? "t" : "", i);
        L_LINKNODE((*node), refcnt) = 0;
	DEC_REFCNT(node);
	}
      mkcode("_free_a(%d);", n);
      goto loop;
      }
    case I_FREE1_A: {
      DBUG_PRINT("PROTO", ("I_FREE1_A"));
      node = (T_PTD)TOP_A();
      if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	mkcode("_%sdeca(0, 1);", isundef(node) ? "t" : "");
      L_LINKNODE((*node), refcnt) = 0;
      DEC_REFCNT(node);
      i = POP_A();
      mkcode("_free1_a();");
      goto loop;
      }
    case I_RTP:
      DBUG_PRINT("PROTO", ("I_RTP"));
      pdesc = (T_PTD)*code++;
      node = enclosetype(TY_UNDEF, 1, typeconst((int)pdesc));
      PUSH_W(node);
      mkcode("_incp(0x%08x, 1);", pdesc);
      mkcode("_rtp(0x%08x);", pdesc);
      goto L_rtf_entry;
    case I_RTM:
      DBUG_PRINT("PROTO", ("I_RTM"));
      node = (T_PTD)POP_A();
      if (R_LINKNODE(*node, refcnt)) {
        mkcode("_%sinca(0, %d);", isundef(node) ? "t" : "", R_LINKNODE(*node, refcnt));
        L_LINKNODE(*node, refcnt) = 0;
        L_DESC((*node), type) = TY_UNDEF;
        }
      mkcode("_rtm();");
      PUSH_W(node);
      goto L_rtf_entry;
    case I_RTF:
      DBUG_PRINT("PROTO", ("I_RTF"));
      node = (T_PTD)TOP_W();
      if ((R_LINKNODE(*node, refcnt) > 0) && ! IS_UNBOXED(node)) {
        mkcode("_%sincw(0, %d);", isundef(node) ? "t" : "", R_LINKNODE(*node, refcnt));
        L_LINKNODE(*node, refcnt) = 0;
        L_DESC((*node), type) = TY_UNDEF;
        }
      mkcode("_rtf();");
      goto L_rtf_entry;
    case I_RTC:  {
      DBUG_PRINT("PROTO", ("I_RTC"));
      n = (int)*code++;
      cmd = "rtc";
      node = enclosetype(TY_UNDEF, 1, typeconst(n));
      PUSH_W(node);
      mkcode("_rtc(%s);", stelname(n));
    L_rtf_entry:
      ntype = POP_R();
      if (T_COND1(ntype)) {
        code = (INSTR *)POP_R();
        pdesc = (T_PTD)code;
        GRAB_DESC(pdesc);
        mkdesc(pdesc, "FUNC", "CONDI", R_CONDI((*pdesc), args),
            R_CONDI((*pdesc), nargs), R_CONDI(*pdesc,ptd), "cond", code);
	PUSH_R(node);
	PUSH_R(SET_ARITY(CON_COND2, ARITY(ntype)));
	i = POP_W();
	}
      else if (T_COND2(ntype)) {
        node2 = (T_PTD)POP_R();
	if (unify(R_LINKNODE((*node), ptype), R_LINKNODE((*node2), ptype)) == 0) {
	  i = POP_W();
	  PUSH_W(enclosetype(TY_UNDEF, 0, undef));
          INC_REFCNT(undef);
	  }
	goto L_rtf_entry;
        }
      else if (T_JUMP(ntype)) {
        code = (INSTR *)POP_R();
	}
      else if (!T_END(ntype))
        post_mortem("unkown contructor on return stack");
      goto loop;
      }
    case I_RTS: {
      DBUG_PRINT("PROTO", ("I_RTS"));
      fprintf(stderr, "I_RTS not implemented yet\n");
      goto loop;
      }
    case I_MKGCLOS:
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_MKGCLOS")); cmd = "mkgclos"; n = *code++; }
    case I_MKBCLOS:
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_MKBCLOS")); cmd = "mkbclos"; n = *code++; }
    case I_MKCCLOS:
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_MKCCLOS")); cmd = "mkcclos"; n = *code++; }
    case I_MKDCLOS:
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_MKDCLOS")); cmd = "mkdclos"; n = *code++; }
      ntype = 0;
    case I_MKAP:
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_MKAP")); cmd = "mkap"; n = *code++; ntype = 1; }
    case I_APPLY: {
      if (cmd == NULL)
        { DBUG_PRINT("PROTO", ("I_APPLY")); cmd = "apply"; n = *code++; ntype = 1; }
      mkcomment("apply-instruction not completely implemented yet");
      node = (T_PTD)POP_W();
      if ((R_LINKNODE((*node), refcnt) == 0) && ! IS_UNBOXED(node))
        if (R_DESC((*node), type) == TY_FREE)
          mkcode("_%sdeallocw(0);", isundef(node) ? "t" : "");
        else
          mkcode("_%sdecw(0, 1);", isundef(node) ? "t" : "");
      DEC_REFCNT(node);
      for(i = 0; i < n; i++) {
        node = (T_PTD)POP_W();
        if ((R_LINKNODE((*node), refcnt) > 1) && ! IS_UNBOXED(node)) {
          mkcode("_%sincw(0, %d);", isundef(node) ? "t" : "",
              R_LINKNODE((*node), refcnt));
          L_DESC((*node), type) = TY_UNDEF;
          L_LINKNODE((*node), refcnt) = 0;
          }
        DEC_REFCNT(node);
        }
      /* INSERT COMPUTATION OF NEW CODE HERE!!!
       */
      PUSH_W(enclosetype(TY_FREE, 0, undef));
      INC_REFCNT(undef);
      if (ntype)
        mkcode("_%s(%d);", cmd, n);
      else
        mkcode("_%s(%d,%d);", cmd, n, *code++);
      goto loop;
      }
    case I_MKSCLOS: {
      fprintf(stderr, "I_MKSCLOS not implemented yet\n");
      goto loop;
      }
    case I_SAPPLY: {
      DBUG_PRINT("PROTO", ("I_SAPPLY"));
      fprintf(stderr, "I_SAPPLY not implemented yet\n");
      goto loop;
      }
    case I_SRET: {
      DBUG_PRINT("PROTO", ("I_SRET"));
      fprintf(stderr, "I_SRET not implemented yet\n");
      goto loop;
      }
    case I_MKTHEN: {
      DBUG_PRINT("PROTO", ("I_MKTHEN"));
      fprintf(stderr, "I_MKTHEN not implemented yet\n");
      goto loop;
      }
    case I_MKELSE: {
      DBUG_PRINT("PROTO", ("I_MKELSE"));
      fprintf(stderr, "I_MKELSE not implemented yet\n");
      goto loop;
      }
    case I_MKCOND: {
      DBUG_PRINT("PROTO", ("I_MKCOND"));
      fprintf(stderr, "I_MKCOND not implemented yet\n");
      goto loop;
      }
    case I_CASE: {
      DBUG_PRINT("PROTO", ("I_CASE"));
      fprintf(stderr, "I_CASE not implemented yet\n");
      goto loop;
      }
    case I_WHEN: {
      DBUG_PRINT("PROTO", ("I_WHEN"));
      fprintf(stderr, "I_WHEN not implemented yet\n");
      goto loop;
      }
    case I_GUARD: {
      DBUG_PRINT("PROTO", ("I_GUARD"));
      fprintf(stderr, "I_GUARD not implemented yet\n");
      goto loop;
      }
    case I_BODY: {
      DBUG_PRINT("PROTO", ("I_BODY"));
      fprintf(stderr, "I_BODY not implemented yet\n");
      goto loop;
      }
    case I_NOMATCH: {
      DBUG_PRINT("PROTO", ("I_NOMATCH"));
      fprintf(stderr, "I_NOMATCH not implemented yet\n");
      goto loop;
      }
    case I_DUPARG: {
      DBUG_PRINT("PROTO", ("I_DUPARG"));
      fprintf(stderr, "I_DUPARG not implemented yet\n");
      goto loop;
      }
    case I_EVAL: {
      DBUG_PRINT("PROTO", ("I_EVAL"));
      fprintf(stderr, "I_EVAL not implemented yet\n");
      goto loop;
      }
    case I_EVAL0: {
      DBUG_PRINT("PROTO", ("I_EVAL0"));
      fprintf(stderr, "I_EVAL0 not implemented yet\n");
      goto loop;
      }
    case I_CONS: {
      DBUG_PRINT("PROTO", ("I_CONS"));
      fprintf(stderr, "I_CONS not implemented yet\n");
      goto loop;
      }
    case I_FCONS: {
      DBUG_PRINT("PROTO", ("I_FCONS"));
      fprintf(stderr, "I_FCONS not implemented yet\n");
      goto loop;
      }
    case I_FIRST: {
      DBUG_PRINT("PROTO", ("I_FIRST"));
      fprintf(stderr, "I_FIRST not implemented yet\n");
      goto loop;
      }
    case I_REST: {
      DBUG_PRINT("PROTO", ("I_REST"));
      fprintf(stderr, "I_REST not implemented yet\n");
      goto loop;
      }
    case I_UPDAT: {
      DBUG_PRINT("PROTO", ("I_UPDAT"));
      fprintf(stderr, "I_UPDAT not implemented yet\n");
      goto loop;
      }
    case I_MKILIST: {
      DBUG_PRINT("PROTO", ("I_MKILIST"));
      fprintf(stderr, "I_MKILIST not implemented yet\n");
      goto loop;
      }
#ifdef TESTSPACE
    case I_SPACE: {
      DBUG_PRINT("PROTO", ("I_SPACE"));
      fprintf(stderr, "I_SPACE not implemented yet\n");
      goto loop;
      }
#endif /* TESTSPACE */
#if D_DIST
    case I_PUSHH: {
      DBUG_PRINT("PROTO", ("I_PUSHH"));
      n = *code++;
      mkcode("pushh(%d);", n);
      goto loop;
      }
    case I_DIST: {
      DBUG_PRINT("PROTO", ("I_DIST"));
      n = code[0];
      for(i = 0; i < n; i++) {
        node = (T_PTD)MID_A(i);
	if ((R_LINKNODE((*node), refcnt) > 0 ) && ! IS_UNBOXED(node))
	  mkcode("_%sinca(%d, 1);", isundef(node) ? "t" : "", i);
        L_DESC((*node), type) = TY_UNDEF;
        L_LINKNODE((*node), refcnt) = 0;
	}
#if WITHTILDE
      mkcode("_dist(%x,%x,%x,%d,%d,%d)",
          code[0], code[1], code[2], code[3], code[4], code[5]);
      code += 6;
#else /* WITHTILDE */
      mkcode("_dist(%x,%x,%x,%d)", code[0], code[1], code[2], code[3]);
      code += 4;
#endif /* WITHTILDE */
      goto loop;
      }
    case I_DISTB: {
      cmd = "distb";
      n = code[0];
      for(i = 0; n; i++, n >>= 1) {
        node = (T_PTD)MID_A(i);
	if ((R_LINKNODE((*node), refcnt) > 0 ) && ! IS_UNBOXED(node))
	  mkcode("_%sinca(%d, 1);", isundef(node) ? "t" : "", i);
        L_DESC((*node), type) = TY_UNDEF;
        L_LINKNODE((*node), refcnt) = 0;
	}
#if WITHTILDE
      mkcode("_distb(%x,%x,%x,%d,%d,%d)",
        code[0], code[1], code[2], code[3], code[4], code[5]);
      code += 6;
#else /* WITHTILDE */
      mkcode("_distb(%x,%x,%x,%d)",
        code[0], code[1], code[2], code[3]);
      code += 4;
#endif /* WITHTILDE */
      goto loop;
      }
    case I_DISTEND: {
      DBUG_PRINT("PROTO", ("I_DISTEND"));
      if ((R_LINKNODE(*node, refcnt) > 0) && ! IS_UNBOXED(node)) {
        mkcode("_%sincw(0, %d);", isundef(node) ? "t" : "", R_LINKNODE(*node, refcnt));
        L_LINKNODE(*node, refcnt) = 0;
        }
      mkcode("distend();");
      goto loop;
      }
    case I_WAIT: {
      DBUG_PRINT("PROTO", ("I_WAIT"));
      mkcode("wait();");
      goto loop;
      }
    case I_POPH: {
      DBUG_PRINT("PROTO", ("I_POPH"));
      mkcode("poph();");
      goto loop;
      }
#endif /* D_DIST */
#if D_MESS
    case I_MHFREEON: {
      DBUG_PRINT("PROTO", ("I_MHFREEON"));
      mkcode("mhfreeon();");
      goto loop;
      }
    case I_MHPCMPON: {
      DBUG_PRINT("PROTO", ("I_MHPCMPON"));
      mkcode("mhpcmpon();");
      goto loop;
      }
    case I_MDESCALLON: {
      DBUG_PRINT("PROTO", ("I_MDESCALLON"));
      mkcode("mdescallon();");
      goto loop;
      }
    case I_MDESCFREON: {
      DBUG_PRINT("PROTO", ("I_MDESCFREON"));
      mkcode("mdescfreon();");
      goto loop;
      }
    case I_MSDISTON: {
      DBUG_PRINT("PROTO", ("I_MSDISTON"));
      mkcode("msdiston();");
      goto loop;
      }
    case I_MSNODIST: {
      DBUG_PRINT("PROTO", ("I_MSNODIST"));
      mkcode("msnodist();");
      goto loop;
      }
    case I_MSDISTCK: {
      DBUG_PRINT("PROTO", ("I_MSDISTCK"));
      mkcode("msdistck();");
      goto loop;
      }
    case I_MPROCREON: {
      DBUG_PRINT("PROTO", ("I_MPROCREON"));
      mkcode("mprocreon();");
      goto loop;
      }
    case I_MSDISTEND: {
      DBUG_PRINT("PROTO", ("I_MSDISTEND"));
      mkcode("msdistend();");
      goto loop;
      }
    case I_MPRSLEEON: {
      DBUG_PRINT("PROTO", ("I_MPRSLEEON"));
      mkcode("mprsleeon();");
      goto loop;
      }
    case I_MPRORUNON: {
      DBUG_PRINT("PROTO", ("I_MPRORUNON"));
      mkcode("mprorunon();");
      goto loop;
      }
    case I_MPROWAKON: {
      DBUG_PRINT("PROTO", ("I_MPROWAKON"));
      mkcode("mprowakon();");
      goto loop;
      }
    case I_MPROTERON: {
      DBUG_PRINT("PROTO", ("I_MPROTERON"));
      mkcode("mproteron();");
      goto loop;
      }
    case I_MCOMSEBON: {
      DBUG_PRINT("PROTO", ("I_MCOMSEBON"));
      mkcode("mcomsebon();");
      goto loop;
      }
    case I_MCOMSEEON: {
      DBUG_PRINT("PROTO", ("I_MCOMSEEON"));
      mkcode("mcomseeon();");
      goto loop;
      }
    case I_MCOMREBON: {
      DBUG_PRINT("PROTO", ("I_MCOMREBON"));
      mkcode("mcomrebon();");
      goto loop;
      }
    case I_MCOMREEON: {
      DBUG_PRINT("PROTO", ("I_MCOMREEON"));
      mkcode("mcomreeon();");
      goto loop;
      }
    case I_MSTCKPUON: {
      DBUG_PRINT("PROTO", ("I_MSTCKPUON"));
      mkcode("mstckpuon();");
      goto loop;
      }
    case I_MSTCKPPON: {
      DBUG_PRINT("PROTO", ("I_MSTCKPPON"));
      mkcode("mstckppon();");
      goto loop;
      }
    case I_MSTKSEGAL: {
      DBUG_PRINT("PROTO", ("I_MSTKSEGAL"));
      mkcode("mstksegal();");
      goto loop;
      }
    case I_MSTKSEGFR: {
      DBUG_PRINT("PROTO", ("I_MSTKSEGFR"));
      mkcode("mstksegfr();");
      goto loop;
      }
#endif /* D_MESS */
    case I_CHKFRAME: {
      DBUG_PRINT("PROTO", ("I_CHKFRAME"));
      fprintf(stderr, "I_CHKFRAME not implemented yet\n");
      goto loop;
      }
    case I_APPEND: {
      DBUG_PRINT("PROTO", ("I_APPEND"));
      fprintf(stderr, "I_APPEND not implemented yet\n");
      goto loop;
      }
#if WITHTILDE
    case I_PUSH_R: cmd = "pushc_r";
    case I_PUSHC_R: {
      DBUG_PRINT("PROTO", (" I_PUSH(C)_R"));
      node = enclosetype(TY_UNDEF, 1, typeconst(*code));
      mkcode("_%s(%s);", (cmd ? cmd : "push_r"), stelname(*code));
      PUSH_R(node);
      code++;
      goto loop;
      }
    case I_PUSH_TW: {
      DBUG_PRINT("PROTO", ("I_PUSH_TW"));
      mkcode("_push_tw(%d);", *code);
      node=(T_PTD)MID_T(*code);
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_W(node);
      INC_REFCNT(node);
      code++;
      goto loop;
      }
    case I_PUSH_TW0: {
      DBUG_PRINT("PROTO", ("I_PUSH_TW0"));
      mkcode("_push_tw(0);");
      node=(T_PTD)TOP_T();
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_W(node);
      INC_REFCNT(node);
      goto loop;
      }
    case I_PUSH_TR: {
      DBUG_PRINT("PROTO", ("I_PUSH_TR"));
      mkcode("_push_tr(%d);", *code);
      node=(T_PTD)MID_T(*code);
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_R(node);
      INC_REFCNT(node);
      code++;
      goto loop;
      }
    case I_PUSH_TR0: {
      DBUG_PRINT("PROTO", ("I_PUSH_TR0"));
      mkcode("_push_tr(0);");
      node=(T_PTD)TOP_T();
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_R(node);
      INC_REFCNT(node);
      goto loop;
      }
    case I_PUSH_AR: {
      DBUG_PRINT("PROTO", ("I_PUSH_AR"));
      mkcode("_push_ar(%d);", *code);
      node=(T_PTD)MID_A(*code);
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_R(node);
      INC_REFCNT(node);
      code++;
      goto loop;
      }
    case I_PUSH_AR0: {
      DBUG_PRINT("PROTO", ("I_PUSH_AR0"));
      mkcode("_push_ar(0);");
      node=(T_PTD)TOP_A();
      L_LINKNODE((*node), refcnt) = R_LINKNODE((*node), refcnt) + 1;
      PUSH_R(node);
      INC_REFCNT(node);
      goto loop;
      }
    case I_MOVE_TW: {
      DBUG_PRINT("PROTO", ("I_MOVE_TW"));
      PUSH_W(POP_T());
      mkcode("_move_tw();");
      goto loop;
      }
    case I_MOVE_TR: {
      DBUG_PRINT("PROTO", ("I_MOVE_TR"));
      PUSH_R(POP_T());
      mkcode("_move_tr();");
      goto loop;
      }
    case I_MOVE_AR: {
      DBUG_PRINT("PROTO", ("I_MOVE_AR"));
      PUSH_R(POP_A());
      mkcode("_move_ar();");
      goto loop;
      }
    case I_MOVE_WR: {
      DBUG_PRINT("PROTO", ("I_MOVE_WR"));
      PUSH_R(POP_W());
      mkcode("_move_wr();");
      goto loop;
      }
    case I_FREE_R: {
      DBUG_PRINT("PROTO", ("I_FREE_R"));
      i = n = *code++;
      for(i = 0; i < n; i++) {
        node = (T_PTD)POP_R();
	if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	  mkcode("_%sdecr(%d, 1);", isundef(node) ? "t" : "", i);
        L_LINKNODE((*node), refcnt) = 0;
	DEC_REFCNT(node);
	}
      mkcode("_free_r(%d);", n);
      goto loop;
      }
    case I_FREE1_R: {
      DBUG_PRINT("PROTO", ("I_FREE1_R"));
      node = (T_PTD)TOP_R();
      if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	mkcode("_%sdecr(0, 1);", isundef(node) ? "t" : "");
      L_LINKNODE((*node), refcnt) = 0;
      DEC_REFCNT(node);
      POP_A();
      mkcode("_free1_a();");
      goto loop;
      }
    case I_RTM_T: {
      DBUG_PRINT("PROTO", ("I_RTM_T"));
      mkcomment("be careful");
      node = (T_PTD)POP_T();
      if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	mkcode("_%sdecr(0, 1);", isundef(node) ? "t" : "");
      PUSH_W(node);
      mkcode("rtm_t();");
      SWITCHSTACKS_T();
      goto L_rtf_entry;
      }
    case I_RTT: {
      DBUG_PRINT("PROTO", ("I_RTT"));
      mkcomment("be careful");
      node = (T_PTD)TOP_T();
      if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	mkcode("_%sdecr(0, 1);", isundef(node) ? "t" : "");
      mkcode("rtt();");
      SWITCHSTACKS_T();
      goto L_rtf_entry;
      }
    case I_RTC_T: {
      DBUG_PRINT("PROTO", ("I_RTC_T"));
      mkcomment("be careful");
      node = (T_PTD)*code++;
      if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	mkcode("_%sdecr(0, 1);", isundef(node) ? "t" : "");
      PUSH_W(node);
      mkcode("rtc_t();");
      SWITCHSTACKS_T();
      goto L_rtf_entry;
      }
    case I_FREESW_T: {
      DBUG_PRINT("PROTO", ("I_FREESW_T"));
      mkcomment("be careful");
      n = *code++;
      for(i = 0; i < n; i++) {
        node = (T_PTD)POP_T();
	if ((R_LINKNODE((*node), refcnt) == 0 ) && ! IS_UNBOXED(node))
	  mkcode("_%sdect(%d, 1);", isundef(node) ? "t" : "", i);
        L_LINKNODE((*node), refcnt) = 0;
	DEC_REFCNT(node);
	}
      mkcode("_freesw_t(%d);", n);
      goto loop;
      }
    case I_GAMMA: {
      DBUG_PRINT("PROTO", ("I_GAMMA"));
      mkcomment("be careful");
      pdesc = (T_PTD)*code++;
      goto loop;
      }
    case I_GAMMABETA: {
      DBUG_PRINT("PROTO", ("I_GAMMABETA"));
      mkcomment("be careful");
      fprintf(stderr, "I_GAMMABETA not implemented yet\n");
      goto loop;
      }
    case I_GAMMACASE: {
      DBUG_PRINT("PROTO", ("I_GAMMACASE"));
      fprintf(stderr, "I_GAMMACASE not implemented yet\n");
      goto loop;
      }
    case I_MKGACLOS: {
      DBUG_PRINT("PROTO", ("I_MKGACLOS"));
      fprintf(stderr, "I_MKGACLOS not implemented yet\n");
      goto loop;
      }
    case I_MKGSCLOS: {
      DBUG_PRINT("PROTO", ("I_MKGSCLOS"));
      fprintf(stderr, "I_MKGSCLOS not implemented yet\n");
      goto loop;
      }
    case I_POPFREE_T: {
      DBUG_PRINT("PROTO", ("I_POPFREE_T"));
      fprintf(stderr, "I_POPFREE_T not implemented yet\n");
      goto loop;
      }
    case I_PUSHRET_T: {
      DBUG_PRINT("PROTO", ("I_PUSHRET_T"));
      fprintf(stderr, "I_PUSHRET_T not implemented yet\n");
      goto loop;
      }
#endif /* WITHTILDE */
    default: {
      sprintf(buf,"inter: invalid instruction (%d)",code[-1]);
      POST_MORTEM(buf);
      }
    } /* end switch */

  fclose(codefp);

#if    DEBUG
  strcpy(S_e.Name,     "e");  stackname_dr(&S_e);
  strcpy(S_a.Name,     "a");  stackname_dr(&S_a);
  strcpy(S_m1.Name,    "m1"); stackname_dr(&S_m1);
  strcpy(S_m.Name,     "m");  stackname_dr(&S_m);
#if WITHTILDE
  strcpy(S_tilde.Name, "tilde"); stackname_dr(&S_tilde); /* mah 251093 */
#endif /* WITHTILDE */
#endif /* DEBUG */
  /* first free complex types */
  if (R_DESC((*boolvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for boolvect is %d\n", R_DESC((*boolvect), ref_count));
  DEC_REFCNT(boolvect);
  if (R_DESC((*booltvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for booltvect is %d\n", R_DESC((*booltvect), ref_count));
  DEC_REFCNT(booltvect);
  if (R_DESC((*boolmat), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for boolmat is %d\n", R_DESC((*boolmat), ref_count));
  DEC_REFCNT(boolmat);
  if (R_DESC((*intvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for intvect is %d\n", R_DESC((*intvect), ref_count));
  DEC_REFCNT(intvect);
  if (R_DESC((*inttvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for inttvect is %d\n", R_DESC((*inttvect), ref_count));
  DEC_REFCNT(inttvect);
  if (R_DESC((*intmat), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for intmat is %d\n", R_DESC((*intmat), ref_count));
  DEC_REFCNT(intmat);
  if (R_DESC((*realvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for realvect is %d\n", R_DESC((*realvect), ref_count));
  DEC_REFCNT(realvect);
  if (R_DESC((*realtvect), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for realtvect is %d\n", R_DESC((*realtvect), ref_count));
  DEC_REFCNT(realtvect);
  if (R_DESC((*realmat), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for realmat is %d\n", R_DESC((*realmat), ref_count));
  DEC_REFCNT(realmat);

  /* free atomic types */
  if (R_DESC((*undef), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for undef is %d\n", R_DESC((*undef), ref_count));
  DEC_REFCNT(undef);
  if (R_DESC((*bool), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for bool is %d\n", R_DESC((*bool), ref_count));
  DEC_REFCNT(bool);
  if (R_DESC((*integer), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for integer is %d\n", R_DESC((*integer), ref_count));
  DEC_REFCNT(integer);
  if (R_DESC((*real), ref_count) != 1)
    fprintf(stderr, "warning: ref_count for real is %d\n", R_DESC((*real), ref_count));
  DEC_REFCNT(real);
  END_MODUL("optasm");
  DBUG_VOID_RETURN;
  } /* end optasm */
