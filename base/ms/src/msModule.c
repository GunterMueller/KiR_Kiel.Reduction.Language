int (*mscanfTime)();
int (*mscanfProcessor)();
int (*mscanfProcess)();
int (*mscanfId)();
int (*mscanfInt)();
int (*mscanfString)();
int (*mscanfLength)();


#include "msModule.h"

extern char lastRequestAnswer[];

extern char ** environ;

static Widget moduleWindow;
static char prefsName[FILE_LEN];
static XColor xColor;
static Pixel buttonPressedColor, buttonReleasedColor, buttonDisabledColor;
static struct FilterArray filterArray[MAX_IDS];
static struct IdsArray idsArray[MAX_IDS];
static struct GroupsArray groupsArray[MAX_GROUPS];
static short numGroups, numIds, numRegularIds;
static FILE *inF;
static long startOfData;
static double timeOffset, lastTimeStamp, realLastTimeStamp;
static char selectedFile[2*FILE_LEN], selectedScript[2*FILE_LEN];
static Boolean firstFrame, scriptIsActive, scriptIsTerminated,
               frameCompleted, doUpdates, finished;
static int actualLine, actualFrame, nextAction, lastAction,
           actualDelay, actualRate;
static double actualTime, frontTime;
static Boolean reachedUpperBoundary, reachedLowerBoundary;
static struct Frame *actualStackFrame, *firstStackFrame, *displayedStackFrame;
static int numStackFramesUsed, numStackFramesAllocated,
           actualData, displayedData;

static Boolean doLocalDoTick;

static Display *display;
static int screen;
static Colormap colorMap;

static Window progressWindow;
static GC gcDark = (GC)NULL,
          gcLight = (GC)NULL,
          gcProgressBlack = (GC)NULL;
static int progressWidth, progressHeight, lastProgressSize;

static Boolean continueFrame;

static char headerFile[DATA_LEN], headerTime[DATA_LEN],
            headerDimension[DATA_LEN], headerSteps[DATA_LEN],
            headerFactor[DATA_LEN], headerBinary[DATA_LEN],
            headerNoDistNum[DATA_LEN], headerHeapsize[DATA_LEN],
            headerHeapdes[DATA_LEN];
static Boolean isBinary;
static double timeFactor;

static int numSlots[NUM_PROCESSORS], heapBases[NUM_PROCESSORS];

void *dynamicHandle;
void (*eval)(), (*rememberEval)();
void (*iNeed)();


extern void dieHard();
extern void doRequest();
extern Boolean isLF();
extern Boolean isBlank();
extern void msFileBrowserInitialize();
extern Boolean XawAsciiSaveAsFile();
extern Widget XawTextGetSource();

extern time_t time();
extern char *ctime();
extern int wait3();
extern usleep();
extern int cfree();
extern int system();

/* Graphics primitives */
void FLUSH();                                            /* Update X display */
void HEADER_FILE();                           /* Get file string from header */
void HEADER_TIME();                           /* Get time string from header */
void HEADER_NODISTNUM();                 /* Get nodistnum string from header */
void HEADER_DIMENSION();                 /* Get dimension string from header */
void HEADER_STEPS();                         /* Get steps string from header */
void HEADER_FACTOR();                       /* Get factor string from header */
void HEADER_BINARY();                       /* Get binary string from header */
void HEADER_HEAPSIZE();                   /* Get heapsize string from header */
void HEADER_HEAPDES();                     /* Get heapdes string from header */
void GET_TIME();                     /* Get start- and end-time of reduction */
void SET_DELAY();  /* Set animation speed (minimal delay between two frames) */
void SET_RATE();              /* Set frame rate (show only every n'th frame) */


/*****************************************************************************/
/*                                                                           */
/*  fetchPreferences: (local function)                                       */
/*                                                                           */
/*  Get initial settings...                                                  */
/*                                                                           */
/*****************************************************************************/

void fetchPreferences()
{
  FILE *f;

  DBUG_ENTER("fetchPreferences");

  if((f = fopen(prefsName, "r")) != NULL) {
    fgets(selectedScript, FILE_LEN, f);
    selectedScript[strlen(selectedScript)-1] = '\0';
    fclose(f);
  } else {
    strcpy(selectedScript, "");
  }

  DBUG_VOID_RETURN;
}

/*****************************************************************************/
/*                                                                           */
/*  storePreferences: (callback)                                             */
/*                                                                           */
/*  Store preferences now...                                                 */
/*                                                                           */
/*****************************************************************************/

