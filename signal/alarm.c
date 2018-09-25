#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <signal.h>  
  
void sig_alarm()   
{ 
  printf("SIGALRM is ok\n");
  exit(0);   
}  
int main(int argc, char *argv[])   
{   
  signal(SIGALRM, sig_alarm);   
  alarm(2);   
  sleep(3);   
  printf("Hello World!\n");   
  return 0;   
}