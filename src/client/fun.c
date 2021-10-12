/*
*********************************************************************************************************
*												FUNCTION 
*
*                                         (c) Copyright 2021/10/07
*
* @File    : fun.c
* @Author  : yqxu
*
* @Description:
* ----------
*  功能函数集合
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-07 :   yqxu  :     功能函数集合
* 
*********************************************************************************************************
*/
#include "../../include/client/fun.h"

/*
****************************************************************************************
*                                  私聊
* @Desc  : @xyq:somethingNeedYouKnow
* @return: 
****************************************************************************************
*/
void priChat(void)
{
	int i,j,k,res,num;
    char buf[1024],send_buf[1024],fname[32],name[32];
    char command[1024];
    Msg msg_send = {1,"none"};

	//罗列好友
	listFriends();

	//输入聊天对象,输入结果交由服务器判断,并作返回
	while(1){
		printf("请选择:");
		scanf("%s",buf);

		strcpy(send_buf,"-");
		strcat(send_buf,buf);
		if(send(curSockfd,send_buf,32,0)<0)
			perror("send");

		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);

		if(strcmp(buf,"exit")==0){
			DPRINTF("[ \033[34mInfo\033[0m ] 退出私聊程序\n");
			if(send(curSockfd,"-exit_priChat",32,0)<0)
				perror("send");
			return;
		}else if(strcmp(buf,"error_input")==0){
			printf("\t\033[31mX\033[0m输入有误\n");
			continue;
		}else{
			/* 可以退出该输入程序 */
			break;
		}
	}

	//启动就绪信号
	if(send(curSockfd,"-start_priChat",32,0)<0)
		perror("send");

	//接收好友的名字
	sem_wait(&global_sem);
	strcpy(fname,global_sem_buf);

	//刷新显示屏
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text," __________________________________________\n");
	myMsgSend(msg_send);
	middleText(fname);
	strcpy(msg_send.text,"|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|\n");
	myMsgSend(msg_send);

	//接收消息数量
	if(send(curSockfd,"-num_ready",32,0)<0)		/* 发送接收就绪信号 */
		perror("send");
	sem_wait(&global_sem);
	strcpy(buf,global_sem_buf);

	num=atoi(buf);	/* 转化字符串 */

	//判断	有无未读消息
	if(num!=0){
		for(i=0;i<num;i++){
			if(send(curSockfd,"-msg_ready",32,0)<0)		/* 发送接收就绪信号 */
				perror("send");
			sem_wait(&global_sem);
			strcpy(buf,global_sem_buf);
			alignLeft(buf);								/* 靠左对齐打印文本 */
			middleText(" ");							/* 换行 */
		}
	}else{
		//发送接收已读消息数量就绪
		if(send(curSockfd,"-num_ready",32,0)<0)		/* 发送接收就绪信号 */
			perror("send");
		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);
		num = atoi(buf);

		if((num>0) && (num<7)){
			for(i=0;i<num;i++){
				if(send(curSockfd,"-msg_ready",32,0)<0)		/* 发送接收就绪信号 */
					perror("send");
				sem_wait(&global_sem);
				strcpy(buf,global_sem_buf);
				alignLeft(buf);								/* 靠左对齐打印文本 */
				middleText(" ");							/* 换行 */
			}
		}else if(num>6){
			for(i=0;i<6;i++){
				if(send(curSockfd,"-msg_ready",32,0)<0)		/* 发送接收就绪信号 */
					perror("send");
				sem_wait(&global_sem);
				strcpy(buf,global_sem_buf);
				alignLeft(buf);								/* 靠左对齐打印文本 */
				middleText(" ");							/* 换行 */
			}
		}else{
			middleText("暂无聊天记录");
		}
	}

	if(send(curSockfd,"-name_ready",32,0)<0)		/* 发送接收就绪信号 */
		perror("send");
	sem_wait(&global_sem);
	strcpy(name,global_sem_buf);					/* name:当前用户的名字 */

	//等待用户输入
	while(1){
		printf("%s","\033[1H\033[2J");
		printf(":");
		scanf("%s",buf);

		if(strCnlen(buf)>99){
			printf("\t超出字数限制\n");
			sleep(1);
			continue;
		}

		/* 作判断 */
		if(strcmp(buf,"exit")==0){
			if(send(curSockfd,"@exit",32,0)<0)		/* 发送接收就绪信号 */
				perror("send");
			break;
		}

		/* 发到显示屏上 */
		strcpy(msg_send.text,name);
		strcat(msg_send.text,":");
		strcat(msg_send.text,buf);
		alignLeft(msg_send.text);

		/* 发给自己的服务器,进行存储,同时也转发给对方 */
		strcpy(send_buf,"@");
		strcat(send_buf,buf);
		if(send(curSockfd,send_buf,128,0)<0)
			perror("send_msg");

		/* 等待处理结果|等待下一次处理就绪信号 */
		sem_wait(&global_sem);
		middleText(" ");
		printf("[ \033[33m✔\033[0m ]\n");
		sleep(1);
	}
	
	sem_wait(&global_sem);	/* 等待退出信号 */
	
	return;
}

