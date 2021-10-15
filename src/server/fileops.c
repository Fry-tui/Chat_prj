/*
*********************************************************************************************************
*											  File_operations
*
*                                         (c) Copyright 2021/09/30
*
* @File    : filsops.c
* @Author  : yqxu
*
* @Description:
* ----------
*  文件操作函数 包括数据的存取,文件的复制等
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-09-30 :   yqxu  :     文件操作函数
* 
*********************************************************************************************************
*/

#include "../../include/server/fileops.h"

/*
****************************************************************************************
*                                  写文件
*
* @Desc  : 将用户链表和红包链表按顺序写入数据文件
* @type  : 根据传入的参数决定保存何种数据 'USER:保存用户数据' 'REDP:保存红包数据' 
* @return: 无返回值
****************************************************************************************
*/
void writeFile(int type)
{
	int fd; /* 创建文件描述符 */
	LinklistU u = U->next; /* 创建用户节点指针 */
	LinklistR r = R->next; /* 创建红包节点指针 */
	LinklistG g = G->next; /* 创建群聊节点指针 */
	
	if(type == USER){
		//保存用户数据 
		fd = open(USER_SRC,O_RDWR|O_CREAT|O_TRUNC,0777); /* 打开文件 */
		while(u){	/*遍历链表*/
			write(fd,&u->user,sizeof(struct User)); /* 往结构体里写入一个节点大小的数据 */
			u = u->next;	/* 指针后移 */
		}
	}else if(type==REDP){
		//保存红包数据 
		fd = open(REDP_SRC,O_RDWR|O_CREAT|O_TRUNC,0777);
		while(r){
			write(fd,&r->redp,sizeof(struct Redp));
			r = r->next;
		}
	}else if(type==GROUP){
		//保存红包数据 
		fd = open(GROUP_SRC,O_RDWR|O_CREAT|O_TRUNC,0777);
		while(g){
			write(fd,&g->group,sizeof(struct Group));
			g = g->next;
		}
	}else{
		printf("\033[31m[Error]\033[0m fileops.c writefile():无法识别要保存的数据类型\n");
		close(fd);
		return;
	}
	//printf("type=%d\n",type);
	close(fd); /* 关闭文件指针 */
	return;
}

/*
****************************************************************************************
*                                  读文件
*
* @Desc  : 将用户链表和红包链表按顺序写入数据文件
* @return: 无返回值
****************************************************************************************
*/
void readFile(void)
{
	int fd; /* 创建文件描述符 */
	struct User user; /* 存读到的数据并加入新节点 */
	struct Redp redp;
	struct Group group;
	fd = open(USER_SRC, O_RDONLY); /* 打开文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()U:已打开\n");
		while( read(fd,&user,sizeof(struct User))> 0){
			addNode(USER,user,redp,group);	/* 将数据存入链表 */
		}
		close(fd); /* 关闭文件指针 */
	}
	
	fd = open(REDP_SRC,O_RDONLY); /* 重新打开新文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()R:已打开\n");
		while(read(fd,&redp,sizeof(struct Redp))> 0){
			addNode(REDP,user,redp,group);
		}
		close(fd);
	}
	
	fd = open(GROUP_SRC,O_RDONLY); /* 重新打开新文件 */
	if(fd!=-1){	
		DPRINTF("[ \033[34mInfo\033[0m ] fileops.c readFile()G:已打开\n");
		while(read(fd,&group,sizeof(struct Group))> 0){
			addNode(GROUP,user,redp,group);
		}
		close(fd);
	}
	
	return;
	
}

