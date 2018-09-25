#include <stdio.h>
#include <stdlib.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <pthread.h>
#include <linux/netlink.h>
#include <linux/version.h>
#include <linux/input.h>
#include <syslog.h>
#include <stdarg.h>
//#include <execinfo.h>
#include <errno.h>
#include <sys/vfs.h>
/************** for tcp ********************/
#include <sys/types.h>			/* See NOTES */ 	 
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include <sys/syscall.h>   /* For SYS_xxx definitions */


static int daemon_init_hotplug_sock(void)
{
	struct sockaddr_nl snl;
    const int buffersize = 2048;
    int retval;
 
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
 
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
 
	int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (hotplug_sock == -1)	{
        printf("[%s:%d] socket: %s\n", __func__, __LINE__, strerror(errno));
        return -1;
    }
 
    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
 
    retval = bind(hotplug_sock, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (retval < 0) {
	    printf("[%s:%d] bind failed: %s\n", __func__, __LINE__, strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
    }
 
    return hotplug_sock;
}
 
 
pid_t gettid()
{
	return syscall(SYS_gettid);
}
 
#define UEVENT_BUFFER_SIZE 512

void hotplug_user(void)
{	
    char buf[UEVENT_BUFFER_SIZE * 2] = {0};
    char temp_buf[20];	//find action
    char *p;	//find blk
    int i;
    int hotplug_sock;	//hotplug socket handle
    int p1_removed = 0;
	
	printf("[%s:%d] This thread id: %ld\n", __func__, __LINE__, (long int)gettid());
 
	hotplug_sock = daemon_init_hotplug_sock();	
    if (hotplug_sock < 0)
	{
		printf("hotplug_sock < 0 return");
		return ;
	}

    while(1) {
        memset(buf, 0, sizeof(buf));
        recv(hotplug_sock, &buf, sizeof(buf), 0);
		printf("buf = %s\r\n",buf);
    }
 
	printf("[%s:%d] Exit thread, id : %ld\n", __func__, __LINE__, (long int)gettid());
}

void main()
{
//		daemon_pth_func(NULL);
	hotplug_user();
		
}
