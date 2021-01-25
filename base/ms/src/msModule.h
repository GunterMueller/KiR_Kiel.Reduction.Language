#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Dialog.h>

#include <X11/Xukc/Toggle.h>
#include <X11/Xukc/Med3Menu.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dlfcn.h>
#include <varargs.h>

#include "msMacros.h"
#include "scriptMacros.h"
#include "dbug.h"
#include "groups.h"
#include "id2index.c"
#include "convert.c"
#include "vars.h"
#include "ioFunctions.c"


/* Button actions */
#define NOACTION     0
#define GOTOSTART    1
#define PLAYBACKWARD 2
#define STEPBACKWARD 3
#define STOP         4
#define STEPFORWARD  5
#define PLAYFORWARD  6
#define GOTOEND      7

#define ERROR -1

#define FRAME_SIZE 1000                     /* Number of elements in a frame */


extern Widget WI_ms_progress_bar;

extern void split();

extern char interfaceFile[FILE_LEN];


struct GroupsArray {
  char group[IDS_LEN];
};


struct IdsArray {
  Boolean selected;
  short group, bar;
};


struct FilterArray {
  short ret, group, mIndex;
  Boolean state, enabled;
  char onString[IDS_LEN+2], offString[IDS_LEN+2];
};

struct Frame {
  struct Frame *previous, *next;
  char data[FRAME_SIZE][sizeof(dummyElement)];
} dummyFrame;
