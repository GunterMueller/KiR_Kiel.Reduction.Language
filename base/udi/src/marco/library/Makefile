# Makefile for marc library

SRCPATH	= $(PWD)
OBJPATH	= obj

# For internal debug feature uncomment next line
#UDIDEBUGS	= -DINTERNAL_UDI_DEBUG=1
UDIDEFINES	= $(UDIDEBUGS) -DUSE_EDITINFO=1


HERE_PATH       = ../../../..

# Reductionunit definitions
RED_OBJ         = /home/batman/base/lib/lred.o
RED_INC         = /home/batman/base/red/src

# Fred Fish debug library (To enable dbug features comment next line out)
FF_DBUG           = -DDBUG_OFF=1
FF_DBUG_PATH      = $(HERE_PATH)/src/dbug
#FF_DBUG_LIB      = $(FF_DBUG_PATH)/dbug.o

SUBMAKEFILEMAC  = "FF_DBUG=$(FF_DBUG)" \
                  "FF_DBUG_PATH=$(FF_DBUG_PATH)" \
                  "FF_DBUG_LIB=$(FF_DBUG_LIB)" \
                  "REDCOMP_PATH=$(HERE_PATH)" \
                  "RED_INC=$(RED_INC)" \
                  "RED_OBJ=$(RED_OBJ)" \
		  "UDIDEFINES=$(UDIDEFINES)"

# Make goals

all	::
	@(cd $(OBJPATH);make all  $(SUBMAKEFILEMAC) $(SUBSETTINGS);cd $(SRCPATH))

clean	::
	@(cd $(OBJPATH);make clean  $(SUBMAKEFILEMAC) $(SUBSETTINGS);cd $(SRCPATH));

install : all
	@(cd $(OBJPATH);make install  $(SUBMAKEFILEMAC) $(SUBSETTINGS);cd $(SRCPATH))

depend	::
	@(cd $(OBJPATH);make depend  $(SUBMAKEFILEMAC) $(SUBSETTINGS);cd $(SRCPATH))
