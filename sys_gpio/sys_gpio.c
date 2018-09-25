#include "sys_gpio.h"

//create gpio file
void Gpio_init(int n)  
{  
    FILE * fp =fopen("/sys/class/gpio/export","r+");  
    if (fp == NULL)  
        perror("export open filed");  
    else  
        fprintf(fp,"%d",n);  
    fclose(fp);  
}   

void Gpio_destroy(int n)  
{  
    FILE * fp =fopen("/sys/class/gpio/unexport","r+");  
    if (fp == NULL)  
        perror("export open filed");  
    else  
        fprintf(fp,"%d",n);  
    fclose(fp);  
} 

//set gpio pin high or low
void GpioSetValue(int n,const char *value)  
{  
    char path[100] = {0};  
    sprintf(path,"/sys/class/gpio/gpio%d/value",n);  
    FILE * fp =fopen(path,"r+");  
    if (fp == NULL)  
        perror("direction open filed");  
    else  
        fprintf(fp,"%s",value);  
    fclose(fp);  
}

//get gpio(n)'s value
int GpioGetValue(int n)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n);  
    fd = open(path, O_RDONLY);  
    if(fd < 0) 
	{  
        perror("Failed to open gpio value for reading!");  
        return -1;  
    }  

    if(read(fd, value_str, 3) < 0) 
	{  
        perror("Failed to read value!");  
        return -1;  
    }  

    close(fd);  
    return (atoi(value_str));  
}   

//set gpio "in" or "out"
void GpioSetDirection(int n,const char *direction)  
{  
    char path[100] = {0};  
    sprintf(path,"/sys/class/gpio/gpio%d/direction",n);  
    FILE * fp =fopen(path,"r+");  
    if (fp == NULL)  
        perror("direction open filed");  
    else  
        fprintf(fp,"%s",direction);  
    fclose(fp);  
}   

//get gpio(n)'s value
int GpioGetDirection(int n)  
{  
    char path[64];  
    char direction_str[5];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", n);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) 
	{  
        perror("Failed to open gpio value for reading!");  
        return -1;  
    }  

    if (read(fd, direction_str, 3) < 0) 
	{  
        perror("Failed to read direction_str!");  
        return -1;  
    }  
		
    close(fd);  
	
	printf("direction_str = %s\r\n",direction_str);
	if(direction_str[0] == 'i')
		return 1;
	else if(direction_str[0] == 'o')
		return 0;
	else
		return -1;  
} 

int main()
{
	Gpio_init(GPIO_PIN);  
    GpioSetDirection(GPIO_PIN,GPIO_DIR_OUT); 
	GpioSetValue(GPIO_PIN,GPIO_VAL_HIGH);
	printf("GpioGetDirection = %d\r\n",GpioGetDirection(GPIO_PIN));
	printf("GpioGetValue = %d\r\n",GpioGetValue(GPIO_PIN));
	Gpio_destroy(GPIO_PIN);
	return 0;
}