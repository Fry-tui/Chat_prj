#ifndef __MSGHAND_H_
#define __MSGHAND_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "client.h"

//消息队列参数结构体
typedef struct Msg{
	int choice;			/* 消息队列页面选项,emun KEY_TYPE 	*/
	char text[1024];	/* 消息队列的文本值 */
}Msg,*MsgP;

//枚举页面choice
enum KEY_TYPE{
	INULLMENU, /* 不显示任何页面存打印发送文本 */
	IMAINMENU, /* 显示主菜单 */
	IUSERMENU, /* 显示用户菜单 */
	IROOTMENU, /* 显示管理员菜单 */
};

void myMsgSend(Msg);

#endif