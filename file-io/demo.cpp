#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
  FILE* fstream;
  
  char msg[100] = "Hello!I have read this file.\n";
  char read[100] = {0};
  
  fstream=fopen("test.txt","r+");
  if(fstream==NULL)
  {
    printf("open file test.txt failed!\n");
    exit(1);
  }
  else
  {
    printf("open file test.txt succeed!\n");
  }
  
  fwrite(msg,100,1,fstream);
//  fprintf(fstream, "%s\n", "hello!");
  
  fseek(fstream,0,SEEK_SET);
  fread(read,100,1,fstream);
  printf("read = %s\r\n",read);
  
  fclose(fstream);
  return 0;
}
