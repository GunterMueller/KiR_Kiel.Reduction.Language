/* --------------------------------------------------------------------------- *
 *  nc_nn_sched.c                                                              *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 *                                                                             *
 * --------------------------------------------------------------------------- */


#include "nc_nn_sched.h"
#include "dbug.h"
#include "rncmessage.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

/* --------------------------- KONSTANTEN ----------------------------------- */
#define UNUSED -100.0         /* Verbindung zwischen Neuronen existiert nicht */
#define BUFLEN 256                                            /* Stringlaenge */

#define NO_UNIT_STR "no. of units : %d"
#define NO_CONN_STR "no. of connections : %d"
#define NO_UTYPE_STR "no. of unit types : %d"
#define NO_SITE_STR "no. of site types : %d"
#define UNIT_DEF_SEC_STR "unit definition section"
#define CONN_DEF_SEC_STR "connection definition section"
#define PTR_SIZE 4

#define NO_DIST -1

/* --------------------------- MACROS --------------------------------------- */
#define ACT_LOG(x) (1/(1+expf(-(x))))                   /* act_logistic fction */
#define ACTIVATION_FKT(x) ACT_LOG(x)
#define OUTPUT_FKT(x) (x)
                                                                                


/* ---------------------------- EXTERNE VARIABLEN ---------------------------- */
extern int proc_id;
extern int cube_dim;
typedef int (* FCTPTR) ();
extern FCTPTR dealloc_tab[];

/* ---------------------------- EXTERNE FUNKTIONEN --------------------------- */
extern void nn_msg_send();
extern void msg_check();                                   /* aus rncmessage.c */
extern void dealloc_li();


/* ---------------------------- TYP DEFINITIONEN ----------------------------- */




/* --------------------------- globale Variablen ---------------------------- */
static char source_weight_str[256];
static char DBUG_HELP_STR[256];


static float *Act;
static float *Bias;
float **Weights;                    /* die Gewichte der Neuronen der H_Layers */
float *Net_Input;

int proc_stat[3];   /* der Status des Knotens in Bezug auf die Proz. */
unsigned short Neighbours[32]=    /* Nachbarschaftszuordnung der nCube-Knoten */           
   {NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,       
    NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,
    NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,
    NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB,NO_NEIGHB};
unsigned short Message_SENT;       /* die Anzahl der versendeten Nachrichten  */

unsigned short *Unit_Type;  
unsigned short Run_Offset;  
unsigned short Wait_Offset; 
unsigned short Ready_Offset;

int no_of_units;      
int no_of_connections;
int no_of_unit_types;
int no_of_site_types;
int no_of_inunits; 
int no_of_outunits;

static int *visited;                   /* Knoten bei Travers. besucht/n. bes. */
static int *NN_update_order;        /* Reihenfolge der Berechnung der Ausgabe */



/* -------------------------------------------------------------------------- *
 * int parse_sw_str(t_no, sw_str)                                        dmk  *
 * -------------------------------------------------------------------------- *
 * durchlaeuft den String sw und legt die gefundenen Gewicht in Weights ab    *
 * -------------------------------------------------------------------------- *
 * Args : int t_no     : die Ziel (Target)-PU                                 *
 *        char *sw_str : der zu untersuchende String                          *
 * Ruckgabewert : 0 OK, 1 sonst                                               * 
 * -------------------------------------------------------------------------- */

int parse_sw_str(t_no, sw_str) 
int t_no;
char *sw_str;
{
   char *str;
   char *str_help;
   int s_no;
   float weight;
   
   DBUG_ENTER("parse_sw_str");
   
   DBUG_PRINT("NN",(" the string to parse %s", sw_str));
   str_help=(str=sw_str);

   DBUG_PRINT("NN",("From Unit %d \n",t_no));
   do {
      while ((*str_help != ',') && (*str_help != '\0'))
        str_help++;

      sscanf(str,"%d : %f,", &s_no, &weight);
      --s_no;
      Weights[t_no][s_no]=weight;

      sprintf(DBUG_HELP_STR,"to %d : %f \t",s_no, Weights[t_no][s_no]);
      DBUG_PRINT("NN",("%s", DBUG_HELP_STR));

      if (*str_help == ',') str=++str_help;
   } while ( (*str_help != '\0') && (*str != '\n'));

   DBUG_RETURN(0);
}

