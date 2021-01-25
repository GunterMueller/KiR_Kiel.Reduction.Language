/***************************************************************************/
/*                                                                         */
/*   file    : d_mess_merge.h                                              */
/*                                                                         */
/*   contents: distributed merging includes                                */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         16.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

#define M_MAX_FILES        3          /* define for max open measure files for one node */
#define M_MAX_CHANNELS     4          /* define for max open measure channels           */

#define M_INPUT_MAX        7          /* M_MAX_FILES + M_MAX_CHANNELS */

#define M_MAX_LINE         120        /* max length of input line without time mark */

#define M_MAX_BUFFER       60000      /* communication input buffer */

/* type definitions */

typedef struct m_input *m_input_ptr;

typedef struct m_input {
           int (*p_m_read_line)();
           void *m_entry1;         /* FILE pointer OR dereference for buffers */
           int  m_entry2;          /* nr. of source node */
        } m_input_struct;

/* extern functions */

extern void m_init_merge();
extern void m_start_merge();

extern int m_read_amline();
extern int m_read_acline();
extern FILE *m_open_mfile();

/* extern variables */
