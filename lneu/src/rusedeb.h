/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */

extern int  _debug_info ; /* rstate.c */

extern int stop_on_stop;         /* rstate.c */
extern int stop_on_error;        /* rstate.c */
extern int stopped_on_stop;      /* rstate.c */
extern int stopped_on_error;     /* rstate.c */
extern unsigned int  _saved_redcnt ; /* rstate.c */
extern unsigned _redcnt;

#define ERR_FAIL \
        if (stop_on_error)   \
            {    stopped_on_error = 1 ; /*TRUE*/     \
                 PUSHSTACK(S_m,SET_ARITY(K_ERROR,1)); \
                 PUSHSTACK(S_m1,SET_ARITY(K_ERROR,1)); \
                 _saved_redcnt = _redcnt;               \
                 _redcnt=0 ;                             \
            }                                             \
            else
