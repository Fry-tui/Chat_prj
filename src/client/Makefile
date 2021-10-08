CC = gcc
OBJS = client.o menu.o msgops.o app.o fun.o ../common/command.o
TARGET = ../../bin/client

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ -lpthread
	
%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o *~
