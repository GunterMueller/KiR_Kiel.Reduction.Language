/* $Log: rintact.c,v $
 * Revision 1.18  1996/07/31  12:37:04  stt
 * Return-state ERROR literally changed to NE.
 *
 * Revision 1.17  1996/03/05  13:26:43  stt
 * intact_tab[] updated
 *
 * Revision 1.16  1996/03/01  17:04:07  stt
 * test_format()
 *
 * Revision 1.15  1996/02/26  14:42:17  stt
 * output shape of print_environment changed.
 *
 * Revision 1.14  1996/02/20  13:26:03  cr
 * added interactions get and put
 *
 * Revision 1.13  1996/02/16  16:16:04  rs
 * some changes concerning the integration of measurements
 * in tasm
 *
 * Revision 1.12  1996/02/15  16:57:39  stt
 * Pointer to red_dummy replaced by NULL.
 * Using stack access macros from rstack.h.
 * binds_on_stack is now global.
 *
 * Revision 1.11  1996/02/13  13:25:46  stt
 * some standard header includes moved to rintact.h
 *
 * Revision 1.10  1996/02/08  17:14:31  stt
 * simple reduction counting
 * 0-ary interactions are no longer put into descriptors and
 * therefore there must be a special case in intact_inter().
 *
 * Revision 1.9  1996/01/18  16:45:53  stt
 * new function print_environment
 * tilde() from editor copied into this file
 *
 * Revision 1.8  1996/01/08  15:36:59  stt
 * major changes for monadic i/o, many comments
 *
 * Revision 1.7  1995/11/27  09:28:48  stt
 * declarations and use of "virtual working directory" in complete_name
 *
 * Revision 1.6  1995/11/17  14:09:48  stt
 * 'environment' initiated with stdin, stdout and stderr
 *
 * Revision 1.5  1995/11/15  16:43:56  stt
 * evaluation of interactions only if it is no subexpression
 *
 * Revision 1.4  1995/11/09  15:46:29  stt
 * interactions moved to riafunc.c
 * defines moved to rintact.h
 *
 * Revision 1.3  1995/11/08  15:17:29  stt
 * Interaction interpreter and functions implemented.
 *
 * Revision 1.2  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.1  1995/09/08  10:17:36  stt
 * Initial revision
 *
 * */

/*
 * --- rintact.c ---------------------------------- stt --- 08.09.95 ----
 *
 * This file contains some global variables like intact_tab, environment
 * etc., some help functions for descriptor and kir file handle handling
 * and, most important, the interaction interpreter routine intact_inter().
 * The routines of the interactions are defined in the file riafunc.c.
 *
 * How does the interaction interpreter work:
 * It is called after the 'normal' interpreter (inter()) has finished. This
 * happens in the function reduce(). The result of inter() lies on the stack W.
 * The interaction interpreter examines the result whether it is some kind of
 * interaction. If not, the reduction continues with the post processing. If
 * yes, the interaction is performed and the new result is checked again, until
 * it is no longer an interaction. If the result is executable, i.e. it is some
 * kind of kir function, than inter() is called again, else the reduction
 * continues with the post processing.
 */


#if D_SLAVE
#include "rncstack.h"
#include "rncmessage.h"
#else
#include "rstackty.h"
#endif

#include "rstdinc.h"
#include "rheapty.h"
#include "rstack.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinter.h"
#include "dbug.h"
#include "rintact.h"


/* --- global variables --------------------------------------------------- */

/*
 * The interaction table combines the name of an interaction with the function
 * which performed it. The position of an interaction in the table have to
 * match the index of the interaction, which is defined in rstelem.h.
 * Interactions with the function pointer NULL are not yet implemented or
 * are special, like exit, bind, unit.
 *
 * The type of the table (INTACT_TAB) is defined in rstdinc.h:
 *   typedef struct { 
 *     FCTPTR fkt;
 *     char   *name;
 *   } INTACT_TAB;
 */
