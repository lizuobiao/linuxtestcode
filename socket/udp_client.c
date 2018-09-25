#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
#include <string.h>  
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define MYPORT 8887  
char* SERVERIP = "218.244.138.118";  
#define ERR_EXIT(m) do{perror(m);exit(EXIT_FAILURE);}while(0)  
#define NET_CARD "eth0"    
void echo_cli(int sock)  
{  
    struct sockaddr_in servaddr,disservaddr,disp_local_addr;  
    int ret;  
    char sendbuf[1024] = {0};  
    char recvbuf[1024] = {0};  
    char * server_ip = NULL;
    unsigned short int port = 0;
    socklen_t addrlen = 0;
    struct timeval timeout = { 3, 0 };
    struct ifreq ifr;
    
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(MYPORT);  
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);  
    
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, NET_CARD,IFNAMSIZ);
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return;
    }
    printf("frname:%s\n",ifr.ifr_name);
    printf("host:%s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));

    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));//设置接收超时
    if(ret == -1)
        perror("sock setsockopt");
    
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,(char *)&ifr, sizeof(ifr)) < 0)//指定哪个网卡发送数据
    {
        perror("setsockopt");//返回Operation not permitted 代码没有权限，改为root就行
        return;
    }
    
    disp_local_addr.sin_family      = AF_INET;
//    disp_local_addr.sin_addr.s_addr = INADDR_ANY;
    disp_local_addr.sin_addr.s_addr = ((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr.s_addr;
    disp_local_addr.sin_port        = htons(0);
    if (bind(sock, (struct sockaddr*)&disp_local_addr, sizeof(disp_local_addr)) == -1)
    {
        perror("sock bind");
        ERR_EXIT("bind");
    }
    
    
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)  
    {  
        printf("向服务器发送：%s\n",sendbuf);  
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));  
        
        addrlen = sizeof(struct sockaddr);
        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&disservaddr, &addrlen);  
        if (ret == -1)  
        {  
            if (errno == EINTR)  
                continue;  
            ERR_EXIT("recvfrom");  //如果设置了超时，超过3S没有获取到数据就会报错 Resource temporarily unavailable
        }
        server_ip = inet_ntoa(disservaddr.sin_addr);//display remove ip addr
        port      = ntohs(disservaddr.sin_port);
        printf("addrlen = %d server_ip = %s  port = %d::",addrlen,server_ip,port);
        printf("从服务器接收：%s\n",recvbuf);  
          
        memset(sendbuf, 0, sizeof(sendbuf));  
        memset(recvbuf, 0, sizeof(recvbuf));  
    }    
    close(sock);  
}  
  
int main(void)  
{  
    int sock;  
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
        ERR_EXIT("socket");  
      
    echo_cli(sock);  
      
    return 0;  
}