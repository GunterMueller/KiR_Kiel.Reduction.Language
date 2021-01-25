REDSRC=/home/base/lib
INCDIRS = -I./
INCS    = ./
CODEDIR = ../src
CC      = gcc
HOSTCFLAGS	= -c -D_ANSI_C -Dsparc -DUNIX=1 -DODDSEX=0 -DWITHTILDE=1\
                -DPI_RED_PLUS=1 -DRED_TO_NF=1 -D UNIX=1 -DSCAVENGE=0  \
                -DSCAV_DEBUG=0 -DWITHTILDE=1 -DCLOSEGOAL=0 -DD_SLAVE=0 \
                -DnCUBE=1 -DUH_ZF_PM=1 -DNO_STACK_REG=1 -g 
LREDPATH	= /home/rs/reduma/tasm/lib
LIBPATH	= ../lib
NCLIB	= /usr/ncube/current/lib/sun4
RTPATH	= ../lib
RTINC	= ../lib
NCINC	= /usr/ncube/current/usr/include
LIBS	= -lm
HOSTCODEDIR	= .
PROTDIR	= .

CFLAGS  = -g -c -D_ANSI_ -Dsparc -DUNIX=1 -DODDSEX=0 \
        -DPI_RED_PLUS=1 -DRED_TO_NF=1 -DDEBUG=0 \
        -DSCAVENGE=0 -DSCAV_DEBUG=0 -DWITHTILDE=1 -DCLOSEGOAL=0 \
        -DUH_ZF_PM=1 -DD_SLAVE=0 -DNOREF=1 -DSTORE=0
FILES	= rapply.o rmkclos.o initdata.o inter.o code.o dbug.o nr_rbibfunc.o nr_rconvert.o nr_rlogfunc.o nr_rlstruct.o nr_rmstruct.o nr_rquery.o nr_rvalfunc.o nr_rvstruct.o tlstruct.o tmvtstruct.o tmvt_val.o debug.o intact.o nr_rfstruct.o


libonly: rapply.o rmkclos.o initdata.o inter.o dbug.o nr_rbibfunc.o nr_rconvert.o nr_rlogfunc.o nr_rlstruct.o nr_rmstruct.o nr_rquery.o nr_rvalfunc.o nr_rvstruct.o tlstruct.o tmvtstruct.o tmvt_val.o debug.o intact.o nr_rfstruct.o

a.out:		$(FILES)
		gcc -g $(FILES) $(REDSRC)/lred.o 
host:   	host.o dbug.o
		$(CC) -o host host.o dbug.o $(LREDPATH)/ditred.o  -lm \
                -L$(NCLIB) -L$(LIBPATH) -lncube
host.o: 	host.c
		$(CC) $(HOSTCFLAGS) -DD_MESS=0 -o host.o host.c -I. -I$(RTINC) -I$(NCINC)
mhost:		mhost.o dbug.o
		$(CC) -o mhost mhost.o dbug.o $(LREDPATH)/mditred.o  -lm \
		-L$(NCLIB) -L$(LIBPATH) -lncube
mhost.o:         host.c
		$(CC) $(HOSTCFLAGS) -DD_MESS=1 -o mhost.o host.c -I. -I$(RTINC) -I$(NCINC)

code.o:		$(CODEDIR)/code.c prolog.h
		$(CC) $(CFLAGS) $(CODEDIR)/code.c $(INCDIRS)

rapply.o:	rapply.c
		$(CC) $(CFLAGS) rapply.c
rmkclos.o:	rmkclos.c
		$(CC) $(CFLAGS) rmkclos.c
initdata.o:	initdata.c
		$(CC) $(CFLAGS) initdata.c
intact.o:	intact.c
		$(CC) $(CFLAGS) intact.c
inter.o:	inter.c
		$(CC) $(CFLAGS) inter.c
dbug.o:		dbug.c
		$(CC) $(CFLAGS) dbug.c
tlstruct.o:	tlstruct.c
tmvtstruct.o:	tmvtstruct.c
tmvt_val.o:	tmvt_val.c
nr_rbibfunc.o:  nr_rbibfunc.c
nr_rconvert.o:  nr_rconvert.c
nr_rlogfunc.o:  nr_rlogfunc.c
nr_rlstruct.o:  nr_rlstruct.c
nr_rmstruct.o:  nr_rmstruct.c
nr_rquery.o:    nr_rquery.c
nr_rvalfunc.o:  nr_rvalfunc.c
nr_rvstruct.o:  nr_rvstruct.c
nr_rfstruct.o:  nr_rfstruct.c


depend:		
		makedepend $(INCDIRS) *.c
clean:
		rm -f $(FILES) 
		rm -f a.out
# DO NOT DELETE THIS LINE -- make depend depends on it.

