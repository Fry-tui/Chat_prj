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

void signle_display(int color,int space,int useful)
{
	int i;
	for(i=0;i<space;i++)
		printf(" ");	//输出space个空格
	printf("\033[4%dm",color);	//改变背景色
	for(i=0;i<useful;i++) 
		printf(" ");	//输出当前行的有色数据块
	printf("\033[0m\n");//清除背景色再结束当前行
	return;
}

void multi_display(int color,int num,int space[],int useful[])
{
	int i,j;
	
	for(j=0;j<num;j++){
		for(i=0;i<space[j];i++)
			printf(" ");	//输出space个空格
		printf("\033[4%dm",color);	//改变背景色
		for(i=0;i<useful[j];i++) 
			printf(" ");	//输出当前行的有色数据块
		printf("\033[0m");//清除背景色
	}
	printf("\n");//结束当前行
	return;
}


int main()
{
	int i,j;
	int arr_space[46],arr_useful[46];
	printf("%s","\033[1H\033[2J");
	printf("\n");//第一行
	printf("\n");//第二行
	for(j=0;j<2;j++){ //因为三,四行一样就一起做
		signle_display(6,9,4); //头
	}
	signle_display(6,10,2);//脖子
	signle_display(6,8,6);//肩膀
	arr_space[0] = 8;
	arr_space[1] = 1;
	arr_space[2] = 1;
	arr_useful[0] = 1;
	arr_useful[1] = 2;
	arr_useful[2] = 1;
	for(j=0;j<3;j++){
		multi_display(6,3,arr_space,arr_useful);	//手臂
	}
	signle_display(6,10,2);//腰
	signle_display(6,9,4);//屁股
	
	arr_space[0] = 9;
	arr_space[1] = 2;
	arr_useful[0] = 1;
	arr_useful[1] = 1;
	for(j=0;j<5;j++){
		multi_display(6,2,arr_space,arr_useful);	//手臂
	}
	while(1);
	return 0;
}	













