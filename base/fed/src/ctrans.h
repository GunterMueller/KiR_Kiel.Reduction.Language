/* algorithmen fuer stack-operationen   trans.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:Mi  31.12.86        16:45:03        */

/* makros fuer die arbeit mit chain     */
#define tr_lett(e,b,el,el1)       if (chained(el)) chmove(e,b); \
       if (marked1(el)) {                       \
         push(b,mark0(el));                     \
         while(marked1(el1 = pop(e))) {         \
           if (chained(el1)) chmove(e,b);       \
           push(b,el1);                         \
         }                                      \
         if (chained(el1)) chmove(e,b);         \
         push(b,mark1(el1));                    \
        }                                       \
       else push(b,el)
/* vorsicht mit ; am ende !     */
#define tr_at(e,b,el)   if (single_atom(el) || marked0(el)) {   \
          if (chained(el)) chmove(e,b);         \
          push(b,el);                           \
         }                                      \
        else {                                  \
          if (chained(el)) chmove(e,b);         \
          push(b,mark0(el));                    \
          while (marked1(el = pop(e))) {        \
            if (chained(el)) chmove(e,b);       \
            push(b,el);                         \
          }                                     \
          if (chained(el)) chmove(e,b);         \
          push(b,mark1(el));                    \
        }
/* vorsicht mit ; am ende !     */
#define tr_m(m,b,el)         { el = pop(m);     \
         if (chained(el)) chmove(m,b);          \
         push(b,el);                            \
        }
/* vorsicht mit ; am ende !     */
/* Dies mit sehr viel vorsicht geniessen !!     */
#define FR1_FD         count = 0;               \
         for ( i = arity(el) ; i > 1 ; i-- ) {  \
            chtransp(e,b);                      \
            if (chcomp(b,VST)) {                \
              bc++;                             \
              count++;                          \
            }                                   \
         }                                      \
         push(MST,noedit(el) | toedit(count));  \
         push(M1ST,el);
#define FR2_FD          bc -= (int)edit(el);    \
          el = pop(M1ST);                       \
          if (chained(el)) chmove(M1ST,b);      \
          push(b,el);
         
/* end of       trans.h */

