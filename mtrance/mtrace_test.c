#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
   
#include <mcheck.h>  
   
   
int main()
{  
    setenv("MALLOC_TRACE", "output", 1);  
    mtrace();  
   
    char * text = ( char * ) malloc (sizeof(char) * 100);  
    memset(text,'/0',100);  
    memcpy(text,"hello,world!",12);  
   
    printf("%s/n",text);  
    return 0;  
} 
