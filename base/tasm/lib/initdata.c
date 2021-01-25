#define WITHTILDE 1
#define PROLOG_EXTERN
#include "prolog.h"
#include <sys/time.h>

/******************************************************************************/
/*                                                                            */
/* Encoding of the Output sending back to reduma                              */
/*                                                                            */
/* output : tag data                                                          */
/*                                                                            */
/* with                                                                       */
/*                                                                            */
/* data   : length output* (in case of ri_list,ri_frame, ri_intact)           */
/*        : tagged_integer | tagged_boolean  (in case of ri_int or ri_bool)   */
/*        : value_of_real (in case of ri_real)                                */
/*        : arity arg graph_adr  (in case of ri_case, ri_comb or ri_cond)     */
/*        : act_nomat reason ptsdes guard_body[0] guard_body[1] guard_body[2] */
/*          guard_body[3]                                                     */
/*        : arity arg (in case of ri_sub)                                     */
/*        : tag arity arg nfv (in case of closure) ...                        */
/*        : name value (in case of ri_slot)                                   */
/*                                                                            */
/* act_nomat : integer                                                        */
/* reason    : integer                                                        */
/* arity     : integer                                                        */
/* arg       : integer                                                        */
/* graph_adr : integer                                                        */
/* guard_body[0] : integer                                                    */
/* guard_body[1] : output                                                     */
/* guard_body[2] : output                                                     */
/* guard_body[3] : output                                                     */
/* name          : output                                                     */
/* value         : output                                                     */
/*                                                                            */
/* tag    : ri_int | ri_real | ri_list | ri_func                              */
/*        | ri_mat | ri_vect | ri_tvect | ri_cons | ri_prim                   */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

typedef enum {
                ri_bool = 0, ri_int = 1, ri_real = 2, ri_list = 3, ri_clos = 4, 
                ri_mat = 5, ri_vect = 6, ri_tvect = 7, ri_cons = 8, ri_prim = 9,
                ri_comb = 10, ri_cond = 11, ri_expr = 12, ri_case = 13,
                ri_CAVE = 14, ri_nomat = 15, ri_sub = 16, ri_dyncomb = 17,
                ri_char = 18, ri_string = 19, ri_var = 20, ri_const = 21,
                ri_name = 22, ri_constant = 23, ri_undef = 24, ri_frame = 25,
                ri_slot = 26, ri_intact
              } RED_INPUT;
/*******************************************************************************/

int newheap(), controlled_exit();
void lo(int), OutputOfTasm2KiR(int);

FILE *tasm2kir  = stdout;
extern int AsciiOutput, KiROutput, withtilde;
extern char ri_tasmoutfile[120];
extern INTACT_TAB intact_tab[];
extern double start_time;

static int st_number_of_descs, st_number_of_data;
static int *st_data_table,*st_ptc_table;
static PTR_DESCRIPTOR *st_funcdesctable;
#define DESC(x)  (* (PTR_DESCRIPTOR) x)


#ifdef __sparc__
#if (0==NO_STACK_REG)
  register int *st_h asm ("g5");
  register int *st_w asm ("g7");
  register int *st_a asm ("g6");
#else
  extern int *st_h;
  extern int *st_w;
  extern int *st_a;
#endif
#else
  extern int *st_h;
  extern int *st_w;
  extern int *st_a;
#endif

extern PTR_DESCRIPTOR _desc;

#define GET_PTC(x)    search_ptc(x)
#define LASTCHAR      0xFFFFFFDF 
#define IS_CHAR(x)    ((((x) & 0x0000000F) == 0x0000000c) && ((x) & 0x00000010))
#define IS_VAR_CHAR(x) ((((x) & 0x0000000F) == 0x0000000c) &&((x) & 0x00000020))
#define GET_THAT_CHAR(x)   ((x) >> 16)
#define UNTAGLAST(x)  ((x) | ~LASTCHAR)
#define TAGLASTCHAR(x)  ((x) & LASTCHAR)
#define MAKE_VAR_CHAR(x)  (((x) << 16) | 0x0000002c)


int initdata_list_initial (int dim, int *args)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_list_initial");
  
  if (0 == dim)
    _desc = _nil;
  else
  {
    MAKEDESC(_desc, 1, C_LIST, TY_UNDEF);
    DBUG_PRINT("INITDATA",("dim: %d",dim));
    L_LIST((*_desc), dim) = dim;
    L_LIST((*_desc), ptdd) = 0;
    GET_HEAP(dim, A_LIST((*_desc), ptdv));
    DBUG_PRINT("INITDATA",("Got heap"));
    RES_HEAP;
    DBUG_PRINT("INITDATA",("heap reserved"));
    pth = R_LIST(*_desc,ptdv);
    DBUG_PRINT("INITDATA",("pth initialized"));
    for (i = 0; i < dim; i++) 
    {
      DBUG_PRINT("INITDATA",("pth[%d] will be copied next",i));
      pth[i] = args[i];
      DBUG_PRINT("INITDATA",("pth[%d] copied",i));
    }
    L_LIST((*_desc), ptdd) = NULL;
    DBUG_PRINT("INITDATA",("ptdd initialized"));
    L_LIST((*_desc), special) = NULL;
    DBUG_PRINT("INITDATA",("special initialized"));
    REL_HEAP;
    DBUG_PRINT("INITDATA",("heap released"));
  }
  DBUG_RETURN((int)_desc);
}


