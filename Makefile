CXX = g++
CFLAGS = -O2 -Wall -g -pthread

OBJS = main.o Msr.o Cpuid.o Perf.o Rapl.o
COBJS = cstate.o Msr.o Cpuid.o Perf.o Rapl.o
SOBJS = sleep.o Msr.o Cpuid.o Perf.o Rapl.o

main:	${OBJS}
	${CXX} ${CFLAGS} -o $@ ${OBJS}

cstate:	${COBJS}
	${CXX} ${CFLAGS} -o $@ ${COBJS}

sleep:	${SOBJS}
	${CXX} ${CFLAGS} -o $@ ${SOBJS}

clean:
	rm -f *.o main cstate sleep

.cc.o:
	${CXX} ${CFLAGS} -c $<