INTACT_TAB intact_tab[] =         /* FUNC_INDEX */
{{red_fopen,    "ia_fopen"      } /*  0 */
,{red_fclose,   "ia_fclose"     } /*  1 */
,{red_fgetc,    "ia_fgetc"      } /*  2 */
,{red_fputc,    "ia_fputc"      } /*  3 */
,{red_ungetc,   "ia_ungetc"     } /*  4 */
,{red_fgets,    "ia_fgets"      } /*  5 */
,{red_read,     "ia_read"       } /*  6 */
,{red_fputs,    "ia_fputs"      } /*  7 */
,{NULL,         "ia_fprintf"    } /*  8 */
,{NULL,         "ia_fscanf"     } /*  9 */
,{red_fseek,    "ia_fseek"      } /*  a */
,{red_ftell,    "ia_ftell"      } /*  b */
,{NULL,         "ia_exit"       } /*  c */
,{red_eof,      "ia_eof"        } /*  d */
,{NULL,         "ia_bind"       } /*  e */
,{NULL,         "ia_unit"       } /*  f */
,{red_finfo,    "ia_finfo"      } /* 10 */
,{red_status,   "ia_status"     } /* 11 */
,{red_remove,   "ia_remove"     } /* 12 */
,{red_rename,   "ia_rename"     } /* 13 */
,{red_copy,     "ia_copy"       } /* 14 */
,{red_mkdir,    "ia_mkdir"      } /* 15 */
,{red_rmdir,    "ia_rmdir"      } /* 16 */
,{red_chdir,    "ia_chdir"      } /* 17 */
,{red_getwd,    "ia_getwd"      } /* 18 */
,{red_dir,      "ia_dir"        } /* 19 */
,{red_get,      "ia_get"        } /* 1a */
,{red_put,      "ia_put"        } /* 1b */
};

const int INTACT_TAB_SIZE = (sizeof(intact_tab)/sizeof(intact_tab[0]));


/*
 * Some variables which are set in reduce(). They contain values which were 
 * hand over from the editor to the reduction unit.
 */
int _issubexpr;   /* The expression, which should be reduced, is a subexpr. */
char *_ptr_start_dir;   /* The directory in which the reduma is started in. */

/*
 * Number of binds on the bind stack. Will be initialised in reduct().
 */
int ia_binds_on_stack;

/*
 * Pointer to the string which contains the "virtual" working directory.
 * For some notes to the "virtual" working directory see description in
 * riafunc.c, red_getwd.
 * The string is initalised in intact_inter().
 */
char *virtual_wd = NULL;

/*
 * Predefined kir file handles for standard I/O.
 */
FILEHANDLE stderr_filehandle = { stderr, "stderr", 4, 0,
                                 "STDERR", NULL };
FILEHANDLE stdout_filehandle = { stdout, "stdout", 4, 0,
                                 "STDOUT", &stderr_filehandle };
FILEHANDLE stdin_filehandle = { stdin, "stdin", 1, 0,
                                "STDIN", &stdout_filehandle };

/*
 * Global variable, which is used as the entry point of the kir file handle
 * list. All (dynamical generated) file handles in a kir program are inserted
 * in this list. When they were closed, they were deleted out of this list.
 * Normally, there are only a few entries in the same time, so there is no need
 * for a hash table or something like that.
 * The list is initalised with the standard I/O file handles.
 */
FILEHANDLE *environment = &stdin_filehandle;



/* --- help function declarations ----------------------------------------- */


/* ------------------------------------------------------------------------
 * Print the environment list to the given output channel.
 */
void print_environment(FILE *out)
{
  FILEHANDLE *fh;
  char modus[3];
  char no_mode[] = "Filehandle has no valid mode.";

  fprintf(out, "\nList of open files:\n\n");
  fh = environment;
  while (fh != NULL)
  {
    if (T_MODE_R(R_FILEHANDLE(*fh, mode)))
      modus[0] = 'r';
    else
    if (T_MODE_W(R_FILEHANDLE(*fh, mode)))
      modus[0] = 'w';
    else
    if (T_MODE_A(R_FILEHANDLE(*fh, mode)))
      modus[0] = 'a';
    else
      modus[0] = 'X';

    if (T_MODE_PLUS(R_FILEHANDLE(*fh, mode)))
    {
      modus[1] = '+';
      modus[2] = '\0';
    }
    else
      modus[1] = '\0';

    fprintf(out, "Name  : %s\n", R_FILEHANDLE(*fh,filename));
    fprintf(out, "Mode  : %s\n", (modus[0] == 'X') ? no_mode : modus);
    fprintf(out, "Handle: '%s`\n\n", R_FILEHANDLE(*fh,handle));

    fh = R_FILEHANDLE(*fh,next);
  }
}


/* ------------------------------------------------------------------------
 * Checks if the argument is a valid format string for printf (print = 1)
 * or scanf (print = 0).
 *
 * Syntax for printf format strings: '[-+ ]DIGITS[.]DIGITS{diouxXfeEgG}`
 * 
 * Syntax for scanf format strings: 'DIGITS{diouxXfeEgG}`
 *
 * (DIGITS is a (maybe empty) string off digits. ('0'..'9'))
 *
 * Returns  0  if it is not
 *          1  if it is valid for integers (d,i,o,u,x,X)
 *          2  if it is valid for reals (f,g,G,e,E)
 *
 * used by: red_sprintf, red_sscanf (rvalfunc.c)
 *
 */
