#ifndef _SYS_GPIO_H_
#define _SYS_GPIO_H_

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h> 

#define GPIO_PIN 18

#define GPIO_DIR_OUT  		"out"
#define GPIO_DIR_IN   		"in"
#define GPIO_VAL_HIGH 		"1"
#define GPIO_VAL_LOW  		"0"

void Gpio_init(int n);  					 		//create gpio file
void Gpio_destroy(int n);							//destroy gpio file
void GpioSetValue(int n,const char *value);  		//set gpio pin high or low
int GpioGetValue(int n);							//get gpio(n)'s value
void GpioSetDirection(int n,const char *direction); //set gpio "in" or "out"
int GpioGetDirection(int n);						//get gpio(n)'s Direction  "in" = 1,"out" = 0 esle -1


#endif