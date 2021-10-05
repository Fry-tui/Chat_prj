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
	int i = system("zenity --forms --title=注册 --add-entry=用户名 --add-password=密码 --add-password=确认密码 --text=请输入");
	printf("%d\n",i);
	if(i==0){//输入完毕
		
	}else{i==256 中断输入
	
	}
	return 0;
}	
