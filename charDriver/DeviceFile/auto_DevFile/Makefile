
obj-m += dynamicDevFile.o 

ifdef ARCH
# You can update your cross compilation Path here
KERN_DIR = /lib/modules/5.15.0-29-generic/build/
else
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd) 
endif

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean

# Command line arg for Cross Compilation
# sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

# Manually Create Device File from cmd 
# mknod -m <permissions> </dev/name> <device type> <major> <minor>
# e.g. sudo mknod -m 666 /dev/myCharDevice c 510 0