int initdata_list (int dim)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_list");
  
  if (0 == dim)
    _desc = _nil;
  else
  {
    MAKEDESC(_desc, 1, C_LIST, TY_UNDEF);
    DBUG_PRINT("INITDATA",("dim: %d",dim));
    L_LIST((*_desc), dim) = dim;
    L_LIST((*_desc), ptdd) = 0;
    GET_HEAP(dim, A_LIST((*_desc), ptdv));
    DBUG_PRINT("INITDATA",("Got heap"));
    RES_HEAP;
    DBUG_PRINT("INITDATA",("heap reserved"));
    pth = R_LIST(*_desc,ptdv);
    DBUG_PRINT("INITDATA",("pth initialized"));
    for (i = 0; i < dim; i++) 
    {
      DBUG_PRINT("INITDATA",("pth[%d] will be copied next",i));
      pth[i] = peek(w, dim - i - 1);
      DBUG_PRINT("INITDATA",("pth[%d] copied",i));
    }
    L_LIST((*_desc), ptdd) = NULL;
    DBUG_PRINT("INITDATA",("ptdd initialized"));
    L_LIST((*_desc), special) = NULL;
    DBUG_PRINT("INITDATA",("special initialized"));
    REL_HEAP;
    DBUG_PRINT("INITDATA",("heap released"));
  }
  DBUG_RETURN((int)_desc);
}

int initdata_i_list (int dim)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_i_list");
  
  if (0 == dim)
    _desc = _nil;
  else
  {
    MAKEDESC(_desc, 1, C_LIST, TY_UNDEF);
    L_LIST((*_desc), dim) = dim;
    L_LIST((*_desc), ptdd) = 0;
    GET_HEAP(dim, A_LIST((*_desc), ptdv));
    RES_HEAP;
    pth = R_LIST(*_desc,ptdv);
    for (i = 0; i < dim; i++) 
      pth[i] = peek(w,i); 
    L_LIST((*_desc), ptdd) = NULL;
    L_LIST((*_desc), special) = NULL;
    REL_HEAP;
  }
  DBUG_RETURN((int)_desc);
}

int initdata_string(int dim, int *args)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_string");

  if (0 == dim)
    _desc = _nilstring;
  else
  {
    MAKEDESC(_desc, 1, C_LIST, TY_STRING);
    L_LIST((*_desc), dim) = dim;
    L_LIST((*_desc), ptdd) = 0;
    GET_HEAP(dim, A_LIST((*_desc), ptdv));
    RES_HEAP;
    pth = R_LIST(*_desc,ptdv);
    for (i = 0; i < dim; i++)
      pth[i] = args[i];
    L_LIST((*_desc), ptdd) = NULL;
    L_LIST((*_desc), special) = NULL;
    REL_HEAP;
  }
  DBUG_RETURN((int)_desc);
}

int initdata_free_var(int dim, char *args)
{
  int i;
  T_PTD ptdesc;
  
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_string");

  MAKEDESC(_desc, 1, C_EXPRESSION, TY_NAME);
  GET_HEAP(dim + 1, A_NAME((*_desc), ptn));
  RES_HEAP;
  pth = R_NAME(*_desc, ptn);
  pth[0]=dim;
  for (i = 1; i <= dim; i++)
  {
    pth[i] = args[i-1];
    pth[i] = MAKE_VAR_CHAR(pth[i]);
  }
  pth[dim] = TAGLASTCHAR(pth[dim]);
  REL_HEAP;
  MAKEDESC(ptdesc, 1, C_CONSTANT, TY_VAR);
  L_VAR(*ptdesc, nlabar) = 0;
  L_VAR(*ptdesc, ptnd) = _desc;
  DBUG_RETURN((int)ptdesc);
}

int initdata_matrix(int c, int t, int rows, int cols, int *mvt)
{
  int i;
  double *rmvt, *new_rmvt;
  T_PTD ptdesc;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_matrix");
  
  if (0 == rows * cols)
  {
    if (C_MATRIX == c)
      _desc = _nilmat;
    else if (C_VECTOR == c)
      _desc = _nilvect;
    else if (C_TVECTOR == c)
      _desc = _niltvect;
  }
  else
  {
    MAKEDESC(_desc, 1, c, t);
    L_MVT(*_desc, ptdd, c) = NULL;
    L_MVT(*_desc, nrows, c) = rows;
    L_MVT(*_desc, ncols, c) = cols;
    if (TY_REAL == t)
    {
      GET_HEAP(rows * cols * sizeof(double) / sizeof(T_HEAPELEM), A_MVT(*_desc, ptdv, c));
      RES_HEAP;
      new_rmvt = (double *)R_MVT(*_desc, ptdv, c);
      rmvt = (double *)mvt;
      for(i = 0; i < rows * cols; i++)
        new_rmvt[i] = (double)rmvt[i];
      L_MVT(*_desc, ptdv, c) = (PTR_HEAPELEM)new_rmvt;
      REL_HEAP;
    }
    else
    {
      GET_HEAP(rows * cols, A_MVT((*_desc), ptdv, c));
      for(i = 0; i < rows * cols; i++)
        L_MVT(*_desc, ptdv, c)[i] = mvt[i];
    }
  }
  DBUG_RETURN((int)_desc);  
}

