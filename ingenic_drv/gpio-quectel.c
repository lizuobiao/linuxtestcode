/*
 * linux/drivers/char/am437x-gpio.c
 * hclydao <hclydao@163.com>
 * Copyright 2016 GzsdInfo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <linux/gpio.h>

#include <linux/suspend.h>
#include <linux/suspend_ioctls.h>
#include <linux/utsname.h>
#include <linux/freezer.h>
#include <linux/compiler.h>

#include <linux/platform_device.h>

#define GPIO_TO_PIN(bank, gpio)		(32 * (bank) + (gpio))

#define MODEL_POWERKEY		GPIO_TO_PIN(3, 2)
#define MODEL_CTRL		GPIO_TO_PIN(3, 5)
#define MODEL_PWREN		GPIO_TO_PIN(1, 1)


static int iopower_probe(struct platform_device *pdev)
{
	int ret;
	ret = gpio_request(MODEL_PWREN, "4g power en");
	if(ret < 0) {
		printk("+++%s : %d error:%d\n",__func__,__LINE__,ret);
		return ret;
	}
	
	ret = gpio_direction_output(MODEL_PWREN, 1);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}

	ret = gpio_request(MODEL_CTRL, "4g ctrl en");
	if(ret < 0) {
		printk("+++%s : %d error:%d\n",__func__,__LINE__,ret);
		return ret;
	}
	
	ret = gpio_direction_output(MODEL_CTRL, 1);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}

	ret = gpio_request(MODEL_POWERKEY, "4g powerkey");
	if(ret < 0) {
		printk("+++%s : %d error:%d\n",__func__,__LINE__,ret);
		return ret;
	}
	
	ret = gpio_direction_output(MODEL_POWERKEY, 0);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}
	mdelay(5000);
	__gpio_set_value(MODEL_POWERKEY, 1);
	mdelay(5000);
	__gpio_set_value(MODEL_POWERKEY, 0);

	return 0;
}


#define CONFIG_PM
#ifdef CONFIG_PM
//static int iopower_suspend(struct platform_device *pdev, pm_message_t state)
static int iopower_suspend(struct device *dev)
{

	int ret;
/*
	ret = gpio_direction_output(MODEL_PWREN, 0);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}
*/
	ret = gpio_direction_output(MODEL_CTRL, 0);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}
	printk("+++%s : %d MODEL_CTRL 0 --info:suspend\n",__func__,__LINE__);

	return 0;
}

//static int iopower_resume(struct platform_device *pdev)
static int iopower_resume(struct device *dev)
{

	int ret;
	ret = gpio_direction_output(MODEL_PWREN, 1);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}
	
	ret = gpio_direction_output(MODEL_CTRL, 1);
	if(ret < 0) {
		printk("+++%s : %d error\n",__func__,__LINE__);
		return ret;
	}
	
/*
	mdelay(50);
	__gpio_set_value(MODEL_POWERKEY, 1);
	mdelay(120);
	__gpio_set_value(MODEL_POWERKEY, 0);
*/
	printk("+++%s : %d MODEL_PWREN, 1 MODEL_CTRL, 1 info:resume\n",__func__,__LINE__);

	return 0;
}
#else
#define iopower_suspend	NULL
#define iopower_resume	NULL
#endif

static SIMPLE_DEV_PM_OPS(iopower_pm_ops, iopower_suspend, iopower_resume);

static int iopower_remove(struct platform_device *pdev)
{

	return 0;
}

static struct platform_driver iopower_device_driver = {
	.probe		= iopower_probe,
	.remove		= iopower_remove,
	.suspend	= iopower_suspend,
	.resume		= iopower_resume,
	.driver		= {
		.name	= "iopower",
		.pm	= &iopower_pm_ops,
		.owner	= THIS_MODULE,
	}
};

static struct platform_device iopower_device = {
	.name	= "iopower",
	.id	= -1,
	.num_resources	= 0,
};

static int __init iopower_init(void) 
{
	int err;
	err = platform_device_register(&iopower_device);
	if (err) {
		printk("iopower_device register failed\n");
		return err;
	}
	return platform_driver_register(&iopower_device_driver);
}

static void __exit iopower_exit(void) 
{
	platform_device_unregister(&iopower_device);
	platform_driver_unregister(&iopower_device_driver);
}

module_init(iopower_init);
module_exit(iopower_exit);

MODULE_LICENSE("GPL");

