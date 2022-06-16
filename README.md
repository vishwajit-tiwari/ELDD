
# **Embedded Linux Device Driver (ELDD)**

## **Day 1**

* ### **Module Programing**
    * module init
    * module exit
    * Makefile
        * Makefile for Native Compilation 
        * Makefile for Cross Compilation

* ### **Corss Compiler & Kernel Environment for rpi4**
    1. **Steps to flash Raspbian OS onto sd card**
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

    1. **Steps for cross compiling Kernel :**
        * On Host(Ubuntu) :
            1. Install Required dependencies : 
                ```bash
                sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
                ```

            1. Install 32-bit Toolchain :
                ```bash
                sudo apt install crossbuild-essential-armhf
                ```
            1. Download / clone Kernel source
                ```bash
                mkdir rpi
                cd rpi
                git clone --depth=1 --branch rpi-5.15.y https://github.com/raspberrypi/linux
                cd linux
                ```
            1. Apply the config file of rpi4 :
                * Check config file for your board(rpi4) using below command.
                    ```bash
                    ls arch/arm/configs
                    ```
                * Default config file for rpi4 is **bcm2711_defconfig**.
                * Now apply config file using below command.
                    ```bash
                    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
                    ```
                
            1. Build Kernel image & Kernel modules for rpi4 :
                ```bash
                make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules
                ``` 
                * Result of above command :
                    ```bash
                    ls arch/arm/boot
                    Zimage  # This is the result.
                    ```
            1. Plug in your SD Card to your HOST PC(Ubuntu) :
                ```bash
                cp arch/arm/boot/zImage /media/<user_name of your PC>/boot
                ```
            1. Install modules onto rootfs partition (or "/") of SD Card    
                ```bash
                make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=<path-to-sdcard-rootfs-partition> modules_install

                sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install  # It will install 5.15.45-v7l+ into "ls /lib/modules"
                ```
                * Example in my PC :
                    ```bash
                    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/media/vishu/rootfs modules_install
                    ```
                * Modules get install in **rootfs/lib/modules** path.
            
            1. Configuring **config.txt** to boot our new Kernel.
                ```bash
                cd /media/<user-name>/boot
                ```
                * Open **config.txt :**
                    ```bash
                    vim config.txt
                    ```
                * Add below line at the end of the file and save file :
                    ```bash
                    kernel=zImage
                    ```
            1. if **ssh** & "wpa_supplicant.conf" files are not in your boot partition then follow steps-2 of flashing raspbian OS.

            1. Plug out your SD Card and insert into your Raspberry pi board.

            1. Board will start booting and access it.
