#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJIT TIWARI");
MODULE_DESCRIPTION("A Module Prog for cdev struct & file operation");

dev_t mydev = 0;                         // Creating Device Number
static struct class *dev_class;         // Creating Device File
static struct cdev my_cdev;             // Creating 

// Module Entry
static int __init charDev_fops_init(void) {

    printk(KERN_ALERT "Inserting charDev_fops into Kernel\n");

    // Allocating Major number Dynamically
    if(alloc_chrdev_region(&mydev,0,1,"charDev_fops") < 0) {
        pr_err("Cannot allocate Major number\n");
        return -1;
    }

    pr_info("Device Number allocated successfully:\n Major = %d Minor = %d\n", MAJOR(mydev), MINOR(mydev));

    // Creating struct Class for Device
    if((dev_class = class_create(THIS_MODULE,"charDev_class")) == NULL) {
        printk(KERN_ERR "Cannot create the struct class for device\n");
        goto r_class;
    }

    /**
     * @brief Creating Device File
     * struct device *device_create (struct *class, struct device *parent, 
     *                               dev_t dev, void * drvdata, const char *fmt, ...);
     */
    if((device_create(dev_class,NULL,mydev,NULL,"charDev_device")) == NULL) {
        pr_err("Cannot create the Device\n");
        goto r_device;
    }

    pr_alert("Kernel Module Inserted Successfully\n");

    return 0;

// To free-up the resources
r_device:
    class_destroy(dev_class);

r_class:
    unregister_chrdev_region(mydev,1);
    return -1;

}
// Module Exit
static void __exit charDev_fops_exit(void) {

    printk(KERN_ALERT "Removing charDev_fops from Kernel\n");
    device_destroy(dev_class,mydev);                // To destroy device 
    class_destroy(dev_class);                       // To destroy device class
    unregister_chrdev_region(mydev,1);              // To unregister Device Number 
    pr_info("Module Removed Successfully\n");
}

module_init(charDev_fops_init);
module_exit(charDev_fops_exit);