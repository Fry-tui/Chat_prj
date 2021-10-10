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
	int res;
	char command[1024];
	char name[]="许玉泉";
	while(1){
		strcpy(command,"zenity --question --title=验证消息 --ok-label=同意 --cancel-label=拒绝 --text=");
		strcat(command,name);
		strcat(command,"请求添加你为好友");

		res = system(command);
		printf("res = %d\n",res);
		break;
	}
	return 0;
	
}
