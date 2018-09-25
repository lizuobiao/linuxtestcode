#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
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
	      	memset(buffer, 0, 1024);
//		sprintf(buffer, "hello,welcome to my server(%d) \r\n", num++);
		send(client_sockfd, buffer, strlen(buffer), 0);
		recbyte = read(client_sockfd, buffer, 1024);
		if(recbyte > 0)
		{
			printf("REC:%d\r\n",recbyte);
//			buffer[recbyte]='\0';
//			printf("%s\r\n",buffer);
			send(client_sockfd, buffer, recbyte, 0);
	
		}
		else
		{
			printf("read data fail ! close client_socket = %d\r\n",client_sockfd);
			break;
	//		exit(EXIT_FAILURE); 
//			return -1;
		}
       }  
  //     free(fd);  
       close(client_sockfd);  
       pthread_exit(NULL);  
}  

int main()
{
	int sfp, nfp, num = 0;
	struct sockaddr_in s_add,c_add;
	int sin_size;
	int recbyte;
	unsigned short portnum=8002;
	int ip_addr_t = 232432119; 
	char ip_addr[32] = {0};
	char buffer[100] = {0};
	pthread_t thread;//创建不同的子线程以区别不同的客户端 

	printf("Hello,welcome to my server !\r\n");

	sfp = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sfp)
	{
		printf("socket fail ! \r\n");
		return -1;
	}

	printf("socket ok !\r\n");

	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr=htonl(INADDR_ANY);
	s_add.sin_port=htons(portnum);

	if(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("bind fail !\r\n");
		return -1;
	}

	printf("bind ok !\r\n");

	if(-1 == listen(sfp,5))
	{
		printf("listen fail !\r\n");
		return -1;
	}

	printf("listen ok\r\n");

	sin_size = sizeof(struct sockaddr_in);
		
/*	nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
	if(-1 == nfp)
	{
		printf("accept fail !\r\n");
		return -1;
	}
	ip_addr_t = c_add.sin_addr.s_addr;
	printf("accept ok!\r\nServer start get connect from %d : %d\r\n", ip_addr_t,ntohs(c_add.sin_port));
	char* ip = inet_ntoa(*(struct in_addr*)&ip_addr_t); 
	printf("accept ok! client ip_addr = %s\r\n",ip);*/
	

	while(1)
	{
		nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
		if(-1 == nfp)
		{
			printf("accept fail !\r\n");
			return -1;
		}
		ip_addr_t = c_add.sin_addr.s_addr;
		printf("accept ok!\r\nServer start get connect from %d : %d\r\n", ip_addr_t,ntohs(c_add.sin_port));
		char* ip = inet_ntoa(*(struct in_addr*)&ip_addr_t); 
		printf("accept ok! client ip_addr = %s\r\n",ip);

		if(pthread_create(&thread, NULL, rec_data, &nfp)!=0)//创建子线程  
		{  
			 perror("pthread_create"); 
			 break;  
		}
/*
		memset(buffer, 0, 100);
//		sprintf(buffer, "hello,welcome to my server(%d) \r\n", num++);
		send(nfp, buffer, strlen(buffer), 0);
		if(-1 != (recbyte = read(nfp, buffer, 1024)))
		{
//			printf("read ok\r\nREC:\r\n");
			buffer[recbyte]='\0';
			printf("%s\r\n",buffer);
			send(nfp, buffer, recbyte, 0);
	
		}
		else
		{
			printf("read data fail !\r\n");
			return -1;
		}
		*/
		sleep(1);
	}

	close(nfp);
	close(sfp);

	return 0;
}
