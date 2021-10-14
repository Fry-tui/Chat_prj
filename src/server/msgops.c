/*
*********************************************************************************************************
*											  Message Handling
*
*                                         (c) Copyright 2021/10/01
*
* @File    : msghand.c
* @Author  : yqxu
*
* @Description:
* ----------
*  封装了recv()函数,对接收到的消息做统一的判断,返回有效值
*  定义了reactMsg()函数,对接收到的消息进行二次判断,判断消息类型,释放不同的信号量
*  定义reactBuf()函数对弹窗接收到的数据进行封装处理
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-01 :   yqxu  :   消息处理函数
* 
*********************************************************************************************************
*/

#include "../../include/server/msgops.h"

/*
****************************************************************************************
*                                  接收处理函数
*
* @Desc  : 对客户端发来的消息进行判断是否合法
* @sockfd: 根据socket号接收判断不同客户端发来的消息
* @Note  : 如果使不合法消息就会关闭连接,该函数禁止被主函数调用
* @return: 放回接收到的消息
* @recv	 :
*	当调用该函数时候：
*		先检查套接字sockfd的接收缓冲区
*		如果sockfd接收缓冲区中没有数据或者协议正在接收数据，那么recv就一直等待，直到协议把数据接收完毕。
*		当协议把数据接收完毕，recv函数就把sockft的接收缓冲中的数据copy到buf中，recv函数返回其实际copy的字节数。

*	如果recv在copy时出错，那么它返回SOCKET_ERROR；
* 	如果recv函数在等待协议接收数据时网络中断了，那么它返回0 。

*	对方优雅的关闭socket并不影响本地recv的正常接收数据；
*	如果协议缓冲区内没有数据，recv返回0，指示对方关闭；
*	如果协议缓冲区有数据，则返回对应数据(可能需要多次recv)，在最后一次recv时，返回0，指示对方关闭。
****************************************************************************************
*/
char *myRecv(int sockfd)
{
	int msg_id;
	int recv_len; /* 接收recv函数的返回值(接收到的数据长度) */
	struct User * user;
	static char buf[1024]; /* 存放接收的数据,若数据无误则作为返回值放回,若数据有误直接结束线程*/
						/* 加static修饰符是为了改变buf的生命周期,使其成为全局变量,供调用者使用 						   */
	memset(buf,0,1024); /* 清除数据区 */
	recv_len = recv(sockfd,buf,1024,0);
	DPRINTF("[ \033[33mRecv_Buf\033[0m ] : %s\n",buf);
	if(recv_len<0){ 
		/* 接收出错 SOCKET_ERROR */
		perror("[ \033[31mError\033[0m ] msghand.c myRecv():");
	}else if(recv_len==0)	{
		/* 网络中断了,关闭sockfd,设置利用该socked上线的用户下线,结束调用该函数的线程*/
		user = reviseUserNode(USERNAME,"",sockfd);
		if(user!=NULL){
			if(user->online_state == 1){
				//若该用户在线,则修改登入时长
				user->duration += (time(NULL)-user->login_t);
			}
			close(user->udp_sockfd);
			user->udp_sockfd = -1;
			user->sockfd = -1;
			user->login_t = -1;
			//user->login_pid = -1; @[Warn]:暂时不知道会不会用到
			user->group_state = 0;
			user->online_state = 0;
			/* 关闭消息队列 */
			msg_id = atoi(user->msg_id_text); /* 获取消息队列表示符 */
			msgctl(msg_id,IPC_RMID,0); 
			killDisplay(user->msg_key_text); /* 关闭对应显示屏进程 */
			strcpy(user->msg_id_text,"");
			strcpy(user->msg_key_text,"");
			writeFile(USER);	/*将修改结果写入文件 */
			
			printf("[ \033[31mError\033[0m ]  %s网络崩溃,已被强制下线\n",user->name);
		}else{
			DPRINTF("[ \033[34mInfo\033[0m ] sockfd:%d 暂无用户使用\n",sockfd);
		}

		close(sockfd);	/* 关闭套接字 */
		
		/* 提示客户端断开连接 */
		printf("[ \033[34mInfo\033[0m ] 客户端%d",sockfd);
	    printf(" \033[31m已断开连接\033[0m\n");
		pthread_exit(0); /* 退出调用myRecv()的线程或是进程  ,	但是只会退出上一级   */	
		/*但如果由主函数调用,会出现直接结束进程的现象,所以该函数禁止被主函数调用*/
	}
	
	return buf;	/* 接收正常的话返回收到的信息 */
}

