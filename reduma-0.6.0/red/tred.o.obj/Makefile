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
LIBS = -lm
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
SRCFILES = red/src/ldel-expr.c red/src/rdig-mul.c red/src/rdig-sub.c \
           red/src/rdig-v1.c red/src/rtrav.c red/src/rdig-v2.c \
           red/src/rval-int.c red/src/lpa-sub.c red/src/rhinout.c \
           red/src/rred-eq.c red/src/lr-args.c red/src/rlog-mvt.c \
           red/src/rdig-add.c red/src/rstpre.c red/src/rdig-div.c \
           red/src/rstpro.c red/src/lr-prot.c red/src/rdig-mod.c \
           red/src/rsts-dec.c red/src/dumpheap.c red/src/rlogfunc.c \
           red/src/rreduct.c red/src/rstate.c red/src/rlstruct.c \
           red/src/rfstruct.c red/src/rret-dec.c red/src/rvalfunc.c \
           red/src/rvalir.c red/src/reac.c red/src/rnf.c \
           red/src/rruntime.c red/src/rconvert.c red/src/rear.c \
           red/src/setup.c red/src/rconvir.c red/src/rheap.c \
           red/src/rquery.c red/src/rst-dec.c red/src/rdesc.c \
           red/src/rreduce.c red/src/rstack.c red/src/rbibfunc.c \
           red/src/rmstruct.c red/src/rvstruct.c red/src/rinter.c \
           red/src/rdummies.c red/src/rextcode.c red/src/rprint.c \
           red/src/rtasm.c red/src/pre-casefunc.c red/src/pre-hfunc.c \
           red/src/pre-lrecfunc.c red/src/pre-nea-cre.c \
           red/src/pre-nsubfunc.c red/src/pre-pm-ptfu.c \
           red/src/pre-pmpatvar.c red/src/pos-rea-ret.c \
           red/src/pos-postfunc.c red/src/pos-casepost.c \
           red/src/pos-p-fail.c red/src/pmdimli.c red/src/pmam.c \
           red/src/pmsupport.c red/src/pmanalyze.c
DEPENDFILES = $(SRCDIR)ldel-expr.c $(SRCDIR)rdig-mul.c \
              $(SRCDIR)rdig-sub.c $(SRCDIR)rdig-v1.c $(SRCDIR)rtrav.c \
              $(SRCDIR)rdig-v2.c $(SRCDIR)rval-int.c $(SRCDIR)lpa-sub.c \
              $(SRCDIR)rhinout.c $(SRCDIR)rred-eq.c $(SRCDIR)lr-args.c \
              $(SRCDIR)rlog-mvt.c $(SRCDIR)rdig-add.c $(SRCDIR)rstpre.c \
              $(SRCDIR)rdig-div.c $(SRCDIR)rstpro.c $(SRCDIR)lr-prot.c \
              $(SRCDIR)rdig-mod.c $(SRCDIR)rsts-dec.c \
              $(SRCDIR)dumpheap.c $(SRCDIR)rlogfunc.c \
              $(SRCDIR)rreduct.c $(SRCDIR)rstate.c $(SRCDIR)rlstruct.c \
              $(SRCDIR)rfstruct.c $(SRCDIR)rret-dec.c \
              $(SRCDIR)rvalfunc.c $(SRCDIR)rvalir.c $(SRCDIR)reac.c \
              $(SRCDIR)rnf.c $(SRCDIR)rruntime.c $(SRCDIR)rconvert.c \
              $(SRCDIR)rear.c $(SRCDIR)setup.c $(SRCDIR)rconvir.c \
              $(SRCDIR)rheap.c $(SRCDIR)rquery.c $(SRCDIR)rst-dec.c \
              $(SRCDIR)rdesc.c $(SRCDIR)rreduce.c $(SRCDIR)rstack.c \
              $(SRCDIR)rbibfunc.c $(SRCDIR)rmstruct.c \
              $(SRCDIR)rvstruct.c $(SRCDIR)rinter.c $(SRCDIR)rdummies.c \
              $(SRCDIR)rextcode.c $(SRCDIR)rprint.c $(SRCDIR)rtasm.c \
              $(SRCDIR)pre-casefunc.c $(SRCDIR)pre-hfunc.c \
              $(SRCDIR)pre-lrecfunc.c $(SRCDIR)pre-nea-cre.c \
              $(SRCDIR)pre-nsubfunc.c $(SRCDIR)pre-pm-ptfu.c \
              $(SRCDIR)pre-pmpatvar.c $(SRCDIR)pos-rea-ret.c \
              $(SRCDIR)pos-postfunc.c $(SRCDIR)pos-casepost.c \
              $(SRCDIR)pos-p-fail.c $(SRCDIR)pmdimli.c $(SRCDIR)pmam.c \
              $(SRCDIR)pmsupport.c $(SRCDIR)pmanalyze.c
