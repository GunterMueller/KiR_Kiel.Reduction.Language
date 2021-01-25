CC		= gcc

# Reductionunit definitions
RED_OBJ         = /home/batman/base/lib/lred.o

SXFED_OBJ       = /home/batman/base/lib/xeditor.o

SERVER_OBJ      = ../clt_srv/server-modul.o

LIBS            = -ltermcap

PROGNAME        = sxred

all: $(PROGNAME)

$(PROGNAME) : $(RED_OBJ) $(SERVER_OBJ) $(SXFED_OBJ)
	$(CC) -o $(PROGNAME) $(RED_OBJ) $(SERVER_OBJ) $(SXFED_OBJ) $(LIBS)

clean::
	rm -f $(PROGNAME)

depend::
