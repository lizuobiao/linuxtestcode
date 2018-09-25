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
  
  
#define ERR_EXIT(m) do{perror(m); exit(EXIT_FAILURE);} while (0)  
  
void echo_ser(int sock)  
{  
    char recvbuf[1024] = {0};  
    struct sockaddr_in peeraddr;    
    socklen_t peerlen;  
	char * server_ip = NULL;
	unsigned short int port = 0;
    int n,i;  
    fd_set recv_fds;
	struct timeval tv;
	int ret;
    
    while (1)  
    {
		FD_ZERO(&recv_fds);
        FD_SET(sock, &recv_fds);
		tv.tv_sec = 3;
        tv.tv_usec = 0;
		
		ret = select(sock+1, &recv_fds, NULL, NULL, &tv);
//		printf("ret = %d\r\n",ret);
        /* socket error */
        if (ret == -1)
		{
			perror("select");
            break;
		}
        /* timeout */
        if (ret == 0)
        {
//			printf("ret == 0");
            continue;
        }

		if (FD_ISSET(sock, &recv_fds))
		{
			peerlen = sizeof(peeraddr);  
			memset(recvbuf, 0, sizeof(recvbuf));  
			n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,  
						 (struct sockaddr *)&peeraddr, &peerlen);  
			if (n <= 0)  
			{
				perror("sock recvfrom");
				if (errno == EINTR)  
					continue;     
				ERR_EXIT("recvfrom error");  
			}  
			else if(n > 0)  
			{  
				server_ip = inet_ntoa(peeraddr.sin_addr);//display remove ip addr
				port	  = ntohs(peeraddr.sin_port);
				printf("client_ip = %s port = %d::",server_ip,port);
				
				printf("接收到的数据：%s\n",recvbuf);  
				sendto(sock, recvbuf, n, 0,  
					   (struct sockaddr *)&peeraddr, peerlen);  
				for(i = 0;i < n;i++)
					printf("%0x ",recvbuf[i]);
				printf("回送的数据：%s\n",recvbuf);  
			}  
		}
    }  
    close(sock);  
}  
  
int main(void)  
{  
    int sock;  
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
        ERR_EXIT("socket error");  
      
    struct sockaddr_in servaddr;  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(MYPORT);  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
      
    printf("监听%d端口\n",MYPORT);  
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  
	{
		perror("sock bind");
        ERR_EXIT("bind error");  
	}
    echo_ser(sock);  
      
    return 0;  
} 
