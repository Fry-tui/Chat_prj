/*
*********************************************************************************************************
*												  display.c
*                                         (c) Copyright 2021/10/02
* @File    : display.c
* @Author  : yqxu
*
* @Description:
* ----------
*  根据接收到的消息,进行各页面的显示 
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-02 :   yqxu  :    客户端显示屏
* system("ps -ef | grep ./display | grep -v grep | awk '{print $2}' >> pid_buffer");
* →通过指令获取进程号并存入本地文件中
*********************************************************************************************************
*/
#include "../../include/display/display.h"

/*
****************************************************************************************
*                                  					主函数
* @argv[0]:cmd
* @argv[1]:msg_id  创建消息队列的key值
* @return : 无返回值
****************************************************************************************
*/
int main(int argc,char *argv[])
{
	Msg recv_msg = {-1,"none"}; /* 接收客户端传递过来的参数 */
	key_t msg_key = (key_t)atoi(argv[1]);	/* 获取消息队列的key值 */
	//打开|创建消息队列
	msg_id = msgget(msg_key,0777|IPC_CREAT); /*msg_key:消息队列关联的键值 IPC_CREAT没有就创建及存取权限 */
	if(msg_key == -1){
		perror("[ \033[31mError\033[0m ] display/display.c main(): msgget");
		exit(EXIT_FAILURE);
	}
	/* @[Warn]:需要执行清空消息队列的操作 */
	while(1){
		myMsgRecv(&recv_msg);
		//printf("recv_msg.choice=%d recv_msg.text=%s\n",recv_msg.choice,recv_msg.text);
		if(recv_msg.choice==IMAINMENU){
			displayMainMenu();
		}else if(recv_msg.choice==IUSERMENU){
			displayUserMenu(2,2);
		}else if(recv_msg.choice==IROOTMENU){
			displayRootMenu();
		}else if(recv_msg.choice==INULLMENU){
			printf("%s",recv_msg.text);
		}else{
			DPRINTF("[ \033[31mError\033[0m ] display.c/main() :recv_msg.choice:%d 无法识别\n",recv_msg.choice);
			printf("%s","\033[1H\033[2J");
		}
	}
	//循环等待
	displayRootMenu();
	while(1);
	return 0;
}

void displayMainMenu(void)
{
	printf("%s","\033[1H\033[2J"); 
	printf ("\t----------诚----------------\n");
	printf ("\t--------------毅------------\n");
	printf ("\t|\t1 名媛认证\t   |\n");	//注册 
	printf ("\t|\t2 名媛上线\t   |\n");	//登入 
	printf ("\t|\t3 重新认证\t   |\n");	//找回密码 
	printf ("\t|\t4 贬回凡人\t   |\n");	//注销账号 
	printf ("\t|\t5 名媛小憩\t   |\n");	//关闭窗口 
	printf ("\t|\t\t\t   |\n");
	printf ("\t----------名----------------\n");
	printf ("\t--------------媛------------\n");
	return;
}

void displayUserMenu(int add_num,int unread_msg_num)
{
	printf("%s","\033[1H\033[2J"); 
	printf ("\t----------\33[34m菜\33[0m-----------------\n");
	printf ("\t--------------\33[34m单\33[0m-------------\n");
	printf ("\t|\t#  验证消息%d\t    |\n",add_num);
	printf ("\t|\t@  未读消息%d\t    |\n",unread_msg_num);
	printf ("\t|\t*  刷新消息键\t    |\n");
	printf ("\t|\t1  罗列闺蜜团\t    |\n");
	printf ("\t|\t2  闺蜜私房话\t    |\n");
	printf ("\t|\t3  名媛拼夕夕\t    |\n");
	printf ("\t|\t4  给闺蜜转账\t    |\n");
	printf ("\t|\t5  首冲大礼包\t    |\n");
	printf ("\t|\t6  图大伙一乐\t    |\n");
	printf ("\t|\t7  拼手气时刻\t    |\n");
	printf ("\t|\t8  寻找真闺蜜\t    |\n");
	printf ("\t|\t10  查询余额\t    |\n");
	printf ("\t|\texit 退出登入\t    |\n");
	printf ("\t----------\33[34m功\33[0m-----------------\n");
	printf ("\t--------------\33[34m能\33[0m-------------\n");
	return;
}

void displayRootMenu(void)
{
	printf("%s","\033[1H\033[2J"); 
	printf("尊敬的管理员:\033[33mroot\033[0m,你好！\n");
	printf ("\t----------\33[34m管\33[0m-----------------\n");
	printf ("\t--------------\33[34m理\33[0m-------------\n");
	printf ("\t|\t1删除指定用户\t    |\n");
	printf ("\t|\t2下线指定用户\t    |\n");
	printf ("\t|\t3罗列在线用户\t    |\n");
	printf ("\t|\t4罗列离线用户\t    |\n");
	printf ("\t|\t5广播发送公告\t    |\n");
	printf ("\t----------\33[34m页\33[0m-----------------\n");
	printf ("\t--------------\33[34m面\33[0m-------------\n");
	return;
}