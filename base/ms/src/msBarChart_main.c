#include "msBarChart_main.h"


static struct BarsArray barsArray[MAX_BARS];
static short numBars;
static long numPoints;
static Widget plotter, xAxis, yAxis, line[MAX_PLOTS];
static double *xDataPoints[MAX_PLOTS], *yDataPoints[MAX_PLOTS];
static Boolean xyPlot, actualLegend;
static int actualPlot;
static double actualXMin, actualXMax, actualYMin, actualYMax;
static short actualMode, actualType;


/* Graphics primitives */
void SET_MODE();                             /* Set modus for following plot */
void SET_X_LABEL();       /* Set text to be printed on lower horizontal axis */
void SET_Y_LABEL();          /* Set text to be printed on left vertical axis */
void SET_X_GRID();                        /* Enable/Disable grid over x axis */
void SET_Y_GRID();                        /* Enable/Disable grid over y axis */
void SET_X_MIN();                            /* Set lower boundary of x axis */
void SET_X_MAX();                            /* Set upper boundary of x axis */
void SET_Y_MIN();                            /* Set lower boundary of y axis */
void SET_Y_MAX();                            /* Set upper boundary of y axis */
void SET_LEGEND();                                /* Make legend (in)visible */
void SET_NAME();                             /* Set plot's name (for legend) */
void SET_TITLE();                          /* Set title of the whole picture */
void SET_STYLE();              /* Set plot style (overwrites menu selection) */
void SET_LINESTYLE();       /* Set linestyle (overwrites menu selection) */
void SET_MARKER();            /* Set marker type (overwrites menu selection) */
void SET_SHADE();     /* Set color/shade of bars (overwrites menu selection) */
void SAVE();                                /* Save actual plot to file (ps) */
void PRINT();                                      /* Print actual plot (ps) */
void CLEAR();                                           /* Clear actual plot */
void CREATE_BAR();                    /* Create new bar and specify it's ids */
void INCREMENT();                                   /* Increment bar of id i */
void SET_HEIGHT();                                     /* Set height of id i */
void REMOVE_XY();                        /* Remove point at coordinate (x,?) */
void ADD_XY();                           /* Insert point at coordinate (x,y) */
void UPDATE_XY();                        /* Update point at coordinate (x,?) */
void NEXT_FRAME();                 /* Display actual frame and begin new one */
void NEXT_FRAME_BUT_CONTINUE();        /* Continue with the CONTINUE-case in */
                                      /* the script after doing NEXT_FRAME() */


/* forwards... */
void prepareLineMenu();
void prepareMarkerMenu();
void prepareShadeMenu();

void msBarChartDisplayXSetGrid();
void msBarChartDisplayYSetGrid();

void updateData();
void recallCommand();
int data2Frame();
double data2Time();
void localStartupHook();
void localExitHook();
void localStartModule();
void localDoTick();
Boolean localDoTickGuard();


#include "msModule.c"


static char actualLegendName[DISPLAY_LEN];


/*****************************************************************************/
/*                                                                           */
/*  getZoomData: (local function)                                            */
/*                                                                           */
/*  (Re-)Read the zoom data from plotter...                                  */
/*                                                                           */
/*****************************************************************************/

void getZoomData()
{
  DBUG_ENTER("getZoomData");

  GET_VALUE(xAxis, XtNmin, &actualXMin);
  GET_VALUE(xAxis, XtNmax, &actualXMax);
  GET_VALUE(yAxis, XtNmin, &actualYMin);
  GET_VALUE(yAxis, XtNmax, &actualYMax);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  putZoomData: (local function)                                            */
/*                                                                           */
/*  Write zoom data to fields...                                             */
/*                                                                           */
/*****************************************************************************/

void putZoomData()
{
  char s[DISPLAY_LEN];

  DBUG_ENTER("putZoomData");

  if((double)((long)actualXMin) == actualXMin) {
    sprintf(s, "%ld", (long)actualXMin);
  } else {
    sprintf(s, "%f", actualXMin);
  }
  SET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, s);

  if((double)((long)actualXMax) == actualXMax) {
    sprintf(s, "%ld", (long)actualXMax);
  } else {
    sprintf(s, "%f", actualXMax);
  }
  SET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, s);

  if((double)((long)actualYMin) == actualYMin) {
    sprintf(s, "%ld", (long)actualYMin);
  } else {
    sprintf(s, "%f", actualYMin);
  }
  SET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, s);

  if((double)((long)actualYMax) == actualYMax) {
    sprintf(s, "%ld", (long)actualYMax);
  } else {
    sprintf(s, "%f", actualYMax);
  }
  SET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, s);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateZoomData: (local function)                                         */
/*                                                                           */
/*  Read zoom data from plotter and write it to fields...                    */
/*                                                                           */
/*****************************************************************************/

