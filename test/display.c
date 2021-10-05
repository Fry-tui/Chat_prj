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


int main(int argc,char *argv[])
{	
	char *cmd;
	char buf[10];
	printf("pid = %d\n",getpid());
	system("grep \"in\" -nr ./* ");
	cmd = (char *)malloc(sizeof(char*));
	strcpy(cmd,"hello");
	printf("cmd = %s \n",cmd);
	printf("cmd = %d \n",(int)sizeof(cmd));
	printf("argv[2]=%s\n",argv[1]);
	while(1){
		printf("%s","\033[1H\033[2J"); 
		printf ("----------诚----------------\n");
		printf ("--------------毅------------\n");
		printf ("|\t1 名媛认证\t   |\n");	//注册 
		printf ("|\t2 名媛上线\t   |\n");	//登入 
		printf ("|\t3 重新认证\t   |\n");	//找回密码 
		printf ("|\t4 贬回凡人\t   |\n");	//注销账号 
		printf ("|\t5 名媛小憩\t   |\n");	//关闭窗口 
		printf ("|\t\t\t   |\n");
		printf ("----------名----------------\n");
		printf ("--------------媛------------\n");
		printf ("请选择："); 
		scanf("%s",buf);
	}
	return 0;
}
