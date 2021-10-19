/*
*********************************************************************************************************
*												  Linklist
*
*                                        (c) Copyright 2021/09/30
*
* @File    : linklist.c
* @Author  : xuyq
*
* @Description:
* ----------
* 链表操作的相关函数,主要有链表的初始化，链表节点的添加，链表节点的更新，链表节点的增删查改
* 使用最方便的函数 reviseUserNode();		 
* 根据用户名或sockfd查找到链表里的用户节点,放回节点指针,可以通过指针直接修改节点的值.
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-09-30 :   yqxu  :   链表操作函数集合
* 
*********************************************************************************************************
*/
#include "../../include/server/linklist.h"

/*
****************************************************************************************
*                                  初始化链表
*
* @Desc  : 初始化链表,为头节点开辟空间,尾部指向NULL
* @Note	 : 读取文件前一定
* @return: 无返回值
****************************************************************************************
*/
void initLink(void)
{
	//为头节点开辟空间
	U = (LinklistU)malloc(sizeof(LnodeU));
	R = (LinklistR)malloc(sizeof(LnodeR));
	G = (LinklistG)malloc(sizeof(LnodeG));
	//让每个链表的尾部指向NULL 
	U->next = NULL;
	R->next = NULL; 
	G->next = NULL;
	return;
}

/*@[Warn]全写完之后需要验证addNode参数(枚举)*/
/*
****************************************************************************************
*                                  增加节点
*
* @Desc  : 把节点加入链表的尾部,因为我有三条链表所以根据不同的type去操作不同的链表
* @type  : 添加的节点类型				--value=[USER|REDP|Group]
* @user  : 添加的用户节点数据域
* @redp  : 添加的红包节点数据域
* @group : 添加的群组节点数据域
* @Note  : 如果添加的是user,那另外两个参数redp和group传个空结构体进来就行
* @return: 无返回值
****************************************************************************************
*/
void addNode(int type,struct User user,struct Redp redp,struct Group group)
{
	//准备遍历用的节点u,r,g	以及新增节点p,q,o;
	LinklistU u=U,p;	
	LinklistR r=R,q;	
	LinklistG g=G,o;
	
	//根据传入类型选择操作
	if(type == USER){
		//添加用户节点 
		while(u->next)	/* 遍历链表至链表尾部 */
			u = u->next;
		p = (LinklistU)malloc(sizeof(LnodeU)); /* 为新节点指针开辟空间 */
		p->user = user; /* 把数据存入指针的数据区 */
		u->next = p; /* 把新节点加入链表尾部 */
		p->next = NULL; /* 使链表尾部指向空 */
	}else if(type == REDP){
		//添加红包节点 
		while(r->next)
			r = r->next;
		q = (LinklistR)malloc(sizeof(LnodeR));
		q->redp = redp;
		r->next = q;
		q->next = NULL;
	}else if(type == GROUP){
		//添加群组节点 
		while(g->next)
			g = g->next;
		o = (LinklistG)malloc(sizeof(LnodeG));
		o->group = group;
		g->next = o;
		o->next = NULL;
	}else{
		//类型有误
		printf("[ \033[31mError\033[0m ] linklist.c addNode():无法识别要新增的节点类型\n");
	}
	return;
}

