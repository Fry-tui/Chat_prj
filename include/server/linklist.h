#ifndef __LINKLIST_H_
#define __LINKLIST_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "server.h"

/*************************************
 *      		STRUCT				*
 ************************************/
/**
 * struct Para
 * @id: Thread ID, used to close the thread
 * @name: current user name
 */
struct Para{
	//参数结构体:解决线程创建时只能传一个参数的问题
	pthread_t id;
	char name[32];
};

/**
 * struct Buffer
 * @avail_flag: 结构体有效与否
 * @name: 名字类
 * @pwd : 密码类
 * @psd : 确认密码类
 */
struct Buffer{
	int avail_flag; /* 判断结构体是否有效 0[LEGAL]:有效结构体 -1[ILLEGAL]:无效数据*/
	char name[32];
	char pwd[32];
	char psd[32]; 
	char text[32];
};

/**
 * struct Friend
 * @puser: Point to the friend structure
 * @chat_len: The number of chat messages
 * @chat_msg: The content of chat messages
 */
struct Friend{
	char f_name[32];	/* 好友名字 */
	struct User* puser; /* 指向具体的好友 */
	/*@[Warn]:用户注销等相关操作需要销毁所有相关好友的好友节点*/
	bool chat_state; /* 和该好友的聊天标志 [0]:关闭 [1]:打开*/
	int chat_len;	/* 聊天记录的数量 */
	char chat_msg[64][128]; /*与该好友的聊天记录*/
};

/**
 * struct User : Note the default values in parentheses
 * @name: username
 * @password: password
 * @login_pid: Process ID of the client that the user logs in to
 * @telenumber: Telephone number , Only 11 digits
 * @sockfd: sockfd for communication  [-1]
 * @add_num: Verify the number of messages [0]
 * @friend_num: Number of friends [0]
 * @unread_msg_num: Number of unread messages
 * @add_name: Verify the sender of the message
 * @add_msg: Verify the content of the message
 * @unread_msg: Unread message records
 * @group_state: Group chat flag bit  [0]
 * @online_state:Online Status bit [0]
 * @balance: balance of account
 * @sem: Synchronous processing message
 * @login_t: online time [0]
 * @duration: the time that sb. has spent online
 * @friends: Array of friend structures
 */
struct User{
	char name[32];	/* 用户名 */
	char password[32]; /* 密码 */
	char login_pid[16];	/* 进程号:获取客户端进程号,防止意外关闭 [默认"null"] */
	char msg_id_text[16]; /* 消息队列标识符:用于关闭消息队列 [默认"null"] 主要是myRecv需要使用 */
	char msg_key_text[16]; /* 消息队列关键字:用于杀死调用同一消息队列的进程 [默认"null"] */
	char inet_ip_text[16]; /* 存放登入的客户端的ip地址 用户buffer通信*/
	char telenumber[16]; /* 手机号 */

	int sockfd;	/* 登入后的socket号 [Default:-1] */
	int udp_sockfd;	/* 用于udp通信的sock号 */
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

	struct sockaddr_in server;
	struct sockaddr_in client;
	
};

/*@[Warn]:设想服务器专门创建一个线程检测红包的效期*/
/**
 * struct Redp
 * @split_num: Total number of splits set by the user
 * @grab_num: The number of people that have been robbed
 * @bless: The text of the blessing of the red envelope
 * @grab_name: An array of user names that grabbed the red envelope
 * @gen_time_text: The time text when the red envelope is generated
 * @balance: Remaining amount of red envelope
 * @gen_time: The time to generate the red envelope
 */
struct Redp{
	char owner_name[32];
	struct User * owner;
	
	int split_num; /* 拆分数量 */
	int grab_num; /* 已抢数量 */
	
	char bless[32]; /* 红包祝语 */
	char grab_name[32][32]; /* 已抢占用户数组 */
	char gen_time_text[32]; /* 生成红包的时间文本格式 */

	float balance; /* 剩余余额 */

	time_t gen_time;	/* 生成时间 */
};


/**
 * struct Group
 
 * @ group_name		: The name of a unique group
 * @owner			: Points to the owner that created the group
 * @permit			: Group access switch
 * @group_mem		: An array of Pointers to group members
 * @group_msg		: Store group chat messages
 */
struct Group{
	char group_name[32];	/* 组名 */
	char owner_name[32];	/* 群主名字 */
	struct User * owner;	/* 群主指针 */
	bool permit;			/* 进群权限: 0:直接进 1:需验证 */

	bool g_state[32];		/* 0:关闭|1:打开 */
	char mem_name[32][32];			/* 群成员名字 */
	struct User * group_mem[32];	/* 群成员指针数组,一个群上限32人 */
	char group_msg[64][218];	/* 群聊消息 :六十四条,每条最多218个字符*/
	int msg_num;	/* 群聊记录数量 */
	int mem_num;	/* 群成员数量 */
};


/*************************************
 *      	   Linklist				 *
 ************************************/
typedef struct LnodeU{
	struct User user;
	struct LnodeU *next;
}LnodeU,*LinklistU;

typedef struct LnodeR{
	struct Redp redp;
	struct LnodeR *next;
}LnodeR,*LinklistR;

typedef struct LnodeG{
	struct Group group;
	struct LnodeG *next;
}LnodeG,*LinklistG;

/*************************************
 *         GLOBAL PROTOTYPES		 *
 ************************************/
LinklistU U;	//名媛链表头节点 
LinklistR R;	//红包链表头节点 
LinklistG G;	//群聊链表头节点 

struct User * init_user_link;
struct Redp * init_redp_link;
struct Group * init_group_link;


int cntUNode(void);
int cntGNode(void);
int modUserNode(struct User);
int delUserNode(int,char[],int);

void initLink(void);
void listLinklistU(int);
void listLinklistG(void);

void clearUnreadMsg(struct User * user);
void addNode(int,struct User,struct Redp,struct Group);

struct User grepUserNode(int,char[],int);
struct User * reviseUserNode(int,char[],int);
struct Group * reviseGroupNode(int,char[],char[]);





#endif
