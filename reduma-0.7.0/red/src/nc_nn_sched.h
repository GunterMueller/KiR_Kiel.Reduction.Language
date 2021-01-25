/* --------------------------- KONSTANTEN ----------------------------------- */
#define P_READY 0                             /* die moegl. Prozess-Zustaende */
#define P_RUN   1
#define P_WAIT  2

#define NO_NEIGHB 64                /* zwischen 2 Knoten sind nicht benachbart */

#define NN_SCALE 20.0
#define MAX_PROCS_PER_PROCESSOR 20


/* --------------------------- MACROS --------------------------------------- */


/* ---------------------------- TYP DEFINITIONEN ----------------------------- */
enum N_Type {IN_UNIT,OUT_UNIT,HIDDEN_UNIT};


/* --------------------------- externe  Variablen ---------------------------- */



/* --------------------------- globale Variablen ---------------------------- */
extern float **Weights;                    /* die Gewichte der Neuronen der H_Layers */
extern float *Net_Input;

extern int proc_stat[3];   /* der Status des Knotens in Bezug auf die Proz. */
extern unsigned short Neighbours[32];    /* Nachbarschaftszuordnung der nCube-Knoten */
extern unsigned short Message_SENT;       /* die Anzahl der versendeten Nachrichten  */

extern unsigned short *Unit_Type;
extern unsigned short Run_Offset;
extern unsigned short Wait_Offset;
extern unsigned short Ready_Offset;


extern int no_of_units;
extern int no_of_connections;
extern int no_of_unit_types;
extern int no_of_site_types;
extern int no_of_inunits;
extern int no_of_outunits;
extern int no_of_patterns;
