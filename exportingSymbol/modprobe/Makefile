obj-m:= avg/ add/
KERN_DIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)

all:
	make -C $(KERN_DIR) M=$(PWD) modules

install:
	make -C $(KERN_DIR) M=$(PWD) modules_install
	depmod -a 

clean:
	make -C $(KERN_DIR) M=$(PWD) clean

remove:
	rm -rf /lib/modules/$(shell uname -r)/extra/*
	depmod -a 