/*
****************************************************************************************
*                                改动用户节点
* @Desc  : 替换链表中原有的节点
* @user  : 在调用该函数前准备好新的用户节点,使用该函数替换链表中原有的节点
* @Note  : 基本没有用到,因为revise函数直接整合了modify和grep的功能
* @return: 修改结果
****************************************************************************************
*/
int modUserNode(struct User user)
{
	LinklistU u = U->next;
	while(u){
		if(strcmp(u->user.name,user.name)==0){
			/* 找到对应的节点后 替换数据区 */
			u->user = user;
			DPRINTF("[ \033[34mInfo\033[0m ] %s节点已替换\n",u->user.name);
			return SUCCESS;
		}
		u = u->next;
	}
	printf("[ \033[31mError\033[0m ] linklist.c modUserNode():%s节点未找到\n",user.name);
	return FAILD;
}
/*
****************************************************************************************
*                               查询用户节点
* @Desc  : 根据关键字查询对应节点,并返回
* @key   : 关键字类型				--value=[USERNAME|SOCKFD]
* @name  : 关键字:用户名
* @sockfd: 关键字:sockfd
* @return: 成功返回结构体(可能包含之前异常退出的结构体),失败返回不合格结构体 
* @Note  : 这个函数就是查询用户链表中有没有对应名字的节点或者有没有对应sockfd节点,
			当时写完以后，觉得这样还是不够高效率,所以写了revise
****************************************************************************************
*/
struct User grepUserNode(int key,char name[],int sockfd)
{
	struct User user;
	LinklistU u = U->next;
	if(key==USERNAME){ /* 匹配类型 */
		while(u){ /* 循环遍历 */
			if(strcmp(u->user.name,name)==0){
				/* 匹配成功,返回节内容 */
				return u->user;
			}
			u = u->next;
		}
		DPRINTF("[ \033[32mWarn\033[0m ] linklist.c grepUserNode():未找到节点\n");
	}else if(key==SOCKFD){
		while(u){
			if(u->user.sockfd==sockfd){
				return u->user;
			}
			u = u->next;
		}
		DPRINTF("[ \033[32mWarn\033[0m ] linklist.c grepUserNode():未找到节点\n");
	}else{
		printf("[ \033[31mError\033[0m ] linklist.c grepUserNode():无法识别关键字类型\n");
	}
	user.avail_flag = ILLEGAL;
	return user; /* 查询失败返回不合格结构体 */
}


/*
****************************************************************************************
*                               查询并返回节点地址
* @Desc  : 根据关键字查询对应节点,并返回节点地址
* @key   : 关键字类型--value=[USERNAME|SOCKFD]
* @name  : 关键字-用户名
* @sockfd: 关键字-sockfd
* @return: 成功返回节点地址,失败返回空指针
* @Note  : 一样的道理若是想用用户名查找节点只需要
			*user = reviseUserNode(USERNAME,"xyq",0);
			把第三个参数,sockfd给一个任意整数字就好
****************************************************************************************
*/
struct User * reviseUserNode(int key,char name[],int sockfd)
{
	LinklistU u = U->next;	/* 用于遍历的指针节点 */
	
	if(key==USERNAME){
		//判断关键字类型
		while(u){
			if(strcmp(u->user.name,name)==0){
				/* 匹配成功,返回地址,供调用者直接修改链表 */
				return &u->user;
			}
			u = u->next;
		}
		DPRINTF("[ \033[34mInfo\033[0m ] linklist.c reviseUserNode():无该节点\n");
	}else if(key==SOCKFD){
		//判断关键字类型
		while(u){
			if(u->user.sockfd==sockfd){
				return &u->user;
			}
			u = u->next;
		}
		DPRINTF("[ \033[34mInfo\033[0m ] linklist.c reviseUserNode():无该节点\n");
	}else{
		printf("[ \033[31mError\033[0m ] linklist.c reviseUserNode():无法识别关键字类型\n");
	}
	
	return NULL; /* 查询失败返回空指针 */
}

/*
****************************************************************************************
*                               查询并返回节点地址
* @Desc  : 根据关键字查询对应节点,并返回节点地址
* @key   : 关键字类型--value=[ONAME|GNAME]
* @owner_name	: 关键字-群主名
* @group_name	: 关键字-群名
* @return: 成功返回节点地址,失败返回空指针
****************************************************************************************
*/
struct Group * reviseGroupNode(int key,char owner_name[],char group_name[])
{
	LinklistG g = G->next;
	if(key==ONAME){
		while(g){
			if(strcmp(g->group.owner->name,owner_name)==0){
				return &g->group;
			}
			g = g->next;
		}
		DPRINTF("[ \033[34mInfo\033[0m ] linklist.c reviseGroupNode():无该节点\n");
	}else if(key==GNAME){
		while(g){
			if(strcmp(g->group.group_name,group_name)==0){
				return &g->group;
			}
			g = g->next;
		}
		DPRINTF("[ \033[34mInfo\033[0m ] linklist.c reviseGroupNode():无该节点\n");
	}else{
		printf("[ \033[31mError\033[0m ] linklist.c reviseGroupNode():无法识别关键字类型\n");
	}
	
	return NULL; /* 查询失败返回空指针 */
}

