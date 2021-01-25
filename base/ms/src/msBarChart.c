#ifndef lint
static char rcsid[] =
   "$Id: minimal.c,v 1.2 90/09/22 17:23:32 rlh2 Rel $";
#endif /* !lint */

/* 
 * Copyright 1990 Richard Hesketh / rlh2@ukc.ac.uk
 *                Computing Lab. University of Kent at Canterbury, UK
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Richard Hesketh and The University of
 * Kent at Canterbury not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 * Richard Hesketh and The University of Kent at Canterbury make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * Richard Hesketh AND THE UNIVERSITY OF KENT AT CANTERBURY DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL Richard Hesketh OR THE
 * UNIVERSITY OF KENT AT CANTERBURY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * Author:  Richard Hesketh / rlh2@ukc.ac.uk, 
 *                Computing Lab. University of Kent at Canterbury, UK
 */

/* Minimal application program required to execute a Dirt generated
 * interface.
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "msBarChart.h"


extern Widget XukcDirtGO();
extern void XukcCreateUserInterface();


/* callbacks */
extern void msFileBrowserUpdate();
extern void msFileBrowserSelectFile();
extern void msFileBrowserAccept();extern void startupHook();
extern void exitHook();
extern void msControlPanelGoToStart();
extern void msControlPanelPlayBackward();
extern void msControlPanelStepBackward();
extern void msControlPanelStop();
extern void msControlPanelStepForward();
extern void msControlPanelPlayForward();
extern void msControlPanelGoToEnd();
extern void msSpeedPanelEnterDelay();
extern void msSpeedPanelDecreaseDelay();
extern void msSpeedPanelIncreaseDelay();
extern void msSpeedPanelSetDelay();
extern void msSpeedPanelEnterRate();
extern void msSpeedPanelDecreaseRate();
extern void msSpeedPanelIncreaseRate();
extern void msSpeedPanelSetRate();
extern void msCursorPanelUpdatePosition();
extern void msFilePanelEnterPath();
extern void msFilePanelEnterName();
extern void msFilePanelBrowse();
extern void msScriptPanelEnterPath();
extern void msScriptPanelEnterName();
extern void msScriptPanelBrowse();
extern void msScriptPanelEdit();
extern void msScriptPanelRun();
extern void msScriptPanelSave();
extern void msBarChartDisplaySetTitle();
extern void msBarChartDisplayClear();
extern void msBarChartDisplayLegend();
extern void msBarChartDisplaySave();
extern void msBarChartDisplayPrint();
extern void msBarChartDisplayXSetTitle();
extern void msBarChartDisplayXSetLog();
extern void msBarChartDisplayXSetGrid();
extern void msBarChartDisplayYSetTitle();
extern void msBarChartDisplayYSetLog();
extern void msBarChartDisplayYSetGrid();
extern void msBarChartZoomXSetMin();
extern void msBarChartZoomXSetMax();
extern void msBarChartZoomYSetMin();
extern void msBarChartZoomYSetMax();
extern void msBarChartZoomZoomOut();
extern void msBarChartZoomShowAll();
extern void msPositionPanelEnterFrame();
extern void msPositionPanelEnterTime();

extern void doTick();


extern void WcRegisterCallback();
extern void WcRegisterAction();


/* actions */
extern void msBarChartResizePlotter();


XtAppContext app;


