#include "net_file.h" 

/********update log*********************
*2018-9-25 V1.0.2  FileBlock_t add timestamp
************ end **********************/



#define VERSION "1.0.2"  
 
#define ERR_EXIT(m) do{perror(m); exit(EXIT_FAILURE);} while (0)
//#define PICTUREPATH "./picture/"
#define PICTUREPATH "/mnt/"

void udpsend(char *buf,unsigned int len);

/***************tool util***********************************/	
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


int stringtoarray(char* str,const char* delim,char **saveptr)
{
    char * pch = NULL;
    int num = 0;
    pch = strtok(str, delim);
    while (pch != NULL)
    {
        *(saveptr+num) = pch;
//		printf("num %d,%s\n",num,pch);
        num++;
        pch = strtok(NULL,delim);
    }
    return num;
}

/***************speed test***********************************/
struct timeval tm_begin, tm_end;
void setstart()
{
	gettimeofday(&tm_begin, NULL);
}

int getspeed(uint32_t file_length)
{
	int run_time;
	
	gettimeofday(&tm_end, NULL);
	run_time = (tm_end.tv_sec - tm_begin.tv_sec) * 1000000 +
				(tm_end.tv_usec - tm_begin.tv_usec);
	printf("send done, total size = %d\r\n", file_length);
	if(run_time > 0)
	{
		printf("USB transfer speed is %.2f MB/s\n\n",
			   (float)file_length / run_time);
	}
	else
	{
		printf("the file size is too small to calculate "
			   "transfer speed\n\n");
	}
}

/***************file rev***********************************/	
//if dir no exis ,to creat,this funtion change FileBlock->file_name
int access_dir(FileBlock_t* FileBlock)
{
	char *saveptr[64];
	char dirpath[80] = {0};
	int num = 0,i;
	printf("file_path %s\n",FileBlock->file_name);
	num = stringtoarray(FileBlock->file_name,"/",saveptr);
	
	if(num < 2)
		return 2;
	memset(dirpath,0,80);
	strcat(dirpath,PICTUREPATH);
	
	for(i = 0; i < num-1;i++)
	{
	//	printf("dir %s\n",saveptr[num-2]);
		strcat(dirpath,"/");
		strcat(dirpath,saveptr[i]);
	//	printf("dirpath %s\n",dirpath);
		
		if(opendir(dirpath) == NULL)
		{
			mkdir(dirpath,0755);
//			return 1;
		}
	}
	
	return 0;
}

FileBlock_t Filemsg;
int fd = 0;
char file_path[FILE_NAME_MAX_LEN] = {0};
char file_path_back[FILE_NAME_MAX_LEN] = {0};
time_t file_creattime = 0;

void cmd_reqsend_deel(Message_t *Message)
{
	int j = 0;
	int recv_len = 0;
	int len = 0,num;
	int i;
	char md5output[16] = {0};
	
	int run_time = 0;
	FileBlock_t* FileBlock = (FileBlock_t*)(Message->buf);
	
	printf("Message.buf_len = %d\n",Message->buf_len);
	Filemsg = *FileBlock;
	file_creattime = FileBlock->timestamp;
	printf("timestamp = %ld\r\n",FileBlock->timestamp);
	
	strcat(file_path,PICTUREPATH);
	strcat(file_path,Filemsg.file_name);
	printf("file_path %s\n",file_path);
	
	strcat(file_path_back,PICTUREPATH);
	strcat(file_path_back,Filemsg.file_name);
	strcat(file_path_back,".filepart");

	access_dir(FileBlock);
	fd = open(file_path_back,O_WRONLY|O_CREAT,0666);
	if(fd < 0)
	{
		perror("file open failed"); 
		Message->type = ACK_FAIL;
	}
	else
	{
		Message->type = ACK_OK;
	}
/*	for(j = 0;j < 16;j++)
		printf("%x",(uint8_t)FileBlock->md5[j]);*/

	Message->buf_len = 0;
	udpsend((char*)Message,MESG_HEAD_LEN);
	
	setstart();
}

void cmd_revfile(Message_t *Message)
{
	int len;
//	printf("cmd_revfile buf_len = %d\r\n",Message->buf_len);
//	printf("buf = %s\n",Message->buf);
	len = write(fd,Message->buf,Message->buf_len);
	
	if(len == -1)
	{
		printf("errno = %d",errno);
		bzero(file_path_back,FILE_NAME_MAX_LEN);  
		bzero(file_path,FILE_NAME_MAX_LEN);
		Message->type = ACK_NOSP;
		close(fd);
	}else
	{
		Message->type = ACK_OK;
	}
	
	
	Message->buf_len = 0;
	udpsend((char*)Message,MESG_HEAD_LEN);
}