/* cr 15/03/95, kir(ff), START */
int initdata_inter (DESC_TYPE type,int dim, int *args)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_inter");
  
  MAKEDESC(_desc, 1, C_INTER, type);
  L_INTER((*_desc), dim) = dim;
  GET_HEAP(dim, A_INTER((*_desc), args));
  RES_HEAP;
  pth = R_INTER(*_desc,args) + dim  - 1;
  for (i = 0; i < dim; i++) 
    pth[-i] = args[i]; 
  REL_HEAP;
  DBUG_RETURN((int)_desc);
}

int initdata_frame (int dim, int *args)
{
  int i;
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_frame");
  
  MAKEDESC(_desc, 1, C_FRAME, TY_FRAME);
  L_FRAME((*_desc), dim) = dim;
  GET_HEAP(dim, A_FRAME((*_desc), slots));
  RES_HEAP;
  pth = R_FRAME(*_desc,slots);
  for (i = 0; i < dim; i++) 
    pth[i] = args[i]; 
  REL_HEAP;
  DBUG_RETURN((int)_desc);
}

int initdata_slot (int dim, int *args)
{
  PTR_HEAPELEM pth; /* pointer to heapelement */
  DBUG_ENTER("initdata_slot");
  
  MAKEDESC(_desc, 1, C_FRAME, TY_SLOT);
  L_SLOT((*_desc),name) = (PTR_DESCRIPTOR)(args[0]);
  L_SLOT((*_desc),value) = args[1];
  DBUG_RETURN((int)_desc);
}
/* cr 15/03/95, kir(ff), END */

/* stt, 13.02.96 */
int initdata_intact(int intact)
{
  int i;
  PTR_HEAPELEM pth;

  DBUG_ENTER("initdata_intact");
  MAKEDESC(_desc, 1, C_INTACT, TY_INTACT);
  L_INTACT(*_desc,dim)  = FUNC_ARITY(intact) + 1;
  GET_HEAP(FUNC_ARITY(intact) + 2, A_INTACT(*_desc,args));
  RES_HEAP;
  pth = R_INTACT(*_desc,args);
  *pth++ = intact;
  for (i=0; i<FUNC_ARITY(intact); i++)
    *pth++ = peek(w, i);
  REL_HEAP;
  DBUG_RETURN((int)_desc);
}


int initdata_scalar(double value)
{
  DBUG_ENTER("initdata_scalar");
  
  MAKEDESC(_desc,1,C_SCALAR, TY_REAL);
  L_SCALAR(*_desc, valr) = value;
  DBUG_RETURN((int)_desc);
}  

PTR_DESCRIPTOR initdata_new_scalar(double value, int h_desc)
{
  DBUG_ENTER("initdata_new_scalar");
  
  MAKEDESC((PTR_DESCRIPTOR)h_desc,1,C_SCALAR, TY_REAL);
  L_SCALAR(*(PTR_DESCRIPTOR)h_desc, valr) = value;
  DBUG_RETURN((PTR_DESCRIPTOR)h_desc);
}  


int search_ptc(int d)
{
  int i, class;
  DBUG_ENTER("search_ptc");
  switch(R_DESC(DESC(d), type))
  {
    case TY_COMB:
      for (i = 0; i < 2 * st_number_of_descs; i+=2)
      {
        if (st_ptc_table[i] == (int)R_COMB(DESC(d), ptd))
          break;
      }
      break;
    case TY_CASE:
      for (i = 0; i < 2 * st_number_of_descs; i+=2)
      {
        if (st_ptc_table[i] == (int)R_CASE(DESC(d), ptd))
          break;
      }
      break;
    case TY_CONDI:
      for (i = 0; i < 2 * st_number_of_descs; i+=2)
      {
        if (st_ptc_table[i] == (int)R_CONDI(DESC(d), ptd))
          break;
      }
      break;
  }
  DBUG_RETURN((int)st_ptc_table[i + 1]);
}

