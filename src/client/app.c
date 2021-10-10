/*
*********************************************************************************************************
*												  APPLICATION
*
*                                         (c) Copyright 2021/10/06
*
* @File    : app.c
* @Author  : yqxu
*
* @Description:
* ----------
*  应用程序的集合
*
* @History
*  Version :    Time    :  Author :   Description
*   v1.0   : 2021-10-06 :   yqxu  :   创建各类应用程序
* 
*********************************************************************************************************
*/

#include "../../include/client/app.h"


/*
****************************************************************************************
*                                  注册函数
* @Desc  : 进入注册 
* @return: 无返回值
****************************************************************************************
*/
void clientRegister(void)
{
	int i,res;
	Msg msg_send = {1,"none"};
	char buf[32],code[8],name[32],number[32],command[256];
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------Register-----------\n");
	myMsgSend(msg_send);

	//手机号
	while(1){
		printf("输入(手机号):");
		scanf("%s",number);

		//判断位数 
		if(strlen(number)==11){ 
			if(send(curSockfd,number,32,0)<0)
				perror("send");
			
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 手机号 \033[32m✔\033[0m\n");
			myMsgSend(msg_send);
			
			break;
		}
		
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 手机号 \033[31mX\033[0m\n");
		myMsgSend(msg_send);
		system("zenity --error --text=\"手机号有误\" --no-wrap --title=注册");
		
	}

	//验证码
	while(1){
		//接收号码
		if(recv(curSockfd,code,8,0)<0)
			perror("recv");

		infoCrtCode(code);//弹出验证码
		printf("输入(验证码):");
		scanf("%s",buf);
		if(strcmp(buf,code)==0){ 
			if(send(curSockfd,"same",32,0)<0)
				perror("send");
			
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 验证码 \033[32m✔\033[0m\n");
			myMsgSend(msg_send);
			
			break;
		}
		
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 验证码 \033[31mX\033[0m\n");
		myMsgSend(msg_send);
		system("zenity --error --text=\"验证码有误\" --no-wrap --title=注册");
		
		if(send(curSockfd,"error",32,0)<0)
			perror("send");
		sleep(1);//休眠两秒
	} 

	//用户名&密码
	while(1){
		/* > ./data/ipbuffer/inet_ip_text_form */
		strcpy(command,"zenity --forms --text=注册 --add-entry=用户名 --add-password=密码 --add-password=确认密码 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */
		
		sprintf(buf,"%d",res);
		if(send(curSockfd,buf,32,0)<0)
			perror("send");

		if(res == 0){ /*成功 */
			//等待服务器读取数据结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
			if(strcmp(buf,"ILLEGAL")==0){	/* 没有读到消息 */
				strcpy(msg_send.text,"\033[31m[Error]\033[0m menu.c register():表单数据缓冲失败,即将终止注册函数\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=注册");
				continue;
			}
			
			//等待服务器读取判断结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
			if(strcmp(buf,"pwd")==0){	/* 接收到pwd代表密码不一致 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 密码 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"密码不一致\" --no-wrap --title=注册");
				continue;
			}else if(strcmp(buf,"name")==0){ /* 接收到name代表已占用 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 名媛名 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"用户名已被占用\" --no-wrap --title=注册");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=注册");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户名 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 密码  \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				break;
			}else{ /*代表接收到了其他send,报错终止注册退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m menu.c register():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止注册函数\n");
				myMsgSend(msg_send);
				//同时告诉服务器退出响应注册函数
				if(send(curSockfd,"recv_error",32,0)<0)
					perror("send");
				return;
			}
		}else{
			//失败:256
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 注册程序 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return;
		}	
	}

	
	//等待注册结果
	if(recv(curSockfd,buf,32,0)<0)
		perror("recv");
	//printf("buf = %s\n",buf);
	if(strcmp(buf,"reg_success")==0){
		if(recv(curSockfd,name,32,0)<0)
			perror("recv");
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户\033[36m");
		strcat(msg_send.text,name);
		strcat(msg_send.text,"\033[0m \033[32m注册成功\033[0m\n");
		myMsgSend(msg_send);
		system("zenity --info --text=\"注册成功\" --no-wrap --title=注册");
	}else{
		system("zenity --warning --text=\"注册中断\" --no-wrap --title=注册");
		return;
	}
	return;
}

/*
****************************************************************************************
*                                  登入函数
* @Desc  : 进入注册 
* @return: 无返回值
****************************************************************************************
*/
void clientLogin(void)
{
	pid_t pid;
	int res;
	char buf[32],name[32],command[256];
	Msg msg_send = {1,"none"};
	int detime = 100000000;//用与延时,耗费0.1s

	/* 进行显示屏清空输出 */
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------Login-----------\n");
	myMsgSend(msg_send);
	
	while(1){
		/* 打开表单弹窗 */
		strcpy(command,"zenity --forms --text=登入 --add-entry=用户名 --add-password=密码 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */
		/* 发送表单处理结果 */
		sprintf(buf,"%d",res);
		if(send(curSockfd,buf,32,0)<0)
			perror("send");
			
		if(res==0){
			//等待服务器读取数据结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
			if(strcmp(buf,"ILLEGAL")==0){	/* 没有读到消息 */
				strcpy(msg_send.text,"\033[31m[Error]\033[0m menu.c login():表单数据缓冲失败,即将终止登入函数\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=登入");
				continue;
			}
			
			//等待服务器读取判断结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");

			if(strcmp(buf,"pwd")==0){	/* 接收到pwd代表密码不一致 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 密码 \033[31m有误\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"密码不正确\" --no-wrap --title=登入");
				continue;
			}else if(strcmp(buf,"name")==0){ /* 接收到name代表未注册 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 该用户 \033[31m未注册\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"用户不存在\" --no-wrap --title=登入");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=登入");
				continue;
			}else if(strcmp(buf,"on_line")==0){ /* 接收到on_line代表已在线 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 该用户 \033[31m已在其他设备登入\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=该用户已在其他设备登入,如不是您本人的操作,请尽快找回密码 --no-wrap --title=登入");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户名 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 密码  \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				break;
			}else if(strcmp(buf,"root_login")==0){
				rootMenu();
				return;
			}else{ /*代表接收到了其他send,报错终止登入退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m menu.c login():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止登入函数\n");
				myMsgSend(msg_send);
				//同时告诉服务器退出响应注册函数
				if(send(curSockfd,"recv_error",32,0)<0)
					perror("send");
				return;
			}
		}else{ /* 窗口点击取消或是关闭 */
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 登入程序 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return;
		}
	}
	
	//发送需要的资源
	/*进程号*/
	pid = getpid();
	sprintf(buf,"%d",pid);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");
	//printf("curSockfd=%d,id=%d,buf=%s\n",curSockfd,pid,buf);
	/*消息队列key值*/
	strcpy(buf,msg_key_text);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");
	//printf("curSockfd=%d msg_key_text=%s buf=%s\n",curSockfd,msg_key_text,buf);
	//sleep(1); /* 以防速度过快 套接字意外结束 */
	while(detime--); /* 大约0.1s */
	/*消息队列标识符*/
	sprintf(buf,"%d",msg_id);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");
	//printf("curSockfd=%d msg_id=%d,buf=%s\n",curSockfd,msg_id,buf);

	//printf("send_over\n");
	//等待登入结果
	if(recv(curSockfd,buf,32,0)<0)
		perror("recv");
	if(recv(curSockfd,name,32,0)<0)
		perror("recv");
	//printf("buf = %s|name = %s\n",buf,name);
	if(strcmp(buf,"log_success")==0){
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户\033[36m");
		strcat(msg_send.text,name);
		strcat(msg_send.text,"\033[0m \033[32m登入成功\033[0m\n");
		myMsgSend(msg_send);
		system("zenity --info --text=\"登入成功\" --no-wrap --title=登入");
	}else{
		system("zenity --warning --text=\"登入失败\" --no-wrap --title=登入");
		return;
	}
	
	//进入用户菜单
	userMenu();
	
	//等待服务器用户菜单响应函数的结束.以及清除状态操作的结果
	if(recv(curSockfd,buf,32,0)<0)
		perror("recv");
	if(strcmp(buf,"login_out_success")==0){
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户\033[36m");
		strcat(msg_send.text,name);
		strcat(msg_send.text,"\033[0m \033[32m即将退出登入\033[0m\n");
		system("zenity --info --text=\"退出登入\" --no-wrap --title=登出");
		myMsgSend(msg_send);
		sleep(1);
	}
	return;
}