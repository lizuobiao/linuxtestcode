#include "netspeed.h"
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

//direction:0 upload,1:downlod
double get_traffic(char *ath0,int direction)
{
    int nDevLen = strlen(ath0);
    if (nDevLen < 1 || nDevLen > 100)
    {
        printf("dev length too long\n");
        return 0;
    }
    int fd = open("/proc/net/dev", O_RDONLY | O_EXCL);
    if (-1 == fd)
    {
        printf("/proc/net/dev not exists!\n");
        return 0;
    }

    char buf[1024*2];
    lseek(fd, 0, SEEK_SET);
    int nBytes = read(fd, buf, sizeof(buf)-1);
    if (-1 == nBytes)
    {
        perror("read error");
        close(fd);
        return 0;
    }
    buf[nBytes] = '\0';
    char* pDev = strstr(buf, ath0);
    if (NULL == pDev)
    {
        printf("don't find dev %s\n", ath0);
        close(fd);
        return 0;
    }
    char *p;
    char *ifconfig_value;
    int i = 0;
    double traffic;
    for (p = strtok(pDev, " \t\r\n"); p; p = strtok(NULL, " \t\r\n"))
    {
        i++;
        ifconfig_value = (char*)malloc(20);
        strcpy(ifconfig_value, p);
        if(i == 2 && direction==1)
        {
            traffic = atof(ifconfig_value);
        }
        if(i == 10 && direction==0)
        {
            traffic = atof(ifconfig_value);
            break;
        }
        free(ifconfig_value);
    }
    close(fd);
    return traffic;
}
double *my_ipconfig(char *ath0)
{

    int nDevLen = strlen(ath0);
    if (nDevLen < 1 || nDevLen > 100)
    {
        printf("dev length too long\n");
        return NULL;
    }
    int fd = open("/proc/net/dev", O_RDONLY | O_EXCL);
    if (-1 == fd)
    {
        printf("/proc/net/dev not exists!\n");
        return NULL;
    }

    char buf[1024*2];
    lseek(fd, 0, SEEK_SET);
    int nBytes = read(fd, buf, sizeof(buf)-1);
    if (-1 == nBytes)
    {
        perror("read error");
        close(fd);
        return NULL;
    }
    buf[nBytes] = '\0';
    char* pDev = strstr(buf, ath0);
    if (NULL == pDev)
    {
        printf("don't find dev %s\n", ath0);
        close(fd);
        return NULL;
    }
    char *p;
    char *ifconfig_value;
    int i = 0;
    static double rx2_tx10[2];
    for (p = strtok(pDev, " \t\r\n"); p; p = strtok(NULL, " \t\r\n"))
    {
        i++;
        ifconfig_value = (char*)malloc(20);
        strcpy(ifconfig_value, p);
        if(i == 2)
        {
            rx2_tx10[0] = atof(ifconfig_value);
        }
        if(i == 10)
        {
            rx2_tx10[1] = atof(ifconfig_value);
            break;
        }
        free(ifconfig_value);
    }
    close(fd);
    return rx2_tx10;
}

int get_if_dbytes(struct if_info* ndev)
{
    assert(ndev);

    struct ifaddrs *ifa_list = NULL;
    struct ifaddrs *ifa = NULL;
    struct if_data *ifd = NULL;
    int     ret = 0;

    ret = getifaddrs(&ifa_list);
    if(ret < 0)
    {
        perror("Get Interface Address Fail:");
        goto end;
    }

    for(ifa=ifa_list; ifa; ifa=ifa->ifa_next)
    {
        if(!(ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_RUNNING))
            continue;

        if(ifa->ifa_data == 0)
            continue;

        ret = strcmp(ifa->ifa_name,ndev->ifi_name);
        if(ret == 0)
        {
            ifd = (struct if_data *)ifa->ifa_data;

            ndev->ifi_ibytes = ifd->ifi_ibytes;
            ndev->ifi_obytes = ifd->ifi_obytes;
            break;
        }
    }

    freeifaddrs(ifa_list);
end:
    return (ret ? -1 : 0);
}

int get_if_speed(struct if_speed *ndev)
{
    assert(ndev);

    struct if_info *p1=NULL,*p2=NULL;

    p1 = (struct if_info *)malloc(sizeof(struct if_info));
    p2 = (struct if_info *)malloc(sizeof(struct if_info));
    bzero(p1,sizeof(struct if_info));
    bzero(p2,sizeof(struct if_info));

    strncpy(p1->ifi_name,ndev->ifs_name,strlen(ndev->ifs_name));
    strncpy(p2->ifi_name,ndev->ifs_name,strlen(ndev->ifs_name));

    int ret = 0;
    ret = get_if_dbytes(p1);
    if(ret < 0)     goto end;
    usleep(ndev->ifs_us);
    ret = get_if_dbytes(p2);
    if(ret < 0)     goto end;

    ndev->ifs_ispeed = p2->ifi_ibytes - p1->ifi_ibytes;
    ndev->ifs_ospeed = p2->ifi_obytes - p1->ifi_obytes;

end:
    free(p1);
    free(p2);

    return 0;
}

int main(void)
{
	double ifconfig_result;
	struct if_speed ndev;
	int ret = 0;
	float ispeed ,ospeed;
	
	ifconfig_result = get_traffic("eth0",0);
	printf("ifconfig_result = %f\r\n",ifconfig_result);
	
	ndev.ifs_us = 100000;
	sprintf(ndev.ifs_name,"eth0");
	ret = get_if_speed(&ndev);
	ispeed = ndev.ifs_ispeed * 1.0/(ndev.ifs_us/1000 * 0.001);
	ospeed = ndev.ifs_ospeed * 1.0/(ndev.ifs_us/1000 * 0.001);
	
	
	return 1;
}