/*
****************************************************************************************
*                                删除用户节点
*
* @Desc  : 根据关键字删除链表节点
* @key   : 关键字类型					参数参考枚举Para_Key
* @user  : 关键字
* @sockfd: 关键字
* @return: 成功:SUCCESS 失败:FAILD
* @Note	 : 主要调用是在用户注销和删除用户的时候
****************************************************************************************
*/
/* @[Warn]:判断用户是否在线,在线需要强制下线|删除用户文件夹 */
int delUserNode(int key,char name[],int sockfd)
{	
	LinklistU p = U; 
	LinklistU u = U->next;
	
	if(key==USERNAME){ /* 判断供查找的关键字的类型 */
		while(u){
			if(strcmp(u->user.name,name)==0){
				p->next = u->next;
				free(u);
				DPRINTF("[ \033[34mInfo\033[0m ] %s节点已删除\n",name);
				return SUCCESS;
			}
			p = u;
			u = u->next;
		}
		printf("[ \033[31mError\033[0m ] linklist.c delUserNode():未找到节点\n");
	}else if(key==SOCKFD){
		while(u){
			if(u->user.sockfd==sockfd){
				p->next = u->next;
				free(u);
				DPRINTF("[ \033[34mInfo\033[0m ] %d节点已删除\n",sockfd);
				return SUCCESS;
			}
			p = u;
			u = u->next;
		}
		DPRINTF("[ \033[32mWarn\033[0m ] linklist.c delUserNode():未找到节点\n");
	}else{
		printf("[ \033[31mError\033[0m ] linklist.c delUserNode():无法识别关键字类型\n");
	}
	
	return FAILD;
}

/*
****************************************************************************************
*                                罗列用户链表
*
* @Desc  : 表单形式罗列节点
* @sockfd: 目前调用是由登入前客户端调用需要通信调用结果,所以需要传sockfd
* @return: 空
command示例:
	zenity --list --print-column=all --text=用户总数:2 --column=用户 --column=密码 --column=电话 --column=
	余额 --column=在线/离线 --column=群聊状态 --column=sockfd --column=msg_id --column=msg_key --column=
	进程号 --column=合法性 --column=上线时长 --column=好友数量 --column=验证消息 --column=未读消息 
	xyq 8926 18950238926 0.00 在线 打开 01 null null null 合法 1633514688 0 0 0 sxl 123 13459864587 0.00 离线 
	关闭 4 null null null 合法 0 0 0 0
****************************************************************************************
*/
void listLinklistU(int sockfd)
{
	int cnt,res;
	char buf[32];
	LinklistU u = U->next;

	/* 
	 * 以弹窗去显示所有的用户信息是需要一个很长的指令,由于指令需要拼接,所以要存在
	 * 一个字符串里面,但是局部变量的空间有限,所以我将他放在了全局变量,标准起名
	 * global_command
	 * 那由于是多线程操作,所以他可能会被抢占
	 * 这时候就需要对其上锁,我这里选择的上锁方式是使用信号量,阻塞被操作,
	 * 所以我定义了一个配套的信号量 global_sem_cmd
	 */
	
	sem_wait(&global_sem_cmd); /* 等待信号量被释放,获取global_sem_cmd的可执行权限 */
	
	cnt = cntUNode();	/* 计算用户节点数量,用于计算需要为cmd开辟的空间大小 */
	sprintf(buf,"%d",cnt);	/* 把整形转化成字符型,便于拼接到指令里,从而弹窗上可以显示当前用户数量 */

	//为存放指令的字符串分配空间,因为用户节点数的动态的,所以要根据用户的节点数动态分配空间
	global_command = (char *)calloc(cnt+1,sizeof(struct User)/128);

	/* 准备指令 */
	/* 多选:--multiple 可编辑:--editable */
	strcpy(global_command,"zenity --list --print-column=all --text=用户总数:");
	strcat(global_command,buf); /* 用户总数 */
	strcat(global_command," --column=用户 --column=密码 --column=电话 --column=余额");
	strcat(global_command," --column=在线/离线 --column=群聊状态 --column=sockfd --column=msg_id");
	strcat(global_command," --column=msg_key --column=进程号 --column=preact_id --column=precv_id --column=合法性");
	strcat(global_command," --column=上线时长 --column=好友数量 --column=验证消息 --column=未读消息 ");
	while(u){
		/* 拼接各类信息 */
		strcat(global_command,u->user.name);
		strcat(global_command," ");
	
		strcat(global_command,u->user.password);
		strcat(global_command," ");
		
		strcat(global_command,u->user.telenumber);
		strcat(global_command," ");
		
		sprintf(buf,"%.2f",u->user.balance);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		if(u->user.online_state==1)
			strcpy(buf,"在线");
		else
			strcpy(buf,"离线");
		strcat(global_command,buf);
		strcat(global_command," ");
		
		if(u->user.group_state==1)
			strcpy(buf,"打开");
		else
			strcpy(buf,"关闭");
		strcat(global_command,buf);
		strcat(global_command," ");
		
		if(u->user.sockfd < 0)
			strcpy(buf,"null");
		else
			sprintf(buf,"%d",u->user.sockfd);
		
		strcat(global_command,buf);
		strcat(global_command," ");
		
		strcat(global_command,u->user.msg_id_text);
		strcat(global_command," ");
		
		strcat(global_command,u->user.msg_key_text);
		strcat(global_command," ");
		
		strcat(global_command,u->user.login_pid);
		strcat(global_command," ");

		sprintf(buf,"%ld",u->user.preact_id);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		sprintf(buf,"%ld",u->user.precv_id);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		if(u->user.avail_flag==LEGAL)
			strcpy(buf,"合法");
		else
			strcpy(buf,"非法");
		strcat(global_command,buf);
		strcat(global_command," ");
		
		sprintf(buf,"%ld",u->user.duration);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		sprintf(buf,"%d",u->user.friend_num);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		sprintf(buf,"%d",u->user.add_num);
		strcat(global_command,buf);
		strcat(global_command," ");
		
		sprintf(buf,"%d",u->user.unread_msg_num);
		strcat(global_command,buf);
		strcat(global_command," ");
		u=u->next;
	}
	//DPRINTF("global_command=%s\n",global_command);

	//执行指令并获取返回值
	res = system(global_command);
	//释放信号量
	sem_post(&global_sem_cmd);
	//释放字符串
	free(global_command);
	
	if(res==0){
		if(send(sockfd,"LIST_SUCCESS",32,0)<0)
			perror("send");
	}else{ /* 256:FAILD */
		if(send(sockfd,"LIST_FAILD",32,0)<0)
			perror("send");
	}
	//printf("结束罗列\n");
	return;
}

