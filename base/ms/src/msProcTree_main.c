#include "msProcTree_main.h"


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
          gcNodist = (GC)NULL,
          gcHome = (GC)NULL,
          gcDistributed = (GC)NULL,
          gcText = (GC)NULL,
          gcNode = (GC)NULL;
static int nodistColor, homeColor, distributedColor, nodeColor;

static Boolean mustRedraw, actualDisplayNodes, actualDisplayLabels;
static char actualTitle[TITLE_LEN];

static int actualLevel;


/* Graphics primitives */
void SET_TITLE();                                    /* Set title of graphic */
void NEXT_FRAME();                        /* End actual frame, begin new one */
void NEXT_FRAME_BUT_CONTINUE();        /* Continue with the CONTINUE-case in */
                                      /* the script after doing NEXT_FRAME() */
void ADD_NODE();                         /* Insert node under specified node */
void REMOVE_NODE();                                 /* Remove specified node */
void SET_COLORS();                        /* Set colors for home/distributed */
void SET_NODES();                              /* Toggle on/off node display */
void SET_LABELS();                            /* Toggle on/off label display */
void SET_DEPTH();                        /* Set number of levels to be shown */
void ZOOM_IN();                                    /* Zoom to specified node */
void ZOOM_OUT();                   /* Zoom to predecessor of actual top node */
void SHOW_TOP();                   /* Zoom to predecessor of global top node */
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

struct {
  short numSons;
  int daddy, firstSon, nextBrother;
  short xPosition, yPosition;
  int level;
  double offset, range;
  char mode;
  char label[10];
} nodes[MAX_NODES];

int globalTopNode, displayedTopNode, pointedNode;

static int numProcessors, numProcesses;


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
/*  drawTreePS: (local function)                                             */
/*                                                                           */
/*  Show tree under specified node... (Postscript)                           */
/*                                                                           */
/*****************************************************************************/