int test_format(const char *format, int print)
{
  int ret = 0, i = 0, state = 0;
  int len;

  DBUG_ENTER ("test_format");
  START_MODUL("test_format");

  len = strlen(format);

  while (i < len)
  {
    switch (state)
    {
      case 0: /* Start with '%'? */
              if (format[i++] == '%')
                state = (print == 1) ? 1 : 2;
              else
                i = len + 1;
              break;
      case 1: /* Test flags in print pattern. */
              if ((format[i] == '+') ||
                  (format[i] == '-') ||
                  (format[i] == ' ') )
                i++;
              else
                state++;
              break;
      case 2: /* fieldwidth */
              if (format[i] >= '0' &&
                  format[i] <= '9' )
                i++;
              else
                state = (print == 1) ? 3 : 5;
              break;
      case 3: /* precision point */
              if (format[i] == '.')
              {
                state = 4;
                i++;
              }
              else
                state = 5;
              break;
      case 4: /* precision digits */
              if (format[i] >= '0' &&
                  format[i] <= '9' )
                i++;
              else
                state++;
              break;
      case 5: /* type */
              switch (format[i++])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                          ret = 1;
                          break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                          ret = 2;
              }
              state++;
              break;
      case 6: /* format string too long */
              ret = 0;
              i = len + 1;
    }
  }

  END_MODUL("test_format");
  DBUG_RETURN(ret);
}


#if UNIX
/* ------------------------------------------------------------------------
 * Do tilde expansion in file names.
 * This is a copy of the function in cfiles.c from the editor.
 */
static int tilde(char *file, char *exp)
{
#if !D_SLAVE
  *exp = '\0';
  if (file) {
    if (*file == '~') {
      char user[128];
      struct passwd *pw = NULL;
      int i = 0;

      user[0] = '\0';
      file++;
      while (*file != '/' && i < sizeof(user))
        user[i++] = *file++;
      user[i] = '\0';
      if (i == 0) {
        char *login = (char *) getlogin();

        if (login == NULL && (pw = getpwuid(getuid())) == NULL)
          return (0);
        if (login != NULL)
          strcpy(user, login);
      }
      if (pw == NULL && (pw = getpwnam(user)) == NULL)
        return (0);
      strcpy(exp, pw->pw_dir);
    }
    strcat(exp, file);
    return (1);
  }
#endif /* D_SLAVE */
  return (0);
}
#endif

/* ------------------------------------------------------------------------
 * Returns a LIST descriptor representation of the argument string.
 */
PTR_DESCRIPTOR makestrdesc (const char *str)
{
  PTR_DESCRIPTOR desc;
  PTR_HEAPELEM   ptdv;
  int dim,i;

  dim = strlen(str);
  MAKEDESC(desc, 1, C_LIST, TY_STRING);
  L_LIST(*desc,dim) = dim;
  L_LIST(*desc,ptdd) = NULL;
  GET_HEAP(dim,A_LIST(*desc,ptdv));
  ptdv = R_LIST(*desc,ptdv);
  RES_HEAP;
  for (i=0; i<dim; i++) ptdv[i] = TAG_STR_CHAR(str[i]);
  REL_HEAP;
  return(desc);
}


/* ------------------------------------------------------------------------
 * Returns a string representation of the argument, if this is a 
 * LIST-STRING descriptor.
 */
char *getstrfromdesc(PTR_DESCRIPTOR desc)
{
  short dim, i;
  char *str;
  PTR_HEAPELEM  ptdv;
  short error = 0;
  
  if (T_POINTER(desc) &&
      (R_DESC(*desc,class) == C_LIST) &&
      (R_DESC(*desc,type)  == TY_STRING))
  {
    dim  = R_LIST(*desc,dim);
    ptdv = R_LIST(*desc,ptdv);
    
    if ((str = (char *) malloc(dim+1)) == NULL)
      post_mortem("rintact: memory allocation for str not possible.");

    for (i=0; i<dim; i++)
    {
      if (T_STR_CHAR(ptdv[i]))
        str[i] = (char) VALUE(ptdv[i]);
      else
        error = 1;
    }
    
    str[dim] = NULL;
  }
  else
    return(NULL);

  if (error == 1)
  {
    free(str);
    return(NULL);
  }
  else
    return(str);

  /* If the return value is NULL, it was not decidable wheter an error
   * occurred or the argument has dimension 0.
   */
}


/* ------------------------------------------------------------------------
 * Returns a constructor descriptor OK{wert} or NE{wert}.
 */
