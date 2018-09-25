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
 
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
	   
#define	RESTART_AK_IPC		"/mnt/mtd/start_ipc.sh"
#define SDP1_DEV_NAME       "/dev/mmcblk0p1"
#define SD_DEV_NAME         "/dev/mmcblk0"
#define SD_TOOL				"dsk_repair"
#define MOUNT_SDP1			"mount -rw /dev/mmcblk0p1 /mnt/" 
#define MOUNT_SD			"mount -rw /dev/mmcblk0 /mnt/" 
#define UMOUNT_SD			"umount /mnt/ -l"
 
/**
 * *  @brief       	do_syscmd
 				执行系统命令
 * *  @author      	aj
 * *  @date       	2014-10-28
 * *  @param[in] 	char * cmd, 要执行的命令
 				char *result， 执行结果
 * *  @return       int
 * *  @retval		lengh of result buf --> success, less than zero --> failed
 * */
 
 int system_start_flag;
int do_syscmd(char *cmd, char *result)
{
	char buf[4096];	
	FILE *filp;
	
	filp = popen(cmd, "r");
	if (NULL == filp){
		printf("[%s:%d] popen %s, cmd:%s!\n", 
			__func__, __LINE__, strerror(errno), cmd);
		return -2;
	}
 
	//fgets(buf, sizeof(buf)-1, filp);
	memset(buf, '\0', sizeof(buf));
	fread(buf, sizeof(char), sizeof(buf)-1, filp);
	
	sprintf(result, "%s", buf);
	
	pclose(filp);
	return strlen(result);	
}
 
/**
 * *  @brief       daemon_check_sd_prepare
 				write something to the TF card
 * *  @author      
 * *  @date        2015-5-18
 * *  @param[in]     void
 * *  @return      
 * */
 
void daemon_test_sd_ro(void)
{
	system("dd if=/dev/zero of=/mnt/test_tf bs=1024 count=1");
	system("rm -rf /mnt/test_tf");
}
 
/**
 * *  @brief       daemon_check_sd_ro
 				check the sd card 
 * *  @author      
 * *  @date        2015-5-18
 * *  @param[in]     int flag, 1->mmcblkp0,    0->mmcblk 
 * *  @return      
 * */
 
void daemon_check_sd_ro(int mmcblk0_p1)
{
	char cmd[128] = {0}, result[256], badfile[100] ={0}, dev_name[20] = {0};
 
	daemon_test_sd_ro();
	
	if(mmcblk0_p1)
		sprintf(dev_name, "%s", SDP1_DEV_NAME);		//"/dev/mmcblk0p1"
	else
		sprintf(dev_name, "%s", SD_DEV_NAME);		//"/dev/mmcblk0"
 
	sprintf(cmd, "mount | grep %s", dev_name); //do "mount | grep dev"
 
	do_syscmd(cmd, result);
	
	if(strlen(result) > 0){
		if(strstr(result, "rw,relatime") == NULL){			//if not read write	
			if(strstr(result, "ro,relatime") != NULL){			//find ro lable, the card was read only
 
				printf("[%s:%d] The SD Card is Read Only, repair it\n", __func__, __LINE__);
 
			}
		}else{
			printf("[%s:%d] The SD Card is OK\n", __func__, __LINE__);
			return;
		}
	}
}
 
/**
 * *  @brief    daemon_umount_sd
 			卸载SD 卡
 * *  @author   
 * *  @date        
 * *  @param[in]   void
 * *  @return      void
 * */
 
/* umount the sd card and delete the sd_test dir */
void daemon_umount_sd(void)
{
	char cmd[128] = {0};
 
	/** notice the anyka_ipc that  **/
	sprintf(cmd, "echo %d > %s; killall -12 aoni_ipc", 0, "/tmp/sd_status");
	system(cmd);
    system(UMOUNT_SD);
	printf("[%s:%d] *** umount the sd ***\n", __func__, __LINE__);
	
}
 
/**
 * *  @brief       daemon_mount_sd
 * *  @author     
 * *  @date        
 * *  @param[in]   int flag, 1->mmcblkp0,    0->mmcblk 
 * *  @return      void
 * */
 
/* create a sd_test dir and mount the sd card in it */
void daemon_mount_sd(int flag)
{
	char cmd[128], status[20] = {0};
	int mmcblk0_p1;
 
    if(flag == 0)
    {
        if(access(SDP1_DEV_NAME, R_OK) >= 0)
        {
            printf("**********we will skip mount /dev/mmcblk0*******\n");
            return;
        }
    }
 
	if (flag){
		mmcblk0_p1 = 1;
		sprintf(cmd, "%s", MOUNT_SDP1);
	}
	else{
		mmcblk0_p1 = 0;
		sprintf(cmd, "%s", MOUNT_SD);
	}
    system(cmd);
 
	//check sdcard readonly or not, and repair it.
	daemon_check_sd_ro(mmcblk0_p1);
 
	/*
	* when the sd card message is not come first time, 
	* it means hot plug and we will send sig to anyka_ipc 
	*/
    if(system_start_flag == 0)
    {
    	//send message to anyka_ipc that card is ready
		sprintf(status, "echo %d > %s", 1, "/tmp/sd_status");
		system(status);
        system("killall -12 aoni_ipc"); //send printf to ipc
    }
	printf("[%s:%d] *** mount the sd to /mnt ***\n", __func__, __LINE__);
	
}
 
 
/**
 * *  @brief    daemon_init_hotplug_sock
 			热插拔检测
 * *  @author   
 * *  @date        
 * *  @param[in]   void
 * *  @return      int， -1， 失败；else 成功
 * */
 
/* create the socket to recevie the uevent */
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
/* waitting the uevent and do something */
void *daemon_pth_func(void *data)
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
		return NULL;
	}
    system_start_flag = 1;
	sleep(1);
    if (access (SDP1_DEV_NAME, F_OK) == 0)
		daemon_mount_sd(1);
	else if (access (SD_DEV_NAME, F_OK) == 0)
		daemon_mount_sd(0);
    system_start_flag = 0;
	
    while(1) {
		//clear buf
        memset(buf, 0, sizeof(buf));
 
		//block here to wait printf
        recv(hotplug_sock, &buf, sizeof(buf), 0);
        p = strrchr(buf, '/');	//get block name
		printf("buf = %s\r\n",buf);
		//get action
        for (i = 0; buf[i] != '@' && buf[i] != 0; i++)
            temp_buf[i] = buf[i];
        temp_buf[i] = 0;
 
		if (strcmp(temp_buf, "change"))
			printf("%s\n", buf);
 
		//card insert
		if (!strcmp(temp_buf, "add")) {
			if (!strcmp(p, "/mmcblk0p1")) {
				//sleep(1);
				daemon_mount_sd(1);
			} else if (!strcmp(p, "/mmcblk0")) {
				//sleep(1);
				daemon_mount_sd(0);
			}
			p1_removed = 0;
			continue;
		}
 
		//card extract
		if (!strcmp(temp_buf, "remove")) {
 
			//if p1 removed, than we do need to umount k0
			if(!strcmp(p, "/mmcblk0p1")) {
				daemon_umount_sd();
				p1_removed = 1;
			} 
			else if((!strcmp(p, "/mmcblk0")) && (!p1_removed))
				daemon_umount_sd();
		}
    }
 
	printf("[%s:%d] Exit thread, id : %ld\n", __func__, __LINE__, (long int)gettid());
	return NULL;
}



void main()
{
		daemon_pth_func(NULL);
		
}
