#ifndef __FILEOPS_H_
#define __FILEOPS_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include "server.h"

/************************************
 *      		DEFINE				*
 ************************************/
#define USER_SRC "./data/user.dat"
#define REDP_SRC "./data/redp.dat"
#define GROUP_SRC "./data/group.dat"

struct User * init_user_file;

/************************************
 *      	  FUNCTION				*
 ************************************/
void readFile(void);
void writeFile(int);
void sendFile(struct User *user);

#endif