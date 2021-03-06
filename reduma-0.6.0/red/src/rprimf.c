/* $Log rprimf.c,v $
 */

/* file rprimf.c
 * -------------
 * car  1.06.94
 *
 * auxiliary functions to deal with types of primitive functions
 * - lookupprimf: check if arguments are of correct type
 * -              compute result type for given arguments
 * 
 */

#include <stdio.h>
#include <string.h>
#include "rheapty.h"
#include "rstelem.h"
#include "rstackty.h"
#include "roptasm.h"
#include "dbug.h"


extern int post_mortem();
extern int equaltype();
#if DEBUG
extern void    DescDump();
#endif /* DEBUG */

extern T_PTD undef, bool, integer, real, boolvect, intvect, realvect,
             booltvect, inttvect, realtvect, boolmat, intmat, realmat;

/* do not change ordering
 * alltypes: ordering has to be compatible with typeids
 * numtypes: all real types must have an odd index
 * valtypes: ordering bool/int/real in scalar/vect/tvect/mat
 */
static char * typeids[] =
  { "?", "b", "i", "r", "vb", "vi", "vr", "tb", "ti", "tr", "mb", "mi", "mr" };
static T_PTD * alltypes[] =
  { &undef,
    &bool, &integer, &real,             &boolvect, &intvect, &realvect,
    &booltvect, &inttvect, &realtvect,  &boolmat, &intmat, &realmat };
static T_PTD * numtypes[] =
  { &integer, &real, &intvect, &realvect, &inttvect, &realtvect, &intmat, &realmat };
static T_PTD * valtypes[] =
  { &bool, &integer, &real,             &boolvect, &intvect, &realvect,
    &booltvect, &inttvect, &realtvect,  &boolmat, &intmat, &realmat };
static T_PTD * booltypes[] =
  { &bool, &boolvect, &booltvect, &boolmat };
static T_PTD * scaltypes[] =
  { &bool, &integer, &real };
static T_PTD * nscaltypes[] =
  { &integer, &real };
static T_PTD * vecttypes[] =
  { &boolvect, &intvect, &realvect, &booltvect, &inttvect, &realtvect };
static T_PTD * nvecttypes[] =
  { &intvect, &realvect, &inttvect, &realtvect };
static T_PTD * mattypes[] =
  { &boolmat, &intmat, &realmat };
static T_PTD * nmattypes[] =
  { &intmat, &realmat };

static char * typeid(T_PTD ptype)
  {
  int n;

  DBUG_ENTER("typeid");
  n = sizeof(alltypes)/sizeof(T_PTD *);
  for(n--; n >= 0; n--)
    if (equaltype(ptype, *alltypes[n]))
      DBUG_RETURN(typeids[n]);
  if ((R_DESC((*ptype), class) == C_COMPTYPE)
      && (R_DESC((*ptype), type) == TY_PROD))
    DBUG_RETURN("l");
  DBUG_RETURN("?");
  }

int getindex(T_PTD ptype, T_PTD * field[], int nfields)
  {
  for(nfields--; nfields >= 0; nfields--) {
    if (equaltype(ptype, *field[nfields])) return nfields;
    }
  return -1;
  }

/* calculate return type of a num -> num arithmetic operation,
 * argument type is read from top of stack
 * functions: neg, abs, frac
 */
