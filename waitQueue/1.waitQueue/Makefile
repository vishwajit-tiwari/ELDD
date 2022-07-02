# Makefile to compile charDriver

obj-m += waitQueue1.o 

ifdef ARCH
# You can put your cross compiler path here
KERN_DIR = /lib/modules/5.15.0-29-7vl+/build/
else
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)
endif

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean

