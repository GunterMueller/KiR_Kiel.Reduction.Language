#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xukc/Toggle.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "msMacros.h"
#include "dbug.h"


extern Widget WI_ms_main_module_menu;
extern Widget WI_ms_main_file_filter_menu;
extern Widget WI_ms_main_file_enter_path;
extern Widget WI_ms_main_file_enter_name;
extern Widget WI_ms_main_reduma_init_menu;
extern Widget WI_ms_main_preferences_toggle;

extern char dbugOptions[DBUG_LEN];

/* from msTools.c */
extern Boolean isLF();
extern Boolean isBlank();


struct FilterArray {
  short ret, group, mIndex;
  Boolean state, enabled;
  char onString[IDS_LEN+2], offString[IDS_LEN+2];
};

struct InitArray {
  short ret, group;
  Boolean state;
  char onString[IDS_LEN+2], offString[IDS_LEN+2];
};
