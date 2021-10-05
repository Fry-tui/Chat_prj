#include <stdio.h>
#include <sys/stat.h>
//msgget
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
//exit头文件
#include <stdlib.h>
#include <unistd.h>
//errno头文件
#include <errno.h>
#include <string.h>

/****************************************************************************************
**									消息队列
** msgrcv():消息队列的接受--类比于read
** 	 ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,int msgflg);
** msqid : 消息队列的标识码(msgget返回的id)
** *msgp : 指向消息缓冲区的指针,需要用户自己定义一个结构体，系统没有提供(满足第一元素要大于0，第二个数字符数组)
** 		   这里是void*,要强制转换下，最好这样。
** msgsz : 消息的长短
** msgtyp: msgtyp等于0 ,则返回队列的最早的一个消息,这里给0
** 		   msgtyp大于0，则返回其类型为mtype的第一个消息
** 		   msgtyp小于0,则返回其类型小于或等于mtype参数的绝对值的最小的一个消息
** msgflg: 标志位，如果为0，表示忽略该标志位
** RETURN VALUE
**   On success, zero is returned. On error, -1 is returned
** msgctl():
**   int msgctl(int msqid, int cmd, struct msqid_ds *buf);
** msqid : 消息队列的标识码(msgget返回的id)
** cmd   : 
**   IPC_RMID: 立即断开连接
** RETURN VALUE
**   On success, IPC_STAT, IPC_SET, and IPC_RMID return 0.On error, -1 is returned
** fgets():从指定流中读出size-1个字符，若读入新行，则存在buffer中，最后要以NULL结尾
** 	 char *fgets(char *s, int size, FILE *stream);
** stream: 几种标准的流/文件句柄
** stpcpy(): 字符串的复制
** 	 char *stpcpy(char *dest, const char *src);
** strncmp(): 字符串比较
** 	 char *strncmp(char *dest, const char *src, size_t n);
****************************************************************************************/

//msgsnd/msgrcv 第二个参数
struct msg_st
{
	long msg_type;       													//标志位
	char msg_text[BUFSIZ];  												//系统常量BUFSIZ是8192--定义在stdio.h中
};


int main(void)
{
	struct msg_st data;
	int msg = -1;															//标识id
	int running = 1;
	char buffer[BUFSIZ];
	long int msgtype = 0;													//这里给0，返回队列的最早的一个消息
	
	//打开消息队列
	msg = msgget((key_t)1234, 0777|IPC_CREAT);								//IPC_CREAT没有就创建及存取权限s
	if(msg == -1)
	{
		fprintf(stderr, "The filure code is %d!!!\n",errno);				//errno--number of last error
		exit(EXIT_FAILURE);
	}
	
	//接受消息队列的数据---以end结束
	while(running)
	{
		if(msgrcv(msg, (void *)&data, BUFSIZ, msgtype, 0) == -1)			//将结构体中数据通过msgrcv接收  
		{
			fprintf(stderr, "magcve error code %d !!!\n",errno);
			exit(EXIT_FAILURE);
		}
		printf("You just wrote:\n%s\n",data.msg_text);
		
		if(strncmp(data.msg_text, "end", 3) == 0)							//以end结束	
		{
			running = 0;
		}
	}
	
	//断开消息队列的连接
	if(msgctl(msg, IPC_RMID, 0) == -1)										//msgctl删除消息队列
	{
		fprintf(stderr, "magctl error code %d !!!\n",errno);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);	
}
