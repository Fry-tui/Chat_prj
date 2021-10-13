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

struct Friend{
	struct User* puser; /* 指向具体的好友 */
	/*@[Warn]:用户注销等相关操作需要销毁所有相关好友的好友节点*/
	bool chat_state; /* 和该好友的聊天标志 [0]:关闭 [1]:打开*/
	int chat_len;	/* 聊天记录的数量 */
	char chat_msg[64][128]; /*与该好友的聊天记录*/
};


struct Group{
	char group_name[32];	/* 组名 */
	struct User * owner;	/* 群主指针 */
	bool permit;			/* 进群权限: 0:直接进 1:需验证 */
	struct User * group_mem[32];	/* 群成员指针数组,一个群上限32人 */
	char group_msg[64][218];	/* 群聊消息 :六十四条,每条最多218个字符*/
	int msg_num;	/* 群聊记录数量 */
	int mem_num;	/* 群成员数量 */
};

struct User{
	char name[32];	/* 用户名 */
	char password[32]; /* 密码 */
	char login_pid[16];	/* 进程号:获取客户端进程号,防止意外关闭 [默认"null"] */
	char msg_id_text[16]; /* 消息队列标识符:用于关闭消息队列 [默认"null"] 主要是myRecv需要使用 */
	char msg_key_text[16]; /* 消息队列关键字:用于杀死调用同一消息队列的进程 [默认"null"] */
	char inet_ip_text[16]; /* 存放登入的客户端的ip地址 用户buffer通信*/
	char telenumber[16]; /* 手机号 */

	int sockfd;	/* 登入后的socket号 [Default:-1] */
	int avail_flag; /* 判断结构体是否有效 0:有效结构体 -1[ILLEGAL]:无效数据|非法退出*/
	int add_num; /* 验证消息数量 [Default:0]*/
	int friend_num; /* 好友数量  [Default:0]*/
	int unread_msg_num; /* 未读消息数量 [Default:0]*/

	char add_name[32][32]; /* 验证消息由谁发来 */
	char add_msg[32][128]; /* 验证消息具体内容 */
	char unread_msg[128][128]; /* 未读消息 */
	
	bool group_state; /* 群聊状态 [Default:0]*/
	bool online_state; /* 在线状态 [Default:0] [在线:1]*/

	float balance; /* 余额 */

	/*	因为多线程同步运行,所以需要把消息上锁,判断好消息类型后,做对应的释放*/
	sem_t sem[32]; /* 信号量,同步处理接收到的消息 */
	char sem_buf[32][128]; /* 与信号量数组一一对应,存放接收到的消息 */
	/* @[Warn]:有些同类型的数据会过来多条,关系需要理清 */
	time_t login_t; /* 上线时间 */
	time_t duration; /* 在线时长 */
	pthread_t preact_id; /* 客户端的响应线程id [0]:无符号长整型*/
	pthread_t precv_id; /* 客户端登入后处理消息的id [0]:无符号长整型*/
	struct Friend friends[32]; /* 好友结构体数组 */
	
};

int main()
{	
	printf("%ld\n",sizeof(struct User)/128);
	return 0;
}	
