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

#include "fileops.h"
#include "linklist.h"
#include "msghandle.h"
#include "menureact.h"
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
};

#endif