void updateZoomData()
{
  DBUG_ENTER("updateZoomData");

  getZoomData();
  putZoomData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateData: (local function)                                             */
/*                                                                           */
/*  Attach/Extend data to plot...                                            */
/*                                                                           */
/*****************************************************************************/

void updateData()
{
  DBUG_ENTER("updateData");

  if(scriptIsActive) {
    if(doUpdates) {
      if(xyPlot) {
	/* AtLinePlotExtendData */
	
	AtXYLinePlotAttachData(line[actualPlot], 
			       (XtPointer)xDataPoints[actualPlot],
			       AtDouble, sizeof(double),
			       (XtPointer)yDataPoints[actualPlot],
			       AtDouble, sizeof(double),
			       0, numPoints);
      } else {
	/*AtBarPlotExtendData(line[actualPlot], numBars);*/
      
	AtBarPlotAttachData(line[actualPlot],
			    (XtPointer)yDataPoints[actualPlot],
			    AtDouble, sizeof(double),
			    0, numBars);
      }

      if(lastAction == STOP) {
	updateZoomData();
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  addPoint: (local function)                                               */
/*                                                                           */
/*  Insert one point into array...                                           */
/*                                                                           */
/*****************************************************************************/

void addPoint(x, y)
double x, y;
{
  int i, j;

  DBUG_ENTER("addPoint");

  DBUG_PRINT("addPoint", ("numPoints=%d", numPoints));

  if(numPoints) {
    /* There are points already... */
    if(x >= xDataPoints[actualPlot][numPoints - 1]) {
      /* Append... */
      xDataPoints[actualPlot][numPoints] = x;
      yDataPoints[actualPlot][numPoints] = y;
      if(++numPoints > MAX_POINTS) {
	doRequest("Number of points in plot too high...", "", "Giving up!",
		  "OK", "", "", NULL);
	dieHard("Number of points in plot too high...");
      }
    } else {
      /* Insert... */
      i = 0;
      while(xDataPoints[actualPlot][i] <= x) {
	++i;
      }

      /* Insert at this position... */
      for(j = numPoints; j > i; --j) {
	xDataPoints[actualPlot][j] = xDataPoints[actualPlot][j - 1];
	yDataPoints[actualPlot][j] = yDataPoints[actualPlot][j - 1];
      }
      if(++numPoints > MAX_POINTS) {
	doRequest("Number of points in plot too high...", "", "Giving up!",
		  "OK", "", "", NULL);
	dieHard("Number of points in plot too high...");
      }
      xDataPoints[actualPlot][i] = x;
      yDataPoints[actualPlot][i] = y;
    }
  } else {
    /* First point... */
    xDataPoints[actualPlot][numPoints] = x;
    yDataPoints[actualPlot][numPoints] = y;
    if(++numPoints > MAX_POINTS) {
      doRequest("Number of points in plot too high...", "", "Giving up!",
		"OK", "", "", NULL);
      dieHard("Number of points in plot too high...");
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  removePoint: (local function)                                            */
/*                                                                           */
/*  Delete one point from array...                                           */
/*                                                                           */
/*****************************************************************************/

Boolean removePoint(x, y)
double x, y;
{
  int i;
  
  DBUG_ENTER("removePoint");

  /* Does this x value exists in the data? */
  for(i=0; i<numPoints; ++i) {
    if(xDataPoints[actualPlot][i] == x) {
      break;
    }
  }

  if(i < numPoints) {
    /* Yupp, it does! --> Destroy it... */
    while(++i < numPoints) {
      xDataPoints[actualPlot][i-1] = xDataPoints[actualPlot][i];
      yDataPoints[actualPlot][i-1] = yDataPoints[actualPlot][i];
    }
    --numPoints;

    DBUG_RETURN(TRUE);
  } else {
    DBUG_RETURN(FALSE);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  updatePoint: (local function)                                            */
/*                                                                           */
/*  Update y-value of one point (the one with the specified x) in array...   */
/*                                                                           */
/*****************************************************************************/

Boolean updatePoint(x, y, oldY)
double x, y, *oldY;
{
  int i;
  
  DBUG_ENTER("updatePoint");

  for(i=0; i<numPoints; ++i) {
    if(xDataPoints[actualPlot][i] == x) {
      break;
    }
  }

  if(i < numPoints) {
    if(oldY) {
      /* Return old y value... */
      *oldY = yDataPoints[actualPlot][i];
    }

    yDataPoints[actualPlot][i] = y;

    DBUG_RETURN(TRUE);
  } else {
    /* No match... */
    DBUG_RETURN(FALSE);
  }
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
  DBUG_ENTER("recallCommand");

  if(xyPlot) {
    Boolean wasUpdate = ((struct XYPlotElement *)data)->wasUpdate, wasAdd;
    double x = ((struct XYPlotElement *)data)->x,
           y = ((struct XYPlotElement *)data)->y, y2;

    if(wasUpdate) {
      y2 = ((struct XYPlotElement *)data)->extra.y2;

      if(forward) {
	/* Update (x, ?) to (x, y2) */ 
	updatePoint(x, y2, NULL);
      } else {
	/* Update (x, ?) to (x, y) */ 
	updatePoint(x, y, NULL);
      }
    } else {
      wasAdd = ((struct XYPlotElement *)data)->extra.wasAdd;

      if((forward && wasAdd) || (!forward && !wasAdd)) {
	addPoint(x, y);
      } else {
	removePoint(x, y);
      }
    }
  } else {
    double diff = ((struct BarChartElement *)data)->diff;
    short bar = ((struct BarChartElement *)data)->bar;

    if(forward) {
      yDataPoints[actualPlot][bar - 1] += diff;
    } else {
      yDataPoints[actualPlot][bar - 1] -= diff;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateColor: (local function)                                            */
/*                                                                           */
/*  Update color of current plot...                                          */
/*                                                                           */
/*****************************************************************************/

void updateColor(color)
int color;
{
  DBUG_ENTER("updateColor");

  if(line[actualPlot]) {
    if(color <= colors[0].red) {
      /* Color */
      SET_VALUE(line[actualPlot], XtNfillColor, colors[color].pixel);
    } else {
      /* Gray-Scale */
      switch(color) {
      case GRAY_0:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY0);
	break;
      case GRAY_1:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY1);
	break;
      case GRAY_2:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY2);
	break;
      case GRAY_3:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY3);
	break;
      case GRAY_4:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY4);
	break;
      case GRAY_5:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY5);
	break;
      case GRAY_6:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY6);
	break;
      case GRAY_7:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY7);
	break;
      case GRAY_8:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY8);
	break; 
      case GRAY_9:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY9);
	break;
      case GRAY_10:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY10);
	break;
      default:
	SET_VALUE(line[actualPlot], XtNshading, AtGRAY0);
	break;
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateLinestyle: (local function                                         */
/*                                                                           */
/*  Update line style of current plot...                                     */
/*                                                                           */
/*****************************************************************************/

void updateLinestyle(style)
int style;
{
  DBUG_ENTER("updateLinestyle");

  if(line[actualPlot]) {
    switch(style) {
    case SOLID:
      SET_VALUE(line[actualPlot], XtNplotLineStyle, AtLineSOLID);
      break;
    case DOTTED:
      SET_VALUE(line[actualPlot], XtNplotLineStyle, AtLineDOTTED);
      break;
    case DASHED:
      SET_VALUE(line[actualPlot], XtNplotLineStyle, AtLineDASHED);
      break;
    case DOTDASHED:
      SET_VALUE(line[actualPlot], XtNplotLineStyle, AtLineDOTDASHED);
      break;
    default:
      SET_VALUE(line[actualPlot], XtNplotLineStyle, AtLineSOLID);
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateMarker: (local function)                                           */
/*                                                                           */
/*  Update marker of current plot...                                         */
/*                                                                           */
/*****************************************************************************/

void updateMarker(marker)
int marker;
{
  DBUG_ENTER("updateMarker");

  if(line[actualPlot]) {
    switch(marker) {
    case RECTANGLE:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkRECTANGLE);
      break;
    case PLUS:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkPLUS);
      break;
    case XMARK:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkXMARK);
      break;
    case STAR:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkSTAR);
      break;
    case DIAMOND:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkDIAMOND);
      break;
    case TRIANGLE1:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkTRIANGLE1);
      break;
    case TRIANGLE2:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkTRIANGLE2);
      break;
    case TRIANGLE3:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkTRIANGLE3);
      break;
    case TRIANGLE4:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkTRIANGLE4);
      break;
    default:
      SET_VALUE(line[actualPlot], XtNplotMarkType, AtMarkRECTANGLE);
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  updateMode: (local function)                                             */
/*                                                                           */
/*  Update mode of current plot...                                           */
/*                                                                           */
/*****************************************************************************/

void updateMode(mode)
int mode;
{
  DBUG_ENTER("updateMode");

  if(line[actualPlot]) {
    switch(mode) {
    case LINES:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotLINES);
      updateLinestyle(actualType);
      break;
    case POINTS:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotPOINTS);
      updateMarker(actualType);
      break;
    case IMPULSES:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotIMPULSES);
      updateLinestyle(actualType);
      break;
    case STEPS:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotSTEPS);
      updateLinestyle(actualType);
      break;
    case BARS:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotBARS);
      updateLinestyle(actualType);
      break;
    case LINEPOINTS:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotLINEPOINTS);
      updateLinestyle(SOLID);
      updateMarker(actualType);
      break;
    case LINEIMPULSES:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotLINEIMPULSES);
      updateLinestyle(actualType);
      break;
    case BARCHART:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotBARS);
      updateColor(actualType);
      break;
    default:
      SET_VALUE(line[actualPlot], XtNplotLineType, AtPlotLINES);
      updateLinestyle(actualType);
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msCursorPanelUpdatePosition: (callback)                                  */
/*                                                                           */
/*  Update position display...                                               */
/*                                                                           */
/*****************************************************************************/

