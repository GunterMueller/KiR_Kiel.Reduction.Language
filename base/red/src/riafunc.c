/*
 * $Log: riafunc.c,v $
 * Revision 1.26  2001/07/02 14:53:11  base
 * compiler warnings eliminated
 *
 * Revision 1.25  1998/07/03 11:28:11  rs
 * minor changes
 *
 * Revision 1.24  1998/06/17 15:27:06  rs
 * support for train primitives as interactions
 *
 * Revision 1.23  1998/02/27  15:00:48  rs
 * further bugfixes on speculative evaluation (preliminary, beware of debug messages !)
 *
 * Revision 1.22  1997/02/18  09:35:47  stt
 * function fredirect completed
 *
 * Revision 1.21  1997/02/17  16:22:02  stt
 * fredirect-dummy inserted
 *
 * Revision 1.20  1996/07/31  12:37:04  stt
 * eof returns OK{true/false} now.
 *
 * Revision 1.19  1996/06/24  17:14:04  base
 * lred/tred bug fix, 3rd
 *
 * Revision 1.18  1996/06/24  17:02:04  base
 * lred bug fix, 2nd
 *
 * Revision 1.17  1996/06/24  16:49:53  cr
 * bug fix: STORE and lred again :-(
 *
 * Revision 1.16  1996/06/24  13:26:51  cr
 * red_put passes expr for store_send_graph in _desc
 * preparations for TASM-variants of put/get/store/receive
 *
 * Revision 1.15  1996/03/06  15:07:00  cr
 * store_receive_result places result directly in _desc (red_get)
 *
 * Revision 1.14  1996/03/05  13:26:43  stt
 * copy, mkdir, rmdir, rename and remove implemented.
 *
 * Revision 1.13  1996/02/21  18:17:22  cr
 * prototype implementation of interactions for arbitrary expressions
 * uses modified send/receive-routines from rmessage.c
 * compile with -DSTORE=1 to get the prototype (interactions get/put)
 * or with -DSTORE=0 to get the original files.
 * involved files so far: rheap.c riafunc.c rstelem.h rmessage.c
 * rmessage.c has to be compiled once with -DSTORE=1 to get the
 * modified send/receive-routines, and perhaps a second time with
 * -DSTORE=0 to get its original functionality for the distributed
 * versions.
 *
 * Revision 1.12  1996/02/20  13:26:03  cr
 * added interactions get and put
 *
 * Revision 1.11  1996/02/16  14:13:56  rs
 * no const variable used for array dimension (ncc problem)
 *
 * Revision 1.10  1996/02/13  13:25:46  stt
 * some standard header includes moved to rintact.h
 *
 * Revision 1.9  1996/02/07  18:44:24  stt
 * bugfix red_read
 * keywords for finfo capitaliesed, keyword 'index' removed
 *
 * Revision 1.8  1996/01/08  15:36:59  stt
 * major changes for monadic i/o, many comments
 *
 * Revision 1.7  1995/11/29  10:21:23  stt
 * fflush() in output-routines added
 * Appending of a Newline in fputs() removed
 *
 * Revision 1.6  1995/11/27  09:28:48  stt
 * interactions uses "virtual working directory"
 *
 * Revision 1.5  1995/11/15  10:45:06  stt
 * error in red_status corrected
 *
 * Revision 1.4  1995/11/14  11:11:30  stt
 * fputs corrected
 *
 * Revision 1.3  1995/11/10  15:39:15  stt
 * red_fgets for long strings corrected
 *
 * Revision 1.2  1995/11/09  15:47:44  stt
 * code restructured
 *
 * Revision 1.1  1995/11/08 16:41:40  stt
 * Initial revision
 */

/* --- riafunc.c ---------------------------------- stt --- 08.11.95 ---- 
 *
 * All interactions (except bind, unit and exit) are defined in this file.
 *
 * Structure of an interaction function:
 * 1. Check all arguments for type and, if possible, value.
 *    Return SYNERR if check fails.
 * 2. Perform interaction.
 * 3. Put result into _desc and return OK, if the interaction was successfull,
 *    ERROR otherwise.
 */


#include <errno.h>
#if (!D_DIST)
/* #include <grp.h> */
#endif
#include <limits.h>
#include <time.h>
#include <sys/stat.h>

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#include "rncmessage.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinter.h"
#include "dbug.h"
#include "rintact.h"

#if TRAIN
#include "/home/railway/inc/RailControl.h"  
#endif /* TRAIN */

#if !TASM_STORE

/* -----------------------------------------------------------------------
 * red_chdir:  Changes working directory.
 */
