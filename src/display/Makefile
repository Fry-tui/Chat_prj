CC = gcc
OBJS = display.o msgrecv.o
TARGET = ../../bin/display

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ -lpthread
	
%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm -rf *.o *~
