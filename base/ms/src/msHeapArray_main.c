#include "msHeapArray_main.h"


/* Screen coordinates... */
static int xSize, ySize;                            /* Size of drawable area */
static int usableXSize, usableYSize;                       /* Size - Margins */
static int leftMargin = LEFT_MARGIN,
           rightMargin = RIGHT_MARGIN,
           topMargin = TOP_MARGIN,
           bottomMargin = BOTTOM_MARGIN;


/* World coordinates... */
static double xMin, yMin, xMax, yMax, xRange, yRange;          /* Boundaries */
static double xSlotSize, ySlotSize, xSlotOffset, ySlotOffset;  /* Slot sizes */

static double xFactor, yFactor;                       /* Screen/World ratios */

static Window window;
static GC gcBlack = (GC)NULL,
          gcWhite = (GC)NULL,
          gcHeapblock = (GC)NULL,
          gcDescriptor = (GC)NULL,
          gcFree = (GC)NULL;
static int heapblockColor, descriptorColor, freeColor;

static Boolean mustRedraw, cachingIsActive, stackingIsActive,
               fragmentationIsActive, separationIsActive;
static char actualTitle[TITLE_LEN];

static struct Memory *memory = NULL;


/* Graphics primitives */
void SET_COLORS();                             /* Set colors for heap-states */
void SET_TITLE();                                    /* Set title of graphic */
void NEXT_FRAME();                        /* End actual frame, begin new one */
void NEXT_FRAME_BUT_CONTINUE();        /* Continue with the CONTINUE-case in */
                                      /* the script after doing NEXT_FRAME() */
void FILL_BLOCK();             /* Fill specified slot according to its state */
void FILL_ALL();                                           /* Fill all slots */
void SET_DIMENSIONS();                                  /* Set numX and numY */
int GET_HEAPBASE();         /* Return base address of heap on specified node */
void DRAW_FRAGMENTATION();                        /* Draw fragmentation line */
void SET_CACHING();                                 /* Toggle caching on/off */
void SET_STACKING();                               /* Toggle stacking on/off */
void SET_SEPARATION();                     /* Toggle gap between rows on/off */
void SET_FRAGMENTATION();       /* Toggle drawing of block boundaries on/off */
void SAVE();                                /* Save actual plot to file (ps) */
void PRINT();                                      /* Print actual plot (ps) */
void CLEAR();                                           /* Clear actual plot */


void updateData();
void recallCommand();
int data2Frame();
double data2Time();
void localStartupHook();
void localExitHook();
void localStartModule();
void redrawData();
void localDoTick();
Boolean localDoTickGuard();
void generatePostscript();


#include "msModule.c"

static int numX, numY;


/*****************************************************************************/
/*                                                                           */
/*  xTransform: (local function)                                             */
/*                                                                           */
/*  Transform real to screen coordinates (x)                                 */
/*                                                                           */
/*****************************************************************************/

int xTransform(real)
double real;
{
  DBUG_ENTER("xTransform");

  DBUG_RETURN((int)((real-xMin)*xFactor)+leftMargin);
}


/*****************************************************************************/
/*                                                                           */
/*  yTransform: (local function)                                             */
/*                                                                           */
/*  Transform real to screen coordinates (y)                                 */
/*                                                                           */
/*****************************************************************************/

int yTransform(real)
double real;
{
  DBUG_ENTER("yTransform");

  DBUG_RETURN((int)((real-yMin)*yFactor)+topMargin);
}


/*****************************************************************************/
/*                                                                           */
/*  xLeft: (local function)                                                  */
/*                                                                           */
/*  Get left x coordinate for a slot...                                      */
/*                                                                           */
/*****************************************************************************/

double xLeft(process)
int process;
{
  DBUG_ENTER("xLeft");

  DBUG_RETURN(process*xRange/numX+xSlotOffset);
}


/*****************************************************************************/
/*                                                                           */
/*  xRight: (local function)                                                 */
/*                                                                           */
/*  Get right x coordinate for a slot...                                     */
/*                                                                           */
/*****************************************************************************/

double xRight(process)
int process;
{
  DBUG_ENTER("xRight");

  DBUG_RETURN(process*xRange/numX+xSlotOffset+xSlotSize);
}


/*****************************************************************************/
/*                                                                           */
/*  yTop: (local function)                                                   */
/*                                                                           */
/*  Get upper y coordinate for a slot...                                     */
/*                                                                           */
/*****************************************************************************/

double yTop(processor)
int processor;
{
  DBUG_ENTER("yTop");

  DBUG_RETURN(processor*yRange/numY+ySlotOffset);
}


