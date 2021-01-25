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

#include "msMain.h"


extern Widget XukcDirtGO();
extern void XukcCreateUserInterface();


extern void startupHook();
extern void groups2Med();
extern void modules2Med();

extern void WcRegisterCallback();


/* callbacks */
extern void msFileBrowserUpdate();
extern void msFileBrowserSelectFile();
extern void msFileBrowserAccept();
extern void msMainModuleCall();
extern void msMainFileBrowse();
extern void msMainFileCall();
extern void msMainExitHook();
extern void msMainStorePreferences();
extern void msMainRedumaCall();

/* actions */


void old_main(argc, argv)
int argc;
char *argv[];
{
	Widget tmp;
	XtAppContext app;

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
	WcRegisterCallback(app, "msMainModuleCall",
			   msMainModuleCall, NULL);
	WcRegisterCallback(app, "msMainFileBrowse",
			   msMainFileBrowse, NULL);
	WcRegisterCallback(app, "msMainFileCall",
			   msMainFileCall, NULL);
	WcRegisterCallback(app, "msMainExitHook",
			   msMainExitHook, NULL);
	WcRegisterCallback(app, "msMainStorePreferences",
			   msMainStorePreferences, NULL);
	WcRegisterCallback(app, "msMainRedumaCall",
			   msMainRedumaCall, NULL);

	XukcCreateUserInterface(tmp);

	/* binding of the user interface to the application done here */

	XtRealizeWidget(tmp);

	startupHook();

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
  ARG 'm': /* re-scan MODULES-file... */
    modules2Med("MODULES", "modules.med", "  Modules");
  ARG 'g': /* re-scan GROUPS-file... */
    groups2Med("GROUPS", "filter.med", "  Filter");
    groups2Med("GROUPS", "init.med", "  Init");
  ARG '?':
  ARG 'h': /* help the DAU */
    printf("Options:\n");
    printf("-#: Turn on Fred Fish's debugging routines.\n");
    printf("    (with parameters following the '-#')\n");
    printf("-m: Re-read 'MODULES'-file.\n");
    printf("-g: Re-read 'GROUPS'-file.\n");
    exit(0);
  OTHER
  ENDOPT
  old_main(argc, argv);
}
