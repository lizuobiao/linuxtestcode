#include <stdio.h>
int main(void)
{
	FILE *pipein_fp, *pipeout_fp;
	char readbuf[80];
	/* 用 popen 建立一个通向"ls:的读管道 */
	if (( pipein_fp = popen("ls", "r")) == NULL)
	{
		perror("popen");
		exit(1);
	}
	/* 用 popen 建立一个通向"sort"的写管道 */
	if (( pipeout_fp = popen("sort", "w")) == NULL)
	{
		perror("popen");
		exit(1);
	}
	/* 进程循环 */
	while(fgets(readbuf, 80, pipein_fp))
		fputs(readbuf, pipeout_fp);
	/* 关闭打开的管道 */
	pclose(pipein_fp);
	pclose(pipeout_fp);
	return(0);
}