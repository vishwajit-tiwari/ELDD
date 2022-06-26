#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/types.h>
#include<linux/device.h>            // To Automatically create Device File

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("Module Program to create Device File automatically");

dev_t mydev = 0;                    // Creating Device Number Dynamically
static struct class *dev_class;     // Creating Device File Class

// Module Entry
static int __init dynamicDevFile_init(void) {

    printk(KERN_ALERT "Inserting Module into Kernel\n");
    
    // Allocating Major number Dynamically
    if(alloc_chrdev_region(&mydev,0,1,"myCharDevice") < 0) {
        pr_err("Cannot allocate Major Number\n");
        return -1;
    }

    printk(KERN_INFO "Device Number allocated successfully:\n Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    /**
     * @brief Creating struct class
     * struct class * class_create (struct module *owner, const char *name);
     * 
     * owner – pointer to the module that is to “own” this struct class
     * name – pointer to a string for the name of this class
     */

    if((dev_class = class_create(THIS_MODULE,"charDev_Class")) == NULL) {
        pr_err("Cannot create the struct class for device\n");
        goto r_class;
    }

    /**
     * @brief Creating Device File
     * struct device *device_create (struct *class, struct device *parent, 
     *                               dev_t dev, void * drvdata, const char *fmt, ...);
     
     * class – pointer to the struct class that this device should be registered to
     * parent – pointer to the parent struct device of this new device, if any
     * devt – the dev_t for the char device to be added
     * drvdata – the data to be added to the device for callbacks
     * fmt – string for the device’s name
     * ... – variable arguments
     */
    
    if((device_create(dev_class,NULL,mydev,NULL,"charDev_Device")) == NULL) {
        pr_err("Cannot create the Device\n");
        goto r_device;
    }

    pr_alert("Kernel Module Inserted Successfully\n");
    
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(mydev,1);
    return -1;
}

// Module Exit
static void __exit dynamicDevFile_exit(void) {

    printk(KERN_ALERT "Removing Module from Kernel\n");
    device_destroy(dev_class,mydev);                    // To Destroy Device class
    class_destroy(dev_class);                           // To Destroy Device File
    unregister_chrdev_region(mydev,1);                  // De-allocating Major Minor number 
    pr_info("De-allocating Major Minor number\n");
    pr_info("Removing Device class & Device file\n");
}

module_init(dynamicDevFile_init);
module_exit(dynamicDevFile_exit);
