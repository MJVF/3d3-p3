OBJC=$(wildcard client/*.c)
OBJS=$(wildcard server/*.c)

CC=gcc

CFLAGS= -Wall -g

LDFLAGS= -lm

TARGETC=client/client
TARGETS=server/server

all : $(OBJC) $(OBJS)
	$(CC) $(CFLAGS) $(OBJC) -o $(TARGETC) $(LDFLAGS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGETS) $(LDFLAGS)