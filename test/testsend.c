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
	int i,j;
	for(i=0;i<9;i++){
		for(j=0;j<10;j++){
			printf("\033[3%dm\033[4%dm i=%d,j=%d \033[0m\n",i,j,i,j);
		}
	}
	return 0;
}	
