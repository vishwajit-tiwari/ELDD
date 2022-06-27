
obj-m += charDev_fops.o 

ifdef ARCH
# You can provide your cross compilation path here
KERN_DIR = /lib/modules/5.15.0-25-generic/build/
else
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)
endif

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules
	gcc -o app1 app1.c 
#	sudo mknod -m 666 /dev/charDev_device c 510 0

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean
	rm -rf app1
#	sudo rm -rf /dev/charDev_device

# Command Line Arugument for cross compilation
#sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- 