static T_PTD num2num(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("num2num");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if (getindex(ptype, numtypes, sizeof(numtypes)/sizeof(T_PTD *)) >= 0) {
    /* type is num */
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num -> int arithmetic operation,
 * argument type is read from top of stack
 * functions: trunc, floor, ceil
 */
static T_PTD num2int(int instr)
  {
  int n;
  T_PTD ptype;

  DBUG_ENTER("num2int");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if ((n = getindex(ptype, numtypes, sizeof(numtypes)/sizeof(T_PTD *)) >= 0)) {
    /* type is num */
    INC_REFCNT(*numtypes[n&~1]);
    DBUG_RETURN(*numtypes[n&~1]);
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num x num -> num arithmetic operation,
 * argument types are read from top of stack
 * special case: instr = _D_DIV -> result is always real
 * functions: plus, minus, mult, div, mod, max, min
 */
static T_PTD numxnum2num(int instr)
  {
  int i = -1, j = -1, n;
  T_PTD ptype1, ptype2;

  DBUG_ENTER("numxnum2num");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  n = sizeof(numtypes)/sizeof(T_PTD *);
  for(n--; n >= 0; n--) {
    if (equaltype(ptype1, *numtypes[n])) i = n;
    if (equaltype(ptype2, *numtypes[n])) j = n;
    }
  if ((i >= 0) && (j >= 0)) {
    /* both types are num */
    if (instr == _D_DIV)
      { i |= 1; j |= 1; }
    if (i > j) { n = i; i = j; j = n; }
    if (i == 0) { INC_REFCNT(*numtypes[j]); DBUG_RETURN(*numtypes[j]); }
    if (i == 1) { INC_REFCNT(*numtypes[j|1]); DBUG_RETURN(*numtypes[j|1]); }
    if (i == j) { INC_REFCNT(*numtypes[i]); DBUG_RETURN(*numtypes[i]); }
    if (i+1 == j) { INC_REFCNT(*numtypes[j]); DBUG_RETURN(*numtypes[j]); }
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
    }
  else {
    /* illegal type(s) */
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
    }
  }

/* calculate return type of a value x value -> bool relational operation,
 * argument types are read from top of stack
 * functions: eq, ne, lt, le, gt, ge
 */
static T_PTD valxval2bool(int instr)
  {
  int i = -1, j = -1, n;
  T_PTD ptype1, ptype2;

  DBUG_ENTER("valxval2bool");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  n = sizeof(valtypes)/sizeof(T_PTD *);
  for(n--; n >= 0; n--) {
    if (equaltype(ptype1, *valtypes[n])) i = n;
    if (equaltype(ptype2, *valtypes[n])) j = n;
    }
  if ((i >= 0) && (j >= 0)) {
    /* both types are value */
    if ((i%3 == 0) && (j%3 == 0)) {
      INC_REFCNT(bool);
      DBUG_RETURN(bool);
      }
    if (i%3 && j%3) { INC_REFCNT(bool); DBUG_RETURN(bool); }
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
    }
  else
    /* illegal type(s) */
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
  }

/* calculate return type of a bool -> bool logical operation,
 * argument type is read from top of stack
 * function: not
 */
static T_PTD bool2bool(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("bool2bool");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if (getindex(ptype, booltypes, sizeof(booltypes)/sizeof(T_PTD *)) >= 0) {
    /* both types are bool */
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a bool x bool -> bool logical operation,
 * argument types are read from top of stack
 * functions: and, or, xor
 */
static T_PTD boolxbool2bool(int instr)
  {
  int i = -1, j = -1, n;
  T_PTD ptype1, ptype2;

  DBUG_ENTER("boolxbool2bool");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  n = sizeof(booltypes)/sizeof(T_PTD *);
  for(n--; n >= 0; n--) {
    if (equaltype(ptype1, *booltypes[n])) i = n;
    if (equaltype(ptype2, *booltypes[n])) j = n;
    }
  if ((i >= 0) && (j >= 0)) {
    /* both types are bool */
    if (i > j) { n = i; i = j; j = n; }
    if (i == 0) { INC_REFCNT(*booltypes[j]); DBUG_RETURN(*booltypes[j]); }
    if (i == j) { INC_REFCNT(*booltypes[i]); DBUG_RETURN(*booltypes[i]); }
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
    }
  else
    /* illegal type(s) */
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
  }

/* calculate return type of a structured type -> bool function
 * argument type is read from top of stack
 * function: empty
 */
static T_PTD lmvt2bool(int instr)
  {
  int ty = R_DESC((*(T_PTD)TOP_W()), type);

  DBUG_ENTER("lmvt2bool");
  if ( (R_DESC((*(T_PTD)TOP_W()), class) == C_COMPTYPE)
      && ((ty == TY_PROD) || (ty == TY_VECT)
         || (ty == TY_TVECT) || (ty == TY_MAT)) ) {
    INC_REFCNT(bool);
    DBUG_RETURN(bool);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a structured type -> int function
 * argument type is read from top of stack
 * function: ldim, vdim
 */
static T_PTD lvt2int(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("lvt2int");
  ptype = (T_PTD)TOP_W();
  if ((R_DESC((*ptype), class) == C_COMPTYPE)
      && (((instr == _LDIMENSION) && (R_DESC((*ptype), type) == TY_PROD))
      ||  ((instr == _VDIMENSION) && (R_DESC((*ptype), type) == TY_VECT))
      ||  ((instr == _VDIMENSION) && (R_DESC((*ptype), type) == TY_TVECT)))) {
    INC_REFCNT(integer);
    DBUG_RETURN(integer);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x num_scalar x matrix -> scalar/matrix
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: mdim
 */

static T_PTD nscalxmat2int(int instr)
  {
  T_PTD ptype1, ptype2;

  DBUG_ENTER("nscalxmat2int");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype2, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0)) {
    INC_REFCNT(integer);
    DBUG_RETURN(integer);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a list -> list function
 * argument type is read from top of stack
 * function: reverse
 */
static T_PTD list2list(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("list2list");
  ptype = (T_PTD)TOP_W();
  if ((R_DESC((*ptype), class) == C_COMPTYPE)
      && (R_DESC((*ptype), type) == TY_PROD)) {
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a int x structured type -> int function
 * argument types are read from top of stack
 * function: dim
 */
static T_PTD intxlmvt2int(int instr)
  {
  int ty = R_DESC((*(T_PTD)MID_W(1)), type);

  DBUG_ENTER("mat2int");
  if ( equaltype(integer, (T_PTD)TOP_W())
     && (R_DESC((*(T_PTD)MID_W(1)), class) == C_COMPTYPE)
       && ((ty = TY_PROD) || (ty == TY_VECT)
           || (ty == TY_TVECT) || (ty == TY_MAT)) ) {
    INC_REFCNT(integer);
    DBUG_RETURN(integer);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a structured type x transposed structured type -> int function
 * argument types are read from top of stack
 * dimensions have to be tested during runtime
 * function: ip
 */
static T_PTD mvtxmtv2num(int instr)
  {
  int i = -1, j = -1, n;
  T_PTD ptype1, ptype2;

  DBUG_ENTER("mvtxmtv2num");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  n = sizeof(numtypes)/sizeof(T_PTD *);
  for(n--; n >= 0; n--) {
    if (equaltype(ptype1, *numtypes[n])) i = n;
    if (equaltype(ptype2, *numtypes[n])) j = n;
    }
  if ((i < 0) || (j < 0)) {
    INC_REFCNT(undef);
    DBUG_RETURN(undef);
    }
  /* vect x tvect -> scalar */
  if ((i/2 == 1) && (j/2 == 2)) {
    INC_REFCNT(*numtypes[(i|j)%2]);
    DBUG_RETURN(*numtypes[(i|j)%2]);
    }
  /* tvect x vect -> mat */
  if ((i/2 == 2) && (j/2 == 1)) {
    INC_REFCNT(*numtypes[(i|j)%2+3]);
    DBUG_RETURN(*numtypes[(i|j)%2+3]);
    }
  /* vect x mat -> vect */
  if ((i/2 == 1) && (j/2 == 3)) {
    INC_REFCNT(*numtypes[(i|j)%2+1]);
    DBUG_RETURN(*numtypes[(i|j)%2+1]);
    }
  /* mat x tvect -> tvect */
  if ((i/2 == 3) && (j/2 == 2)) {
    INC_REFCNT(*numtypes[(i|j)%2+2]);
    DBUG_RETURN(*numtypes[(i|j)%2+2]);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x list -> expression
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: lselect, lcut
 */

static T_PTD nscalxlist2expr(int instr)
  {
  T_PTD ptype1, ptype2, ptype;

  DBUG_ENTER("nscalxlist2expr");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  if (getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
    if ((R_DESC((*ptype2), class) == C_COMPTYPE)
        && (R_DESC((*ptype2), type) == TY_PROD)) {
      ptype = (instr != _LSELECT ? ptype2 :
          R_COMPTYPE((*ptype2), cnt) ? (T_PTD)R_COMPTYPE((*ptype2), ptypes)[0] : undef);
      INC_REFCNT(ptype);
      DBUG_RETURN(ptype);
      }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x vector -> scalar/vector
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: vselect, vcut
 */

static T_PTD nscalxvect2expr(int instr)
  {
  T_PTD ptype1, ptype2, ptype;

  DBUG_ENTER("nscalxvect2expr");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      &&  (getindex(ptype2, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *)) >= 0)) {
    ptype = (instr != _VSELECT ? ptype2 : (T_PTD)R_COMPTYPE((*ptype2), ptypes)[0]);
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x num_scalar x matrix -> scalar/matrix
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: mselect, mcut
 */

static T_PTD nscalxnscalxmat2expr(int instr)
  {
  T_PTD ptype1, ptype2, ptype3, ptype;

  DBUG_ENTER("nscalxnscalxmat2expr");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype2, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype3, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0)) {
    ptype = (instr != _MSELECT ? ptype3 : (T_PTD)R_COMPTYPE((*ptype3), ptypes)[0]);
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a  list x list -> list
 * argument types are read from top of stack
 * function: lunite
 */

static T_PTD listxlist2list(int instr)
  {
  T_PTD ptype, ptype1, ptype2;

  DBUG_ENTER("listxlist2list");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  if ((R_DESC((*ptype1), class) == C_COMPTYPE)
      && (R_DESC((*ptype1), type) == TY_PROD)
      && (R_DESC((*ptype2), class) == C_COMPTYPE)
      && (R_DESC((*ptype2), type) == TY_PROD)) {
    if (R_COMPTYPE((*ptype1), cnt) == 0) {
      INC_REFCNT(ptype2);
      DBUG_RETURN(ptype2);
      }
    if (R_COMPTYPE((*ptype2), cnt) == 0) {
      INC_REFCNT(ptype1);
      DBUG_RETURN(ptype1);
      }
    if (equaltype(ptype1, ptype2)) {
      INC_REFCNT(ptype1);
      DBUG_RETURN(ptype1);
      }
    else {
      /* WARNING: reference count of type descriptor set to 0 */
      MAKEDESC(ptype, 1, C_COMPTYPE, TY_PROD);
      GET_HEAP(1, A_COMPTYPE((*ptype), ptypes));
      L_COMPTYPE((*ptype), cnt) = 1;
      L_COMPTYPE((*ptype), ptypes)[0] = (T_HEAPELEM)undef;
      INC_REFCNT(undef);
      DBUG_RETURN(ptype);
      }
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a vector x vector -> vector
 * argument types are read from top of stack
 * function: vunite
 */

static T_PTD vectxvect2vect(int instr)
  {
  int i, j;
  T_PTD ptype1, ptype2;

  DBUG_ENTER("vectxvect2vect");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  i = getindex(ptype1, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *));
  j = getindex(ptype2, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *));
  switch(i) {
    case 0: INC_REFCNT(j == 0 ? ptype1 : undef);
            DBUG_RETURN(j == 0 ? ptype1 : undef);
    case 1: INC_REFCNT((j == 1) || (j == 2) ? ptype2 : undef);
            DBUG_RETURN((j == 1) || (j == 2) ? ptype2 : undef);
    case 2: INC_REFCNT((j == 1) || (j == 2) ? ptype1 : undef);
            DBUG_RETURN((j == 1) || (j == 2) ? ptype1 : undef);
    case 3: INC_REFCNT(j == 3 ? ptype1 : undef);
            DBUG_RETURN(j == 3 ? ptype1 : undef);
    case 4: INC_REFCNT((j == 4) || (j == 5) ? ptype2 : undef);
            DBUG_RETURN((j == 4) || (j == 5) ? ptype2 : undef);
    case 5: INC_REFCNT((j == 4) || (j == 5) ? ptype1 : undef);
            DBUG_RETURN((j == 4) || (j == 5) ? ptype1 : undef);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar X matrix x matrix -> matrix
 * argument types are read from top of stack
 * function: munite
 */

static T_PTD nscalxmatxmat2mat(int instr)
  {
  int i, j;
  T_PTD ptype1, ptype2, ptype3;

  DBUG_ENTER("nscalxmatxmat2mat");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  if (getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0) {
    if (equaltype(ptype2, boolmat) && equaltype(ptype3, boolmat)) {
      INC_REFCNT(ptype2);
      DBUG_RETURN(ptype2);
      }
    i = getindex(ptype2, nmattypes, sizeof(nmattypes)/sizeof(T_PTD *));
    j = getindex(ptype3, nmattypes, sizeof(nmattypes)/sizeof(T_PTD *));
    if ((i >= 0) && (j >= 0))  {
      INC_REFCNT(*nmattypes[i > j ? i : j]);
      DBUG_RETURN(*nmattypes[i > j ? i : j]);
      }
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a vector -> num arithmetic operation,
 * argument type is read from top of stack
 * functions: vc_plus, vc_minus, vc_mult, vc_div, vc_min, vc_max
 */
static T_PTD vect2num(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("vect2num");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if (getindex(ptype, nvecttypes, sizeof(nvecttypes)/sizeof(T_PTD *)) >= 0) {
    /* type is num */
    INC_REFCNT((T_PTD)(R_COMPTYPE((*ptype), ptypes)[0]));
    DBUG_RETURN((T_PTD)(R_COMPTYPE((*ptype), ptypes)[0]));
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar/vector -> mat arithmetic operation,
 * argument type is read from top of stack
 * functions: to_matrix
 */
static T_PTD svt2mat(int instr)
  {
  int i;
  T_PTD ptype;

  DBUG_ENTER("svt2mat");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if ((i = getindex(ptype, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *))) >= 0) {
    INC_REFCNT(*nmattypes[i]);
    DBUG_RETURN(*nmattypes[i]);
    }
  if ((i = getindex(ptype, nvecttypes, sizeof(nvecttypes)/sizeof(T_PTD *))) >= 0) {
    INC_REFCNT(*nmattypes[i&2]);
    DBUG_RETURN(*nmattypes[i&2]);
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar/matrix -> mat arithmetic operation,
 * argument type is read from top of stack
 * functions: to_vector
 */
static T_PTD sm2vect(int instr)
  {
  int i;
  T_PTD ptype;

  DBUG_ENTER("sm2vect");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if ((i = getindex(ptype, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *))) >= 0) {
    INC_REFCNT(*(instr == _TO_VECT ? nvecttypes : &nvecttypes[2])[i]);
    DBUG_RETURN(*(instr == _TO_VECT ? nvecttypes : &nvecttypes[2])[i]);
    }
  if ((i = getindex(ptype, nmattypes, sizeof(nmattypes)/sizeof(T_PTD *))) >= 0) {
    INC_REFCNT(*(instr == _TO_VECT ? nvecttypes : &nvecttypes[2])[i]);
    DBUG_RETURN(*(instr == _TO_VECT ? nvecttypes : &nvecttypes[2])[i]);
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a matrix/vector -> scalar
 * argument type is read from top of stack
 * functions: to_scal
 */
static T_PTD mvt2scal(int instr)
  {
  T_PTD ptype;

  DBUG_ENTER("mvt2scal");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if ((getindex(ptype, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *)) >= 0)
      || (getindex(ptype, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *)) >= 0)) {
    INC_REFCNT((T_PTD)(R_COMPTYPE((*ptype), ptypes)[0]));
    DBUG_RETURN((T_PTD)(R_COMPTYPE((*ptype), ptypes)[0]));
    }
  /* illegal type */
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a structured type -> transposed structured type
 * argument type is read from top of stack
 * function: tranpose
 */
static T_PTD vtm2tvm(int instr)
  {
  int i;
  T_PTD ptype;

  DBUG_ENTER("vtm2tvm");
  ptype = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  if (getindex(ptype, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0) {
    INC_REFCNT(ptype);
    DBUG_RETURN(ptype);
    }
  if ((i = getindex(ptype, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *))) >= 0) {
    INC_REFCNT(i >= 3 ? *vecttypes[i-3] : *vecttypes[i+3]);
    DBUG_RETURN(i >= 3 ? *vecttypes[i-3] : *vecttypes[i+3]);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a matrix -> list function
 * argument type is read from top of stack
 * function: transform
 */
static T_PTD mat2list(int instr)
  {
  T_PTD ptype, ltype;

  DBUG_ENTER("mat2list");
  ptype = (T_PTD)TOP_W();
  if (getindex(ptype, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0) {
    /* WARNING: reference count of type descriptor set to 0 */
    MAKEDESC(ltype, 1, C_COMPTYPE, TY_PROD);
    GET_HEAP(1, A_COMPTYPE((*ltype), ptypes));
    L_COMPTYPE((*ltype), cnt) = 1;
    L_COMPTYPE((*ltype), ptypes)[0] = R_COMPTYPE((*ptype), ptypes)[0];
    INC_REFCNT((T_PTD)R_COMPTYPE((*ptype), ptypes)[0]);
    DBUG_RETURN(ltype);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x num_scalar x list -> matrix function
 * argument types are read from top of stack
 * function: ltransform
 */
static T_PTD nscalxnscalxlist2mat(int instr)
  {
  int i;
  T_PTD ptype1, ptype2, ptype3;

  DBUG_ENTER("nscalxnscalxlist2mat");
  ptype1 = (T_PTD)TOP_W();
  ptype2 = (T_PTD)MID_W(1);
  ptype3 = (T_PTD)MID_W(2);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype2, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (R_DESC((*ptype2), class) == C_COMPTYPE)
      && (R_DESC((*ptype2), type) == TY_PROD)) {
    ptype1 = (T_PTD)R_COMPTYPE((*ptype1), ptypes)[0];
    if ((i = getindex(ptype1, scaltypes, sizeof(scaltypes)/sizeof(T_PTD *))) >= 0) {
      INC_REFCNT(*mattypes[i]);
      DBUG_RETURN(*mattypes[i]);
      }
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x expr x list -> list
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: lreplace
 */

static T_PTD nscalxexprxlist2list(int instr)
  {
  T_PTD ptype1, ptype2, ptype3, ltype;

  DBUG_ENTER("nscalxexprxlist2list");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (R_DESC((*ptype3), class) == C_COMPTYPE)
      && (R_DESC((*ptype3), type) == TY_PROD)
      && (equaltype(ptype2, (T_PTD)R_COMPTYPE((*ptype3), ptypes)[0]))) {
    INC_REFCNT(ptype3);
    DBUG_RETURN(ptype3);
    }
  /* WARNING: reference count of type descriptor set to 0 */
  MAKEDESC(ltype, 1, C_COMPTYPE, TY_PROD);
  GET_HEAP(1, A_COMPTYPE((*ltype), ptypes));
  L_COMPTYPE((*ltype), cnt) = 1;
  L_COMPTYPE((*ltype), ptypes)[0] = (T_HEAPELEM)undef;
  INC_REFCNT(undef);
  DBUG_RETURN(ltype);
  }

/* calculate return type of a num_scalar x num_scalar x vector -> vector
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: vreplace
 */
    
static T_PTD nscalxnscalxvect2vect(int instr)
  {
  T_PTD ptype1, ptype2, ptype3;

  DBUG_ENTER("nscalxnscalxvect2vect");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype3, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *)) >= 0)
      && (equaltype(ptype2, (T_PTD)R_COMPTYPE((*ptype3), ptypes)[0]))) {
    INC_REFCNT(ptype3);
    DBUG_RETURN(ptype3);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x num_scalar x num_scalar x matrix -> matrix
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: mreplace
 */
    
static T_PTD nscalxnscalxnscalxmat2mat(int instr)
  {
  T_PTD ptype1, ptype2, ptype3, ptype4;

  DBUG_ENTER("nscalxnscalxnscalxmat2mat");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  ptype4 = R_LINKNODE((*(T_PTD)MID_W(3)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype2, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype4, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0)
      && (equaltype(ptype3, (T_PTD)R_COMPTYPE((*ptype4), ptypes)[0]))) {
    INC_REFCNT(ptype4);
    DBUG_RETURN(ptype4);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* calculate return type of a num_scalar x vector x matrix -> matrix
 * argument types are read from top of stack
 * boundries have to be checked dynamically
 * function: mreplace_c, mreplace_r
 */
    
static T_PTD nscalxvectxmat2mat(int instr)
  {
  T_PTD ptype1, ptype2, ptype3;

  DBUG_ENTER("nscalxvectxmat2mat");
  ptype1 = R_LINKNODE((*(T_PTD)TOP_W()), ptype);
  ptype2 = R_LINKNODE((*(T_PTD)MID_W(1)), ptype);
  ptype3 = R_LINKNODE((*(T_PTD)MID_W(2)), ptype);
  if ((getindex(ptype1, nscaltypes, sizeof(nscaltypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype2, vecttypes, sizeof(vecttypes)/sizeof(T_PTD *)) >= 0)
      && (getindex(ptype3, mattypes, sizeof(mattypes)/sizeof(T_PTD *)) >= 0)
      && (equaltype((T_PTD)R_COMPTYPE((*ptype2), ptypes)[0],
                    (T_PTD)R_COMPTYPE((*ptype3), ptypes)[0]))) {
    INC_REFCNT(ptype3);
    DBUG_RETURN(ptype3);
    }
  INC_REFCNT(undef);
  DBUG_RETURN(undef);
  }

/* list of primitive functions
 * format instr_id, instr_name, arity, function, return_type, arg_type_1 ,...
 */
T_asm_primf_fkt asm_primf_fkt[] = {
  { _M_NEG, "neg", 1, num2num },
  { _M_ABS, "abs", 1, num2num },
  { _M_TRUNCATE, "trunc", 1, num2int },
  { _M_FLOOR, "floor", 1, num2int },
  { _M_FRAC, "frac", 1, num2num },
  { _M_CEIL, "ceil", 1, num2int },
  { _M_SIN, "sin", 1, NULL, &real, &integer },
  { _M_SIN, "sin", 1, NULL, &real, &real },
  { _M_COS, "cos", 1, NULL, &real, &integer },
  { _M_COS, "cos", 1, NULL, &real, &real },
  { _M_TAN, "tan", 1, NULL, &real, &integer },
  { _M_TAN, "tan", 1, NULL, &real, &real },
  { _M_LN, "ln", 1, NULL, &real, &integer },
  { _M_LN, "ln", 1, NULL, &real, &real },
  { _M_EXP, "exp", 1, NULL, &real, &integer },
  { _M_EXP, "exp", 1, NULL, &real, &real },
  { _D_PLUS, "plus", 2, numxnum2num },
  { _D_MINUS, "minus", 2, numxnum2num },
  { _D_MULT, "mult", 2, numxnum2num },
  { _D_DIV, "div", 2, numxnum2num }, /* special treatment within numxnum2num */
  { _D_MOD, "mod", 2, numxnum2num },
  { _D_MAX, "max", 2, numxnum2num },
  { _D_MIN, "min", 2, numxnum2num },
  { _D_QUOT, "quot", 2, NULL, &integer, &integer, &integer },
  { _D_EQ, "eq", 2, valxval2bool },
  { _D_NEQ, "ne", 2, valxval2bool },
  { _D_LESS, "lt", 2, valxval2bool },
  { _D_LEQ, "le", 2, valxval2bool },
  { _D_GREAT, "gt", 2, valxval2bool },
  { _D_GEQ, "ge", 2, valxval2bool },
  { _M_NOT, "not", 2, bool2bool },
  { _D_AND, "and", 2, boolxbool2bool },
  { _D_OR, "or", 2, boolxbool2bool },
  { _D_XOR, "xor", 2, boolxbool2bool },
  { _IP, "ip", 2, mvtxmtv2num },
  { _M_EMPTY, "empty", 1, lmvt2bool },
  { _DIMENSION, "dim", 2, intxlmvt2int },
  { _VDIMENSION, "vdim", 1, lvt2int },
  { _MDIMENSION, "mdim", 2, nscalxmat2int },
  { _LDIMENSION, "ldim", 1, lvt2int },
  { _LSELECT, "lselect", 2, nscalxlist2expr },
  { _VSELECT, "vselect", 2, nscalxvect2expr },
  { _MSELECT, "mselect", 3, nscalxnscalxmat2expr },
  { _LCUT, "lcut", 2, nscalxlist2expr },
  { _VCUT, "vcut", 2, nscalxvect2expr },
  { _MCUT, "mcut", 3, nscalxnscalxmat2expr },
  { _LUNI, "lunite", 2, listxlist2list },
  { _VUNI, "vunite", 2, vectxvect2vect },
  { _MUNI, "munite", 3, nscalxmatxmat2mat },
  { _LROTATE, "lrotate", 2, nscalxlist2expr },
  { _VROTATE, "vrotate", 2, nscalxvect2expr },
  { _MROTATE, "mrotate", 2, nscalxnscalxmat2expr },
  { _REVERSE, "reverse", 1, list2list },
  { _TRANSPOSE, "transpose", 1, vtm2tvm },
  { _LREPLACE, "lreplace", 3, nscalxexprxlist2list },
  { _VREPLACE, "vreplace", 3, nscalxnscalxvect2vect },
  { _MREPLACE, "mreplace", 3, nscalxnscalxnscalxmat2mat },
  { _MREPLACE_C, "mreplace_c", 3, nscalxvectxmat2mat },
  { _MREPLACE_R, "mreplace_r", 3, nscalxvectxmat2mat },
  { _TRANSFORM, "transform", 1, mat2list },
  { _LTRANSFORM, "ltransform", 3, nscalxnscalxlist2mat },
  { _TO_SCAL, "to_scalar", 1, mvt2scal },
  { _TO_VECT, "to_vector", 1, sm2vect },
  { _TO_TVECT, "to_tvector", 1, sm2vect },
  { _TO_MAT, "to_matrix", 1, svt2mat },
  { _VC_PLUS, "plus", 2, vect2num },
  { _VC_MINUS, "minus", 2, vect2num },
  { _VC_MULT, "mult", 2, vect2num },
  { _VC_DIV, "div", 2, vect2num },
  { _VC_MAX, "max", 2, vect2num },
  { _VC_MIN, "min", 2, vect2num },
  { _C_PLUS, "plus", 0, NULL, &undef }, /* return type not decidable */
  { _C_MINUS, "minus", 0, NULL, &undef },
  { _C_MULT, "mult", 0, NULL, &undef },
  { _C_DIV, "div", 0, NULL, &undef },
  { _C_MAX, "max", 0, NULL, &undef },
  { _C_MIN, "min", 0, NULL, &undef }
  };

/* create type entry for primitive function
 */
T_PTD lookupprimf(int instr)
  {
  int i, n;
  T_PTD ptype = undef;

  DBUG_ENTER("lookupprimf");
  MAKEDESC(ptype, 1, C_COMPTYPE, TY_FUNC);
  n = sizeof(asm_primf_fkt)/sizeof(T_asm_primf_fkt);
  for(i = 0; i < n; i++)
    if (asm_primf_fkt[i].instrid == instr) {
      L_COMPTYPE((*ptype), fkt) = instr;
      L_COMPTYPE((*ptype), cnt) = 0;
      L_COMPTYPE((*ptype), nargs) = asm_primf_fkt[i].nargs;
      L_COMPTYPE((*ptype), ptypes) = NULL;
      i = n;
      }
  DBUG_RETURN(ptype);
  }


/* find primitiv function within array asm_primf_fkt
 * return result type for given argumenttypes
 */
T_PTD getretprimf(int instr, char * name)
  {
  int i, j, k = -1, n;
  T_PTD ptype = undef;

  DBUG_ENTER("getretprimf");
  n = sizeof(asm_primf_fkt)/sizeof(T_asm_primf_fkt);
  for(i = 0; i < n; i++)
    if (asm_primf_fkt[i].instrid == instr)
      if (asm_primf_fkt[i].restypefkt) {
        ptype = asm_primf_fkt[i].restypefkt(instr);
        k = i; i = n;
        }
      else {
        for(j = asm_primf_fkt[i].nargs-1; j >= 0; j--)
          if (!equaltype(* asm_primf_fkt[i].argtype[j],
              R_LINKNODE((*(T_PTD)MID_W(j)), ptype)))
            break;
        if (j < 0) {
          ptype = * asm_primf_fkt[i].resulttype;
          INC_REFCNT(ptype);
          k = i; i = n;
          }
        }
  if (k >= 0) {
    strcpy(name, asm_primf_fkt[k].name);
    if (ptype != undef) {
      /* argument types found */
      strcat(name, "_");
      for(j = 0; j < asm_primf_fkt[k].nargs; j++)
        strcat(name, typeid(R_LINKNODE((*(T_PTD)MID_W(j)), ptype)));
      }
    }
  DBUG_RETURN(ptype);
  }
