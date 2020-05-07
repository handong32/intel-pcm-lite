CC = gcc
CXX = g++
CFLAGS = -O2 -Wall -g -pthread

OBJS = main.o Msr.o Cpuid.o Perf.o Rapl.o

main:	${OBJS}
	${CXX} ${CFLAGS} -o $@ ${OBJS}

clean:
	rm -f *.o main

.cc.o:
	${CC} ${CFLAGS} -c $<




