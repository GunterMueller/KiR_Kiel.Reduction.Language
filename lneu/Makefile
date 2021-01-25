# Makefile for Reduma
# (c) 1993 University of Kiel
#
# This Makefile is automatically generated
# DO NOT EDIT BELOW !

CD = cd
LN = ln -s
RM = rm -f
TAR = tar cf
PACK = compress


LIBS = -ltermcap -lm

all: lneu

_editor:
	@($(CD) editor; $(MAKE) editor.o)
lneu: _lneu
	$(RM) lneu
	$(LN) lneu.o/lneu lneu
_lneu: _editor
	@($(CD) lneu.o; $(MAKE) lneu)
clean:
	@($(CD) editor; $(MAKE) clean)
	$(RM) lneu
	@($(CD) lneu.o; $(MAKE) clean)
dist:
	(cd ..; $(TAR) lneu/red.tar lneu/configure lneu/reduma.cnf lneu/editor.cnf lneu/src/*.[ch] lneu/editor/*.[ch] lneu/.red.*.*)
	$(PACK) red.tar
tape:
	$(TAR) cf /dev/rst0 configure reduma.cnf editor.cnf src fed
