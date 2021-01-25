EDO = /home/majestix/base/fed.o/editor.o

RM = rm -rf

BC_FILES = ldel-expr.c lr-ret.c rdig-mul.c rpos.c rstswi.c lfun-def.c \
	lr-stor.c rdig-sub.c rr1eapos.c rsubfunc.c lnr1pre.c lte-frev.c \
	rdig-v1.c rr1pr.c rtrav.c lnrecfunc.c rdig-v2.c rrecfunc.c \
	rval-int.c lpa-sub.c rclose.c rhinout.c rred-eq.c rwhen-pre.c \
	lr-args.c rcomp.c rlog-mvt.c rst-match.c lr-close.c rdig-add.c \
	rpartrec.c rstpre.c lr-num.c rdig-div.c rpartsub.c rstpro.c lr-prot.c \
	rdig-mod.c rpatvar.c rsts-dec.c
PC_FILES = dumpheap.c rlogfunc.c rreduct.c rstate.c ram.c rea-cre.c \
	rlstruct.c rret-dec.c rvalfunc.c rea-ret.c rretrieve.c rvalir.c \
	reac.c rnf.c rruntime.c rconvert.c rear.c rscavenge.c setup.c \
	rconvir.c rheap.c rquery.c rst-dec.c rdesc.c rreduce.c rstack.c

C_FILES = $(BC_FILES) $(PC_FILES)

I_FILES = rinter.c rdummies.c
D_FILES = r-debug.c rbreak.c rprint.c
L_FILES = rbibfunc.c rmstruct.c rvstruct.c
S_FILES = rbibfunc.small.c rmstruct.small.c rvstruct.small.c

BH_FILES = rextern.h rmeasure.h rinstr.h rops.h rruntime.h cparm.h rmodulnr.h
PH_FILES = dumpheap.h rinter.h rprimf.h rstdinc.h traps.h rdigit.h rlmvt.h \
	rscavenge.h rstelem.h rheapty.h rnf.h rstackty.h tags.h
H_FILES = $(BH_FILES) $(PH_FILES)

LIBS = -ltermcap -lm

# select CFLAGS for Sun or Apollo
FLAGS  = -c -DUNIX=1 -DODDSEX=0 -DPI_RED_PLUS=1 -DRED_TO_NF=1 $(CFLAGS)
#FLAGS  = -DUNIX=1 -DAPOLLO=1 -DSYS5=1 DODDSEX=0 -DPI_RED_PLUS=1 -DRED_TO_NF=1 $(CFLAGS)
LFLAGS  = -DDEBUG=0 -DDBUG_OFF -DSCAVENGE=0 -DSCAV_DEBUG=0 $(FLAGS)
SFLAGS  = -DDEBUG=0 -DDBUG_OFF -DSCAVENGE=1 -DSCAV_DEBUG=0 $(FLAGS)
DLFLAGS = -DDEBUG=1 -DSCAVENGE=0 -DSCAV_DEBUG=0 $(FLAGS)
DSFLAGS = -DDEBUG=1 -DSCAVENGE=1 -DSCAV_DEBUG=1 $(FLAGS)

VPATH = $(OFILE_DIR)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $(OFILE_DIR)/$*.o

l:
	@echo "making lred ..."
	@$(MAKE) depend "OFILE_DIR = ../l.o" \
	    "FILES = $(C_FILES) $(L_FILES) $(I_FILES)" "CFLAGS = $(LFLAGS) -O"
	@$(MAKE) lred "OFILE_DIR = ../l.o" "FILES = $(C_FILES) $(L_FILES) $(I_FILES)" \
	    "LDFILES = $(C_FILES) $(L_FILES)" "PRODUCT = lred" "LDFLAGS = $(LDFLAGS) -O"
s:
	@echo "making sred ..."
	@$(MAKE) depend "OFILE_DIR = ../s.o" \
	    "FILES = $(C_FILES) $(S_FILES) $(I_FILES)" "CFLAGS = $(SFLAGS) -O"
	@$(MAKE) sred "OFILE_DIR = ../s.o" "FILES = $(C_FILES) $(S_FILES) $(I_FILES)" \
	    "LDFILES = $(C_FILES) $(S_FILES)" "PRODUCT = sred" "LDFLAGS = $(LDFLAGS) -O"
dl:
	@echo "making dlred ..."
	@$(MAKE) depend "OFILE_DIR = ../dl.o" \
	    "FILES = $(C_FILES) $(L_FILES) $(D_FILES) $(I_FILES)" \
	    "CFLAGS = $(DLFLAGS) -g"
	@$(MAKE) dlred "OFILE_DIR = ../dl.o" \
	    "FILES = $(C_FILES) $(L_FILES) $(D_FILES) $(I_FILES)" \
	    "LDFILES = $(C_FILES) $(L_FILES) $(D_FILES)" \
	    "PRODUCT = dlred" "LDFLAGS = $(LDFLAGS) -g"
ds:
	@echo "making dsred ..."
	@$(MAKE) depend "OFILE_DIR = ../ds.o" \
	    "FILES = $(C_FILES) $(S_FILES) $(D_FILES) $(I_FILES)" \
	    "CFLAGS = $(DSFLAGS) -g"
	@$(MAKE) dsred "OFILE_DIR = ../ds.o" \
	    "FILES = $(C_FILES) $(S_FILES) $(D_FILES) $(I_FILES)" \
	    "LDFILES = $(C_FILES) $(S_FILES) $(D_FILES)" \
	    "PRODUCT = dsred" "LDFLAGS = $(LDFLAGS) -g"

all: l s dl ds

dependencies:
	makedepend -- $(DLFLAGS) -- $(C_FILES) $(L_FILES) $(S_FILES)

clean:
	$(RM) lred sred dlred dsred lred.o sred.o dlred.o dsred.o
	$(RM) ../l.o ../s.o ../dl.o ../ds.o

$(OFILE_DIR):
	mkdir $@

depend: $(OFILE_DIR) $(FILES:.c=.o)

$(PRODUCT): $(FILES:.c=.o)
	$(CC) $(LDFLAGS) -o $(PRODUCT) $(FILES:.c=.o) $(EDO) $(LIBS)
	$(LD) -r -o $(PRODUCT).o $(LDFILES:.c=.o)

