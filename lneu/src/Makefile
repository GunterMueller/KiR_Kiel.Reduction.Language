EDO = /home/batman/base/fed.o/editor.o

RM = rm -rf

C_FILES = redclos.c reac.c rhinout.c rstpro.c rstpre.c rea-ret.c rdesc.c \
	rheap.c nsubfunc.c casefunc.c nea-cre.c lrecfunc.c hfunc.c pm-ptfu.c \
	rreduct.c  rprint.c rstack.c rstate.c pmam.c pmdimli.c  rvalir.c \
	pmred-nomat.c pmredswi.c pmredeswi.c pmredwhen.c p-fail.c\
	casepost.c condfunc.c close-cond.c postfunc.c rreduce.c \
	rlstruct.c rquery.c rred-eq.c rruntime.c  rtrav.c  pm-debug.c \
	pmpatvar.c red-nomat-when.c rst-dec.c rconvir.c rbreak.c dbug.c
S_FILES =rear.neu_sc.c rlogfunc_sc.c rvalfunc_sc.c
L_FILES =rear.neu.c rlogfunc.c rvalfunc.c pmst-slice.c preproc.c \
	os-send.neu.c testf.c rdig-add.c rdig-div.c rdig-mod.c \
	rdig-sub.c rdig-v1.c rdig-v2.c rvstruct.c rret-dec.c \
	rval-int.c rsts-dec.c rlog-mvt.c rdig-mul.c \
	rconvert.c rbibfunc.c rmstruct.c

LIBS = -ltermcap -lm

# select CFLAGS for Sun or Apollo
FLAGS  = -c -DUNIX=1 -DLETREC=0 -DODDSEX=0 -DSTANDALONE=1 -DDREDUMA=1 -DEAGER=1 $(CFLAGS)
#FLAGS  = -DUNIX=1 -DAPOLLO=1 -DSYS5=1 -DLETREC=0 -DODDSEX=0 \
#	-DSTANDALONE=1 -DDREDUMA=1 -DEAGER=1 $(CFLAGS)
LFLAGS  = -DDEBUG=0 -DLARGE=1 -DDBUG_OFF $(FLAGS)
SFLAGS  = -DDEBUG=0 -DLARGE=0 -DDBUG_OFF $(FLAGS)
DLFLAGS = -DDEBUG=1 -DLARGE=1 $(FLAGS)
DSFLAGS = -DDEBUG=1 -DLARGE=0 $(FLAGS)

VPATH = $(OFILE_DIR)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $(OFILE_DIR)/$*.o

l:
	@echo "making lneu ..."
	@$(MAKE) depend "OFILE_DIR = ../l.o" \
	    "FILES = $(C_FILES) $(L_FILES)" "CFLAGS = $(LFLAGS) -O"
	@$(MAKE) lneu "OFILE_DIR = ../l.o" "FILES = $(C_FILES) $(L_FILES)" \
	    "PRODUCT = lneu" "LDFLAGS = $(LDFLAGS) -O"
s:
	@echo "making sneu ..."
	@$(MAKE) depend "OFILE_DIR = ../s.o" \
	    "FILES = $(C_FILES) $(S_FILES)" "CFLAGS = $(SFLAGS) -O"
	@$(MAKE) sneu "OFILE_DIR = ../s.o" "FILES = $(C_FILES) $(S_FILES)" \
	    "PRODUCT = sneu" "LDFLAGS = $(LDFLAGS) -O"
dl:
	@echo "making dlneu ..."
	@$(MAKE) depend "OFILE_DIR = ../dl.o" \
	    "FILES = $(C_FILES) $(L_FILES)" "CFLAGS = $(DLFLAGS) -g"
	@$(MAKE) dlneu "OFILE_DIR = ../dl.o" "FILES = $(C_FILES) $(L_FILES)" \
	    "PRODUCT = dlneu" "LDFLAGS = $(LDFLAGS) -g"
