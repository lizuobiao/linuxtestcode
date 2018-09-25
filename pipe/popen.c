#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 0
#define MAXSTRS 5
int main(void)
{
	int cntr;
	FILE *pipe_fp;
	char *strings[MAXSTRS] = { "roy", "zixia", "gouki","supper", "mmwan"};
	/* 用 popen 建立管道 */
	if (( pipe_fp = popen("sort", "w")) == NULL)
	{
		perror("popen");
		exit(1);
	}
	/* Processing loop */
	for(cntr=0; cntr<MAXSTRS; cntr++)
	{
		fputs(strings[cntr], pipe_fp);
		fputc('\n', pipe_fp);
	}
	/* 关闭管道 */
	pclose(pipe_fp);
	return(0);
}
#endif



void cmd_exec(const char* cmd,char*result)
{
	FILE *fp = NULL;  
	
    if(result == NULL)
		return;
	
	fp = popen(cmd,"r"); 
	if(!fp) 
	{  
		perror("popen");  
		exit(EXIT_FAILURE);  
	}  
	while(fgets(result,128, fp) != 0 ) 
	{  
//		printf("%s", result);  
	}  
	pclose(fp);  
}

int main(void)  
{  		
    char buf[128] = {0};  
 //   cmd_exec("ntpdate time",buf);  
    printf("%s",buf);
    cmd_exec("route -n | awk '/UG/{print $8}'",buf);
    printf("%s",buf);  
	
    return 0;
} 



