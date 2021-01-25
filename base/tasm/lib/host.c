/* $Log: host.c,v $
 * Revision 1.5  1996/02/23 16:42:54  rs
 * minor changes
 *
 * Revision 1.4  1996/02/21  17:22:11  rs
 * more options for D_MESS
 *
 * */

#include "prolog.h"
#include <stdio.h>
#include "dbug.h"
#if nCUBE
#include <nhost.h>
#endif /* nCUBE */
#include "rncmessage.h"

#if D_MESS
extern char m_mesfilepath[], m_mesfilehea[], m_remote_host[], m_target_dir[],  
            m_remote_dir[], m_remote_bin[];
extern int m_merge_strat,m_ackno,m_del_files;
#endif /* D_MESS */

extern int _formated, red_cnt;
extern int cube_dim;
int m_merge_strat;
extern char *_errmes;

int cube, subcube;
extern char ticket_file[];
char *cubebin = "a.out";
int measure_enabled = 0;

void main(int argc, char * argv[])
{
  extern char * optarg;
  extern int optind, opterr;
  extern int getopt(int, char **, char *);
  extern int stack_seg_size;
  extern int ticket_pool_counter;
  int src=0, type=-1;
  char buffer[]="nix drinne hier... :-)";
  char c;

  DBUG_ENTER("main");
 
  DBUG_PUSH ("d:t");

  _errmes=malloc(80);

  DBUG_PRINT("HOST", ("parsing options..."));

  while((c = getopt(argc, argv, "Z:E:X:M:Q:Y:m:q:y:z:d:x:n:t:f:V:#:k:h:s:R:oI:"))  != -1)
  {
    switch(c)
    {
      case 'n': cube_dim = atoi(optarg); 
                DBUG_PRINT("HOST", ("cube_dim: %d", cube_dim));
                break;
      case 't': sprintf(ticket_file, "%s", optarg);
                DBUG_PRINT("HOST", ("ticket_file: %s", ticket_file));
                break;
      case 'f': cubebin = optarg; 
                DBUG_PRINT("HOST", ("cubebin: %s", cubebin));
                break;
#if D_MESS
      case 'q': m_merge_strat = atoi(optarg); 
                DBUG_PRINT("HOST", ("m_merge_strat: %d", m_merge_strat));
                break;
      case 'x': m_ackno = atoi(optarg); 
                DBUG_PRINT("HOST", ("m_ackno: %d", m_ackno));
                break;
      case 'y': sprintf(m_mesfilehea, "%s", optarg); 
                DBUG_PRINT("HOST", ("m_mesfilehea: %s", m_mesfilehea));
                break;
      case 'z': sprintf(m_mesfilepath, "%s", optarg); 
                DBUG_PRINT("HOST", ("m_mesfilepath: %s", m_mesfilepath));
                break;
      case 'E': sprintf(_errmes, "%s", optarg);
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
                break;
#else /* D_MESS */
      case 'q':
      case 'x':
      case 'y':
      case 'z':
      case 'E':
      case 'X':
      case 'M':
      case 'Q':
      case 'Y':
                printf("\nHOST program not compiled with D_MESS=1...\n\n");
                break;
#endif /* D_MESS */
    }
  }
  
  DBUG_PRINT("HOST", ("parsing options finished..."));

  if ((cube = nopen(cube_dim)) < 0)
   { printf("error: could not allocate subcube!"); exit(1);}
  subcube = nodeset(cube,"all");

  DBUG_PRINT("HOST", ("first available argv-string: %s", argv[1]));
  DBUG_PRINT("HOST", ("Ich installiere nCUBE-Binary : %s", cubebin));
  if (rexecv(subcube,cubebin,argv).np_pid == -1)
    {printf("init_ncube: cannot install nCUBE binary !"); exit(1);}
  wait_for_ncube(cube_dim);
  DBUG_PRINT("HOST", ("nCUBE ready"));

  /* waiting for the result */
 
  DBUG_PRINT("HOST", ("waiting for slave 0..."));
  nread(cube, buffer, 10, &src, &type);
  DBUG_PRINT("HOST", ("signal received from slave 0..."));

  if (type!=MT_RESULT) {
    DBUG_PRINT ("HOST", ("result: %d", type));
    DBUG_PRINT ("HOST", ("execution failed (somehow)... :-("));
    sleep(10);
    exit_ncube();
    exit(99);
    }

#if D_MESS
    if (m_merge_strat)
      m_super_merging();
    else
      m_merging();
    if (!m_merge_strat)
      m_remote_copy();
#endif /* D_MESS */

  exit_ncube();
  DBUG_PRINT("HOST", ("finished !"));
  exit(0);
}

void getkey(void){}
void clearscreen(void){}
