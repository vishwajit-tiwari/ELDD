
obj-m += charDev_fops.o 

ifdef ARCH
# You can provide your cross compilation path here
KERN_DIR = /lib/modules/5.15.0-29-generic/build/
else
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)
endif

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean

# Command Line Arugument for cross compilation
#sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- 