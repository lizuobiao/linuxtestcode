
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* seconddrvtest 
  */
int main(int argc, char **argv)
{
	int fd;
	unsigned char key_vals[4];
	int cnt = 0;
	
	fd = open("/dev/led", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	write(fd,"11",2);
	write(fd,"22",2);
	write(fd,"32",2);
	
	close(fd);
	
	return 0;
}