/*************************************************************/
/* gets a stack element and prints it out                    */
/* now only lists, reals, booleans and integers              */
/*************************************************************/
void lo(int lw)
{
int i, j;
char class[10];
double *rmvt;
int mvt_class;

  if (T_INT(lw))  
    printf("%d",VAL_INT(lw));       
  else if (T_BOOLEAN(lw))                           
  {                                              
    if (T_SA_FALSE(lw))                        
      printf("false");                  
    else if (T_SA_TRUE(lw))                    
      printf("true");                   
    else printf("Error in boolean result !\n");  
  }                                              
  else if ((IS_CHAR(lw)) || (IS_VAR_CHAR(lw)))
  {
    if (isprint((char)GET_THAT_CHAR(lw)))
      printf("%c", (char)GET_THAT_CHAR(lw));
    else
      printf("'\\%03o'", (char)GET_THAT_CHAR(lw));      /* stt 20.02.96 */
  }
  else if (T_POINTER(lw)) 
  {                        
    if ((R_DESC(DESC(lw),class)==C_SCALAR) && (R_DESC(DESC(lw),type)==TY_REAL))
      printf("%.16g", R_SCALAR(DESC(lw),valr)) ; 
    else if ((R_DESC(DESC(lw), class) == C_MATRIX) 
              || (R_DESC(DESC(lw), class) == C_VECTOR)
              || (R_DESC(DESC(lw), class) == C_TVECTOR))
    {
      if (R_DESC(DESC(lw), class) == C_MATRIX)
      {
        strcpy(class, "mat");
        mvt_class = C_MATRIX;
      }
      else if (R_DESC(DESC(lw), class) == C_VECTOR)
      {
        strcpy(class, "vect");
        mvt_class = C_VECTOR;
      }
      else if (R_DESC(DESC(lw), class) == C_TVECTOR)
      {
        strcpy(class, "tvect");
        mvt_class = C_TVECTOR;
      }
      switch (R_MVT(DESC(lw), type, mvt_class))
      {
        case TY_INTEGER:
          printf("%s\n", class);
          if (0 == R_MVT(DESC(lw), nrows, mvt_class) * R_MVT(DESC(lw), nrows, mvt_class))
            printf("<>\n");
          else
          {
            int rows, cols;
            if (R_DESC(DESC(lw), class) == C_TVECTOR)
            {
              cols = R_MVT(DESC(lw), nrows, mvt_class);
              rows = R_MVT(DESC(lw), ncols, mvt_class);
            }
            else
            {
              rows = R_MVT(DESC(lw), nrows, mvt_class);
              cols = R_MVT(DESC(lw), ncols, mvt_class);
            }
            for(i = 0; i < rows; i++)
            {
              printf("<");
              if (0 < R_MVT(DESC(lw), ncols, mvt_class))
                printf("%d", 
                  R_MVT(DESC(lw), ptdv, mvt_class)[i * R_MVT(DESC(lw), ncols, mvt_class)]);
              for(j = 1; j < cols; j++)
              {
                if (R_DESC(DESC(lw), class) == C_TVECTOR)
                  printf(",\n %d", 
                    R_MVT(DESC(lw), ptdv, mvt_class)[i*R_MVT(DESC(lw), ncols, mvt_class)+j]);
                else
                  printf(", %d", 
                    R_MVT(DESC(lw), ptdv, mvt_class)[i*R_MVT(DESC(lw), ncols, mvt_class)+j]);
              }
              printf(">\n");
            }
          }
          break;
        case TY_BOOL:
          printf("b%s\n", class);
          if (0 == R_MVT(DESC(lw), nrows, mvt_class) * R_MVT(DESC(lw), nrows, mvt_class))
            printf("<>\n");
          else
          {
            for(i = 0; i < R_MVT(DESC(lw), nrows, mvt_class); i++)
            {
              printf("<");
              if (0 < R_MVT(DESC(lw), ncols, mvt_class))
                printf("%s", 
                  R_MVT(DESC(lw), ptdv, mvt_class)[i*R_MVT(DESC(lw), ncols, mvt_class)] 
                  ? "true" : "false");
              for(j = 1; j < R_MVT(DESC(lw), ncols, mvt_class); j++)
              {
                if (R_DESC(DESC(lw), class) == C_TVECTOR)
                  printf(",\n\t%s", R_MVT(DESC(lw), ptdv, mvt_class)
                          [i*R_MVT(DESC(lw), ncols, mvt_class) + j] ? "true" : "false");
                else
                  printf(", %s",
                    R_MVT(DESC(lw), ptdv, mvt_class)[i*R_MVT(DESC(lw), ncols, mvt_class) + j]
                                  ? "true" : "false");
              }
              printf(">\n");
            }
          }
          break;
        case TY_REAL:
          rmvt = (double *)R_MVT(DESC(lw), ptdv, mvt_class);
          printf("%s\n", class);
          if (0 == R_MVT(DESC(lw), nrows, mvt_class) * R_MVT(DESC(lw), nrows, mvt_class))
            printf("<>\n");
          else
          {
            for(i = 0; i < R_MVT(DESC(lw), nrows, mvt_class); i++)
            {
              printf("<");
              if (0 < R_MVT(DESC(lw), ncols, mvt_class))
                printf("%.16g",
                  rmvt[i * R_MVT(DESC(lw), ncols, mvt_class)]);
              for(j = 1; j < R_MVT(DESC(lw), ncols, mvt_class); j++)
              {
                if (R_DESC(DESC(lw), class) == C_TVECTOR)
                  printf(",\n\t%.16g",
                    rmvt[i * R_MVT(DESC(lw), ncols, mvt_class) + j]);
                else
                  printf(", %.16g",
                    rmvt[i * R_MVT(DESC(lw), ncols, mvt_class) + j]);
              }
              printf(">\n");
            }
          }
          break;
        case TY_UNDEF:
          printf("nil%s", class);
        break;
        default:;
      }
    } 
    else if (R_DESC(DESC(lw),class) == C_LIST)  
    {
      if (R_DESC(DESC(lw), type) == TY_UNDEF)
      {
        if (0 != ((T_PTD)lw)->u.li.dim)
        {
          printf("< ");        
          for(i=0; i+1<((T_PTD)lw)->u.li.dim; i++)
          {
            lo(((T_PTD)lw)->u.li.ptdv[i]);
            printf(", ");
          }
          lo(((T_PTD)lw)->u.li.ptdv[i]);
          printf(" >");
        }
        else printf("<>");
      }
      else if(R_DESC(DESC(lw), type) == TY_STRING)
      {
        if (0 != ((T_PTD)lw)->u.li.dim)
        {
          printf("'");
          for(i=0; i < ((T_PTD)lw)->u.li.dim; i++)
            lo(((T_PTD)lw)->u.li.ptdv[i]);
          printf("`");
        }
        else printf("'`");
      }
      else
        printf("(lo) unknown C_LIST object %d\n",lw);
    }
    else if (R_DESC(DESC(lw), class) == C_CONSTANT)
    {
      if (R_DESC(DESC(lw),type) == TY_VAR)
      {
        lo((int)((T_PTD)lw)->u.va.ptnd);
      }
      else
        printf("(lo) unknown C_CONSTANT object %d\n",lw);
    }
    else if (R_DESC(DESC(lw), class) == C_EXPRESSION)
    {
      if (R_DESC(DESC(lw), type) == TY_NAME)
      {
        int dim;
        int *help;
        dim = (((T_PTD)lw)->u.na.ptn)[0];
        help = (int *)(((T_PTD)lw)->u.na.ptn);
        for (i = 1; i < dim; i++)
          printf("%c", (char)(GET_THAT_CHAR(help[i])));
        printf("%c", (char)(GET_THAT_CHAR(UNTAGLAST(help[dim]))));
      }
      else
        printf("(lo) unknown C_EXPRESSION object %d\n",lw);
    }
/* cr 09/03/95, kir(ff), START */
      else if (R_DESC(DESC(lw),class) == C_FRAME)
      {
        if (R_DESC(DESC(lw),type) == TY_FRAME)
        {
            if (0 != ((T_PTD)lw)->u.frame.dim)
            {
              printf("<: ");        
              for(i=0; i+1<((T_PTD)lw)->u.frame.dim; i++)
              {
                lo(((T_PTD)lw)->u.frame.slots[i]);
                printf(", ");
              }
              lo(((T_PTD)lw)->u.frame.slots[i]);
              printf(" :>");
            }
            else printf("<: :>");
        }
        else if (R_DESC(DESC(lw),type) == TY_SLOT)
        {
            lo((int)(((T_PTD)lw)->u.slot.name));
            printf(":: ");
            lo(((T_PTD)lw)->u.slot.value);
        }
        else
            printf("(lo) unknown C_FRAME object %d\n",lw);
      }
      else if (R_DESC(DESC(lw),class) == C_INTER)
        switch (R_DESC(DESC(lw),type))
        {
            case TY_PUT: printf("put{ ");
                         lo(((T_PTD)lw)->u.inter.args[0]);
                         printf(", ");
                         lo(((T_PTD)lw)->u.inter.args[1]);
                         printf(", ");
                         lo(((T_PTD)lw)->u.inter.args[2]);
                         printf("} ");
                         break;
            case TY_GET: printf("get{ ");
                         lo(((T_PTD)lw)->u.inter.args[0]);
                         printf(", ");
                         lo(((T_PTD)lw)->u.inter.args[1]);
                         printf("} ");
                         break;
            case TY_DONE: printf("done{ } ");
                         break;
            default:     printf("(lo) unknown C_INTER object %d\n",lw);
        }
/* cr 09/03/95, kir(ff), END */
         /* stt, 13.02.96 */
    else if ((R_DESC(DESC(lw),class) == C_INTACT) &&
             (R_DESC(DESC(lw),type) == TY_INTACT) )
    {
      PTR_HEAPELEM pth; /* pointer to heapelement */

      pth = (PTR_HEAPELEM) R_INTACT(DESC(lw),args);
      printf("%s( ", intact_tab[FUNC_INDEX(*pth++)].name);
      for(i = R_INTACT(DESC(lw),dim) - 1; i>0; i--)
      {
        lo(*pth++);
        if (i == 1)
          printf(" ) ");
        else
          printf(", ");
      }
    }
    else
      printf("(lo) unknown Pointer %d !\n",lw);
  }
  else if T_IAFUNC(lw)
    printf("%s ", intact_tab[FUNC_INDEX(lw)].name);
  else                                       
    printf("(lo) Something not a list, integer or real in result %d !\n",lw); 
}

