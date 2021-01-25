CC	= gcc
CFLAGS  = -g -Wall -c
LDFLAGS = -g
INCDIRS = -I./ -I/usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include
LEX	= flex
YACC	= bison
VERSION = version
CAT     = cat
FILES	= main.o tasm2c.tab.o lex.yy.o error.o tcorrect.o support.o\
	  buildtree.o output.o str_support.o tiny.o georg.o ris.o \
	  tis.o c_output.o compile.o comp_support.o heap.o dbug.o optimizer.o

tasm:	$(FILES)
	$(CC) $(LDFLAGS) -o tasm $(FILES) -ly -ll

main.o: main.c 
	@chmod u+w main.vers
	$(CC) $(CFLAGS) -DVERSION='"'`$(VERSION) main.vers`'"' $*.c
	@chmod u-w main.vers
georg.o:georg.c
	@chmod u+w georg.vers
	$(CC) $(CFLAGS) -DVGEORG='"'`$(VERSION) georg.vers`'"' $*.c
	@chmod u-w georg.vers
tis.o:  tis.c 
	@chmod u+w tis.vers
	$(CC) $(CFLAGS) -DVTIS='"'`$(VERSION) tis.vers`'"' tis.c
	@chmod u-w tis.vers

ris.o:  ris.c
	@chmod u+w ris.vers
	$(CC) $(CFLAGS) -DVRIS='"'`$(VERSION) ris.vers`'"' $*.c
	@chmod u-w ris.vers
compile.o:      compile.c
	@chmod u+w compile.vers
	$(CC) $(CFLAGS) -DVCOMPILE='"'`$(VERSION) compile.vers`'"' $*.c
	@chmod u-w compile.vers
c_output.o:     c_output.c
optimizer.o:    optimizer.c 
comp_support.o: comp_support.c 
tiny.o:         tiny.c 
output.o:       output.c 
buildtree.o:    buildtree.c 
support.o:	support.c 
comp_support.o: comp_support.c 
str_support.o:  str_support.c 
heap.o:         heap.c 
error.o:        error.c 
tcorrect.o:     tcorrect.c 
tasm2c.tab.o:   tasm2c.tab.c 
lex.yy.o:       lex.yy.c 
lex.yy.c:       tasm2c.l 
	$(LEX) -t tasm2c.l >lex.yy.c
tasm2c.tab.c tasm2c.tab.h: tasm2c.y
	$(YACC) -d tasm2c.y
dbug.o:          dbug.c

depend:	
	makedepend $(INCDIRS) *.c
clean:
	rm -f $(FILES)
	rm -f tasm2c.tab.c tasm2c.tab.h
	rm -f lex.yy.c
	rm -f tasm

# DO NOT DELETE THIS LINE -- make depend depends on it.

buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
buildtree.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
buildtree.o: dbug.h mytypes.h buildtree.h main.h tiny.h loctypes.h error.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/math.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/floatingpoint.h
c_output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/ieeefp.h
c_output.o: dbug.h mytypes.h buildtree.h error.h
comp_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
comp_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
comp_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
comp_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
comp_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
comp_support.o: mytypes.h main.h dbug.h buildtree.h optimizer.h
compile.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
compile.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
compile.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
compile.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
compile.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
compile.o: main.h mytypes.h dbug.h globals.h buildtree.h comp_support.h
compile.o: error.h c_output.h optimizer.h
dbug.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
dbug.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
dbug.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
error.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
error.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
error.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
error.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
error.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
error.o: main.h mytypes.h
georg.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
georg.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
georg.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
georg.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
georg.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
georg.o: main.h mytypes.h buildtree.h error.h loctypes.h tiny.h stack.h
georg.o: dbug.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/time.h
heap.o: heap.h stack.h
heap.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
heap.o: mytypes.h error.h dbug.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/math.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/floatingpoint.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/ieeefp.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
lex.yy.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/ctype.h
lex.yy.o: mytypes.h tasm2c.tab.h error.h
main.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
main.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
main.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
main.o: main.h
main.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
main.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
main.o: mytypes.h error.h dbug.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/varargs.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
optimizer.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
optimizer.o: dbug.h mytypes.h buildtree.h comp_support.h error.h c_output.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
output.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/ctype.h
output.o: mytypes.h buildtree.h loctypes.h main.h tiny.h dbug.h error.h
ris.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
ris.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
ris.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
ris.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
ris.o: ris.h main.h mytypes.h buildtree.h support.h tiny.h loctypes.h heap.h
ris.o: stack.h
ris.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
ris.o: error.h dbug.h
str_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
str_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
str_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
str_support.o: error.h dbug.h
str_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
str_support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdlib.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
support.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/ctype.h
support.o: mytypes.h buildtree.h loctypes.h main.h tiny.h support.h dbug.h
support.o: error.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
tasm2c.tab.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/memory.h
tasm2c.tab.o: mytypes.h support.h buildtree.h error.h
tcorrect.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
tcorrect.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
tcorrect.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
tcorrect.o: dbug.h
tcorrect.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
tcorrect.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
tcorrect.o: error.h main.h mytypes.h tcorrect.h buildtree.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/ctype.h
tiny.o: mytypes.h
tiny.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
tiny.o: loctypes.h tiny.h tis.h buildtree.h comp_support.h dbug.h error.h
tis.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/malloc.h
tis.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/string.h
tis.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/sys/stdtypes.h
tis.o: mytypes.h
tis.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdio.h
tis.o: /usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.6.3/include/stdarg.h
tis.o: loctypes.h dbug.h buildtree.h support.h tiny.h tis.h error.h main.h
