/* $Log: c_main.c,v $
 * Revision 1.17  1996/07/03 08:47:07  rs
 * D_DISTRIBUTE-definitions moved to prolog.h
 *
 * Revision 1.16  1996/04/11  13:27:29  cr
 * prepare for separate compilation (again:-(
 * step I: avoid use of program-specific global variables
 * in tasm-generated code inside initdata.c and rmkclos.c
 * (cf. compile.c c_output.c)
 *
 * Revision 1.15  1996/03/08  13:29:36  rs
 * D_DISTRIBUTE definition fixed
 *
 * Revision 1.14  1996/02/23  14:13:13  rs
 * new (t)ASM commands: count(x), msdistend, msnodist
 *
 * Revision 1.13  1996/02/21  17:22:26  rs
 * some more changes for D_MESS
 *
 * */

#include <sys/time.h>

int ta_heapsize = 2048*2048, ta_no_of_desc = 10000, ta_stacksize = 102400;
double start_time;

#if (D_SLAVE && D_MESS)
#include "d_mess_io.h"
extern PROCESS_CONTEXT* curr_pid;
extern INSTR *distribute();
extern PROCESS_CONTEXT *process_create();
extern void process_terminate();
extern void process_sleep();
extern void process_wakeup();
extern void process_run();
extern void msg_send();
extern void msg_receive();
extern int proc_tab_size;
#endif