/*
****************************************************************************************
*                                  响应发送文件函数
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void sendFile(struct User *user)
{
	int i,j,k,len;
	int f_sockfd;
	int u_sockfd;
	int fileTrans;
	int addrlen = sizeof(struct sockaddr);
	char *buf;
	char f_name[32];
	char filename[128];
	char filepath[128];
	char send_text[1024];
	char recv_text[1024];
	char server_path[128];
	struct User * fuser;
	struct sockaddr_in client;
	FILE *fp;
	buf = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	bzero(buf,BUFFER_SIZE);
	u_sockfd = user->udp_sockfd;
	f_sockfd = user->sockfd;

	/* 创建一个服务器文件缓冲区 */
	system("mkdir -p data/server/");
	
	/* 获取发送对象用户名 */
	sem_wait(&user->sem[0]);
	strcpy(f_name,user->sem_buf[0]);

	/* 判断用户是否存在 */
	fuser = (struct User *)malloc(sizeof(struct User));
	fuser = reviseUserNode(USERNAME, f_name, 0);
	if(strcmp(user->name,f_name)==0){
		if(send(f_sockfd,"-same",1024,0)<0)
			perror("send");
		sem_wait(&user->sem[0]);
		return;
	}else if(fuser==NULL){
		if(send(f_sockfd,"-none_exist",1024,0)<0)
			perror("send");
		sem_wait(&user->sem[0]);
		return;
	}else{
		if(send(f_sockfd,"-exist",1024,0)<0)
			perror("send");
	}

	
	//等待弹窗处理结果
	sem_wait(&user->sem[0]);
	strcpy(recv_text,user->sem_buf[0]);
	if(strcmp(recv_text,"0")==0){
		/* 存路径成功 */

		//发送同步信号
		if(send(f_sockfd,"-syn",1024,0)<0)
			perror("send");

		//len = recvfrom(f_sockfd,filename,128,0,(struct sockaddr *)&client,&addrlen);
		//获取文件名
		sem_wait(&user->sem[0]);
		strcpy(filename,user->sem_buf[0]);

		//拼接存放路径 data/server/xyq|sxl|test.c
		strcpy(server_path,"./data/server/");
		strcat(server_path,user->name);
		strcat(server_path,"|");
		strcat(server_path,fuser->name);
		strcat(server_path,"|");
		strcat(server_path,filename);
		//printf("len=%ld\n",strlen(filename));
		strcat(server_path,"\0");
		//printf("serverpath=%s \n",server_path);
		
		fp = fopen(server_path,"w");	//打开文件
		if(fp!=NULL){
			while(1){
				//printf("wait recv u_sockfd=%d\n",u_sockfd);
				fileTrans=recvfrom(u_sockfd,buf,BUFFER_SIZE,0,(struct sockaddr *)&client,&addrlen);
				//printf("recv_over u_sockfd:%d\n",u_sockfd);
				if(fileTrans<0){
					perror("recv");
					break;
				}
				len = fwrite(buf,sizeof(char),fileTrans,fp);

				if(fileTrans<BUFFER_SIZE){
					//printf("finish writing!\n");
					break;
				}else{
					//printf("write successful!\n");
					//break;
				}

				bzero(buf,BUFFER_SIZE);
			}
			DPRINTF("[ \033[32mOutCome\033[0m ] sendFile():文件传输完成\n");

			//通知好友
			if(fuser->online_state==1){
				//在线
				//弹窗消息        !out|许玉泉给你发了一份文件快去接收吧
				strcpy(send_text,"!out|");
				strcat(send_text,user->name);
				strcat(send_text,"给你发了一份文件,待接收");
				if(send(fuser->sockfd,send_text,1024,0)<0) /* 给对方发消息 */
					perror("send");
			}

			//不管在与不在都要写入未读消息			文件接收的未读消息:~xyq:test.c
			strcpy(send_text,"!");
			strcat(send_text,user->name);
			strcat(send_text,":");
			strcat(send_text,filename);

			/* 离线:存入未读消息 */
			strcpy(fuser->unread_msg[fuser->unread_msg_num],"~");
			strcat(fuser->unread_msg[fuser->unread_msg_num],user->name);
			strcat(fuser->unread_msg[fuser->unread_msg_num],":");
			strcat(fuser->unread_msg[fuser->unread_msg_num],filename);
			fuser->unread_msg_num++;

			writeFile(USER);
			
		}else{
			//printf("open fail errno = %d reason = %s \n", errno, strerrno(errno));
			printf("Error: %s\n", strerror(errno));
			DPRINTF("[ \033[31mError\033[0m ] sendFile():文件打开失败！\n");
			if(send(user->sockfd,"-exit",1024,0)<0) /* 给对方发消息 */
				perror("send");
			return;
		}
		//接收文件
		//printf("res=0\n");
		fclose(fp);
	}else{
		printf("[ \033[31mError\033[0m ] sendFile():用户中断文件发送\n");
		return;
	}
	//printf("exit here\n");
	sem_wait(&user->sem[0]);
	return;
}


