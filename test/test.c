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

struct User{
	int i;
	char buf;
};

int main()
{
	int i;
	struct User user[3];
	struct User *fuser;
	char arr[3][4]={
		"123",
		"123",
		"456"
	};
	char *buf;
	
	buf = (char *)malloc(20);
	fuser = (struct User *)malloc(20);
	
	fuser = &user[1];
	
	strcpy(buf,arr[2]);
	
	for(i=0;i<3;i++){
		//printf("arr[i]=%s\n",arr[i]);
	}	
	
	fuser->i = 6;
	
	printf("%d\n",user[1].i);
	
	return 0;
}	
