#include "gpio_class.h"

GPIO *pwren_4g  = new GPIO(85);
GPIO *pwrkey_4g = new GPIO(97);

void init()
{
	pwren_4g->GpioSetDirection(GPIO_DIR_OUT);
	pwrkey_4g->GpioSetDirection(GPIO_DIR_OUT);
	
	pwren_4g->GpioSetValue(GPIO_VAL_HIGH);
	pwrkey_4g->GpioSetValue(GPIO_VAL_LOW);
}

void open(void)
{
//	pwren_4g->GpioSetDirection(GPIO_DIR_OUT);
	
	pwren_4g->GpioSetValue(GPIO_VAL_HIGH);
	pwrkey_4g->GpioSetValue(GPIO_VAL_LOW);
	usleep(500*1000);
	pwrkey_4g->GpioSetValue(GPIO_VAL_HIGH);
	usleep(500*1000);
	pwrkey_4g->GpioSetValue(GPIO_VAL_LOW);
	
}

void close(void)
{
	pwren_4g->GpioSetValue(GPIO_VAL_LOW);
}

int main()
{
	init();
//	open();
//	close();
	printf("GpioGetDirection = %d\r\n",pwren_4g->GpioGetDirection());
	printf("GpioGetValue = %d\r\n",pwren_4g->GpioGetValue());
	
	sleep(1);
//	delete pwren_4g;
	return 0;
}