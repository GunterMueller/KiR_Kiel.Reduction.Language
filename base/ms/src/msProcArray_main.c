#include "msProcArray_main.h"


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
          gcCreateArrow = (GC)NULL,
          gcHistoryArrow = (GC)NULL,
          gcFutureArrow = (GC)NULL,
          gcReady = (GC)NULL,
          gcActive = (GC)NULL,
          gcSuspended = (GC)NULL,
          gcTerminated = (GC)NULL;
static int readyColor, activeColor, suspendedColor, terminatedColor;

static int actualHistory, actualFuture, actualStyle;
static Boolean mustRedraw, actualMeter;

static int drawnHistoryNum, drawnFutureNum;
static short drawnHistoryProcessor[MAX_HISTORY_NUM],
             drawnHistoryProcess[MAX_HISTORY_NUM],
             drawnHistoryLevel[MAX_HISTORY_NUM],
             drawnFutureProcessor[MAX_FUTURE_NUM],
             drawnFutureProcess[MAX_FUTURE_NUM],
             drawnFutureLevel[MAX_FUTURE_NUM];
static char actualTitle[TITLE_LEN];


/* Graphics primitives */
void SET_COLORS();                          /* Set colors for process-states */
void SET_TITLE();                                    /* Set title of graphic */
void NEXT_FRAME();                        /* End actual frame, begin new one */
void NEXT_FRAME_BUT_CONTINUE();        /* Continue with the CONTINUE-case in */
                                      /* the script after doing NEXT_FRAME() */
void CREATION_ARROW();                                /* Draw creation arrow */
void SHOW_HISTORY();                    /* Show history of specified process */
void SHOW_FUTURE();                      /* Show future of specified process */
void FILL_SLOT();              /* Fill specified slot according to its state */
void SET_STYLE();                   /* Set display style (none, text, color) */
void SET_METER();                                /* Turn on/off Load-O-Meter */
void DRAW_METER();                               /* Draw/update Load-O-Meter */
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
void rescaleGraphic();


#include "msModule.c"

static int numProcessors, numProcesses;
static int pointedProcessor, pointedProcess;
static struct Processes processes[MAX_PROCESSORS][MAX_PROCESSES];

static struct {
         short x1, y1, x2, y2, t;
       } arrows[MAX_ARROWS];
static int numArrows;

static struct {
         short load, comm;
       } loadOMeter[MAX_PROCESSORS];


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

  switch(numProcessors) {
  case 1:
    DBUG_RETURN((int)((real-yMin)*yFactor/8)+topMargin+(usableYSize*7/16));
    break;
  case 2:
    DBUG_RETURN((int)((real-yMin)*yFactor/4)+topMargin+(usableYSize*3/8));
    break;
  case 4:
    DBUG_RETURN((int)((real-yMin)*yFactor/2)+topMargin+(usableYSize/4));
    break;
  default:
    DBUG_RETURN((int)((real-yMin)*yFactor)+topMargin);
    break;
  }
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

  DBUG_RETURN(process*xRange/numProcesses+xSlotOffset);
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

  DBUG_RETURN(process*xRange/numProcesses+xSlotOffset+xSlotSize);
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

  DBUG_RETURN(processor*yRange/numProcessors+ySlotOffset);
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

  DBUG_RETURN(processor*yRange/numProcessors+ySlotOffset+ySlotSize);
}


/*****************************************************************************/
/*                                                                           */
/*  drawFuturePS: (local function)                                           */
/*                                                                           */
/*  Draw lines to the sons of this process... (Postscript)                   */
/*                                                                           */
/*****************************************************************************/

