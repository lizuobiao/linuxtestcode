#ifndef _SYS_GPIO_H_
#define _SYS_GPIO_H_

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h> 

#define GPIO_DIR_OUT  		"out"
#define GPIO_DIR_IN   		"in"
#define GPIO_VAL_HIGH 		"1"
#define GPIO_VAL_LOW  		"0"

class GPIO{
    public:
		GPIO(int gpio_no_);
        ~GPIO(void);
		void GpioSetValue(const char *value);  				//set gpio pin high or low
		int  GpioGetValue(void);							//get gpio(n)'s value
		void GpioSetDirection(const char *direction); 		//set gpio "in" or "out"
		int  GpioGetDirection(void);						//get gpio(n)'s Direction  "in" = 1,"out" = 0 esle -1
		
	protected:
		
    private:
		int gpio_no;
		void Gpio_init(void);  					 			//create gpio file
		void Gpio_destroy(void);							//destroy gpio file
};
#endif