/* $Log: reac.c,v $
 * Revision 1.11  1996/05/20  17:16:44  cr
 * manage list of statically imported modules
 * (to avoid repeated loads and to keep the names for tasm)
 *
 * Revision 1.10  1996/03/18  16:50:05  cr
 * static loading of precompiled graph and code
 *
 * Revision 1.9  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.8  1995/08/23  14:13:01  rs
 * some non-det. case <> letpar problems solved
 *
 * Revision 1.7  1994/03/08  18:45:43  mah
 * tilde version
 *
 * Revision 1.6  1993/12/30  10:57:25  rs
 * bugfix for Revision 1.4
 * (christmas gift for carsten r.)
 *
 * Revision 1.5  1993/12/22  09:53:36  mah
 * PI_RED_PLUS pattern match disabled in tilde preprocessor
 *
 * Revision 1.4  1993/12/21  13:57:07  rs
 * letpars transformed to ap / sub in non-distributed version
 * (christmas gift for carsten r.)
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:50  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */




/*-----------------------------------------------------------------------------
 *  eac.c  - external:  eac;
 *         Aufgerufen durch:  ea_create
 *-----------------------------------------------------------------------------
 */

/* last update  by     why

   copied       by     to

*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "dbug.h"

/*---------------------------------------------------------------------------
 * Externe Variablen:
 *---------------------------------------------------------------------------
 */
extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern STACKELEM st_name();                 /* storepre.c */
extern STACKELEM st_cond();                 /* storepre.c */
extern STACKELEM st_var();                  /* storepre.c */
extern STACKELEM st_list();                 /* storepro.c */
extern STACKELEM st_dec();                  /* storepre.c */
extern STACKELEM st_mvt();                  /* rstpre.c */
extern void      pmprepa ();

/* ach 29/10/92 */
extern void stack_error();                  /* rstack.c */
extern void trav ();                        /* aeh... */
extern STACKELEM st_expr();                 /* aeh... */
extern void ptrav_a_e();                    /* rtrav.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();               /* rstack.c */
extern void test_inc_refcnt();              /* aeh... */
#endif
/* end of ach */

#if STORE
extern PTR_DESCRIPTOR _desc;
extern char KiR_lib[];                  /* setup.c */
extern int store;                       /* rmessage.c */
extern void store_receive_result();     /* rmessage.c */
#include <fcntl.h>
#include <string.h>
#include <malloc.h>

typedef struct module_list { char *name;
                             PTR_DESCRIPTOR expr;
                             struct module_list * next;} MODULE_LIST;

MODULE_LIST *m_start=NULL,**m_end = &m_start, *m_tmp;

STACKELEM load_module() 
{
  STACKELEM           elem;
  char     fname[80],name[81],message[200];
  int         namelength=0;
  MODULE_LIST *m = m_start;

  START_MODUL("load_module");

  POPSTACK(S_e);        /* '@' */

  /* Namen von E-Stack einlesen: */
  while (T_STR1((elem = POPSTACK(S_e))) )
  {
    name[namelength++] = VALUE(elem);
    if (namelength > 80)
      post_mortem("load_module: Letterstring too long");
  }

  name[namelength++] = VALUE(elem);
  name[namelength++] = '\0';

  sprintf(fname,"%s%s",KiR_lib,name);
  for ( _desc = NULL ; m != NULL ; m = m->next )
    if (!strcmp(m->name,fname)) _desc = m->expr; /* already loaded */

  if (_desc == NULL)
  {
    if ((store = open(fname,O_RDONLY)) == -1)
    {
      sprintf(message,"load_module: can't open file %s",fname);
      post_mortem(message);
    }
    else
    {
      store_receive_result();
      close(store);
    }

    /* add to list of module names and imported expressions */
    if ((m = (MODULE_LIST*)malloc(sizeof(MODULE_LIST))) != NULL)
    {
      if ((m->name = malloc(strlen(fname)+1)) != NULL)
      {
        m->name = strcpy(m->name,fname);
        m->expr = _desc;
        m->next = NULL;
        *m_end = m;
        m_end = &(m->next);
      }
      else 
        post_mortem("load_module: malloc for m->name failed");
    }
    else
      post_mortem("load_module: malloc for m failed");
  }

  DBUG_PRINT("load_module",("fname: %s",fname));
#ifndef DBUG_OFF
  if (T_POINTER(_desc))
    DBUG_PRINT("load_module",("POINTER %x class %d type %d",_desc,R_DESC(*_desc,class),R_DESC(*_desc,type)));
  else
    DBUG_PRINT("load_module",("STACKELEM %x",_desc));
#endif

  END_MODUL("load_module");
  return((STACKELEM)_desc);
}

