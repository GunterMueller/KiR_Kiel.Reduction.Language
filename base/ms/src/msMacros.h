/*#define DBUG_OFF 42*/

#define FOREVER while(42)

#define GET_VALUE(W, N, V) XtVaGetValues(W, N, V, NULL)
#define SET_VALUE(W, N, V) XtVaSetValues(W, N, V, NULL)

#define ADD_CALLBACK(W, T, C) XtAddCallback(W, T, C, NULL);

#define GET_LABEL(W, S) {\
			  String *label;\
			  GET_VALUE(W, XtNlabel, &label);\
			  strcpy(S, (char *)label);\
		        }
#define SET_LABEL(W, S) SET_VALUE(W, XtNlabel, S)

#define SET_POSITION(W, P) SET_VALUE(W, XtNx, P);

#define GET_SIZE(W, X, Y) XtVaGetValues(W, XtNwidth, X, XtNheight, Y, NULL);
#define SET_SIZE(W, X, Y) XtUnmanageChild(W);\
                          XtVaSetValues(W, XtNwidth, X, XtNheight, Y, NULL);\
                          XtManageChild(W)

#define CREATE_MANAGED_WIDGET(L, C, W) XtVaCreateManagedWidget(L, C, W, NULL)

#define GET_DISPLAY(A) XtDisplay(A)
#define GET_SCREEN(D) DefaultScreen(D)
#define GET_COLORMAP(D, S) DefaultColormap(D, S)

#define SET_BACKGROUND(W, P) SET_VALUE(W, XtNbackground, P)
#define SET_FOREGROUND(W, P) SET_VALUE(W, XtNforeground, P)
#define GET_BACKGROUND(W, P) GET_VALUE(W, XtNbackground, P)
#define GET_FOREGROUND(W, P) GET_VALUE(W, XtNforeground, P)

#define TOGGLE_ENABLE(W, B) XukcChangeButtonStatus(W, B, TRUE)
#define TOGGLE_DISABLE(W, B) XukcChangeButtonStatus(W, B, FALSE)

#define BIND_ITEM(W, N, P, D) XukcBindButton(W, N, P, D)
#define BIND_TOGGLE(W, B, ON, OFF, A, P, C) XukcBindToggle(W, B, ON, OFF,\
							   A, P, C)
#define UNBIND_TOGGLE(W, B) XukcUnbindToggle(W, B)

#define UNSELECT_ITEMS(W) XukcScrListUnselectItems(W)
#define SELECT_ITEM(W, N) XukcScrListSelectItem(W, N, FALSE, TRUE)
#define UNBIND_FORCE_DOWN(W, B) XukcUnbindForceDown(W, B)

#define SET_LIST(W, S, I) XtVaSetValues(W, XtNlist, S,\
                                           XtNnumberStrings, I,\
                                           NULL)

#define DESTROY_LIST(L) XukcDestroyList(L)
#define ADD_OBJECT_TO_LIST(L, O) XukcAddObjectToList(L, O, TRUE)
#define GET_LIST_POINTER(L, K) XukcGetListPointer(L, K)

#define NEW_STRING(S) XtNewString(S)

#define SET_TOGGLE(W, B) XukcSetToggle(W, B)
#define UNSET_TOGGLE(W, B) XukcUnsetToggle(W, B)
#define GET_TOGGLE_STATE(W, T) GET_VALUE(W, XtNstate, T);
		

#define ERROR_LEN 1000
#define DBUG_LEN 100
#define DATA_LEN 1000                /* Maximal length of lines in data-file */
#define DISPLAY_LEN 100                        /* Maximal length of displays */
#define TICK_DELAY 1                          /* Time between two ticks (ms) */
#define FILE_LEN 1000
#define MIN_DELAY 0
#define MAX_DELAY 1000
#define DEFAULT_DELAY 0
#define MIN_RATE 1
#define MAX_RATE 1000
#define DEFAULT_RATE 1
#define FILE_MASK "*.ms"
#define SCRIPT_MASK "*.scr"
#define PS_MASK "*.ps"
#define EPS_MASK "*.eps"
#define EPS_X_FACTOR 0.65
#define EPS_Y_FACTOR 0.65

#define HEADER 1
#define INIT 2
#define TICKET 3
#define DATA 4

#define MODULES_HEIGHT 25             /* Heigth of the items in MODULES-menu */
#define MODULES_WIDTH 200              /* Width of the items in MODULES-menu */
#define IDS_ROWS 25              /* Rows in GROUPS- and IDS-menu, <2 invalid */
#define IDS_HEIGHT 20         /* Height of the items in GROUPS- and IDS-menu */
#define IDS_WIDTH 150          /* Width of the items in GROUPS- and IDS-menu */
#define NUM_PROCESSORS 32                  /* Number of available processors */
#define COMMANDS_LEN 1000
#define MAX_COMMANDS 10
#define MODULES_LEN 100
#define MAX_MODULES 10            /* Maximal number of items in modules menu */
#define MAX_PIDS 1000                   /* Maximal number of modules running */
#define IDS_LEN 20
#define MAX_IDS 1000
#define MAX_GROUPS 100

#define X_MIN 0                              /* Dimensions of virtual screen */
#define X_MAX 100
#define Y_MIN 0
#define Y_MAX 100
