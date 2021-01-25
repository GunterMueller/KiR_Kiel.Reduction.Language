SRCPATH		= ../

PROGNAME	= marco
MARCINC		= ../../include

BINPATH		= ../../../../bin

CC		= gcc
WARNINGFLAGS    = -Wall -ansi -pedantic
INCLUDES	= -I$(MARCINC) -I$(FF_DBUG_PATH) -I.
DEBUG		= -O2
GCFLAGS		= -c $(DEBUG) $(FF_DBUG) $(INCLUDES)
CFLAGS		= $(WARNINGFLAGS) $(GCFLAGS)

LIBS		= -ll

LEX		= flex
LFLAGS		=

YACC		= bison
YFLAGS		= -d -t

SRC1		= $(SRCPATH)gen-ccode.c \
		  $(SRCPATH)gen-scanner.c \
		  $(SRCPATH)gen-parser.c \
		  $(SRCPATH)gen-options.c \
		  $(SRCPATH)list-modu.c \
		  $(SRCPATH)err-modu.c \
		  $(SRCPATH)hash-fcts.c \
		  $(SRCPATH)trans-tbl.c \
		  $(SRCPATH)main.c

SRC		= $(SRC1) \
		  $(SRCPATH)ccomp.l \
		  $(SRCPATH)ccomp.y
                  

MOBJS		= gen-ccode.o gen-scanner.o gen-parser.o gen-options.o\
                  list-modu.o err-modu.o trans-tbl.o main.o hash-fcts.o
GOBJS		= ccomp.tab.o lex.yy.o
OBJS		= $(GOBJS) $(MOBJS)

# Make goals

all	: $(PROGNAME)
	@(rm -f $(SRCPATH)$(PROGNAME);ln -s obj/$(PROGNAME) $(SRCPATH)$(PROGNAME))

clean	:
	@(rm -f $(OBJS) $(PROGNAME) ccomp.tab.c ccomp.tab.h lex.yy.c ccomp.c);
	@(rm -f $(SRCPATH)$(PROGNAME))

clobber : clean
	@(rm -f $(BINPATH)/$(PROGNAME))

install : all
	@(cp $(PROGNAME) $(BINPATH))

depend	:
	@(touch ccomp.tab.h;)
	@(makedepend -s "# DO NOT DELETE" -- $(DEFINES) $(INCLUDES) -I. -I$(SRCPATH) -- $(SRC))

# sub goals

$(PROGNAME) : $(OBJS)
	$(CC) -o $(PROGNAME) $(OBJS) $(FF_DBUG_LIB) $(LIBS)

lex.yy.o : lex.yy.c
	$(CC) $(GCFLAGS) lex.yy.c

lex.yy.c : $(SRCPATH)ccomp.l ccomp.tab.c
	$(LEX) $(LFLAGS) $(SRCPATH)ccomp.l

ccomp.tab.o : ccomp.tab.c
	$(CC) $(GCFLAGS) ccomp.tab.c;

ccomp.tab.c : $(SRCPATH)ccomp.y
	$(YACC) $(YFLAGS) $(SRCPATH)ccomp.y
	mv $(SRCPATH)ccomp.tab.c .;
	mv $(SRCPATH)ccomp.tab.h .

gen-ccode.o : $(SRCPATH)gen-ccode.c
	$(CC) $(CFLAGS) $(SRCPATH)gen-ccode.c

gen-scanner.o : $(SRCPATH)gen-scanner.c
	$(CC) $(CFLAGS) $(SRCPATH)gen-scanner.c

gen-parser.o : $(SRCPATH)gen-parser.c
	$(CC) $(CFLAGS) $(SRCPATH)gen-parser.c

gen-options.o : $(SRCPATH)gen-options.c
	$(CC) $(CFLAGS) $(SRCPATH)gen-options.c

list-modu.o : $(SRCPATH)list-modu.c
	$(CC) $(CFLAGS) $(SRCPATH)list-modu.c

err-modu.o : $(SRCPATH)err-modu.c
	$(CC) $(CFLAGS) $(SRCPATH)err-modu.c

trans-tbl.o : $(SRCPATH)trans-tbl.c
	$(CC) $(CFLAGS) $(SRCPATH)trans-tbl.c

hash-fcts.o : $(SRCPATH)hash-fcts.c
	$(CC) $(CFLAGS) $(SRCPATH)hash-fcts.c

main.o : $(SRCPATH)main.c
	$(CC) $(CFLAGS) $(SRCPATH)main.c

# DO NOT DELETE

