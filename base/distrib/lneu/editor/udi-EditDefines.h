#ifndef _UDI_EDITDEFINES_
#define _UDI_EDITDEFINES_ 1
/* Syntax tty status modes : *****************************************************/
#define S_TTY_READONLY          1               /* set when only allowed to read */
#define S_TTY_EXECUTE           2                 /* set when allowed to execute */
#define S_TTY_COMMANDLINE       4          /* Set when allowed to enter commands */
#define S_TTY_FUNCTIONKEYS      8             /* Set when allowed to press fkeys */
#define S_TTY_NORMAL    (S_TTY_FUNCTIONKEYS|S_TTY_COMMANDLINE|S_TTY_EXECUTE)

extern int UDI_EDIT_MODE;

#define UDI_CHECK_MODE(mode) ((UDI_EDIT_MODE&mode)==mode)
#endif