/*****************************************************************************/
/*                                                                           */
/*  yBottom: (local function)                                                */
/*                                                                           */
/*  Get lower y coordinate for a slot...                                     */
/*                                                                           */
/*****************************************************************************/

double yBottom(processor)
int processor;
{
  DBUG_ENTER("yBottom");

  DBUG_RETURN(processor*yRange/numY+ySlotOffset+ySlotSize);
}


/*****************************************************************************/
/*                                                                           */
/*  generatePostscript: (local function)                                     */
/*                                                                           */
/*  Make postscript file from actual graphic...                              */
/*                                                                           */
/*****************************************************************************/

void generatePostscript(file, name)
char file[], name[];
{
  FILE *psFile;
  int y, x;
  int rememberXFrom, rememberXTo, rememberYFrom, rememberYTo, rememberState;

  DBUG_ENTER("generatePostscript");

  /* Open file... */
  if((psFile = fopen(file, "w")) == NULL) {
    doRequest("", "Save failed!", "", "OK", "", "", NULL);
  } else {
    generatePostscriptHeader(psFile, name,
			     (int)(xSize*EPS_X_FACTOR),
			     (int)(ySize*EPS_Y_FACTOR));

    fprintf(psFile, "gsave %f %f scale\n", EPS_X_FACTOR, EPS_Y_FACTOR);

    if(strlen(actualTitle)) {
      topMargin = TOP_MARGIN+20;

      fprintf(psFile, "%.2f setgray\n", PS_TEXT_COLOR);
      fprintf(psFile, "/Helvetica findfont [12 0 0 12 0 0] makefont setfont ");
      fprintf(psFile, "%d %d moveto (%s) ccshow\n",
	      (int)(xSize/2), ySize - 18, actualTitle);
    } else {
      topMargin = TOP_MARGIN;
    }

    /* Now draw the segments... */
    if(memory != NULL) {
      rememberXFrom = rememberXTo = 
      rememberYFrom = rememberYTo = rememberState = -1;

      for(y=0; y<numY; ++y) {
	for(x=0; x<numX; ++x) {
	  int xFrom = xTransform(xLeft(x));
	  int xTo = xTransform(xRight(x));
	  int yFrom = ySize - yTransform(yTop(y));
	  int yTo = ySize - yTransform(yBottom(y));
	  int actualState = memory[y*numX+x].state;

	  if(separationIsActive) {
	    --yFrom;
	  }

	  if((rememberState != actualState) ||               /* change color */
	     (rememberYFrom != yFrom)) {                         /* new line */

	    if(rememberState != -1) {
	      fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		      rememberXFrom, rememberYFrom,
		      rememberXFrom, rememberYTo,
		      rememberXTo, rememberYTo);
	      fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		      rememberXTo, rememberYFrom,
		      rememberXFrom, rememberYFrom);
	    }

	    switch(actualState) {
	    case FREE:
	      fprintf(psFile, "%.2f setgray\n", PS_FREE_COLOR);
	      break;
	    case HEAPBLOCK:
	      fprintf(psFile, "%.2f setgray\n", PS_HEAPBLOCK_COLOR);
	      break;
	    case DESCRIPTOR:
	      fprintf(psFile, "%.2f setgray\n", PS_DESCRIPTOR_COLOR);
	      break;	    
	    }

	    rememberState = actualState;
	    rememberXFrom = xFrom;

	    if(rememberYFrom != yFrom) {
	      rememberYFrom = yFrom;
	      rememberYTo = yTo;
	    }
	  }

	  rememberXTo = xTo;
	}
      }

      if(fragmentationIsActive) {
	fprintf(psFile, "%.2f setgray\n", PS_BORDER_COLOR);

	for(y=0; y<numY; ++y) {
	  for(x=0; x<numX; ++x) {
	    if(memory[y*numX+x].separation) {
	      int xFrom = xTransform(xLeft(x));
	      int xTo = xFrom;
	      int yFrom = ySize - yTransform(yTop(y));
	      int yTo = ySize - yTransform(yTop(y+1))-1;

	      if(separationIsActive) {
		--yFrom;
	      }

	      fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
		      xFrom, yFrom, xTo, yTo);
	    }
	  }
	}
      }
       
      /* Now the border... */
      {
	int xFrom = xTransform(xLeft(0));
	int xTo = xTransform(xRight(numX-1));
	int yFrom = ySize - yTransform(yTop(0));
	int yTo = ySize - yTransform(yTop(numY))-1;
      
	fprintf(psFile, "%.2f setgray\n", PS_BORDER_COLOR);
      
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom, yFrom, xFrom, yTo, xTo, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto stroke\n",
		xTo, yFrom, xFrom, yFrom);
      }
    }

    fprintf(psFile, "grestore\n");
    fprintf(psFile, "showpage\n");
    fclose(psFile);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  drawLine: (local function)                                               */
