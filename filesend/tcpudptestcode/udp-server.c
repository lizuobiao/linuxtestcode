#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8888
#define MSG_SIZE 1024

int main()
{
	int sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buffer[MSG_SIZE];
	int addrlen;
	int n,num = 0;
	//创建套接字
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	//绑定地址
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
	printf("client is connect\r\n");	
	//接收数据
	while(1)
	{
//		addrlen = sizeof(struct sockaddr);
//		bzero(buffer,sizeof(buffer));
		n = recvfrom(sockfd,buffer,MSG_SIZE,0,(struct sockaddr *)(&client_addr),&addrlen);
		if(n > 0)
			printf("num=%d len=%d\r\n",num++,n);
//		buffer[n] = 0;
//		printf("server received :%s\n",buffer);
	}
	//关闭连接
	close(sockfd);
	
	return 0;
} 




