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

#define BUF_TEXT 512


/****************************************************************************************
**									消息队列
** msgget():创建消息队列--类比于open
**	 int msgget(key_t key, int msgflg);
** key：
**	 消息队列关联的标识符
** msgflg：消息队列的建立标志和存取权限
**	 IPC_CREAT:如果内核中没有此队列则创建它
**	 当IPC_EXCL和IPC_CREAT一起使用时，如果队列已经存在，则失败
** RETURN VALUE
** 	 执行成功则返回消息队列的标识符(a nonnegative integer-非负整数)，否则返回-1
** 注意：
**		key_t本质是整数，你自己弄个整数强制转化下就可以了，如(key_t)1234	 		
** msgsnd():消息队列的发送--类比于write
** 	 int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
** msqid : 消息队列的标识码(msgget返回的id)
** *msgp : 指向消息缓冲区的指针,需要用户自己定义一个结构体，系统没有提供(满足第一元素要大于0，第二个数字符数组)
** 		   这里是void*,要强制转换下，最好这样。
** msgsz : 消息的长短
** msgflg: 标志位
** RETURN VALUE
**   On success, zero is returned. On error, -1 is returned
** msgrcv():消息队列的接受--类比于read
** 	 ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,int msgflg);
** msqid : 消息队列的标识码(msgget返回的id)
** *msgp : 指向消息缓冲区的指针,需要用户自己定义一个结构体，系统没有提供(满足第一元素要大于0，第二个数字符数组)
** 		   这里是void*,要强制转换下，最好这样。
** msgsz : 消息的长短
** msgtyp: msgtyp等于0 ,则返回队列的最早的一个消息
** 		   msgtyp大于0，则返回其类型为mtype的第一个消息
** 		   msgtyp小于0,则返回其类型小于或等于mtype参数的绝对值的最小的一个消息
** msgflg: 标志位，如果为0，表示忽略该标志位
** RETURN VALUE
**   成功返回数据长度，错误返回-1
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
	long msg_type;       										//标志位
	char msg_text[BUF_TEXT];  									//发送数据缓冲区
};

int main(void)
{
	struct msg_st data;							
	int msg = -1;												//标识id
	int running = 1;
	char buffer[BUFSIZ];										//系统常量BUFSIZ是8192--定义在stdio.h中
	
	msg = msgget((key_t)1234, 0777|IPC_CREAT);					//IPC_CREAT没有就创建及存取权限s
	if(msg == -1)
	{
		fprintf(stderr, "The filure code is %d!!!\n",errno);	//errno--number of last error
		exit(EXIT_FAILURE);
	}
	
	while(running)
	{
		printf("Please enter data: \n");
		fgets(buffer, BUFSIZ, stdin);							// 标准输入向buffer获取BUFSIZ-1个字符
		data.msg_type = 1;										//标志置1，表示要通信了。其他数也可以
		stpcpy(data.msg_text, buffer);							//将buffer的数据复制到结构体的msg_text中
		
		if(msgsnd(msg, (void *)&data, BUF_TEXT, 0) == -1)		//发送数据到data结构体的数组中
		{
			fprintf(stderr, "magsnd error!!!\n");
			exit(EXIT_FAILURE);
		}
		if(strncmp(data.msg_text, "end", 3) == 0)				//判断字符串是否相等，自己弄个结束标志	
		{
			running = 0;										//推出while
		}
		sleep(1);
	}
	exit(EXIT_SUCCESS);	   
}
