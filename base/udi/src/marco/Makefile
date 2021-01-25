# Configure first subdirectories !

HERE_PATH	= ..
MARC_COMP_PATH	= compgen
MARC_LIB_PATH	= library

EXAMPLE_PATH	= examples
EXAMPLES	= kir calc

# Make goals

all	:
	@(echo "Making all in subdirs !";\
	cd $(MARC_COMP_PATH);make all $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	cd $(MARC_LIB_PATH);make all $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	for i in $(EXAMPLES) ;\
	do \
	 ( cd $(EXAMPLE_PATH);cd $$i;\
	   echo "Making example" $$i "in" $(EXAMPLE_PATH)/$$i;\
	   make all $(SUBMAKEFILEMAC) ;\
	   cd $(HERE_PATH)\
	 );\
	done)

clean   :
	@(echo "Making clean in subdirs !";\
	cd $(MARC_COMP_PATH);make clean $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	cd $(MARC_LIB_PATH);make clean $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	for i in $(EXAMPLES) ;\
	do \
	 ( cd $(EXAMPLE_PATH);cd $$i;\
	   echo "Clean example" $$i "in" $(EXAMPLE_PATH)/$$i;\
	   make clean $(SUBMAKEFILEMAC) ;\
	   cd $(HERE_PATH)\
	 );\
	done)

install :
	@(echo "Making install in subdirs !";\
	cd $(MARC_COMP_PATH);make install $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	cd $(MARC_LIB_PATH);make install $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	for i in $(EXAMPLES) ;\
	do \
	 ( cd $(EXAMPLE_PATH);cd $$i;\
	   echo "Install example" $$i "in" $(EXAMPLE_PATH)/$$i;\
	   make install $(SUBMAKEFILEMAC) ;\
	   cd $(HERE_PATH)\
	 );\
	done)

depend  :
	@(echo "Making depend in subdirs !";\
	cd $(MARC_COMP_PATH);make depend $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	cd $(MARC_LIB_PATH);make depend $(SUBMAKEFILEMAC) ;cd $(HERE_PATH);\
	for i in $(EXAMPLES) ;\
	do \
	 ( cd $(EXAMPLE_PATH);cd $$i;\
	   echo "Making dependencies " $$i "in" $(EXAMPLE_PATH)/$$i;\
	   make depend $(SUBMAKEFILEMAC) ;\
	   cd $(HERE_PATH)\
	 );\
	done)
