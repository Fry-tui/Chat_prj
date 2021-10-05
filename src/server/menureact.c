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

void readBuffer(int type,int cnt,struct Buffer *buffer,char inet_ip[])
{
	int i=0,j=0,k=0;
	int fd;
	char src[64];
	char buf[64],out[cnt][32];
	if(type == REGFORMBUF){
		//打开文件
		strcpy(src,"./data/ipbuffer/form_");
		strcat(src,inet_ip);
		fd = open(src, O_RDONLY);
		printf("src=%s\n",src);
		//用分隔符号去读取文件
		if(fd!=-1){	
			printf("fd!=-1\n");	
			while(read(fd,buf,sizeof(buf)) > 0){
				break;
			}
		}
		//printf("buf=%s\n",buf);

		for(i=0;i<3;i++){
			k=0;
			while((buf[j] != '|' ) && (buf[j] != '\n')){
				out[i][k++]=buf[j++];
			}
			out[i][k]='\0';
			j++;
		}
		strcpy(buffer->name,out[0]);
		strcpy(buffer->pwd,out[1]);
		strcpy(buffer->psd,out[2]);
		
		//关闭文件指针
		close(fd);
		/* @[Warn] :做info输出 */
		DPRINTF("[ \033[34mInfo\033[0m ] 用户名:%s|密码:%s|密码:%s\n",buffer->name,buffer->pwd,buffer->psd);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else{
		printf("\033[31m[Error]\033[0m bufops.c readBuffer():无法识别要解析的缓冲类型\n");
		buffer->avail_flag = ILLEGAL; /*让结构体失效 */
		return;
	}
	return;
}

/*
****************************************************************************************
*                              		注册响应函数
* @Desc  : 对客户端注册函数进行响应
* @return: 无返回值
****************************************************************************************
*/
void reactRegister(int sockfd,char inet_ip[])
{
	int i,num;
	char buf[32];
	char command[32];
	struct User user;
	struct Redp empty;
	struct Buffer *buffer;
	//接收合理的手机号 
	strcpy(user.telenumber,myRecv(sockfd));
	
	//获取验证码并发送 
	while(1){
		srand(time(NULL));
		num = rand()%9000+1000;
		sprintf(buf,"%d",num);//转化成char类型 
		if(send(sockfd,buf,8,0)<0)
			perror("send");
		
		//接收消息 
		strcpy(buf,myRecv(sockfd));
		
		//退出条件 
		if(strcmp(buf,"same")==0)
			break;
	}
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	//用户名&密码
	while(1){
		strcpy(buf,myRecv(sockfd));
		if(strcmp(buf,"0")==0){	/* 存成功 */
			//读表单数据函数
			readBuffer(REGFORMBUF,3,(void *)buffer,inet_ip);// 格式,
			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else{
				//发送读到了
				if(send(sockfd,"READOVER",32,0)<0)
					perror("send");
			}

			/* 如果接收空输入 */
			if(strcmp(buffer->name,"")==0)||(strcmp(buffer->pwd,"")==0))
			{
				if(send(sockfd,"NULL",32,0)<0)
					perror("send");
				return;
			}
			//判断数据 用户名和密码一起判断
			if(strcmp(buffer->psd,buffer->pwd)!=0){
				//告知密码不一致
				if(send(sockfd,"pwd",32,0)<0)
					perror("send");
				continue;
			}else if(reviseUserNode(USERNAME,buffer->name,0)!=NULL){
				//告知用户名已存在
				if(send(sockfd,"name",32,0)<0)
					perror("send");
			}else{
				//告知成功
				if(send(sockfd,"success",32,0)<0)
					perror("send");
				break;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果客户端退出 */
			return;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRegister():客户端中断注册\n");
			return;
		}
	}
	/*@[Warn]:说实话我觉得服务器关闭显示屏有点多余了*/
	//初始化结构体
	
	//printf("set start\n");
	strcpy(user.name,buffer->name);
	strcpy(user.password,buffer->pwd);
	strcpy(user.login_pid,"");
	strcpy(user.msg_id_text,"");
	strcpy(user.msg_key_text,"");
	
	//printf("strcpy ovrt\n");
	user.avail_flag = LEGAL;
	user.sockfd = sockfd;
	user.add_num = 0;
	user.friend_num = 0;
	user.unread_msg_num = 0;
	
	//printf("int ovrt\n");
	for(i=0;i<32;i++){
		strcpy(user.add_name[i],"");
		strcpy(user.add_msg[i],"");
	}
	for(i=0;i<128;i++){
		strcpy(user.unread_msg[i],"");
	}
	//printf("arr ovrt\n");
	user.group_state=-1;
	user.online_state=-1;
	user.balance = 0;
	user.login_t = 0;
	user.duration = 0;
	for(i=0;i<64;i++){
		sem_init(&user.sem[i],0,0);
	}
	//printf("set over\n");
	//添加节点
	addNode(USER,user,empty);
	//printf("addNode over\n");
	//存档
	writeFile(USER);
	//printf("writeFile over\n");
	if(send(sockfd,"reg_success",32,0)<0)
		perror("send");
	DPRINTF("[ \033[34mInfo\033[0m ] 用户:%s完成注册\n",user.name);
	if(send(sockfd,user.name,32,0)<0)
		perror("send");	
	/* @[Warn]:数据data文件夹新增用户文件,登入时需要添加，移除时也要 */
	strcpy(command,"mkdir ./data/");
	strcat(command,user.name);
	system(command);
	/* @[Warn]:有用到avail_flag成员的地方需要注意,默认初始值是随机数 */
	return;
}

/*
****************************************************************************************
*                              		登录响应函数
* @Desc  : 对客户端注册函数进行响应
* @return: 无返回值
****************************************************************************************
*/
void reactLogin(int sockfd,char inet_ip[])
{
	struct User user;

	//获取其余数据
	strcpy(user.login_pid,myRecv(sockfd));
	strcpy(user.msg_id_text,myRecv(sockfd));
	strcpy(user.msg_key_text,myRecv(sockfd));
	return;
}

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
			reactLogin();
		}else if(strcmp(buf,"3")==0){
			//reactSet();
		}else if(strcmp(buf,"4")==0){
			//reactDes(*sockfd); 
		}else if(strcmp(buf,"5")==0){
			//结束该客户端的响应进程
			close(*sockfd);
	        printf("[ \033[34mInfo\033[0m ] 客户端%d",*sockfd);
	        printf(" \033[33m已正常结束\033[0m\n");
	        pthread_exit(0);
		}else if(strcmp(buf,"ls")==0){
			//listUser();
		}else if(strcmp(buf,"set")==0){
			//设置状态
			//reactSet(*sockfd);
		}
		
	}
	return;
}

