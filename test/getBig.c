/*
*   通过函数 ioctl(); 获得终端界面的参数
*   @author 李政　<1244109467@qq.com>
*/
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "my_err.h"

//打印函数
static void pr_winsize(int fd)
{
    //定义一个结构体变量 size
    struct winsize size;

    //返回值判断
    if(ioctl(fd, TIOCGWINSZ, (char *)&size) < 0)
    {
        err_sys("TIOCGWINSZ error");
    }

    //每次接收到信号打印　行、列数
    printf("%d rows, %d columns\n", size.ws_row, size.ws_col);

}

//signal 的函数
static void sig_winch(int signo)
{
    printf("SIGWINCH received\n");
    pr_winsize(STDIN_FILENO);
}

int main(void)
{
    //首先通过　isatty();函数判断文件描述符是否为终端机
    if( isatty(STDIN_FILENO) == 0 )
    {
        exit(1);
    }
    //接受　SIGWINCH　信号
    if(signal(SIGWINCH, sig_winch) == SIG_ERR)
    {
        err_sys("signal error");
    }
    //首先打印窗口的size
    pr_winsize(STDIN_FILENO);    /* print initial size */

    //循环接受程序不能死
    for(;;)                /* and sleep forever */
        pause();
}
