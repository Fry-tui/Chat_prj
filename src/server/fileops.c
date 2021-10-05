/*
*********************************************************************************************************
*											  File_operations
*
*                                         (c) Copyright 2021/09/30
*
* @File    : filsops.c
* @Author  : yqxu
*
* @Description:
* ----------
*  文件操作函数 包括数据的存取,文件的复制等
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-09-30 :   yqxu  :     文件操作函数
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
	if(type == USER){
		//保存用户数据 
		fd = open(USER_SRC,O_RDWR|O_CREAT); /* 打开文件 */
		while(u){	/*遍历链表*/
			write(fd,&u->user,sizeof(struct User)); /* 往结构体里写入一个节点大小的数据 */
			u = u->next;	/* 指针后移 */
		}
	}else if(type==REDP){
		//保存红包数据 
		fd = open(REDP_SRC,O_RDWR|O_CREAT);
		while(r){
			write(fd,&r->redp,sizeof(struct Redp));
			r = r->next;
		}
	}else{
		printf("\033[31m[Error]\033[0m fileops.c writefile():无法识别要保存的数据类型\n");
		close(fd);
		return;
	}
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
	FILE *fp;	/* 声明文件指针 */
	struct User user; /* 存读到的数据并加入新节点 */
	struct Redp redp;
	fp = fopen(USER_SRC,"r+b"); /* 打开文件 */
	if(fp!=NULL){
		while(fread(&user,sizeof(struct User),1,fp)==1){
			addNode(USER,user,redp);	/* 将数据存入链表 */
		}
		fclose(fp); /* 关闭文件指针 */
	}
	
	fp = fopen(REDP_SRC,"r+b"); /* 重新打开新文件 */
	if(fp!=NULL){
		while(fread(&redp,sizeof(struct Redp),1,fp)==1){
			addNode(REDP,user,redp);
		}
		fclose(fp);
	}
	
	return;
	
}

void sendFile(char source_src[],char target_src[])
{
	
}

