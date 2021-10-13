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
*                                  私聊
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void priChat(struct User * user)
{
	int i,j,k;
	int sockfd;
	char buf[1024],send_text[1024];
	char unread_msg_name[32];
	char unread_msg[128][128];
	int unread_num=0;
	struct Buffer *buffer;
	struct Friend *u_fnode;//user节点中的fnode
	struct Friend *f_unode;//好友节点中的unode
	sockfd = user->sockfd;
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));

	//罗列好友
	listFriends(user);

	//等待客户端发送选择结果,返回判断结果
	while(1){
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);

		if(strcmp(buf,"exit")==0){
			if(send(sockfd,"-exit",32,0)<0)
				perror("send");
			sem_wait(&user->sem[0]);
			DPRINTF("[ \033[34mInfo\033[0m ] 退出私聊响应程序\n");
			return;
		}else{
			for(i=0;i<user->friend_num;i++){
				if(strcmp(user->friends[i].puser->name,buf)==0){
					/* 找到用户 */
					break;
				}
			}
			
			if(i>=user->friend_num){
				/* 发送错误输入 */
				if(send(sockfd,"-error_input",32,0)<0)
					perror("send");
				continue;
			}else{
				if(send(sockfd,"-end_input",32,0)<0)
					perror("send");
				break;
			}
		}
	}

	/* 等待就绪信号 */
	sem_wait(&user->sem[0]);

	//指向操作:便于管理
	/* 直接访问对方用户节点 -> 获取user->friends[i],包括了自己的一些聊天记录 */
	u_fnode = (struct Friend *)malloc(sizeof(struct Friend));	/* 指针初始化 */
	u_fnode = &user->friends[i];
	/* 访问对方用户节点里的好友结构体数组关于我的那个 user->friends[i]*/
	f_unode = (struct Friend *)malloc(sizeof(struct Friend));
	for(i=0;i<u_fnode->puser->friend_num;i++){
		/* 寻找对方用户节点中属于我的好友结构体 */
		if(strcmp(user->name,u_fnode->puser->friends[i].puser->name)==0){
			f_unode = &u_fnode->puser->friends[i];
			break;
		}
	}

	/* 打开私聊标志位 */
	u_fnode->chat_state = 1;
	printf("u_fnode->chat_state = %d\n",u_fnode->chat_state);
	//发送好友名字
	strcpy(send_text,"-");
	strcat(send_text,u_fnode->puser->name);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");

	
	printf("f_unode->chat_state = %d\n",f_unode->chat_state);
	//判断对方状态进行弹窗提示		对方正在输入或聊天已就绪
	if(f_unode->chat_state==1){
		//弹窗消息        !out|对方正在输入...
		strcpy(send_text,"!out|对方正在输入...");
		if(send(u_fnode->puser->sockfd,send_text,1024,0)<0) /* 给对方发消息 */
			perror("send");
	}
	
	//判断有无属于该好友未读消息	| 私聊的未读消息需要加	@xyq:你吃了么
	for(i=0;i<user->unread_msg_num;i++){
		if(user->unread_msg[i][0]=='@'){
			j=1;
			while(user->unread_msg[i][j]!=':'){
				unread_msg_name[j-1] = user->unread_msg[i][j];
				j++;
			}
			unread_msg_name[j-1] = '\0';
			if(strcmp(unread_msg_name,u_fnode->puser->name)==0){
				/* 如果是该用户的消息 就存起来,一伙输出 */
				strcpy(unread_msg[unread_num],user->unread_msg[i]);
				unread_num++;

				/* @[Warn]:存到历史记录 ✔*/
				strcpy(u_fnode->chat_msg[u_fnode->chat_len],user->unread_msg[i]);
				u_fnode->chat_len++;
				
				/* @[Warn]:清除未读消息 ✔*/
				strcpy(user->unread_msg[i],"");
			}
			
		}
	}

	//把未读数量发过去
	sem_wait(&user->sem[0]);	/* 等待接收就绪信号 */
	sprintf(buf,"%d",unread_num);
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");

	//根据未读数量判断输出的是未读消息还是历史记录
	if(unread_num!=0){
		/* 清除节点里已读的未读消息 所有被赋值位""的消息都被清除*/
		clearUnreadMsg(user);
		
		/* 输出未读消息 */
		for(i=0;i<unread_num;i++){
			/* 等待客户端传过来的接收就绪信号 */
			sem_wait(&user->sem[0]);

			/* 发送未读消息 */
			strcpy(send_text,"-");
			strcat(send_text,unread_msg[i]);
			if(send(sockfd,send_text,1024,0)<0)
				perror("send");

		}
	}else{
		/* 如果没有未读消息,输出历史记录的最后六条 */
	
		sem_wait(&user->sem[0]);	/* 等待一个接收就绪信号 */

		/* 发送历史记录的数量 */
		sprintf(buf,"%d",u_fnode->chat_len);
		strcpy(send_text,"-");
		strcat(send_text,buf);
		if(send(sockfd,send_text,32,0)<0)
		perror("send");

		//判断历史记录数量
		if((u_fnode->chat_len>0)&&(u_fnode->chat_len<7)){
			/* 六条以内全部输出 */
			for(i=0;i<u_fnode->chat_len;i++){
				/* 等待客户端传过来的接收就绪信号 */
				sem_wait(&user->sem[0]);

				/* 发送历史消息 */
				strcpy(send_text,"-");
				strcat(send_text,u_fnode->chat_msg[i]);
				if(send(sockfd,send_text,1024,0)<0)
					perror("send");
			}
		}else if(u_fnode->chat_len>6){
			/* 六条以外输出倒六 */
			for(i=u_fnode->chat_len-6;i<u_fnode->chat_len;i++){
				/* 等待客户端传过来的接收就绪信号 */
				sem_wait(&user->sem[0]);

				/* 发送历史消息 */
				strcpy(send_text,"-");
				strcat(send_text,u_fnode->chat_msg[i]);
				if(send(sockfd,send_text,1024,0)<0)
					perror("send");
			}
		}else{
			/* 什么都不做继续向下 */
		}
	}

	writeFile(USER);
	
	//发送当前用户名:作消息封装
	/* 等待客户端传过来的接收就绪信号 */
	sem_wait(&user->sem[0]);
	strcpy(send_text,"-");
	strcat(send_text,user->name);
	if(send(sockfd,send_text,1024,0)<0)
		perror("send");

	while(1){
		//等待用户输入|存入记录
		sem_wait(&user->sem[1]);
		strcpy(buf,user->sem_buf[1]);
		
		if(strcmp(buf,"exit")==0){
			/* 退出操作 */
			//printf("exit\n");
			u_fnode->chat_state = 0;
			break;
			//printf("test\n");
		}

		//需要处理的消息

		/* 存入自己的记录 */
		if(u_fnode->chat_len>63){
			/* 消息过多,需要清理,删一半 */
			for(i=0;i<u_fnode->chat_len/2;i++){
				strcpy(u_fnode->chat_msg[i],u_fnode->chat_msg[i+32]);
			}
			u_fnode->chat_len == i;
		}
		//strcpy(u_fnode->chat_msg[u_fnode->chat_len++],buf);
		strcpy(u_fnode->chat_msg[u_fnode->chat_len],user->name);
		strcat(u_fnode->chat_msg[u_fnode->chat_len],":");
		strcat(u_fnode->chat_msg[u_fnode->chat_len],buf);
		u_fnode->chat_len++;
		
		/* 判断对方的状态        */
		if(u_fnode->puser->online_state==0){
			/* 离线:存入未读消息 */
			strcpy(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],"@");
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],user->name);
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],":");
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],buf);
			u_fnode->puser->unread_msg_num++;
		}else if(u_fnode->puser->online_state==1&&f_unode->chat_state==0){
			/* 在线不对话:弹窗提醒|存入未读消息 */
			strcpy(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],"@");
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],user->name);
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],":");
			strcat(u_fnode->puser->unread_msg[u_fnode->puser->unread_msg_num],buf);
			u_fnode->puser->unread_msg_num++;

			/* 发送弹窗类消息 格式:			!out|许玉泉给你发了一条私信 */
			strcpy(send_text,"!out|");
			strcat(send_text,user->name);
			strcat(send_text,"给你发了一条私信\0");
			if(send(u_fnode->puser->sockfd,send_text,1024,0)<0) /* 给被下线的用户发送验证消息 */
				perror("send");
		}else{
			/* 在线在对话:封装发送给对方客户端线程|存入已读消息@[Warn]:存入对方已读 */
			strcpy(send_text,"@");
			strcat(send_text,user->name);
			strcat(send_text,":");
			strcat(send_text,buf);
			if(send(u_fnode->puser->sockfd,send_text,1024,0)<0) /* 给被下线的用户发送验证消息 */
				perror("send");

			/* 存入对方的记录 */
			if(f_unode->chat_len>63){
				/* 消息过多,需要清理,删一半 */
				for(i=0;i<f_unode->chat_len/2;i++){
					strcpy(f_unode->chat_msg[i],f_unode->chat_msg[i+32]);
				}
				f_unode->chat_len == i;
			}
			strcpy(f_unode->chat_msg[f_unode->chat_len],user->name);
			strcat(f_unode->chat_msg[f_unode->chat_len],":");
			strcat(f_unode->chat_msg[f_unode->chat_len],buf);
			f_unode->chat_len++;
		}

		if(send(sockfd,"-over_sand",1024,0)<0) /* 发送处理完成等待就绪的信号 */
			perror("send");
		
	}

	//printf("here\n");
	u_fnode->chat_state = 0;	
	writeFile(USER);
	//printf("写入完成\n");
	if(send(sockfd,"-exit_priChat",1024,0)<0) /* 发送处理完成等待就绪的信号 */
		perror("send");
	return;
}