/*                                                                           */
/*  Draw a line...                                                           */
/*                                                                           */
/*****************************************************************************/

void drawLine(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
  int xFrom, yFrom, xTo, yTo;

  DBUG_ENTER("drawLine");

  if(mustRedraw &&
     ((lastAction == PLAYBACKWARD) || (lastAction == STEPBACKWARD) ||
      (lastAction == PLAYFORWARD) || (lastAction == STEPFORWARD))) {
    redrawData();
  }

  if(doUpdates) {
    xFrom = xTransform(xLeft(x1));
    xTo = xTransform(xLeft(x2));
    yFrom = yTransform(yTop(y1));
    yTo = yTransform(yTop(y2))-1;

    if(separationIsActive) {
      ++yFrom;
    }

    XDrawLine(display, window, gcBlack, xFrom, yFrom, xTo, yTo);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  fillAll: (local function)                                                */
/*                                                                           */
/*  Fill all slots (with destroyMemory == TRUE for initial use only)...      */
/*                                                                           */
/*****************************************************************************/

void fillAll(state, destroyMemory)
short state;
Boolean destroyMemory;
{
  int xFrom, yFrom, xTo, yTo, x, y;

  DBUG_ENTER("fillAll");

  if(destroyMemory && cachingIsActive) {
    for(y=0; y<numY; ++y) {
      for(x=0; x<numX; ++x) {
	memory[y*numX+x].state = state;
	memory[y*numX+x].separation = FALSE;
      }
    }
  }

  if(doUpdates) {
    xFrom = xTransform(xLeft(0));
    xTo = xTransform(xRight(numX-1));
    yFrom = yTransform(yTop(0));
    yTo = yTransform(yBottom(numY-1));

    switch(state) {
    case HEAPBLOCK:
      XFillRectangle(display, window, gcHeapblock,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case DESCRIPTOR:
      XFillRectangle(display, window, gcDescriptor,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case FREE:
      XFillRectangle(display, window, gcFree,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  fillSlot: (local function)                                               */
/*                                                                           */
/*  Draw slot...                                                             */
/*                                                                           */
/*****************************************************************************/

void fillSlot(y, x, state)
int y, x, state;
{
  int xFrom, yFrom, xTo, yTo;

  DBUG_ENTER("fillSlot");

  if(mustRedraw &&
     ((lastAction == PLAYBACKWARD) || (lastAction == STEPBACKWARD) ||
      (lastAction == PLAYFORWARD) || (lastAction == STEPFORWARD))) {
    redrawData();
  }

  if(doUpdates) {
    xFrom = xTransform(xLeft(x));
    xTo = xTransform(xRight(x));
    yFrom = yTransform(yTop(y));
    yTo = yTransform(yBottom(y));

    if(separationIsActive) {
      ++yFrom;
    }

    DBUG_PRINT("fillSlot", 
	       ("xFrom=%d yFrom=%d xTo=%d yTo=%d", xFrom, yFrom, xTo, yTo));

    switch(state) {
    case HEAPBLOCK:
      XFillRectangle(display, window, gcHeapblock,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case DESCRIPTOR:
      XFillRectangle(display, window, gcDescriptor,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case FREE:
      XFillRectangle(display, window, gcFree,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  rescaleGraphic: (local function)                                         */
/*                                                                           */
/*  Calculate new xFactor and yFactor...                                     */
/*                                                                           */
/*****************************************************************************/

void rescaleGraphic()
{
  Dimension width, height;

  DBUG_ENTER("rescaleGraphic");

  GET_SIZE(WI_ms_heap_array_graphic_area, &width, &height);

  xRange = xMax - xMin;
  yRange = yMax - yMin;
  DBUG_PRINT("rescaleGraphic", ("xRange=%lf yRange=%lf", xRange, yRange));

  xSize = width;
  ySize = height;
  DBUG_PRINT("rescaleGraphic", ("xSize=%d ySize=%d", xSize, ySize));

  usableXSize = xSize - (leftMargin + rightMargin);
  usableYSize = ySize - (topMargin + bottomMargin);
  DBUG_PRINT("rescaleGraphic", ("usableXSize=%d usableYSize=%d",
				usableXSize, usableYSize));

  xFactor = ((double)usableXSize)/xRange;
  yFactor = ((double)usableYSize)/yRange;
  DBUG_PRINT("rescaleGraphic", ("xFactor=%lf yFactor=%lf", xFactor, yFactor));

  xSlotSize = (xRange/numX);
  ySlotSize = (yRange/numY);

  xSlotOffset = xSlotSize * (1-X_SLOT_FACTOR) / 2;
  ySlotOffset = ySlotSize * (1-X_SLOT_FACTOR) / 2;
  DBUG_PRINT("rescaleGraphic", ("xSlotOffset=%lf ySlotOffset=%lf",
				xSlotOffset, ySlotOffset));

  xSlotSize *= X_SLOT_FACTOR;
  ySlotSize *= Y_SLOT_FACTOR;
  DBUG_PRINT("rescaleGraphic", ("xSlotSize=%lf ySlotSize=%lf",
				xSlotSize, ySlotSize));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  redrawData: (local function)                                             */
/*                                                                           */
/*  Redraw all...                                                            */
/*                                                                           */
/*****************************************************************************/

void redrawData()
{
  int x, y, s;

  DBUG_ENTER("redrawData");

  mustRedraw = FALSE;

  /* First clear all... */
  XClearWindow(display, window);

  if(strlen(actualTitle)) {
    XTextItem textItem[1];

    topMargin = TOP_MARGIN+20;
    rescaleGraphic();

    textItem[0].chars = actualTitle;
    textItem[0].nchars = strlen(actualTitle);
    textItem[0].delta = 10;
    textItem[0].font = None;

    XDrawText(display, window, gcBlack,
	      xSize/2 - strlen(actualTitle)*3, 18, textItem, 1);
  } else {
    topMargin = TOP_MARGIN;
    rescaleGraphic();
  }

  if(memory != NULL) {
    fillAll(FREE, FALSE);
    for(y=0; y<numY; ++y) {
      for(x=0; x<numX; ++x) {
	if((s = memory[y*numX+x].state) != FREE) {
	  fillSlot(y, x, s);
	}
	if(fragmentationIsActive && memory[y*numX+x].separation) {
	  drawLine(x, y, x, y+1);
	}
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateData: (local function)                                             */
/*                                                                           */
/*  Update data...                                                           */
/*                                                                           */
/*****************************************************************************/

void updateData()
{
  DBUG_ENTER("updateData");

  if(scriptIsActive) {
    if((doUpdates) &&
       ((lastAction != PLAYFORWARD) || (actualRate > 1)) &&
       ((lastAction != PLAYBACKWARD) || (actualRate > 1)) &&
       (lastAction != STEPFORWARD) && (lastAction != STEPBACKWARD)) {
      redrawData();
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  recallCommand: (local function)                                          */
/*                                                                           */
/*  Execute one (stacked) command...                                         */
/*                                                                           */
/*****************************************************************************/

void recallCommand(data, forward)
char *data;
Boolean forward;
{
  short action, x, y;
  char state;

  DBUG_ENTER("recallCommand");

  action = ((struct Element *)data)->new;
  x = ((struct Element *)data)->x;
  y = ((struct Element *)data)->y;

  switch(action) {
  case HEAPBLOCK:
  case DESCRIPTOR:
  case FREE:
    state = (forward ? action : ((struct Element *)data)->old);

    DBUG_PRINT("recallCommand", ("x=%d y=%d state=%d", x, y, state));

    fillSlot(y, x, state);
    memory[y*numX+x].state = state;
    if(fragmentationIsActive && memory[y*numX+x].separation) {
      drawLine(x, y, x, y+1);
    }
    break;

  case FRAGMENT:
    memory[y*numX+x].separation = forward;

    if(fragmentationIsActive) {
      if(forward) {
	drawLine(x, y, x, y+1);
      } else {
	fillSlot(y, x, memory[y*numX+x].state);
      }
    }
    break;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  data2Frame: (local function)                                             */
/*                                                                           */
/*  Return frame of given data...                                            */
/*                                                                           */
/*****************************************************************************/

int data2Frame(data)
void *data;
{
  DBUG_ENTER("data2Frame");

  DBUG_RETURN(((struct Element *)data)->frame);
}


/*****************************************************************************/
/*                                                                           */
/*  data2Time: (local function)                                              */
/*                                                                           */
/*  Return time of given data...                                             */
/*                                                                           */
/*****************************************************************************/

double data2Time(data)
void *data;
{
  DBUG_ENTER("data2Time");

  DBUG_RETURN(((struct Element *)data)->time);
}


/*****************************************************************************/
/*                                                                           */
/*  localDoTick: (local function)                                            */
/*                                                                           */
/*  Do something in the event loop... (called my 'doTick')                   */
/*                                                                           */
/*****************************************************************************/

void localDoTick()
{
  /*DBUG_ENTER("localDoTick");*/

  /*DBUG_VOID_RETURN;*/

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  localDoTickGuard: (local function)                                       */
/*                                                                           */
/*  Prevent execution of certain actions... (called my 'doTick')             */
/*                                                                           */
/*****************************************************************************/

Boolean localDoTickGuard(action)
short action;
{
  Boolean returnValue = TRUE;

  /*DBUG_ENTER("localDoTickGuard");*/

  if(!cachingIsActive) {
    /* No hidden actions allowed... */
    returnValue &= ((action != GOTOSTART) && (action != GOTOEND));
  }

  if(!stackingIsActive) {
    /* No backward action allowed... */
    returnValue &= ((action != GOTOSTART) && (action != PLAYBACKWARD) &&
		    (action != STEPBACKWARD));
  }

  /*DBUG_RETURN(returnValue);*/

  return(returnValue);
}


/*****************************************************************************/
/*                                                                           */
/*  localInitColors: (local function)                                        */
/*                                                                           */
/*  Do some more initialization... (called my 'localStartupHook')            */
/*                                                                           */
/*****************************************************************************/

void localInitColors()
{
  unsigned long valuemask;
  XGCValues values;

  DBUG_ENTER("localInitColors");

  window = XtWindow(WI_ms_heap_array_graphic_area);

  /* First define all needed GCs... */
  if(gcBlack == (GC)NULL) { 
    valuemask = GCFunction | GCForeground;
    values.function = GXcopy;
    values.foreground = BlackPixel(display, screen);
    gcBlack = XCreateGC(display, window, valuemask, &values);
  }

  if(gcWhite == (GC)NULL) {
    valuemask = GCFunction | GCForeground;
    values.function = GXcopy;
    values.foreground = WhitePixel(display, screen);
    gcWhite = XCreateGC(display, window, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[heapblockColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcHeapblock == (GC)NULL) {
    gcHeapblock = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcHeapblock, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[descriptorColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcDescriptor == (GC)NULL) {
    gcDescriptor = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcDescriptor, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[freeColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcFree == (GC)NULL) {
    gcFree = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcFree, valuemask, &values);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  localStartupHook: (local function)                                       */
/*                                                                           */
/*  Do some initialization... (called my 'startupHook')                      */
/*                                                                           */
/*****************************************************************************/

void localStartupHook()
{
  DBUG_ENTER("localStartupHook");

  moduleWindow = WI_ms_heap_array;
  strcpy(prefsName, "heapArray.prefs");

  highlightButton(WI_ms_control_panel_stop);

  xMin = X_MIN;
  yMin = Y_MIN;
  xMax = X_MAX;
  yMax = Y_MAX;

  numX = 500;
  numY = 32;

  rescaleGraphic();

  if(getenv("MS_BLACKWHITE")) {
    /* Run in black-and-white-mode... */
    heapblockColor = WhitePixel(display, screen);
    descriptorColor = WhitePixel(display, screen);
    freeColor = BlackPixel(display, screen);
  } else {
    heapblockColor = LIGHT_RED;
    descriptorColor = LIGHT_YELLOW;
    freeColor = GREEN;
  }

  localInitColors();

  cachingIsActive = stackingIsActive = separationIsActive = TRUE;
  fragmentationIsActive = FALSE;
  SET_TOGGLE(WI_ms_display_panel_caching, FALSE);
  SET_TOGGLE(WI_ms_display_panel_stacking, FALSE);
  UNSET_TOGGLE(WI_ms_display_panel_fragmentation, FALSE);
  SET_TOGGLE(WI_ms_display_panel_separation, FALSE);

  strcpy(selectedScript, "HeapArraySCRs/");

  SET_LABEL(WI_ms_cursor_panel_show_x, "------");
  SET_LABEL(WI_ms_cursor_panel_show_y, "------");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  localExitHook: (local function)                                          */
/*                                                                           */
/*  Keep your environment tidy...                                            */
/*                                                                           */
/*****************************************************************************/

void localExitHook()
{
  DBUG_ENTER("localExitHook");

  XFreeGC(display, gcBlack);
  XFreeGC(display, gcWhite);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  localStartModule: (local function)                                       */
/*                                                                           */
/*  Start script...                                                          */
/*                                                                           */
/*****************************************************************************/

void localStartModule()
{
  DBUG_ENTER("localStartModule");

  mustRedraw = FALSE;

  updateLine(actualLine = 0);
  updateFrame((actualFrame = 1) - 1);
  updateTime(actualTime = 0.0);

  rescaleGraphic();

  /* Free cache memory... */
  if(memory != NULL) {
    cfree((char *)memory);
    memory = NULL;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSave: (callback)                                           */
/*                                                                           */
/*  Save actual plot...                                                      */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSave(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msDisplayPanelSave");

  msFileBrowserInitialize(2, "Save postscript...", EPS_MASK, "./", "plot.eps");
  if(!(scriptIsActive || scriptIsTerminated)) {
    doRequest("", "There's no graphic to save!", "", "OK", "", "", NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelPrint: (callback)                                       */
/*                                                                           */
/*  Print actual plot...                                                     */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelPrint(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[FILE_LEN], command[COMMANDS_LEN];

  DBUG_ENTER("msDisplayPanelPrint");

  if(scriptIsActive || scriptIsTerminated) {
    tmpnam(temp);
    SAVE_PS(temp, temp);
    sprintf(command, "lpr %s ; rm -f %s", temp, temp);
    system(command);
  } else {
    doRequest("", "There's no graphic to print!", "", "OK", "", "", NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetTitle: (callback)                                       */
/*                                                                           */
/*  Enter label for whole plot...                                            */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetTitle(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msDisplayPanelSetTitle");

  GET_LABEL(WI_ms_display_panel_enter_title, actualTitle);
  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetCaching: (callback)                                     */
/*                                                                           */
/*  Turn caching on/off. Caching is needed for "hidden" plays...             */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetCaching(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetCaching");

  GET_TOGGLE_STATE(WI_ms_display_panel_caching, &state);
  if(scriptIsActive && !firstFrame) {
    /* Not allowed while script is running --> undo... */
    if(state) {
      UNSET_TOGGLE(WI_ms_display_panel_caching, FALSE);
    } else {
      SET_TOGGLE(WI_ms_display_panel_caching, FALSE);
    }

    DBUG_PRINT("msDisplayPanelSetCaching",
	       ("Can't do this change while script is active..."));

    doRequest("Can't do this change", "while script is active...", "",
	      "OK", "", "", NULL);
  } else {
    cachingIsActive = state;

    if(state) {
      SET_TOGGLE(WI_ms_display_panel_caching, FALSE);

      if(stackingIsActive) {
	enableButton(WI_ms_control_panel_go_to_start);
      }
      enableButton(WI_ms_control_panel_go_to_end);
    } else {
      UNSET_TOGGLE(WI_ms_display_panel_caching, FALSE);

      UNSET_TOGGLE(WI_ms_display_panel_stacking, TRUE);
      UNSET_TOGGLE(WI_ms_display_panel_fragmentation, TRUE);

      disableButton(WI_ms_control_panel_go_to_start);
      disableButton(WI_ms_control_panel_go_to_end);
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetStacking: (callback)                                    */
/*                                                                           */
/*  Turn stacking on/off. Stacking is needed for reverse plays...            */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetStacking(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetStacking");

  GET_TOGGLE_STATE(WI_ms_display_panel_stacking, &state);
  if(scriptIsActive && !firstFrame) {
    /* Not allowed while script is running --> undo... */
    if(state) {
      UNSET_TOGGLE(WI_ms_display_panel_stacking, FALSE);
    } else {
      SET_TOGGLE(WI_ms_display_panel_stacking, FALSE);
    }

    DBUG_PRINT("msDisplayPanelSetCaching",
	       ("Can't do this change while script is active..."));

    doRequest("Can't do this change", "while script is active...", "",
	      "OK", "", "", NULL);
  } else {
    stackingIsActive = state;

    if(state) {
      SET_TOGGLE(WI_ms_display_panel_stacking, FALSE);

      SET_TOGGLE(WI_ms_display_panel_caching, TRUE);

      if(cachingIsActive) {
	enableButton(WI_ms_control_panel_go_to_start);
      }
      enableButton(WI_ms_control_panel_play_backward);
      enableButton(WI_ms_control_panel_skip_backward);
    } else {
      UNSET_TOGGLE(WI_ms_display_panel_stacking, FALSE);

      disableButton(WI_ms_control_panel_go_to_start);
      disableButton(WI_ms_control_panel_play_backward);
      disableButton(WI_ms_control_panel_skip_backward);
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetFragmentation: (callback)                               */
/*                                                                           */
/*  Turn fragmentation lines on/off...                                       */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetFragmentation(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state, fragmentationWasActive;

  DBUG_ENTER("msDisplayPanelSetFragmentation");

  fragmentationWasActive = fragmentationIsActive;

  GET_TOGGLE_STATE(WI_ms_display_panel_fragmentation, &state);
  fragmentationIsActive = state;

  if(state) {
    SET_TOGGLE(WI_ms_display_panel_fragmentation, FALSE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_fragmentation, FALSE);
  }

  if(scriptIsActive && cachingIsActive &&
     (fragmentationIsActive != fragmentationWasActive)) {
    state = doUpdates;
    doUpdates = TRUE;
    redrawData();
    doUpdates = state;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetSeparation: (callback)                                  */
/*                                                                           */
/*  Turn separation of heap blocks on/off...                                 */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetSeparation(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state, separationWasActive;

  DBUG_ENTER("msDisplayPanelSetSeparation");

  separationWasActive = separationIsActive;

  GET_TOGGLE_STATE(WI_ms_display_panel_separation, &state);
  separationIsActive = state;

  if(state) {
    SET_TOGGLE(WI_ms_display_panel_separation, FALSE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_separation, FALSE);
  }

  if(scriptIsActive && (separationIsActive != separationWasActive)) {
    state = doUpdates;
    doUpdates = TRUE;
    redrawData();
    doUpdates = state;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelClear: (callback)                                          */
/*                                                                           */
/*  Clear the plot...                                                        */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelClear(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msDisplayPanelClear");

  if(!strlen(lastRequestAnswer)) {
    doRequest("Do you really want", "to clear the graphic?", "",
	      "Yes", "No", "", &msDisplayPanelClear);
  } else {
    if(!strcmp(lastRequestAnswer, "Yes")) {
      XClearWindow(display, window);

      scriptIsActive = scriptIsTerminated = FALSE;

      updateLine(actualLine = 0);
      updateFrame((actualFrame = 1) - 1);
      updateTime(actualTime = 0.0);
      updateProgress();
    }

    strcpy(lastRequestAnswer, "");   
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHeapArrayMousePosition: (action)                                       */
/*                                                                           */
/*  Get position of mouse and display it...                                  */
/*                                                                           */
/*****************************************************************************/

void msHeapArrayMousePosition(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  Window rootWindow, myWindow;
  int rootX, rootY, myX, myY;
  unsigned int mask;
  double x, y;

  DBUG_ENTER("msHeapArrayMousePosition");

  XQueryPointer(display, window, &rootWindow, &myWindow,
		&rootX, &rootY, &myX, &myY, &mask);

  DBUG_PRINT("msHeapArrayMousePosition", ("x=%d y=%d", myX, myY));

  x = ((double)(myX-leftMargin))/xFactor+xMin;
  y = ((double)(myY-topMargin))/yFactor+yMin;

  DBUG_PRINT("msHeapArrayMousePosition", ("x=%lf y=%lf", x, y));

  SET_LABEL(WI_ms_cursor_panel_show_x, "------");
  SET_LABEL(WI_ms_cursor_panel_show_y, "------");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHeapArrayClickLeft: (action)                                           */
/*                                                                           */
/*  Left mouse button clicked --> ...                                        */
/*                                                                           */
/*****************************************************************************/

void msHeapArrayClickLeft(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msHeapArrayClickLeft");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHeapArrayClickMiddle: (action)                                         */
/*                                                                           */
/*  Middle mouse button clicked --> ...                                      */
/*                                                                           */
/*****************************************************************************/

void msHeapArrayClickMiddle(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msHeapArrayClickMiddle");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHeapArrayClickRight: (action)                                          */
/*                                                                           */
/*  Right mouse button clicked --> ...                                       */
/*                                                                           */
/*****************************************************************************/

void msHeapArrayClickRight(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msHeapArrayClickRight");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  The following functions are the graphics primitives...                   */
/*                                                                           */
/*****************************************************************************/


void SET_COLORS(h, d, a)                       /* Set colors for heap-states */
int h, d, a;
{
  DBUG_ENTER("SET_COLORS");

  heapblockColor = h;
  descriptorColor = d;
  freeColor = a;

  localInitColors();
  updateData();

  DBUG_VOID_RETURN;
}


void SET_TITLE(s)                                    /* Set title of graphic */
char *s;
{
  DBUG_ENTER("SET_TITLE");

  strcpy(actualTitle, s);
  SET_LABEL(WI_ms_display_panel_enter_title, actualTitle);
  redrawData();

  DBUG_VOID_RETURN;
}

 
void NEXT_FRAME()                         /* End actual frame, begin new one */
{
  DBUG_ENTER("NEXT_FRAME");

  frameCompleted = TRUE;

  if(firstFrame) {
    firstFrame = FALSE;
  } else {
    ++actualFrame;
  }

  DBUG_VOID_RETURN;
}

void NEXT_FRAME_BUT_CONTINUE()         /* Continue with the CONTINUE-case in */
                                      /* the script after doing NEXT_FRAME() */
{
  DBUG_ENTER("NEXT_FRAME_BUT_CONTINUE");

  continueFrame = TRUE;
  NEXT_FRAME();

  DBUG_VOID_RETURN;
}


void FILL_BLOCK(y, x, s)       /* Fill specified slot according to its state */
short y, x, s;
{
  struct Element *temp;

  DBUG_ENTER("FILL_BLOCK");

  if(stackingIsActive) {
    temp = (struct Element *)newData();
    temp->time = actualTime;
    temp->frame = actualFrame;
    temp->new = s;
    temp->old = memory[y*numX+x].state;
    temp->x = x;
    temp->y = y;
  }

  DBUG_PRINT("FILL_BLOCK", ("y=%d x=%d s=%d", y, x, s));

  if(cachingIsActive) {
    memory[y*numX+x].state = s;
  }
  fillSlot(y, x, s);

  DBUG_VOID_RETURN;
}


void FILL_ALL(s)                                           /* Fill all slots */
short s;
{
  DBUG_ENTER("FILL_ALL");

  fillAll(s, TRUE);

  DBUG_VOID_RETURN;
}


void SET_DIMENSIONS(y, x)                               /* Set numX and numY */
int y, x;
{
  int i, j;

  DBUG_ENTER("SET_DIMENSIONS");

  numY = y;
  numX = x;

  DBUG_PRINT("SET_DIMENSIONS", ("numY=%d numX=%d", numY, numX));

  rescaleGraphic();

  if(cachingIsActive) {
    if(memory != NULL) {
      cfree((char *)memory);
    }
    if((memory = (struct Memory *)calloc(numY*numX,
					 sizeof(dummyMemory))) == NULL) {
      dieHard("Could not get memory for state storage...");
    }
    for(i=0; i<numY; ++i) {
      for(j=0; j<numX; ++j) {
	memory[i*numX+j].state = FREE;
	memory[i*numX+j].separation = FALSE;
      }
    }
  }

  DBUG_VOID_RETURN;
}


int GET_HEAPBASE(p)         /* Return base address of heap on specified node */
short p;
{
  DBUG_ENTER("GET_HEAPBASE");

  DBUG_RETURN(heapBases[p]);
}


void DRAW_FRAGMENTATION(y, x)        /* Draw separation (fragmentation) line */
short y, x;
{
  struct Element *temp;

  DBUG_ENTER("DRAW_FRAGMENTATION");

  if(stackingIsActive) {
    temp = (struct Element *)newData();
    temp->time = actualTime;
    temp->frame = actualFrame;
    temp->new = FRAGMENT;
    temp->x = x;
    temp->y = y;
  }

  if(cachingIsActive) {
    memory[y*numX+x].separation = TRUE;
  }

  if(fragmentationIsActive) {
    drawLine(x, y, x, y+1);
  }

  DBUG_VOID_RETURN;
}


void SET_CACHING(s)                                 /* Toggle caching on/off */
short s;
{
  DBUG_ENTER("SET_CACHING");

  if((cachingIsActive = (s == ON))) {
    SET_TOGGLE(WI_ms_display_panel_caching, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_caching, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SET_STACKING(s)                               /* Toggle stacking on/off */
short s;
{
  DBUG_ENTER("SET_STACKING");

  if((stackingIsActive = (s == ON))) {
    SET_TOGGLE(WI_ms_display_panel_stacking, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_stacking, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SET_SEPARATION(s)                     /* Toggle gap between rows on/off */
short s;
{
  DBUG_ENTER("SET_SEPARATION");

  if((separationIsActive = (s == ON))) {
    SET_TOGGLE(WI_ms_display_panel_separation, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_separation, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SET_FRAGMENTATION(s)       /* Toggle drawing of block boundaries on/off */
short s;
{
  DBUG_ENTER("SET_FRAGMENTATION");

  if((fragmentationIsActive = (s == ON))) {
    SET_TOGGLE(WI_ms_display_panel_fragmentation, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_fragmentation, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SAVE(f, n)                             /* Save actual plot to file (ps) */
char *f, *n;
{
  DBUG_ENTER("SAVE");

  DBUG_PRINT("SAVE", ("fullname=%s name=%s", f, n));

  SAVE_PS(f, n);

  DBUG_VOID_RETURN;
}


void PRINT()                                       /* Print actual plot (ps) */
{
  DBUG_ENTER("PRINT");

  msDisplayPanelPrint();

  DBUG_VOID_RETURN;
}


void CLEAR()                                            /* Clear actual plot */
{
  DBUG_ENTER("CLEAR");

  msDisplayPanelClear();

  DBUG_VOID_RETURN;
}
