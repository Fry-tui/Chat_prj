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

#include "../../include/server/msghandle.h"

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
			user->sockfd = -1;
			user->login_t = -1;
			//user->login_pid = -1; @[Warn]:暂时不知道会不会用到
			user->group_state = -1;
			user->online_state = -1;
			/* 关闭消息队列 */
			msg_id = atoi(user->msg_id_text); /* 获取消息队列表示符 */
			msgctl(msg_id,IPC_RMID,0); 
			killDisplay(user->msg_key_text); /* 关闭对应显示屏进程 */
			strcpy(user->msg_id_text,"");
			strcpy(user->msg_key_text,"");
			writeFile(USERNAME);	/*将修改结果写入文件 */
			
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
*                                服务器消息处理线程
*
* @Desc  : 为每个登入成功的客户端打开一个消息处理线程,对接收的消息进行二次处理
* @
* @return: 
* @Note  : 
* sem[0]-c-选项类消息
* sem[1]-f-金钱类消息
* sem[2]-n-名字类消息
* sem[3]-m-验证类消息
* sem[4]...
* sem[5]
****************************************************************************************
*/
void reactMsg()
{
	
}
