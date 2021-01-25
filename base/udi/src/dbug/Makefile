CC     = gcc
CFLAGS = -c

all : dbug.o

clean:
	rm -f dbug.o

install : all

depend :

dbug.o: dbug.c dbug.h
	$(CC) $(CFLAGS) dbug.c