/*
****************************************************************************************
*                                服务器接收消息处理线程
*
* @Desc  : 为每个登入成功的客户端打开一个消息处理线程,对接收的消息进行二次处理
* @
* @return: 
* @Note  : 
* sem[0] - 普通收发数据
* sem[0] 
* sem[1] 
* sem[2] 
* sem[3] 
* sem[4]...
* sem[5]
****************************************************************************************
*/
void pthread_Recv(struct User *user)
{
	int i=0,j=0;	/* 主要用于字符索引 */
	int index;	/* 主要用于索引 */
	int len;
	int res;
	int sockfd;
	char buf[1024],msg[1024],name[1024],group_name[1024];
	char send_text[1024];
	struct User * fuser = NULL;
	struct Group * group = NULL;
	pthread_t preact_id;
	
	pthread_detach(pthread_self());	/*修改线程资源等级,意外结束时自动释放资源*/

	sockfd = user->sockfd; /* 获取sockfd,是为了 少打点字&结束线程的顺序合理 */
	
	//循环等待接收
	while(1){
		len = recv(sockfd,buf,1024,0);
		DPRINTF("[ \033[33mPRecv_Buf\033[0m ] : %s\n",buf);
		if(len<0){
			perror("server/msgops.c() pthread_Recv:recv_len --");
			close(sockfd);	/* 关闭套接字 */
			
			/* @[Warn]:应该是不需要杀死客户端,那就不需要进程号,<0会有哪些实例 	*/
			strcpy(buf,"kill ");
			strcat(buf,user->login_pid);
			system(buf);			/* 结束客户端函数 */
		
			/* 执行下线状态清除 */
			strcpy(user->login_pid,"null");
			strcpy(user->msg_id_text,"null");
			strcpy(user->msg_key_text,"null");
			strcpy(user->inet_ip_text,"null");	

			close(user->udp_sockfd);
			user->udp_sockfd = -1;
			user->sockfd = -1;
			user->group_state = 0;
			user->online_state = 0;
			user->login_t = 0;
			
			user->duration += (time(NULL)-user->login_t);
			preact_id = user->preact_id;	/* 获取父线程的id */
			user->precv_id = 0;
			user->preact_id = 0;
			//user->avail_flag = ILLEGAL;		/* 设置节点状态为非法退出 */
			//保存修改结果
			writeFile(USER);
			//关闭父线程 也就是每个客户端的响应线程
			pthread_cancel(preact_id);

			//提示输出
			printf("#system msg:\t\33[34m%s\33[0m 名媛已被强制下线！\n",user->name);
			printf("[ \033[31mError\033[0m ] 客户端%d",sockfd);
	    	printf(" \033[31m已断开连接\033[0m\n");

			//关闭孙线程 也就是当前每个客户端响应线程的消息接收处理线程
			pthread_exit(NULL);
		}else if(len==0){ /* 网络异常&通讯中断 */
			printf("[ \033[31mError\033[0m ] server/msgops.c pthread_Recv():用户%s异常中断通信\n",user->name);
			close(sockfd);	/* 关闭套接字 */
			/* 执行下线状态清除 */
			strcpy(user->login_pid,"null");
			strcpy(user->msg_id_text,"null");
			strcpy(user->msg_key_text,"null");
			strcpy(user->inet_ip_text,"null");	
			
			close(user->udp_sockfd);
			user->udp_sockfd = -1;
			user->sockfd = -1;
			user->group_state = 0;
			user->online_state = 0;
			user->login_t = 0;
			
			user->duration += (time(NULL)-user->login_t);
			preact_id = user->preact_id;	/* 获取父线程的id */
			user->precv_id = 0;
			user->preact_id = 0;
			//user->avail_flag = ILLEGAL;		/* 设置节点状态为非法退出 */
			//保存修改结果
			writeFile(USER);
			//关闭父线程 也就是每个客户端的响应线程
			pthread_cancel(preact_id);

			//提示输出
			printf("#system msg:\t\33[34m%s\33[0m 名媛已被强制下线！\n",user->name);
			printf("[ \033[31mError\033[0m ] 客户端%d",sockfd);
	    	printf(" \033[31m已断开连接\033[0m\n");

			//关闭孙线程 也就是当前每个客户端响应线程的消息接收处理线程
			pthread_exit(NULL);
		}

		//正常接收->正常处理
		/* 剔除标识符 */
		len = strlen(buf);
		for(i=1;i<len;i++){
			msg[i-1] = buf[i];
		}	
		msg[i-1]='\0';

		/* 判断消息类型 */
		if(buf[0]=='-'){		/* sem[0]:当前客户端发来的普通消息 */
			strcpy(user->sem_buf[0],msg);
			sem_post(&user->sem[0]);
		}else if(buf[0]=='@'){		/* sem[1]:收到的私聊消息格式 : @hello           */ 
			strcpy(user->sem_buf[1],msg);	/* 存储等待处理 */
			sem_post(&user->sem[1]);
		}else if(buf[0]=='$'){		/* sem[2]:收到的群聊消息格式 : $hello           */ 
			strcpy(user->sem_buf[2],msg);	/* 存储等待处理 */
			sem_post(&user->sem[2]);
		}else if(buf[0]=='a'){		/* sem[无需sem]:当前客户端处理完添*加后的结果(弹窗操作结果) */
			/* 实例 msg = 0|许玉泉  		操作结果|请求对象 */
			//printf("pthread_Recv():[a] msg = %s\n",msg);
			i=0;
			while(msg[i]!='|')
				buf[i] = msg[i++];
			buf[i++]='\0'; /* 取得的是操作结果 res [0|256] */
			//printf("pthread_Recv():[a] buf = %s\n",buf);
			j=0;
			while(msg[i]!='\0')
				name[j++] = msg[i++];
			name[j] = '\0';
			//printf("pthread_Recv():[a] name = %s\n",name);
			fuser = (struct User *)malloc(sizeof(struct User));
			fuser = reviseUserNode(USERNAME, name, 0); /* 根据用户名获取用户节点 */
			if(fuser==NULL){
				/* 用户注销了 */
				//发送弹窗类消息  		   示例："!out|用户:许玉泉已注销,无法加为好友!"
				strcpy(send_text,"!out|用户:");
				strcat(send_text,name);
				strcat(send_text,"已注销,无法加为好友!");
				
				//printf("pthread_Recv():[a] 未找到用户\n");
				
				if(send(user->sockfd,send_text,1024,0)<0) /* 给自己发送验证消息 */
					perror("send");
				continue;
			}
			/* 有没有可能同意添加的时候已经是好友了 */
			for(index=0;index < user->friend_num;i++){
				if(strcmp(user->friends[index].puser->name,name)==0)
					break;
			}
			printf("pthread_Recv():[a] inde=%d user->friend_num=%d\n",index,user->friend_num);
			//如果已是好友 index<user->friend_num
			if(strcmp(buf,"0")==0){
				/* 同意添加 */
				//判断是不是已是好友
				
				//printf("pthread_Recv():[a] 同意添加\n");
				
				if(index < user->friend_num){
					/* 是好友 */
					strcpy(send_text,"!out|用户:");
					strcat(send_text,name);
					strcat(send_text,"已经是你好友了,快去聊天吧!");
					
					//printf("pthread_Recv():[a] 重复添加\n");
					
					if(send(user->sockfd,send_text,1024,0)<0) /* 给自己发送消息 */
						perror("send");
					continue;
				}

				//printf("pthread_Recv():[a] 开始互加处理\n");
				strcpy(fuser->friends[fuser->friend_num].f_name,user->name);
				fuser->friends[fuser->friend_num].puser = user;
				fuser->friend_num++;
				strcpy(user->friends[user->friend_num].f_name,fuser->name);
				user->friends[user->friend_num].puser = fuser;
				user->friend_num++;
				
				//printf("pthread_Recv():[a] 处理完成,准备弹窗\n");
				//弹窗类消息	    
				if(fuser->online_state==1){
					//"!out|用户:田恩竹已通过好友验证,快来聊天吧!"
					strcpy(send_text,"!out|用户:");
					strcat(send_text,user->name);
					strcat(send_text,"已通过好友验证,快来聊天吧!");
					if(send(fuser->sockfd,send_text,1024,0)<0) /* 给对方发送消息 */
						perror("send");
				}else{
					//存入结构体验证消息
					/* 不在线添加消息到对方用户fuser的结构体 */
					strcpy(fuser->add_name[fuser->add_num],user->name);
					strcpy(fuser->add_msg[fuser->add_num],"-已通过好友验证,快来聊天吧!");
					fuser->add_num++;
				}

				strcpy(send_text,"!out|添加成功!");
				if(send(user->sockfd,send_text,1024,0)<0) /* 给自己发送验证消息 */
					perror("send");
			}else{
				/* 拒绝添加 */
			
				if(index < user->friend_num){
					/* 是好友 */
					strcpy(send_text,"!out|用户:");
					strcat(send_text,name);
					strcat(send_text,"已经是你好友了,无法拒绝哦!");
					if(send(user->sockfd,send_text,1024,0)<0) /* 给自己发送消息 */
						perror("send");
					break;
				}
				
				//弹窗类消息	    
				if(fuser->online_state==1){
					//"!out|用户:田恩竹已通过好友验证,快来聊天吧!"
					strcpy(send_text,"!out|用户:");
					strcat(send_text,user->name);
					strcat(send_text,"已拒绝进入你的鱼塘!");
					if(send(fuser->sockfd,send_text,1024,0)<0) /* 给对方发送消息 */
						perror("send");
				}else{
					//存入结构体验证消息
					/* 不在线添加消息到对方用户fuser的结构体 */
					strcpy(fuser->add_name[fuser->add_num],user->name);
					strcpy(fuser->add_msg[fuser->add_num],"-已拒绝进入你的鱼塘!");
					fuser->add_num++;
				}
				strcpy(send_text,"!out|驳回成功!");
				if(send(user->sockfd,send_text,1024,0)<0) /* 给自己发送验证消息 */
					perror("send");
			}
			fuser = NULL;
			writeFile(USER);
			//strcpy(user->sem_buf[1],msg);
			//sem_post(&user->sem[1]);
		}else if(buf[0]=='b'){
			/* 示例:msg = 0|某某某:申请加入-啥啥啥 */
			i=0;
			while(msg[i]!='|'){
				buf[i]=msg[i];
				i++;
			}
			buf[i++]='\0';
			res = atoi(buf);
			j=0;
			while(msg[i]!=':'){
				name[j++] = msg[i];
				i++;
			}
			name[j]='\0';
			while(msg[i++]!='-');
			j=0;
			while(msg[i]!='\0'){
				group_name[j++] = msg[i++];
			}
			group_name[j] = '\0';

			printf("res=%d|name=%s|group_name=%s\n",res,name,group_name);

			printf("初始化指针group\n");
			group = (struct Group *)malloc(sizeof(struct Group));
			printf("初始化完成\n");
			group = reviseGroupNode(GNAME, "", group_name);
			printf("查找完成\n");

			if(group==NULL){
				strcpy(send_text,"!out|群组:");
				strcat(send_text,group_name);
				strcat(send_text,"已解散,无法加人");
				if(send(user->sockfd,send_text,1024,0)<0)
					perror("send");
				continue;
			}

			printf("group->name=%s\n",group->group_name);
	
			/* 判断一下有无重复添加 */
			printf("group->mem_num=%d\n",group->mem_num);
			for(i=0;i<group->mem_num;i++){
				if(strcmp(group->group_mem[i]->name,name)==0)
						break;
			}
			printf("i=%d\n",i);
			if((group->mem_num!=0&&i<group->mem_num)||strcmp(name,group->owner->name)==0){
				/* 已经是成员重复验证 */
				strcpy(send_text,"!out|用户:");
				strcat(send_text,name);
				strcat(send_text,"本就是群聊成员,无需重复验证");
				if(send(user->sockfd,send_text,1024,0)<0)
					perror("send");
				continue;
			}

			/* 找到用户节点指针 */
			printf("malloc fuser\n");
			fuser = (struct User *)malloc(sizeof(struct User));
			printf("malloc over\n");
			fuser = reviseUserNode(USERNAME, name, 0);
			printf("fuser->name=%s\n",fuser->name);
			printf("找到节点\n");
			/* 判断群主操作结果 */
			if(res == 0){
				/* 同意进群 */
				printf("同意进群\n");
				strcpy(group->mem_name[group->mem_num],fuser->name);
				printf("group->mem_name[%d]=%s\n",group->mem_num,group->mem_name[group->mem_num]);
				group->group_mem[group->mem_num] = (struct User *)malloc(sizeof(struct User));
				printf("groupmalloc完成\n");
				group->group_mem[group->mem_num++] = fuser;
				printf("指向成功\n");
				
				if(fuser->online_state==0){
					/* 不在线 写入验证消息*/
					strcpy(fuser->add_name[fuser->add_num],group->group_name);
					strcpy(fuser->add_msg[fuser->add_num],"-已通过验证,快去找他们聊天吧!");/* 提示out弹窗 */
					fuser->add_num++;
				}else{
					/* 在线,弹窗提醒 */
					strcpy(send_text,"!out|群组:");
					strcat(send_text,group_name);
					strcat(send_text,"已同意你的请求,快来聊天吧!");
					if(send(fuser->sockfd,send_text,1024,0)<0)
						perror("send");
				}
			}else{
				/* 拒绝进群 */
				if(fuser->online_state==0){
					/* 不在线 写入验证消息*/
					strcpy(fuser->add_name[fuser->add_num],group->group_name);
					strcpy(fuser->add_msg[fuser->add_num],"-已驳回你的请求!");/* 提示out弹窗 */
					fuser->add_num++;
				}else{
					/* 在线,弹窗提醒 */
					strcpy(send_text,"!out|群组:");
					strcat(send_text,group_name);
					strcat(send_text,"已驳回你的请求!");
					if(send(fuser->sockfd,send_text,1024,0)<0)
						perror("send");
				}
			}
			
			writeFile(USER);
			printf("write_user\n");
			writeFile(GROUP);
			printf("write_groupover\n");
		}else{
			printf("[ \033[32mWarn\033[0m ] pthread_Recv(): 数据:%s 无法识别\n",buf);
		}
	}
	
}
