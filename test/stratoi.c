#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
	char buf[1024] = "3 ";
	int i,len;
	while(1){
		scanf("%s",buf);
		len = strlen(buf);
		if(len>2)
			printf("违法len=%d\n",len);
		i = atoi(buf);
		printf("i=%d\n",i);
	}
	return 0;
}	
