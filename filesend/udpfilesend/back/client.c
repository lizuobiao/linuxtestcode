#include "net_file.h"

//#define IP_ADDR "192.168.24.138" 
#define IP_ADDR "10.0.0.2"
struct sockaddr_in     serv_addr;  
char                   buf[MAXLINE];  
int                    sock_id;   
pthread_t 			   receive_thread_;


int MD5_file(const char *path,char *output)
{
    MD5_CTX ctx;
    char buf[4096];
    int rval;
    int fd = 0;
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("open file to read");
        return -1;
    }
    MD5_Init(&ctx);
    while(1)
    {
        rval = read(fd, buf, sizeof(buf));
        if (rval <= 0)
            break;
        MD5_Update(&ctx, buf, rval);
    }
    MD5_Final((unsigned char*)output, &ctx);
    close(fd);
    return 0;
}



int socketconnect()
{
	int i_ret; 
    if ((sock_id = socket(AF_INET,SOCK_STREAM,0)) < 0) 
	{  
        perror("Create socket failed\n");  
        return -1;  
    }  

    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_port = htons(PORT);  
    inet_pton(AF_INET,IP_ADDR, &serv_addr.sin_addr);  
     
    i_ret = connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));  
    if (-1 == i_ret) 
    {  
        printf("Connect socket failed\n");  
        return -1;  
    }  
	
//	pthread_create(&receive_thread_, NULL, ReceiveThread, NULL);
//	pthread_detach(receive_thread_); // pthread_detach(tid)使线程处于分离状态
	
	return 0;  
}
int socketsend(char *buf,int len)
{
	int send_len; 
	send_len = send(sock_id, buf,len, 0);
	if(send_len < 0)
	{
		perror("socket send failed\n");  
		return -1;
	}
	return send_len;
}

int socketclose()
{
	close(sock_id);  
	return 0;
}

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}  

int request_sendfile(const char* file_path)
{
	int len            		  = 0;
	int i 					  = 0;
	int fd 					  = 0;
	int 				   num = 0;
	Message_t* Message_repose = NULL;
	Message_t* Message        = (Message_t *)buf;
	
	FileBlock_t *FileBlock = (FileBlock_t*)Message->buf;
	strcpy(FileBlock->file_name,file_path);
	MD5_file(file_path,FileBlock->md5);
	FileBlock->file_length = get_file_size(file_path);
	printf("file_length = %d\n",FileBlock->file_length);
	/*for(i = 0;i < 16;i++)
		printf("%x",(uint8_t)FileBlock->md5[i]);*/
	
	Message->cmd     = CMD_SEND_FILEMSG_REQ;
	Message->type    = REQ_NOR;
	Message->buf_len = sizeof(FileBlock_t);
	socketsend((char*)Message,Message->buf_len+MESG_HEAD_LEN);
	
	bzero(buf, MAXLINE);
	len = recv(sock_id,buf,BUFSIZ,0);
	Message_repose = (Message_t*)buf;
	if(Message_repose->cmd    == CMD_SEND_FILEMSG_REQ
	   &&Message_repose->type == ACK_OK)
	{	
	
		bzero(buf, MAXLINE);
		Message->cmd     = CMD_SEND_FILE;
		Message->type    = REQ_NORE;
		fd = open(file_path, O_RDONLY);
		if(fd < 0)
		{
			perror("open failed");  
			return -1;
		}
			
		while(1)
		{
			bzero(buf, MAXLINE);
			len = read(fd, buf,BUF_MAX_LEN);
			if (len <= 0)
				break;
			
	//		printf("num = %d,buf_len = %d\n",num++,len);
			socketsend(buf,len);
		}
		
		close(fd);
		printf("file send finish\n");
		
		bzero(buf, MAXLINE);
		len = recv(sock_id,buf,MAXLINE,0);
		Message_repose = (Message_t*)buf;
		if(Message_repose->cmd  == CMD_SEND_FINISH
	    &&Message_repose->type  == ACK_OK)
	    {
		   printf("file send is ok\n");
	    }else
		{
		   printf("file send is fail\n");
		}
		
	}
}

int main(int argc,char **argv)  
{  
	if (argc != 2)  
    {  
        printf("Usage: %s file_name\n", argv[0]);  
        exit(1);  
    } 
	
	socketconnect();
	request_sendfile(argv[1]);
	
    return 0;  
}  