void msCursorPanelUpdatePosition(w, client_data, call_data)
Widget w;
XtPointer client_data;
AtPointCallbackData *call_data;
{
  char buffer[DISPLAY_LEN];

  DBUG_ENTER("msCursorPanelUpdatePosition");

  sprintf(buffer, "%.2f", call_data->x1);
  SET_LABEL(WI_ms_cursor_panel_show_x, buffer);
  sprintf(buffer, "%.2f", call_data->y1);
  SET_LABEL(WI_ms_cursor_panel_show_y, buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomDrag (callback)                                            */
/*                                                                           */
/*  Select area to zoom in...                                                */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomDrag(w, client_data, call_data)
Widget w;
XtPointer client_data;
AtRectangleCallbackData *call_data;
{
  double t, x1, x2, y1, y2;
  char s[DISPLAY_LEN];

  DBUG_ENTER("msBarChartZoomDrag");

  x1 = call_data->x11;
  x2 = call_data->x12;
  y1 = call_data->y11;
  y2 = call_data->y12;

  if(x1 > x2) {
    t = x1;
    x1 = x2;
    x2 = t;
  }

  if(y1 > y2) {
    t = y1;
    y1 = y2;
    y2 = t;
  }

  if(((x2 - x1) >= MINIMAL_X_ZOOM) && ((y2 - y1) >= MINIMAL_Y_ZOOM)) {
    /* Do the zoom... */

    actualXMin = call_data->x11;
    actualXMax = call_data->x12;
    actualYMin = call_data->y11;
    actualYMax = call_data->y12;  

    sprintf(s, "%f", actualXMin);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, s);
    sprintf(s, "%f", actualXMax);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, s);
    sprintf(s, "%f", actualYMin);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, s);
    sprintf(s, "%f", actualYMax);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, s);

    SET_BOUNDS(xAxis, actualXMin, actualXMax);
    SET_BOUNDS(yAxis, actualYMin, actualYMax);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartResizePlotter: (action)                                        */
/*                                                                           */
/*  (Re-)size the plotter widget...                                          */
/*                                                                           */
/*****************************************************************************/