/*
****************************************************************************************
*                                  修改密码
* @Desc  : 修改用户密码
* @return: 返回操作结果
****************************************************************************************
*/
int setPwd(void)
{
    int res;
    char buf[1024],send_buf[1024];;
    char command[1024];
    Msg msg_send = {1,"none"};

    msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------修改密码-----------\n");
	myMsgSend(msg_send);

    while(1){
        strcpy(command,"zenity --forms --text=修改密码 --add-password=原密码 --add-password=新密码 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */

        sprintf(buf,"%d",res);
		strcpy(send_buf,"-");
		strcat(send_buf,buf);
		if(send(curSockfd,send_buf,32,0)<0)
			perror("send");

        if(res == 0){ /*成功 */
			//等待服务器读取数据结果
			sem_wait(&global_sem);
			strcpy(buf,global_sem_buf);
			
			if(strcmp(buf,"ILLEGAL")==0){	/* 没有读到消息 */
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c setPwd():表单数据缓冲失败,即将终止操作\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=修改密码");
				continue;
			}
			
			//等待服务器读取判断结果
			sem_wait(&global_sem);
			strcpy(buf,global_sem_buf);
			if(strcmp(buf,"old_error")==0){	/* 接收到old_error代表原密码有误 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 原密码 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"原密码有误\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"same")==0){ /* 接收到same代表新旧密码相同 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 新密码 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"新密码不得与原密码相同\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 原密码 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 新密码 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
                system("zenity --error --text=\"修改成功\" --no-wrap --title=修改密码");
                
				break;
			}else{ /*代表接收到了其他send,报错终止注册退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c setPwd():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止修改操作\n");
				myMsgSend(msg_send);
				//同时告诉服务器退出响应注册函数
				if(send(curSockfd,"-recv_error",32,0)<0)
					perror("send");
				return FAILD;
			}
		}else{
			//失败:256
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 修改操作 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return FAILD;
		}	
        
    }
    return SUCCESS;
}

/*
****************************************************************************************
*                                  		添加好友
* @Desc  : 输入用户名查询是否存在,是否已是好友
* @return: 无返回
* @Note	 : 三种情况执行后会被退出 	一、成功;	二、用户名不存在 	三、自我添加
****************************************************************************************
*/
void addFriend(void)
{
	int res;
    char buf[1024],send_buf[1024];;
    char command[1024];
    Msg msg_send = {1,"none"};

    msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------添加好友-----------\n");
	myMsgSend(msg_send);
	
	while(1){
        strcpy(command,"zenity --forms --text=添加好友 --add-entry=好友名 --add-entry=请求消息 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */

        sprintf(buf,"%d",res);
		strcpy(send_buf,"-");
		strcat(send_buf,buf);
		if(send(curSockfd,send_buf,32,0)<0)
			perror("send");

        if(res == 0){ /*成功 */
			//等待服务器读取数据结果
			sem_wait(&global_sem);
			strcpy(buf,global_sem_buf);

			if(strcmp(buf,"ILLEGAL")==0){	/* 没有读到消息 */
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c setPwd():表单数据缓冲失败,即将终止添加\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=添加好友");
				continue;
			}
			
			//等待服务器读取判断结果
			sem_wait(&global_sem);
			strcpy(buf,global_sem_buf);

			if(strcmp(buf,"none_user")==0){	/* 接收到none_user代表用户不存在 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户名 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"用户不存在\" --no-wrap --title=添加好友");
				continue;
			}else if(strcmp(buf,"added")==0){ /* 接收到added代表已经添加过了 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 添加好友 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=你们已经是好友了 --no-wrap --title=添加好友");
				break;
			}else if(strcmp(buf,"addself")==0){ /* 接收到addself代表添加的是自己 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 添加 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=不允许自娱自乐 --no-wrap --title=添加好友");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=添加好友");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 发送请求 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
                system("zenity --error --text=\"请求发送成功\" --no-wrap --title=添加好友");
                
				break;
			}else{ /*代表接收到了其他send,报错终止注册退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c addFriends():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止添加操作\n");
				myMsgSend(msg_send);
				//同时告诉服务器退出响应注册函数
				if(send(curSockfd,"-recv_error",32,0)<0)
					perror("send");
				return;
			}
		}else{
			//失败:256
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 添加好友操作 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return;
		}	
    }
	return;
}

/*
****************************************************************************************
*                                  罗列好友列表
* @Desc  : 接收验证数量,逐个获取验证消息输出到显示屏
* @return: 无返回值
* @Note  : ！！！注意：给消息队列发文本前几个不能是中文，原因待查
		   ！！！注意：由于recv和sem_wait的处理速度不匹配所以接收下一字符的时候需要发送
		   				准备提示符!
****************************************************************************************
*/
void listAddMsg(void)
{
	int i=0,add_num;
	char buf[1024],index[8];
	Msg msg_send = {1,"none"};
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t---------验证消息列表---------\n");
	myMsgSend(msg_send);

	if(send(curSockfd,"-num_ready",32,0)<0)
		perror("send");

	/* 等待消息数量 */
	sem_wait(&global_sem);
	strcpy(buf,global_sem_buf);
	add_num = atoi(buf);
	//printf("接收到的验证消息数量:%d\n",add_num);

	for(i=0;i<add_num;i++){
		msg_send.choice=INULLMENU;
		//逐行接收名字与文本,并做处理

		//printf("send_nameready\n");
		if(send(curSockfd,"-name_ready",32,0)<0)
			perror("send");

		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);
		//printf("接收到的名字是:%s\n",global_sem_buf);
		strcpy(msg_send.text,"\t");
		sprintf(index,"%d",i+1);
		strcat(msg_send.text,index);
		strcat(msg_send.text,". ");
		strcat(msg_send.text,"\033[36m");
		strcat(msg_send.text,buf);
		strcat(msg_send.text,"\033[0m");
		strcat(msg_send.text," : ");


		//printf("send_textready\n");
		if(send(curSockfd,"-text_ready",32,0)<0)
			perror("send");

		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);
		//printf("接收到的文本是:%s\n",buf);
		strcat(msg_send.text,buf);
		strcat(msg_send.text,"\n\0");
		//printf("%d需要显示的文本:%s\n",msg_send.choice,msg_send.text);
		myMsgSend(msg_send);
	}
	
	msg_send.choice=INULLMENU;

	strcpy(msg_send.text,"\n\texit:退出\n");
	myMsgSend(msg_send);

	if(send(curSockfd,"-exit_listAddMsg",32,0)<0)
		perror("send");
	return;
}

/*
****************************************************************************************
*                                  处理验证消息
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void disposeAddMsg(void)
{
	char buf[1024];
	char send_text[1024];

	/* 等待处理 */
	printf("请处理:");
	scanf("%s",buf);

	/* 等待就绪信号 */
	sem_wait(&global_sem);

	/* 发送处理选项 */
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(curSockfd,send_text,32,0)<0)
		perror("send");

	if(strcmp(buf,"exit")==0){
		return;
	}

	/* 等待处理结果 */
	sem_wait(&global_sem);
	strcpy(buf,global_sem_buf);


	if(strcmp(buf,"error_input")==0){
		/* 提示输入有误 */
		system("zenity --error --text=错误输入 --no-wrap --title=处理添加");
		
	}else if(strcmp(buf,"process_over")==0){
		system("zenity --info --text=处理结束,即将回到主菜单 --no-wrap --title=处理添加");
	}
	
	/* 准备退出,给出退出就绪信号 */
	if(send(curSockfd,"-exit_dispose",32,0)<0)
		perror("send");
	return;
}

/*
****************************************************************************************
*                                 	 罗列当前用户的所有好友
* @Desc  : 由于害怕收发速度不匹配全局变量直接被覆盖,所以接收到数据后需要返回以后就绪信号
* @return: 无返回值
****************************************************************************************
*/
void listFriends(void)
{
	int i=0,friends_num;
	char buf[1024],index[8];
	Msg msg_send = {1,"none"};
	
	/* 刷新显示屏 */
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------好友列表-----------\n");
	myMsgSend(msg_send);

	/* 获取消息数量 */
	sem_wait(&global_sem);
	strcpy(buf,global_sem_buf);
	friends_num = atoi(buf);

	for(i=0;i<friends_num;i++){
		/* 发送就绪信号 */
		if(send(curSockfd,"-name_ready",32,0)<0)
			perror("send");	
		
		/* 获取名字 */
		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);

		strcpy(msg_send.text,"\t");
		sprintf(index,"%d",i+1);
		strcat(msg_send.text,index);
		strcat(msg_send.text,". ");
		strcat(msg_send.text,"\033[36m");
		strcat(msg_send.text,buf);
		strcat(msg_send.text,"\033[0m\t");

		/* 发送就绪信号 */
		if(send(curSockfd,"-state_ready",32,0)<0)
			perror("send");

		/* 等待在线状态 */
		sem_wait(&global_sem);
		strcpy(buf,global_sem_buf);
		strcat(msg_send.text,buf);
		strcat(msg_send.text,"\n\0");

		/* 发送消息至显示屏 */
		myMsgSend(msg_send);
	}

	strcpy(msg_send.text,"\n\texit:退出\n");
	myMsgSend(msg_send);

	/* @[Warn]:这个函数应该被封装成通用函数,后续撤掉功能 */
	//sleep(1);

	if(send(curSockfd,"-exit_listFriends",32,0)<0)
		perror("send");
	return;
}

/*
****************************************************************************************
*                                 	 下线指定用户
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void offLineUser(void)
{
	int res;
	char name[32],buf[1024],command[1024];
	Msg msg_send = {1,"none"};
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t---------强制下线用户---------\n");
	myMsgSend(msg_send);

	while(1){
		/* 打开表单弹窗 */
		strcpy(command,"zenity --forms --text=登入 --add-entry=用户名 > ./data/ipbuffer/");
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
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c offLineUser():表单数据缓冲失败,即将终止下线\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=下线操作");
				continue;
			}
			
			//等待服务器读取判断结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
				
			if(strcmp(buf,"name")==0){ /* 接收到name代表未注册 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 该用户 \033[31m不存在\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=用户不存在 --no-wrap --title=下线操作");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=下线操作");
				continue;
			}else if(strcmp(buf,"off_line")==0){ /* 接收到off_line不在线 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 该用户 \033[31m不在线\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --info --text=用户不在线 --no-wrap --title=下线操作");
				return;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户 \033[32m已被强制下线\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --info --text=用户已被强制下线 --no-wrap --title=下线操作");
				
				return;
			}else{ /*代表接收到了其他send,报错终止登入退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c offLineUser():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止下线操作\n");
				myMsgSend(msg_send);
				//同时告诉服务器
				if(send(curSockfd,"recv_error",32,0)<0)
					perror("send");
				return;
			}
		}else{ /* 窗口点击取消或是关闭 */
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 下线用户操作 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return;
		}
	}
	return;
}

/*
****************************************************************************************
*                                 	 远程关闭服务器
* @Desc  : 询问是否关闭,等待服务器操作结果,如果成功提示后exit(0)
* @return: 无返回值
****************************************************************************************
*/
void closeServer(void)
{
	int res;
	char name[32],buf[1024],command[1024];
	Msg msg_send = {1,"none"};
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t---------关闭客户端---------\n");
	myMsgSend(msg_send);

	//弹窗询问
	res = system("zenity --question --title=危 --ok-label=同意 --cancel-label=拒绝 --text=是否远程关闭服务器");

	sprintf(buf,"%d",res);
	if(send(curSockfd,buf,32,0)<0)
		perror("send");

	if(res == 0){
		/* 执行下线操作 */
		if(recv(curSockfd,buf,32,0)<0)
			perror("recv");
		
		if(strcmp(buf,"success")==0){
			system("zenity --warning --title=警告 --text=服务器已关闭,即将自动结束连接,请注意!\n");
			exit(0);
		}else{
			printf("[ \033[31mError\033[0m ]:undefined msg:%s\n",buf);
			return;
		}
	}else{
		system("zenity --info --title=Info --text=终止操作\n");
		return;
	}

	return;
}


/*
****************************************************************************************
*                            测试字符串的长度(包含中文)
* @Desc  : 两个中文=三个英文的长度
*		 : 写太早了忘记干嘛的,但是可以用,区别于strlen测出来的
			strCnlen("中文123");	//1+1+1+1+1=5
			strlen("中文123");//3+3+3+1+1+1=12
			
* @return: 返回字符串的个数
****************************************************************************************
*/
int strCnlen(char msg[])
{
	int j=0,count=0,msgLen=0;
	for(j=0;msg[j];j++){
		if(msg[j] & 0x80)
			count++;
		else
			msgLen++;
		if(count==3){
			count=0;
			msgLen+=2;
		}
	}
	msgLen--;
	return msgLen;
}

/*
****************************************************************************************
*                            居中显示字符串
* @Desc  : 
* @text	 : 需要居中显示的文本			
* @return: 
****************************************************************************************
*/
void middleText(char text[])
{
	int i,j,k;
	int cnLen;
	char buf[1024];
	Msg msg_send = {1,"none"};
	
	//计算字符串个数
	cnLen=strCnlen(text);
	
	strcpy(buf,"|");	/* 拼头 */
	
	for(i=(41-cnLen)/2;i>0;i--)	
		strcat(buf," ");	/* 拼前空 */
		
	strcat(buf,text);
	
	for(i=(41+cnLen)/2;i<41;i++)
		strcat(buf," ");	/* 拼后空 */
	
	strcat(buf,"|\n\0");	/* 拼尾 */
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,buf);
	myMsgSend(msg_send);
	
	return;
}

/*
****************************************************************************************
*                            左对齐显示字符串
* @Desc  : 
* @text	 : 需要左对齐显示的文本			
* @return: 
****************************************************************************************
*/
void alignLeft(char text[])
{	
	int i=1,j=0,k=0;
	int cnt=0,ent=1;
	int enLen,cnLen;
	char buf[1024];
	Msg msg_send = {1,"none"};

	enLen=strlen(text);			//字符长度
	cnLen=strCnlen(text);		//字符个数
	msg_send.choice=INULLMENU;	//选择无页面输出

	if(cnLen<43){	
		strcpy(buf,"|");	/* 拼头 */
		strcat(buf,text);	/* 拼文本 */
		for(i=cnLen;41-i>0;i++)
			strcat(buf," ");
		strcat(buf,"|\n\0");/* 拼尾 */
		strcpy(msg_send.text,buf);
		myMsgSend(msg_send);
	}else{
		strcpy(buf,"|");	/* 拼头 */
		
		while(j<enLen){
			/* 计算个数:处理行末 */
			if(text[j] & 0x80)
				cnt++;
			else
				ent++;
			
			/* 逐一赋值 */
			buf[i++]=text[j++];
			
			/* 计数进位 */
			if(cnt>2){
				cnt = 0;
				ent += 2;
			}
			
			if((cnt==0)&&(ent>40&&ent<43)){
				//printf("ent=%d\n",ent);
				/* 只有中文完整的输出后 并且到了行末 剩下的空格补位*/
				for(ent=43-ent;ent>0;ent--){
					buf[i++] = ' ';
				}
				buf[i++] = '|';
				buf[i++] = '\n';
				buf[i++] = '|';
			}
		}
		for(ent = 42-ent;ent>0;ent--)
			buf[i++] = ' ';
		buf[i++] = '|';
		buf[i++] = '\n';
		buf[i++] = '\0';
		strcpy(msg_send.text,buf);
		myMsgSend(msg_send);
	}
	return;
}


/*
****************************************************************************************
*                                 	 管理员删除指定用户
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void rmUser(void)
{
	int res;
	char name[32],buf[1024],command[1024];
	Msg msg_send = {1,"none"};
	
	msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t---------删除指定用户---------\n");
	myMsgSend(msg_send);

	while(1)
	{
		strcpy(command,"zenity --forms --text=删除用户 --add-entry=用户名 > ./data/ipbuffer/");
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
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c offLineUser():表单数据缓冲失败,即将终止下线\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=删除操作");
				continue;
			}
			
			//等待服务器读取判断结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
				
			if(strcmp(buf,"name")==0){
				system("zenity --info --text=用户不存在 --no-wrap --title=删除操作");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=删除操作");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 用户 \033[32m已被强制下线\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --info --text=用户已被删除 --no-wrap --title=删除操作");
				return;
			}else{ /*代表接收到了其他send,报错终止登入退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c offLineUser():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止删除操作\n");
				myMsgSend(msg_send);
				//同时告诉服务器
				if(send(curSockfd,"recv_error",32,0)<0)
					perror("send");
				return;
			}
		}else{ /* 窗口点击取消或是关闭 */
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 下线用户操作 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return;
		}
	}
	return ;
}