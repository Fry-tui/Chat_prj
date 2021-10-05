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
*  处理客户端和服务器进行套接字通信时的接收函数
*  处理客户端与显示屏进行消息队列通信时的发送函数
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-02 :   yqxu  :   消息处理函数
* 
*********************************************************************************************************
*/

#include "../../include/client/msghandle.h"

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
