CC = gcc
CFLAGS = -Wall
PROGRAMS = prog
TARGET = main client server data

prog: ${TARGET}
	${CC} ${CFLAGS} -o prog

clean:
	rm -f ${PROGRAMS}
