/*
*********************************************************************************************************
*												  Linklist
*
*                                         (c) Copyright 2021/09/30
*
* @File    : linklist.c
* @Author  : yqxu
*
* @Description:
* ----------
*  链表操作的相关函数,包括增删查改等
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
* @Desc  : 初始化两条链表,开辟头节点,尾部指向NULL
* @return: 无返回值
****************************************************************************************
*/
void initLink(void)
{
	//为头节点开辟空间
	U = (LinklistU)malloc(sizeof(LnodeU));
	R = (LinklistR)malloc(sizeof(LnodeR));
	//让每个链表的尾部指向NULL 
	U->next = NULL;
	R->next = NULL; 
	return;
}
/*@[Warn]全写完之后需要验证addNode参数(枚举)*/
/*
****************************************************************************************
*                                  增加节点
*
* @Desc  : 向链表中加入节点
* @type  : 添加的节点类型				--value=[USER|REDP]
* @user  : 添加的节点内容
* @redp  : 添加的节点内容
* @Note  : 如果添加的是user,那个redp参数一般是一个空结构体
* @return: 默认返回0
****************************************************************************************
*/
void addNode(int type,struct User user,struct Redp redp)
{
	//准备遍历用的节点u,r	以及新增节点p,q
	LinklistU u=U,p;	
	LinklistR r=R,q;
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
	}else{
		//类型有误 
		printf("[ \033[31mError\033[0m ] linklist.c addNode():无法识别要新增的节点类型\n");
	}
	return;
}

/*
****************************************************************************************
*                                改动用户节点
* @Desc  : 修改链表中原有的节点
* @user  : 修改对象以及,修改内容
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
* @name  : 关键字
* @sockfd: 关键字
* @return: 成功返回结构体,失败返回不合格结构体
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
* @Desc  : 根据关键字查询对应节点,并返回节点指针
* @key   : 关键字类型--value=[USERNAME|SOCKFD]
* @name  : 关键字
* @sockfd: 关键字
* @return: 成功返回节点地址,失败返回空指针
****************************************************************************************
*/
struct User * reviseUserNode(int key,char name[],int sockfd)
{
	LinklistU u = U->next;
	if(key==USERNAME){
		while(u){
			if(strcmp(u->user.name,name)==0){
				/* 匹配成功,返回地址,供调用者直接修改链表 */
				return &u->user;
			}
			u = u->next;
		}
		DPRINTF("[ \033[34mInfo\033[0m ] linklist.c reviseUserNode():无该节点\n");
	}else if(key==SOCKFD){
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
*                                删除用户节点
*
* @Desc  : 根据关键字删除链表节点
* @key   : 关键字类型					参数参考枚举Para_Key
* @user  : 关键字
* @sockfd: 关键字
* @return: 成功:SUCCESS 失败:FAILD
****************************************************************************************
*/
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

void listLinklist(void)
{
	LinklistU u = U->next;
	while(u){
		printf("name:%s|pwd:%s|pid:%s|\nmsg_id:%s|msg_key:%s|tel:%s\n",u->user.name,u->user.password,u->user.login_pid,u->user.msg_id_text,u->user.msg_key_text,u->user.telenumber);
		printf("fd:%d|add_n=%d|gps:%d|online:%d\n",u->user.sockfd,u->user.add_num,u->user.group_state,u->user.online_state);
		printf("money:%f\n",u->user.balance);
		u=u->next;
	}
	return;
}
