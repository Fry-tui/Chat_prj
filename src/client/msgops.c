/*
*********************************************************************************************************
*											  Message Handling
*
*                                         (c) Copyright 2021/10/02
*
* @File    : msghand.c
* @Author  : yqxu
*
* @Description:
* ----------
*  处理客户端和[--服务器--]进行套接字通信时的    接收函数
*  处理客户端与[ -显示屏- ]进行消息队列通信时的  发送函数
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-02 :   yqxu  :   消息处理函数
* 
*********************************************************************************************************
*/

#include "../../include/client/msgops.h"

/*
****************************************************************************************
*                               消息队列发送函数
*
* @Desc  : 判断发给显示屏的函数是否成功
* @buf	 : 需要发送的文本
* @Note  : 失败就会杀死显示屏进程,关闭消息队列
* @Note	 : 结构体send_msg一定不能有空指针成员
* @return: 无返回值
****************************************************************************************
*/
void myMsgSend(Msg send_msg)
{
	if(msgsnd(msg_id,(void *)&send_msg,sizeof(send_msg),0)==-1){
		perror("[ \033[31mError\033[0m ] client/menu.c : msgsnd");
		killDisplay(msg_key_text);
		msgctl(msg_id,IPC_RMID,0); 
		exit(EXIT_FAILURE); 
	}
	return;
}

/*
****************************************************************************************
*                               	套接字接收函数线程
*!!!暂时废弃,因为设计该函数在特定情况下会运行出bug,只有改成线程函数才能解决问
* 	题,但是这样的话,对于原来的写法没有升级,所以暂时不需要
*---重新启用，需要在本地弹出实时弹出弹窗,就需要预判处理
* @Desc  : 客户端登入成功后,所有接收到的消息都要经过这个函数处理后使用
* 			可能会接收到各类消息,但是调用的失火
* @buf	 : 需要发送的文本
* @Note  : 失败就会杀死显示屏进程,关闭消息队列
* @Note	 : 结构体send_msg一定不能有空指针成员
* @return: 无返回值
****************************************************************************************
*/
void pthread_Recv(void)
{
	int i,len;
	char buf[1024],msg[1024];
	char command[1024];
	pthread_detach(pthread_self());	/*修改线程资源等级,意外结束时自动释放资源*/
	Msg msg_send = {1,"none"}; /* 用于存放需要通过消息队列传递的消息 */

	while(1){
		len = recv(curSockfd,buf,1024,0);

		//printf("收到的消息:%s\n",buf);

		//接收结果发送到显示屏调试
		/*
		msg_send.choice=INULLMENU;
		strcpy(msg_send.text,"\033[33m#\033[0mpRecv msg:");
		strcat(msg_send.text,buf);
		strcat(msg_send.text,"\n\0");
		myMsgSend(msg_send);
		*/
		if(len<0){
			perror("client/msgops.c() pthread_Recv--recv_len");
			close(curSockfd);	/* 关闭套接字 */

			/* @[Warn]:重写服务器终止信号,下线所有用户 */

			//关闭主进程
			system("zenity --error --text=服务器崩溃,你已被强制下线☠ --no-wrap --title=⚠");
			exit(EXIT_FAILURE);

			//如果接收线程未退出
			pthread_exit(NULL);
		}else if(len==0){
			close(curSockfd);	/* 关闭套接字 */

			//关闭主进程
			system("zenity --error --text=服务器崩溃,你已被强制下线☠ --no-wrap --title=⚠");
			exit(EXIT_FAILURE);

			//如果接收线程未退出
			pthread_exit(NULL);
		}

		//消息去头
		len = strlen(buf);
		for(i=1;i<len;i++){
			msg[i-1] = buf[i];
		}	
		msg[i-1]='\0';

		//消息正常
		if(buf[0]=='-'){
			//普通消息
			strcpy(global_sem_buf,msg);
			sem_post(&global_sem);
		}else if(buf[0]=='!'){
			//弹窗消息
			zenityOps(msg);
		}else if(buf[0]=='@'){
			//私聊消息:左对齐打印到显示屏
			
			msg_send.choice=INULLMENU;

			strcpy(msg_send.text,"\033[32m");
			myMsgSend(msg_send);

			alignLeft(msg);

			strcpy(msg_send.text,"\033[0m");
			myMsgSend(msg_send);

			middleText(" ");	/* 换行 */
		}else{
			//未识别消息
			strcpy(command,"zenity --warning --no-wrap --title=警报 --text=接收到非法消息:");
			strcat(command,buf);
			system(command);
		}
	}

	return;
}

