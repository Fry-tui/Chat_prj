#ifndef __S_H_
#define __S_H_

/************************************
 *      		INCLUDE				*
 ************************************/
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "bufops.h"
#include "msgops.h"
#include "fileops.h"
#include "linklist.h"
#include "menureact.h"
#include "appserver.h"
#include "../common/command.h"

//宏开关
#define DEBUG
#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF(...)
#endif

/************************************
 *      		DEFINE				*
 ************************************/
#define SUCCESS 1
#define FAILD 0
#define VALID 1
#define INVALID 0
#define LEGAL 0
#define ILLEGAL -1
#define LENILLEGAL 1

/************************************
 *      		ENUM				*
 ************************************/
//枚举参数种类
enum PARA_TYPE{
	USER,
	REDP,
};
//枚举关键词种类
enum KEY_TYPE{
	SOCKFD,
	USERNAME,
};
//枚举缓冲种类
enum BUF_TYPE{
	REGFORMBUF,
	LOGFORMBUF,
	SETFORMBUF,
};

/* 
	存放供system执行的指令,
	由于局部变量的大小有限制,
	所以需要特别长的指令时,存放于此.
	但是由于是全局变量,又有多线程操作,
	所以需要给一个对应的信号量,初始值给1.
	只供一个人使用,使用完释放信号量.
*/
sem_t global_sem_cmd;
char *global_command;	

#endif