
obj-m += dynamic_mjmn.o 

ifdef ARCH
# You can update your Cross Compilation Path Here
KERN_DIR = /lib/modules/5.15.0-25-generic/build/
else
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)
endif

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean


# In case of cross compilation 
# We need to pass Cross Compilation toolchain from Command Line
# sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-