int red_chdir(STACKELEM path)
{
  int  returnval = SYNERR;
  char *newpath = NULL;
  DIR *dir;
  
  START_MODUL("red_chdir");
  
  GETSTRING(newpath,(PTR_DESCRIPTOR) path);

  if (newpath != NULL)
  {
    COMPLETENAME(newpath, ISDIR);

    if ((dir = opendir(newpath)) == NULL &&
        errno != EACCES)
    {
      MAKESTRDESC(_desc,"chdir: Argument is not a directory.");
      returnval = ERROR;
      free(newpath);
    }
    else
    {
      closedir(dir);
      free(virtual_wd);
      virtual_wd = newpath;
      MAKESTRDESC(_desc,"chdir");
      returnval = OK;
    }
  }
  
  END_MODUL("red_chdir");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_copy: Copies a file.
 */
int red_copy(STACKELEM sname, STACKELEM tname)
{
  int returnval = SYNERR;
  char *source = NULL, *target = NULL;
  FILE *fs, *ft;
  int nitems = 1024, readed;
  char buf[1024];
  struct stat statbuf;

  START_MODUL("red_copy");

  GETSTRING(source, (PTR_DESCRIPTOR) sname);
  GETSTRING(target, (PTR_DESCRIPTOR) tname);

  if (source != NULL)
  {
    if (target != NULL)
    {
      COMPLETENAME(source, ISFILE);
      COMPLETENAME(target, ISFILE);

      if ((fs = fopen(source,"r")) == NULL)
      {
        returnval = ERROR;
        MAKESTRDESC(_desc,"copy: Cannot open source file.");
      }
      else if ((ft = fopen(target,"w")) == NULL)
      {
        fclose(fs);
        returnval = ERROR;
        MAKESTRDESC(_desc,"copy: Cannot open target file.");
      }
      else
      {
        if ((stat(source, &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)))
        {
          returnval = ERROR;
          MAKESTRDESC(_desc,"copy: Source file is a directory.");
        }
        else
        {
          while ((readed = fread(buf, 1, nitems, fs)) > 0)
            fwrite(buf, 1, readed, ft);
          returnval = OK;
          MAKESTRDESC(_desc,"copy");
        }
        fclose(fs);
        fclose(ft);
      }
      free(target);
    }
    free(source);
  }
  
  END_MODUL("red_copy");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_dir:  Returns a list of all entries in the given directory.
 */
int red_dir(STACKELEM path)
{
  DIR *dir;
  struct dirent *entry;
  char *pathname;
  PTR_DESCRIPTOR *names;
  PTR_HEAPELEM   ptdv;
  int num_names, max_names;
  const int STEPNAMES = 50;
  int returnval = SYNERR;

  START_MODUL("red_dir");
  
  GETSTRING(pathname, (PTR_DESCRIPTOR) path);

  if (pathname != NULL)
  {
    COMPLETENAME(pathname, ISDIR);
    if ((dir = opendir(pathname)) == NULL)
    {
      MAKESTRDESC(_desc, "dir: Argument is not a directory or cannot be "
                         "accessed.");
      returnval = ERROR;
    }
    else
    {
      max_names = STEPNAMES;
      num_names = 0;
      if ((names = (PTR_DESCRIPTOR *)
                   malloc(max_names*sizeof(PTR_DESCRIPTOR))) == NULL)
        post_mortem("rintact: memory allocation for names not possible.");
    
      while ((entry = readdir(dir)) != NULL)
      {
        if (num_names >= max_names)
        {
          max_names += STEPNAMES;
          if ((names = (PTR_DESCRIPTOR *)
                    realloc(names, max_names*sizeof(PTR_DESCRIPTOR))) == NULL)
            post_mortem("rintact: memory reallocation for names not possible.");
        }
    
        MAKESTRDESC(names[num_names++], entry->d_name);
      }
      
      closedir(dir);
    
      MAKEDESC(_desc, 1, C_LIST, TY_UNDEF);
      L_LIST(*_desc,dim) = num_names;
      L_LIST(*_desc,ptdd) = NULL;
      GET_HEAP(num_names, A_LIST(*_desc,ptdv));
      ptdv = R_LIST(*_desc,ptdv);
      RES_HEAP;
      memcpy((void *) ptdv, (void *) names, num_names*sizeof(PTR_DESCRIPTOR));
      REL_HEAP;

      free(names);
      returnval = OK;
    }
    free(pathname);
  }
  
  END_MODUL("red_dir");
  return(returnval);
}


/* ----------------------------------------------------------------------- 
 * red_eof:  Checks eof condition of file handle. Therefor it does a getc
 * and an ungetc on the file.
 */
int red_eof(STACKELEM handle)
{
  char c, *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;
  
  START_MODUL("red_eof");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh,handlestr);
  
    if (fh == NULL)
    {
      MAKESTRDESC(_desc,"eof: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if ((c = fgetc(R_FILEHANDLE(*fh,fp))) != EOF)
      {
        ungetc(c, R_FILEHANDLE(*fh,fp));
        _desc = (PTR_DESCRIPTOR) SA_FALSE;
      }
      else
        _desc = (PTR_DESCRIPTOR) SA_TRUE;
      returnval = OK;
    }
    free(handlestr);
  }
  
  END_MODUL("red_eof");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fclose:  Close a file and removes its file handle from the
 *              file handle list.
 */
int red_fclose(STACKELEM handle)
{
  char *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;

  START_MODUL("red_fclose");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);
  
    if (fh == NULL)
    {
      MAKESTRDESC(_desc,"fclose:Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if (fclose(R_FILEHANDLE(*fh,fp)) == EOF) 
      {
        MAKESTRDESC(_desc,"fclose: fclose not successful.");
        returnval = ERROR;
      }
      else
      {
        if (RMFILEHANDLE(fh) != 0)
        {
          MAKESTRDESC(_desc,"fclose: Cannot remove file handle.");
          returnval = ERROR;
        }
        else
        {
          MAKESTRDESC(_desc,"fclose");
          returnval = OK;
        }
      }
    }
    free(handlestr);
  }
  
  END_MODUL("red_fclose");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fgetc:  Reads one character from the given file.
 *             A character is a string with length 1.
 */
int red_fgetc(STACKELEM handle)
{
  char *handlestr;
  FILEHANDLE *fh;
  char cstr[] = { '\0', '\0'};
  int returnval = SYNERR;

  START_MODUL("red_fgetc");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);

    if (fh == NULL)
    {
      MAKESTRDESC(_desc,"fgetc: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if ( ! T_MODE_READ(R_FILEHANDLE(*fh,mode)))
      {
        MAKESTRDESC(_desc, "fgetc: File is not readable.");
        returnval = ERROR;
      }
      else
      {
        if ((cstr[0] = (char) fgetc(R_FILEHANDLE(*fh,fp))) == EOF)
        {
          MAKESTRDESC(_desc,"fgetc: End of file.");
          returnval = ERROR;
        }
        else
        {
          MAKESTRDESC(_desc,cstr);  
          returnval = 0;
        }
      }
    }
    free(handlestr);
  }

  END_MODUL("red_fgetc");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fgets:  Reads one line from the given file.
 *             Reads till next '\n', '\n' is not returned.
 */
int red_fgets(STACKELEM handle)
{
  int LENGTH = 250, number;
  char str[250], *bigstr, *newstr, *handlestr;
  FILEHANDLE *fh;
  int ende, returnval = SYNERR;
  
  START_MODUL("red_fgets");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);
  
    if (fh == NULL)
    {
      MAKESTRDESC(_desc,"fgets: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if ( ! T_MODE_READ(R_FILEHANDLE(*fh,mode)))
      {
        MAKESTRDESC(_desc, "fgets: File is not readable.");
        returnval = ERROR;
      }
      else
      {
        if (fgets(str, LENGTH, R_FILEHANDLE(*fh,fp)) == NULL)
        {
          MAKESTRDESC(_desc,"fgets: End of file.");
          returnval = ERROR;
        }
        else
        {
          if (strlen(str) < LENGTH -1)
            bigstr = str;
          else
          {
            number = 1;
            if ((bigstr = (char *) malloc(number * LENGTH)) == NULL)
             post_mortem("rintact: memory allocation for bigstr not possible.");
            
            strcpy(bigstr,str);
            ende = 0;

            while ((ende == 0) &&
                   (fgets(str, LENGTH, R_FILEHANDLE(*fh,fp)) != NULL))
            {
              number++;
              if ((newstr = (char *) realloc(bigstr, number*LENGTH)) == NULL)
                post_mortem("rintact: memory reallocation for bigstr "
                            "not possible.");
              bigstr = newstr;
              strcat(bigstr,str);
              if (strlen(str) < LENGTH -1)
                ende = 1;
            }
          }
            
          number = strlen(bigstr);
          if (bigstr[number - 1] == '\n')
            bigstr[number - 1] = '\0';
          MAKESTRDESC(_desc,bigstr);  
          returnval = OK;
          free(bigstr);
        }
      }
    }
    free(handlestr);
  }
  
  END_MODUL("red_fgets");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_finfo:  Returns one element of the handle, depending on item.
 *             'FULLNAME`   file name with path
 *             'NAME`       file name without path
 *             'PATH`       path only
 *             'MODE`       file mode
 */
int red_finfo(STACKELEM handle, STACKELEM item)
{
  char *handlestr, *itemstr;
  FILEHANDLE *fh;
  int returnval = SYNERR;
    
  START_MODUL("red_finfo");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  GETSTRING(itemstr, (PTR_DESCRIPTOR) item);
  
  if ((handlestr != NULL) && (itemstr != NULL))
  {
    GETFILEHANDLE(fh, handlestr);
    
    if (fh == NULL)
    {
      MAKESTRDESC(_desc, "finfo: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else if (strcmp(itemstr, "FULLNAME") == 0)
    {
      MAKESTRDESC(_desc, R_FILEHANDLE(*fh, filename));
      returnval = OK;
    }
    else if (strcmp(itemstr, "NAME") == 0)
    {
      char *fullname = strdup(R_FILEHANDLE(*fh, filename));
      int i = strlen(fullname);
      while ((--i >= 0) && (fullname[i] != '/'))
        ;
      MAKESTRDESC(_desc, fullname+i+1);
      free(fullname);
      returnval = OK;
    }
    else if (strcmp(itemstr, "PATH") == 0)
    {
      char *fullname = strdup(R_FILEHANDLE(*fh, filename));
      int i = strlen(fullname);
      while ((--i >= 0) && (fullname[i] != '/'))
        ;
      fullname[i+1] = '\0';
      MAKESTRDESC(_desc, fullname);
      free(fullname);
      returnval = OK;
    }
    else if (strcmp(itemstr, "MODE") == 0)
    {
      char modus[3];
      
      modus[0] = '\0';
      if (T_MODE_R(R_FILEHANDLE(*fh, mode)))
        modus[0] = 'r';
      else
      if (T_MODE_W(R_FILEHANDLE(*fh, mode)))
        modus[0] = 'w';
      else
      if (T_MODE_A(R_FILEHANDLE(*fh, mode)))
        modus[0] = 'a';

      if (modus[0] == '\0')
      {
        MAKESTRDESC(_desc, "finfo: Filehandle has not a valid mode.");
        returnval = ERROR;
      }
      else
      {
        if (T_MODE_PLUS(R_FILEHANDLE(*fh, mode)))
        {
          modus[1] = '+';
          modus[2] = '\0';
        }
        else
          modus[1] = '\0';
          
        MAKESTRDESC(_desc,modus);
        returnval = OK;
      }
    }
    /* else returnval = SYNERR; */
  }
  free(handlestr);
  free(itemstr);

  END_MODUL("red_finfo");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fopen:  Opens a file. If successful, a FILEHANDLE will be returned
 *             and appended to the environment-List.
 */
int red_fopen(STACKELEM name, STACKELEM mode)
{
  char *namestr, *modestr, *handle;
  FILE *fp;
  int len, returnval = SYNERR, wrong_mode = 0;
  
  START_MODUL("red_fopen");
  
  GETSTRING(namestr,(PTR_DESCRIPTOR) name);
  GETSTRING(modestr,(PTR_DESCRIPTOR) mode);

  if ((namestr != NULL) && (modestr != NULL))
  {
    len = strlen(modestr);
    if ( (len == 1) ||  ( (len == 2) && (modestr[1] == '+') ) )
    {
      switch (modestr[0])
      {
        case 'r':
        case 'w':
        case 'a': break;
        default : wrong_mode = 1;
      }
    }
    else
      wrong_mode = 1;
  
    if (wrong_mode != 1)
    {
      COMPLETENAME(namestr, ISFILE);
    
      if ((fp = fopen(namestr, modestr)) == NULL)
      {
        MAKESTRDESC(_desc,"fopen: Cannot open file.");
        returnval = ERROR;
      }
      else
      {
        ADDFILEHANDLE(handle,fp,namestr,modestr);
        
        if (handle == NULL)
        {
          MAKESTRDESC(_desc,"fopen: Cannot create handle.");
          returnval = ERROR;
        }
        else
        {
          MAKESTRDESC(_desc,handle);
          returnval = OK;
        }
      }
    }
    free(namestr);
    free(modestr);
  }
  
  END_MODUL("red_fopen");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fputc:  Puts the character into the output file.
 */
int red_fputc(STACKELEM handle, STACKELEM c)
{
  char *handlestr, *outchar;
  FILEHANDLE *fh;
  int returnval = SYNERR;
 
  START_MODUL("red_fputc");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  GETSTRING(outchar, (PTR_DESCRIPTOR) c);
  
  if ((handlestr != NULL) && (outchar != NULL))
  {
    if (strlen(outchar) == 1)
    {
      GETFILEHANDLE(fh, handlestr);
      
      if (fh == NULL)
      {
        MAKESTRDESC(_desc, "fputc: 1. argument is not a valid file handle.");
        returnval = ERROR;
      }
      else
      {
        if ( ! T_MODE_WRITE(R_FILEHANDLE(*fh,mode)))
        {
          MAKESTRDESC(_desc, "fputc: File is not writeable.");
          returnval = ERROR;
        }
        else
        {
          if (fputc(outchar[0], R_FILEHANDLE(*fh,fp)) == EOF)
          {
            MAKESTRDESC(_desc,"fputc: fputc not successful.");
            returnval = ERROR;
          }
          else
          {
            fflush(R_FILEHANDLE(*fh,fp));
            MAKESTRDESC(_desc, "fputc");
            returnval = OK;
          }
        }
      }
    }
    free(handlestr);
    free(outchar);
  }
  
  END_MODUL("red_fputc");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fputs:  Puts the string into the output file.
 */
int red_fputs(STACKELEM handle, STACKELEM str)
{
  char *handlestr, *outstring;
  FILEHANDLE *fh;
  int returnval = SYNERR;
  
  START_MODUL("red_fputs");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  GETSTRING(outstring, (PTR_DESCRIPTOR) str);
  
  if (handlestr != NULL)
  {
    if (outstring != NULL)
    {
      GETFILEHANDLE(fh, handlestr);
      
      if (fh == NULL)
      {
        MAKESTRDESC(_desc, "fputs: 1. argument is not a valid file handle.");
        returnval = ERROR;
      }
      else
      {
        if ( ! T_MODE_WRITE(R_FILEHANDLE(*fh,mode)))
        {
          MAKESTRDESC(_desc, "fputs: File is not writeable.");
          returnval = ERROR;
        }
        else
        {
          if (fputs(outstring, R_FILEHANDLE(*fh,fp)) == EOF)
          {
            MAKESTRDESC(_desc,"fputs: fputs not successful."); 
            returnval = ERROR;
          }
          else
          {
            fflush(R_FILEHANDLE(*fh,fp));
            MAKESTRDESC(_desc, "fputs");
            returnval = OK;
          }
        }
      }
      free(outstring);
    }
    free(handlestr);
  }

  END_MODUL("red_fputs");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_redirect: redirects the standard input or output channel to
 *               another terminal or file.
 */
int red_fredirect(STACKELEM channel, STACKELEM target)
{
  char *channelstr, *targetstr;
  FILEHANDLE *fh;
  FILE *fp;
  int returnval = SYNERR;
  int chan = 0;                 /* 2:STDOUT, 1:STDIN, 0:no filehandle */
  
  START_MODUL("red_fredirect");
  
  fh = NULL;    /* avoid 'uninitialized using' warning */
  
  GETSTRING(channelstr, (PTR_DESCRIPTOR) channel);
  GETSTRING(targetstr, (PTR_DESCRIPTOR) target);
  
  if (channelstr != NULL)
  {
    if (targetstr != NULL)
    {
      if (strcmp(channelstr,"STDIN") == 0)
        if ((GETFILEHANDLE(fh, "STDIN")) != NULL)
          chan = 1;
        else
        {
          MAKESTRDESC(_desc,"fredirect: Cannot find filehandle 'STDIN'.");
          returnval = ERROR;
        }
      else if (strcmp(channelstr,"STDOUT") == 0)
      {
        if ((GETFILEHANDLE(fh, "STDOUT")) != NULL)
          chan = 2;
        else
        {
          MAKESTRDESC(_desc,"fredirect: Cannot find filehandle 'STDOUT'.");
          returnval = ERROR;
        }
      }
      /* else SYNERR */

      if (chan == 1)
      {
        if (strcmp(targetstr,"RESET_IO") == 0)
        {
          L_FILEHANDLE(*fh,fp) = stdin;
          MAKESTRDESC(_desc, "fredirect");
          returnval = OK;
        }
        else
        {
          if ((fp = fopen(targetstr,"r")) != NULL)
          {
            L_FILEHANDLE(*fh,fp) = fp;
            MAKESTRDESC(_desc, "fredirect");
            returnval = OK;
          }
          else
          {
            MAKESTRDESC(_desc,"fredirect: Cannot open target.");
            returnval = ERROR;
          }
        }
      }
      else if (chan == 2)
      {
        if (strcmp(targetstr,"RESET_IO") == 0)
        {
          L_FILEHANDLE(*fh,fp) = stdout;
          MAKESTRDESC(_desc, "fredirect");
          returnval = OK;
        }
        else
        {
          if ((fp = fopen(targetstr,"a")) != NULL)
          {
            L_FILEHANDLE(*fh,fp) = fp;
            MAKESTRDESC(_desc, "fredirect");
            returnval = OK;
          }
          else
          {
            MAKESTRDESC(_desc,"fredirect: Cannot open target.");
            returnval = ERROR;
          }
        }
      }
      free(targetstr);
    }
    free(channelstr);
  }

  END_MODUL("red_fredirect");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_fseek:  Set the read/write pointer of the file handle to new position.
 */
int red_fseek(STACKELEM handle, STACKELEM offset, STACKELEM origin)
{
  char *handlestr, *origstr;
  FILEHANDLE *fh;
  int orig = 0, error = 0;
  long int offs = 0;
  int returnval = SYNERR;
  
  START_MODUL("red_fseek");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  
  if (handlestr != NULL)
  {
    if (T_INT(offset))
    {
      offs = (long int) offset / 2;
      GETSTRING(origstr, (PTR_DESCRIPTOR) origin);

      if (origstr != NULL)
      {
        if (strcmp(origstr, "SEEK_SET") == 0)
          orig = SEEK_SET;
        else
        if (strcmp(origstr, "SEEK_CUR") == 0)
          orig = SEEK_CUR;
        else
        if (strcmp(origstr, "SEEK_END") == 0)
          orig = SEEK_END;
        else
          error = 1;
          
        if (error == 0)
        {
          GETFILEHANDLE(fh, handlestr);
          
          if (fh == NULL)
          {
            MAKESTRDESC(_desc,"fseek: 1.argument is not a valid file handle.");
            returnval = ERROR;
          }
          else
          {
            if (fseek(R_FILEHANDLE(*fh,fp), offs, orig) != 0)
            {
              MAKESTRDESC(_desc, "fseek: fseek not successful.");
              returnval = ERROR;
            }
            else
            {
              MAKESTRDESC(_desc,"fseek");
              returnval = OK;
            }
          }
        }
        free(origstr);
      }
    }
    free(handlestr);
  }

  END_MODUL("red_fseek");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_ftell:  Returns the position of the file pointer in the named file.
 */
int red_ftell(STACKELEM handle)
{
  int pos;
  char *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;
  
  START_MODUL("red_ftell");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  
  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);
    
    if (fh == NULL)
    {
      MAKESTRDESC(_desc, "ftell: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      pos = ftell(R_FILEHANDLE(*fh,fp));
      if (pos < 0)
      {
        MAKESTRDESC(_desc,"ftell: read error.");
        returnval = ERROR;
      }
      else
      {
        _desc = (PTR_DESCRIPTOR) TAG_INT(pos);
        returnval = OK;
      }
    }
    free(handlestr);
  }
  
  END_MODUL("red_ftell");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_getwd:  Returns the actual (virtual) working directory.
 */
int red_getwd()
{
  START_MODUL("red_getwd");
  
  MAKESTRDESC(_desc,virtual_wd);

  END_MODUL("red_getwd");
  return(OK);
}


/* -----------------------------------------------------------------------
 * red_mkdir:  Creates a new directory.
 */
int red_mkdir(STACKELEM path)
{
  int returnval = SYNERR;
  char *newdir = NULL;

  START_MODUL("red_mkdir");
  
  GETSTRING(newdir, (PTR_DESCRIPTOR) path);

  if (newdir != NULL)
  {
    COMPLETENAME(newdir, ISDIR);

    if (mkdir(newdir, 0755) != 0)
    {
      returnval = ERROR;
      switch (errno)
      {
        case EACCES:
                MAKESTRDESC(_desc, "mkdir: Access denied for one component "
                                   "of path.");
                break;
        case ENOENT:
        case ENOTDIR:
                MAKESTRDESC(_desc, "mkdir: One component of path does not "
                                   "exist or is not a directory.");
                break;
        case EEXIST:
                MAKESTRDESC(_desc, "mkdir: File / Directory already exists.");
                break;
        default:
                MAKESTRDESC(_desc, "mkdir: Unspecified error occurred.");
      }
    }
    else
    {
      returnval = OK;
      MAKESTRDESC(_desc,"mkdir");
    }
    free(newdir);
  }

  END_MODUL("red_mkdir");
  return(returnval);

}


/* -----------------------------------------------------------------------
 * red_read:  Read anz bytes from the file.
 */
int red_read(STACKELEM handle, STACKELEM anz)
{
  char *handlestr, *readed;
  FILEHANDLE *fh;
  int anzahl, gelesen;
  int returnval = SYNERR;
  
  START_MODUL("red_read");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    if (T_INT(anz))
    {
      GETFILEHANDLE(fh, handlestr);
 
      if (fh == NULL)
      {
        MAKESTRDESC(_desc,"read: 1. argument is not a valid file handle.");
        returnval = ERROR;
      }
      else
      {
        anzahl = GET_INT(anz);
        if ( ! T_MODE_READ(R_FILEHANDLE(*fh,mode)))
        {
          MAKESTRDESC(_desc, "read: File is not readable.");
          returnval = ERROR;
        }
        else
        {
          if ((readed = (char *) malloc(anzahl + 1)) == NULL)
            post_mortem("rintact: memory allocation for readed not possible.");
          
          clearerr(R_FILEHANDLE(*fh,fp));
          gelesen = fread(readed, 1, anzahl, R_FILEHANDLE(*fh,fp));
          if (gelesen == 0)
          {
            returnval = ERROR;
            if (feof(R_FILEHANDLE(*fh,fp)))
              MAKESTRDESC(_desc,"read: End of file.");
            else
              MAKESTRDESC(_desc,"read: Read error.");
          }
          else
          {
            readed[gelesen] = '\0';
            MAKESTRDESC(_desc,readed);
            returnval = OK;
          }
        }
      }
    }
    free(handlestr);
  }
  
  END_MODUL("red_read");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_remove:  Removes the file.
 */
int red_remove(STACKELEM name)
{
  int returnval = SYNERR;
  char *namestr;
  struct stat statbuf;

  START_MODUL("red_remove");
  
  GETSTRING(namestr, (PTR_DESCRIPTOR) name);

  if (namestr != NULL)
  {
    COMPLETENAME(namestr, ISFILE);

    if ((stat(namestr, &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)))
    {
      returnval = ERROR;
      MAKESTRDESC(_desc,"remove: Argument is a directory.");
    }
    else if (unlink(namestr) != 0)
    {
      returnval = ERROR;
      switch (errno)
      {
        case EACCES:
                MAKESTRDESC(_desc, "remove: Access denied for one component "
                                   "of path.");
                break;
        case EINVAL:
                MAKESTRDESC(_desc, "remove: Cannot delete current directory."); 
                break;
        case ENOENT:
        case ENOTDIR:
                MAKESTRDESC(_desc, "remove: One component of path does not "
                                   "exist or is not a directory.");
                break;
        default:
                MAKESTRDESC(_desc, "remove: Unspecified error occurred.");
      }
    }
    else
    {
      returnval = OK;
      MAKESTRDESC(_desc,"remove");
    }
    free(namestr);
  }

  END_MODUL("red_remove");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_rename:  Renames the file.
 */
int red_rename(STACKELEM sname, STACKELEM tname)
{
  int returnval = SYNERR;
  char *source = NULL, *target = NULL;

  START_MODUL("red_rename");

  GETSTRING(source, (PTR_DESCRIPTOR) sname);
  GETSTRING(target, (PTR_DESCRIPTOR) tname);

  if (source != NULL)
  {
    if (target != NULL)
    {
      COMPLETENAME(source, ISFILE);
      COMPLETENAME(target, ISFILE);

      if (rename(source, target) != 0)
      {
        returnval = ERROR;
        switch (errno)
        {
          case EACCES:
                  MAKESTRDESC(_desc, "rename: Access denied for one argument.");
                  break;
          case EINVAL:
                  MAKESTRDESC(_desc, "rename: Arg1 is parent directory of "
                                     "Arg2.");
                  break;
          case ENOENT:
          case ENOTDIR:
                  MAKESTRDESC(_desc, "rename: One component of path does not "
                                     "exist or is not a directory.");
                  break;
          case EISDIR:
                  MAKESTRDESC(_desc, "rename: Arg2 is a directory.");
                  break;
          case ENOTEMPTY:
                  MAKESTRDESC(_desc, "rename: Target directory is not empty.") ;
                  break;
          default:
                  MAKESTRDESC(_desc, "rename: Unspecified error occurred.");
        }
      }
      else
      {
        returnval = OK;
        MAKESTRDESC(_desc,"rename");
      }
      free(target);
    }
    free(source);
  }

  END_MODUL("red_rename");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_rmdir:  Removes the directory.
 */
int red_rmdir(STACKELEM path)
{
  int returnval = SYNERR;
  char *olddir = NULL;
  
  START_MODUL("red_rmdir");
  
  GETSTRING(olddir, (PTR_DESCRIPTOR) path);

  if (olddir != NULL)
  {
    COMPLETENAME(olddir, ISDIR);

    if (rmdir(olddir) != 0)
    {
      returnval = ERROR;
      switch (errno)
      {
        case EACCES:
        case EINVAL:
                MAKESTRDESC(_desc, "rmdir: Access denied for one component "
                                   "of path.");
                break;
        case ENOENT:
        case ENOTDIR:
                MAKESTRDESC(_desc, "rmdir: One component of path not "
                                   "exist or is not a directory.");
                break;
        case ENOTEMPTY:
                MAKESTRDESC(_desc, "rmdir: Directory is not empty.");
                break;
        default:
                MAKESTRDESC(_desc, "rmdir: Unspecified error occurred.");
      }
    }
    else
    {
      returnval = OK;
      MAKESTRDESC(_desc,"rmdir");
    }
    free(olddir);
  }

  END_MODUL("red_rmdir");
  return(returnval);
}


/* ----------------------------------------------------------------------- 
 * red_status:  Returns a list with permission string (like ls -l), user id,
 *              group id, size and date of the last modification.
 */
int red_status(STACKELEM name)
{
  struct stat buf;
  char *filename, helpstr[40];
  struct group *group;
  struct passwd *user;
  PTR_HEAPELEM ptdv;
  PTR_DESCRIPTOR per_desc, uid_desc, gid_desc, siz_desc, dat_desc;
  char permission[] = "----------";
  int returnval = SYNERR;
  
  START_MODUL("red_status");
  
  GETSTRING(filename, (PTR_DESCRIPTOR) name);

  if (filename != NULL)
  {
    COMPLETENAME(filename, ISFILE);
    
    if (stat(filename, &buf) != 0)
    {
      switch (errno)
      {
        case EACCES:  MAKESTRDESC(_desc,"status: Search permission denied.");
                      break;
        case ENOENT:  MAKESTRDESC(_desc,"status: File does not exist.");
                      break;
        case ENOTDIR: MAKESTRDESC(_desc, "status: A component of the path "
                                         "prefix is not a directory.");
                      break;
        default:      MAKESTRDESC(_desc,"status: Unspecified error occurred.");
      }
      returnval = ERROR;
    }
    else
    {
      switch (buf.st_mode & S_IFMT)
      {
        case S_IFREG:  permission[0] = '-'; break;
        case S_IFCHR:  permission[0] = 'c'; break;
        case S_IFBLK:  permission[0] = 'b'; break;
        case S_IFDIR:  permission[0] = 'd'; break;
        case S_IFLNK:  permission[0] = 'l'; break;
        case S_IFSOCK: permission[0] = 's'; break;
        case S_IFIFO:  permission[0] = 'p'; break;
        default:       permission[0] = '?';
      }
    
      if (buf.st_mode & S_IRUSR) permission[1] = 'r';
      if (buf.st_mode & S_IWUSR) permission[2] = 'w';
      if (buf.st_mode & S_IXUSR) permission[3] = 'x';
      if (buf.st_mode & S_IRGRP) permission[4] = 'r';
      if (buf.st_mode & S_IWGRP) permission[5] = 'w';
      if (buf.st_mode & S_IXGRP) permission[6] = 'x';
      if (buf.st_mode & S_IROTH) permission[7] = 'r';
      if (buf.st_mode & S_IWOTH) permission[8] = 'w';
      if (buf.st_mode & S_IXOTH) permission[9] = 'x';
    
      if (buf.st_mode & S_ISUID)
      {
        if (permission[3] == '-') permission[3] = 'S';
        else                      permission[3] = 's';
      }
      
      if (buf.st_mode & S_ISGID)
      {
        if (permission[6] == '-') permission[6] = 'S';
        else                      permission[6] = 's';
      }
      
      if (buf.st_mode & S_ISVTX)
      {
        if (permission[9] == '-') permission[9] = 'T';
        else                      permission[9] = 't';
      }
      
      MAKESTRDESC(per_desc, permission);
      
      if ((user = getpwuid(buf.st_uid)) == NULL)
        post_mortem("rintact: Cannot get user.");
      MAKESTRDESC(uid_desc, user->pw_name);
      
#if (!D_DIST)
  /*    if ((group = getgrgid(buf.st_gid)) == NULL)
        post_mortem("rintact: Cannot get goup.");
      MAKESTRDESC(gid_desc, group->gr_name); */
#endif
      
      sprintf(helpstr,"%d", (int) buf.st_size);
      MAKESTRDESC(siz_desc, helpstr);
    
      strftime(helpstr, 40, "%H:%M:%S %d.%m.%Y", localtime(&buf.st_mtime));
      MAKESTRDESC(dat_desc, helpstr);
    
      MAKEDESC(_desc, 1, C_LIST, TY_UNDEF);
      L_LIST(*_desc,dim) = 5;
      L_LIST(*_desc,ptdd) = NULL;
      GET_HEAP(5, A_LIST(*_desc,ptdv));
      ptdv = R_LIST(*_desc,ptdv);
      RES_HEAP;
      ptdv[0] = (int) per_desc;
      ptdv[1] = (int) uid_desc;
      ptdv[2] = (int) gid_desc;
      ptdv[3] = (int) siz_desc;
      ptdv[4] = (int) dat_desc;
      REL_HEAP;
      returnval = OK;
    }
    free(filename);
  }

  END_MODUL("red_status");
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_ungetc:  Puts the character c back into the named input stream.
 *              If successful, the next read on this stream will return c.
 */
int red_ungetc(STACKELEM handle, STACKELEM c)
{
  char *cstr, *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;

  START_MODUL("red_ungetc");
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  GETSTRING(cstr, (PTR_DESCRIPTOR) c);

  if ((handlestr != NULL) && (cstr != NULL))
  {
    if (strlen(cstr) == 1)
    {
      GETFILEHANDLE(fh, handlestr);
    
      if (fh == NULL)
      {
        MAKESTRDESC(_desc,"ungetc: Argument is not a valid file handle.");
        returnval = ERROR;
      }
      else
      {
        if (ungetc(cstr[0], R_FILEHANDLE(*fh,fp)) == EOF) 
        {
          MAKESTRDESC(_desc,"ungetc: ungetc not successful.");
          returnval = ERROR;
        }
        else
        {
          MAKESTRDESC(_desc,"ungetc");  
          returnval = OK;
        }
      }
    }
    free(handlestr);
  }
  
  END_MODUL("red_ungetc");
  return(returnval);
}

#endif /* !TASM_STORE */

#if STORE

#if TASM_STORE
#define PREFIX(f)               tasm_ ## f
#define STRING_PREFIX(f)        "tasm_" f
#else /* TASM_STORE */
#define PREFIX(f)               f
#define STRING_PREFIX(f)        f
#endif /* TASM_STORE */

extern int store; /* file descriptor of the store */
extern void PREFIX(store_receive_result)();
extern void PREFIX(store_send_graph)();

#else /* STORE */
#define PREFIX(f)               f
#define STRING_PREFIX(f)        f
#endif /* STORE */


/* -----------------------------------------------------------------------
 * red_get:  Reads one expression from the given file.
 */
int PREFIX(red_get)(STACKELEM handle)
{
  char *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;

  START_MODUL(STRING_PREFIX("red_get"));
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);

  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);

    if (fh == NULL)
    {
      MAKESTRDESC(_desc,"get: Argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if ( ! T_MODE_READ(R_FILEHANDLE(*fh,mode)))
      {
        MAKESTRDESC(_desc, "get: File is not readable.");
        returnval = ERROR;
      }
      else
      {
#if STORE
        store = fileno(R_FILEHANDLE(*fh,fp));

        PREFIX(store_receive_result)();            /* places result in _desc */

        returnval = OK;
#endif
      }
    }
    free(handlestr);
  }

  END_MODUL(STRING_PREFIX("red_get"));
  return(returnval);
}


/* -----------------------------------------------------------------------
 * red_put:  Puts the expression into the output file.
 */
int PREFIX(red_put)(STACKELEM handle, STACKELEM expr)
{
  char *handlestr;
  FILEHANDLE *fh;
  int returnval = SYNERR;
  
  START_MODUL(STRING_PREFIX("red_put"));
  
  GETSTRING(handlestr, (PTR_DESCRIPTOR) handle);
  
  if (handlestr != NULL)
  {
    GETFILEHANDLE(fh, handlestr);
    
    if (fh == NULL)
    {
      MAKESTRDESC(_desc, "put: 1. argument is not a valid file handle.");
      returnval = ERROR;
    }
    else
    {
      if ( ! T_MODE_WRITE(R_FILEHANDLE(*fh,mode)))
      {
        MAKESTRDESC(_desc, "put: File is not writeable.");
        returnval = ERROR;
      }
      else
      {
#if STORE
        store = fileno(R_FILEHANDLE(*fh,fp));
        _desc = (PTR_DESCRIPTOR)expr;

        PREFIX(store_send_graph)();

        fflush(R_FILEHANDLE(*fh,fp));
        MAKESTRDESC(_desc, "put");
        returnval = OK;
#endif /* STORE */
      }
    }
    free(handlestr);
  }

  END_MODUL(STRING_PREFIX("red_put"));
  return(returnval);
}

/*********************************************************************************/
/* functions for control of the model railroad */

#if TRAIN

int red_set_speed(STACKELEM b, STACKELEM s)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_set_speed");

  START_MODUL("red_set_speed");

  if (T_INT(b) && T_INT(s)) {
    i = set_speed(VAL_INT(b), VAL_INT(s));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }

  END_MODUL("red_set_speed");
  DBUG_RETURN(returnval);
}

int red_get_speed(STACKELEM b)
{
  int returnval = SYNERR;
  int i;

  DBUG_ENTER("red_get_speed");

  START_MODUL("red_get_speed");

  if (T_INT(b)) {
    i = get_speed(VAL_INT(b));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }

  END_MODUL("red_get_speed");
  DBUG_RETURN(returnval);
}

int red_set_direction(STACKELEM b, STACKELEM d)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_set_direction");
 
  START_MODUL("red_set_direction");
 
  if (T_INT(b) && T_INT(d)) {
    i = set_direction(VAL_INT(b), VAL_INT(d));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_set_direction");
  DBUG_RETURN(returnval);
}

int red_get_direction(STACKELEM b)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_get_direction");
 
  START_MODUL("red_get_direction");
 
  if (T_INT(b)) {
    i = get_direction(VAL_INT(b));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_get_direction");
  DBUG_RETURN(returnval);
}

int red_set_point(STACKELEM p, STACKELEM o)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_set_point");
 
  START_MODUL("red_set_point");
 
  if (T_INT(p) && T_INT(o)) {
    i = set_point(VAL_INT(p), VAL_INT(o));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_set_point");
  DBUG_RETURN(returnval);
}

int red_get_point(STACKELEM p)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_get_point");
 
  START_MODUL("red_get_point");
 
  if (T_INT(p)) {
    i = get_point(VAL_INT(p));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_get_point");
  DBUG_RETURN(returnval);
}

int red_set_relay(STACKELEM b, STACKELEM p)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_set_relay");
 
  START_MODUL("red_set_relay");
 
  if (T_INT(b) && T_INT(p)) {
    i = set_relay(VAL_INT(b), VAL_INT(p));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_set_relay");
  DBUG_RETURN(returnval);
}

int red_get_relay(STACKELEM b)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_get_relay");
 
  START_MODUL("red_get_relay");
 
  if (T_INT(b)) {
    i = get_relay(VAL_INT(b));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_get_relay");
  DBUG_RETURN(returnval);
}

int red_set_signal(STACKELEM s, STACKELEM p)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_set_signal");
 
  START_MODUL("red_set_signal");
 
  if (T_INT(s) && T_INT(p)) {
    i = set_signal(VAL_INT(s), VAL_INT(p));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    } 
    
  END_MODUL("red_set_signal");
  DBUG_RETURN(returnval);
} 

int red_get_signal(STACKELEM s)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_get_signal");
 
  START_MODUL("red_get_signal");
 
  if (T_INT(s)) {
    i = get_signal(VAL_INT(s));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_get_signal");
  DBUG_RETURN(returnval);
}

int red_read_M1(STACKELEM b)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_read_M1");
 
  START_MODUL("red_read_M1");
 
  if (T_INT(b)) {
    i = read_M1(VAL_INT(b));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_read_M1");
  DBUG_RETURN(returnval);
}

int red_read_M2(STACKELEM b)
{
  int returnval = SYNERR;
  int i;
 
  DBUG_ENTER("red_read_M2");
 
  START_MODUL("red_read_M2");
 
  if (T_INT(b)) {
    i = read_M2(VAL_INT(b));
    if (i != FAILURE) {
      _desc = TAG_INT(i);
      returnval = OK;
      }
    }
 
  END_MODUL("red_read_M2");
  DBUG_RETURN(returnval);
}

#endif
