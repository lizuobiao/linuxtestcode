#include "storagemanage.h"

int main(void)  
{         
	uint32_t cap;
	int fd;
	
	cap = storage_cmd_exec(INTER_AVA);
	printf("get_storagecap cap = %d\r\n",cap);
	 
    return 0;
}

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
    
    while(fgets(result, STOCMD_BUF_LEN, fp) != 0 )
    {  
//        printf("%s", result);  
    }  
    pclose(fp);  
}

storagecmd_t storagecmd[] = {
	{EXTERNAL_ALL_CMD,  EXTERNAL_ALL},
	{INTER_CMD_ALL,     INTER_ALL},
	{EXTERNAL_AVA_CMD,  EXTERNAL_AVA},
	{INTER_AVA_CMD,	    INTER_AVA},
	{FORMAT_SD_CMD,	    FORMAT_SD},
};

uint32_t storage_cmd_exec(uint8_t type)
{
	uint32_t cap = 0;
	char buf[STOCMD_BUF_LEN] = {0};  

	cmd_exec(storagecmd[type].systemcmd,buf);//1
	
	printf("buf %s\n", buf); 
	if(strlen(buf) != 0)
	{
		sscanf(buf,"%d",&cap);
//		printf("cap = %d\r\n",cap);
	}
	
	return cap;
}








