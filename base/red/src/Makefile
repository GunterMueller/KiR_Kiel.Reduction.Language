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
MAKEDEPEND = gcc -M >>Makefile
TAR = tar cvf
COMPRESS = gzip
BINDIR = bin/
LIBDIR = lib/
CFLAGS = -ansi -Wall -D_ANSI_ -D_BSD_SOURCE -Dsparc -DUNIX=1 -DODDSEX=0
LIBS = -lm
SRCDIR = red/src/
OBJDIR = ./
EDO = ../../fed/editor.o.obj/editor.o
IE = /home/sbs/Clib/xtermio.o
DBH = /home/sbs/c/redsys/ncred/src/DBH.o
HEADERFILES = red/src/rextern.h red/src/rmeasure.h red/src/rinstr.h \
              red/src/rops.h red/src/rruntime.h red/src/cparm.h \
              red/src/rmodulnr.h red/src/dumpheap.h red/src/rinter.h \
              red/src/rprimf.h red/src/rstdinc.h red/src/traps.h \
              red/src/rdigit.h red/src/rlmvt.h red/src/rscavenge.h \
              red/src/rstelem.h red/src/rheapty.h red/src/rnf.h \
              red/src/rstackty.h red/src/rstack.h red/src/tags.h \
              red/src/DBHexport.h red/src/DBMexport.h red/src/DBdebug.h \
              red/src/GLobal.h red/src/case.h red/src/d_mess_groups.h \
              red/src/d_mess_io.h red/src/d_mess_merge.h \
              red/src/d_mess_names.h red/src/dbug.h red/src/list.h \
              red/src/pminstr.h red/src/rncheap.h red/src/rncmessage.h \
              red/src/rncsched.h red/src/rncstack.h red/src/rops.h

all: dlred.o dtred.o dlred tred tred.o dbug.o dtred lred lred.o

install: dlred.o dtred.o dlred tred tred.o dbug.o dtred lred lred.o
	($(CD) red/dlred.o.obj; $(MAKE) install)
	($(CD) red/dtred.o.obj; $(MAKE) install)
	($(CD) red/dlred.obj; $(MAKE) install)
	($(CD) red/tred.obj; $(MAKE) install)
	($(CD) red/tred.o.obj; $(MAKE) install)
	($(CD) red/dbug.o.obj; $(MAKE) install)
	($(CD) red/dtred.obj; $(MAKE) install)
	($(CD) red/lred.obj; $(MAKE) install)
	($(CD) red/lred.o.obj; $(MAKE) install)

dlred.o:
	($(CD) red/dlred.o.obj; $(MAKE) dlred.o)
dtred.o:
	($(CD) red/dtred.o.obj; $(MAKE) dtred.o)
dlred: dlred.o dbug.o
	($(CD) red/dlred.obj; $(MAKE) dlred)
tred: tred.o
	($(CD) red/tred.obj; $(MAKE) tred)
tred.o:
	($(CD) red/tred.o.obj; $(MAKE) tred.o)
dbug.o:
	($(CD) red/dbug.o.obj; $(MAKE) dbug.o)
dtred: dtred.o dbug.o
	($(CD) red/dtred.obj; $(MAKE) dtred)
lred: lred.o
	($(CD) red/lred.obj; $(MAKE) lred)
lred.o:
	($(CD) red/lred.o.obj; $(MAKE) lred.o)

install-dlred.o: dlred.o
	($(CD) red/dlred.o.obj; $(MAKE) install)
install-dtred.o: dtred.o
	($(CD) red/dtred.o.obj; $(MAKE) install)
install-dlred: dlred
	($(CD) red/dlred.obj; $(MAKE) install)
install-tred: tred
	($(CD) red/tred.obj; $(MAKE) install)
install-tred.o: tred.o
	($(CD) red/tred.o.obj; $(MAKE) install)
install-dbug.o: dbug.o
	($(CD) red/dbug.o.obj; $(MAKE) install)
install-dtred: dtred
	($(CD) red/dtred.obj; $(MAKE) install)
install-lred: lred
	($(CD) red/lred.obj; $(MAKE) install)
install-lred.o: lred.o
	($(CD) red/lred.o.obj; $(MAKE) install)

depend:
	($(CD) red/dlred.o.obj; $(MAKE) depend)
	($(CD) red/dtred.o.obj; $(MAKE) depend)
	($(CD) red/dlred.obj; $(MAKE) depend)
	($(CD) red/tred.obj; $(MAKE) depend)
	($(CD) red/tred.o.obj; $(MAKE) depend)
	($(CD) red/dbug.o.obj; $(MAKE) depend)
	($(CD) red/dtred.obj; $(MAKE) depend)
	($(CD) red/lred.obj; $(MAKE) depend)
	($(CD) red/lred.o.obj; $(MAKE) depend)

clean:
	($(CD) red/dlred.o.obj; $(MAKE) clean)
	($(CD) red/dtred.o.obj; $(MAKE) clean)
	($(CD) red/dlred.obj; $(MAKE) clean)
	($(CD) red/tred.obj; $(MAKE) clean)
	($(CD) red/tred.o.obj; $(MAKE) clean)
	($(CD) red/dbug.o.obj; $(MAKE) clean)
	($(CD) red/dtred.obj; $(MAKE) clean)
	($(CD) red/lred.obj; $(MAKE) clean)
	($(CD) red/lred.o.obj; $(MAKE) clean)

distclean:
	$(RM) red/dlred.o.obj
	$(RM) red/dtred.o.obj
	$(RM) red/dlred.obj
	$(RM) red/tred.obj
	$(RM) red/tred.o.obj
	$(RM) red/dbug.o.obj
	$(RM) red/dtred.obj
	$(RM) red/lred.obj
	$(RM) red/lred.o.obj
	$(RM) Makefile

tar:
	($(CD) ..; $(TAR) red.tar src)
	$(COMPRESS) red.tar

tar-dlred.o:
	($(CD) red/dlred.o.obj/; $(MAKE) tar)
tar-dtred.o:
	($(CD) red/dtred.o.obj/; $(MAKE) tar)
tar-dlred:
	($(CD) red/dlred.obj/; $(MAKE) tar)
tar-tred:
	($(CD) red/tred.obj/; $(MAKE) tar)
tar-tred.o:
	($(CD) red/tred.o.obj/; $(MAKE) tar)
tar-dbug.o:
	($(CD) red/dbug.o.obj/; $(MAKE) tar)
tar-dtred:
	($(CD) red/dtred.obj/; $(MAKE) tar)
tar-lred:
	($(CD) red/lred.obj/; $(MAKE) tar)
tar-lred.o:
	($(CD) red/lred.o.obj/; $(MAKE) tar)