/*
****************************************************************************************
*                                罗列群聊链表
*
* @Desc  : 表单形式罗列群聊节点,同上面一样的操作
* @return: 空
示例:
zenity --list --print-column=all --title=群聊列表 --text=群聊总数:3 --column=群聊名称 --column=群主 --
column=进群权限 --column=成员数量 --column=消息数量 吃瓜1群 许玉泉 无需验证 12 24 吃瓜2群 
吴洁铃 无需验证 12 33 嘘！小点声 吴洁铃 需验证 99+ 999+

****************************************************************************************
*/
void listLinklistG(void)
{
	int cnt,res;
	char buf[32];
	LinklistG g = G->next;

	sem_wait(&global_sem_cmd); /* 等待信号量被释放 */
	
	cnt = cntGNode();
	sprintf(buf,"%d",cnt);
	global_command = (char *)calloc(cnt+1,sizeof(struct Group)/24);
	/* 多选:--multiple 可编辑:--editable */
	strcpy(global_command,"zenity --list --print-column=all --title=群聊列表 --text=群聊总数:");
	strcat(global_command,buf); /* 群聊总数 */
	strcat(global_command," --column=群聊名称 --column=群主 --column=进群权限");
	strcat(global_command," --column=成员数量 --column=消息数量 ");
	//system(global_command);
	while(g){
		
		//system(global_command);
		
		strcat(global_command,g->group.group_name);
		strcat(global_command," ");
		
		//system(global_command);

		//printf("name:%s\n",g->group.owner->name);
		strcat(global_command,g->group.owner->name);
		strcat(global_command," ");

		
		//system(global_command);
		
		if(g->group.permit==1){
			strcat(global_command,"需验证 ");
		}else{
			strcat(global_command,"无需验证 ");
		}
		
		//system(global_command);
		
		sprintf(buf,"%d",g->group.mem_num);
		strcat(global_command,buf);
		strcat(global_command," ");

		
		//system(global_command);
		
		sprintf(buf,"%d",g->group.msg_num);
		strcat(global_command,buf);
		strcat(global_command," ");

		//system(global_command);
		g = g->next;
	}
	//DPRINTF("global_command=%s\n",global_command);
	res = system(global_command);
	sem_post(&global_sem_cmd);

	free(global_command);
	
	return;
}

