#include <sys/types.h> 
#include <utime.h>
#include <stdio.h>

void main(void)
{
	struct utimbuf file_time = {1537496189,1537496189};
	if(utime("./time.c",&file_time) != 0)
		printf("utime fail\r\n");
}