/*************************************************************/

void t_output(int lw, int number_of_descs, PTR_DESCRIPTOR *funcdesctable, int number_of_data, int *data_table, int *ptc_table)
{
  struct timeval t0;
  double used_time;

  if (1 == AsciiOutput) lo(lw);
  if (1 == KiROutput)
  {
    tasm2kir = stdout;
    if (strcmp(ri_tasmoutfile, "__default"))
    if (NULL == (tasm2kir = fopen(ri_tasmoutfile, "w")))
    {
      fprintf(stderr, " Couldn't open %s !\n", ri_tasmoutfile);
      exit(1);
    }

    /* used time */
    gettimeofday (&t0, NULL);
    used_time = t0.tv_sec + t0.tv_usec/1000000.0 - start_time;
    fprintf(tasm2kir, "%f\n", used_time);
    /*************/

    st_number_of_descs = number_of_descs;  /* prepare for call to OutputOfTasm2KiR */
    st_number_of_data = number_of_data;    /* need to avoid global variables */
    st_data_table = data_table;            /* if we want to use modules */
    st_ptc_table = ptc_table;              /* but don't want to pass recursively */
    st_funcdesctable = funcdesctable;      /* thus we copy them to static */
                                           /* variables here */

    OutputOfTasm2KiR(lw);
    fclose(tasm2kir);
  }
}