dbug.o: /usr/include/stdio.h /usr/include/malloc.h
host.o: prolog.h /usr/include/stdio.h /usr/include/stdlib.h
host.o: /usr/include/sys/stdtypes.h /usr/include/signal.h
host.o: /usr/include/sys/signal.h /usr/include/vm/faultcode.h
host.o: /usr/include/math.h /usr/include/floatingpoint.h
host.o: /usr/include/sys/ieeefp.h dbug.h rstdinc.h rheapty.h rstackty.h
host.o: rmkclos.h rapply.h rstelem.h rintact.h /usr/include/string.h
host.o: /usr/include/dirent.h /usr/include/sys/types.h
host.o: /usr/include/sys/sysmacros.h /usr/include/sys/dirent.h
host.o: /usr/include/pwd.h tprimf.h rncmessage.h
initdata.o: prolog.h /usr/include/stdio.h /usr/include/stdlib.h
initdata.o: /usr/include/sys/stdtypes.h /usr/include/signal.h
initdata.o: /usr/include/sys/signal.h /usr/include/vm/faultcode.h
initdata.o: /usr/include/math.h /usr/include/floatingpoint.h
initdata.o: /usr/include/sys/ieeefp.h dbug.h rstdinc.h rheapty.h rstackty.h
initdata.o: rmkclos.h rapply.h rstelem.h rintact.h /usr/include/string.h
initdata.o: /usr/include/dirent.h /usr/include/sys/types.h
initdata.o: /usr/include/sys/sysmacros.h /usr/include/sys/dirent.h
initdata.o: /usr/include/pwd.h tprimf.h
intact.o: prolog.h /usr/include/stdio.h /usr/include/stdlib.h
intact.o: /usr/include/sys/stdtypes.h /usr/include/signal.h
intact.o: /usr/include/sys/signal.h /usr/include/vm/faultcode.h
intact.o: /usr/include/math.h /usr/include/floatingpoint.h
intact.o: /usr/include/sys/ieeefp.h dbug.h rstdinc.h rheapty.h rstackty.h
intact.o: rmkclos.h rapply.h rstelem.h rintact.h /usr/include/string.h
intact.o: /usr/include/dirent.h /usr/include/sys/types.h
intact.o: /usr/include/sys/sysmacros.h /usr/include/sys/dirent.h
intact.o: /usr/include/pwd.h tprimf.h
inter.o: /usr/include/string.h /usr/include/sys/stdtypes.h prolog.h
inter.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/signal.h
inter.o: /usr/include/sys/signal.h /usr/include/vm/faultcode.h
inter.o: /usr/include/math.h /usr/include/floatingpoint.h
inter.o: /usr/include/sys/ieeefp.h dbug.h rstdinc.h rheapty.h rstackty.h
inter.o: rmkclos.h rapply.h rstelem.h rintact.h /usr/include/dirent.h
inter.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
inter.o: /usr/include/sys/dirent.h /usr/include/pwd.h tprimf.h
ncdbug.o: /usr/include/stdio.h /usr/include/malloc.h
nr_rbibfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rextern.h
nr_rbibfunc.o: rstelem.h rmeasure.h /usr/include/string.h
nr_rbibfunc.o: /usr/include/sys/stdtypes.h /usr/include/math.h
nr_rbibfunc.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h
nr_rbibfunc.o: /usr/include/setjmp.h dbug.h
nr_rconvert.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rconvert.o: rextern.h rmeasure.h /usr/include/setjmp.h dbug.h
nr_rfstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rfstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h dbug.h rlmvt.h
nr_rlogfunc.o: dbug.h /usr/include/stdio.h rstdinc.h rstackty.h rheapty.h
nr_rlogfunc.o: rstelem.h rextern.h rops.h rmeasure.h /usr/include/setjmp.h
nr_rlstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rlstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h dbug.h rlmvt.h
nr_rmstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rmstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h rlmvt.h dbug.h
nr_rquery.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rquery.o: rextern.h rmeasure.h /usr/include/setjmp.h rlmvt.h dbug.h
nr_rvalfunc.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rvalfunc.o: rextern.h rops.h rmeasure.h /usr/include/setjmp.h dbug.h
nr_rvstruct.o: rstdinc.h /usr/include/stdio.h rstackty.h rheapty.h rstelem.h
nr_rvstruct.o: rextern.h rmeasure.h /usr/include/setjmp.h rlmvt.h dbug.h
rapply.o: rprimf.h rmkclos.h prolog.h /usr/include/stdio.h
rapply.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
rapply.o: /usr/include/signal.h /usr/include/sys/signal.h
rapply.o: /usr/include/vm/faultcode.h /usr/include/math.h
rapply.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h dbug.h
rapply.o: rstdinc.h rheapty.h rstackty.h rapply.h rstelem.h rintact.h
rapply.o: /usr/include/string.h /usr/include/dirent.h
rapply.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
rapply.o: /usr/include/sys/dirent.h /usr/include/pwd.h tprimf.h
rmkclos.o: prolog.h /usr/include/stdio.h /usr/include/stdlib.h
rmkclos.o: /usr/include/sys/stdtypes.h /usr/include/signal.h
rmkclos.o: /usr/include/sys/signal.h /usr/include/vm/faultcode.h
rmkclos.o: /usr/include/math.h /usr/include/floatingpoint.h
rmkclos.o: /usr/include/sys/ieeefp.h dbug.h rstdinc.h rheapty.h rstackty.h
rmkclos.o: rmkclos.h rapply.h rstelem.h rintact.h /usr/include/string.h
rmkclos.o: /usr/include/dirent.h /usr/include/sys/types.h
rmkclos.o: /usr/include/sys/sysmacros.h /usr/include/sys/dirent.h
rmkclos.o: /usr/include/pwd.h tprimf.h
tlstruct.o: /usr/include/stdio.h /usr/include/stdlib.h
tlstruct.o: /usr/include/sys/stdtypes.h /usr/include/math.h
tlstruct.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h dbug.h
tlstruct.o: rstdinc.h rstelem.h rheapty.h rstackty.h
tmvt_val.o: /usr/include/stdio.h /usr/include/stdlib.h
tmvt_val.o: /usr/include/sys/stdtypes.h /usr/include/math.h
tmvt_val.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h dbug.h
tmvt_val.o: rstdinc.h rstelem.h rheapty.h rstackty.h
tmvtstruct.o: /usr/include/stdio.h /usr/include/stdlib.h
tmvtstruct.o: /usr/include/sys/stdtypes.h /usr/include/math.h
tmvtstruct.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h dbug.h
tmvtstruct.o: rstdinc.h rstelem.h rheapty.h rstackty.h
