#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define PORT 8888

int main(int argc,char **argv)
{
	
	char buffer[BUF_SIZE];
	struct sockaddr_in server_addr;
	int sockfd;
	int n,num = 0;
	
	if(argc != 2)
	{
		printf("Usage: %s server_ip\n",argv[0]);
		exit(1);
	}
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	inet_aton(argv[1],&server_addr.sin_addr);
	
	while(1)
	{
//		printf("please input char :\n");
//		fgets(buffer,BUF_SIZE,stdin);
		n = sendto(sockfd,buffer,BUF_SIZE,0,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
		if(n > 0)
			printf("num=%d len=%d\r\n",num++,n);
//		bzero(buffer,BUF_SIZE);
	}
	close(sockfd);
	return 0;
}











