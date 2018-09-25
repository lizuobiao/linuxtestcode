#include <unistd.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <iostream>

int stdout_to_file(const char * file_name) 
{  
	int fd;  
	int refd;  
	char buf[] ="Advanced Programming!\n";  
	fd = open(file_name,O_RDWR|O_CREAT,0644);  
	if(fd==-1)  
	{  
			printf("open file error:%m\n");  
			return (-1);  
	}  
	refd = dup2(fd,fileno(stdout));  
	if(refd==-1)  
	{  
			printf("redirect standard out error:%m\n");  
			return (-1);  
	}  

	//写数据，本应该写入到stdout的信息重定向而写入到目标文件中（test.txt）  
	printf("dup2的返回值\n"); 
//	return refd; //这样子会有异常。主函数printf打印不了
	return 0;//这样子就ok
}  

int main() 
{
	int fd;  
	const char file_name[] = "stdoutlog";
	fd = stdout_to_file(file_name);	
	std::cout << "dup2 stdout test" << std::endl;
	printf("dup2的返回值：%d\n",fd); 
//	sleep(1);
//	close(fd); 
	return 0;
}
