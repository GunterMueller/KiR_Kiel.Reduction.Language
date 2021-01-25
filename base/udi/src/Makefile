HERE_PATH	= ..

SUBDIRS		= clt_srv dbug clt_srv marco sxred

# Make goals

all	:
	@(echo "Making all in subdirs !";\
	for i in $(SUBDIRS) ;\
	do \
	 ( cd $$i;\
	   echo "Making all in " $$i;\
	   make all ;\
	   cd $(HERE_PATH)\
	 );\
	done;\
	echo "Making all in xv";\
	cd xv;xmkmf;make all ;cd $(HERE_PATH))

clean   :
	@(echo "Making clean in subdirs !";\
	for i in $(SUBDIRS) ;\
	do \
	 ( cd $$i;\
	   echo "Making clean in " $$i;\
	   make clean ;\
	   cd $(HERE_PATH)\
	 );\
	done;\
	echo "Making clean in xv";\
	cd xv;xmkmf;make clean ;cd $(HERE_PATH))

install :
	@(echo "Making install in subdirs !";\
	for i in $(SUBDIRS) ;\
	do \
	 ( cd $$i;\
	   echo "Making install in " $$i;\
	   make install ;\
	   cd $(HERE_PATH)\
	 );\
	done;\
	echo "Making install in xv";\
	cd xv;xmkmf;make install ; cd $(HERE_PATH))

depend  :
	@(echo "Making depend in subdirs !";\
	for i in $(SUBDIRS) ;\
	do \
	 ( cd $$i;\
	   echo "Making depend in " $$i;\
	   make depend ;\
	   cd $(HERE_PATH)\
	 );\
	done;\
	echo "Making depend in xv";\
	cd xv;xmkmf;make depend ;cd $(HERE_PATH))
