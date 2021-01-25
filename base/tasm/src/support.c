/************************************************************************/
/* support.c: Functions and data which are used in several modules.     */
/************************************************************************/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mytypes.h"
#include "buildtree.h"
#include "loctypes.h"
#include "main.h"
#include "tiny.h"
#include "support.h"
#include "dbug.h"
#include "error.h"



/************************************************************************/
/* get_ia_arity returns the arity of an interaction.                    */
/* (used in tis.c, ris.c and tasm2c.y)                                  */
/************************************************************************/
int get_ia_arity(IACTION iaction)
{
  int dim;

  DBUG_ENTER("get_ia_arity");
  switch (iaction)
  {
    case ia_getwd:
                    dim = 0;
                    break;
    case ia_fclose:
    case ia_fgetc:
    case ia_fgets:
    case ia_ftell:
    case ia_exit:
    case ia_eof:
    case ia_unit:
    case ia_status:
    case ia_remove:
    case ia_mkdir:
    case ia_rmdir:
    case ia_chdir:
    case ia_dir:
    case ia_get:
                    dim = 1;
                    break;
    case ia_fopen:
    case ia_fputc:
    case ia_ungetc:
    case ia_read:
    case ia_fputs:
    case ia_fredirect:
    case ia_bind:
    case ia_finfo:
    case ia_rename:
    case ia_copy:
    case ia_put:
                    dim = 2;
                    break;
    case ia_fprintf:
    case ia_fseek:
                    dim = 3;
                    break;
    default:                           /* interaction not known */
                    yyfail("%s","unknown interaction!");
                    dim = 0;
  }
  DBUG_RETURN(dim);
}

