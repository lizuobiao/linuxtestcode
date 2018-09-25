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
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		perror("Create socket failed\n");  
        return -1; 
	}
	
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
	printf("client is connect\r\n");	
	
	while(1)
	{
		addrlen = sizeof(struct sockaddr);
		n = recvfrom(sockfd,buffer,MSG_SIZE,0,(struct sockaddr *)(&client_addr),&addrlen);
		if(n > 0)
			printf("num=%d len=%d\r\n",num++,n);
	}
	
	close(sockfd);
	
	return 0;
} 




