SVC_OBJS=svc_main.o
SIM_OBJS=sim_main.o
OBJS=$(SVC_OBJS) $(SIM_OBJS)
CC=gcc
LD=gcc
CFLAGS=-g -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -std=c11 -I../common -D_GNU_SOURCE
LDFLAGS=-lsysrepo
SVC_TARGET=service
SIM_TARGET=simulate-change
TARGETS=$(SVC_TARGET) $(SIM_TARGET)

build: svc sim

svc: $(SVC_OBJS)
	$(LD) -o $(SVC_TARGET) $(SVC_OBJS) $(LDFLAGS)

sim: $(SIM_OBJS)
	$(LD) -o $(SIM_TARGET) $(SIM_OBJS) $(LDFLAGS)

.c.o: $<
	$(CC) -c $< $(CFLAGS)

clean:
	$(RM) $(OBJS) $(TARGETS)

install-yang:
	sysrepoctl --install observer.yang