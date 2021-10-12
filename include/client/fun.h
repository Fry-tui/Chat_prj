#ifndef __FUN_H_
#define __FUN_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "client.h"

//普通用户功能
int setPwd(void);
void priChat(void);
void addFriend(void);
void listAddMsg(void);
void listFriends(void);
void disposeAddMsg(void);

int strCnlen(char[]);
void alignLeft(char[]);
void middleText(char[]);

//管理员功能
void rmUser(void);
void offLineUser(void);
void closeServer(void);


#endif