/*
****************************************************************************************
*                                  响应群聊
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/

void groupChat(struct User * user)
{	
	int i,index;
	char buf[1024],send_text[1024];
	int sockfd = user->sockfd;
	struct Group * group;
	group = (struct Group *)malloc(sizeof(struct Group));
	
	while(1){
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);

		if(strcmp(buf,"exit")==0){
			if(send(sockfd,"-exit",32,0)<0)
				perror("send");
			sem_wait(&user->sem[0]);	/* 等待退出信号 */
			DPRINTF("[ \033[34mInfo\033[0m ] 退出群聊响应程序\n");
			return;
		}else{
			/* 判断群是否存在 */
			group = reviseGroupNode(GNAME, "", buf);

			if(group == NULL){
				/* 没找到 */
				if(send(sockfd,"-error_input",32,0)<0)
					perror("send");
				continue;
			}else{
				/* 查看群成员有没有自己 */
				for(i=0;i<group->mem_num;i++){
					if(strcmp(user->name,group->group_mem[i]->name)==0){
						if(send(sockfd,"-end_input",32,0)<0)
							perror("send");
						index = i;
						break;
					}
				}
				if(i>=group->mem_num){
					if(send(sockfd,"-error_input",32,0)<0)
						perror("send");
					continue;
				}else{
					break;
				}
			}
		}
	}

	sem_wait(&user->sem[0]);	/* 同步信号 */

	//开启群聊
	group->g_state[index]=1;
	/* 发送群名 */
	//发送群聊名字
	strcpy(send_text,"-");
	strcat(send_text,group->group_name);
	if(send(sockfd,send_text,64,0)<0)
		perror("send");

	sem_wait(&user->sem[0]);	/* 等待同步信号 */
	if(group->msg_num>6){
		if(send(sockfd,"-6",64,0)<0)
			perror("send");
	}else{
		sprintf(buf,"%d",group->msg_num);
		strcpy(send_text,"-");
		strcat(send_text,buf);
		if(send(sockfd,send_text,64,0)<0)
			perror("send");
	}

	if(group->msg_num>0&&group->msg_num<7){
		for(i=0;i<group->msg_num;i++){
			sem_wait(&user->sem[0]);
			strcpy(send_text,"-");
			strcat(send_text,group->group_msg[i]);
			if(send(sockfd,send_text,1024,0)<0)
				perror("send");
		}
	}else if(group->msg_num>6){
		for(i=6;i>0;i--){
			sem_wait(&user->sem[0]);
			strcpy(send_text,"-");
			strcat(send_text,group->group_msg[group->msg_num-i]);
			if(send(sockfd,send_text,1024,0)<0)
				perror("send");
		}
	}

	sem_wait(&user->sem[0]);	/* 同步 */
	strcpy(send_text,"-");
	strcat(send_text,user->name);
	if(send(sockfd,send_text,64,0)<0)
		perror("send");

	/* 等待用户输入 */
	while(1){
		sem_wait(&user->sem[2]);
		strcpy(buf,user->sem_buf[2]);

		if(strcmp(buf,"exit")==0){
			/* 退出操作 */
			break;
		}

		if(group->msg_num>63){
			/* 消息过多,需要清理,删一半 */
			for(i=0;i<group->msg_num/2;i++){
				strcpy(group->group_msg[i],group->group_msg[i+32]);
			}
			group->msg_num == i;
		}
		/* 把消息存入群聊记录 xxx:sadadsadad */
		strcpy(group->group_msg[group->msg_num],user->name);
		strcat(group->group_msg[group->msg_num],":");
		strcat(group->group_msg[group->msg_num],buf);
		group->msg_num++;

		for(i=0;i<group->mem_num;i++){
			if((group->group_mem[i]->online_state==1)&&(group->g_state[i]==1)&&(index!=i)){
				/* 发送过去 */
				strcpy(send_text,"@");
				strcat(send_text,user->name);
				strcat(send_text,":");
				strcat(send_text,buf);
				if(send(group->group_mem[i]->sockfd,send_text,1024,0)<0) 
					perror("send");
			}
		}

		if(send(sockfd,"-over_sand",1024,0)<0) /* 发送处理完成等待就绪的信号 */
			perror("send");
	}

	group->g_state[index] = 0;
	writeFile(USER);
	if(send(sockfd,"-exit_groupChat",1024,0)<0) /* 发送处理完成等待就绪的信号 */
		perror("send");
	return;
}


