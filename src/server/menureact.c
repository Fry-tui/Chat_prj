/*
*********************************************************************************************************
*												  ReactMenu
*
*                                         (c) Copyright 2021/10/02
*
* @File    : menureact.c
* @Author  : yqxu
*
* @Description:
* ----------
*  对客户端 普通用户的主菜单、用户菜单进行和管理用用户菜单 响应 
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-02 :   yqxu  :   处理客户端的菜单
* 
*********************************************************************************************************
*/
#include "../../include/server/menureact.h"

/*
****************************************************************************************
*                                  主菜单响应函数
* @Desc  : 响应主菜单函数
* @return: 无返回值
****************************************************************************************
*/
void reactMainMenu(int *sockfd)
{
	char buf[1024]; /* 存放从客户端接收到的数据 */
	char inet_ip[32]; /* 存放网络地址号 */
	pthread_detach(pthread_self());	/*修改线程资源等级,意外结束时自动释放资源*/
	
	/* 接收进程的IP地址,做登入前的zenity缓冲通道名 */
	strcpy(inet_ip,myRecv(*sockfd));

	while(1){
		//接收选项
		strcpy(buf,myRecv(*sockfd)); /* 阻塞接收客户端消息,若成功返回消息内容 */
									 /* 若失败myRecv()函数会下线客户端,并结束为这个客户端开设的线程,即reactMainMenu函数*/
		//判断选项 
		if(strcmp(buf,"1")==0){
			reactRegister(*sockfd,inet_ip);
		}else if(strcmp(buf,"2")==0){
			reactLogin(*sockfd,inet_ip);
		}else if(strcmp(buf,"3")==0){
			//reactSet();
		}else if(strcmp(buf,"4")==0){
			//reactDes(*sockfd); 
		}else if(strcmp(buf,"5")==0){
			//结束该客户端的响应进程
			close(*sockfd);
	        printf("[ \033[34mInfo\033[0m ] 客户端%d",*sockfd);
	        printf(" \033[33m已正常结束\033[0m\n");
			sleep(1);
	        pthread_exit(0);
		}else if(strcmp(buf,"ls")==0){
			listLinklistU(*sockfd);
		}else if(strcmp(buf,"set")==0){
			//设置状态
			//reactSet(*sockfd);
		}
		
	}
	return;
}

/*
****************************************************************************************
*                                  主菜单响应函数
* @Desc  : 响应用户菜单函数|进入这个函数开始所有的服务器接收都使用封装好的接收线程
* @*user : 指向具体用户的节点,可以通过这个结构体指针直接修改链表里节点的数据
* @Note	 : 
* @return: 无返回值
****************************************************************************************
*/
void reactUserMenu(struct User *user)
{
	int res;
	char buf[1024],send_text[1024];
	char add_num[8],unread_msg_num[8];
	pthread_t id;
	int detime = 10000000;
	
	//Step 1:创建接收线程,退出时销毁
	pthread_create(&id,NULL,(void *)pthread_Recv,(void *)user);
	user->preact_id = id;
	writeFile(USER);
	/* @[Warn]:之前这里做了一个信号量的清除操作,目前暂时用不到 */
	
	while(1){
		/* 封装 验证消息|未读消息 拼接字符 并发送 */
		sprintf(add_num,"%d",user->add_num);
		sprintf(unread_msg_num,"%d",user->unread_msg_num);
		strcpy(buf,add_num);
		strcat(buf,"|");
		strcat(buf,unread_msg_num);
		strcat(buf,"\0");
		strcpy(send_text,"-");
		strcat(send_text,buf);
		if(send(user->sockfd,send_text,1024,0)<0)
			perror("send");
		//printf("add|read=%s\n",buf);
		
		//获取线程接收到的普通数据
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);
		
		//判断选项
		if(strcmp(buf,"1")==0){
			listFriends(user);
			sem_wait(&user->sem[0]);
		}else if(strcmp(buf,"2")==0){
			priChat(user);
		}else if(strcmp(buf,"3")==0){
			createGroup(user);
		}else if(strcmp(buf,"4")==0){
			listGroups(user);
			joinGroup(user);
		}else if(strcmp(buf,"5")==0){
			listMyGroups(user);
			groupChat(user);
		}else if(strcmp(buf,"6")==0){
			//sendRedp();
		}else if(strcmp(buf,"7")==0){
			//grabRedp();
		}else if(strcmp(buf,"8")==0){
			addFriend(user);
			detime = 10000000;
			while(detime--);	/* 延时0.1s */
		}else if(strcmp(buf,"9")==0){
			
		}else if(strcmp(buf,"10")==0){
			//inquireBalance();
		}else if(strcmp(buf,"11")==0){
			res = setPwd(user);
			if(res==SUCCESS){
				DPRINTF("[ \033[34mInfo\033[0m ] 用户:%s修改密码成功,自动登出就绪\n",user->name);
				pthread_cancel(user->preact_id);
				break;
			}
		}else if(strcmp(buf,"12")==0){
			//delFriend();
		}else if(strcmp(buf,"13")==0){
			//groChat();
		}else if(strcmp(buf,"14")==0){
			//sendFile();
		}else if(strcmp(buf,"15")==0){
			//cancelUser(user);
		}else if(strcmp(buf,"#")==0){
			listAddMsg(user);
			disposeAddMsg(user);
		}else if(strcmp(buf,"@")==0){
			//listUnreadMsg();
		}else if(strcmp(buf,"*")==0){
			/* 不执行操作 */
		}else if(strcmp(buf,"exit")==0){
			pthread_cancel(user->preact_id);
			break;
		}else{
			printf("[ \033[32mWarn\033[0m ] menureact.c reactUserMenu():无法识别:%s\n",buf);
			sleep(1);
		}
		//inspectRedp();	/* 检测红包效期函数 */
	}

	pthread_cancel(id);
	return;
}

/*
****************************************************************************************
*                                  管理员菜单
* @Desc  : 
* @*user : 
* @Note	 : 
* @return: 无返回值
****************************************************************************************
*/
void reactRootMenu(int sockfd,char inet_ip[])
{
	int res;
	char buf[1024],send_text[1024];
	int detime = 10000000;
	
	while(1){
		//printf("re recv\n");
		//接收选项
		strcpy(buf,myRecv(sockfd));
		//printf("recive over!%s\n",buf);
		//判断选项
		if(strcmp(buf,"1")==0){
			rmUser(sockfd,inet_ip);
		}else if(strcmp(buf,"2")==0){
			offLineUser(sockfd,inet_ip);
		}else if(strcmp(buf,"3")==0){
			listLinklistU(sockfd);
			//printf("exit function\n");
		}else if(strcmp(buf,"4")==0){
			//bcAnnouncement();
		}else if(strcmp(buf,"5")==0){
			listLinklistG();
			if(send(sockfd,"end_list",32,0)<0)
				perror("send");
		}else if(strcmp(buf,"6")==0){
			closeServer(sockfd);
		}else if(strcmp(buf,"exit")==0){
			return;
		}else{
			printf("[ \033[32mWarn\033[0m ] menureact.c reactRootMenu():无法识别:%s\n",buf);
			sleep(1);
		}
	}

	return;
}

