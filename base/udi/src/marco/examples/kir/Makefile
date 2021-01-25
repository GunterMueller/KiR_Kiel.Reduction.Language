###################################################################################
##
## Example Makefile for Marco (Make a reduktion compiler)
## Adjust it for you purposes
##
###################################################################################
## Author : Ulrich Diedrichsen
## Copy   : University of Kiel (germany) 1993
###################################################################################

HERE_PATH       = ../../../..

# Reductionunit definitions
RED_OBJ         = /home/batman/base/lib/lred.o
RED_INC         = /home/batman/base/red/src

# Fred Fish debug library (To enable dbug features set DBUG_OFF to 0)
FF_DBUG           = -DDBUG_OFF=1
FF_DBUG_PATH      = $(HERE_PATH)/src/dbug
#FF_DBUG_LIB      = $(FF_DBUG_PATH)/dbug.o

SUBMAKEFILEMAC  = "FF_DBUG=$(FF_DBUG)" \
                  "FF_DBUG_PATH=$(FF_DBUG_PATH)" \
                  "FF_DBUG_LIB=$(FF_DBUG_LIB)" \
                  "REDCOMP_PATH=$(HERE_PATH)" \
                  "RED_INC=$(RED_INC)" \
                  "RED_OBJ=$(RED_OBJ)"

###################################################################################
#
# Some definition needed for makefile (Please adjust them)
#
###################################################################################

# Destination directory for generated compiler
BINPATH		= $(HERE_PATH)/bin

# Root dir for directory marc source
MARCSRC_PATH	= $(HERE_PATH)/src/marco

# Local or system lib directory for marc libraries
MARCLIB_PATH	= $(HERE_PATH)/lib

# Name of this languages
LANGUAGENAME	= kir
LANGUAGESRC	= kir.lang

# Compiler generator options
COGEN		= $(BINPATH)/marco
COGEN-FILENAME	= -f
COGEN-INC	= -I$(MARCSRC_PATH)/include

# Compiler generator libraries
COGEN-LIB	= $(MARCLIB_PATH)/libmarco.o
COGEN-MAIN	= $(MARCLIB_PATH)/marco-main.o

# Additional libraries need (actually mathlib)
ADD_LIBS	= -lm -ll

###################################################################################
#
# Some definition needed for generated sub makefile (Please adjust them)
#
###################################################################################

LIBMKFILE	= $(LANGUAGENAME).mk

# Name of language depend library
LANGLIBNAME	= lib$(LANGUAGENAME).o

# Programm to delete files with options
RM		= rm
RMOPTS		= -f

# Move (rename) programm with options
MV		= mv
MVOPTS		=

# LD linker programm and options
LD		= ld
LDOPTS		= -r -o

# C Compiler with options (Last option must be to specify the output file)
CC		= gcc
CFLAGS		= -O2 $(FF_DBUG) $(COGEN-INC) -I$(FF_DBUG_PATH)

# Bison (yacc) compiler compiler with options (to generate a header file)
BISON		= bison
BISONOPTS	= -d

# Scannergenerator with options (lex or flex)
FLEX		= flex
FLEXOPTS	=

###################################################################################
#
# End of definitions for submakefile and user (system) depend part
#
###################################################################################

SUBMAKEFILEMAC	= "LANGLIBNAME=$(LANGLIBNAME)" \
		  "RM=$(RM)" "RMOPTS=$(RMOPTS)" \
		  "MV=$(MV)" "MVOPTS=$(MVOPTS)" \
		  "LD=$(LD)" "LDOPTS=$(LDOPTS)" \
		  "FFDBUGOPTS=$(FF_DBUG)" \
		  "CC=$(CC)" "CFLAGS=$(CFLAGS)" \
		  "BISON=$(BISON)" "BISONOPTS=$(BISONOPTS)" \
		  "FLEX=$(FLEX)" "FLEXOPTS=$(FLEXOPTS)"

###################################################################################
# Goals
###################################################################################

all : $(LANGUAGENAME) $(COGEN-LIB) $(COGEN-MAIN) $(RED_OBJ) $(FFDBUGLIB)

$(LANGUAGENAME) : $(LANGLIBNAME) $(COGEN-LIB) $(COGEN-MAIN)
	@($(CC) -o $(LANGUAGENAME) $(LANGLIBNAME) $(COGEN-LIB) $(COGEN-MAIN) $(RED_OBJ) $(ADD_LIBS) $(FF_DBUG_LIB))

clean ::
	@(make clean_langlib;$(RM) $(RMOPTS) $(LANGUAGENAME))

clobber: clean
	@(rm -f $(BINPATH)/$(LANGUAGENAME) $(MARCLIB_PATH)/$(LANGLIBNAME))

depend::

install:: all
	cp $(LANGUAGENAME) $(BINPATH);
	cp $(LANGLIBNAME) $(MARCLIB_PATH)

###################################################################################
# Subgoals
###################################################################################

$(LANGLIBNAME) : $(LIBMKFILE) $(LANGUAGESRC)
	@(make all_langlib)

###################################################################################
# Subgoals for language depend parts (DO NOT EDIT)
###################################################################################

$(LIBMKFILE) : $(LANGUAGESRC)
	$(COGEN) $(COGEN-FILENAME) $(LANGUAGESRC)

all_langlib : $(LIBMKFILE) $(LIBNAME)
	@(make all -f $(LIBMKFILE) $(SUBMAKEFILEMAC))

clean_langlib : $(LIBMKFILE) $(LIBNAME)
	@(make clean -f $(LIBMKFILE) $(SUBMAKEFILEMAC))

clobber_langlib : $(LIBMKFILE) $(LIBNAME)
	@(make clobber -f $(LIBMKFILE) $(SUBMAKEFILEMAC))

install_langlib : $(LIBMKFILE) $(LIBNAME)
	@(make install -f $(LIBMKFILE) $(SUBMAKEFILEMAC))
