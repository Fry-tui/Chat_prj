#ifndef __COMMAND_H_
#define __COMMAND_H_
/*@[Warn]:用不到的很多头文件可以删除*/
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

//宏开关
#define DEBUG
#ifdef DEBUG
#define DPRINTF printf
#else
#define DPRINTF(...)
#endif

void killDisplay(char[]);
void infoCrtCode(char code[]);
void createDisplay(char[],int argc,char *argv[]);

#endif
