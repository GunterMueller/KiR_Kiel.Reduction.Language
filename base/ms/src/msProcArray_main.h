#include <X11/At/Plotter.h>
#include <X11/At/XYAxis.h>
#include <X11/At/XYLinePlot.h>
#include <X11/At/BarPlot.h>
#include <X11/At/LabelAxis.h>

#define SAVE_PS(f, n) generatePostscript(f, n)

#define PS_BORDER_COLOR      0.0
#define PS_ARROW_COLOR       0.0
#define PS_HISTORY_COLOR     0.0
#define PS_FUTURE_COLOR      0.0
#define PS_ACTIVE_COLOR      0.4
#define PS_READY_COLOR       0.6
#define PS_SUSPENDED_COLOR   0.8
#define PS_TERMINATED_COLOR  1.0
#define PS_LOAD_COLOR        0.5
#define PS_COMM_COLOR        0.0
#define PS_IDLE_COLOR        1.0
#define PS_TEXT_COLOR        0.0
#define PS_LABEL_COLOR       1.0
#define PS_BORDER_WIDTH      1.0
#define PS_ARROW_WIDTH      10.0
#define PS_HISTORY_WIDTH     7.0
#define PS_FUTURE_WIDTH      4.0

#define MAX_ARROWS MAX_PROCESSORS*MAX_PROCESSES

#define LEFT_MARGIN 10       /* Space (in pixel) at left boundary of graphic */
#define RIGHT_MARGIN 10     /* Space (in pixel) at right boundary of graphic */
#define TOP_MARGIN 10                  /* Space (in pixel) at top of graphic */
#define BOTTOM_MARGIN 10            /* Space (in pixel) at bottom of graphic */

#define X_SLOT_FACTOR 1.0                       /* Relative size of one slot */
#define Y_SLOT_FACTOR 0.8                       /* Relative size of one slot */

#define LOADOMETER_GAP 10         /* Space between graphics and Load-O-Meter */
#define LOADOMETER_SIZE 50                           /* Size of Load-O-Meter */

#define MIN_HISTORY 0           /* Boundaries for History- and Future-Levels */
#define MAX_HISTORY 1000
#define MIN_FUTURE 0
#define MAX_FUTURE 1000

#define MAX_HISTORY_NUM 1000   /* Remember 1000 History-Clicks for PS-Output */
#define MAX_FUTURE_NUM  1000    /* Remember 1000 Future-Clicks for PS-Output */

#define TITLE_LEN 1000


extern Widget WI_ms_proc_array;
extern Widget WI_ms_proc_array_graphic_box;
extern Widget WI_ms_proc_array_graphic_area;
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
extern Widget WI_ms_file_panel_enter_path;
extern Widget WI_ms_file_panel_enter_name;
extern Widget WI_ms_script_panel_enter_path;
extern Widget WI_ms_script_panel_enter_name;
extern Widget WI_ms_script_panel_mode;
extern Widget WI_ms_script_panel_type;
extern Widget WI_editor_edit_window;
extern Widget WI_ms_file_panel_filter;
extern Widget WI_ms_history_panel_enter_level;
extern Widget WI_ms_history_panel_scroll;
extern Widget WI_ms_history_panel_scroll_left;
extern Widget WI_ms_history_panel_scroll_right;
extern Widget WI_ms_future_panel_enter_level;
extern Widget WI_ms_future_panel_scroll;
extern Widget WI_ms_future_panel_scroll_left;
extern Widget WI_ms_future_panel_scroll_right;
extern Widget WI_ms_display_panel_enter_title;
extern Widget WI_ms_display_panel_none_style;
extern Widget WI_ms_display_panel_text_style;
extern Widget WI_ms_display_panel_color_style;
extern Widget WI_ms_display_panel_set_load_o_meter;


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

/* (action == CREATE) --> "Coordinates" in s[0] to s[3].
** (action == FILL) --> "Coordinates" in s[0] and s[1],
**                      old state in s[2], new state in s[3],
**                      father in s[4] and s[5].
** (action == HISTORY) --> "Coordinates" in s[0] and s[1], level in s[2].
** (action == FUTURE) --> "Coordinates in s[0] and s[1], level in s[2].
*/

struct Element {
  double time;
  int frame;
  char action;
  short s[6];
} dummyElement;

struct Processes {
  short fatherProcessor, fatherProcess, state;
};