#endif /* STORE */

/*-----------------------------------------------------------------------------
 *  eac        --  Lagert aus einem auf dem E-Stack liegenden Ausdruck alle
 *                 Constanten aus, liefert Ergebnisausdruck auf dem E-Stack ab.
 *  globals    --  S_e,S_a     <w>
 *-----------------------------------------------------------------------------
 */
void eac()
{
  register STACKELEM  x;
#if !WITHTILDE
  register STACKELEM  y;
#endif /* !WITHTILDE */
  register unsigned short  arity;
  extern jmp_buf _interruptbuf;             /* reduct.c */
/*  PTR_DESCRIPTOR desc;        ach 29/10/92 */

  START_MODUL("eac");

  setjmp(_interruptbuf);                    /* Interruptbehandlung Preprocessing */
  if (_interrupt)                           /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("eac: Interrupt received");
  }

#if STORE
 /* Liste der importierten Module initialisieren */
 for (; m_start != NULL ;)
 {
   free(m_start->name);
   m_tmp = m_start;
   m_start = m_start->next;
   free(m_tmp);
 }
#endif /* STORE */

  PUSHSTACK(S_m,DOLLAR);                             /* Ende-Symbol */

main_e:
  x = READSTACK(S_e);

  if T_NILMAT(x) {                         /* leere Matrix ?                 */
    PPOPSTACK(S_e);
    INC_REFCNT(_nilmat);                   /* durch nilmat pointer ersetzen  */
    PUSHSTACK(S_a,(STACKELEM)_nilmat);
    goto main_m;
  }

  if T_NILVECT(x) {                        /* leerer Vektor ?                */
    PPOPSTACK(S_e);                        /* durch nilvect pointer ersetzen */
    INC_REFCNT(_nilvect);
    PUSHSTACK(S_a,(STACKELEM)_nilvect);
    goto main_m;
  }

  if T_NILTVECT(x) {                       /* leerer TVektor ?               */
    PPOPSTACK(S_e);                        /* durch niltvect pointer ersetzen*/
    INC_REFCNT(_niltvect);
    PUSHSTACK(S_a,(STACKELEM)_niltvect);
    goto main_m;
  }
  if T_NIL(x) {                            /* leere Liste ?                  */
    PPOPSTACK(S_e);                        /* durch nil pointer ersetzen     */
    INC_REFCNT(_nil);
    PUSHSTACK(S_a,(STACKELEM)_nil);
    goto main_m;
  }
  if T_NILSTRING(x) {                      /* leerer String ?                */
    PPOPSTACK(S_e);                        /* durch nilstring pointer ersetzen*/
    INC_REFCNT(_nilstring);
    PUSHSTACK(S_a,(STACKELEM)_nilstring);
    goto main_m;
  }

  if T_NILSTRUCT(x) {                      /* leerer Frame ?                  */
    PPOPSTACK(S_e);                        /* durch nilstruct pointer ersetzen*/
    INC_REFCNT(_nilstruct);
    PUSHSTACK(S_a,(STACKELEM)_nilstruct);
    goto main_m;
  }

  if (T_CON(x))
  {
    if (T_SCALAR(x))
    {
      post_mortem("eac: INT and REAL no more allowed");
    }

    if T_MATRIX(x) {
      PUSHSTACK(S_a,st_mvt(C_MATRIX));                /* Matrix  auslagern   */
      goto main_m;
    }

    if T_VECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_VECTOR));                /* Vektor  auslagern   */
      goto main_m;
    }

    if T_TVECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_TVECTOR));               /* TVektor auslagern   */
      goto main_m;
    }
    if (T_PLUS(x) || T_MINUS(x) || T_EXP(x))
    {                                       /* digit-string auslagern */
       PUSHSTACK(S_a,st_dec());
       goto main_m;
    }


#if !WITHTILDE

#if PI_RED_PLUS
    if (T_PM_CASE(x)) {
      DBUG_PRINT("EAC", ("It's a case !"));
      PPOPSTACK(S_e);
      if (T_PM_WHEN(READSTACK(S_e))) {
        if (ARITY(READSTACK(S_e)) > 3) {
          /* multiple pattern, number of patterns onto a */
          PUSHSTACK(S_a,TAG_INT(ARITY(READSTACK(S_e))-2));
          /* now replace the CAS/SWITCH-constructor by NCASE */
          PUSHSTACK(S_m,SET_ARITY(PM_NCASE,ARITY(x)));
          /* and then increment arity of NCASE */
          PUSHSTACK(S_m1,SET_ARITY(PM_NCASE,ARITY(x)+1));
        }
        else {
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
        }
        goto main_e;
      }
      else post_mortem("eac: WHEN expected on stack e");
    }