/*
****************************************************************************************
*                                  创建群聊
* @Desc  : 创建群聊
* @return: 无返回值
****************************************************************************************
*/
void createGroup(struct User * user)
{
	int i;
	int res;
	int sockfd;
	char buf[1024],send_text[1024];
	struct Group * group;
	struct Buffer *buffer;
	struct User empty_u;
	struct Redp empty_r;
	sockfd = user->sockfd;
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	group = (struct Group *)malloc(sizeof(struct Group));

	/* 发送一个同步信号 */
	if(send(sockfd,"-ready_create",32,0)<0)
		perror("send");

	/* 获取表单数据 */
	while(1){
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);
		//printf("NAME表单操作结果:%s\n",buf);
		if(strcmp(buf,"0")==0){  /* 存入数据成功 */
			//读表单数据函数
			readBuffer(NAMEFORMBUF,1,"_form",(void *)buffer,user->inet_ip_text);
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

			/* 等待同步信号 */
			sem_wait(&user->sem[0]);
			//printf("同步信号:%s\n",user->sem_buf[0]);
			
			/* 如果接收空输入 */
			if(strcmp(buffer->name,"")==0)
			{
				if(send(sockfd,"-NULL",32,0)<0)
					perror("send");
				continue;
			}
			//printf("查询链表前\n");
			group = reviseGroupNode(GNAME,"",buffer->name);
			//printf("查询结果:\n");
			if(group!=NULL){
				/* 群名已被占用 */
				//printf("group!=NULL\n");
				if(send(sockfd,"-name",32,0)<0)
					perror("send");
				continue;
			}else{
				//printf("group==NULL\n");
				group = (struct Group *)malloc(sizeof(struct Group));
				//printf("开始赋值名字:%s\n",buffer->name);
				strcpy(group->group_name,buffer->name);
				//printf("赋值名字结束-group->name:%s\n",group->group_name);
				/* 告知群名可用 */
				if(send(sockfd,"-none_group",32,0)<0)
					perror("send");
				break;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果管理员退出 */
			return;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRootMenu():用户中断创建群聊操作\n");
			//printf("存储失败准备退出\n");
			sem_wait(&user->sem[0]);
			return;
		}
	}

	//printf("等待同步信号\n");
	/* 等待同步信号 */
	sem_wait(&user->sem[0]);

	//printf("等待权限询问接结果\n");
	sem_wait(&user->sem[0]);
	res = atoi(user->sem_buf[0]);
	//printf("权限询问结果:%s res=%d\n",user->sem_buf[0],res);
	if(res == 0){	 /* 开启验证 */
		group->permit = 1;
	}else{/* 关闭验证 */
		group->permit = 0;
	}
	//printf("group->permit=%d\n",group->permit);
	strcpy(group->owner_name,user->name);
	group->owner = user;
	group->msg_num = 0;
	group->mem_num = 0;

	for(i=0;i<32;i++){
		group->group_mem[i] = NULL;
		group->g_state[i]=0;
	}
	for(i=0;i<64;i++){
		strcpy(group->group_msg[i],"");
	}

	/* 让群主成为第一个群成员 */
	strcpy(group->mem_name[i],user->name);
	group->group_mem[0] = (struct User *)malloc(sizeof(struct User));
	group->group_mem[0] = user;
	group->mem_num=1;
	addNode(GROUP, empty_u, empty_r, *group);
	//printf("添加成功\n");
	writeFile(GROUP);
	//printf("写入成功\n");

	free(group);
	if(send(sockfd,"-exit_createGroup",32,0)<0)
		perror("send");
	return;
}