void tick(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{    
  doTick();
  XtAppAddTimeOut(app, (unsigned long)TICK_DELAY, tick, (XtPointer)NULL);
  return;
}


void old_main(argc, argv)
int argc;
char *argv[];
{
	Widget tmp;

	tmp = XukcDirtGO(NULL, &argc, argv, NULL, 0, NULL);
	app = XtWidgetToApplicationContext(tmp);

	/* register any callback or action routines with Wc via
	 * WcRegisterCallback() and WcRegisterAction().
	 */

	WcRegisterCallback(app, "msFileBrowserUpdate",
			   msFileBrowserUpdate, NULL);
	WcRegisterCallback(app, "msFileBrowserSelectFile",
			   msFileBrowserSelectFile, NULL);
	WcRegisterCallback(app, "msFileBrowserAccept",
			   msFileBrowserAccept, NULL);

	WcRegisterCallback(app, "exitHook",
                           exitHook, NULL);

	WcRegisterCallback(app, "msCursorPanelUpdatePosition",
                           msCursorPanelUpdatePosition, NULL);

	WcRegisterCallback(app, "msControlPanelGoToStart",
                           msControlPanelGoToStart, NULL);
	WcRegisterCallback(app, "msControlPanelPlayBackward",
                           msControlPanelPlayBackward, NULL);
	WcRegisterCallback(app, "msControlPanelStepBackward",
                           msControlPanelStepBackward, NULL);
	WcRegisterCallback(app, "msControlPanelStop",
                           msControlPanelStop, NULL);
	WcRegisterCallback(app, "msControlPanelStepForward",
                           msControlPanelStepForward, NULL);
	WcRegisterCallback(app, "msControlPanelPlayForward",
                           msControlPanelPlayForward, NULL);
	WcRegisterCallback(app, "msControlPanelGoToEnd",
                           msControlPanelGoToEnd, NULL);

	WcRegisterCallback(app, "msSpeedPanelEnterDelay",
                           msSpeedPanelEnterDelay, NULL);
	WcRegisterCallback(app, "msSpeedPanelSetDelay",
                           msSpeedPanelSetDelay, NULL);
	WcRegisterCallback(app, "msSpeedPanelDecreaseDelay",
                           msSpeedPanelDecreaseDelay, NULL);
	WcRegisterCallback(app, "msSpeedPanelIncreaseDelay",
                           msSpeedPanelIncreaseDelay, NULL);
	WcRegisterCallback(app, "msSpeedPanelEnterRate",
                           msSpeedPanelEnterRate, NULL);
	WcRegisterCallback(app, "msSpeedPanelSetRate",
                           msSpeedPanelSetRate, NULL);
	WcRegisterCallback(app, "msSpeedPanelDecreaseRate",
                           msSpeedPanelDecreaseRate, NULL);
	WcRegisterCallback(app, "msSpeedPanelIncreaseRate",
                           msSpeedPanelIncreaseRate, NULL);

	WcRegisterCallback(app, "msFilePanelEnterPath",
			   msFilePanelEnterPath, NULL);
	WcRegisterCallback(app, "msFilePanelEnterName",
			   msFilePanelEnterName, NULL);
	WcRegisterCallback(app, "msFilePanelBrowse",
			   msFilePanelBrowse, NULL);

	WcRegisterCallback(app, "msScriptPanelEnterPath",
			   msScriptPanelEnterPath, NULL);
	WcRegisterCallback(app, "msScriptPanelEnterName",
			   msScriptPanelEnterName, NULL);
	WcRegisterCallback(app, "msScriptPanelBrowse",
			   msScriptPanelBrowse, NULL);
	WcRegisterCallback(app, "msScriptPanelEdit",
			   msScriptPanelEdit, NULL);
	WcRegisterCallback(app, "msScriptPanelRun",
			   msScriptPanelRun, NULL);
	WcRegisterCallback(app, "msScriptPanelSave",
			   msScriptPanelSave, NULL);

	WcRegisterCallback(app, "msBarChartDisplaySetTitle",
			   msBarChartDisplaySetTitle, NULL);
	WcRegisterCallback(app, "msBarChartDisplayClear",
			   msBarChartDisplayClear, NULL);
	WcRegisterCallback(app, "msBarChartDisplayLegend",
			   msBarChartDisplayLegend, NULL);
	WcRegisterCallback(app, "msBarChartDisplaySave",
			   msBarChartDisplaySave, NULL);
	WcRegisterCallback(app, "msBarChartDisplayPrint",
			   msBarChartDisplayPrint, NULL);
	WcRegisterCallback(app, "msBarChartDisplayXSetTitle",
			   msBarChartDisplayXSetTitle, NULL);
	WcRegisterCallback(app, "msBarChartDisplayXSetLog",
			   msBarChartDisplayXSetLog, NULL);
	WcRegisterCallback(app, "msBarChartDisplayXSetGrid",
			   msBarChartDisplayXSetGrid, NULL);
	WcRegisterCallback(app, "msBarChartDisplayYSetTitle",
			   msBarChartDisplayYSetTitle, NULL);
	WcRegisterCallback(app, "msBarChartDisplayYSetLog",
			   msBarChartDisplayYSetLog, NULL);
	WcRegisterCallback(app, "msBarChartDisplayYSetGrid",
			   msBarChartDisplayYSetGrid, NULL);

	WcRegisterCallback(app, "msBarChartZoomXSetMin",
			   msBarChartZoomXSetMin, NULL);
	WcRegisterCallback(app, "msBarChartZoomXSetMax",
			   msBarChartZoomXSetMax, NULL);
	WcRegisterCallback(app, "msBarChartZoomYSetMin",
			   msBarChartZoomYSetMin, NULL);
	WcRegisterCallback(app, "msBarChartZoomYSetMax",
			   msBarChartZoomYSetMax, NULL);
	WcRegisterCallback(app, "msBarChartZoomZoomOut",
			   msBarChartZoomZoomOut, NULL);
	WcRegisterCallback(app, "msBarChartZoomShowAll",
			   msBarChartZoomShowAll, NULL);

	WcRegisterCallback(app, "msPositionPanelEnterFrame",
			   msPositionPanelEnterFrame, NULL);
	WcRegisterCallback(app, "msPositionPanelEnterTime",
			   msPositionPanelEnterTime, NULL);

	WcRegisterAction(app, "msBarChartResizePlotter",
			 msBarChartResizePlotter, NULL);

	XukcCreateUserInterface(tmp);

	/* binding of the user interface to the application done here */

	XtRealizeWidget(tmp);

	startupHook();

	XtAppAddTimeOut(app, (unsigned long)TICK_DELAY, tick, (XtPointer)NULL);

	XtAppMainLoop(XtWidgetToApplicationContext(tmp));
}


char dbugOptions[DBUG_LEN];


void MAIN
{
  strcpy(dbugOptions, "");

  OPT
  ARG '#': /* Turn on Fred Fish's debugging routines... */
    PARM
    strcpy(dbugOptions, *argv);
    DBUG_PUSH(dbugOptions);
    NEXTOPT
  ARG '?':
  ARG 'h': /* help the DAU */
    printf("Options:\n");
    printf("-#: Turn on Fred Fish's debugging routines.\n");
    printf("    (with parameters following the '-#')\n");
    exit(0);
  OTHER
    printf("Unknown option on command line: %c\n",**argv);
    printf("Use option '-h' for list of valid options...\n");
  ENDOPT

  old_main(argc, argv);
}
