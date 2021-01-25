# Makefile for marco compiler generator

SRCPATH	= $(PWD)
OBJPATH	= obj

HERE_PATH       = ../../../..

# Fred Fish debug library (To enable dbug features comment next line out)
FF_DBUG           = -DDBUG_OFF=1
FF_DBUG_PATH      = $(HERE_PATH)/src/dbug
#FF_DBUG_LIB      = $(FF_DBUG_PATH)/dbug.o

SUBMAKEFILEMAC  = "FF_DBUG=$(FF_DBUG)" \
                  "FF_DBUG_PATH=$(FF_DBUG_PATH)" \
                  "FF_DBUG_LIB=$(FF_DBUG_LIB)"

# Make goals

all	::
	@(cd $(OBJPATH);make all $(SUBMAKEFILEMAC);cd $(SRCPATH))

clean	::
	@(cd $(OBJPATH);make clean $(SUBMAKEFILEMAC) ;cd $(SRCPATH));

install : all
	@(cd $(OBJPATH);make install $(SUBMAKEFILEMAC) ;cd $(SRCPATH))

depend	::
	@(cd $(OBJPATH);make depend $(SUBMAKEFILEMAC) ;cd $(SRCPATH))
