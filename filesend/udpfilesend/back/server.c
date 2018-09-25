#include "net_file.h"


#define PICTUREPATH "./picture/"

int    				   sock_id;
struct sockaddr_in     serv_addr; 
pthread_t 			   receive_thread_;
int 				   link_id;
char   				   buf[MAXLINE];

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

FileBlock_t Filemsg;
int fd = 0;

char file_path[FILE_NAME_MAX_LEN] = {0};
char file_path_back[FILE_NAME_MAX_LEN] = {0};
void cmd_reqsend_deel(Message_t *Message)
{
	int j = 0;
	int recv_len = 0;
	int len = 0,num;
	int i;
	char md5output[16] = {0};
	char systemcmd[FILE_NAME_MAX_LEN] = {0};
	struct timeval tm_begin, tm_end;
	int run_time = 0;

	FileBlock_t* FileBlock = (FileBlock_t*)(Message->buf);
	
	printf("Message.buf_len = %d\n",Message->buf_len);
	Filemsg = *FileBlock;
	
	strcat(file_path,PICTUREPATH);
	strcat(file_path_back,PICTUREPATH);
	printf("file_name = %s\n",Filemsg.file_name);
	strcat(file_path,Filemsg.file_name);
	printf("file_path %s\n",file_path);
	printf("file_path %s\n",file_path);
	printf("file_length %d\n",FileBlock->file_length);
	
	strcat(file_path_back,Filemsg.file_name);
	strcat(file_path_back,".filepart");
	
	gettimeofday(&tm_begin, NULL);
	fd = open(file_path_back,O_WRONLY|O_CREAT,0666);
	if(fd < 0)
		perror("file open failed\n"); 
	
/*	for(j = 0;j < 16;j++)
		printf("%x",(uint8_t)FileBlock->md5[j]);*/

	Message->type = ACK_OK;
	Message->buf_len = 0;
	send(link_id,Message,MESG_HEAD_LEN,0);
	
	while (recv_len = recv(link_id, buf, MAXLINE, 0)) 
	{  
		if(recv_len < 0) 
		{  
			printf("Recieve Data From Server Failed!\n");  
			break;  
		}  
		
		len += write(fd,buf,recv_len);
		bzero(buf,MAXLINE);  
//		printf("num=%d,len=%d,recv_len=%d\r\n",num++,len,recv_len);
		if(len >= (Filemsg.file_length))
		{
//			printf("file_length %d\n",Filemsg.file_length);
			break;
		}
	}  	
	
	close(fd);
	printf("cmd_revfilefinsh\n");

	MD5_file(file_path_back,md5output);
	for(i = 0;i < 16;i++)
	{
		if(md5output[i] != Filemsg.md5[i])
			break;
	}
	
	Message->cmd = CMD_SEND_FINISH;
	if(i != 16)
		Message->type = ACK_FAIL;
	else
	{
		strcat(systemcmd,"mv ");
		strcat(systemcmd,file_path_back);
		strcat(systemcmd," ");
		strcat(systemcmd,file_path);
		printf("systemcmd %s\n",systemcmd);
		system(systemcmd);
		
		Message->type = ACK_OK;
	}
	
	bzero(file_path_back,FILE_NAME_MAX_LEN);  
	bzero(file_path,FILE_NAME_MAX_LEN);
		
	Message->buf_len = 0;
	send(link_id,Message,MESG_HEAD_LEN,0);
	
	gettimeofday(&tm_end, NULL);
	run_time = (tm_end.tv_sec - tm_begin.tv_sec) * 1000000 +
				(tm_end.tv_usec - tm_begin.tv_usec);
	printf("send done, total size = %d\r\n", Filemsg.file_length);
	if(run_time > 0)
	{
		printf("USB transfer speed is %.2f MB/s\n\n",
			   (float)Filemsg.file_length / run_time);
	}
	else
	{
		printf("the file size is too small to calculate "
			   "transfer speed\n\n");
	}
}

void cmd_revfile(Message_t *Message)
{
	int len;
//	printf("cmd_revfile buf_len = %d\r\n",Message->buf_len);
//	printf("buf = %s\n",Message->buf);
	len = write(fd,Message->buf,Message->buf_len);
}

