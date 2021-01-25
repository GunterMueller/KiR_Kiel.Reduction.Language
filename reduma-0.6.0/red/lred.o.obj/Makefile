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
SRCFILES = red/src/ldel-expr.c red/src/lr-ret.c red/src/rdig-mul.c \
           red/src/rpos.c red/src/rstswi.c red/src/lfun-def.c \
           red/src/lr-stor.c red/src/rdig-sub.c red/src/rr1eapos.c \
           red/src/rsubfunc.c red/src/lnr1pre.c red/src/lte-frev.c \
           red/src/rdig-v1.c red/src/rr1pr.c red/src/rtrav.c \
           red/src/lnrecfunc.c red/src/rdig-v2.c red/src/rrecfunc.c \
           red/src/rval-int.c red/src/lpa-sub.c red/src/rclose.c \
           red/src/rhinout.c red/src/rred-eq.c red/src/rwhen-pre.c \
           red/src/lr-args.c red/src/rcomp.c red/src/rlog-mvt.c \
           red/src/rst-match.c red/src/lr-close.c red/src/rdig-add.c \
           red/src/rpartrec.c red/src/rstpre.c red/src/lr-num.c \
           red/src/rdig-div.c red/src/rpartsub.c red/src/rstpro.c \
           red/src/lr-prot.c red/src/rdig-mod.c red/src/rpatvar.c \
           red/src/rsts-dec.c red/src/dumpheap.c red/src/rlogfunc.c \
           red/src/rreduct.c red/src/rstate.c red/src/ram.c \
           red/src/rea-cre.c red/src/rlstruct.c red/src/rfstruct.c \
           red/src/rret-dec.c red/src/rvalfunc.c red/src/rea-ret.c \
           red/src/rretrieve.c red/src/rvalir.c red/src/reac.c \
           red/src/rnf.c red/src/rruntime.c red/src/rconvert.c \
           red/src/rear.c red/src/setup.c red/src/rconvir.c \
           red/src/rheap.c red/src/rquery.c red/src/rst-dec.c \
           red/src/rdesc.c red/src/rreduce.c red/src/rstack.c \
           red/src/rbibfunc.c red/src/rmstruct.c red/src/rvstruct.c \
           red/src/rinter.c red/src/rdummies.c red/src/rextcode.c \
           red/src/rprint.c red/src/rtasm.c
DEPENDFILES = $(SRCDIR)ldel-expr.c $(SRCDIR)lr-ret.c \
              $(SRCDIR)rdig-mul.c $(SRCDIR)rpos.c $(SRCDIR)rstswi.c \
              $(SRCDIR)lfun-def.c $(SRCDIR)lr-stor.c \
              $(SRCDIR)rdig-sub.c $(SRCDIR)rr1eapos.c \
              $(SRCDIR)rsubfunc.c $(SRCDIR)lnr1pre.c \
              $(SRCDIR)lte-frev.c $(SRCDIR)rdig-v1.c $(SRCDIR)rr1pr.c \
              $(SRCDIR)rtrav.c $(SRCDIR)lnrecfunc.c $(SRCDIR)rdig-v2.c \
              $(SRCDIR)rrecfunc.c $(SRCDIR)rval-int.c \
              $(SRCDIR)lpa-sub.c $(SRCDIR)rclose.c $(SRCDIR)rhinout.c \
              $(SRCDIR)rred-eq.c $(SRCDIR)rwhen-pre.c \
              $(SRCDIR)lr-args.c $(SRCDIR)rcomp.c $(SRCDIR)rlog-mvt.c \
              $(SRCDIR)rst-match.c $(SRCDIR)lr-close.c \
              $(SRCDIR)rdig-add.c $(SRCDIR)rpartrec.c $(SRCDIR)rstpre.c \
              $(SRCDIR)lr-num.c $(SRCDIR)rdig-div.c $(SRCDIR)rpartsub.c \
              $(SRCDIR)rstpro.c $(SRCDIR)lr-prot.c $(SRCDIR)rdig-mod.c \
              $(SRCDIR)rpatvar.c $(SRCDIR)rsts-dec.c \
              $(SRCDIR)dumpheap.c $(SRCDIR)rlogfunc.c \
              $(SRCDIR)rreduct.c $(SRCDIR)rstate.c $(SRCDIR)ram.c \
              $(SRCDIR)rea-cre.c $(SRCDIR)rlstruct.c \
              $(SRCDIR)rfstruct.c $(SRCDIR)rret-dec.c \
              $(SRCDIR)rvalfunc.c $(SRCDIR)rea-ret.c \
              $(SRCDIR)rretrieve.c $(SRCDIR)rvalir.c $(SRCDIR)reac.c \
              $(SRCDIR)rnf.c $(SRCDIR)rruntime.c $(SRCDIR)rconvert.c \
              $(SRCDIR)rear.c $(SRCDIR)setup.c $(SRCDIR)rconvir.c \
              $(SRCDIR)rheap.c $(SRCDIR)rquery.c $(SRCDIR)rst-dec.c \
              $(SRCDIR)rdesc.c $(SRCDIR)rreduce.c $(SRCDIR)rstack.c \
              $(SRCDIR)rbibfunc.c $(SRCDIR)rmstruct.c \
              $(SRCDIR)rvstruct.c $(SRCDIR)rinter.c $(SRCDIR)rdummies.c \
              $(SRCDIR)rextcode.c $(SRCDIR)rprint.c $(SRCDIR)rtasm.c
