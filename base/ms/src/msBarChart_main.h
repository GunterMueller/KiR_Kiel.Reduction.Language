#include <X11/At/Plotter.h>
#include <X11/At/XYAxis.h>
#include <X11/At/XYLinePlot.h>
#include <X11/At/BarPlot.h>
#include <X11/At/LabelAxis.h>

#define CREATE_PLOTTER(L, C, W) XtVaCreateManagedWidget(L, C, W,\
							XtNheight, 100,\
							XtNwidth, 100,\
							NULL)
#define CREATE_AXIS(L, C, W, N, V) XtVaCreateManagedWidget(L, C, W,\
							   XtNlabel, N,\
							   XtNvertical, V,\
							   NULL)
#define ASSIGN_AXES(W, X, Y) XtVaSetValues(W, XtNxAxis, X, XtNyAxis, Y,	NULL)
#define SET_BOUNDS(W, L, U) XtVaSetValues(W,\
					  XtNmin, &L,\
					  XtNmax, &U,\
					  XtNautoScale, FALSE,\
					  NULL)
#define SET_TYPE(W, I) SET_VALUE(W, XtNaxisTransform,\
                                 (I == LINEAR ?\
                                  AtTransformLINEAR :\
                                  AtTransformLOGARITHMIC))
#define SET_GRID(W, B) SET_VALUE(W, XtNdrawGrid, B)
#define SET_SUBGRID(W, B) SET_VALUE(W, XtNdrawSubgrid, B)
#define SAVE_PS(f, n) AtPlotterGeneratePostscript(f,\
                                                  (AtPlotterWidget) plotter,\
			                          n, 50, 50, 520, 400, FALSE);


#define NUM_MODES 8                     /* Number of available plotter modes */
#define NUM_LINES 4                       /* Number of available line styles */
#define NUM_MARKERS 9                    /* Number of available marker types */
#define NUM_SHADES colors[0].red+11   /* Number of available colors for bars */
#define MAX_BARS 1000                 /* Maximal number of bars in Bar-Chart */
#define MAX_POINTS 100000                /* Maximal number of points in Plot */
#define MAX_PLOTS 100                 /* Maximal number of plots per picture */
#define MINIMAL_X_ZOOM 0.01                         /* Don't zoom in further */
#define MINIMAL_Y_ZOOM 0.01                         /* Don't zoom in further */

extern Widget WI_ms_bar_chart;
extern Widget WI_ms_bar_chart_graphic_box;
extern Widget WI_ms_position_panel_show_line;
extern Widget WI_ms_position_panel_enter_frame;
extern Widget WI_ms_position_panel_enter_time;
extern Widget WI_ms_control_panel_go_to_start;
extern Widget WI_ms_control_panel_play_backward;
extern Widget WI_ms_control_panel_skip_backward;
extern Widget WI_ms_control_panel_stop;
extern Widget WI_ms_control_panel_skip_forward;
extern Widget WI_ms_control_panel_play_forward;
extern Widget WI_ms_control_panel_go_to_end;
extern Widget WI_ms_speed_panel_enter_delay;
extern Widget WI_ms_speed_panel_scroll_left_delay;
extern Widget WI_ms_speed_panel_scroll_right_delay;
extern Widget WI_ms_speed_panel_scroll_delay;
extern Widget WI_ms_speed_panel_enter_rate;
extern Widget WI_ms_speed_panel_scroll_left_rate;
extern Widget WI_ms_speed_panel_scroll_right_rate;
extern Widget WI_ms_speed_panel_scroll_rate;
extern Widget WI_ms_cursor_panel_show_x;
extern Widget WI_ms_cursor_panel_show_y;
extern Widget WI_ms_bar_chart_display_enter_title;
extern Widget WI_ms_bar_chart_display_legend;
extern Widget WI_ms_bar_chart_display_x_title;
extern Widget WI_ms_bar_chart_display_x_log;
extern Widget WI_ms_bar_chart_display_x_grid;
extern Widget WI_ms_bar_chart_display_y_title;
extern Widget WI_ms_bar_chart_display_y_log;
extern Widget WI_ms_bar_chart_display_y_grid;
extern Widget WI_ms_bar_chart_zoom_enter_x_from;
extern Widget WI_ms_bar_chart_zoom_enter_x_to;
extern Widget WI_ms_bar_chart_zoom_enter_y_from;
extern Widget WI_ms_bar_chart_zoom_enter_y_to;
extern Widget WI_ms_file_panel_enter_path;
extern Widget WI_ms_file_panel_enter_name;
extern Widget WI_ms_script_panel_enter_path;
extern Widget WI_ms_script_panel_enter_name;
extern Widget WI_ms_script_panel_mode;
extern Widget WI_ms_script_panel_type;
extern Widget WI_editor_edit_window;
extern Widget WI_ms_file_panel_filter;


#define BARS_LEN 100

struct BarsArray {
  char bar[BARS_LEN];
};


struct BarChartElement {
  double time;
  int frame;
  double diff;
  short bar;
};


/* (wasUpdate == TRUE) --> change (x,y) to (x, y2) 
** (wasUpdate == FALSE) --> (wasAdd == TRUE) --> Add point at (x,y)
**                          (wasAdd == FALSE) --> Remove point at (x,y)
*/

struct XYPlotElement {
  double time;
  int frame;
  Boolean wasUpdate;
  double x, y;
  union {
    double y2;
    Boolean wasAdd;
  } extra;
} dummyElement;


struct Colors {
  int red, green, blue;
  Pixel pixel;
} colors[] = {
  {    14,     14,     14, 0}, /* num  */
  {0xff00, 0xff00, 0xff00, 0}, /* #ffffff */
  {0x0000, 0x0000, 0x0000, 0}, /* #000000 */
  {0xcc00, 0x0000, 0x0000, 0}, /* #cc0000 */
  {0x9900, 0x0000, 0x0000, 0}, /* #990000 */
  {0xff00, 0x0000, 0x0000, 0}, /* #ff0000 */
  {0x0000, 0xcc00, 0x0000, 0}, /* #00cc00 */
  {0x0000, 0x9900, 0x0000, 0}, /* #009900 */
  {0x0000, 0xff00, 0x0000, 0}, /* #00ff00 */
  {0x0000, 0x0000, 0xcc00, 0}, /* #0000cc */
  {0x0000, 0x0000, 0x9900, 0}, /* #000099 */
  {0x0000, 0x0000, 0xff00, 0}, /* #0000ff */
  {0xcc00, 0xcc00, 0x0000, 0}, /* #cccc00 */
  {0x9900, 0x9900, 0x0000, 0}, /* #999900 */
  {0xff00, 0xff00, 0x0000, 0}  /* #ffff00 */
};
