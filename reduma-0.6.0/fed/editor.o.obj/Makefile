# This file is part of the reduma package. Copyright (C)
# 1988, 1989, 1992, 1993  University of Kiel. You may copy,
# distribute, and modify it freely as long as you preserve this copyright
# and permission notice.

# This Makefile is automatically generated
# DO NOT EDIT BELOW !

LD = ld -r
RM = rm -rf
CD = cd
LN = ln
MAKE = make
STRIP = strip
INSTALL = install -m 755
MAKEDEPEND = gcc -M -I/usr/ncube/current/usr/include >>Makefile
TAR = tar cvf
COMPRESS = gzip
BINDIR = ../../bin/
LIBDIR = ../../lib/
LIBS = -lm
SRCDIR = ../../fed/src/
OBJDIR = ./
EDO = /home/batman/base/lib/editor.o
IE = /home/cheops/sbs/Clib/xtermio.o
DBH = /home/cheops/sbs/c/redsys/ncred/src/DBH.o
HEADERFILES = fed/src/rextern.h fed/src/rmeasure.h fed/src/rinstr.h \
              fed/src/rops.h fed/src/rruntime.h fed/src/cparm.h \
              fed/src/rmodulnr.h fed/src/dumpheap.h fed/src/rinter.h \
              fed/src/rprimf.h fed/src/rstdinc.h fed/src/traps.h \
              fed/src/rdigit.h fed/src/rlmvt.h fed/src/rscavenge.h \
              fed/src/rstelem.h fed/src/rheapty.h fed/src/rnf.h \
              fed/src/rstackty.h fed/src/tags.h fed/src/DBHexport.h \
              fed/src/DBMexport.h fed/src/DBdebug.h fed/src/GLobal.h \
              fed/src/case.h fed/src/d_mess_groups.h \
              fed/src/d_mess_io.h fed/src/d_mess_merge.h \
              fed/src/d_mess_names.h fed/src/dbug.h fed/src/list.h \
              fed/src/pminstr.h fed/src/rncheap.h fed/src/rncmessage.h \
              fed/src/rncsched.h fed/src/rncstack.h fed/src/rops.h
SRCFILES = fed/src/cmain.c fed/src/cedit.c fed/src/cglobals.c \
           fed/src/ciotab.c fed/src/cetab.c fed/src/cinput.c \
           fed/src/cfiles.c fed/src/cgetexp.c fed/src/cgetstr.c \
           fed/src/cmatrix.c fed/src/cmatch.c fed/src/cscanexp.c \
           fed/src/cscroll.c fed/src/cnewscr.c fed/src/ctermio.c \
           fed/src/csearch.c fed/src/cdispl.c fed/src/cfdispl.c \
           fed/src/cscred.c fed/src/cstacks.c fed/src/cpexp.c \
           fed/src/ccommand.c fed/src/chelp.c fed/src/csubst.c \
           fed/src/credclr.c fed/src/ctransl1.c fed/src/initredp.c
DEPENDFILES = $(SRCDIR)cmain.c $(SRCDIR)cedit.c $(SRCDIR)cglobals.c \
              $(SRCDIR)ciotab.c $(SRCDIR)cetab.c $(SRCDIR)cinput.c \
              $(SRCDIR)cfiles.c $(SRCDIR)cgetexp.c $(SRCDIR)cgetstr.c \
              $(SRCDIR)cmatrix.c $(SRCDIR)cmatch.c $(SRCDIR)cscanexp.c \
              $(SRCDIR)cscroll.c $(SRCDIR)cnewscr.c $(SRCDIR)ctermio.c \
              $(SRCDIR)csearch.c $(SRCDIR)cdispl.c $(SRCDIR)cfdispl.c \
              $(SRCDIR)cscred.c $(SRCDIR)cstacks.c $(SRCDIR)cpexp.c \
              $(SRCDIR)ccommand.c $(SRCDIR)chelp.c $(SRCDIR)csubst.c \
              $(SRCDIR)credclr.c $(SRCDIR)ctransl1.c $(SRCDIR)initredp.c
OTHERFILES = configure fed.cnf README INSTALL
CC = gcc
CFLAGS = -DTERMIOS=1 -DSETBUFFER -DX_UDI=0 -DUNIX -DKEDIT -DM_PATT \
         -DTRACE -DClausR -DB_DEBUG=0 $(EDFLAGS)