/* -------------------------------------------------------------------------- *
 * int init_nn()                                                         dmk  *
 * -------------------------------------------------------------------------- *
 * liest aus dem File red.net (von SNNS generiertes Net-File)                 *
 *   - die Anzahl der PUs (Neuronen), Verbindungen, PU-Typen, Site-Typen      *
 *   - die Gewichte der Verbindungen                                          *
 *   - die Schwellwerte (Bias) der PUs                                        *
 *   - die Anzahl der Site-Typen                                              *
 * Alloziert Speicher fuer div. Felder und initialisiert diese :              *
 *   -  Weights, Bias, Act, Unit_Type                                         *
 * -------------------------------------------------------------------------- *
 * Args : keine                                                               *
 * Rueckgabewert : Groesse der Process-table ( z.Z. default 100 )             *
 * -------------------------------------------------------------------------- */
int init_nn()
{
   int i;
   int j;
   int cnt=0;
   char buf[BUFLEN];
   float bias;
   int target_no;
   float act;
   char type_char;
   FILE *f;
   char fname[32];


   DBUG_ENTER("init_nn");
     /* DBUG_PRINT("NNInit",("beginning init_nn")); */
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in init_nn(start)");
   sprintf(fname,"red.net.%d",proc_id);
   if ((f=fopen(fname,"r")) == NULL) 
     if ((f=fopen("red.net","r")) == NULL) {
        post_mortem("Error in init_nn\n File not found ");
        DBUG_RETURN(100);
     }
     do {
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");
           DBUG_RETURN(100);
        } else {
           if (sscanf(buf,NO_UNIT_STR,&no_of_units)!=0) {
              cnt++;
              Bias=(float *)calloc(no_of_units,sizeof(float));
              Act=(float *)calloc(no_of_units,sizeof(float));
              Net_Input=(float *)calloc(no_of_units,sizeof(float));
              Unit_Type=(unsigned short *)calloc(no_of_units,sizeof(short));
              Weights=(float **)calloc(no_of_units,PTR_SIZE);
              visited=(int *)calloc(no_of_units,sizeof(int));
              NN_update_order=(int *)calloc(no_of_units,sizeof(int));
              for (i=0; i<no_of_units; i++) {
                Weights[i]=(float *)calloc(no_of_units,sizeof(float));
                for (j=0; j<no_of_units; j++) {
                   Weights[i][j] = UNUSED;
                }
              }
           } else if (sscanf(buf,NO_CONN_STR,&no_of_connections)!=0) cnt++;
           else if (sscanf(buf,NO_UTYPE_STR,&no_of_unit_types)!=0) cnt++;
           else if (sscanf(buf,NO_SITE_STR,&no_of_site_types)!=0) cnt++;
        }
     } while (cnt < 4);
     /* DBUG_PRINT("NNInit",("read all constants in init_nn")); */
     /* DBUG_PRINT("NNInit",("units : %d \t connections : %d \t unit_ty : %d \n",
                       no_of_units, no_of_connections, no_of_unit_types)); */


     /* ---- Read file until unit definition section begins ---------------- */
     do {
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");
           DBUG_RETURN(100);
        }
     } while (strstr(buf,UNIT_DEF_SEC_STR)==NULL);
         
     /* ---- Read file until unit definition table begins ------------------- */
     do {
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");
           DBUG_RETURN(100);
        }
     } while (strstr(buf,"---")==NULL);
         
     /* ---- Read unit definition table  ------------------------------------ */
     for(;;)  {
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");
           DBUG_RETURN(100);
        } else if (strstr(buf,"---")!=NULL) {
           break;
        } else {
           sscanf(buf,"%d | | | %f | %f | %c | ",
                  &target_no, &act, &bias, &type_char);
           Act[--target_no] = act;
           Bias[target_no]=bias;
           if (type_char=='i') {
              Unit_Type[target_no]=IN_UNIT;
              no_of_inunits++;
           } else if (type_char=='h') {
              Unit_Type[target_no]=HIDDEN_UNIT;
           } else {
              Unit_Type[target_no]=OUT_UNIT;
              no_of_outunits++;
           }
           sprintf(buf,"unit : %d Bias %f Act %f",
                        target_no, Bias[target_no], Act[target_no]);
           /* DBUG_PRINT("NNInit",("%s",buf)); */
        }
     }
         
     /* DBUG_PRINT("NNInit",("read all units in init_nn")); */
     /* ---- Read file until connection definition section begins --------- */
     do {
        if (fgets(buf,BUFLEN,f) == NULL) {            
           post_mortem("Error in init_nn\n unexpected EOF");
           DBUG_RETURN(100);      
        }                                         
     } while (strstr(buf,CONN_DEF_SEC_STR)==NULL);
                                                                          
     /* ---- Read file until connection definition table begins ------------ */
     do {                                        
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");                 
           DBUG_RETURN(100);
        }                                                                                  
     } while (strstr(buf,"---")==NULL);

     for (;;) {
        if (fgets(buf,BUFLEN,f) == NULL) {
           post_mortem("Error in init_nn\n unexpected EOF");                 
           DBUG_RETURN(100);
        } else if (strstr(buf,"---")!=NULL) {
           break;
        } else {
           if (strchr(buf,'|')!=NULL) {
	      /*  ----------------- begin read connection for new target ... */
              sscanf(buf,"%d | | %[^\n]",&target_no, source_weight_str);
              --target_no;
              parse_sw_str(target_no, source_weight_str);
	    } else {  /* read 2nd or ... line of connections for old target   */
              parse_sw_str(target_no, buf);
            }
        }
     }
     /* DBUG_PRINT("NNInit",("read all connections in init_nn")); */
  for (i=0; i<cube_dim; i++) {
     j = (proc_id ^ (1 << i));
     Neighbours[j] = i;
  }
  Run_Offset = 0;
  Ready_Offset = cube_dim;
  Wait_Offset = 2*cube_dim;
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in init_nn(end)");
  DBUG_RETURN(MAX_PROCS_PER_PROCESSOR*2);
}

