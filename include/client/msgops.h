#ifndef __MSGHAND_H_
#define __MSGHAND_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "client.h"

//消息队列参数结构体
typedef struct Msg{
	int choice;
	char text[1024];
}Msg,*MsgP;

//枚举页面choice
enum KEY_TYPE{
	INULLMENU,
	IMAINMENU,
	IUSERMENU,
	IROOTMENU,
};

void myMsgSend(Msg);

#endif