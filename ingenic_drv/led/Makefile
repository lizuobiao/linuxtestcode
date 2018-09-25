KERN_DIR = /home/biao/sdb1/ingenic-linux-kernel3.10.14-x1000-v6.0-20170428/kernel

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= led_drv.o