/*
****************************************************************************************
*                                  罗列群聊名称
* @Desc  : 罗列群组名称
* @return: 无返回值
****************************************************************************************
*/
void listGroups(struct User * user)
{
	int i=0;
	char buf[1024];
	char send_text[1024];
	int sockfd = user->sockfd;
	int group_num;
	LinklistG g = G->next;

	group_num = cntGNode();

	sprintf(buf,"%d",group_num);
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");

	while(g){
		sem_wait(&user->sem[0]);
		strcpy(send_text,"-");
		strcat(send_text,g->group.group_name);
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
		g = g->next;
	}

	sem_wait(&user->sem[0]);	/* 等待退出信号 */
	
	return;
}


/*
****************************************************************************************
*                               罗列自己加入的群聊名称
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/

void listMyGroups(struct User * user)
{	
	int i=0;
	char buf[1024];
	char send_text[1024];
	int sockfd = user->sockfd;
	int group_num=0;
	char group_name[64][64];
	LinklistG g = G->next;

	//获取有我的群的数量
	while(g){
		if(strcmp(user->name,g->group.owner_name)==0){
			strcpy(group_name[group_num],g->group.group_name);
			group_num++;
			g = g->next;
			continue;
		}
		for(i=0;i<g->group.mem_num;i++){
			if(strcmp(user->name,g->group.group_mem[i]->name)==0){
				strcpy(group_name[group_num],g->group.group_name);
				group_num++;
				break;
			}
		}
		g = g->next;
	}

	sprintf(buf,"%d",group_num);
	strcpy(send_text,"-");
	strcat(send_text,buf);
	if(send(sockfd,send_text,32,0)<0)
		perror("send");

	for(i=0;i<group_num;i++){
		sem_wait(&user->sem[0]);
		strcpy(send_text,"-");
		strcat(send_text,group_name[i]);
		if(send(sockfd,send_text,1024,0)<0)
			perror("send");
	}

	sem_wait(&user->sem[0]);	/* 等待退出信号 */
	
	return;

}


