# Makefile for Cross Compilation 

obj-m := helloModule.o 
KERN_DIR = /lib/modules/5.15.45-v7l+/build/

#all:
#	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf--C $(KERN_DIR) M=$(PWD) modules 

#clean:
#	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf--C $(KERN_DIR) M=$(PWD) clean 

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=$(PWD) modules 

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=$(PWD) clean 

