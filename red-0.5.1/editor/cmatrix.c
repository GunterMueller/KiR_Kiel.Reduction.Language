/*                                                cmatrix.c   */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "cedit.h"
#include "cestack.h"
#include "cencod.h"
#include "cetb.h"
#include "cback.h"

/* -------- matpath1 ------------------------------------------------- */
matpath1()
{
 STACKELEM m;
 STACK X;

 asgnst(&X,MST);
 popnf(&X);
 while (class(m = popnf(&X)) == AR0(LIST))
  { if (arity(m) > 1L)
       return(FALSE);
  }
  if (arity(m) > 1L)
    return(FALSE);
 return(TRUE);
}
/* -------- savemasc ------------------------------------------------- */
/* matrix-maske ?                                                      */
savemasc(st1,st2)
STACK *st1,*st2;
{
 static STACKELEM m;
 register long n;

 if ((m = topof(st1)) != STBOTTOM)
  { if (isconstr(m))
     { movenf(st1,MST);
       for (n = arity(m) ; n > 0L ; --n)
          savemasc(st1,st2);
       move(MST,st2);
     }
    else /* atom */
     { push(st2,PLACEH | (m & EDIT_FIELD));
       deletenf(st1);
     }
  }
}
/* ----------- getmtel ----------------------------------------------- */
getmtel(pstk,eno,pts)
STACK *pstk;
int eno;
char *(*pts);
{
 char *skipblanks();
 static STACKELEM m;
 register long n;
 static int c;

 *pts = skipblanks(*pts);
 c = *(*pts);
 if (c != '\0')
  { if (c == CRSEXPR)
     { if (iskomp(etptb[eno].exptp,etptb[(int)edit6(topof(EST))].eetp) &&
            matchmat(pstk,EST))
        { m = pop(EST);
          push(EST,(m & ~EDIT_FIELD) | toedit6(eno));
          duplicate(EST,BST);
          pop(EST);
          push(EST,m);
          ++(*pts);
          push(BST,(pop(BST) & ~EDIT_FIELD) | toedit6(eno));
        }
       else
        { push(BST,PLACEH);
          c = -1;
        }
     }
    else
     { m = topof(pstk);
       if (isconstr(m)  && class(m) == AR0(LIST))
        { popnf(pstk);
          if (c == LSTBEGIN)
           { ++(*pts);
             push(MST,m & ~VALUE_FIELD);
             for (n = arity(m) ; n > 0L ; --n)
              { incrarity(MST);
                if ((c = getmtel(pstk,eno,pts)) < 0)
                   break;
                if ((c == ',') && (n > 1L))
                   ++(*pts);
              }
             move(MST,BST);
             if (c == LSTEND)
                ++(*pts);
           }
          else
           { push(BST,PLACEH);
             return(-1);
           }
        }
       else
        { if (isatom(m)) popnf(pstk);
          else deletenf(pstk);
          switch(eno = (int)edit6(m))
           { case n_nmatels : c = getreal(eno,pts);  /* modified       */
                              break;                 /* oder getnumber ? */
             case n_bmatels : c = getexp(n_bval,pts,0);    /* ? */
                              break;
             case n_smatels : c = getstr(eno,pts);
                              break;
             default        : c = getexp(eno,pts,0);       /* ? */
           }
        }
     }
  }
 else transpnf(pstk,BST);
 if (c < 0)
    return(c);
 *pts = skipblanks(*pts);
 return(*(*pts));
}