PTR_DESCRIPTOR makereturnval (int status, PTR_DESCRIPTOR wert)
{
  PTR_DESCRIPTOR list;
  PTR_DESCRIPTOR desc;
  PTR_HEAPELEM   ptdv;

  if (status == OK)
    MAKESTRDESC(list, "_OK");
  else
    MAKESTRDESC(list, "_NE");
  
  MAKEDESC(desc, 1, C_LIST, TY_UNDEF);
  L_LIST(*desc,dim)     = 2;
  L_LIST(*desc,special) = 9;    /* display list in constructor form */
  L_LIST(*desc,ptdd)    = NULL;
  GET_HEAP(2,A_LIST(*desc,ptdv));
  ptdv = R_LIST(*desc,ptdv);
  RES_HEAP;
  ptdv[0] = (int) list;
  ptdv[1] = (int) wert;
  REL_HEAP;
  return(desc);
}


/* ------------------------------------------------------------------------
 * Returns the complete path and name of the argument. If name is not  an
 * absolute pathname, name will be appended to the virtual working directory
 * (virtual_wd). If name starts with a '~', the '~' will be extended to the
 * users home directory. After that, the full name will be simplified by
 * deleting such sequences as 'dir_name/../' or '/./'.
 */
char *completename(char *name, int dir_or_file)
{
  char *fullname, *tmp;
  int pos_t, pos_s;                     /* position target, source */
  
  if ((fullname = (char *) malloc(MAXNAMLEN + 1)) == NULL)
    post_mortem("rintact: memory allocation for fullname not possible.");

  if ((name[0] == '/') ||
      (name[0] == '~'))
  {
    if (strlen(name) > MAXNAMLEN)
      post_mortem("rintact: filename too long");
    strcpy(fullname, name);
  }
  else
  {
    if ((strlen(name) + strlen(virtual_wd)) > MAXNAMLEN)
      post_mortem("rintact: filename too long");
    strcpy(fullname,virtual_wd);
    strcat(fullname,name);       /* concat virtual_wd and name */
  }

  if (dir_or_file == ISDIR)
  {
    pos_t = strlen(fullname);
    if (fullname[pos_t-1] != '/')
    {
      fullname[pos_t] = '/';        /* append '/' at the end of the name */
      fullname[pos_t+1] = '\0';
    }
  }
  
#if UNIX
  if (fullname[0] == '~')
  {
    char ext[MAXNAMLEN+1];
    extern tilde();

    if (tilde(fullname, ext) == 1)   /* substitute '~' with users home dir. */
      strcpy(fullname, ext);
  }
#endif

  /* simplification of fullname */
  
  while ((tmp = strstr(fullname, "//")) != NULL)
  {
    pos_t = tmp - fullname;
    while (fullname[pos_t] != '\0')
    {
      fullname[pos_t] = fullname[pos_t + 1];
      pos_t++;
    }
  }

  while ((tmp = strstr(fullname, "/./")) != NULL)
  {
    pos_t = tmp - fullname;
    while (fullname[pos_t] != '\0')
    {
      fullname[pos_t] = fullname[pos_t+2];
      pos_t++;
    }
  }

  while ((tmp = strstr(fullname, "/../")) != NULL)
  {
    pos_t = tmp - fullname;
    if (pos_t == 0)
      pos_s = 3;
    else
    {
      pos_s = pos_t + 4;
      while ((pos_t > 0) && (fullname[pos_t-1] != '/'))
        pos_t--;
    }

    while (fullname[pos_t] != '\0')
      fullname[pos_t++] = fullname[pos_s++];
  }

  free(name);
  return(fullname);
}


/* ------------------------------------------------------------------------
 * Free a FILEHANDLE structure.
 */
void freefilehandle(FILEHANDLE *fh)
{
  free(R_FILEHANDLE(*fh,fp));
  free(R_FILEHANDLE(*fh,filename));
  free(R_FILEHANDLE(*fh,handle));
  free(fh);
}


/* ------------------------------------------------------------------------
 * Returns 1, if the filename field and the mode field of the two argument
 * file handles are equal, else 0.
 */
int cmpfilehandle(FILEHANDLE *fh1, FILEHANDLE *fh2)
{
  if ((R_FILEHANDLE(*fh1,mode) == R_FILEHANDLE(*fh2,mode)) &&
      (strcmp(R_FILEHANDLE(*fh1,filename), R_FILEHANDLE(*fh2,filename)) == 0))
    return(0);
  else
    return(1);
}


/* ------------------------------------------------------------------------
 * Adds a new file handle to the file handle list and returns the kir file
 * handle string. 
 * mode had to be in {r w a r+ w+ a+}
 */