void zenityOps(char buf[])
{
	int i=0,j=0,len=0,res=256;
	char type[64]; /* 拆分buf里的弹窗类型 */
	char name[64]; /* 拆分buf里的用户名 */
	char text[1024]; /* 可以存放操作结果 */
	char command[1024];
	char send_text[1024];

	while(buf[i]!='|'){
		type[i] = buf[i];
		i++;
	}
	type[i] = '\0';

	if(strcmp(type,"addRequire")==0){
		i++;
		j=0;
		while(buf[i]!='\0'){
			name[j++] = buf[i++];
		}
		name[j] = '\0';

		strcpy(command,"zenity --question --title=验证消息 --ok-label=同意 --cancel-label=拒绝 --text=");
		strcat(command,name);
		strcat(command,"请求添加你为好友");

		res = system(command);

		//把处理结果给到服务器,服务器进行makefriends处理,然后返回结果
		sprintf(buf,"%d",res);
		strcpy(send_text,"a");
		strcat(send_text,buf);
		strcat(send_text,"|");
		strcat(send_text,name);
		/* 实例 a0|许玉泉 	a:添加结果类信息 0:添加选择确定 |:拼接字符 许玉泉:添加对象*/
		if(send(curSockfd,send_text,1024,0)<0)
			perror("send");
	}else if(strcmp(type,"addGroup")==0){
		i++;
		j=0;
		while(buf[i]!='\0'){
			text[j++] = buf[i++];
		}
		text[j] = '\0';
		/* text:许玉泉:请求加入-群聊1 */
		strcpy(command,"zenity --question --title=验证消息 --ok-label=同意 --cancel-label=拒绝 --text=");
		strcat(command,text);

		res = system(command);

		//把处理结果给到服务器,服务器进行makefriends处理,然后返回结果
		sprintf(buf,"%d",res);
		strcpy(send_text,"b");
		strcat(send_text,buf);
		strcat(send_text,"|");
		strcat(send_text,text);
		/* 实例 b0|许玉泉:请求加入-闲聊群1 	a:添加结果类信息 0:添加选择确定 |:拼接字符 添加对象:添加信息*/
		if(send(curSockfd,send_text,1024,0)<0)
			perror("send");
	}else if(strcmp(type,"out")==0){
		/* 示例: !out|text  */
		i++;
		j=0;
		while(buf[i]!='\0'){
			text[j++] = buf[i++];
		}
		text[j] = '\0';
		strcpy(command,"zenity --info --no-wrap --title=消息  --text=提示");
		strcat(command,text);

		res = system(command);
	}else{
		printf("[ \033[31mError\033[0m ] pthread_Recv : 收到无法识别类型的弹窗信息:!%s\n",buf);
	}
	return;
}


/*
****************************************************************************************
*                                 	读取缓冲数据(file_seceltion)
*
* @Desc  : 参考server.c临时改动
****************************************************************************************
*/
void readBuffer(int type,int cnt,char suffix[],struct Buffer *buffer,char inet_ip[])
{
	int i;
	int fd;				/* 接收文件打开结果的标识符 */
	int res=1;			/* 存放文件读取的结果，-1:失败;0:read前光标已末尾;>0:读取到的数据长度 */
	char src[64];		/* 存放拼接好的路径:比如'./data/ipbuffer/127.0.0.1_form' */
	char buf[1024],out[cnt][32]; /* buf:暂存字符串	out:数据输出字符串数组	*/
	memset(buf,0,1024);
	/* 打开文件 */
	//路径格式:"./data/ipbuffer/[inet_ip][suffix]"
	//示例路径:"./data/ipbuffer/127.0.0.1_form"
	strcpy(src,"./data/ipbuffer/");
	strcat(src,inet_ip);
	strcat(src,suffix);
	
	fd = open(src, O_RDONLY);	/* 只读打开,获取文件描述符 */
	//DPRINTF("[ \033[34mInfo\033[0m ] 缓冲路径:%s|读取结果:%d\n",src,fd);
	
	/* @[Warn]:其实判断长度过长也还是有bug如果输入|抑或是用户名110位后面都合格也会通过 */
	/* 解决了一半,一旦数据读了两次,直接退出,言下之意总数据超过96个 */
	if(fd!=-1){					/* 判断打开成功与否 */
		//printf("fd!=-1\n");	
		while(res>0){	/* 一次直接读取1024位的数据 */
			res = read(fd,buf,sizeof(buf));
			if(res==-1){
				printf("[ \033[31mError\033[0m ] msgops.c readBuffer():读取失败[res:%d]\n",res);
				buffer->avail_flag = ILLEGAL; /* 失效结构体 */
				return;
			}
		}
	}
	
	strcpy(buffer->src,buf);

	//做处理 buf含有一个\n需要处理掉
	//printf("msglen=%d\n",strlen(buffer->src)-1);
	buffer->src[strlen(buffer->src)-1] = '\0';

	//使其有效
	buffer->avail_flag = LEGAL;

	//关闭文件指针
	close(fd);
	return;
}