FILES = cmain.o cedit.o cglobals.o ciotab.o cetab.o cinput.o cfiles.o \
        cgetexp.o cgetstr.o cmatrix.o cmatch.o cscanexp.o cscroll.o \
        cnewscr.o ctermio.o csearch.o cdispl.o cfdispl.o cscred.o \
        cstacks.o cpexp.o ccommand.o chelp.o csubst.o credclr.o \
        ctransl1.o initredp.o

editor.o: $(FILES)
	$(LD) $(LDFLAGS) -o editor.o $(FILES) $(LIBS)

install: editor.o
	if [ -d .. ] ; then true ; else mkdir .. ; fi
	if [ -d ../.. ] ; then true ; else mkdir ../.. ; fi
	if [ -d ../../lib ] ; then true ; else mkdir ../../lib ; fi
	$(INSTALL) editor.o $(LIBDIR)

depend:
	$(MAKEDEPEND) $(CFLAGS) $(DEPENDFILES)

clean:
	$(RM) $(FILES)
	$(RM) editor.o

tar:
	($(CD) /tmp/reduma/reduma; $(TAR) editor.o.tar $(OTHERFILES) $(SRCFILES) $(HEADERFILES))
	($(CD) /tmp/reduma/reduma; $(COMPRESS) editor.o.tar)

cmain.o: $(SRCDIR)cmain.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cmain.o $(SRCDIR)cmain.c
cedit.o: $(SRCDIR)cedit.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cedit.o $(SRCDIR)cedit.c
cglobals.o: $(SRCDIR)cglobals.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cglobals.o $(SRCDIR)cglobals.c
ciotab.o: $(SRCDIR)ciotab.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ciotab.o $(SRCDIR)ciotab.c
cetab.o: $(SRCDIR)cetab.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cetab.o $(SRCDIR)cetab.c
cinput.o: $(SRCDIR)cinput.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cinput.o $(SRCDIR)cinput.c
cfiles.o: $(SRCDIR)cfiles.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cfiles.o $(SRCDIR)cfiles.c
cgetexp.o: $(SRCDIR)cgetexp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cgetexp.o $(SRCDIR)cgetexp.c
cgetstr.o: $(SRCDIR)cgetstr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cgetstr.o $(SRCDIR)cgetstr.c
cmatrix.o: $(SRCDIR)cmatrix.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cmatrix.o $(SRCDIR)cmatrix.c
cmatch.o: $(SRCDIR)cmatch.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cmatch.o $(SRCDIR)cmatch.c
cscanexp.o: $(SRCDIR)cscanexp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cscanexp.o $(SRCDIR)cscanexp.c
cscroll.o: $(SRCDIR)cscroll.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cscroll.o $(SRCDIR)cscroll.c
cnewscr.o: $(SRCDIR)cnewscr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cnewscr.o $(SRCDIR)cnewscr.c
ctermio.o: $(SRCDIR)ctermio.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ctermio.o $(SRCDIR)ctermio.c
csearch.o: $(SRCDIR)csearch.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o csearch.o $(SRCDIR)csearch.c
cdispl.o: $(SRCDIR)cdispl.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cdispl.o $(SRCDIR)cdispl.c
cfdispl.o: $(SRCDIR)cfdispl.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cfdispl.o $(SRCDIR)cfdispl.c
cscred.o: $(SRCDIR)cscred.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cscred.o $(SRCDIR)cscred.c
cstacks.o: $(SRCDIR)cstacks.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cstacks.o $(SRCDIR)cstacks.c
cpexp.o: $(SRCDIR)cpexp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o cpexp.o $(SRCDIR)cpexp.c
ccommand.o: $(SRCDIR)ccommand.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ccommand.o $(SRCDIR)ccommand.c
chelp.o: $(SRCDIR)chelp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o chelp.o $(SRCDIR)chelp.c
csubst.o: $(SRCDIR)csubst.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o csubst.o $(SRCDIR)csubst.c
credclr.o: $(SRCDIR)credclr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o credclr.o $(SRCDIR)credclr.c
ctransl1.o: $(SRCDIR)ctransl1.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ctransl1.o $(SRCDIR)ctransl1.c
initredp.o: $(SRCDIR)initredp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o initredp.o $(SRCDIR)initredp.c