/*
****************************************************************************************
*                                  加入群聊
* @Desc  : 若群里无需验证直接加入,需验证(群主在线->弹窗)|(群主不在线->未读)
* @return: 无返回值
****************************************************************************************
*/
void joinGroup(struct User * user)
{
	int i;
	char buf[1024],send_text[1024];
	int sockfd = user->sockfd;
	struct Group * group;
	group = (struct Group *)malloc(sizeof(struct Group));
	while(1){
		sem_wait(&user->sem[0]);
		strcpy(buf,user->sem_buf[0]);

		if(strcmp(buf,"exit")==0){
			if(send(sockfd,"-exit",32,0)<0)
				perror("send");
			sem_wait(&user->sem[0]);	/* 等待退出信号 */
			DPRINTF("[ \033[34mInfo\033[0m ] 退出加群响应程序\n");
			return;
		}else{
			/* 判断群是否存在 */
			group = reviseGroupNode(GNAME, "", buf);

			if(group == NULL){
				/* 没找到 */
				if(send(sockfd,"-error_input",32,0)<0)
					perror("send");
				continue;
			}else{
				if(send(sockfd,"-end_input",32,0)<0)
					perror("send");
				break;
			}
		}
	}

	//printf("等待同步信号\n");
	sem_wait(&user->sem[0]);	/* 同步信号 */
	//printf("group->mem_num=%d\n",group->mem_num);
	for(i=0;i<group->mem_num;i++){
		printf("user->name=%s\n",user->name);
		printf("group->group_mem[%d]=%s\n",i,group->group_mem[i]->name);
		if(strcmp(group->group_mem[i]->name,user->name)==0)
			break;
	}
	printf("i=%d\n",i);
	
	if((group->mem_num!=0&&i<group->mem_num)||strcmp(user->name,group->owner->name)==0){
		if(send(sockfd,"-mem_exist",32,0)<0)
			perror("send");
		sem_wait(&user->sem[0]);	/* 等待退出信号 */
		return;
	}
	printf("group->permit=%d\n",group->permit);
	
	/* 判断是否需要验证 */
	if(group->permit==1){
		/* 需要验证 */
		
		/* 判断群主在不在线 */
		if(group->owner->online_state==1){
			/* 在线直接发送弹窗消息 !addGroup|许玉泉:请求加入闲聊群*/
			strcpy(send_text,"!");
			strcat(send_text,"addGroup|");
			strcat(send_text,user->name);
			strcat(send_text,":请求加入-");
			strcat(send_text,group->group_name);
			if(send(group->owner->sockfd,send_text,1024,0)<0)
				perror("send");
		}else{
			/* 不在线->发送到验证消息到群主的结构体 许玉泉 #请求加入闲聊群*/
			strcpy(group->owner->add_name[group->owner->add_num],user->name);
			strcpy(group->owner->add_msg[group->owner->add_num],":请求加入-");
			strcat(group->owner->add_msg[group->owner->add_num],group->group_name);
			group->owner->add_num++;
		}
		
		if(send(sockfd,"-require_over",32,0)<0)
			perror("send");
	}else{
		/* 无需验证 */
		strcpy(group->mem_name[group->mem_num],user->name);
		group->group_mem[group->mem_num++]=user;
		
		if(send(sockfd,"-add_over",32,0)<0)
			perror("send");
	}

	writeFile(GROUP);
	sem_wait(&user->sem[0]);	/* 等待退出信号 */
	return;
}


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

			while(detime-->0); /* 大致延时0.1s */
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

	sem_wait(&user->sem[0]);
	
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

	/* 发送接收就绪信号 */
	if(send(sockfd,"-disposeAddMsg_start",1024,0)<0)
		perror("send");
	
	//等待操作结果
	sem_wait(&user->sem[0]);
	strcpy(buf,user->sem_buf[0]);

	/* 索引转化 */
	index = atoi(buf); /* 获取的是从1开始罗列的选项 */
	index--;	/* 因为数据索引从0开始 */
	
	if(strcmp(buf,"exit")==0){
		return;
	}else if((index<0)||(index>=user->add_num)||(strlen(buf)>2)){
		//printf("输入有误\n");
		if(send(sockfd,"-error_input",1024,0)<0)
			perror("send");
	}else{
		DPRINTF("[ \033[36mInfo\033[0m ]处理的索引是:%d 处理的用户:%s 处理的文本:%s\n",index,user->add_name[index],user->add_msg[index]);

		/* 判断消息类型 */
		if(user->add_msg[index][0]==':'){
			/* 请求类消息 */
			/* 直接发送弹窗消息 */
			strcpy(send_text,"!");
			strcat(send_text,"addGroup|");
			strcat(send_text,user->add_name[index]);
			strcat(send_text,user->add_msg[index]);
			if(send(sockfd,send_text,1024,0)<0)
				perror("send");
		}else if(user->add_msg[index][0]!='-'){
			/* 请求类消息 */
			/* 直接发送弹窗消息 */
			strcpy(send_text,"!");
			strcat(send_text,"addRequire|");
			strcat(send_text,user->add_name[index]);
			if(send(sockfd,send_text,1024,0)<0)
				perror("send");
		}else{
			/* 通知类消息 */
			/* 发送通知弹窗消息 */
			strcpy(send_text,"!");
			strcat(send_text,"out|");
			strcat(send_text,user->add_name[index]);
			strcat(send_text,user->add_msg[index]);
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
		//printf("发送的文本是:%s\n",send_text);
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
	char buf[46];
	char send_text[1024];
	struct User * user;
	struct Buffer * buffer;
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));

	printf("sizeof(user) = %ld\n",sizeof(struct User));
	while(1){
		//printf("wait recv\n");
		strcpy(buf,myRecv(sockfd)); /* 等待执行表单的返回结果 */
		//printf("Recv_buf:%s\n",buf);
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
			//printf("初始前\n");
			user = (struct User *)malloc(sizeof(struct User));
			//printf("初始后\n");
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
* @return: 无返回值
****************************************************************************************
*/
void rmUser(int sockfd,char inet_ip[])
{
	int res=FAILD;
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
			res = delUserNode(USERNAME,buffer->name,0);
			if(res==FAILD){
				//告知用户未注册
				if(send(sockfd,"name",32,0)<0)
					perror("send");
				continue;
			}else{
				writeFile(USER);
				if(send(sockfd,"success",32,0)<0)
					perror("send");
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
