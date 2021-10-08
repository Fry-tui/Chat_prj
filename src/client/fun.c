/*
*********************************************************************************************************
*												FUNCTION 
*
*                                         (c) Copyright 2021/10/07
*
* @File    : fun.c
* @Author  : yqxu
*
* @Description:
* ----------
*  功能函数集合
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-07 :   yqxu  :     功能函数集合
* 
*********************************************************************************************************
*/
#include "../../include/client/fun.h"
/*
****************************************************************************************
*                                  修改密码
* @Desc  : 修改用户密码
* @return: 放回操作结果
****************************************************************************************
*/
int setPwd(void)
{
    int res;
    char buf[1024],send_buf[1024];;
    char command[1024];
    Msg msg_send = {-1,"none"};

    msg_send.choice=INULLMENU;
	strcpy(msg_send.text,"\033[1H\033[2J");
	myMsgSend(msg_send);
	strcpy(msg_send.text,"\t-----------修改密码-----------\n");
	myMsgSend(msg_send);

    while(1){
        strcpy(command,"zenity --forms --text=修改密码 --add-password=原密码 --add-password=新密码 > ./data/ipbuffer/");
		strcat(command,inet_ip_text);
		strcat(command,"_form");
		res = system(command); /* 0:成功 256:退出 */

        sprintf(buf,"%d",res);
		strcpy(send_buf,"-");
		strcat(send_buf,buf);
		if(send(curSockfd,send_buf,32,0)<0)
			perror("send");

        if(res == 0){ /*成功 */
			//等待服务器读取数据结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
			if(strcmp(buf,"ILLEGAL")==0){	/* 没有读到消息 */
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c setPwd():表单数据缓冲失败,即将终止操作\n");
				myMsgSend(msg_send);
				sleep(1);
				return;
			}else if(strcmp(buf,"LENILLEGAL")==0){ 
				/*长度违法*/
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入数据过长\" --no-wrap --title=修改密码");
				continue;
			}
			
			//等待服务器读取判断结果
			if(recv(curSockfd,buf,32,0)<0)
				perror("recv");
			if(strcmp(buf,"old_error")==0){	/* 接收到old_error代表原密码有误 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 原密码 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"原密码有误\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"same")==0){ /* 接收到same代表新旧密码相同 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 新密码 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"新密码不得与原密码相同\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"NULL")==0){ /* 有表单输入为空 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 输入 \033[31mX\033[0m\n");
				myMsgSend(msg_send);
				system("zenity --error --text=\"输入框不能为空\" --no-wrap --title=修改密码");
				continue;
			}else if(strcmp(buf,"success")==0){ /* 接收到success代表成功 */
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 原密码 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
				strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 新密码 \033[32m✔\033[0m\n");
				myMsgSend(msg_send);
                system("zenity --error --text=\"修改成功\" --no-wrap --title=修改密码");
                
				break;
			}else{ /*代表接收到了其他send,报错终止注册退出*/
				strcpy(msg_send.text,"\033[31m[Error]\033[0m fun.c setPwd():无法识别");
				strcat(msg_send.text,buf);
				strcat(msg_send.text,",即将终止修改操作\n");
				myMsgSend(msg_send);
				//同时告诉服务器退出响应注册函数
				if(send(curSockfd,"-recv_error",32,0)<0)
					perror("send");
				return FAILD;
			}
		}else{
			//失败:256
			strcpy(msg_send.text,"\033[33m#\033[0msystem msg: 修改操作 \033[31m已被中断,exiting..\033[0m\n");
			myMsgSend(msg_send);
			sleep(1);
			return FAILD;
		}	
        
    }
    return SUCCESS;
}