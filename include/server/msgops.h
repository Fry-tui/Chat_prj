#ifndef _MSGOPS_H
#define _MSGOPS_H

/************************************
 *      		INCLUDE				*
 ************************************/
#include "server.h"

struct User *init_user_msgops;

char *myRecv(int);
void pthread_Recv(struct User *user);

#endif