/* ------------ getmatels -------------------------------------------- */
getmatels(eno,pts)
int eno;
char *(*pts);
{
 static int c;
 STACK X,Y;

 X.stptr = SNULL;
 crstack(&X,8);

 if (!matpath1()) {
    if (arity(topof(M1ST)) != 0L)  /* maske auf E    */
       asgnst(&Y,EST);                      else
    if (topof(U1ST) != STBOTTOM)   /* neu (ca), wg. listen */
       asgnst(&Y,U1ST);
    else
       asgnst(&Y,BST);
    savemasc(&Y,&X);
    c = getmtel(&X,eno,pts);
  }
 else /* mapath1 == YES */
  { if (arity(topof(M1ST)) > 1L)
     { asgnst(&Y,EST);             /* richtig ? ca */
       savemasc(&Y,&X);
       asgnst(&Y,&X);
       while ((c = getmtel(&Y,eno,pts)) == ',')
        { incrarity(MST);
          ++(*pts);
          asgnst(&Y,&X);
        }
     }
    else {
       if (arity(topof(MST)) > arity(topof(M1ST))) {  /* > 1 oder > 0 */
          if (topof(BST) != STBOTTOM)
             asgnst(&Y,BST);
          else
          if (topof(U1ST) != STBOTTOM)      /* neu (ca)       */
             asgnst(&Y,U1ST);
          else
             asgnst(&Y,AST);
          savemasc(&Y,&X);
          asgnst(&Y,&X);
          while ((c = getmtel(&Y,eno,pts)) == ',')
           { incrarity(MST);
             ++(*pts);
             asgnst(&Y,&X);
           }
        }
       else  /* arity(topof(MST)) == arity(topof(M1ST)) */
        { if ((c = getexp(eno,pts,0)) == ',')              /* ? */
             { incrarity(MST);
               ++(*pts);
               c = getmatels(eno,pts);
             }
        }
     }
  }
 killstack(&X);
 return(c);
}

/* ------------ getmatrows ------------------------------------------- */
getmatrows(eno,pts)
int eno;
char *(*pts);
{
 static int c;
 STACK X,Y;

 X.stptr = SNULL;
 crstack(&X,32);

 if (arity(topof(M1ST)) > 1L)
  { asgnst(&Y,EST);                /* richtig ? (ca) */
    savemasc(&Y,&X);
    asgnst(&Y,&X);
    while ((c = getmtel(&Y,eno,pts)) == ',')
     { incrarity(MST);
       ++(*pts);
       asgnst(&Y,&X);
     }
  }
 else {
   if (arity(topof(MST)) > arity(topof(M1ST))) {
       if (topof(BST) != STBOTTOM)
          asgnst(&Y,BST);
       else
       if (topof(U1ST) != STBOTTOM)         /* neu (ca)       */
          asgnst(&Y,U1ST);
       else
          asgnst(&Y,AST);
       savemasc(&Y,&X);
       asgnst(&Y,&X);
       while ((c = getmtel(&Y,eno,pts)) == ',')
        { incrarity(MST);
          ++(*pts);
          asgnst(&Y,&X);
        }
     }
    else
     { if ((c = getexp(eno,pts,0)) == ',')                 /* ? */
        { incrarity(MST);
          ++(*pts);
          c = getmatrows(eno,pts);
        }
     }
  }
 killstack(&X);
 return(c);
}

/* ----------- mtchmat ----------------------------------------------- */
mtchmat(st1,st2)
STACK *st1,*st2;
{
 static STACKELEM m;
 register long n;

 if (isconstr(m = topof(st1)))
 if (class(m) != AR0(LIST)  &&  class(topof(st2)) != AR0(LIST)) {
   deletenf(st1);
   deletenf(st2);
   return(TRUE);
  }
 else
  { if ((popnf(st1) & ~EDIT_FIELD) == (popnf(st2) & ~EDIT_FIELD))
     { for (n = arity(m) ; n > 0L ; --n)
        { if (!mtchmat(st1,st2))
             return(FALSE);
        }
       return(TRUE);
     }
    else return(FALSE);
  }
 else
  { if (class(topof(st2)) != AR0(LIST))
     { deletenf(st1);
       deletenf(st2);
       return(TRUE);
     }
    else return(FALSE);
  }
}

/* -------- matchmat ------------------------------------------------- */
matchmat(st1,st2)
STACK *st1,*st2;
{
 STACK X,Y;

 asgnst(&X,st1);
 asgnst(&Y,st2);
 return(mtchmat(&X,&Y));
}

/******************************************************* End of file ******/