int main(int argc, char * argv[])
{
  extern void trap_handler();
  extern int intact_tasm(void);
  int dummy, no_output = 0;
  char c, * dbug_args = NULL;
  extern int red_cnt;
  extern char * optarg;
  extern int optind, opterr;
  extern int getopt(int, char **, char *), controlled_exit(const char *);
  extern char ri_tasmoutfile[120];
#if D_SLAVE
  extern int stack_seg_size;
  extern int ticket_pool_counter;
  char filename[80];
  int type,flags=-1;
#endif /* D_SLAVE */
  struct timeval t0;

  DBUG_ENTER("main");

#if D_SLAVE
  whoami(&proc_id,&dummy,&host,&cube_dim);
#endif

  DBUG_PRINT("AUX", ("parsing command line arguments..."));

  while((c = getopt(argc, argv, "Z:E:X:M:Q:Y:m:q:x:y:z:n:f:t:V:#:k:h:d:R:I:s:o")) != -1) {
    DBUG_PRINT("AUX", ("opt = %c", c));
    switch(c)
    {
      case 'V': /* only for the debugger */

      case '#': 
#if D_SLAVE
               if (((int)rindex(optarg,':')-(int)optarg)==
                   ((int)index(optarg,'o')-1-(int)optarg)) { /* :o ? */
                 /* appending number of processor */
                 sprintf(filename, "%s%d", optarg, proc_id);
                 DBUG_PUSH(filename);
                 DBUG_PRINT("AUX", ("dbug-args: %s", filename));
                 } 
               else {
                 DBUG_PUSH(dbug_args = optarg);
                 DBUG_PRINT("AUX", ("dbug-args: %s", optarg));
                 }
#else
                DBUG_PUSH(dbug_args = optarg); 
#endif
                DBUG_PRINT("AUX", ("fred fish enabled..."));
                break;
#if (D_SLAVE && D_MESS)
      case 'm': d_bib_mask = atoi(optarg);
                DBUG_PRINT("AUX", ("d_bib_mask: %d", d_bib_mask));
                break;
      case 'q': m_merge_strat = atoi(optarg);
                DBUG_PRINT("AUX", ("m_merge_strat: %d", m_merge_strat));
                break;
      case 'x': m_ackno = atoi(optarg);
                DBUG_PRINT("AUX", ("m_ackno: %d", m_ackno));
                break;
      case 'y': sprintf(m_mesfilehea, "%s", optarg);
                DBUG_PRINT("AUX", ("m_mesfilehea: %s", m_mesfilehea));
                break;
      case 'z': sprintf(m_mesfilepath, "%s", optarg);
                DBUG_PRINT("AUX", ("m_mesfilepath: %s", m_mesfilepath));
                break;
/*      case 'E': sprintf(_errmes, "%s", optarg);
                DBUG_PRINT("HOST", ("_errmes: %s", _errmes));
                break;
      case 'X': sprintf(m_remote_host, "%s", optarg);
                DBUG_PRINT("HOST", ("m_remote_host: %s", m_remote_host));
                break; 
      case 'M': m_del_files = atoi(optarg);
                DBUG_PRINT("HOST", ("m_del_files: %d", m_del_files));
                break;
      case 'Q': sprintf(m_target_dir, "%s", optarg);
                DBUG_PRINT("HOST", ("m_target_dir: %s", m_target_dir));
                break;
      case 'Y': sprintf(m_remote_dir, "%s", optarg);
                DBUG_PRINT("HOST", ("m_remote_dir: %s", m_remote_dir));
                break;  
      case 'Z': sprintf(m_remote_bin, "%s", optarg);
                DBUG_PRINT("HOST", ("m_remote_bin: %s", m_remote_bin));
                break; */
#endif /* (D_SLAVE && D_MESS) */
      case 'k': AsciiOutput = 0; KiROutput = 1; strcpy(ri_tasmoutfile, optarg); 
                DBUG_PRINT("AUX", ("outfile: %s", ri_tasmoutfile));
                break;
      case 'h': ta_heapsize = atoi(optarg); 
                DBUG_PRINT("AUX", ("ta_heapsize: %d", ta_heapsize));
                break;
      case 's': ta_stacksize = atoi(optarg); 
                DBUG_PRINT("AUX", ("ta_stacksize: %d", ta_stacksize));
                break;
      case 'd': ta_no_of_desc = atoi(optarg); 
                DBUG_PRINT("AUX", ("ta_no_of_desc: %d", ta_no_of_desc));
                break;
      case 'R': red_cnt = atoi(optarg); 
                DBUG_PRINT("AUX", ("red_cnt: %d", red_cnt));
                break;
      case 'o': no_output = 1; 
                DBUG_PRINT("AUX", ("no_output: %d", no_output));
                break;
/* cr 28/03/95, kir(ff), START */
      case 'I': inter_allowed = atoi(optarg); 
                DBUG_PRINT("AUX", ("inter_allowed: %d", inter_allowed)); 
                break;
/* cr 28/03/95, kir(ff), END */
    }
  }

  DBUG_PRINT("AUX", ("parsing command line arguments done !"));

  /* time starts NOW 

  gettimeofday (&t0, NULL);
  start_time = t0.tv_sec + t0.tv_usec/1000000.0;   NOT !!!! ;-) */

  signal (SIGFPE, trap_handler);
  _base = 10000;
  InitHeapManagement(ta_heapsize, ta_no_of_desc);
#if D_SLAVE
  stack_seg_size = 256;
  ticket_pool_counter = 0;
  alloc_stack_memory(ta_stacksize);
#else /* D_SLAVE */
  if (NULL ==(_st_w = (int *)malloc(sizeof(int) * ta_stacksize)))
  {
    if (KiROutput)
      controlled_exit("unable to allocate stack memory");
    else
    {
      fprintf(stderr, "unable to allocate stack memory\n");
      exit(1);
    }
  }  
  if (NULL ==(_st_a = (int *)malloc(sizeof(int) * ta_stacksize)))
  {
    if (KiROutput)
      controlled_exit("unable to allocate stack memory");
    else
    {
      fprintf(stderr, "unable to allocate stack memory\n");
      exit(1);
    }
  }  
  if (NULL ==(_st_t = (int *)malloc(sizeof(int) * ta_stacksize)))
  {
    if (KiROutput)
      controlled_exit("unable to allocate stack memory");
    else
    {
      fprintf(stderr, "unable to allocate stack memory\n");
      exit(1);
    }
  }  
  if (NULL ==(_st_r = (int *)malloc(sizeof(int) * ta_stacksize)))
  {
    if (KiROutput)
      controlled_exit("unable to allocate stack memory");
    else
    {
      fprintf(stderr, "unable to allocate stack memory\n");
      exit(1);
    }
  }  
  st_w = _st_w;
  st_a = _st_a;
  st_t = _st_t;
  st_r = _st_r;
#endif /* D_SLAVE */
  build_dyn_funcdesctable(funcdesctable,stat_funcdesctable, number_of_descs);
  build_static_clos(funcclostable,funcdesctable,number_of_descs);
  initdata();
#if D_SLAVE
  DBUG_PRINT("AUX", ("I'm Processor %d out of %d", proc_id, 1<<cube_dim));
  if (cube_dim && dbug_args) {
    char dbug_buf[256];
    DBUG_PRINT("AUX", ("I'm here inside"));
    sprintf(dbug_buf, "dbug%d", (int)proc_id);
    unlink(dbug_buf);
    sprintf(dbug_buf, "%s:o,dbug%d", dbug_args, (int)proc_id);
    DBUG_POP();
    DBUG_PUSH(dbug_buf);
    }
    DBUG_PRINT("AUX", ("I'm here outside before nc_init"));
  nc_init(ta_heapsize,ta_stacksize);

#if D_MESS

/* sorry, some D_MESS code extracted from rncmessage.c */
/* I don't think there's need for another .c-file */
   
  d_init_mess(proc_id);

#if (D_PVM && !D_PVM_NCUBE)
      MPRINT_SPECIAL(D_MESS_FILE, M_TIMER, PROCESS_NR, -1, proc_tab_size, m_base);
#else
      MPRINT_SPECIAL(D_MESS_FILE, 0.0, PROCESS_NR, -1, proc_tab_size, m_base);
#endif /* D_PVM */

  if (d_bib_mask & (1 << MINDEX_PROC_DIST)) {
    DBUG_PRINT ("MSG", ("PROC DIST measure enabled !"));
    d_m_distribute = m_distribute; }
  else
    d_m_distribute = distribute;
  if (d_bib_mask & (1 << MINDEX_PROC_CREATE)) {
    DBUG_PRINT ("MSG", ("PROC CREATE measure enabled !"));
    d_m_process_create = m_process_create; }
  else
    d_m_process_create = process_create;
  if (d_bib_mask & (1 << MINDEX_PROC_TERMINATE)) {
    DBUG_PRINT ("MSG", ("PROC TERMINATE measure enabled !"));
    d_m_process_terminate = m_process_terminate; }
  else
    d_m_process_terminate = process_terminate;
  if (d_bib_mask & (1 << MINDEX_PROC_SLEEP)) {
    DBUG_PRINT ("MSG", ("PROC SLEEP measure enabled !"));
    d_m_process_sleep = m_process_sleep; }
  else
    d_m_process_sleep = process_sleep;
  if (d_bib_mask & (1 << MINDEX_PROC_RUN)) {
    DBUG_PRINT ("MSG", ("PROC RUN measure enabled !"));
    d_m_process_run = m_process_run; }
  else
    d_m_process_run = process_run;
  if (d_bib_mask & (1 << MINDEX_PROC_WAKEUP)) {
    DBUG_PRINT ("MSG", ("PROC WAKEUP measure enabled !"));
    d_m_process_wakeup = m_process_wakeup; }
  else
    d_m_process_wakeup = process_wakeup;

  d_m_msg_send = msg_send;
  d_m_msg_receive = msg_receive;

  if (d_bib_mask & (1 << MINDEX_SEND_BEGIN)) {
    DBUG_PRINT ("MSG", ("SEND BEGIN measure enabled !"));
    d_m_msg_send = m_msg_send;
    m_msg_send_begin = 1; }
  else
    m_msg_send_begin = 0;
  if (d_bib_mask & (1 << MINDEX_SEND_END)) {
    DBUG_PRINT ("MSG", ("SEND END measure enabled !"));
    d_m_msg_send = m_msg_send;
    m_msg_send_end = 1; }
  else
    m_msg_send_end = 0;
  if (d_bib_mask & (1 << MINDEX_RECEIVE_BEGIN)) {
    DBUG_PRINT ("MSG", ("RECEIVE BEGIN measure enabled !"));
    d_m_msg_receive = m_msg_receive;
    m_msg_receive_begin = 1; }
  else
    m_msg_receive_begin = 0;
  if (d_bib_mask & (1 << MINDEX_RECEIVE_END)) {
    DBUG_PRINT ("MSG", ("RECEIVE END measure enabled !"));
    d_m_msg_receive = m_msg_receive;
    m_msg_receive_end = 1; }
  else
    m_msg_receive_end = 0;

  m_init_merge();
#endif /* D_MESS */

  DBUG_PRINT("AUX", ("I'm here outside after nc_init"));

  /* time starts NOW */

  gettimeofday (&t0, NULL);
  start_time = t0.tv_sec + t0.tv_usec/1000000.0; 

  if (proc_id == 0)
#if (D_SLAVE && D_MESS && D_MSCHED)
    (*d_m_process_create)(map(0,0), NULL, red_cnt);
#else
    process_create(map(0,0), NULL, red_cnt);
#endif
  for(;;) {
    DBUG_PRINT("AUX", ("entering scheduler"));
    schedule();
    push_r((STACKELEM)unmapl((int)code));
    DBUG_PRINT("EXEC", ("code=%x",code));
    /* subject to change */
    switch(apply(R_COMB(*(funcdesctable[unmapf((int)code)]), ptc))) {
      case IR_EXT:
        DBUG_PRINT("AUX", ("execution done"));
        t_output(pop(w),number_of_descs,funcdesctable,number_of_data,data_table,ptc_table);
        fprintf(stdout,"\n");
#if (D_SLAVE && D_MESS && D_MSCHED)
        (*d_m_process_terminate)(curr_pid);
#else
        process_terminate(curr_pid);
#endif
        DBUG_PRINT("AUX", ("sending message back to the host..."));
        type=MT_RESULT;
        nwrite(NULL,0,host,type,&flags);
        /* exit(0); ?!              slaves will be terminated by host.c */
        break;
      case IR_DIST_END:
        DBUG_PRINT("AUX", ("distribution done"));
#if (D_SLAVE && D_MESS && D_MSCHED)
        (*d_m_process_terminate)(curr_pid);
#else
        process_terminate(curr_pid);
#endif
        break;
      case IR_WAIT:
        DBUG_PRINT("AUX", ("process suspended"));
#if (D_SLAVE && D_MESS && D_MSCHED)
        (*d_m_process_sleep)(curr_pid);
#else
        process_sleep(curr_pid);
#endif
        break;
      }
      if (sig_msg_flag)
        msg_check();
    }
#else /* D_SLAVE */

  /* time starts NOW */

  gettimeofday (&t0, NULL);
  start_time = t0.tv_sec + t0.tv_usec/1000000.0;  

  DBUG_PRINT("AUX", ("running program"));
  push_label(0);
  push_r(((int)(&intact_tasm) | FUNCTIONRETURN));
  push_label(0);
  apply(goal);
  if (!no_output)
    t_output(pop(w),number_of_descs,funcdesctable,number_of_data,data_table,ptc_table);
  fprintf(stdout,"\n");
  exit(0);
#endif /* D_SLAVE */
}

