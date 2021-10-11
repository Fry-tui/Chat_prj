/*
*********************************************************************************************************
*												FUNCTION REACT
*
*                                         (c) Copyright 2021/10/07
*
* @File    : funserver.c
* @Author  : yqxu
*
* @Description:
* ----------
*  对功能函数进行响应
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-07 :   yqxu  :   响应功能函数操作
* 
*********************************************************************************************************
*/
#include "../../include/server/funserver.h"


/*
****************************************************************************************
*                                  修改密码
* @Desc  : 修改用户密码
* @return: 无返回值
****************************************************************************************
*/
int setPwd(struct User *user)
{
	int sockfd;
	char buf[1024];
	struct Buffer *buffer;

	sockfd = user->sockfd;
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	
	while(1){
		//DPRINTF("[ \033[34mInfo\033[0m ] 等待表单执行结果\n");
		sem_wait(&user->sem[0]);
		//DPRINTF("[ \033[34mInfo\033[0m ] 已读取,准备接收数据\n");
		strcpy(buf,user->sem_buf[0]);
		//DPRINTF("[ \033[34mInfo\033[0m ] 执行结果:%s|读取缓冲就绪\n",buf);
		if(strcmp(buf,"0")==0){	/* 存入buffer成功 */
			//读表单数据函数
			readBuffer(SETFORMBUF,2,"_form",(void *)buffer,user->inet_ip_text);
			//DPRINTF("[ \033[34mInfo\033[0m ] 读表单数据完成\n");
			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"-ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else if(buffer->avail_flag==LENILLEGAL){
				//发送长度不合法
				if(send(sockfd,"-LENILLEGAL",32,0)<0)
					perror("send");
				continue;
			}else{
				//发送读到了
				if(send(sockfd,"-READOVER",32,0)<0)
					perror("send");
			}

			/* 如果接收空输入 */
			if((strcmp(buffer->pwd,"")==0)||(strcmp(buffer->pwd,"")==0))
			{
				if(send(sockfd,"-NULL",32,0)<0)
					perror("send");
				continue;
			}
			//判断数据 
			if(strcmp(buffer->pwd,user->password)!=0){
				//告知旧密码
				if(send(sockfd,"-old_error",32,0)<0)
					perror("send");
				continue;
			}else if(strcmp(buffer->psd,buffer->pwd)==0){
				//告知新旧密码一致
				if(send(sockfd,"-same",32,0)<0)
					perror("send");
				continue;
			}else{
				//告知成功
				strcpy(user->password,buffer->psd);
				if(send(sockfd,"-success",32,0)<0)
					perror("send");
				break;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果客户端因为接收异常退出退出 */
			printf("[ \033[31mError\033[0m ] funserver.o setPwd:客户端收到无法解析的数据,异常退出\n");
			return FAILD;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRegister():客户端中断修改\n");
			return FAILD;
		}
	}
	return SUCCESS;
}

