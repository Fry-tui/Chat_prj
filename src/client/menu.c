/*
*********************************************************************************************************
*												  Menu
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
#include "../../include/client/menu.h"

/*
****************************************************************************************
*                                  主菜单响应函数
* @Desc   : 主菜单函数 开启新进程 启动display程序 
* @inet_ip: 把当前ip地址传给mainMenu 做登入前的zenity缓冲通道名
* @return : 无返回值
****************************************************************************************
*/
/*@[Warn]:消息队列的值本应该由服务器同意管理分配*/
void mainMenu()
{
	int i;
	char buf[1024]; 
	int value_num=2;
	char *value[value_num];
	Msg msg_send = {1,"none"}; /* 用于存放需要通过消息队列传递的消息 */
	
	//打开显示屏
	/* 为指针数组里的每个指针分配空间 */
	for(i=0;i<value_num;i++){
		value[i] = (char *)malloc(32);
	}
	/* 准备选项值 底*高+X+Y*/
	strcpy(value[0],"46x30+10+10");
	strcpy(value[1],cur_ip_text);
	//Step 1:打开显示屏,并尝试读取其进程号
	createDisplay(msg_key_text,value_num,value);
	//Step 2:打开|创建消息队列			执行成功则返回消息队列的标识符(非负整数)，否则返回-1
	msg_id = msgget(msg_key,0777|IPC_CREAT); /*msg_key:消息队列关联的键值 IPC_CREAT没有就创建及存取权限 */
	if(msg_id == -1){
		perror("[ \033[31mError\033[0m ] client/menu.c mainMenu(): msgget");
		killDisplay(msg_key_text);			/* 1:关闭对应的display进程(执行者:client) */
								/* 2:关闭socket号(执行者:server/reactMainMenu()/myRecv()) */
		exit(EXIT_FAILURE); 	/* 3:退出客户端(执行者:client) */
	}

	/* 把IP地址发送给客户端 */
	if(send(curSockfd,inet_ip_text,32,0)<0)
		perror("send");
	
	while(1){
		msg_send.choice=IMAINMENU;
		myMsgSend(msg_send); /* 发送页面选项 */
		printf("%s","\033[1H\033[2J"); 
		printf ("请选择："); 
		scanf("%s",buf);
		//发送选项 
		if(send(curSockfd,buf,1024,0)<0)
			perror("send");
		//判断选项
		if(strcmp(buf,"1")==0){
			clientRegister();
		}else if(strcmp(buf,"2")==0){
			clientLogin();
		}else if(strcmp(buf,"3")==0){
			//reSet(); 
		}else if(strcmp(buf,"4")==0){
			//myDes();
		}else if(strcmp(buf,"5")==0){	/* 退出客户端 */
			if(msg_id>=0)
				msgctl(msg_id,IPC_RMID,0); 
			killDisplay(msg_key_text);
			printf("%s","\033[1H\033[2J"); 
			exit(0);
		}else if(strcmp(buf,"ls")==0){	/* 罗列列表 */
			//等待结果
			if(recv(curSockfd,buf,1024,0)<0)
				perror("recv");
			if(strcmp(buf,"LIST_SUCCESS")==0){
				DPRINTF("罗列完毕(%s)\n",buf);
			}else{
				DPRINTF("罗列失败(%s)\n",buf);
			}
		}else{
			printf("\t输入有误,请重新输入\n"); 
			sleep(1);
		}
	}
	return;
}

void userMenu(void)
{
	int res;
	pthread_t id;
	char buf[1024],send_buf[1024];
	Msg msg_send = {1,"none"}; /* 用于存放需要通过消息队列传递的消息 */

	//printf("UserMenu:CD UserMenu\n");
	pthread_create(&id,NULL,(void *)pthread_Recv,NULL);
	//printf("UserMenu:Create pthread\n");

	while(1){
		//printf("Wait sem such as: 0|5\n");
		//接收验证消息|未读消息拼接字符
		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);
		//printf("Info:client/userMenu():recv msg_num:%s\n",buf);

		msg_send.choice=IUSERMENU;	/* 显示用户菜单 */
		strcpy(msg_send.text,buf);
		myMsgSend(msg_send); /* 发送页面选项 */
		
		//printf("add|read=%s\n",msg_send.text);
		
		printf("%s","\033[1H\033[2J"); 
		//等待选择
		printf ("请选择："); 
		scanf("%s",buf);
		
		strcpy(send_buf,"-");
		strcat(send_buf,buf);

		//将选项发送给线程
		if(send(curSockfd,send_buf,1024,0)<0)
			perror("send");
		
		//判断选项
		if(strcmp(buf,"1")==0){
			listFriends();
		}else if(strcmp(buf,"2")==0){
			//priChat();
		}else if(strcmp(buf,"3")==0){
			//pubChat();
		}else if(strcmp(buf,"4")==0){
			//tranAccount();
		}else if(strcmp(buf,"5")==0){	
			//topUp();
		}else if(strcmp(buf,"6")==0){
			//sendRedp();
		}else if(strcmp(buf,"7")==0){
			//grabRedp();
		}else if(strcmp(buf,"8")==0){
			addFriend();
		}else if(strcmp(buf,"9")==0){
			
		}else if(strcmp(buf,"10")==0){
			//inquireBalance();
		}else if(strcmp(buf,"11")==0){
			res = setPwd();
			if(res == SUCCESS){
				msg_send.choice=INULLMENU;
				strcpy(msg_send.text,"[ \033[32mWarn\033[0m ] 修改成功即将自动登出账户");
				myMsgSend(msg_send);
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
			listAddMsg();
			disposeAddMsg();
		}else if(strcmp(buf,"@")==0){
			//listUnreadMsg();
		}else if(strcmp(buf,"*")==0){
			system("zenity --info --text=刷新成功 --no-wrap --title=Refresh");
		}else if(strcmp(buf,"exit")==0){
			break;
		}else{
			printf("\t输入有误,请重新输入\n"); 
			sleep(1);
		}
	}

	//关闭接收线程
	pthread_cancel(id);
	return;
}

void rootMenu(void)
{
	int res;
	char buf[1024],send_buf[1024];
	Msg msg_send = {1,"none"}; /* 用于存放需要通过消息队列传递的消息 */

	while(1){
		msg_send.choice=IROOTMENU;	/* 显示管理员菜单 */
		myMsgSend(msg_send); /* 发送页面选项 */
		
		printf("%s","\033[1H\033[2J"); 
		//等待选择
		printf ("请选择："); 
		scanf("%s",buf);

		//将选项发送给线程
		if(send(curSockfd,buf,1024,0)<0)
			perror("send");
		
		//判断选项
		if(strcmp(buf,"1")==0){
			rmUser();
		}else if(strcmp(buf,"2")==0){
			offLineUser();
		}else if(strcmp(buf,"3")==0){
			//等待结果
			if(recv(curSockfd,buf,1024,0)<0)
				perror("recv");
			if(strcmp(buf,"LIST_SUCCESS")==0){
				DPRINTF("罗列完毕(%s)\n",buf);
			}else{
				DPRINTF("罗列失败(%s)\n",buf);
			}
		}else if(strcmp(buf,"4")==0){
			//bcAnnouncement();
		}else if(strcmp(buf,"5")==0){
			closeServer();
		}else if(strcmp(buf,"exit")==0){
			return;
		}else{
			printf("\t输入有误,请重新输入\n"); 
			sleep(1);
		}
	}
	return;
}