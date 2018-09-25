#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 

void *rec_data(void *fd)  
{  
       int client_sockfd;  
       int i,byte,recbyte = 0;  
       char buffer[1024] = {0};//存放数据  
       client_sockfd=*((int*)fd);  
       printf("rec_data creard = %d!\r\n",client_sockfd);
       for(;;)  
       {  
		recbyte = read(client_sockfd, buffer, 1024);
		if(recbyte > 0)
		{
			printf("REC:%d\r\n",recbyte);
			buffer[recbyte]='\0';
			printf("%s\r\n",buffer);
	
		}
		else
		{
			printf("read data fail !\r\n");
			break; 
		}
       }   
       close(client_sockfd);  
       pthread_exit(NULL);  
} 

int main(int argc, char **argv)
{
	int cfd;
	int recbyte;
	int sin_size;
	char buffer[1024] = {0};
	unsigned char send_data[12] = "test data";
	pthread_t thread;

	struct sockaddr_in s_add, c_add;
	unsigned short portnum = 80;

	printf("Hello,welcome to client!\r\n");

	if(argc != 2)
	{
		printf("usage: echo ip\n");
		return -1;
	}

	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == cfd)
	{
		printf("socket fail ! \r\n");
		return -1;
	}

	printf("socket ok !\r\n");
	printf("s_addr = %s\r\n",argv[1]);

	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr= inet_addr(argv[1]);
	s_add.sin_port=htons(portnum);
	printf("s_addr = %#x ,port : %d\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}

	printf("connect ok !\r\n");
	
	if(pthread_create(&thread, NULL, rec_data, &cfd)!=0)//创建子线程  
        {  
              perror("pthread_create");  
        }
	printf("rec_data pthread_create ok !\r\n");
	send(cfd,send_data,12,0);
	sleep(1);
	while(1)
	{
		send(cfd,send_data,10,0);
		sleep(1);
	}

	close(cfd);

	return 0;

}