gen-ccode.o: /usr/include/stdio.h /usr/include/stdlib.h
gen-ccode.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
gen-ccode.o: /usr/include/string.h ../hash-fcts.h ../../../../src/dbug/dbug.h
gen-ccode.o: ../gen-scanner.h /usr/include/sys/types.h
gen-ccode.o: /usr/include/sys/sysmacros.h /usr/include/fcntl.h
gen-ccode.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h
gen-ccode.o: ../gen-options.h ../gen-parser.h ../gen-ccode.h
gen-ccode.o: ../../include/marco-create.h ../trans-tbl.h ../list-modu.h
gen-scanner.o: /usr/include/stdio.h /usr/include/stdlib.h
gen-scanner.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
gen-scanner.o: /usr/include/string.h ../hash-fcts.h ../gen-scanner.h
gen-scanner.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
gen-scanner.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
gen-scanner.o: /usr/include/sys/stat.h ../gen-options.h ../gen-parser.h
gen-scanner.o: ../gen-ccode.h ../../../../src/dbug/dbug.h ccomp.tab.h
gen-scanner.o: ../scanner-txt.h
gen-parser.o: /usr/include/stdio.h /usr/include/stdlib.h
gen-parser.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
gen-parser.o: /usr/include/string.h ../hash-fcts.h
gen-parser.o: ../../../../src/dbug/dbug.h ../gen-scanner.h
gen-parser.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
gen-parser.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
gen-parser.o: /usr/include/sys/stat.h ../gen-options.h ../gen-parser.h
gen-parser.o: ../gen-ccode.h ../list-modu.h ../err-modu.h ../trans-tbl.h
gen-options.o: /usr/include/stdio.h /usr/include/stdlib.h
gen-options.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
gen-options.o: /usr/include/string.h ../gen-options.h
gen-options.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
gen-options.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
gen-options.o: /usr/include/sys/stat.h ../gen-scanner.h ../gen-parser.h
gen-options.o: ../gen-ccode.h ../../../../src/dbug/dbug.h ccomp.tab.h
list-modu.o: /usr/include/stdlib.h /usr/include/sys/stdtypes.h
list-modu.o: ../../../../src/dbug/dbug.h /usr/include/stdio.h
err-modu.o: /usr/include/stdio.h /usr/include/stdlib.h
err-modu.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
err-modu.o: /usr/include/string.h ../../../../src/dbug/dbug.h ../err-modu.h
err-modu.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
err-modu.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
err-modu.o: /usr/include/sys/stat.h
hash-fcts.o: /usr/include/stdio.h /usr/include/assert.h /usr/include/string.h
hash-fcts.o: /usr/include/sys/stdtypes.h ../hash-fcts.h
hash-fcts.o: ../../../../src/dbug/dbug.h
trans-tbl.o: /usr/include/stdio.h /usr/include/stdlib.h
trans-tbl.o: /usr/include/sys/stdtypes.h /usr/include/ctype.h
trans-tbl.o: /usr/include/string.h ../gen-ccode.h /usr/include/sys/types.h
trans-tbl.o: /usr/include/sys/sysmacros.h /usr/include/fcntl.h
trans-tbl.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h
trans-tbl.o: ../gen-options.h ../list-modu.h ../trans-tbl.h ../gen-parser.h
trans-tbl.o: ../../include/marco-create.h ../../../../src/dbug/dbug.h
main.o: /usr/include/stdio.h /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
main.o: /usr/include/sys/stdtypes.h /usr/include/sys/stat.h
main.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
main.o: /usr/include/stdlib.h /usr/include/string.h
main.o: ../../../../src/dbug/dbug.h ../gen-options.h ../gen-parser.h
main.o: ../gen-ccode.h ../trans-tbl.h ../err-modu.h
ccomp.o: ../gen-parser.h /usr/include/sys/types.h /usr/include/sys/stdtypes.h
ccomp.o: /usr/include/sys/sysmacros.h /usr/include/fcntl.h
ccomp.o: /usr/include/sys/fcntlcom.h /usr/include/sys/stat.h ../gen-options.h
ccomp.o: ../gen-ccode.h ccomp.tab.h
ccomp.o: ../../../../src/dbug/dbug.h /usr/include/stdio.h
ccomp.o: /usr/include/string.h /usr/include/sys/stdtypes.h ../gen-options.h
ccomp.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
ccomp.o: /usr/include/fcntl.h /usr/include/sys/fcntlcom.h
ccomp.o: /usr/include/sys/stat.h ../gen-scanner.h ../gen-parser.h
ccomp.o: ../gen-ccode.h ../trans-tbl.h ../err-modu.h ../list-modu.h