/* -------------------------------------------------------------------------- *
 * int calculate(float val_vec[])                                        dmk  *
 * -------------------------------------------------------------------------- *
 * berechnet die Ausgabe des NN auf Eingabe in val_vec[Eingabevektoren]       *
 * -------------------------------------------------------------------------- *
 * Args : val_vec - Vektor der Ausgaben der einzelnen Neuronen                *
 *                  die Eingabewerte entsprechen den Ausgaben der I-Neuronen  *
 * -------------------------------------------------------------------------- *
 * glob. Vars : Weights                                                       *
 * -------------------------------------------------------------------------- */
int calculate(val_vec)
float val_vec[];
{
  int i;
  int target;              /* das Neuron, dessen aktiv. Wert zu berechnen ist */
  int source;               /* das Neuron, das einen link zu "target" besitzt */
  int prev_n;
  int act_num_n;
  int prev_num_n;
  float temp=0.0;

  DBUG_ENTER("calculate");

  for (i=0;i<no_of_inunits; i++) {          
     sprintf(DBUG_HELP_STR, "Target : %d (Bias[%d]=%f) Out= %f\n",
           i,i,Bias[i],val_vec[i]);
     DBUG_PRINT("NNCalc",("%s", DBUG_HELP_STR));
  }

  for (i=no_of_inunits; i<no_of_units; i++) {          
     target=i;
     DBUG_PRINT("NNCalc",("-------------------------------------------------------\n"));
     sprintf(DBUG_HELP_STR, "Target : %d (Bias[%d]=%f)\n",
                                   target,target,Bias[target]);
     DBUG_PRINT("NNCalc",("%s", DBUG_HELP_STR));
     DBUG_PRINT("NNCalc",(" src \t weight \t output \t summe \n"));
        temp=0.0;
        for (source=0; source<target; source++) {
           if (Weights[target][source] != UNUSED) {
              temp+=Weights[target][source]*val_vec[source];
              sprintf(DBUG_HELP_STR," %d \t %f \t %f \t %f \n", 
                                source, Weights[target][source],val_vec[source],  temp);
              DBUG_PRINT("NNCalc",("%s", DBUG_HELP_STR));
           }
        } 
        temp+=Bias[target];
        val_vec[target]=ACT_LOG(temp);
        sprintf(DBUG_HELP_STR, "Ausgabe [%d] (temp=%f,bias=%f) =  %f \n ",
                                  target, temp, Bias[target], val_vec[target]);
        DBUG_PRINT("NNCalc",("%s", DBUG_HELP_STR));
        DBUG_PRINT("NNCalc",("-------------------------------------------------------\n"));
  } /* for  */
  DBUG_RETURN(1);
}


