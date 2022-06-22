
# Embedded Linux Device Driver

## Course Outline:

> **Be introduced to the** 
- Linux Kernel environment, 
- Build system, 
- Kernel configuration and customization.
- Compilation and setting up a Linux environment
- Basic understanding of kernel programming concepts like 
    * Module Programming and
    * Device Drivers Development
    *  A character driver on x86 PCs and ARM based Linux Environments
    * Understand cross tooling environments and be exposed to development of device drivers for a target hardware platform.
___

## **Day 1**

## Introduction to EOS(Embedded Operating System):

* Basic of EOS
* Host, Target, Development Environment, 
* Debug Environment and OS

## The Linux Embedded System:
* The Basic Toolchain, Library, Debugers
* The Board Support Packages
    * Bootloader
    * Kernel
    * File System


## Classification of Embedded System:
> ### On basis of size and power consumption:

* Small Embedded System
* Medium Embedded System
* Large Embedded System

### 1. Small Embedded System: 
* Low Power CPU
* Less than 4MB of ROM ( or Flash)
* RAM is 8-16MB 

### 2. Medium Embedded System:
* Medium Power CPU
* 32MB+ ROM and
* 64-128MB RAM with Optional Secondary Storage

### 3. Large Embedded System: 
* High Power CPU
* Large Memory Banks

    example: Mobile Phone

> ### On basis of Timing Constraints:
* **Straingent Deadline:** *Hard and Soft Realtime*
* **Mild Deadline:** *Timing is not a major issuees*

### 1. Straingent Deadline:
**(a) Hard Realtime System:** *When ever the daedline is missed the system will fail.*

**(b) Soft Realtime System:** *Missing or failure of few deadline will degrade the performance so, there is a thresold of performance.* 

### 2. Mild Deadline: *Timing is not a major issue*

> ***The Nuclear Reactor or Space crafts comes under Straingent Deadline type Embedded System.***

## The Linux Embedded System: 

* *The Basic Tool-chains, Library, Debugers*
* *The Board Support Package (BSP)*
    * *Boot-loader*
    * *Kernel*
    * *File System*

---

## **Day 2**

## Steps to flash Raspbian OS onto sd card
* On Host(Ubuntu):
    1. Open terminal & Install rpi-imager:
        ```bash
        sudo apt install rpi-imager
        snap install rpi-imager
        ```

    1. Run rpi-imager
        ```bash 
        rpi-imager
        ```
        * Choose OS: Raspberry Pi OS (other) => rasperry pi OS Lite 32-bit
        * Choose storage: choose your SD Card
        * Click on write & then click on yes - This will take some time.

    1. After completing flashing image plug out SD Card and insert SD Card again.
        ```bash
        cd /media/<user-name of your machine>/boot
        touch ssh
        touch wpa_supplicant.conf
        vim wpa_supplicant.conf
        ```
        * Write the following code in wpa_supplicant.conf file and save it.
        ```bash
        country=IN
        ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev network={
            ssid="Your Hotspot name"
            psk="Your Hotspot Password"
            key_mgmt=WPA-PSK
            }
        ```
    1. Plug out the SD Card and insert into your raspberry pi board.

    1. Board will start booting & then access it.

---

## Corss Compiler & Kernel Environment for rpi4
### **Steps for cross compiling Kernel :**
* **On Host(Ubuntu) :**
    1. Install Required dependencies : 
        ```bash
        sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
        ```

    2. Install 32-bit Toolchain :
        ```bash
        sudo apt install crossbuild-essential-armhf
        ```
    3. Download / clone Kernel source
        ```bash
        mkdir rpi
        cd rpi
        git clone --depth=1 --branch rpi-5.15.y https://github.com/raspberrypi/linux
        cd linux
        ```
    4. Apply the config file of rpi4 :
        * Check config file for your board(rpi4) using below command.
            ```bash
            ls arch/arm/configs
            ```
        * Default config file for rpi4 is **bcm2711_defconfig**.
        * Now apply config file using below command.
            ```bash
            make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
            ```
                
    5. Build Kernel image & Kernel modules for rpi4 :
        ```bash
        make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules
        ``` 
        * Result of above command :
            ```bash
            ls arch/arm/boot
            Zimage  # This is the result.
            ```
    6. Plug in your SD Card to your HOST PC(Ubuntu) :
        ```bash
        cp arch/arm/boot/zImage /media/<user_name of your PC>/boot
        ```
    7. Install modules onto rootfs partition (or "/") of SD Card    
        ```bash
        make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=<path-to-sdcard-rootfs-partition> modules_install

        sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install  # It will install 5.15.45-v7l+ into "ls /lib/modules"
        ```
        * Example in my PC :
            ```bash
            make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/media/vishu/rootfs modules_install
            ```
        * Modules get install in **rootfs/lib/modules** path.                
            
    8. Configuring **config.txt** to boot our new Kernel.
        ```bash
        cd /media/<user-name>/boot
        ```
        * Open **config.txt :**
            ```bash
            vim config.txt
            ```
        * Add below line at the end of the file and save file.
            ```bash
            kernel=zImage
            ```
    9. if **ssh** & "wpa_supplicant.conf" files are not in your boot partition then follow steps-2 of flashing raspbian OS.

    10. Plug out your SD Card and insert into your Raspberry pi board.

    11. Board will start booting and access it.

___

## **Day 3**
### Module Programing    
* module init
* module exit
    ```c
    #include<linux/init.h>
    #include<linux/module.h>
    #include<linux/kernel.h>

    MODULE_AUTHOR("Any_Name");
    MODULE_LICENSE("GPL");          // Tells Module bear free License
    MODULE_DESCRIPTION("Hello World Module");

    static int helloModule_init(void) {

    printk(KERN_ALERT "Hello World!\nInserting Module into Kernel\n");
    return 0;
    }

    static void helloModule_exit(void) {
    
    printk(KERN_ALERT"Good bye...\nRemoving Module from Kernel\n");
    }

    module_init(helloModule_init);  // Insert Module into Kernel
    module_exit(helloModule_exit);  // Remove Module from Kernel
    ```
* Makefile
    * Makefile for Native Compilation 
        ```make
        # Makefile for Native Compilation 
        obj-m := helloModule.o 
        KERN_DIR = /lib/modules/$(shell uname -r)/build/
        PWD = $(shell pwd)

        all:
	        make -C $(KERN_DIR) M=$(PWD) modules 

        clean:
	        make -C $(KERN_DIR) M=$(PWD) clean 
        ```

    * Makefile for Cross Compilation
        ```make
        # Makefile for Cross Compilation 
        obj-m := helloModule.o 
        KERN_DIR = /lib/modules/5.15.45-v7l+/build/

        all:
	        make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=$(PWD) modules 

        clean:
	        make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERN_DIR) M=$(PWD) clean 
        ```

## **Day 4**

