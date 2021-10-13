/*
*********************************************************************************************************
*											  buffer_operations
*
*                                         (c) Copyright 2021/10/05
*
* @File    : buffer_operations.c
* @Author  : yqxu & ytzhang
*
* @Description:
* ----------
*  读取缓冲区数据并进行处理返回
*
* @History
*  Version :    Time    :  Author :  	Description
*   v1.0   : 2021-10-05 :   yqxu  :     readbuffer
* 
*********************************************************************************************************
*/

#include "../../include/server/bufops.h"

/*
****************************************************************************************
*                                 	读取缓冲数据
*
* @Desc  : 读取位于./data/下的缓冲数据
* @type  : 传递需要读取的缓冲数据类型,具体有哪些类型参考server.h中的枚举
*			--REGFORMBUF	:	读取注册表单数据,把读取到的数据分割三次
*			--LOGFORMBUF	:	读取登入表单数据,把读取到的数据分割两次
* @cnt	 : 需要分隔几次数据
* @suffix:  文件名后缀,用于区分缓冲数据类型
			"_form"	:	表单数据
* @buffer:  结构体指针,指向一个需要修改的结构体值,缓冲数据的存放厂库
* @inet_ip: 客户机登入的ip地址,不允许重复,封装做文件名
* @NOTE	 :	由于有多个客户端,每个客户端都需要有自己的缓冲区,是通过ip地址区分,
			所以ip地址必须一样,不然数据会混乱,该功能由制定网关协议的公司专门处理,
			所以本项目不做特殊处理。
			
			特殊处理:对于分隔符分开的每个数据最多可以有32位超过则提示消息过长

			参数buffer传参前一定要malloc
* @return: 无返回值,但是结构体指针buffer指向的地址的值发生变化,达到传参回去的效果
****************************************************************************************
*/
void readBuffer(int type,int cnt,char suffix[],struct Buffer *buffer,char inet_ip[])
{
	int i=0,j=0,k=0;	/* 用于循环的索引 */
	int fd;				/* 接收文件打开结果的标识符 */
	int res=1;			/* 存放文件读取的结果，-1:失败;0:read前光标已末尾;>0:读取到的数据长度 */
	int count=0;			/* 存放读取缓冲区的次数 */
	char src[64];		/* 存放拼接好的路径:比如'./data/ipbuffer/127.0.0.1_form' */
	char buf[96],out[cnt][32]; /* buf:暂存字符串	out:数据输出字符串数组	*/
	
	/* 打开文件 */
	//路径格式:"./data/ipbuffer/[inet_ip][suffix]"
	//示例路径:"./data/ipbuffer/127.0.0.1_form"
	strcpy(src,"./data/ipbuffer/");
	strcat(src,inet_ip);
	strcat(src,suffix);
	
	fd = open(src, O_RDONLY);	/* 只读打开,获取文件描述符 */
	//DPRINTF("[ \033[34mInfo\033[0m ] 缓冲路径:%s|读取结果:%d\n",src,fd);
	
	/* @[Warn]:其实判断长度过长也还是有bug如果输入|抑或是用户名110位后面都合格也会通过 */
	/* 解决了一半,一旦数据读了两次,直接退出,言下之意总数据超过96个 */
	if(fd!=-1){					/* 判断打开成功与否 */
		//printf("fd!=-1\n");	
		while(res>0){	/* 一次最多读取96位数据,小于96位都是有效数据 */
			res = read(fd,buf,sizeof(buf));
			if(res==-1){
				printf("[ \033[31mError\033[0m ] bufops.c readBuffer():读取失败[res:%d]\n",res);
				buffer->avail_flag = ILLEGAL; /* 失效结构体 */
				return;
			}
			count++;	/* 记录读取的次数 */
			if(count>2){	/* 缓冲总数据超过了的超过buf的大小 */
				printf("[ \033[31mError\033[0m ] bufops.c readBuffer():表单数据超过预期[count:%d]\n",count);
				buffer->avail_flag = LENILLEGAL; 	/*让结构体失效 */
				return; /* 退出 */
			}
		}
	}
	
	/* 分割读取,分割字符:'|' 因为一个数据最多 */
	//printf("buf=%s\n",buf);
	for(i=0;i<cnt;i++){		/* 分割几次 */
		k=0;	/* out的列索引 */
		while((buf[j] != '|' ) && (buf[j] != '\n') && (k < 32) ){
			/* 一次最多32位数据所以 k<32 */
			/* 读到 | 结束  或读到 \0 借宿 */
			out[i][k++]=buf[j++];
		}
		if(k==32){
			//接收到的数据长度有问题
			printf("[ \033[31mError\033[0m ] bufops.c readBuffer():单源数据长度违法[k:%d]\n",k);
			buffer->avail_flag = LENILLEGAL; /* 让结构体失效 */
			return;
		}
		out[i][k]='\0'; /* importaance:必须给每行介素符,不然会报错 */
		j++; /* 跳过buf里的分割符,准备下一次分割 */
	}
	
	if(type == REGFORMBUF){		/* 注册表单数据获取 */
		/* 获取名字,一密,二密 */
		strcpy(buffer->name,out[0]);
		strcpy(buffer->pwd,out[1]);
		strcpy(buffer->psd,out[2]);
		/* 客户端输出接收的结果 */
		DPRINTF("[ \033[34mInfo\033[0m ] REG_BUF  用户名:\033[34m%s\033[0m|密码:\033[34m%s\033[0m|密码:\033[34m%s\033[0m\n",buffer->name,buffer->pwd,buffer->psd);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else if(type==LOGFORMBUF){		/* 登入表单数据获取 */
		strcpy(buffer->name,out[0]);
		strcpy(buffer->pwd,out[1]);

		DPRINTF("[ \033[34mInfo\033[0m ] LOG_BUF  用户名:\033[34m%s\033[0m|密码:\033[34m%s\033[0m\n",buffer->name,buffer->pwd);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else if(type==SETFORMBUF){		/* 修改密码表单数据获取 */
		//DPRINTF("[ \033[34mInfo\033[0m ] 读取结果:%s-%s\n",out[0],out[1]);
		strcpy(buffer->pwd,out[0]);
		strcpy(buffer->psd,out[1]);
		
		DPRINTF("[ \033[34mInfo\033[0m ] PWD_BUF  原密码:\033[34m%s\033[0m|新密码:\033[34m%s\033[0m\n",buffer->pwd,buffer->psd);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else if(type==ADDFORMBUF){		/* 添加好友表单数据获取 */
		strcpy(buffer->name,out[0]);
		strcpy(buffer->text,out[1]);
		
		DPRINTF("[ \033[34mInfo\033[0m ] ADD_BUF  添加好友:\033[34m%s\033[0m|添加请求:\033[34m%s\033[0m\n",buffer->name,buffer->text);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else if(type==NAMEFORMBUF){		/* 添加好友表单数据获取 */
		strcpy(buffer->name,out[0]);
		
		DPRINTF("[ \033[34mInfo\033[0m ] NAME_BUF  名字:\033[34m%s\033[0m\n",buffer->name);
		//使其有效
		buffer->avail_flag = LEGAL;
		return;
	}else{
		printf("\033[31m[Error]\033[0m bufops.c readBuffer():无法识别要解析的缓冲类型:type=%d\n",type);
		buffer->avail_flag = ILLEGAL; /*让结构体失效 */
		close(fd);
		return;
	}
	//关闭文件指针
	close(fd);
	return;
}
