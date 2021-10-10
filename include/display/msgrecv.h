#ifndef __MSGRECV_H
#define __MSGRECV_H

/************************************
 *      		INCLUDE				*
 ************************************/
#include "display.h"

//消息队列参数结构体
typedef struct Msg{
	int choice;
	char text[1024];
}Msg,*MsgP;

//枚举页面choice
enum KEY_TYPE{
	INULLMENU=2,
	IMAINMENU,
	IUSERMENU,
	IROOTMENU,
};

#endif