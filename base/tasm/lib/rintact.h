/* $Log: rintact.h,v $
 * Revision 1.11  1998/06/17 15:27:06  rs
 * support for train primitives as interactions
 *
 * Revision 1.10  1997/02/17  16:22:02  stt
 * new prototype red_fredirect
 *
 * Revision 1.9  1996/03/01  17:48:04  stt
 * prototype test_format()
 *
 * Revision 1.8  1996/02/21  12:35:00  stt
 * include d_mess_io.h
 *
 * Revision 1.7  1996/02/20  13:26:03  cr
 * added interactions get and put
 *
 * Revision 1.6  1996/02/16  14:10:57  rs
 * no sys_errlist in distributed version (ncc problem ?!)
 * ,
 * :-)
 *
 * Revision 1.5  1996/02/15  16:57:39  stt
 * Some declarations changed.
 *
 * Revision 1.4  1996/02/13  13:25:46  stt
 * some new standard header includes.
 * some new extern declarations for rintact.c-variables.
 *
 * Revision 1.3  1996/02/08  16:05:09  stt
 * extern int _redcnt;
 *
 * Revision 1.2  1996/01/08  15:36:59  stt
 * major changes for monadic i/o, many comments
 *
 * Revision 1.1  1995/11/09  15:49:42  stt
 * Initial revision
 *
 * */

/*
 * --- rintact.h ---------------------------------- stt --- 09.11.95 ----
 *
 * Include file for rintact.c and riafunc.c.
 */


#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#if D_MESS
#include "d_mess_io.h"
#endif

/* --- structure for internal file managment ---------------------------- */

typedef struct filehandle {
  FILE               *fp;
  char               *filename;
  char               mode;
  int                index;
  char               *handle;
  struct filehandle  *next;
} FILEHANDLE;

#define L_FILEHANDLE(p,x)   ((p).x)
#define R_FILEHANDLE(p,x)   ((p).x)


/* --- external variables/functions ------------------------------------- */

/* reduma */
extern T_PTD  newdesc();            /* rheap.c */
extern int    newheap();            /* rheap.c */
extern void   rel_heap();           /* rheap.c */
extern void   res_heap();           /* rheap.c */
extern void   DescDump();           /* rdesc.c */
extern void   test_dec_refcnt();    /* rheap.c */
extern void   test_inc_refcnt();    /* rheap.c */
extern void   stack_error();        /* rstack.c */

extern PTR_DESCRIPTOR _desc;       /* rstate.c */
extern int            _redcnt;     /* rstate.c */
extern int            UseTasm;     /* setup.c */

/* system */
extern int errno;
#if !D_SLAVE
extern char *sys_errlist[];
#endif /* D_SLAVE */

/* interaction interpreter */
extern int _issubexpr;                  /* rintact.c */
extern char *_ptr_start_dir;            /* rintact.c */
extern FILEHANDLE stderr_filehandle;    /* rintact.c */
extern FILEHANDLE stdout_filehandle;    /* rintact.c */
extern FILEHANDLE stdin_filehandle;     /* rintact.c */
extern FILEHANDLE *environment;      /* rintact.c */
extern char *virtual_wd;             /* rintact.c */
extern const int INTACT_TAB_SIZE;
extern INTACT_TAB intact_tab[];

/* Interaction prototypes
 * This file is included from two other files. In one of them, these functions
 * are intern, in the other they are extern. To avoid the distinction I left
 * out the keyword 'extern' because it is assumed the function isn't intern.
 */
