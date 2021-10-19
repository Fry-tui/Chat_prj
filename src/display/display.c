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
	Msg recv_msg = {1,"none"}; /* 接收客户端传递过来的参数 */
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
			//分隔消息:recv_msg.text;"add_num|unread_msg_num" -> 直接交给该函数自己完成
			displayUserMenu(recv_msg.text); 
		}else if(recv_msg.choice==IROOTMENU){
			displayRootMenu();
		}else if(recv_msg.choice==INULLMENU){
			printf("%s",recv_msg.text);
		}else{
			printf("[ \033[31mError\033[0m ] display.c/main() :recv_msg.choice:%d 无法识别\n",recv_msg.choice);
			printf("%s","\033[1H\033[2J");
		}
	}
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

void displayUserMenu(char num_text[])
{
	int i=0,j=0;
	char add_num[8],unread_msg_num[8];
	/* 分隔文件,分割条件:| */
	while(num_text[i]!='|'){
		add_num[i] = num_text[i];
		i++;
	}
	add_num[i]='\0';
	i++;
	while(num_text[i]!='\0'){
		unread_msg_num[j++] = num_text[i];
		i++;
	}
	unread_msg_num[j]='\0';
	printf("%s","\033[1H\033[2J"); 
	printf ("\t----------\33[34m菜\33[0m-----------------\n");
	printf ("\t--------------\33[34m单\33[0m-------------\n");
	printf ("\t|\t#  验证消息%s\t    |\n",add_num);
	printf ("\t|\t@  未读消息%s\t    |\n",unread_msg_num);
	printf ("\t|\t*  刷新消息键\t    |\n");
	printf ("\t|\t1   罗列闺蜜 \t    |\n");
	printf ("\t|\t2   闺中密谈 \t    |\n");
	printf ("\t|\t3   快乐建群 \t    |\n");
	printf ("\t|\t4   加入群聊 \t    |\n");
	printf ("\t|\t5   找群叭叭 \t    |\n");
	printf ("\t|\t6   豪气一下 \t    |\n");
	printf ("\t|\t7   手气红包 \t    |\n");
	printf ("\t|\t8   添加好友 \t    |\n");
	printf ("\t|\t9   充值金额 \t    |\n");
	printf ("\t|\t10  查询余额\t    |\n");
	printf ("\t|\t11  修改密码\t    |\n");
	printf ("\t|\t12  删除好友\t    |\n");
	printf ("\t|\t13  一起八卦\t    |\n");
	printf ("\t|\t14  发送文件\t    |\n");
	printf ("\t|\t15  接收文件\t    |\n");
	printf ("\t|\t16  注销用户\t    |\n");
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
	printf ("\t|\t3罗列所有用户\t    |\n");
	printf ("\t|\t4广播发送公告\t    |\n");
	printf ("\t|\t5罗列所有群聊\t    |\n");
	printf ("\t|\t6远程关闭服务器\t    |\n");
	printf ("\t|\texit:退出管理\t    |\n");
	printf ("\t----------\33[34m页\33[0m-----------------\n");
	printf ("\t--------------\33[34m面\33[0m-------------\n");
	return;
}