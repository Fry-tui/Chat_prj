CC = gcc
OBJS = server.o fileops.o linklist.o msgops.o bufops.o menureact.o appserver.o ../common/command.o
TARGET = ../../bin/server

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ -lpthread
	
%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o *~