void cmd_revfilefinsh(Message_t *Message)
{
	int i;
	char md5output[16] = {0};
	char systemcmd[FILE_NAME_MAX_LEN] = {0};
	
	printf("cmd_revfilefinsh\n");
	
	
	MD5_file(file_path_back,md5output);
	for(i = 0;i < 16;i++)
	{
		if(md5output[i] != Filemsg.md5[i])
			break;
	}
	
	if(i != 16)
		Message->type = ACK_FAIL;
	else
	{
		strcat(systemcmd,"mv ");
		strcat(systemcmd,file_path_back);
		strcat(systemcmd," ");
		strcat(systemcmd,file_path);
		printf("systemcmd %s\n",systemcmd);
		system(systemcmd);
		
		Message->type = ACK_OK;
	}
	
	bzero(file_path_back,FILE_NAME_MAX_LEN);  
	bzero(file_path,FILE_NAME_MAX_LEN);
		
	Message->buf_len = 0;
	send(link_id,Message,MESG_HEAD_LEN,0);
}

#define CMD_SIZE 3
cmd_repose_t cmd_repose[] = {
	{CMD_SEND_FILEMSG_REQ,REQ_NOR , cmd_reqsend_deel},
	{CMD_SEND_FILE	     ,REQ_NORE,cmd_revfile},
	{CMD_SEND_FINISH	 ,REQ_NOR ,cmd_revfilefinsh}
};

void* ReceiveThread(void *arg)
{
	struct sockaddr_in     clie_addr;      
    int                    recv_len;  
    int                    write_leng;  
    int                    clie_addr_len;  
    int 				   i;
	int 				   num = 0;
	Message_t* Message = NULL;
	
	
acceptsock:	
		
    while(1)
    {    
		printf("accept running...!\n");
        clie_addr_len = sizeof(clie_addr);
		link_id = accept(sock_id, (struct sockaddr *)&clie_addr, &clie_addr_len);  
		if (-1 == link_id) 
		{  
			perror("Accept socket failed");       
		}     
		
        bzero(buf, MAXLINE);  
		//disconnect recv_len = 0
        while (recv_len = recv(link_id, buf, MAXLINE, 0)) 
        {  
			
            if(recv_len < 0) 
            {  
				//if client send file is empty
                printf("Recieve Data From Server Failed!\n"); 			
                break;  
            }  
			
			Message = (Message_t*)buf;
			printf("num = %d,cmd = %d\n",num++,Message->cmd);
//			printf("cmd_revfile buf_len = %d   buf = %s\n",Message->buf_len,Message->buf);
			for(i = 0;i < CMD_SIZE;i++)
			{
				if(cmd_repose[i].cmd == Message->cmd&&cmd_repose[i].type == Message->type)
				{
					  cmd_repose[i].cmd_ops(Message);
					  break;
				}
			}
			
			
            bzero(buf,MAXLINE);  
        }  	
		close(link_id);   
    }
	  
    return NULL;
}
	
int socketconnect()
{
	
	/*<-----------------------------------------socket---------------------------------------------->*/
    if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {  
        perror("Create socket failed\n");  
        return -1; 
    }  
	/*<-----------------------------------------bind------------------------------------------------->*/
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_port = htons(PORT);  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
  
    if (bind(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
    {  
        perror("Bind socket failed");
		return -1;     
    }  
	/*<-----------------------------------------listen----------------------------------------------->*/
    if (-1 == listen(sock_id, 10)) 
    {  
        perror("Listen socket failed\n");  
        return -1; 
    }  
	
	pthread_create(&receive_thread_, NULL, ReceiveThread, NULL);
	pthread_detach(receive_thread_); // pthread_detach(tid)使线程处于分离状态
	return 0;
}

int main(int argc,char **argv)   
{   
	int ret = 0;
	ret = socketconnect();
	if(ret != 0)
	{
		printf("connect fail\n");
	}
	
	while(1)
	{
		sleep(2);
	}
	
    return 0;  
}  
