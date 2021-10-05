#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
int main(void){
	//term用于存储获得的终端参数信息
	struct termios term;
	int err;
	//获得标准输入的终端参数，将获得的信息保存在term变量中
	if(tcgetattr(STDIN_FILENO,&term)==-1){
		perror("Cannot get standard input description");
	}
	printf("%d\n",term.c_iflag);
	printf("%d\n",term.c_iflag);
	return 1;
}
