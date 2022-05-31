CC = gcc
CFLAGS = -Wall
PROGRAMS = client server

all: ${PROGRAMS}

client: client.c data.c data.h
	${CC} ${CFLAGS} -o client client.c data.c data.h

server: server.c data.c data.h
	${CC} ${CFLAGS} -o server server.c data.c data.h

clean:
	rm -f ${PROGRAMS}