/*
****************************************************************************************
*                  重新读取文件生成新链表后要去更新链表节点里的指针地址
* @Desc  : 当数据域里涉及了指针信息,进行储存到本地文件,重启服务器,读取文件后
			去访问新生成的链表里的某个节点里的指针成员会直接段错误,
			主要的原因是,指针存的是地址,储存在本地也是把指向的地址存进去,读取也是读取之
			存入的地址,但实际重启服务器以后对应地址的内容早已被释放,需要的地址也已经被
			更新,所以为了解决这个问题,就写了一个更新链表的函数.

			这个函数在服务器启动后只调用一次就是readFile()文件后,会把所有指针指向的地址
			进行更新,要想看懂函数,得先看懂三个结构体

* @return: 无返回值
* @Note	 : 这个函数的缺点就是开销大,如果要跑大型数据肯定是有问题的,如何优化可以思考一下
****************************************************************************************
*/
void updateLink(void)
{
	int i;
	LinklistU u = U->next;
	LinklistR r = R->next;
	LinklistG g = G->next;
	
	/* User 结构体没有用到指针,但是他的成员struct Friends结构体用到了所以也要更新 */
	while(u){	/* 去遍历链表里的每个节点 */
		
		for(i=0;i<u->user.friend_num;i++){	/* 遍历现有的好友结构体 */

			/* 根据好友的名字去查找好友用户节点的新地址 */
			u->user.friends[i].puser = reviseUserNode(USERNAME, u->user.friends[i].f_name, 0);
		}
		u = u->next;
	}

	while(r){
		/* 根据红包的主人的名字去查找红包主人的新用户节点地址 */
		r->redp.owner = reviseUserNode(USERNAME,r->redp.owner_name, 0);
		r = r->next;
	}
	
	/* 更新群组链表 */
	while(g){
		/* 根据群主的名字去获取群主新的用户节点地址 */
		g->group.owner = reviseUserNode(USERNAME, g->group.owner_name, 0);

		for(i=0;i<g->group.mem_num;i++){
			/* 遍历所有群成员,根据每个成员的名字去获取成员新用户节点的地址 */
			g->group.group_mem[i] = reviseUserNode(USERNAME, g->group.mem_name[i], 0);
		}
	
		g = g->next;
	}
	return;
}


/*
****************************************************************************************
*                                计算用户节点个数
*
* @Desc  : 遍历节点数
* @return: 节点数
****************************************************************************************
*/
int cntUNode(void)
{
	int cnt=0;
	LinklistU u = U->next;
	while(u){
		cnt++;
		u=u->next;
	}
	return cnt;
}

/*
****************************************************************************************
*                                计算群聊节点个数
*
* @Desc  : 遍历节点数
* @return: 节点数
****************************************************************************************
*/
int cntGNode(void)
{
	int cnt=0;
	LinklistG g = G->next;
	while(g){
		cnt++;
		g = g->next;
	}
	return cnt;
}


/****************************************************************************************
*                          清楚未读消息字符串数组里的空字符串
*
* @Desc  : 遍历每一个元素,如有空值查找下一个有效值,进行替换,直到有效值全部替换完毕
* @user	 : 需要清理未读消息的用户结构体指针
* @return: 无返回值
****************************************************************************************
*/
void clearUnreadMsg(struct User * user)
{
	int i,j,k;
	for(i=0;i<user->unread_msg_num;i++){
		/* 遍历每一条未读消息 */
		if(strcmp(user->unread_msg[i],"")==0){
			/* 如果这条未读消息被清空 */
			/* 寻找下一条有用的字符串 */
			for(k=i+1;k<user->unread_msg_num;k++){
				/* 若在结束前找到,则退出循环进行赋值操作 */
				if(strcmp(user->unread_msg[k],"")!=0)
					break;
			}
			/* 如果索引值大等于消息数量 说明有效信息已被遍历完 */
			if(k==user->unread_msg_num){
				user->unread_msg_num = i;
				return; 	/* 全部清除完毕 */
			}

			/* 如果没有结束,说明找到了新的有效信息,就进行赋值操作 */
			strcpy(user->unread_msg[i],user->unread_msg[k]);
			strcmp(user->unread_msg[k],""); /* 并清楚那条有效信息 */
		}
	}
	return;
}


