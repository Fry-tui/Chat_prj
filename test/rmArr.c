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

int strCnlen(char msg[])
{
	int j=0,count=0,msgLen=0;
	for(j=0;msg[j];j++){
		if(msg[j] & 0x80)
			count++;
		else
			msgLen++;
		if(count==3){
			count=0;
			msgLen+=2;
		}
	}
	msgLen--;
	return msgLen;
}

void middleText(char text[])
{
	int i,j,k;
	int cnLen;
	char buf[1024];
	
	cnLen=strCnlen(text);		//字符个数
	
	strcpy(buf,"|");	/* 拼头 */
	
	for(i=(41-cnLen)/2;i>0;i--)	
		strcat(buf," ");	/* 拼前空 */
		
	strcat(buf,text);
	
	for(i=(41+cnLen)/2;i<41;i++)
		strcat(buf," ");	/* 拼后空 */
	
	strcat(buf,"|\n\0");	/* 拼尾 */
	
	printf("%s",buf);
	return;
}

void alignLeft(char text[])
{	
	int i=1,j=0,k=0;
	int cnt=0,ent=1;
	int enLen,cnLen;
	char buf[1024];
	
	enLen=strlen(text);			//字符长度
	cnLen=strCnlen(text);		//字符个数
	
	if(cnLen<43){	
		strcpy(buf,"|");	/* 拼头 */
		strcat(buf,text);	/* 拼文本 */
		for(i=cnLen;41-i>0;i++)
			strcat(buf," ");
		strcat(buf,"|\n\0");/* 拼尾 */
		printf("%s",buf);
	}else{
		strcpy(buf,"|");	/* 拼头 */
		
		while(j<enLen){
			/* 计算个数:处理行末 */
			if(text[j] & 0x80)
				cnt++;
			else
				ent++;
			
			/* 逐一赋值 */
			buf[i++]=text[j++];
			
			/* 计数进位 */
			if(cnt>2){
				cnt = 0;
				ent += 2;
			}
			
			if((cnt==0)&&(ent>40&&ent<43)){
				//printf("ent=%d\n",ent);
				/* 只有中文完整的输出后 并且到了行末 剩下的空格补位*/
				for(ent=43-ent;ent>0;ent--){
					buf[i++] = ' ';
				}
				buf[i++] = '|';
				buf[i++] = '\n';
				buf[i++] = '|';
			}
		}
		for(ent = 42-ent;ent>0;ent--)
			buf[i++] = ' ';
		buf[i++] = '|';
		buf[i++] = '\n';
		buf[i++] = '\0';
		printf("%s",buf);
	}
	return;
}

int main()
{
	int i,j,k;
	char buf[1024];
	char name[32] = "许";

	while(1){
		printf("请输入:");
		scanf("%s",name);
		printf("%s","\033[1H\033[2J"); 
		printf(" __________________________________________\n");
		//middleText(name);		
		middleText(" ");
		printf("|^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|\n");
		middleText("暂无消息");
		middleText(" ");
		alignLeft(name);
		middleText(" ");
		alignLeft(name);
		middleText(" ");
	}
	return 0;
}




























