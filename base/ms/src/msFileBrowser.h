#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xukc/ScrollList.h>

#include <pwd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "msMacros.h"
#include "dbug.h"


/* from msMain_layout.h */
extern Widget WI_ms_main_file_enter_path;
extern Widget WI_ms_main_file_enter_name;
extern Widget WI_ms_main_file_browse;

extern Widget WI_my_requester_select_label;
extern Widget WI_my_requester_select_scroll;
extern Widget WI_my_requester_select_enter_mask;
extern Widget WI_my_requester_select_enter_path;
extern Widget WI_my_requester_select_enter_file;

extern Widget WI_my_requester_box;

/* from msTools.c */
extern void dieHard();

/* from msMain_main.c */
extern void fileSelectedHook();
