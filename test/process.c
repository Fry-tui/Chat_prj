# include <time.h>
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <signal.h> 
# include <unistd.h>
# include <pthread.h>
# include <sys/shm.h>
# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/stat.h>
# include <semaphore.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

int main()
{	
	int pid;
	system("gnome-terminal -e ./display --geometry=30x25+100+100 --title=Xuyq");
	sleep(2);
	system("ps -ef | grep ./display | grep -v grep | awk '{print $2}' ");
	system("ps -ef | grep ./display | grep -v grep | awk '{print $2}' >> pid_buffer");
	
	return 0;
}
