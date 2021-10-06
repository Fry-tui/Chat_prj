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
			listLinklist();
		}else if(strcmp(buf,"set")==0){
			//设置状态
			//reactSet(*sockfd);
		}
		
	}
	return;
}