void drawTreePS(psFile, node, level, offset, range)
FILE *psFile;
int node, level;
double offset, range;
{
  int numSons, nextNode, i;
  double newRange, newOffset;

  DBUG_ENTER("drawTreePS");

  if((node != NOCARE) && (level < actualLevel-1)) {
    nodes[node].xPosition = xTransform(offset);
    nodes[node].yPosition = ySize - yTransform((yRange / (actualLevel+1)) *
					       (level+1));

    nodes[node].level = level;
    nodes[node].offset = offset;
    nodes[node].range = range;

    if(actualDisplayNodes) {
      int xFrom = nodes[node].xPosition-5;
      int xTo = nodes[node].xPosition+5;
      int yFrom = nodes[node].yPosition-5;
      int yTo = nodes[node].yPosition+5;

      fprintf(psFile, "%.2f setgray\n", PS_MARKER_COLOR);

      fprintf(psFile, "%d %d moveto %d %d lineto %d %d lineto ",
	      xFrom, yFrom, xFrom, yTo, xTo, yTo);
      fprintf(psFile, "%d %d lineto %d %d lineto stroke\n",
	      xTo, yFrom, xFrom, yFrom);
    }

    if(actualDisplayLabels) {
      fprintf(psFile, "%.2f setgray\n", PS_LABEL_COLOR);

      fprintf(psFile, "/Helvetica findfont [12 0 0 12 0 0] makefont setfont ");
      fprintf(psFile, "%d %d moveto (%s) ccshow\n",
	      nodes[node].xPosition + 5, nodes[node].yPosition + 10,
	      nodes[node].label);
    }

    if(node != displayedTopNode) {
      switch(nodes[node].mode) {
      case NODIST:
	fprintf(psFile, "%.2f setgray\n", PS_NODIST_COLOR);
	break;
      case HOME:
	fprintf(psFile, "%.2f setgray\n", PS_HOME_COLOR);
	break;
      case DISTRIBUTED:
	fprintf(psFile, "%.2f setgray\n", PS_DISTRIBUTED_COLOR);
	break;
      }

      fprintf(psFile, "%d %d moveto %d %d lineto stroke\n",
	      nodes[node].xPosition, nodes[node].yPosition,
	      nodes[nodes[node].daddy].xPosition,
	      nodes[nodes[node].daddy].yPosition);
    }
    
    if((numSons = nodes[node].numSons) && (level < actualLevel-1)) {
      nextNode = nodes[node].firstSon;
      
      if(numSons > 1) {
	newRange = range / numSons;
	newOffset = newRange/2;

	for(i=0; i<numSons; ++i) {
	  drawTreePS(psFile, nextNode, level+1,
		     offset - range/2 + newOffset + i*newRange, newRange);
	  nextNode = nodes[nextNode].nextBrother;
	}
      } else {
	drawTreePS(psFile, nextNode, level+1, offset, range);
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

    drawTreePS(psFile, displayedTopNode, 0, 0.0, xRange);


    fprintf(psFile, "grestore\n");
    fprintf(psFile, "showpage\n");    
    fclose(psFile);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  xLeft: (local function)                                                  */
/*                                                                           */
/*  Get left x coordinate for a slot...                                     */
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
/*  drawTree: (local function)                                               */
/*                                                                           */
/*  Show tree under specified node...                                        */
/*                                                                           */
/*****************************************************************************/

void drawTree(node, level, offset, range)
int node, level;
double offset, range;
{
  int numSons, nextNode, i;
  double newRange, newOffset;
  XTextItem textItem[1];


  DBUG_ENTER("drawTree");

  if((node != NOCARE) && (level < actualLevel-1)) {
    DBUG_PRINT("drawTree", ("node=%d level=%d offset=%lf range=%lf",
			    node, level, offset, range));

    if(doUpdates) {
      if(!level) {
	XClearWindow(display, window);

	if(strlen(actualTitle)) {
	  textItem[0].chars = actualTitle;
	  textItem[0].nchars = strlen(actualTitle);
	  textItem[0].delta = 10;
	  textItem[0].font = None;

	  XDrawText(display, window, gcBlack,
		    xSize/2 - strlen(actualTitle)*3, 18, textItem, 1);
	}

	displayedTopNode = node;

	for(i=0; i<MAX_NODES; ++i) {
	  nodes[i].xPosition = nodes[i].yPosition = nodes[i].level = NOCARE;
	}
      }

      nodes[node].xPosition = xTransform(offset);
      nodes[node].yPosition = yTransform((yRange / (actualLevel+1)) *
					 (level+1));

      nodes[node].level = level;
      nodes[node].offset = offset;
      nodes[node].range = range;

      if(actualDisplayNodes) {
	XDrawArc(display, window, gcNode,
		 nodes[node].xPosition-5, nodes[node].yPosition-5,
		 10, 10, 0, 23040);
      }

      if(actualDisplayLabels) {
	textItem[0].chars = nodes[node].label;
	textItem[0].nchars = strlen(nodes[node].label);
	textItem[0].delta = 10;
	textItem[0].font = None;

	XDrawText(display, window, gcText,
		  nodes[node].xPosition - 5,
		  nodes[node].yPosition - 10, textItem, 1);
      }

      if(node != displayedTopNode) {
	XDrawLine(display, window,
		  (nodes[node].mode == (char)NODIST ? gcNodist :
		   (nodes[node].mode == (char)HOME ? gcHome : gcDistributed)),
		  nodes[node].xPosition, nodes[node].yPosition,
		  nodes[nodes[node].daddy].xPosition,
		  nodes[nodes[node].daddy].yPosition);
      }
    
      if((numSons = nodes[node].numSons) && (level < actualLevel-1)) {
	nextNode = nodes[node].firstSon;
      
	if(numSons > 1) {
	  newRange = range / numSons;
	  newOffset = newRange/2;

	  for(i=0; i<numSons; ++i) {
	    drawTree(nextNode, level+1,
		     offset - range/2 + newOffset + i*newRange, newRange);
	    nextNode = nodes[nextNode].nextBrother;
	  }
	} else {
	  drawTree(nextNode, level+1, offset, range);
	}
      }

      if(!level) {
	XSync(display, FALSE);
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  addNode: (local function)                                                */
/*                                                                           */
/*  Add new node under specified node...                                     */
/*                                                                           */
/*****************************************************************************/

void addNode(node, daddy, mode, label)
int node, daddy, mode;
char *label;
{
  int temp;

  DBUG_ENTER("addNode");

  nodes[node].daddy = daddy;
  nodes[node].numSons = 0;
  nodes[node].mode = (char)mode;
  strcpy(nodes[node].label, label);

  if(daddy == NOCARE) {
    /* 1st node... */
    globalTopNode = node;

    drawTree(globalTopNode, 0, 0.0, xRange);
  } else {
    if(nodes[daddy].numSons) {
      if(nodes[daddy].level != NOCARE) {
	/* Daddy is visible --> first erase old (sub-)tree... */
	drawTree(daddy,
		 nodes[daddy].level, nodes[daddy].offset, nodes[daddy].range);
      }
    }

    /* Insert new node... */
    if(++nodes[daddy].numSons > 1) {
      /* There are brothers of the new node... */
      temp = nodes[daddy].firstSon;
      nodes[daddy].firstSon = node;

      nodes[node].nextBrother = temp;
    } else {
      /* The new node is the first son... */
      nodes[daddy].firstSon = node;
    }

    if(nodes[daddy].level != NOCARE) {
      /* Daddy is visible --> draw new tree... */
      if(nodes[daddy].numSons > 1) {
	drawTree(daddy,
		 nodes[daddy].level, nodes[daddy].offset, nodes[daddy].range);
      } else {
	drawTree(node, nodes[daddy].level+1,
		 nodes[daddy].offset, nodes[daddy].range);
      }
    }
  } 
  
  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  removeNode: (local function)                                             */
/*                                                                           */
/*  Remove specified node...                                                 */
/*                                                                           */
/*****************************************************************************/

void removeNode(node)
int node;
{
  int actual, next, i;
  int daddy = nodes[node].daddy;

  DBUG_ENTER("removeNode");

  /* Clear this (sub-)tree... */
  drawTree(daddy,
	   nodes[daddy].level, nodes[daddy].offset, nodes[daddy].range);

  /* First remove all subnodes... */
  next = nodes[node].firstSon;
  for(i=0; i<nodes[node].numSons; ++i) {
    actual = next;
    next = nodes[actual].nextBrother;

    REMOVE_NODE(actual);
  }

  /* Now remove the node itself... */
  if(node == globalTopNode) {
    globalTopNode = NOCARE;
  } else {
    next = nodes[node].nextBrother;

    if(--nodes[daddy].numSons) {
      if(nodes[daddy].firstSon == node) {
	nodes[daddy].firstSon = next;
      } else {
	i = nodes[daddy].firstSon;
	while(nodes[i].nextBrother != node) {
	  i = nodes[i].nextBrother;
	}
	nodes[i].nextBrother = next;
      }
    }
  }

  /* Finally draw the new tree... */
  drawTree(daddy,
	   nodes[daddy].level, nodes[daddy].offset, nodes[daddy].range);

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

  GET_SIZE(WI_ms_proc_tree_graphic_area, &width, &height);

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

  drawTree(displayedTopNode, 0, 0.0, xRange);

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
  Boolean wasAdd;
  int node, daddy, mode;
  char *label;
 
  DBUG_ENTER("recallCommand");

  wasAdd = ((struct Element *)data)->wasAdd;
  node = ((struct Element *)data)->node;
  daddy = ((struct Element *)data)->daddy;
  mode = (int)((struct Element *)data)->mode;
  label = ((struct Element *)data)->label;
  
  if(wasAdd) {
    if(forward) {
      addNode(node, daddy, mode, label);
    } else {
      removeNode(node);
    }
  } else {
    if(forward) {
      removeNode(node);
    } else {
      addNode(node, daddy, mode, label);
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

  window = XtWindow(WI_ms_proc_tree_graphic_area);

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

  if(gcNodist == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = colors[nodistColor].pixel;
    values.background = WhitePixel(display, screen);
    values.line_width = 1;
    gcNodist = XCreateGC(display, window, valuemask, &values);
  }

  if(gcHome == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = colors[homeColor].pixel;
    values.background = WhitePixel(display, screen);
    values.line_width = 1;
    gcHome = XCreateGC(display, window, valuemask, &values);
  }

  if(gcDistributed == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = colors[distributedColor].pixel;
    values.background = WhitePixel(display, screen);
    values.line_width = 1;
    gcDistributed = XCreateGC(display, window, valuemask, &values);
  }

  if(gcText == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = BlackPixel(display, screen);
    values.background = WhitePixel(display, screen);
    values.line_width = 1;
    gcText = XCreateGC(display, window, valuemask, &values);
  }

  if(gcNode == (GC)NULL) {
    valuemask = GCFunction | GCForeground | GCBackground | GCLineWidth;
    values.function = GXxor;
    values.foreground = colors[nodeColor].pixel;
    values.background = WhitePixel(display, screen);
    values.line_width = 1;
    gcNode = XCreateGC(display, window, valuemask, &values);
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
  char temp[DISPLAY_LEN];

  DBUG_ENTER("localStartupHook");

  moduleWindow = WI_ms_proc_tree;
  strcpy(prefsName, "procTree.prefs");

  highlightButton(WI_ms_control_panel_stop);

  xMin = X_MIN - (X_MAX-X_MIN)/2;
  yMin = Y_MIN;
  xMax = X_MIN + (X_MAX-X_MIN)/2;
  yMax = Y_MAX;

  numProcessors = MAX_PROCESSORS;
  numProcesses = MAX_PROCESSES;

  rescaleGraphic();

  nodistColor = LIGHT_YELLOW;
  homeColor = BLUE;
  distributedColor = GREEN;
  nodeColor = LIGHT_RED;
  localInitColors();

  actualDisplayNodes = TRUE;
  actualDisplayLabels = FALSE;
  SET_TOGGLE(WI_ms_display_panel_set_node, FALSE);
  UNSET_TOGGLE(WI_ms_display_panel_set_label, FALSE);

  globalTopNode = displayedTopNode = NOCARE;

  strcpy(selectedScript, "ProcTreeSCRs/");

  sprintf(temp, "%d", actualLevel = 20);
  SET_LABEL(WI_ms_zoom_panel_enter_level, temp);
  SET_POSITION(WI_ms_zoom_panel_scroll, actualLevel);

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
  XFreeGC(display, gcNodist);
  XFreeGC(display, gcHome);
  XFreeGC(display, gcDistributed);
  XFreeGC(display, gcText);
  XFreeGC(display, gcNode);

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
  short i;

  DBUG_ENTER("localStartModule");

  mustRedraw = FALSE;

  updateLine(actualLine = 0);
  updateFrame((actualFrame = 1) - 1);
  updateTime(actualTime = 0.0);

  i = atoi(headerDimension);
  numProcessors = (1 << i);

  /* ..... */

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

    msFileBrowserInitialize(2, "Save postscript...", EPS_MASK, "./",
			    "plot.eps");
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
/*  msDisplayPanelSetNode: (callback)                                        */
/*                                                                           */
/*  Toggle on/off displaying of nodes...                                     */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetNode(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetNode");

  GET_TOGGLE_STATE(WI_ms_display_panel_set_node, &actualDisplayNodes);

  state = doUpdates;
  doUpdates = TRUE;
  redrawData();
  doUpdates = state;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msDisplayPanelSetLabel: (callback)                                       */
/*                                                                           */
/*  Toggle on/off displaying of labels...                                    */
/*                                                                           */
/*****************************************************************************/

void msDisplayPanelSetLabel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msDisplayPanelSetLabel");

  GET_TOGGLE_STATE(WI_ms_display_panel_set_label, &actualDisplayLabels);

  state = doUpdates;
  doUpdates = TRUE;
  redrawData();
  doUpdates = state;

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
/*  msProcTreeMousePosition: (action)                                        */
/*                                                                           */
/*  Get position of mouse and display it...                                  */
/*                                                                           */
/*****************************************************************************/

void msProcTreeMousePosition(w, event, params, num_params, action_name)
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

  int i, nearestNode;
  double smallestDistance, actualDistance, xDistance, yDistance;

  DBUG_ENTER("msProcTreeMousePosition");

  XQueryPointer(display, window, &rootWindow, &myWindow,
		&rootX, &rootY, &myX, &myY, &mask);

  DBUG_PRINT("msProcTreeMousePosition", ("x=%d y=%d", myX, myY));

  x = ((double)(myX-leftMargin))/xFactor+xMin;
  y = ((double)(myY-topMargin))/yFactor+yMin;

  DBUG_PRINT("msProcTreeMousePosition", ("x=%lf y=%lf", x, y));

  nearestNode = NOCARE;
  smallestDistance = 100000.0;
  for(i=0; i<MAX_NODES; ++i) {
    if((nodes[i].xPosition != NOCARE) && (nodes[i].yPosition != NOCARE)) {
      xDistance = (double)myX - (double)nodes[i].xPosition;
      yDistance = (double)myY - (double)nodes[i].yPosition;
      actualDistance = sqrt(xDistance*xDistance + yDistance*yDistance);

      if((actualDistance < smallestDistance) && 
	 (actualDistance < CLICK_TOLERANCE)) {
	smallestDistance = actualDistance;
	nearestNode = i;
      }
    }
  }

  DBUG_PRINT("msProcTreeMousePosition", ("node=%d", nearestNode));

  pointedNode = nearestNode;

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelZoomOut: (callback)                                           */
/*                                                                           */
/*  Zoom to predecessor of actual top node...                                */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelZoomOut(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msZoomPanelZoomOut");

  if(displayedTopNode != globalTopNode) {
    displayedTopNode = nodes[displayedTopNode].daddy;
  }

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelShowTop: (callback)                                           */
/*                                                                           */
/*  Zoom to global top node...                                               */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelShowTop(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msZoomPanelShowTop");

  displayedTopNode = globalTopNode;
  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcTreeClickLeft: (action)                                            */
/*                                                                           */
/*  Left mouse button clicked --> Zoom to selected node...                   */
/*                                                                           */
/*****************************************************************************/

void msProcTreeClickLeft(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcTreeClickLeft");

  /* Get new coordinates... */
  msProcTreeMousePosition();

  if(pointedNode != NOCARE) {
    displayedTopNode = pointedNode;
    redrawData();
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcTreeClickMiddle: (action)                                          */
/*                                                                           */
/*  Middle mouse button clicked --> Show the whole tree...                   */
/*                                                                           */
/*****************************************************************************/

void msProcTreeClickMiddle(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcTreeClickMiddle");

  msZoomPanelShowTop();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msProcTreeClickRight: (action)                                           */
/*                                                                           */
/*  Right mouse button clicked --> Zoom out to parent of actual top node...  */
/*                                                                           */
/*****************************************************************************/

void msProcTreeClickRight(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  DBUG_ENTER("msProcTreeClickRight");

  msZoomPanelZoomOut();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelEnterLevel: (callback)                                        */
/*                                                                           */
/*  Enter display level...                                                   */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelEnterLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];
  int tempLevel;

  DBUG_ENTER("msZoomPanelEnterLevel");

  GET_LABEL(WI_ms_zoom_panel_enter_level, temp);
  tempLevel = atoi(temp);

  if((tempLevel >= MIN_LEVEL) && (tempLevel <= MAX_LEVEL)) {
    actualLevel = tempLevel;
    SET_POSITION(WI_ms_zoom_panel_scroll, actualLevel);
  } else {
    sprintf(temp, "%d", actualLevel);
    SET_LABEL(WI_ms_zoom_panel_enter_level, temp);
  }

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelDecreaseLevel: (callback)                                     */
/*                                                                           */
/*  Decrease value for Zoom level...                                         */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelDecreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msZoomPanelDecreaseLevel");

  if(actualLevel > MIN_LEVEL) {
    sprintf(temp, "%d", --actualLevel);
    SET_LABEL(WI_ms_zoom_panel_enter_level, temp);
    SET_POSITION(WI_ms_zoom_panel_scroll, actualLevel);
  }

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelIncreaseLevel: (callback)                                     */
/*                                                                           */
/*  Increase value for Zoom level...                                         */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelIncreaseLevel(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msZoomPanelIncreaseLevel");

  if(actualLevel < MAX_LEVEL) {
    sprintf(temp, "%d", ++actualLevel);
    SET_LABEL(WI_ms_zoom_panel_enter_level, temp);
    SET_POSITION(WI_ms_zoom_panel_scroll, actualLevel);
  }

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msZoomPanelSetLevel: (callback)                                          */
/*                                                                           */
/*  Set Zoom level...                                                        */
/*                                                                           */
/*****************************************************************************/

void msZoomPanelSetLevel(w, which, position)
Widget w;
XtPointer *which;
float *position;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("msZoomPanelSetLevel");

  sprintf(temp, "%d",
	  actualLevel = *position*(MAX_LEVEL-MIN_LEVEL)+MIN_LEVEL);
  SET_LABEL(WI_ms_zoom_panel_enter_level, temp);

  redrawData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  The following functions are the graphics primitives...                   */
/*                                                                           */
/*****************************************************************************/


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


void ADD_NODE(node, daddy, where, label) /* Insert node under specified node */
int node, daddy, where;
char *label;
{
  struct Element *temp; 
  
  DBUG_ENTER("ADD_NODE");

  DBUG_PRINT("ADD_NODE", ("node=%d daddy=%d where=%d label=%s",
			  node, daddy, where, label));
  DBUG_PRINT("ADD_NODE", ("time=%lf", actualTime));

  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->wasAdd = TRUE;
  temp->node = node;
  temp->daddy = daddy;
  temp->mode = (char)where;
  strcpy(temp->label, label);

  addNode(node, daddy, temp->mode, label);

  DBUG_VOID_RETURN;
}


void REMOVE_NODE(node)                              /* Remove specified node */
int node;
{
  struct Element *temp; 

  DBUG_ENTER("REMOVE_NODE");

  DBUG_PRINT("REMOVE_NODE", ("node=%d", node));
 
  temp = (struct Element *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->wasAdd = FALSE;
  temp->node = node;
  temp->daddy = nodes[node].daddy;
  temp->mode = nodes[node].mode;
  strcpy(temp->label, nodes[node].label);

  removeNode(node);

  drawTree(globalTopNode, 0, 0.0, xRange);

  DBUG_VOID_RETURN;
}


void SET_COLORS(h1, h2, d, n)               /* Set colors for process-states */
int h1, h2, d, n;
{
  DBUG_ENTER("SET_COLORS");

  nodistColor = h1;
  homeColor = h2;
  distributedColor = d;
  nodeColor = n;
  localInitColors();
  
  updateData();

  DBUG_VOID_RETURN;
}


void SET_NODES(b)                              /* Toggle on/off node display */
short b;
{
  DBUG_ENTER("SET_NODES");

  if(b == ON) {
    SET_TOGGLE(WI_ms_display_panel_set_node, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_set_node, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SET_LABELS(b)                            /* Toggle on/off label display */
Boolean b;
{
  DBUG_ENTER("SET_LABELS");

  if(b) {
    SET_TOGGLE(WI_ms_display_panel_set_label, TRUE);
  } else {
    UNSET_TOGGLE(WI_ms_display_panel_set_label, TRUE);
  }

  DBUG_VOID_RETURN;
}


void SET_DEPTH(n)                        /* Set number of levels to be shown */
int n;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("SET_DEPTH");

  if((n >= MIN_LEVEL) && (n <= MAX_LEVEL)) {
    actualLevel = n;
    SET_POSITION(WI_ms_zoom_panel_scroll, actualLevel);
    sprintf(temp, "%d", actualLevel);
    SET_LABEL(WI_ms_zoom_panel_enter_level, temp);
  }

  DBUG_VOID_RETURN;
}



void ZOOM_IN(n)                                    /* Zoom to specified node */
int n;
{
  DBUG_ENTER("ZOOM_IN");

  displayedTopNode = n;
  redrawData();

  DBUG_VOID_RETURN;
}


void ZOOM_OUT()                    /* Zoom to predecessor of actual top node */
{
  DBUG_ENTER("ZOOM_OUT");

  msZoomPanelZoomOut();

  DBUG_VOID_RETURN;
}


void SHOW_TOP()                    /* Zoom to predecessor of global top node */
{
  DBUG_ENTER("SHOW_TOP");

  msZoomPanelShowTop();

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
