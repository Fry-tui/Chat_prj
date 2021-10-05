#include "../../include/common/command.h"
/*
****************************************************************************************
*                                  创建显示屏函数
* @Desc  : 根据参数封装打开新终端的指令,并执行.		
* @格式	 : gnome-terminal -e "./bin/display [消息队列关键字]" --geometry=长x宽+X+Y --title=[ip地址] 
* @实例	 : command = gnome-terminal -e "./bin/display 1234" --geometry=30x25+100+100 --title=1234 
* @argc	 : 参数个数
* @argv  : 选项值 例如 argv[0]="30x25+100+100" argv[1]="1234"
* @return: 无返回值
****************************************************************************************
*/
void createDisplay(char id_text[],int argc,char *argv[])
{
	int i;
	char command[BUFSIZ]; /* system函数参数 存放需要执行的指令 */
	char options[argc][32]; /* 存放选项名 */
	
	/* 准备选项名 */
	strcpy(options[0],"--geometry=");
	strcpy(options[1],"--title=");
	
	strcpy(command,"gnome-terminal -e \"./bin/display ");	/* 写入指令gnome-terminal 选项-e 
															   执行语句:./bin/display */
	strcat(command,id_text);	/* 写入消息队列关键字,作为新端口 执行语句的参数传入 */
	strcat(command,"\" "); /*写入"与空格,区分开下一选项*/

	//循环输入选项名与选项值
	for(i;i<argc;i++){
		strcat(command,options[i]); /* 写入选项 */
		strcat(command,argv[i]); /* 写入值 */
		strcat(command," "); /* 写入空格 */
	}
	//DPRINTF("command:%s\n",command);
	system(command);
}

/*
****************************************************************************************
*                                  关闭显示屏函数
* @Desc  : 根据参数封装打开新终端的指令,并执行.		
* @格式	 : ps -ef | grep \"./display [msg_id_text]\" | grep -v grep | awk '{print $2}' | xargs kill -9
* @实例	 : ps -ef | grep \"./display 1234\" | grep -v grep | awk '{print $2}' | xargs kill -9
* @return: 无返回值
****************************************************************************************
*/
void killDisplay(char id_text[])
{
	char command[BUFSIZ]; /* system函数参数 存放需要执行的指令 */

	strcpy(command,"ps -ef | grep \"./display ");
	strcat(command,id_text);
	strcat(command,"\" | grep -v grep | awk '{print $2}' | xargs kill -9");
		
	//DPRINTF("command = %s",command);
	system(command);
	return;
}


/*
****************************************************************************************
*                                  提示验证码
* @Desc  : 弹出验证码框		
* @格式	 : zenity --info --text="验证码:[code]" --no-wrap --title=注册
* @实例	 : zenity --info --text="验证码:1234" --no-wrap --title=注册
* @return: 无返回值
****************************************************************************************
*/
void infoCrtCode(char code[])
{
	char command[BUFSIZ]; /* system函数参数 存放需要执行的指令 */

	strcpy(command,"zenity --info --text=\"验证码:");
	strcat(command,code);
	strcat(command,"\" --no-wrap --title=注册");
		
	//DPRINTF("command = %s",command);
	system(command);
	return;
}

