#ifndef __C_H_
#define __C_H_

#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "msgops.h"
#include "../common/command.h"

//宏开关
#define DEBUG
#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF(...)
#endif

/*************************************
 *         GLOBAL PROTOTYPES		 *
 ************************************/
int curSockfd; /* 当前客户端的sockid */
char cur_ip_text[16]; /* 存储当前客户端的ip地址 */

int msg_id; /* 消息队列标识符,消息队列创建成功后的放回值,用于消息队列通信 */
key_t msg_key; /* 创建消息队列的key,由程序执行时的第四个参数传入 */
char msg_key_text[8]; /* key的字符串版本*/

char inet_ip_text[32];

void sig_ctl(int);

#endif