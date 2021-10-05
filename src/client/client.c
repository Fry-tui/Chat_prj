/*
*********************************************************************************************************
*												  Client
*
*                                         (c) Copyright 2021/09/30
*
* @File    : client.c
* @Author  : yqxu
*
* @Description:
* ----------
*  创建socket套接字的客户端并尝试连接服务器,作为客户端程序的入口代码
*
* @History
*  Version :    Time    :  Author :   Description
*   v1.0   : 2021-09-30 :   yqxu  :   创建客户端
* 
*********************************************************************************************************
*/

#include "../../include/client/client.h"

/*
****************************************************************************************
*                                  主函数
*
* @Desc  : 创建客户端
*
* @argc  : 参数个数
* @argv[]: 	argv[1]用来存放网络IP,
			argv[2]用来存放和服务端通信的端口号,
			argv[3]存放和显示屏通信的消息队列关键字
*
* @return: 默认返回0
****************************************************************************************
*/
int main(int argc, char * argv[])
{	
	int sockfd;
	/* 描述网络地址等信息 */
	struct sockaddr_in serv_addr;
	serv_addr.sin_port = atoi(argv[2]); /* 与服务器进行通信的端口号 */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	memset(serv_addr.sin_zero,0,8);

	strcpy(cur_ip_text,argv[1]); /* 获取当前客户端的ip地址 */
	strcpy(msg_key_text,argv[3]); /* 获取与显示屏进行通信的消息队列key的文本格式 */
	msg_key = (key_t)atoi(argv[3]); /* 将id转为文本格式 */
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);	/* 创建socket */

	if(sockfd<0){
		/* 判断是否创建成功 */
		perror("sockfd");
	}
	curSockfd = sockfd;	/*存放到全局变量中*/
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))<0){
		/*通过端口号 连接服务器*/
		perror("connect");
		return 0;
	}
	msg_id = -1;
	signal(SIGINT,sig_ctl);
	
	strcpy(inet_ip_text,argv[1]); /* 把当前ip地址存为全局变量 做登入前的zenity缓冲通道名*/
	
	mainMenu();	
	
	return 0;
}

/*
****************************************************************************************
*                                  信号控制函数
*
* @Desc  : 重写各sigle的作用
*
* @signum: 信号类型
* SIGINT:发送 [error]指令,关闭消息队列,结束显示屏
* @return: 默认返回0
****************************************************************************************
*/
void sig_ctl(int signum)
{
	if(signum == SIGINT){
		/* 关闭消息队列 */
		if(msg_id>=0)
			msgctl(msg_id,IPC_RMID,0); 
		killDisplay(msg_key_text);
		printf("%s","\033[1H\033[2J"); 
		system("zenity --warning --text=客户端意外终止 --no-wrap");
		exit(0);
	}else{
		system("zenity --warning --text=接收到未识别的信号 --no-wrap");
	}
}

