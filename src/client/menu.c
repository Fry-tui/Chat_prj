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
	Msg msg_send = {-1,"none"}; /* 用于存放需要通过消息队列传递的消息 */
	
	/* 为指针数组里的每个指针分配空间 */
	for(i=0;i<value_num;i++){
		value[i] = (char *)malloc(32);
	}
	/* 准备选项值 底*高+X+Y*/
	strcpy(value[0],"46x20+10+10");
	strcpy(value[1],cur_ip_text);
	//Step 1:打开显示屏,并尝试读取其进程号
	createDisplay(msg_key_text,value_num,value);
	//Step 2:打开|创建消息队列			执行成功则返回消息队列的标识符(非负整数)，否则返回-1
	msg_id = msgget(msg_key,0777|IPC_CREAT); /*msg_key:消息队列关联的键值 IPC_CREAT没有就创建及存取权限 */
	if(msg_key == -1){
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
		}else if(strcmp(buf,"ls")==0){	/* 退出客户端 */
			
		}else{
			printf("\t输入有误,请重新输入\n"); 
			sleep(1);
		}
	}
	return;
}

