# Makefile for Reduma
# (c) 1993 University of Kiel
#
# This Makefile is automatically generated
# DO NOT EDIT BELOW !

RM = rm -f
LD = ld -r

CFLAGS = -O
RFLAGS = -DKEDIT=0 -DM_PATT=1 -DTRACE=0 -DClausR=0 -DB_DEBUG=0
MFLAGS = -DUNIX=1 -DODDSEX=0 -DTERMIOS=1

CFILES = cfiles.c cmain.c cscred.c cgetexp.c \
	cmatch.c cscroll.c ctransl1.c ccommand.c cgetstr.c cmatrix.c csearch.c \
	cdispl.c cglobals.c cnewscr.c cstacks.c initredp.c cedit.c \
	chelp.c cpexp.c csubst.c cetab.c cinput.c credclr.c ctermio.c \
	cfdispl.c ciotab.c cscanexp.c
OFILES = cfiles.o cmain.o cscred.o cgetexp.o \
	cmatch.o cscroll.o ctransl1.o ccommand.o cgetstr.o cmatrix.o csearch.o \
	cdispl.o cglobals.o cnewscr.o cstacks.o initredp.o cedit.o \
	chelp.o cpexp.o csubst.o cetab.o cinput.o credclr.o ctermio.o \
	cfdispl.o ciotab.o cscanexp.o

.c.o:
	$(CC) -c $(CFLAGS) $(MFLAGS) $(RFLAGS) $(STDFLAGS) $*.c

editor.o: $(OFILES)
	$(LD) -o editor.o $(OFILES)
clean:
	$(RM) editor.o $(OFILES) *.i *.bak tags *.log core *~


# DO NOT DELETE THIS LINE -- make depend depends on it.

cfiles.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/unistd.h
cfiles.o: /usr/include/ctype.h cedit.h cencod.h cetb.h cscred.h ctermio.h
cfiles.o: cestack.h cfiles.h
cmain.o: cswitches.h cportab.h /usr/include/stdio.h cedit.h cscred.h
cmain.o: cestack.h cparm.h cedstate.h cglobals.h ccommand.h
cscred.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cscred.o: cedit.h cscred.h ctermio.h
cgetexp.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cgetexp.o: ctermio.h cedit.h cencod.h cetb.h cscred.h cestack.h ckwtb.h
cgetexp.o: cback.h
cmatch.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cmatch.o: ctermio.h cedit.h
cscroll.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cscroll.o: cedit.h cencod.h cscred.h ctermio.h cestack.h
ctransl1.o: cswitches.h cportab.h /usr/include/stdio.h cedit.h cestack.h
ctransl1.o: cencod.h ckwtb.h ctrans.h ctred.h cscred.h cfiles.h cetb.h
ctransl1.o: cparm.h
ccommand.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
ccommand.o: cscred.h ctermio.h cestack.h cedit.h cencod.h cparm.h ccommand.h
ccommand.o: ckwtb.h cetb.h cfiles.h cedstate.h cglobals.h
cgetstr.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cgetstr.o: cedit.h cencod.h cetb.h cestack.h cback.h ckwtb.h
cmatrix.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cmatrix.o: cedit.h cestack.h cencod.h cetb.h cback.h
csearch.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
csearch.o: cedit.h cencod.h cetb.h cscred.h cestack.h ckwtb.h cparm.h
csearch.o: ccommand.h
cdispl.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cdispl.o: cedit.h cencod.h cscred.h ctermio.h cestack.h cetb.h ckwtb.h
cglobals.o: cswitches.h cportab.h cedit.h cestack.h cparm.h cedstate.h
cglobals.o: cglobals.h
cnewscr.o: cswitches.h cportab.h /usr/include/stdio.h cestack.h cedit.h
cnewscr.o: cencod.h
cstacks.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cstacks.o: cedit.h cencod.h cscred.h ctermio.h cestack.h ckwtb.h ctred.h
initredp.o: /usr/include/stdio.h cparm.h cedstate.h
cedit.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cedit.o: cedit.h cscred.h ctermio.h cestack.h cencod.h cetb.h cback.h
cedit.o: cfiles.h cparm.h cedstate.h cglobals.h ccommand.h
chelp.o: cswitches.h cportab.h /usr/include/stdio.h cedit.h cfiles.h cscred.h
chelp.o: ctermio.h
cpexp.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cpexp.o: cedit.h cencod.h cetb.h cscred.h cestack.h cfiles.h
csubst.o: cswitches.h cportab.h cestack.h cedit.h cscred.h cencod.h ctred.h
cetab.o: cswitches.h cportab.h cedit.h cetb.h
cinput.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cinput.o: cedit.h cencod.h cetb.h cestack.h cscred.h ctermio.h cback.h
cinput.o: ckwtb.h
credclr.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
credclr.o: cedit.h cscred.h ctermio.h cestack.h cencod.h cetb.h ctred.h
credclr.o: cparm.h ccommand.h ckwtb.h cfiles.h
ctermio.o: cportab.h /usr/include/stdio.h /usr/include/ctype.h cswitches.h
ctermio.o: ctermio.h
cfdispl.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cfdispl.o: cedit.h cencod.h cscred.h ctermio.h cestack.h ckwtb.h cfiles.h
ciotab.o: cswitches.h cportab.h cedit.h cestack.h cencod.h cetb.h ckwtb.h
cscanexp.o: cswitches.h cportab.h /usr/include/stdio.h /usr/include/ctype.h
cscanexp.o: ctermio.h cedit.h cencod.h cetb.h cscred.h cestack.h ckwtb.h
cscanexp.o: cback.h
