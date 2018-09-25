#include "net_file.h"

/********update log*********************
*2018-9-25 V1.0.2  add FileBlock_t timestamp member
				   add Resource temporarily unavailable error deel
************ end **********************/

#define VERSION "1.0.2"

void udpsend(char *buf,unsigned int len);
int udprev(char *buf);

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

time_t get_file_mtime(const char *path)  
{  
    time_t file_mtime = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return file_mtime;  
    }else{  
        file_mtime = statbuff.st_mtime;  
    }  
    return file_mtime;  
}  

int sendfile(const char* file_path)  
{  
    int ret = 0;
    char sendbuf[MAXLINE] = {0};  
    char recvbuf[MAXLINE] = {0}; 
	int len            		  = 0;
	int i 					  = 0;
	int fd 					  = 0;
	int num = 0;
	Message_t* Message_repose = NULL;
	Message_t* Message        = (Message_t *)sendbuf;
	
	
	FileBlock_t *FileBlock = (FileBlock_t*)Message->buf;
	strcpy(FileBlock->file_name,file_path);
	MD5_file(file_path,FileBlock->md5);
	FileBlock->file_length = get_file_size(file_path);
	FileBlock->timestamp   = get_file_mtime(file_path);
	
	printf("file_length = %d\n",FileBlock->file_length);
	printf("timestamp = %ld\n",FileBlock->timestamp);
	/*for(i = 0;i < 16;i++)
		printf("%x",(uint8_t)FileBlock->md5[i]);*/
	
	Message->cmd     = CMD_SEND_FILEMSG_REQ;
	Message->type    = REQ_NOR;
	Message->buf_len = sizeof(FileBlock_t);
	udpsend((char*)Message,Message->buf_len+MESG_HEAD_LEN);
	
	bzero(sendbuf, MAXLINE);
	len = udprev(sendbuf);
	if(len <= 0)
	{
		printf("udprev fail\n");
	}
	
	Message_repose = (Message_t*)sendbuf;
	if(Message_repose->cmd    == CMD_SEND_FILEMSG_REQ
	   &&Message_repose->type == ACK_OK)
	{	
		bzero(sendbuf, MAXLINE);
		Message->cmd     = CMD_SEND_FILE;
		Message->type    = REQ_NORE;
		fd = open(file_path, O_RDONLY);
		if(fd < 0)
		{
			perror("open failed");  
			return -1;
		}
		
		len = read(fd, Message->buf,BUF_MAX_LEN);
		if (len <= 0)
				return -1;
		while(1)
		{
			Message->buf_len = len;
//			printf("num = %d,buf_len = %d\n",num++,Message->buf_len);
			udpsend((char*)Message,Message->buf_len+MESG_HEAD_LEN);
			
			ret = udprev(recvbuf);
			Message_repose = (Message_t*)recvbuf;
			if(Message_repose->cmd  == CMD_SEND_FILE
			&&Message_repose->type  == ACK_OK)
			{
//			   printf("file send is ok\n");
				len = read(fd, Message->buf,BUF_MAX_LEN);
				if (len <= 0)
						break;
			}else if(Message_repose->type == ACK_NOSP)
			{
				 printf("server No space\r\n");
				 return -1;
			}
			else
			{
			   printf("file send fail!retry send\n");
			   udpsend((char*)Message,Message->buf_len+MESG_HEAD_LEN);
			}
		}
		
		close(fd);
		printf("file send finish\n");
		
		bzero(sendbuf, MAXLINE);
	
		Message->cmd     = CMD_SEND_FINISH;
		Message->type    = REQ_NOR;
		Message->buf_len = 0;
		udpsend((char*)Message,MESG_HEAD_LEN);
		len = udprev(sendbuf);
		Message_repose = (Message_t*)sendbuf;
		if(Message_repose->cmd  == CMD_SEND_FINISH
	    &&Message_repose->type  == ACK_OK)
	    {
		   printf("file send is ok\n");
	    }else
		{
		   printf("file send is fail\n");
		   printf("cmd = %d,type = %d\r\n",Message_repose->cmd,Message_repose->type);
		}
	}
	else
	{
		printf("Message_repose fail = %d\n",Message_repose->type);
	}
	
	return 0;
 /*     
	printf("send：%s\n",sendbuf); 
	udpsend(sendbuf,5);
	ret = udprev(recvbuf);
	printf("len = %d rev:%s\n",ret,recvbuf);
	memset(sendbuf, 0, sizeof(sendbuf));  
	memset(recvbuf, 0, sizeof(recvbuf));  */
	
    
	
}  


/***********************udp send rev**********************************************/
char* SERVERIP = "192.168.24.138";  
//char* SERVERIP = "10.0.0.2";  
#define ERR_EXIT(m) do{perror(m);}while(0)  
	
int sock; 
struct sockaddr_in servaddr,disservaddr,disp_local_addr; 

void udpsocketinit()
{
	struct timeval timeout = { 1, 0 };
	int ret;
	
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
	{
		perror("sock creat fail");
	}
    
	memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(PORT);  
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);
    
	//设置接收超时
	ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    if(ret == -1)
        perror("sock setsockopt");
	
}

void Restempunavail_error_handler(void)
{
	system("ifconfig usb0 down");
	system("ifconfig usb0 up");
}

void udpsend(char *buf,unsigned int len)
{
	sendto(sock, buf, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));  
}

int udprev(char *buf)
{
	 int length;
	char * server_ip = NULL;
    unsigned short int port = 0;
	socklen_t addrlen = sizeof(struct sockaddr);
	
	length = recvfrom(sock, buf,BUF_MAX_LEN, 0, (struct sockaddr*)&disservaddr,&addrlen); 
	if (length == -1)  
	{   
		ERR_EXIT("recvfrom");  //如果设置了超时，超过3S没有获取到数据就会报错 Resource temporarily unavailable
		Restempunavail_error_handler();
		return -1;
	}
	
//	server_ip = inet_ntoa(disservaddr.sin_addr);//display remove ip addr
//	port      = ntohs(disservaddr.sin_port);
//	printf("addrlen = %d server_ip = %s  port = %d::",addrlen,server_ip,port);
	  
	return length;
}


int main(int argc,char **argv)  
{
    printf("VERSION:%s\r\n",VERSION);    
	if (argc != 2)  
    {  
        printf("Usage: %s file_name\n", argv[0]);  
        exit(1);  
    } 
	
	
	udpsocketinit();
//	while(1)
	{
		sendfile(argv[1]);
		sleep(1);
	}		
	
	close(sock);  	
    return 0;  
}




 /*   
	#define NET_CARD "eth0" 
    struct timeval timeout = { 3, 0 };
    struct ifreq ifr;
    
    
   
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, NET_CARD,IFNAMSIZ);
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return;
    }
    printf("frname:%s\n",ifr.ifr_name);
    printf("host:%s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
*/

/*    
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,(char *)&ifr, sizeof(ifr)) < 0)//指定哪个网卡发送数据
    {
        perror("setsockopt");//返回Operation not permitted 代码没有权限，改为root就行
        return;
    }
*/	
 /*   
    disp_local_addr.sin_family      = AF_INET;
//    disp_local_addr.sin_addr.s_addr = INADDR_ANY;
    disp_local_addr.sin_addr.s_addr = ((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr.s_addr;
    disp_local_addr.sin_port        = htons(0);
    if (bind(sock, (struct sockaddr*)&disp_local_addr, sizeof(disp_local_addr)) == -1)
    {
        perror("sock bind");
        ERR_EXIT("bind");
    }
   */