OTHERFILES = configure red.cnf README INSTALL
FILES = ldel-expr.o rdig-mul.o rdig-sub.o rdig-v1.o rtrav.o rdig-v2.o \
        rval-int.o lpa-sub.o rhinout.o rred-eq.o lr-args.o rlog-mvt.o \
        rdig-add.o rstpre.o rdig-div.o rstpro.o lr-prot.o rdig-mod.o \
        rsts-dec.o dumpheap.o rlogfunc.o rreduct.o rstate.o rlstruct.o \
        rfstruct.o rret-dec.o rvalfunc.o rvalir.o reac.o rnf.o \
        rruntime.o rconvert.o rear.o setup.o rconvir.o rheap.o rquery.o \
        rst-dec.o rdesc.o rreduce.o rstack.o rbibfunc.o rmstruct.o \
        rvstruct.o rinter.o rdummies.o rextcode.o rprint.o rtasm.o \
        pre-casefunc.o pre-hfunc.o pre-lrecfunc.o pre-nea-cre.o \
        pre-nsubfunc.o pre-pm-ptfu.o pre-pmpatvar.o pos-rea-ret.o \
        pos-postfunc.o pos-casepost.o pos-p-fail.o pmdimli.o pmam.o \
        pmsupport.o pmanalyze.o
CFLAGS = -ansi -Wall -D_ANSI_ -Dsparc -DUNIX=1 -DODDSEX=0 -O \
         -DPI_RED_PLUS=1 -DRED_TO_NF=1 -DDEBUG=0 -DDBUG_OFF \
         -DSCAVENGE=0 -DSCAV_DEBUG=0 -DWITHTILDE=1 -DCLOSEGOAL=0 \
         -DUH_ZF_PM=1

tred.o: $(FILES)
	$(LD) $(LDFLAGS) -o tred.o $(FILES) $(LIBS)

install: tred.o
	if [ -d .. ] ; then true ; else mkdir .. ; fi
	if [ -d ../.. ] ; then true ; else mkdir ../.. ; fi
	if [ -d ../../lib ] ; then true ; else mkdir ../../lib ; fi
	$(INSTALL) tred.o $(LIBDIR)

depend:
	$(MAKEDEPEND) $(CFLAGS) $(DEPENDFILES)

clean:
	$(RM) $(FILES)
	$(RM) tred.o

tar:
	($(CD) /tmp/reduma/reduma; $(TAR) tred.o.tar $(OTHERFILES) $(SRCFILES) $(HEADERFILES))
	($(CD) /tmp/reduma/reduma; $(COMPRESS) tred.o.tar)

