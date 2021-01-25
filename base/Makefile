CP = cp -r -f -p -d
MKDIR = mkdir -p
LN = ln -s -f
RM = rm -r -f
TAR = tar cvf
GZIP = gzip

.PHONY: all dist_red dist_lneu clean

all: dist_red dist_lneu

dist_red:
	$(MKDIR) distrib/red/fed distrib/red/fed/src distrib/red/red distrib/red/red/src distrib/red/doc
	$(CP) COPYRIGHT distrib/red
	$(CP) fed/src/*.* distrib/red/fed/src
	$(CP) red/src/*.* distrib/red/red/src
	$(CP) red/src/configure distrib/red/red/src
	$(CP) red/red.init distrib/red
	$(CP) red/INSTALL distrib/red
	$(CP) red/README distrib/red
	$(CP) red/creduma.hlp distrib/red/doc
	$(CP) papers/reduma-guide-wk.ps.gz distrib/red/doc/reduma.ps.gz
	$(LN) fed/src/fed.cnf distrib/red/fed.cnf
	$(LN) red/src/red.cnf distrib/red/red.cnf
	$(LN) red/src/configure distrib/red/configure
	( cd distrib; $(TAR) red.tar red; $(GZIP) red.tar )

dist_lneu:
	$(MKDIR) distrib/lneu/editor distrib/lneu/src
	$(CP) COPYRIGHT distrib/lneu
	$(CP) fed/src/*.* distrib/lneu/editor
	$(CP) lneu/src/*.* distrib/lneu/src
	$(CP) lneu/src/configure distrib/lneu/src
	$(CP) lneu/red.init distrib/lneu
	$(CP) lneu/README distrib/lneu
	$(CP) lneu/editor.cnf distrib/lneu
#	$(LN) editor/src/fed.cnf distrib/lneu/editor.cnf
	$(LN) src/reduma.cnf distrib/lneu/reduma.cnf
	$(LN) src/configure distrib/lneu/configure
	( cd distrib; $(TAR) lneu.tar lneu; $(GZIP) lneu.tar )


clean_red:
	$(RM) distrib/red/*
	$(RM) distrib/red.tar.gz

clean_lneu:
	$(RM) distrib/lneu/*
	$(RM) distrib/lneu.tar.gz

clean: clean_red clean_lneu