#endif /* PI_RED_PLUS     auskommentiert RS 3.12.1992 */
#if PI_RED_PLUS
        if (T_PM_WHEN (x))
        {
          if (!T_PM_CASE(READSTACK(S_m))) {
            trav(&S_e,&S_a);
            PUSHSTACK(S_e,PM_END);
            trav(&S_a,&S_e);
            PUSHSTACK(S_e,SET_ARITY(PM_SWITCH,2));
            goto main_e;
          }
#endif /* PI_RED_PLUS    auskommentiert RS 3.12.1992 */

          PPOPSTACK (S_e);

#if PI_RED_PLUS
          if (ARITY(x) > 3) {
            /* multiple patterns into a list */
            PUSHSTACK(S_e,SET_ARITY(LIST,ARITY(x)-2));
          }
#endif /* PI_RED_PLUS   auskommentiert RS 3.12.1992 */
            pmprepa ();
            goto main_e;
        }
#endif /* !WITHTILDE */

        if (T_K_COMM (x)) {
           PPOPSTACK(S_e);
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
/*           if (T_STRING(READSTACK(S_e))) {                changed by RS 17.6.1993 to: */
             if (T_STRING(READSTACK(S_e)) || T_NILSTRING(READSTACK(S_e))) {
 	     goto main_e;
 	   }
           else {
             if ((x = st_expr()) != 0)
               PUSHSTACK(S_a,x);
             else
               post_mortem("eac: heap out of space");
             goto main_m;
	   }
        }

    if (ARITY(x) > 0)
    {

#if (!D_DIST)   /* this is for carsten rathsack, special christmas 1993 gift... */
       if (((x) & 0x000000ffL) == 0x0000000aL) /* 0aL to 0fL, that's better...RS30/12/93 */
         x = SET_ARITY(AP, ARITY(x)); 
#endif /* RS 12/21/93 */

       PPOPSTACK(S_e);       /* sonstiger Konstruktor mit Stelligkeit > 0 */
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }

    PPOPSTACK(S_e);          /* sonst. Konstruktor mit Stelligkeit 0 */
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  if ((T_STR(x)) && !(T_STR_CHAR(x)))       /* digitstring oder letterstring  */
  {                                         /* auslagern?                     */
#if WITHTILDE
    if (T_LET1(x) && VALUE(x) == '%')
      PPOPSTACK(S_e);
#endif
    
#if STORE
    if (T_LET1(x) && VALUE(x) == '@')       /* referenz auf externes modul, */
      PUSHSTACK(S_a,load_module());         /* laden - cr 12.03.96          */    
    else
#endif
    PUSHSTACK(S_a,st_name());               /*     Namenspointer erzeugen     */
    goto main_m;                            /* recfunc bzw. subfunc behandelt,*/
  }                                         /* koennen also hier nicht auf-   */
                                            /* tauchen;                       */
  if (T_ATOM(x))
  {                                         /*         sonstige atoms         */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  post_mortem("eac: No match successful on main_e");

main_m:
  x = READSTACK(S_m);

#if !WITHTILDE
  if ((T_PM_REC(x)) && (T_PM_MATCH(READSTACK(S_a))) )
  {
    /* when Klausel zurueck*/
    trav(&S_a, &S_e);
    y = POPSTACK(S_a);/* der rekursive Parameter*/
    /* when Klausel wieder nach a*/
    trav(&S_e, &S_a);
    PUSHSTACK(S_a, y);/* der rekursive Parameter des caserec*/

  } 
#endif /* !WITHTILDE */

  arity = ARITY(x);
  if (arity > 1)  /* dh die Arity des Konstruktors war >= 2 */
  {
    WRITE_ARITY(S_m,arity-1);
    goto main_e;
  }
  if (arity > 0)          /* DOLLAR hat arity = 0 */
  {
    PPOPSTACK(S_m);
    x = POPSTACK(S_m1);
    PUSHSTACK(S_a,x);
    goto main_m;
  }
  /* Ein DOLLAR. Dann ist eac zu beenden */
  PPOPSTACK(S_m);         /* DOLLAR */

  PTRAV_A_E;             /* ausdruck wieder zurueck */

  END_MODUL("eac");
}
/* end of eac.c */