ds:
	@echo "making dsneu ..."
	@$(MAKE) depend "OFILE_DIR = ../ds.o" \
	    "FILES = $(C_FILES) $(S_FILES)" "CFLAGS = $(DSFLAGS) -g"
	@$(MAKE) dsneu "OFILE_DIR = ../ds.o" "FILES = $(C_FILES) $(S_FILES)" \
	    "PRODUCT = dsneu" "LDFLAGS = $(LDFLAGS) -g"

all: l s dl ds

dependencies:
	makedepend -- $(DLFLAGS) -- $(C_FILES) $(L_FILES) $(S_FILES)

clean:
	$(RM) lneu sneu dlneu dsneu lneu.o sneu.o dlneu.o dsneu.o
	$(RM) ../l.o ../s.o ../dl.o ../ds.o

$(OFILE_DIR):
	mkdir $@

depend: $(OFILE_DIR) $(FILES:.c=.o)

$(PRODUCT): $(FILES:.c=.o)
	$(CC) $(LDFLAGS) -o $(PRODUCT) $(FILES:.c=.o) $(EDO) $(LIBS)
	$(LD) -r -o $(PRODUCT).o $(FILES:.c=.o)

# DO NOT DELETE THIS LINE -- make depend depends on it.

redclos.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
redclos.o: cparm.h rextern.h rusedeb.h ct-tab.h case.h /usr/include/setjmp.h
redclos.o: dbug.h
reac.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
reac.o: rextern.h rmeasure.h /usr/include/setjmp.h
rhinout.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rhinout.o: rextern.h rmeasure.h
rstpro.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rstpro.o: rextern.h
rstpre.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rstpre.o: rextern.h rmeasure.h
rea-ret.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rea-ret.o: rextern.h /usr/include/setjmp.h list.h case.h
rheap.o: rstdinc.h /usr/include/stdio.h rheapty.h rextern.h rstackty.h
rheap.o: rmeasure.h /usr/include/string.h /usr/include/sys/stdtypes.h list.h
rheap.o: case.h
nsubfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nsubfunc.o: rextern.h list.h
casefunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
casefunc.o: rextern.h list.h case.h
nea-cre.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nea-cre.o: rextern.h /usr/include/setjmp.h list.h
lrecfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
lrecfunc.o: rextern.h /usr/include/setjmp.h list.h
hfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
hfunc.o: rextern.h list.h
pm-ptfu.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pm-ptfu.o: rextern.h pminstr.h /usr/include/setjmp.h case.h
rreduct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rreduct.o: rextern.h list.h
rstack.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rstack.o: rextern.h
rstate.o: rstdinc.h /usr/include/stdio.h rstackty.h rstelem.h rheapty.h
rstate.o: /usr/include/setjmp.h list.h
pmam.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmam.o: rextern.h pminstr.h case.h
pmdimli.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmdimli.o: rextern.h pminstr.h
pmred-nomat.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmred-nomat.o: rextern.h pminstr.h
pmredswi.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmredswi.o: rextern.h pminstr.h
pmredeswi.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmredeswi.o: rextern.h pminstr.h
pmredwhen.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmredwhen.o: rextern.h pminstr.h
p-fail.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
p-fail.o: rextern.h ct-tab.h
casepost.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
casepost.o: rextern.h list.h case.h
condfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
condfunc.o: rextern.h list.h
close-cond.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
close-cond.o: rextern.h
postfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
postfunc.o: rextern.h rmeasure.h list.h
rreduce.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rreduce.o: rextern.h rusedeb.h cparm.h /usr/include/math.h
rreduce.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h
rreduce.o: /usr/include/signal.h /usr/include/sys/signal.h
rreduce.o: /usr/include/vm/faultcode.h /usr/include/setjmp.h
rlstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rlstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h rlmvt.h
rquery.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rquery.o: rextern.h rusedeb.h /usr/include/setjmp.h
rred-eq.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rred-eq.o: rextern.h pminstr.h
rruntime.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rruntime.o: rextern.h rruntime.h /usr/include/ctype.h /usr/include/setjmp.h
rtrav.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rtrav.o: rextern.h rmeasure.h
pm-debug.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pm-debug.o: rextern.h pminstr.h
pmpatvar.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmpatvar.o: rextern.h rinstr.h
red-nomat-when.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h
red-nomat-when.o: rstelem.h rextern.h pminstr.h
rst-dec.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rst-dec.o: rextern.h rmeasure.h rdigit.h
rconvir.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rconvir.o: rextern.h rmeasure.h
rbreak.o: rstdinc.h /usr/include/stdio.h rmeasure.h rruntime.h rmodulnr.h
rbibfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rbibfunc.o: rstelem.h rmeasure.h /usr/include/string.h
rbibfunc.o: /usr/include/sys/stdtypes.h /usr/include/math.h
rbibfunc.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h
rbibfunc.o: /usr/include/setjmp.h
rmstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rmstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h rlmvt.h
dbug.o: /usr/include/stdio.h
rear.neu.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rear.neu.o: cparm.h rextern.h rusedeb.h ct-tab.h case.h /usr/include/setjmp.h
rear.neu.o: dbug.h
rlogfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rlogfunc.o: rextern.h rops.h /usr/include/setjmp.h
rvalfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rvalfunc.o: rextern.h rops.h rmeasure.h /usr/include/setjmp.h
pmst-slice.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
pmst-slice.o: rextern.h pminstr.h
preproc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
preproc.o: rextern.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
preproc.o: /usr/include/sys/stdtypes.h /usr/include/sys/stat.h
preproc.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
os-send.neu.o: /usr/include/sys/errno.h /usr/include/setjmp.h rstdinc.h
os-send.neu.o: /usr/include/stdio.h rstackty.h rheapty.h rextern.h rinstr.h
os-send.neu.o: dbug.h case.h
testf.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
testf.o: rextern.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
testf.o: /usr/include/sys/stdtypes.h /usr/include/sys/stat.h
testf.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h list.h
rdig-add.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-add.o: rmeasure.h rdigit.h
rdig-div.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-div.o: rmeasure.h rdigit.h
rdig-mod.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-mod.o: rmeasure.h rdigit.h
rdig-sub.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-sub.o: rmeasure.h rdigit.h
rdig-v1.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-v1.o: rmeasure.h rdigit.h
rdig-v2.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-v2.o: rmeasure.h rdigit.h
rvstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rvstruct.o: rextern.h /usr/include/setjmp.h rlmvt.h
rret-dec.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rret-dec.o: rextern.h rdigit.h
rval-int.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rval-int.o: rextern.h rops.h rmeasure.h /usr/include/setjmp.h
rsts-dec.o: /usr/include/string.h /usr/include/sys/stdtypes.h
rsts-dec.o: /usr/include/ctype.h rstdinc.h /usr/include/stdio.h rheapty.h
rsts-dec.o: rstackty.h rextern.h rmeasure.h rdigit.h
rlog-mvt.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rlog-mvt.o: rextern.h rops.h rmeasure.h /usr/include/setjmp.h
rvalir.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rvalir.o: rextern.h rmeasure.h
rdig-mul.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
rdig-mul.o: rmeasure.h rdigit.h
rconvert.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rconvert.o: rextern.h rmeasure.h /usr/include/setjmp.h
rear.neu_sc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rear.neu_sc.o: cparm.h rextern.h rusedeb.h ct-tab.h case.h
rear.neu_sc.o: /usr/include/setjmp.h dbug.h
rlogfunc_sc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rlogfunc_sc.o: rextern.h rops.h /usr/include/setjmp.h
rvalfunc_sc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
rvalfunc_sc.o: rextern.h rops.h rmeasure.h /usr/include/setjmp.h
