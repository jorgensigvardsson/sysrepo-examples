OBJS=main.o
CC=gcc
LD=gcc
CFLAGS=-g -Wall -Wextra -std=c11 -I../common
LDFLAGS=-lsysrepo
TARGET=set-get-config

build: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

.c.o: $<
	$(CC) -c $< $(CFLAGS)

clean:
	$(RM) $(OBJS) $(TARGET)

install-yang:
	sysrepoctl --install set-get-config.yang