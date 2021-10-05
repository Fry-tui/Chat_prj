CC = gcc
OBJS = server.o fileops.o linklist.o msghandle.o menureact.o ../common/command.o
TARGET = ../../bin/server

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ -lpthread
	
%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o *~