void drawFuturePS(psFile, processor, process, level)
FILE *psFile;
short processor, process, level;
{
  int i, j;
  int xFrom, yFrom, xTo, yTo;

  DBUG_ENTER("drawFuturePS");

  if((processor != -1) && (process != -1)) {
    if(level) {
      for(i=0; i<numProcessors; ++i) {
	for(j=0; j<numSlots[i]; ++j) {
	  if((processes[i][j].fatherProcessor == processor) &&
	     (processes[i][j].fatherProcess == process)) {

	    xFrom = xTransform(xLeft(process) + xSlotSize/2);
	    yFrom = ySize - yTransform(yTop(processor) + ySlotSize/2);
	    xTo = xTransform(xLeft(j) + xSlotSize/2);
	    yTo = ySize - yTransform(yTop(i) + ySlotSize/2);
    
	    fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
		    xFrom, yFrom, xTo, yTo);

	    xFrom = xTransform(xLeft(process) + xSlotSize/4);
	    yFrom = ySize - yTransform(yTop(processor) + ySlotSize/4);
	    xTo = xTransform(xRight(process) - xSlotSize/4);
	    yTo = ySize - yTransform(yBottom(processor) - ySlotSize/4);

	    fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		    xFrom, yFrom, xFrom, yTo, xTo, yTo);
	    fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		    xTo, yFrom, xFrom, yFrom);

	    drawFuturePS(psFile, i, j, level-1);
	  }
	}
      }
    }
  }

  DBUG_VOID_RETURN;
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
  int i, j;

  DBUG_ENTER("generatePostscript");

  /* Open file... */
  if(((psFile = fopen(file, "w")) == NULL) ||
     !(scriptIsActive || scriptIsTerminated)) {
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

    if(actualMeter) {
      /* Display Load-O-Meter at the right... */
      rightMargin = RIGHT_MARGIN + LOADOMETER_SIZE + LOADOMETER_GAP;
    } else {
      rightMargin = RIGHT_MARGIN;
    }
    
    rescaleGraphic();
    
    /* Now draw the slots... */
    fprintf(psFile, "%.2f setlinewidth\n", PS_BORDER_WIDTH);
    for(i=0; i<numProcessors; ++i) {
      for(j=0; j<numSlots[i]; ++j) {
	int xFrom = xTransform(xLeft(j));
	int xTo = xTransform(xRight(j));
	int yFrom = ySize - yTransform(yTop(i));
	int yTo = ySize - yTransform(yBottom(i));
	
	switch(processes[i][j].state) {
	case READY:
	  fprintf(psFile, "%.2f setgray\n", PS_READY_COLOR);
	  break;
	case ACTIVE:
	  fprintf(psFile, "%.2f setgray\n", PS_ACTIVE_COLOR);
	  break;
	case SUSPENDED:
	case HALF_SUSPENDED:
	  fprintf(psFile, "%.2f setgray\n", PS_SUSPENDED_COLOR);
	  break;
	case TERMINATED:
	case HALF_TERMINATED:
	  fprintf(psFile, "%.2f setgray\n", PS_TERMINATED_COLOR);
	  break;
	}

	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom, yFrom, xFrom, yTo, xTo, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		xTo, yFrom, xFrom, yFrom);

	if((processes[i][j].state == HALF_SUSPENDED) ||
	   (processes[i][j].state == HALF_TERMINATED)) {
	  fprintf(psFile, "%.2f setgray\n", PS_ACTIVE_COLOR);

	  fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		  xFrom, yFrom, xFrom, yTo, xTo, yTo);
	  fprintf(psFile, "%d %d lineto fill\n",
		  xFrom, yFrom);
	}

	fprintf(psFile, "%.2f setgray\n", PS_BORDER_COLOR);
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom, yFrom, xFrom, yTo, xTo, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto stroke\n",
		xTo, yFrom, xFrom, yFrom);
      }
      
      if(actualMeter) {
	/* Draw Load-O-Meter for processor i... */
	int xFrom = xSize - RIGHT_MARGIN - LOADOMETER_SIZE;
	int yFrom = ySize - yTransform(yTop(i));
	int xTo = xSize - RIGHT_MARGIN;
	int yTo = ySize - yTransform(yBottom(i));
	short load = loadOMeter[i].load;
        short comm = loadOMeter[i].comm;
        /*short idle = LOADOMETER_SIZE - load - comm - 1;*/

	fprintf(psFile, "%.2f setgray\n", PS_LOAD_COLOR);
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom, yFrom, xFrom, yTo, xFrom+1+load, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		xFrom+1+load, yFrom, xFrom, yFrom);

	fprintf(psFile, "%.2f setgray\n", PS_COMM_COLOR);
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom+1+load, yFrom, xFrom+1+load, yTo,
		xFrom+1+load+comm, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		xFrom+1+load+comm, yFrom, xFrom+1+load, yFrom);

	fprintf(psFile, "%.2f setgray\n", PS_IDLE_COLOR);
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom+1+load+comm, yFrom, xFrom+1+load+comm, yTo, xTo, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		xTo, yFrom, xFrom+1+load+comm, yFrom);

	fprintf(psFile, "%.2f setgray\n", PS_BORDER_COLOR);
	fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		xFrom, yFrom, xFrom, yTo, xTo, yTo);
	fprintf(psFile, "%d %d lineto %d %d lineto stroke\n",
		xTo, yFrom, xFrom, yFrom);

	fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
		xFrom+1+load, yFrom, xFrom+1+load, yTo);
	fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
		xFrom+1+load+comm, yFrom, xFrom+1+load+comm, yTo);
      }
    }
    
    /* And now the arrows... */
    fprintf(psFile, "%.2f setgray\n", PS_ARROW_COLOR);
    fprintf(psFile, "%.2f setlinewidth\n", PS_ARROW_WIDTH);      
    
    for(i=0; i<numArrows; ++i) {
      int xFrom, yFrom, xTo, yTo;

      xFrom = xTransform(xLeft(arrows[i].y1) + xSlotSize/2);
      yFrom = ySize - yTransform(yTop(arrows[i].x1) + ySlotSize/2);
      xTo = xTransform(xLeft(arrows[i].y2) + xSlotSize/2);
      yTo = ySize - yTransform(yTop(arrows[i].x2) + ySlotSize/2);
    
      fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
	      xFrom, yFrom, xTo, yTo);

      xFrom = xTransform(xLeft(arrows[i].y2) + xSlotSize/8);
      yFrom = ySize - yTransform(yTop(arrows[i].x2) + ySlotSize/8);
      xTo = xTransform(xRight(arrows[i].y2) - xSlotSize/8);
      yTo = ySize - yTransform(yBottom(arrows[i].x2) - ySlotSize/8);

      fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
	      xFrom, yFrom, xFrom, yTo, xTo, yTo);
      fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
	      xTo, yFrom, xFrom, yFrom);
    }

    /* History... */
    fprintf(psFile, "%.2f setgray\n", PS_HISTORY_COLOR);
    fprintf(psFile, "%.2f setlinewidth\n", PS_HISTORY_WIDTH);      

    if(drawnHistoryNum) {
      int num;

      for(num=0; num<drawnHistoryNum; ++num) {
	short processor = drawnHistoryProcessor[num];
	short process = drawnHistoryProcess[num];
	short level = drawnHistoryLevel[num];

	int i, xFrom, yFrom, xTo, yTo;
	short fatherProcessor, fatherProcess, actualProcessor, actualProcess;

	if((processor != -1) && (process != -1)) {
	  actualProcessor = processor;
	  actualProcess = process;

	  for(i=0; i<level; ++i) {
	    if((fatherProcessor =
		processes[actualProcessor][actualProcess].fatherProcessor) ==
	       -1) {
	      break;
	    }
	    if((fatherProcess =
		processes[actualProcessor][actualProcess].fatherProcess) ==
	       -1) {
	      break;
	    }

	    xFrom = xTransform(xLeft(fatherProcess) + xSlotSize/2);
	    yFrom = ySize - yTransform(yTop(fatherProcessor) + ySlotSize/2);
	    xTo = xTransform(xLeft(actualProcess) + xSlotSize/2);
	    yTo = ySize - yTransform(yTop(actualProcessor) + ySlotSize/2);
    
	    fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
		    xFrom, yFrom, xTo, yTo);

	    xFrom = xTransform(xLeft(actualProcess) + xSlotSize/4);
	    yFrom = ySize - yTransform(yTop(actualProcessor) + ySlotSize/4);
	    xTo = xTransform(xRight(actualProcess) - xSlotSize/4);
	    yTo = ySize - yTransform(yBottom(actualProcessor) - ySlotSize/4);

	    fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
		    xFrom, yFrom, xFrom, yTo, xTo, yTo);
	    fprintf(psFile, "%d %d lineto %d %d lineto fill\n",
		    xTo, yFrom, xFrom, yFrom);

	    actualProcessor = fatherProcessor;
	    actualProcess = fatherProcess;
	  }
	}
      }
    }

    /* Future... */
    fprintf(psFile, "%.2f setgray\n", PS_FUTURE_COLOR);
    fprintf(psFile, "%.2f setlinewidth\n", PS_FUTURE_WIDTH);      

    if(drawnFutureNum) {
      int num;

      for(num=0; num<drawnFutureNum; ++num) {
	drawFuturePS(psFile, drawnFutureProcessor[num],
		     drawnFutureProcess[num], drawnFutureLevel[num]);
      }
    }

    /* Now the labels... */
    fprintf(psFile, "%.2f setgray\n", PS_LABEL_COLOR);

    for(i=0; i<numProcessors; ++i) {
      for(j=0; j<numSlots[i]; ++j) {
	int xFrom = xTransform(xLeft(j));
	int xTo = xTransform(xRight(j));
	int yFrom = ySize - yTransform(yTop(i));
	int yTo = ySize - yTransform(yBottom(i));

	switch(actualStyle) {
	case NONE:
	  break;
	case TEXT: {
	  int fatherProcessor = processes[i][j].fatherProcessor;
	  int fatherProcess = processes[i][j].fatherProcess;
	  char temp[10];

	  if((fatherProcessor != -1) && (fatherProcess != -1)) {
	    sprintf(temp, "%02d%02d", fatherProcessor, fatherProcess);
	  } else {
	    sprintf(temp, "    ");
	  }

	  fprintf(psFile, "%d %d moveto (%s) ccshow\n",
		  (int)((xFrom+xTo)/2), (int)((yFrom+yTo)/2)-3, temp);
    
	  break;
	}

	case COLOR:
	  break;
	}
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
/*  storeArrow: (local function)                                             */
/*                                                                           */
/*  Store coordinates and type of a arrow...                                 */
/*                                                                           */
/*****************************************************************************/

void storeArrow(x1, y1, x2, y2, t)
short x1, y1, x2, y2, t;
{
  DBUG_ENTER("storeArrow");

  arrows[numArrows].x1 = x1;
  arrows[numArrows].y1 = y1;
  arrows[numArrows].x2 = x2;
  arrows[numArrows].y2 = y2;
  arrows[numArrows].t = t;
  if(++numArrows >= MAX_ARROWS) {
    dieHard("Number of arrows in plot too high...");
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  isArrow: (local function)                                                */
/*                                                                           */
/*  Test, whether is a arrow with this coordinates and type stored...        */
/*                                                                           */
/*****************************************************************************/

Boolean isArrow(x1, y1, x2, y2, t)
short x1, y1, x2, y2, t;
{
  int i;

  DBUG_ENTER("isArrow");

  for(i=0; i<numArrows; ++i) {
    if((arrows[i].x1 == x1) && (arrows[i].y1 == y1) &&
       (arrows[i].x2 == x2) && (arrows[i].y2 == y2) &&
       (arrows[i].t == t)) {
      DBUG_RETURN(TRUE);
    }
  }

  DBUG_RETURN(FALSE);
}


/*****************************************************************************/
/*                                                                           */
/*  eraseArrow: (local function)                                             */
/*                                                                           */
/*  Erase arrow with specified coordinates and type...                       */
/*                                                                           */
/*****************************************************************************/

Boolean eraseArrow(x1, y1, x2, y2, t)
short x1, y1, x2, y2, t;
{
  int i;

  DBUG_ENTER("eraseArrow");

  for(i=0; i<numArrows; ++i) {
    if((arrows[i].x1 == x1) && (arrows[i].y1 == y1) &&
       (arrows[i].x2 == x2) && (arrows[i].y2 == y2) &&
       (arrows[i].t == t)) {
      break;
    }
  }

  if(i < numArrows) {
    while(++i < numArrows) {
      arrows[i-1].x1 = arrows[i].x1;
      arrows[i-1].y1 = arrows[i].y1;
      arrows[i-1].x2 = arrows[i].x2;
      arrows[i-1].y2 = arrows[i].y2;
      arrows[i-1].t = arrows[i].t;
    }
    --numArrows;

    DBUG_RETURN(TRUE);
  }

  DBUG_RETURN(FALSE);
}


/*****************************************************************************/
/*                                                                           */
/*  drawLine: (local function)                                               */
/*                                                                           */
/*  Draw a line (history, future, creation) from one slot to another one...  */
/*                                                                           */
/*****************************************************************************/

void drawLine(fromProcessor, fromProcess, toProcessor, toProcess, type, pure)
int fromProcessor, fromProcess, toProcessor, toProcess, type;
Boolean pure;
{
  int xFrom, yFrom, xTo, yTo, arcX, arcWidth, arcY, arcHeight;

  DBUG_ENTER("drawLine");

  if(mustRedraw &&
     ((lastAction == PLAYBACKWARD) || (lastAction == STEPBACKWARD) ||
      (lastAction == PLAYFORWARD) || (lastAction == STEPFORWARD))) {
    redrawData();
  }

  if(!pure) {
    if(isArrow(fromProcessor, fromProcess, toProcessor, toProcess, type)) {
      eraseArrow(fromProcessor, fromProcess, toProcessor, toProcess, type);
    } else {
      storeArrow(fromProcessor, fromProcess, toProcessor, toProcess, type);
    }
  }

  if(doUpdates) {
    xFrom = xTransform(xLeft(fromProcess) + xSlotSize/2);
    yFrom = yTransform(yTop(fromProcessor) + ySlotSize/2);
    xTo = xTransform(xLeft(toProcess) + xSlotSize/2);
    yTo = yTransform(yTop(toProcessor) + ySlotSize/2);

    arcX = xTransform(xLeft(toProcess) + xSlotSize/4);
    arcY = yTransform(yTop(toProcessor) + ySlotSize/4);

    arcWidth = xSlotSize/2 * xFactor;
    switch(numProcessors) {
    case 1:
      arcHeight = ySlotSize/2 * yFactor / 8;
      break;
    case 2:
      arcHeight = ySlotSize/2 * yFactor / 4;
      break;
    case 4:
      arcHeight = ySlotSize/2 * yFactor / 2;
      break;
    default:
      arcHeight = ySlotSize/2 * yFactor;
      break;
    }
    
    switch(type) {
    case CREATE:
      XDrawLine(display, window, gcCreateArrow, xFrom, yFrom, xTo, yTo);
      XDrawArc(display, window, gcCreateArrow,
	       arcX, arcY, arcWidth, arcHeight, 0, 23040);
      break;
    case HISTORY:
      XDrawLine(display, window, gcHistoryArrow, xFrom, yFrom, xTo, yTo);
      XDrawArc(display, window, gcHistoryArrow,
	       arcX, arcY, arcWidth, arcHeight, 0, 23040);
      break;
    case FUTURE:
      XDrawLine(display, window, gcFutureArrow, xFrom, yFrom, xTo, yTo);
      XDrawArc(display, window, gcFutureArrow,
	       arcX, arcY, arcWidth, arcHeight, 0, 23040);
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  drawHistory: (local function)                                            */
/*                                                                           */
/*  Draw lines to the fathers of this process...                             */
/*                                                                           */
/*****************************************************************************/

void drawHistory(processor, process, level)
short processor, process, level;
{
  int i;
  short fatherProcessor, fatherProcess, actualProcessor, actualProcess;

  DBUG_ENTER("drawHistory");

  if((processor != -1) && (process != -1)) {
    actualProcessor = processor;
    actualProcess = process;

    for(i=0; i<level; ++i) {
      if((fatherProcessor =
	  processes[actualProcessor][actualProcess].fatherProcessor) == -1) {
	break;
      }
      if((fatherProcess =
	  processes[actualProcessor][actualProcess].fatherProcess) == -1) {
	break;
      }

      drawLine(fatherProcessor, fatherProcess, actualProcessor, actualProcess,
	       HISTORY, TRUE);
      mustRedraw = TRUE;

      actualProcessor = fatherProcessor;
      actualProcess = fatherProcess;
    }
  }

  drawnHistoryProcessor[drawnHistoryNum] = processor;
  drawnHistoryProcess[drawnHistoryNum] = process;
  drawnHistoryLevel[drawnHistoryNum] = level;
  ++drawnHistoryNum;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  drawFuture: (local function)                                             */
/*                                                                           */
/*  Draw lines to the sons of this process...                                */
/*                                                                           */
/*****************************************************************************/

void drawFuture(processor, process, level)
short processor, process, level;
{
  int i, j;

  DBUG_ENTER("drawFuture");

  if((processor != -1) && (process != -1)) {
    if(level) {
      for(i=0; i<numProcessors; ++i) {
	for(j=0; j<numSlots[i]; ++j) {
	  if((processes[i][j].fatherProcessor == processor) &&
	     (processes[i][j].fatherProcess == process)) {
	    drawLine(processor, process, i, j, FUTURE, TRUE);
	    mustRedraw = TRUE;
	    drawFuture(i, j, level-1);
	  }
	}
      }
    }
  }

  drawnFutureProcessor[drawnFutureNum] = processor;
  drawnFutureProcess[drawnFutureNum] = process;
  drawnFutureLevel[drawnFutureNum] = level;
  ++drawnFutureNum;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  labelSlot: (local function)                                              */
/*                                                                           */
/*  Write string into slot...                                                */
/*                                                                           */
/*****************************************************************************/

void labelSlot(processor, process, label)
int processor, process;
char *label;
{
  int x, y;
  XTextItem textItem[1];

  DBUG_ENTER("labelSlot");

  x = xTransform(xLeft(process) + xSlotSize/2) - 22;
  y = yTransform(yTop(processor) + ySlotSize/2) + 5;

  textItem[0].chars = label;
  textItem[0].nchars = strlen(label);
  textItem[0].delta = 10;
  textItem[0].font = None;

  switch(processes[processor][process].state) {
  case READY:
  case ACTIVE:
  case TERMINATED:
  case HALF_TERMINATED:
  case HALF_SUSPENDED:
    XDrawText(display, window, gcBlack, x, y, textItem, 1);
    break;
  case SUSPENDED:
    XDrawText(display, window, gcWhite, x, y, textItem, 1);
    break;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  drawMeter: (local function)                                              */
/*                                                                           */
/*  Draw a Load-O-Meter slot...                                              */
/*                                                                           */
/*****************************************************************************/

void drawMeter(processor)
int processor;
{
  int xFrom, yFrom, xTo, yTo;
  short load = loadOMeter[processor].load,
        comm = loadOMeter[processor].comm,
        idle = LOADOMETER_SIZE - load - comm - 1;

  DBUG_ENTER("drawMeter");

  if(mustRedraw &&
     ((lastAction == PLAYBACKWARD) || (lastAction == STEPBACKWARD) ||
      (lastAction == PLAYFORWARD) || (lastAction == STEPFORWARD))) {
    redrawData();
  }

  DBUG_PRINT("drawMeter", ("processor=%d load=%d comm=%d idle=%d",
			   processor, load, comm, idle));

  if(doUpdates) {
    xFrom = xSize - RIGHT_MARGIN - LOADOMETER_SIZE;
    yFrom = yTransform(yTop(processor));
    xTo = xSize - RIGHT_MARGIN;
    yTo = yTransform(yBottom(processor));

    XFillRectangle(display, window, gcActive,
		   xFrom+1, yFrom, load, yTo-yFrom);
    XFillRectangle(display, window, gcBlack,
		   xFrom+1+load, yFrom, comm, yTo-yFrom);
    XFillRectangle(display, window, gcTerminated,
		   xFrom+1+load+comm, yFrom, idle, yTo-yFrom);
    XDrawRectangle(display, window, gcBlack,
		   xFrom, yFrom, xTo-xFrom, yTo-yFrom);
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

void fillSlot(processor, process, state)
int processor, process, state;
{
  int xFrom, yFrom, xTo, yTo;
  char temp[5];
  XPoint points[3];

  DBUG_ENTER("fillSlot");

  if(mustRedraw &&
     ((lastAction == PLAYBACKWARD) || (lastAction == STEPBACKWARD) ||
      (lastAction == PLAYFORWARD) || (lastAction == STEPFORWARD))) {
    redrawData();
  }

  processes[processor][process].state = state;

  if(doUpdates) {
    xFrom = xTransform(xLeft(process));
    xTo = xTransform(xRight(process));
    yFrom = yTransform(yTop(processor));
    yTo = yTransform(yBottom(processor));

    switch(state) {
    case READY:
      XFillRectangle(display, window, gcReady,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case ACTIVE:
      XFillRectangle(display, window, gcActive,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case SUSPENDED:
      XFillRectangle(display, window, gcSuspended,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      break;
    case TERMINATED:
      XFillRectangle(display, window, gcTerminated,
		     xFrom, yFrom,
		     (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));
      processes[processor][process].fatherProcessor =
      processes[processor][process].fatherProcess = -1;
      break;
    case HALF_SUSPENDED:
      points[0].x = xFrom;
      points[0].y = yFrom;
      points[1].x = xTo;
      points[1].y = yTo;
      points[2].x = xTo;
      points[2].y = yFrom;
      XFillPolygon(display, window, gcSuspended,
		   points, 3, Convex, CoordModeOrigin);
      points[2].x = xFrom;
      points[2].y = yTo;
      XFillPolygon(display, window, gcActive,
		   points, 3, Convex, CoordModeOrigin);
      break;
    case HALF_TERMINATED:
      points[0].x = xFrom;
      points[0].y = yFrom;
      points[1].x = xTo;
      points[1].y = yTo;
      points[2].x = xTo;
      points[2].y = yFrom;
      XFillPolygon(display, window, gcTerminated,
		   points, 3, Convex, CoordModeOrigin);
      points[2].x = xFrom;
      points[2].y = yTo;
      XFillPolygon(display, window, gcActive,
		   points, 3, Convex, CoordModeOrigin);
      break;
    }

    XDrawRectangle(display, window, gcBlack,
		   xFrom, yFrom,
		   (unsigned int)(xTo-xFrom), (unsigned int)(yTo-yFrom));

    switch(actualStyle) {
    case NONE:
      break;
    case TEXT:
      yFrom = processes[processor][process].fatherProcessor;
      xFrom = processes[processor][process].fatherProcess;

      if((xFrom != -1) && (yFrom != -1)) {
	sprintf(temp, "%02d%02d", yFrom, xFrom);
      } else {
	sprintf(temp, "    ");
      }
      labelSlot(processor, process, temp);
      break;
    case COLOR:
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

  GET_SIZE(WI_ms_proc_array_graphic_area, &width, &height);

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

  xSlotSize = (xRange/numProcesses);
  ySlotSize = (yRange/numProcessors);

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
  int i, j;

  DBUG_ENTER("redrawData");

  mustRedraw = FALSE;
  drawnHistoryNum = drawnFutureNum = 0;

  /* First clear all... */
  XClearWindow(display, window);

  if(strlen(actualTitle)) {
    XTextItem textItem[1];

    topMargin = TOP_MARGIN+20;

    textItem[0].chars = actualTitle;
    textItem[0].nchars = strlen(actualTitle);
    textItem[0].delta = 10;
    textItem[0].font = None;

    XDrawText(display, window, gcBlack,
	      xSize/2 - strlen(actualTitle)*3, 18, textItem, 1);
  } else {
    topMargin = TOP_MARGIN;
  }

  if(actualMeter) {
    /* Display Load-O-Meter at the right... */
    rightMargin = RIGHT_MARGIN + LOADOMETER_SIZE + LOADOMETER_GAP;
  } else {
    rightMargin = RIGHT_MARGIN;
  }

  rescaleGraphic();

  /* Now draw the slots... */
  for(i=0; i<numProcessors; ++i) {
    for(j=0; j<numSlots[i]; ++j) {
      fillSlot(i, j, processes[i][j].state);
    }

    if(actualMeter) {
      /* Draw Load-O-Meter for processor i... */
      drawMeter(i);
    }
  }

  /* And now the arrows... */
  for(i=0; i<numArrows; ++i) {
    drawLine(arrows[i].x1, arrows[i].y1,
	     arrows[i].x2, arrows[i].y2, arrows[i].t, TRUE);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateHistory: (local function)                                          */
/*                                                                           */
/*  Update history...                                                        */
/*                                                                           */
/*****************************************************************************/

void updateHistory(i)
int i;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("updateHistory");

  sprintf(temp, "%d", actualHistory);
  SET_LABEL(WI_ms_history_panel_enter_level, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateFuture: (local function)                                           */
/*                                                                           */
/*  Update future...                                                         */
/*                                                                           */
/*****************************************************************************/

void updateFuture(i)
int i;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("updateFuture");

  sprintf(temp, "%d", actualFuture);
  SET_LABEL(WI_ms_future_panel_enter_level, temp);

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

  XSync(display, FALSE);

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
  short action, s[6], i;

  DBUG_ENTER("recallCommand");

  action = ((struct Element *)data)->action;
  for(i=0; i<6; ++i) {
    s[i] = ((struct Element *)data)->s[i];
  }

  switch(action) {
  case CREATE:
    drawLine(s[0], s[1], s[2], s[3], CREATE, FALSE);
    break;

  case FILL:
    processes[s[0]][s[1]].fatherProcessor = s[4];
    processes[s[0]][s[1]].fatherProcess = s[5];
    fillSlot(s[0], s[1], (forward ? s[3] : s[2]));
    break;

  case HISTORY:
    drawHistory(s[0], s[1], s[2]);
    break;

  case FUTURE:
    drawFuture(s[0], s[1], s[2]);
    break;

  case LOADOMETER:
    loadOMeter[s[0]].load = s[1];
    loadOMeter[s[0]].comm = s[2];

    if(actualMeter) {
      drawMeter(s[0]);
    }
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

Boolean localDoTickGuard()
{
  /*DBUG_ENTER("localDoTickGuard");*/

  /*DBUG_RETURN(TRUE);*/

  return(TRUE);
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

  window = XtWindow(WI_ms_proc_array_graphic_area);

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

  if(gcCreateArrow == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);
    values.line_width = 10;
    gcCreateArrow = XCreateGC(display, window, valuemask, &values);
  }

  if(gcHistoryArrow == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);
    values.line_width = 7;
    gcHistoryArrow = XCreateGC(display, window, valuemask, &values);
  }

  if(gcFutureArrow == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);
    values.line_width = 4;
    gcFutureArrow = XCreateGC(display, window, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[activeColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcActive == (GC)NULL) {
    gcActive = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcActive, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[terminatedColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcTerminated == (GC)NULL) {
    gcTerminated = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcTerminated, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[readyColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcReady == (GC)NULL) {
    gcReady = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcReady, valuemask, &values);
  }

  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = colors[suspendedColor].pixel;
  values.background = WhitePixel(display, screen);
  if(gcSuspended == (GC)NULL) {
    gcSuspended = XCreateGC(display, window, valuemask, &values);
  } else {
    XChangeGC(display, gcSuspended, valuemask, &values);
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

  moduleWindow = WI_ms_proc_array;
  strcpy(prefsName, "procArray.prefs");

  highlightButton(WI_ms_control_panel_stop);

  xMin = X_MIN;
  yMin = Y_MIN;
  xMax = X_MAX;
  yMax = Y_MAX;

  numProcessors = MAX_PROCESSORS;
  numProcesses = MAX_PROCESSES;
  pointedProcessor = pointedProcess = -1;

  rescaleGraphic();

  readyColor = LIGHT_YELLOW;
  activeColor = GREEN;
  suspendedColor = LIGHT_RED;
  terminatedColor = WHITE;
  localInitColors();

  updateHistory(actualHistory = 1);
  updateFuture(actualFuture = 1);

  actualStyle = NONE;
  UNSET_TOGGLE(WI_ms_display_panel_none_style, FALSE);
  SET_TOGGLE(WI_ms_display_panel_text_style, FALSE);
  UNSET_TOGGLE(WI_ms_display_panel_color_style, FALSE);

  actualMeter = FALSE;
  UNSET_TOGGLE(WI_ms_display_panel_set_load_o_meter, FALSE);

  strcpy(selectedScript, "ProcArraySCRs/");

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
  XFreeGC(display, gcCreateArrow);
  XFreeGC(display, gcHistoryArrow);
  XFreeGC(display, gcFutureArrow);
  XFreeGC(display, gcReady);
  XFreeGC(display, gcActive);
  XFreeGC(display, gcSuspended);
  XFreeGC(display, gcTerminated);

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
  short i, j;

  DBUG_ENTER("localStartModule");

  numArrows = 0;
  mustRedraw = FALSE;
  drawnHistoryNum = drawnFutureNum = 0;

  updateLine(actualLine = 0);
  updateFrame((actualFrame = 1) - 1);
  updateTime(actualTime = 0.0);

  i = atoi(headerDimension);
  numProcessors = (1 << i);

  /* Find maximal number of process slots... */
  numProcesses = 0;
  for(i=0; i<numProcessors; ++i) {
    if(numSlots[i] > numProcesses) {
      numProcesses = numSlots[i];
    }
  }
  DBUG_PRINT("localStartModule", ("numProcesses=%d", numProcesses));
  rescaleGraphic();

  for(i=0; i<numProcessors; ++i) {
    for(j=0; j<numProcesses; ++j) {
      processes[i][j].fatherProcessor = processes[i][j].fatherProcess = -1;
      processes[i][j].state = TERMINATED;
    }

    loadOMeter[i].load = loadOMeter[i].comm = 0.0;
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
/*  msDisplayPanelRedraw: (callback)                                         */
/*                                                                           */
/*  Redraw the plot...                                                       */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelRedraw(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msDisplayPanelRedraw");

  redrawData();

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
/*  msDisplayPanelSetStyleNone: (callback)                                   */
/*                                                                           */
/*  Set Style (text, color) for plot...                                      */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetStyleNone(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetStyleNone");

  GET_TOGGLE_STATE(WI_ms_display_panel_none_style, &state);
  if(state) {
    actualStyle = NONE;
    UNSET_TOGGLE(WI_ms_display_panel_text_style, FALSE);
    UNSET_TOGGLE(WI_ms_display_panel_color_style, FALSE);

    redrawData();
  } else {
    SET_TOGGLE(WI_ms_display_panel_none_style, FALSE);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetStyleText: (callback)                                   */
/*                                                                           */
/*  Set Style (text, color) for plot...                                      */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetStyleText(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetStyleText");

  GET_TOGGLE_STATE(WI_ms_display_panel_text_style, &state);
  if(state) {
    actualStyle = TEXT;
    UNSET_TOGGLE(WI_ms_display_panel_none_style, FALSE);
    UNSET_TOGGLE(WI_ms_display_panel_color_style, FALSE);

    redrawData();
  } else {
    SET_TOGGLE(WI_ms_display_panel_text_style, FALSE);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetStyleColor: (callback)                                  */
/*                                                                           */
/*  Set Style (text, color) for plot...                                      */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetStyleColor(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetStyleColor");

  GET_TOGGLE_STATE(WI_ms_display_panel_color_style, &state);
  if(state) {
    actualStyle = COLOR;
    UNSET_TOGGLE(WI_ms_display_panel_none_style, FALSE);
    UNSET_TOGGLE(WI_ms_display_panel_text_style, FALSE);

    redrawData();
  } else {
    SET_TOGGLE(WI_ms_display_panel_color_style, FALSE);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetMeter: (callback)                                       */
/*                                                                           */
/*  Turn on/off Load-O-Meter...                                              */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetMeter(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetMeter");

  GET_TOGGLE_STATE(WI_ms_display_panel_set_load_o_meter, &state);
  actualMeter = state;
  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcArrayMousePosition: (action)                                       */
/*                                                                           */
/*  Get position of mouse and display it...                                  */
/*                                                                           */
/*****************************************************************************/

void msProcArrayMousePosition(w, event, params, num_params, action_name)
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
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msProcArrayMousePosition");

  XQueryPointer(display, window, &rootWindow, &myWindow,
		&rootX, &rootY, &myX, &myY, &mask);

  DBUG_PRINT("msProcArrayMousePosition", ("x=%d y=%d", myX, myY));

  x = ((double)(myX-leftMargin))/xFactor+xMin;
  y = ((double)(myY-topMargin))/yFactor+yMin;

  DBUG_PRINT("msProcArrayMousePosition", ("x=%lf y=%lf", x, y));

  if(x < xMin) {
    SET_LABEL(WI_ms_cursor_panel_show_x, "------");
    pointedProcess = -1;
  } else {
    pointedProcess = (int)(x*X_SLOT_FACTOR/xSlotSize);
    if(pointedProcess >= numProcesses) {
      SET_LABEL(WI_ms_cursor_panel_show_x, "------");
      pointedProcess = -1;
    } else {
      sprintf(temp, "%5d", pointedProcess);
      SET_LABEL(WI_ms_cursor_panel_show_x, temp);
    }
  }

  if(y < yMin) {
    SET_LABEL(WI_ms_cursor_panel_show_y, "------");
    pointedProcessor = -1;
  } else {
    pointedProcessor = (int)(y*Y_SLOT_FACTOR/ySlotSize);
    if(pointedProcessor >= numProcessors) {
      SET_LABEL(WI_ms_cursor_panel_show_y, "------");
      pointedProcessor = -1;
    } else {
      sprintf(temp, "%5d", pointedProcessor);
      SET_LABEL(WI_ms_cursor_panel_show_y, temp);
    }
  }

  DBUG_PRINT("msProcArrayMousePosition", ("processor=%d process=%d",
					  pointedProcessor, pointedProcess));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcArrayClickLeft: (action)                                           */
/*                                                                           */
/*  Left mouse button clicked --> draw history...                            */
/*                                                                           */
/*****************************************************************************/

void msProcArrayClickLeft(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcArrayClickLeft");

  drawHistory(pointedProcessor, pointedProcess, actualHistory);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcArrayClickMiddle: (action)                                         */
/*                                                                           */
/*  Middle mouse button clicked --> redraw plot...                           */
/*                                                                           */
/*****************************************************************************/

void msProcArrayClickMiddle(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcArrayClickMiddle");

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcArrayClickRight: (action)                                          */
/*                                                                           */
/*  Right mouse button clicked --> draw future...                            */
/*                                                                           */
/*****************************************************************************/

void msProcArrayClickRight(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcArrayClickRight");

  drawFuture(pointedProcessor, pointedProcess, actualFuture);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHistoryPanelEnterLevel: (callback)                                     */
/*                                                                           */
/*  Enter history level...                                                   */
/*                                                                           */
/*****************************************************************************/

void msHistoryPanelEnterLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];
  int tempHistory;

  DBUG_ENTER("msHistoryPanelEnterLevel");

  GET_LABEL(WI_ms_history_panel_enter_level, temp);
  tempHistory = atoi(temp);

  if((tempHistory >= MIN_HISTORY) && (tempHistory <= MAX_HISTORY)) {
    actualHistory = tempHistory;
    SET_POSITION(WI_ms_history_panel_scroll, actualHistory);
  } else {
    sprintf(temp, "%d", actualHistory);
    SET_LABEL(WI_ms_history_panel_enter_level, temp);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHistoryPanelDecreaseLevel: (callback)                                  */
/*                                                                           */
/*  Decrease value for history level...                                      */
/*                                                                           */
/*****************************************************************************/

void msHistoryPanelDecreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msHistoryPanelDecreaseLevel");

  if(actualHistory > MIN_HISTORY) {
    sprintf(temp, "%d", --actualHistory);
    SET_LABEL(WI_ms_history_panel_enter_level, temp);
    SET_POSITION(WI_ms_history_panel_scroll, actualHistory);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHistoryPanelIncreaseLevel: (callback)                                  */
/*                                                                           */
/*  Increase value for history level...                                      */
/*                                                                           */
/*****************************************************************************/

void msHistoryPanelIncreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msHistoryPanelIncreaseLevel");

  if(actualHistory < MAX_HISTORY) {
    sprintf(temp, "%d", ++actualHistory);
    SET_LABEL(WI_ms_history_panel_enter_level, temp);
    SET_POSITION(WI_ms_history_panel_scroll, actualHistory);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msHistoryPanelSetLevel: (callback)                                       */
/*                                                                           */
/*  Set history level...                                                     */
/*                                                                           */
/*****************************************************************************/

void msHistoryPanelSetLevel(w, which, position)
Widget w;
XtPointer *which;
float *position;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msHistoryPanelSetLevel");

  sprintf(temp, "%d",
	  actualHistory = *position*(MAX_HISTORY-MIN_HISTORY)+MIN_HISTORY);
  SET_LABEL(WI_ms_history_panel_enter_level, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFuturePanelEnterLevel: (callback)                                      */
/*                                                                           */
/*  Enter future level...                                                    */
/*                                                                           */
/*****************************************************************************/

void msFuturePanelEnterLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];
  int tempFuture;

  DBUG_ENTER("msFuturePanelEnterLevel");

  GET_LABEL(WI_ms_future_panel_enter_level, temp);
  tempFuture = atoi(temp);

  if((tempFuture >= MIN_FUTURE) && (tempFuture <= MAX_FUTURE)) {
    actualFuture = tempFuture;
    SET_POSITION(WI_ms_future_panel_scroll, actualFuture);
  } else {
    sprintf(temp, "%d", actualFuture);
    SET_LABEL(WI_ms_future_panel_enter_level, temp);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFuturePanelDecreaseLevel: (callback)                                   */
/*                                                                           */
/*  Decrease value for future level...                                       */
/*                                                                           */
/*****************************************************************************/

void msFuturePanelDecreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msFuturePanelDecreaseLevel");

  if(actualFuture > MIN_FUTURE) {
    sprintf(temp, "%d", --actualFuture);
    SET_LABEL(WI_ms_future_panel_enter_level, temp);
    SET_POSITION(WI_ms_future_panel_scroll, actualFuture);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFuturePanelIncreaseLevel: (callback)                                   */
/*                                                                           */
/*  Increase value for future level...                                       */
/*                                                                           */
/*****************************************************************************/

void msFuturePanelIncreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msFuturePanelIncreaseLevel");

  if(actualFuture < MAX_FUTURE) {
    sprintf(temp, "%d", ++actualFuture);
    SET_LABEL(WI_ms_future_panel_enter_level, temp);
    SET_POSITION(WI_ms_future_panel_scroll, actualFuture);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFuturePanelSetLevel: (callback)                                        */
/*                                                                           */
/*  Set future level...                                                      */
/*                                                                           */
/*****************************************************************************/

void msFuturePanelSetLevel(w, which, position)
Widget w;
XtPointer *which;
float *position;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msFuturePanelSetLevel");

  sprintf(temp, "%d",
	  actualFuture = *position*(MAX_FUTURE-MIN_FUTURE)+MIN_FUTURE);
  SET_LABEL(WI_ms_future_panel_enter_level, temp);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  The following functions are the graphics primitives...                   */
/*                                                                           */
/*****************************************************************************/


void SET_COLORS(r, a, s, t)                 /* Set colors for process-states */
int r, a, s, t;
{
  DBUG_ENTER("SET_COLORS");

  readyColor = r;
  activeColor = a;
  suspendedColor = s;
  terminatedColor = t;
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
    redrawData();
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


void CREATION_ARROW(fromProcessor, fromProcess,
		    toProcessor, toProcess)           /* Draw creation arrow */
short fromProcessor, fromProcess, toProcessor, toProcess;
{
  struct Element *temp;

  DBUG_ENTER("CREATION_ARROW");

  drawLine(fromProcessor, fromProcess, toProcessor, toProcess, CREATE, FALSE);

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->action = CREATE;
  temp->s[0] = fromProcessor;
  temp->s[1] = fromProcess;
  temp->s[2] = toProcessor;
  temp->s[3] = toProcess;

  DBUG_PRINT("CREATE", 
	     ("fromProcessor=%d fromProcess=%d toProcessor=%d toProcess=%d",
	      fromProcessor, fromProcess, toProcessor, toProcess));

  DBUG_VOID_RETURN;
}


void FILL_SLOT(processor, process, fatherProcessor, fatherProcess,
	       state)          /* Fill specified slot according to its state */
short processor, process, fatherProcessor, fatherProcess, state;
{
  struct Element *temp;

  DBUG_ENTER("FILL_SLOT");

  if(fatherProcessor != NOCARE) {
    processes[processor][process].fatherProcessor = fatherProcessor;
  }
  if(fatherProcess != NOCARE) {
    processes[processor][process].fatherProcess = fatherProcess;
  }

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->action = FILL;
  temp->s[0] = processor;
  temp->s[1] = process;
  temp->s[2] = processes[processor][process].state;
  temp->s[3] = state;
  temp->s[4] = processes[processor][process].fatherProcessor;
  temp->s[5] = processes[processor][process].fatherProcess;

  fillSlot(processor, process, state);

  DBUG_PRINT("FILL_SLOT", ("processor=%d process=%d", processor, process));
  DBUG_PRINT("FILL_SLOT", ("fatherProcessor=%d fatherProcess=%d state=%d",
			   fatherProcessor, fatherProcess, state));

  DBUG_VOID_RETURN;
}


void SHOW_HISTORY(y, x, l)              /* Show history of specified process */
short y, x, l;
{
  struct Element *temp;

  DBUG_ENTER("SHOW_HISTORY");

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->action = FUTURE;
  temp->s[0] = y;
  temp->s[1] = x;
  temp->s[2] = l;

  drawHistory(y, x, l);

  DBUG_VOID_RETURN;
}


void SHOW_FUTURE(y, x, l)                /* Show future of specified process */
short y, x, l;
{
  struct Element *temp;

  DBUG_ENTER("SHOW_FUTURE");

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->action = FUTURE;
  temp->s[0] = y;
  temp->s[1] = x;
  temp->s[2] = l;

  drawFuture(y, x, l);

  DBUG_VOID_RETURN;
}


void SET_STYLE(s)                   /* Set display style (none, text, color) */
short s;
{
  DBUG_ENTER("SET_STYLE");

  switch(s) {
  case NONE:
    msDisplayPanelSetStyleNone();
    break;
  case TEXT:
    msDisplayPanelSetStyleText();
    break;
  case COLOR:
    msDisplayPanelSetStyleColor();
    break;
  }

  DBUG_VOID_RETURN;
}


void SET_METER(b)                                /* Turn on/off Load-O-Meter */
short b;
{
  DBUG_ENTER("SET_METER");

  if(b == ON) {
    SET_TOGGLE(WI_ms_display_panel_set_load_o_meter, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_set_load_o_meter, TRUE);
  }

  DBUG_VOID_RETURN;
}


void DRAW_METER(p, l, c, i)                      /* Draw/update Load-O-Meter */
int p;
double l, c, i;
{
  struct Element *temp;
  double loadFactor, commFactor, total;

  DBUG_ENTER("DRAW_METER");

  total = l + c + i;
  loadFactor = l / total;
  commFactor = c / total;

  DBUG_PRINT("DRAW_METER", ("loadFactor=%lf commFactor=%lf",
			   loadFactor, commFactor));

  loadOMeter[p].load = (short)((double)(LOADOMETER_SIZE) * loadFactor);
  loadOMeter[p].comm = (short)((double)(LOADOMETER_SIZE) * commFactor);

  DBUG_PRINT("DRAW_METER", ("loadWidth=%d commWidth=%d",
			    loadOMeter[p].load, loadOMeter[p].comm));

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->action = LOADOMETER;
  temp->s[0] = p;
  temp->s[1] = loadOMeter[p].load;
  temp->s[2] = loadOMeter[p].comm;

  if(actualMeter) {
    drawMeter(p);
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
