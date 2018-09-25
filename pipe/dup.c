#include <unistd.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
  
void main()  
{  
        int fd,newfd;  
        char *bufFD="Advanced Programming! write by fd\n";  
        char *bufNewFD="Advanced Programming! write by NewFD\n";  
        fd = open("test.txt",O_RDWR|O_CREAT,0644);  
        if(fd==-1)  
        {  
                printf("open file error%m\n");  
                exit(-1);  
        }  
  
        //开始复制了  
        newfd = dup(fd);  
        //使用fd写  
        write(fd,bufFD,strlen(bufFD));  
        close(fd);  
  
        //使用newfd写  
        write(newfd,bufNewFD,strlen(bufNewFD));  
  
        if(close(newfd)==-1)  
        {  
                printf("close error\n");  
        }  
        exit(0);  
}  