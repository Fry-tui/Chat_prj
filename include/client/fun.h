#ifndef __FUN_H_
#define __FUN_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "client.h"

//普通用户功能
int setPwd(void);
void priChat(void);
void groupChat(void);
void recvFile(void);
void sendFile(void);
void addFriend(void);
void joinGroup(void);
void listAddMsg(void);
void listGroups(void);
void listFriends(void);
void createGroup(void);
void listMyGroups(void);
void disposeAddMsg(void);

int strCnlen(char[]);
void alignLeft(char[]);
void middleText(char[]);

//管理员功能
void rmUser(void);
void offLineUser(void);
void closeServer(void);
void bcAnnouncement(void);

#endif

