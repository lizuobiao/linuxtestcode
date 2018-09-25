#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>

void* thread1(void *arg)
{
	while(1)
	{
		usleep(100 * 1000);
		printf("thread1 running...!\n");
	}
	printf("Leave thread1!\n");
	return NULL;
}

int main(int argc, char** argv)
{
	pthread_t tid;
	pthread_create(&tid, NULL, (void*)thread1, NULL);
	pthread_detach(tid); // pthread_detach(tid)使线程处于分离状态
	pthread_join(tid);  // 使线程处于阻塞
	sleep(1);
	printf("Leave main thread!\n");
	return 0;
} 