ldel-expr.o: $(SRCDIR)ldel-expr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ldel-expr.o $(SRCDIR)ldel-expr.c
rdig-mul.o: $(SRCDIR)rdig-mul.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-mul.o $(SRCDIR)rdig-mul.c
rdig-sub.o: $(SRCDIR)rdig-sub.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-sub.o $(SRCDIR)rdig-sub.c
rdig-v1.o: $(SRCDIR)rdig-v1.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-v1.o $(SRCDIR)rdig-v1.c
rtrav.o: $(SRCDIR)rtrav.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rtrav.o $(SRCDIR)rtrav.c
rdig-v2.o: $(SRCDIR)rdig-v2.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-v2.o $(SRCDIR)rdig-v2.c
rval-int.o: $(SRCDIR)rval-int.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rval-int.o $(SRCDIR)rval-int.c
lpa-sub.o: $(SRCDIR)lpa-sub.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lpa-sub.o $(SRCDIR)lpa-sub.c
rhinout.o: $(SRCDIR)rhinout.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rhinout.o $(SRCDIR)rhinout.c
rred-eq.o: $(SRCDIR)rred-eq.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rred-eq.o $(SRCDIR)rred-eq.c
lr-args.o: $(SRCDIR)lr-args.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-args.o $(SRCDIR)lr-args.c
rlog-mvt.o: $(SRCDIR)rlog-mvt.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rlog-mvt.o $(SRCDIR)rlog-mvt.c
rdig-add.o: $(SRCDIR)rdig-add.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-add.o $(SRCDIR)rdig-add.c
rstpre.o: $(SRCDIR)rstpre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstpre.o $(SRCDIR)rstpre.c
rdig-div.o: $(SRCDIR)rdig-div.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-div.o $(SRCDIR)rdig-div.c
rstpro.o: $(SRCDIR)rstpro.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstpro.o $(SRCDIR)rstpro.c
lr-prot.o: $(SRCDIR)lr-prot.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-prot.o $(SRCDIR)lr-prot.c
rdig-mod.o: $(SRCDIR)rdig-mod.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-mod.o $(SRCDIR)rdig-mod.c
rsts-dec.o: $(SRCDIR)rsts-dec.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rsts-dec.o $(SRCDIR)rsts-dec.c
dumpheap.o: $(SRCDIR)dumpheap.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o dumpheap.o $(SRCDIR)dumpheap.c
rlogfunc.o: $(SRCDIR)rlogfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rlogfunc.o $(SRCDIR)rlogfunc.c
rreduct.o: $(SRCDIR)rreduct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rreduct.o $(SRCDIR)rreduct.c
rstate.o: $(SRCDIR)rstate.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstate.o $(SRCDIR)rstate.c
rlstruct.o: $(SRCDIR)rlstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rlstruct.o $(SRCDIR)rlstruct.c
rfstruct.o: $(SRCDIR)rfstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rfstruct.o $(SRCDIR)rfstruct.c
rret-dec.o: $(SRCDIR)rret-dec.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rret-dec.o $(SRCDIR)rret-dec.c
rvalfunc.o: $(SRCDIR)rvalfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rvalfunc.o $(SRCDIR)rvalfunc.c
rvalir.o: $(SRCDIR)rvalir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rvalir.o $(SRCDIR)rvalir.c
reac.o: $(SRCDIR)reac.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o reac.o $(SRCDIR)reac.c
rnf.o: $(SRCDIR)rnf.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rnf.o $(SRCDIR)rnf.c
rruntime.o: $(SRCDIR)rruntime.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rruntime.o $(SRCDIR)rruntime.c
rconvert.o: $(SRCDIR)rconvert.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rconvert.o $(SRCDIR)rconvert.c
rear.o: $(SRCDIR)rear.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rear.o $(SRCDIR)rear.c
setup.o: $(SRCDIR)setup.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o setup.o $(SRCDIR)setup.c
rconvir.o: $(SRCDIR)rconvir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rconvir.o $(SRCDIR)rconvir.c
rheap.o: $(SRCDIR)rheap.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rheap.o $(SRCDIR)rheap.c
rquery.o: $(SRCDIR)rquery.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rquery.o $(SRCDIR)rquery.c
rst-dec.o: $(SRCDIR)rst-dec.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rst-dec.o $(SRCDIR)rst-dec.c
rdesc.o: $(SRCDIR)rdesc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdesc.o $(SRCDIR)rdesc.c
rreduce.o: $(SRCDIR)rreduce.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rreduce.o $(SRCDIR)rreduce.c
rstack.o: $(SRCDIR)rstack.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstack.o $(SRCDIR)rstack.c
rbibfunc.o: $(SRCDIR)rbibfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rbibfunc.o $(SRCDIR)rbibfunc.c
rmstruct.o: $(SRCDIR)rmstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rmstruct.o $(SRCDIR)rmstruct.c
rvstruct.o: $(SRCDIR)rvstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rvstruct.o $(SRCDIR)rvstruct.c
rinter.o: $(SRCDIR)rinter.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rinter.o $(SRCDIR)rinter.c
rdummies.o: $(SRCDIR)rdummies.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdummies.o $(SRCDIR)rdummies.c
rextcode.o: $(SRCDIR)rextcode.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rextcode.o $(SRCDIR)rextcode.c
rprint.o: $(SRCDIR)rprint.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rprint.o $(SRCDIR)rprint.c
rtasm.o: $(SRCDIR)rtasm.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rtasm.o $(SRCDIR)rtasm.c
pre-casefunc.o: $(SRCDIR)pre-casefunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-casefunc.o $(SRCDIR)pre-casefunc.c
pre-hfunc.o: $(SRCDIR)pre-hfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-hfunc.o $(SRCDIR)pre-hfunc.c
pre-lrecfunc.o: $(SRCDIR)pre-lrecfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-lrecfunc.o $(SRCDIR)pre-lrecfunc.c
pre-nea-cre.o: $(SRCDIR)pre-nea-cre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-nea-cre.o $(SRCDIR)pre-nea-cre.c
pre-nsubfunc.o: $(SRCDIR)pre-nsubfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-nsubfunc.o $(SRCDIR)pre-nsubfunc.c
pre-pm-ptfu.o: $(SRCDIR)pre-pm-ptfu.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-pm-ptfu.o $(SRCDIR)pre-pm-ptfu.c
pre-pmpatvar.o: $(SRCDIR)pre-pmpatvar.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pre-pmpatvar.o $(SRCDIR)pre-pmpatvar.c
pos-rea-ret.o: $(SRCDIR)pos-rea-ret.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pos-rea-ret.o $(SRCDIR)pos-rea-ret.c
pos-postfunc.o: $(SRCDIR)pos-postfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pos-postfunc.o $(SRCDIR)pos-postfunc.c
pos-casepost.o: $(SRCDIR)pos-casepost.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pos-casepost.o $(SRCDIR)pos-casepost.c
pos-p-fail.o: $(SRCDIR)pos-p-fail.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pos-p-fail.o $(SRCDIR)pos-p-fail.c
pmdimli.o: $(SRCDIR)pmdimli.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pmdimli.o $(SRCDIR)pmdimli.c
pmam.o: $(SRCDIR)pmam.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pmam.o $(SRCDIR)pmam.c
pmsupport.o: $(SRCDIR)pmsupport.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pmsupport.o $(SRCDIR)pmsupport.c
pmanalyze.o: $(SRCDIR)pmanalyze.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o pmanalyze.o $(SRCDIR)pmanalyze.c
