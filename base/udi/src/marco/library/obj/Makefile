SRCPATH		= ../

MARCINC		= ../../include

LIBPATH 	= ../../../../lib

CC		= gcc

INCLUDES	= -I$(RED_INC) -I$(FF_DBUG_PATH) -I$(MARCINC)

DEFINES		= $(FF_DBUG) $(marcoDEFINES) $(UDIDEFINES)

DEBUGFLAGS	= -O2

WARNINGFLAGS	= -Wall -ansi

CFLAGS		= -c $(DEBUGFLAGS) $(WARNINGFLAGS) $(INCLUDES) $(DEFINES)


LIBSRC	= $(SRCPATH)marco-print.c \
	  $(SRCPATH)marco-create.c \
	  $(SRCPATH)marco-retrans.c \
	  $(SRCPATH)marco-prnode.c \
	  $(SRCPATH)marco-misc.c \
	  $(SRCPATH)marco-gencode.c \
	  $(SRCPATH)marco-prste.c \
	  $(SRCPATH)marco-options.c

LIBOBJS = marco-print.o marco-create.o marco-retrans.o marco-prnode.o \
	  marco-misc.o marco-gencode.o marco-prste.o marco-options.o

# Make goals

all     : libmarco.o marco-main.o

clean   :
	@(rm -f $(LIBOBJS) libmarco.o marco-main.o)

clobber : clean
	@(rm -f $(LIBPATH)/marco-main.o $(LIBPATH)/libmarco.o)

install : all
	cp libmarco.o $(LIBPATH);
	cp marco-main.o $(LIBPATH)

depend  :
	@(makedepend -s "# DO NOT DELETE" -- $(DEFINES) $(INCLUDES) -I. -- $(LIBSRC) $(SRCPATH)marco-main.c)

# sub goals

libmarco.o : $(LIBOBJS)
	rm -f libmarco.o;
	ld -r -o libmarco.o $(LIBOBJS)

marco-main.o : $(SRCPATH)marco-main.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-main.c -o $@

marco-print.o : $(SRCPATH)marco-print.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-print.c -o $@

marco-create.o : $(SRCPATH)marco-create.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-create.c -o $@

marco-retrans.o : $(SRCPATH)marco-retrans.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-retrans.c -o $@

marco-prnode.o : $(SRCPATH)marco-prnode.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-prnode.c -o $@

marco-misc.o : $(SRCPATH)marco-misc.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-misc.c -o $@

marco-gencode.o : $(SRCPATH)marco-gencode.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-gencode.c -o $@

marco-prste.o : $(SRCPATH)marco-prste.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-prste.c -o $@

marco-options.o : $(SRCPATH)marco-options.c
	$(CC) $(CFLAGS) $(SRCPATH)marco-options.c -o $@

# DO NOT DELETE

marco-print.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
marco-print.o: /usr/include/stdio.h /usr/include/ctype.h
marco-print.o: /usr/include/string.h ../../include/marco-print.h
marco-print.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
marco-print.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
marco-print.o: /usr/include/sys/stat.h ../../include/marco-create.h
marco-print.o: /home/batman/base/red/src/dbug.h
marco-create.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
marco-create.o: /usr/include/stdio.h /usr/include/ctype.h
marco-create.o: /usr/include/string.h ../../include/marco-create.h
marco-create.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
marco-create.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
marco-create.o: /usr/include/sys/stat.h /home/batman/base/red/src/dbug.h
marco-retrans.o: /usr/include/stdio.h /usr/include/stdlib.h
marco-retrans.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
marco-retrans.o: /usr/include/string.h /home/batman/base/red/src/dbug.h
marco-retrans.o: ../../include/marco-options.h /usr/include/sys/types.h
marco-retrans.o: /usr/include/sys/sysmacros.h /usr/include/fcntl.h
marco-retrans.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h
marco-retrans.o: /home/batman/base/red/src/rstackty.h
marco-retrans.o: /home/batman/base/red/src/rstelem.h
marco-retrans.o: /home/batman/base/red/src/cparm.h ../../include/marco-misc.h
marco-retrans.o: ../../include/marco-create.h ../../include/marco-retrans.h
marco-retrans.o: ../../include/marco-prnode.h ../../include/marco-print.h
marco-misc.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
marco-misc.o: /usr/include/stdio.h /usr/include/ctype.h /usr/include/string.h
marco-misc.o: ../../include/marco-options.h /usr/include/sys/types.h
marco-misc.o: /usr/include/sys/sysmacros.h /usr/include/fcntl.h
marco-misc.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h
marco-misc.o: /home/batman/base/red/src/rstackty.h
marco-misc.o: /home/batman/base/red/src/rstelem.h
marco-misc.o: /home/batman/base/red/src/cparm.h ../../include/marco-misc.h
marco-misc.o: ../../include/marco-retrans.h ../../include/marco-create.h
marco-misc.o: ../../include/marco-print.h /home/batman/base/red/src/dbug.h
marco-gencode.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
marco-gencode.o: /usr/include/stdio.h /usr/include/ctype.h
marco-gencode.o: /usr/include/string.h ../../include/marco-create.h
marco-gencode.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
marco-gencode.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
marco-gencode.o: /usr/include/sys/stat.h ../../include/marco-prnode.h
marco-gencode.o: ../../include/marco-options.h
marco-gencode.o: /home/batman/base/red/src/rstackty.h
marco-gencode.o: /home/batman/base/red/src/rstelem.h
marco-gencode.o: /home/batman/base/red/src/cparm.h ../../include/marco-misc.h
marco-gencode.o: /home/batman/base/red/src/dbug.h
marco-options.o: /usr/include/stdio.h /usr/include/fcntl.h
marco-options.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stdtypes.h
marco-options.o: /usr/include/sys/stat.h /usr/include/sys/types.h
marco-options.o: /usr/include/sys/sysmacros.h /usr/include/stdlib.h
marco-options.o: /usr/include/string.h /home/batman/base/red/src/dbug.h
marco-options.o: ../../include/marco-options.h
marco-options.o: /home/batman/base/red/src/rstackty.h
marco-options.o: /home/batman/base/red/src/rstelem.h
marco-options.o: /home/batman/base/red/src/cparm.h
marco-options.o: ../../include/marco-create.h ../../include/marco-misc.h
marco-options.o: ../../include/marco-print.h ../../include/marco-gencode.h
marco-main.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
marco-main.o: /usr/include/stdio.h /usr/include/fcntl.h
marco-main.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h
marco-main.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
marco-main.o: /usr/include/string.h /home/batman/base/red/src/dbug.h
marco-main.o: ../../include/marco-create.h ../../include/marco-options.h
marco-main.o: /home/batman/base/red/src/rstackty.h
marco-main.o: /home/batman/base/red/src/rstelem.h
marco-main.o: /home/batman/base/red/src/cparm.h