/*
****************************************************************************************
*                                  响应添加好友
* @Desc  : 
对客户端修改好友操作进行判断,重复添加|自我添加|用户存在与否,成功后将消息发送给目标用户
* @return: 无返回值
****************************************************************************************
*/
void addFriend(struct User * user)
{
	int i;
	int sockfd;
	char buf[1024],send_text[1024];
	struct User *fuser;
	struct Buffer *buffer;
	int detime = 100000000;//用与延时,耗费0.1s
	
	sockfd = user->sockfd;
	fuser = (struct User *)malloc(sizeof(struct User));
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	//printf("react addFriend strat\n");
	while(1){
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);
		if(strcmp(buf,"0")==0){	/* 存入buffer成功 */
			//读表单数据函数
			readBuffer(ADDFORMBUF,2,"_form",(void *)buffer,user->inet_ip_text);

			//DPRINTF("[ \033[34mInfo\033[0m ] 读表单数据完成\n");

			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"-ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else if(buffer->avail_flag==LENILLEGAL){
				//发送长度不合法
				if(send(sockfd,"-LENILLEGAL",32,0)<0)
					perror("send");
				continue;
			}else{
				//发送读到了
				if(send(sockfd,"-READOVER",32,0)<0)
					perror("send");
			}

			while(detime--); /* 大致延时0.1s */
			/* 如果接收空输入 */
			if((strcmp(buffer->name,"")==0)||(strcmp(buffer->text,"")==0))
			{
				if(send(sockfd,"-NULL",32,0)<0)
					perror("send");
				continue;
			}

			/* 遍历用户节点 */
			fuser = reviseUserNode(USERNAME,buffer->name,0);
			for(i=0;i<user->friend_num;i++){
				if(strcmp(buffer->name,user->friends[i].puser->name)==0){
					break;		/* 找到用户就退出 , i<len */
				}
			}
			/* 若是好友中没有遍历到这个请求用户 , i==len */
			
			//判断数据 
			if(strcmp(buffer->name,user->name)==0){
				//自我添加
				if(send(sockfd,"-addself",32,0)<0)
					perror("send");
				continue;
			}else if(fuser==NULL){
				//告知用户不存在
				if(send(sockfd,"-none_user",32,0)<0)
					perror("send");
				continue;
			}else if(i < user->friend_num){
				//告知已经添加过了
				if(send(sockfd,"-added",32,0)<0)
					perror("send");
				break;
			}else{
				//发送请求
				if(fuser->online_state==1){
					/* 在线直接发送弹窗消息 */
					strcpy(send_text,"!");
					strcat(send_text,"addRequire|");
					strcat(send_text,user->name);
					if(send(fuser->sockfd,send_text,1024,0)<0)
						perror("send");
				}else{
					/* 不在线添加消息到对方用户的结构体 */
					strcpy(fuser->add_name[fuser->add_num],user->name);
					strcpy(fuser->add_msg[fuser->add_num],buffer->text);
					fuser->add_num++;
					writeFile(USER);
				}

				if(send(sockfd,"-success",32,0)<0)
					perror("send");
				break;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果客户端因为接收异常退出退出 */
			printf("[ \033[31mError\033[0m ] funserver.o addFriend():客户端收到无法解析的数据,异常退出\n");
			return;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] addFriend():客户端中断添加操作\n");
			return;
		}
	}

	return;
	
}


/*
****************************************************************************************
*                                  罗列好友列表
* @Desc  : 发送验证数量,逐个发送验证消息
* @return: 无返回值
* @Note  : 
！！！注意：由于客户端recv和sem_wait的处理速度不匹配所以下一次发送需要等到就绪信号�
�
****************************************************************************************
*/
void listAddMsg(struct User * user)
{
	int i=0;
	char buf[1024];
	char send_text[1024];
	int sockfd = user->sockfd;

	sprintf(buf,"%d",user->add_num);
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");
	//printf("发送的验证消息数量为:%s\n",send_text);

	for(i=0;i<user->add_num;i++){
		sem_wait(&user->sem[0]);	//等待对方可以接收新数据
		
		strcpy(send_text,"-");
		strcat(send_text,user->add_name[i]);
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		//printf("发送的名字是:%s\n",send_text);
		
		sem_wait(&user->sem[0]);	//等待对方可以接收新数据
		strcpy(send_text,"-");
		strcat(send_text,user->add_msg[i]);
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		//printf("发送的文本是:%s\n",send_text);
	}	

	sem_wait(&user->sem[0]);	//等待退出信号
	return;
}


/*
****************************************************************************************
*                                  处理验证消息
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void disposeAddMsg(struct User * user)
{
	int i,index=0;
	int sockfd = user->sockfd;
	char buf[1024],send_text[1024];
	
	//等待操作结果
	sem_wait(&user->sem[0]);
	strcpy(buf,user->sem_buf[0]);

	/* 索引转化 */
	index = atoi(buf); /* 获取的是从1开始罗列的选项 */
	index--;	/* 因为数据索引从0开始 */
	if((index<0)||(index>=user->add_num)||(strlen(buf)>2)){
		//printf("输入有误\n");
		if(send(sockfd,"-error_input",1024,0)<0)
			perror("send");
	}else{
		DPRINTF("[ \033[36Info\033[0m ]处理的索引是:%d 处理的用户:%s 处理的文本:%s\n",index,user->add_name[index],user->add_msg[index]);

		/* 判断消息类型 */
		if(user->add_msg[index][0]!='-'){
			/* 请求类消息 */
			/* 直接发送弹窗消息 */
			strcpy(send_text,"!");
			strcat(send_text,"addRequire|");
			strcat(send_text,user->add_name[i]);
			if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		}else{
			/* 通知类消息 */
			/* 发送通知弹窗消息 */
			strcpy(send_text,"!");
			strcat(send_text,"out|");
			strcat(send_text,user->add_name[i]);
			strcat(send_text,user->add_msg[i]);
			if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		}

		

		/* 删除消息 */
		for(i=index;i<user->add_num-1;i++){
			strcpy(user->add_name[i],user->add_name[i+1]);
			strcpy(user->add_msg[i],user->add_msg[i+1]);
		}
		user->add_num--;

		writeFile(USER);
		if(send(sockfd,"-process_over",1024,0)<0)
			perror("send");
	}

	/* 等待客户端的退出就绪信号 */
	sem_wait(&user->sem[0]);
	return;
}