int red_chdir(STACKELEM path);
int red_copy(STACKELEM sname, STACKELEM tname);
int red_dir(STACKELEM path);
int red_eof(STACKELEM handle);
int red_fclose(STACKELEM handle);
int red_fgetc(STACKELEM handle);
int red_fgets(STACKELEM handle);
int red_fopen(STACKELEM name, STACKELEM mode);
int red_fprintf(STACKELEM fp, STACKELEM format, STACKELEM args);
int red_fputc(STACKELEM handle, STACKELEM c);
int red_fputs(STACKELEM handle, STACKELEM str);
int red_fredirect(STACKELEM channel, STACKELEM target);
int red_fscanf(STACKELEM fp, STACKELEM format);
int red_fseek(STACKELEM fp, STACKELEM offset, STACKELEM origin);
int red_finfo(STACKELEM handle, STACKELEM item);
int red_ftell(STACKELEM handle);
int red_getwd();
int red_mkdir(STACKELEM path);
int red_read(STACKELEM fp, STACKELEM anz);
int red_remove(STACKELEM name);
int red_rmdir(STACKELEM path);
int red_rename(STACKELEM sname, STACKELEM tname);
int red_status(STACKELEM name);
int red_ungetc(STACKELEM fp, STACKELEM c);
int red_get(STACKELEM handle);
int red_put(STACKELEM handle, STACKELEM c);

int red_set_speed(STACKELEM b, STACKELEM s);
int red_get_speed(STACKELEM b);
int red_set_direction(STACKELEM b, STACKELEM d);
int red_get_direction(STACKELEM b);
int red_set_point(STACKELEM p, STACKELEM o);
int red_get_point(STACKELEM p);
int red_set_relay(STACKELEM b, STACKELEM p);
int red_get_relay(STACKELEM b);
int red_set_signal(STACKELEM s, STACKELEM p);
int red_get_signal(STACKELEM s);
int red_read_M1(STACKELEM b);
int red_read_M2(STACKELEM b);

/*
 * Help function prototypes.
 */
PTR_DESCRIPTOR  makereturnval (int status, PTR_DESCRIPTOR wert);
PTR_DESCRIPTOR  makestrdesc (const char *str);
char            *getstrfromdesc(PTR_DESCRIPTOR desc);
char            *completename(char *name, int dir_or_file);
void            freefilehandle(FILEHANDLE *fh);
int             cmpfilehandle(FILEHANDLE *fh1, FILEHANDLE *fh2);
char            *addfilehandle(FILE *fp, char *filename, char *mode);
int             rmfilehandle(FILEHANDLE *fh);
FILEHANDLE      *getfilehandle(char *handle);
void            recoverbinds(void);
int             test_format(const char *format, int print);

/*
 * Macros for help functions.
 */
#define MAKERETURNVAL(x,d)              d = makereturnval(x,d)
#define MAKESTRDESC(a,b)                a = makestrdesc(b)
#define GETSTRING(a,b)                  a = getstrfromdesc(b)
#define GETFILEHANDLE(a,b)              a = getfilehandle(b)
#define ADDFILEHANDLE(a,b,c,d)          a = addfilehandle(b, c, d)
#define CMPFILEHANDLE(a,b)              cmpfilehandle(a,b)
#define FREEFILEHANDLE(a)               freefilehandle(a)
#define COMPLETENAME(a,b)               a = completename(a,b)
#define RMFILEHANDLE(a)                 rmfilehandle(a)

/*
 * Flags and macros for file mode.
 */
#define MODE_R          1
#define MODE_W          2
#define MODE_A          4
#define MODE_PLUS       8
#define P_MODE_READ     (MODE_R | MODE_PLUS)
#define P_MODE_WRITE    (MODE_W | MODE_A | MODE_PLUS)
#define T_MODE_R(x)     (((x) & MODE_R) == MODE_R)
#define T_MODE_W(x)     (((x) & MODE_W) == MODE_W)
#define T_MODE_A(x)     (((x) & MODE_A) == MODE_A)
#define T_MODE_PLUS(x)  (((x) & MODE_PLUS) == MODE_PLUS)
#define T_MODE_READ(x)  (((x) & P_MODE_READ) != 0)
#define T_MODE_WRITE(x) (((x) & P_MODE_WRITE) != 0)


/*
 * Return values for primitive interactions
 */
#define OK      0
#define ERROR   1
#define SYNERR  2

/*
 * Argument for COMPLETENAME, decides whether to append an '/' at the
 * file name.
 */
#define ISDIR   1
#define ISFILE  2

