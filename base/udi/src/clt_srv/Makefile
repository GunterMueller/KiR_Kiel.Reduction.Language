CC=gcc
CFLAGS= -c #-DMK_LOGFILE

all: server-modul.o client-modul.o

clean:
	rm -f client server server-modul.o client-modul.o LOGFILE client.o server.o

depend:

install: all

test : client server

server: server-modul.o server.o
	$(CC) -o server server.o server-modul.o

server-modul.o: server-modul.c server-modul.h clt-srv.h
	$(CC) $(CFLAGS) server-modul.c

server.o: server.c server-modul.h clt-srv.h
	$(CC) $(CFLAGS) server.c

client: client-modul.o client.o
	$(CC) -o client client.o client-modul.o

client-modul.o: client-modul.c client-modul.h clt-srv.h
	$(CC) $(CFLAGS) client-modul.c

client.o: client.c client-modul.h clt-srv.h
	$(CC) $(CFLAGS) client.c
