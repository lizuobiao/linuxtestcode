#ifndef __NET_FILE_H__
#define __NET_FILE_H__

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <pthread.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/time.h>

#define    BUF_MAX_LEN    (2048)
#define    MAXLINE        (BUF_MAX_LEN+MESG_HEAD_LEN)
#define    PORT 		  1235

enum Command
{
    CMD_SEND_FILEMSG_REQ = 0x01,
	CMD_SEND_FILE 		 = 0x02,
	CMD_SEND_FINISH 	 = 0x03,
};

enum Type
{
	REQ_NOR  = 0x00,
    ACK_OK   = 0x01,
    ACK_FAIL = 0x02,
	REQ_NORE = 0x04,
};

#define FILE_NAME_MAX_LEN 64
typedef struct
{
	char file_name[FILE_NAME_MAX_LEN];
    char md5[16];
    unsigned int file_length;
/*
    unsigned long file_name_len;
    int picture_num;
    int picture_total;
    int block_size;*/
} FileBlock_t;

#define MESG_HEAD_LEN 8
typedef struct
{
	uint16_t cmd;	
	uint16_t type;     //0 request 1 respose
	uint32_t buf_len;
	char     buf[BUF_MAX_LEN];
}Message_t;



typedef void (*CmdOps)(Message_t*);  

typedef struct 
{
	unsigned char cmd;
	unsigned char type;
	CmdOps    cmd_ops;
}cmd_repose_t;

#endif

