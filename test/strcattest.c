# include <time.h>
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <signal.h> 
# include <unistd.h>
# include <pthread.h>
# include <sys/shm.h>
# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/stat.h>
# include <semaphore.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

int main()
{
	char str[1024];
	char buf[20]="he";
	char msg[20]="did";
	char arr_buf[20][10]={"123","456","789"};
	printf("str=%d\n",(int)sizeof(str));
	strcpy(str,buf);
	printf("str=%d\n",(int)sizeof(str));
	strcat(str,msg);
	printf("str=%d\n",(int)sizeof(str));
	printf("str=%s\n",str);
	strcat(str,arr_buf[0]);
	printf("str=%d\n",(int)sizeof(str));
	printf("str=%s\n",str);
	strcat(str,arr_buf[1]);
	printf("str=%d\n",(int)sizeof(str));
	printf("str=%s\n",str);
	return 0;	
}
