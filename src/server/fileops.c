/*
*********************************************************************************************************
*											   File_operations
*
*                                         (c) Copyright 2021/09/30
*
* @File    : filsops.c
* @Author  : xuyq
*
* @Description:
* ----------
*  文件操作函数 包括数据的存取
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-09-30 :   xuyq  :     文件操作函数
* 
*********************************************************************************************************
*/

#include "../../include/server/fileops.h"

/*
****************************************************************************************
*                                  写文件
*
* @Desc  : 将用户链表和红包链表按顺序写入数据文件
* @type  : 根据传入的参数决定保存何种数据 'USER:保存用户数据' 'REDP:保存红包数据' 
* @return: 无返回值
****************************************************************************************
*/
void writeFile(int type)
{
	int fd; /* 创建文件描述符 */
	LinklistU u = U->next; /* 创建用户节点指针 */
	LinklistR r = R->next; /* 创建红包节点指针 */
	LinklistG g = G->next; /* 创建群聊节点指针 */
	
	if(type == USER){
		//保存用户数据 
		fd = open(USER_SRC,O_RDWR|O_CREAT|O_TRUNC,0777); /* 打开文件 */
		while(u){	/*遍历链表*/
			write(fd,&u->user,sizeof(struct User)); /* 往结构体里写入一个节点大小的数据 */
			u = u->next;	/* 指针后移 */
		}
	}else if(type==REDP){
		//保存红包数据 
		fd = open(REDP_SRC,O_RDWR|O_CREAT|O_TRUNC,0777);
		while(r){
			write(fd,&r->redp,sizeof(struct Redp));
			r = r->next;
		}
	}else if(type==GROUP){
		//保存红包数据 
		fd = open(GROUP_SRC,O_RDWR|O_CREAT|O_TRUNC,0777);
		while(g){
			write(fd,&g->group,sizeof(struct Group));
			g = g->next;
		}
	}else{
		printf("\033[31m[Error]\033[0m fileops.c writefile():无法识别要保存的数据类型\n");
		close(fd);
		return;
	}
	//printf("type=%d\n",type);
	close(fd); /* 关闭文件指针 */
	return;
}

/*
****************************************************************************************
*                                  读文件
*
* @Desc  : 将用户链表和红包链表按顺序写入数据文件
* @return: 无返回值
****************************************************************************************
*/
void readFile(void)
{
	int fd; /* 创建文件描述符 */
	struct User user; /* 存读到的数据并加入新节点 */
	struct Redp redp;
	struct Group group;
	fd = open(USER_SRC, O_RDONLY); /* 打开文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()U:已打开\n");
		while( read(fd,&user,sizeof(struct User))> 0){
			addNode(USER,user,redp,group);	/* 将数据存入链表 */
		}
		close(fd); /* 关闭文件指针 */
	}
	
	fd = open(REDP_SRC,O_RDONLY); /* 重新打开新文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()R:已打开\n");
		while(read(fd,&redp,sizeof(struct Redp))> 0){
			addNode(REDP,user,redp,group);
		}
		close(fd);
	}
	
	fd = open(GROUP_SRC,O_RDONLY); /* 重新打开新文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()G:已打开\n");
		while(read(fd,&group,sizeof(struct Group))> 0){
			addNode(GROUP,user,redp,group);
		}
		close(fd);
	}
	
	return;
	
}


