#include <xview/xview.h>
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
#include <xview/panel.h>
#include <xview/tty.h>
#include <xview/textsw.h>

#define BREAK_FUNCS "","inter","ear","eac"


#ifdef XVIEW_COMMENT
     XView CONVERSION - Due to a name space clash with Xlib, the SunView
     data-type Window is now Xv_Window in XView
#endif

/*
Window frame, panel, ttysw ;
*/

Frame frame;
Panel panel;
Window ttysw ;

Panel_item f1,f2 ,f3 ,f4, f5, f6, f7, f8,
           f9,f10,f11,f12,f13,f14,f15,f16,
	   pcontinue,pstep,pexit,pdis,
	   break_point ;

Menu	red_menu;

Icon icon ;
static short icon_image[] = {
#include "edtool.icon"
} ;
mpr_static(edtool,64,64,1,icon_image) ;

void inputproc(panel)
Panel_item panel ;
{
 char *client_data ;
 char cmdstring[80] ;
 client_data = 
#ifdef XVIEW_COMMENT
     XView CONVERSION - You may wish to change this to xv_get format
     Look at panel.h and/or See Sect 3.1
#endif
panel_get(panel,PANEL_CLIENT_DATA,0) ;
 sprintf(cmdstring,"%s\n",client_data) ;
 ttysw_input(ttysw,cmdstring,strlen(cmdstring)) ;
}

void f1_proc() { inputproc(f1) ; }
void f2_proc() { inputproc(f2) ; }
void f3_proc() { inputproc(f3) ; }
void f4_proc() { inputproc(f4) ; }
void f5_proc() { inputproc(f5) ; }
void f6_proc() { inputproc(f6) ; }
void f7_proc() { inputproc(f7) ; }
void f8_proc() { inputproc(f8) ; }
void f9_proc() { inputproc(f9) ; }
void f10_proc() { inputproc(f10) ; }
void f11_proc() { inputproc(f11) ; }
void f12_proc() { inputproc(f12) ; }
void f13_proc() { inputproc(f13) ; }
void f14_proc() { inputproc(f14) ; }
void f15_proc() { inputproc(f15) ; }
void f16_proc() { inputproc(f16) ; }
void step_proc() { inputproc(pstep) ; }
void continue_proc() { inputproc(pcontinue) ; }
void dis_proc() { inputproc(pdis) ; }

void exit_proc()
{
 ttysw_input(ttysw,"exit\n",strlen("exit\n")) ;
 sleep(1) ;
 ttysw_input(ttysw,"\n",strlen("\n")) ;
}

void quit_proc()
{
 
#ifdef XVIEW_COMMENT
     XView CONVERSION - If this is being called from within an event/notify
     call-back proc, call func xv_destroy_safe instead
#endif
xv_destroy(frame) ;
}

void break_proc()
{
 int ival ;
 char* cval ;
 char cmdstring[80] ;
 ival = (int) 
#ifdef XVIEW_COMMENT
     XView CONVERSION - You may wish to change this to xv_get format
     Look at panel.h and/or See Sect 3.1
#endif
panel_get_value(break_point) ;
 cval = (char*) 
#ifdef XVIEW_COMMENT
     XView CONVERSION - You may wish to change this to xv_get format
     Look at panel.h and/or See Sect 3.1
#endif
panel_get(break_point,PANEL_CHOICE_STRING,ival,0) ;
 sprintf(cmdstring,"break %s\n",cval) ;
 if(ival)
   {
    ttysw_input(ttysw,cmdstring,strlen(cmdstring)) ;
   }
}

void red_menu_proc(menu,menu_item)
Menu menu;
Menu_item menu_item;
{
    printf("Menu Item: %s\n", xv_get(menu_item, MENU_STRING));
    if (!strcmp((char *)xv_get(menu_item, MENU_STRING), "Quit"))
        exit(0);
}

void icon_proc()
{
 xv_set(frame,FRAME_CLOSED, TRUE,0) ;
}

create_panel_items()
{

 f1 = xv_create(panel,PANEL_BUTTON,
                PANEL_LABEL_STRING, "reduce",
		PANEL_NOTIFY_PROC, f1_proc,
/*
		XV_Y     , ATTR_ROW(0) ,
*/
		   PANEL_CLIENT_DATA, ":1",
		   NULL) ;

 f2 = xv_create(panel,PANEL_BUTTON,
                   PANEL_LABEL_STRING, "load",
		   PANEL_NOTIFY_PROC, f2_proc,
		   PANEL_CLIENT_DATA, ":2",
		   NULL) ;

 f3 = xv_create(panel,PANEL_BUTTON,
                   PANEL_LABEL_STRING, "display",
		   PANEL_NOTIFY_PROC, f3_proc,
		   /* XV_Y     , ATTR_ROW(0) ,	*/
		   PANEL_CLIENT_DATA, ":3",
		   0) ;

 f4 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "help",
		   PANEL_NOTIFY_PROC, f4_proc,
		   /* XV_Y     , ATTR_ROW(0) ,*/
		   PANEL_CLIENT_DATA, ":4",
		   0) ;

 f5 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "flip",
		   PANEL_NOTIFY_PROC, f5_proc,
		   /* XV_Y     , ATTR_ROW(0) ,*/
		   PANEL_CLIENT_DATA, ":5",
		   0) ;

 f6 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "rd aux 1",
		   PANEL_NOTIFY_PROC, f6_proc,
		   /* XV_Y     , ATTR_ROW(0) ,*/
		   PANEL_CLIENT_DATA, ":6",
		   0) ;

 f7 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "rd aux 2",
		   PANEL_NOTIFY_PROC, f7_proc,
		   /* XV_Y     , ATTR_ROW(0) ,*/
		   PANEL_CLIENT_DATA, ":7",
		   0) ;

 f8 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "command",
		   PANEL_NOTIFY_PROC, f8_proc,
		   /* XV_Y     , ATTR_ROW(0) ,*/
		   PANEL_CLIENT_DATA, ":8",
		   0) ;

 f9 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "red1000",
		   PANEL_NOTIFY_PROC, f9_proc,
		   /* XV_X     , ATTR_ROW(0) ,*/
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":9",
		   0) ;

 f10 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "save",
		   PANEL_NOTIFY_PROC, f10_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":10",
		   0) ;

 f11 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "display up",
		   PANEL_NOTIFY_PROC, f11_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":11",
		   0) ;

