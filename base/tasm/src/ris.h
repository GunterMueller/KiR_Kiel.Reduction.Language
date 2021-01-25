#ifndef RIS_H
#define RIS_H

typedef enum { ST_A = 1, ST_R, ST_T, ST_W } stackid;

#define freeframe(s, c) if (refmode != SREFisexplicit)                          \
                          { int i;                                              \
                            for(i = 0; i < (int)top(s); i++)                    \
                            mkinc(c, (desc *)peek(s, i+1), ST_ ## s, i); }      \
                        drop(s, (int)top(s)+1);

typedef enum
  { null = 0, FSYNC, FSYNCMERGE, SYNC, SYNCMERGE, NOOP, CSYNC, CSYNCMERGE } doneid;

#endif /* RIS_H */