/*
****************************************************************************************
*                                  响应接收文件函数
* @Desc  : 
* @return: 无返回值
****************************************************************************************
*/
void recvFile(struct User *user)
{
	int i,j,k,cnt=0;
	char *buf;
	int fileTrans;
	char recv_text[1024];
	char send_text[1024];
	char file_path[1024];
	char write_path[1024];
	char file_name[12][128];
	char user_name[12][32];
	char unread_clear_text[1024];
	FILE *fp;
	FILE *fpw;
    struct sockaddr_in server;
    int addrlen=sizeof(struct sockaddr);
	buf = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	//罗列属于用户的文件 未读消息 ~xxx:www.c
	for(i=0;i<user->unread_msg_num;i++){
		if(user->unread_msg[i][0]=='~'){
			j=1;
			while(user->unread_msg[i][j]!=':')
				user_name[cnt][j-1]=user->unread_msg[i][j++];
			j++;
			k=0;
			while(user->unread_msg[i][j]!='\0')
				file_name[cnt][k++] = user->unread_msg[i][j++];
			cnt++;
		}
	}

	sprintf(recv_text,"%d",cnt);
	strcpy(send_text,"-");
	strcat(send_text,recv_text);
	if(send(user->sockfd,send_text,32,0)<0)
		perror("send");

	for(i=0;i<cnt;i++){
		sem_wait(&user->sem[0]);

		strcpy(send_text,"-");
		strcat(send_text,user_name[i]);
		if(send(user->sockfd,send_text,32,0)<0)
			perror("send");

		sem_wait(&user->sem[0]);

		strcpy(send_text,"-");
		strcat(send_text,file_name[i]);
		if(send(user->sockfd,send_text,128,0)<0)
			perror("send");
	}

	while(1){
		sem_wait(&user->sem[0]);
		strcpy(recv_text,user->sem_buf[0]);

		if(strcmp(recv_text,"exit")==0){
			return;
		}

		if(strcmp(recv_text,"0")==0){
			break;
		}else{
			printf("[ \033[31mError\033[0m ] recvFile():用户中断文件接收\n");
			return;
		}
	}

	//等待弹窗处理结果
	sem_wait(&user->sem[0]);
	strcpy(recv_text,user->sem_buf[0]);
	if(strcmp(recv_text,"0")==0){
		/* 存路径成功 */
		
		//发送同步信号
		if(send(user->sockfd,"-syn",1024,0)<0)
			perror("send");
		//接收文件作者
		sem_wait(&user->sem[0]);
		strcpy(recv_text,user->sem_buf[0]);
		
		strcpy(unread_clear_text,"~");
		strcat(unread_clear_text,recv_text);
		strcat(unread_clear_text,":");
		
		strcpy(file_path,"./data/server/");
		strcat(file_path,recv_text);
		strcat(file_path,"|");
		strcat(file_path,user->name);
		strcat(file_path,"|");
		
		if(send(user->sockfd,"-syn",1024,0)<0)
			perror("send");
		//接收文件名
		sem_wait(&user->sem[0]);

		strcat(unread_clear_text,recv_text);
		
		strcpy(recv_text,user->sem_buf[0]);
		strcat(file_path,recv_text);

		
		printf("filepath=%s\n",file_path);

		//接收存储路径
		sem_wait(&user->sem[0]);
		strcpy(write_path,user->sem_buf[0]);
		fpw = fopen(write_path,"w");
		printf("write_path=%s\n",write_path);
		fp = fopen(file_path,"r");	//打开文件
		if(fp!=NULL){
			cnt = 0;
			while((fileTrans = fread(buf,sizeof(char),BUFFER_SIZE,fp))>0){
				
				fwrite(buf,sizeof(char),fileTrans,fpw);
				printf("cnt=%d\n",cnt++);
				sleep(1);
				//printf("udp_sockfd=%d\n",user->udp_sockfd);
				//printf("server.sinport=%d\n",user->server.sin_port);
				/*
				if(sendto(user->udp_sockfd,buf,fileTrans,0,(struct sockaddr *)&user->server,addrlen)<0)
					break;
				*/
				//printf("udp_sockfd=%d\n",user->udp_sockfd);
				//printf("send successful!\n");
				sprintf(recv_text,"%d",cnt);
				strcpy(send_text,"-");
				strcat(send_text,recv_text);
				if(send(user->sockfd,send_text,1024,0)<0)
					perror("send");

				
				if(fileTrans < BUFFER_SIZE){
					if(send(user->sockfd,"-cpover",1024,0)<0)
						perror("send");
					break;
				}
				
				bzero(buf,BUFFER_SIZE);
			}
		
			DPRINTF("[ \033[32mOutCome\033[0m ] recvFile():文件传输完成\n");

			//清除未读消息 未读消息:~xyq:test.c
			for(i=0;i<user->unread_msg_num;i++){
				if(strcmp(user->unread_msg[i],unread_clear_text)==0){
					for(j=i;j<user->unread_msg_num-1;j++){
						strcpy(user->unread_msg[j],user->unread_msg[j+1]);
					}
					user->unread_msg_num--;
					break;
				}
			}
				
			writeFile(USER);
		}else{
			//printf("open fail errno = %d reason = %s \n", errno, strerrno(errno));
			printf("Error: %s\n", strerror(errno));
			DPRINTF("[ \033[31mError\033[0m ] sendFile():文件打开失败！\n");
			if(send(user->sockfd,"-exit",1024,0)<0) /* 给对方发消息 */
				perror("send");
			return;
		}
		//接收文件
		//printf("res=0\n");
		fclose(fp);
		fclose(fpw);
	}else{
		printf("[ \033[31mError\033[0m ] sendFile():用户中断文件接收\n");
		return;
	}
	
	return;
}


