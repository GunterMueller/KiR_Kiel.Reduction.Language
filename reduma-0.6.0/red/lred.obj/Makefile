# This file is part of the reduma package. Copyright (C)
# 1988, 1989, 1992, 1993  University of Kiel. You may copy,
# distribute, and modify it freely as long as you preserve this copyright
# and permission notice.

# This Makefile is automatically generated
# DO NOT EDIT BELOW !

LD = ld -r
RM = rm -rf
CD = cd
CC = gcc 
LN = ln
MAKE = make
STRIP = strip
INSTALL = install -m 755
MAKEDEPEND = gcc -M -I/usr/ncube/current/usr/include >>Makefile
TAR = tar cvf
COMPRESS = gzip
BINDIR = ../../bin/
LIBDIR = ../../lib/
SRCDIR = ../../red/src/
OBJDIR = ./
EDO = /home/batman/base/lib/editor.o
IE = /home/cheops/sbs/Clib/xtermio.o
DBH = /home/cheops/sbs/c/redsys/ncred/src/DBH.o
HEADERFILES = red/src/rextern.h red/src/rmeasure.h red/src/rinstr.h \
              red/src/rops.h red/src/rruntime.h red/src/cparm.h \
              red/src/rmodulnr.h red/src/dumpheap.h red/src/rinter.h \
              red/src/rprimf.h red/src/rstdinc.h red/src/traps.h \
              red/src/rdigit.h red/src/rlmvt.h red/src/rscavenge.h \
              red/src/rstelem.h red/src/rheapty.h red/src/rnf.h \
              red/src/rstackty.h red/src/tags.h red/src/DBHexport.h \
              red/src/DBMexport.h red/src/DBdebug.h red/src/GLobal.h \
              red/src/case.h red/src/d_mess_groups.h \
              red/src/d_mess_io.h red/src/d_mess_merge.h \
              red/src/d_mess_names.h red/src/dbug.h red/src/list.h \
              red/src/pminstr.h red/src/rncheap.h red/src/rncmessage.h \
              red/src/rncsched.h red/src/rncstack.h red/src/rops.h
CFLAGS = -ansi -Wall -D_ANSI_ -Dsparc -DUNIX=1 -DODDSEX=0 -O \
         -DPI_RED_PLUS=1 -DRED_TO_NF=1 -DDEBUG=0 -DDBUG_OFF \
         -DSCAVENGE=0 -DSCAV_DEBUG=0
SRCFILES =
DEPENDFILES =
OTHERFILES = configure red.cnf README INSTALL
FILES = ../../red/lred.o.obj/lred.o ../../fed/editor.o.obj/editor.o
LIBS = -lm -ltermcap

lred: $(FILES)
	$(CC) $(LDFLAGS) -o lred $(FILES) $(LIBS)

install: lred
	if [ -d .. ] ; then true ; else mkdir .. ; fi
	if [ -d ../.. ] ; then true ; else mkdir ../.. ; fi
	if [ -d ../../bin ] ; then true ; else mkdir ../../bin ; fi
	$(INSTALL) lred $(BINDIR)
	$(STRIP) $(BINDIR)lred

depend:
	$(MAKEDEPEND) $(CFLAGS) $(DEPENDFILES)

clean:
	$(RM) $(FILES)
	$(RM) lred

tar:
	($(CD) /tmp/reduma/reduma; $(TAR) lred.tar $(OTHERFILES) $(SRCFILES) $(HEADERFILES))
	($(CD) /tmp/reduma/reduma; $(COMPRESS) lred.tar)