void cmd_revfilefinsh(Message_t *Message)
{
	int i;
	char md5output[16] = {0};
	char systemcmd[FILE_NAME_MAX_LEN+FILE_NAME_MAX_LEN] = {0};
	struct utimbuf utimebuf = {0};
	
	getspeed(Filemsg.file_length);
	printf("cmd_revfilefinsh\n");
	
	MD5_file(file_path_back,md5output);
	for(i = 0;i < 16;i++)
	{
		if(md5output[i] != Filemsg.md5[i])
			break;
	}
	
	if(i != 16)
	{
		Message->type = ACK_FAIL;
		Message->buf_len = 0;
		udpsend((char*)Message,MESG_HEAD_LEN);
	}
	else
	{
		Message->type = ACK_OK;
		Message->buf_len = 0;
		udpsend((char*)Message,MESG_HEAD_LEN);
		strcat(systemcmd,"mv ");
		strcat(systemcmd,file_path_back);
		strcat(systemcmd," ");
		strcat(systemcmd,file_path);
		printf("systemcmd %s\n",systemcmd);
		system(systemcmd);
		
		utimebuf.actime  = file_creattime;
		utimebuf.modtime = file_creattime;
		utime(file_path,&utimebuf);
	}
	
	bzero(file_path_back,FILE_NAME_MAX_LEN);  
	bzero(file_path,FILE_NAME_MAX_LEN);
}

#define CMD_SIZE 3
cmd_repose_t cmd_repose[] = {
	{CMD_SEND_FILEMSG_REQ,REQ_NOR ,cmd_reqsend_deel},
	{CMD_SEND_FILE	     ,REQ_NORE,cmd_revfile},
	{CMD_SEND_FINISH	 ,REQ_NOR ,cmd_revfilefinsh}
};

int num = 0;
void onrecv(char *buf,int len)
{
	int i;
	Message_t* Message = NULL;
	
//	printf("rev:%s,len=%d\n",buf,len);  rev send test
//	udpsend(buf,len);
	
	Message = (Message_t*)buf;
//	printf("num = %d,cmd = %d\n",num++,Message->cmd);
	for(i = 0;i < CMD_SIZE;i++)
	{
		if(cmd_repose[i].cmd == Message->cmd&&cmd_repose[i].type == Message->type)
		{
			  cmd_repose[i].cmd_ops(Message);
			  break;
		}
	}
}
	

/***************socket udp send rev***********************************/	
int sock;
struct sockaddr_in peeraddr; 
struct sockaddr_in servaddr;  

void udpsocketinit()
{
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
        ERR_EXIT("socket error");  
      
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(PORT);  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
      
    printf("port %d\n",PORT);  
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  
    {
        perror("sock bind");
        ERR_EXIT("bind error");  
    }
}

void udpsend(char *buf,unsigned int len)
{
	sendto(sock, buf, len, 0, (struct sockaddr *)&peeraddr, sizeof(servaddr));  
}

char recvbuf[MAXLINE] = {0}; 
void socketrev(void)  
{  
    socklen_t peerlen;  
    char * server_ip = NULL;
    unsigned short int port = 0;
    int n;  
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
        /* socket error */
        if (ret == -1)
        {
            perror("select");
            break;
        }
        /* timeout */
        if (ret == 0)
        {
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
            }  
            else if(n > 0)  
            {  
  //              server_ip = inet_ntoa(peeraddr.sin_addr);//display remove ip addr
  //              port      = ntohs(peeraddr.sin_port);
  //              printf("client_ip = %s port = %d::",server_ip,port);
                
				onrecv(recvbuf,n);
            }  
        }
    }  
    close(sock);  
}  

int access_netcard(const char *net_card)
{
	struct ifreq ifr;
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, net_card,IFNAMSIZ);
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return -1;
    }
//    printf("frname:%s\n",ifr.ifr_name);
//    printf("host:%s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
}


pthread_t usb_net_recv_pthread_;
void *usb_net_recvthread(void *param)
{
	udpsocketinit();
	while(1)
	{
		socketrev();
//		DLS_PrintErr("usb_net_recvthread is start\r\n");
		sleep(5);
	}
}

static void usb_net_rev_init(void)
{
	pthread_create(&usb_net_recv_pthread_, NULL, usb_net_recvthread,NULL);
	pthread_detach(usb_net_recv_pthread_);
}


int main(void)  
{
//	udpsocketinit();
//	if(access_netcard("eth0") < 0)//eth0
//	{
//		 return -1;  
//	}
//   socketrev();  
    printf("VERSION:%s\r\n",VERSION);  
    usb_net_rev_init();
	while(1)
	{
		sleep(10);
	}
    return 0;  
}



