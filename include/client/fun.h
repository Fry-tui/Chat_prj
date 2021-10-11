#ifndef __FUN_H_
#define __FUN_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "client.h"

//普通用户功能
int setPwd(void);
void addFriend(void);
void listAddMsg(void);
void listFriends(void);
void disposeAddMsg(void);

//管理员功能
void offLineUser(void);
void closeServer(void);

void rmUser(void);

#endif