/* -------------------------------------------------------------------------- *
 * int nn_decide()                                                            *
 * -------------------------------------------------------------------------- *
 * - fordert zunaechst die aktuelle Prozessverteilung seiner Nachbarknoten    *
 *   an (per Message-Passing)                                                 *
 * - entscheidet durch ein trainiertes NN ob und wohin eine Verteilung        *
 *   eines Ausdruckes durchgefuehrt werden soll                               *
 * ACHTUNG : es wird eine first-fit Strategie benutzt                         *
 * -------------------------------------------------------------------------- *
 * 
 * -------------------------------------------------------------------------- */
int nn_decide()
{
    int cnt=0;
    int cube_size=0;
    float total_procs=0.0;
    int i;

    DBUG_ENTER("nn_decide");
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in nn_decide");
    DBUG_PRINT("NNDecide", (" Eingabe/Prozess-Verteilung auf Knoten %d\n", proc_id));
    
    /* --- an alle Nachbarn einen Status-Request senden ---------------------- */
    cube_size = 1<<cube_dim;
    for (i=0; i<cube_size; i++ ) {
      if ( Neighbours[i] != NO_NEIGHB ) {              /* Knoten i ist Nachbar */
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in nn_decide bef. msg_send");
       nn_msg_send(i,MT_STAT_REQ);
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in nn_decide after. msg_send");
       Message_SENT++;
       while (Message_SENT != 0) { 

 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error before msg_check in nn_decide");
   msg_check(); };
       }
    } /* of for */
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error before calculate");
    calculate(Net_Input);
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error after calculate");
    for (i=0; i<no_of_units; i++) {
       if (Unit_Type[i]==OUT_UNIT) {                       /* Output-Neuron ? */
          sprintf(DBUG_HELP_STR, " Ausgabe von NN[%d] = %f \n Nachbar %d "
                               ,(i+1)
                               ,Net_Input[i]
                               ,proc_id^(1<<cnt));
           DBUG_PRINT("NNdecide",("%s", DBUG_HELP_STR));
 if ((void *)&dealloc_li != (void *)dealloc_tab[3]) post_mortem("dealloc_tab Error in nn_decide");
          if (Net_Input[i]>=0.7) {                   /* Output-Neuron aktiv ? */
             total_procs=Net_Input[cnt+Run_Offset]+
                         Net_Input[cnt+Wait_Offset]+
                         Net_Input[cnt+Ready_Offset];
             if (total_procs < (MAX_PROCS_PER_PROCESSOR/NN_SCALE) )
                DBUG_RETURN(proc_id^(1<<cnt));       /* zug. Nachbarknoten best. */
             else
                DBUG_RETURN(NO_DIST);
          }
          cnt++;
       }
    }
    DBUG_RETURN(NO_DIST);
}
