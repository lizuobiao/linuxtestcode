#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int stringtoarray(char* str,const char* delim,char **saveptr)
{
    char * pch = NULL;
	int num = 0;
    pch = strtok(str, delim);
    while (pch != NULL)
    {
//		saveptr[num] = pch;
		*(saveptr+num) = pch;
		num++;
//        printf("%s\n", *saveptr);
        pch = strtok(NULL, " ,.-");
    }
	return num;
}



int main(int argc, char* argv[]) 
{
    char* delim = " ,.-";
	char str[] = "- This, a sample string.";
	char *saveptr[10];
	int num = 0,i;
	
	printf("Splitting string \"%s\" into tokens:\n", str);
	num = stringtoarray(str,delim,saveptr);
//	strtok_r(str,delim,saveptr);
	for(i = 0;i < num;i++)
		printf("saveptr = %s\n",saveptr[i]);
	printf("str: %s\n", str);
    return 0;
}