char *addfilehandle(FILE *fp, char *filename, char *mode)
{
  FILEHANDLE *fh, **fhptr;
  char       *fname, *handlestr, mode_code, indexstr[8];
  int        index, gap_found;

  if ((fh = (FILEHANDLE *) malloc(sizeof(FILEHANDLE))) == NULL)
    post_mortem("rintact: memory allocation for fh not possible.");
  if ((fname = (char *) malloc(strlen(filename)+1)) == NULL)
    post_mortem("rintact: memory allocation for fname not possible.");

  strcpy(fname, filename);
  
  L_FILEHANDLE(*fh,fp)       = fp;
  L_FILEHANDLE(*fh,filename) = fname;

  mode_code = 0;
  switch (mode[0])
  {
    case 'r': mode_code |= MODE_R;  break;
    case 'w': mode_code |= MODE_W;  break;
    case 'a': mode_code |= MODE_A;  break;
  }
  if (mode[1] == '+')
    mode_code |= MODE_PLUS;
    
  L_FILEHANDLE(*fh,mode)     = mode_code;
  
  /*
   * The file handle list will be scanned for entries with the same filename
   * and the same mode as the arguments. There may be several handles which
   * do fulfill this, separated by their index. The new file handle should
   * obtain the smallest free index (there may be gaps in the sequence of
   * indices after closing a file handle) and inserted in the list between
   * the two file handles (if existing) with the next smaller and the next
   * bigger index.  fhptr always points to the next entry which is to be
   * examined. The loop will be executed until fhptr points to the end of the
   * list or a file handle with the same filename and mode and an index
   * greater than the actual index-counter, in which case there is a index
   * gap.  The new file handle will be inserted right before the element
   * fhptr points to.
   */
  index = 1;
  gap_found = 0;
  fhptr = &environment;
  while (*fhptr != NULL && gap_found == 0)
  {
    if (CMPFILEHANDLE(fh, *fhptr) == 0) 
    {
      if (R_FILEHANDLE(**fhptr, index) > index)
        gap_found = 1;
      else
        index++;
    }
    
    if (gap_found == 0)
      fhptr = &(R_FILEHANDLE(**fhptr,next));
  }
  L_FILEHANDLE(*fh,next) = *fhptr;
  *fhptr = fh;

  L_FILEHANDLE(*fh,index)     = index;
  sprintf(indexstr, "%d", index);
  
  /* Length of handlestr = len(fname)+4(asterix)+6(Index)+2(Mode)+1(EOS) */
  if ((handlestr = (char *) malloc(strlen(fname)+13)) == NULL)
    post_mortem("rintact: memory allocation for handlestr not possible.");
  
  strcpy(handlestr,"*");
  strcat(handlestr,fname);
  strcat(handlestr,"*");
  strcat(handlestr,mode);
  strcat(handlestr,"*");
  strcat(handlestr,indexstr);
  strcat(handlestr,"*");

  L_FILEHANDLE(*fh,handle)   = handlestr;
  return(handlestr);
}


/* ------------------------------------------------------------------------ 
 * Removes a file handle out of the list.
 */
int rmfilehandle(FILEHANDLE *fh)
{
  FILEHANDLE *lastfh, *aktfh;

  if (environment == NULL)
    return(1);

  if (environment == fh)
  {
    environment = R_FILEHANDLE(*environment,next);
    FREEFILEHANDLE(fh);
    return(0);
  }

 
  lastfh = environment;
  aktfh  = R_FILEHANDLE(*environment,next);

  while (aktfh != NULL)
  {
    if (aktfh == fh)
    {
      L_FILEHANDLE(*lastfh,next) = R_FILEHANDLE(*aktfh,next);
      FREEFILEHANDLE(fh);
      return(0);
    }
    
    lastfh = aktfh;
    aktfh  = R_FILEHANDLE(*aktfh,next);
  }

  return(1); 
}


/* ------------------------------------------------------------------------ 
 * Returns a pointer to the file handle which is associated to the
 * file handle string argument.
 */
FILEHANDLE *getfilehandle(char *handle)
{
  FILEHANDLE *fh;
  
  fh = environment;
  while (fh != NULL)
  {
    if (strcmp(R_FILEHANDLE(*fh,handle),handle) == 0)
      break;
    fh  = R_FILEHANDLE(*fh,next);
  }
  /* If the file handle is not in the list NULL is returned. */
  
  return(fh);
}


/* ------------------------------------------------------------------------ 
 * Replace top of stack W with itself, embedded into the remaining
 * bind constructors on the bind stack.
 * Example:
 *  BIND_STACK: bind(x1,y1) bind(x2,y2)
 *  TOP_W() := bind(bind(TOP_W(),y2),y1)
 */