#ifdef UnBenutzt
 f12 = xv_create(panel,PANEL_BUTTON,
PANEL_LABEL_STRING, "F12",
		   PANEL_NOTIFY_PROC, f12_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":12",
		   0) ;
#endif

 f13 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "wr backup",
		   PANEL_NOTIFY_PROC, f13_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":13",
		   0) ;

 f14 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "wr aux 1",
		   PANEL_NOTIFY_PROC, f14_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":14",
		   0) ;

 f15 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "wr aux 2",
		   PANEL_NOTIFY_PROC, f15_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":15",
		   0) ;

 f16 = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "exit",
		   PANEL_NOTIFY_PROC, f16_proc,
		   /* XV_Y     , ATTR_ROW(1) ,*/
		   PANEL_CLIENT_DATA, ":16",
		   0) ;

#ifdef UnBenutzt
 pstep = xv_create(panel,PANEL_BUTTON,
PANEL_LABEL_STRING, "STEP",
		   PANEL_NOTIFY_PROC, step_proc,
		   XV_X     , ATTR_COL(104) ,
		   XV_Y     , ATTR_ROW(0) ,
		   PANEL_CLIENT_DATA, "s",
		   0) ;

 pcontinue = xv_create(panel,PANEL_BUTTON,
PANEL_LABEL_STRING, "CONTINUE",
		   PANEL_NOTIFY_PROC, continue_proc,
		   XV_X     , ATTR_COL(116) ,
		   XV_Y     , ATTR_ROW(0) ,
		   PANEL_CLIENT_DATA, "continue",
		   0) ;

 pexit = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "EXIT",
		   PANEL_NOTIFY_PROC, exit_proc,
		   XV_X     , ATTR_COL(104) ,
		   XV_Y     , ATTR_ROW(1) ,
		   PANEL_CLIENT_DATA, "exit\n",
		   0) ;

 xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "QUIT",
		   PANEL_NOTIFY_PROC, quit_proc,
		   XV_X     , ATTR_COL(116) ,
		   XV_Y     , ATTR_ROW(1) ,
		   0) ;

 pdis = xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "DISPLAY",
		   PANEL_NOTIFY_PROC, dis_proc,
		   XV_X     , ATTR_COL(104) ,
		   XV_Y     , ATTR_ROW(2) ,
		   PANEL_CLIENT_DATA, "display",
		   0) ;

 xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "ICON",
		   PANEL_NOTIFY_PROC, icon_proc,
		   XV_X     , ATTR_COL(116) ,
		   XV_Y     , ATTR_ROW(2) ,
		   0) ;

 break_point = xv_create(panel,PANEL_CYCLE,
		   XV_X     , ATTR_COL(0) ,
		   XV_Y     , ATTR_ROW(2) ,
		   PANEL_DISPLAY_LEVEL, PANEL_CURRENT,
		   PANEL_LABEL_STRING, "breakpoint: ",
		   PANEL_CHOICE_STRINGS, BREAK_FUNCS, 0,
		   0) ;

 xv_create(panel,PANEL_BUTTON,
		   
PANEL_LABEL_STRING, "confirm break",
		   PANEL_NOTIFY_PROC, break_proc ,
		   XV_X     , ATTR_COL(30) ,
		   XV_Y     , ATTR_ROW(2) ,
		   0) ;

#endif

}


main(argc,argv)
int argc ;
char *argv[] ;
{
 char cmdstring[85] ;
 char progname[80] ;

/*
 if(fork())
   {
    exit(1) ;
   }
*/

 if(argc>1)
   {
    if(argv[argc-1][0] != '-')
      {
       strcpy(progname,argv[--argc]) ;
      }
   }
 icon = icon_create(ICON_IMAGE, &edtool,0) ;

 xv_init(XV_INIT_ARGC_PTR_ARGV,&argc,argv,NULL);

 frame = xv_create(NULL,FRAME,
		       
#ifdef XVIEW_COMMENT
     XView CONVERSION - Make sure to use xv_init tto process the attrs first,
     XVPM 3.2
#endif
FRAME_ARGS, argc, argv,
		       FRAME_ICON, icon ,
		       XV_LABEL, "edtool",
		       WIN_COLUMNS, 135,
		       WIN_ROWS, 5,
		       FRAME_NO_CONFIRM, FALSE,
		       0) ;

 panel = xv_create(frame,PANEL,0) ;
 create_panel_items() ;
 ttysw  = xv_create(frame,TTY,
		     WIN_COLUMNS, 135,
		     WIN_ROWS, 22,
		     0) ;
 sprintf(cmdstring,"reduma %s\n",progname) ;
 ttysw_input(ttysw,cmdstring,strlen(cmdstring)) ;
/*
 window_fit(panel) ;

 window_fit_height(frame) ;
 window_main_loop(frame) ;	*/
 window_fit_height(frame) ;
 xv_main_loop(frame) ;
}
