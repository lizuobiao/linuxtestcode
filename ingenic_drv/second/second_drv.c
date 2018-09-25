#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/sched.h>//jiffies define
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/gpio.h>

static struct class *leddrv_class;
static struct device *leddrv_class_dev;

struct timer_list timer;//define a timer
unsigned int t_10ms;

#define LED_SYS 0
#define LED_NET 2
#define LED_UART 4

#define LED_SYS_ON   gpio_set_value(LED_SYS,1)
#define LED_SYS_OFF  gpio_set_value(LED_SYS,0)
#define LED_NET_ON   gpio_set_value(LED_NET,1)
#define LED_NET_OFF  gpio_set_value(LED_NET,0)
#define LED_UART_ON  gpio_set_value(LED_UART,1)
#define LED_UART_OFF gpio_set_value(LED_UART,0)


uint8_t LED_SYS_STATE  = 1;
uint8_t LED_GPRS_STATE = 0;
uint8_t LED_UART_STATE = 0;


void LED_CORL(void)
{
	enum
	{
		LED_ON=0,
		LED_WINK_FAST,
		LED_WINK_SLOW
	};
	static uint16_t COUNT=0;
	static uint8_t LED_UART_WINK_TIME=6;
	
	COUNT++;
	switch(LED_SYS_STATE)
	{
		case LED_ON:
			LED_SYS_ON;
			break;
		case LED_WINK_FAST:
			if(COUNT%2==0)
				LED_SYS_OFF;
			else
				LED_SYS_ON;
			break;
		case LED_WINK_SLOW:
			if(COUNT%5==0)
				LED_SYS_OFF;
			else
				LED_SYS_ON;
			break;
		default:
			LED_SYS_OFF;
			break;
	}
	
	switch(LED_GPRS_STATE)
	{
		case LED_ON:
			LED_NET_ON;
			break;
		case LED_WINK_FAST:
			if(COUNT%2==0)
				LED_NET_OFF;
			else
				LED_NET_ON;
			break;
		case LED_WINK_SLOW:
			if(COUNT%5==0)
				LED_NET_OFF;
			else
				LED_NET_ON;
			break;
		default:
			break;
	}
	
	switch(LED_UART_STATE)
	{
		case LED_ON:
			LED_UART_ON;
			break;
		case LED_WINK_FAST:
			if(LED_UART_WINK_TIME--)
			{
				if(COUNT%2==0)
					LED_UART_OFF;
				else
					LED_UART_ON;
			}
			else
			{
				LED_UART_WINK_TIME=6;
				LED_UART_STATE = 2;
			}
			break;
		case LED_WINK_SLOW:
			if(COUNT%5==0)
				LED_UART_OFF;
			else
				LED_UART_ON;
			break;
		default:
			LED_UART_OFF;
			break;
	}

	
}
static void timer_function(unsigned long arg)
{
	LED_CORL();
    mod_timer(&timer, jiffies + (t_10ms));//重新设置定时器，每隔5秒执行一次
}

void timer_init(unsigned int timer_10ms)
{
	init_timer(&timer);     //init timer

    timer.expires = jiffies+(timer_10ms);//set overtime

    timer.data = 5;    //arg

    timer.function = timer_function;//set funtion
	t_10ms = timer_10ms;
    add_timer(&timer); //add and setup timer
}

static int led_drv_open(struct inode *inode, struct file *file)
{
	
	return 0;
}

static ssize_t led_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

static ssize_t led_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *ppos);
{
	char led_buf[2] = {0};
	unsigned int count = size;
	
	if(copy_from_user(led_buf, buf, count))
	{
		printk("led_drv_write is fail");
		return -EFAULT;
	}else
	{
		printk("led_buf = %s\r\n",led_buf);
	}
	
	return ret;
}


static struct file_operations sencod_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   led_drv_open,     
	.read	=	led_drv_read,
	.write  = 	led_drv_write,
};


int major;
static int led_drv_init(void)
{
	major = register_chrdev(0, "led_drv", &sencod_drv_fops);

	leddrv_class = class_create(THIS_MODULE, "led_drv");

	leddrv_class_dev = device_create(leddrv_class, NULL, MKDEV(major, 0), NULL, "led"); /* /dev/buttons */
	gpio_request_one(LED_SYS,GPIOF_INIT_LOW,"led0");
	gpio_request_one(LED_NET,GPIOF_INIT_LOW,"led1");
	gpio_request_one(LED_UART,GPIOF_INIT_LOW,"led2");
	timer_init(20);
//	gpio_set_value(2,1);
	return 0;
}

static void led_drv_exit(void)
{
	unregister_chrdev(major, "led_drv");
	device_unregister(leddrv_class_dev);
	class_destroy(leddrv_class);
	gpio_free(4);
	gpio_free(2);
	gpio_free(0);
	del_timer(&timer);
	return 0;
}


module_init(led_drv_init);

module_exit(led_drv_exit);

MODULE_LICENSE("GPL");

