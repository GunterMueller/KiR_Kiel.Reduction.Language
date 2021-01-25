CC = gcc
OPTS = -g -pipe -DDBUG_OFF -Wall

BINDIR = ../bin

INCLUDE_PATHS = -I/usr/X11R5/include 
INCLUDES =
INCS = $(INCLUDE_PATHS) $(INCLUDES)

SPECIALS = -DINCLUDE_ALLOCA_H -DXAW -DUKC_WIDGETS -DUNALIGNED -target sun4

LIBRARY_PATHS = -L/usr/X11R5/lib 
LIBRARIES = -lXukc -lWc -lAt -lXaw -lXmu -lXt -lXext\
            -lX11 -lm -ldbm -lm -lposix -ldl
LIBS = $(LIBRARY_PATHS) $(LIBRARIES)


MSMAIN_SRCS = msMain.c msMain_create.c msMain_main.c msFileBrowser.c\
              msRequester.c msTools.c msMacros.h dbug.c
MSMAIN_OBJS = msMain.o msMain_create.o msMain_main.o msFileBrowser.o\
              msRequester.o msTools.o dbug.o

MSBARCHART_SRCS = msBarChart.c msBarChart_create.c msBarChart_main.c\
                  msFileBrowser.c msRequester.c msTools.c msModule.c\
                  msMacros.h dbug.c
MSBARCHART_OBJS = msBarChart.o msBarChart_create.o msBarChart_main.o\
                  msFileBrowser.o msRequester.o msTools.o dbug.o

MSPROCARRAY_SRCS = msProcArray.c msProcArray_create.c msProcArray_main.c\
                   msFileBrowser.c msRequester.c msTools.c msModule.c\
                   msMacros.h dbug.c
MSPROCARRAY_OBJS = msProcArray.o msProcArray_create.o msProcArray_main.o\
                   msFileBrowser.o msRequester.o msTools.o dbug.o

MSHEAPARRAY_SRCS = msHeapArray.c msHeapArray_create.c msHeapArray_main.c\
                   msFileBrowser.c msRequester.c msTools.c msModule.c\
                   msMacros.h dbug.c
MSHEAPARRAY_OBJS = msHeapArray.o msHeapArray_create.o msHeapArray_main.o\
                   msFileBrowser.o msRequester.o msTools.o dbug.o

MSPROCTREE_SRCS = msProcTree.c msProcTree_create.c msProcTree_main.c\
                  msFileBrowser.c msRequester.c msTools.c msModule.c\
                  msMacros.h dbug.c
MSPROCTREE_OBJS = msProcTree.o msProcTree_create.o msProcTree_main.o\
                  msFileBrowser.o msRequester.o msTools.o dbug.o

all: msMain msBarChart msProcArray msHeapArray msProcTree bin2ascii

bin2ascii: bin2ascii.o
	$(CC) -o ${BINDIR}/bin2ascii dbug.o bin2ascii.o

bin2ascii.o: bin2ascii.c dbug.c
	$(CC) $(OPTS) -c bin2ascii.c

msMain: $(MSMAIN_OBJS)
	$(RM) $@
	$(CC) -o ${BINDIR}/$@ $(MSMAIN_OBJS) $(LIBS)

msBarChart: $(MSBARCHART_OBJS)
	$(RM) $@
	$(CC) -o ${BINDIR}/$@ $(MSBARCHART_OBJS) $(LIBS) 

msProcArray: $(MSPROCARRAY_OBJS)
	$(RM) $@
	$(CC) -o ${BINDIR}/$@ $(MSPROCARRAY_OBJS) $(LIBS) 

msHeapArray: $(MSHEAPARRAY_OBJS)
	$(RM) $@
	$(CC) -o ${BINDIR}/$@ $(MSHEAPARRAY_OBJS) $(LIBS) 

msProcTree: $(MSPROCTREE_OBJS)
	$(RM) $@
	$(CC) -o ${BINDIR}/$@ $(MSPROCTREE_OBJS) $(LIBS) 

.c.o:
	$(CC) $(OPTS) $(INCS) $(SPECIALS) -c $*.c

clean:
	$(RM) *.o *~

cleansrc: clean

cleanscripts:
	find ${BINDIR}/BarChartSCRs \( -name \*.[co] -o -name \*.so -o -name \*~ \)\
             -exec rm {} \;
	find ${BINDIR}/ProcArraySCRs \( -name \*.[co] -o -name \*.so -o -name \*~ \)\
             -exec rm {} \;
	find ${BINDIR}/HeapArraySCRs \( -name \*.[co] -o -name \*.so -o -name \*~ \)\
             -exec rm {} \;
	find ${BINDIR}/ProcTreeSCRs \( -name \*.[co] -o -name \*.so -o -name \*~ \)\
             -exec rm {} \;

cleanbin:
	${RM} ${BINDIR}/msMain\
	      ${BINDIR}/msBarChart\
	      ${BINDIR}/msProcArray\
	      ${BINDIR}/msHeapArray\
	      ${BINDIR}/msProcTree \
	      ${BINDIR}/bin2ascii

cleanall: cleansrc cleanscripts cleanbin