int AreYouInfuncdesctable(const int me)
{
  int i;
  for (i = 0; i < st_number_of_descs; i++)
    if (me == (int)st_funcdesctable[i]) return(TRUE);
  return(FALSE);
}

int AreYouIndatatable(const int me)
{
  int i;
  for (i = 0; i < st_number_of_data; i++)
    if (me == (int)&st_data_table[i]) return(TRUE);
  return(FALSE);
}
/*******************************************************************************/
/* "unreadable" output to file tasm2kir to send result back to reduma          */
/*******************************************************************************/
void OutputOfTasm2KiR(int lw)
{
  int i, dim;
  int mvt_class;
  PTR_HEAPELEM pth;
  
  if (NULL == lw)
    controlled_exit("Unexpected ZERO on top of stack W");
  else if (T_INT(lw))
    fprintf(tasm2kir, " %d %d", ri_int, lw);
  else if (lw &0x2)
    fprintf(tasm2kir, " %d %d", ri_int, lw);
  else if (T_BOOLEAN(lw))
    fprintf(tasm2kir, " %d %d", ri_bool, lw);
  else if (T_CONSTANT(lw))
    fprintf(tasm2kir, " %d %d", ri_constant, lw);
  else if ((T_FUNC(lw) && (!AreYouInfuncdesctable(lw))) || T_IAFUNC(lw))
    fprintf(tasm2kir, " %d %d", ri_prim, lw);
  else if (T_STR_CHAR(lw))
    fprintf(tasm2kir, " %d %d", ri_char, lw);
  else if ((R_DESC(DESC(lw),class) == C_SCALAR) && (R_DESC(DESC(lw),type)==TY_REAL))
    fprintf(tasm2kir, " %d %.16g", ri_real, R_SCALAR(DESC(lw), valr));
  else if ((R_DESC(DESC(lw),class) == C_MATRIX) || (R_DESC(DESC(lw),class) == C_VECTOR) ||
                (R_DESC(DESC(lw),class) == C_TVECTOR))
  {
    mvt_class = R_DESC(DESC(lw), class);
    switch(mvt_class)
    {
      case C_MATRIX:
        fprintf(tasm2kir, " %d", ri_mat);
        break;
      case C_VECTOR:
        fprintf(tasm2kir, " %d", ri_vect);
        break;
      case C_TVECTOR:
        fprintf(tasm2kir, " %d", ri_tvect);
        break;
      default:;
    }
    switch(R_DESC(DESC(lw), type))
    {
      case TY_INTEGER:
        fprintf(tasm2kir, " %d", ri_int);
        break;
      case TY_BOOL:
        fprintf(tasm2kir, " %d", ri_bool);
        break;
      case TY_REAL:
        fprintf(tasm2kir, " %d", ri_real);
        break;
      case TY_UNDEF:
        fprintf(tasm2kir, " %d", ri_undef);
        break;
    }
    fprintf(tasm2kir, " %d", R_MVT(DESC(lw), nrows, mvt_class));
    fprintf(tasm2kir, " %d", R_MVT(DESC(lw), ncols, mvt_class));
    switch(R_DESC(DESC(lw), type))
    {
      case TY_UNDEF:
        break;
      case TY_INTEGER:
      case TY_BOOL:
        for(i = 0; 
          i < R_MVT(DESC(lw), nrows, mvt_class) * R_MVT(DESC(lw), ncols, mvt_class); i++)
          fprintf(tasm2kir, " %d", R_MVT(DESC(lw), ptdv, mvt_class)[i]);
        break;
      case TY_REAL:
      {
        double *rmvt;
        rmvt = (double *)R_MVT(DESC(lw), ptdv, mvt_class);
        for(i = 0; 
          i < R_MVT(DESC(lw), nrows, mvt_class) * R_MVT(DESC(lw), ncols, mvt_class); i++)
          fprintf(tasm2kir, " %.16g", rmvt[i]);
        break;
      }
    }
  }
  else if (C_CONSTANT == R_DESC(DESC(lw),class))
  {
    if (TY_VAR == R_DESC(DESC(lw), type))
    {
      fprintf(tasm2kir, " %d %d %d", ri_const, ri_var, R_VAR(DESC(lw), nlabar));
      OutputOfTasm2KiR((int)R_VAR(DESC(lw), ptnd));
    }
  }
  else if (C_LIST == R_DESC(DESC(lw),class))
  {
    if (TY_UNDEF == R_DESC(DESC(lw), type))
    {
      RED_INPUT hilfe;
      int isstr;
      dim = R_LIST(DESC(lw), dim);
      hilfe = ri_list;
#if 0
      if (0 < dim)
        if (T_POINTER(lw) || 
                       (T_FUNC(R_LIST(DESC(lw), ptdv)[0]) && (!AreYouIndatatable(lw))))
          isstr = R_LIST(DESC(lw), ptdv)[0];
          if (T_POINTER(isstr))
            if ((TY_STRING == R_DESC(DESC(isstr), type)) && R_LIST(DESC(isstr), dim))
              if (MAKE_CHAR('\137') == R_LIST(DESC(R_LIST(DESC(lw), ptdv)[0]), ptdv)[0])
                hilfe = ri_cons; /* it is a _ -> assuming a userdefined constructor */
#endif
      fprintf(tasm2kir, " %d %d", hilfe, dim);
      pth = R_LIST(DESC(lw), ptdv);
      for (i = 0; i < dim; i++) OutputOfTasm2KiR(pth[i]);
    }
    else if (TY_STRING == R_DESC(DESC(lw), type))
    {
      dim = R_LIST(DESC(lw), dim);
      fprintf(tasm2kir, " %d %d", ri_string, dim);
      pth = R_LIST(DESC(lw), ptdv);
      for (i = 0; i < dim; i++) 
        OutputOfTasm2KiR(pth[i]);
    }
  }
  else if (C_FRAME == R_DESC(DESC(lw),class)) /* cr 16.02.96 */
  {
    if (TY_FRAME == R_DESC(DESC(lw), type))
    {
      RED_INPUT hilfe;
      int isstr;
      dim = R_FRAME(DESC(lw), dim);
      hilfe = ri_frame;
      fprintf(tasm2kir, " %d %d", hilfe, dim);
      pth = R_FRAME(DESC(lw), slots);
      for (i = 0; i < dim; i++) OutputOfTasm2KiR(pth[i]);
    }
    else if (TY_SLOT == R_DESC(DESC(lw), type))
    {
      fprintf(tasm2kir, " %d", ri_slot);
      OutputOfTasm2KiR((T_HEAPELEM)R_SLOT(DESC(lw), name));
      OutputOfTasm2KiR(R_SLOT(DESC(lw), value));
    }
  }
  else if (C_EXPRESSION == R_DESC(DESC(lw), class))
  {
    if (TY_NAME == R_DESC(DESC(lw), type))
    {
      int j;
      fprintf(tasm2kir, " %d %d", ri_expr, ri_name);
      i = R_NAME(DESC(lw), ptn)[0];
      fprintf(tasm2kir, " %d", i);
      for (j = 1; j <= i; j++)
        fprintf(tasm2kir, " %d",R_NAME(DESC(lw), ptn)[j]);
    }
    else if (TY_EXPR == R_DESC(DESC(lw), type))
    {
      fprintf(tasm2kir, " %d %d %d %d", ri_expr, ri_expr, 
                                     R_EXPR(DESC(lw), pte)[0],
                                     R_EXPR(DESC(lw), pte)[1]);
      pth = (PTR_HEAPELEM)R_EXPR(DESC(lw), pte) + 2;
      for (i = 0; i < R_EXPR(DESC(lw), pte)[0] - 1; i++)
        OutputOfTasm2KiR(pth[i]);
    }
    if (TY_SUB == R_DESC(DESC(lw), type))
    {
      int arg1, arg2;

      pth = (PTR_HEAPELEM)R_EXPR(DESC(lw), pte);
      arg1 = (int)*pth++;
      arg2 = (int)*pth++;
      fprintf(tasm2kir," %d %d %d %d", ri_expr, ri_sub, arg1, arg2);
      for (i = 0; i < arg1 - 1; i++)
        OutputOfTasm2KiR(pth[i]);
    }

    else if (TY_NOMAT == R_DESC(DESC(lw), type))
    {
      fprintf(tasm2kir, " %d %d %d %d %d %d %d %d", ri_expr, ri_nomat,
                                        R_NOMAT(DESC(lw), act_nomat),
                                        R_NOMAT(DESC(lw), reason),
                                        (int)R_NOMAT(DESC(lw), ptsdes),
                                        R_NOMAT(DESC(lw), guard_body)[0], /* length */
                                        R_NOMAT(DESC(lw), guard_body)[1], /* guard */
                                        R_NOMAT(DESC(lw), guard_body)[2]); /* body */
      OutputOfTasm2KiR(R_NOMAT(DESC(lw), guard_body)[3]); /* arg from match */

    }
      
  }
  else if (C_FUNC == R_DESC(DESC(lw), class))
  {
    int arity; /* used args */
    int arg;   /* needed args */
    int nfv;   /* # free vars if withtilde */
    int graph_adr; /* where to find to @-graph */
    if (TY_CASE == R_DESC(DESC(lw), type))
    {
      arity = R_COMB(DESC(lw), nargs);
      arg = R_COMB(DESC(lw), args);
      graph_adr = (int) R_COMB(DESC(lw), ptd);
      fprintf(tasm2kir, " %d %d %d %d %d", ri_case, arity, arg, graph_adr, GET_PTC(lw));
    }
    if (TY_COMB == R_DESC(DESC(lw), type))
    {
      arity = R_COMB(DESC(lw), nargs);
      arg = R_COMB(DESC(lw), args);
      graph_adr = (int) R_COMB(DESC(lw), ptd);
      if (AreYouInfuncdesctable(lw))
        fprintf(tasm2kir, " %d %d %d %d %d", ri_comb, arity, arg, graph_adr, GET_PTC(lw));
      else
      {  
        fprintf(tasm2kir, " %d %d %d", ri_dyncomb, arity, arg, graph_adr);
        OutputOfTasm2KiR(graph_adr);
      }
    }
    if (TY_CONDI == R_DESC(DESC(lw), type))
    {
      arity = R_CONDI(DESC(lw), nargs);
      arg = R_CONDI(DESC(lw), args);
      graph_adr = (int) R_CONDI(DESC(lw), ptd);
      fprintf(tasm2kir, " %d %d %d %d %d", ri_cond, arity, arg, graph_adr, GET_PTC(lw));
    }
    else if (TY_CLOS == R_DESC(DESC(lw), type))
    {
      if (TY_NOMATBODY == R_CLOS(DESC(lw), ftype))
      {
        int hilf;
        
        pth = R_CLOS(DESC(lw), pta);
        arity = R_CLOS(DESC(lw), args);
        arg = R_CLOS(DESC(lw), nargs);
        nfv = R_CLOS(DESC(lw), nfv);

        fprintf(tasm2kir, " %d %d %d %d %d", ri_clos, ri_nomat, arity, arg, nfv);
       
        hilf = nfv + arity;
        fprintf(tasm2kir, " %d %d", pth[0], pth[1]); /* ptr to SWITCH-expression, 
                                                        # of failed when */
        OutputOfTasm2KiR(pth[2]);   /* guard */
        OutputOfTasm2KiR(pth[3]);   /* match */
        pth += 4;
        while (hilf-- > 0)       /* first arity elems are bound vars, other nfvs */
          OutputOfTasm2KiR(*pth++);
      }
      else if ((TY_PRIM   == R_CLOS(DESC(lw), ftype)) ||
               (TY_INTACT == R_CLOS(DESC(lw), ftype)))
      {
        pth = R_CLOS(DESC(lw), pta);
        arity = R_CLOS(DESC(lw), args); 
        arg = R_CLOS(DESC(lw), nargs);
        fprintf(tasm2kir," %d %d %d %d %d", ri_clos,
                   ((TY_PRIM == R_CLOS(DESC(lw), ftype)) ? ri_prim : ri_intact),
                   *pth++, arity, arg);
        for (i = 0; i < arity; i++) OutputOfTasm2KiR(pth[i]);
      }
      else if (TY_COMB == R_CLOS(DESC(lw), ftype))
      {
        if (withtilde)
          nfv = R_CLOS(DESC(lw), nfv);
        else
          nfv = 0;
  
        arity = R_CLOS(DESC(lw), args);
        arg = R_CLOS(DESC(lw), nargs);
        fprintf(tasm2kir," %d %d %d %d %d", ri_clos, ri_comb, arity, arg, nfv);
        pth = R_CLOS(DESC(lw), pta);
        for (i = 0; i < arity + nfv + 1; i++) OutputOfTasm2KiR(pth[i]);
      }
      else if (TY_CASE == R_CLOS(DESC(lw), ftype))
      { 
        if (withtilde)
          nfv = R_CLOS(DESC(lw), nfv);
        else
          nfv = 0;

        arity = R_CLOS(DESC(lw), args);
        arg = R_CLOS(DESC(lw), nargs);
        fprintf(tasm2kir," %d %d %d %d %d", ri_clos, ri_case, arity, arg, nfv);
        pth = R_CLOS(DESC(lw), pta);
        for (i = 0; i < arity + nfv + 1; i++) OutputOfTasm2KiR(pth[i]);
      }

      else if (TY_CONDI == R_CLOS(DESC(lw), ftype))
      {
        if (withtilde)
          nfv = (R_CLOS(DESC(lw), nfv));
        else
          nfv = 0;
        arity = R_CLOS(DESC(lw), args);
        arg = R_CLOS(DESC(lw), nargs);
        fprintf(tasm2kir," %d %d %d %d %d", ri_clos, ri_cond, arity, arg, nfv);
        pth = R_CLOS(DESC(lw), pta);
        for (i = 0; i < arity + nfv + 1; i++) OutputOfTasm2KiR(pth[i]);
      }
    }
  } 
  else if ((C_INTACT == R_DESC(DESC(lw), class)) &&
           (TY_INTACT == R_DESC(DESC(lw), type)))
  {
    int dim;

    dim = R_INTACT(DESC(lw), dim);
    pth = R_INTACT(DESC(lw), args);
    fprintf(tasm2kir, " %d %d", ri_intact, dim);
    for (i = 0; i < dim; i++) OutputOfTasm2KiR(pth[i]);
  }
}


int controlled_exit(const char *s)
{
  FILE *error_out;
  int a, i;

  if (KiROutput)
  {
    error_out = fopen(ri_tasmoutfile, "w");
    fprintf(error_out, " %d %d", ri_string, strlen(s));
    for (i = 0; i < strlen(s); i++)
    {
      a = (int)s[i];
      fprintf(error_out, " %d %d", ri_char, MAKE_CHAR(a));
    }
    fclose(error_out);
  }
  else 
    fprintf(stderr, "%s\n", s);
  exit(0);
  return(1);
}

void trap_handler(void)
{
  controlled_exit("Division by zero");
}

static int matherr(struct exception *a)
{
#if (0==D_SLAVE)
  if (DOMAIN == a->type)
    return(controlled_exit("Error occured in function"));
  else
#endif /* D_SLAVE */
    return(controlled_exit("Unknown Error"));
#if (0==D_SLAVE)
  return(0);
#endif /* D_SLAVE */
}

void clearscreen(void)
{}