/*
****************************************************************************************
*                                  罗列好友列表
* @Desc  : 罗列出用户节点里的所有好友,由于担心收发速度不匹配,所以发送每个数据后需要
*			重新等待对方的接收就绪信号
* @return: 无返回值
****************************************************************************************
*/
void listFriends(struct User * user)
{
	int i=0;
	char buf[1024];
	char send_text[1024];
	int sockfd = user->sockfd;

	sprintf(buf,"%d",user->friend_num);
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");
	//printf("发送的验证消息数量为:%s\n",send_text);

	for(i=0;i<user->friend_num;i++){
		sem_wait(&user->sem[0]);	/* 等待接收就绪信号 */
		
		strcpy(send_text,"-");
		//printf("发送的名字是:%s\n",user->friends[i].puser->name);
		strcat(send_text,user->friends[i].puser->name);
		//printf("发送的名字是:%s\n",send_text);
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		
		sem_wait(&user->sem[0]);	/* 等待接收就绪信号 */
		strcpy(send_text,"-");
		if(user->friends[i].puser->online_state == 0){
			strcat(send_text,"\033[31m离线\033[0m");
		}else{
			strcat(send_text,"\033[32m在线\033[0m");
		}
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		printf("发送的文本是:%s\n",send_text);
	}
	sem_wait(&user->sem[0]);	/* 等待退出信号 */
}