void msBarChartResizePlotter(w, event, params, num_params, action_name)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
String action_name;
{
  Dimension width, height;

  DBUG_ENTER("msBarChartResizePlotter");

  GET_SIZE(WI_ms_bar_chart_graphic_box, &width, &height);
  DBUG_PRINT("msBarChartResizePlotter", ("size=%dx%d", width, height));
  SET_SIZE(plotter, width, height);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  attachAxes: (local function)                                             */
/*                                                                           */
/*  Unmanage old axes, manage new ones and attach them to plotter widget...  */
/*                                                                           */
/*****************************************************************************/

static void attachAxes(firstCall)
Boolean firstCall;
{
  DBUG_ENTER("attachAxes");

  if(!firstCall) {
    XtDestroyWidget(xAxis);
    XtDestroyWidget(yAxis);
  }

  xAxis = CREATE_AXIS("xAxis",
		      (xyPlot ?
		       atXYAxisWidgetClass :
		       atXYAxisWidgetClass /*atLabelAxisWidgetClass*/),
		      plotter,
		      "X-Axis",
		      FALSE);

  yAxis = CREATE_AXIS("yAxis",
		      (/*xyPlot*/ 42 ?
		       atXYAxisWidgetClass :
		       atLabelAxisWidgetClass),
		      plotter,
		      "Y-Axis",
		      TRUE);

  msBarChartDisplayXSetGrid();
  msBarChartDisplayYSetGrid();

  ASSIGN_AXES(plotter, xAxis, yAxis);
  
  DBUG_VOID_RETURN;
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

  updateZoomData();

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
/*  initializePlotter: (local function)                                      */
/*                                                                           */
/*  Initialize the plotter widget... (called by 'localStartupHook')          */
/*                                                                           */
/*****************************************************************************/

static void initializePlotter()
{
  DBUG_ENTER("initializePlotter");

  plotter = CREATE_PLOTTER("plotter", atPlotterWidgetClass,
			   WI_ms_bar_chart_graphic_box);
  msBarChartResizePlotter();

  attachAxes(TRUE);

  ADD_CALLBACK(plotter, XtNmotionCallback, msCursorPanelUpdatePosition);
  ADD_CALLBACK(plotter, XtNdragCallback, msBarChartZoomDrag);

  SET_VALUE(plotter, XtNshowLegend, actualLegend = FALSE);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplaySetTitle: (callback)                                    */
/*                                                                           */
/*  Enter label for whole plot...                                            */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplaySetTitle(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DATA_LEN];

  DBUG_ENTER("msBarChartDisplaySetTitle");

  GET_LABEL(WI_ms_bar_chart_display_enter_title, buffer);
  SET_VALUE(plotter, XtNtitle, buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayClear: (callback)                                       */
/*                                                                           */
/*  Clear the plot...                                                        */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayClear(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  int i;

  DBUG_ENTER("msBarChartDisplayClear");

  if(!strlen(lastRequestAnswer)) {
    doRequest("Do you really want", "to clear the graphic?", "",
	      "Yes", "No", "", &msBarChartDisplayClear);
  } else {
    if(!strcmp(lastRequestAnswer, "Yes")) {
      XtDestroyWidget(plotter);

      for(i=0; i<=actualPlot; ++i) {
	line[i] = (Widget)NULL;

	cfree((char *)xDataPoints[i]);
	cfree((char *)yDataPoints[i]);
	xDataPoints[i] = yDataPoints[i] = (double *)NULL;
      }

      scriptIsActive = scriptIsTerminated = FALSE;
    
      actualPlot = 0;

      initializePlotter();

      updateLine(actualLine = 0);
      updateFrame((actualFrame = 1) - 1);
      updateTime(actualTime = 0.0);
      updateProgress();
      XSync(display, FALSE);
    }

    strcpy(lastRequestAnswer, "");   
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayLegend: (callback)                                      */
/*                                                                           */
/*  Toggle legend on/off...                                                  */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayLegend(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msBarChartDisplayLegend");

  SET_VALUE(plotter, XtNshowLegend, actualLegend = (actualLegend ?
						    FALSE :
						    TRUE));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplaySave: (callback)                                        */
/*                                                                           */
/*  Save actual plot...                                                      */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplaySave(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msBarChartDisplaySave");

  msFileBrowserInitialize(2, "Save postscript...", PS_MASK, "./", "plot.ps");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayPrint: (callback)                                       */
/*                                                                           */
/*  Print actual plot...                                                     */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayPrint(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char temp[FILE_LEN], command[COMMANDS_LEN];

  DBUG_ENTER("msBarChartDisplayPrint");

  tmpnam(temp);
  SAVE_PS(temp, temp);
  sprintf(command, "lpr %s ; rm -f %s", temp, temp);
  system(command);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayXSetTitle: (callback)                                   */
/*                                                                           */
/*  Enter label for x axis...                                                */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayXSetTitle(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DATA_LEN];

  DBUG_ENTER("msBarChartDisplayXSetTitle");

  GET_LABEL(WI_ms_bar_chart_display_x_title, buffer);
  SET_X_LABEL(buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayXSetLog: (callback)                                     */
/*                                                                           */
/*  Make x axis logarithmic...                                               */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayXSetLog(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msBarChartDisplayXSetLog");

  GET_TOGGLE_STATE(WI_ms_bar_chart_display_x_log, &state);
  SET_TYPE(xAxis, (state ? LOGARITHMIC : LINEAR));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayXSetGrid: (callback)                                    */
/*                                                                           */
/*  Toggle x grid on/off...                                                  */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayXSetGrid(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msBarChartDisplayXSetGrid");

  GET_TOGGLE_STATE(WI_ms_bar_chart_display_x_grid, &state);
  SET_GRID(xAxis, state);
  SET_SUBGRID(xAxis, state);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayYSetTitle: (callback)                                   */
/*                                                                           */
/*  Enter label for y axis...                                                */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayYSetTitle(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DATA_LEN];

  DBUG_ENTER("msBarChartDisplayYSetTitle");

  GET_LABEL(WI_ms_bar_chart_display_y_title, buffer);
  SET_Y_LABEL(buffer);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayYSetLog: (callback)                                     */
/*                                                                           */
/*  Make y axis logarithmic...                                               */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayYSetLog(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;
  
  DBUG_ENTER("msBarChartDisplayYSetLog");

  GET_TOGGLE_STATE(WI_ms_bar_chart_display_y_log, &state);
  SET_TYPE(yAxis, (state ? LOGARITHMIC : LINEAR));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartDisplayYSetGrid: (callback)                                    */
/*                                                                           */
/*  Toggle y grid on/off...                                                  */
/*                                                                           */
/*****************************************************************************/

void msBarChartDisplayYSetGrid(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean state;

  DBUG_ENTER("msBarChartDisplayYSetGrid");

  GET_TOGGLE_STATE(WI_ms_bar_chart_display_y_grid, &state);
  SET_GRID(yAxis, state);
  SET_SUBGRID(yAxis, state);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomXSetMin: (callback)                                        */
/*                                                                           */
/*  Enter start of x axis...                                                 */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomXSetMin(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DISPLAY_LEN];
  double t;

  DBUG_ENTER("msBarChartZoomXSetMin");

  GET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, buffer);
  t = atof(buffer);

  if((t < actualXMax) && ((actualXMax - t) >= MINIMAL_X_ZOOM)) {
    actualXMin = t;
    SET_BOUNDS(xAxis, actualXMin, actualXMax);
  } else {
    sprintf(buffer, "%f", actualXMin);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, buffer);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomXSetMax: (callback)                                        */
/*                                                                           */
/*  Enter end of x axis...                                                   */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomXSetMax(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DISPLAY_LEN];
  double t;

  DBUG_ENTER("msBarChartZoomXSetMax");

  GET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, buffer);
  t = atof(buffer);

  if((actualXMin < t) && ((t - actualXMin) >= MINIMAL_X_ZOOM)) {
    actualXMax = t;
    SET_BOUNDS(xAxis, actualXMin, actualXMax);
  } else {
    sprintf(buffer, "%f", actualXMax);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, buffer);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomYSetMin: (callback)                                        */
/*                                                                           */
/*  Enter start of y axis...                                                 */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomYSetMin(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DISPLAY_LEN];
  double t;

  DBUG_ENTER("msBarChartZoomYSetMin");

  GET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, buffer);
  t = atof(buffer);

  if((t < actualYMax) && ((actualYMax - t) >= MINIMAL_Y_ZOOM)) {
    actualYMin = t;
    SET_BOUNDS(yAxis, actualYMin, actualYMax);
  } else {
    sprintf(buffer, "%f", actualYMin);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, buffer);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomYSetMax: (callback)                                        */
/*                                                                           */
/*  Enter end of y axis...                                                   */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomYSetMax(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[DISPLAY_LEN];
  double t;

  DBUG_ENTER("msBarChartZoomYSetMax");

  GET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, buffer);
  t = atof(buffer);

  if((actualYMin < t) && ((t - actualYMin) >= MINIMAL_Y_ZOOM)) {
    actualYMax = t;
    SET_BOUNDS(yAxis, actualYMin, actualYMax);
  } else {
    sprintf(buffer, "%f", actualYMax);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, buffer);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomZoomOut: (callback)                                        */
/*                                                                           */
/*  Zoom out...                                                              */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomZoomOut(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  double diff;

  DBUG_ENTER("msBarChartZoomZoomOut");

  getZoomData();

  diff = actualXMax - actualXMin;
  actualXMin -= diff/2;
  actualXMax += diff/2;

  diff = actualYMax - actualYMin;
  actualYMin -= diff/2;
  actualYMax += diff/2;

  SET_BOUNDS(xAxis, actualXMin, actualXMax);
  SET_BOUNDS(yAxis, actualYMin, actualYMax);

  putZoomData();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartZoomShowAll: (callback)                                        */
/*                                                                           */
/*  Show the whole picture...                                                */
/*                                                                           */
/*****************************************************************************/

void msBarChartZoomShowAll(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msBarChartZoomShowAll");

  SET_VALUE(xAxis, XtNautoScale, TRUE);
  SET_VALUE(yAxis, XtNautoScale, TRUE);

  doLocalDoTick = TRUE;

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

  if(xyPlot) {
    DBUG_RETURN(((struct XYPlotElement *)data)->frame);
  } else {
    DBUG_RETURN(((struct BarChartElement *)data)->frame);
  }
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

  if(xyPlot) {
    DBUG_RETURN(((struct XYPlotElement *)data)->time);
  } else {
    DBUG_RETURN(((struct BarChartElement *)data)->time);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  mode2Short: (local function)                                             */
/*                                                                           */
/*  Return index of selected mode...                                         */
/*                                                                           */
/*****************************************************************************/

short mode2Short(mode)
char *mode;
{
  DBUG_ENTER("mode2Short");

  if(!strcmp(mode, "Points")) {
    DBUG_RETURN(POINTS);
  }
  if(!strcmp(mode, "Impulses")) {
    DBUG_RETURN(IMPULSES);
  }
  if(!strcmp(mode, "Steps")) {
    DBUG_RETURN(STEPS);
  }
  if(!strcmp(mode, "Bars")) {
    DBUG_RETURN(BARS);
  }
  if(!strcmp(mode, "Line-Points")) {
    DBUG_RETURN(LINEPOINTS);
  }
  if(!strcmp(mode, "Line-Impulses")) {
    DBUG_RETURN(LINEIMPULSES);
  }
  if(!strcmp(mode, "Bar-Chart")) {
    DBUG_RETURN(BARCHART);
  }

  DBUG_RETURN(ERROR);
}


/*****************************************************************************/
/*                                                                           */
/*  line2Short: (local function)                                             */
/*                                                                           */
/*  Return index of selected line...                                         */
/*                                                                           */
/*****************************************************************************/

short line2Short(line)
char *line;
{
  DBUG_ENTER("line2Short");

  if(!strcmp(line, "___________")) {
    DBUG_RETURN(SOLID);
  }
  if(!strcmp(line, ". . . . . .")) {
    DBUG_RETURN(DOTTED);
  }
  if(!strcmp(line, "_ _ _ _ _ _")) {
    DBUG_RETURN(DASHED);
  }
  if(!strcmp(line, "_ . _ . _ .")) {
    DBUG_RETURN(DOTDASHED);
  }

  DBUG_RETURN(ERROR);
}


/*****************************************************************************/
/*                                                                           */
/*  marker2Short: (local function)                                           */
/*                                                                           */
/*  Return index of selected marker...                                       */
/*                                                                           */
/*****************************************************************************/

short marker2Short(marker)
char *marker;
{
  DBUG_ENTER("marker2Short");

  if(!strcmp(marker, "Rectangle")) {
    DBUG_RETURN(RECTANGLE);
  }
  if(!strcmp(marker, "Plus")) {
    DBUG_RETURN(PLUS);
  }
  if(!strcmp(marker, "X-Mark")) {
    DBUG_RETURN(XMARK);
  }
  if(!strcmp(marker, "Star")) {
    DBUG_RETURN(STAR);
  }
  if(!strcmp(marker, "Diamond")) {
    DBUG_RETURN(DIAMOND);
  }
  if(!strcmp(marker, "Triangle 1")) {
    DBUG_RETURN(TRIANGLE1);
  }
  if(!strcmp(marker, "Triangle 2")) {
    DBUG_RETURN(TRIANGLE2);
  }
  if(!strcmp(marker, "Triangle 3")) {
    DBUG_RETURN(TRIANGLE3);
  }
  if(!strcmp(marker, "Triangle 4")) {
    DBUG_RETURN(TRIANGLE4);
  }

  DBUG_RETURN(ERROR);
}


/*****************************************************************************/
/*                                                                           */
/*  shade2Short: (local function)                                            */
/*                                                                           */
/*  Return index of selected shade...                                        */
/*                                                                           */
/*****************************************************************************/

short shade2Short(shade)
char *shade;
{
  DBUG_ENTER("shade2Short");

  if(!strcmp(shade, "White")) {
    DBUG_RETURN(WHITE);
  }
  if(!strcmp(shade, "Black")) {
    DBUG_RETURN(BLACK);
  }
  if(!strcmp(shade, "Red")) {
    DBUG_RETURN(RED);
  }
  if(!strcmp(shade, "Dark Red")) {
    DBUG_RETURN(DARK_RED);
  }
  if(!strcmp(shade, "Light Red")) {
    DBUG_RETURN(LIGHT_RED);
  }
  if(!strcmp(shade, "Green")) {
    DBUG_RETURN(GREEN);
  }
  if(!strcmp(shade, "Dark Green")) {
    DBUG_RETURN(DARK_GREEN);
  }
  if(!strcmp(shade, "Light Green")) {
    DBUG_RETURN(LIGHT_GREEN);
  }
  if(!strcmp(shade, "Blue")) {
    DBUG_RETURN(BLUE);
  }
  if(!strcmp(shade, "Dark Blue")) {
    DBUG_RETURN(DARK_BLUE);
  }
  if(!strcmp(shade, "Light Blue")) {
    DBUG_RETURN(LIGHT_BLUE);
  }
  if(!strcmp(shade, "Yellow")) {
    DBUG_RETURN(YELLOW);
  }
  if(!strcmp(shade, "Dark Yellow")) {
    DBUG_RETURN(DARK_YELLOW);
  }
  if(!strcmp(shade, "Light Yellow")) {
    DBUG_RETURN(LIGHT_YELLOW);
  }
  if(!strcmp(shade, "Raster 0")) {
    DBUG_RETURN(GRAY_0);
  }
  if(!strcmp(shade, "Raster 1")) {
    DBUG_RETURN(GRAY_1);
  }
  if(!strcmp(shade, "Raster 2")) {
    DBUG_RETURN(GRAY_2);
  }
  if(!strcmp(shade, "Raster 3")) {
    DBUG_RETURN(GRAY_3);
  }
  if(!strcmp(shade, "Raster 4")) {
    DBUG_RETURN(GRAY_4);
  }
  if(!strcmp(shade, "Raster 5")) {
    DBUG_RETURN(GRAY_5);
  }
  if(!strcmp(shade, "Raster 6")) {
    DBUG_RETURN(GRAY_6);
  }
  if(!strcmp(shade, "Raster 7")) {
    DBUG_RETURN(GRAY_7);
  }
  if(!strcmp(shade, "Raster 8")) {
    DBUG_RETURN(GRAY_8);
  }
  if(!strcmp(shade, "Raster 9")) {
    DBUG_RETURN(GRAY_9);
  }
  if(!strcmp(shade, "Raster 10")) {
    DBUG_RETURN(GRAY_10);
  }

  DBUG_RETURN(ERROR);
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartModeCall: (callback)                                           */
/*                                                                           */
/*  Propagate selected mode...                                               */
/*                                                                           */
/*****************************************************************************/

void msBarChartModeCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  short temp;

  DBUG_ENTER("msBarChartModeCall");

  temp = mode2Short(call_data);

  if((((actualMode == BARCHART) && (temp != BARCHART)) ||
      ((actualMode != BARCHART) && (temp == BARCHART))) && scriptIsActive) {
    DBUG_PRINT("msDisplayPanelSetCaching",
	       ("Can't do this change while plot of other type is active..."));

    doRequest("Can't do this change", "while plot of other",
	      "type is active...",
	      "OK", "", "", NULL);
  } else {
    actualMode = temp;

    DBUG_PRINT("msBarChartModeCall", ("actualMode=%d", actualMode));

    updateMode(actualMode);

    switch(actualMode) {
    case BARCHART:
      /* Type menu = "Colors & Shades" */
      prepareShadeMenu();
      break;
    case POINTS:
    case LINEPOINTS:
      /* Type menu = "Marker" */
      prepareMarkerMenu();
      break;
    default:
      /* Type menu = "Lines" */
      prepareLineMenu();
      break;
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msBarChartTypeCall: (callback)                                           */
/*                                                                           */
/*  Propagate selected type (marker/shade)...                                */
/*                                                                           */
/*****************************************************************************/

void msBarChartTypeCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  DBUG_ENTER("msBarChartTypeCall");

  switch(actualMode) {
  case BARCHART:
    updateColor(actualType = shade2Short(call_data));
    break;
  case POINTS:
  case LINEPOINTS:
    updateMarker(actualType = marker2Short(call_data));
    break;
  default:
    updateLinestyle(actualType = line2Short(call_data));
  }

  DBUG_PRINT("msBarChartShadeCall", ("actualType=%d", actualType));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareModeMenu: (local function)                                        */
/*                                                                           */
/*  Bind function to items...                                                */
/*                                                                           */
/*****************************************************************************/

void prepareModeMenu()
{
  int i;

  DBUG_ENTER("prepareModeMenu");
  
  for(i=1; i<=NUM_MODES; ++i) {
    BIND_ITEM(WI_ms_script_panel_mode, i, msBarChartModeCall, NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareLineMenu: (local function)                                        */
/*                                                                           */
/*  Bind function to items...                                                */
/*                                                                           */
/*****************************************************************************/

void prepareLineMenu()
{
  int i;

  DBUG_ENTER("prepareLineMenu");
  
  SET_VALUE(WI_ms_script_panel_type, XtNmenuFile, "line.med");

  for(i=1; i<=NUM_LINES; ++i) {
    BIND_ITEM(WI_ms_script_panel_type, i, msBarChartTypeCall, NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareMarkerMenu: (local function)                                      */
/*                                                                           */
/*  Bind function to items...                                                */
/*                                                                           */
/*****************************************************************************/

void prepareMarkerMenu()
{
  int i;

  DBUG_ENTER("prepareMarkerMenu");
  
  SET_VALUE(WI_ms_script_panel_type, XtNmenuFile, "marker.med");

  for(i=1; i<=NUM_MARKERS; ++i) {
    BIND_ITEM(WI_ms_script_panel_type, i, msBarChartTypeCall, NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareShadeMenu: (local function)                                       */
/*                                                                           */
/*  Bind function to items...                                                */
/*                                                                           */
/*****************************************************************************/

void prepareShadeMenu()
{
  int i;

  DBUG_ENTER("prepareShadeMenu");
  
  SET_VALUE(WI_ms_script_panel_type, XtNmenuFile, "shade.med");

  for(i=1; i<=NUM_SHADES; ++i) {
    BIND_ITEM(WI_ms_script_panel_type, i, msBarChartTypeCall, NULL);
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
  int i;

  DBUG_ENTER("localStartupHook");

  for(i=0; i<MAX_PLOTS; ++i) { 
    line[i] = (Widget)NULL;
    xDataPoints[i] = yDataPoints[i] = (double *)NULL;
  }
  actualPlot = 0;

  moduleWindow = WI_ms_bar_chart;
  strcpy(prefsName, "barChart.prefs");

  actualPlot = 0;
  updateMode(actualMode = BARCHART);
  updateColor(actualType = BLUE);
  prepareModeMenu();
  prepareShadeMenu();

  xyPlot = TRUE;
  initializePlotter();

  highlightButton(WI_ms_control_panel_stop);

  /* Zoom stuff... */
  msBarChartDisplayXSetLog();
  msBarChartDisplayYSetLog();

  actualXMin = actualYMin = 0;
  actualXMax = actualYMax = 1;

  SET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, "0");
  SET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, "1");
  SET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, "0");
  SET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, "1");

  strcpy(selectedScript, "BarChartSCRs/");

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
  int i;

  DBUG_ENTER("localExitHook");

  /* Free data space... */
  for(i=0; i<MAX_PLOTS; ++i) {
    if(xDataPoints[i] != NULL) {
      cfree((char *)xDataPoints[i]);
    }
    if(yDataPoints[i] != NULL) {
      cfree((char *)yDataPoints[i]);
    }
  }

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

  numBars = numPoints = 0;
  updateLine(actualLine = 0);
  updateFrame((actualFrame = 1) - 1);
  updateTime(actualTime = 0.0);

  /* Next plot... */
  ++actualPlot;

  strcpy(actualLegendName, "Legend");

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  The following functions are the graphics primitives...                   */
/*                                                                           */
/*****************************************************************************/

void SET_MODE(mode)                          /* Set modus for following plot */
short mode;
{
  DBUG_ENTER("SET_MODE");

  xyPlot = (mode == XY_PLOT ? TRUE : FALSE);
  attachAxes(FALSE);

  if(!xyPlot && (actualMode != BARCHART)) {
    updateMode(actualMode = BARCHART);
    updateColor(GRAY_0);
    updateColor(actualType = BLUE);                          /* default type */
    prepareShadeMenu();
  }

  if(xyPlot && (actualMode == BARCHART)) {
    updateMode(SOLID);
    updateMode(actualMode = LINES);
    updateMarker(actualType = RECTANGLE);                    /* default type */
    prepareMarkerMenu();
  }

  DBUG_VOID_RETURN;
}

void SET_TITLE(s)                                       /* Set graphic title */
char *s;
{
  DBUG_ENTER("SET_TITLE");

  SET_VALUE(plotter, XtNtitle, s);
  SET_LABEL(WI_ms_bar_chart_display_enter_title, s);

  DBUG_VOID_RETURN;
}

void SET_X_LABEL(s)       /* Set text to be printed on lower horizontal axis */
char *s;
{
  DBUG_ENTER("SET_X_LABEL");

  SET_LABEL(xAxis, s);
  SET_LABEL(WI_ms_bar_chart_display_x_title, s);

  DBUG_VOID_RETURN;
}

void SET_Y_LABEL(s)          /* Set text to be printed on left vertical axis */
char *s;
{
  DBUG_ENTER("SET_Y_LABEL");

  SET_LABEL(yAxis, s);
  SET_LABEL(WI_ms_bar_chart_display_y_title, s);

  DBUG_VOID_RETURN;
}

void SET_X_GRID(which)                    /* Enable/Disable grid over x axis */
short which;
{
  DBUG_ENTER("SET_X_GRID");

  switch(which) {
  case OFF:
    UNSET_TOGGLE(WI_ms_bar_chart_display_x_grid, TRUE);
    break;
  case ON:
    SET_TOGGLE(WI_ms_bar_chart_display_x_grid, TRUE);
    break;
  }

  msBarChartDisplayXSetGrid();

  DBUG_VOID_RETURN;
}


void SET_Y_GRID(which)                    /* Enable/Disable grid over y axis */
short which;
{
  DBUG_ENTER("SET_Y_GRID");

  switch(which) {
  case OFF:
    UNSET_TOGGLE(WI_ms_bar_chart_display_y_grid, TRUE);
    break;
  case ON:
    SET_TOGGLE(WI_ms_bar_chart_display_y_grid, TRUE);
    break;
  }

  msBarChartDisplayYSetGrid();

  DBUG_VOID_RETURN;
}


void SET_X_MIN(f)                            /* Set lower boundary of x axis */
double f;
{
  char s[DISPLAY_LEN];

  DBUG_ENTER("SET_X_MIN");

  if(f < actualXMax) {
    sprintf(s, "%f", actualXMin = f);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_from, s);

    SET_BOUNDS(xAxis, actualXMin, actualXMax);
  }

  DBUG_VOID_RETURN;
}


void SET_X_MAX(f)                            /* Set upper boundary of x axis */
double f;
{
  char s[DISPLAY_LEN];

  DBUG_ENTER("SET_X_MAX");

  if(f > actualXMin) {
    sprintf(s, "%f", actualXMax = f);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_x_to, s);

    SET_BOUNDS(xAxis, actualXMin, actualXMax);
  }

  DBUG_VOID_RETURN;
}


void SET_Y_MIN(f)                            /* Set lower boundary of y axis */
double f;
{
  char s[DISPLAY_LEN];

  DBUG_ENTER("SET_Y_MIN");

  if(f < actualYMax) {
    sprintf(s, "%f", actualYMin = f);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_from, s);

    SET_BOUNDS(yAxis, actualYMin, actualYMax);
  }

  DBUG_VOID_RETURN;
}


void SET_Y_MAX(f)                            /* Set upper boundary of y axis */
double f;
{
  char s[DISPLAY_LEN];

  DBUG_ENTER("SET_Y_MAX");

  if(f > actualYMin) {
    sprintf(s, "%f", actualYMax = f);
    SET_LABEL(WI_ms_bar_chart_zoom_enter_y_to, s);

    SET_BOUNDS(yAxis, actualYMin, actualYMax);
  }

  DBUG_VOID_RETURN;
}


void SET_LEGEND(which)                            /* Make legend (in)visible */
short which;
{
  DBUG_ENTER("SET_LEGEND");
  
  SET_VALUE(plotter, XtNshowLegend, actualLegend = ((which == ON) ?
						    TRUE : FALSE));

  DBUG_VOID_RETURN;
}


void SET_NAME(name)                          /* Set plot's name (for legend) */
char *name;
{
  DBUG_ENTER("SET_NAME");

  strcpy(actualLegendName, name);

  DBUG_VOID_RETURN;
}


void SET_STYLE(which)          /* Set plot style (overwrites menu selection) */
short which;
{
  DBUG_ENTER("SET_STYLE");

  updateMode(actualMode = which);

  switch(actualMode) {
  case BARCHART:
    /* Type menu = "Colors & Shades" */
    prepareShadeMenu();
    break;
  case POINTS:
  case LINEPOINTS:
    /* Type menu = "Marker" */
    prepareMarkerMenu();
    break;
  default:
    /* Type menu = "Lines" */
    prepareLineMenu();
    break;
  }

  DBUG_VOID_RETURN;
}


void SET_LINESTYLE(which)       /* Set linestyle (overwrites menu selection) */
short which;
{
  DBUG_ENTER("SET_MARKER");

  updateLinestyle(actualType = which);

  DBUG_VOID_RETURN;
}


void SET_MARKER(which)        /* Set marker type (overwrites menu selection) */
short which;
{
  DBUG_ENTER("SET_MARKER");

  updateMarker(actualType = which);

  DBUG_VOID_RETURN;
}


void SET_SHADE(which) /* Set color/shade of bars (overwrites menu selection) */
short which;
{
  DBUG_ENTER("SET_SHADE");

  updateColor(actualType = which);

  DBUG_VOID_RETURN;
}


void CREATE_BAR(va_alist)             /* Create new bar and specify it's ids */
va_dcl
{
  va_list ap;
  int i;

  DBUG_ENTER("CREATE_BAR");

  va_start(ap);

  ++numBars;
  strcpy(barsArray[numBars].bar, va_arg(ap, char *));
  DBUG_PRINT("CREATE_BAR", ("bar=%s", barsArray[numBars].bar));

  while((i = va_arg(ap, int)) >0 /*!= NULL.....*/) {
    idsArray[i].bar = numBars;
    DBUG_PRINT("CREATE_BAR", ("id=%d (%s)", i, ids[i]));
  }

  va_end(ap);

  DBUG_VOID_RETURN;
}

void INCREMENT(i, d)                           /* Increment bar of id i by d */
int i;
double d;
{
  struct BarChartElement *temp;
  short bar = idsArray[i].bar;

  DBUG_ENTER("INCREMENT");

  DBUG_PRINT("INCREMENT", ("Increment bar %d by %f", bar, d));
  
  if(bar) {
    yDataPoints[actualPlot][idsArray[i].bar - 1] += d;

    temp = (struct BarChartElement *)newData();
    temp->time = actualTime;
    temp->frame = actualFrame;
    temp->diff = d;
    temp->bar = idsArray[i].bar;

    DBUG_PRINT("INCREMENT", ("frame=%d", actualFrame));
  }     

  DBUG_VOID_RETURN;
}

void SET_HEIGHT(i, d)                                  /* Set height of id i */
int i;
double d;
{
  DBUG_ENTER("SET_HEIGHT");

  DBUG_PRINT("SET_HEIGHT", ("Set height of bar %d to %f", idsArray[i].bar, d));
  
  if(idsArray[i].bar) {  
    INCREMENT(i, d - yDataPoints[actualPlot][idsArray[i].bar - 1]);
  }     

  DBUG_VOID_RETURN;
}

void REMOVE_XY(x, y)                     /* Remove point at coordinate (x,?) */
double x, y;
{
  struct XYPlotElement *temp;

  DBUG_ENTER("REMOVE_XY");

  DBUG_PRINT("REMOVE_XY", ("x=%f y=%f", x, y));

  if(removePoint(x, y)) {
    temp = (struct XYPlotElement *)newData();
    temp->time = actualTime;
    temp->frame = actualFrame;
    temp->wasUpdate = FALSE;
    temp->x = x;
    temp->y = y;
    temp->extra.wasAdd = FALSE;
  }

  DBUG_VOID_RETURN;
}


void ADD_XY(x, y)                        /* Insert point at coordinate (x,?) */
double x, y;
{
  struct XYPlotElement *temp;

  DBUG_ENTER("ADD_XY");

  DBUG_PRINT("ADD_XY", ("x=%f y=%f", x, y));

  temp = (struct XYPlotElement *)newData();
  temp->time = actualTime;
  temp->frame = actualFrame;
  temp->wasUpdate = FALSE;
  temp->x = x;
  temp->y = y;
  temp->extra.wasAdd = TRUE;

  addPoint(x, y);

  DBUG_VOID_RETURN;
}


void UPDATE_XY(x, y)                     /* Update point at coordinate (x,?) */
double x, y;
{
  struct XYPlotElement *temp;
  double oldY;

  DBUG_ENTER("UPDATE_XY");

  DBUG_PRINT("UPDATE_XY", ("x=%f y=%f", x, y));

  if(updatePoint(x, y, &oldY)) {
    /* There's a point with the specified x... */
    temp = (struct XYPlotElement *)newData();
    temp->time = actualTime;
    temp->frame = actualFrame;
    temp->wasUpdate = TRUE;
    temp->x = x;                                                 /* Common x */
    temp->y = oldY;                                                 /* Old y */
    temp->extra.y2 = y;                                             /* New y */
  } else {
    /* No point with this x yet! Add the point... */
    ADD_XY(x, y);
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

  msBarChartDisplayPrint();

  DBUG_VOID_RETURN;
}


void CLEAR()                                            /* Clear actual plot */
{
  DBUG_ENTER("CLEAR");

  msBarChartDisplayClear();

  DBUG_VOID_RETURN;
}


void NEXT_FRAME()                  /* Display actual frame and begin new one */
{
  struct BarChartElement *temp;
  int i;

  DBUG_ENTER("NEXT_FRAME");

  frameCompleted = TRUE;

  if(firstFrame) {
    /* Create dummy entry in stack... */
    temp = (struct BarChartElement *)newData();
    temp->time = 0.0;
    temp->frame = 0;

    firstFrame = FALSE;

    /* Initialize bars... */
    if(xyPlot) {
      if((xDataPoints[actualPlot] = 
	  (double *)calloc(MAX_POINTS, sizeof(double))) == NULL) {
	doRequest("Could not get memory for plotdata...", "", "Giving up!",
		  "OK", "", "", NULL);
	dieHard("Could not get memory for plotdata...");
      }
      if((yDataPoints[actualPlot] =
	  (double *)calloc(MAX_POINTS, sizeof(double))) == NULL) {
	doRequest("Could not get memory for plotdata...", "", "Giving up!",
		  "OK", "", "", NULL);
	dieHard("Could not get memory for plotdata...");
      }
    } else {
      if((yDataPoints[actualPlot] =
	  (double *)calloc(numBars, sizeof(double))) == NULL) {
	doRequest("Could not get memory for plotdata...", "", "Giving up!",
		  "OK", "", "", NULL);
	dieHard("Could not get memory for plotdata...");
      }
    }

    for(i=0; i<numBars; ++i) {
      yDataPoints[actualPlot][i] = 0;
    }

    if(!xyPlot) {
      /* Label the bars... */
/*      AtLabelAxisAttachData((AtLabelAxisWidget) xAxis,
			    (String *) barsArray[1].bar, 
			    sizeof(barsArray[1]), 1, 1);
*/    }
    
    line[actualPlot] = XtVaCreateWidget("line",
					(xyPlot ?
					 atXYLinePlotWidgetClass :
					 atBarPlotWidgetClass),
					plotter,
					XtNlegendName, actualLegendName,
					NULL);

    updateMode(actualMode);

    if(xyPlot) {
      AtXYLinePlotAttachData(line[actualPlot], 
			     (XtPointer)xDataPoints[actualPlot],
			     AtDouble, sizeof(double),
			     (XtPointer)yDataPoints[actualPlot],
			     AtDouble, sizeof(double),
			     0, numPoints);
    } else {
      AtBarPlotAttachData(line[actualPlot],
			  (XtPointer)yDataPoints[actualPlot],
			  AtDouble, sizeof(double),
			  0, numBars);
    }
  } else {
    updateData();
    if(!scriptIsTerminated) {
      ++actualFrame;
    } 
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
