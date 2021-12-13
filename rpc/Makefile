SVC_OBJS=svc_main.o
CLI_OBJS=cli_main.o
OBJS=$(SVC_OBJS) $(CLI_OBJS)
CC=gcc
LD=gcc
CFLAGS=-g -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -std=c11 -I../common -D_GNU_SOURCE
LDFLAGS=-lsysrepo -lyang
SVC_TARGET=service
CLI_TARGET=client
TARGETS=$(SVC_TARGET) $(CLI_TARGET)

build: svc cli

svc: $(SVC_OBJS)
	$(LD) -o $(SVC_TARGET) $(SVC_OBJS) $(LDFLAGS)

cli: $(CLI_OBJS)
	$(LD) -o $(CLI_TARGET) $(CLI_OBJS) $(LDFLAGS)

.c.o: $<
	$(CC) -c $< $(CFLAGS)

clean:
	$(RM) $(OBJS) $(TARGETS)

install-yang:
	sysrepoctl --install rpc.yang