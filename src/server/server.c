/*
*********************************************************************************************************
*												  Server
*
*                                         (c) Copyright 2021/09/30
*
* @File    : server.c
* @Author  : yqxu
*
* @Description:
* ----------
*  创建服务器,等待客户端的连接
*
* @History
*  Version :    Time    :  Author :  		Description
*   v1.0   : 2021-09-30 :   yqxu  :   创建服务器,等待客户端的连接
* 
*********************************************************************************************************
*/

#include "../../include/server/server.h"

/*
****************************************************************************************
*                                  主函数
*
* @Desc  : 实现socket套接字服务器的创建,等待客户端的连接,创建响应线程。
*
* @argc  : 参数个数
* @argv[]: 参数数组,argv[2]用来存放端口号
*
* @return: 默认返回0
****************************************************************************************
*/
int main(int argc, char * argv [ ])
{	
	int *p;		/*用于存放客户端id,并作为参数,传入响应线程*/
	pthread_t id;	/*线程id,动态分配线程*/
	int sockfd,addrlen,newSockfd;	/*sockfd:服务器id*/	/*newSockfd:新客户端id号*/
	/* 实例化地址结构体 */
	struct sockaddr_in my_addr,serv_addr;
	serv_addr.sin_port = atoi(argv[1]); /*取参数1 即传入的端口号*/
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(serv_addr.sin_zero,0,8);
     
    //初始化链表
	initLink();
	//读取本地文件
	readFile();
	
	printf("%s","\033[1H\033[2J"); /*清屏输出*/
    printf("***********************************\033[44m服务器启动\033[0m***********************************\n"); 

	sockfd = socket(AF_INET,SOCK_STREAM,0);	/*准备套接字*/
	if(sockfd<0){	
		/*判断是否创建成功*/
		perror("socket");
	}
	if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))<0){
		/*绑定*/
		perror("bind");
	}
	if(listen(sockfd,100)<0){
		/*监听*/
		perror("listen");
	}
	while(1){
		/*accept:阻塞直到有新客户端连接*/
		newSockfd = accept(sockfd,(struct sockaddr *)&my_addr,&addrlen);
		/*判断是否连接成功*/
		if(newSockfd<0){
            perror("accept");
		}
		/*提示连接成功*/
		printf("[ \033[34mInfo\033[0m ] 客户端%d",newSockfd);
	    printf(" \033[32m已开启连接\033[0m\n");
		/*为指针分配空间*/
		p = (int *)malloc(sizeof(int));
		/*存放客户端id号*/
		*p = newSockfd;
		/*创建线程函数响应菜单,并传入参数p*/
		pthread_create(&id,NULL,(void *)reactMainMenu,(void *)p); 
	}
	/*等待线程挂起{但实际逻辑不会执行到这里,所以是多此一举},但是代码规范嘛*/
	pthread_join(id,NULL); 
	return 0;
}
