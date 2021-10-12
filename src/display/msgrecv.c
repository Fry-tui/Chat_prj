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
#include "../../include/display/msgrecv.h"


void myMsgRecv(MsgP precv_msg)
{
	if(msgrcv(msg_id,(void *)precv_msg,sizeof(struct Msg),0,MSG_NOERROR)==-1){
		perror("[ \033[31mError\033[0m ] msgrecv.c/myMsgRecv() : msgrcv");
		msgctl(msg_id,IPC_RMID,0); 
		exit(EXIT_FAILURE); 
	}
	return;
}

