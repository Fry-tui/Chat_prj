/*
*********************************************************************************************************
*												FUNCTION REACT
*
*                                         (c) Copyright 2021/10/07
*
* @File    : funserver.c
* @Author  : yqxu
*
* @Description:
* ----------
*  对功能函数进行响应
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-07 :   yqxu  :   响应功能函数操作
* 
*********************************************************************************************************
*/
#include "../../include/server/funserver.h"


/*
****************************************************************************************
*                                  修改密码
* @Desc  : 修改用户密码
* @return: 放回操作结果
****************************************************************************************
*/
int setPwd(struct User *user)
{
	int sockfd;
	char buf[1024];
	struct Buffer *buffer;

	sockfd = user->sockfd;
	buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
	
	while(1){
		DPRINTF("[ \033[34mInfo\033[0m ] 等待表单执行结果\n");
		sem_wait(&user->sem[0]);
		DPRINTF("[ \033[34mInfo\033[0m ] 已读取,准备接收数据\n");
		strcpy(buf,user->sem_buf[0]);
		DPRINTF("[ \033[34mInfo\033[0m ] 执行结果:%s|读取缓冲就绪\n",buf);
		if(strcmp(buf,"0")==0){	/* 存入buffer成功 */
			//读表单数据函数
			readBuffer(SETFORMBUF,2,"_form",(void *)buffer,user->inet_ip_text);
			DPRINTF("[ \033[34mInfo\033[0m ] 读表单数据完成\n");
			if(buffer->avail_flag==ILLEGAL){
				//发送不合法
				if(send(sockfd,"ILLEGAL",32,0)<0)
					perror("send");
				return;
			}else if(buffer->avail_flag==LENILLEGAL){
				//发送长度不合法
				if(send(sockfd,"LENILLEGAL",32,0)<0)
					perror("send");
				continue;
			}else{
				//发送读到了
				if(send(sockfd,"READOVER",32,0)<0)
					perror("send");
			}

			/* 如果接收空输入 */
			if((strcmp(buffer->pwd,"")==0)||(strcmp(buffer->pwd,"")==0))
			{
				if(send(sockfd,"NULL",32,0)<0)
					perror("send");
				continue;
			}
			//判断数据 
			if(strcmp(buffer->pwd,user->password)!=0){
				//告知旧密码
				if(send(sockfd,"old_error",32,0)<0)
					perror("send");
				continue;
			}else if(strcmp(buffer->psd,buffer->pwd)==0){
				//告知新旧密码一致
				if(send(sockfd,"same",32,0)<0)
					perror("send");
				continue;
			}else{
				//告知成功
				strcpy(user->password,buffer->psd);
				if(send(sockfd,"success",32,0)<0)
					perror("send");
				break;
			}
		}else if(strcmp(buf,"recv_error")==0){ /* 如果客户端因为接收异常退出退出 */
			printf("[ \033[31mError\033[0m ] funserver.o setPwd:客户端收到无法解析的数据,异常退出\n");
			return FAILD;
		}else{	/* 存失败 */
			printf("[ \033[31mError\033[0m ] reactRegister():客户端中断修改\n");
			return FAILD;
		}
	}
	return SUCCESS;
}