void recoverbinds(void)
{
  STACKELEM      *args;
  short          not_bind = 0;
  PTR_DESCRIPTOR ptdesc2, ptdesc1;

  START_MODUL("recoverbinds");
  
  ptdesc1 = (PTR_DESCRIPTOR) POP_W();
  while (ia_binds_on_stack > 0)
  {
    ptdesc2 = ptdesc1;
    ptdesc1 = (PTR_DESCRIPTOR) POP_R();
    ia_binds_on_stack--;
    
    if (T_POINTER(ptdesc1) &&
        (R_DESC(*ptdesc1,class) == C_INTACT) &&
        (R_DESC(*ptdesc1,type) == TY_INTACT) )
    {
      args   = (STACKELEM *) R_INTACT(*ptdesc1,args);
      if (T_IA_BIND((STACKELEM) args[0]))
      {
        if (R_DESC(*ptdesc1,ref_count) > 1)
        {
          /* Decrement ref_count, make new bind with ptdesc2 and arg[2] */
          DEC_REFCNT(ptdesc1);
          MAKEDESC(ptdesc1, 1, C_INTACT, TY_INTACT);
          L_INTACT(*ptdesc1,dim)  = 3;
          GET_HEAP(4, A_INTACT(*ptdesc1,args));
          RES_HEAP;
          R_INTACT(*ptdesc1,args)[0] = IA_BIND;
          R_INTACT(*ptdesc1,args)[1] = (STACKELEM) ptdesc2;
          /* insert the y of the original bind(x,y) */
          R_INTACT(*ptdesc1,args)[2] = args[2];
          REL_HEAP;
          if (T_POINTER(args[2]))
            INC_REFCNT(((PTR_DESCRIPTOR) args[2]));
        }
        else
        {
          /* replace 1. bind-arg with ptdesc2 */
          if (T_POINTER(args[1]))
           DEC_REFCNT(((PTR_DESCRIPTOR) args[1]));
          RES_HEAP;
          args[1] = (STACKELEM) ptdesc2;
          REL_HEAP;
        }
      }
      else
        not_bind = 1;
    }
    else
      not_bind = 1;

    if (not_bind)
      post_mortem("rintact.c: Item on bind stack is not a bind().");
  }
  
  END_MODUL("recoverbinds");
  
  WRITE_W(ptdesc1);
}


/***************************************************************************
 *  intact_inter -- The interaction interpreter.
 ***************************************************************************/

/* If there is an interaction lying on the a-stack (as the result of inter()),
 * the interaction interpreter will perform it until the result is no longer an
 * interaction. After that, inter() will be called again.
 * The continuation based version (14.09.95) now is completely replaced by the
 * monadic style version (20.12.95).
 *
 * At this time, there are three special interactions: bind, unit and exit.
 * bind combines two interactions (or expressions) and dictates a computation
 *  order on interaction. First the interactions in the first argument must be
 *  reduced, and if their result is an unit, the second argument will be
 *  applied to the unit argument.
 * unit enclose the result of a normal interaction (typical) or any other
 *  expression. The only way to accessing the argument of a unit is to use the
 *  unit as the first argument of a bind.
 * exit terminates the program execution (the whole reduma) immediately.
 * The other interactions (if implemented) are performed by calling their
 * function. Their result will be enclosed by a unit construct.
 *
 * How does the monadic style version work:
 * As mentioned above, to execute several interactions you have to combine
 * them with bind. The reduction of a bind goes like this:
 * If the 1.argument of the bind is an interaction (other than unit), push
 * the bind constructor on the bind stack and reduce the argument. When it is
 * reduced to a unit, then pop the bind from the stack and apply the 2.argument
 * to the unit. So, the stack is used to implement a recursive descent into the
 * left subtree of a bind.
 * If, in some state of reduction, the (sub)expression is not reducible,
 * neither by  inter() nor by intact_inter(), and there are bind's on the bind
 * stack left, you have to recover the hole expression, i.e. take the bind's
 * from the stack and re-combinate them.
 * For further details see the comments below and the master thesis of Stephan
 * Timm, 1996.
 */
