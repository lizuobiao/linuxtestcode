#include "md5.h"  
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
//#include <openssl/md5.h>
/*
int NetMD5_file(const char *path,char *output)
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
*/
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
    MD5Init(&ctx);
    while(1)
    {
        rval = read(fd, buf, sizeof(buf));
        if (rval <= 0)
            break;
        MD5Update(&ctx, buf, rval);
    }
    MD5Final(&ctx,(unsigned char*)output);
    close(fd);
    return 0;
}

void main(void)
{
	char md5output[16] = {0};
	int j;

	MD5_file("./md5.c",md5output);
        for(j = 0;j < 16;j++)
		printf("%2x",(uint8_t)md5output[j]);
      	printf("\r\n");
}