void storePreferences()
{
  FILE *f;
  char error[ERROR_LEN];

  DBUG_ENTER("storePreferences");

  if((f = fopen(prefsName, "w")) == NULL) {
    sprintf(error, "Can't open preferences file 'ms.prefs' for write...");
    dieHard(error);
  }

  fprintf(f, "%s\n", selectedScript);
    
  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  reaper: (local function)                                                 */
/*                                                                           */
/*  Avoid defunct-processes when forked process exits (from Netrek)...       */
/*                                                                           */
/*****************************************************************************/

static void reaper(sig)
int sig;
{
  while (wait3((union wait *) 0, WNOHANG, (struct rusage *) 0) > 0)
    ;
}


/*****************************************************************************/
/*                                                                           */
/*  generatePostscriptHeader: (global function)                              */
/*                                                                           */
/*  Write Postscript-Header to file...                                       */
/*                                                                           */
/*****************************************************************************/

void generatePostscriptHeader(f, p, xSize, ySize)
FILE *f;
char p[];
int xSize, ySize;
{
  time_t now = time(NULL);

  DBUG_ENTER("generatePostscriptHeader");

  fprintf(f, "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%Title: %s\n%%%%Creator: %s\n",
	  selectedFile, "ms");
  fprintf(f, "%%%%CreationDate: %s", ctime(&now));
  fprintf(f, "%%%%Pages: 0 - 1\n%%%%DocumentFonts: Helvetica\n");
  fprintf(f, "%%%%BoundingBox: %d %d %d %d\n", 0, 0, xSize, ySize);
  fprintf(f, "%%%%EndComments\n");

  fprintf(f, "/ccshow { dup stringwidth exch 2 div neg exch 1.5 sub rmoveto show } def\n");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  newData: (local function)                                                */
/*                                                                           */
/*  Return pointer to next data entry in stack...                            */
/*                                                                           */
/*****************************************************************************/

void *newData()
{
  struct Frame *temp;

  DBUG_ENTER("newData");

  DBUG_PRINT("newData", ("actualData=%d", actualData));

  if(!numStackFramesAllocated) {
    /* Get first frame... */
    numStackFramesAllocated = numStackFramesUsed = 1;
    DBUG_PRINT("newData", ("Allocate frame #%d", numStackFramesAllocated));

    if(!(actualStackFrame = (struct Frame *)calloc(1, sizeof(dummyFrame)))) {
      dieHard("Can't allocate another frame for stacking...");
    }

    firstStackFrame = actualStackFrame;
    firstStackFrame->previous = (struct Frame *)NULL;
  }

  if(++actualData >= FRAME_SIZE) {
    /* Need new frame... */
    if(numStackFramesUsed < numStackFramesAllocated) {
      /* Great! There's another one allocated, yet... */
      actualStackFrame = actualStackFrame->next;
    } else {
      /* Pah, they are all gone! Allocate new one... */

      ++numStackFramesAllocated;
      DBUG_PRINT("newData", ("Allocate frame #%d", numStackFramesAllocated));

      temp = actualStackFrame;
      if(!(actualStackFrame = (struct Frame *)calloc(1, sizeof(dummyFrame)))) {
	dieHard("Can't allocate another frame for stacking...");
      }

      temp->next = actualStackFrame;
      actualStackFrame->previous = temp;
    }
    ++numStackFramesUsed;
    DBUG_PRINT("newData", ("(Re-)Use frame #%d", numStackFramesUsed));
    actualData = 0;
  }

  if(firstFrame) {
    /* Dummy entry (ordered by NEXT_FRAME)... */
    actualData = 0;
  }

  displayedStackFrame = actualStackFrame;
  displayedData = actualData;

  DBUG_RETURN((void *)actualStackFrame->data[actualData]);
}


/*****************************************************************************/
/*                                                                           */
/*  updateTime: (local function)                                             */
/*                                                                           */
/*  Update time display...                                                   */
/*                                                                           */
/*****************************************************************************/

static void updateTime(time)
double time;
{
  char buffer[DISPLAY_LEN];

  DBUG_ENTER("updateTime");

  sprintf(buffer, "%.0f", time);
  SET_LABEL(WI_ms_position_panel_enter_time, buffer);
  sprintf(buffer, "%.1f", time/timeFactor);
  SET_LABEL(WI_ms_position_panel_show_line, buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateLine: (local function)                                             */
/*                                                                           */
/*  Update line display...                                                   */
/*                                                                           */
/*****************************************************************************/

static void updateLine(line)
int line;
{
  char buffer[DISPLAY_LEN];

  DBUG_ENTER("updateLine");

  /* Disabeled --> Real time (see "updateTime") instead... */

  DBUG_VOID_RETURN;

  sprintf(buffer, "%d", line);
  SET_LABEL(WI_ms_position_panel_show_line, buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateFrame: (local function)                                            */
/*                                                                           */
/*  Update frame display...                                                  */
/*                                                                           */
/*****************************************************************************/

static void updateFrame(frame)
int frame;
{
  char buffer[DISPLAY_LEN];

  DBUG_ENTER("updateFrame");

  sprintf(buffer, "%d", frame);
  SET_LABEL(WI_ms_position_panel_enter_frame, buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  getNewSize: (local function)                                             */
/*                                                                           */
/*  Get size at startup or after resize...                                   */
/*                                                                           */
/*****************************************************************************/

void getNewSize()
{
  Dimension width, height;

  DBUG_ENTER("getNewSize");

  GET_SIZE(WI_ms_progress_bar, &width, &height);

  progressWidth = width;
  progressHeight = height;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateProgress: (local function)                                         */
/*                                                                           */
/*  Update progress bar...                                                   */
/*                                                                           */
/*****************************************************************************/

void updateProgress()
{
  int size = (int)((double)progressWidth*actualTime/lastTimeStamp),
      front = (int)((double)progressWidth*frontTime/lastTimeStamp);
  
  DBUG_ENTER("updateProgress");

  if(reachedUpperBoundary) {
    size = progressWidth;
  }
  if(reachedLowerBoundary) {
    size = 0;
  }

  if(size != lastProgressSize) {
    XFillRectangle(display, progressWindow, gcDark,
		   0, 0, size, progressHeight);
    XFillRectangle(display, progressWindow, gcLight,
		   size, 0, progressWidth-size, progressHeight);
    XDrawLine(display, progressWindow, gcProgressBlack,
	      size, 0, size, progressHeight);

    /* Draw front marker... */
    XDrawLine(display, progressWindow, gcProgressBlack,
	      front, 0, front, progressHeight);
  }

  lastProgressSize = size;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  doNothing: (local function)                                              */
/*                                                                           */
/*  Dummy "eval" (to skip the trailing part of lines in binary format...     */
/*                                                                           */
/*****************************************************************************/

void doNothing()
{
  return;
}


/*****************************************************************************/
/*                                                                           */
/*  nextLine: (local function)                                               */
/*                                                                           */
/*  Feed the next line of data to the script...                              */
/*                                                                           */
/*****************************************************************************/

static Boolean nextLine()
{
  char buffer[DATA_LEN];
  short dummyLength, length;

  DBUG_ENTER("nextLine");

  if(continueFrame) {
    continueFrame = FALSE;

    ID = MINDEX_CONTINUE;
    (*eval)();

    DBUG_RETURN(TRUE);
  } else {
    FOREVER {
      dummyLength = MSCAN(inF, &TIME, &PROCESSOR, &PROCESS, &ID);

      if(feof(inF) || (TIME > realLastTimeStamp) || finished) {
	DBUG_PRINT("nextLine", ("EOF reached..."));

	if(!scriptIsTerminated) {
	  scriptIsTerminated = TRUE;
	  ID = MINDEX_END;
	  (*eval)();
	}

	lastAction = nextAction = NOACTION;

	DBUG_RETURN(FALSE);
      } else {
	DBUG_PRINT("nextLine",
		   ("time=%lf processor=%d process=%d id=%d (%s)",
		    TIME, PROCESSOR, PROCESS, ID, ids[ID]));

	finished = (TIME == realLastTimeStamp);

	/* Filter this id? */
	if(idsArray[ID].selected &&
	   idsArray[numRegularIds+PROCESSOR].selected) {
	  /* It's not filtered, convert it... */
	  ++actualLine;                    /* Count only non-filtered lines! */
	  
	  frontTime = actualTime = TIME -= timeOffset;

	  if((length = convert(inF)) == -1) {
	    dieHard("Giving up...");
	  }

	  if(isBinary) {
	    MSCAN_LENGTH(inF, &dummyLength);

	    if((length+14) != dummyLength) {
	      char message1[100], message2[100];

	      sprintf(message1, "Error at timestamp %.0f:", TIME);
	      sprintf(message2, "Found length %d (should be %d)!",
		      dummyLength, length);

	      doRequest(message1, message2, "", "OK", "", "", NULL);

	      DBUG_PRINT("nextLine", ("TIME=%.0f LENGTH=%d length=%d",
				      TIME, dummyLength, length));

	      scriptIsTerminated = TRUE;
	      lastAction = nextAction = NOACTION;
	      DBUG_RETURN(FALSE);
	    } 
	  }

	  break;
	} else {
	  /* It's filtered, ignore rest of line and continue with next... */
	  if(isBinary) {
	    eval = &doNothing;
	    if((length = convert(inF)) == -1) {
	      dieHard("Giving up...");
	    }
	    eval = rememberEval;

	    MSCAN_LENGTH(inF, &dummyLength);

	    if((length+14) != dummyLength) {
	      char message1[100], message2[100];
	      
	      sprintf(message1, "Error at timestamp %.0f:", TIME);
	      sprintf(message2, "Found length %d (should be %d)!",
		      dummyLength, length);

	      doRequest(message1, message2, "", "OK", "", "", NULL);

	      DBUG_PRINT("nextLine", ("TIME=%.0f LENGTH=%d length=%d",
				      TIME, dummyLength, length));

	      scriptIsTerminated = TRUE;
	      lastAction = nextAction = NOACTION;
	      DBUG_RETURN(FALSE);
	    }    
	  } else {
	    fgets(buffer, DATA_LEN, inF);
	  }
	}
      }
    }

    DBUG_RETURN(TRUE);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  restoreFrame: (local function)                                           */
/*                                                                           */
/*  Update frame (when walking trough the movie)...                          */
/*                                                                           */
/*****************************************************************************/

void restoreFrame(forward)
Boolean forward;
{
  struct Frame *tempStackFrame;
  int startFrame, frame, tempData;
  Boolean first;

  DBUG_ENTER("restoreFrame");

  first = TRUE;

  FOREVER {
    tempStackFrame = displayedStackFrame;
    tempData = displayedData;

    if(forward) {
      if((displayedStackFrame == actualStackFrame) &&
	 (displayedData == actualData)) {
	/* End of stacked data reached... */
	break;
      } else {
	/* Skip to next command... */
	if(++tempData >= FRAME_SIZE) {
	  tempStackFrame = displayedStackFrame->next;
	  tempData = 0;
	}
      }
    } else {
      if((displayedStackFrame == firstStackFrame) &&
	 (displayedData == 0)) {
	/* Begif of stacked data reached... */
	break;
      }
    }

    frame = data2Frame((void *)(tempStackFrame->data[tempData]));

    if(first) {
      startFrame = frame;
      first = FALSE;
    }

    DBUG_PRINT("restoreFrame", ("startFrame=%d frame=%d", startFrame, frame));

    if(startFrame != frame) {
      /* New frame... */
      break;
    } else {
      if(forward) {
	/* Execute command... */
	displayedStackFrame = tempStackFrame;
	displayedData = tempData;
	recallCommand(displayedStackFrame->data[displayedData], forward);
      } else {
	recallCommand(displayedStackFrame->data[displayedData], forward);
      
	/* Skip to previous command... */
	if(--displayedData < 0) {
	  displayedStackFrame = displayedStackFrame->previous;
	  displayedData = FRAME_SIZE-1;
	}
      }
    }
  }

  actualTime = data2Time((void *)(displayedStackFrame->data[displayedData]));
  actualFrame = data2Frame((void *)(displayedStackFrame->data[displayedData]));

  ++actualFrame;
  updateData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  doDelay: (local function)                                                */
/*                                                                           */
/*  Wait specified time (actualDelay)...                                     */
/*                                                                           */
/*****************************************************************************/

void doDelay()
{
  DBUG_ENTER("doDelay");

  if(doUpdates && actualDelay &&
     ((lastAction == PLAYBACKWARD) || (lastAction == PLAYFORWARD))) {
    /* Delay... */
    XSync(display, FALSE);
    usleep((unsigned) actualDelay*1000);           /* do it proportional ??? */
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  gotoFrame: (local function)                                              */
/*                                                                           */
/*  Jump to specified frame...                                               */
/*                                                                           */
/*****************************************************************************/

void gotoFrame(destination)
int destination;
{
  Boolean b;

  DBUG_ENTER("gotoFrame");

  DBUG_PRINT("gotoFrame", ("destination=%d actualFrame=%d",
			   destination, actualFrame));

  if(destination > actualFrame) {
    /* Forward... */

    while(actualFrame < destination) {
      if((displayedStackFrame != actualStackFrame) ||
	 (displayedData != actualData)) {
	/* There are buffered frames ahead... */
	restoreFrame(TRUE);
	reachedLowerBoundary = FALSE;
      } else {
	if(!scriptIsTerminated) {
	  /* Generate next frame from file... */
	  while((b = nextLine())) {
	    if(frameCompleted) {
	      frameCompleted = FALSE;
	      break;
	    }
	  }
	  if(!b) {
	    reachedUpperBoundary = TRUE;
	    if(actualFrame) {
	      --actualFrame;
	    }
	  }

	  reachedLowerBoundary = FALSE;
	} else {
	  reachedUpperBoundary = TRUE;
	  break;
	}
      }

      doDelay();
    }

    updateProgress();

    DBUG_VOID_RETURN;
  }

  if(destination < actualFrame) {
    /* Backward... */

    while(actualFrame > destination) {
      if((displayedStackFrame != firstStackFrame) ||
	 (displayedData > 0)) {
	/* There are buffered frames behind... */
	restoreFrame(FALSE);
	reachedUpperBoundary = FALSE;
      } else {
	reachedLowerBoundary = TRUE;
	break;
      }

      doDelay();
    }    

    updateProgress();

    DBUG_VOID_RETURN;
  }

  /* You are on this frame, stupid! */

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  gotoTime: (local function)                                               */
/*                                                                           */
/*  Jump to specified time...                                                */
/*                                                                           */
/*****************************************************************************/

void gotoTime(destination)
double destination;
{
  struct Frame *temp;
  int i, frame, rememberFrame;
  Boolean goOn;
  long time, rememberTime;

  DBUG_ENTER("gotoTime");

  doUpdates = FALSE;

  if(destination == actualTime) {
    /* Fool! You are there already... */
    DBUG_VOID_RETURN;
  }

  if((actualStackFrame == firstStackFrame) && !actualData) {
    gotoFrame(2);
  }

  if(destination <= data2Time((void *)(actualStackFrame->data[actualData]))) {
    /* Destination is in stack already... */

    temp = firstStackFrame;
    goOn = TRUE;
    rememberTime = 0;
    rememberFrame = 1;

    while(temp) {
      for(i=0; i<FRAME_SIZE; ++i) {
	time = data2Time((void *)(temp->data[i]));
	frame = data2Frame((void *)(temp->data[i]));

	if((destination >= rememberTime) && (destination < time)) {
	  goOn = FALSE;
	  break;
	} else {
	  rememberTime = time;
	  rememberFrame = frame;
	}
      }

      if(!goOn) {
	break;
      }      
      temp = temp->next;
    }

    gotoFrame(frame);
  } else {
    /* Destination lies ahead, but not in the stack, yet... */
    /* Skip to end of stacked data... */
    frame = data2Frame((void *)(actualStackFrame->data[actualData]));
    gotoFrame(frame);
    
    /* Do the rest... */
    doUpdates=FALSE;
    while((destination > actualTime) && !reachedUpperBoundary) {
      gotoFrame(++frame);
    }

    /* Reached EOF? No --> One step back! */
    gotoFrame(frame-(reachedUpperBoundary ? 0 : 1));
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateDelay: (local function)                                            */
/*                                                                           */
/*  Update delay...                                                          */
/*                                                                           */
/*****************************************************************************/

void updateDelay(i)
int i;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("updateDelay");

  sprintf(temp, "%d", actualDelay);
  SET_LABEL(WI_ms_speed_panel_enter_delay, temp); 

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateRate: (local function)                                             */
/*                                                                           */
/*  Update rate...                                                           */
/*                                                                           */
/*****************************************************************************/

void updateRate(i)
int i;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("updateRate");

  sprintf(temp, "%d", actualRate);
  SET_LABEL(WI_ms_speed_panel_enter_rate, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  makeModule: (local function)                                             */
/*                                                                           */
/*  Make .so-file from script (via awk, gcc and ld)...                       */
/*                                                                           */
/*****************************************************************************/

void makeModule(basename)
char *basename;
{
  char command[COMMANDS_LEN], file[FILE_LEN];
  struct stat buf;
  time_t modifyTime;

  DBUG_ENTER("makeModule");

  sprintf(file, "%s.scr", basename);
  stat(file, &buf);
  modifyTime = buf.st_mtime;

  sprintf(file, "%s.so", basename);
  if(stat(file, &buf) || (modifyTime > buf.st_mtime)) {
    sprintf(command, "rm -f %s", file);
    system(command);
    sprintf(command, "scr2c.awk %s.scr >%s.c", basename, basename);
    system(command);
    sprintf(command, "/bin/cc -I. -o %s.o -pic -c %s.c", basename, basename);
    system(command);
    sprintf(command, "/bin/ld -o %s.so %s.o", basename, basename);
    system(command);
  }
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  checkIds: (local function)                                               */
/*                                                                           */
/*  Check for compatibility of actual measurement file and script...         */
/*                                                                           */
/*****************************************************************************/

Boolean checkIds()
{
  char needed[IDS_LEN][MAX_IDS];
  int i = 0, j;

  DBUG_ENTER("checkIds");

  (*iNeed)(needed);

  while(strlen(needed[i])) {
    DBUG_PRINT("checkIds", ("Checking %s...", needed[i]));

    for(j=3; j<=numIds; ++j) {
      if(!strcmp(needed[i], filterArray[j].onString+2)) {
	if(!(filterArray[j].enabled && filterArray[j].state)) {
	  DBUG_RETURN(FALSE);
	}
      }
    }

    ++i;
  }

  DBUG_RETURN(TRUE);
}


/*****************************************************************************/
/*                                                                           */
/*  startModule: (local function)                                            */
/*                                                                           */
/*  Start script...                                                          */
/*                                                                           */
/*****************************************************************************/

void startModule()
{
  char temp[FILE_LEN], path[2*FILE_LEN], file[2*FILE_LEN];


  DBUG_ENTER("startModule");

  reachedUpperBoundary = reachedLowerBoundary = FALSE;
  numStackFramesUsed = 1;
  actualData = displayedData = 0;
  displayedStackFrame = actualStackFrame = firstStackFrame;
  firstFrame = TRUE;
  frameCompleted = FALSE;
  doUpdates = TRUE;
  continueFrame = FALSE;

  lastProgressSize = 0;

  finished = FALSE;

  localStartModule();

  /* Prepare file... */
  fseek(inF, startOfData, 0);

  split(selectedScript, path, file);
  if(strlen(file)) {
    /* Translate script... */
    strcpy(temp, selectedScript);
    *((char *)rindex(temp, '.')) = '\0';
    makeModule(temp);
    strcat(temp, ".so");

    /* Make sure, that the old module is unlinked... */
    DBUG_PRINT("startModule", ("Unlinking old module..."));
    if(eval != NULL) {
      dlclose(dynamicHandle);
    }

    DBUG_PRINT("startModule", ("Opening new module..."));
    if((dynamicHandle = dlopen(temp, 1)) == NULL) {
      doRequest("Compilation failed!", 
		"See output for further information...", "",
		"OK", "", "", NULL);
      DBUG_VOID_RETURN;
      /*dieHard("'dlopen' failed...");*/
    }

    DBUG_PRINT("startModule", ("Linking new module..."));
    if((rememberEval = eval = dlsym(dynamicHandle, "eval")) == NULL) {
      doRequest("Compilation failed!", 
		"See output for further information...", "",
		"OK", "", "", NULL);
      DBUG_VOID_RETURN;
      /*dieHard("'dlsym' failed...");*/
    } 

    iNeed = dlsym(dynamicHandle, "iNeed");

    /* Check for compatibility of measure file and script... */
    if(!checkIds()) {
      doRequest("Some data needed by this script", 
		"wasn't recorded in the selected", "measurement file!",
		"OK", "", "", NULL);

      DBUG_VOID_RETURN;
    } else {
      scriptIsActive = TRUE;

      ID = MINDEX_BEGIN;
      (*eval)();
    }
  } else {
    scriptIsActive = FALSE;
    DBUG_PRINT("startModule", ("No script selected..."));

    doRequest("", "Please select a script...", "", "OK", "", "", NULL);

    /*dieHard("Please select a script...");*/
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelGoToStart: (callback)                                      */
/*                                                                           */
/*  Restore the first frame...                                               */
/*                                                                           */
/*****************************************************************************/

void msControlPanelGoToStart(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelGoToStart");

  nextAction = GOTOSTART;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelPlayBackward: (callback)                                   */
/*                                                                           */
/*  Play frames backward...                                                  */
/*                                                                           */
/*****************************************************************************/

void msControlPanelPlayBackward(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelPlayBackward");

  nextAction = PLAYBACKWARD;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelStepBackward: (callback)                                   */
/*                                                                           */
/*  Show the previous frame...                                               */
/*                                                                           */
/*****************************************************************************/

void msControlPanelStepBackward(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelStepBackward");

  nextAction = STEPBACKWARD;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelStop: (callback)                                           */
/*                                                                           */
/*  Stop...                                                                  */
/*                                                                           */
/*****************************************************************************/

void msControlPanelStop(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelStop");

  nextAction = STOP;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelStepForward: (callback)                                    */
/*                                                                           */
/*  Show the next frame...                                                   */
/*                                                                           */
/*****************************************************************************/

void msControlPanelStepForward(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelStepForward");

  nextAction = STEPFORWARD;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelPlayForward: (callback)                                    */
/*                                                                           */
/*  Play frames forward...                                                   */
/*                                                                           */
/*****************************************************************************/

void msControlPanelPlayForward(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelPlayForward");

  nextAction = PLAYFORWARD;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msControlPanelGoToEnd: (callback)                                        */
/*                                                                           */
/*  Show the final frame...                                                  */
/*                                                                           */
/*****************************************************************************/

void msControlPanelGoToEnd(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msControlPanelGoToEnd");

  nextAction = GOTOEND;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  getLastTimeStamp: (local function)                                       */
/*                                                                           */
/*  Return upper time limit... (called my 'fileSelectedHook')                */
/*                                                                           */
/*****************************************************************************/

void getLastTimeStamp()
{
  short dummyProcessor, dummyId;
  int dummyProcess, dummyLength, i;
  Boolean flags[MAX_PROCESSORS];
  short lineLength;
  long rememberPosition;
  
  DBUG_ENTER("getLastTimeStamp");

  if(isBinary) {
    fseek(inF, -1, 2);
    bscanfLength(inF, &lineLength);
    fseek(inF, -(lineLength+1), 2);
  } else {
    fseek(inF, -2, 2);
    while(!isLF(getc(inF))) {
      fseek(inF, -2, 1);
    }
  }

  rememberPosition = ftell(inF);
  dummyLength = MSCAN(inF, &realLastTimeStamp, &dummyProcessor, &dummyProcess,
		      &dummyId);

  if(dummyId == MINDEX_RECEIVE_BEGIN) {
    /* Skip the RECEIVE_BEGINs... */

    for(i=0; i<MAX_PROCESSORS; ++i) {
      flags[i] = FALSE;
    }

    FOREVER {
      fseek(inF, rememberPosition, 0);
      if(isBinary) {
	fseek(inF, -1, 1);
	bscanfLength(inF, &lineLength);
	fseek(inF, -(lineLength+1), 1);
      } else {
	fseek(inF, -2, 2);
	while(!isLF(getc(inF))) {
	  fseek(inF, -2, 1);
	}
      }
    
      rememberPosition = ftell(inF);
      dummyLength = MSCAN(inF, &realLastTimeStamp, &dummyProcessor,
			  &dummyProcess, &dummyId);

      if((dummyId != MINDEX_RECEIVE_BEGIN) || flags[(int)dummyProcessor]) {
	break;
      } else {
	flags[(int)dummyProcessor] = TRUE;
      }
    }
  }

  lastTimeStamp = realLastTimeStamp - timeOffset;
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  fileSelectedHook: (exported function)                                    */
/*                                                                           */
/*  Examine file after selection... (called my 'msFileBrowserSelectFile')    */
/*                                                                           */
/*****************************************************************************/

void fileSelectedHook(which, path, file)
short which;
char *path, *file;
{
  char id[IDS_LEN], error[ERROR_LEN], header[DATA_LEN];
  short expect = HEADER, i, group;
  char temp[2*FILE_LEN];
  struct stat statBuffer;

  DBUG_ENTER("fileSelectedHook");

  switch(which) {
  case 0:                                                   /* File selected */
    SET_LABEL(WI_ms_file_panel_enter_name, file);
    SET_LABEL(WI_ms_file_panel_enter_path, path);
    strcpy(selectedFile, path);
    strcat(selectedFile, file);

    /* Close actual file... */
    fclose(inF);

    /* Terminate script... */
    scriptIsActive = scriptIsTerminated = FALSE; 
    if(eval != NULL) {
      dlclose(dynamicHandle);
    }

    if(strlen(selectedFile) && 
       !stat(selectedFile, &statBuffer) &&
       (S_ISREG(statBuffer.st_mode) || S_ISLNK(statBuffer.st_mode))) {

      /* Disable all (but processor) items in filter menu... */
      for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
	TOGGLE_DISABLE(WI_ms_file_panel_filter, i);
	filterArray[i].enabled = FALSE;
      }

      /* Scan the 'INIT'-part of the selected file... */
      if((inF = fopen(selectedFile, "r")) == NULL) {
	sprintf(error, "Can't open data file '%s' for read...", selectedFile);
	strcpy(selectedFile, "");
      } else {
	timeOffset = 0.0;
	while(!feof(inF)) {
	  switch(expect) {
	    
	  case HEADER:
	    fscanf(inF, "%s", header);
	    
	    if(!strcmp(header, "file:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerFile, DATA_LEN, inF);
	      headerFile[strlen(headerFile)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerFile=%s", headerFile));
	    }
	    if(!strcmp(header, "time:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerTime, DATA_LEN, inF);
	      headerTime[strlen(headerTime)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerTime=%s", headerTime));
	    }
	    if(!strcmp(header, "cubedim:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerDimension, DATA_LEN, inF);
	      headerDimension[strlen(headerDimension)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerDimension=%s", headerDimension));
	    }
	    if(!strcmp(header, "reduction")) {
	      fscanf(inF, "%s", header);                    /* Skip "steps:" */
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerSteps, DATA_LEN, inF);
	      headerSteps[strlen(headerSteps)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerSteps=%s", headerSteps));
	    }
	    if(!strcmp(header, "time")) {
	      fscanf(inF, "%s", header);                   /* Skip "factor:" */
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerFactor, DATA_LEN, inF);
	      headerFactor[strlen(headerFactor)-1] = '\0';
	      timeFactor = atof(headerFactor);
	      DBUG_PRINT("fileSelectedHook",
			 ("headerFactor=%s", headerFactor));
	    }
	    if(!strcmp(header, "nodistnum:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerNoDistNum, DATA_LEN, inF);
	      headerNoDistNum[strlen(headerNoDistNum)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerNoDistNum=%s", headerNoDistNum));
	    }
	    if(!strcmp(header, "binary:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerBinary, DATA_LEN, inF);
	      headerBinary[strlen(headerBinary)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerBinary=%s", headerBinary));
	      
	      if((isBinary = (atoi(headerBinary) == 1))) {
		mscanfTime = &bscanfTime;
		mscanfProcessor = &bscanfProcessor;
		mscanfProcess = &bscanfProcess;
		mscanfId = &bscanfId;
		mscanfInt = &bscanfInt;
		mscanfString = &bscanfString;
		mscanfLength = &bscanfLength;
	      } else {
		mscanfTime = &ascanfTime;
		mscanfProcessor = &ascanfProcessor;
		mscanfProcess = &ascanfProcess;
		mscanfId = &ascanfId;
		mscanfInt = &ascanfInt;
		mscanfString = &ascanfString;
		mscanfLength = &ascanfLength;
	      }
	    }
	    if(!strcmp(header, "heapsize:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerHeapsize, DATA_LEN, inF);
	      headerHeapsize[strlen(headerHeapsize)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerHeapsize=%s", headerHeapsize));
	    }
	    if(!strcmp(header, "heapdes:")) {
	      fgetc(inF);                                      /* Skip space */
	      fgets(headerHeapdes, DATA_LEN, inF);
	      headerHeapdes[strlen(headerHeapdes)-1] = '\0';
	      DBUG_PRINT("fileSelectedHook",
			 ("headerHeapdes=%s", headerHeapdes));
	    }
	    if(!strcmp(header, "---")) {
	      expect = INIT;
	    }
	    break;
	    
	  case INIT:
	    fscanf(inF, "%s", id);
	    
	    if(!strcmp(id, "---")) {
	      expect = TICKET;
	    } else {
	      DBUG_PRINT("fileSelectedHook", ("id=%s", id));
	      
	      /* Enable item(s)... */
	      for(i=3; i<=numIds; ++i) {
		if(!strcmp(id, filterArray[i].onString+2)) {
		  TOGGLE_ENABLE(WI_ms_file_panel_filter, i);
		  filterArray[i].enabled = TRUE;
		  break;
		}
	      }
	      
	      /* Group? */
	      if(filterArray[i].onString[0] == 'G') {
		/* Group --> enable all items of this group... */
		group = filterArray[i].group;
		for(i=3; i<=numIds; ++i) {
		  if(group == filterArray[i].group) {
		    TOGGLE_ENABLE(WI_ms_file_panel_filter, i);
		    filterArray[i].enabled = TRUE;
		  }
		}
	      } else {
		/* No group --> enable group... */
		group = filterArray[i].group;
		for(i=3; i<=numIds; ++i) {
		  if((group == filterArray[i].group) &&
		     (filterArray[i].onString[0] == 'G')) {
		    TOGGLE_ENABLE(WI_ms_file_panel_filter, i);
		    filterArray[i].enabled = TRUE;
		    break;
		  }
		}
	      }
	    }
	    break;
	    
	  case TICKET:
	    fscanf(inF, "%s", header);
	    
	    if(!strcmp(header, "---")) {
	      fgetc(inF);               /* Skip LF-character in input stream */
	      expect = DATA;
	    }
	    break;
	    
	  case DATA:
	    startOfData = ftell(inF);
	    
	    /* Read in ticket stuff... */
	    {
	      short processor, id;
	      int process, numProcesses, heapBase, dummyLength;
	      
	      mscanfTime(inF, &timeOffset);
	      mscanfProcessor(inF, &processor);
	      mscanfProcess(inF, &process);
	      mscanfId(inF, &id);
	      mscanfInt(inF, &numProcesses);
	      mscanfInt(inF, &heapBase);
	      mscanfLength(inF, &dummyLength);
	      
	      DBUG_PRINT("fileSelectedHook", 
			 ("Tickets: %l.0f %d %d %d %d %d",
			  timeOffset, processor, process,
			  id, numProcesses, heapBase));
	      
	      if(timeOffset == 0.0) {
		if(id) {
		  dieHard("Illegal id in ticket initialization...");
		}
		
		numSlots[processor] = numProcesses;
		heapBases[processor] = heapBase;
		
		DBUG_PRINT("fileSelectedHook", 
			   ("Processor %d: %d process slots",
			    processor, numProcesses));
	      }
	    }
	    
	    break;
	  }
	  
	  if((expect == DATA) && (timeOffset > 0)) {
	    break;
	  }
	}

	getLastTimeStamp();

	DBUG_PRINT("fileSelectedHook", ("startOfData=%d", startOfData));
	DBUG_PRINT("fileSelectedHook",
		   ("timeOffset=%lf lastTimeStamp=%lf realLastTimeStamp=%lf",
		    timeOffset, lastTimeStamp, realLastTimeStamp));
      }
    } else {
      doRequest("", "No measurement file selected!", "", "OK", "", "", NULL);
    }

    break;

  case 1:                                      /* Script selected (for load) */
  case 3:                                      /* Script selected (for save) */
    SET_LABEL(WI_ms_script_panel_enter_name, file);
    SET_LABEL(WI_ms_script_panel_enter_path, path);
    strcpy(selectedScript, path);
    strcat(selectedScript, file);

    if((strlen(selectedScript) && 
	(!stat(selectedScript, &statBuffer) &&
	 (S_ISREG(statBuffer.st_mode) || S_ISLNK(statBuffer.st_mode)))) ||
       (which == 3)) {

      if(which == 3) {
	/* Save it... */
	XawAsciiSaveAsFile(XawTextGetSource(WI_editor_edit_window),
			   (String)selectedScript);
      }

      if(scriptIsActive) {
	scriptIsActive = scriptIsTerminated = FALSE;
	dlclose(dynamicHandle);
      }

      SET_VALUE(WI_editor_edit_window, XtNstring, selectedScript);
    } else {
      /*doRequest("", "No script selected!", "", "OK", "", "", NULL);*/
    }

    break;

  case 2:                                        /* Postscript file selected */
    strcpy(temp, path);
    strcat(temp, file);
    SAVE_PS(temp, file);
    break;
  }

  DBUG_PRINT("fileSelectedHook", ("selected: %s%s", path, file));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFilePanelEnterPath: (callback)                                         */
/*                                                                           */
/*  Enter path of file...                                                    */
/*                                                                           */
/*****************************************************************************/

void msFilePanelEnterPath(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msFilePanelEnterPath");

  GET_LABEL(WI_ms_file_panel_enter_path, path);
  GET_LABEL(WI_ms_file_panel_enter_name, file);
  
  fileSelectedHook(0, path, file);
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFilePanelEnterName: (callback)                                         */
/*                                                                           */
/*  Enter name of file...                                                    */
/*                                                                           */
/*****************************************************************************/

void msFilePanelEnterName(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msFilePanelEnterName");

  GET_LABEL(WI_ms_file_panel_enter_path, path);
  GET_LABEL(WI_ms_file_panel_enter_name, file);
  
  fileSelectedHook(0, path, file);
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFilePanelBrowse: (callback)                                            */
/*                                                                           */
/*  Select file...                                                           */
/*                                                                           */
/*****************************************************************************/

void msFilePanelBrowse(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msFilePanelBrowse");

  GET_LABEL(WI_ms_file_panel_enter_path, path);
  GET_LABEL(WI_ms_file_panel_enter_name, file);
  
  msFileBrowserInitialize(0, "Load measure file...", FILE_MASK,
			  path, file);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelEnterPath: (callback)                                       */
/*                                                                           */
/*  Enter path...                                                            */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelEnterPath(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msScriptPanelEnterPath");

  GET_LABEL(WI_ms_script_panel_enter_path, path);
  GET_LABEL(WI_ms_script_panel_enter_name, file);
  
  fileSelectedHook(1, path, file);
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelEnterName: (callback)                                       */
/*                                                                           */
/*  Enter name...                                                            */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelEnterName(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msScriptPanelEnterName");

  GET_LABEL(WI_ms_script_panel_enter_path, path);
  GET_LABEL(WI_ms_script_panel_enter_name, file);
  
  fileSelectedHook(1, path, file);
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelBrowse: (callback)                                          */
/*                                                                           */
/*  Select file...                                                           */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelBrowse(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[2*FILE_LEN], file[2*FILE_LEN];

  DBUG_ENTER("msScriptPanelBrowse");

  split(selectedScript, path, file);
  msFileBrowserInitialize(1, "Load Script...", SCRIPT_MASK, path, file);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelEdit: (callback)                                            */
/*                                                                           */
/*  Invoke editor for selected file...                                       */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelEdit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msScriptPanelEdit");

  SET_VALUE(WI_editor_edit_window, XtNstring, selectedScript);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelRun: (callback)                                             */
/*                                                                           */
/*  Start script (same as play in control panel)...                          */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelRun(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msScriptPanelRun");

  if(strlen(selectedScript)) {
    msControlPanelPlayForward();
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msScriptPanelSave: (callback)                                            */
/*                                                                           */
/*  Save (modified) script...                                                */
/*                                                                           */
/*****************************************************************************/

void msScriptPanelSave(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[2*FILE_LEN], file[2*FILE_LEN];

  DBUG_ENTER("msScriptPanelSave");

  split(selectedScript, path, file);
  msFileBrowserInitialize(3, "Save Script...", SCRIPT_MASK, path, file);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  exitHook: (callback)                                                     */
/*                                                                           */
/*  Keep your environment tidy...                                            */
/*                                                                           */
/*****************************************************************************/

void exitHook(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  struct Frame *temp;

  DBUG_ENTER("exitHook");

  if(!strlen(lastRequestAnswer)) {
    doRequest("", "Do you really want to exit this module?", "",
	      "Yes", "No", "", &exitHook);
  } else {
    if(!strcmp(lastRequestAnswer, "Yes")) {
      fclose(inF);

      /* Free stack... */
      while((temp = firstStackFrame)) {
	firstStackFrame = firstStackFrame->next;
	cfree((char *)temp);
      }

      /* Unlink script... */
      if(eval != NULL) {
	dlclose(dynamicHandle);
      }

      XFreeGC(display, gcDark);
      XFreeGC(display, gcLight);
      XFreeGC(display, gcProgressBlack);

      localExitHook();

      storePreferences();

      exit(0);
    }

    strcpy(lastRequestAnswer, "");
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  highlightButton: (local function)                                        */
/*                                                                           */
/*  Change background of widget to green, all other widgets become gray...   */
/*                                                                           */
/*****************************************************************************/

static void highlightButton(w)
Widget w;
{
  Pixel tempColor;

  DBUG_ENTER("hightlightButton");

  GET_BACKGROUND(WI_ms_control_panel_go_to_start, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_go_to_start, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_play_backward, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_play_backward, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_skip_backward, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_skip_backward, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_stop, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_stop, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_skip_forward, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_skip_forward, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_play_forward, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_play_forward, buttonReleasedColor);
  }

  GET_BACKGROUND(WI_ms_control_panel_go_to_end, &tempColor);
  if(tempColor != buttonDisabledColor) {
    SET_BACKGROUND(WI_ms_control_panel_go_to_end, buttonReleasedColor);
  }

  if(w) {
    SET_BACKGROUND(w, buttonPressedColor);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  disableButton: (local function)                                          */
/*                                                                           */
/*  Change background of widget to black...                                  */
/*                                                                           */
/*****************************************************************************/

static void disableButton(w)
Widget w;
{
  DBUG_ENTER("disableButton");

  if(w) {
    SET_BACKGROUND(w, buttonDisabledColor);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  enableButton: (local function)                                           */
/*                                                                           */
/*  Change background of widget to gray...                                   */
/*                                                                           */
/*****************************************************************************/

static void enableButton(w)
Widget w;
{
  DBUG_ENTER("enableButton");

  if(w) {
    SET_BACKGROUND(w, buttonReleasedColor);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFilterCall: (callback)                                                 */
/*                                                                           */
/*  Determine selected item in filter menu (de-)select item(s)...            */
/*                                                                           */
/*****************************************************************************/

void msFilterCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  short ret = *(short *)client_data,
        group = filterArray[ret].group,
        i;
  Boolean state = filterArray[ret].state;

  DBUG_ENTER("msFilterCall");

  /* Terminate script... */
  scriptIsActive = scriptIsTerminated = FALSE;

  if((ret > 2) && (ret != numIds-NUM_PROCESSORS)) {
    DBUG_PRINT("msFilterCall", ("ret=%d group=%d toggle=%d",
				ret, group, state));

    if(filterArray[ret].onString[0] == 'G') {
      /* Group */
      for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
	if((filterArray[i].group == group) && (filterArray[i].ret != ret)) {
	  filterArray[i].state = state;
	  UNBIND_TOGGLE(WI_ms_file_panel_filter, i);
	  BIND_TOGGLE(WI_ms_file_panel_filter, i,
		      filterArray[i].onString, filterArray[i].offString,
		      &(filterArray[i].state),
		      msFilterCall, &(filterArray[i].ret));
	  idsArray[filterArray[i].mIndex].selected = state;
	}
      }
    } else {
      idsArray[filterArray[ret].mIndex].selected = state;
    }
  } else {
    /* "Special" IDs */
    if(ret == 2) {
      /* Turn on/off all GROUPS-IDs... */

      for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
	filterArray[i].state = state;
	UNBIND_TOGGLE(WI_ms_file_panel_filter, i);
	BIND_TOGGLE(WI_ms_file_panel_filter, i,
		    filterArray[i].onString, filterArray[i].offString,
		    &(filterArray[i].state),
		    msFilterCall, &(filterArray[i].ret));
	idsArray[filterArray[i].mIndex].selected = state;
      }
    } else {
      /* Turn on/off all processors... */

      for(i=numIds-NUM_PROCESSORS+1; i<=numIds; ++i) {
	filterArray[i].state = state;
	UNBIND_TOGGLE(WI_ms_file_panel_filter, i);
	BIND_TOGGLE(WI_ms_file_panel_filter, i,
		    filterArray[i].onString, filterArray[i].offString,
		    &(filterArray[i].state),
		    msFilterCall, &(filterArray[i].ret));
	idsArray[filterArray[i].mIndex].selected = state;
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareIdMenus: (local function)                                         */
/*                                                                           */
/*  Make the items non-down-forcing and disable them...                      */
/*                                                                           */
/*****************************************************************************/

void prepareIdMenus()
{
  FILE *f;
  char id[IDS_LEN], error[ERROR_LEN], c;
  short ret = 1,
        actualGroup = 0,
        mIndex = 1;
  int i;
  Boolean expectGroup;

  DBUG_ENTER("prepareIdMenus");

  if((f = fopen("GROUPS", "r")) == NULL) {
    sprintf(error, "Can't open groups file 'GROUPS' for read...");
    dieHard(error);
  }

  numIds = numRegularIds = 1;

  /* All IDs! */
  ++numIds;
  ++ret;

  filterArray[ret].ret = ret;
  filterArray[ret].state = TRUE;
  strcpy(filterArray[ret].onString, "  All IDs!");
  strcpy(filterArray[ret].offString, filterArray[ret].onString);

  BIND_TOGGLE(WI_ms_file_panel_filter, ret,
	      filterArray[ret].onString, filterArray[ret].offString,
	      &(filterArray[ret].state),
	      msFilterCall, &(filterArray[ret].ret));
  UNBIND_FORCE_DOWN(WI_ms_file_panel_filter, ret);
  TOGGLE_ENABLE(WI_ms_file_panel_filter, ret);


  expectGroup = TRUE;

  while(!feof(f)) {
    if(expectGroup) {
      strcpy(id, "");
      fscanf(f, "%s", id);

      if(strlen(id) && (id[0] != '#')) {
	DBUG_PRINT("prepareIdMenus", ("group=%s", id));

	++ret;
	++numIds;
	++actualGroup;

	filterArray[ret].ret = ret;
	filterArray[ret].group = actualGroup;
	filterArray[ret].mIndex = 0;
	filterArray[ret].state = FALSE;
	filterArray[ret].enabled = FALSE;
	strcpy(filterArray[ret].onString, "G ");
	strcat(filterArray[ret].onString, id);
	strcpy(filterArray[ret].offString, filterArray[ret].onString);

	BIND_TOGGLE(WI_ms_file_panel_filter, ret,
		    filterArray[ret].onString, filterArray[ret].offString,
		    &(filterArray[ret].state),
		    msFilterCall, &(filterArray[ret].ret));
	UNBIND_FORCE_DOWN(WI_ms_file_panel_filter, ret);
	TOGGLE_DISABLE(WI_ms_file_panel_filter, ret);
      }
	
      while((fgetc(f) != '\n') && !feof(f))
	;

      expectGroup = FALSE;
    } else {
      c = fgetc(f);
      if(!isLF(c)) {
        ungetc(c, f);
	
        if(!isBlank(c)) {
          expectGroup = TRUE;
        } else {
          strcpy(id, "");
          fscanf(f, "%s", id);

          if(strlen(id)) {
            DBUG_PRINT("prepareIdMenus", ("\tid=%s", id));

	    ++ret;
	    ++numIds;
	    ++numRegularIds;

	    filterArray[ret].ret = ret;
	    filterArray[ret].group = actualGroup;
	    filterArray[ret].mIndex = mIndex;
	    filterArray[ret].state = FALSE;
	    filterArray[ret].enabled = FALSE;
	    strcpy(filterArray[ret].onString, "+ ");
	    strcat(filterArray[ret].onString, id);
	    strcpy(filterArray[ret].offString, "- ");
	    strcat(filterArray[ret].offString, id);

	    ++mIndex;

	    BIND_TOGGLE(WI_ms_file_panel_filter, ret,
			filterArray[ret].onString, filterArray[ret].offString,
			&(filterArray[ret].state),
			msFilterCall, &(filterArray[ret].ret));
	    UNBIND_FORCE_DOWN(WI_ms_file_panel_filter, ret);
	    TOGGLE_DISABLE(WI_ms_file_panel_filter, ret);
          }

          /* skip rest of line */
          while((fgetc(f) != '\n') && !feof(f))
            ;
        }
      }
    }
  }

  fclose(f);

  /* Processor entries... */

  ++numIds;
  ++ret;

  /* All Processors! */
  filterArray[ret].ret = ret;
  filterArray[ret].state = TRUE;
  strcpy(filterArray[ret].onString, "  All Processors!");
  strcpy(filterArray[ret].offString, "  All Processors!");

  BIND_TOGGLE(WI_ms_file_panel_filter, ret,
	      filterArray[ret].onString, filterArray[ret].offString,
	      &(filterArray[ret].state),
	      msFilterCall, &(filterArray[ret].ret));
  UNBIND_FORCE_DOWN(WI_ms_file_panel_filter, ret);
  TOGGLE_ENABLE(WI_ms_file_panel_filter, ret);


  for(i=0; i<NUM_PROCESSORS; ++i) {
    sprintf(id, "Processor %d", i);

    DBUG_PRINT("prepareIdMenus", ("\tid=%s", id));

    ++ret;
    ++numIds;

    filterArray[ret].ret = ret;
    filterArray[ret].group = 0;
    filterArray[ret].mIndex = mIndex;
    filterArray[ret].state = FALSE;
    filterArray[ret].enabled = TRUE;
    strcpy(filterArray[ret].onString, "+ ");
    strcat(filterArray[ret].onString, id);
    strcpy(filterArray[ret].offString, "- ");
    strcat(filterArray[ret].offString, id);

    ++mIndex;

    BIND_TOGGLE(WI_ms_file_panel_filter, ret,
		filterArray[ret].onString, filterArray[ret].offString,
		&(filterArray[ret].state),
		msFilterCall, &(filterArray[ret].ret));
    UNBIND_FORCE_DOWN(WI_ms_file_panel_filter, ret);
    TOGGLE_ENABLE(WI_ms_file_panel_filter, ret);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  doTick: (exported function)                                              */
/*                                                                           */
/*  Catch timer events...                                                    */
/*                                                                           */
/*****************************************************************************/

void doTick()
{
  /*DBUG_ENTER("doTick");*/ /* No debugging here... It's too much! */

  /*DBUG_PRINT("doTick", ("lastAction=%d nextAction=%d",
                         lastAction, nextAction));*/

  if(!localDoTickGuard(nextAction)) {
    /* This action is disabled... */
    nextAction = NOACTION;
  }

  if(doLocalDoTick) {
    doLocalDoTick = FALSE;
    localDoTick();
  }

  switch(nextAction) {
  case GOTOSTART:
    if(lastAction != nextAction) {
      highlightButton(WI_ms_control_panel_go_to_start);
      lastAction = nextAction;
    }

    if(scriptIsActive) {
      if(reachedLowerBoundary) {
	nextAction = STOP;
      } else {
	doUpdates = FALSE;
	gotoFrame(actualFrame-actualRate);

	if(reachedLowerBoundary) {
	  nextAction = STOP;
	}
      }
    } else {
      nextAction = STOP;
    }

    break;

  case PLAYBACKWARD:
    if((lastAction == GOTOSTART) || (lastAction == GOTOEND)) {
      doUpdates = TRUE;
      updateData();
    }

    if(lastAction != nextAction) {
      highlightButton(WI_ms_control_panel_play_backward);
      lastAction = nextAction;
    }

    if(scriptIsActive) {
      if(reachedLowerBoundary) {
	nextAction = STOP;
      } else {
	doUpdates = (actualRate==1);
	gotoFrame(actualFrame-actualRate);
	frameCompleted = FALSE;

	if(actualRate > 1) {
	  doUpdates = TRUE;
	  updateData();
	}
      }
    } else {
      nextAction = STOP;
    }

    break;

  case STEPBACKWARD:
    if((lastAction == GOTOSTART) || (lastAction == GOTOEND)) {
      doUpdates = TRUE;
      updateData();
    }

    highlightButton(WI_ms_control_panel_skip_backward);
    lastAction = nextAction;
    nextAction = STOP;

    if(scriptIsActive) {
      if(!reachedLowerBoundary) {
	doUpdates = TRUE;
	gotoFrame(actualFrame-1);
      }
    }

    break;

  case STOP:
    if(lastAction != nextAction) {
      highlightButton(WI_ms_control_panel_stop);
    }
    
    if(!doUpdates) {
      doUpdates = TRUE;
      updateData();
    }
    updateTime(actualTime);
    updateFrame(actualFrame-1);
    updateLine(actualLine);

    lastAction = nextAction;
    nextAction = NOACTION;

    doLocalDoTick = TRUE;

    break;

  case STEPFORWARD:
    if((lastAction == GOTOSTART) || (lastAction == GOTOEND)) {
      doUpdates = TRUE;
      updateData();
    }

    highlightButton(WI_ms_control_panel_skip_forward);
    lastAction = nextAction;
    nextAction = STOP;

    if(!scriptIsActive) {
      startModule();
    }

    if(scriptIsActive) {
      if(!reachedUpperBoundary) {
	doUpdates = TRUE;
	gotoFrame(actualFrame+1);
	frameCompleted = FALSE;
      }
    }

    break;

  case PLAYFORWARD:
    if((lastAction == GOTOSTART) || (lastAction == GOTOEND)) {
      doUpdates = TRUE;
      updateData();
    }

    if(lastAction != nextAction) {
      highlightButton(WI_ms_control_panel_play_forward);
      lastAction = nextAction;
    }

    if(!scriptIsActive) {
      startModule();
    }

    if(scriptIsActive) {
      if(reachedUpperBoundary) {
	nextAction = STOP;
      } else {
	doUpdates = (actualRate==1);
	gotoFrame(actualFrame+actualRate);
	frameCompleted = FALSE;
	
	if(reachedUpperBoundary) {
	  nextAction = STOP;
	} else {
	  if(actualRate > 1) {
	    doUpdates = TRUE;
	    updateData();
	  }
	}
      }
    } else {
      nextAction = STOP;
    }

    break;

  case GOTOEND:
    if(lastAction != nextAction) {
      highlightButton(WI_ms_control_panel_go_to_end);
      lastAction = nextAction;
    }

    if(!scriptIsActive) {
      startModule();
    }

    if(scriptIsActive) {
      if(reachedUpperBoundary) {
	nextAction = STOP;
      } else {
	doUpdates = FALSE;
	gotoFrame(actualFrame+actualRate);
	frameCompleted = FALSE;
	
	if(reachedUpperBoundary) {
	  nextAction = STOP;
	}
      }
    } else {
      nextAction = STOP;
    }

    break;

  default:
    /* no action pending */

    break;
  }

  /*DBUG_VOID_RETURN;*/

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelEnterDelay: (callback)                                       */
/*                                                                           */
/*  Enter value for delay...                                                 */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelEnterDelay(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];
  int tempDelay;
  
  DBUG_ENTER("msSpeedPanelEnterDelay");

  GET_LABEL(WI_ms_speed_panel_enter_delay, temp);
  tempDelay = atoi(temp);

  if((tempDelay >= MIN_DELAY) && (tempDelay <= MAX_DELAY)) {
    actualDelay = tempDelay;
    SET_POSITION(WI_ms_speed_panel_scroll_delay, actualDelay);
  } else {
    sprintf(temp, "%d", actualDelay);
    SET_LABEL(WI_ms_speed_panel_enter_delay, temp);
  }


  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelDecreaseDelay: (callback)                                    */
/*                                                                           */
/*  Decrease value for delay...                                              */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelDecreaseDelay(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelDecreaseDelay");

  if(actualDelay > MIN_DELAY) {
    sprintf(temp, "%d", --actualDelay);
    SET_LABEL(WI_ms_speed_panel_enter_delay, temp);
    SET_POSITION(WI_ms_speed_panel_scroll_delay, actualDelay);
  } 

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelIncreaseDelay: (callback)                                    */
/*                                                                           */
/*  Increase value for delay...                                              */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelIncreaseDelay(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelIncreaseDelay");

  if(actualDelay < MAX_DELAY) {
    sprintf(temp, "%d", ++actualDelay);
    SET_LABEL(WI_ms_speed_panel_enter_delay, temp);
    SET_POSITION(WI_ms_speed_panel_scroll_delay, actualDelay);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelSetDelay: (callback)                                         */
/*                                                                           */
/*  Set delay time between two lines...                                      */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelSetDelay(w, which, position)
Widget w;
XtPointer *which;
float *position;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelSetDelay");

  sprintf(temp, "%d", actualDelay = *position*(MAX_DELAY-MIN_DELAY)+MIN_DELAY);
  SET_LABEL(WI_ms_speed_panel_enter_delay, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelEnterRate: (callback)                                        */
/*                                                                           */
/*  Enter value for Rate...                                                  */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelEnterRate(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];
  int tempRate;

  DBUG_ENTER("msSpeedPanelEnterRate");

  GET_LABEL(WI_ms_speed_panel_enter_rate, temp);
  tempRate = atoi(temp);

  if((tempRate >= MIN_RATE) && (tempRate <= MAX_RATE)) {
    actualRate = tempRate;
    SET_POSITION(WI_ms_speed_panel_scroll_rate, actualRate);
  } else {
    sprintf(temp, "%d", actualRate);
    SET_LABEL(WI_ms_speed_panel_enter_rate, temp);
  }


  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelDecreaseRate: (callback)                                     */
/*                                                                           */
/*  Decrease value for Rate...                                               */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelDecreaseRate(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelDecreaseRate");

  if(actualRate > MIN_RATE) {
    sprintf(temp, "%d", --actualRate);
    SET_LABEL(WI_ms_speed_panel_enter_rate, temp);
    SET_POSITION(WI_ms_speed_panel_scroll_rate, actualRate);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelIncreaseRate: (callback)                                     */
/*                                                                           */
/*  Increase value for Rate...                                               */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelIncreaseRate(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelIncreaseRate");

  if(actualRate < MAX_RATE) {
    sprintf(temp, "%d", ++actualRate);
    SET_LABEL(WI_ms_speed_panel_enter_rate, temp);
    SET_POSITION(WI_ms_speed_panel_scroll_rate, actualRate);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msSpeedPanelSetRate: (callback)                                          */
/*                                                                           */
/*  Set hidden/shown frames ratio...                                         */
/*                                                                           */
/*****************************************************************************/

void msSpeedPanelSetRate(w, which, position)
Widget w;
XtPointer *which;
float *position;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msSpeedPanelSetRate");

  sprintf(temp, "%d", actualRate = *position*(MAX_RATE-MIN_RATE)+MIN_RATE);
  SET_LABEL(WI_ms_speed_panel_enter_rate, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msPositionPanelEnterFrame: (callback)                                    */
/*                                                                           */
/*  Enter frame to jump to...                                                */
/*                                                                           */
/*****************************************************************************/

void msPositionPanelEnterFrame(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msPositionPanelEnterFrame");

  GET_LABEL(WI_ms_position_panel_enter_frame, temp);

  if(!scriptIsActive) {
    startModule();
  }

  if(scriptIsActive) {
    doUpdates = FALSE;
    gotoFrame(atoi(temp)+1);

    doUpdates = TRUE;
    updateLine(actualLine);
    updateFrame(actualFrame-1);
    updateTime(actualTime);
    updateData();
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msPositionPanelEnterTime: (callback)                                     */
/*                                                                           */
/*  Enter timestamp to jump to...                                            */
/*                                                                           */
/*****************************************************************************/

void msPositionPanelEnterTime(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msPositionPanelEnterTime");

  GET_LABEL(WI_ms_position_panel_enter_time, temp);

  if(!scriptIsActive) {
    startModule();
  }

  if(scriptIsActive) {
    doUpdates = FALSE;
    gotoTime(atof(temp));

    doUpdates = TRUE;
    updateLine(actualLine);
    updateFrame(actualFrame-1);
    updateTime(actualTime);
    updateData();
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  initColors: (local function)                                             */
/*                                                                           */
/*  Do some more initialization... (called my 'startupHook')                 */
/*                                                                           */
/*****************************************************************************/

void initColors()
{
  unsigned long valuemask;
  XGCValues values;

  DBUG_ENTER("initColors");

  progressWindow = XtWindow(WI_ms_progress_bar);

  if(gcDark == (GC)NULL) { 
    valuemask = GCFunction | GCForeground;
    values.function = GXcopy;
    values.foreground = buttonPressedColor;
    gcDark = XCreateGC(display, progressWindow, valuemask, &values);
  }

  if(gcLight == (GC)NULL) {
    valuemask = GCFunction | GCForeground;
    values.function = GXcopy;
    values.foreground = buttonReleasedColor;
    gcLight = XCreateGC(display, progressWindow, valuemask, &values);
  }

  if(gcProgressBlack == (GC)NULL) { 
    valuemask = GCFunction | GCForeground;
    values.function = GXcopy;
    values.foreground = BlackPixel(display, screen);
    gcProgressBlack = XCreateGC(display, progressWindow, valuemask, &values);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  startupHook: (exported function)                                         */
/*                                                                           */
/*  Do some initialization... (called my 'main')                             */
/*                                                                           */
/*****************************************************************************/

void startupHook()
{
  char id[IDS_LEN], path[FILE_LEN], file[FILE_LEN];
  int i, j, group;

  DBUG_ENTER("startupHook");

  /* Netrek trick... */
  (void)signal(SIGCHLD, reaper);

  fscanf(stdin, "%s\n", selectedFile);
  DBUG_PRINT("startupHook", ("selected: %s", selectedFile));

  firstStackFrame = NULL;
  numStackFramesAllocated = 0;

  prepareIdMenus();
  split(selectedFile, path, file);
  fileSelectedHook(0, path, file);

  /* Scan the selected IDs and save them in an array... */
  numGroups = 0;

  for(i=0; i<MAX_IDS; ++i) {
    idsArray[i].selected = FALSE;
  }

  while(!feof(stdin)) {
    if((fscanf(stdin, "%s", id) > 0) && strlen(id)) {
      if(id[strlen(id)-1] == ':') {
	id[strlen(id)-1] = '\0';

	DBUG_PRINT("startupHook", ("group=%s", id));

	++numGroups;
	strcpy(groupsArray[numGroups].group, id);
      } else {
	i = atoi(id);
	if(i < numRegularIds) {
	  /* "Normal" ids... */
	  DBUG_PRINT("startupHook", ("id=%d (%s)", i, ids[i]));
	  idsArray[i].selected = TRUE;
	  idsArray[i].bar = 0;
	  idsArray[i].group = numGroups;

	  /* '+'-sify item(s)... */
	  for(j=2; j<=numIds-NUM_PROCESSORS; ++j) {
	    if(!strcmp(ids[i], filterArray[j].onString+2)) {
	      filterArray[j].state = TRUE;
	      UNBIND_TOGGLE(WI_ms_file_panel_filter, j);
	      BIND_TOGGLE(WI_ms_file_panel_filter, j,
			  filterArray[j].onString, filterArray[j].offString,
			  &(filterArray[j].state),
			  msFilterCall, &(filterArray[j].ret));
	      break;
	    }
	  }

	  group = filterArray[j].group;
	  for(j=2; j<=numIds-NUM_PROCESSORS; ++j) {
	    if((group == filterArray[j].group) &&
	       (filterArray[j].onString[0] == 'G')) {
	      filterArray[j].state = TRUE;
	      UNBIND_TOGGLE(WI_ms_file_panel_filter, j);
	      BIND_TOGGLE(WI_ms_file_panel_filter, j,
			  filterArray[j].onString, filterArray[j].offString,
			  &(filterArray[j].state),
			  msFilterCall, &(filterArray[j].ret));
	      break;
	    }
	  }
	} else {
	  /* Processor ids... */
	  DBUG_PRINT("startupHook", ("id=%d (Processor %d)",
				     i, i-numRegularIds));
	  idsArray[i].selected = TRUE;
	  idsArray[i].bar = 0;
	  idsArray[i].group = 0;

	  /* '+'-sify item(s)... */
	  for(j=numRegularIds; j<=numIds; ++j) {
	    sprintf(id, "Processor %d", i-numRegularIds);
	    if(!strcmp(id, filterArray[j].onString+2)) {
	      filterArray[j].state = TRUE;
	      UNBIND_TOGGLE(WI_ms_file_panel_filter, j);
	      BIND_TOGGLE(WI_ms_file_panel_filter, j,
			  filterArray[j].onString, filterArray[j].offString,
			  &(filterArray[j].state),
			  msFilterCall, &(filterArray[j].ret));
	      break;
	    }
	  }
	}
      }
    }
  }

  DBUG_PRINT("startupHook", ("numGroups=%d", numGroups));

  /* Do some further initialization... */
  scriptIsActive = scriptIsTerminated = FALSE;
  lastAction = nextAction = NOACTION;

  display = GET_DISPLAY(WI_ms_control_panel_play_forward);
  screen = GET_SCREEN(display);
  colorMap = GET_COLORMAP(display, screen);

  updateDelay(actualDelay = DEFAULT_DELAY);
  updateRate(actualRate = DEFAULT_RATE);

  for(i=1; i<=colors[0].red; ++i) {
    xColor.red = colors[i].red;
    xColor.green = colors[i].green;
    xColor.blue = colors[i].blue;
    xColor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, colorMap, &xColor);
    colors[i].pixel = xColor.pixel;
  }

  if(getenv("MS_BLACKWHITE")) {
    /* Run in black-and-white-mode... */
    buttonDisabledColor = buttonReleasedColor = WhitePixel(display, screen);
    buttonPressedColor = BlackPixel(display, screen);
  } else {
    /* Run in color-mode.. */
    xColor.red = 0xbb00;
    xColor.green = 0xbb00;
    xColor.blue = 0xbb00;
    xColor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, colorMap, &xColor);
    buttonReleasedColor = xColor.pixel;

    xColor.red = 0x0000;
    xColor.green = 0xdd00;
    xColor.blue = 0xdd00;
    xColor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, colorMap, &xColor);
    buttonPressedColor = xColor.pixel;

    buttonDisabledColor = colors[LIGHT_RED].pixel;
  }

  initColors();
  getNewSize();

  doLocalDoTick = FALSE;

  /* Do the module-specific stuff... */
  localStartupHook();

  fetchPreferences();

  split(selectedScript, path, file);
  fileSelectedHook(1, path, file);

  disableButton(NULL);
  enableButton(NULL);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  The following functions are the graphics primitives...                   */
/*                                                                           */
/*****************************************************************************/


void FLUSH()                                             /* Update X display */
{
  DBUG_ENTER("FLUSH");

  XSync(display, FALSE);

  DBUG_VOID_RETURN;
}


void HEADER_FILE(s)                           /* Get file string from header */
char *s;
{
  DBUG_ENTER("HEADER_FILE");

  strcpy(s, headerFile);

  DBUG_VOID_RETURN;
}


void HEADER_TIME(s)                           /* Get time string from header */
char *s;
{
  DBUG_ENTER("HEADER_TIME");

  strcpy(s, headerTime);

  DBUG_VOID_RETURN;
}


void HEADER_DIMENSION(s)                 /* Get dimension string from header */
char *s;
{
  DBUG_ENTER("HEADER_DIMENSION");

  strcpy(s, headerDimension);

  DBUG_VOID_RETURN;
}


void HEADER_STEPS(s)                         /* Get steps string from header */
char *s;
{
  DBUG_ENTER("HEADER_STEPS");

  strcpy(s, headerSteps);

  DBUG_VOID_RETURN;
}


void HEADER_NODISTNUM(s)                 /* Get nodistnum string from header */
char *s;
{
  DBUG_ENTER("HEADER_NODISTNUM");

  strcpy(s, headerNoDistNum);

  DBUG_VOID_RETURN;
}


void HEADER_FACTOR(s)                       /* Get factor string from header */
char *s;
{
  DBUG_ENTER("HEADER_FACTOR");

  strcpy(s, headerFactor);

  DBUG_VOID_RETURN;
}


void HEADER_BINARY(s)                       /* Get binary string from header */
char *s;
{
  DBUG_ENTER("HEADER_BINARY");

  strcpy(s, headerBinary);

  DBUG_VOID_RETURN;
}


void GET_TIME(s, e)                  /* Get start- and end-time of reduction */
double *s, *e;
{
  DBUG_ENTER("GET_TIME");

  *s = 0.0;
  *e = lastTimeStamp;

  DBUG_VOID_RETURN;
}


void HEADER_HEAPSIZE(s)                   /* Get heapsize string from header */
char *s;
{
  DBUG_ENTER("HEADER_HEAPSIZE");

  strcpy(s, headerHeapsize);

  DBUG_VOID_RETURN;
}


void HEADER_HEAPDES(s)                     /* Get heapdes string from header */
char *s;
{
  DBUG_ENTER("HEADER_HEAPDES");

  strcpy(s, headerHeapdes);

  DBUG_VOID_RETURN;
}


void SET_DELAY(i)  /* Set animation speed (minimal delay between two frames) */
int i;
{
  DBUG_ENTER("SET_DELAY");

  updateDelay(actualDelay = i);

  DBUG_VOID_RETURN;
}


void SET_RATE(i)              /* Set frame rate (show only every n'th frame) */
int i;
{
  DBUG_ENTER("SET_RATE");

  updateRate(actualRate = i);

  DBUG_VOID_RETURN;
}