OTHERFILES = configure red.cnf README INSTALL
FILES = ldel-expr.o lr-ret.o rdig-mul.o rpos.o rstswi.o lfun-def.o \
        lr-stor.o rdig-sub.o rr1eapos.o rsubfunc.o lnr1pre.o lte-frev.o \
        rdig-v1.o rr1pr.o rtrav.o lnrecfunc.o rdig-v2.o rrecfunc.o \
        rval-int.o lpa-sub.o rclose.o rhinout.o rred-eq.o rwhen-pre.o \
        lr-args.o rcomp.o rlog-mvt.o rst-match.o lr-close.o rdig-add.o \
        rpartrec.o rstpre.o lr-num.o rdig-div.o rpartsub.o rstpro.o \
        lr-prot.o rdig-mod.o rpatvar.o rsts-dec.o dumpheap.o rlogfunc.o \
        rreduct.o rstate.o ram.o rea-cre.o rlstruct.o rfstruct.o \
        rret-dec.o rvalfunc.o rea-ret.o rretrieve.o rvalir.o reac.o \
        rnf.o rruntime.o rconvert.o rear.o setup.o rconvir.o rheap.o \
        rquery.o rst-dec.o rdesc.o rreduce.o rstack.o rbibfunc.o \
        rmstruct.o rvstruct.o rinter.o rdummies.o rextcode.o rprint.o \
        rtasm.o
CFLAGS = -ansi -Wall -D_ANSI_ -Dsparc -DUNIX=1 -DODDSEX=0 -O \
         -DPI_RED_PLUS=1 -DRED_TO_NF=1 -DDEBUG=0 -DDBUG_OFF \
         -DSCAVENGE=0 -DSCAV_DEBUG=0

lred.o: $(FILES)
	$(LD) $(LDFLAGS) -o lred.o $(FILES) $(LIBS)

install: lred.o
	if [ -d .. ] ; then true ; else mkdir .. ; fi
	if [ -d ../.. ] ; then true ; else mkdir ../.. ; fi
	if [ -d ../../lib ] ; then true ; else mkdir ../../lib ; fi
	$(INSTALL) lred.o $(LIBDIR)

depend:
	$(MAKEDEPEND) $(CFLAGS) $(DEPENDFILES)

clean:
	$(RM) $(FILES)
	$(RM) lred.o

tar:
	($(CD) /tmp/reduma/reduma; $(TAR) lred.o.tar $(OTHERFILES) $(SRCFILES) $(HEADERFILES))
	($(CD) /tmp/reduma/reduma; $(COMPRESS) lred.o.tar)

