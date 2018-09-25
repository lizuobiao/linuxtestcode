#include <sys/time.h> 
#include <stdio.h>
#include <unistd.h>

void main(void)
{
	unsigned int time = 0;
	struct timeval start,now, res;
	
	gettimeofday(&start, NULL);
	usleep(10*1000);
    gettimeofday(&now, NULL);
    timersub(&now, &start, &res);
    time = (res.tv_sec)*1000 + (res.tv_usec)/1000;
	
	printf("time = %d\r\n",time);
	return;
}
