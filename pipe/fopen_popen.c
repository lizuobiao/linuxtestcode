#include <stdio.h>
int main(int argc, char *argv[])
{
	FILE *pipe_fp, *infile;
	char readbuf[80];
	if( argc != 3) {
		fprintf(stderr, "USAGE: popen3 [command] [filename]\n");
		exit(1);
	}
	/* 打开输入文件 */
	if (( infile = fopen(argv[2], "rt")) == NULL)
	{
		perror("fopen");
		exit(1);
	}
	/* 建立写管道 */
	if (( pipe_fp = popen(argv[1], "w")) == NULL)
	{
		perror("popen");
		exit(1);
	}
	/* Processing loop */
	do{
		fgets(readbuf, 80, infile);
//		printf("readbuf = %s\n",readbuf);
		if(feof(infile)) break;//无法理解
		fputs(readbuf, pipe_fp);
	} while(!feof(infile));
	fclose(infile);
	pclose(pipe_fp);
	return(0);
}