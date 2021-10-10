#ifndef __DISPLAY_H_
#define __DISPLAY_H_

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

#include "msgrecv.h"

//宏开关
#define DEBUG
#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF(...)
#endif

int msg_id; /* 存放消息队列标识符 */

void displayMainMenu(void);
void displayRootMenu(void);
void displayUserMenu(char[]);

#endif