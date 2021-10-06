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
*                                  注册函数
* @Desc  : 进入注册 
* @return: 无返回值
****************************************************************************************
*/
void clientRegister(void)
{
	int i,res;
	Msg msg_send = {-1,"none"};
	char buf[32],code[8],name[32],number[32],command[256];
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"%s\",\"\033[1H\033[2J");
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
void clientLogin()
{
	pid_t pid;
	int res;
	char buf[32],name[32],command[256];
	Msg msg_send = {-1,"none"};

	/* 进行显示屏清空输出 */
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"%s\",\"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------Login-----------\n");
	myMsgSend(msg_send);
	
	while(1){
		/* 打开表单弹窗 */
		strcpy(command,"zenity --forms --text=登入 --add-entry=用户名 --add-password=密码 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */
		/* 发送表单函数 */
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
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户名 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 密码  \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				break;
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
	printf("curSockfd=%d,id=%d,buf=%s\n",curSockfd,pid,buf);
	/*消息队列key值*/
	strcpy(buf,msg_key_text);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");
	printf("curSockfd=%d msg_key_text=%s buf=%s\n",curSockfd,msg_key_text,buf);
	sleep(1);
	/*消息队列标识符*/
	sprintf(buf,"%d",msg_id);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");
	printf("curSockfd=%d msg_id=%d,buf=%s\n",curSockfd,msg_id,buf);

	printf("send_over\n");
	//等待登入结果
	if(recv(curSockfd,buf,32,0)<0)
		perror("recv");
	//printf("buf = %s\n",buf);
	if(strcmp(buf,"log_success")==0){
		if(recv(curSockfd,name,32,0)<0)
			perror("recv");
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户\033[36m");
		strcat(msg_send.text,name);
		strcat(msg_send.text,"\033[0m \033[32m登入成功\033[0m\n");
		myMsgSend(msg_send);
		system("zenity --info --text=\"登入成功\" --no-wrap --title=登入");
	}else{
		system("zenity --warning --text=\"登入失败\" --no-wrap --title=登入");
		return;
	}
	DPRINTF("cd userMenu\n");
	//userMenu();
	DPRINTF("exit userMenu\n");
	
	if(recv(curSockfd,buf,32,0)<0)
		perror("recv");
	if(strcmp(buf,"login_out_success")==0){
		strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户\033[36m");
		strcat(msg_send.text,name);
		strcat(msg_send.text,"\033[0m \033[32m即将退出登入\033[0m\n");
		myMsgSend(msg_send);
		sleep(1);
	}
	return;
}

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

