#ifndef __FUNSERVER_H_
#define __FUNSERVER_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "server.h"

struct User * init_user_fun;

<<<<<<< HEAD
//普通用户
int setPwd(struct User *user);
void addFriend(struct User * user);
void listAddMsg(struct User * user);
void disposeAddMsg(struct User * user);

//管理员
void closeServer(int);
void offLineUser(int,char[]);
=======
int setPwd(struct User *user);
>>>>>>> d74237340dcc58332f739d9a56481c6f68ee272d


#endif