ldel-expr.o: $(SRCDIR)ldel-expr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ldel-expr.o $(SRCDIR)ldel-expr.c
lr-ret.o: $(SRCDIR)lr-ret.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-ret.o $(SRCDIR)lr-ret.c
rdig-mul.o: $(SRCDIR)rdig-mul.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-mul.o $(SRCDIR)rdig-mul.c
rpos.o: $(SRCDIR)rpos.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rpos.o $(SRCDIR)rpos.c
rstswi.o: $(SRCDIR)rstswi.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstswi.o $(SRCDIR)rstswi.c
lfun-def.o: $(SRCDIR)lfun-def.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lfun-def.o $(SRCDIR)lfun-def.c
lr-stor.o: $(SRCDIR)lr-stor.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-stor.o $(SRCDIR)lr-stor.c
rdig-sub.o: $(SRCDIR)rdig-sub.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-sub.o $(SRCDIR)rdig-sub.c
rr1eapos.o: $(SRCDIR)rr1eapos.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rr1eapos.o $(SRCDIR)rr1eapos.c
rsubfunc.o: $(SRCDIR)rsubfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rsubfunc.o $(SRCDIR)rsubfunc.c
lnr1pre.o: $(SRCDIR)lnr1pre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lnr1pre.o $(SRCDIR)lnr1pre.c
lte-frev.o: $(SRCDIR)lte-frev.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lte-frev.o $(SRCDIR)lte-frev.c
rdig-v1.o: $(SRCDIR)rdig-v1.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-v1.o $(SRCDIR)rdig-v1.c
rr1pr.o: $(SRCDIR)rr1pr.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rr1pr.o $(SRCDIR)rr1pr.c
rtrav.o: $(SRCDIR)rtrav.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rtrav.o $(SRCDIR)rtrav.c
lnrecfunc.o: $(SRCDIR)lnrecfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lnrecfunc.o $(SRCDIR)lnrecfunc.c
rdig-v2.o: $(SRCDIR)rdig-v2.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-v2.o $(SRCDIR)rdig-v2.c
rrecfunc.o: $(SRCDIR)rrecfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rrecfunc.o $(SRCDIR)rrecfunc.c
rval-int.o: $(SRCDIR)rval-int.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rval-int.o $(SRCDIR)rval-int.c
lpa-sub.o: $(SRCDIR)lpa-sub.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lpa-sub.o $(SRCDIR)lpa-sub.c
rclose.o: $(SRCDIR)rclose.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rclose.o $(SRCDIR)rclose.c
rhinout.o: $(SRCDIR)rhinout.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rhinout.o $(SRCDIR)rhinout.c
rred-eq.o: $(SRCDIR)rred-eq.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rred-eq.o $(SRCDIR)rred-eq.c
rwhen-pre.o: $(SRCDIR)rwhen-pre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rwhen-pre.o $(SRCDIR)rwhen-pre.c
lr-args.o: $(SRCDIR)lr-args.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-args.o $(SRCDIR)lr-args.c
rcomp.o: $(SRCDIR)rcomp.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rcomp.o $(SRCDIR)rcomp.c
rlog-mvt.o: $(SRCDIR)rlog-mvt.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rlog-mvt.o $(SRCDIR)rlog-mvt.c
rst-match.o: $(SRCDIR)rst-match.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rst-match.o $(SRCDIR)rst-match.c
lr-close.o: $(SRCDIR)lr-close.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-close.o $(SRCDIR)lr-close.c
rdig-add.o: $(SRCDIR)rdig-add.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-add.o $(SRCDIR)rdig-add.c
rpartrec.o: $(SRCDIR)rpartrec.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rpartrec.o $(SRCDIR)rpartrec.c
rstpre.o: $(SRCDIR)rstpre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstpre.o $(SRCDIR)rstpre.c
lr-num.o: $(SRCDIR)lr-num.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-num.o $(SRCDIR)lr-num.c
rdig-div.o: $(SRCDIR)rdig-div.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-div.o $(SRCDIR)rdig-div.c
rpartsub.o: $(SRCDIR)rpartsub.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rpartsub.o $(SRCDIR)rpartsub.c
rstpro.o: $(SRCDIR)rstpro.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rstpro.o $(SRCDIR)rstpro.c
lr-prot.o: $(SRCDIR)lr-prot.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o lr-prot.o $(SRCDIR)lr-prot.c
rdig-mod.o: $(SRCDIR)rdig-mod.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rdig-mod.o $(SRCDIR)rdig-mod.c
rpatvar.o: $(SRCDIR)rpatvar.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rpatvar.o $(SRCDIR)rpatvar.c
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
ram.o: $(SRCDIR)ram.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o ram.o $(SRCDIR)ram.c
rea-cre.o: $(SRCDIR)rea-cre.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rea-cre.o $(SRCDIR)rea-cre.c
rlstruct.o: $(SRCDIR)rlstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rlstruct.o $(SRCDIR)rlstruct.c
rfstruct.o: $(SRCDIR)rfstruct.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rfstruct.o $(SRCDIR)rfstruct.c
rret-dec.o: $(SRCDIR)rret-dec.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rret-dec.o $(SRCDIR)rret-dec.c
rvalfunc.o: $(SRCDIR)rvalfunc.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rvalfunc.o $(SRCDIR)rvalfunc.c
rea-ret.o: $(SRCDIR)rea-ret.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rea-ret.o $(SRCDIR)rea-ret.c
rretrieve.o: $(SRCDIR)rretrieve.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o rretrieve.o $(SRCDIR)rretrieve.c
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