/*
****************************************************************************************
*                                  下线指定用户
* @Desc  : 判断用户状态,放回操作结果
* @return: 无返回值
****************************************************************************************
*/
void offLineUser(int sockfd,char inet_ip[])
{
	char buf[1024];
	char send_text[1024];
	struct User * user;
	struct Buffer * buffer;
	user = (struct User *)malloc(sizeof(struct User));
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	
	while(1){
		strcpy(buf,myRecv(sockfd)); /* 等待执行表单的返回结果 */

		if(strcmp(buf,"0")==0){	 /* 存入数据成功 */
			//读表单数据函数
			readBuffer(NAMEFORMBUF,1,"_form",(void *)buffer,inet_ip);
			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else if(buffer->avail_flag==LENILLEGAL){
				//发送长度不合法
				if(send(sockfd,"LENILLEGAL",32,0)<0)
					perror("send");
				continue;
			}else{
				//发送读到了
				if(send(sockfd,"READOVER",32,0)<0)
					perror("send");
			}

			/* 如果接收空输入 */
			if(strcmp(buffer->name,"")==0)
			{
				if(send(sockfd,"NULL",32,0)<0)
					perror("send");
				continue;
			}
			
			//判断数据			
			user = reviseUserNode(USERNAME,buffer->name,0);
			if(user==NULL){
				//告知用户未注册
				if(send(sockfd,"name",32,0)<0)
					perror("send");
				continue;
			}else if(user->online_state==0){
				//不在线
				if(send(sockfd,"off_line",32,0)<0)
					perror("send");
				return;
			}else{
				//存在且在线

				//判断是不是异常关闭
				if(user->sockfd!=sockfd){
					/* 发送一个弹窗消息 */
					//发送弹窗类消息  		   示例："!out|#您已被强制下线!"
					strcpy(send_text,"!out|#您已被强制下线!");
					if(send(user->sockfd,send_text,1024,0)<0) /* 给被下线的用户发送验证消息 */
						perror("send");
					//关闭通道
					close(user->sockfd);
				}
				/* 清除状态 */
				strcpy(user->login_pid,"null");
				strcpy(user->msg_id_text,"null");
				strcpy(user->msg_key_text,"null");
				strcpy(user->inet_ip_text,"null");	
				
				
				user->sockfd = -1;
				user->group_state = 0;
				user->online_state = 0;
				user->login_t = 0;
				
				user->duration += (time(NULL)-user->login_t);
				user->precv_id = 0;
				user->preact_id = 0;
				//保存修改结果
				writeFile(USER);
				//printf("成功写入\n");
				//告知成功
				if(send(sockfd,"success",32,0)<0)
					perror("send");
				//printf("发送成功成功\n");
				return;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果管理员退出 */
			return;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRootMenu():管理员中断下线用户操作\n");
			return;
		}
	}
	return;
}


/*
****************************************************************************************
*                                 	 远程关闭服务器
* @Desc  : 如果管理员选择确定,则遍历链表,清除所有在线用户的状态标志位,
*			如果用户状态正常，会发送弹窗消息提示下线;
* @return: 无返回值
****************************************************************************************
*/
void closeServer(int sockfd)
{
	char buf[1024];
	char send_text[1024];
	LinklistU u = U->next;
	strcpy(buf,myRecv(sockfd)); /* 等待执行表单的返回结果 */

	if(strcmp(buf,"0")==0){	 /* 存入数据成功 */
		/* 执行挨个下线 */
		while(u){
			if(u->user.online_state==1){
				//判断是不是异常关闭
				if((u->user.sockfd!=sockfd)&&(u->user.sockfd>0)){
					/* 发送一个弹窗消息 */
					//发送弹窗类消息  		   示例："!out|#您已被强制下线!"
					strcpy(send_text,"!out|#服务器主动断开连接,您已被强制下线!");
					if(send(u->user.sockfd,send_text,1024,0)<0) /* 给被下线的用户发送验证消息 */
						perror("send");
					//关闭通道
					close(u->user.sockfd);
				}
				/* 清除状态 */
				strcpy(u->user.login_pid,"null");
				strcpy(u->user.msg_id_text,"null");
				strcpy(u->user.msg_key_text,"null");
				strcpy(u->user.inet_ip_text,"null");	
				
				
				u->user.sockfd = -1;
				u->user.group_state = 0;
				u->user.online_state = 0;
				u->user.login_t = 0;
				
				u->user.duration += (time(NULL)-u->user.login_t);
				u->user.precv_id = 0;
				u->user.preact_id = 0;
			}
			u = u->next;
		}
		writeFile(USER);
		if(send(sockfd,"success",32,0)<0)
			perror("send");
		system("zenity --warning --title=警告 --text=管理员:root远程关闭本机\n");
		exit(0);
		return;
	}else{
		return;
	}
	return;
}

/*
****************************************************************************************
*                                 	 删除指定用户
* @Desc  :
* @return: 返回值1为成功，0为失败
****************************************************************************************
*/
void rmUser(int sockfd,char inet_ip[])
{
	char buf[1024];
	char send_text[1024];
	struct User * user;
	struct Buffer * buffer;
	user = (struct User *)malloc(sizeof(struct User));
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	
	while(1){
		strcpy(buf,myRecv(sockfd)); /* 等待执行表单的返回结果 */

		if(strcmp(buf,"0")==0){	 /* 存入数据成功 */
			//读表单数据函数
			readBuffer(NAMEFORMBUF,1,"_form",(void *)buffer,inet_ip);
			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else if(buffer->avail_flag==LENILLEGAL){
				//发送长度不合法
				if(send(sockfd,"LENILLEGAL",32,0)<0)
					perror("send");
				continue;
			}else{
				//发送读到了
				if(send(sockfd,"READOVER",32,0)<0)
					perror("send");
			}

			/* 如果接收空输入 */
			if(strcmp(buffer->name,"")==0)
			{
				if(send(sockfd,"NULL",32,0)<0)
					perror("send");
				continue;
			}
			
			//判断数据			
			user = delUserNode(USERNAME,buffer->name,0);
			if(user==NULL){
				//告知用户未注册
				if(send(sockfd,"name",32,0)<0)
					perror("send");
				continue;
			}else{
				//存在
				//保存修改结果
				writeFile(USER);
				//printf("成功写入\n");
				//告知成功
				if(send(sockfd,"success",32,0)<0)
					perror("send");
				//printf("发送成功成功\n");
				return;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果管理员退出 */
			return;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRootMenu():管理员中断下线用户操作\n");
			return;
		}
	}
	return;
	
		
}