int intact_inter(void)
{
  /* Static code segments, used for applying a continuation to a result.
   * Example:  bind(unit(x), cont) -> ap cont to x
   */
  static INSTR beta_code[] = { I_BETA, 0, I_EXT, I_END};
  static INSTR apply_code[] = { I_APPLY, 1, I_EXT, I_END};
  
  PTR_DESCRIPTOR desc, ptdesc;           /* pointers to descriptors          */
  STACKELEM      *args, intact;          /* pointers to heap elements        */
  FCTPTR         fctptr;                 /* pointer to interaction functions */
  int dim;                          /* dimension of descriptors              */
  int status = 0;                   /* return value of interaction functions */
  int ret = 0;                      /* address of asm code segment to be     */
                                    /* interpreted by inter().               */
                                     
  
  START_MODUL("intact_inter");

  if (virtual_wd == NULL)                 /* If not yet initiated do it now. */
  {
    dim = strlen(_ptr_start_dir) + 2;
    if ((virtual_wd = (char *) malloc(dim)) == NULL)
      post_mortem("rintact: Cannot allocate memory for _ptr_start_dir.");
    strcpy(virtual_wd, _ptr_start_dir);
    dim = strlen(virtual_wd);
    if (virtual_wd[dim-1] != '/')   /* virtual_wd always has to end with a / */
    {
      virtual_wd[dim] = '/';
      virtual_wd[dim+1] = '\0';
    }
  }
  
  desc = (PTR_DESCRIPTOR) TOP_W();           /* Get result of inter() */
  
  /*
   * Repeat the loop as long as desc is an interaction (new values are assigned
   * to desc in the loop). 0-ary interactions are simple stack elements while
   * n-ary, n>0, interactions are stored as interaction descriptors.
   * After leaving the loop, there are two possibilities:
   * 1. ret == 0
   *   The result isn't reducible any more. Do not call inter() again.
   * 2. ret != 0
   *   The result is an application which is to be reduced by inter(). ret is
   *   the address of a asm code sequence to be executed. 
   */
  while ( ( ( T_POINTER(desc)   /* Is it an interaction? */
              &&
              (R_DESC(*desc,class) == C_INTACT)
              &&
              (R_DESC(*desc,type) == TY_INTACT)
            )
            ||
            T_IA_GETWD((STACKELEM) desc)
          )
          &&
          !_issubexpr     /* and not a subexpression selected in the editor? */
          &&
          (_redcnt > 0)                   /* Are there Reduction steps left? */
        )
  {                              /* (Reduce interactions on top level only.) */
    ret = 0;
    
    if T_IA_GETWD((STACKELEM) desc)
    {
      status = (int) red_getwd();
      if (status == SYNERR)
      {
        /*
         * The arguments are syntactical wrong. No reduction will be made,
         * interrupt loop.
         */
        WRITE_W(desc);
        desc = (PTR_DESCRIPTOR) NULL;
      }
      else
      {
        /*
         * Interaction was performed, display result.
         */
        MAKERETURNVAL(status, _desc);

        /* Make unit descriptor */
        MAKEDESC(ptdesc, 1, C_INTACT, TY_INTACT);
        L_INTACT(*ptdesc,dim)  = 2;
        GET_HEAP(3, A_INTACT(*ptdesc,args));
        RES_HEAP;
        R_INTACT(*ptdesc,args)[0] = IA_UNIT;
        R_INTACT(*ptdesc,args)[1] = (T_HEAPELEM) _desc;
        REL_HEAP;
        _desc = NULL;

        WRITE_W(ptdesc);
        _redcnt -= 1;
        desc = ptdesc;      /* desc is unit -> repeat loop */
      }
    }
    else
    {
      args   = (STACKELEM *) R_INTACT(*desc,args);
      intact = (STACKELEM) args[0];                   /* type of interaction */
  
      if (FUNC_INDEX(intact) >= INTACT_TAB_SIZE)
        post_mortem("rintact: interaction has wrong index");
        
      /*
       * The interactions bind, unit and exit are special and therefore be
       * treated in special cases.
       */
      if (T_IA_BIND(intact))
      {
        /*
         * The first argument of a bind will be remembered in ptdesc.
         * By decreasing the reference count of the bind (later) the refcnt of
         * of ptdesc get wrong. Therefore the bind being pushed on the stack
         * gets a NULL-Pointer as its 1.argument. (hack)
         */
        ptdesc = (PTR_DESCRIPTOR) args[1];      /* ptdesc = x from bind(x,y) */
        args[1] = (STACKELEM) NULL;
        PUSH_R(desc);
        ia_binds_on_stack++;
        desc = ptdesc;
        WRITE_W(desc);
      }
      else if (T_IA_UNIT(intact))
      {
        if (ia_binds_on_stack > 0)
        {
          /*
           * At this point, I have made the following assumption:
           * The first time intact_inter() is called the bind stack (S_m) is
           * empty.
           * If inter() is (re)called, the stack is returned in the same state
           * as it was before the call (even if there are bind's on it).
           * -> If inter() uses the stack, it recovers the original state before
           *    exiting.
           * If I leave intact_inter for the last time, the stack is empty
           * again.
           * So, if the stack is not empty, the item on the stack had to be a
           * bind constructor which I have put on the stack earlier.
           * In this case I have to pop and 'execute' it, i.e. the 2.argument
           * of the bind (the continuation) has to be applied to the argument
           * of the unit ( R_INTACT(*desc,args)[1] ). This corresponds to one
           * ascending step in the recursive reduction.
           */
           
          short not_bind = 0;        /* There had to be a bind on the stack. */
  
          ptdesc = (PTR_DESCRIPTOR) POP_R();
          ia_binds_on_stack--;
          if (T_POINTER(ptdesc) &&
              (R_DESC(*ptdesc,class) == C_INTACT) &&
              (R_DESC(*ptdesc,type) == TY_INTACT) )
          {
            args   = (STACKELEM *) R_INTACT(*ptdesc,args);
            if (T_IA_BIND((STACKELEM) args[0]))
            {
              PTR_DESCRIPTOR cont = (PTR_DESCRIPTOR) args[2];
              WRITE_W(R_INTACT(*desc,args)[1]);     /* save unit arg. */
              
              if (T_POINTER(cont) &&
                  (R_DESC(*cont,class) == C_FUNC) &&
                  (R_DESC(*cont,type ) == TY_COMB) &&
                  (R_COMB(*cont,nargs) == 1) )
              {
                beta_code[1] = (INSTR) R_COMB(*cont,ptc);
                ret = (int) beta_code;
              }
              else
              {
                PUSH_W(cont);
                ret = (int) apply_code;
              }
              /*
               * Doing some reference counting. (hard job)
               * The bind and the unit constructors are no longer needed, but
               * their arguments are. Therefor increment the refcnt of the
               * unit argument and the 2.argument of the bind (cont) and
               * decrement the refcnt of bind and unit.
               */
              if (T_POINTER(cont))
                INC_REFCNT(cont);
              DEC_REFCNT(ptdesc);                        /* bind constructor */
              if (T_POINTER((PTR_DESCRIPTOR) R_INTACT(*desc,args)[1]))
                INC_REFCNT((PTR_DESCRIPTOR) R_INTACT(*desc,args)[1]);
              DEC_REFCNT(desc);                          /* unit constructor */
              _redcnt -= 1;                    /* decrease reduction counter */
            }
            else
              not_bind = 1;
          }
          else
            not_bind = 1;
  
          if (not_bind)
            post_mortem("rintact.c: Item on bind stack is not a bind().");
        }
        else
        {
          WRITE_W(desc); /* unit(...) is the result of the reduction.*/
        }
        desc = (PTR_DESCRIPTOR) NULL;   /* Interrupt the loop in every case. */
      }
      else if (T_IA_EXIT(intact))
      {
        /*
         * The distributed version of kir may get into difficulties if this
         * exit was executed in a son process.
         */
        if (T_INT(args[1]))
          exit((long int) args[1] / 2);
        else
          exit(1);
      }
      else if ((fctptr = intact_tab[FUNC_INDEX(intact)].fkt) != NULL)
      {
        /*
         * desc is a primitive interaction. Call interaction function an
         * return unit( Status{Value} ).
         */
         
        /*
         * dim is equal to arity of interaction + 1 (the interaction)
         */
        dim    = R_INTACT(*desc,dim);
  
        switch (dim - 1)
        {
          case 1:  status = (int) ((*fctptr)(args[1]));  break;
          case 2:  status = (int) ((*fctptr)(args[1],args[2]));  break;
          case 3:  status = (int) ((*fctptr)(args[1],args[2],args[3]));  break;
          default: post_mortem("rintact.c: interaction has wrong dimension");
        }
        
        if (status == SYNERR)
        {
          /*
           * The arguments are syntactical wrong. No reduction will be made,
           * interrupt loop.
           */
          WRITE_W(desc);
          desc = (PTR_DESCRIPTOR) NULL;
        }
        else 
        {
          /*
           * Reduce interaction, display result.
           */
          DEC_REFCNT(desc);
          MAKERETURNVAL(status, _desc);
      
          /* Make unit descriptor */
          MAKEDESC(ptdesc, 1, C_INTACT, TY_INTACT);
          L_INTACT(*ptdesc,dim)  = 2;
          GET_HEAP(3, A_INTACT(*ptdesc,args));
          RES_HEAP;
          R_INTACT(*ptdesc,args)[0] = IA_UNIT;
          R_INTACT(*ptdesc,args)[1] = (T_HEAPELEM) _desc;
          REL_HEAP;
          _desc = NULL;
      
          WRITE_W(ptdesc);
          _redcnt -= 1;
          desc = ptdesc;                     /* desc is unit -> repeat loop */
        }
      }
      else
      {
        /*
         * desc is a primitive, but not yet implemented interaction. No further
         * reduction can be made, the loop will be interrupted.
         */
        WRITE_W(desc);
        desc = (PTR_DESCRIPTOR) NULL;
      }
    }
  } /* while (desc is interaction) */

  if ( ret == 0 && ia_binds_on_stack > 0 )
  {
    /*
     * The reduction is finished, any bind's left on the bind stack must be
     * recovered to get back the hole expression.
     */
    recoverbinds();
    _desc = NULL;
  }
  
  END_MODUL("intact_inter");
  return(ret);
}

